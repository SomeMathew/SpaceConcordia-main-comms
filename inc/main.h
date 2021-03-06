/**
 * @file main.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief main project header
 * 
 * This header should be included by all module of the application, it includes needed definitions
 * by all modules.
 * 
 */
 
#ifndef __MAIN_H
#define __MAIN_H
 
#include <stdint.h>
 
#include "stm32f1xx_hal.h"
#include "scheduler.h"
#include "mcuDevices.h"


#define LENGTH_OF_ARRAY(__ARRAY__) ((sizeof(__ARRAY__))/(sizeof(__ARRAY__[0])))

/**
 * @brief Macro to initialize a basic structure to hold an array and its size.
 *
 * The macro uses the array pointer and includes the length of the given array at compile time to
 * to traverse the list safely.
 */
#define ARRAY_WITH_SIZE(__ARRAY__) {(__ARRAY__), (LENGTH_OF_ARRAY(__ARRAY__))}

// Sensor polling rate

#define POLLING_RATE_ACCEL 50
#define POLLING_RATE_BAROMETER 50
#define POLLING_RATE_PITOT 50

// Module index for logging control

#define MODULE_INDEX_MAINTEST 0
#define MODULE_INDEX_DATA_GATHERER 1
#define MODULE_INDEX_XBEE 2
#define MODULE_INDEX_I2C 3
#define MODULE_INDEX_LSM303 4
#define MODULE_INDEX_MPL311 5
#define MODULE_INDEX_SPI 6
#define MODULE_INDEX_PITOT 7

// Device drivers and GPIO configurations
// TODO more clean implementation or move to an other module?


#define SERIALPC_CONF_BAUDRATE 9600
#define SERIALPC_CONF_WORDLENGTH UART_WORDLENGTH_8B
#define SERIALPC_CONF_STOPBITS UART_STOPBITS_1
#define SERIALPC_CONF_PARITY UART_PARITY_NONE

#define XBEE_CONF_BAUDRATE 57600
#define XBEE_CONF_WORDLENGTH UART_WORDLENGTH_8B
#define XBEE_CONF_STOPBITS UART_STOPBITS_1
#define XBEE_CONF_PARITY UART_PARITY_NONE

#define LSM303DLHC_MS_INTERVAL 200

enum {
    DRIVER_STATUS_ERROR = -1,
    DRIVER_STATUS_OK = 1,
};

/**
 * @brief Converts an unsigned int to ASCII inside the buffer
 * @returns the number of character written to buffer
 */
size_t ui2ascii(uint32_t n, uint8_t* buffer);

#endif /* __MAIN_H */
