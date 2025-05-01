/*
 * usart.h
 *
 *  Created on: Mar 24, 2025
 *      Author: ivanc
 */

#ifndef USART_H_
#define USART_H_
#include "stm32f091xc.h"
#include "utilities.h"
#include "queue.h"

/*
 * Abstraction file to access USART functions needed to transmit and receive via USART interrupts
 */

/*
 *@brief: USART initialization needed for an interrupt approach. Initializes GPIO pins for usart, and the
 *usart itself for 8 data bits, 1 odd parity bit, and 2 stop bits
 */
void usart_init();

/*
 * @brief: Usart transmit function for abstraction to send a character to the transmitter buffer, which will then
 * be retrieved from an ISR to write to the serial terminal
 * @parameter: message to send to the transmit buffer, since only storing one letter at a time this will just be
 * a single character
 * @return none
 */
void usart_transmit(const char *buf, int num_chars);

/*
 * @brief: Usart receive function for abstraction to get a character from the receiver buffer
 * @parameter: pointer to an integer, will place the value from the receiver buffer into this integer buffer, since
 * this only gets one character at a time, this will only write the value into one character
 * @return: none
 */
void usart_receive(int *buf);

/**
 * @brief Checks if data is available from the USART receive queue.
 * If a character is available in the receiver queue, this function
 * dequeues it and returns true else returns false
 *
 * @parameter: ch Pointer to a variable where the received character will be stored.
 * @return 1 if data was available and stored in *ch, 0 if the queue was empty.
 */

uint8_t usart_check_ready(uint8_t *ch);

#endif /* USART_H_ */
