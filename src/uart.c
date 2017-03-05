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
#include "stm32f1xx_hal.h"

#define BUFFER_TX_MAX_SIZE 512
#define BUFFER_RX_MAX_SIZE 64


#define DEFAULT_BAUDRATE 115200
#define DEFAULT_WORDLENGTH UART_WORDLENGTH_8B
#define DEFAULT_STOPBITS UART_STOPBITS_1
#define DEFAULT_PARITY UART_PARITY_NONE
#define DEFAULT_MODE UART_MODE_TX_RX
#define DEFAULT_HWFLOWCTL UART_HWCONTROL_NONE
#define DEFAULT_OVERSAMPLING UART_OVERSAMPLING_16

/*
 * UART_HandleTypeDef must stay as the first member of this struct so it can be cast back to 
 * struct uart_Peripheral in the callback from the hal.
 */
struct uart_Peripheral {
	UART_HandleTypeDef huart;
	struct circularBuffer bufferTx;
	uint8_t bufferTxArray[BUFFER_TX_MAX_SIZE];
	struct circularBuffer bufferRx;
	uint8_t bufferRxArray[BUFFER_RX_MAX_SIZE];
	uint8_t memRxChar[4];
};

static struct uart_Peripheral device_uart1 = {
	.huart = {
		.Instance = USART1,
		.Init = {
			.BaudRate = DEFAULT_BAUDRATE,
			.WordLength = DEFAULT_WORDLENGTH,
			.StopBits = DEFAULT_STOPBITS,
			.Parity = DEFAULT_PARITY,
			.Mode = DEFAULT_MODE,
			.HwFlowCtl = UART_HWCONTROL_RTS,
			.OverSampling = DEFAULT_OVERSAMPLING,
		},
	},
};

static struct uart_Peripheral device_uart2 = {
	.huart = {
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
	},
};

McuDevice_UART mcuDevice_serialPC = &device_uart2;
McuDevice_UART mcuDevice_serialXBee = &device_uart1;

static int sendBuffer(UART_HandleTypeDef * device, struct circularBuffer * buffer);


int uart_open(McuDevice_UART UARTx, struct uart_ioConf * conf) {
	struct uart_Peripheral * device = (struct uart_Peripheral *) UARTx;
	device->huart.Init.BaudRate = conf->baudrate;
	device->huart.Init.WordLength = conf->wordlength;
	device->huart.Init.Parity = conf->parity;
	device->huart.Init.StopBits = conf->stopbits;
	
	if (HAL_UART_Init(&device->huart) != HAL_OK) {
		return DRIVER_STATUS_ERROR;
	};
	
	buffer_attachArray(&device->bufferTx, device->bufferTxArray, LENGTH_OF_ARRAY(device->bufferTxArray));
	buffer_attachArray(&device->bufferRx, device->bufferRxArray, LENGTH_OF_ARRAY(device->bufferRxArray));
	
	HAL_UART_Receive_IT(&device->huart, device->memRxChar, 1);
	
	return DRIVER_STATUS_OK;
}

int uart_ioctl_set(McuDevice_UART UARTx, int ioSetMask, struct uart_ioConf * conf) {
	struct uart_Peripheral * device = (struct uart_Peripheral *) UARTx;
	UART_HandleTypeDef * usartDeviceHandle = &device->huart;

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

size_t uart_write(McuDevice_UART UARTx, uint8_t * data, size_t size) {
	struct uart_Peripheral * device = (struct uart_Peripheral *) UARTx;
	UART_HandleTypeDef * usartDeviceHandle = &device->huart;
	struct circularBuffer * buffer = &device->bufferTx;
	
	size_t writtenSize = buffer_enqueue(buffer, data, size);
	
	// send the buffer if nothing is waiting to send
	if (buffer_peekSize(buffer) == 0) {
		sendBuffer(usartDeviceHandle, buffer);
	}
	return writtenSize;
}

size_t uart_read(McuDevice_UART UARTx, uint8_t * data, size_t size) {
	struct uart_Peripheral * device = (struct uart_Peripheral *) UARTx;
	struct circularBuffer * buffer = &device->bufferRx;
	
	return buffer_dequeue(buffer, data, size);
}

void USART1_IRQHandler(void) {
	HAL_UART_IRQHandler(&device_uart1.huart);
}

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&device_uart2.huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	struct uart_Peripheral * device = (struct uart_Peripheral *) huart;
	struct circularBuffer * buffer = &device->bufferTx;
	
	if (buffer_peekSize(buffer) > 0) {
		buffer_advanceLinear(buffer);
	}
	if (buffer_size(buffer) > 0) {
		sendBuffer(huart, buffer);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	struct uart_Peripheral * device = (struct uart_Peripheral *) huart;
	struct circularBuffer * buffer = &device->bufferRx;
	buffer_enqueue(buffer, device->memRxChar, 1);
	HAL_UART_Receive_IT(&device->huart, device->memRxChar, 1);
}

static int sendBuffer(UART_HandleTypeDef * deviceHandle, struct circularBuffer * buffer) {
	uint8_t * data;
	size_t size = buffer_peekLinear(buffer, &data);
	return HAL_UART_Transmit_IT(deviceHandle, data, size);
}
