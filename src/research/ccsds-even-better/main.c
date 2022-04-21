#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "ccsds/uslp/uslp_init.h"

void print_byte_arr(const uint8_t* arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", arr[i]);
    }
}

void print_bit_byte(uint8_t val) {
    for (int i = 0; i < 8; i++) {
        printf("%d", val & 1);
        val >>= 1;
    }
}

void print_bit_byte_arr(const uint8_t* arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        print_bit_byte(arr[i]);
        printf(" ");
    }
}

void map_cb(void* arg, const uint8_t* data, size_t size) {
    epp_packet_t packet = {0};
    if (epp_extract_packet(&packet, data, size)) {
        printf("DEAD\n");
        return;
    }
    printf("%*s", size, data);
    //printf("%d %d %d %d %d\n", packet.header.ccsds_defined_field, packet.header.epp_id, packet.header.epp_id_ex, packet.header.lol);

}

int main() {
    printf("Hello world\n");

    map_t map = {0};
    vc_t vc = {0};
    mc_t mc = {0};
    pc_t pc = {0};
    sap_t sap = {0};
    uslp_core_t uslp = {0};
    sep_t sep = {0};

    map_config_t map_config = {0};
    map_config.map_id = 13;
    map_config.tfdz_capacity = 10;

    vc_config_t vc_config = {0};
    vc_config.vc_id = 45;
    vc_config.cop_type = COP_NONE;
    vc_config.tfdz_max_size = 10;
    vc_config.cop_queue_count = 1;
    vc_config.ex_frame_count_length = 2;
    vc_config.sc_frame_count_length = 3;
    
    mc_config_t mc_config = {0};
    mc_config.sc_id = 8347;
    mc_config.sc_id_is_destination = false;
    mc_config.ocf_is_used = false;
    
    pc_config_t pc_config = {0};
    pc_config.tfvn = 12;
    pc_config.is_fec_presented = false;
    pc_config.insert_size = 0;

    sap_config_t sap_config = {0};
    sap_config.service = SERVICE_MAPA;
    sap_config.data_capacity = 200;


    uslp_init(&uslp, malloc, free);
    pc_init(&uslp, &pc, &pc_config);
    mc_init(&uslp, &mc, &pc, &mc_config);
    vc_init(&uslp, &vc, &mc, &vc_config);
    map_init(&uslp, &map, &vc, &map_config);
    sap_init(&uslp, &sap, &map, &sap_config);
    sep_init(&uslp, &sep, &pc);

    sap_receive_t sapr = {0};
    vc_receive_t vcr = {0};

    sap_receive_config_t sapr_config = {0};
    sapr_config.demx_entry_count = 1;
    sapr_config.insert_size = 0;
    sapr_config.is_fec_presented = false;
    sapr_config.tfvn = 12;

    vc_receive_config_t vcr_config = {0};
    vcr_config.cop_type = COP_NONE;
    vcr_config.map_cb = map_cb;
    vcr_config.map_cb_arg = 0;
    vcr_config.map_id = 13;
    vcr_config.pvn = PVN_EP;
    vcr_config.upid = UPID_MAPA;
    vcr_config.vc_id = 45;
    vcr_config.sc_id = 8347;
    vcr_config.sc_is_destination = false;
    vcr_config.sapr = &sapr;

    sap_receive_init(&uslp, &sapr, &sapr_config);
    vc_receive_init(&uslp, &vcr, &vcr_config);



    uint8_t data[] = "Hello World! What a beautiful day outside! Birds are singing, flowers are flowering...";
    sap_send(&sap, data, sizeof(data));
    size_t end_size = 0;
#define Period 1 //ms
    time_t prev = time(0) - Period;
    while (1) {
            
        uint8_t end_data[100] = {0};
        end_size = sep_get_data(&sep, end_data, sizeof(end_data), false);
        if (end_size == 0) {
            break;
        }
        sep_push_data(&sapr, end_data, end_size);
        // print_byte_arr(end_data, end_size);
        // printf(" | ");
        // for (size_t i = 0; i < end_size; i++) {
        //     printf("%c", isprint(end_data[i]) ? (char)end_data[i] : '.');
        // }
        // printf("\n");
    } 

}