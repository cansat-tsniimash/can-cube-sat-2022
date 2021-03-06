/*
 * dosim.h
 *
 *  Created on: May 8, 2021
 *      Author: User
 */

#ifndef INC_SENSORS_DOSIM_H_
#define INC_SENSORS_DOSIM_H_

#include "mavlink_main.h"

void dosim_init(void);

void dosim_work(void);

void dosim_read_momentary(mavlink_pld_dosim_data_t * msg);

void dosim_read_windowed(mavlink_pld_dosim_data_t * msg);

#endif /* INC_SENSORS_DOSIM_H_ */
