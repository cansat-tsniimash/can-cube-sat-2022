#ifndef __MP5515_PACKING_H__
#define __MP5515_PACKING_H__


#include <stdint.h>
#include <stddef.h>


#define MP5515_REG_VENDOR_ID_ADDRESS 0x00
#define MP5515_REG_VENDOR_ID_SIZE 1

#define MP5515_REG_SYS_CTRL1_ADDRESS 0x01
#define MP5515_REG_SYS_CTRL1_SIZE 1

#define MP5515_REG_SYS_CTRL2_ADDRESS 0x20
#define MP5515_REG_SYS_CTRL2_SIZE 1

#define MP5515_REG_SYS_CTRL3_ADDRESS 0x21
#define MP5515_REG_SYS_CTRL3_SIZE 1

#define MP5515_REG_SYS_CTRL4_ADDRESS 0x22
#define MP5515_REG_SYS_CTRL4_SIZE 1

#define MP5515_REG_INT1_STATUS_ADDRESS 0x02
#define MP5515_REG_INT1_STATUS_SIZE 1

#define MP5515_REG_INT2_STATUS_ADDRESS 0x03
#define MP5515_REG_INT2_STATUS_SIZE 1

#define MP5515_REG_INT1_MASK_CTRL_ADDRESS 0x04
#define MP5515_REG_INT1_MASK_CTRL_SIZE 1

#define MP5515_REG_INT2_MASK_CTRL_ADDRESS 0x05
#define MP5515_REG_INT2_MASK_CTRL_SIZE 1

#define MP5515_REG_BKP_CAP_THD_ADDRESS 0x06
#define MP5515_REG_BKP_CAP_THD_SIZE 1

#define MP5515_REG_TEMP_WARN_THD_ADDRESS 0x07
#define MP5515_REG_TEMP_WARN_THD_SIZE 1

#define MP5515_REG_ADC_SRC_SELECT_ADDRESS 0x09
#define MP5515_REG_ADC_SRC_SELECT_SIZE 1

#define MP5515_REG_ADC_INPUT_VOLTAGE_DATA_ADDRESS 0x0A
#define MP5515_REG_ADC_INPUT_VOLTAGE_DATA_SIZE 2

#define MP5515_REG_ADC_INPUT_CURRENT_DATA_ADDRESS 0x0C
#define MP5515_REG_ADC_INPUT_CURRENT_DATA_SIZE 2

#define MP5515_REG_ADC_TEMP_VOLTAGE_DATA_ADDRESS 0x0E
#define MP5515_REG_ADC_TEMP_VOLTAGE_DATA_SIZE 2

#define MP5515_REG_ADC_BKP_VOLTAGE_DATA1_ADDRESS 0x12
#define MP5515_REG_ADC_BKP_VOLTAGE_DATA1_SIZE 2

#define MP5515_REG_ADC_BKP_VOLTAGE_DATA2_ADDRESS 0x14
#define MP5515_REG_ADC_BKP_VOLTAGE_DATA2_SIZE 2

#define MP5515_REG_CAP_TEST_TIMER_ADDRESS 0x16
#define MP5515_REG_CAP_TEST_TIMER_SIZE 2

#define MP5515_REG_VIN_RECOVER_CTRL_ADDRESS 0x1B
#define MP5515_REG_VIN_RECOVER_CTRL_SIZE 1


typedef struct {
    // Size: 2 bits
    uint8_t fab;
    // Size: 2 bits
    uint8_t major_rev;
    // Size: 2 bits
    uint8_t minor_rev;
    // Size: 2 bits
    uint8_t vendor_id;
} mp5515_reg_vendor_id_t;

typedef struct {
    // Size: 1 bits
    uint8_t start_cap_test;
    // Size: 1 bits
    uint8_t start_adc;
    // Size: 1 bits
    uint8_t ench;
} mp5515_reg_sys_ctrl1_t;

typedef struct {
    // Size: 1 bits
    uint8_t input_ilimt_set;
    // Size: 3 bits
    uint8_t buck_fsw;
    // Size: 1 bits
    uint8_t force_buck_release;
} mp5515_reg_sys_ctrl2_t;

