/*
 * sensors.c
 *
 *  Created on: 22 авг. 2020 г.
 *      Author: developer
 */
#include <errno.h>
#include <string.h>

#include "sensors.h"
#include "drivers/mems/mems.h"
#include "errors.h"
#include "vector.h"

//	Magnetometer bias & transform matrix
#define X_MAGN_OFFSET		 -726.977486
#define Y_MAGN_OFFSET		 159.732996
#define Z_MAGN_OFFSET		 193.624403
#define XX_MAGN_TRANSFORM_MATIX	 0.00317875
#define YY_MAGN_TRANSFORM_MATIX	 0.00409985
#define ZZ_MAGN_TRANSFORM_MATIX	 0.00136249
#define XY_MAGN_TRANSFORM_MATIX	 0.00011033
#define XZ_MAGN_TRANSFORM_MATIX	 -0.00046611
#define YZ_MAGN_TRANSFORM_MATIX	 0.00000621


//	Magnetometer bias and transform matrix (to provide real values)
static const float offset_vector[3] = {X_MAGN_OFFSET, Y_MAGN_OFFSET, Z_MAGN_OFFSET};
static const float transform_matrix[3][3] = {
		{ XX_MAGN_TRANSFORM_MATIX, XY_MAGN_TRANSFORM_MATIX, XZ_MAGN_TRANSFORM_MATIX },
		{ XY_MAGN_TRANSFORM_MATIX, YY_MAGN_TRANSFORM_MATIX, YZ_MAGN_TRANSFORM_MATIX },
		{ XZ_MAGN_TRANSFORM_MATIX, YZ_MAGN_TRANSFORM_MATIX, ZZ_MAGN_TRANSFORM_MATIX }
};

i2c_error_codes i2c_errors;

mems_state_t mems_state = { 0 };


// выстывляем ошибки для шины
static void bus_failure(int error)
{
	mems_state.bus_error = error;
	mems_state.bus_error_counter++;

	mems_state.bus_ready = 0;
	mems_state.lis3mdl_ready = 0;
	mems_state.lsm6ds3_ready = 0;
}


// выстывляем ошибки для lsm
static void lsm6ds3_failure(int error)
{
	mems_state.lsm6ds3_error_counter++;

	switch (error)
	{
		case af :
			mems_state.lsm6ds3_error = error;
			mems_state.lsm6ds3_ready = 0;
			break;

		case (-ENODEV):
			mems_state.lsm6ds3_error = error;
			mems_state.lsm6ds3_ready = 0;
			break;

		default:
			bus_failure(error);
			mems_state.lsm6ds3_error = error;
			break;
	};
}


// выстывляем ошибки для lis
static void lis3mdl_failure(int error)
{
	mems_state.lis3mdl_error_counter++;

	switch (error)
	{
		case af:
			mems_state.lis3mdl_error = error;
			mems_state.lis3mdl_ready = 0;
			break;

		case (-ENODEV):
			mems_state.lis3mdl_error = error;
			mems_state.lis3mdl_ready = 0;
			break;

		default:
			bus_failure(error);
			mems_state.lis3mdl_error = error;
			break;
	};
}


// переинициализируем шину
static int reinit_bus(void)
{
	if (mems_state.bus_ready)
		return 0;

	int error = mems_init_bus();
	if (error)
	{
		bus_failure(error);
		return error;
	}

	mems_state.lsm6ds3_accel_data_counter = 0;
	mems_state.lsm6ds3_gyro_data_counter = 0;
	mems_state.bus_ready = 1;
	return 0;
}


// переинициализируем lsm
static int reinit_lsm6ds3(void)
{
	if (mems_state.lsm6ds3_ready)
		return 0;

	int error = mems_lsm6ds3_init();
	if (error)
	{
		lsm6ds3_failure(error);
		return error;
	}

	mems_state.lsm6ds3_ready = 1;
	return 0;
}


