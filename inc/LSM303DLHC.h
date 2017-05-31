/**
 * @file LSM303DLHC.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Driver for the LSM303DLHC accelerometer functionality.
 * 
 * This driver currently only implements the accelerometer and fills its buffer
 * with the unscaled ADC value. To use call the open function with the I2C device 
 * connected to the accelerometer. The driver will initialize the device and create a scheduler task to run independently.
 */


#ifndef __LSM303DLHC_H
#define __LSM303DLHC_H

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
int lsm303dlhc_open(McuDevice_I2C bus, struct i2c_slaveDevice * device, uint32_t msInterval);

#endif /* __LSM303DLHC_H */
