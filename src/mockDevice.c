/**
 * This module is used to test the acquisitionBuffer and mock the 
 * transmission of data to the xbee for the base station tests.
 */

#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "xbee.h"
#include "mockDevice.h"
#include "acquisitionBuffers.h"
#include "sysTimer.h"

#define LOOP_MS_INTERVAL 500
#define DUMMY_BUFFER_SIZE (ACQBUFF_TIMESTAMP_BUFF_CAPACITY \
							+ ACQBUFF_PITOT_BUFF_CAPACITY \
							+ ACQBUFF_BAROMETER_BUFF_CAPACITY \
							+ ACQBUFF_GPSALTITUDE_BUFF_CAPACITY \
							+ ACQBUFF_GPSPOSITION_BUFF_CAPACITY \
							+ ACQBUFF_ACCELEROMETER_BUFF_CAPACITY \
							+ ACQBUFF_GYROSCOPE_BUFF_CAPACITY)

char DUMMY_FIX_PITOT[] = "2048";
char DUMMY_FIX_BAROMETER[] = "99325";
char DUMMY_FIX_GPS_ALTITUDE[] = "167.8";
char DUMMY_FIX_GPS_POSITION[] = "4529.8360#N#7334.74137#W";
char DUMMY_FIX_ACCELEROMETER[] = "101#101#101";
char DUMMY_FIX_GYROSCOPE[] = "101#101#101";


char dummy_buffer[DUMMY_BUFFER_SIZE];

static struct task * mockTask = NULL;

static void loop(uint32_t event, void * args);

void mockDevice_init() {
	mockTask = createTask(loop, 0, NULL, LOOP_MS_INTERVAL, true, 2);
}


static void fillBuffers() {
	acqBuff_write(acqbuff_Pitot, (uint8_t *) DUMMY_FIX_PITOT, strlen(DUMMY_FIX_PITOT));
	acqBuff_write(acqbuff_Barometer, (uint8_t *) DUMMY_FIX_BAROMETER, strlen(DUMMY_FIX_BAROMETER));
	acqBuff_write(acqbuff_GPSAltitude, (uint8_t *) DUMMY_FIX_GPS_ALTITUDE, strlen(DUMMY_FIX_GPS_ALTITUDE));
	acqBuff_write(acqbuff_GPSPosition, (uint8_t *) DUMMY_FIX_GPS_POSITION, strlen(DUMMY_FIX_GPS_POSITION));
	acqBuff_write(acqbuff_Accelerometer, (uint8_t *) DUMMY_FIX_ACCELEROMETER, strlen(DUMMY_FIX_ACCELEROMETER));
	acqBuff_write(acqbuff_Gyroscope, (uint8_t *) DUMMY_FIX_GYROSCOPE, strlen(DUMMY_FIX_GYROSCOPE));
}


static void loop(uint32_t event, void * args) {
	fillBuffers();
}
