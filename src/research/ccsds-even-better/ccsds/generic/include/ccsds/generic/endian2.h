#pragma once

#include <stdint.h>
#include "ccsds_endian.h"

typedef struct {
    uint8_t* ptr;
    size_t bit_start;
    size_t bit_end;
} bit_array_t;

 
void ccsds_insert(bit_array_t* ba, const void* var, size_t bitcount);
void ccsds_extract(bit_array_t* ba, void* var, size_t bitcount);