/*
 * control_heat.h
 *
 *  Created on: Aug 8, 2020
 *      Author: sereshotes
 */

#ifndef COMPONENTS_CONTROL_HEAT_INC_CONTROL_HEAT_H_
#define COMPONENTS_CONTROL_HEAT_INC_CONTROL_HEAT_H_

#include "shift_reg.h"

//#define CONTROL_HEAT_HIGHTHD 34.0
#define CONTROL_HEAT_LOWTHD (30.0)
#define CONTROL_HEAT_UPDATE_PERIOD 10000 //ms
#define CONTROL_HEAT_RADIO_HIGH_THD 35.0
#define CONTROL_HEAT_RADIO_LOW_THD 30.0

int control_heat_init(shift_reg_handler_t *hsr, int shift, int task_on);

void control_heat_bsk_enable(int pin, int is_on);

void control_heat_set_consumption(int id, int current);

void control_heat_set_max_consumption(int current);

void control_heat_suspend();

void control_heat_resume();

#endif /* COMPONENTS_CONTROL_HEAT_INC_CONTROL_HEAT_H_ */
