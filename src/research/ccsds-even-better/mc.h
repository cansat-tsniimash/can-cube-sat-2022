#pragma once

#include "uslp_types.h"


mc_data_t mc_get_data(mc_t* mc) {
    mc_data_t data = {0};
    data.frame_count = mc->frame_count;
    data.sc_id = mc->sc_id;
    data.tfvn = mc->tfvn;
    data.ocf_valid = true;
    data.ocf[0] = 0xAA;
    data.ocf[1] = 0xAA;
    data.ocf[2] = 0xFF;
    data.ocf[3] = 0xAA;

    return data;
}

void mc_pop_data(mc_t* mc) {
    mc->frame_count++;
}