typedef struct {
    // Size: 3 bits
    uint8_t internal_ilim_thd;
    // Size: 2 bits
    uint8_t dvdt;
    // Size: 2 bits
    uint8_t tpor;
    // Size: 0 bits
    uint8_t en;
} mp5515_reg_sys_ctrl3_t;

typedef struct {
    // Size: 1 bits
    uint8_t ilim_en;
    // Size: 3 bits
    uint8_t ilim_tune;
} mp5515_reg_sys_ctrl4_t;

typedef struct {
    // Size: 1 bits
    uint8_t cap_test_done;
    // Size: 1 bits
    uint8_t adc_done;
    // Size: 1 bits
    uint8_t input_over_current;
    // Size: 1 bits
    uint8_t input_over_voltage;
    // Size: 1 bits
    uint8_t pgs_not_ok;
    // Size: 1 bits
    uint8_t temp_warn;
} mp5515_reg_int1_status_t;

typedef struct {
    // Size: 1 bits
    uint8_t sas_dis;
    // Size: 1 bits
    uint8_t isofet_off;
    // Size: 1 bits
    uint8_t ic_tj_shutdown;
    // Size: 1 bits
    uint8_t ic_tj_warn;
} mp5515_reg_int2_status_t;

typedef struct {
    // Size: 1 bits
    uint8_t cap_test_done_mask;
    // Size: 1 bits
    uint8_t adc_done_mask;
    // Size: 1 bits
    uint8_t input_over_currunt_mask;
    // Size: 1 bits
    uint8_t input_over_voltage_mask;
    // Size: 1 bits
    uint8_t pgs_not_ok_mask;
    // Size: 1 bits
    uint8_t temp_warn_mask;
} mp5515_reg_int1_mask_ctrl_t;

typedef struct {
    // Size: 1 bits
    uint8_t sas_dis_mask;
    // Size: 1 bits
    uint8_t isofet_off_mask;
    // Size: 1 bits
    uint8_t ic_tj_shutdown_mask;
    // Size: 1 bits
    uint8_t ic_tj_warn_mask;
} mp5515_reg_int2_mask_ctrl_t;

typedef struct {
    // Size: 2 bits
    uint8_t ich;
    // Size: 4 bits
    uint8_t pgs_thd;
} mp5515_reg_bkp_cap_thd_t;

typedef struct {
    // Size: 5 bits
    uint8_t temp_adc_warn_level;
} mp5515_reg_temp_warn_thd_t;

typedef struct {
    // Size: 1 bits
    uint8_t bkp_voltage;
    // Size: 1 bits
    uint8_t temp_pin;
    // Size: 1 bits
    uint8_t input_current;
    // Size: 1 bits
    uint8_t input_voltage;
} mp5515_reg_adc_src_select_t;

typedef struct {
    // Size: 10 bits
    uint16_t value;
} mp5515_reg_adc_input_voltage_data_t;

typedef struct {
    // Size: 10 bits
    uint16_t value;
} mp5515_reg_adc_input_current_data_t;

typedef struct {
    // Size: 10 bits
    uint16_t value;
} mp5515_reg_adc_temp_voltage_data_t;

typedef struct {
    // Size: 10 bits
    uint16_t value;
} mp5515_reg_adc_bkp_voltage_data1_t;

typedef struct {
    // Size: 10 bits
    uint16_t value;
} mp5515_reg_adc_bkp_voltage_data2_t;

typedef struct {
    // Size: 16 bits
    uint16_t value;
} mp5515_reg_cap_test_timer_t;

typedef struct {
    // Size: 1 bits
    uint8_t ldo_en;
    // Size: 1 bits
    uint8_t vin_recover_mode;
} mp5515_reg_vin_recover_ctrl_t;



// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_vendor_id_serialize(const mp5515_reg_vendor_id_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_vendor_id_parse(mp5515_reg_vendor_id_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_sys_ctrl1_serialize(const mp5515_reg_sys_ctrl1_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_sys_ctrl1_parse(mp5515_reg_sys_ctrl1_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_sys_ctrl2_serialize(const mp5515_reg_sys_ctrl2_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_sys_ctrl2_parse(mp5515_reg_sys_ctrl2_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_sys_ctrl3_serialize(const mp5515_reg_sys_ctrl3_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_sys_ctrl3_parse(mp5515_reg_sys_ctrl3_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_sys_ctrl4_serialize(const mp5515_reg_sys_ctrl4_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_sys_ctrl4_parse(mp5515_reg_sys_ctrl4_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_int1_status_serialize(const mp5515_reg_int1_status_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_int1_status_parse(mp5515_reg_int1_status_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_int2_status_serialize(const mp5515_reg_int2_status_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_int2_status_parse(mp5515_reg_int2_status_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_int1_mask_ctrl_serialize(const mp5515_reg_int1_mask_ctrl_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_int1_mask_ctrl_parse(mp5515_reg_int1_mask_ctrl_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_int2_mask_ctrl_serialize(const mp5515_reg_int2_mask_ctrl_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_int2_mask_ctrl_parse(mp5515_reg_int2_mask_ctrl_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_bkp_cap_thd_serialize(const mp5515_reg_bkp_cap_thd_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_bkp_cap_thd_parse(mp5515_reg_bkp_cap_thd_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_temp_warn_thd_serialize(const mp5515_reg_temp_warn_thd_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_temp_warn_thd_parse(mp5515_reg_temp_warn_thd_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_adc_src_select_serialize(const mp5515_reg_adc_src_select_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_adc_src_select_parse(mp5515_reg_adc_src_select_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 2 bytes to send to mp5515 device
void mp5515_reg_adc_input_voltage_data_serialize(const mp5515_reg_adc_input_voltage_data_t* reg, uint8_t* data);

// Parse raw "data" of size 2 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_adc_input_voltage_data_parse(mp5515_reg_adc_input_voltage_data_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 2 bytes to send to mp5515 device
void mp5515_reg_adc_input_current_data_serialize(const mp5515_reg_adc_input_current_data_t* reg, uint8_t* data);

// Parse raw "data" of size 2 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_adc_input_current_data_parse(mp5515_reg_adc_input_current_data_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 2 bytes to send to mp5515 device
void mp5515_reg_adc_temp_voltage_data_serialize(const mp5515_reg_adc_temp_voltage_data_t* reg, uint8_t* data);

// Parse raw "data" of size 2 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_adc_temp_voltage_data_parse(mp5515_reg_adc_temp_voltage_data_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 2 bytes to send to mp5515 device
void mp5515_reg_adc_bkp_voltage_data1_serialize(const mp5515_reg_adc_bkp_voltage_data1_t* reg, uint8_t* data);

// Parse raw "data" of size 2 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_adc_bkp_voltage_data1_parse(mp5515_reg_adc_bkp_voltage_data1_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 2 bytes to send to mp5515 device
void mp5515_reg_adc_bkp_voltage_data2_serialize(const mp5515_reg_adc_bkp_voltage_data2_t* reg, uint8_t* data);

// Parse raw "data" of size 2 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_adc_bkp_voltage_data2_parse(mp5515_reg_adc_bkp_voltage_data2_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 2 bytes to send to mp5515 device
void mp5515_reg_cap_test_timer_serialize(const mp5515_reg_cap_test_timer_t* reg, uint8_t* data);

// Parse raw "data" of size 2 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_cap_test_timer_parse(mp5515_reg_cap_test_timer_t* reg, const uint8_t* data);


// Serialize "reg" struct to raw "data" of size 1 bytes to send to mp5515 device
void mp5515_reg_vin_recover_ctrl_serialize(const mp5515_reg_vin_recover_ctrl_t* reg, uint8_t* data);

// Parse raw "data" of size 1 bytes  to "reg" struct received from mp5515 device
void mp5515_reg_vin_recover_ctrl_parse(mp5515_reg_vin_recover_ctrl_t* reg, const uint8_t* data);


#endif // __MP5515_PACKING_H__
