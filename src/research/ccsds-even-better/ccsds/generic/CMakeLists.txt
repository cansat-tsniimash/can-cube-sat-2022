cmake_minimum_required(VERSION 3.0.0)
project(CCSDS VERSION 0.1.0)

add_library(ccsds_generic src/buffer.c src/ccsds_endian.c src/endian.c src/endian2.c src/fifo.c)
add_library(ccsds::generic ALIAS ccsds_generic)

target_include_directories(ccsds_generic
    PUBLIC
        ./include
    PRIVATE 
        src
)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
