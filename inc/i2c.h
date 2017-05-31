/**
 * @file i2c.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Interrupt or DMA based i2c device driver.
 * 
 * NOTE: Currently only the interrupt based transfer is implemented. 
 */

#ifndef __I2C_H
#define __I2C_H

#include "stm32f1xx.h"
#include "mcuDevices.h"

enum i2c_status {
	I2C_STATUS_BUSY = -2,
	I2C_STATUS_OK = 1,
	I2C_STATUS_ERROR = -1,
};

enum i2c_event {
	I2C_EVENT_TX_TRANSFER_DONE,
	I2C_EVENT_RX_TRANSFER_DONE,
	I2C_EVENT_ERROR,
};

enum i2c_mode {
	I2C_MODE_IT,
	I2C_MODE_DMA, // unavailable
	I2C_MODE_POLLING, // unavailable
};

enum i2c_addressing_mode {
	I2C_ADDRESS_7BIT,
	I2C_ADDRESS_10BIT,
};

enum i2c_addressSize {
	I2C_ADDRESS_SIZE_8BIT = I2C_MEMADD_SIZE_8BIT,
	I2C_ADDRESS_SIZE_16BIT = I2C_MEMADD_SIZE_16BIT,
};

enum i2c_busSetMask {
	I2C_BUSSET_CLOCKSPEED = 0x01,
	I2C_BUSSET_TRANSFERMODE = 0x02,
	I2C_BUSSET_ADDRESSINGMODE = 0x04,
};

enum i2c_slaveSetMask {
	I2C_SLAVESET_ADDRESS = 0x01,
	I2C_SLAVESET_CALLBACK = 0x02,
};

struct i2c_busConf {
    uint32_t clockSpeed; // max 400khz
    enum i2c_mode transferMode; // currently only IT works
    enum i2c_addressing_mode addressingMode;
};

struct i2c_slaveConf {
	uint16_t address; // give in 7 bit format
	void (*callback)(uint32_t, void *);
};

struct i2c_slaveDevice {
	uint8_t address; // in 8 bit format (<7bit addr> << 1)
	void (*callback)(uint32_t, void *);
	McuDevice_I2C bus;
};

int i2c_open(McuDevice_I2C bus, struct i2c_busConf * conf);
//~ int i2c_ioctl_setBus(McuDevice_I2C bus, int busSetMask, struct i2c_busConf * conf); 
int i2c_ioctl_setSlave(McuDevice_I2C bus, struct i2c_slaveDevice * slave, 
		int slaveSetMask, struct i2c_slaveConf * conf); 
		
//~ int i2c_write(struct i2c_slaveDevice * slave, uint8_t * data, size_t size);
//~ int i2c_read(struct i2c_slaveDevice * slave, uint8_t * data, size_t size);
//~ int i2c_start(struct i2c_slaveDevice * slave);
//~ int i2c_stop(struct i2c_slaveDevice * slave);

/**
 * @brief Write a specified register to the slave device.
 */
int i2c_writeRegister(struct i2c_slaveDevice * slave, uint16_t memoryAddress, 
		enum i2c_addressSize addSize, uint8_t * data, size_t size);

/**
 * @brief Read a specified register from the slave device. 
 * 
 * This is a non-blocking read, the callback vector of the slave will be called when the transfer is completed.
 */
int i2c_readRegister(struct i2c_slaveDevice * slave, uint16_t memoryAddress, enum i2c_addressSize addSize, 
		uint8_t * data, size_t size);

#endif /* __I2C_H */
