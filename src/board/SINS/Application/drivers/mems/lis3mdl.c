/*
 * lis3mdl.c
 *
 *  Created on: 7 дек. 2019 г.
 *      Author: developer
 */

#include "lis3mdl.h"

#include <stdint.h>
#include <errno.h>

#include <stm32f4xx.h>

#include <stm32f4xx_hal.h>
#include "library/lis3mdl_STdC/driver/lis3mdl_reg.h"

#include "../common.h"
#include "mems.h"
#include "vector.h"



#define LSM_TIMEOUT	3
#define MDPS_TO_RAD	M_PI / 180 / 1000




typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;


typedef union{
  int16_t i16bit;
  uint8_t u8bit[2];
} axis1bit16_t;



static int32_t lis3mdl_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static int32_t lis3mdl_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);

static stmdev_ctx_t lis3mdl_dev_ctx = {
		.write_reg = lis3mdl_write,
		.read_reg = lis3mdl_read,
		.handle = HMEMS_BUS_HANDLE
};


int mems_lis3mdl_init(void)
{
	int error = 0;
	uint8_t whoamI = 0;

	// Reset to defaults
	lis3mdl_reg_t reg;
	reg.ctrl_reg2.soft_rst = 1;
	error = lis3mdl_write_reg(&lis3mdl_dev_ctx, LIS3MDL_CTRL_REG2, &reg.byte, 1);
	if (error != 0)
		return error;
	// Check who_am_i
	error = lis3mdl_device_id_get(&lis3mdl_dev_ctx, &whoamI);
	if (whoamI != LIS3MDL_ID)
	{
//		trace_printf("lis3mdl not found, %d\terror: %d\n", whoamI, error);
		return -ENODEV;
	}

//	trace_printf("lis3mdl OK\n");


	error = lis3mdl_block_data_update_set(&lis3mdl_dev_ctx, PROPERTY_DISABLE);
	if (0 != error) return error;

	error = lis3mdl_data_rate_set(&lis3mdl_dev_ctx, LIS3MDL_HP_300Hz);
	if (0 != error) return error;

	error = lis3mdl_full_scale_set(&lis3mdl_dev_ctx, LIS3MDL_16_GAUSS);
	if (0 != error) return error;

	error = lis3mdl_operating_mode_set(&lis3mdl_dev_ctx, LIS3MDL_CONTINUOUS_MODE);
	if (0 != error) return error;

	return 0;
}


int mems_lis3mdl_get_m_data_mG(int16_t * data, float * magn)
{

	magn[0] = 1000 * LIS3MDL_FROM_FS_16G_TO_G(data[0]);
	magn[1] = 1000 * LIS3MDL_FROM_FS_16G_TO_G(data[1]);
	magn[2] = 1000 * LIS3MDL_FROM_FS_16G_TO_G(data[2]);


	return 0;
}


int mems_lis3mdl_get_m_data_raw(int16_t * data)
{
	int error;
	axis3bit16_t data_raw_magnetic;

	//	Read data
	error = lis3mdl_magnetic_raw_get(&lis3mdl_dev_ctx, data_raw_magnetic.u8bit);
	if (0 != error)
		return error;

	for(int i = 0; i < 3; i++)
		data[i] = data_raw_magnetic.i16bit[i];

	return 0;
}

static int32_t lis3mdl_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
	HAL_StatusTypeDef hal_error;

	reg |= 0x80;
	hal_error = HAL_I2C_Mem_Write(handle, LIS3MDL_I2C_ADD_H, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, LSM_TIMEOUT);
	if (hal_error != HAL_OK)
		return sins_hal_status_to_errno(hal_error);

	return 0;
}


static int32_t lis3mdl_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
	HAL_StatusTypeDef hal_error;

	reg |= 0x80;
	hal_error = HAL_I2C_Mem_Read(handle, LIS3MDL_I2C_ADD_H, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, LSM_TIMEOUT);
	if (hal_error != HAL_OK)
		return sins_hal_status_to_errno(hal_error);

	return 0;
}

