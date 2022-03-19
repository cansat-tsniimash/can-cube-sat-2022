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

void sap_set_data(sap_t* sap) {
    mx_node* sapmx = &sap->mx;
    mx_node* mapmx = mx_get_parent(sapmx);
    mx_node* vcmx = mx_get_parent(mapmx);
    mx_node* mcmx = mx_get_parent(vcmx);
    map_t* map = mx_get_map(mapmx);

    mx_try_push_to_parent_updated(sapmx);
    mx_try_push_to_parent_updated(mapmx);
    mx_try_push_to_parent_updated(vcmx);
    mx_try_push_to_parent_updated(mcmx);
}


void vc_pull_everything_from_bottom(vc_t* vc) {
    mx_node* vcmx = &vc->mx;
    mx_node* mcmx = mx_get_parent(vcmx);
 
    
    tfdf_t tfdf = {0};
    bool release_sap = false;
    bool release_map = false;
    while (true) {
        
        mx_node* mapmx = mx_current_updated(vcmx);
        if (mapmx == 0) {  
            break;
        }
        map_t* map = mx_get_map(mapmx);
        mx_node* sapmx = mx_current_updated(mapmx);
        sap_t* sap = 0;
        bool tfdf_legit = false;
        if (sapmx != 0) {
            sap = mx_get_sap(sapmx);
            map_set_source(map, sap->data.data, sap->data.size);
            
            tfdf_legit = map_pull_tfdf(map, &tfdf, &release_sap);
            if (!release_sap || !release_map || tfdf_legit) {
                mx_try_push_to_parent_updated(sapmx);
                mx_try_push_to_parent_updated(mapmx);
                mx_try_push_to_parent_updated(vcmx);
                mx_try_push_to_parent_updated(mcmx);
            }
            if (release_sap) {
                sap_clear(sap);
                map_clear_source(map);
                mx_remove_from_parent_updated(sapmx);
            }
        }

        if (tfdf_legit) {
            
            if (vc_is_full(vc)) {
                return;
            }
            vc_frame_t frame = vc_frame_from_tfdf(vc, &tfdf);
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
        }
        if (release_map) {
            mx_remove_from_parent_updated(mapmx);
            
        }
        
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
        sap_set_data(sap);
        vc_t* vc = mx_get_vc(mx_get_parent(mx_get_parent(&sap->mx)));
        vc_pull_everything_from_bottom(vc);
    }

    return size;
}

