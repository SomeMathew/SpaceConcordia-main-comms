/**
 * @brief Xbee uart driver to send data through the Xbee transmitter. 
 */

#ifndef __XBEE_H
#define __XBEE_H

#include <stdint.h>
#include "mcuDevices.h"

/**
 * @brief Initialize the xbee module.
 * 
 * The UART should be opened and initialized by the user application.
 */
int xbee_open(McuDevice_UART UARTx);

/**
 * @brief Reset the xbee module.
 * 
 * STUB unimplemented
 */
int xbee_close();
int xbee_write(uint8_t * data, size_t size);

#endif /* __XBEE_H */
