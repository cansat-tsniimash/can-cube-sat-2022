#include "mc.h"
mc_data_t mc_get_data(mc_t* mc) {
    mc_data_t data = {0};
    data.frame_count = mc->frame_count;
    data.sc_id_is_destination = mc->sc_id_is_destination;
    data.sc_id = mc->sc_id;
    data.ocf_valid = mc->ocf_is_used;
    data.ocf[0] = 0xAB;
    data.ocf[1] = 0xBA;
    data.ocf[2] = 0xFF;
    data.ocf[3] = 0xAA;

    return data;
}

void mc_pop_data(mc_t* mc) {
    mc->frame_count++;
}
