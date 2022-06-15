cmake_minimum_required(VERSION 3.16)


project(its-server-radio
	LANGUAGES C
)


add_executable(gps-test
	serial.h
	serial.c
	gps.c
	gps.h
	main.c
	neo6-cfg.c
	ubx_packet_parser.c
	ubx_packet_parser.h
	ubx_stream_parser.c
	ubx_stream_parser.h
)