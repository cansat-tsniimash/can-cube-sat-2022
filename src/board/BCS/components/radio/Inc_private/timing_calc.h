#ifndef COMPONENTS_RADIO_INC_PRIVATE_TIMING_CALC_H_
#define COMPONENTS_RADIO_INC_PRIVATE_TIMING_CALC_H_

#include "radio_help.h"



void timing_calc_finish_all_tx(radio_t* server, radio_timings_t* state);

void timing_calc_finish_one_tx(radio_t* server, radio_timings_t* state);

void timing_calc_init(radio_t* server, radio_timings_t* state);

#endif /* COMPONENTS_RADIO_INC_PRIVATE_TIMING_CALC_H_ */