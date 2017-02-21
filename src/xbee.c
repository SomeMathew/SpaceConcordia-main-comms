/**
 * @brief XBEE driver implementation
 * 
 * The driver uses the UART driver to enqueue new data for transmission in
 * transparant mode, this part is handled transparently by the UART
 * driver.
 */ 

#include <stdbool.h>

#include "main.h"
#include "xbee.h"
#include "uart.h"
#include "scheduler.h"

#define XBEE_CONF_BAUDRATE 57600
#define XBEE_CONF_WORDLENGTH UART_WORDLENGTH_8B
#define XBEE_CONF_STOPBITS UART_STOPBITS_1
#define XBEE_CONF_PARITY UART_PARITY_NONE

static McuDevice_UART xbeeUartDevice = NULL;

int xbee_open(McuDevice_UART uartDevice) {
	if (xbeeUartDevice != NULL) {
		return DRIVER_STATUS_ERROR;
	}
	
	struct uart_ioConf setConfig = {
		.baudrate = XBEE_CONF_BAUDRATE,
		.parity = XBEE_CONF_PARITY,
		.wordlength = XBEE_CONF_WORDLENGTH,
		.stopbits = XBEE_CONF_STOPBITS,
	};
	
	if (uart_open(uartDevice, &setConfig) == DRIVER_STATUS_ERROR) {
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
		return DRIVER_STATUS_ERROR;
	}
	uart_write(xbeeUartDevice, data, size);
	return DRIVER_STATUS_OK;
}
