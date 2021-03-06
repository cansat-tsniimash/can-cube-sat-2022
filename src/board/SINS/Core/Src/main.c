/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <math.h>

#include <stm32f4xx_hal.h>
//#include <diag/Trace.h>

#include "sins_config.h"

#include "drivers/time_svc/time_svc.h"
#include "drivers/mems/mems.h"
#include "drivers/gps/gps.h"
#include "drivers/uplink.h"
#include "drivers/analog/analog.h"
#include "drivers/time_svc/timers_world.h"
#include "backup_sram.h"
#include "drivers/led.h"
#include "errors.h"
#include "SolTrack.h"

#include "mav_packet.h"

#include "state.h"

#include "ahrs.h"
#include "MadgwickAHRS.h"
#include "quaternion.h"
#include "sensors.h"
#include "lds.h"

#include "commissar.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define    DWT_CYCCNT    *(volatile uint32_t*)0xE0001004
#define    DWT_CONTROL   *(volatile uint32_t*)0xE0001000
#define    SCB_DEMCR     *(volatile uint32_t*)0xE000EDFC

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c2;

IWDG_HandleTypeDef hiwdg;

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

error_system_t error_system;
state_system_t state_system;
state_zero_t state_zero;
stateSINS_isc_t stateSINS_isc;
stateSINS_isc_t stateSINS_isc_prev;
//stateSINS_lds_t stateSINS_lds;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_ADC1_Init(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */

static void read_ldiods(float arr[ITS_SINS_LDS_COUNT]);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void dwt_init()
{
	SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;// ?????????????????? ???????????????????????? DWT
	DWT_CONTROL|= DWT_CTRL_CYCCNTENA_Msk; // ???????????????? ??????????????
	DWT_CYCCNT = 0;// ???????????????? ??????????????
}


void system_reset()
{
	led_blink(5, 400);
	HAL_NVIC_SystemReset();
}

static uint32_t last_gps_packet_ts = 0;
static uint32_t last_gps_fix_packet_ts = 0;


/*
static void calibration_accel()
{
	backup_sram_enable();
	backup_sram_erase();

	sensors_init();

	for(;;)
	{
		//	Arrays
		float accel[3] = {0, 0, 0};
		float gyro[3] = {0, 0, 0};
//		float magn[3] = {0, 0, 0};

//		error_system.lis3mdl_error = sensors_lis3mdl_read(magn);
	//	trace_printf("lis error %d\n", error_system.lis3mdl_init_error);

		error_system.lsm6ds3_error = sensors_lsm6ds3_read(accel, gyro);
	//	trace_printf("lsm error %d\n", error_system.lsm6ds3_init_error);

		if (error_system.lsm6ds3_error != 0)
			continue;

		//	?????????????????????????? ???? ?? ???????????????????? ?? ??????????????????
		stateSINS_isc.tv.tv_sec = HAL_GetTick();

		for (int k = 0; k < 3; k++)
		{

			stateSINS_isc.accel[k] = accel[k];
		}

		mavlink_sins_isc(&stateSINS_isc);
		led_toggle();
	}
}
*/


/*
static void calibration_magn()
{
	backup_sram_enable();
	backup_sram_erase();

	sensors_init();

	for(;;)
	{
		//	Arrays
//		float accel[3] = {0, 0, 0};
//		float gyro[3] = {0, 0, 0};
		float magn[3] = {0, 0, 0};

		error_system.lis3mdl_error = sensors_lis3mdl_read(magn);

		if (error_system.lis3mdl_error!= 0)
			continue;

		//	?????????????????????????? ???? ?? ???????????????????? ?? ??????????????????
		stateSINS_isc.tv.tv_sec = HAL_GetTick();

		for (int k = 0; k < 3; k++)
		{

			stateSINS_isc.magn[k] = magn[k];
		}

		mavlink_sins_isc(&stateSINS_isc);
		led_toggle();
	}
}
*/

