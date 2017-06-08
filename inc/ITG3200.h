/**
 * @file ITG3200.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Driver for the ITG3200 gyroscope functionality.
 * 
 * This driver currently gyroscope and fills its buffer
 * with the unscaled ADC value. To use call the open function with the I2C device 
 * connected to the gyroscope. The driver will initialize the device and create a 
 * scheduler task to run independently.
 */


#ifndef __ITG3200_H
#define __ITG3200_H

#include <stddef.h>
#include <stdint.h>
#include "mcuDevices.h"
#include "i2c.h"
#include "main.h"

/**
 * @brief initializes the driver and set up the recurring scheduler task
 * @param bus the initialized bus to use
 * @param msInterval millisecond interval to fetch a new value
 * @returns DRIVER_STATUS_OK on success
 */
int itg3200_open(McuDevice_I2C bus, struct i2c_slaveDevice * device, uint32_t msInterval);

#endif /* __ITG3200_H */
