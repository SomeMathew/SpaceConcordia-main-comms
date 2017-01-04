/**
 * @file uart.c
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Basic interupt based uart driver to send logging data.
 * 
 */
 
#include <stdbool.h> 

#include "uart.h"
#include "circularBuffer.h"

#define BUFFER_MAX_SIZE 512


#define DEFAULT_BAUDRATE 115200
#define DEFAULT_WORDLENGTH UART_WORDLENGTH_8B
#define DEFAULT_STOPBITS UART_STOPBITS_1
#define DEFAULT_PARITY UART_PARITY_NONE
#define DEFAULT_MODE UART_MODE_TX_RX
#define DEFAULT_HWFLOWCTL UART_HWCONTROL_NONE
#define DEFAULT_OVERSAMPLING UART_OVERSAMPLING_16


static struct circularBuffer bufferUart2;
static uint8_t bufferArrayUart2[BUFFER_MAX_SIZE];

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

static int sendBuffer(UART_HandleTypeDef * device, struct circularBuffer * buffer);


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
		
		buffer_attachArray(&bufferUart2, bufferArrayUart2, LENGTH_OF_ARRAY(bufferArrayUart2));
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
	
	struct circularBuffer * buffer;
	if (USARTx == USART2) {
		buffer = &bufferUart2;
	}
	
	if (buffer_enqueue(buffer, data, size) < size) {
		return DRIVER_STATUS_ERROR;	// full buffer
	}
	
	if (buffer_peekSize(buffer) == 0) {
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
		if (buffer_peekSize(&bufferUart2) > 0) {
			buffer_advanceLinear(&bufferUart2);
		}
		if (buffer_size(&bufferUart2) > 0) {
			sendBuffer(huart, &bufferUart2);
		}
	}
}

static int sendBuffer(UART_HandleTypeDef * deviceHandle, struct circularBuffer * buffer) {
	uint8_t * data;
	size_t size = buffer_peekLinear(buffer, &data);
	return HAL_UART_Transmit_IT(deviceHandle, data, size);
}
