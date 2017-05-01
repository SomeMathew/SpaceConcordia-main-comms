#ifndef __GPSTEST_H
#define __GPSTEST_H

#include "mcuDevice.h"

/**
 * @brief Initialize the gpsTest module.
 * 
 * The UART should be opened and initialized by the user application.
 */
void gpsTest_open(McuDevice_UART uartDevice);

/**
 * @brief Reads a line of data 
 */
void gpsTest_read(uint8_t *data);


#endif /* __GPSTEST_H */
