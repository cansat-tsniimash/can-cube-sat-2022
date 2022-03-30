#include <stdio.h>
#include <time.h>

#include "uslp_init.h"

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
    
    mc_config_t mc_config = {0};
    mc_config.sc_id = 8347;
    mc_config.tfvn = 1;
    
    pc_config_t pc_config = {0};

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


    uint8_t data[] = "Hello World! What a beautiful day outside! Birds are singing, flowers are flowering...";
    sap_send(&sap, data, sizeof(data));
    size_t end_size = 0;
#define Period 1 //ms
    time_t prev = time(0) - Period;
    while (1) {
            
        uint8_t end_data[100] = {0};
        end_size = sep_get_data(&sep, end_data, sizeof(end_data), true);
        if (end_size == 0) {
            break;
        }
        printf("%s\n", end_data);
        
        
    } 

}