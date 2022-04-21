#pragma once

#include "uslp_types.h"


void mx_queue_push_to_parent(mx_queue_node* node) {
    assert(node->parent != 0);
    if (node->parent->begin == 0) {
        node->parent->begin = node;
        node->parent->end = node;
        node->next = 0;
        node->prev = 0;
    } else {
        node->parent->end->next = node;
        node->prev = node->parent->end;
        node->next = 0;
        
        node->parent->end = node;
    }
    node->is_in_queue = true;
}
mx_queue_node* mx_queue_begin(mx_queue_node* node) {
    return node->begin;
}
void mx_queue_pop_first(mx_queue_node* node) {
    assert(node->begin != 0);
    if (node->begin->next == 0) {
        node->begin = 0;
        node->end = 0;
    } else {
        mx_queue_node* new_begin = node->begin->next;
        mx_queue_node* old_begin = node->begin;
        new_begin->prev = 0;
        old_begin->next = 0;
        node->begin = new_begin;
    }
    node->is_in_queue = false;
}
void mx_queue_remove_from_parent(mx_queue_node* node) {
    if (!node->is_in_queue) {
        return;
    }
    mx_queue_node* prev = node->prev;
    mx_queue_node* next = node->next;
    mx_queue_node* parent = node->parent;
    if (prev) {
        prev->next = next;
    } else {
        parent->begin = next;
    }
    if (next) {
        next->prev = prev;
    } else {
        parent->end = prev;
    }
    node->next = 0;
    node->prev = 0;
    node->is_in_queue = false;
}


mx_node* mx_get_parent(mx_node* node) {
    return node->parent;
}

void mx_push_to_parent_ready(mx_node* node) {
    assert(node->state == MX_STATE_NONE);
    node->state = MX_STATE_READY;
    mx_node* parent = node->parent;
    if (parent->child_ready_end) {
        parent->child_ready_end->next = node;
        node->prev = parent->child_ready_end;
        node->next = 0;

        parent->child_ready_end = node;
    } else {
        parent->child_ready_start = node;
        parent->child_ready_end = node;
        node->next = 0;
        node->prev = 0;

    }
}

void mx_push_to_parent_updated(mx_node* node) {
    assert(node->state == MX_STATE_NONE);
    node->state = MX_STATE_UPDATED;
    mx_node* parent = node->parent;
    if (parent->child_upd_end) {
        parent->child_upd_end->next = node;
        node->prev = parent->child_upd_end;
        node->next = 0;
        
        parent->child_upd_end = node;
    } else {
        parent->child_upd_start = node;
        parent->child_upd_end = node;
        node->next = 0;
        node->prev = 0;

    }
}

void mx_remove_from_parent_ready(mx_node* node) {
    assert (node->state == MX_STATE_READY);
    mx_node* parent = node->parent;
    mx_node* prev = node->prev;
    mx_node* next = node->next;

    if (prev) {
        prev->next = next;
    } else {
        parent->child_ready_start = next;
    }

    if (next) {
        next->prev = prev;
    } else {
        parent->child_ready_end = prev;
    }
    node->next = 0;
    node->prev = 0;
    node->state = MX_STATE_NONE;
}
void mx_remove_from_parent_updated(mx_node* node) {
    assert (node->state == MX_STATE_UPDATED);
    mx_node* parent = node->parent;
    mx_node* prev = node->prev;
    mx_node* next = node->next;

    if (prev) {
        prev->next = next;
    } else {
        parent->child_upd_start = next;
    }

    if (next) {
        next->prev = prev;
    } else {
        parent->child_upd_end = prev;
    }
    node->next = 0;
    node->prev = 0;
    node->state = MX_STATE_NONE;
}
bool mx_try_push_to_parent_updated(mx_node* node) {
    if (node->state != MX_STATE_NONE) {
        return false;
    }
    mx_push_to_parent_updated(node);
    return true;
}

void mx_push_to_parent_ready_forced(mx_node* node) {
    if (node->state == MX_STATE_READY) {
        return;
    }
    if (node->state == MX_STATE_UPDATED) {
        mx_remove_from_parent_updated(node);
    }
    node->state = MX_STATE_NONE;
    mx_push_to_parent_ready(node);
}
bool mx_is_in_ready(mx_node* node) {
    return node->state == MX_STATE_READY;
}
bool mx_is_in_updated(mx_node* node) {
    return node->state == MX_STATE_UPDATED;
}
bool mx_is_in_queue(mx_node* node) {
    return node->state != MX_STATE_NONE;
}
bool mx_is_begin_ready(mx_node* node) {
    return node == node->parent->child_ready_start;
}
bool mx_is_begin_updated(mx_node* node) {
    return node == node->parent->child_upd_start;
}

