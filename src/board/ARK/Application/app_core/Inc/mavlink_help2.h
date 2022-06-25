/*
 * mavlink_help2.h
 *
 *  Created on: Jul 16, 2020
 *      Author: sereshotes
 */

#ifndef INC_MAVLINK_HELP2_H_
#define INC_MAVLINK_HELP2_H_


#define MAVLINK_HELP_DATA_SIZE 290

#include <stdint.h>
#include <stddef.h>

typedef enum {
    mavlink_help_type_timestamp
} mavlink_help_type_t;



typedef struct {
    uint64_t time_s; /*< [s] Time since the Unix epoch.*/
    uint32_t time_us; /*< [us] Millionth of a second*/
    uint32_t time_steady; /*< [ms] Steady board time*/
    uint8_t time_base; /*<  Time base*/
} timestamp_data_t;

typedef struct {
    uint64_t time_s; /*< [s] Time since the Unix epoch*/
    uint32_t time_us; /*< [us] Millionth of a second*/
    uint32_t time_steady; /*< [ms] Steady board time*/
    float deg; /*<  Temperature of a computer*/
    float vdda; /*<  Voltage over VDDA input of computer*/
    float vbat; /*<  Voltage at VBAT input of computer*/
} own_temp_t;

int mavlink_help_parse_packet(const uint8_t* data, size_t size, mavlink_help_type_t* type);

void mavlink_help_timestamp_parse(const uint8_t* data, size_t size, timestamp_data_t* ts);

void mavlink_help_own_temp_serialize(uint8_t* data, size_t size, const own_temp_t* ts);

void mavlink_help_timestamp_parse(const uint8_t* data, size_t size, timestamp_data_t* ts);

#endif /* INC_MAVLINK_HELP2_H_ */