void on_gps_packet_ahrs(void *arg, const ubx_any_packet_t * packet) {
    if (!ITS_SINS_AHRS_USE_LDS) {
        return;
    }

    int is_packet_valid =
            (packet->pid == UBX_PID_NAV_SOL &&
             packet->packet.navsol.gps_fix == UBX_FIX_TYPE__3D);

    if (!is_packet_valid || time_svc_timebase() != TIME_SVC_TIMEBASE__GPS) {
        return;
    }

    struct timeval tv = {0};
    time_svc_world_get_time(&tv);
    struct Time time = {0};
    time_t seconds = tv.tv_sec;
    struct tm *lt = gmtime(&seconds);
    time.second = tv.tv_sec % 60 + tv.tv_usec / 1000000.0;
    time.minute = lt->tm_min;
    time.hour = lt->tm_hour;
    time.day = lt->tm_mday;
    time.month = lt->tm_mon + 1;
    time.year = lt->tm_year + 1900;

    struct Location location = {0};
    //location.latitude = packet->packet.navsol.pos_ecef;

    int useDegrees = 0;
    int useNorthEqualsZero = 1;
    int computeRefrEquatorial = 0;
    int computeDistance = 0;

    struct Position position = {0};

    float long_lat_coords[3] = {0};
    float xyz_coords[3] = {packet->packet.navsol.pos_ecef[0], packet->packet.navsol.pos_ecef[1], packet->packet.navsol.pos_ecef[2]};
    dekart_to_euler(xyz_coords, long_lat_coords);
    location.latitude = (M_PI / 2 - long_lat_coords[1]);
    location.longitude = (long_lat_coords[2]);
    SolTrack(time, location, &position, useDegrees, useNorthEqualsZero, computeRefrEquatorial, computeDistance);

    float dir_sph[3] = {0};
    dir_sph[0] = 1;
    dir_sph[1] = M_PI / 2 - position.altitude;
    dir_sph[2] = position.azimuthRefract;
    float dir_xyz[3] = {0};

    euler_to_dekart(dir_xyz, dir_sph);

    ahrs_updateVecReal(AHRS_LIGHT, vec_arrToVec(dir_xyz));

    stateSINS_isc.dir_real_sph[0] = dir_sph[0];
    stateSINS_isc.dir_real_sph[1] = dir_sph[1];
    stateSINS_isc.dir_real_sph[2] = dir_sph[2];
    stateSINS_isc.last_valid_gps_packet_time = HAL_GetTick();
    stateSINS_isc.should_we_use_lds = 1;
}
// ?????????????? ?????? ???????????????? ???? ?????????????????? GPS ?? ???????????????? ?????? ?????????????? ?? ??????????????
static void on_gps_packet_main(void * arg, const ubx_any_packet_t * packet)
{
	switch (packet->pid)
	{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
	case UBX_PID_NAV_SOL:
		// ???????????????? ?????????? ?????????????????? ???????????? ?? ????????????
		if (packet->packet.navsol.gps_fix == UBX_FIX_TYPE__3D)
			last_gps_fix_packet_ts = HAL_GetTick();
#pragma GCC diagnostic pop
		/* no break */

	case UBX_PID_NAV_TIMEGPS:
	case UBX_PID_TIM_TP:
		// ???????????????? ?????????? ?????????????????? ???????????? ????????????-???????? ????????????
		last_gps_packet_ts = HAL_GetTick();
		break;

	default:
		// ?????????????????? ???????????? ?????? ???? ????????????????????
		break;
	}

	// ???????????????? ???????????? ?????? ??????????????????
	on_gps_packet(arg, packet);
	on_gps_packet_ahrs(arg, packet);
}

void quat_to_angles(float* quat, float *ang){

    ang[0] = (float)atan((double)((2 * (quat[0] * quat[1] + quat[2] * quat[3])) / (1 - 2 * (quat[1] * quat[1] + quat[2] * quat[2]))));

    ang[1] = (float)asin((double)(2 * (quat[0] * quat[2] - quat[3] * quat[1])));

    ang[2] = (float)atan((double)((2 * (quat[0] * quat[3] + quat[1] * quat[2])) / (1 - 2 * (quat[2] * quat[2] + quat[3] * quat[3]))));
}
/**
  * @brief	Collects data from SINS, stores it and makes quat using "S.Madgwick's" algo
  * @retval	R/w IMU error
  */
