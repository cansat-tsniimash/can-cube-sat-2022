# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(app-template)


set(EXTRA_COMPONENT_DIRS "components/imi"
                         "components/internet"
                         "components/my_i2c"
                         "components/ark_time_sync"
                         "its_config"
                         "router"
                         "mavlink_comp"
                         "uart_mavlink"
                         "ntp_server"
                         "radio"
                         "time_sync"
                         "sdio"
                         "shift_reg"
                         "control_heat"
                         "control_magnet"
                         "control_vcc"
                         "owb"
                         "ds18b20"
                         "sensors"
                         "log_collector"
                         "operator"
                         "components/operator2"
                         "msp")
                         
