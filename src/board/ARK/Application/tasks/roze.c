/*
 * roze.c
 *
 *  Created on: Jun 25, 2022
 *      Author: seres
 */

#include "roze.h"

#define PERIOD 100

void task_roze_init(void* arg) {

}

static uint32_t last_time = 0;

void task_roze_update(void* arg) {
	uint32_t now = HAL_GetTick();
	if (now - last_time < PERIOD) {
		return;
	}
	last_time = now;
	int roze[4] = {0};
	roze[0] = HAL_GPIO_ReadPin(ROZE1_GPIO_Port, ROZE1_Pin);
	roze[1] = HAL_GPIO_ReadPin(ROZE2_GPIO_Port, ROZE2_Pin);
	roze[2] = HAL_GPIO_ReadPin(ROZE3_GPIO_Port, ROZE3_Pin);
	roze[3] = HAL_GPIO_ReadPin(ROZE4_GPIO_Port, ROZE4_Pin);
//	printf("ROZE: ");
//	for (int i = 0; i < 4; i++) {
//		printf("[%d]: %d ", i, roze[i]);
//	}
//	printf("\n");
	its_time_t here = {0};
	its_gettimeofday(&here);


	own_temp_t ot = {0};

	ot.time_s = here.sec;
    ot.time_us = here.msec * 1000;
    ot.time_steady = HAL_GetTick();
	ot.vdda = vdda/1000;
	ot.deg = temp_c;

	uint8_t buf[MAVLINK_HELP_DATA_SIZE];

	mavlink_help_own_temp_serialize(buf, sizeof(buf), &ot);
	uplink_packet2(buf, sizeof(buf));

}