int UpdateDataAll(void)
{
	int error = 0;

	//30 ??????????
    if (HAL_GetTick() - stateSINS_isc.last_valid_gps_packet_time > 1000 * 1800) {
        stateSINS_isc.should_we_use_lds = 0;
    }
	//	Arrays
	float accel[3] = {0, 0, 0};
	float gyro[3] = {0, 0, 0};
	float magn[3] = {0, 0, 0};
	float magn_raw[3] ={0, 0, 0};

	error_system.lis3mdl_error = sensors_lis3mdl_read(magn_raw);
	float magn_callibrated[3] = {0};
	sensors_magn_callibrate(magn_raw, magn_callibrated);
	magn[0] = -magn_callibrated[1];
	magn[1] = -magn_callibrated[0];
	magn[2] = magn_callibrated[2];

//	trace_printf("lis error %d\n", error_system.lis3mdl_init_error);

	error_system.lsm6ds3_error = sensors_lsm6ds3_read(accel, gyro);
//	trace_printf("lsm error %d\n", error_system.lsm6ds3_init_error);

	float light[3];
	if (ITS_SINS_AHRS_USE_LDS) {
	    float arr[ITS_SINS_LDS_COUNT] = {0};
	    read_ldiods(arr);
        for (int i = 0; i < ITS_SINS_LDS_COUNT; i++) {
            stateSINS_isc.sensor[i] = arr[i];
        }

        float lds_ginv_buf[LDS_DIM * LDS_COUNT];
        Matrixf lds_ginv = matrix_create_static(LDS_DIM, LDS_COUNT, lds_ginv_buf, LDS_DIM * LDS_COUNT);
        int res = lds_get_ginversed(arr, 0.01, &lds_ginv);
        lds_solution(arr, &lds_ginv, light);
        stateSINS_isc.dir_error = lds_calc_error(arr, 0.1, &lds_ginv, light);
        if (isnan(light[0]) || isnan(light[1]) || isnan(light[2]) || res) {
            light[0] = 1;
            light[1] = 0;
            light[2] = 0;
            stateSINS_isc.dir_error = -1;
        }


        //lds_find_bound(light, arr, 0.1);

	    //lds_find(light, arr);

	    {
            float light_sph[3];
            dekart_to_euler(light, light_sph);

            for (int i = 0; i < 3; i++) {
                stateSINS_isc.dir_sph[i] = light_sph[i];
                stateSINS_isc.dir_xyz[i] = light[i];
            }
            static uint32_t tick = 0;
            if (tick % 50 == 0) {
    //            printf("lds: %.3f %.3f %.3f\n", light[0], light[1], light[2]);
            }
            tick++;
        }


        time_svc_world_get_time(&stateSINS_isc.tv);
	}

	time_svc_world_get_time(&stateSINS_isc.tv);




	if ((error_system.lsm6ds3_error != 0) && (error_system.lis3mdl_error != 0))
		return -22;

	/////////////////////////////////////////////////////
	/////////////	UPDATE QUATERNION  //////////////////
	/////////////////////////////////////////////////////

	float dt = ((float)((stateSINS_isc.tv.tv_sec * 1000 + stateSINS_isc.tv.tv_usec / 1000)  - (stateSINS_isc_prev.tv.tv_sec * 1000 + stateSINS_isc_prev.tv.tv_usec / 1000))) / 1000;
//	trace_printf("dt = %f", dt);
	stateSINS_isc_prev.tv.tv_sec = stateSINS_isc.tv.tv_sec;
	stateSINS_isc_prev.tv.tv_usec = stateSINS_isc.tv.tv_usec;

    float beta = 6.0;
    quaternion_t ori = {1, 0, 0, 0};

    if (error_system.lsm6ds3_error == 0) {
        stateSINS_isc.do_we_use_lds = ITS_SINS_AHRS_USE_LDS && stateSINS_isc.should_we_use_lds;
        stateSINS_isc.do_we_use_mag = (error_system.lis3mdl_error == 0) && ITS_SINS_AHRS_USE_MAG && !(stateSINS_isc.do_we_use_lds && ITS_SINS_AHRS_LDS_MAG_EXCLUSIVE);
        stateSINS_isc.do_we_use_mag = 0;
        stateSINS_isc.do_we_use_lds = 1;


        ahrs_vectorActivate(AHRS_MAG, stateSINS_isc.do_we_use_mag);
        ahrs_vectorActivate(AHRS_LIGHT, stateSINS_isc.do_we_use_lds);
        ahrs_vectorActivate(AHRS_ACCEL, 1);
        ahrs_setKoefB(beta);

        ahrs_updateVecReal(AHRS_MAG, ahrs_get_good_vec_from_mag(vec_arrToVec(magn)));
        ahrs_updateVecReal(AHRS_LIGHT, vec_init(1, -1, 0));
        //Real of light is set in on_gps_packet_ahrs
        //Real of accel is set on start

        ahrs_updateVecMeasured(AHRS_MAG, vec_arrToVec(magn));
        ahrs_updateVecMeasured(AHRS_LIGHT, vec_arrToVec(light));
        ahrs_updateVecMeasured(AHRS_ACCEL, vec_arrToVec(accel));

        ahrs_updateGyroData(vec_arrToVec(gyro));
        ahrs_calculateOrientation(dt);
    }

    ori = ahrs_getOrientation();
    stateSINS_isc.quaternion[0] = ori.w;
    stateSINS_isc.quaternion[1] = ori.x;
    stateSINS_isc.quaternion[2] = ori.y;
    stateSINS_isc.quaternion[3] = ori.z;


	/////////////////////////////////////////////////////
	///////////  ROTATE VECTORS TO ISC  /////////////////
	/////////////////////////////////////////////////////

    //vector_t accel_ISC = vec_arrToVec(accel);
	//accel_ISC = vec_rotate(&t, &ori);
    //vector_t mag_ISC = vec_arrToVec(magn);
	//mag_ISC = vec_rotate(&t, &ori);

	for (int i = 0; i < 3; i++) {
		if (0 == error_system.lsm6ds3_error) {
			stateSINS_isc.accel[i] = accel[i];
			stateSINS_isc.gyro[i] = gyro[i];
		} else {
			stateSINS_isc.accel[i] = NAN;
			stateSINS_isc.gyro[i] = NAN;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (0 == error_system.lis3mdl_error) {
			stateSINS_isc.magn[i] = magn[i];
			stateSINS_isc.magn_raw[i] = magn_raw[i];
		} else {
			stateSINS_isc.magn[i] = NAN;
			stateSINS_isc.magn_raw[i] = NAN;
		}
	}

	float ang[3];
	quat_to_angles(stateSINS_isc.quaternion, ang);
	//printf("ang: %6.2f %6.2f %6.2f\n", ang[0], ang[1], ang[2]);
	return error;
}


/**
  * @brief	Special function for updating previous values structures by current values
  */
void SINS_updatePrevData(void)
{
	for(int i = 0; i < 3; i++)
		if (isnanf(stateSINS_isc.quaternion[i]))		//???????????????? ???? ??????
			return;

	__disable_irq();
	memcpy(&stateSINS_isc_prev,			&stateSINS_isc,			sizeof(stateSINS_isc));
//	memcpy(&state_system_prev, 			&state_system,		 	sizeof(state_system));		//FIXME: ?????????? ?????? ?????????????
	__enable_irq();
}


//FIXME: ???????????? ?????? ????????????????
int check_SINS_state(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpioc;
	gpioc.Mode = GPIO_MODE_INPUT;
	gpioc.Pin = GPIO_PIN_9;
	gpioc.Pull = GPIO_NOPULL;
	gpioc.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB, &gpioc);


	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)) //?????? ?? ??????????????????
		return 1;
	else
		return 0;
}


