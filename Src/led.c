/*
 * led.c
 *
 *  Created on: Mar 26, 2025
 *      Author: ivanc
 */

#include "led.h"

#define ULED_PORT			(GPIOA)
#define ULED_PIN			(5)
#define MASK(x)				(1 << x)

/*
 * @brief: Initializes the clock for the user LED, and sets the output led as gpio's in output mode to
 * turn the LED on and off
 */
void led_init()
{
	// enable clock for user led located in GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//set ULED GPIO as output
	MODIFY_FIELD(ULED_PORT->MODER, GPIO_MODER_MODER5, ESF_GPIO_MODER_OUTPUT);
}

/*
 * @brief: Turns on the user led
 */
void led_on()
{
	//writing a 1 into output data register at pin of user LED
	ULED_PORT->ODR |= (MASK(ULED_PIN));
}

/*
 * @brief: function used to turn an led off
 */
void led_off()
{
	//writing a 0 or removing a 1 fromt the output data register at pin of user LED to get rid of LED
	ULED_PORT->ODR &= ~(MASK(ULED_PIN));
}
