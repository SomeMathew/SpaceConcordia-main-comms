#include "main.h"
#include "scheduler.h"
#include "uart.h"
#include "logging.h"

#define MSG_MAX_LENGTH 83

static struct task * reader = NULL;
static McuDevice_UART gpsUartDevice = NULL;

//~ static uint8_t buffer[MSG_MAX_LENGTH];

bool runScheduler(void);
struct task * createTask(void (*vector)(uint32_t, void *), uint32_t event, void * argument,
		uint32_t timeInterval, bool repeat, uint8_t priority);
bool destroyTask(struct task *);


static void readUart(uint32_t p, void * arg) {
	uint8_t buffer[MSG_MAX_LENGTH];
	size_t readSize = uart_read(mcuDevice_serialPC, buffer, MSG_MAX_LENGTH - 1);
	if (readSize > 0) {
		buffer[readSize] = '\0';
		logging_send((char *) buffer, MODULE_INDEX_GPSTEST, LOG_DEBUG);
	}
}

 size_t uart_read(McuDevice_UART UARTx, uint8_t * data, size_t size);

int gpsTest_open(McuDevice_UART uartDevice) {
	if (gpsUartDevice != NULL) {
		logging_send("gpsTest open fail", MODULE_INDEX_GPSTEST, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	
	gpsUartDevice = uartDevice;
	reader = createTask(readUart, 0, NULL, 100, true, 2);

	return DRIVER_STATUS_OK;
}

//~ /**
 //~ * @brief Reads a line of data up to new line
 //~ */
//~ void gpsTest_read(uint8_t *data) {
	
//~ }
