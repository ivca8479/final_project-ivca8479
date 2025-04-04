/*
 * queue.h
 *
 *  Created on: Mar 24, 2025
 *      Author: ivanc
 *
 *
 *  @brief:
 */

#ifndef QUEUE_H_
#define QUEUE_H_
/*
* Circular Queue Implementation in order to create and interface with USART and transmit messages
*/
#include "stdint.h"
#include "stdio.h"
#include "stm32f091xc.h"
#define CBFIFO_CAPACITY (256)
//#define FULL	1
//#define EMPTY (1)


/*
 *
 */
typedef struct
{
	uint8_t queue[CBFIFO_CAPACITY];
	uint16_t write_pos;
	uint16_t read_pos;
	uint16_t length;
	uint8_t full;
	uint8_t empty;
}CBFIFO;

/*
 * @brief: function used to initialize a circular buffer.
 * @param: circular buffer data structure
 */

void queue_init(CBFIFO *cbfifo);
/*
 * Enqueue
 * Brief: Adds data into a circular buffer at its current write position. Correctly updates the write
 * position
 */
uint8_t enqueue(CBFIFO *cbfifo, uint8_t data);

/*
 * @brief: Takes away data from a buffer queue. Will update the current read position
 * @parameter: Circular buffer data structure we wish to extract data from. Will use the read position to extract
 * this value
 */
uint32_t dequeue(CBFIFO *cbfifo);
/*
 * @brief: Gets the current number of items inside the cbfifo
 * @parameters: CBFIFO data structure we wish to check the length of
 */
size_t queue_length(CBFIFO *cbfifo);
/*
 * @brief: Returns true if the cbfifo is full
 * @parameters: CBFIFO data structure we wish to check if it is in the full condition
 */
uint8_t queue_is_full(CBFIFO *cbfifo);
/*
 * @brief: Returns true if the cbfifo data structure is empty
 * @parameters: CBFIFO data structure we wish to check the empty condition
 */
uint8_t queue_is_empty(CBFIFO *cbfifo);


#endif /* QUEUE_H_ */
