#pragma once

#include "uslp_types.h"
#include "ccsds\generic\endian2.h"
#include "sep.h"

bool parse(pcr_t* pcr, uint8_t* data, size_t size, transfer_frame_t* frame);


bool sep_push_data(sap_receive_t* sep, uint8_t* data, size_t size);

void demx_add_insert(demx_t* demx, void* arg, sep_callback_insert cb);
void demx_add_ocf(demx_t* demx, void* arg, sep_callback_ocf cb, 
                  int sc_id, bool sc_id_is_destination);
void demx_add_mcf(demx_t* demx, void* arg, sep_callback_mcf cb, 
                  int sc_id, bool sc_id_is_destination);
void demx_add_vcf(demx_t* demx, void* arg, sep_callback_vcf cb, 
                  int sc_id, bool sc_id_is_destination, int vc_id);