/*
 * radio_help.h
 *
 *  Created on: Jul 6, 2021
 *      Author: sereshotes
 */

#ifndef COMPONENTS_RADIO_INC_PRIVATE_RADIO_HELP_H_
#define COMPONENTS_RADIO_INC_PRIVATE_RADIO_HELP_H_

#include "radio.h"

typedef struct  {
	int id; //ID сообщения
	float period; //Период отправки сообщений (в кол-ве сообщений)
	int last; //Номер сообщения, когда был отправлен
	int is_updated; //Обновлен ли после последнего отправления
	mavlink_message_t last_msg; //Сообщение на отправку
} msg_container;

#define log_error(fmt, ...) ESP_LOGE("radio", fmt, ##__VA_ARGS__)
#define log_trace(fmt, ...) ESP_LOGD("radio", fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) ESP_LOGW("radio", fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) ESP_LOGI("radio", fmt, ##__VA_ARGS__)



/*
 * Буфер всех заданных сообщений
 */
#define F(__msg_hash, __msg_id, __period) {__msg_id, __period, -__period, 0, {0}},
static const msg_container arr_id[] = {
		RADIO_SEND_ID_ARRAY(F)
};
#undef F


typedef enum {
	RS_NONE,
	RS_TX,
	RS_RX,
} radio_trans_dir_state_t;

typedef struct {
	uint32_t last_left_count;
	uint32_t tx_count;
	uint32_t did_panic_happened;
	uint32_t min_tx_left;
	uint32_t big_tx_count;
	int32_t buffered_packet_count;
} radio_timings_t;

typedef struct {
	int packet_done;
	int i_really_want_to_start_now;
	int64_t last_sent;
	int64_t last_added;
	int64_t period_start;
	uint32_t error_count_tx;
	uint32_t error_count_rx;
	radio_trans_dir_state_t dir_state;
	radio_timings_t timings;
} radio_private_state_t;

/*
 * Настройки safe_uart_send
 */
typedef struct  {
	uint32_t baud_send; //Баудрейт устр-ва в бит/сек
	uint32_t buffer_size; //Размер буфера внутри устр-ва
	uart_port_t port; //Порт уарта
	int super_portion_byte_limit;
	int empty_buffer_time_limit;
	int64_t sleep_delay;
} safe_send_cfg_t;

/*
 * Параметры safe_uart_send
 */

typedef enum sleep_state_t
{
	SLEEP_STATE_SENDING,
	SLEEP_STATE_WAIT_ZERO,
	SLEEP_STATE_WAIT_TIME,
} sleep_state_t;



typedef struct  {
	safe_send_cfg_t cfg; //Настройки
	int filled;	//Заполненность буферв
	int64_t last_checked; //Время в мкс последнего изменения filled
	sleep_state_t sleep_state;
	int super_portion_byte_counter;
	int empty_buffer_time_deadline;
} safe_send_t;


int rbuf_fill(radio_t * server);

radio_buf_t* rbuf_get(radio_t * server);

void rbuf_pull(radio_t * server);

void rbuf_reset(radio_t* server, radio_private_state_t* state);

void update_msg(const mavlink_message_t *msg);

#endif /* COMPONENTS_RADIO_INC_PRIVATE_RADIO_HELP_H_ */
