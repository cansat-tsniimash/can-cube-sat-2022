#include "mp5515_packing.h"


static inline void set_bit(uint8_t* arr, size_t index, uint8_t bit) {
    arr[index / 8] &= ~(1 << (index % 8));
    arr[index / 8] |= (bit << (index % 8));
}
static inline uint8_t get_bit(uint8_t* arr, size_t index) {
    return (arr[index / 8] >> (index % 8)) & 1;
}
static inline void set_bits_in_arr(uint8_t* to, const uint8_t* from, size_t to_offset, size_t from_offset, size_t size) {
    for (size_t i = 0; i < size; i++) {
        set_bit(to, i + to_offset, get_bit(from, i + from_offset));
    }
}

void mp5515_reg_vendor_id_serialize(const mp5515_reg_vendor_id_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->fab, 6, 0, 2);
    set_bits_in_arr(data, &reg->major_rev, 4, 0, 2);
    set_bits_in_arr(data, &reg->minor_rev, 2, 0, 2);
    set_bits_in_arr(data, &reg->vendor_id, 2, 0, 2);
}

void mp5515_reg_vendor_id_parse(mp5515_reg_vendor_id_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->fab, data, 0, 6, 2);
    set_bits_in_arr(&reg->major_rev, data, 0, 4, 2);
    set_bits_in_arr(&reg->minor_rev, data, 0, 2, 2);
    set_bits_in_arr(&reg->vendor_id, data, 0, 2, 2);
}


void mp5515_reg_sys_ctrl1_serialize(const mp5515_reg_sys_ctrl1_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->start_cap_test, 3, 0, 1);
    set_bits_in_arr(data, &reg->start_adc, 2, 0, 1);
    set_bits_in_arr(data, &reg->ench, 0, 0, 1);
}

void mp5515_reg_sys_ctrl1_parse(mp5515_reg_sys_ctrl1_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->start_cap_test, data, 0, 3, 1);
    set_bits_in_arr(&reg->start_adc, data, 0, 2, 1);
    set_bits_in_arr(&reg->ench, data, 0, 0, 1);
}


void mp5515_reg_sys_ctrl2_serialize(const mp5515_reg_sys_ctrl2_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->input_ilimt_set, 6, 0, 1);
    set_bits_in_arr(data, &reg->buck_fsw, 1, 0, 3);
    set_bits_in_arr(data, &reg->force_buck_release, 0, 0, 1);
}

void mp5515_reg_sys_ctrl2_parse(mp5515_reg_sys_ctrl2_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->input_ilimt_set, data, 0, 6, 1);
    set_bits_in_arr(&reg->buck_fsw, data, 0, 1, 3);
    set_bits_in_arr(&reg->force_buck_release, data, 0, 0, 1);
}


void mp5515_reg_sys_ctrl3_serialize(const mp5515_reg_sys_ctrl3_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->internal_ilim_thd, 5, 0, 3);
    set_bits_in_arr(data, &reg->dvdt, 3, 0, 2);
    set_bits_in_arr(data, &reg->tpor, 1, 0, 2);
    set_bits_in_arr(data, &reg->en, 0, 0, 0);
}

void mp5515_reg_sys_ctrl3_parse(mp5515_reg_sys_ctrl3_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->internal_ilim_thd, data, 0, 5, 3);
    set_bits_in_arr(&reg->dvdt, data, 0, 3, 2);
    set_bits_in_arr(&reg->tpor, data, 0, 1, 2);
    set_bits_in_arr(&reg->en, data, 0, 0, 0);
}


void mp5515_reg_sys_ctrl4_serialize(const mp5515_reg_sys_ctrl4_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->ilim_en, 3, 0, 1);
    set_bits_in_arr(data, &reg->ilim_tune, 0, 0, 3);
}

void mp5515_reg_sys_ctrl4_parse(mp5515_reg_sys_ctrl4_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->ilim_en, data, 0, 3, 1);
    set_bits_in_arr(&reg->ilim_tune, data, 0, 0, 3);
}


void mp5515_reg_int1_status_serialize(const mp5515_reg_int1_status_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->cap_test_done, 7, 0, 1);
    set_bits_in_arr(data, &reg->adc_done, 6, 0, 1);
    set_bits_in_arr(data, &reg->input_over_current, 4, 0, 1);
    set_bits_in_arr(data, &reg->input_over_voltage, 3, 0, 1);
    set_bits_in_arr(data, &reg->pgs_not_ok, 2, 0, 1);
    set_bits_in_arr(data, &reg->temp_warn, 0, 0, 1);
}

