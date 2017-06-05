/*
 * file: pitot.c
 * TODO add description
 */
#include "pitot.h"
#include "stm32f1xx_hal.h"

static SPI_HandleTypeDef spi_handle;

bool init_pitot(void) {
	spi_handle.Instance               = SPI2;
	spi_handle.Init.Mode              = SPI_MODE_MASTER;
	spi_handle.Init.Direction         = SPI_DIRECTION_1LINE;
	spi_handle.Init.DataSize          = SPI_DATASIZE_16BIT;
	spi_handle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	spi_handle.Init.CLKPhase          = SPI_PHASE_1EDGE;           // Pls check this
	spi_handle.Init.NSS               = SPI_NSS_HARD_OUTPUT;       // and this
	spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; // and this
	spi_handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	spi_handle.Init.TIMode            = SPI_TIMODE_DISABLE;

	return HAL_SPI_Init(&spi_handle) == HAL_OK;
}
