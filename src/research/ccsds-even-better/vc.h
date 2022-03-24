#pragma once
#include "uslp_types.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

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


void vc_save_frame(vc_t* vc, const vc_frame_t* frame) {
    vc_frame_t* frame_to = 0;
    if (vc->cop_type == COP_NONE) {
        assert(frame->tfdf.size <= vc->container.cop_none.tfdz_size);
        frame_to = &vc->container.cop_none.frame;
        vc->container.cop_none.is_full = true;
    } else if (vc->cop_type == COP_1) {
        assert(frame->tfdf.size <= vc->container.cop1.tfdz_max_size);
        frame_to = cop1_new_frame(&vc->container.cop1);
    } else {
        assert(0);
    }
    memcpy(frame_to->tfdf.tfdz, frame->tfdf.tfdz, frame->tfdf.size);
    uint8_t* tfdz = frame_to->tfdf.tfdz;
    *frame_to = *frame;
    frame_to->tfdf.tfdz = tfdz;
}

vc_frame_t vc_frame_from_tfdf(vc_t* vc, const tfdf_t* tfdf) {
    vc_frame_t frame = {0};
    frame.tfdf = *tfdf;
    
    frame.vc_id = vc->vc_id;
    frame.vc_frame_count = vc->frame_count;
    
    return frame;
}


bool vc_is_full(vc_t* vc) {
    if (vc->cop_type == COP_NONE) {
        return vc->container.cop_none.is_full;
    } else if (vc->cop_type == COP_1) {
        return cop1_is_full(&vc->container.cop1);
    } else {
        assert(0);
        return true;
    }
}

vc_frame_t* vc_get_frame(vc_t* vc) { 
    if (vc->cop_type == COP_NONE) {
        assert(vc->container.cop_none.is_full);
        return &vc->container.cop_none.frame;
    } else if (vc->cop_type == COP_1) {
        return cop1_get_next(&vc->container.cop1);
    } else {
        assert(0);
        return 0;
    }
}
void vc_release_frame(vc_t* vc) {
    if (vc->cop_type == COP_NONE) {
        vc->container.cop_none.is_full = false;
        memset(vc->container.cop_none.frame.tfdf.tfdz, 0, vc->container.cop_none.tfdz_size);
    } else if (vc->cop_type == COP_1) {
        cop1_release_frame(&vc->container.cop1);
    } else {
        assert(0);
    }
}
