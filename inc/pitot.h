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
int init_pitot(uint32_t msInverval);


#endif
