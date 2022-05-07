#pragma once
#include "uslp_types.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

bool cop1_is_full(const cop1_t* cop);
bool cop1_is_empty(const cop1_t* cop);
vc_frame_t* cop1_new_frame(cop1_t* cop);
void cop1_pop(cop1_t* cop);
bool cop1_is_there_next_frame(const cop1_t* cop);
vc_frame_t* cop1_get_next(cop1_t* cop);
void cop1_release_frame(cop1_t* cop);


bool vc_push_new_frame(vc_t* vc, const map_data_t* md);
void vc_pull_everything_from_bottom(vc_t* vc);

vc_frame_t* vc_get_frame(vc_t* vc);
void vc_release_frame(vc_t* vc);
