/*
 * MIT License
 *
 * Copyright (c) 2017 David Antliff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "sensors.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"

#include "pinout_cfg.h"
#include "router.h"
#include "mavlink_help2.h"

#include "ds18b20.h"
#include "ina219.h"

//#define SENSORS_ONEWIRE_SEARCH

static void sensors_ina_task(void *arg);
static void sensors_task(void *arg);
#define SAMPLE_PERIOD 1000
#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)
static char *TAG = "SENSORS";

esp_err_t sensors_init(void) {
	xTaskCreatePinnedToCore(sensors_task, "Sensors task", configMINIMAL_STACK_SIZE + 3000, 0, 1, 0, tskNO_AFFINITY);
	xTaskCreatePinnedToCore(sensors_ina_task, "Sensors ina task", configMINIMAL_STACK_SIZE + 3000, 0, 2, 0, tskNO_AFFINITY);
	return 0;
}

//#define USE_GPIO
static void sensors_task(void *arg) {
	OneWireBus * owb = 0;
#ifdef USE_GPIO
	owb_gpio_driver_info gpio_driver_info = {0};
	vTaskDelay(2000/portTICK_PERIOD_MS);
	owb = owb_gpio_initialize(&gpio_driver_info, ITS_PIN_OWB);
	vTaskDelay(2000/portTICK_PERIOD_MS);
#else
	owb_rmt_driver_info rmt_driver_info = {0};
	owb = owb_rmt_initialize(&rmt_driver_info, ITS_PIN_OWB, RMT_CHANNEL_1, RMT_CHANNEL_0);
#endif
	owb_use_crc(owb, true);  // enable CRC check for ROM code

#ifndef SENSORS_ONEWIRE_SEARCH
	// ???????? ?????? ???? ??????????

	const OneWireBus_ROMCode device_rom_codes[ITS_OWB_MAX_DEVICES] = {

			{ .bytes = { 0x28, 0xC3, 0xA1, 0xE6, 0x0B, 0x00, 0x00, 0xA7 } }, // ??????   +
			{ .bytes = { 0x28, 0x24, 0xDF, 0xAB, 0x0A, 0x00, 0x00, 0x12 } }, // ?????? 1 +
			{ .bytes = { 0x28, 0x1B, 0xB1, 0xAB, 0x0A, 0x00, 0x00, 0x68 } }, // ?????? 2 +
			{ .bytes = { 0x28, 0x13, 0x4D, 0xAC, 0x0A, 0x00, 0x00, 0xC8 } }, // ?????? 3 +
			{ .bytes = { 0x28, 0x2F, 0x68, 0xAC, 0x0A, 0x00, 0x00, 0x72 } }, // ?????? 4 +
			{ .bytes = { 0x28, 0x36, 0x86, 0xE6, 0x0B, 0x00, 0x00, 0x97 } }  // ?????? 5 +


	};

	const int num_devices = 6;
#else // if defined SENSORS_ONEWIRE_SEARCH
	OneWireBus_ROMCode device_rom_codes[ITS_OWB_MAX_DEVICES];
	ESP_LOGI(TAG, "Find devices:");

	int num_devices = 0;

	OneWireBus_SearchState search_state = {0};
	bool found = false;
	owb_search_first(owb, &search_state, &found);
	while (found)
	{
		char rom_code_s[17];
		owb_string_from_rom_code(search_state.rom_code, rom_code_s, sizeof(rom_code_s));
		uint8_t *p = search_state.rom_code.bytes;
		ESP_LOGI(TAG, "  %d : %s : 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
				num_devices, rom_code_s, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
		device_rom_codes[num_devices] = search_state.rom_code;
		++num_devices;
		owb_search_next(owb, &search_state, &found);
	}
	ESP_LOGI(TAG, "Found %d device%s", num_devices, num_devices == 1 ? "" : "s");

#endif

	for (int i = 0; i < num_devices; i++)
	{
		const OneWireBus_ROMCode * rom_code = &device_rom_codes[i];
		char rom_code_s[OWB_ROM_CODE_STRING_LENGTH] = {0};
		owb_string_from_rom_code(*rom_code, rom_code_s, sizeof(rom_code_s));

		ESP_LOGI(TAG, "checking ds18b20 number %d", i);

		bool is_present;
		owb_status search_status = owb_verify_rom(owb, *rom_code, &is_present);
		if (search_status == OWB_STATUS_OK)
		{
			ESP_LOGI(TAG, "Device %s is %s", rom_code_s, is_present ? "present" : "not present");
		}
		else
		{
			ESP_LOGE(TAG, "An error occurred searching for known device: %d", search_status);
		}
	}

	ESP_LOGV(TAG, "Creating devices: ");
	// Create DS18B20 devices on the 1-Wire bus
	DS18B20_Info * devices[ITS_OWB_MAX_DEVICES] = {0};
	for (int i = 0; i < num_devices; ++i)
	{
		DS18B20_Info * ds18b20_info = ds18b20_malloc();  // heap allocation
		ESP_LOGV(TAG, "%d, %d", i, ds18b20_info != 0);
		devices[i] = ds18b20_info;
		ESP_LOGV(TAG, "c1");

		ds18b20_init(ds18b20_info, owb, device_rom_codes[i]); // associate with bus and device
		ESP_LOGV(TAG, "c2");
		ds18b20_use_crc(ds18b20_info, true);           // enable CRC check on all reads
		ESP_LOGV(TAG, "c3");
		ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION);
		ESP_LOGV(TAG, "c4");
	}

	// Check for parasitic-powered devices
	bool parasitic_power = false;
	ds18b20_check_for_parasite_power(owb, &parasitic_power);
	if (parasitic_power) {
		ESP_LOGI(TAG, "Parasitic-powered devices detected");
	}

	// In parasitic-power mode, devices cannot indicate when conversions are complete,
	// so waiting for a temperature conversion must be done by waiting a prescribed duration
	owb_use_parasitic_power(owb, 0);

#ifdef CONFIG_ENABLE_STRONG_PULLUP_GPIO
	// An external pull-up circuit is used to supply extra current to OneWireBus devices
	// during temperature conversions.
	owb_use_strong_pullup_gpio(owb, CONFIG_STRONG_PULLUP_GPIO);
#endif

	// Read temperatures more efficiently by starting conversions on all devices at the same time
	//int errors_count[ITS_OWB_MAX_DEVICES] = {0};

	ESP_LOGD(TAG, "Start cycle");

	ESP_LOGV(TAG, "Main cycle");
	if (num_devices > 0)
	{
		TickType_t last_wake_time = xTaskGetTickCount();

		while (1)
		{
			ESP_LOGV(TAG, "step");
			last_wake_time = xTaskGetTickCount();

			ds18b20_convert_all(owb);
			ESP_LOGV(TAG, "converted");

			// In this application all devices use the same resolution,
			// so use the first device to determine the delay
			ds18b20_wait_for_conversion(devices[0]);
			ESP_LOGV(TAG, "waited");
			struct timeval tv = {0};
			gettimeofday(&tv, 0);

			// Read the results immediately after conversion otherwise it may fail
			// (using printf before reading may take too long)
			float readings[ITS_OWB_MAX_DEVICES] = { 0 };
			DS18B20_ERROR errors[ITS_OWB_MAX_DEVICES] = { 0 };

			for (int i = 0; i < num_devices; ++i)
			{
				errors[i] = ds18b20_read_temp(devices[i], &readings[i]);
				taskYIELD();
			}
			ESP_LOGV(TAG, "read");
			for (int i = 0; i < num_devices; i++) {
				ESP_LOGI(TAG, "@ds  [%d] temp: %0.2f", i, readings[i]);
			}

			for (int i = 0; i < num_devices; ++i) {
				mavlink_thermal_state_t mts = {0};
				mavlink_message_t msg = {0};
				mts.time_s = tv.tv_sec;
				mts.time_us = tv.tv_usec;
				mts.time_steady = esp_timer_get_time() / 1000;
				mts.temperature = errors[i] == DS18B20_OK ? readings[i] : NAN;
				mavlink_msg_thermal_state_encode(mavlink_system, i, &msg, &mts);
				its_rt_sender_ctx_t ctx = {0};

				ctx.from_isr = 0;
				its_rt_route(&ctx, &msg, 0);
			}

			vTaskDelayUntil(&last_wake_time, SAMPLE_PERIOD / portTICK_PERIOD_MS);
		}
	}
	else
	{
		ESP_LOGE(TAG, "No DS18B20 devices detected!");
	}

	// clean up dynamically allocated data
	for (int i = 0; i < num_devices; ++i)
	{
		ds18b20_free(&devices[i]);
	}
	owb_uninitialize(owb);

	vTaskDelete(0);
}

static void _ina_set_cfg(ina219_t *ina, const ina219_cfg_t *cfg, const char *name) {
	int rc;
	rc = ina219_set_cfg(ina, cfg);
	ESP_LOGD(TAG, "ina %s set: %d",name, rc);
	ina219_cfg_t cfg2 = {0};
	rc = ina219_get_cfg(ina, &cfg2);
	ESP_LOGD(TAG, "ina %s get: %d", name, rc);
}


static void sensors_ina_task(void *arg) {
#define INA_MAX 4
#define INA_VOLTAGE_BUS_LSB     0.004
	ina219_t ina[INA_MAX];
	int timeout = 10 / portTICK_PERIOD_MS;

	ina219_init(&ina[0], ITS_I2CTM_PORT, INA219_I2CADDR_A1_GND_A0_VSP, timeout); //BSK1
	ina219_init(&ina[1], ITS_I2CTM_PORT, INA219_I2CADDR_A1_GND_A0_SDA, timeout); //BSK2
	ina219_init(&ina[2], ITS_I2CTM_PORT, INA219_I2CADDR_A1_GND_A0_SCL, timeout); //BSK3
	ina219_init(&ina[3], ITS_I2CTM_PORT, INA219_I2CADDR_A1_VSP_A0_GND, timeout); //BSK4

	for (int i = 0; i < INA_MAX; i++) {
		ina219_sw_reset(&ina[i]);
	}
	vTaskDelay(timeout);
	int rc = 0;
	ina219_cfg_t cfg = {0};
	cfg.bus_range = INA219_BUS_RANGE_16V;
	cfg.bus_res = INA219_ADC_RES_12_BIT_OVS_128;
	cfg.shunt_range = INA219_SHUNT_RANGE_320MV;
	cfg.shunt_res = INA219_ADC_RES_12_BIT_OVS_128;
	cfg.mode = INA219_MODE_SHUNT_AND_BUS_CONT;
	cfg.current_lsb =  ((ina219_float_t)(30.0/0x8000));
	cfg.shunt_r = 0.1;

	_ina_set_cfg(&ina[0], &cfg, "0");
	cfg.shunt_r = 0.1;
	_ina_set_cfg(&ina[1], &cfg, "1");
	_ina_set_cfg(&ina[2], &cfg, "2");
	_ina_set_cfg(&ina[3], &cfg, "3");



	while (1) {
		struct timeval tp = {0};
		gettimeofday(&tp, 0);
		mavlink_electrical_state_t mes[INA_MAX];
		int64_t now = esp_timer_get_time();
		for (int i = 0; i < INA_MAX; i++) {
			ESP_LOGV(TAG, "@ina reading %d", i);
			ina219_data_t data = {0};
			if (ina219_read_all_data(&ina[i], &data)) {
				mes[i].current = NAN;
				mes[i].voltage = NAN;
			} else {
				mes[i].current = data.current;
				mes[i].voltage = data.busv;
			}
			ESP_LOGD(TAG, "ina raw: %f", data.shuntv);
			ESP_LOGV(TAG, "@ina fin reading %d", i);

			mes[i].time_s = tp.tv_sec;
			mes[i].time_us = tp.tv_usec;
			mes[i].time_steady = now / 1000;
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
		for (int i = 0; i < INA_MAX; i++) {
			ESP_LOGI("SENSORS", "@ina [%d] current: %f, voltage: %0.7f", i, mes[i].current, mes[i].voltage);
		}
		for (int i = 0; i < INA_MAX; i++) {
			mavlink_message_t msg;

			mavlink_msg_electrical_state_encode(CUBE_1_BCU, i, &msg, &mes[i]);

			its_rt_sender_ctx_t ctx = {0};
			ctx.from_isr = 0;
			its_rt_route(&ctx, &msg, 0);
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
