#pragma once

#include "uslp_types.h"

void map_force_finish(map_t* map);

void map_set_source(map_t* map, uint8_t* data, size_t size);

bool map_pull_data(map_t* map, map_data_t* md, bool* release_sap, bool* release_map);

void map_clear_tfdf(map_t* map);
void map_clear_source(map_t* map);