void mp5515_reg_int1_status_parse(mp5515_reg_int1_status_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->cap_test_done, data, 0, 7, 1);
    set_bits_in_arr(&reg->adc_done, data, 0, 6, 1);
    set_bits_in_arr(&reg->input_over_current, data, 0, 4, 1);
    set_bits_in_arr(&reg->input_over_voltage, data, 0, 3, 1);
    set_bits_in_arr(&reg->pgs_not_ok, data, 0, 2, 1);
    set_bits_in_arr(&reg->temp_warn, data, 0, 0, 1);
}


void mp5515_reg_int2_status_serialize(const mp5515_reg_int2_status_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->sas_dis, 6, 0, 1);
    set_bits_in_arr(data, &reg->isofet_off, 5, 0, 1);
    set_bits_in_arr(data, &reg->ic_tj_shutdown, 5, 0, 1);
    set_bits_in_arr(data, &reg->ic_tj_warn, 3, 0, 1);
}

void mp5515_reg_int2_status_parse(mp5515_reg_int2_status_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->sas_dis, data, 0, 6, 1);
    set_bits_in_arr(&reg->isofet_off, data, 0, 5, 1);
    set_bits_in_arr(&reg->ic_tj_shutdown, data, 0, 5, 1);
    set_bits_in_arr(&reg->ic_tj_warn, data, 0, 3, 1);
}


void mp5515_reg_int1_mask_ctrl_serialize(const mp5515_reg_int1_mask_ctrl_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->cap_test_done_mask, 7, 0, 1);
    set_bits_in_arr(data, &reg->adc_done_mask, 6, 0, 1);
    set_bits_in_arr(data, &reg->input_over_currunt_mask, 4, 0, 1);
    set_bits_in_arr(data, &reg->input_over_voltage_mask, 3, 0, 1);
    set_bits_in_arr(data, &reg->pgs_not_ok_mask, 2, 0, 1);
    set_bits_in_arr(data, &reg->temp_warn_mask, 0, 0, 1);
}

void mp5515_reg_int1_mask_ctrl_parse(mp5515_reg_int1_mask_ctrl_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->cap_test_done_mask, data, 0, 7, 1);
    set_bits_in_arr(&reg->adc_done_mask, data, 0, 6, 1);
    set_bits_in_arr(&reg->input_over_currunt_mask, data, 0, 4, 1);
    set_bits_in_arr(&reg->input_over_voltage_mask, data, 0, 3, 1);
    set_bits_in_arr(&reg->pgs_not_ok_mask, data, 0, 2, 1);
    set_bits_in_arr(&reg->temp_warn_mask, data, 0, 0, 1);
}


void mp5515_reg_int2_mask_ctrl_serialize(const mp5515_reg_int2_mask_ctrl_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->sas_dis_mask, 6, 0, 1);
    set_bits_in_arr(data, &reg->isofet_off_mask, 5, 0, 1);
    set_bits_in_arr(data, &reg->ic_tj_shutdown_mask, 4, 0, 1);
    set_bits_in_arr(data, &reg->ic_tj_warn_mask, 3, 0, 1);
}

void mp5515_reg_int2_mask_ctrl_parse(mp5515_reg_int2_mask_ctrl_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->sas_dis_mask, data, 0, 6, 1);
    set_bits_in_arr(&reg->isofet_off_mask, data, 0, 5, 1);
    set_bits_in_arr(&reg->ic_tj_shutdown_mask, data, 0, 4, 1);
    set_bits_in_arr(&reg->ic_tj_warn_mask, data, 0, 3, 1);
}


void mp5515_reg_bkp_cap_thd_serialize(const mp5515_reg_bkp_cap_thd_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->ich, 4, 0, 2);
    set_bits_in_arr(data, &reg->pgs_thd, 0, 0, 4);
}

void mp5515_reg_bkp_cap_thd_parse(mp5515_reg_bkp_cap_thd_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->ich, data, 0, 4, 2);
    set_bits_in_arr(&reg->pgs_thd, data, 0, 0, 4);
}


