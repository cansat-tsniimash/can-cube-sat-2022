/*
 * usdl_types.h
 *
 *  Created on: 8 янв. 2021 г.
 *      Author: HP
 */

#ifndef SDL_USDL_USDL_TYPES_H_
#define SDL_USDL_USDL_TYPES_H_

#include <stdbool.h>
#include <inttypes.h>


#include "usdl_defines.h"
#include "usdl_basic_types.h"




typedef struct map_mx_t map_mx_t;
typedef struct map_t map_t;
typedef struct vc_t vc_t;
typedef struct vc_mx_t vc_mx_t;
typedef struct mc_t mc_t;
typedef struct mc_mx_t mc_mx_t;
typedef struct pc_t pc_t;


typedef struct map_t {
	map_id_t map_id;
	map_mx_t *map_mx;
	bool size_fixed;
	bool split_allowed;
	quality_of_service_t qos;

	tfdz_construction_rules_t flags;
	map_buffer_t buf_ex;
	map_buffer_t buf_sc;
	map_buffer_t buf_packet;
	quality_of_service_t packet_qos;
#if MAP_BUFFER_SIZE > 0 && defined(MAP_BUFFER_STATIC)
	//uint8_t buffer[MAP_BUFFER_SIZE];
	//size_t payload_size;
	//size_t buffer_index;
#elif !defined(MAP_BUFFER_STATIC)
	uint8_t *buffer;
	size_t buffer_index;
	size_t payload_size;
#endif
} map_t;



typedef struct map_mx_t {
	map_id_t map_active;
	enum map_mx_state_t state;
	map_t *map_arr[MAP_COUNT];
	vc_t *vc;
} map_mx_t;


#define FOP_DATA_SIZE 200
#define FOP_QUEUE_SIZE 10

typedef enum {
	FOP_STATE_ACTIVE,
	FOP_STATE_RETR_WO_WAIT,
	FOP_STATE_RETR_W_WAIT,
	FOP_STATE_INIT_WO_BCF,
	FOP_STATE_INIT_W_BCF,
	FOP_STATE_INITIAL,
} fop_state_t;


typedef struct {
	uint8_t data[FOP_DATA_SIZE];
	size_t size;
	map_params_t map_params;
	vc_params_t vc_params;
} queue_value_t;

typedef struct queue_element_t queue_element_t;

typedef struct queue_element_t{
	queue_value_t value;
	queue_element_t *next;
} queue_element_t;

typedef struct {
	queue_element_t buf[FOP_QUEUE_SIZE];
	size_t size;
	queue_element_t *empty;
	queue_element_t *first;
	queue_element_t *last;
} queue_t;

typedef struct {
	queue_t queue;
	fop_state_t state;
	size_t frame_count;
} fop_t;



typedef struct {

	transfer_frame_t transfer_frame_type;
	vc_id_t vc_id;
	vcf_count_length_t vcf_count_lenght_expedited;
	vcf_count_length_t vcf_count_lenght_seq_control;
	cop_in_effect_t cop_in_effect;
	clcw_version_t clcw_version;
	clcw_report_rate_t clcw_report_rate;
	map_t *map_set[16];
	uint32_t truncated_frame_length;
	union {
		bool ocf_allowed;
		bool ocf_required;
	};
	uint8_t repetition_max_user;
	uint8_t repetition_max_cop;
	uint32_t max_delay_to_release;
	uint32_t max_delay_between_frames;
} vc_parameters_t;

typedef struct vc_t {
	transfer_frame_t transfer_frame_type;
	vc_id_t vc_id;
	vcf_count_t frame_count_expedited;
	vcf_count_t frame_count_seq_control;
	sod_flag_t src_or_dest_id;
	vc_parameters_t vc_parameters;
	fop_t fop;
	vc_mx_t *vc_mx;
	map_mx_t *map_mx;
	uint32_t mapcf_length_ex;
	uint32_t mapcf_length_sc;
} vc_t;



typedef struct vc_mx_t {
	vc_t *vc_arr[VC_COUNT];
	vc_id_t vc_active;
	mc_t *mc;
} vc_mx_t;

typedef enum {
	MC_OCF_PROHIBTED,
	MC_OCF_ALLOWED,
	MC_OCF_REQUIRED
} mc_ocf_t;

typedef struct {
	uint16_t scid;
	transfer_frame_t tft;

	mc_ocf_t ocf_flag;
} mc_paramaters_t;


typedef struct mc_t {
	vc_mx_t *vc_mx;
	mc_id_t mc_id;
	mc_mx_t *mc_mx;
	mc_paramaters_t mc_parameters;
	uint8_t ocf[4];
	uint32_t vcf_length;
} mc_t;


typedef struct mc_mx_t {
	mc_t *mc_arr[MC_COUNT];
	mc_id_t mc_active;
	pc_t *pc;
} mc_mx_t;

typedef struct {
	uint16_t scid;
	transfer_frame_t tft;
	int tf_length;
	tfvn_t tfvn;
	uint8_t fec_length;
	bool is_fec_presented;
	bool can_generate_oid;

} pc_paramaters_t;

typedef struct pc_t {
	uint8_t *data;
	uint8_t size;
	int is_valid;
	mc_mx_t *mc_mx;
	pc_paramaters_t pc_parameters;
	uint8_t *insert_data;
	size_t insert_size;
	uint8_t fec_field[4];
	uint32_t mcf_length;
} pc_t;

#endif /* SDL_USDL_USDL_TYPES_H_ */