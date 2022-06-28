/*
 * operator.c
 *
 *  Created on: Aug 22, 2020
 *      Author: sereshotes
 */

#include <assert.h>
#include <string.h>

#include "operator2.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "router.h"
#include "log_collector.h"
#include "shift_reg.h"
#include "pinout_cfg.h"
#include "control_heat.h"

#define ROZE_COUNT 8

const static char *TAG = "OP2";

typedef enum {
	CS_OFF,
	CS_GOT_MSG,
	CS_ON_LOW,
	CS_ON_HIGH
} control_state_t;

typedef struct {
	uint16_t count_recieved_cmds;
	uint16_t count_executed_cmds;
	uint16_t count_errors;
	uint8_t last_executed_cmd_seq;
} op_stats_t;

typedef struct {
	uint32_t last_switch;
	uint32_t pulse_count;
	uint32_t period;
	float duty;

	uint32_t state;
	uint8_t seq;
} state_t;
typedef struct {
	state_t roze[ROZE_COUNT];
} op_state_t;


extern shift_reg_handler_t hsr;


const static int roze_pins[] = {
	ITS_PIN_SR_BSK1_CMD,
	ITS_PIN_SR_BSK2_CMD,
	ITS_PIN_SR_BSK3_CMD,
	ITS_PIN_SR_BSK4_CMD,
	ITS_PIN_SR_BSK1_ROZE,
	ITS_PIN_SR_BSK2_ROZE,
	ITS_PIN_SR_BSK3_ROZE,
	ITS_PIN_SR_BSK4_ROZE,
};


