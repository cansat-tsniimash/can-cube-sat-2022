#pragma once

#include "recieve.h"

typedef struct {
    upid_t upid;
    int map_id;
    int pvn;
} map_receive_t;
 
typedef struct {
    cop_enum_t cop_type;
    map_receive_t* mapr_list;
    size_t mapr_count;
    size_t mapr_list_size;
    buffer_t buffer;
    bool is_buffer_finished;
} vc_receive_t;

bool farm_1_process_frame(const map_data_t* md, const vc_data_t* vd) {
    //TODO
    return true;
}

int get_pvn(uint8_t* data, size_t size) {

}

void map_receive_octet(vc_receive_t* vcr, const map_data_t* md) {
    
}

void map_receive_mapp(vc_receive_t* vcr, const map_data_t* md) {
    
}

bool map_receive_mapa(vc_receive_t* vcr, const map_data_t* md) {
    size_t size = md->tfdf.size;
    if (md->tfdf.pointer_fh_lo != 0xFFFF) {
        size = md->tfdf.pointer_fh_lo;
        vcr->is_buffer_finished = true;
    }
    if (vcr->buffer.capacity - vcr->buffer.size < size) {
        return false;
    }
    memcpy(vcr->buffer.data + vcr->buffer.size, md->tfdf.tfdz, size);
    vcr->buffer.size += size;
    return true;
}

void vc_receive_cb(void* arg, const map_data_t* md, const vc_data_t* vd) {
    vc_receive_t* vcr = (vc_receive_t*)arg;
    if (vcr->is_buffer_finished) {
        //TODO: Send overflow warning
        return;
    }

    if (md->qos == QOS_SEQ_CTRLD) {
        if (vcr->cop_type == COP_NONE) {
            //TODO: Send warning to user about receiving sequence controlled
            //frame and not having FARM for accepting it;
            return;
        }
        if (!farm_1_process_frame(md, vd)) {
            //TODO: Send FARM message to user
            return;
        }
    } else {
        //TODO: Process count of frames and send warning about
        //lost frames
    }
    //NOTE: What to about PCC packets?

    if (md->tfdf.upid == UPID_IDLE) {
        return;
    }
    if (md->tfdf.upid == UPID_PROX1_PPID1 || 
        md->tfdf.upid == UPID_PROX1_PPID2 || 
        md->tfdf.upid == UPID_PROX1_SPDU || 
        md->tfdf.upid == UPID_COP1 || 
        md->tfdf.upid == UPID_COPP || 
        md->tfdf.upid == UPID_SDLS) {
        // Unsupported upids
        return;
    }
    
    bool test = false;
    for (size_t i = 0; i < vcr->mapr_count; i++) {
        map_receive_t* cur = &vcr->mapr_list[i];
        if (md->tfdf.upid == cur->upid) {
            if (cur->upid == UPID_SP_OR_EP && 
                cur->pvn == get_pvn(md->tfdf.tfdz, md->tfdf.size)) {
                map_receive_mapp(vcr, md); 
                test = true;
            } else if (cur->upid == UPID_OCTET) {
                map_receive_octet(vcr, md); 
                test = true;
            } else if (cur->upid == UPID_MAPA) {
                map_receive_mapa(vcr, md); 
                test = true;
            }
        }
    }
    if (!test) {
        //TODO: Send warning about unreceived map data
    }
    
}