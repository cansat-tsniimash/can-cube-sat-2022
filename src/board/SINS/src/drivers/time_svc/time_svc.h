/*
 * time_svc.h
 *
 *  Created on: 7 апр. 2020 г.
 *      Author: snork
 */

#ifndef DRIVERS_TIME_SVC_TIME_SVC_H_
#define DRIVERS_TIME_SVC_TIME_SVC_H_

#include <stdint.h>

#include <time.h>


//! Запуск службы мирового времени
/*! Эта служба времени запускается от RTC и затем корректируется по GPS */
int time_svc_world_init(void);

//! Получение мирового времени в эпохе UNIX для UTC+0 зоны
/*! Возвращает стандартный struct timeval как gettimeoftheday */
void time_svc_world_get_time(struct timeval * tv);

//! Запуск стабильной службы времени
int time_svc_steady_init(void);

//! Получение стабильного времени
/*! Возвращает микросекунды от запуска службы */
uint64_t time_svc_steady_get_time(void);




// Коррекция времени по GPS
// Таймеры, обеспечивающие внешний PPS и субсекундный счет сбрасываются по PPS
// сигналу аппаратно через соответсвующие пины микроконтроллера
// более глобальное время нужно проставлять в ручную


//! Синхронизация службы времени по PPS сигналу от GPS приёмника
/*! Таймеры TIM4 и TIM3 будут сброшены на 0 аппартно для обеспечения
 *  выровненого начала секунды. Этот же метод выставит правильные
 *  значения недели gps и миллиссекунд недели */
void time_svc_world_set_time(time_t the_time);

//! Время последней коррекции в steady шкале
uint64_t time_svc_last_adjust_timestamp(void);


#endif /* DRIVERS_TIME_SVC_TIME_SVC_H_ */