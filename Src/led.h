/*
 * led.h
 *
 *  Created on: Mar 26, 2025
 *      Author: ivanc
 *  @brief: This file provides abstraction to access the output led on the board. Used for led on and led off commands
 *  in the command processor
 */

#ifndef LED_H_
#define LED_H_

#include "stm32f091xc.h"
#include "utilities.h"

/*
 * @brief: function used to intiialize the user LED pin's GPIO as an output pin
 */
void led_init();
/*
 * Function used to turn the user led on, after this function is on the green led on the board should remain on
 */
void led_on();
/*
 * Function used to turn the user led off, after this function is on the green led on the board should remain on
 */
void led_off();


#endif /* LED_H_ */
