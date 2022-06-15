/*
 * serial.h
 *
 *  Created on: 15 июн. 2022 г.
 *      Author: snork
 */

#ifndef ITS_SERVER_RADIO_SERIAL_H_
#define ITS_SERVER_RADIO_SERIAL_H_

#include <stdint.h>

struct serial;
typedef struct serial serial;

serial * serial_open(const char * name);
void serial_close(serial * port);
int serial_read(serial * port, uint8_t * buffer, int buffer_size);
int serial_write(serial * port, const uint8_t * buffer, int buffer_size);

#endif /* ITS_SERVER_RADIO_SERIAL_H_ */
