/*
 * Time.h
 *
 *  Created on: 10 мая 2019 г.
 *      Author: sereshotes
 */

#ifndef TIME_H_
#define TIME_H_

#include <stdint.h>

#include <stm32f1xx_hal.h>

void delay_us_init(void);

inline __attribute__((always_inline)) void delay_us(uint32_t us)
{
	volatile uint32_t startTick = DWT->CYCCNT;
	volatile uint32_t delayTicks = us * (SystemCoreClock/1000000);

	while (DWT->CYCCNT - startTick < delayTicks);
}

struct Time {
    uint32_t ms;
};

struct Time gettime();

#endif /* TIME_H_ */
