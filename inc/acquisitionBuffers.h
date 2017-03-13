/**
 * This module is the interface between the sensors array and the data 
 * gatherer.
 * 
 * The buffers are composed of null ended string as uint8_t.
 * 
 * To use a buffer an Acqbuff_Buffer must be statically added for each 
 * new sensor. The buffer table and buffer array definition must be
 * handled in acquisitionBuffers.c
 * 
 * 		1. Add an Acqbuff_Buffer here
 * 		2. Add the corresponding entries in acquisitionBuffers.c
 * 
 * Note/Warning: It is important that the buffers be always manipulated 
 * through the task/event manager to prevent concurrency problems.
 */



#ifndef __ACQ_BUFFERS_H
#define __ACQ_BUFFERS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// TODO Stub size
 
#define ACQBUFF_PITOT_BUFF_CAPACITY 32
#define ACQBUFF_BAROMETER_BUFF_CAPACITY 32
#define ACQBUFF_GPSALTITUDE_BUFF_CAPACITY 32 
#define ACQBUFF_GPSPOSITION_BUFF_CAPACITY 64
#define ACQBUFF_ACCELEROMETER_BUFF_CAPACITY 32
#define ACQBUFF_GYROSCOPE_BUFF_CAPACITY 64

typedef void * AcqBuff_Buffer;

extern AcqBuff_Buffer acqbuff_Pitot;
extern AcqBuff_Buffer acqbuff_Barometer;
extern AcqBuff_Buffer acqbuff_GPSAltitude;
extern AcqBuff_Buffer acqbuff_GPSPosition;
extern AcqBuff_Buffer acqbuff_Accelerometer;
extern AcqBuff_Buffer acqbuff_Gyroscope;


/**
 * @brief Write the string data to the buffer, for safety the string length
 * must be specified.
 * 
 * @return count written to buffer
 */
size_t acqBuff_write(AcqBuff_Buffer buffer, uint8_t * data, size_t count);

/**
 * @brief Reads the buffer to data up to the capacity of the buffer.
 * 
 * @return count read from buffer.
 */  
size_t acqBuff_read(AcqBuff_Buffer buffer, uint8_t * data); 

/**
 * @brief Returns true if the buffer has new data since the last read.
 */
bool acqBuff_isNew(AcqBuff_Buffer buffer);

#endif /* __ACQ_BUFFERS_H */
