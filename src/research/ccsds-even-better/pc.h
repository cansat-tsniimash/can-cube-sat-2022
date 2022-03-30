#pragma once

#include "uslp_types.h"



pc_data_t pc_get_data(pc_t* pc) {
    pc_data_t data = {0};
    data.fec_valid = true;
    data.fec[0] = 'Y';
    data.fec[1] = 'A';
    data.fec[2] = 'S';
    data.fec[3] = 'S';

    data.insert_data = pc->insert_data;
    data.insert_size = pc->insert_size;
    return data;
}

void pc_pop_data(pc_t* pc) {
    memset(pc->insert_data, 0, pc->insert_size);
    pc->frame_count++;
}