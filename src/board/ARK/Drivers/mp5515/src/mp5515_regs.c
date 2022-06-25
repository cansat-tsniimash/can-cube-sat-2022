#include "mp5515_regs.h"


int mp5515_reg_vendor_id_write(mp5515_t* dev, const mp5515_reg_vendor_id_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_vendor_id_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_VENDOR_ID_ADDRESS, data, sizeof(data));
}

int mp5515_reg_vendor_id_read(mp5515_t* dev, mp5515_reg_vendor_id_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_VENDOR_ID_ADDRESS, data, sizeof(data));
    mp5515_reg_vendor_id_parse(reg, data);
    return ret;
}


int mp5515_reg_sys_ctrl1_write(mp5515_t* dev, const mp5515_reg_sys_ctrl1_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_sys_ctrl1_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_SYS_CTRL1_ADDRESS, data, sizeof(data));
}

int mp5515_reg_sys_ctrl1_read(mp5515_t* dev, mp5515_reg_sys_ctrl1_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_SYS_CTRL1_ADDRESS, data, sizeof(data));
    mp5515_reg_sys_ctrl1_parse(reg, data);
    return ret;
}


int mp5515_reg_sys_ctrl2_write(mp5515_t* dev, const mp5515_reg_sys_ctrl2_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_SYS_CTRL2_ADDRESS, data, sizeof(data));
    if (ret) {
        return ret;
    }
    mp5515_reg_sys_ctrl2_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_SYS_CTRL2_ADDRESS, data, sizeof(data));
}

int mp5515_reg_sys_ctrl2_read(mp5515_t* dev, mp5515_reg_sys_ctrl2_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_SYS_CTRL2_ADDRESS, data, sizeof(data));
    mp5515_reg_sys_ctrl2_parse(reg, data);
    return ret;
}


int mp5515_reg_sys_ctrl3_write(mp5515_t* dev, const mp5515_reg_sys_ctrl3_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_sys_ctrl3_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_SYS_CTRL3_ADDRESS, data, sizeof(data));
}

int mp5515_reg_sys_ctrl3_read(mp5515_t* dev, mp5515_reg_sys_ctrl3_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_SYS_CTRL3_ADDRESS, data, sizeof(data));
    mp5515_reg_sys_ctrl3_parse(reg, data);
    return ret;
}


int mp5515_reg_sys_ctrl4_write(mp5515_t* dev, const mp5515_reg_sys_ctrl4_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_sys_ctrl4_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_SYS_CTRL4_ADDRESS, data, sizeof(data));
}

int mp5515_reg_sys_ctrl4_read(mp5515_t* dev, mp5515_reg_sys_ctrl4_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_SYS_CTRL4_ADDRESS, data, sizeof(data));
    mp5515_reg_sys_ctrl4_parse(reg, data);
    return ret;
}


int mp5515_reg_int1_status_write(mp5515_t* dev, const mp5515_reg_int1_status_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_int1_status_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_INT1_STATUS_ADDRESS, data, sizeof(data));
}

int mp5515_reg_int1_status_read(mp5515_t* dev, mp5515_reg_int1_status_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_INT1_STATUS_ADDRESS, data, sizeof(data));
    mp5515_reg_int1_status_parse(reg, data);
    return ret;
}


int mp5515_reg_int2_status_write(mp5515_t* dev, const mp5515_reg_int2_status_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_int2_status_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_INT2_STATUS_ADDRESS, data, sizeof(data));
}

int mp5515_reg_int2_status_read(mp5515_t* dev, mp5515_reg_int2_status_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_INT2_STATUS_ADDRESS, data, sizeof(data));
    mp5515_reg_int2_status_parse(reg, data);
    return ret;
}


int mp5515_reg_int1_mask_ctrl_write(mp5515_t* dev, const mp5515_reg_int1_mask_ctrl_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_int1_mask_ctrl_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_INT1_MASK_CTRL_ADDRESS, data, sizeof(data));
}

int mp5515_reg_int1_mask_ctrl_read(mp5515_t* dev, mp5515_reg_int1_mask_ctrl_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_INT1_MASK_CTRL_ADDRESS, data, sizeof(data));
    mp5515_reg_int1_mask_ctrl_parse(reg, data);
    return ret;
}


int mp5515_reg_int2_mask_ctrl_write(mp5515_t* dev, const mp5515_reg_int2_mask_ctrl_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_int2_mask_ctrl_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_INT2_MASK_CTRL_ADDRESS, data, sizeof(data));
}

int mp5515_reg_int2_mask_ctrl_read(mp5515_t* dev, mp5515_reg_int2_mask_ctrl_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_INT2_MASK_CTRL_ADDRESS, data, sizeof(data));
    mp5515_reg_int2_mask_ctrl_parse(reg, data);
    return ret;
}


int mp5515_reg_bkp_cap_thd_write(mp5515_t* dev, const mp5515_reg_bkp_cap_thd_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_bkp_cap_thd_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_BKP_CAP_THD_ADDRESS, data, sizeof(data));
}

int mp5515_reg_bkp_cap_thd_read(mp5515_t* dev, mp5515_reg_bkp_cap_thd_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_BKP_CAP_THD_ADDRESS, data, sizeof(data));
    mp5515_reg_bkp_cap_thd_parse(reg, data);
    return ret;
}


