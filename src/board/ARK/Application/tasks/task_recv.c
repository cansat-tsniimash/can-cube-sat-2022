/*
 * task_recv.c
 *
 *  Created on: Apr 28, 2020
 *      Author: sereshotes
 */
#include "task_recv.h"

#include "main.h"

#include "task.h"
#include "its-i2c-link.h"
#include "assert.h"
#include "its-time.h"


void task_recv_init(void *arg) {

}

static int count = 0;

void time_recv_callback(const timestamp_data_t *msg) {

    uint8_t time_base = 0;
    its_get_time_base(&time_base);
    if (time_base > msg->time_base) {
        return;
    }

    its_set_time_base(msg->time_base);
    its_time_t t;
    t.sec = msg->time_s;
    t.msec = msg->time_us / 1000;
    its_sync_time(&t);
}
void task_recv_update(void *arg) {
    uint8_t buf[MAVLINK_HELP_DATA_SIZE];
    int size = its_i2c_link_read(buf, sizeof(buf));
    if (size < 0) {
        return;
    }
    mavlink_help_type_t mht = 0;
    if (!mavlink_help_parse_packet(buf, size, &mht)) {
        return;
    }
    if (mht == mavlink_help_type_timestamp) {
        timestamp_data_t ts = {0};
        mavlink_help_timestamp_parse(buf, size, &ts);
        time_recv_callback(&ts);
    }
}
