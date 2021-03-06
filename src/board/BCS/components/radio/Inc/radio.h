/*
 * send.h
 *
 *  Created on: Jul 24, 2020
 *      Author: sereshotes
 */

#ifndef COMPONENTS_RADIO_INC_RADIO_H_
#define COMPONENTS_RADIO_INC_RADIO_H_

#include "ring_buffer.h"
#include "sx126x_drv.h"
#include "mavlink_help2.h"

// Периоды отправки сообщений. Больше - реже
#define RADIO_SEND_ID_ARRAY(F) \
	F(0, MAVLINK_MSG_ID_HEARTBEAT, 2000000000) \
	F(1, MAVLINK_MSG_ID_ELECTRICAL_STATE, 40) \
	F(2, MAVLINK_MSG_ID_OWN_TEMP, 20) \
	F(3, MAVLINK_MSG_ID_THERMAL_STATE, 40) \
	F(4, MAVLINK_MSG_ID_PLD_BME280_DATA, 20) \
	F(5, MAVLINK_MSG_ID_PLD_MS5611_DATA, 20) \
	F(6, MAVLINK_MSG_ID_PLD_ME2O2_DATA, 20) \
	F(7, MAVLINK_MSG_ID_PLD_MICS_6814_DATA, 20) \
	F(8, MAVLINK_MSG_ID_SINS_isc, 10) \
	F(9, MAVLINK_MSG_ID_GPS_UBX_NAV_SOL, 20) \

#define RADIO_SEND_BAN(F) \
	F(MAVLINK_MSG_ID_TIMESTAMP)

#define RADIO_DEFAULT_PERIOD 30
#define RADIO_SLEEP_DEFAULT 2000

#define RADIO_PACKET_SIZE ITS_RADIO_PACKET_SIZE
#define RADIO_RX_TIMEDOUT_LIMIT 5
#define RADIO_RSSI_PERIOD_MS (500)
#define SERVER_TX_STATE_PERIOD_MS (500)

#define _DIV_TRUNC(a, b) (((a) + (b) - 1) / (b))

#define RADIO_TX_PERIOD (5000)
//#define RADIO_RX_PERIOD (2000)
#define RADIO_RX_PERIOD (3000)
#define RADIO_START_ANYWAY (10000)
/*
 * Это поле не меняется автоматически!!! См. _radio_init
 */
#warning "Это поле меняется вручную в зависисимости от периода отправки сообщений"
//#define RADIO_TX_COUNT 7
#define RADIO_TX_COUNT 50

typedef uint64_t msg_cookie_t;
#define MSG_COOKIE_T_PLSHOLDER PRIu64


typedef struct {
	size_t size;
	size_t index;
	size_t capacity;
	uint8_t buf[ITS_RADIO_PACKET_SIZE];
} radio_buf_t;


typedef struct {
	size_t size;
	size_t index;
	size_t capacity;
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];
} mav_buf_t;

typedef struct {
	uint16_t count_recieved_mavlink_msgs;
	uint32_t last_packet_time;
} radio_stats_t;

typedef struct radio_t {
	sx126x_drv_t dev;
	mav_buf_t mav_buf;

	radio_buf_t radio_buf[RADIO_TX_COUNT];
	ring_buffer_t radio_ring_buffer;

	uint32_t wait_period;

	uint8_t mavlink_chan;
	uint32_t msg_count;
	uint16_t packet_count;
	int is_ready_to_send;

	radio_stats_t stats;
} radio_t;


/*
 * Инициализирует отправку сообщений по радио с заданными через
 * RADIO_SEND_ID_ARRAY, RADIO_SEND_BAN, RADIO_DEFAULT_PERIOD их частотами.
 */
void radio_send_init(void);

void radio_send_suspend(void);

void radio_send_resume(void);

void radio_send_set_sleep_delay(int64_t sleep_delay);


#endif /* COMPONENTS_RADIO_INC_RADIO_H_ */
