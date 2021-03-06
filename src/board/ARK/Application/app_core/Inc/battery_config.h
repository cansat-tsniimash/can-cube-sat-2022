/*
 * battery_config.h
 *
 *  Created on: 15 мар. 2020 г.
 *      Author: sereshotes
 */

#ifndef INC_BATTERY_CONFIG_H_
#define INC_BATTERY_CONFIG_H_


#define BATTERY_IN_ROW 2
#define BATTERY_MAX_ROW_COUNT 6
#define BATTERY_DEAD_TIMEOUT 60000 //ms
#define BATTERY_THD_NORMAL_TO_OVERHEATED 40
#define BATTERY_THD_OVERHEATED_TO_NORMAL 35
#define BATTERY_BASE_PORT GPIOA
#define BATTERY_BASE_PIN 1
#define BATTERY_INA_BUS 0
#define BATTERY_INA_BAT 1


#define BATTERY_CNT_BAT_IN_ROW 2
#define BATTERY_CNT_ROW_IN_PACK 2 //Max 6

#define BATTERY_TEMP_BOT_THD 40.0
#define BATTERY_TEMP_TOP_THD 45.0


#define BATTERY_DPD_BAT_MIN 5.8
#define BATTERY_DPD_BAT_MAX 8.56

#define BATTERY_DPD_BUS_MIN 6.0
#define BATTERY_DPD_BUS_MAX 6.2

#define BATTERY_DPD_BUS_TOP_THRESHOLD 8.0
#define BATTERY_CHARGE_BAT_VOLTAGE 8.0

//#define BATTERY_CHARGE_LOW_THD 6.0
//#define BATTERY_CHARGE_TOP_THD 8.0

#endif /* INC_BATTERY_CONFIG_H_ */
