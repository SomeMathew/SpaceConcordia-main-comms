/*
 * file: pitot.h
 * Declares functions for initializing and reading from the pitot tube ADC.
 */
#ifndef PITOT_H
#define PITOT_H

#include <stdbool.h>
#include "stm32f1xx.h"

/*
 * Initializes the HAL device struct. Returns true if initialization was
 * successful.
 */
bool init_pitot(void);

/*
 * Reads from the pitot tube ADC, converts value to ASCII, and stores in the
 * pitot tube's acquisition buffer.
 */
void read_pitot();

#endif
