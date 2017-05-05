/*
 * TODO add module level documentation
 */

#include <stddef.h>
#include <stdint.h>

#include "acquisitionBuffers.h"
#include "dataGatherer.h"
#include "sysTimer.h"

#define TELEM_PACKET_BUFF_CAPACITY ( \
		ACQBUFF_TIMESTAMP_BUFF_CAPACITY + \
		ACQBUFF_PITOT_BUFF_CAPACITY + \
		ACQBUFF_BAROMETER_BUFF_CAPACITY + \
		ACQBUFF_GPSALTITUDE_BUFF_CAPACITY + \
		ACQBUFF_GPSPOSITION_BUFF_CAPACITY + \
		ACQBUFF_ACCELEROMETER_BUFF_CAPACITY + \
		ACQBUFF_GYROSCOPE_BUFF_CAPACITY )

static uint8_t telem_packet_buff[TELEM_PACKET_BUFF_CAPACITY];
static size_t read_telem_data(void) {
	uint8_t * current = telem_packet_buff;

	const uint32_t time = sysTimer_GetTick();
	// TODO add the time.

	current += acqBuff_read(acqbuff_Pitot, current);
	*current++ = ',';

	current += acqBuff_read(acqbuff_Barometer);
	*current++ = ',';

	current += acqBuff_read(acqbuff_GPSAltitude);
	*current++ = ',';

	current += acqBuff_read(acqbuff_GPSPosition);
	*current++ = ',';

	current += acqBuff_read(acqbuff_Accelerometer);
	*current++ = ',';

	current += acqBuff_read(acqbuff_Gyroscope);
	*current++ = ',';

	*current++ = '\n';

	return current - telem_packet_buff;
}