int mp5515_reg_temp_warn_thd_write(mp5515_t* dev, const mp5515_reg_temp_warn_thd_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_temp_warn_thd_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_TEMP_WARN_THD_ADDRESS, data, sizeof(data));
}

int mp5515_reg_temp_warn_thd_read(mp5515_t* dev, mp5515_reg_temp_warn_thd_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_TEMP_WARN_THD_ADDRESS, data, sizeof(data));
    mp5515_reg_temp_warn_thd_parse(reg, data);
    return ret;
}


int mp5515_reg_adc_src_select_write(mp5515_t* dev, const mp5515_reg_adc_src_select_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_adc_src_select_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_ADC_SRC_SELECT_ADDRESS, data, sizeof(data));
}

int mp5515_reg_adc_src_select_read(mp5515_t* dev, mp5515_reg_adc_src_select_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_ADC_SRC_SELECT_ADDRESS, data, sizeof(data));
    mp5515_reg_adc_src_select_parse(reg, data);
    return ret;
}


int mp5515_reg_adc_input_voltage_data_write(mp5515_t* dev, const mp5515_reg_adc_input_voltage_data_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    mp5515_reg_adc_input_voltage_data_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_ADC_INPUT_VOLTAGE_DATA_ADDRESS, data, sizeof(data));
}

int mp5515_reg_adc_input_voltage_data_read(mp5515_t* dev, mp5515_reg_adc_input_voltage_data_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_ADC_INPUT_VOLTAGE_DATA_ADDRESS, data, sizeof(data));
    mp5515_reg_adc_input_voltage_data_parse(reg, data);
    return ret;
}


int mp5515_reg_adc_input_current_data_write(mp5515_t* dev, const mp5515_reg_adc_input_current_data_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    mp5515_reg_adc_input_current_data_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_ADC_INPUT_CURRENT_DATA_ADDRESS, data, sizeof(data));
}

int mp5515_reg_adc_input_current_data_read(mp5515_t* dev, mp5515_reg_adc_input_current_data_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_ADC_INPUT_CURRENT_DATA_ADDRESS, data, sizeof(data));
    mp5515_reg_adc_input_current_data_parse(reg, data);
    return ret;
}


int mp5515_reg_adc_temp_voltage_data_write(mp5515_t* dev, const mp5515_reg_adc_temp_voltage_data_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    mp5515_reg_adc_temp_voltage_data_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_ADC_TEMP_VOLTAGE_DATA_ADDRESS, data, sizeof(data));
}

int mp5515_reg_adc_temp_voltage_data_read(mp5515_t* dev, mp5515_reg_adc_temp_voltage_data_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_ADC_TEMP_VOLTAGE_DATA_ADDRESS, data, sizeof(data));
    mp5515_reg_adc_temp_voltage_data_parse(reg, data);
    return ret;
}


int mp5515_reg_adc_bkp_voltage_data1_write(mp5515_t* dev, const mp5515_reg_adc_bkp_voltage_data1_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    mp5515_reg_adc_bkp_voltage_data1_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_ADC_BKP_VOLTAGE_DATA1_ADDRESS, data, sizeof(data));
}

int mp5515_reg_adc_bkp_voltage_data1_read(mp5515_t* dev, mp5515_reg_adc_bkp_voltage_data1_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_ADC_BKP_VOLTAGE_DATA1_ADDRESS, data, sizeof(data));
    mp5515_reg_adc_bkp_voltage_data1_parse(reg, data);
    return ret;
}


int mp5515_reg_adc_bkp_voltage_data2_write(mp5515_t* dev, const mp5515_reg_adc_bkp_voltage_data2_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    mp5515_reg_adc_bkp_voltage_data2_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_ADC_BKP_VOLTAGE_DATA2_ADDRESS, data, sizeof(data));
}

int mp5515_reg_adc_bkp_voltage_data2_read(mp5515_t* dev, mp5515_reg_adc_bkp_voltage_data2_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_ADC_BKP_VOLTAGE_DATA2_ADDRESS, data, sizeof(data));
    mp5515_reg_adc_bkp_voltage_data2_parse(reg, data);
    return ret;
}


int mp5515_reg_cap_test_timer_write(mp5515_t* dev, const mp5515_reg_cap_test_timer_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    mp5515_reg_cap_test_timer_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_CAP_TEST_TIMER_ADDRESS, data, sizeof(data));
}

int mp5515_reg_cap_test_timer_read(mp5515_t* dev, mp5515_reg_cap_test_timer_t* reg) {
    int ret = 0;
    uint8_t data[2] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_CAP_TEST_TIMER_ADDRESS, data, sizeof(data));
    mp5515_reg_cap_test_timer_parse(reg, data);
    return ret;
}


int mp5515_reg_vin_recover_ctrl_write(mp5515_t* dev, const mp5515_reg_vin_recover_ctrl_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    mp5515_reg_vin_recover_ctrl_serialize(reg, data);
    return mp5515_reg_write(dev, MP5515_REG_VIN_RECOVER_CTRL_ADDRESS, data, sizeof(data));
}

int mp5515_reg_vin_recover_ctrl_read(mp5515_t* dev, mp5515_reg_vin_recover_ctrl_t* reg) {
    int ret = 0;
    uint8_t data[1] = {0};
    ret = mp5515_reg_read(dev, MP5515_REG_VIN_RECOVER_CTRL_ADDRESS, data, sizeof(data));
    mp5515_reg_vin_recover_ctrl_parse(reg, data);
    return ret;
}



