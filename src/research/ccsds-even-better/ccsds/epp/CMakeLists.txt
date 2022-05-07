cmake_minimum_required(VERSION 3.0.0)
project(CCSDS VERSION 0.1.0)

add_library(ccsds_epp src/epp.c)
add_library(ccsds::epp ALIAS ccsds_epp)

target_link_libraries(ccsds_epp PRIVATE ccsds_generic)
target_include_directories(ccsds_epp PUBLIC 
    PUBLIC
        ./include
    PRIVATE 
        src
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
