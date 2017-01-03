/**
 * @file circularBuffer.c
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Fifo circular queue adt.
 * 
 * The implementation uses a front pointer for the head of the queue and a back pointer for the tail. 
 * The tail must point to the first empty location to simplify the implementation, therefore the 
 * effective capacity of the queue is arraySize - 1.
 * 
 */
 
#include "circularBuffer.h"

static inline size_t remainingCapacity(struct circularBuffer * buffer) {
	return (buffer->size - buffer->arraySize - 1)
}

int buffer_attachArray(struct circularBuffer * buffer, void * arrayStart, size_t arraySize) {
	if (buffer == NULL || arraySize == 0) {
		return BUFFER_STATUS_ERROR;
	}
	
	buffer->size = 0;
	buffer->arraySize = arraySize;
	buffer->start = arrayStart;
	buffer->end = arrayStart + arraySize;
	buffer->front = 0;
	buffer->back = 0;
	
	return BUFFER_STATUS_OK;
}

int buffer_enqueue(struct circularBuffer * buffer, void * element, size_t count) {
	if (count > remainingCapacity(buffer)) {
		return BUFFER_STATUS_FULL;
	}
	
}
void * buffer_dequeue(struct circularBuffer * buffer);

size_t buffer_size(struct circularBuffer * buffer) {
	if (buffer == NULL) {
		return 0;
	}
	return buffer->size;
}
