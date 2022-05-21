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
	return ubx_make_pid(packet_header[0], packet_header[1]);
}


uint16_t ubx_packet_payload_size(const uint8_t * packet_header)
{
	return ((uint16_t)packet_header[3] << 8) | (packet_header[2]);
}


uint16_t ubx_packet_checksum(const uint8_t * data_start, int data_size)
{
	uint8_t crc_a = 0, crc_b = 0;
	for (int i = 0; i < data_size; i++)		//алгоритм подсчета контрольной суммы
	{
		crc_a += data_start[i];
		crc_b += crc_a;
	}

	return ubx_uint16crc_make(crc_a, crc_b);
}


uint16_t ubx_uint16crc_make(uint8_t crc_a, uint8_t crc_b)
{
	return (uint16_t)crc_a << 8 | crc_b;
}


uint8_t ubx_uint16crc_get_crca(uint16_t crc16)
{
	return (uint8_t)((crc16 >> 8) & 0xFF);
}


uint8_t ubx_uint16crc_get_crcb(uint16_t crc16)
{
	return (uint8_t)(crc16 & 0xFF);
}


uint16_t ubx_make_pid(uint8_t packet_class, uint8_t packet_id)
{
	return ((uint16_t)packet_class << 8) | packet_id;
}



static uint32_t _read_u32(const uint8_t * data)
{
	return	  (uint32_t)data[3] << 3*8
			| (uint32_t)data[2] << 2*8
			| (uint32_t)data[1] << 1*8
			| (uint32_t)data[0] << 0*8
	;
}


static int32_t _read_i32(const uint8_t * data)
{
	uint32_t buffer = _read_u32(data);
	return *(int32_t*)&buffer;
}


static uint16_t _read_u16(const uint8_t * data)
{
	return	  (uint16_t)data[1] << 1*8
			| (uint16_t)data[0] << 0*8
	;
}


static int16_t _read_i16(const uint8_t* data)
{
	uint16_t buffer = _read_u16(data);
	return *(int16_t*)&buffer;
}


//TODO: дописать
static void _ubx_parse_nav_sol(const uint8_t * payload, ubx_any_packet_t * packet_)
{
	// обрезание пакета до 48 байт т.к. дальше идут зарезервированные поля
	ubx_navsol_packet_t * packet = &packet_->packet.navsol;

	packet->i_tow		= _read_u32(payload + 0);
	packet->f_tow		= _read_i32(payload + 4);
	packet->week		= _read_u16(payload + 8);
	packet->gps_fix		= *(payload + 10);
	packet->flags		= *(payload + 11);
	packet->pos_ecef[0] = _read_i32(payload + 12);
	packet->pos_ecef[1] = _read_i32(payload + 16);
	packet->pos_ecef[2] = _read_i32(payload + 20);
	packet->p_acc		= _read_u32(payload + 24);
	packet->vel_ecef[0] = _read_i32(payload + 28);
	packet->vel_ecef[1] = _read_i32(payload + 32);
	packet->vel_ecef[2] = _read_i32(payload + 36);
	packet->s_acc		= _read_u32(payload + 40);
	packet->p_dop		= _read_u16(payload + 44);
	// 1 байт резерва
	packet->num_sv		= *(payload + 47);
	// 4 байта в резерве
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
	packet->ftow = _read_i32(payload + 4);
	packet->week = _read_u16(payload + 8);
	packet->leaps = *((int8_t*)payload + 10);
	packet->valid_flags = *(payload + 11);
	packet->t_acc = _read_u32(payload + 12);
}


static void _ubx_parse_ack(const uint8_t * payload, ubx_any_packet_t * packet_)
{
	ubx_ack_packet_t * packet = &packet_->packet.ack;

	packet->packet_pid = ubx_make_pid(payload[0], payload[1]);
}


static void _ubx_parse_nack(const uint8_t * payload, ubx_any_packet_t * packet_)
{
	ubx_nack_packet_t * packet = &packet_->packet.nack;

	packet->packet_pid = ubx_make_pid(payload[0], payload[1]);
}


static void _ubx_parse_mon_hw2(const uint8_t* payload, ubx_any_packet_t* packet_)
{
	ubx_monhw2_packet_t* packet = &packet_->packet.monhw2;

	packet->ofsI         = *((int8_t*)payload + 0);
	packet->magI         =          *(payload + 1);
	packet->ofsQ         = *((int8_t*)payload + 2);
	packet->magQ         =          *(payload + 3);
	packet->cfgSource    =          *(payload + 4);
	packet->lowLevCfg    =  _read_u32(payload + 8);
	packet->postStatus   =  _read_u32(payload + 20);
}


