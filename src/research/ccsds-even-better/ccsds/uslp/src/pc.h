#pragma once

#include "uslp_types.h"



pc_data_t pc_get_data(pc_t* pc);

void pc_pop_data(pc_t* pc);

uint16_t calc_crc(uint8_t* data, size_t size);

