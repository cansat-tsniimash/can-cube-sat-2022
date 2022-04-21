#pragma once

#include "uslp_types.h"
#include "buffer.h"
#include "../epp/epp_empty.h"

void _map_access_set_empty(uint8_t* data, size_t size) {
    static const uint8_t pattern[] = {0xDE, 0xAF, 0xBA, 0xBE};
    size_t di = 0;
    size_t pi = 0;
    while (di < size) {
        data[di] = pattern[pi];
        di++;
        pi++;
        pi %= sizeof(pattern);
    }
}

void _map_octet_set_empty(uint8_t* data, size_t size) {
    static const uint8_t pattern[] = {0xDE, 0xAD, 0xBA, 0xBE};
    size_t di = 0;
    size_t pi = 0;
    while (di < size) {
        data[di] = pattern[pi];
        di++;
        pi++;
        pi %= sizeof(pattern);
    }
}

void map_force_finish(map_t* map) {
    if (map->map_type == MAP_TYPE_ACCESS) {
        assert(0 && "No, you cannot force MAPA");
    } else if (map->map_type == MAP_TYPE_PACKET) {
        epp_set_empty_packet(map->tfdf.data + map->tfdf.index, map->tfdf.size - map->tfdf.index);
    } else {
        _map_octet_set_empty(map->tfdf.data + map->tfdf.index, map->tfdf.size - map->tfdf.index);
        map->pointer_fh_lo = (uint16_t)map->tfdf.index;

    }
    map->tfdf.index = map->tfdf.size;
}

void map_set_source(map_t* map, uint8_t* data, size_t size) {
    if (map->source_buffer.data == 0) {
        map->source_buffer.data = data;
        map->source_buffer.capacity = size;
        map->source_buffer.size = size;
        map->source_buffer.index = 0;

        if (map->map_type == MAP_TYPE_PACKET && map->pointer_fh_lo == 0xFFFF) {
            map->pointer_fh_lo = (uint16_t)map->tfdf.index;
        }
    }
}

bool map_pull_data(map_t* map, map_data_t* md, bool* release_sap, bool* release_map) {
    tfdf_t* tfdf = &md->tfdf;

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

    if (map->map_type == MAP_TYPE_ACCESS) {
        if (src_size > dst_size) {
            map->pointer_fh_lo = 0xFFFF;
        } else {
            map->pointer_fh_lo = (uint16_t)map->tfdf.index;
        }
    }
    if (map->tfdf.index == map->tfdf.size || map->map_type == MAP_TYPE_ACCESS) {
        tfdf->tfdz = map->tfdf.data;
        tfdf->size = map->tfdf.size;
        md->frame_count = map->tfdf_count;
        md->qos = map->qos;
        tfdf->map_id = map->map_id;
        tfdf->pointer_fh_lo = map->pointer_fh_lo;
        tfdf->tfdz_rule = map->tfdz_rule;
        tfdf->upid = map->upid;

        if (map->map_type == MAP_TYPE_ACCESS) {
            _map_access_set_empty(map->tfdf.data + map->tfdf.index, map->tfdf.size - map->tfdf.index);
            *release_map = true;
        } else if (map->map_type == MAP_TYPE_PACKET) {
            *release_map = *release_sap;
        } else if (map->map_type == MAP_TYPE_OCTET) {
            *release_map = true;
        }
        return true;
    } else if (map->tfdf.index == 0) {
        if (map->map_type == MAP_TYPE_ACCESS) {
            *release_map = true;
        } else if (map->map_type == MAP_TYPE_PACKET) {
            *release_map = true;
        } else if (map->map_type == MAP_TYPE_OCTET) {
            *release_map = true;
        }
        return false;
    } else {
        if (map->map_type == MAP_TYPE_ACCESS) {
            *release_map = false;
        } else if (map->map_type == MAP_TYPE_PACKET) {
            *release_map = false;
        } else if (map->map_type == MAP_TYPE_OCTET) {
            *release_map = true;
        }
        return false;
    }
}

void map_clear_tfdf(map_t* map) {
    map->tfdf.index = 0;
    map->tfdf_count++;
    if (map->map_type == MAP_TYPE_ACCESS) {
        if (map->source_buffer.data == 0) {
            map->tfdz_rule = TFDZ_RULE_MAPA_START;
        } else {
            map->tfdz_rule = TFDZ_RULE_MAPA_CONTINUE;
        }
    } else if (map->map_type == MAP_TYPE_PACKET) {
        map->pointer_fh_lo = 0xFFFF;
    } else {
        map->pointer_fh_lo = 0xFFFF;
    }
}
void map_clear_source(map_t* map) {
    map->source_buffer.index = 0;
    map->source_buffer.size = 0;
    map->source_buffer.data = 0;
    map->source_buffer.capacity = 0;
}

