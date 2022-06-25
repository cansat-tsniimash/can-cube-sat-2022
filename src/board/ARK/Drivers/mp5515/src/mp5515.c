/*
 * mp5515.c
 *
 *  Created on: May 28, 2022
 *      Author: seres
 */
#include "main.h"
#include "mp5515.h"
#include "mp5515_regs.h"
#include <math.h>


int mp5515_reg_write(mp5515_t* dev, uint8_t address, const uint8_t* data, size_t size) {
    HAL_I2C_Mem_Write(dev->hi2c, dev->address, address, 1, (uint8_t*) data, size, dev->timeout);
}
int mp5515_reg_read(mp5515_t* dev, uint8_t address, uint8_t* data, size_t size) {
    HAL_I2C_Mem_Read(dev->hi2c, dev->address, address, 1, data, size, dev->timeout);
}

void mp5515_init(mp5515_t* dev) {

}



GPIO_PinState gpio_read(gpio_t gpio) {
    return HAL_GPIO_ReadPin(gpio.port, gpio.pin);
}

int mp5515_get_events(mp5515_t* dev, mp5515_event_t* event) {
    *event = 0;

    mp5515_reg_int1_status_t reg1 = {0};
    mp5515_reg_int2_status_t reg2 = {0};
    int res = 0;
    if (mp5515_reg_int1_status_read(dev, &reg1)) {
        res = 1;
        *event |= MP5515_EVENT_IO_FAIL;
    }
    if (mp5515_reg_int2_status_read(dev, &reg2)) {
        res = 1;
        *event |= MP5515_EVENT_IO_FAIL;
    }
    if (res == 0) {
        if (reg1.adc_done) {
            *event |= MP5515_EVENT_ADC_DONE;
        }
        if (reg1.cap_test_done) {
            *event |= MP5515_EVENT_CAP_TEST_DONE;
        }
        if (reg1.input_over_current) {
            *event |= MP5515_EVENT_INPUT_OVER_CURRENT;
            res = 1;
        }
        if (reg1.input_over_voltage) {
            *event |= MP5515_EVENT_INPUT_OVER_VOLTAGE;
            res = 1;
        }
        if (reg1.pgs_not_ok) {
            *event |= MP5515_EVENT_PGS_NOT_OK;
            res = 1;
        }
        if (reg1.temp_warn) {
            *event |= MP5515_EVENT_TEMP_WARN;
            res = 1;
        }
        if (reg2.ic_tj_shutdown) {
            *event |= MP5515_EVENT_FORED_BUCK;
            res = 1;
        }
        if (reg2.ic_tj_warn) {
            *event |= MP5515_EVENT_DIE_TEMP_WARN;
            res = 1;
        }
        if (reg2.sas_dis) {
            *event |= MP5515_EVENT_SAS_DISABLE_SUPPLY;
            res = 1;
        }
        if (reg2.isofet_off) {
            *event |= MP5515_EVENT_ISOFET_OFF;
            res = 1;
        }
    }
    int pin_pfi = gpio_read(dev->pin_pfi);
    int pin_int = gpio_read(dev->pin_int);
    int pin_pgs = gpio_read(dev->pin_pgs);
    int pin_pgb = gpio_read(dev->pin_pgb);
    if (!pin_pfi && !pin_int) {
        *event |= MP5515_EVENT_INPUT_POWER_FAIL;
        res = 1;
    }
    if (!pin_pgs) {
        *event |= MP5515_EVENT_PGS_NOT_OK;
        res = 1;
    }
    if (!pin_pgb) {
        *event |= MP5515_EVENT_PGB_NOT_OK;
        res = 1;
    }

    return res;
}


int mp5515_poll_events(mp5515_t* dev, mp5515_event_t* event) {
    uint32_t start = HAL_GetTick();
    while(!gpio_read(dev->pin_int) && gpio_read(dev->pin_pfi)) {
        if (HAL_GetTick() - start < dev->timeout) {
            *event = MP5515_EVENT_TIMEOUT;
            return 1;
        }

    }
    return mp5515_get_events(dev, event);
}

int mp5515_reset_int(mp5515_t* dev) {
    mp5515_reg_int1_status_t reg1 = {0};
    mp5515_reg_int2_status_t reg2 = {0};
    if (mp5515_reg_int1_status_write(dev, &reg1)) {
        return 1;
    }
    if (mp5515_reg_int2_status_write(dev, &reg2)) {
        return 1;
    }
    return 0;
}

int mp5515_reset_int_mask(mp5515_t* dev) {
    mp5515_reg_int1_mask_ctrl_t reg1 = {0};
    mp5515_reg_int2_mask_ctrl_t reg2 = {0};

    if (mp5515_reg_int1_mask_ctrl_write(dev, &reg1)) {
        return 1;
    }
    if (mp5515_reg_int2_mask_ctrl_write(dev, &reg2)) {
        return 1;
    }
    return 0;
}


