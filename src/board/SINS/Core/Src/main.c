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
#include "drivers/time_svc/timers_world.h"
#include "drivers/temp/analog.h"
#include "backup_sram.h"
#include "drivers/led.h"
#include "errors.h"

#include "mav_packet.h"
#include "watchdog.h"

#include "state.h"

#include "MadgwickAHRS.h"
#include "vector.h"
#include "quaternion.h"
#include "sensors.h"


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

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

IWDG_HandleTypeDef hiwdg;

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

error_system_t error_system;
state_system_t state_system;
stateSINS_rsc_t stateSINS_rsc;
state_zero_t state_zero;
stateSINS_isc_t stateSINS_isc;
stateSINS_isc_t stateSINS_isc_prev;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_IWDG_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void dwt_init()
{
	SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;// разрешаем использовать DWT
	DWT_CONTROL|= DWT_CTRL_CYCCNTENA_Msk; // включаем счётчик
	DWT_CYCCNT = 0;// обнуляем счётчик
}


void system_reset()
{
	led_blink(5, 400);
	HAL_NVIC_SystemReset();
}

static uint32_t last_gps_packet_ts = 0;
static uint32_t last_gps_fix_packet_ts = 0;


static void calibration_accel()
{
	backup_sram_enable();
	backup_sram_erase();

	sensors_init();

	debug_uart_init();

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

		//	пересчитываем их и записываем в структуры
		stateSINS_isc.tv.tv_sec = HAL_GetTick();

		for (int k = 0; k < 3; k++)
		{

			stateSINS_isc.accel[k] = accel[k];
		}

		mavlink_sins_isc(&stateSINS_isc);
		led_toggle();
	}
}


static void calibration_magn()
{
	backup_sram_enable();
	backup_sram_erase();

	sensors_init();

	debug_uart_init();

	for(;;)
	{
		//	Arrays
//		float accel[3] = {0, 0, 0};
//		float gyro[3] = {0, 0, 0};
		float magn[3] = {0, 0, 0};

		error_system.lis3mdl_error = sensors_lis3mdl_read(magn);

		if (error_system.lis3mdl_error!= 0)
			continue;

		//	пересчитываем их и записываем в структуры
		stateSINS_isc.tv.tv_sec = HAL_GetTick();

		for (int k = 0; k < 3; k++)
		{

			stateSINS_isc.magn[k] = magn[k];
		}

		mavlink_sins_isc(&stateSINS_isc);
		led_toggle();
	}
}


// Функция для слежения за здоровьем GPS и передачи его пакетов в мавлинк
static void on_gps_packet_main(void * arg, const ubx_any_packet_t * packet)
{
	switch (packet->pid)
	{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
	case UBX_PID_NAV_SOL:
		// Отмечаем время получения пакета с фиксом
		if (packet->packet.navsol.gps_fix == UBX_FIX_TYPE__3D)
			last_gps_fix_packet_ts = HAL_GetTick();
#pragma GCC diagnostic pop
		/* no break */

	case UBX_PID_NAV_TIMEGPS:
	case UBX_PID_TIM_TP:
		// Отмечаем время получения вообще какого-либо пакета
		last_gps_packet_ts = HAL_GetTick();
		break;

	default:
		// остальные пакеты нас не интересуют
		break;
	}

	// Передаем дальше для обработки
	on_gps_packet(arg, packet);
}


/**
  * @brief	Collects data from SINS, stores it and makes quat using S.Madgwick's algo
  * @retval	R/w IMU error
  */
