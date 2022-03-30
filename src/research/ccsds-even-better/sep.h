#pragma once

#include "uslp_types.h"
#include "vc.h"
#include "uslp_mx.h"
#include <stdio.h>
#include "map.h"
#include "mc.h"
#include "pc.h"
 
typedef struct {
    uslp_core_t* uslp;
    pc_t* pc;
    struct {
        mc_t* mc;
        vc_t* vc;
    } path;
} sep_t;

typedef struct {
    map_data_t map_data;
    vc_data_t vc_data;
    mc_data_t mc_data;
    pc_data_t pc_data;
} transfer_frame_t;

transfer_frame_t _sep_get_from_mcf(sep_t* sep) {
    transfer_frame_t frame = {0};
    return frame;
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


    frame.map_data = fr->map_data;
    frame.vc_data = fr->vc_data;
    frame.mc_data = mc_get_data(mc);
    frame.pc_data = pc_get_data(pc);

    return frame;
}

void _sep_forcing(sep_t* sep) {
    mx_node* pcmx = &sep->pc->mx;
    mx_node* mcmx = mx_current_updated(pcmx);
    mx_node* vcmx = mx_current_updated(mcmx);
    mx_node* mapmx = mx_current_updated(vcmx);
    map_t* map = mx_get_map(mapmx);
    vc_t* vc= mx_get_vc(vcmx);

    map_force_finish(map);
    vc_pull_everything_from_bottom(vc);
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
    if ((size_t)(end - ptr) < frame->map_data.tfdf.size) {
        return 0;
    }
    memcpy(ptr, frame->map_data.tfdf.tfdz, frame->map_data.tfdf.size);
    ptr += frame->map_data.tfdf.size;
    snprintf((char*)ptr, ptr - data, " %d %d %d", 
            frame->map_data.tfdf.map_id, frame->vc_data.vc_id, (int)frame->vc_data.vc_frame_count);

    return ptr - data;
}

void _sep_pop_transfer_frame(sep_t* sep) {
    if (sep->path.vc != 0) {
        
        pc_t* pc = sep->pc;
        mc_t* mc = sep->path.mc;
        vc_t* vc = sep->path.vc;

        mc_pop_data(mc);
        pc_pop_data(pc);
        vc_release_frame(vc);

        mx_remove_from_parent_ready(&vc->mx);
        mx_remove_from_parent_ready(&mc->mx);
        vc_pull_everything_from_bottom(vc);
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