int mp5515_adc_start(mp5515_t* dev, mp5515_reg_adc_src_select_t select) {
    mp5515_reg_sys_ctrl1_t ctrl = {0};
    ctrl.start_adc = 1;
    int res = 0;

    res = mp5515_reset_int(dev);
    if (res) {
        return res;
    }
    res = mp5515_reg_adc_src_select_write(dev, &select);
    if (res) {
        return res;
    }
    res = mp5515_reg_sys_ctrl1_write(dev, &ctrl);
    if (res) {
        return res;
    }
    return 0;
}

int mp5515_adc_poll(mp5515_t* dev) {
    mp5515_event_t event = 0;
    if (mp5515_poll_events(dev, &event)) {
        return 1;
    }
    mp5515_reset_int(dev);
    if (event & MP5515_EVENT_ADC_DONE) {
        return 0;
    } else {
        return 1;
    }
}

int mp5515_adc_get_in_voltage(mp5515_t* dev, float* vin) {
    mp5515_reg_adc_input_voltage_data_t vin_reg = {0};
    int res = mp5515_reg_adc_input_voltage_data_read(dev, &vin_reg);

    *vin = vin_reg.value / 1023.0 * 18.4;
    return res;
}

int mp5515_adc_get_in_current(mp5515_t* dev, float* current) {
    mp5515_reg_adc_input_current_data_t cur_reg = {0};
    int res = mp5515_reg_adc_input_current_data_read(dev, &cur_reg);

    float vlsb = 1.28 / 1023.0;
    *current = vlsb * cur_reg.value * 64260.0 / dev->r_lim + 241.0 / dev->r_lim + 0.08;
    return res;
}

int mp5515_adc_get_bkp_voltage1(mp5515_t* dev, float* voltage) {
    mp5515_reg_adc_bkp_voltage_data1_t vin_reg = {0};
    mp5515_reg_adc_bkp_voltage_data1_read(dev, &vin_reg);

    *voltage = vin_reg.value / 1023.0 * 40.1;
}

int mp5515_adc_get_bkp_voltage2(mp5515_t* dev, float* voltage) {
    mp5515_reg_adc_bkp_voltage_data2_t vin_reg = {0};
    mp5515_reg_adc_bkp_voltage_data2_read(dev, &vin_reg);

    *voltage = vin_reg.value / 1023.0 * 40.1;
}

int mp5515_get_vin(mp5515_t* dev, float* vin) {

    mp5515_reg_adc_src_select_t select = {0};
    select.input_voltage = 1;
    mp5515_reg_sys_ctrl1_t ctrl = {0};
    ctrl.start_adc = 1;

    mp5515_reset_int(dev);
    mp5515_reg_adc_src_select_write(dev, &select);
    mp5515_reg_sys_ctrl1_write(dev, &ctrl);
    mp5515_event_t event = 0;

    if (mp5515_poll_events(dev, &event)) {
        return 1;
    }
    if (event & MP5515_EVENT_ADC_DONE) {
        mp5515_reg_adc_input_voltage_data_t vin_reg = {0};
        mp5515_reg_adc_input_voltage_data_read(dev, &vin_reg);

        *vin = vin_reg.value / 1024.0 * 18.4;
    } else {
        return 1;
    }
    mp5515_reset_int(dev);
    return 0;

}

int mp5515_cap_test(mp5515_t* dev, float* c_storage) {
    int res = 0;
    mp5515_reset_int(dev);

    mp5515_reg_int1_mask_ctrl_t reg = {0};
    reg.pgs_not_ok_mask = 1;
    reg.adc_done_mask = 1;
    mp5515_reg_int1_mask_ctrl_write(dev, &reg);

    dev->regs.bkp_cap_thd.pgs_thd = 0; // 80%
    mp5515_reg_bkp_cap_thd_write(dev, &dev->regs.bkp_cap_thd);

    dev->regs.sys_ctrl1.start_cap_test = 1;
    mp5515_reg_sys_ctrl1_write(dev, &dev->regs.sys_ctrl1);

    mp5515_event_t event = 0;
    if (mp5515_poll_events(dev, &event)) {
        res = 1;
        goto end;
    }

    if (!(event & MP5515_EVENT_CAP_TEST_DONE)) {
        res = 1;
        goto end;
    }
    mp5515_reg_adc_bkp_voltage_data1_t v_final_reg = {0};
    mp5515_reg_adc_bkp_voltage_data1_read(dev, &v_final_reg);
    mp5515_reg_adc_bkp_voltage_data2_t v_initial_reg = {0};
    mp5515_reg_adc_bkp_voltage_data2_read(dev, &v_initial_reg);
    mp5515_reg_cap_test_timer_t time_reg = {0};
    mp5515_reg_adc_bkp_voltage_data2_read(dev, &time_reg);

    float v_final = v_final_reg.value;
    float v_initial = v_initial_reg.value;
    float time = time_reg.value;
    float r_dc = dev->r_dc / 1000.0;
    float i_ldo = 0;
    *c_storage = time / (r_dc * log((v_initial + r_dc * i_ldo) / (v_final + r_dc * i_ldo)));



end:
    mp5515_reset_int(dev);
    mp5515_reset_int_mask(dev);
    return res;

}
