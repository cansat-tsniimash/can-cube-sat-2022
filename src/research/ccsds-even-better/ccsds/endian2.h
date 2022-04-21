#pragma once

#include <stdint.h>
#include "ccsds_endian.h"

typedef struct {
    uint8_t* ptr;
    size_t bit_start;
    size_t bit_end;
} bit_array_t;

 
void ccsds_insert(bit_array_t* ba, const void* var, size_t bitcount) {
    ccsds_endian_insert(ba->ptr, ba->bit_end, ba->bit_start, var, bitcount);
    ba->bit_start += bitcount;
}
void ccsds_extract(bit_array_t* ba, void* var, size_t bitcount) {
    ccsds_endian_extract(ba->ptr, ba->bit_start, var, bitcount);
    ba->bit_start += bitcount;
}