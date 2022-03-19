#pragma once

#include "uslp_types.h"
#include "vc.h"
#include "uslp_mx.h"
#include <stdio.h>

typedef struct {
    uslp_core_t* uslp;
    pc_t* pc;
    struct {
        mc_t* mc;
        vc_t* vc;
    } path;
} sep_t;

typedef struct {
    size_t tfdz_size;
    
    uint8_t* tfdz;
    uint64_t vc_frame_count;
    uint64_t map_frame_count;
    uint64_t mc_frame_count;
    uint64_t pc_frame_count;
    int map_id;
    int vc_id;
    int sc_id;
    int tfvn;
} transfer_frame_t;

transfer_frame_t _sep_get_from_mcf(sep_t* sep) {

}
transfer_frame_t _sep_get_from_vc(sep_t* sep) {
    mx_node* pcmx = &sep->pc->mx;
    mx_node* mcmx = mx_current_ready(pcmx);
    mx_node* vcmx = mx_current_ready(mcmx);

    pc_t* pc = mx_get_pc(pcmx);
    mc_t* mc = mx_get_mc(mcmx);
    vc_t* vc = mx_get_vc(vcmx);
    vc_frame_t* fr = vc_get_frame(vc);
    transfer_frame_t frame = {0};
    frame.tfdz = fr->tfdz;
    frame.tfdz_size = fr->tfdz_size;
    frame.map_frame_count = fr->map_frame_count;
    frame.map_id = fr->map_id;
    frame.vc_id = fr->vc_id;
    frame.vc_frame_count = fr->vc_frame_count;
    return frame;
}

void _sep_forcing(sep_t* sep) {
    mx_node* pcmx = &sep->pc->mx;
    mx_node* mcmx = mx_current_updated(pcmx);
    mx_node* vcmx = mx_current_updated(mcmx);
    mx_node* mapmx = mx_current_updated(vcmx);
    map_t* map = mx_get_map(mapmx);
    //Closing tfdf of map, pushing it to VC, 
    //setting ready every channel on the way

}

transfer_frame_t _sep_idle(sep_t* sep) {
    assert(0 && "Not implemented");
    int a = 5;
    transfer_frame_t fr = {0};
    return fr;
}

bool _sep_choose_path(sep_t* sep, bool forced) {
    mx_node* pcmx = &sep->pc->mx;
    mx_node* mcmx = mx_current_ready(pcmx);
    if (mcmx == 0) {
        mcmx = mx_current_updated(pcmx);
        if (mcmx == 0) {
            sep->path.mc = 0;
            sep->path.vc = 0;
        } else {
            if (!forced) {
                return false;
            }
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
    return true;
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
    uint8_t* ptr = data;
    uint8_t* end = data + size;
    if ((size_t)(end - ptr) < frame->tfdz_size) {
        return 0;
    }
    memcpy(ptr, frame->tfdz, frame->tfdz_size);
    ptr += frame->tfdz_size;
    snprintf((char*)ptr, ptr - data, " %d %d %d", frame->map_id, frame->vc_id, (int)frame->vc_frame_count);

    return ptr - data;
}

void _sep_pop_transfer_frame(sep_t* sep) {
    if (sep->path.vc != 0) {
        vc_release_frame(sep->path.vc);
        mx_remove_from_parent_ready(&sep->path.vc->mx);
        mx_remove_from_parent_ready(&sep->path.mc->mx);
        vc_pull_everything_from_bottom(sep->path.vc);
    }
}

size_t sep_get_data(sep_t* sep, uint8_t* buffer, size_t size, bool forced) {
    if (!_sep_choose_path(sep, forced)) {
        return 0;
    }
    transfer_frame_t frame = _sep_get_transfer_frame(sep);
    size_t count = _sep_parse_transfer_frame(&frame, buffer, size);
    _sep_pop_transfer_frame(sep);
    return count;
}
