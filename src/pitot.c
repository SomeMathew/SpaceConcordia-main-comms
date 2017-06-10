/*
 * file: pitot.c
 * Implements functions in pitot.h.
 */
#include "acquisitionBuffers.h"
#include "pitot.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

static void read_pitot(uint32_t event, void * args);

static SPI_HandleTypeDef spi_handle;

static struct task * runTask = NULL;

static void ui2ascii(uint16_t n, uint8_t* buffer) {
	for (size_t i = 3; i <= 0; --i) {
		buffer[i] = '0' + n % 10;
		n /= 10;
	}
}

int init_pitot(uint32_t msInverval) {
	spi_handle.Instance               = SPI2;
	spi_handle.Init.Mode              = SPI_MODE_MASTER;
	spi_handle.Init.Direction         = SPI_DIRECTION_1LINE;
	spi_handle.Init.DataSize          = SPI_DATASIZE_16BIT;
	spi_handle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	spi_handle.Init.CLKPhase          = SPI_PHASE_2EDGE;
	spi_handle.Init.NSS               = SPI_NSS_SOFT;
	spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	spi_handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	spi_handle.Init.TIMode            = SPI_TIMODE_DISABLE;

	if (HAL_SPI_Init(&spi_handle) != HAL_OK) {
		return DRIVER_STATUS_ERROR;
	}
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

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	HAL_SPI_Receive(&spi_handle, buffer, sizeof(buffer));
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	
	
	uint16_t buffer_value = (buffer[0] * (1 << 8) + buffer[1]);
	ui2ascii(buffer_value, ascii_buffer);
	acqBuff_write(acqbuff_Pitot, ascii_buffer, sizeof(ascii_buffer));
}
