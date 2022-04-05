#pragma once

#include "uslp_types.h"
#include "endian2.h"
#include "pc.h"
#include "sep.h"

bool parse(pc_t* pc, uint8_t* data, size_t size, transfer_frame_t* frame) {
    bit_array_t ba = {0};
    ba.ptr = data;
    uint16_t frame_length = 0;
    
    uint8_t sod_flag = 0;
    uint8_t eofph_flag = 0;

    
    uint16_t tfdz_size = size - 4;

    ccsds_extract(&ba, &frame->pc_data.tfvn, 4);
    if (frame->pc_data.tfvn != pc->tfvn) {
        return false;
    }
	ccsds_extract(&ba, &frame->mc_data.sc_id, 16);
	ccsds_extract(&ba, &sod_flag, 1);
	ccsds_extract(&ba, &frame->vc_data.vc_id, 6);
	ccsds_extract(&ba, &frame->map_data.tfdf.map_id, 4);
	ccsds_extract(&ba, &eofph_flag, 1);

    frame->mc_data.sc_id_is_destination = sod_flag == 1;
    frame->vc_data.frame_trancated = eofph_flag == 1;


	if (!frame->vc_data.frame_trancated) {
        tfdz_size -= 3;
        uint8_t bsc_flag = 0;
        uint8_t pcc_flag = 0;
        uint8_t ocfp_flag = 0;
        uint8_t reserve = 0;
        

		ccsds_extract(&ba, &frame_length, 16);
		ccsds_extract(&ba, &bsc_flag, 1);
		ccsds_extract(&ba, &pcc_flag, 1);
		ccsds_extract(&ba, &reserve, 2);
		ccsds_extract(&ba, &ocfp_flag, 1);
		ccsds_extract(&ba, &frame->vc_data.vc_frame_count_length, 3);
		ccsds_extract(&ba, &frame->vc_data.vc_frame_count, 8 * (frame->vc_data.vc_frame_count_length));
        
        frame->map_data.qos = bsc_flag == 1 ? QOS_EXPEDITED : QOS_SEQ_CTRLD;
        frame->vc_data.contains_protocol_control_commands = pcc_flag == 1;
        frame->mc_data.ocf_valid = ocfp_flag == 1;
        tfdz_size -= frame->vc_data.vc_frame_count_length;
        
        //insert zone
        frame->pc_data.insert_data = ba.ptr + ba.bit_start / 8;
        ba.bit_start += 8 * pc->insert_size;

        tfdz_size -= pc->insert_size;
	}
    

	//TFDF
	ccsds_extract(&ba, &frame->map_data.tfdf.tfdz_rule, 3);
	ccsds_extract(&ba, &frame->map_data.tfdf.upid, 5);
	if (frame->map_data.tfdf.tfdz_rule <= 3) {
		ccsds_extract(&ba, &frame->map_data.tfdf.pointer_fh_lo, 16);
	}
	frame->map_data.tfdf.tfdz = ba.ptr + ba.bit_start / 8;

    if (!frame->vc_data.frame_trancated) {
        //FEC
        if (pc->is_fec_presented) {
            frame->pc_data.use_fec = true;
            uint16_t fec_crc = 0;
            ccsds_endian_extract(data + size - 2, 0, (uint8_t*)&fec_crc, 16);
            if (calc_crc(data, size - 2) != fec_crc) {
                return false;
            }
            size -= 2;
            tfdz_size -= 2;
        }

        //OCF
        if (frame->mc_data.ocf_valid) {
            frame->mc_data.ocf_valid = true;
            frame->mc_data.ocf[0] = data[size - 4];
            frame->mc_data.ocf[1] = data[size - 3];
            frame->mc_data.ocf[2] = data[size - 2];
            frame->mc_data.ocf[3] = data[size - 1];
            size -= 4;
            tfdz_size -= 4;
        }
    }
    frame->map_data.tfdf.size = tfdz_size;


	return true;
}

typedef struct {
    bool is_fec_presented;
    size_t insert_size;
    int tfvn;
} sep_receive_t;



typedef void (*sep_callback_insert)(void*, const uint8_t*, size_t);
typedef void (*sep_callback_ocf)(void*, const uint8_t*);
typedef void (*sep_callback_mcf)(void*, const map_data_t*, const vc_data_t*, const mc_data_t*);
typedef void (*sep_callback_vcf)(void*, const map_data_t*, const vc_data_t*);

