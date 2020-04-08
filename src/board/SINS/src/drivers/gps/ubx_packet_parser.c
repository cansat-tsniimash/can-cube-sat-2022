/*
 * ubx_packets.c
 *
 *  Created on: Apr 4, 2020
 *      Author: snork
 */

#include "ubx_packet_parser.h"

#include <errno.h>


uint16_t ubx_packet_pid(const uint8_t * packet_header)
{
	return ((uint16_t)packet_header[1] << 8) | packet_header[0];
}


uint16_t ubx_packet_payload_size(const uint8_t * packet_header)
{
	return ((uint16_t)packet_header[3] << 8) | (packet_header[2]);
}


uint16_t ubx_packet_payload_expected_size(ubx_pid_t pid)
{
	switch (pid)
	{
	case UBX_PID_NAV_SOL:
		return 52;

	case UBX_PID_TIM_TP:
		return 16;

	case UBX_PID_NAV_TIMEGPS:
		return 16;

	default:
		return 0;
	}
}



static uint32_t _read_u32(const uint8_t * data)
{
	return	  (uint32_t)data[3] << 3*8
			| (uint32_t)data[2] << 2*8
			| (uint32_t)data[1] << 1*8
			| (uint32_t)data[0] << 0*8
	;
}


static uint32_t _read_u16(const uint8_t * data)
{
	return	  (uint16_t)data[1] << 1*8
			| (uint16_t)data[0] << 0*8
	;
}


//TODO: дописать
static void _ubx_parse_nav_sol(const uint8_t * payload, ubx_any_packet_t * packet_)
{
	// обрезание пакета до 48 байт т.к. дальше идут зарезервированные поля
	//сбор mavlink пакета
}


// время GPS
static void _ubx_parse_tim_tp(const uint8_t * payload, ubx_any_packet_t * packet_)
{
	ubx_timtp_packet_t * packet = &packet_->packet.timtp;

	packet->tow_ms = _read_u32(payload);
	packet->week = _read_u16(payload + 12);		// +12 т.к. время в неделях с 12 по 14 байт
}


static void _ubx_parse_nav_timegps(const uint8_t * payload, ubx_any_packet_t * packet_)
{
	ubx_gpstime_packet_t * packet = &packet_->packet.gpstime;

	packet->tow_ms = _read_u32(payload);
	packet->week = _read_u16(payload + 8);		// +8 т.к. время в неделях с 8 по 10 байт
}


int ubx_parse_any_packet(const uint8_t * packet_start, ubx_any_packet_t * packet)
{
	uint16_t pid = ubx_packet_pid(packet_start);
	const uint8_t * payload_start = packet_start + UBX_HEADER_SIZE;
	int rc = 0;

	switch (pid)
	{
	case UBX_PID_NAV_SOL:
		_ubx_parse_nav_sol(payload_start, packet);
		break;

	case UBX_PID_NAV_TIMEGPS:
		_ubx_parse_nav_timegps(payload_start, packet);
		break;

	case UBX_PID_TIM_TP:
		_ubx_parse_tim_tp(payload_start, packet);
		break;

	default:
		rc = -ENOSYS;
		return -ENOSYS;
	};

	packet->pid = pid;
	return rc;
}

