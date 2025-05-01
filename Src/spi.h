/*
 * spi.h
 *
 *  Created on: Apr 22, 2025
 *      Author: ivanc
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f091xc.h"
#include "utilities.h"
#include "usart.h"

void spi_init();

uint8_t SPI_Send_Receive_Byte(uint8_t d_out);

#endif /* SPI_H_ */
