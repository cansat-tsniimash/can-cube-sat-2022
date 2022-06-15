#include "../Inc_private/timing_calc.h"
#include "../Inc_private/radio_help.h"


/*
 * Хэш-функция для получения индекса в массиве arr_id для заданного
 * номера сообщения.
 * Работает через switch/case. В теории C компилятор способен
 * оптимизировать это при больших кол-ах case. Поэтому через
 * define определенно switch/case выражение, которое возвращает
 * индекс в массиве arr_id для msgid данного сообщения.
 */
static int get_hash(int id) {
#define F(x,a,b) case a: return x;
	switch(id) {
	RADIO_SEND_ID_ARRAY(F)
	default:
		return -1;
	}
#undef F
}

//! Проверка на то, что сообщение забанено на отправку по радио
/*! Вернет 0, если не забанено и не 0, если забанено */
static int is_msg_banned(int msg_id)
{
#define F(__msg_id) case __msg_id: return 1;
	switch (msg_id) {
	RADIO_SEND_BAN(F)
	default: break;
	}
#undef F

	return 0;
}

#define RADIO_SEND_BUF_SIZE 100
static msg_container arr_buf[RADIO_SEND_BUF_SIZE];
static int arr_buf_size = 0;


/*
 * Поиск контейнера для данного сообщения в буфере arr_buf
 */
static msg_container* find(const mavlink_message_t *msg) {
	for (int i = 0; i < arr_buf_size; i++) {
		if (arr_buf[i].last_msg.msgid == msg->msgid &&
			arr_buf[i].last_msg.compid == msg->compid &&
			arr_buf[i].last_msg.sysid == msg->sysid) {
			return  &arr_buf[i];
		}
	}
	return 0;
}
/*
 * Создание контейнера в буфере arr_buf для данного сообщения
 */
static int add(const mavlink_message_t *msg) {

	if (is_msg_banned(msg->msgid)) {
		return 2;
	}

	if (arr_buf_size >= RADIO_SEND_BUF_SIZE) {
		ESP_LOGE("radio", "No free space for new msg");
		return 1;
	}

	log_trace("Add: %d %d:%d", msg->msgid, msg->sysid, msg->compid);
	int id = get_hash(msg->msgid);
	if (id >= 0) {
		arr_buf_size++;
		arr_buf[arr_buf_size - 1] = arr_id[id];
	} else {
		arr_buf_size++;
		arr_buf[arr_buf_size - 1].id = msg->msgid;
		arr_buf[arr_buf_size - 1].period = RADIO_DEFAULT_PERIOD;
		arr_buf[arr_buf_size - 1].last = -RADIO_DEFAULT_PERIOD;
	}
	return 0;
}

/*
 * Обновление сообщения в соответствующем контейнере из буфера arr_buf.
 * Если контейнера нет - создает новый.
 */
void update_msg(const mavlink_message_t *msg) {
	msg_container *st = find(msg);
	if (!st) {
		if (add(msg)) {
			return;
		}
		st = &arr_buf[arr_buf_size - 1];
	}
	st->last_msg = *msg;
	st->is_updated = 1;
}
/*
 * Коэффициент важности/срочности/хорошести данного сообщения.
 * Больше - важнее.
 */
static float get_coef(const msg_container *a, int now) {
	//assert(a->period > 0);
	return (now - a->last) / a->period;
}
/*
 * Отбирает самое важное/срочное/лучшее сообщение для отправки.
 * now - количество отпраленных сообщений до этого момента.
 */
static msg_container *get_best(int now) {
	msg_container *st_best = 0;
	float coef_best = 0;
	for (int i = 0; i < arr_buf_size; i++) {
		float coef = get_coef(&arr_buf[i], now);
		if (coef > coef_best && arr_buf[i].is_updated) {
			st_best = &arr_buf[i];
			coef_best = coef;
		}
	}
	if (st_best) {
		log_trace("chosen %d with coef %f, period %f, last %d, now %d",
				st_best->id, coef_best, st_best->period, st_best->last, now);
	} else {
		log_trace("chosen none");
	}
	return st_best;
}

/*

static 	safe_send_t sst = {
	.cfg = {
			.baud_send = 2400 / 2,
			.buffer_size = 1000,
			.port = ITS_UARTR_PORT,
			.super_portion_byte_limit = 300,
			.empty_buffer_time_limit = 0,
			.sleep_delay = RADIO_DEFAULT_PERIOD,
	},
	.sleep_state = SLEEP_STATE_SENDING,
	.super_portion_byte_counter = 300
};
*/

