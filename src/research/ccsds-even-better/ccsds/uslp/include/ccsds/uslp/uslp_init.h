#pragma once

#include "sap.h"
#include "sep.h"
#include "uslp_types.h"

typedef struct {
    size_t queue_size;
    size_t tfdz_size;
} cop1_config_t;
void cop1_init(uslp_core_t* uslp, cop1_t* cop, const cop1_config_t* config);

void uslp_init(uslp_core_t* uslp, void* (*_alloc)(size_t count), void (*_free)(void*));

typedef struct {
    int vc_id;
    cop_enum_t cop_type;
    size_t tfdz_max_size;
    int ex_frame_count_length;
    
    size_t cop_queue_count;
    int sc_frame_count_length; 
    int seq_ctrld_ttl;
} vc_config_t;

void vc_init(uslp_core_t* uslp, vc_t* vc, mc_t* mc, const vc_config_t* config);

typedef struct {
    size_t tfdz_capacity;
    int map_id;
    map_type_t map_type;
} map_config_t;

void map_init(uslp_core_t* uslp, map_t* map, vc_t* vc, const map_config_t* config);

typedef struct {
    service_t service;

    size_t data_capacity;

} sap_config_t;

void sap_init(uslp_core_t* uslp, sap_t* sap, map_t* map, const sap_config_t* config);


typedef struct {
    int sc_id;
    bool sc_id_is_destination;
    bool ocf_is_used;
} mc_config_t;

void mc_init(uslp_core_t* uslp, mc_t* mc, pc_t* pc, const mc_config_t* config);

typedef struct {
    int pc_id;
    int tfvn;
    bool is_fec_presented;
    size_t insert_size;
} pc_config_t;

void pc_init(uslp_core_t* uslp, pc_t* pc, const pc_config_t* config);

void sep_init(uslp_core_t* uslp, sep_t* sep, pc_t* pc);

typedef struct {
    int tfvn;
    int is_fec_presented;
    size_t insert_size;
    size_t demx_entry_count;
} sap_receive_config_t;

void sap_receive_init(uslp_core_t* uslp, sap_receive_t* sapr, const sap_receive_config_t* config);

typedef struct {
    upid_t upid;
    int map_id;
    int vc_id;
    int sc_id;
    bool sc_is_destination;
    pvn_t pvn;
    cop_enum_t cop_type;
    size_t buffer_size;
    void (*map_cb)(void* arg, const uint8_t* data, size_t size);
    void *map_cb_arg;
    sap_receive_t* sapr;
} vc_receive_config_t;

void vc_receive_init(uslp_core_t* uslp, vc_receive_t* vcr, const vc_receive_config_t* config);
