/*
 * mp5515.h
 *
 *  Created on: May 28, 2022
 *      Author: seres
 */

#ifndef MP5515_INC_MP5515_H_
#define MP5515_INC_MP5515_H_

#include "main.h"
#include "mp5515_regs.h"

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} gpio_t;

typedef enum {

    mp5515_event_io_fail,
    mp5515_event_input_over_current,
    mp5515_event_input_over_voltage,
    mp5515_event_input_power_fail,
    mp5515_event_pgs_not_ok,
    mp5515_event_temp_warn,
    mp5515_event_die_temp_warn,
    mp5515_event_forced_buck,
    mp5515_event_sas_disable_supply,
    mp5515_event_pgb_not_ok,
    mp5515_event_isofet_off,
    mp5515_event_timeout,

    mp5515_event_error_count,

    mp5515_event_adc_done = mp5515_event_error_count,
    mp5515_event_cap_test_done,

    mp5515_event_type_count,
} mp5515_event_type_t;

typedef uint16_t mp5515_event_t;
#define MP5515_EVENT_IO_FAIL                (1 << mp5515_event_io_fail)
#define MP5515_EVENT_INPUT_OVER_CURRENT     (1 << mp5515_event_input_over_current)
#define MP5515_EVENT_INPUT_OVER_VOLTAGE     (1 << mp5515_event_input_over_voltage)
#define MP5515_EVENT_INPUT_POWER_FAIL       (1 << mp5515_event_input_power_fail)
#define MP5515_EVENT_PGS_NOT_OK             (1 << mp5515_event_pgs_not_ok)
#define MP5515_EVENT_TEMP_WARN              (1 << mp5515_event_temp_warn)
#define MP5515_EVENT_DIE_TEMP_WARN          (1 << mp5515_event_die_temp_warn)
#define MP5515_EVENT_FORED_BUCK             (1 << mp5515_event_forced_buck)
#define MP5515_EVENT_SAS_DISABLE_SUPPLY     (1 << mp5515_event_sas_disable_supply)
#define MP5515_EVENT_PGB_NOT_OK             (1 << mp5515_event_pgb_not_ok)
#define MP5515_EVENT_ISOFET_OFF             (1 << mp5515_event_isofet_off)
#define MP5515_EVENT_TIMEOUT                (1 << mp5515_event_timeout)
#define MP5515_EVENT_ADC_DONE               (1 << mp5515_event_adc_done)
#define MP5515_EVENT_CAP_TEST_DONE          (1 << mp5515_event_cap_test_done)

typedef struct _mp5515_t {
    I2C_HandleTypeDef* hi2c;
    uint8_t address;
    uint32_t timeout;
    int poll_or_int;
    gpio_t pin_int;
    gpio_t pin_pfi;
    gpio_t pin_pgs;
    gpio_t pin_pgb;
    uint32_t r_dc; //Ohms
    uint32_t r_lim; //Ohms
    struct {
        mp5515_reg_int1_mask_ctrl_t int1_mask_ctrl;
        mp5515_reg_int1_mask_ctrl_t int2_mask_ctrl;
        mp5515_reg_bkp_cap_thd_t bkp_cap_thd;
        mp5515_reg_temp_warn_thd_t temp_warn_thd;
        mp5515_reg_vin_recover_ctrl_t vin_recover_ctrl;
        mp5515_reg_sys_ctrl1_t sys_ctrl1;
        mp5515_reg_sys_ctrl2_t sys_ctrl2;
        mp5515_reg_sys_ctrl3_t sys_ctrl3;
        mp5515_reg_sys_ctrl4_t sys_ctrl4;
    } regs;
} mp5515_t;

void mp5515_init(mp5515_t* dev);

int mp5515_reset_int(mp5515_t* dev);


int mp5515_adc_start(mp5515_t* dev, mp5515_reg_adc_src_select_t select);

int mp5515_adc_poll(mp5515_t* dev);

int mp5515_adc_get_in_voltage(mp5515_t* dev, float* vin);

int mp5515_adc_get_in_current(mp5515_t* dev, float* current);

int mp5515_adc_get_bkp_voltage1(mp5515_t* dev, float* voltage);

int mp5515_adc_get_bkp_voltage2(mp5515_t* dev, float* voltage);


int mp5515_get_events(mp5515_t* dev, mp5515_event_t* event);

#endif /* MP5515_INC_MP5515_H_ */