typedef enum {
    DEMX_SERVICE_INSERT,
    DEMX_SERVICE_OCF,
    DEMX_SERVICE_MCF,
    DEMX_SERVICE_VCF
} demx_service_t;
typedef struct {
    int vc_id;
    int sc_id;
    bool sc_is_destination;
    void* arg;
    union {
        sep_callback_insert insert;
        sep_callback_ocf ocf;
        sep_callback_mcf mcf;
        sep_callback_vcf vcf;
    } cb;
    demx_service_t service;
} demx_entry_t;

typedef struct {
    
    demx_entry_t* entry_list;
    size_t entry_count;
    size_t entry_list_size;
} demx_t;

typedef struct {
    uint8_t* data;
    size_t size;
    pc_t* pc;
    demx_t demx;
} sap_receive_t;
bool sep_push_data(sap_receive_t* sep, uint8_t* data, size_t size) {
    transfer_frame_t frame = {0};
    if (!parse(sep->pc, data, size, &frame)) {
        return false;
    }
    demx_entry_t *cur = &sep->demx.entry_list[0];
    for (int i = 0; i < sep->demx.entry_count; i++) {
        if (cur->service == DEMX_SERVICE_INSERT) {
            cur->cb.insert(cur->arg, 
                           frame.pc_data.insert_data, 
                           frame.pc_data.insert_size);
        } else if (cur->service == DEMX_SERVICE_MCF) {
            
            if (cur->sc_id == frame.mc_data.sc_id && 
                cur->sc_is_destination == frame.mc_data.sc_id_is_destination) {
                
                cur->cb.mcf(cur->arg, &frame.map_data, &frame.vc_data, &frame.mc_data);
            }
            
        } else if (cur->service == DEMX_SERVICE_OCF) {
            if (cur->sc_id == frame.mc_data.sc_id && 
                cur->sc_is_destination == frame.mc_data.sc_id_is_destination &&
                frame.mc_data.ocf_valid) {
                
                cur->cb.ocf(cur->arg, frame.mc_data.ocf);
            }
        } else if (cur->service == DEMX_SERVICE_VCF) {
            if (cur->sc_id == frame.mc_data.sc_id && 
                cur->sc_is_destination == frame.mc_data.sc_id_is_destination &&
                cur->vc_id == frame.vc_data.vc_id) {
                
                cur->cb.vcf(cur->arg, &frame.map_data, &frame.vc_data);
            }
        }
    }
    return true;
}

void demx_add_insert(demx_t* demx, void* arg, sep_callback_insert cb) {
    assert(demx->entry_count < demx->entry_list_size);
    demx_entry_t* entry = &demx->entry_list[demx->entry_count++];
    entry->service = DEMX_SERVICE_INSERT;
    entry->cb.insert = cb;
    entry->arg = arg;
}
void demx_add_ocf(demx_t* demx, void* arg, sep_callback_ocf cb, 
                  int sc_id, bool sc_id_is_destination) {
    assert(demx->entry_count < demx->entry_list_size);
    demx_entry_t* entry = &demx->entry_list[demx->entry_count++];
    entry->service = DEMX_SERVICE_OCF;
    entry->cb.ocf = cb;
    entry->arg = arg;
    entry->sc_id = sc_id;
    entry->sc_is_destination = sc_id_is_destination;
}
void demx_add_mcf(demx_t* demx, void* arg, sep_callback_mcf cb, 
                  int sc_id, bool sc_id_is_destination) {
    assert(demx->entry_count < demx->entry_list_size);
    demx_entry_t* entry = &demx->entry_list[demx->entry_count++];
    entry->service = DEMX_SERVICE_MCF;
    entry->cb.mcf = cb;
    entry->arg = arg;
    entry->sc_id = sc_id;
    entry->sc_is_destination = sc_id_is_destination;
}
void demx_add_vcf(demx_t* demx, void* arg, sep_callback_vcf cb, 
                  int sc_id, bool sc_id_is_destination, int vc_id) {
    assert(demx->entry_count < demx->entry_list_size);
    demx_entry_t* entry = &demx->entry_list[demx->entry_count++];
    entry->service = DEMX_SERVICE_VCF;
    entry->cb.vcf = cb;
    entry->arg = arg;
    entry->sc_id = sc_id;
    entry->sc_is_destination = sc_id_is_destination;
    entry->vc_id = vc_id;
}