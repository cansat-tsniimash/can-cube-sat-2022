/*
 * main.c
 *
 *  Created on: 15 янв. 2021 г.
 *      Author: HP
 */

#include <ccsds/sdl/usdl/usdl.h>
#include <stdio.h>
#include <endian.h>
#include <ccsds/nl/epp/epp.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ccsds/sdl/usdl/facade_static.h>

void print_bit(uint8_t num) {
	for(int i = 0; i < 8; ++i) {
		printf("%u", num >> (7 - i) & 1);
	}
}


void print_bit_arr(uint8_t *arr, size_t size) {
	for (size_t i = 0; i < size; i++) {
		print_bit(arr[i]);
		printf(" ");
	}
}

//#define MAIN_MAPP

typedef enum {
	USLPS_MAPP,
	USLPS_MAPA,
	USLPS_VCF,
} uslp_service_t;

typedef struct uslp_t {
	pc_t pc;
	mc_t mc;
	vc_t vc;
	map_t map;
} uslp_t;
#define FRAME_SIZE 100
#define PACKET_SIZE 1000
USDL_STATIC_ALLOCATE(10, 4, 1, PACKET_SIZE, FRAME_SIZE) usdl1_static;
usdl_t usdl1 = {0};
USDL_STATIC_ALLOCATE(10, 4, 1, PACKET_SIZE, FRAME_SIZE) usdl2_static;
usdl_t usdl2 = {0};
int main() {
	USDL_STATIC_INIT(usdl1_static, usdl1);
	USDL_STATIC_INIT(usdl2_static, usdl2);

	{
		pc_paramaters_t pc_p = {0};
		pc_p.can_generate_oid = 0;
		pc_p.is_fec_presented = 0;
		pc_p.tf_length = FRAME_SIZE;
		pc_p.tft = TF_FIXED;
		pc_p.tfvn = 0x0C;
		usdl_pc_init(&usdl1, &pc_p);
		usdl_pc_init(&usdl2, &pc_p);
	}
	{
		mc_paramaters_t mc_p = {0};
		mc_p.ocf_flag = 0;
		mc_p.scid = 0x1234;
		mc_p.tft = TF_FIXED;
		usdl_mc_init(&usdl1, &mc_p, 0);
		usdl_mc_init(&usdl2, &mc_p, 0);
	}
	{
		vc_parameters_t vc_p = {0};
		vc_p.clcw_report_rate = 0;
		vc_p.clcw_version = 0;
		vc_p.cop_in_effect = COP_NONE;
		vc_p.ocf_allowed = 0;
		vc_p.repetition_max_cop = 1;
		vc_p.repetition_max_user = 1;
		vc_p.transfer_frame_type = TF_FIXED;
		vc_p.vc_id = 0;
		vc_p.vcf_count_lenght_expedited = 1;
		vc_p.vcf_count_lenght_seq_control = 1;

		usdl_vc_init(&usdl1, &vc_p, 0, 0);
		usdl_vc_init(&usdl2, &vc_p, 0, 0);
	}

	{
		usdl_mapp_init(&usdl1, 0, 0, 0);
		usdl_mapa_init(&usdl1, 0, 0, 1);
		usdl_mapp_init(&usdl2, 0, 0, 0);
		usdl_mapa_init(&usdl2, 0, 0, 1);
	}

	uint8_t packet[PACKET_SIZE];

	uint8_t packet2[PACKET_SIZE];
	for (int i = 0; i < PACKET_SIZE; i++) {
		packet[i] = i % 256;
	}
	sap_t mapa01s = usdl_get_map_sap(&usdl1, 0, 0, 1);
	sap_t mapa01r = usdl_get_map_sap(&usdl2, 0, 0, 1);

	size_t sent = 0;
	size_t recieved = 0;

	uint8_t *pcs_data = 0;
	size_t *pcs_size;


	while (sent < sizeof(packet) || recieved < sizeof(packet)) {
		printf("\n");
		sent += usdl_mapa_send(mapa01s, packet + sent, sizeof(packet) - sent, QOS_EXPEDITED);

		uint8_t frame[FRAME_SIZE];
		usdl_pc_get_frame(&usdl1, &pcs_data, &pcs_size);
		if (usdl_pc_is_frame_buf_empty(&usdl2)) {
			memcpy(frame, pcs_data, *pcs_size);
			size_t f_size = *pcs_size;
			usdl_pc_set_frame_buf_empty(&usdl1, 1);
			usdl_pc_set_frame(&usdl2, frame, f_size);
		}

		quality_of_service_t qos = 0;
		recieved += usdl_mapa_recieve(mapa01r, packet2 + recieved, sizeof(packet2) - recieved, &qos);
		printf("Sent: %d \t Received: %d \n", (int)sent, (int)recieved);
	}
	for (int i = 0; i < recieved; i++) {
		printf("%02X ", packet2[i]);
	}

}
/*
void main_uslp_init(uslp_t *uslp, size_t tf_length, uslp_service_t service, size_t max_packet_size) {
	pc_paramaters_t pc_p = {0};
	pc_p.can_generate_oid = 0;
	pc_p.is_fec_presented = 0;
	pc_p.tf_length = tf_length;
	pc_p.tft = TF_FIXED;
	pc_p.tfvn = 0x0C;
	uint8_t *pc_arr = malloc(tf_length);
	pc_init(&uslp->pc, &pc_p, pc_arr, tf_length);

	mc_mx_init(&uslp->mc_mx, &uslp->pc);

	mc_paramaters_t mc_p = {0};
	mc_p.ocf_flag = 0;
	mc_p.scid = 0x1234;
	mc_p.tft = TF_FIXED;
	mc_init(&uslp->mc, &uslp->mc_mx, &mc_p, 0);

	vc_mx_init(&uslp->vc_mx, &uslp->mc);

	vc_parameters_t vc_p = {0};
	vc_p.clcw_report_rate = 0;
	vc_p.clcw_version = 0;
	vc_p.cop_in_effect = COP_NONE;
	vc_p.ocf_allowed = 0;
	vc_p.repetition_max_cop = 1;
	vc_p.repetition_max_user = 1;
	vc_p.transfer_frame_type = TF_FIXED;
	vc_p.vc_id = 0;
	vc_p.vcf_count_lenght_expedited = 1;
	vc_p.vcf_count_lenght_seq_control = 1;
	vc_init(&uslp->vc, &uslp->vc_mx, &vc_p, 0);

	map_mx_init(&uslp->map_mx, &uslp->vc);

	uint8_t *map_buffer = malloc(tf_length);
	uint8_t *map_buffer_p = malloc(max_packet_size);
	uslp->map.buf_ex.data = map_buffer;
	uslp->map.buf_ex.max_size = tf_length;
	uslp->map.mapr.tfdz.data = map_buffer;
	uslp->map.mapr.tfdz.max_size = tf_length;
	uslp->map.mapr.packet.data = map_buffer_p;
	uslp->map.mapr.packet.max_size = max_packet_size;
	if (service == USLPS_MAPA) {
		mapa_init(&uslp->map, &uslp->map_mx, 0x01);
	} else {
		mapp_init(&uslp->map, &uslp->map_mx, 0x01);
	}
}

void test_send_recieve(uslp_t *us, uslp_t *ur, uint8_t *data, size_t size, uslp_service_t service) {
	int s = 0;
	int sent = 0;
	int recieved = 0;
	while (sent < size || recieved < size) {
		if (service == USLPS_MAPP) {
			s = mapp_send(&us->map, &data[sent], size - sent, PVN_ENCAPSULATION_PROTOCOL, QOS_EXPEDITED);
		} else {
			s =	mapa_send(&us->map, &data[sent], size - sent, QOS_EXPEDITED);
		}


		sent += s;
		pc_request_from_down(&us->pc);
		if (us->pc.is_valid) {
			for (int i = 0; i < us->pc.pc_parameters.tf_length && s; i++) {
				printf("0x%x ", us->pc.data[i]);
			}
			printf("\n");
			if (pc_parse(&ur->pc, us->pc.data, us->pc.pc_parameters.tf_length)) {
				us->pc.is_valid = 0;
			}
		}

		quality_of_service_t qos = 0;
		if (service == USLPS_MAPP) {
			s = mapp_recieve(&ur->map.mapr, data, size, &qos);
		} else {
			s = mapa_recieve(&ur->map.mapr, data, size, &qos);
		}
		recieved += s;
		if (s) {
			break;
		}
	}
	if (s) {
		for (int i = 0; i < s; i++) {
			printf("0x%x ", data[i]);
		}
		printf("\n");
	}
}

typedef struct {
	int parse_fault_count;
	int packet_recieved;
} test_t;

test_t test_recieve(uslp_t *ur, uint8_t *data, size_t frame_length, size_t frame_count,
		uint8_t *dest, size_t d_size, uslp_service_t service) {
	int s = 0;
	int recieved = 0;
	test_t test = {0};
	for (int i = 0; i < frame_count; i++) {
		uint8_t *frame = &data[i * frame_length];

		if (!pc_parse(&ur->pc, frame, frame_length)) {
			test.parse_fault_count++;
		}

		quality_of_service_t qos = 0;
		if (service == USLPS_MAPP) {
			s = mapp_recieve(&ur->map.mapr, dest, d_size - recieved, &qos);
		} else {
			s = mapa_recieve(&ur->map.mapr, dest, d_size - recieved, &qos);
		}
		recieved += s;
		if (s) {
			test.packet_recieved++;
		}
	}
	return test;
}



int main() {
	const int frame_length = 142;
	const int size = 2000;
	uint8_t pl_arr[size];
	memset(pl_arr, 0, size);
	int pl_size = size - 4;
	const uslp_service_t test_service = USLPS_MAPA;
	printf("HELLO\n");
	uslp_t us = {0};
	uslp_t ur = {0};
	map_t *mappr;
	map_t *mapar;
	main_uslp_init(&us, frame_length, test_service, size);
	main_uslp_init(&ur, frame_length, USLPS_MAPA, size);

	{
		static map_t map;
		size_t tf_length = frame_length;
		size_t max_packet_size = size;

		uint8_t *map_buffer = malloc(tf_length);
		uint8_t *map_buffer_p = malloc(max_packet_size);
		map.buf_ex.data = map_buffer;
		map.buf_ex.max_size = tf_length;
		map.mapr.tfdz.data = map_buffer;
		map.mapr.tfdz.max_size = tf_length;
		map.mapr.packet.data = map_buffer_p;
		map.mapr.packet.max_size = max_packet_size;
		mapp_init(&map, &ur.map_mx, 0x02);
		mappr = &map;
	}
	mapar = &ur.map;

	const char filename[] = "input2";
	int fid = open(filename, O_RDONLY | O_BINARY);

	if (fid < 0) {
		fprintf(stderr, "can't open file\n");
		fflush(stderr);
		return -1;
	}
	pl_size = 0;
	while (1) {
		int s = read(fid, &pl_arr[pl_size], size - pl_size);
		if (s < 0) {
			fprintf(stderr, "read error\n");
			fflush(stderr);
			return -1;
		} else if (s) {
			pl_size += s;
		} else {
			break;
		}
	}
	//test_t test = test_recieve(&ur, pl_arr, frame_length, pl_size / frame_length, dest, size, USLPS_MAPA);
	//printf("%d %d\n", test.packet_recieved, test.parse_fault_count);
	{
		uint8_t dest[size];
		uint8_t *data = pl_arr;
		int frame_count = pl_size / frame_length;
		int s = 0;
		int recieved = 0;
		test_t test = {0};
		for (int i = 0; i < frame_count; i++) {
			uint8_t *frame = &data[i * frame_length];

			if (!pc_parse(&ur.pc, frame, frame_length)) {
				test.parse_fault_count++;
			}

			quality_of_service_t qos = 0;
			s = mapp_recieve(&mappr->mapr, dest, size, &qos);
			if (s) {
				recieved += s;
				test.packet_recieved++;
				printf("MAPP: ");
				for (int i = 0; i < s; i++) {
					printf("0x%02X ", dest[i]);
				}
				printf("\n");
			}

			s = mapa_recieve(&mapar->mapr, dest, size, &qos);
			if (s) {
				recieved += s;
				test.packet_recieved++;
				printf("MAPP: ");
				for (int i = 0; i < s; i++) {
					printf("0x%2X", dest[i]);
				}
				printf("\n");
			}
		}

		const char filename[] = "output";
		int fid = open(filename, O_WRONLY | O_BINARY | O_CREAT);

		if (fid < 0) {
			fprintf(stderr, "can't open file\n");
			fflush(stderr);
			return -1;
		}
		write(fid, dest, recieved);
	}



	/*
	epp_header_t epp_header = {0};
	epp_header.epp_id = 1;


	if (test_service == USLPS_MAPP) {
		int epp_size = epp_make_header_auto_length2(&epp_header, pl_arr, sizeof(pl_arr), pl_size);
		for (int i = 0; i < pl_size; i++) {
			pl_arr[i + epp_size] = i + 1;
		}
		pl_size += epp_size;
	} else {
		for (int i = 0; i < pl_size; i++) {
			pl_arr[i] = i + 1;
		}
	}

	test_send_recieve(&us, &ur, pl_arr, pl_size, test_service);
	printf("\n\nHELLO3\n");

/*
	uint8_t arr0[] = {0b11001010, 0b00110101};
	uint8_t arr1[4] = {0};

	uint8_t val = 0;
	for (int i = 3; i < 12; i++) {
		val = _stream_get_bit(arr0, 11, i, ENDIAN_LSBIT_MSBYTE);
		printf("%d", (int)val);
	}
	printf("\n");
	endian_stream_set(arr1, sizeof(arr1) * 8, 6, ENDIAN_LSBIT_LSBYTE,
					  arr0, 13, 1, ENDIAN_MSBIT_LSBYTE);
	print_bit_arr(arr1, sizeof(arr1));

	return 0;
}*/

