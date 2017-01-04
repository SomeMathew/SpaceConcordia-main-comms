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
	return (buffer_size(buffer) - buffer->arraySize - 1);
}

int buffer_attachArray(struct circularBuffer * buffer, uint8_t * arrayStart, size_t arraySize) {
	if (buffer == NULL || arraySize == 0) {
		return BUFFER_STATUS_ERROR;
	}
	
	buffer->size = 0;
	buffer->arraySize = arraySize;
	buffer->mem = arrayStart;
	buffer->front = 0;
	buffer->back = 0;
	
	return BUFFER_STATUS_OK;
}

size_t buffer_enqueue(struct circularBuffer * buffer, uint8_t * elements, size_t count) {
	size_t i;
	for (i = 0; i < count && remainingCapacity(buffer) > 0; i++) {
		buffer->mem[buffer->back] = elements[i];
		buffer->back = (buffer->back + 1) % buffer->arraySize;
	}
	
	return i;
}

size_t buffer_dequeue(struct circularBuffer * buffer, uint8_t * out, size_t count) {
	size_t i;
	for (i = 0; i < count && buffer_size(buffer) > 0; i++) {
		out[i] = buffer->mem[buffer->front];
		buffer->front = (buffer->front + 1) % buffer->arraySize;
	}
	return i;
}

size_t buffer_peakLinear(struct circularBuffer * buffer, uint8_t ** startOut) {
	*startOut = buffer->mem + buffer->front;
	size_t size;
	// Always count data in non circular fashion, if wrapped around only count front to arrayEnd
	if (buffer->front <= buffer->back) {
		size = buffer_size(buffer);
	} else {
		size = buffer->arraySize - buffer->front;
	}
	
	buffer->peakLinearSize = size;
	return buffer->peakLinearSize;
}

void buffer_advance(struct circularBuffer * buffer) {
	buffer->front = (buffer->front + buffer_peakSize(buffer)) % buffer->arraySize;
	buffer->peakLinearSize = 0;
}
