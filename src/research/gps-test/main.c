// test.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <windows.h>
#include <stdio.h>

#include "gps.h"

HANDLE hSerial;


void packet_callback(void* arg, const ubx_any_packet_t* packet)
{
    printf("ID: %ld ", packet->pid);
    switch (packet->pid)
    {
    case UBX_PID_NAV_SOL:
        printf("UBX_PID_NAV_SOL\n");
        break;
    case UBX_PID_TIM_TP:
        printf("UBX_PID_TIM_TP\n");
        break;
    case UBX_PID_NAV_TIMEGPS:
        printf("UBX_PID_NAV_TIMEGPS\n");
        break;
    case UBX_PID_CFG_NACK:
        printf("UBX_PID_CFG_NACK\n");
        break;
    case UBX_PID_CFG_ACK:
        printf("UBX_PID_CFG_ACK\n");
        break;
    case UBX_PID_MON_HW2:
        printf("UBX_PID_MON_HW2\n");
        break;
        ubx_monhw2_packet_t* packet_monhw2 = &packet->packet.monhw2;
        printf("ofsI = %d\n",       packet_monhw2->ofsI);
        printf("magI = %d\n",       packet_monhw2->magI);
        printf("ofsQ = %d\n",       packet_monhw2->ofsQ);
        printf("magQ = %d\n",       packet_monhw2->magQ);
        printf("cfgSource = %d\n",  packet_monhw2->cfgSource);
        printf("lowLevCfg = %d\n",  packet_monhw2->lowLevCfg);
        printf("postStatus = %d\n", packet_monhw2->postStatus);
        break;
    case UBX_PID_MON_HW:
        printf("UBX_PID_MON_HW\n");
        break;
        ubx_monhw_packet_t* packet_monhw = &packet->packet.monhw;
        printf("pinSel = %d\n",     packet_monhw->pinSel);
        printf("pinBank = %d\n",    packet_monhw->pinBank);
        printf("pinDir = %d\n",     packet_monhw->pinDir);
        printf("pinVal = %d\n",     packet_monhw->pinVal);
        printf("noisePerMS = %d\n", packet_monhw->noisePerMS);
        printf("agcCnt = %d\n",     packet_monhw->agcCnt);
        printf("aStatus = %d\n",    packet_monhw->aStatus);
        printf("aPower = %d\n",     packet_monhw->aPower);
        printf("flags = %d\n",      packet_monhw->flags);
        printf("usedMask = %d\n",   packet_monhw->usedMask);
        for (uint8_t i = 0; i < 17; i++)
        {
            printf("VP[%d] = %d\n", i, packet_monhw->VP[i]);
        }
        printf("jamInd = %d\n",     packet_monhw->jamInd);
        printf("pinIrq = %d\n",     packet_monhw->pinIrq);
        printf("pullH = %d\n",      packet_monhw->pullH);
        printf("pullL = %d\n",      packet_monhw->pullL);
        break;
    case UBX_PID_RXM_SVSI:
        printf("UBX_PID_RXM_SVSI\n");
        ubx_rxmsvsi_packet_t* packet_rxmsvsi = &packet->packet.rxmsvsi;
        printf("iTOW = %d\n", packet_rxmsvsi->iTOW);
        printf("week = %d\n", packet_rxmsvsi->week);
        printf("numVis = %d\n", packet_rxmsvsi->numVis);
        printf("numSV = %d\n", packet_rxmsvsi->numSV);
        ubx_rxmsvsi_SV_packet_t SV_packet;
        for (uint8_t i = 0; i < ubx_parse_rxm_svsi_SV_num(*packet_rxmsvsi); i++)
        {
            ubx_parse_rxm_svsi_SV(*packet_rxmsvsi, i, &SV_packet);
            if (ubx_parse_rxm_svsi_SV_ura(SV_packet) < 15)
            {
                printf("svid = %d\n", SV_packet.svid);
                printf("SV_ura = %d\n", ubx_parse_rxm_svsi_SV_ura(SV_packet));
                printf("SV_healthy = %d\n", ubx_parse_rxm_svsi_SV_healthy(SV_packet));
                printf("SV_ephVal = %d\n", ubx_parse_rxm_svsi_SV_ephVal(SV_packet));
                printf("SV_almVal = %d\n", ubx_parse_rxm_svsi_SV_almVal(SV_packet));
                printf("SV_notAvail = %d\n", ubx_parse_rxm_svsi_SV_notAvail(SV_packet));
                printf("azim = %d\n", SV_packet.azim);
                printf("elev = %d\n", SV_packet.elev);
                printf("SV_almAge = %d\n", ubx_parse_rxm_svsi_SV_almAge(SV_packet));
                printf("SV_ephAge = %d\n", ubx_parse_rxm_svsi_SV_ephAge(SV_packet));
            }
        }
        break;
    default:
        printf("invalid packet\n");
        break;
    }
}

int main()
{
    const char sPortName[] = "\\\\.\\COM4";
    hSerial = CreateFileA(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        DWORD last_error = GetLastError();
        if (last_error == ERROR_FILE_NOT_FOUND)
        {
            printf("serial port does not exist.\n");
        }
        printf("some other error occurred. %i\n", last_error);
    }
    else 
    {
        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams))
        {
            printf("getting state error\n");
        }
        dcbSerialParams.BaudRate = CBR_9600;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        if (!SetCommState(hSerial, &dcbSerialParams))
        {
            printf("error setting serial port state\n");
        }
        COMMTIMEOUTS timeout;
        timeout.ReadIntervalTimeout = 50;
        timeout.ReadTotalTimeoutMultiplier = 50;
        timeout.ReadTotalTimeoutConstant = 50;
        timeout.WriteTotalTimeoutMultiplier = 50;
        timeout.WriteTotalTimeoutConstant = 50;
        SetCommTimeouts(hSerial, &timeout);
        gps_packet_callback_t gps_packet_callback = packet_callback;
        gps_init(gps_packet_callback, NULL);
        gps_configure_begin();
        while (1)
        {
            DWORD iSize;
            char sReceivedChar;
            ReadFile(hSerial, &sReceivedChar, 1, &iSize, 0);
            if (iSize > 0)
            {
                //printf("%c", sReceivedChar);
                gps_consume_byte((uint8_t)sReceivedChar);
            }
                
            gps_poll();
        }
    }
}

    // Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
    // Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

    // Советы по началу работы 
    //   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
    //   2. В окне Team Explorer можно подключиться к системе управления версиями.
    //   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
    //   4. В окне "Список ошибок" можно просматривать ошибки.
    //   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
    //   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
