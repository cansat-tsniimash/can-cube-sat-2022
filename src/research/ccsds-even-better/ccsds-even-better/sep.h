#pragma once

#include "uslp_types.h"
#include "vc.h"
#include "uslp_mx.h"
#include "map.h"

typedef struct {
    uslp_core_t* uslp;
    pc_t* pc;
    struct {
        mc_t* mc;
        vc_t* vc;
    } path;
} sep_t;

typedef struct {
    int map_id;
    uint8_t* tfdz;
    size_t tfdz_size;
} transfer_frame_t;

transfer_frame_t _sep_get_from_mcf(sep_t* sep) {

}
transfer_frame_t _sep_get_from_vc(sep_t* sep) {
    mx_node* pcmx = &sep->pc->mx;
    mx_node* mcmx = mx_current_updated(pcmx);
    mx_node* vcmx = mx_current_updated(mcmx);

    pc_t* pc = mx_get_pc(pcmx);
    mc_t* mc = mx_get_mc(mcmx);
    vc_t* vc = mx_get_vc(vcmx);
    frame_t* fr = vc_get_frame(vc);
    transfer_frame_t frame = {0};
    frame.tfdz = fr->tfdz;
    frame.tfdz_size = fr->tfdz_size;

}

void _sep_forcing(sep_t* sep) {
    mx_node* pcmx = &sep->pc->mx;
    mx_node* mcmx = mx_current_updated(pcmx);
    mx_node* vcmx = mx_current_updated(mcmx);
    mx_node* mapmx = mx_current_updated(vcmx);
    map_t* map = mx_get_map(mapmx);
    vc_t* vc= mx_get_vc(vcmx);

    map_force_finish(map);
    
    //pushing it to VC, 
    //setting ready every channel on the way

}

transfer_frame_t _sep_idle(sep_t* sep) {
    assert(0 && "Not implemented");
    return {0};
}

void _sep_choose_path(sep_t* sep) {
    mx_node* pcmx = &sep->pc->mx;
    mx_node* mcmx = mx_current_ready(pcmx);
    if (mcmx == 0) {
        mcmx = mx_current_updated(pcmx);
        if (mcmx == 0) {
            sep->path.mc = 0;
            sep->path.vc = 0;
        } else {
            _sep_forcing(sep);
            mx_node* mcmx = mx_current_ready(pcmx);
            mx_node* vcmx = mx_current_ready(mcmx);
            sep->path.mc = mx_get_mc(mcmx);
            sep->path.vc = mx_get_vc(vcmx);
        }
    } else {
        if (mcmx->object_type == MX_OBJ_MCF) {
            sep->path.mc = mx_get_mc(mcmx);
            sep->path.vc = 0;
        } else {
            mx_node* vcmx = mx_current_ready(mcmx);
            sep->path.mc = mx_get_mc(mcmx);
            sep->path.vc = mx_get_vc(vcmx);
        }
    }
}

transfer_frame_t _sep_get_transfer_frame(sep_t* sep) {
    transfer_frame_t frame = {0};
    if (sep->path.mc == 0) {
        frame = _sep_idle(sep);
    } else if (sep->path.vc == 0) {
        frame = _sep_get_from_mcf(sep);
    } else {
        frame = _sep_get_from_vc(sep);
    }
    return frame;
}

size_t _sep_parse_transfer_frame(const transfer_frame_t* frame, uint8_t* data, size_t size) {
    assert(0);
}

void _sep_pop_transfer_frame(sep_t* sep) {

}

size_t sep_get_data(sep_t* sep, uint8_t* buffer, size_t size) {
    _sep_choose_path(sep);
    transfer_frame_t frame = _sep_get_transfer_frame(sep);
    size_t count = _sep_parse_transfer_frame(&frame, buffer, size);
    _sep_pop_transfer_frame(sep);
    return count;
}
