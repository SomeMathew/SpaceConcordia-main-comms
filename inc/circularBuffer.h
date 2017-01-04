/**
 * @file circularBuffer.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Fifo circular queue adt.
 * 
 * To use create a struct circularBuffer and use buffer_attachArray to attach a fixed capacity mem
 * location for the buffer.
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <stddef.h>
#include <stdint.h>

enum buffer_status {
	BUFFER_STATUS_OK,
	//~ BUFFER_STATUS_FULL,
	BUFFER_STATUS_ERROR,
};

struct circularBuffer {
    size_t size;
    size_t arraySize;
    uint8_t * mem;
    size_t front;
    size_t back;
    size_t peakLinearSize;
};

int buffer_attachArray(struct circularBuffer * buffer, uint8_t * arrayStart, size_t arraySize);
//~ int buffer_detachArray(struct circularBuffer * buffer); // Not currently needed


/**
 * @brief Enqueue count of elements from element at the rear of the buffer.
 * 
 * @return the number of elements successfully added to the buffer.
 */
size_t buffer_enqueue(struct circularBuffer * buffer, uint8_t * element, size_t count);

/**
 * @brief dequeue count of elements into the out location.
 * 
 * @return the number of elements removed by the buffer.
 */
size_t buffer_dequeue(struct circularBuffer * buffer, uint8_t * out, size_t count);

/**
 * @brief get the maximal linearly consecutive elements possible in the memory.
 * 
 * Once the elements are handled by the user application, advanceLinear() should be called to dequeue
 * the elements and free the space.
 * 
 * This function can be dangerous as it doesn't copy the elements, only points to the sequence 
 * in memory. It is useful for a quicker access to the buffer.
 * 
 * @param startOut Pointer will be set to point to the first element in the sequence.
 * @return number of consecutive elements pointed to by startOut
 */
size_t buffer_peakLinear(struct circularBuffer * buffer, uint8_t ** startOut);

/**
 * @brief Advance the front pointer by the amount of memory last peaked by peakLinear().
 */
void buffer_advanceLinear(struct circularBuffer * buffer);

/**
 * @brief Size of last peak that hasn't been advanced.
 */
static inline size_t buffer_peakSize(struct circularBuffer * buffer) {
	return buffer->peakLinearSize;	
}

static inline size_t buffer_size(struct circularBuffer * buffer) {
	return buffer->size;
}

#endif /* CIRCULARBUFFER_H_ */