static void op_task(void *arg) {
	op_state_t op_state = {0};
	op_stats_t stats = {0};
	mavlink_message_t msg = {0};

	its_rt_task_identifier tid = {
			.name = "op2"
	};
	tid.queue = xQueueCreate(10, MAVLINK_MAX_PACKET_LEN);
	its_rt_register(MAVLINK_MSG_ID_ROZE_ACTIVATE_COMMAND, tid);
	its_rt_register(MAVLINK_MSG_ID_CMD_ACTIVATE_COMMAND, tid);
	its_rt_register(MAVLINK_MSG_ID_IDLE_COMMAND, tid);
	int heat_state = 1;
	while (1) {
		vTaskDelay(1);

		//-------------------------------------------------------------------------------
		// Читаем сообщения
		//-------------------------------------------------------------------------------
		while (xQueueReceive(tid.queue, &msg, 0) == pdTRUE) {
			if (msg.msgid == MAVLINK_MSG_ID_ROZE_ACTIVATE_COMMAND) {
				mavlink_roze_activate_command_t mrac = {0};
				mavlink_msg_roze_activate_command_decode(&msg, &mrac);
				op_state.roze[mrac.area_id - 1 + 4].state = CS_GOT_MSG;
				op_state.roze[mrac.area_id - 1 + 4].seq = msg.seq;
				op_state.roze[mrac.area_id - 1 + 4].pulse_count = mrac.pulse_count;
				op_state.roze[mrac.area_id - 1 + 4].duty = mrac.pulse_duty_cycle / (float)(0xFFFF);
				op_state.roze[mrac.area_id - 1 + 4].period = mrac.pulse_duration;

				ESP_LOGV(TAG, "%d:%06d: roze for BSK%d seq: %d pulse count: %d duty: %d period: %d", 
				         (int)mrac.time_s, 
						 mrac.time_us, 
						 mrac.area_id, 
						 msg.seq, 
						 mrac.pulse_count, 
						 mrac.pulse_duty_cycle,
						 mrac.pulse_duration);
				stats.count_recieved_cmds++;
			}
			if (msg.msgid == MAVLINK_MSG_ID_CMD_ACTIVATE_COMMAND) {
				mavlink_cmd_activate_command_t mcac = {0};
				mavlink_msg_cmd_activate_command_decode(&msg, &mcac);
				op_state.roze[mcac.area_id - 1].state = CS_GOT_MSG;
				op_state.roze[mcac.area_id - 1].seq = msg.seq;
				op_state.roze[mcac.area_id - 1].pulse_count = mcac.pulse_count;
				op_state.roze[mcac.area_id - 1].duty = mcac.pulse_duty_cycle / (float)(0xFFFF);
				op_state.roze[mcac.area_id - 1].period = mcac.pulse_duration;

				ESP_LOGV(TAG, "%d:%06d: cmd for BSK%d seq: %d pulse count: %d duty: %d period: %d", 
				         (int)mcac.time_s, 
						 mcac.time_us, 
						 mcac.area_id, 
						 msg.seq, 
						 mcac.pulse_count, 
						 mcac.pulse_duty_cycle,
						 mcac.pulse_duration);
				stats.count_recieved_cmds++;
			}
			if (msg.msgid == MAVLINK_MSG_ID_IDLE_COMMAND) {

				mavlink_idle_command_t mic = {0};
				mavlink_msg_idle_command_decode(&msg, &mic);
				ESP_LOGV(TAG, "%d:%06d: idle %d", (int)mic.time_s, mic.time_us, msg.seq);
				stats.count_recieved_cmds++;
				stats.count_executed_cmds++;
				stats.last_executed_cmd_seq = msg.seq;
				ESP_LOGI(TAG, "set %d", stats.last_executed_cmd_seq);
			}
		}

		//-------------------------------------------------------------------------------
		// Управление нагревателем
		//-------------------------------------------------------------------------------
		int heat_state_new = 1;
		for (int i = 0; i < ROZE_COUNT; i++) {
			if (op_state.roze[i].state != CS_OFF) {
				heat_state_new = 0;
				break;
			}
		}
		if (heat_state_new == 1 && heat_state == 0) {
			control_heat_resume();
		} else if (heat_state_new == 0 && heat_state == 1) {
			control_heat_suspend();
		}
		heat_state = heat_state_new;

		//-------------------------------------------------------------------------------
		// Управление розе
		//-------------------------------------------------------------------------------
		int64_t now = esp_timer_get_time() / 1000;
		int is_state_changed = 0;
		for (int i = 0; i < ROZE_COUNT; i++) {
			state_t* cur = &op_state.roze[i];
			uint32_t low_period = cur->period * (1 - cur->duty);
			uint32_t high_period = cur->period * cur->duty;
			if (cur->state == CS_GOT_MSG) {
				ESP_LOGI(TAG, "roze %d turn on", i);
				ESP_LOGI(TAG, "roze %d set %d", i, 1);
				cur->last_switch = now;
				cur->state = CS_ON_HIGH;
				is_state_changed = 1;
			}

			if (cur->state == CS_ON_HIGH && cur->last_switch + high_period <= now) {
				ESP_LOGI(TAG, "roze %d set %d", i, 0);
				cur->pulse_count--;
				cur->state = CS_ON_LOW;
				cur->last_switch = now;
				is_state_changed = 1;
			}

			if (cur->state == CS_ON_LOW && cur->last_switch + low_period <= now) {
				ESP_LOGI(TAG, "roze %d set %d",  i, 1);
				cur->state = CS_ON_HIGH;
				cur->last_switch = now;
				is_state_changed = 1;
			}

			if ((cur->state == CS_ON_HIGH || cur->state == CS_ON_LOW) && cur->pulse_count == 0) {
				ESP_LOGI(TAG, "roze %d set %d", i, 0);
				ESP_LOGI(TAG, "roze %d turn off", i);
				cur->state = CS_OFF;
				stats.count_executed_cmds++;
				stats.last_executed_cmd_seq = cur->seq;
				ESP_LOGI(TAG, "set %d", stats.last_executed_cmd_seq);
				is_state_changed = 1;
			}
		}

		if (is_state_changed) {
			shift_reg_take(&hsr, portMAX_DELAY);
			//Запишем на сдвиговые регистры новое состояние
			for (int i = 0; i < ROZE_COUNT; i++) {
				if (op_state.roze[i].state == CS_ON_HIGH) {
					shift_reg_set_level_pin(&hsr, roze_pins[i], 1);					
				} else {
					shift_reg_set_level_pin(&hsr, roze_pins[i], 0);
				}
			}
			esp_err_t rc = shift_reg_load(&hsr);
			shift_reg_return(&hsr);
		}


		//-------------------------------------------------------------------------------
		// Логи
		//-------------------------------------------------------------------------------

		if (log_collector_take(0) == pdTRUE) {
			mavlink_bcu_radio_conn_stats_t *conn_stats = log_collector_get_conn_stats();
			conn_stats->count_operator_cmds_executed = stats.count_executed_cmds;
			conn_stats->count_operator_cmds_recieved = stats.count_recieved_cmds;
			conn_stats->count_operator_errors = stats.count_errors;
			conn_stats->last_executed_cmd_seq = stats.last_executed_cmd_seq;
			//ESP_LOGI(TAG, "%d", conn_stats->last_executed_cmd_seq);
			conn_stats->update_time_operator = esp_timer_get_time() / 1000;
			log_collector_give();
		}
	}
}
void op2_init() {
	ESP_LOGV(TAG, "inited");
	xTaskCreatePinnedToCore(op_task, "OP2 task", configMINIMAL_STACK_SIZE + 3000, 0, 20, 0, tskNO_AFFINITY);
}
