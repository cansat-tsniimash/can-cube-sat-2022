#pragma once
#include "uslp_types.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

//TODO: COP
bool cop1_is_full(const cop1_t* cop) {
    return cop->is_full;
}
bool cop1_is_empty(const cop1_t* cop) {
    return cop->head == cop->tail && !cop->is_full;
}
vc_frame_t* cop1_new_frame(cop1_t* cop) {
    assert(!cop1_is_full(cop));
    vc_frame_t* frame = &cop->arr[cop->tail];
    cop->tail = (cop->tail + 1) % cop->size;
    if (cop->head == cop->tail) {
        cop->is_full = true;
    }
    return frame;
}
void cop1_pop(cop1_t* cop) {
    assert(!cop1_is_empty(cop));

    if (cop->is_full) {
        cop->is_full = false;
    }
    cop->head = (cop->head + 1) % cop->size;
}
bool cop1_is_there_next_frame(const cop1_t* cop) {
    return cop->cur != cop->tail;
}
vc_frame_t* cop1_get_next(cop1_t* cop) {
    assert(cop1_is_there_next_frame(cop));
    vc_frame_t* frame = &cop->arr[cop->cur];
    return frame;
}
void cop1_release_frame(cop1_t* cop) {
    cop->cur = (cop->cur + 1) % cop->size;
}


bool vc_push_new_frame(vc_t* vc, const map_data_t* md) {
    
    vc_frame_t* frame_to = 0;
    if (vc->cop_type == COP_NONE || md->qos == QOS_EXPEDITED) {
        assert(md->tfdf.size <= vc->container.expedited_frame.tfdz_size);
        if (vc->container.expedited_frame.is_full) {
            return false;
        }
        vc->container.expedited_frame.is_full = true;
        frame_to = &vc->container.expedited_frame.frame;

        frame_to->ttl = 0;
        frame_to->vc_data.vc_frame_count = vc->ex_frame_count++;
        frame_to->vc_data.vc_frame_count_length = vc->ex_frame_count_length;
    } else if (vc->cop_type == COP_1) {
        assert(md->tfdf.size <= vc->container.cop1.tfdz_max_size);
        frame_to = cop1_new_frame(&vc->container.cop1);
        if (frame_to == 0) {
            return false;
        }
        frame_to->ttl = vc->seq_ctrld_ttl;
        frame_to->vc_data.vc_frame_count = vc->sc_frame_count++;
        frame_to->vc_data.vc_frame_count_length = vc->sc_frame_count_length;
    } else {
        assert(0 && "Other cops are not implemented");//TODO
    }
    memcpy(frame_to->map_data.tfdf.tfdz, md->tfdf.tfdz, md->tfdf.size);
    uint8_t* tfdz = frame_to->map_data.tfdf.tfdz;
    frame_to->map_data = *md;
    frame_to->map_data.tfdf.tfdz = tfdz;
    
    frame_to->vc_data.contains_protocol_control_commands = 
        md->tfdf.upid == UPID_COP1 ||
        md->tfdf.upid == UPID_COPP ||
        md->tfdf.upid == UPID_SDLS; 
    //WARN: I have no idea about PROXIMITY packets, so some of them 
    //maybe should be included here
    frame_to->vc_data.frame_trancated = false;
    frame_to->vc_data.vc_id = vc->vc_id;
    if (vc->cop_type == COP_NONE) {
        frame_to->map_data.qos = QOS_EXPEDITED;
    }
    return true;
}

vc_frame_t* vc_get_frame(vc_t* vc) { 
    if (vc->container.expedited_frame.is_full) {
        return &vc->container.expedited_frame.frame;
    } else if (vc->cop_type == COP_1) {
        return cop1_get_next(&vc->container.cop1);
    } else {
        assert(0);
        abort();
    }
}
void vc_release_frame(vc_t* vc) {
    if (vc->container.expedited_frame.is_full) {
        vc->container.expedited_frame.is_full = false;
        //NOTE: Следующая строка в готовой версии будет не нужна
        memset(vc->container.expedited_frame.frame.map_data.tfdf.tfdz, 0, vc->container.expedited_frame.tfdz_size);
    } else if (vc->cop_type == COP_1) {
        cop1_release_frame(&vc->container.cop1);
    } else {
        assert(0);
        abort();
    }
}
