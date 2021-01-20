#include <ccsds/sdl/usdl/map_service.h>
#include <ccsds/sdl/usdl/map_multiplexer.h>
#include <ccsds/sdl/usdl/usdl_types.h>
#include <string.h>
#include <assert.h>
#include <ccsds/nl/epp/epp.h>


int mapp_send(map_t *map, uint8_t *data, size_t size, pvn_t pvn, quality_of_service_t qos) {
#if MAP_BUFFER_SIZE == 0
	return 0;
#else
	assert(map->split_allowed);
	assert(map->size_fixed);
	assert(map->payload_size <= MAP_BUFFER_SIZE);
	map_buffer_t *buf = (qos == QOS_EXPEDITED ? &map->buf_ex : &map->buf_sc);

	map_params_t params = {0};
	params.qos = qos;
	params.map_id = map->map_id;
	params.rules = TFDZCR_SPAN;
	params.upid = UPID_SP_OR_EP;
	params.fhd = buf->fhd;

	if (pvn != PVN_ENCAPSULATION_PROTOCOL && pvn != PVN_SPACE_PACKET) {
		return 0;
	}

	if (buf->index == map->payload_size) {
		if (map_mx_multiplex(map->map_mx, &params, buf->data, map->payload_size)) {
			params.fhd = buf->fhd = 0;
			buf->index = 0;
		} else {
			return 0;
		}
	}

	if (buf->fhd == (uint16_t)-1) {
		params.fhd = buf->fhd = buf->index;
	}
	size_t data_index = 0;
	size_t count_to_send = 0;

	while (1) {
		count_to_send = map->payload_size - buf->index < size - data_index
				? map->payload_size - buf->index : size - data_index;

		memcpy(&buf->data[buf->index], &data[data_index], count_to_send);
		buf->index += count_to_send;
		data_index += count_to_send;
		if (buf->index == map->payload_size &&
				map_mx_multiplex(map->map_mx, &params, buf->data, count_to_send)) {
			buf->index = 0;
			params.fhd = buf->fhd = (fhd_t)~0;
		} else {
			break;
		}
	}
	return data_index;
#endif
}

int mapa_send();

int map_stream_send();

int map_request_from_down(map_t *map) {
	map_buffer_t *buf = 0;

	map_params_t params = {0};

	if (map->buf_ex.index > 0) {
		params.qos = QOS_EXPEDITED;
		buf = &map->buf_ex;
	} else if (map->buf_sc.index > 0) {
		params.qos = QOS_SEQ_CONTROL;
		buf = &map->buf_sc;
	} else {
		return 0;
	}
	if (buf->index < map->payload_size) {
		epp_header_t head = {0};
		head.epp_id = EPP_ID_IDLE;
		epp_make_header_auto_length(&head, &buf->data[buf->index],
				map->payload_size - buf->index, map->payload_size - buf->index);
	}

	params.map_id = map->map_id;
	params.rules = TFDZCR_SPAN;
	params.upid = UPID_SP_OR_EP;
	params.fhd = buf->fhd;

	int ret = map_mx_multiplex(map->map_mx, &params, buf->data, map->payload_size);
	if (ret) {
		buf->fhd = 0;
		buf->index = 0;
	}
	return ret;
}

pvn_t _map_get_pvn(uint8_t v) {
	return (v >> 5) & 3;
}

int _map_try_calc_size(uint8_t *data, int size) {
	pvn_t pvn = _map_get_pvn(data[0]);
	if (pvn == PVN_ENCAPSULATION_PROTOCOL) {
		epp_header_t h = {0};
		if (epp_extract_header(&h, data, size)) {
			return h.packet_length;
		} else {
			return 0;
		}
	} else {
		return -1;
	}
}


int _map_fhd_is_valid(mapr_t *map) {
	if (map->tfdz.fhd == (fhd_t)~0) {
		return map->packet.size - map->packet.index >= map->tfdz.size - map->tfdz.index;
	}

	return map->packet.size - map->packet.index == map->tfdz.fhd - map->tfdz.index;
}

int _map_push_from_down(mapr_t *map) {
	if (map->state == MAPR_STATE_FINISH) {
		return 0;
	}
	if (map->state == MAPR_STATE_BEGIN) {
		if (map->tfdz.fhd == (fhd_t)~0) {
			map->tfdz.index = map->tfdz.size;
			return 0;
		}

		map->tfdz.index = map->tfdz.fhd;
		map->state = MAPR_STATE_SIZE_UNKNOWN;
	}

	if (map->state == MAPR_STATE_SIZE_UNKNOWN) {
		while (map->tfdz.index < map->tfdz.size && map->packet.index < map->packet.max_size) {
			if (map->tfdz.index == map->tfdz.fhd) {
				map->packet.index = 0;
			}
			map->packet.data[map->packet.index++] = map->tfdz.data[map->tfdz.index++];
			int ret = _map_try_calc_size(map->packet.data, map->packet.index);

			if (ret > 0 && ret <= map->packet.max_size) {
				map->packet.size = ret;
				map->state = MAPR_STATE_SIZE_KNOWN;
				break;
			} else if (ret < 0 || ret > map->packet.max_size) {
				map->tfdz.index = map->tfdz.size;
				map->packet.index = 0;
				map->state = MAPR_STATE_BEGIN;
				break;
			}
		}
		if (map->packet.index == map->packet.max_size && map->state == MAPR_STATE_SIZE_UNKNOWN) {
			map->state = MAPR_STATE_SIZE_UNKNOWN;
			map->packet.index = 0;
		}
	}

	if (map->state == MAPR_STATE_SIZE_KNOWN) {
		if (!_map_fhd_is_valid(map)) {
			map->state = MAPR_STATE_BEGIN;
			map->packet.size = 0;
			map->packet.index = 0;
		}
		while (map->tfdz.index < map->tfdz.size && map->packet.index < map->packet.size) {
			map->packet.data[map->packet.index++] = map->tfdz.data[map->tfdz.index++];
		}
		if (map->packet.index == map->packet.size) {
			map->state = MAPR_STATE_FINISH;
			map->packet.index = 0;
			return 1;
		}
	}
	return 0;
}

int _map_save_from_down(mapr_t *map, const uint8_t *data, size_t size, const map_params_t *params) {

	if (map->tfdz.size != 0 || map->tfdz.max_size < size) {
		return 0;
	} else {
		memcpy(map->tfdz.data, data, size);
		map->tfdz.size = size;
		map->tfdz.index = 0;
		map->tfdz.fhd = params->fhd;
		return size;
	}
}



int map_recieve_from_down(mapr_t *map, const uint8_t *data, size_t size, const map_params_t *params)  {
	_map_push_from_down(map);
	int ret = _map_save_from_down(map, data, size, params);
	_map_push_from_down(map);
	return ret;
}

int map_recieve(mapr_t *map, uint8_t *data, size_t size, quality_of_service_t *qos) {
	if (map->state != MAPR_STATE_FINISH || size < map->packet.size) {
		return 0;
	}

	memcpy(data, map->packet.data, map->packet.size);
	return map->packet.size;
}

