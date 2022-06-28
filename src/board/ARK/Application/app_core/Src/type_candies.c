/*
 * type_candies.c
 *
 *  Created on: Jun 9, 2022
 *      Author: seres
 */


#include "type_candies.h"

typedef struct {
    uint8_t* ptr;
    uint8_t* start;
    size_t size;
} raw_data_t;

void raw_data_start(raw_data_t* raw, uint8_t* data, size_t size) {
    raw->ptr = data;
    raw->size = size;
    raw->start = data;
}
void raw_data_retrieve(raw_data_t* raw, uint8_t* to, size_t size) {
    assert(size <= raw->size - (raw->ptr - raw->start));
    memcpy(to, raw->ptr, size);
    raw->ptr += size;
}
void raw_data_place(raw_data_t* raw, const uint8_t* from, size_t size) {
    assert(size <= raw->size - (raw->ptr - raw->start));
    memcpy(raw->ptr, from, size);
    raw->ptr += size;
}

#define raw_get(raw, var) raw_data_retrieve(&raw, (uint8_t*)&var, sizeof(var))
#define raw_set(ptr, var) raw_data_place(&raw, (const uint8_t*)&var, sizeof(var))
#define raw_used(raw) (raw->ptr - raw->start)

void candy_time_unwrap(const candy_in_wrapper_t* wrapped, candy_time_t* unwrapped) {
    raw_data_t raw;
    raw_data_start(&raw, wrapped->candy, sizeof(wrapped->candy));
    raw_get(raw, unwrapped->time_base);
    raw_get(raw, unwrapped->time_s);
    raw_get(raw, unwrapped->time_steady);
    raw_get(raw, unwrapped->time_us);
}

void candy_time_wrap(const candy_in_wrapper_t* wrapped, candy_time_t* unwrapped) {
    raw_data_t raw;
    raw_data_start(&raw, wrapped->candy, sizeof(wrapped->candy));
    raw_set(raw, unwrapped->time_base);
    raw_set(raw, unwrapped->time_s);
    raw_set(raw, unwrapped->time_steady);
    raw_set(raw, unwrapped->time_us);

}
