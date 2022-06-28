/*
 * interupts.c
 *
 *  Created on: Jun 2, 2022
 *      Author: seres
 */

#include "main.h"
#include <its-time.h>
#include <task_dcdc.h>


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
    case TIME_Pin:
        its_sync_time_exti_cb();
        break;

    case PGB_C_Pin:
        task_dcdc_exti_cb(mp5515_pin_pgb, dcdc_type_charge);
        break;
    case PGS_C_Pin:
        task_dcdc_exti_cb(mp5515_pin_pgs, dcdc_type_charge);
        break;
    case PFI_C_Pin:
        task_dcdc_exti_cb(mp5515_pin_pfi, dcdc_type_charge);
        break;
    case INT_C_Pin:
        task_dcdc_exti_cb(mp5515_pin_int, dcdc_type_charge);
        break;

    case PGB_D_Pin:
        task_dcdc_exti_cb(mp5515_pin_pgb, dcdc_type_discharge);
        break;
    case PGS_D_Pin:
        task_dcdc_exti_cb(mp5515_pin_pgs, dcdc_type_discharge);
        break;
    case PFI_D_Pin:
        task_dcdc_exti_cb(mp5515_pin_pfi, dcdc_type_discharge);
        break;
    case INT_D_Pin:
        task_dcdc_exti_cb(mp5515_pin_int, dcdc_type_discharge);
        break;
    }
}