// переинициализируем lis
static int reinit_lis3mdl(void)
{
	if (mems_state.lis3mdl_ready)
		return 0;

	int error = mems_lis3mdl_init();
	if (error)
	{
		lis3mdl_failure(error);
		return error;
	}
	mems_state.lis3mdl_magn_data_counter = 0;
	mems_state.lis3mdl_ready = 1;
	return 0;
}


int sensors_init(void)
{
	reinit_bus();
	if (!mems_state.bus_ready)
		return mems_state.bus_error;

	reinit_lsm6ds3();
	reinit_lis3mdl();

	if (!mems_state.lis3mdl_ready)
		return mems_state.lis3mdl_error;
	if (!mems_state.lsm6ds3_ready)
		return mems_state.lsm6ds3_error;

	return 0;
}


static int lsm6ds3_check_repeat_data(int16_t * data, int16_t * prew_data, int * counter)
{
	if (memcmp(data, prew_data, 3 * sizeof(int16_t)) == 0)
		(*counter)++;
	else
	{
		for(int i = 0; i < 3; i++)
			prew_data[i] = data[i];
		(*counter) = 0;
	}

	if ((*counter) == 5)
	{
		(*counter) = 0;
		return 44;
	}

	return 0;
}


static int lis3mdl_check_repeat_data(int16_t * data, int16_t * prew_data, int * counter)
{
	if (memcmp(data, prew_data, 3 * sizeof(int16_t)) == 0)
		(*counter)++;
	else
	{
		for(int i = 0; i < 3; i++)
			prew_data[i] = data[i];
		(*counter) = 0;
	}

	if ((*counter) == 5)
	{
		(*counter) = 0;
		return 44;
	}

	return 0;
}


int sensors_lsm6ds3_read(float * accel, float * gyro)
{
	reinit_bus();
	if (!mems_state.bus_ready)
		return mems_state.bus_error;

	reinit_lsm6ds3();
	if (!mems_state.lsm6ds3_ready)
		return mems_state.lsm6ds3_error;

	int16_t data_gyro[3] = {0};

	int error = mems_lsm6ds3_get_g_data_raw(data_gyro);
	mems_lsm6ds3_get_g_data_rps(data_gyro, gyro);

	if (error)
	{
		lsm6ds3_failure(error);
		return error;
	}

	error = lsm6ds3_check_repeat_data(data_gyro, mems_state.gyro_prew, &mems_state.lsm6ds3_gyro_data_counter);

	if (error)
	{
		lsm6ds3_failure(error);
		return error;
	}


	int16_t data_accel[3] = {0};

	error = mems_lsm6ds3_get_xl_data_raw(data_accel);
	mems_lsm6ds3_get_xl_data_g(data_accel, accel);

	if (error)
	{
		lsm6ds3_failure(error);
		return error;
	}

	error = lsm6ds3_check_repeat_data(data_accel, mems_state.accel_prew, &mems_state.lsm6ds3_accel_data_counter);

	if (error)
	{
		lsm6ds3_failure(error);
		return error;
	}

	return 0;
}

void sensors_magn_callibrate(const float* in, float* out) {

	vmv(in, (const float*)offset_vector, out);
	mxv((float(*)[3])transform_matrix, out, out);
}

int sensors_lis3mdl_read(float * magn)
{
	reinit_bus();
	if (!mems_state.bus_ready)
		return mems_state.bus_error;

	reinit_lis3mdl();
	if (!mems_state.lis3mdl_ready)
		return mems_state.lis3mdl_error;

	int16_t data_magn[3] = {0};

	int error = mems_lis3mdl_get_m_data_raw(data_magn);
	mems_lis3mdl_get_m_data_mG(data_magn, magn);

	if (error)
	{
		lis3mdl_failure(error);
		return error;
	}

	error = lis3mdl_check_repeat_data(data_magn, mems_state.magn_prew, &mems_state.lis3mdl_magn_data_counter);

	if (error)
	{
		lis3mdl_failure(error);
		return error;
	}

	return 0;
}
