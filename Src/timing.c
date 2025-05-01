/*
 * timing.c
 *
 *  Created on: Apr 27, 2025
 *      Author: ivanc
 */
#include "timing.h"

#define F_SYS_CLK 			(48000000UL)				//48MHz Microcontroller Source clock
#define HIGHEST_PRIORITY	(0)							//the highest priority will be the systick at 0
#define TEN_HZ				(10)						//want 10 hz frequency inteerupt
#define TIMER_PERIOD		(100)						//100ms is the period of the timer used to convert the intervals to ms

volatile static uint32_t time;//used to keep track of how long it's been since a reset was called
volatile static uint32_t total_time;//used to keep track of the total time it's been since the train has been moving
volatile static uint32_t elapsed_delay;

void SysTick_Handler()
{
	// Gets updated every 0.1 seconds
	time++;
}

//initalize the systick with a 10hz frequency to have an interrupt every 1/10 seconds/100ms
void systick_init()
{
	SysTick->LOAD = ((F_SYS_CLK / TEN_HZ) - 1);
	//set interupt priority
	NVIC_SetPriority(SysTick_IRQn, HIGHEST_PRIORITY);
	//force load of reload value
	SysTick->VAL = 0;
	//enable interrupt, and pick the 48MHz clock source
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk
			| SysTick_CTRL_ENABLE_Msk;
}

void delay(uint32_t ticks)
{
	uint32_t start_time = time;
	// wait until amount of ticks that have passes is the same as the ticks given
	while ((time - start_time) < ticks)
		;
}
