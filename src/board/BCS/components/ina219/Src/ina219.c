/*
*
 * @file ina219.c
 *
 * ESP-IDF driver for INA219/INA220 Zerø-Drift, Bidirectional
 * Current/Power Monitor
 *
 * Copyright (C) 2019 Ruslan V. Uss <unclerus@gmail.com>
 *
 * BSD Licensed as described in the file LICENSE
*/


#define LOG_LOCAL_LEVEL ESP_LOG_ERROR
#include <esp_log.h>
#include <math.h>
#include <esp_err.h>
#include "ina219.h"


static const char *TAG = "INA219";
/*
 * ina219.c
 *
 *  Created on: 18 марта 2017 г.
 *      Author: developer
 */
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>

#include "ina219.h"


#ifdef INA219_FLT_DOUBLE
#define INA219_FLOAT(x) x
#else
#define INA219_FLOAT(x) x##f
#endif


#define INA219_SET_BITS(reg_data, bitname, data) \
				((reg_data & ~(bitname##_MSK)) | \
				((data << bitname##_POS) & bitname##_MSK))

#define INA219_GET_BITS(reg_data, bitname)  ((reg_data & (bitname##_MSK)) >> \
							(bitname##_POS))

#define INA219_CFGREG_ADDR         (0x00)

#define INA219_CFGREG__RESET_POS   (15)
#define INA219_CFGREG__RESET_MSK    (0x0001 << 15)

#define INA219_CFGREG__BRNG_POS    (13)
#define INA219_CFGREG__BRNG_MSK    (0x0001 << 13)

#define INA219_CFGREG__PG_POS      (11)
#define INA219_CFGREG__PG_MSK      (0x0003 << 11)

#define INA219_CFGREG__BADC_POS    (7)
#define INA219_CFGREG__BADC_MSK    (0x000f << 7)

#define INA219_CFGREG__SADC_POS    (3)
#define INA219_CFGREG__SADC_MSK    (0x000f << 3)

#define INA219_CFGREG__MODE_POS    (0)
#define INA219_CFGREG__MODE_MSK    (0x0007 << 0)


#define INA219_SHUNTVREG_ADDR      (0x01)


#define INA219_BUSVREG_ADDR        (0x02)

#define INA219_BUSVREG__BUSV_POS   (3)
#define INA219_BUSVREG__BUSV_MSK   (0x1fff << 3)

#define INA219_BUSVREG__CNVR_POS   (1)
#define INA219_BUSVREG__CNVR_MSK   (0x0001 << 1)

#define INA219_BUSVREG__OVF_POS    (0)
#define INA219_BUSVREG__OVF_MSK    (0x0001 << 0)


#define INA219_POWERREG_ADDR       (0x03)


#define INA219_CURRENTREG_ADDR     (0x04)


#define INA219_CALREG_ADDR         (0x05)
// формально тут следует написать, что CALREG имеет поле CAL, которые начинается с 1ого бита
// а не с нулевого, но это настолько частный случай, что не будем этим заниматься -
// опишем в коде как << 1


//! Запись одного регистра ины в I2C шину
static int _write_reg(ina219_t * device, uint8_t reg_addr, uint16_t reg_value) {
	ESP_LOGV(TAG, "write_reg_16 %d %d %d", device->i2c_address, reg_addr, reg_value);
    uint16_t v = (reg_value >> 8) | (reg_value << 8);

    if (i2c_master_prestart(device->i2c_port, device->tick_timeout) != pdTRUE) {
    	return ESP_ERR_TIMEOUT;
    }
	i2c_cmd_handle_t cmd =  i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (device->i2c_address << 1) | I2C_MASTER_WRITE, 1);
	i2c_master_write_byte(cmd, reg_addr, 1);
	i2c_master_write(cmd, (uint8_t*) &v, sizeof(v), 1);
	i2c_master_stop(cmd);
	esp_err_t err = i2c_master_cmd_begin(device->i2c_port, cmd, 0);
	i2c_cmd_link_delete(cmd);
	i2c_master_postend(device->i2c_port);


    return err;
}


//! Чтение регистра ины из I2C шины
/*! К сожалению ина не умеет читать несколько регистров за раз */
static esp_err_t _read_reg(ina219_t * device, uint8_t reg_addr, uint16_t * reg_value){

	ESP_LOGV(TAG, "read_reg_16 %d %d", device->i2c_address, reg_addr);

	if (i2c_master_prestart(device->i2c_port, device->tick_timeout) != pdTRUE) {
		return ESP_ERR_TIMEOUT;
	}
	esp_err_t err;
	{
		i2c_cmd_handle_t cmd =  i2c_cmd_link_create();

		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (device->i2c_address << 1) | I2C_MASTER_WRITE, 1);
		i2c_master_write_byte(cmd, reg_addr, 1);
		i2c_master_stop(cmd);
		err = i2c_master_cmd_begin(device->i2c_port, cmd, 0);
		i2c_cmd_link_delete(cmd);
	}
	uint8_t raw[2] = {0};
	if (!err) {
		i2c_cmd_handle_t cmd =  i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (device->i2c_address << 1) | I2C_MASTER_READ, 1);
		i2c_master_read_byte(cmd, &raw[0], I2C_MASTER_ACK);
		i2c_master_read_byte(cmd, &raw[1], I2C_MASTER_NACK);
		i2c_master_stop(cmd);
		err = i2c_master_cmd_begin(device->i2c_port, cmd, 0);
		i2c_cmd_link_delete(cmd);
	}
	i2c_master_postend(device->i2c_port);

	*reg_value = raw[1] | (raw[0] << 8ull);
	ESP_LOGV(TAG, "read_reg_16 val = %d", *reg_value);

	return err;
}


//! Построение значения калибровочного регистра из его базовых значений
static uint16_t _make_cal_reg(ina219_float_t current_lsb, ina219_float_t shunt_r)
{
	// Если что-то задано по нулям, то калибровки не будет
	if (current_lsb == INA219_FLOAT(0.0) || shunt_r == INA219_FLOAT(0.0))
		return 0x00;

	// волшебные цифры из даташита
	const ina219_float_t calib = INA219_FLOAT(0.04096)/(current_lsb * shunt_r);
	return ((uint16_t)calib);
	// << 1 так как нулевой бит регистра резервирован, а фактическое значение идет с бита 1
}


//! Построение значения cfg регистра из структуры конфигурации
static uint16_t _make_cfg_reg(const ina219_cfg_t * cfg)
{
	uint16_t retval = 0;

	retval = INA219_SET_BITS(retval, INA219_CFGREG__BRNG, cfg->bus_range);
	retval = INA219_SET_BITS(retval, INA219_CFGREG__PG, cfg->shunt_range);
	retval = INA219_SET_BITS(retval, INA219_CFGREG__BADC, cfg->bus_res);
	retval = INA219_SET_BITS(retval, INA219_CFGREG__SADC, cfg->shunt_res);
	retval = INA219_SET_BITS(retval, INA219_CFGREG__MODE, cfg->mode);

	return retval;
}
//! Построение значения cfg регистра из структуры конфигурации
static void _parse_cfg_reg(uint16_t reg, ina219_cfg_t *cfg)
{

	cfg->bus_range = INA219_GET_BITS(reg, INA219_CFGREG__BRNG);
	cfg->shunt_range = INA219_GET_BITS(reg, INA219_CFGREG__PG);
	cfg->bus_res = INA219_GET_BITS(reg, INA219_CFGREG__BADC);
	cfg->shunt_res = INA219_GET_BITS(reg, INA219_CFGREG__SADC);
	cfg->mode = INA219_GET_BITS(reg, INA219_CFGREG__MODE);
}


//! нарезка значения "первичных" регистров на "исходные" данные
inline static void _slice_prim_data(const uint16_t * primregs, ina219_primary_data_t * primdata)
{
	// тут нужно хитро. нужно скопировать байты из uint16_t так, чтобы они воспринимались
	// как int16_t
	uint16_t * const uintptr = (uint16_t *)&primdata->shuntv;
	*uintptr = primregs[0];

	const uint16_t busvreg = primregs[1];
	primdata->busv = INA219_GET_BITS(busvreg, INA219_BUSVREG__BUSV);
	primdata->cnvr = INA219_GET_BITS(busvreg, INA219_BUSVREG__CNVR);
	primdata->ovf = INA219_GET_BITS(busvreg, INA219_BUSVREG__OVF);

}


//! нарезка значений "расчетных" регистров на "расчетные" данные
inline static void _slice_sec_data(const uint16_t * secregs, ina219_secondary_data_t * secdata)
{
	// все очень просто
	secdata->power = secregs[0];

	// тут нужно хитро. нужно скопировать бфайты из uint16_t так, чтобы они воспринимались
	// как int16_t
	uint16_t * const uintptr = (uint16_t *)&secdata->current;
	*uintptr = secregs[1];
}



void ina219_load_default_cfg(ina219_cfg_t * cfg)
{
	cfg->bus_range = INA219_BUS_RANGE_32V;
	cfg->bus_res = INA219_ADC_RES_12_BIT_OVS_1;

	cfg->shunt_range = INA219_SHUNT_RANGE_320MV;
	cfg->shunt_res = INA219_ADC_RES_12_BIT_OVS_1;

	cfg->mode = INA219_MODE_SHUNT_AND_BUS_CONT;

	cfg->shunt_r = 0.0f;
	cfg->current_lsb = 0.0f;
}


void ina219_init(ina219_t * device, i2c_port_t port, uint8_t i2c_addr, uint32_t timeout)
{
	device->i2c_port = port;
	device->i2c_address = i2c_addr;
	device->tick_timeout = timeout;
	ina219_load_default_cfg(&device->cfg);
}


void ina219_deinit(ina219_t * device)
{
	(void)device;
	// нечего делать..,
}


int ina219_sw_reset(ina219_t * device)
{
	uint16_t cfgreg = 0;
	cfgreg = INA219_SET_BITS(cfgreg, INA219_CFGREG__RESET, 1);
	return _write_reg(device, INA219_CFGREG_ADDR, cfgreg);
}


int ina219_set_cfg(ina219_t * device, const ina219_cfg_t * cfg)
{
	uint16_t cfgreg = _make_cfg_reg(cfg);
	int error = _write_reg(device, INA219_CFGREG_ADDR, cfgreg);
	if (error)
		return error;

	device->cfg = *cfg;
	// FIXME: не все копировать

	uint16_t calreg = _make_cal_reg(cfg->current_lsb, cfg->shunt_r);
	error = _write_reg(device, INA219_CALREG_ADDR, calreg);
	if (error)
		return error;


	device->cfg.current_lsb = cfg->current_lsb;
	device->cfg.shunt_r = cfg->shunt_r;

	return 0;
}


int ina219_get_cfg(ina219_t * device, ina219_cfg_t *cfg)
{
	ina219_cfg_t _cfg = {0};
	uint16_t raw = 0;;
	int error = _read_reg(device, INA219_CFGREG_ADDR, &raw);
	if (error)
		return error;
	_parse_cfg_reg(raw, &_cfg);
	*cfg = _cfg;
	return 0;
}


int ina219_set_mode(ina219_t * device, const ina219_mode_t mode)
{
	// запоминаем старый режим, на случай если что-то пойдет не так
	const ina219_mode_t old_mode = device->cfg.mode;
	// пробуем обновить режим в дескрипторе
	device->cfg.mode = mode;

	// пробуем зашить новый конфиг регистр
	uint16_t cfgreg = _make_cfg_reg(&device->cfg);
	int error = _write_reg(device, INA219_CFGREG_ADDR, cfgreg);

	// Если что-то пошло не так - восстанавливаем старый режим
	// он наиболее соответствует действительности
	if (error)
		device->cfg.mode = old_mode;

	return error;
}


int ina219_read_primary(ina219_t * device, ina219_primary_data_t * data)
{
	uint16_t raw_reg_data[2];
	int error = _read_reg(device, INA219_SHUNTVREG_ADDR, &raw_reg_data[0]);
	if (error)
		return error;

	error = _read_reg(device, INA219_BUSVREG_ADDR, &raw_reg_data[1]);
	if (error)
		return error;


	_slice_prim_data(raw_reg_data, data);
	return 0;
}


int ina219_read_all_data(ina219_t *device, ina219_data_t *data) {
	int err = 0;
	uint16_t raw[4];

	err = _read_reg(device, INA219_BUSVREG_ADDR, &raw[0]);
	if (err) {
		return 1;
	} else {
		int16_t t = *(int16_t *)&raw[0];
		data->busv = (t >> 3) * 0.004;
		data->ovf = raw[0] & 1;
		data->cnvr = (raw[0] >> 1) & 1;
	}

	err = _read_reg(device, INA219_SHUNTVREG_ADDR, &raw[1]);
	if (err) {
		data->shuntv = NAN;
	} else {
		data->shuntv = raw[1] * 0.00001;
	}

	err = _read_reg(device, INA219_CURRENTREG_ADDR, &raw[2]);
	if (err) {
		data->current = NAN;
	} else {
		data->current = device->cfg.current_lsb * *(int16_t *)&raw[2];
	}

	err = _read_reg(device, INA219_POWERREG_ADDR, &raw[3]);
	if (err) {
		data->power = NAN;
	} else {
		data->power = device->cfg.current_lsb * raw[3] * 20;
	}

	return 0;
}


int ina219_read_secondary(ina219_t * device, ina219_secondary_data_t * data)
{
	uint16_t raw_reg_data[2];
	int error = _read_reg(device, INA219_POWERREG_ADDR, &raw_reg_data[0]);
	if (error)
		return error;

	error = _read_reg(device, INA219_CURRENTREG_ADDR, &raw_reg_data[1]);
		if (error)
			return error;


	_slice_sec_data(raw_reg_data, data);
	return 0;
}


int ina219_read_all(ina219_t * device, ina219_primary_data_t * prim_data,
		ina219_secondary_data_t * secondary_data)
{
	int error = ina219_read_primary(device, prim_data);
	if (error)
		return error;

	error = ina219_read_secondary(device, secondary_data);
	return error;
}

int ina219_read_reg(ina219_t * device, uint8_t regaddr, uint16_t * regdata)
{
	return _read_reg(device, regaddr, regdata);
}




