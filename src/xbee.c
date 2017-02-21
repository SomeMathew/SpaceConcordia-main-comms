/**
 * @brief Asynchronous xbee driver implementation
 * 
 * The driver uses the UART driver to enqueue new data for transmission in
 * transparant mode, this part is handled transparently by the UART
 * driver.
 * 
 * For AT Command modes, the module uses the task scheduler and a 
 * circular buffer to handle the guard time requirements
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

//~ #define XBEE_MODULE_CONF_BAUDRATE 0x06

//~ #define XBEE_MODULE_IOCTL_CMD "+++"
//~ #define XBEE_MODULE_IOCTL_CMD_LEN 3
//~ #define XBEE_MODULE_IOCTL_GUARDTIME 1000

//~ #define IOCTL_BUFFER_SIZE 128
//~ static const char IOCTL_BUFFER_SEPARATOR = '#'

static McuDevice_UART xbeeUartDevice = NULL;

//~ static struct task * waitTask = NULL; 
//~ static bool inATMode = false;
//~ struct circularBuffer bufferATCMD;
//~ uint8_t bufferATCMDArray[IOCTL_BUFFER_SIZE];

//~ static void xbeeInitConfig();
//~ static void enterATMode();
//~ static void asyncATPRocess(uint32_t event, void * args);
//~ static void sendATCommand(char * command, uint8_t * param);

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
	//~ buffer_attachArray(&bufferATCMD, bufferATCMDArray, LENGTH_OF_ARRAY(bufferATCMDArray));
	
	
	//~ xbeeInitConfig();
	
	return DRIVER_STATUS_OK;
}

/**
 * Implementation:
 * 		close and removes the UART
 * 		Clear and remove the buffers
 */
int xbee_close() {
	//~ if (xbeeUartDevice == NULL) {
		//~ return DRIVER_STATUS_ERROR;
	//~ }
	//~ uart_close(
	return DRIVER_STATUS_ERROR; //TODO method stub
}

int xbee_write(uint8_t * data, size_t size) {
	if (xbeeUartDevice == NULL) {
		return DRIVER_STATUS_ERROR;
	}
	uart_write(xbeeUartDevice, data, size);
	return DRIVER_STATUS_OK;
}

//~ static void xbeeInitConfig() {
	//~ // TODO function stub
//~ }

//~ /**
 //~ * @brief send command and param in AT Command mode asynchronously.
 //~ * 
 //~ * @param command null-terminated ASCII string
 //~ * @param param Hex Parameter
 //~ */
//~ static sendATCommand(char * command, uint8_t * param, size_t paramSize) {
	//~ buffer_enqueue(&bufferATCMD, (uint8_t *) &IOCTL_BUFFER_SEPARATOR, 1);
	//~ buffer_enqueue(&bufferATCMD, (uint8_t *) 
	
	//~ enterATMode();
//~ }

//~ static void enterATMode() {
	//~ xbee_write(XBEE_MODULE_IOCTL_CMD, XBEE_MODULE_IOCTL_CMD_LEN);
	//~ waitTask = createTask(asyncInitConf, 0, NULL, 
	
//~ }

//~ static void asyncATPRocess(uint32_t event, void * args) {
	//~ void (*callback)() = (void (*)()) args;
	
//~ }
