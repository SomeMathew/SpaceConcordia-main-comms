/**
 * @brief Xbee uart driver to send data through the Xbee transmitter. 
 */

#ifndef __XBEE_H
#define __XBEE_H

#include <stdint.h>
#include "mcuDevices.h"

//~ enum xbee_ioSetMask {
    //~ XBEE_IOSET_BAUDRATE = 0x01,
    //~ SERIAL_IOSET_INITALL = 0xFF,
//~ };

//~ struct xbee_ioConf {
    //~ uint32_t baudrate;
    //~ uint32_t parity;
    //~ uint32_t wordlength;
    //~ uint32_t stopbits;
//~ };

//~ enum xbee_ioctl {
	//~ XBEE_IOCTL_TEST = 0x01,
	
//~ };

/**
 * @brief Initialize the given UART and the xbee module.
 */
int xbee_open(McuDevice_UART UARTx);

/**
 * @brief Closes the UART driver and reset the module.
 * 
 * STUB unimplemented
 */
int xbee_close();
int xbee_write(uint8_t * data, size_t size);
//~ int xbee_ioctl(uint32_t ioctlFlag);

#endif /* __XBEE_H */
