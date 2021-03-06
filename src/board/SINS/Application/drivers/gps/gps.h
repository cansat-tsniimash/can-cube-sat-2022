/*
 * gps.h
 *
 *  Created on: 14 мар. 2020 г.
 *      Author: developer
 */

#ifndef DRIVERS_GPS_H_
#define DRIVERS_GPS_H_

#include "ubx_stream_parser.h"

#include <stm32f4xx_hal.h>

#include "main.h"


extern UART_HandleTypeDef huart2;

//! Хендл для уарта для GPS
#define GPS_BUS_HANDLE (&huart2)

//! Колбек для передачи gps сообщений
/*! Сюда передаются все сообщения, даже ACK NACK конфигурации
 * Почему не ubx_sparser_packet_callback_t? Потому что инкапсуляция! */
typedef ubx_sparser_packet_callback_t gps_packet_callback_t;


//! Инициализация драйвера GPS
/*! Настройка нужной перефирии и потокового парсера GPS */
int gps_init(
		gps_packet_callback_t packet_callback, void * packet_callback_arg
);

//! Обработка накопленных в прерывании байт и поиск в них сообщений.
/*! Передает сообщения через указанный ранее колбек. Возвращает количество байт, которое было опрошено */
int gps_poll(void);

//! Начать конфигурацию GPS
/*! Конфигурация оформлена в неблокирующем режиме и работает во
 * время вызовов gps_poll. Результат конфигурации можно получить при помощи
 * функции gps_configure_status */
void gps_configure_begin(void);

//! Текущий статус конфигурации
/*! Если в данный момент конфигурация еще не закончилась - возвращает код
 * -EWOULDBLOCK. Если конфигурация закончилась - вовзращает код последней ошибки
 * из-за которой конфигурация была остановлена, или же 0, если ошибок не было */
int gps_configure_status(void);

//! Эту функцию полагается дергать по получению байтика от GPS
void gps_consume_byte(uint8_t byte);

//! Эту функцию полагается дергать в прерывании от PPS сигнала
void gps_pps_signal_handler(void);

#endif /* DRIVERS_GPS_H_ */
