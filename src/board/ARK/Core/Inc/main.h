/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_i2c.h"
#include "stm32f1xx_ll_rtc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_dma.h"

#include "stm32f1xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define BATTERY_PACK_COUNT 2
#define BATTERY_COUNT_IN_PACK 2
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
int tmain(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BMS5_Pin GPIO_PIN_13
#define BMS5_GPIO_Port GPIOC
#define BMS6_Pin GPIO_PIN_14
#define BMS6_GPIO_Port GPIOC
#define BMS3_Pin GPIO_PIN_15
#define BMS3_GPIO_Port GPIOC
#define BMS4_Pin GPIO_PIN_0
#define BMS4_GPIO_Port GPIOA
#define BMS1_Pin GPIO_PIN_1
#define BMS1_GPIO_Port GPIOA
#define BMS2_Pin GPIO_PIN_2
#define BMS2_GPIO_Port GPIOA
#define TIME_Pin GPIO_PIN_3
#define TIME_GPIO_Port GPIOA
#define OW_Pin GPIO_PIN_5
#define OW_GPIO_Port GPIOA
#define ROZE1_Pin GPIO_PIN_7
#define ROZE1_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_0
#define LED_GPIO_Port GPIOB
#define ROZE4_Pin GPIO_PIN_1
#define ROZE4_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_10
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_11
#define SDA_GPIO_Port GPIOB
#define PGS_D_Pin GPIO_PIN_13
#define PGS_D_GPIO_Port GPIOB
#define PFI_D_Pin GPIO_PIN_14
#define PFI_D_GPIO_Port GPIOB
#define PGB_D_Pin GPIO_PIN_15
#define PGB_D_GPIO_Port GPIOB
#define INT_D_Pin GPIO_PIN_8
#define INT_D_GPIO_Port GPIOA
#define ROZE3_Pin GPIO_PIN_11
#define ROZE3_GPIO_Port GPIOA
#define DC_DC_Pin GPIO_PIN_15
#define DC_DC_GPIO_Port GPIOA
#define ROZE2_Pin GPIO_PIN_3
#define ROZE2_GPIO_Port GPIOB
#define INT_C_Pin GPIO_PIN_4
#define INT_C_GPIO_Port GPIOB
#define INT_C_EXTI_IRQn EXTI4_IRQn
#define PGS_C_Pin GPIO_PIN_5
#define PGS_C_GPIO_Port GPIOB
#define PFI_C_Pin GPIO_PIN_6
#define PFI_C_GPIO_Port GPIOB
#define PGB_C_Pin GPIO_PIN_7
#define PGB_C_GPIO_Port GPIOB
#define SCLTM_Pin GPIO_PIN_8
#define SCLTM_GPIO_Port GPIOB
#define SDATM_Pin GPIO_PIN_9
#define SDATM_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