void mx_pop_ready(mx_node* parent) {
    assert (parent->child_ready_start != 0);
    mx_node* old_begin = parent->child_ready_start;
    mx_node* new_begin = parent->child_ready_start->next;
    old_begin->state = MX_STATE_NONE;
    old_begin->next = 0;
    if (new_begin) {
        new_begin->prev = 0;
        parent->child_ready_start = new_begin;
    } else {
        parent->child_ready_start = 0;
        parent->child_ready_end = 0;
    }
}

void mx_pop_updated(mx_node* parent) {
    assert (parent->child_upd_start != 0);
    mx_node* old_begin = parent->child_upd_start;
    mx_node* new_begin = parent->child_upd_start->next;
    old_begin->state = MX_STATE_NONE;
    old_begin->next = 0;
    if (new_begin) {
        new_begin->prev = 0;
        parent->child_upd_start = new_begin;
    } else {
        parent->child_upd_start = 0;
        parent->child_upd_end = 0;
    }
}


mx_node* mx_current_ready(mx_node* node) {
    return node->child_ready_start;
}
mx_node* mx_current_updated(mx_node* node) {
    return node->child_upd_start;
}


void mx_sap_init(sap_t* sap, map_t* map) {
    sap->mx.parent = &map->mx;
    sap->mx.object = sap;
    sap->mx.object_type = MX_OBJ_SAP;
    sap->mx.state = MX_STATE_NONE;
    sap->mx.child_ready_start = 0;
    sap->mx.child_ready_end = 0;
    sap->mx.child_upd_start = 0;
    sap->mx.child_upd_end = 0;
    sap->mx.prev = 0;
    sap->mx.next = 0;
}

void mx_map_init(map_t* map, vc_t* vc) {
    map->mx.parent = &vc->mx;
    map->mx.object = map;
    map->mx.object_type = MX_OBJ_MAP;
    map->mx.state = MX_STATE_NONE;
    map->mx.child_ready_start = 0;
    map->mx.child_ready_end = 0;
    map->mx.child_upd_start = 0;
    map->mx.child_upd_end = 0;
    map->mx.prev = 0;
    map->mx.next = 0;
}

void mx_vc_init(vc_t* vc, mc_t* mc) {
    vc->mx.parent = &mc->mx;
    vc->mx.object = vc;
    vc->mx.object_type = MX_OBJ_VC;
    vc->mx.state = MX_STATE_NONE;
    vc->mx.child_ready_start = 0;
    vc->mx.child_ready_end = 0;
    vc->mx.child_upd_start = 0;
    vc->mx.child_upd_end = 0;
    vc->mx.prev = 0;
    vc->mx.next = 0;
}

void mx_mc_init(mc_t* mc, pc_t* pc) {   
    mc->mx.parent = &pc->mx;
    mc->mx.object = mc;
    mc->mx.object_type = MX_OBJ_MC;
    mc->mx.state = MX_STATE_NONE;
    mc->mx.child_ready_start = 0;
    mc->mx.child_ready_end = 0;
    mc->mx.child_upd_start = 0;
    mc->mx.child_upd_end = 0;
    mc->mx.prev = 0;
    mc->mx.next = 0;
}
void mx_pc_init(pc_t* pc) {   
    pc->mx.parent = 0;
    pc->mx.object = pc;
    pc->mx.object_type = MX_OBJ_PC;
    pc->mx.state = MX_STATE_NONE;
    pc->mx.child_ready_start = 0;
    pc->mx.child_ready_end = 0;
    pc->mx.child_upd_start = 0;
    pc->mx.child_upd_end = 0;
    pc->mx.prev = 0;
    pc->mx.next = 0;
}

sap_t* mx_get_sap(mx_node* node) {
    assert(node->object_type == MX_OBJ_SAP);
    return (sap_t*) node->object;
}
map_t* mx_get_map(mx_node* node) {
    assert(node->object_type == MX_OBJ_MAP);
    return (map_t*) node->object;
}
vc_t* mx_get_vc(mx_node* node) {
    assert(node->object_type == MX_OBJ_VC);
    return (vc_t*) node->object;
}
mc_t* mx_get_mc(mx_node* node) {
    assert(node->object_type == MX_OBJ_MC);
    return (mc_t*) node->object;
}
pc_t* mx_get_pc(mx_node* node) {
    assert(node->object_type == MX_OBJ_PC);
    return (pc_t*) node->object; 
}