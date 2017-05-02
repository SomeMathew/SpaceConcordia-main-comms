#include "main.h"
#include "scheduler.h"
#include "uart.h"
#include "logging.h"

#define MSG_MAX_LENGTH 83

static struct task * reader = NULL;
static McuDevice_UART gpsUartDevice = NULL;

//~ static uint8_t buffer[MSG_MAX_LENGTH];

static void readUart(uint32_t p, void * arg) {
	logging_send("In readUart", MODULE_INDEX_GPSTEST, LOG_DEBUG);
	uint8_t buffer[MSG_MAX_LENGTH];
	size_t readSize = uart_read(mcuDevice_serialPC, buffer, MSG_MAX_LENGTH - 1);
	if (readSize > 0) {
		buffer[readSize] = '\0';
		logging_send((char *) buffer, MODULE_INDEX_GPSTEST, LOG_DEBUG);
	}
	//~ char test[20] = ito;
	//~ logging_send("read size in readUart", MODULE_INDEX_GPSTEST, LOG_DEBUG);
}

int gpsTest_open(McuDevice_UART uartDevice) {
	if (gpsUartDevice != NULL) {
		logging_send("gpsTest open fail", MODULE_INDEX_GPSTEST, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	
	gpsUartDevice = uartDevice;
	reader = createTask(readUart, 0, NULL, 200, true, 2);

	return DRIVER_STATUS_OK;
}

//~ /**
 //~ * @brief Reads a line of data up to new line
 //~ */
//~ void gpsTest_read(uint8_t *data) {
	
//~ }
