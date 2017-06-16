/*
 * file: pitot.c
 * Implements functions in pitot.h.
 */
#include <stdio.h>
#include <inttypes.h>
 
#include "acquisitionBuffers.h"
#include "main.h"
#include "pitot.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "pinmapping.h"
#include "logging.h"

#define PITOT_READ_TIMEOUT 250

static void read_pitot(uint32_t event, void * args);

static SPI_HandleTypeDef spi_handle;

static struct task * runTask = NULL; 
static char testBuffer[128];

static void ui2ascii16(uint16_t n, uint8_t* buffer) {
	for (size_t i = 3; i >= 0 && i <= 3; --i) {
		buffer[i] = '0' + n % 10;
		n /= 10;
	}
}

int init_pitot(uint32_t msInterval) {
	spi_handle.Instance               = SPI2;
	spi_handle.Init.Mode              = SPI_MODE_MASTER;
	spi_handle.Init.Direction         = SPI_DIRECTION_2LINES_RXONLY;
	spi_handle.Init.DataSize          = SPI_DATASIZE_8BIT;
	spi_handle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	spi_handle.Init.CLKPhase          = SPI_PHASE_2EDGE;
	spi_handle.Init.NSS               = SPI_NSS_SOFT;
	spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	spi_handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	spi_handle.Init.TIMode            = SPI_TIMODE_DISABLE;
	spi_handle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	spi_handle.Init.CRCPolynomial     = 7;

	if (HAL_SPI_Init(&spi_handle) != HAL_OK) {
		return DRIVER_STATUS_ERROR;
	}
	
	/* Initialize the pitot cs GPIO */
	GPIO_InitTypeDef gpioInit = {0};
	gpioInit.Pin = PITOT_CS_PIN;
	gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioInit.Pull = GPIO_NOPULL;
	gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(PITOT_CS_PORT, &gpioInit);
	
	runTask = createTask(read_pitot, 0, NULL, msInterval, true, 1);
	return DRIVER_STATUS_OK;
}


/*
 * Reads from the pitot tube ADC, converts value to ASCII, and stores in the
 * pitot tube's acquisition buffer.
 */
static void read_pitot(uint32_t event, void * args) {
	uint8_t buffer[2];
	uint8_t ascii_buffer[4];

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_SPI_Receive(&spi_handle, buffer, sizeof(buffer), PITOT_READ_TIMEOUT);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	
	sprintf(testBuffer, "pitot : %" PRIx8 ", %" PRIx8, buffer[0], buffer[1]);
	logging_send(testBuffer, MODULE_INDEX_PITOT, LOG_DEBUG);

	uint16_t buffer_value = (buffer[0] * (1 << 8) + buffer[1]);
	
	sprintf(testBuffer, "pitot2 : %" PRIu16, buffer_value);
	logging_send(testBuffer, MODULE_INDEX_PITOT, LOG_DEBUG);
	
	ui2ascii16(buffer_value, ascii_buffer);
	acqBuff_write(acqbuff_Pitot, ascii_buffer, sizeof(ascii_buffer));
}
