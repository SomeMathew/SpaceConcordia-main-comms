/*
 * dataGatherer.h
 *
 * Created on: May 5, 2017
 *     Author: Alessandro Power
 *
 * TODO add module level description
 */

#ifndef DATAGATHERER_H_
#define DATAGATHERER_H_

#include <stddef.h>
#include <stdint.h>
#include "acquisitionBuffers.h"

#define PACKET_BUFF_CAPACITY ( \
		ACQBUFF_TIMESTAMP_BUFF_CAPACITY + \
		ACQBUFF_PITOT_BUFF_CAPACITY + \
		ACQBUFF_BAROMETER_BUFF_CAPACITY + \
		ACQBUFF_GPSALTITUDE_BUFF_CAPACITY + \
		ACQBUFF_GPSPOSITION_BUFF_CAPACITY + \
		ACQBUFF_ACCELEROMETER_BUFF_CAPACITY + \
		ACQBUFF_GYROSCOPE_BUFF_CAPACITY )

/*
 * TODO add documentation
 */
size_t read_telem(uint8_t * buffer);

#endif /* DATAGATHERER_H_ */
