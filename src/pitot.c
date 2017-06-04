/**
 * file: pitot.c
 * TODO add description
 */
#include "pitot.h"

bool init_pitot(void) {
	SPI_HandleTypeDef spi_handle = {0};

	spi_handle.Instance         = SPIx;
	spi_handle.Init.Mode        = SPI_MODE_MASTER;
	spi_handle.Init.Direction   = SPI_DIRECTION_1LINE;
	spi_handle.Init.DataSize    = SPI_DATASIZE_16BIT;
	spi_handle.Init.CLKPolarity = SPI_POLARITY_HIGH;
	spi_handle.Init.CLKPhase    = SPI_PHASE1_EDGE;
	spi_handle.Init.FirstBit    = SPI_FIRSTBIT_MSB;

	return HAL_SPI_INIT(&spi_handle) == HAL_OK:
}
