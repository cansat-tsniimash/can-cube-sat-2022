#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "vc.h"
#include "uslp_mx.h"
#include "map.h"



void sap_clear(sap_t* sap) {
    sap->is_empty = true;
}

void uslp_core_sap_map_has_data(uslp_core_t* core, sap_t* sap) {
    mx_node* sapmx = &sap->mx;
    mx_node* mapmx = mx_get_parent(sapmx);
    mx_node* vcmx = mx_get_parent(mapmx);
    mx_node* mcmx = mx_get_parent(vcmx);
    mx_node* pcmx = mx_get_parent(mcmx);
    map_t* map = mx_get_map(mapmx);
    vc_t* vc = mx_get_vc(vcmx);

    if (!mx_is_in_ready(sapmx)) {
        mx_push_to_parent_ready(sapmx);
    }
    
    if (!mx_is_begin_ready(sapmx) || mx_is_in_ready(mapmx)) {
        return;
    }

    map_set_source(map, sap->data.data, sap->data.size);
    if (mx_try_push_to_parent_updated(mapmx)) {
        if (mx_try_push_to_parent_updated(vcmx)) {
            mx_try_push_to_parent_updated(mcmx);
        }
    }
    
    tfdf_t tfdf = {0};
    bool release_sap = false;

    while (true) {
        bool tfdf_legit = map_pull_tfdf(map, &tfdf, &release_sap);
        
        //Checking if map's source is empty
        if (release_sap) {
            //Clear source to set a new one received from mx queue.
            map_clear_source(map);
            sap_clear(sap);
            mx_pop_ready(mapmx); 
            sapmx = mx_current_ready(mapmx);
            
            //If there's antother source, we set it
            if (sapmx != 0) {
                sap = mx_get_sap(sapmx);
                map_set_source(map, sap->data.data, sap->data.size);
            }
        }
        if (!tfdf_legit) {
            if (sapmx) {
                continue;
            } else {
                break;
            }
        }
        mx_remove_from_parent_updated(mapmx);
        mx_push_to_parent_ready(mapmx);
        
        if (vc_is_full(vc)) {
            break;
        }
        frame_t frame = vc_frame_from_tfdf(vc, &tfdf);
        vc_save_frame(vc, &frame);
        if (!mx_is_in_ready(vcmx)) {
            mx_remove_from_parent_updated(vcmx);
            mx_push_to_parent_ready(vcmx);
            if (!mx_is_in_ready(mcmx)) {
                mx_remove_from_parent_updated(mcmx);
                mx_push_to_parent_ready(mcmx);
            }
        }
        //deleting
        map_clear_tfdf(map);
        mx_remove_from_parent_ready(mapmx);
        mapmx = mx_current_ready(vcmx);
        if (mapmx == 0) {
            break;
        }
        sapmx = mx_current_ready(mapmx);
        if (sapmx == 0) {
            break;
        }
        sap = mx_get_sap(sapmx);
        map = mx_get_map(mapmx);
    }
}

size_t sap_send(sap_t* sap, uint8_t const* data, size_t size) {
    if (!sap->is_empty || sap->data.capacity < size ||
        (sap->policy == POLICY_SIZE_STRICT && size != sap->data.capacity))
    {
        return 0;
    }

    sap->is_empty = false;
    memcpy(sap->data.data, data, size);
    sap->data.size = size;
    if (sap->service == SERVICE_MAP_OCTET || sap->service == SERVICE_MAPP || sap->service == SERVICE_MAPA) {
        uslp_core_sap_map_has_data(sap->uslp, sap);
    }

    return size;
}