static uint16_t _fetch_reset_cause(void)
{
	int rc = 0;

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
		rc |= MCU_RESET_PIN;

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
		rc |= MCU_RESET_POR;

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
		rc |= MCU_RESET_SW;

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
		rc |= MCU_RESET_WATCHDOG;

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
		rc |= MCU_RESET_WATCHDOG2;

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
		rc |= MCU_RESET_LOWPOWER;

	__HAL_RCC_CLEAR_RESET_FLAGS();
	return rc;
}


#define RTC_BKP_DR1                     0x00000001U

static uint32_t RTC_BAK_GetRegister(RTC_TypeDef *RTCx, uint32_t BackupRegister)
{
	register uint32_t tmp = 0U;

	  tmp = (uint32_t)(&(RTCx->BKP0R));
	  tmp += (BackupRegister * 4U);

	  /* Read the specified register */
	  return (*(__IO uint32_t *)tmp);
}


static void RTC_BAK_SetRegister(RTC_TypeDef *RTCx, uint32_t BackupRegister, uint32_t Data)
{
  register uint32_t tmp = 0U;

  tmp = (uint32_t)(&(RTCx->BKP0R));
  tmp += (BackupRegister * 4U);

  /* Write the specified register */
  *(__IO uint32_t *)tmp = (uint32_t)Data;
}


