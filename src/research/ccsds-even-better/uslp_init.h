#pragma once

#include "uslp_socket.h"
#include "sep.h"

typedef struct {
    size_t queue_size;
    size_t tfdz_size;
} cop1_config_t;
void cop1_init(uslp_core_t* uslp, cop1_t* cop, const cop1_config_t* config) {
    cop->uslp = uslp;
    cop->tfdz_max_size = config->tfdz_size;
    cop->is_full = false;
    cop->head = 0;
    cop->tail = 0;
    cop->cur = 0;
    cop->size = config->queue_size;

    uint8_t* arr = (uint8_t*)cop->uslp->mem._alloc(config->tfdz_size * config->queue_size);
    vc_frame_t* queue = (vc_frame_t*)cop->uslp->mem._alloc(sizeof(vc_frame_t) * config->queue_size);
    assert(arr);
    assert(queue);
    cop->arr = queue;

    for (size_t i = 0; i < config->queue_size; i++) {
        queue[i].tfdz = &arr[i * config->tfdz_size];
    }
}

void uslp_init(uslp_core_t* uslp, void* (*_alloc)(size_t count), void (*_free)(void*)) {
    uslp->mem._alloc = _alloc;
    uslp->mem._free = _free;
}

typedef struct {
    int vc_id;
    cop_enum_t cop_type;
    size_t tfdz_max_size;
    size_t cop_queue_count;
} vc_config_t;

void vc_init(uslp_core_t* uslp, vc_t* vc, mc_t* mc, const vc_config_t* config) {
    mx_vc_init(vc, mc);
    vc->uslp = uslp;
    vc->frame_count = 0;
    vc->vc_id = config->vc_id;
    vc->cop_type = config->cop_type;
    if (config->cop_type == COP_NONE) {
        vc->container.cop_none.tfdz_size = config->tfdz_max_size;
        vc->container.cop_none.frame.tfdz = (uint8_t*) vc->uslp->mem._alloc(config->tfdz_max_size);
        assert(vc->container.cop_none.frame.tfdz);
    } else if (config->cop_type == COP_1) {
        cop1_config_t cfg = {0};
        cfg.queue_size = config->cop_queue_count;
        cfg.tfdz_size = config->tfdz_max_size;
        cop1_init(uslp, &vc->container.cop1, &cfg);
    } else {
        assert(0);
    }
}

typedef struct {
    size_t tfdz_capacity;
    int map_id;
    map_type_t map_type;
} map_config_t;

void map_init(uslp_core_t* uslp, map_t* map, vc_t* vc, const map_config_t* config) {
    mx_map_init(map, vc);
    map->tfdf_count = 0;

    map->uslp = uslp;
    map->map_id = config->map_id;
    map->map_type = config->map_type;

    if (map->map_type == MAP_TYPE_ACCESS) {
        map->tfdz_rule = TFDZ_RULE_MAPA_START;
        map->upid = UPID_MAPA;
    } else if (map->map_type == MAP_TYPE_PACKET) {
        map->tfdz_rule = TFDZ_RULE_MAPP_SPAN;
        map->upid = UPID_SP_OR_EP;
    } else { // map->map_type == MAP_TYPE_OCTET
        map->tfdz_rule = TFDZ_RULE_MAP_OCTET;
        map->upid = UPID_OCTET;
    } 


    map->tfdf.capacity = config->tfdz_capacity;
    map->tfdf.data = (uint8_t*)map->uslp->mem._alloc(map->tfdf.capacity);

    assert(map->tfdf.data != 0);
}

typedef struct {
    service_t service;

    size_t data_capacity;

} sap_config_t;

void sap_init(uslp_core_t* uslp, sap_t* sap, map_t* map, const sap_config_t* config) {
    mx_sap_init(sap, map);
    sap->uslp = uslp;
    sap->service = config->service;
    sap->data.capacity = config->data_capacity;
    sap->data.data = (uint8_t*)sap->uslp->mem._alloc(sap->data.capacity);

    assert(sap->data.data != 0);


    sap->policy = POLICY_NONE;
    sap->is_empty = true;
    sap->packet_count = 0;

    sap->service = config->service;
}


typedef struct {
    int mc_id;
} mc_config_t;

void mc_init(uslp_core_t* uslp, mc_t* mc, pc_t* pc, const mc_config_t* config) {
    mx_mc_init(mc, pc);

    mc->mc_id = config->mc_id;
}

typedef struct {
    int pc_id;
} pc_config_t;

void pc_init(uslp_core_t* uslp, pc_t* pc, const pc_config_t* config) {
    mx_pc_init(pc);
}

void sep_init(uslp_core_t* uslp, sep_t* sep, pc_t* pc) {
    sep->pc = pc;
    sep->uslp = uslp;
}
