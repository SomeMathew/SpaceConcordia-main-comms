/*
 * sysTimer.c
 *
 *  Created on: Jul 28, 2016
 *      Author: math
 */

#include "sysTimer.h"
#include "main.h"

void SysTick_Handler(void)
{
  HAL_IncTick();
}

uint32_t sysTimer_GetTick(void) {
    return HAL_GetTick();
}



