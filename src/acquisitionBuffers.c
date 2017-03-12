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

#include "acquisitionBuffers.h"

/*
 * Buffer size in bytes define
 */

// TODO Stub size
 
#define PITOT_BUFF_SIZE 32
#define BAROMETER_BUFF_SIZE 32
#define GPSALTITUDE_BUFF_SIZE 32 
#define GPSPOSITION_BUFF_SIZE 64
#define ACCELEROMETER_BUFF_SIZE 32
#define GYROSCOPE_BUFF_SIZE 64



struct entry {
	bool newData;
	uint8_t * buffer;
	size_t bufferSize;
};

/*
 * Device buffer entries
 * 
 * Enter them as follow:
 * 
 * 		static uint8_t sensor_buffer[SENSOR_BUFF_SIZE];
 * 		static struct entry sensor_entry = {
 * 			.newData = false,
 * 			.buffer = sensor_buffer,
 * 			.bufferSize = SENSOR_BUFF_SIZE,
 * 		};
 * 		Acqbuff_Buffer acqbuff_Sensor = &sensor_entry;
 */
static uint8_t pitot_buffer[PITOT_BUFF_SIZE];
static struct entry pitot_entry = {
	.newData = false,
	.buffer = pitot_buffer,
	.bufferSize = PITOT_BUFF_SIZE,
};
Acqbuff_Buffer acqbuff_Pitot = &pitot_entry;

static uint8_t barometer_buffer[BAROMETER_BUFF_SIZE];
static struct entry barometer_entry = {
	.newData = false,
	.buffer = barometer_buffer,
	.bufferSize = BAROMETER_BUFF_SIZE,
};
Acqbuff_Buffer acqbuff_Barometer = &barometer_entry;

static uint8_t gpsAltitude_buffer[GPSALTITUDE_BUFF_SIZE];
static struct entry gpsAltitude_entry = {
	.newData = false,
	.buffer = gpsAltitude_buffer,
	.bufferSize = GPSALTITUDE_BUFF_SIZE,
};
Acqbuff_Buffer acqbuff_GPSAltitude = &gpsAltitude_entry;

static uint8_t gpsPosition_buffer[GPSPOSITION_BUFF_SIZE];
static struct entry gpsPosition_entry = {
	.newData = false,
	.buffer = gpsPosition_buffer,
	.bufferSize = GPSPOSITION_BUFF_SIZE,
};
Acqbuff_Buffer acqbuff_GPSPosition = &gpsPosition_entry;

static uint8_t accelerometer_buffer[ACCELEROMETER_BUFF_SIZE];
static struct entry accelerometer_entry = {
	.newData = false,
	.buffer = accelerometer_buffer,
	.bufferSize = ACCELEROMETER_BUFF_SIZE,
};
Acqbuff_Buffer acqbuff_Accelerometer = &accelerometer_entry;

static uint8_t gyroscope_buffer[GYROSCOPE_BUFF_SIZE];
static struct entry gyroscope_entry = {
	.newData = false,
	.buffer = gyroscope_buffer,
	.bufferSize = GYROSCOPE_BUFF_SIZE,
};
Acqbuff_Buffer acqbuff_Gyroscope = &gyroscope_entry;


int write(Acqbuff_Buffer buffer, uint8_t * data, size_t length);

size_t read(Acqbuff_Buffer buffer, uint8_t * data, size_t length); 

bool isNew(Acqbuff_Buffer buffer);