/*
static int is_sleeping(safe_send_t *sst) {
	int new_state = sst->sleep_state;
	int64_t now = esp_timer_get_time();
	if (sst->sleep_state == 0) {
		new_state = (now - sst->last_changed_sleep_time) / 1000 > RADIO_SLEEP_AWAKE_LEGNTH ? 1 : 0;
	} else {
		new_state = (now - sst->last_changed_sleep_time) / 1000 > RADIO_SLEEP_SLEEP_LENGTH ? 0 : 1;
	}
	if (new_state != sst->sleep_state) {
		ESP_LOGD("radio", "state: %d %d %d", (int)now, (int)sst->last_changed_sleep_time, (int)sst->sleep_state);
		sst->last_changed_sleep_time = now;
	}
	sst->sleep_state = new_state;
	return sst->sleep_state;
}
*//*
static int fill_one_packet(radio_t * server) {
	log_info("fill_one_packet %d" ,server->packet_count);
	if (server->radio_buf.index == 0) {
		uint16_t count = server->packet_count;

		memcpy(server->radio_buf.buf, (uint8_t *)&count, sizeof(count));

		server->packet_count++;
		server->radio_buf.index += sizeof(count);
	}
	if (server->radio_buf.index < server->radio_buf.size) {
		log_info("gen %d %d", server->radio_buf.index, server->radio_buf.size);
		if (server->mav_buf.size - server->mav_buf.index <= 0) {
			msg_container *st = 0;
			st = get_best(server->msg_count);
			if (0 == st) {
				goto end;
			}

			server->mav_buf.size = mavlink_msg_to_send_buffer(server->mav_buf.buf, &st->last_msg);
			server->mav_buf.index = 0;
			st->is_updated = 0; // Сообщение внутри контейнера уже не свежее
			st->last = server->msg_count;// Запоминаем, когда сообщение было отправленно в последний раз
			server->msg_count++;
		}

		uint8_t *out = server->mav_buf.buf + server->mav_buf.index;
		int cnt = server->mav_buf.size - server->mav_buf.index;
		int diff = server->radio_buf.size - server->radio_buf.index;
		if (cnt > diff) {
			cnt = diff;
		}
		memcpy(server->radio_buf.buf + server->radio_buf.index, out, cnt);
		server->mav_buf.index += cnt;
		server->radio_buf.index += cnt;

	}
end:
	return server->radio_buf.index == server->radio_buf.size;
}*/


int rbuf_fill(radio_t * server) {
	log_trace("rbuf_fill %d %d", server->radio_ring_buffer.put, server->radio_ring_buffer.get);
	if (ring_buffer_put_avail(&server->radio_ring_buffer) == 0) {
		return 0;
	}
	radio_buf_t *buf = ring_buffer_put(&server->radio_ring_buffer);
	ring_buffer_put_advance(&server->radio_ring_buffer);
	buf->index = 0;
	uint16_t count = server->packet_count;

	memcpy(buf->buf, (uint8_t *)&count, sizeof(count));

	server->packet_count++;

	buf->index += sizeof(count);

	while (buf->index < buf->size) {
		log_trace("gen %d %d", buf->index, buf->size);
		if (server->mav_buf.size - server->mav_buf.index > 0) {
			uint8_t *out = server->mav_buf.buf + server->mav_buf.index;
			int cnt = server->mav_buf.size - server->mav_buf.index;
			int diff = buf->size - buf->index;
			if (cnt > diff) {
				cnt = diff;
			}
			memcpy(buf->buf + buf->index, out, cnt);
			server->mav_buf.index += cnt;
			buf->index += cnt;
		} else {
			msg_container *st = 0;
			st = get_best(server->msg_count);
			if (0 == st) {
				static uint8_t idle_count = 0;
				mavlink_heartbeat_t mhb = {0};
				mhb.system_status = idle_count++;
				mavlink_message_t msg = {0};
				mavlink_msg_heartbeat_encode(mavlink_system, 0, &msg, &mhb);
				update_msg(&msg);

				st = get_best(server->msg_count);
			}

			server->mav_buf.size = mavlink_msg_to_send_buffer(server->mav_buf.buf, &st->last_msg);
			server->mav_buf.index = 0;
			st->is_updated = 0; // Сообщение внутри контейнера уже не свежее
			st->last = server->msg_count;// Запоминаем, когда сообщение было отправленно в последний раз
			server->msg_count++;
		}
	}
	if (buf->index < buf->size) {
		memset(buf->buf + buf->index, 0, buf->size - buf->index);
	}
	return 1;
}

radio_buf_t* rbuf_get(radio_t * server) {
	log_trace("rbuf_get %d", server->radio_ring_buffer.get);
	assert(ring_buffer_get_avail(&server->radio_ring_buffer) > 0);
	return ring_buffer_get(&server->radio_ring_buffer);
}
void rbuf_pull(radio_t * server) {
	log_trace("rbuf_pull %d", server->radio_ring_buffer.get);

	ring_buffer_get_advance(&server->radio_ring_buffer);
}

void rbuf_reset(radio_t* server, radio_private_state_t* state) {
	log_trace("radio reset");
	timing_calc_init(server, &state->timings);
	server->radio_ring_buffer.put = 0;

}
