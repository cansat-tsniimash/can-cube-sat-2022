#include <stdio.h>

#include "uslp_init.h"

int main() {
    printf("Hello world\n");

    map_t map = {0};
    vc_t vc = {0};
    mc_t mc = {0};
    pc_t pc = {0};
    sap_t sap = {0};
    uslp_core_t uslp = {0};

    map_config_t map_config = {0};
    map_config.map_id = 13;
    map_config.tfdz_capacity = 10;

    vc_config_t vc_config = {0};
    vc_config.vc_id = 45;
    vc_config.cop_type = COP_NONE;
    vc_config.tfdz_max_size = 10;
    vc_config.cop_queue_count = 1;
    
    mc_config_t mc_config = {0};
    mc_config.mc_id = 8347;
    
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


    uint8_t data[] = "Hello World! What a beautiful day outside! Birds are singing, flowers are flowering...";
    sap_send(&sap, data, sizeof(data));
    

}