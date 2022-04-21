#pragma once

#include "uslp_types.h"



pc_data_t pc_get_data(pc_t* pc) {
    pc_data_t data = {0};
    data.use_fec = pc->is_fec_presented;

    data.tfvn = pc->tfvn;
    data.insert_data = pc->insert_data;
    data.insert_size = pc->insert_size;
    return data;
}

void pc_pop_data(pc_t* pc) {
    memset(pc->insert_data, 0, pc->insert_size);
    pc->frame_count++;
}

uint16_t calc_crc(uint8_t* data, size_t size) {
    return 0xFADE; //TODO
}

