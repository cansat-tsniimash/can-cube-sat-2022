#include "serial.h"


// C library headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#ifdef __linux__

// Вдохновлено https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()


struct serial
{
	int fd;
};


static int configure_port(serial * port)
{
	struct termios tty;
	if(tcgetattr(port->fd, &tty) != 0) {
		perror("unable to get serial port params");
		return 1;
	}

	// Выключаем парити
	tty.c_cflag &= ~PARENB;
	// Один стоп бит
	tty.c_cflag &= ~CSTOPB;
	// 8 бит на символ
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	// Выключаем flow control
	tty.c_cflag &= ~CRTSCTS;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	// Мы не модем
	tty.c_cflag |= CLOCAL;
	// Мы хотим читать из порта
	tty.c_cflag |= CREAD;

	// мы работает в неканоне (не ждем конца строки на входе)
	tty.c_lflag &= ~ICANON;
	// Глушим все это
	tty.c_lflag &= ~ECHO;
	tty.c_lflag &= ~ECHOE;
	tty.c_lflag &= ~ECHONL;
	// Глушим спецсимволы и всякие конверсии
	tty.c_lflag &= ~ISIG;
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

	// хитрый неблокирующий режим
	tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;

	// бауд 9600
	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);

	if (tcsetattr(port->fd, TCSANOW, &tty) != 0) {
		perror("unable to set parameters for serial port");
		return 2;
	}

	return 0;
}


serial * serial_open(const char * name)
{
	int fd = open(name, O_RDWR);
	if (fd <= 0)
	{
		perror("unable to open port");
		return NULL;
	}

	serial * port = malloc(sizeof(serial));
	if (NULL == port)
	{
		perror("unable to alloc serial structure");
		close(fd);
		return NULL;
	}
	port->fd = fd;

	int rc = configure_port(port);
	if (0 != rc)
	{
		close(port->fd);
		free(port);
	}
	return port;
}


void serial_close(serial * port)
{
	if (NULL == port)
		return;

	close(port->fd);
	free(port);
}


int serial_read(serial * port, uint8_t * buffer, int buffer_size)
{
	return read(port->fd, buffer, buffer_size);
}


int serial_write(serial * port, const uint8_t * buffer, int buffer_size)
{
	return write(port->fd, buffer, buffer_size);
}


#else
#include <windows.h>


struct serial
{
	HANDLE h;
};


serial * serial_open(const char * port_name)
{
	const char prefix[] = "\\\\.\\"; // Важно для винды
	char port_name_buffer[100] = {0};
	strcat(port_name_buffer, prefix);
	strncat(port_name_buffer, port_name, sizeof(port_name_buffer) - sizeof(prefix) - 1); // -1 на терминатор

	HANDLE hSerial = CreateFileA(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		DWORD last_error = GetLastError();
		if (last_error == ERROR_FILE_NOT_FOUND)
		{
			printf("serial port does not exist.\n");
		}
		printf("some other error occurred. %i\n", last_error);
		return NULL;
	}
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


	serial * port = malloc(sizeof(serial));
	if (NULL == port)
	{
		perror("unable to alloc serial structure");
		CloseHandle(hSerial);
		return NULL;
	}
	port->h = hSerial;
	return port;
}


void serial_close(serial * port)
{
	if (NULL == port)
		return;

	CloseHandle(port->h);
	free(port);
}


int serial_read(serial * port, uint8_t * buffer, int buffer_size)
{
	DWORD iSize = 0;
	ReadFile(hSerial, (char*)buffer, buffer_size, &iSize, NULL);
	return iSize; // FIXME: Проверка на ошибки чтения
}


int serial_write(serial * port, const uint8_t * buffer, int buffer_size)
{
	DWORD iSize;
	WriteFile(hSerial, (char*)buffer, buffer_size, &iSize, NULL);
	return iSize; // FIXME: Проверка на ошибки записи
}


#endif
