idf_component_register(SRCS "./Src/send.c" "./sx126x/src/sx126x_drv.c" "./sx126x/src/sx126x_api.c" "./Src/sx126x_board.c" "./Src/ring_buffer.c" "./Src/timing_calc.c" "./Src/radio_help.c"
                       INCLUDE_DIRS "./Inc" "./sx126x/inc"
                       REQUIRES router its_config mavlink_comp log_collector)