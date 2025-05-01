/*
 * usart.c
 *
 *  Created on: Mar 24, 2025
 *      Author: ivanc
 */


#include "usart.h"

/*
 * Settings used for the USART terminal
 */
#define BAUD_RATE	(9600)
#define NINE_DATA_BITS	(1)
#define ODD_PARITY		(1)
#define TWO_STOP_BITS	(2)
#define ENABLE	(1)
#define DISABLE (0)
#define USART_PRIORITY (2)
/*
* USART is in the APB clock, which is using 24MHz
*/
#define F_USART_CLOCK (24UL * 1000UL * 1000UL)
/*
* Static variables for the transmitter and receiver queue
*/
static CBFIFO transmitter;
static CBFIFO receiver;
/*
* Enabling the USART2 in interrupt mode, will be used for serial communication
*/
void usart_init()
{
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// GPIO A pin 2 and 3 in alternate function 1 (USART2)
	// Set mode field to 2 for alternate function
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER2, ESF_GPIO_MODER_ALT_FUNC);
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER3, ESF_GPIO_MODER_ALT_FUNC);
	// Select USART2 (AF = 1) as alternate function
	MODIFY_FIELD(GPIOA->AFR[0], GPIO_AFRL_AFSEL2, ENABLE);
	MODIFY_FIELD(GPIOA->AFR[0], GPIO_AFRL_AFSEL3, ENABLE);

	// 9600 baud rate
	USART2->BRR = F_USART_CLOCK/BAUD_RATE;
	// Enable Parity
	MODIFY_FIELD(USART2->CR1, USART_CR1_PCE, ENABLE);
	//	select no parity
	MODIFY_FIELD(USART2->CR1, USART_CR1_PS, 0);
	// 9 data bits
	MODIFY_FIELD(USART2->CR1, USART_CR1_M, 1);
	// 1 Stop bit
	MODIFY_FIELD(USART2->CR2, USART_CR2_STOP, 1);

	// Enable interrupt generation
	MODIFY_FIELD(USART2->CR1, USART_CR1_TXEIE, 1);
	MODIFY_FIELD(USART2->CR1, USART_CR1_RXNEIE, 1);

	// Enable transmitter, receiver and USART
	MODIFY_FIELD(USART2->CR1, USART_CR1_TE, 1);
	MODIFY_FIELD(USART2->CR1, USART_CR1_RE, 1);
	MODIFY_FIELD(USART2->CR1, USART_CR1_UE, 1);

	// Enable USART2 interrupts in NVIC
	NVIC_SetPriority(USART2_IRQn, 2);
	NVIC_ClearPendingIRQ(USART2_IRQn);
	NVIC_EnableIRQ(USART2_IRQn);

	// Initialize buffers
	queue_init(&transmitter);
	queue_init(&receiver);
}

/*
 * brief: USART IRQ handler used to handle USART interrupts. Checks for a receiver interrupt
 * or a transmitter interrupt where then it decides to to enqueue or dequeue data
 */
void USART2_IRQHandler(void)
{
	uint8_t ch;
	if (USART2->ISR & USART_ISR_RXNE)
	{
		// read from read register and enqueue character to receive buffer if its not full
		ch = USART2->RDR;
		if (!queue_is_full(&receiver))
		{
				enqueue(&receiver, ch);
		}
	}

	if (USART2->ISR & USART_ISR_TXE)
	{
		// put character to the transmit register from the transmitter buffer if there is a character available
		if (!queue_is_empty(&transmitter))
		{
				USART2->TDR = dequeue(&transmitter);
		}
		else
		{
			// disable TXE interrupts when full
			MODIFY_FIELD(USART2->CR1, USART_CR1_TXEIE, DISABLE);
		}
	}
}

uint8_t usart_check_ready(uint8_t *ch)
{
	// Check the reciever has a character at least
    if (!queue_is_empty(&receiver))
    {
    	// dequeue char from the reciever
        *ch = dequeue(&receiver);
        // return 1 if we found a char in reciever
        return 1;
    }
    return 0;
}


void usart_transmit(const char *buf, int num_chars)
{
    while (num_chars)
    {
        while (queue_is_full(&transmitter));
        enqueue(&transmitter, *buf);
        buf++;
        num_chars--;
    }
    MODIFY_FIELD(USART2->CR1, USART_CR1_TXEIE, ENABLE);
}

void usart_receive(int *buf)
{
    // if the queue is empty, wait until it is not empty using a while loop
		while (queue_is_empty(&receiver));
		// dequeue from the receiver and put into the buffer
		*buf = dequeue(&receiver);
		// increment location of the buffer
		buf++;
}

/*
 * @brief: To link the printf() call to the usart and print characters onto the USART terminal using printf
 * call
 */
int __io_putchar(int ch)
{
    char c = (char)ch;
    usart_transmit(&c, 1);
    return ch;
}

/*
 * @brief: To link the getchar() call to the usart and interact with the serial terminal using getchar() function
 * call
 */

int __io_getchar()
{
		int ch;
		// get the character from the receive buffer
		usart_receive(&ch);
		// return the retrieved character
		return ch;
}

