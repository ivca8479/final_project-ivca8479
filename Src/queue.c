/*
 * queue.c
 *
 *  Created on: Mar 24, 2025
 *      Author: ivanc
 */
#include "queue.h"
#define ERROR	(-1)
#define SUCCESS (1)

#define FALSE (0)
#define TRUE	(1)

void queue_init(CBFIFO *cbfifo)
{
	for (int i = 0; i < CBFIFO_CAPACITY; i++) //set all values in the buffer to o
	{
		cbfifo->queue[i] = 0;
	}
	//set everything to the right starting position
	cbfifo->read_pos = 0;
	cbfifo->length = 0;
	cbfifo->write_pos = 0;
	cbfifo->empty = TRUE;
	cbfifo->full = FALSE;
}
uint8_t enqueue(CBFIFO *cbfifo, uint8_t data)
{
	if (!(cbfifo->full))
	{
		cbfifo->queue[cbfifo->write_pos] = data;
		// disable interrupts
		uint32_t masking_state = __get_PRIMASK();
		__disable_irq();
		cbfifo->write_pos = (cbfifo->write_pos + 1) & (CBFIFO_CAPACITY - 1); //upate write position in circular fashion, when it reaches position 512 this will become index 0 again and go through each index again
		cbfifo->length++;
		if (cbfifo->length == CBFIFO_CAPACITY) //if length is at capacity then the buffer is full, set flasg to indicate it is full
		{
			cbfifo->full = TRUE;
		}
		// enabple interrupts
		__set_PRIMASK(masking_state);
		cbfifo->empty = 0;
		return SUCCESS;
	}
	return ERROR;
}
uint32_t dequeue(CBFIFO *cbfifo)
{
	uint8_t data = 0;
	if (!(cbfifo->empty))           //check we can dequeu from buffer
	{
		data = cbfifo->queue[cbfifo->read_pos];
		cbfifo->queue[cbfifo->read_pos] = 0;               //set byte to 0/empty
		uint32_t masking_state = __get_PRIMASK();
		__disable_irq();
		cbfifo->read_pos = (cbfifo->read_pos + 1) & (CBFIFO_CAPACITY - 1); //update read position in next circular index
		cbfifo->length--;
		if (cbfifo->length == 0) //if it is detected length is 0, no more values in fifo, then put fifo empty flag
		{
			// change state of emptyness
			cbfifo->empty = TRUE;
		}
		__set_PRIMASK(masking_state);
		cbfifo->full = 0;
	}
	return data;
}

size_t queue_length(CBFIFO *cbfifo)
{
	return cbfifo->length;    //return length of cbfifo which is global variable
}
uint8_t queue_is_full(CBFIFO *cbfifo)
{
	// check if fifo data structure is full
	return cbfifo->full;
}
uint8_t queue_is_empty(CBFIFO *cbfifo)
{
	// check if the current fifo is empty
	return cbfifo->empty;
}

