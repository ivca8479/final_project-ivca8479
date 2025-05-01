/*
 * timing.h
 *
 *  Created on: Apr 27, 2025
 *      Author: ivanc
 */

#ifndef TIMING_H_
#define TIMING_H_

#include "stm32f091xc.h"

/**
 * @brief Initializes the SysTick timer for 1ms intervals.
 *
 * Configures the SysTick peripheral to generate an interrupt every 100 milliseconds,
 */
void systick_init();
/*
 * @brief Delays based of how many 100ms ticks is wanted
 */
void delay(uint32_t ticks);

#endif /* TIMING_H_ */
