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

/*
 * ADC value
 * 12 bit integer value
 * 0 to 4095
 * 
 * 4 char total
 */
#define ACQBUFF_PITOT_BUFF_CAPACITY 8

/*
 * In Pa
 * 18 bit uinteger value 
 * 0 to 262143
 * + 2 bit fractional
 * 0.xx (.25 increment)
 * 
 * 9 char total
 */
#define ACQBUFF_BAROMETER_BUFF_CAPACITY 16

/*
 * 
 */
#define ACQBUFF_GPSALTITUDE_BUFF_CAPACITY 32 

/*
 * Latitude in ddmm.mmmm format
 * Leading zeros transmitted
 * 
 * Latitude hemisphere indicator
 * ‘N’ = North
 * ‘S’ = South
 * 
 * Longitude in dddmm.mmmm format
 * Leading zeros transmitted
 * 
 * Longitude hemisphere indicator
 * 'E' = East
 * 'W' = West
 * 
 * Lat#LatInd#Long#LongInd
 * 
 * 24 char total
 */
#define ACQBUFF_GPSPOSITION_BUFF_CAPACITY 32

/*
 * 
 * X#Y#Z where each is 16bit 2's complement
 * Range: -32768 to 32767
 * 
 * 20 char total
 */
#define ACQBUFF_ACCELEROMETER_BUFF_CAPACITY 24

/*
 * 
 * X#Y#Z where each is 16bit 2's complement
 * Range: -32768 to 32767
 * 
 * 20 char total
 */
#define ACQBUFF_GYROSCOPE_BUFF_CAPACITY 24

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
