/**
 * @brief XBEE driver implementation
 * 
 * The driver uses the UART driver to enqueue new data for transmission in
 * transparant mode, this part is handled transparently by the UART
 * driver.
 */ 

#include <stdbool.h>

#include "main.h"
#include "logging.h"
#include "xbee.h"
#include "uart.h"
#include "scheduler.h"

static McuDevice_UART xbeeUartDevice = NULL;

int xbee_open(McuDevice_UART uartDevice) {
	if (xbeeUartDevice != NULL) {
		logging_send("xbee open fail", MODULE_INDEX_XBEE, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	
	xbeeUartDevice = uartDevice;

	return DRIVER_STATUS_OK;
}

/**
 * Implementation:
 * 		close and removes the UART
 * 		Clear and remove the buffers
 */
int xbee_close() {
	return DRIVER_STATUS_ERROR; //TODO method stub
}

int xbee_write(uint8_t * data, size_t size) {
	if (xbeeUartDevice == NULL) {
		logging_send("xbee write uart device is null", MODULE_INDEX_XBEE, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	uart_write(xbeeUartDevice, data, size);
	return DRIVER_STATUS_OK;
}
