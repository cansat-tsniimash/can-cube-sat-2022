#pragma once

#include "recieve.h"
#include "fifo.h"

 
typedef struct {
    struct {
        upid_t upid;
        int map_id;
        int pvn;
    } mapr;
    cop_enum_t cop_type;
    fifo_t fifo;
} vc_receive_t;

bool farm_1_process_frame(const map_data_t* md, const vc_data_t* vd) {
    //TODO
    return true;
}

int get_pvn(uint8_t* data, size_t size) {

}

bool map_receive_octet(vc_receive_t* vcr, const map_data_t* md) {
    
    return true;
}

bool map_receive_mapp(vc_receive_t* vcr, const map_data_t* md) {
    
    return true;
}

bool map_receive_mapa(vc_receive_t* vcr, const map_data_t* md) {
    size_t size = md->tfdf.size;
    if (md->tfdf.pointer_fh_lo != 0xFFFF) {
        size = md->tfdf.pointer_fh_lo;
    }
    if (fifo_free_space(&vcr->fifo) < size + sizeof(size)) {
        return false;
    }
    uint8_t* arr = (uint8_t*)&size;
    fifo_push_arr(&vcr->fifo, arr, sizeof(size));
    fifo_push_arr(&vcr->fifo, md->tfdf.tfdz, size);
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
    
    bool map_received = false;
    if (md->tfdf.upid == vcr->mapr.upid && md->tfdf.map_id == vcr->mapr.map_id) {
        if (md->tfdf.upid == UPID_SP_OR_EP) {
            map_received = map_receive_mapp(vcr, md); 
        } else if (md->tfdf.upid == UPID_OCTET) {
            map_received = map_receive_octet(vcr, md); 
        } else if (md->tfdf.upid == UPID_MAPA) {
            map_received = map_receive_mapa(vcr, md); 
        }
    }
    
    if (!map_received) {
        //TODO: Send warning about unreceived map data
    }
    
}