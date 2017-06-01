/**
 * file: pitot.c
 * TODO add description
 */
#include "pitot.h"

void init_pitot(void) {
	SPI_InitTypeDef spi_handle = { 0 };

	spi_init.Mode        = SPI_MODE_MASTER;
	spi_init.Direction   = SPI_DIRECTION_1LINE;
	spi_init.DataSize    = SPI_DATASIZE_16BIT;
	spi_init.CLKPolarity = SPI_POLARITY_HIGH;
	spi_init.CLKPhase    = SPI_PHASE1_EDGE;
	spi_init.FirstBit    = SPI_FIRSTBIT_MSB;

	return HAL_SPI_INIT(&spi_handle) == HAL_OK:
}
