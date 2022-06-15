// test.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include "gps.h"
#include "serial.h"


serial * port;


void packet_callback(void* arg, const ubx_any_packet_t* packet_)
{
	//printf("ID: %ld ", packet_->pid);
	switch (packet_->pid)
	{
	case UBX_PID_NAV_SOL: {
		printf("UBX_PID_NAV_SOL\n");
		const ubx_navsol_packet_t * packet = &packet_->packet.navsol;
		printf("fix = %d\n", packet->gps_fix);
		} break;

	case UBX_PID_TIM_TP: {
		printf("UBX_PID_TIM_TP\n");
		} break;

	case UBX_PID_NAV_TIMEGPS: {
		printf("UBX_PID_NAV_TIMEGPS\n");
		} break;

	case UBX_PID_CFG_NACK: {
		printf("UBX_PID_CFG_NACK\n");
		} break;

	case UBX_PID_CFG_ACK: {
		printf("UBX_PID_CFG_ACK\n");
		} break;

	case UBX_PID_MON_HW2: {
		printf("UBX_PID_MON_HW2\n");
		const ubx_monhw2_packet_t* packet = &packet_->packet.monhw2;
		printf("ofsI = %d\n",       packet->ofsI);
		printf("magI = %d\n",       packet->magI);
		printf("ofsQ = %d\n",       packet->ofsQ);
		printf("magQ = %d\n",       packet->magQ);
		printf("cfgSource = %d\n",  packet->cfgSource);
		printf("lowLevCfg = %d\n",  packet->lowLevCfg);
		printf("postStatus = %d\n", packet->postStatus);
		} break;

	case UBX_PID_MON_HW: {
		printf("UBX_PID_MON_HW\n");
		const ubx_monhw_packet_t* packet = &packet_->packet.monhw;
		printf("pinSel = %d\n",     packet->pinSel);
		printf("pinBank = %d\n",    packet->pinBank);
		printf("pinDir = %d\n",     packet->pinDir);
		printf("pinVal = %d\n",     packet->pinVal);
		printf("noisePerMS = %d\n", packet->noisePerMS);
		printf("agcCnt = %d\n",     packet->agcCnt);
		printf("aStatus = %d\n",    packet->aStatus);
		printf("aPower = %d\n",     packet->aPower);
		printf("flags = %d\n",      packet->flags);
		printf("usedMask = %d\n",   packet->usedMask);
		for (uint8_t i = 0; i < 17; i++)
			printf("VP[%d] = %d\n", i, packet->VP[i]);

		printf("jamInd = %d\n",     packet->jamInd);
		printf("pinIrq = %d\n",     packet->pinIrq);
		printf("pullH = %d\n",      packet->pullH);
		printf("pullL = %d\n",      packet->pullL);
		} break;

	case UBX_PID_RXM_SVSI: {
		printf("UBX_PID_RXM_SVSI\n");
		const ubx_rxmsvsi_packet_t* packet = &packet_->packet.rxmsvsi;
		printf("iTOW = %d\n", packet->iTOW);
		printf("week = %d\n", packet->week);
		printf("numVis = %d\n", packet->numVis);
		printf("numSV = %d\n", packet->numSV);
		ubx_rxmsvsi_SV_packet_t SV_packet;
		for (uint8_t i = 0; i < ubx_parse_rxm_svsi_SV_num(packet); i++)
		{
			ubx_parse_rxm_svsi_SV(packet, i, &SV_packet);
			if (ubx_parse_rxm_svsi_SV_ura(&SV_packet) < 15)
			{
				printf("svid = %d\n", SV_packet.svid);
				printf("SV_ura = %d\n", ubx_parse_rxm_svsi_SV_ura(&SV_packet));
				printf("SV_healthy = %d\n", ubx_parse_rxm_svsi_SV_healthy(&SV_packet));
				printf("SV_ephVal = %d\n", ubx_parse_rxm_svsi_SV_ephVal(&SV_packet));
				printf("SV_almVal = %d\n", ubx_parse_rxm_svsi_SV_almVal(&SV_packet));
				printf("SV_notAvail = %d\n", ubx_parse_rxm_svsi_SV_notAvail(&SV_packet));
				printf("azim = %d\n", SV_packet.azim);
				printf("elev = %d\n", SV_packet.elev);
				printf("SV_almAge = %d\n", ubx_parse_rxm_svsi_SV_almAge(&SV_packet));
				printf("SV_ephAge = %d\n", ubx_parse_rxm_svsi_SV_ephAge(&SV_packet));
			}
		}
		} break;

	case UBX_PID_NAV_SVINFO:
		printf("UBX_PID_NAV_SVINFO\n"); {
		const ubx_navsvinfo_packet_t * packet = &packet_->packet.navsvinfo;
		printf("iTOW = %ld\n", (int64_t)packet->iTOW);
		printf("global_flags = %d\n", (int)packet->globalFlags);
		printf("numCh = %d\n", (int)packet->numCh);
		int chip_gen = ubx_parse_nav_svinfo_chipGen(packet);
		printf("chipGen = %d\n", chip_gen);

		const int channels_count = ubx_parse_nav_svinfo_CH_num(packet);
		for (int i = 0; i < channels_count; i++)
		{
			ubx_navsvinfo_CH_packet_t CH_packet;
			printf("ch_index = %d\n", i);
			ubx_parse_nav_svinfo_CH(packet, i, &CH_packet);
			printf("ch_id = %d\n", (int)CH_packet.chn);
			printf("sv_id = %d\n", (int)CH_packet.svid);
			printf("c_no = %d\n", (int)CH_packet.cno);
			printf("c_no = %d\n", (int)CH_packet.prRes);
			printf("ch_svUsed = %d\n", (int)ubx_parse_nav_svinfo_CH_svUsed(&CH_packet));
			printf("ch_diffCorr = %d\n", (int)ubx_parse_nav_svinfo_CH_diffCorr(&CH_packet));
			printf("ch_orbitAvail = %d\n", (int)ubx_parse_nav_svinfo_CH_orbitAvail(&CH_packet));
			printf("ch_orbitEph = %d\n", (int)ubx_parse_nav_svinfo_CH_orbitEph(&CH_packet));
			printf("ch_unhealthy = %d\n", (int)ubx_parse_nav_svinfo_CH_unhealthy(&CH_packet));
			printf("ch_orbitAlm = %d\n", (int)ubx_parse_nav_svinfo_CH_orbitAlm(&CH_packet));
			printf("ch_orbitAop = %d\n", (int)ubx_parse_nav_svinfo_CH_orbitAop(&CH_packet));
			printf("ch_smoothed = %d\n", (int)ubx_parse_nav_svinfo_CH_smoothed(&CH_packet));
			printf("ch_quality = %d\n", (int)ubx_parse_nav_svinfo_CH_quality(&CH_packet));
		}
		} break;

	default:
		printf("!!!!!!!!!!!!!!!!!!!11 invalid packet\n");
		break;
	}
}


int main()
{
	const char port_name[] = "/dev/ttyUSB1"; //COM6
	port = serial_open(port_name);

	gps_init(packet_callback, NULL);
	gps_configure_begin();

	while (1)
	{
		uint8_t buffer[100];
		int size = serial_read(port, buffer, sizeof(buffer));
		if (size < 0)
		{
			printf("unable to read from port");
			break;
		}

		for (size_t i = 0; i < size; i++)
			gps_consume_byte(buffer[i]);

		gps_poll();
	}
}