void mp5515_reg_temp_warn_thd_serialize(const mp5515_reg_temp_warn_thd_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->temp_adc_warn_level, 0, 0, 5);
}

void mp5515_reg_temp_warn_thd_parse(mp5515_reg_temp_warn_thd_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->temp_adc_warn_level, data, 0, 0, 5);
}


void mp5515_reg_adc_src_select_serialize(const mp5515_reg_adc_src_select_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->bkp_voltage, 4, 0, 1);
    set_bits_in_arr(data, &reg->temp_pin, 2, 0, 1);
    set_bits_in_arr(data, &reg->input_current, 1, 0, 1);
    set_bits_in_arr(data, &reg->input_voltage, 0, 0, 1);
}

void mp5515_reg_adc_src_select_parse(mp5515_reg_adc_src_select_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->bkp_voltage, data, 0, 4, 1);
    set_bits_in_arr(&reg->temp_pin, data, 0, 2, 1);
    set_bits_in_arr(&reg->input_current, data, 0, 1, 1);
    set_bits_in_arr(&reg->input_voltage, data, 0, 0, 1);
}


void mp5515_reg_adc_input_voltage_data_serialize(const mp5515_reg_adc_input_voltage_data_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->value, 8, 0, 2);
    set_bits_in_arr(data, &reg->value, 0, 2, 8);
}

void mp5515_reg_adc_input_voltage_data_parse(mp5515_reg_adc_input_voltage_data_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->value, data, 0, 8, 2);
    set_bits_in_arr(&reg->value, data, 2, 0, 8);
}


void mp5515_reg_adc_input_current_data_serialize(const mp5515_reg_adc_input_current_data_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->value, 8, 0, 2);
    set_bits_in_arr(data, &reg->value, 0, 2, 8);
}

void mp5515_reg_adc_input_current_data_parse(mp5515_reg_adc_input_current_data_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->value, data, 0, 8, 2);
    set_bits_in_arr(&reg->value, data, 2, 0, 8);
}


void mp5515_reg_adc_temp_voltage_data_serialize(const mp5515_reg_adc_temp_voltage_data_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->value, 8, 0, 2);
    set_bits_in_arr(data, &reg->value, 0, 2, 8);
}

void mp5515_reg_adc_temp_voltage_data_parse(mp5515_reg_adc_temp_voltage_data_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->value, data, 0, 8, 2);
    set_bits_in_arr(&reg->value, data, 2, 0, 8);
}


void mp5515_reg_adc_bkp_voltage_data1_serialize(const mp5515_reg_adc_bkp_voltage_data1_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->value, 8, 0, 2);
    set_bits_in_arr(data, &reg->value, 0, 2, 8);
}

void mp5515_reg_adc_bkp_voltage_data1_parse(mp5515_reg_adc_bkp_voltage_data1_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->value, data, 0, 8, 2);
    set_bits_in_arr(&reg->value, data, 2, 0, 8);
}


void mp5515_reg_adc_bkp_voltage_data2_serialize(const mp5515_reg_adc_bkp_voltage_data2_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->value, 8, 0, 2);
    set_bits_in_arr(data, &reg->value, 0, 2, 8);
}

void mp5515_reg_adc_bkp_voltage_data2_parse(mp5515_reg_adc_bkp_voltage_data2_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->value, data, 0, 8, 2);
    set_bits_in_arr(&reg->value, data, 2, 0, 8);
}


void mp5515_reg_cap_test_timer_serialize(const mp5515_reg_cap_test_timer_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->value, 0, 0, 16);
}

void mp5515_reg_cap_test_timer_parse(mp5515_reg_cap_test_timer_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->value, data, 0, 0, 16);
}


void mp5515_reg_vin_recover_ctrl_serialize(const mp5515_reg_vin_recover_ctrl_t* reg, uint8_t* data) {
    set_bits_in_arr(data, &reg->ldo_en, 1, 0, 1);
    set_bits_in_arr(data, &reg->vin_recover_mode, 0, 0, 1);
}

void mp5515_reg_vin_recover_ctrl_parse(mp5515_reg_vin_recover_ctrl_t* reg, const uint8_t* data) {
    set_bits_in_arr(&reg->ldo_en, data, 0, 1, 1);
    set_bits_in_arr(&reg->vin_recover_mode, data, 0, 0, 1);
}



