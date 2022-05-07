#include "sap.h"
#include "uslp_mx.h"
#include "map.h"
#include "vc.h"

static void _sap_set_data(sap_t* sap) {
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


size_t sap_epp_send(sap_t* sap, const epp_packet_t* packet) {
    if (!sap->is_empty || sap->data.capacity < packet->size || sap->service != SERVICE_MAPP ||
        (sap->policy == POLICY_SIZE_STRICT && packet->size != sap->data.capacity))
    {
        return 0;
    }

    size_t count = epp_serialize_packet(packet, sap->data.data, sap->data.capacity);
    sap->data.size = count;
    sap->data.index = 0;
    sap->is_empty = false;

    if (sap->service == SERVICE_MAP_OCTET || sap->service == SERVICE_MAPP || sap->service == SERVICE_MAPA) {
        _sap_set_data(sap);
        vc_t* vc = mx_get_vc(mx_get_parent(mx_get_parent(&sap->mx)));
        vc_pull_everything_from_bottom(vc);
    }

    return 1;
}

size_t sap_send(sap_t* sap, const uint8_t* data, size_t size) {
    if (!sap->is_empty || sap->data.capacity < size ||
        (sap->policy == POLICY_SIZE_STRICT && size != sap->data.capacity))
    {
        return 0;
    }

    sap->is_empty = false;
    memcpy(sap->data.data, data, size);
    sap->data.size = size;
    if (sap->service == SERVICE_MAP_OCTET || sap->service == SERVICE_MAPP || sap->service == SERVICE_MAPA) {
        _sap_set_data(sap);
        vc_t* vc = mx_get_vc(mx_get_parent(mx_get_parent(&sap->mx)));
        vc_pull_everything_from_bottom(vc);
    }

    return size;
}
