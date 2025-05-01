/*
 * spi.c
 *
 *  Created on: Apr 22, 2025
 *      Author: ivanc
 */

#include "spi.h"

/*
 * Clocks: Processor = 48 Mhz. AHB = 48 MHz. APB = 24 MHz.
 *
 * Want 6MHz clock speed for the sensor. SPI1 is using the APB2 Clock (24Mhz)
 *
 * Baud_Rate = 3MHz = 24MHz/2^(BR + 1) = 24MHz/(2 + 1), BR = 2
 *
 * For this code used Dean Chapter 8 on SPI
 */

#define F_SPI_CLOCK	(24UL * 1000UL * 1000UL)
#define BR		(3)

void spi_init()
{
	// Enable Port Clock for SPI
	RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);
	// Enable Ports for GPIOS needed for SPI
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
	// Set AF mode for PA15  SS
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER15, ESF_GPIO_MODER_ALT_FUNC);
	// Select SPI1
	MODIFY_FIELD(GPIOA->AFR[1], GPIO_AFRH_AFSEL15, 0);
	// Set GPIO pins to AF mode
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER3, 2);
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER4, 2);
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER5, 2);
	// Set MISO, MOSI, SCK pins
	MODIFY_FIELD(GPIOB->AFR[0], GPIO_AFRL_AFSEL3, 0);
	MODIFY_FIELD(GPIOB->AFR[0], GPIO_AFRL_AFSEL4, 0);
	MODIFY_FIELD(GPIOB->AFR[0], GPIO_AFRL_AFSEL5, 0);

	// Set NSS Pin to standard GPIO output mode
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER15, ESF_GPIO_MODER_OUTPUT);

	// Clock is divided by 3
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_BR, BR);
	// Select as Master Mode
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_MSTR, 1);
//	MODIFY_FIELD(SPI1->CR2, SPI_CR2_SSOE, 1);

// Data is sampled on rising edge
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_CPHA, 0);
	// Data is idle when Low
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_CPOL, 0);

	// The most significant byte must be sent first
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_LSBFIRST, 0);

	// Data is 8 bits long
	MODIFY_FIELD(SPI1->CR2, SPI_CR2_DS, 7);

	MODIFY_FIELD(SPI1->CR2, SPI_CR2_FRXTH, 1);

	MODIFY_FIELD(SPI1->CR2, SPI_CR2_NSSP, 1);

	// Enable SPI
	SPI1->CR1 |= SPI_CR1_SPE;
}

uint8_t SPI_Send_Receive_Byte(uint8_t d_out)
{
	uint8_t d_in;
	// Wait until transmitter buffer is empty
	while ((SPI1->SR & SPI_SR_TXE) == 0)
		;

	*((uint8_t*) &(SPI1->DR)) = d_out;

	while ((SPI1->SR & SPI_SR_RXNE) == 0)
		;

	d_in = (uint8_t) SPI1->DR;

	return d_in;
}

