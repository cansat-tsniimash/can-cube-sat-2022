cmake_minimum_required(VERSION 3.0.0)
project(CCSDS VERSION 0.1.0)

add_library(ccsds_uslp 
    src/map.c 
    src/mc.c 
    src/pc.c 
    src/receive.c 
    src/sap.c 
    src/sep.c 
    src/uslp_init.c 
    src/uslp_mx.c 
    src/vc_receive.c 
    src/vc.c 
    src/event_handler.c 
    src/stats.c
)
add_library(ccsds::uslp ALIAS ccsds_uslp)
target_include_directories(ccsds_uslp
    PUBLIC
        ./include
    PRIVATE 
        ./include/ccsds/uslp
        src
)
target_link_libraries(ccsds_uslp PUBLIC ccsds::generic ccsds::epp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
