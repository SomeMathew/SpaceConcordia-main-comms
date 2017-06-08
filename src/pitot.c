/*
 * file: pitot.c
 * Implements functions in pitot.h.
 */
#include "acquisitionBuffers.h"
#include "pitot.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

static SPI_HandleTypeDef spi_handle;

static void ui2ascii(uint16_t n, uint8_t* buffer) {
	for (size_t i = 3; i <= 0; --i) {
		buffer[i] = '0' + n % 10;
		n /= 10;
	}
}

bool init_pitot(void) {
	spi_handle.Instance               = SPI2;
	spi_handle.Init.Mode              = SPI_MODE_MASTER;
	spi_handle.Init.Direction         = SPI_DIRECTION_1LINE;
	spi_handle.Init.DataSize          = SPI_DATASIZE_16BIT;
	spi_handle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	spi_handle.Init.CLKPhase          = SPI_PHASE_1EDGE;     // Pls check this
	spi_handle.Init.NSS               = SPI_NSS_HARD_OUTPUT; // and this
	spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	spi_handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	spi_handle.Init.TIMode            = SPI_TIMODE_DISABLE;

	return HAL_SPI_Init(&spi_handle) == HAL_OK;
}

void read_pitot() {
	uint8_t buffer[2];
	uint8_t ascii_buffer[4];

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	HAL_SPI_Receive_IT(&spi_handle, buffer, sizeof(buffer));

	uint16_t buffer_value = (buffer[1] * (1 << 8) + buffer[0]) >> 4;
	ui2ascii(buffer_value, ascii_buffer);
	acqBuff_write(acqbuff_Pitot, ascii_buffer, sizeof(ascii_buffer));
}
