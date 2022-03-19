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
frame_t* cop1_new_frame(cop1_t* cop) {
    assert(!cop1_is_full(cop));
    frame_t* frame = &cop->arr[cop->tail];
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
frame_t* cop1_get_next(cop1_t* cop) {
    assert(cop1_is_there_next_frame(cop));
    frame_t* frame = &cop->arr[cop->cur];
    return frame;
}
void cop1_release_frame(cop1_t* cop) {
    cop->cur = (cop->cur + 1) % cop->size;
}


void vc_save_frame(vc_t* vc, const frame_t* frame) {
    frame_t* frame_to = 0;
    if (vc->cop_type == COP_NONE) {
        assert(frame->tfdz_size <= vc->container.cop_none.tfdz_size);
        frame_to = &vc->container.cop_none.frame;
        vc->container.cop_none.is_full = true;
    } else if (vc->cop_type == COP_1) {
        assert(frame->tfdz_size <= vc->container.cop1.tfdz_max_size);
        frame_to = cop1_new_frame(&vc->container.cop1);
    } else {
        assert(0);
    }
    memcpy(frame_to->tfdz, frame->tfdz, frame->tfdz_size);
    frame_to->map_frame_count = frame->map_frame_count;
    frame_to->map_id = frame->map_id;
    frame_to->tfdz_size = frame->tfdz_size;
}

frame_t vc_frame_from_tfdf(vc_t* vc, const tfdf_t* tfdf) {
    frame_t frame = {0};
    frame.map_id = tfdf->map_id;
    frame.map_frame_count = tfdf->map_frame_count;
    frame.tfdz = tfdf->tfdz;
    frame.tfdz_size = tfdf->size;
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

frame_t* vc_get_frame(vc_t* vc) { 
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
    } else if (vc->cop_type == COP_1) {
        cop1_release_frame(&vc->container.cop1);
    } else {
        assert(0);
    }
}
