/*
 * dataGatherer.h
 *
 * Created on: May 5, 2017
 * Author: Alessandro Power
 *
 * The data_gatherer_init function adds a task to the scheduler that reads the
 * acquisition buffers of the sensors and sends their data to the xbee. The
 * format of the data is detailed in GS_InterfaceSchema.
 * 
 */

#ifndef DATAGATHERER_H_
#define DATAGATHERER_H_

void data_gatherer_init(void);

#endif /* DATAGATHERER_H_ */