int UpdateDataAll(void)
{
	int error = 0;

	//	Arrays
	float accel[3] = {0, 0, 0};
	float gyro[3] = {0, 0, 0};
	float magn[3] = {0, 0, 0};

	error_system.lis3mdl_error = sensors_lis3mdl_read(magn);
//	trace_printf("lis error %d\n", error_system.lis3mdl_init_error);

	error_system.lsm6ds3_error = sensors_lsm6ds3_read(accel, gyro);
//	trace_printf("lsm error %d\n", error_system.lsm6ds3_init_error);

	time_svc_world_get_time(&stateSINS_isc.tv);

	//	пересчитываем их и записываем в структуры
	for (int k = 0; k < 3; k++) {
		stateSINS_rsc.accel[k] = accel[k];
		gyro[k] -= state_zero.gyro_staticShift[k];
		stateSINS_rsc.gyro[k] = gyro[k];
		stateSINS_rsc.magn[k] = magn[k];
	}

	if ((error_system.lsm6ds3_error != 0) && (error_system.lis3mdl_error != 0))
		return -22;

	/////////////////////////////////////////////////////
	/////////////	UPDATE QUATERNION  //////////////////
	/////////////////////////////////////////////////////
	float quaternion[4] = {1, 0, 0, 0};


	float dt = ((float)((stateSINS_isc.tv.tv_sec * 1000 + stateSINS_isc.tv.tv_usec / 1000)  - (stateSINS_isc_prev.tv.tv_sec * 1000 + stateSINS_isc_prev.tv.tv_usec / 1000))) / 1000;
//	trace_printf("dt = %f", dt);
	stateSINS_isc_prev.tv.tv_sec = stateSINS_isc.tv.tv_sec;
	stateSINS_isc_prev.tv.tv_usec = stateSINS_isc.tv.tv_usec;


	float beta = 6.0;
	if ((error_system.lsm6ds3_error == 0) && (error_system.lis3mdl_error == 0))
		MadgwickAHRSupdate(quaternion, gyro[0], gyro[1], gyro[2], accel[0], accel[1], accel[2], magn[0], magn[1], magn[2], dt, beta);
	else if (error_system.lsm6ds3_error == 0)
		MadgwickAHRSupdateIMU(quaternion, gyro[0], gyro[1], gyro[2], accel[0], accel[1], accel[2], dt, beta);

	//	копируем кватернион в глобальную структуру
	stateSINS_isc.quaternion[0] = quaternion[0];
	stateSINS_isc.quaternion[1] = quaternion[1];
	stateSINS_isc.quaternion[2] = quaternion[2];
	stateSINS_isc.quaternion[3] = quaternion[3];

	/////////////////////////////////////////////////////
	///////////  ROTATE VECTORS TO ISC  /////////////////
	/////////////////////////////////////////////////////

	float accel_ISC[3] = {0, 0, 0};
	vect_rotate(accel, quaternion, accel_ISC);

	if (0 == error_system.lsm6ds3_error)
	{
		for (int i = 0; i < 3; i++)
		{
			accel_ISC[i] -= state_zero.accel_staticShift[i];
			stateSINS_isc.accel[i] = accel_ISC[i];
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
			stateSINS_isc.accel[i] = 0;
	}

	//	Copy vectors to global structure
	if (0 == error_system.lis3mdl_error)
	{
		for (int i = 0; i < 3; i++)
		{
			stateSINS_isc.magn[i] = magn[i];
		}
	}
	else
	{
		for(int i = 0; i < 3; i++)
			stateSINS_isc.magn[i] = 0;
	}

	return error;
}


/**
  * @brief	Special function for updating previous values structures by current values
  */
void SINS_updatePrevData(void)
{
	for(int i = 0; i < 3; i++)
		if (isnanf(stateSINS_isc.quaternion[i]))		//проверка на нан
			return;

	__disable_irq();
	memcpy(&stateSINS_isc_prev,			&stateSINS_isc,			sizeof(stateSINS_isc));
//	memcpy(&state_system_prev, 			&state_system,		 	sizeof(state_system));		//FIXME: зачем это делать?
	__enable_irq();
}


//FIXME: теперь нет джампера
int check_SINS_state(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpioc;
	gpioc.Mode = GPIO_MODE_INPUT;
	gpioc.Pin = GPIO_PIN_9;
	gpioc.Pull = GPIO_NOPULL;
	gpioc.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB, &gpioc);


	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)) //пин с джампером
		return 1;
	else
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_IWDG_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_RTC_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  //	Global structures init
	memset(&stateSINS_isc, 			0x00, sizeof(stateSINS_isc));
	memset(&stateSINS_isc_prev, 	0x00, sizeof(stateSINS_isc_prev));
  	memset(&stateSINS_rsc, 			0x00, sizeof(stateSINS_rsc));
  	memset(&state_system,			0x00, sizeof(state_system));
  	memset(&state_zero,				0x00, sizeof(state_zero));
  	memset(&error_system, 			0x00, sizeof(error_system));

  	led_init();

  	dwt_init();

  	if (CALIBRATION_LSM)
  		calibration_accel();

  	if (CALIBRATION_LIS)
  		calibration_magn();



  	if (check_SINS_state() == 1)
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
  	else
  	{
  		iwdg_init(&transfer_uart_iwdg_handle);

  		time_svc_steady_init();

  		backup_sram_enable_after_reset();
  		backup_sram_read_zero_state(&state_zero);

  		backup_sram_read_reset_counter(&error_system.reset_counter);
  		error_system.reset_counter++;
  		backup_sram_write_reset_counter(&error_system.reset_counter);

  		int error = time_svc_world_preinit_with_rtc();
  		error_system.rtc_error = error;
  		if (error != 0)
  			time_svc_world_preinit_without_rtc(); 		//не смогли запустить rtc. Запустимся без него
  		else
  			time_svc_world_init();			//Смогли запустить rtc. Запустим все остальное

  		error = 0;
  		error = uplink_init();
  		error_system.uart_transfer_init_error = error;
  		if (error != 0)
  			system_reset();				//Если не запустился uart, то мы - кирпич

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

  	//	int rc = gps_init(_on_gps_packet, NULL);
  	//	trace_printf("configure rc = %d\n", rc);

  		error = 0;
  		error = analog_init();
  		error_system.analog_sensor_init_error = error;
  		if (error != 0)
  			{
  				HAL_Delay(500);
  				error_system.analog_sensor_init_error = analog_restart();
  			}

  		sensors_init();
  		error_mems_read();

  		time_svc_world_get_time(&stateSINS_isc_prev.tv);

  		error_system_check();

  		uint32_t prew_time = HAL_GetTick();
  		uint32_t time = 0;

  		for (; ; )
  		{
  			for (int u = 0; u < 5; u++)
  			{
  //				uint32_t x = HAL_GetTick();
  				for (int i = 0; i < 30; i++)
  				{

  					UpdateDataAll();
  					SINS_updatePrevData();
  				}

  //				volatile uint32_t z = HAL_GetTick() - x;

  		//		struct timeval tmv;
  		//		time_svc_world_timers_get_time(&tmv);
  		//		struct tm * tm = gmtime(&tmv.tv_sec);
  		//		char buffer[sizeof "2011-10-08T07:07:09Z"] = {0};
  		//		strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", tm);
  		//		trace_printf("time is %s\n", buffer);

  				gps_poll();

  				const int gps_cfg_status = gps_configure_status();
  				if (gps_cfg_status != -EWOULDBLOCK) // конфигурация уже закончилась
  				{
  					error_system.gps_config_error = gps_cfg_status;
  					uint32_t now = HAL_GetTick();

  					if (gps_cfg_status != 0)
  					{
  						// закончилась но плохо. Начинаем опять
  						error_system.gps_reconfig_counter++;
  						gps_configure_begin();
  					}
  					else if (now - last_gps_fix_packet_ts > ITS_SINS_GPS_MAX_NOFIX_TIME)
  					{
  						// Если GPS слишком долго не фиксится
  						// Тоже отправляем его в реконфигурациюs
  						// Перед этим сделаем вид, что фикс у нас был
  						// Так как он редко появляется сразу, а если это время не обновить -
  						// то эта ветка будет срабатывать постоянно
  						error_system.gps_reconfig_counter++;
  						last_gps_fix_packet_ts = HAL_GetTick();
  						gps_configure_begin();
  					}
  					else if (now - last_gps_packet_ts > ITS_SINS_GPS_MAX_NOPACKET_TIME)
  					{
  						// Если слишком давно не приходило интересных нам пакетов
  						// Отправляем gps в реконфигурацию
  						// Сбросим время,чтобы не крутитсяв этом цикле вечно, если что-то пошло не так
  						error_system.gps_reconfig_counter++;
  						last_gps_packet_ts = HAL_GetTick();
  						gps_configure_begin();
  					}
  				}

  				if ((error_system.lsm6ds3_error != 0) && (error_system.lis3mdl_error != 0))
  					continue;
  				mavlink_sins_isc(&stateSINS_isc);

  			}
  			time = HAL_GetTick();
  			if (time - prew_time < 1000)
  				continue;
  			prew_time = time;
  			mavlink_timestamp();
  			own_temp_packet();

  			error_mems_read();
  			mavlink_errors_packet();
  		}
  	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
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
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
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
  hadc1.Init.ScanConvMode = DISABLE;
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
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
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

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c2.Init.ClockSpeed = 400000;
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

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA10 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
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

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
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