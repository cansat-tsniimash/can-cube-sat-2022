/*
 * type_candies.h
 *
 *  Created on: Jun 9, 2022
 *      Author: seres
 */

#ifndef INC_TYPE_CANDIES_H_
#define INC_TYPE_CANDIES_H_

#include "main.h"

#define CANDY_SIZE 256

typedef enum {
    candy_type_time,
} candy_type_t;

typedef struct {
    candy_type_t type;
    uint8_t candy[CANDY_SIZE];
} candy_in_wrapper_t;

typedef struct {
    uint64_t time_s; /*< [s] Time since the Unix epoch.*/
    uint32_t time_us; /*< [us] Millionth of a second*/
    uint32_t time_steady; /*< [ms] Steady board time*/
    uint8_t time_base; /*<  Time base*/
} candy_time_t;

#endif /* INC_TYPE_CANDIES_H_ */
