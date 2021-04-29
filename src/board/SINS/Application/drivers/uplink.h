/*
 * uplink.h
 *
 *  Created on: 8 апр. 2020 г.
 *      Author: snork
 */

#ifndef DRIVERS_UPLINK_H_
#define DRIVERS_UPLINK_H_


#include <its/mavlink.h>

int uplink_init(void);

int uplink_write_raw(const void * data, int data_size);

int uplink_write_mav(const mavlink_message_t * msg);


#endif /* DRIVERS_UPLINK_H_ */
