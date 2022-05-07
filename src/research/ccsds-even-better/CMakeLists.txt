cmake_minimum_required(VERSION 3.0.0)
project(CCSDS VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(CCSDS_TEST main.c)
add_subdirectory(ccsds)
target_link_libraries(CCSDS_TEST PRIVATE ccsds::uslp)
 
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
   