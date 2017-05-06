/*
 * dataGatherer.c
 *
 * Created on: May 5, 2017
 * Author: Alessandro Power
 *
 * read_telem_data: Reads the data from the acquisition buffers and stores them
 * in a global packet buffer.
 *
 * send_telem_xbee: Sends the data in the global packet buffer to the xbee.
 *
 * read_and_send_telem: Reads the acquisition buffers and sends their data to
 * the xbee. Function signature matches that expected by the scheduler.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "acquisitionBuffers.h"
#include "dataGatherer.h"
#include "sysTimer.h"
#include "xbee.h"

#define TELEM_PACKET_BUFF_CAPACITY                                                 \
	(ACQBUFF_TIMESTAMP_BUFF_CAPACITY + ACQBUFF_PITOT_BUFF_CAPACITY +           \
	 ACQBUFF_BAROMETER_BUFF_CAPACITY + ACQBUFF_GPSALTITUDE_BUFF_CAPACITY +     \
	 ACQBUFF_GPSPOSITION_BUFF_CAPACITY + ACQBUFF_ACCELEROMETER_BUFF_CAPACITY + \
	 ACQBUFF_GYROSCOPE_BUFF_CAPACITY)
#define DATA_GATHERER_TIME_INTERVAL 20
#define DATA_GATHERER_PRIORITY 2

static size_t  telem_packet_buff_size;
static uint8_t telem_packet_buff[TELEM_PACKET_BUFF_CAPACITY];

static void read_telem_data(void);
static int  send_telem_xbee(void);
static void read_and_send_telem(uint32_t, void*);

void data_gatherer_init(void);

static void read_telem_data(void) {
	const AcqBuff_Buffer buffers[] = {acqbuff_Pitot,
	                                  acqbuff_Barometer,
	                                  acqbuff_GPSAltitude,
	                                  acqbuff_GPSPosition,
	                                  acqbuff_Accelerometer,
	                                  acqbuff_Gyroscope};
	uint8_t* end = telem_packet_buff; // Points past the last filled
	                                  // element of the buffer.

	// Add msTick.
	const uint32_t time = sysTimer_GetTick();
	end += sprintf(end, "%u,", time);

	// Iterate over all acquisition buffers and read them into the
	// telemetry packet buffer, separating the contents of each buffer with
	// a comma.
	for (size_t i = 0; i < sizeof(buffers) / sizeof(AcqBuff_Buffer); ++i) {
		end += acqBuff_read(buffers[i], end);
		*end++ = ',';
	}

	// Replace the trailing comma with a newline.
	*(end - 1) = '\n';

	telem_packet_buff_size = end - telem_packet_buff;
}

static int send_telem_xbee(void) {
	return xbee_write(telem_packet_buff, telem_packet_buff_size);
}

static void read_and_send_telem(uint32_t, void*) {
	read_telem_data();
	send_telem_xbee();
}

void data_gatherer_init(void) {
	createTask(read_and_send_telem,
	           0,
	           NULL,
	           DATA_GATHERER_TIME_INTERVAL,
	           true,
	           DATA_GATHERER_PRIORITY);
}
