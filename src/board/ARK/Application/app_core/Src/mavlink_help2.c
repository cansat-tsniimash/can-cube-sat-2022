/*
 * mavlink_help2.c
 *
 *  Created on: Sep 6, 2020
 *      Author: snork
 */


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
// Тут генерируется множество вот таких сообщений
// warning: taking address of packed member of 'struct __mavlink_vision_speed_estimate_t' may result in an unaligned pointer value [-Waddress-of-packed-member]
// Мы доверяем мавлинку в том, что он не сгенерит ничего невыровненого, поэтому давим эти варнинги
#include <mavlink.h>
#include "mavlink_help2.h"

#include "assert.h"

#if defined CUBE_1
const static uint8_t mavlink_system = CUBE_1_PCU;
#elif defined CUBE_2
const static uint8_t mavlink_system = CUBE_2_PCU;
#endif



mavlink_channel_t mavlink_claim_channel(void) {
    static int channel = -1;
    channel++;
    assert(channel < MAVLINK_COMM_NUM_BUFFERS);
    return (mavlink_channel_t) channel;
}


static int _parse_packet(const uint8_t* data, size_t size, mavlink_message_t* msg) {
    for (size_t i = 0; i < size; i++) {

        if (mavlink_parse_char(0,  data[i], msg, 0)) {
            return 1;
        }
    }
    return 0;
}
int mavlink_help_parse_packet(const uint8_t* data, size_t size, mavlink_help_type_t* type) {
    mavlink_message_t msg;
    int res = _parse_packet(data, size, &msg);
    if (!res) {
        return res;
    }
    switch (msg.msgid) {
    case MAVLINK_MSG_ID_TIMESTAMP:
        *type = mavlink_help_type_timestamp;
        return 1;
    default:
        return 0;
    }
}

void mavlink_help_timestamp_parse(const uint8_t* data, size_t size, timestamp_data_t* ts) {
    mavlink_message_t msg = {0};
    _parse_packet(data, size, &msg);
    mavlink_timestamp_t mts = {0};
    mavlink_msg_timestamp_decode(&msg, &mts);

    ts->time_s = mts.time_s;
    ts->time_us = mts.time_us;
    ts->time_steady = mts.time_steady;
    ts->time_base = mts.time_base;
}

uint16_t mavlink_help_own_temp_serialize(uint8_t* data, size_t size, const own_temp_t* ot) {

    mavlink_message_t msg = {0};
    mavlink_own_temp_t mot = {0};
    mot.time_s = ot->time_s;
    mot.time_us = ot->time_us;
    mot.time_steady = ot->time_steady;
    mot.deg = ot->deg;
    mot.vbat = ot->vbat;
    mot.vdda = ot->vdda;


    mavlink_msg_own_temp_encode(mavlink_system, 0, &msg, &mot);
    return mavlink_msg_to_send_buffer(data, &msg);
}

uint16_t mavlink_help_sp_deployment_state_serialize(uint8_t* data, size_t size, const sp_deployment_state_t* sds) {

    mavlink_message_t msg = {0};
    mavlink_ark_sp_deployment_state_t masds = {0};
    masds.time_s = sds->time_s;
    masds.time_us = sds->time_us;
    masds.time_steady = sds->time_steady;

    for (int i = 0; i < 4; i++) {
    	masds.is_open[i] = sds->is_opened[i];
    }

    mavlink_msg_ark_sp_deployment_state_encode(mavlink_system, 0, &msg, &masds);
    return mavlink_msg_to_send_buffer(data, &msg);
}