static void _ubx_parse_mon_hw(const uint8_t* payload, ubx_any_packet_t* packet_)
{
	ubx_monhw_packet_t* packet = &packet_->packet.monhw;

	packet->pinSel     = _read_u32(payload + 0);
	packet->pinBank    = _read_u32(payload + 4);
	packet->pinDir     = _read_u32(payload + 8);
	packet->pinVal     = _read_u32(payload + 12);
	packet->noisePerMS = _read_u16(payload + 16);
	packet->agcCnt     = _read_u16(payload + 18);
	packet->aStatus    =         *(payload + 20);
	packet->aPower     =         *(payload + 21);
	packet->flags      =         *(payload + 22);
	packet->usedMask   = _read_u32(payload + 24);
	for (uint8_t i = 0; i < 17; i++)
	{
		packet->VP[i] = *(payload + 28 + i);
	}
	packet->jamInd     =         *(payload + 45);
	packet->pinIrq     = _read_u32(payload + 46);
	packet->pullH      = _read_u32(payload + 52);
	packet->pullL      = _read_u32(payload + 56);
}


static void _ubx_parse_rxm_svsi(const uint8_t* payload, ubx_any_packet_t* packet_)
{
	ubx_rxmsvsi_packet_t* packet = &packet_->packet.rxmsvsi;

	packet->iTOW      =  _read_u32(payload + 0);
	packet->week      =  _read_i16(payload + 4);
	packet->numVis    =          *(payload + 6);
	packet->numSV     =          *(payload + 7);
	packet->SVbuf_ptr = (uint8_t*)(payload + 8);
}


uint8_t ubx_parse_rxm_svsi_SV_num(ubx_rxmsvsi_packet_t packet)
{
	return packet.numSV;
}


int ubx_parse_rxm_svsi_SV(ubx_rxmsvsi_packet_t packet, uint8_t SV_index, ubx_rxmsvsi_SV_packet_t * SV_packet)
{
	if (SV_index < ubx_parse_rxm_svsi_SV_num(packet))
	{
		SV_packet->svid   =          *(packet.SVbuf_ptr + 0 + 6 * SV_index);
		SV_packet->svFlag =          *(packet.SVbuf_ptr + 1 + 6 * SV_index);
		SV_packet->azim   =  _read_i16(packet.SVbuf_ptr + 2 + 6 * SV_index);
		SV_packet->elev   = *((int8_t*)packet.SVbuf_ptr + 4 + 6 * SV_index);
		SV_packet->age    =          *(packet.SVbuf_ptr + 5 + 6 * SV_index);
		return 0;
	}
	return EADDRNOTAVAIL;
}


uint8_t ubx_parse_rxm_svsi_SV_ura(ubx_rxmsvsi_SV_packet_t SV_packet)
{
	return SV_packet.svFlag & 0x0F;
}


uint8_t ubx_parse_rxm_svsi_SV_healthy(ubx_rxmsvsi_SV_packet_t SV_packet)
{
	return (SV_packet.svFlag & (1 << 4)) >> 4;
}


uint8_t ubx_parse_rxm_svsi_SV_ephVal(ubx_rxmsvsi_SV_packet_t SV_packet)
{
	return (SV_packet.svFlag & (1 << 5)) >> 5;
}


uint8_t ubx_parse_rxm_svsi_SV_almVal(ubx_rxmsvsi_SV_packet_t SV_packet)
{
	return (SV_packet.svFlag & (1 << 6)) >> 6;
}


uint8_t ubx_parse_rxm_svsi_SV_notAvail(ubx_rxmsvsi_SV_packet_t SV_packet)
{
	return (SV_packet.svFlag & (1 << 7)) >> 7;
}


uint8_t ubx_parse_rxm_svsi_SV_almAge(ubx_rxmsvsi_SV_packet_t SV_packet)
{
	return SV_packet.age & 0x0F;
}


uint8_t ubx_parse_rxm_svsi_SV_ephAge(ubx_rxmsvsi_SV_packet_t SV_packet)
{
	return (SV_packet.age >> 4) & 0x0F;
}


