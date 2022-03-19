#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "vc.h"
#include "uslp_mx.h"


bool buffer_is_empty(const buffer_t* buffer) {
    return buffer->size > 0;
}
bool buffer_is_full(const buffer_t* buffer) {
    return buffer->size > 0 && buffer->index == buffer->size;
}
void buffer_clear(buffer_t* buffer) {
    buffer->index = 0;
    buffer->size = 0;
}
void buffer_init(buffer_t* buffer, uint8_t* arr, size_t size) {
    buffer->data = arr;
    buffer->capacity = size;
    buffer->size = 0;
    buffer->capacity;
}
bool buffer_try_copy(buffer_t* buffer, uint8_t const* arr, size_t size) {
    if (size > buffer->capacity) {
        return false;
    }
    memcpy(buffer->data, arr, size);
    buffer->size = size;
    return true;
}


void map_set_source(map_t* map, uint8_t* data, size_t size) {
    if (map->source_buffer.data == 0) {
        map->source_buffer.data = data;
        map->source_buffer.capacity = size;
        map->source_buffer.size = size;
        map->source_buffer.index = 0;
    }
}

bool map_pull_tfdf(map_t* map, tfdf_t* tfdf, bool* release_sap, bool* release_map, bool forced) {
    *release_map = false;
    uint8_t* src_begin = map->source_buffer.data + map->source_buffer.index;
    uint8_t* dst_begin = map->tfdf.data + map->tfdf.index;

    size_t src_size = map->source_buffer.size - map->source_buffer.index;
    size_t dst_size = map->tfdf.size - map->tfdf.index;

    size_t count_to_copy = src_size < dst_size ? src_size : dst_size;

    memcpy(dst_begin, src_begin, count_to_copy);
    map->source_buffer.index += count_to_copy;
    map->tfdf.index += count_to_copy;

    if (map->source_buffer.index == map->source_buffer.size) {
        *release_sap = true;
    } else {
        *release_sap = false;
    }

    if (map->tfdf.index == map->tfdf.size) {
        tfdf->tfdz = map->tfdf.data;
        tfdf->size = map->tfdf.size;
        tfdf->map_frame_count = map->tfdf_count;
        tfdf->map_id = map->map_id;
        return true;
    } else if (forced) {
        // here we fill mapa and map_octet with a template data and mapp with empty packet
        // we fill all the other fields accordingly.
        memset(map->tfdf.data + map->tfdf.index, 0, map->tfdf.size - map->tfdf.index);



        tfdf->map_frame_count = map->tfdf_count;
        tfdf->tfdz = map->tfdf.data;
        tfdf->size = map->tfdf.size;
        tfdf->map_id = map->map_id;
        return true;
    } else {
        return false;
    }
}

void map_clear_tfdf(map_t* map) {
    map->tfdf.index = 0;
    map->tfdf_count++;
}
void map_clear_source(map_t* map) {
    map->source_buffer.index = 0;
    map->source_buffer.size = 0;
    map->source_buffer.data = 0;
    map->source_buffer.capacity = 0;
}



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
        mx_node* sapmx = mx_current_updated(mapmx);
        

        if (sapmx != 0) {
            sap_t* sap = mx_get_sap(sapmx);
            map_set_source(map, sap->data.data, sap->data.size);
        }
        map_t* map = mx_get_map(mapmx);
        bool tfdf_legit = map_pull_tfdf(map, &tfdf, &release_sap, &release_map, false);
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

