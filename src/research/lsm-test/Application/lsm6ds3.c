/*
 * lsm6ds3.c
 *
 *  Created on: 7 дек. 2019 г.
 *      Author: developer
 */

#include "lsm6ds3.h"
#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stm32f4xx_hal.h>


#define LSM_TIMEOUT	1000
#define MG_TO_MPS2	9.80665 / 1000
#define MDPS_TO_RAD	M_PI / 180 / 1000


//	Accelerometer bias & transform matrix
#define X_ACCEL_OFFSET		0.000000
#define Y_ACCEL_OFFSET		0.000000
#define Z_ACCEL_OFFSET		0.000000
#define XX_ACCEL_TRANSFORM_MATIX	 1.000000
#define YY_ACCEL_TRANSFORM_MATIX	 1.000000
#define ZZ_ACCEL_TRANSFORM_MATIX	 1.000000
#define XY_ACCEL_TRANSFORM_MATIX	 0.000000
#define XZ_ACCEL_TRANSFORM_MATIX	 0.000000
#define YZ_ACCEL_TRANSFORM_MATIX	 0.000000

typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;

typedef union{
  int16_t i16bit;
  uint8_t u8bit[2];
} axis1bit16_t;


stmdev_ctx_t lsm6ds3_dev_ctx;
extern SPI_HandleTypeDef hspi1;


static int32_t lsm6ds3_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static int32_t lsm6ds3_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);


int32_t lsm6ds3_init(void)
{
	int error = 0;

	lsm6ds3_dev_ctx.write_reg = lsm6ds3_write;
	lsm6ds3_dev_ctx.read_reg = lsm6ds3_read;
	lsm6ds3_dev_ctx.handle = &hspi1;

	// Reset to defaults
	lsm6ds3_ctrl3_c_t reg = {0};
	reg.sw_reset = 1;
	lsm6ds3_write_reg(&lsm6ds3_dev_ctx, LSM6DS3_CTRL3_C, (uint8_t*)&reg, 1);
	HAL_Delay(500);
	lsm6ds3_i2c_interface_set(&lsm6ds3_dev_ctx, LSM6DS3_I2C_DISABLE);

	// Check who_am_i
	uint8_t whoami = 0;
	while(1)
	{
		error |= lsm6ds3_device_id_get(&lsm6ds3_dev_ctx, &whoami);
		printf("who am i 0x%02X\n", (int)whoami);
		HAL_Delay(500);
	}

	if (whoami != LSM6DS3_ID)
	{
		printf("lsm6ds3 not found, %d\terror: %d\n", (int)whoami, (int)error);
		return -19;
	}

	printf("lsm6ds3 OK\n");
	error |= lsm6ds3_fifo_mode_set(&lsm6ds3_dev_ctx, PROPERTY_DISABLE);
	error |= lsm6ds3_block_data_update_set(&lsm6ds3_dev_ctx, PROPERTY_DISABLE);
	error |= lsm6ds3_xl_full_scale_set(&lsm6ds3_dev_ctx, LSM6DS3_4g);
	error |= lsm6ds3_gy_full_scale_set(&lsm6ds3_dev_ctx, LSM6DS3_1000dps);
	error |= lsm6ds3_xl_data_rate_set(&lsm6ds3_dev_ctx, LSM6DS3_XL_ODR_104Hz);
	error |= lsm6ds3_gy_data_rate_set(&lsm6ds3_dev_ctx, LSM6DS3_GY_ODR_104Hz);
	error |= lsm6ds3_xl_filter_analog_set(&lsm6ds3_dev_ctx, LSM6DS3_ANTI_ALIASING_200Hz);

	return error;
}


/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t lsm6ds3_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{

	uint32_t error = 0;
	HAL_GPIO_WritePin(LSM_CS_GPIO_Port, LSM_CS_Pin, GPIO_PIN_RESET);
	error |= HAL_SPI_Transmit(handle, &reg, 1, 1000);
	error |= HAL_SPI_Transmit(handle, bufp, len, 1000);
	HAL_GPIO_WritePin(LSM_CS_GPIO_Port, LSM_CS_Pin, GPIO_PIN_SET);

	return error;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t lsm6ds3_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
	uint32_t error = 0;
	/* Read command */
	reg |= 0x80;
	memset(bufp, 0xFF, len);
	HAL_GPIO_WritePin(LSM_CS_GPIO_Port, LSM_CS_Pin, GPIO_PIN_RESET);
	error |= HAL_SPI_Transmit(handle, &reg, 1, 1000);
	error |= HAL_SPI_Receive(handle, bufp, len, 1000);
	HAL_GPIO_WritePin(LSM_CS_GPIO_Port, LSM_CS_Pin, GPIO_PIN_SET);

	static int i = 0;
	printf("%d: 0x%02X\n", i++, (int)*bufp);
	return error;
}

