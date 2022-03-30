#pragma once


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef enum {
    MX_OBJ_SAP,
    MX_OBJ_MAP,
    MX_OBJ_VC,
    MX_OBJ_MC,
    MX_OBJ_PC,
    MX_OBJ_MCF
} mx_object_type_t;

typedef enum {
    MX_STATE_NONE,
    MX_STATE_UPDATED,
    MX_STATE_READY,
} mx_state_t;

typedef struct mx_node mx_node;

typedef struct mx_queue_node mx_queue_node;

typedef struct mx_queue_node {
    mx_queue_node* parent;
    mx_queue_node* next;
    mx_queue_node* prev;
    mx_queue_node* begin;
    mx_queue_node* end;
    bool is_in_queue;
} mx_queue_node;

typedef struct mx_node {
    mx_state_t state;
    mx_object_type_t object_type;
    void* object;
    mx_node* parent;
    mx_node* prev;
    mx_node* next;
    mx_node* child_ready_start;
    mx_node* child_ready_end;
    mx_node* child_upd_start;
    mx_node* child_upd_end;
} mx_node;

typedef struct {
    void* (*_alloc)(size_t);
    void (*_free)(void*);
} _memory_t;

typedef struct {
    _memory_t mem;
} uslp_core_t;

typedef struct {
    uint8_t* data;
    size_t capacity;
    size_t size;
    size_t index;
} buffer_t;

typedef enum {
    MAP_TYPE_ACCESS,
    MAP_TYPE_PACKET,
    MAP_TYPE_OCTET,    
} map_type_t;

typedef enum {
    TFDZ_RULE_MAPP_SPAN = 0b000,
    TFDZ_RULE_MAPA_START = 0b001,
    TFDZ_RULE_MAPA_CONTINUE = 0b010,
    TFDZ_RULE_MAP_OCTET = 0b011,
    TFDZ_RULE_MAPAV_START = 0b100,
    TFDZ_RULE_MAPAV_CONTINUE = 0b101,
    TFDZ_RULE_MAPAV_LAST = 0b110,
    TDFZ_RULE_MAPAV_NO_SEGM = 0b111,
} tfdz_rule_t;

typedef enum {
    UPID_SP_OR_EP = 0b00000,
    UPID_COP1 = 0b00001,
    UPID_COPP = 0b00010,
    UPID_SDLS = 0b00011,
    UPID_OCTET = 0b00100,
    UPID_MAPA = 0b00101,
    UPID_PROX1_PPID1 = 0b00110,
    UPID_PROX1_SPDU = 0b00111,
    UPID_PROX1_PPID2 = 0b01000,
    UPID_IDLE = 0b11111,
} upid_t;

typedef enum {
    QOS_EXPEDITED,
    QOS_SEQ_CTRLD
} qos_t;

typedef struct map_t {
    map_type_t map_type;
    mx_node mx;
    uslp_core_t* uslp;
    buffer_t source_buffer;
    buffer_t tfdf;
    uint64_t tfdf_count;
    tfdz_rule_t tfdz_rule;
    upid_t upid;
    qos_t qos;
    uint16_t pointer_fh_lo;
    int map_id;
} map_t;

typedef struct {
    uint8_t* tfdz;
    size_t size;
    int map_id;
    tfdz_rule_t tfdz_rule;
    upid_t upid;
    uint16_t pointer_fh_lo;
} tfdf_t;

typedef struct {
    tfdf_t tfdf;
    uint64_t frame_count;
    qos_t qos;
} map_data_t;
 
typedef struct {
    uint64_t vc_frame_count;
    int vc_id;
} vc_data_t;

typedef struct {
    map_data_t map_data;
    vc_data_t vc_data;
    int ttl;
} vc_frame_t;

typedef enum {
    COP_NONE,
    COP_1,
    COP_P
} cop_enum_t;

typedef struct {
    uslp_core_t* uslp;
    vc_frame_t* arr;
    size_t size;
    size_t head;
    size_t tail;
    size_t cur;
    bool is_full;
    size_t tfdz_max_size;
} cop1_t;

typedef struct vc_t {
    mx_node mx;
    uslp_core_t* uslp; 
    uint64_t ex_frame_count;
    uint64_t sc_frame_count;  
    int vc_id;
    cop_enum_t cop_type; 
    int seq_ctrld_ttl;
    
    struct {
        struct {
            vc_frame_t frame; 
            bool is_full;
            size_t tfdz_size;
        } expedited_frame;
        
        union {
            cop1_t cop1;
            int copp; // TODO: copp struct
        } ;
    } container;
} vc_t;

typedef struct mc_t {
    mx_node mx;
    int sc_id;
    int tfvn;
    uint64_t frame_count;
} mc_t;



typedef struct {
    uint8_t ocf[4];
    int sc_id;
    int tfvn;
    bool ocf_valid;
    uint64_t frame_count;
} mc_data_t;

typedef struct {
    mc_t* start;
    mc_t* end;
} mc_mx_t;

typedef struct sap_t sap_t;

typedef struct sap_map_mx_t sap_map_mx_t;

typedef enum {
    SERVICE_MAPP,
    SERVICE_MAPA,
    SERVICE_MAP_OCTET,
    SERVICE_VCF,
    SERVICE_MCF,
    SERVICE_OCF,
    SERVICE_INSERT
} service_t;

typedef enum {
    POLICY_NONE,
    POLICY_SIZE_STRICT,
} policy_t;

typedef struct sap_t {
    mx_node mx;
    buffer_t data;
    bool is_empty;
    uslp_core_t* uslp;
    map_t* map;
    vc_t* vc;
    mc_t* mc;
    service_t service;
    policy_t policy;
    uint64_t packet_count;
} sap_t;


typedef struct {
    mx_node mx;
    mc_t* mc_idle;
    uint64_t frame_count;
    uint8_t* insert_data; 
    size_t insert_size;
} pc_t;


typedef struct {
    uint8_t* insert_data;
    size_t insert_size;
    uint8_t fec[4];
    bool fec_valid;
} pc_data_t;

