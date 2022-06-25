/*
 * task_dcdc.h
 *
 *  Created on: Jun 2, 2022
 *      Author: seres
 */

#ifndef INC_TASK_DCDC_H_
#define INC_TASK_DCDC_H_



typedef enum {
    mp5515_pin_pgb,
    mp5515_pin_pgs,
    mp5515_pin_pfi,
    mp5515_pin_int,
} mp5515_pin_t;

typedef enum {
    dcdc_type_charge,
    dcdc_type_discharge,

    dcdc_type_count
} dcdc_type_t;

void task_dcdc_exti_cb(mp5515_pin_t pin, dcdc_type_t dcdc_type);

#endif /* INC_TASK_DCDC_H_ */
