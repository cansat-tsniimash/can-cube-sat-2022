#pragma once

#include "uslp_types.h"

mx_node* mx_get_parent(mx_node* node);

mx_queue_node* mx_queue_begin(mx_queue_node* node);
void mx_queue_pop_first(mx_queue_node* node);

void mx_queue_push_to_parent(mx_queue_node* node);
void mx_queue_remove_from_parent(mx_queue_node* node);
void mx_push_to_parent_ready(mx_node* node);
void mx_push_to_parent_updated(mx_node* node);
void mx_remove_from_parent_ready(mx_node* node);
void mx_remove_from_parent_updated(mx_node* node);
bool mx_try_push_to_parent_updated(mx_node* node);
void mx_push_to_parent_ready_forced(mx_node* node);

bool mx_is_in_ready(mx_node* node);
bool mx_is_in_updated(mx_node* node);
bool mx_is_in_queue(mx_node* node);
bool mx_is_begin_ready(mx_node* node);
bool mx_is_begin_updated(mx_node* node);

void mx_pop_ready(mx_node* parent);
void mx_pop_updated(mx_node* parent);

mx_node* mx_current_ready(mx_node* node);
mx_node* mx_current_updated(mx_node* node);

void mx_sap_init(sap_t* sap, map_t* map);
void mx_map_init(map_t* map, vc_t* vc);
void mx_vc_init(vc_t* vc, mc_t* mc);
void mx_mc_init(mc_t* mc, pc_t* pc);
void mx_pc_init(pc_t* pc);

sap_t* mx_get_sap(mx_node* node);
map_t* mx_get_map(mx_node* node);
vc_t* mx_get_vc(mx_node* node);
mc_t* mx_get_mc(mx_node* node);
pc_t* mx_get_pc(mx_node* node);