cmake_minimum_required(VERSION 3.0.0)
project(CCSDS VERSION 0.1.0)

add_library(CCSDS ccsds_endian.c endian.c ./epp/epp.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
