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
                printf("%c", sReceivedChar);
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
