#include "uslp_init.h"
#include "uslp_mx.h"
#include "vc_receive.h"

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
        queue[i].map_data.tfdf.tfdz = &arr[i * config->tfdz_size];
        queue[i].map_data.tfdf.size = config->tfdz_size;
    }
}

void uslp_init(uslp_core_t* uslp, void* (*_alloc)(size_t count), void (*_free)(void*)) {
    uslp->mem._alloc = _alloc;
    uslp->mem._free = _free;
}

void vc_init(uslp_core_t* uslp, vc_t* vc, mc_t* mc, const vc_config_t* config) {
    mx_vc_init(vc, mc);
    vc->uslp = uslp;
    vc->sc_frame_count = 0;
    vc->ex_frame_count = 0;
    vc->vc_id = config->vc_id;
    vc->ex_frame_count_length = (uint8_t)config->ex_frame_count_length;
    vc->sc_frame_count_length = (uint8_t)config->sc_frame_count_length;
    vc->seq_ctrld_ttl = config->seq_ctrld_ttl;
    vc->cop_type = config->cop_type;
    if (config->cop_type == COP_NONE) {
        vc->container.expedited_frame.tfdz_size = config->tfdz_max_size;
        vc->container.expedited_frame.frame.map_data.tfdf.tfdz = (uint8_t*) vc->uslp->mem._alloc(config->tfdz_max_size);
        assert(vc->container.expedited_frame.frame.map_data.tfdf.tfdz);
    } else if (config->cop_type == COP_1) {
        cop1_config_t cfg = {0};
        cfg.queue_size = config->cop_queue_count;
        cfg.tfdz_size = config->tfdz_max_size;
        cop1_init(uslp, &vc->container.cop1, &cfg);
    } else {
        assert(0);
    }
}

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
    map->tfdf.size = config->tfdz_capacity;
    map->tfdf.data = (uint8_t*)map->uslp->mem._alloc(map->tfdf.capacity);


    assert(map->tfdf.data != 0);
}

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

void mc_init(uslp_core_t* uslp, mc_t* mc, pc_t* pc, const mc_config_t* config) {
    mx_mc_init(mc, pc);

    mc->sc_id = config->sc_id;
    mc->ocf_is_used = config->ocf_is_used;
    mc->sc_id_is_destination = config->sc_id_is_destination;
    mc->frame_count = 0;
}

void pc_init(uslp_core_t* uslp, pc_t* pc, const pc_config_t* config) {
    mx_pc_init(pc);
    pc->tfvn = config->tfvn;
    pc->is_fec_presented = config->is_fec_presented;
    pc->insert_size = config->insert_size;
    pc->frame_count = 0;
    if (pc->insert_size > 0) {
        pc->insert_data = uslp->mem._alloc(pc->insert_size);
        assert(pc->insert_data);
    } else {
        pc->insert_data = 0;
    }
}

void sep_init(uslp_core_t* uslp, sep_t* sep, pc_t* pc) {
    sep->pc = pc;
    sep->uslp = uslp;
}

void sap_receive_init(uslp_core_t* uslp, sap_receive_t* sapr, const sap_receive_config_t* config) {
    sapr->pcr.insert_size = config->insert_size;
    sapr->pcr.is_fec_presented = config->is_fec_presented;
    sapr->pcr.tfvn = config->tfvn;
    sapr->demx.entry_capacity = config->demx_entry_count;
    sapr->demx.entry_count = 0;
    sapr->demx.entry_arr = uslp->mem._alloc(config->demx_entry_count * sizeof(sapr->demx.entry_arr[0]));
}

void vc_receive_init(uslp_core_t* uslp, vc_receive_t* vcr, const vc_receive_config_t* config) {
    vcr->mapr.map_id = config->map_id;
    vcr->mapr.pvn = config->pvn;
    vcr->mapr.upid = config->upid;

    vcr->buffer.capacity = config->buffer_size;
    vcr->buffer.index = 0;
    vcr->buffer.size = 0;
    vcr->buffer.data = uslp->mem._alloc(config->buffer_size);

    vcr->ex_frame_count = 0;
    vcr->sc_frame_count = 0;
    vcr->map_cb = config->map_cb;
    vcr->map_cb_arg = config->map_cb_arg;

    demx_add_vcf(&config->sapr->demx, vcr, vc_receive_cb, config->sc_id, config->sc_is_destination, config->vc_id);
}
