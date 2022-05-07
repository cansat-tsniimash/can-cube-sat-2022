#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t* data;
    size_t capacity;
    size_t size;
    size_t index;
} buffer_t;

bool buffer_is_empty(const buffer_t* buffer);
bool buffer_is_full(const buffer_t* buffer);
void buffer_clear(buffer_t* buffer);
void buffer_init(buffer_t* buffer, uint8_t* arr, size_t size);
bool buffer_try_copy(buffer_t* buffer, uint8_t const* arr, size_t size);