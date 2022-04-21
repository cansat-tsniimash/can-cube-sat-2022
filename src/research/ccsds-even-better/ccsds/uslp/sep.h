#pragma once

#include "uslp_types.h"
#include "vc.h"
#include "uslp_mx.h"
#include <stdio.h>
#include "map.h"
#include "mc.h"
#include "pc.h"
#include "../endian2.h"
 
typedef struct {
    uslp_core_t* uslp;
    pc_t* pc;
    struct {
        mc_t* mc;
        vc_t* vc;
    } path;
} sep_t;


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
    assert(0 && "Idle frame is not implemented");
    int a = 5;
    transfer_frame_t fr = {0};
    return fr;
}

bool _sep_choose_path(sep_t* sep, bool forced) {
    mx_node* pcmx = &sep->pc->mx;
    mx_node* mcmx = mx_current_ready(pcmx);
    if (mcmx == 0) {
        if (!forced) {
            return false;   
        }
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

uint16_t _sep_calc_size(const transfer_frame_t* frame) {
    uint16_t fsize = 0;
    fsize += 4;
    if (!frame->vc_data.frame_trancated) {
        fsize += 3 + frame->vc_data.vc_frame_count_length;
    }
    if (frame->mc_data.ocf_valid) {
        fsize += 4;
    }
    if (frame->pc_data.use_fec) {
        fsize += 2;
    }
    fsize += (uint16_t)frame->pc_data.insert_size;
    fsize += (uint16_t)frame->map_data.tfdf.size;
    if (frame->map_data.tfdf.tfdz_rule <= 3) {
        fsize += 3;
    } else {
        fsize += 1;
    }
    return fsize;
}

size_t _sep_serialize_transfer_frame(const transfer_frame_t* frame, uint8_t* data, size_t size) {

    bit_array_t ba = {0};
    ba.ptr = data;
    ba.bit_start = 0;
    ba.bit_end = size * 8;

    uint8_t sod_flag = frame->mc_data.sc_id_is_destination ? 1 : 0;
    uint8_t eofph_flag = frame->vc_data.frame_trancated ? 1 : 0;
    uint8_t bsc_flag = frame->map_data.qos == QOS_EXPEDITED ? 1 : 0;
    uint8_t pcc_flag = frame->vc_data.contains_protocol_control_commands ? 1 : 0;
    uint8_t reserve = 0;
    uint8_t ocfp_flag = frame->mc_data.ocf_valid ? 1 : 0;
    uint16_t fsize = _sep_calc_size(frame);

	ccsds_insert(&ba, &frame->pc_data.tfvn, 4);
	ccsds_insert(&ba, &frame->mc_data.sc_id, 16);
	ccsds_insert(&ba, &sod_flag, 1);
	ccsds_insert(&ba, &frame->vc_data.vc_id, 6);
	ccsds_insert(&ba, &frame->map_data.tfdf.map_id, 4);
    ccsds_insert(&ba, &eofph_flag, 1);
    
    if (!frame->vc_data.frame_trancated) {
		ccsds_insert(&ba, &fsize, 16);
		ccsds_insert(&ba, &bsc_flag, 1);
		ccsds_insert(&ba, &pcc_flag, 1);
        ccsds_insert(&ba, &reserve, 2);
        
		ccsds_insert(&ba, &ocfp_flag, 1);
		ccsds_insert(&ba, &frame->vc_data.vc_frame_count_length, 3);
		ccsds_insert(&ba, &frame->vc_data.vc_frame_count, 8 * (frame->vc_data.vc_frame_count_length));
	}

    
	// Insert zone
    memcpy(ba.ptr + ba.bit_start / 8, frame->pc_data.insert_data, frame->pc_data.insert_size);
    ba.bit_start += 8 * frame->pc_data.insert_size;
	

	//TFDF
    ccsds_insert(&ba, &frame->map_data.tfdf.tfdz_rule, 3);
    ccsds_insert(&ba, &frame->map_data.tfdf.upid, 5);

	if (frame->map_data.tfdf.tfdz_rule <= 3) {
		ccsds_insert(&ba, &frame->map_data.tfdf.pointer_fh_lo, 16);
	}
    memcpy(ba.ptr + ba.bit_start / 8, frame->map_data.tfdf.tfdz, frame->map_data.tfdf.size);
    ba.bit_start += 8 * frame->map_data.tfdf.size;
	

	//OCF
	if (frame->mc_data.ocf_valid) {
		memcpy(&ba.ptr + ba.bit_start / 8, frame->mc_data.ocf, 4);
		ba.bit_start += 32;
	}

	//FEC
	if (frame->pc_data.use_fec) {
        uint16_t fec_crc = calc_crc(data, ba.bit_start / 8);
        ccsds_insert(&ba, &fec_crc, 16);
	}

    return ba.bit_start / 8;
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
    size_t count = _sep_serialize_transfer_frame(&frame, buffer, size);
    _sep_pop_transfer_frame(sep);
    return count;
}
