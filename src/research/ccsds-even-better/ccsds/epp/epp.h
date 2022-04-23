/*
 * epp.h
 *
 *  Created on: 17 янв. 2021 г.
 *      Author: HP
 */

#ifndef INCLUDE_CCSDS_NL_EPP_EPP_H_
#define INCLUDE_CCSDS_NL_EPP_EPP_H_

#include <inttypes.h>

typedef enum {
	EPP_ID_IDLE 		= 0x0,
	EPP_ID_EXTENSION 	= 0x6,
	EPP_ID_USER_DEFINED = 0x7,
} epp_id_t;

typedef struct {
	uint8_t pvn;
	epp_id_t epp_id;
	uint8_t lol;
	uint8_t udf;
	uint8_t epp_id_ex;
	uint16_t ccsds_defined_field;
	uint32_t packet_length;
} epp_header_t;

typedef struct {
	epp_header_t header;
	uint8_t* data;
	size_t size;
} epp_packet_t;

uint8_t epp_calc_min_lol_packet(size_t packet_length);

uint8_t epp_calc_min_lol_payload(size_t payload_length);

int epp_serialize_header(const epp_header_t *epp_header, uint8_t *arr, int size);

int epp_extract_header(epp_header_t *epp_header, const uint8_t *arr, int size);

size_t epp_serialize_packet(const epp_packet_t* packet, uint8_t* arr, size_t size);

int epp_extract_packet(epp_packet_t* packet, uint8_t* arr, size_t size);


#endif /* INCLUDE_CCSDS_NL_EPP_EPP_H_ */
