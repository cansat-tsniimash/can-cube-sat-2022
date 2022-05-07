#pragma once

#include "receive.h"
#include "ccsds/generic/fifo.h"



void vc_receive_cb(void* arg, const map_data_t* md, const vc_data_t* vd);