/**
 * @file uart.c
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Basic interupt based uart driver to send logging data.
 * 
 */
 
#include <stdbool.h> 

#include "uart.h"

#define BUFFER_MAX_SIZE 512


#define DEFAULT_BAUDRATE 115200
#define DEFAULT_WORDLENGTH UART_WORDLENGTH_8B
#define DEFAULT_STOPBITS UART_STOPBITS_1
#define DEFAULT_PARITY UART_PARITY_NONE
#define DEFAULT_MODE UART_MODE_TX_RX
#define DEFAULT_HWFLOWCTL UART_HWCONTROL_NONE
#define DEFAULT_OVERSAMPLING UART_OVERSAMPLING_16

struct buffer {
	uint8_t mem[BUFFER_MAX_SIZE];
	size_t front;
	size_t rear;
	size_t sendWait;
};

static struct buffer bufferUart2;

static UART_HandleTypeDef device_uart2 = {
        .Instance = USART2,
        .Init = {
                .BaudRate = DEFAULT_BAUDRATE,
                .WordLength = DEFAULT_WORDLENGTH,
                .StopBits = DEFAULT_STOPBITS,
                .Parity = DEFAULT_PARITY,
                .Mode = DEFAULT_MODE,
                .HwFlowCtl = DEFAULT_HWFLOWCTL,
                .OverSampling = DEFAULT_OVERSAMPLING,
        },
};

static int enqueueBuffer(struct buffer * buffer, uint8_t * data, size_t count);
static void dequeueBuffer(struct buffer * buffer, size_t count);
static int sendBuffer(UART_HandleTypeDef * device, struct buffer * buffer);

static inline size_t bufferSize(struct buffer * buffer) {
	return (BUFFER_MAX_SIZE - buffer->front + buffer->rear) % BUFFER_MAX_SIZE;
}

static inline bool bufferIsFull(struct buffer * buffer) {
	return (bufferSize(buffer) >= BUFFER_MAX_SIZE - 1);
}

static inline UART_HandleTypeDef * getDevice(USART_TypeDef * USARTx) {
	if (USARTx == USART2) {
		return &device_uart2;
	} else {
		return NULL;
	}
}

int uart_open(USART_TypeDef * USARTx, struct uart_ioConf * conf) {
	if (USARTx == USART2) {
		device_uart2.Init.BaudRate = conf->baudrate;
		device_uart2.Init.WordLength = conf->wordlength;
		device_uart2.Init.Parity = conf->parity;
		device_uart2.Init.StopBits = conf->stopbits;
		if (HAL_UART_Init(&device_uart2) != HAL_OK) {
			return DRIVER_STATUS_ERROR;
		};
	} else {
		return DRIVER_STATUS_ERROR;
	}
	
	return DRIVER_STATUS_OK;
}

int uart_ioctl_set(USART_TypeDef * USARTx, int ioSetMask, struct uart_ioConf * conf) {
	UART_HandleTypeDef * usartDeviceHandle = getDevice(USARTx);
	if (usartDeviceHandle == NULL) {
		return DRIVER_STATUS_ERROR;
	}

	if (ioSetMask & UART_IOSET_BAUDRATE) {
		usartDeviceHandle->Init.BaudRate = conf->baudrate;
	}
	if (ioSetMask & UART_IOSET_PARITY) {
		usartDeviceHandle->Init.Parity = conf->parity;
	}
	if (ioSetMask & UART_IOSET_WORDLENGTH) {
		usartDeviceHandle->Init.WordLength = conf->wordlength;
	}
	if (ioSetMask & UART_IOSET_STOPBITS) {
		usartDeviceHandle->Init.StopBits = conf->stopbits;
	}
	
	if (HAL_UART_Init(usartDeviceHandle) != HAL_OK) {
		return DRIVER_STATUS_ERROR;
	} else {
		return DRIVER_STATUS_OK;
	}
}

int uart_write(USART_TypeDef * USARTx, uint8_t * data, uint16_t size) {
	UART_HandleTypeDef * usartDeviceHandle = getDevice(USARTx);
	if (usartDeviceHandle == NULL) {
		return DRIVER_STATUS_ERROR;
	}
	
	struct buffer * buffer;
	if (USARTx == USART2) {
		buffer = &bufferUart2;
	}
	
	if (enqueueBuffer(buffer, data, size) < size) {
		return DRIVER_STATUS_ERROR;	// full buffer
	}
	
	if (buffer->sendWait == 0) {
		sendBuffer(usartDeviceHandle, buffer);
	}
	return DRIVER_STATUS_OK;
}

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&device_uart2);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &device_uart2) {
		if (bufferUart2.sendWait > 0) {
			dequeueBuffer(&bufferUart2, bufferUart2.sendWait);
			sendBuffer(huart, &bufferUart2);
		}
	}
}

static int enqueueBuffer(struct buffer * buffer, uint8_t * data, size_t count) {
	int i;
	for (i = 0; i < count && !bufferIsFull(buffer); i++) {
		buffer->mem[buffer->rear] = data[i];
		buffer->rear = (buffer->rear + 1) % BUFFER_MAX_SIZE;
	}
	
	return i;
}

static void dequeueBuffer(struct buffer * buffer, size_t count) {
	size_t size = bufferSize(buffer);
	if (count <= size) {
		buffer->front = (buffer->front + count) % BUFFER_MAX_SIZE;
	} else {
		buffer->front = (buffer->front + size) % BUFFER_MAX_SIZE;
	}
}

static int sendBuffer(UART_HandleTypeDef * deviceHandle, struct buffer * buffer) {
	uint8_t * data = buffer->mem + buffer->front;
	uint16_t size;
	// Always send data in non circular fashion, if wrapped around only send front to arrayEnd
	if (buffer->front <= buffer->rear) {
		size = bufferSize(buffer);
	} else {
		size = BUFFER_MAX_SIZE - buffer->front;
	}
	
	buffer->sendWait = size;
	return HAL_UART_Transmit_IT(deviceHandle, data, size);
}
