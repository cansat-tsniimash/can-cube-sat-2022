#ifndef __MP5515_REGS_H__
#define __MP5515_REGS_H__


#include <stdint.h>
#include <stddef.h>
#include "mp5515_packing.h"


struct _mp5515_t;
typedef struct _mp5515_t mp5515_t;


// These functions should be defined by user of this driver
// These functions should write/read to/from register
int mp5515_reg_write(mp5515_t* dev, uint8_t address, const uint8_t* data, size_t size);
int mp5515_reg_read(mp5515_t* dev, uint8_t address, uint8_t* data, size_t size);


int mp5515_reg_vendor_id_write(mp5515_t* dev, const mp5515_reg_vendor_id_t* reg);
int mp5515_reg_vendor_id_read(mp5515_t* dev, mp5515_reg_vendor_id_t* reg);

int mp5515_reg_sys_ctrl1_write(mp5515_t* dev, const mp5515_reg_sys_ctrl1_t* reg);
int mp5515_reg_sys_ctrl1_read(mp5515_t* dev, mp5515_reg_sys_ctrl1_t* reg);

int mp5515_reg_sys_ctrl2_write(mp5515_t* dev, const mp5515_reg_sys_ctrl2_t* reg);
int mp5515_reg_sys_ctrl2_read(mp5515_t* dev, mp5515_reg_sys_ctrl2_t* reg);

int mp5515_reg_sys_ctrl3_write(mp5515_t* dev, const mp5515_reg_sys_ctrl3_t* reg);
int mp5515_reg_sys_ctrl3_read(mp5515_t* dev, mp5515_reg_sys_ctrl3_t* reg);

int mp5515_reg_sys_ctrl4_write(mp5515_t* dev, const mp5515_reg_sys_ctrl4_t* reg);
int mp5515_reg_sys_ctrl4_read(mp5515_t* dev, mp5515_reg_sys_ctrl4_t* reg);

int mp5515_reg_int1_status_write(mp5515_t* dev, const mp5515_reg_int1_status_t* reg);
int mp5515_reg_int1_status_read(mp5515_t* dev, mp5515_reg_int1_status_t* reg);

int mp5515_reg_int2_status_write(mp5515_t* dev, const mp5515_reg_int2_status_t* reg);
int mp5515_reg_int2_status_read(mp5515_t* dev, mp5515_reg_int2_status_t* reg);

int mp5515_reg_int1_mask_ctrl_write(mp5515_t* dev, const mp5515_reg_int1_mask_ctrl_t* reg);
int mp5515_reg_int1_mask_ctrl_read(mp5515_t* dev, mp5515_reg_int1_mask_ctrl_t* reg);

int mp5515_reg_int2_mask_ctrl_write(mp5515_t* dev, const mp5515_reg_int2_mask_ctrl_t* reg);
int mp5515_reg_int2_mask_ctrl_read(mp5515_t* dev, mp5515_reg_int2_mask_ctrl_t* reg);

int mp5515_reg_bkp_cap_thd_write(mp5515_t* dev, const mp5515_reg_bkp_cap_thd_t* reg);
int mp5515_reg_bkp_cap_thd_read(mp5515_t* dev, mp5515_reg_bkp_cap_thd_t* reg);

int mp5515_reg_temp_warn_thd_write(mp5515_t* dev, const mp5515_reg_temp_warn_thd_t* reg);
int mp5515_reg_temp_warn_thd_read(mp5515_t* dev, mp5515_reg_temp_warn_thd_t* reg);

int mp5515_reg_adc_src_select_write(mp5515_t* dev, const mp5515_reg_adc_src_select_t* reg);
int mp5515_reg_adc_src_select_read(mp5515_t* dev, mp5515_reg_adc_src_select_t* reg);

int mp5515_reg_adc_input_voltage_data_write(mp5515_t* dev, const mp5515_reg_adc_input_voltage_data_t* reg);
int mp5515_reg_adc_input_voltage_data_read(mp5515_t* dev, mp5515_reg_adc_input_voltage_data_t* reg);

int mp5515_reg_adc_input_current_data_write(mp5515_t* dev, const mp5515_reg_adc_input_current_data_t* reg);
int mp5515_reg_adc_input_current_data_read(mp5515_t* dev, mp5515_reg_adc_input_current_data_t* reg);

int mp5515_reg_adc_temp_voltage_data_write(mp5515_t* dev, const mp5515_reg_adc_temp_voltage_data_t* reg);
int mp5515_reg_adc_temp_voltage_data_read(mp5515_t* dev, mp5515_reg_adc_temp_voltage_data_t* reg);

int mp5515_reg_adc_bkp_voltage_data1_write(mp5515_t* dev, const mp5515_reg_adc_bkp_voltage_data1_t* reg);
int mp5515_reg_adc_bkp_voltage_data1_read(mp5515_t* dev, mp5515_reg_adc_bkp_voltage_data1_t* reg);

int mp5515_reg_adc_bkp_voltage_data2_write(mp5515_t* dev, const mp5515_reg_adc_bkp_voltage_data2_t* reg);
int mp5515_reg_adc_bkp_voltage_data2_read(mp5515_t* dev, mp5515_reg_adc_bkp_voltage_data2_t* reg);

int mp5515_reg_cap_test_timer_write(mp5515_t* dev, const mp5515_reg_cap_test_timer_t* reg);
int mp5515_reg_cap_test_timer_read(mp5515_t* dev, mp5515_reg_cap_test_timer_t* reg);

int mp5515_reg_vin_recover_ctrl_write(mp5515_t* dev, const mp5515_reg_vin_recover_ctrl_t* reg);
int mp5515_reg_vin_recover_ctrl_read(mp5515_t* dev, mp5515_reg_vin_recover_ctrl_t* reg);


#endif // __MP5515_REGS_H__