int HSI_SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
							  |RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.LSEState = RCC_LSE_BYPASS;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	return 1;
	}
	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
	return 1;
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
	return 1;
	}

	return 0;
}


//?????????????????? ?????????????????????? ???? ??????????
int HSE_SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
//  RCC_OscInitStruct.LSEState = RCC_LSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    return 1;
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    return 1;
  }
//  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
//  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    return 1;
  }
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();

  return 0;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
#ifdef WITHOUT_RTC
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
#endif


  //?????????????????????? ?? RTC
  int error = HSE_SystemClock_Config();
  if (error)
  {
	  error = 0;
	  error = HSI_SystemClock_Config();
	  if (1 == error)
		  HAL_NVIC_SystemReset();
  }



  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_RTC_Init();
  MX_ADC1_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */

  //	Global structures init
	memset(&stateSINS_isc, 			0x00, sizeof(stateSINS_isc));
	memset(&stateSINS_isc_prev, 	0x00, sizeof(stateSINS_isc_prev));
  	memset(&state_system,			0x00, sizeof(state_system));
  	memset(&state_zero,				0x00, sizeof(state_zero));
  	memset(&error_system, 			0x00, sizeof(error_system));

  	led_init();

  	dwt_init();
  	commissar_init();


  	if (RCC_PLLSOURCE_HSI == __HAL_RCC_GET_PLL_OSCSOURCE())
  		error_system.osc_source = ACTIVE_OSCILLATOR_HSI;
	else
		error_system.osc_source = ACTIVE_OSCILLATOR_HSE;

  	/*if (CALIBRATION_LSM)
  		calibration_accel();

  	if (CALIBRATION_LIS)
  		calibration_magn();*/



  	/*if (check_SINS_state() == 1)
  	{
  		backup_sram_enable();
  		backup_sram_erase();

  		sensors_init();
  		error_mems_read();

  		HAL_Delay(1000);
  		int error;
  		for (int i = 0; i < 2; i++)
  		{
  			error = mems_get_gyro_staticShift(state_zero.gyro_staticShift);
  			error += mems_get_accel_staticShift(state_zero.accel_staticShift);
  			if (error != 0)
  			{
  				system_reset();
  			}
  		}

  		error_system.reset_counter = 0;
  		backup_sram_write_reset_counter(&error_system.reset_counter);
  		backup_sram_write_zero_state(&state_zero);

  	}
  	else*/
  	{
  		// ???????????? ???? ?????????? ?????????????????? ???????????????????? ??????????????????, ?????????????? ?? ???????? ??????????????????
		error_system.reset_counter = RTC_BAK_GetRegister(RTC, RTC_BKP_DR1);
		error_system.reset_counter += 1;
		RTC_BAK_SetRegister(RTC, RTC_BKP_DR1, error_system.reset_counter);

		// ?????????????? ?????????????? ???????????????????? ????????????
		error_system.reset_cause = _fetch_reset_cause();


  		//iwdg_init(&transfer_uart_iwdg_handle);

  		time_svc_steady_init();

  		int error = time_svc_world_preinit_with_rtc();
  		error_system.rtc_error = error;
  		if (error != 0)
  			time_svc_world_preinit_without_rtc(); 		//???? ???????????? ?????????????????? rtc. ???????????????????? ?????? ????????
  		else
  			time_svc_world_init();			//???????????? ?????????????????? rtc. ???????????????? ?????? ??????????????????

  		error = 0;
  		error = uplink_init();
  		if (error != 0)
  			system_reset();				//???????? ???? ???????????????????? uart, ???? ???? - ????????????

  		// ?????????????????????? GPS

  		error = 0;
  		error = gps_init(on_gps_packet_main, NULL);
  		gps_configure_begin();
  		if (error != 0)
  		{
  			error_system.gps_uart_init_error = error;
  		}
  		else
  		{
  			error_system.gps_config_error = error;
  		}
  		printf("gps init error %d\n", error);

  		// ?????????????????????? ???????????????????? ?????????????? (???????? ???????????? ???????????????????? ??????????????????)

  		error = 0;
  		error = analog_init();
  		error_system.analog_sensor_init_error = error;
  		if (error != 0)
  			{
  				HAL_Delay(500);
  				error_system.analog_sensor_init_error = analog_restart();
  			}

  		ahrs_init();
        ahrs_vectorActivate(AHRS_ACCEL, 1);
        ahrs_updateVecReal(AHRS_ACCEL, vec_init(0, 0, 1));
        ahrs_updateVecPortion(AHRS_ACCEL, 1);

        ahrs_vectorActivate(AHRS_MAG, 1);
        ahrs_updateVecPortion(AHRS_MAG, 1);

        ahrs_vectorActivate(AHRS_LIGHT, 1);
        ahrs_updateVecPortion(AHRS_LIGHT, 1);


  		// ???????????
  		sensors_init();
  		error_mems_read();


  		// ???????????????? ?????????????? ?????????? (???? ??????????)
  		time_svc_world_get_time(&stateSINS_isc_prev.tv);

  		// ???
  		error_system_check();

  		// ?????????? ?????????? ?? ???? ???? ???????????? (???? ??????????)
  		uint32_t prew_time = HAL_GetTick();
  		uint32_t time = 0;

  		commissar_init();

  		int HSE_start_time = 0;
  		//???????? ???????? ???? HSE ?????????????????????????????? ???? ??????????
  		if (RCC_PLLSOURCE_HSI == __HAL_RCC_GET_PLL_OSCSOURCE())
  		{
  			//?????????? ???????????????? ?????????????????????? ???? HSE ???????????? 10 ??????????
			HSE_start_time = HAL_GetTick();

  		}

  		int comissar_last_report_time = HAL_GetTick();
  		// ??????????, ????????????????
  		for (; ; )
  		{
  			// ?????????????????? ?????????????? ???????????? (?????????????? ???????????? ????????????)
  			for (int u = 0; u < 5; u++)
  			{
  				for (int i = 0; i < 10; i++)
  				{
  				    {
  				        static uint32_t prev = 0;
  				        uint32_t now= HAL_GetTick();
  				        if (now - prev >= 1000) {
  				            prev += 1000;

                            struct timeval tv;
                            time_svc_world_get_time(&tv);
  				            printf("@TIME: %d.%06d | %d ticks\n", (int)tv.tv_sec, (int)tv.tv_usec, (int)now);
  				        }
  				    }
  					// ???????????? ?????????????? ??????????????
  					UpdateDataAll();
  					SINS_updatePrevData();
  				}

  				// ????????????, ?????????? ????????????????
  				gps_poll();
  	  			// ?????????????? ?????????????????? ???????????????????? ??????????????????
  	  			uplink_flush_injected();

  				// ?????????????????? ?????????????? ????????????????????????
  				const int gps_cfg_status = gps_configure_status();
  				if (gps_cfg_status != -EWOULDBLOCK) // ???????????????????????? ?????? ??????????????????????
  				{
  					error_system.gps_config_error = gps_cfg_status;
  					uint32_t now = HAL_GetTick();

  					if (gps_cfg_status != 0)
  					{
  						// ?????????????????????? ???? ??????????. ???????????????? ??????????
  						error_system.gps_reconfig_counter++;
  						gps_configure_begin();
  					}
  					else if (now - last_gps_fix_packet_ts > ITS_SINS_GPS_MAX_NOFIX_TIME)
  					{
  						// ???????? GPS ?????????????? ?????????? ???? ????????????????
  						// ???????? ???????????????????? ?????? ?? ????????????????????????????s
  						// ?????????? ???????? ?????????????? ??????, ?????? ???????? ?? ?????? ??????
  						// ?????? ?????? ???? ?????????? ???????????????????? ??????????, ?? ???????? ?????? ?????????? ???? ???????????????? -
  						// ???? ?????? ?????????? ?????????? ?????????????????????? ??????????????????
  						error_system.gps_reconfig_counter++;
  						last_gps_fix_packet_ts = HAL_GetTick();
  						gps_configure_begin();
  					}
  					else if (now - last_gps_packet_ts > ITS_SINS_GPS_MAX_NOPACKET_TIME)
  					{
  						// ???????? ?????????????? ?????????? ???? ?????????????????? ???????????????????? ?????? ??????????????
  						// ???????????????????? gps ?? ????????????????????????????
  						// ?????????????? ??????????,?????????? ???? ?????????????????? ???????? ?????????? ??????????, ???????? ??????-???? ?????????? ???? ??????
  						error_system.gps_reconfig_counter++;
  						last_gps_packet_ts = HAL_GetTick();
  						gps_configure_begin();
  					}
  				}

  				// ?????????????????? ???????????????????? ???? ????????????
  				if ((error_system.lsm6ds3_error != 0) && (error_system.lis3mdl_error != 0))
  					continue;
  				// ???????????????????? ???????????? ???? ?????????????? ??????
  				mavlink_sins_isc(&stateSINS_isc); //<<---------------TODO: ????????????????????
  				//mavlink_ahrs_stats(&stateSINS_lds);

  			}
  			time = HAL_GetTick();
  			if (time - prew_time < 1000)
  				continue;
  			prew_time = time;

  			// ???????????????????? ???????????? ???????????? ???????????? (?????? ??????)
  			mavlink_timestamp();
  			own_temp_packet();
  			//mavlink_light_diode(&stateSINS_lds);

  			error_mems_read();
  			mavlink_errors_packet();
  			mavlink_its_link_stats();
  			commissar_work();
  			if (comissar_last_report_time + 1000 >= HAL_GetTick())
  			{
  				mavlink_comissar_report();
  				comissar_last_report_time  = HAL_GetTick();
  			}

  			if (RCC_PLLSOURCE_HSI == __HAL_RCC_GET_PLL_OSCSOURCE())
  			{
  				int HSE_stop_time = HAL_GetTick();

  				if ((HSE_stop_time - HSE_start_time) > 10 * 60 * 1000)
  					HAL_NVIC_SystemReset();
  			}
  		}
  	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    break;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  HAL_NVIC_SystemReset();
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**I2C1 GPIO Configuration
  PB8   ------> I2C1_SCL
  PB9   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* I2C1 DMA Init */

  /* I2C1_RX Init */
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_0, LL_DMA_CHANNEL_1);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_0, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_0, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_0, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_0, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_0, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_0, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_0, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_0);

  /* I2C1_TX Init */
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_6, LL_DMA_CHANNEL_1);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_6, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_6, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_6, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_6, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_6, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_6, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_6, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_6);

  /* I2C1 interrupt Init */
  NVIC_SetPriority(I2C1_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  NVIC_EnableIRQ(I2C1_EV_IRQn);
  NVIC_SetPriority(I2C1_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  NVIC_EnableIRQ(I2C1_ER_IRQn);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  /** I2C Initialization
  */
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.ClockSpeed = 400000;
  I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
  I2C_InitStruct.OwnAddress1 = 244;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C1, 0);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */
#ifndef DEBUG
  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */
#endif

#ifdef DEBUG
  return;
#endif
  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
	return;

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, PWR_MEMS_Pin|PWR_GPS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : GPS_PPS_INPUT_Pin */
  GPIO_InitStruct.Pin = GPS_PPS_INPUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPS_PPS_INPUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PWR_MEMS_Pin PWR_GPS_Pin */
  GPIO_InitStruct.Pin = PWR_MEMS_Pin|PWR_GPS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */

static void read_ldiods(float arr[ITS_SINS_LDS_COUNT]) {
    static const analog_target_t order_adc[ITS_SINS_LDS_COUNT] = {
        ANALOG_TARGET_LED_0,
        ANALOG_TARGET_LED_1,
        ANALOG_TARGET_LED_2,
        ANALOG_TARGET_LED_3,
        ANALOG_TARGET_LED_4,
        ANALOG_TARGET_LED_5,
        ANALOG_TARGET_LED_6,
        ANALOG_TARGET_LED_7,
        ANALOG_TARGET_LED_8,
        ANALOG_TARGET_LED_9
    };
    static const analog_target_t order_led[ITS_SINS_LDS_COUNT] = {
        LDIODE_SWAP_0,
        LDIODE_SWAP_1,
        LDIODE_SWAP_2,
        LDIODE_SWAP_3,
        LDIODE_SWAP_4,
        LDIODE_SWAP_5,
        LDIODE_SWAP_6,
        LDIODE_SWAP_7,
        LDIODE_SWAP_8,
        LDIODE_SWAP_9,
    };

    for (int i = 0; i < ITS_SINS_LDS_COUNT; i++)
    {
        uint16_t value;
        int rc = analog_get_raw(order_adc[i], 5, &value);
        assert(0 == rc);
        float v = ((float)value / (1 << 12)) * 3.3;
        arr[order_led[i]] = v;
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
#ifdef DEGUB
  while (1)
  {
  }
#endif
  HAL_NVIC_SystemReset();


  __disable_irq();


  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
