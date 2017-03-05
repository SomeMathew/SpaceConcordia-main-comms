/**
 * @file mcuDevices.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Device structures to abstract the MCU peripherals.
 * 
 * The MCU specific implementation should be in mcuDevices.c, and in the <mcuDevice.c> implementation
 * for the given device driver.
 */

#ifndef __MCU_DEVICES_H
#define __MCU_DEVICES_H

typedef void * McuDevice_I2C;
typedef void * McuDevice_UART;

extern McuDevice_I2C mcuDevice_i2cBus1;
extern McuDevice_I2C mcuDevice_i2cBus2;

extern McuDevice_UART mcuDevice_serialPC;
extern McuDevice_UART mcuDevice_serialXBee;

#endif /* __MCU_DEVICES_H */