static void _ubx_parse_nav_svinfo(const uint8_t* payload, ubx_any_packet_t* packet_)
{
	ubx_navsvinfo_packet_t* packet = &packet_->packet.navsvinfo;

	packet->iTOW        =  _read_u32(payload + 0);
	packet->numCh       =          *(payload + 4);
	packet->globalFlags =          *(payload + 5);
	packet->CHbuf_ptr   = (uint8_t*)(payload + 8);
}


uint8_t ubx_parse_nav_svinfo_CH_num(ubx_navsvinfo_packet_t packet)
{
	return packet.numCh;
}


int ubx_parse_nav_svinfo_CH(ubx_navsvinfo_packet_t packet, uint8_t CH_index, ubx_navsvinfo_CH_packet_t * CH_packet)
{
	if (CH_index < ubx_parse_nav_svinfo_CH_num(packet))
	{
		CH_packet->chn     =          *(packet.CHbuf_ptr + 0 + 12 * CH_index);
		CH_packet->svid    =          *(packet.CHbuf_ptr + 1 + 12 * CH_index);
		CH_packet->flags   =          *(packet.CHbuf_ptr + 2 + 12 * CH_index);
		CH_packet->quality =          *(packet.CHbuf_ptr + 3 + 12 * CH_index);
		CH_packet->cno     =          *(packet.CHbuf_ptr + 4 + 12 * CH_index);
		CH_packet->elev    = *((int8_t*)packet.CHbuf_ptr + 5 + 12 * CH_index);
		CH_packet->azim    =  _read_i16(packet.CHbuf_ptr + 6 + 12 * CH_index);
		CH_packet->prRes   =  _read_i32(packet.CHbuf_ptr + 8 + 12 * CH_index);
		return 0;
	}
	return EADDRNOTAVAIL;
}


uint8_t ubx_parse_nav_svinfo_chipGen(ubx_navsvinfo_packet_t packet)
{
	return packet.globalFlags & 0x07;
}


uint8_t ubx_parse_nav_svinfo_CH_svUsed(ubx_navsvinfo_CH_packet_t CH_packet)
{
	return CH_packet.flags & 0x01;
}


uint8_t ubx_parse_nav_svinfo_CH_diffCorr(ubx_navsvinfo_CH_packet_t CH_packet)
{
	return (CH_packet.flags & (1 << 1)) >> 1;
}


uint8_t ubx_parse_nav_svinfo_CH_orbitAvail(ubx_navsvinfo_CH_packet_t CH_packet)
{
	return (CH_packet.flags & (1 << 2)) >> 2;
}


uint8_t ubx_parse_nav_svinfo_CH_orbitEph(ubx_navsvinfo_CH_packet_t CH_packet)
{
	return (CH_packet.flags & (1 << 3)) >> 3;
}


uint8_t ubx_parse_nav_svinfo_CH_unhealthy(ubx_navsvinfo_CH_packet_t CH_packet)
{
	return (CH_packet.flags & (1 << 4)) >> 4;
}


uint8_t ubx_parse_nav_svinfo_CH_orbitAlm(ubx_navsvinfo_CH_packet_t CH_packet)
{
	return (CH_packet.flags & (1 << 5)) >> 5;
}


uint8_t ubx_parse_nav_svinfo_CH_orbitAop(ubx_navsvinfo_CH_packet_t CH_packet)
{
	return (CH_packet.flags & (1 << 6)) >> 6;
}


uint8_t ubx_parse_nav_svinfo_CH_smoothed(ubx_navsvinfo_CH_packet_t CH_packet)
{
	return (CH_packet.flags & (1 << 7)) >> 7;
}

uint8_t ubx_parse_nav_svinfo_CH_quality(ubx_navsvinfo_CH_packet_t CH_packet)
{
	return CH_packet.quality & 0x0F;
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

	case UBX_PID_CFG_ACK:
		_ubx_parse_ack(payload_start, packet);
		break;

	case UBX_PID_CFG_NACK:
		_ubx_parse_nack(payload_start, packet);
		break;

	case UBX_PID_MON_HW2:
		_ubx_parse_mon_hw2(payload_start, packet);
		break;

	case UBX_PID_MON_HW:
		_ubx_parse_mon_hw(payload_start, packet);
		break;

	case UBX_PID_RXM_SVSI:
		_ubx_parse_rxm_svsi(payload_start, packet);
		break;

	case UBX_PID_NAV_SVINFO:
		_ubx_parse_nav_svinfo(payload_start, packet);
		break;

	default:
		rc = -ENOSYS;
		break;
	};

	packet->pid = pid;
	return rc;
}

