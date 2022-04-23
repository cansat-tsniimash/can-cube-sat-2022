/*
 * epp.c
 *
 *  Created on: 19 янв. 2021 г.
 *      Author: HP
 */

#include <string.h>

#include "epp.h"
#include "../ccsds_endian.h"

#define PVN 0x7


uint8_t epp_calc_min_lol_packet(size_t packet_length) {
	uint8_t lol = 0;
	if (packet_length == 1) {
		lol = 0;
	} else if (packet_length < 1 << 8) {
		lol = 1;
	} else if (packet_length < 1 << 16) {
		lol = 2;
	} else {
		lol = 3;
	}
	return lol;
}

uint8_t epp_calc_min_lol_payload(size_t payload_length) {
	uint8_t lol = 0;
	if (payload_length == 1) {
		lol = 0;
	} else if (payload_length < (1 << 8) - 1) {
		lol = 1;
	} else if (payload_length < (1 << 16) - 2) {
		lol = 2;
	} else {
		lol = 3;
	}
	return lol;
}

size_t epp_serialize_packet(const epp_packet_t* packet, uint8_t* arr, size_t size) {
	if (size < packet->size + (1 << packet->header.lol)) {
		return 0;
	}
	const epp_header_t* epp_header = &packet->header;

	epp_serialize_header(epp_header, arr, size);
	memcpy(arr + (1 << packet->header.lol), packet->data, packet->size);
	return packet->size + (1 << packet->header.lol);
}

int epp_extract_packet(epp_packet_t* packet, uint8_t* arr, size_t size) {
	epp_header_t* epp_header = &packet->header;
	size_t count = epp_extract_header(epp_header, arr, size);
	if (!count || epp_header->packet_length > size) {
		return -1;
	}

	packet->data = arr + count;
	packet->size = epp_header->packet_length - count;

	return 0;
}


int epp_serialize_header(const epp_header_t *epp_header, uint8_t *arr, int size) {
	if (1 << epp_header->lol > size || epp_header->pvn != PVN) {
		return 0;
	}
	int k = 0;
	ccsds_endian_insert(arr, size * 8, k, &epp_header->pvn, 3);
	ccsds_endian_insert(arr, size * 8, k + 3, &epp_header->epp_id, 3);
	ccsds_endian_insert(arr, size * 8, k + 6, &epp_header->lol, 2);
	k += 8;
	if (epp_header->lol > 2) {
		ccsds_endian_insert(arr, size * 8, k, &epp_header->udf, 4);
		ccsds_endian_insert(arr, size * 8, k + 4, &epp_header->epp_id_ex, 4);
		k += 8;
	}
	if (epp_header->lol == 3) {
		ccsds_endian_insert(arr, size * 8, k, &epp_header->ccsds_defined_field, 16);
		k += 16;
	}
	if (epp_header->lol > 0) {
		ccsds_endian_insert(arr, size * 8, k, &epp_header->packet_length, 1 << (epp_header->lol + 2));
		k += 1 << (epp_header->lol + 2);
	}
	return k / 8;
}


int epp_extract_header(epp_header_t *epp_header, const uint8_t *arr, int size) {
	if (size <= 0) {
		return 0;
	}
	int k = 0;
	ccsds_endian_extract(arr, k, &epp_header->pvn, 3);
	ccsds_endian_extract(arr, k + 3, &epp_header->epp_id, 3);
	ccsds_endian_extract(arr, k + 6, &epp_header->lol, 2);
	k += 8;
	if (1 << epp_header->lol > size) {
		return 0;
	}

	if (epp_header->lol >= 2) {
		ccsds_endian_extract(arr, k, &epp_header->udf, 4);
		ccsds_endian_extract(arr, k + 4, &epp_header->epp_id_ex, 4);
		k += 8;
	} else {
		epp_header->udf = 0;
		epp_header->epp_id_ex = 0;
	}
	if (epp_header->lol == 3) {
		ccsds_endian_extract(arr, k, &epp_header->ccsds_defined_field, 16);
		k += 16;
	} else {
		epp_header->ccsds_defined_field = 0;
	}
	if (epp_header->lol > 0) {
		ccsds_endian_extract(arr, k, &epp_header->packet_length, 1 << (epp_header->lol + 2));
		k += 1 << (epp_header->lol + 2);
	} else {
		epp_header->packet_length = 1;
	}
	return 1 << epp_header->lol;
}

void epp_serialize_empty_packet(uint8_t* data, size_t size) {
    if (size == 0) {
        return;
    }
    epp_header_t eh = {0};
    eh.epp_id = EPP_ID_IDLE;
    eh.pvn = PVN;
	eh.lol = epp_calc_min_lol_packet(size);
	eh.packet_length = size;

	size_t count = epp_serialize_header(&eh, data, size);
    memset(data + count, 0, size - count);
}