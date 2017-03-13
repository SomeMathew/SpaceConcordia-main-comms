/**
 * The buffers are implemented statically and must be
 * modified to add new sensor buffers.
 * 
 * To add a new buffer add the buffer size
 * as a define and follow the instruction in the "Device buffer entries"
 * section. You should also add an external declaration inside the 
 * header "acquisitionBuffers.h".
 */
 
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "acquisitionBuffers.h"

/*
 * Buffer size in bytes define
 */

struct entry {
	bool newData;
	uint8_t * buffer;
	size_t bufferCapacity;
	size_t bufferSize;
};

/*
 * Device buffer entries
 * 
 * Enter them as follow:
 * 
 * 		static uint8_t sensor_buffer[ACQBUFF_SENSOR_BUFF_CAPACITY];
 * 		static struct entry sensor_entry = {
 * 			.newData = false,
 * 			.buffer = sensor_buffer,
 * 			.bufferCapacity = ACQBUFF_SENSOR_BUFF_CAPACITY,
 * 			.bufferSize = 0,
 * 		};
 * 		AcqBuff_Buffer acqbuff_Sensor = &sensor_entry;
 */
static uint8_t pitot_buffer[ACQBUFF_PITOT_BUFF_CAPACITY];
static struct entry pitot_entry = {
	.newData = false,
	.buffer = pitot_buffer,
	.bufferCapacity = ACQBUFF_PITOT_BUFF_CAPACITY,
	.bufferSize = 0,
};
AcqBuff_Buffer acqbuff_Pitot = &pitot_entry;

static uint8_t barometer_buffer[ACQBUFF_BAROMETER_BUFF_CAPACITY];
static struct entry barometer_entry = {
	.newData = false,
	.buffer = barometer_buffer,
	.bufferCapacity = ACQBUFF_BAROMETER_BUFF_CAPACITY,
	.bufferSize = 0,
};
AcqBuff_Buffer acqbuff_Barometer = &barometer_entry;

static uint8_t gpsAltitude_buffer[ACQBUFF_GPSALTITUDE_BUFF_CAPACITY];
static struct entry gpsAltitude_entry = {
	.newData = false,
	.buffer = gpsAltitude_buffer,
	.bufferCapacity = ACQBUFF_GPSALTITUDE_BUFF_CAPACITY,
	.bufferSize = 0,
};
AcqBuff_Buffer acqbuff_GPSAltitude = &gpsAltitude_entry;

static uint8_t gpsPosition_buffer[ACQBUFF_GPSPOSITION_BUFF_CAPACITY];
static struct entry gpsPosition_entry = {
	.newData = false,
	.buffer = gpsPosition_buffer,
	.bufferCapacity = ACQBUFF_GPSPOSITION_BUFF_CAPACITY,
	.bufferSize = 0,
};
AcqBuff_Buffer acqbuff_GPSPosition = &gpsPosition_entry;

static uint8_t accelerometer_buffer[ACQBUFF_ACCELEROMETER_BUFF_CAPACITY];
static struct entry accelerometer_entry = {
	.newData = false,
	.buffer = accelerometer_buffer,
	.bufferCapacity = ACQBUFF_ACCELEROMETER_BUFF_CAPACITY,
	.bufferSize = 0,
};
AcqBuff_Buffer acqbuff_Accelerometer = &accelerometer_entry;

static uint8_t gyroscope_buffer[ACQBUFF_GYROSCOPE_BUFF_CAPACITY];
static struct entry gyroscope_entry = {
	.newData = false,
	.buffer = gyroscope_buffer,
	.bufferCapacity = ACQBUFF_GYROSCOPE_BUFF_CAPACITY,
	.bufferSize = 0,
};
AcqBuff_Buffer acqbuff_Gyroscope = &gyroscope_entry;


size_t acqBuff_write(AcqBuff_Buffer buffer, uint8_t * data, size_t count) {
	struct entry * bufferEntry = (struct entry *) buffer;
	
	int i;
	for (i = 0; i < count || i < bufferEntry->bufferCapacity; i++) {
		(bufferEntry->buffer)[i] = data[i]; 
	}
	bufferEntry->newData = true;
	bufferEntry->bufferSize = i;
	
	return (size_t) i;
}

size_t acqBuff_read(AcqBuff_Buffer buffer, uint8_t * data) {
	struct entry * bufferEntry = (struct entry *) buffer;
	
	int i;
	for (i = 0; i < bufferEntry->bufferSize; i++) {
		data[i] = (bufferEntry->buffer)[i];
	}
	// reset the newData flag only if everything was read
	if (i == bufferEntry->bufferSize) {
		bufferEntry->newData = false;
	}
	
	return (size_t) i;
}

bool acqBuff_isNew(AcqBuff_Buffer buffer) {
	struct entry * bufferEntry = (struct entry *) buffer;
	
	return bufferEntry->newData;
}




