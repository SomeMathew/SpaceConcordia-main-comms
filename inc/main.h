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


#define LENGTH_OF_ARRAY(__ARRAY__) ((sizeof(__ARRAY__))/(sizeof(__ARRAY__[0])))

/**
 * @brief Macro to initialize a basic structure to hold an array and its size.
 *
 * The macro uses the array pointer and includes the length of the given array at compile time to
 * to traverse the list safely.
 */
#define ARRAY_WITH_SIZE(__ARRAY__) {(__ARRAY__), (LENGTH_OF_ARRAY(__ARRAY__))}


#endif /* __MAIN_H */
