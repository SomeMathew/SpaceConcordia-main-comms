/**
 * file: pitot.c
 * TODO add description
 */
#include "pitot.h"

void init_pitot(void) {
	SPI_InitTypeDef spi_handle = { 0 };

	spi_handle.Mode        = SPI_MODE_MASTER;
	spi_handle.Direction   = SPI_DIRECTION_1LINE;
	spi_handle.DataSize    = SPI_DATASIZE_16BIT;
	spi_handle.CLKPolarity = SPI_POLARITY_HIGH;
	spi_handle.CLKPhase    = SPI_PHASE1_EDGE;
	spi_handle.FirstBit    = SPI_FIRSTBIT_MSB;

	return HAL_SPI_INIT(&spi_handle) == HAL_OK:
}
