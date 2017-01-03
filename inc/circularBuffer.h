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

enum buffer_status {
	BUFFER_STATUS_OK,
	BUFFER_STATUS_FULL,
	BUFFER_STATUS_ERROR,
}

struct circularBuffer {
    size_t size;
    size_t arraySize
    void * start;
    void * end; // array[arraySize]
    size_t front;
    size_t back;
};

int buffer_attachArray(struct circularBuffer * buffer, void * arrayStart, size_t arraySize);
//~ int buffer_detachArray(struct circularBuffer * buffer); // Not currently needed



/**
 * @brief Enqueue count of elements from element at the rear of the queue.
 * 
 * @return BUFFER_STATUS_FULL if the count of elements would exceed the remaining capacity.
 */
int buffer_enqueue(struct circularBuffer * buffer, void * element, size_t count);
void * buffer_dequeue(struct circularBuffer * buffer, size_t count);
size_t buffer_size(struct circularBuffer * buffer);


#endif /* CIRCULARBUFFER_H_ */
