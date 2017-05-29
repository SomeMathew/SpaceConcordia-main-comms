/**
 * @file i2c.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Interrupt or DMA based i2c device driver.
 * 
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
	I2C_EVENT_TRANSFER_DONE,
	I2C_EVENT_ERROR,
};

enum i2c_mode {
	I2C_MODE_IT,
	I2C_MODE_DMA,
	I2C_MODE_POLLING,
};

enum i2c_addressing_mode {
	I2C_ADDRESS_7BIT,
	I2C_ADDRESS_10BIT,
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
    enum i2c_mode transferMode;
    enum i2c_addressing_mode addressingMode;
};

struct i2c_slaveConf {
	uint16_t address;
	void (*callback)(uint32_t, void *);
};

struct i2c_slaveDevice {
	uint8_t address; // in 8 bit format (<7bit addr> << 1)
	McuDevice_I2C bus;
	void (*callback)(uint32_t, void *);
};

enum i2c_status i2c_open(McuDevice_I2C bus, struct i2c_busConf * conf);
enum i2c_status i2c_ioctl_setBus(McuDevice_I2C bus, int busSetMask, struct i2c_busConf * conf); 
enum i2c_status i2c_ioctl_setSlave(McuDevice_I2C bus, struct i2c_slaveDevice * slave, 
		int slaveSetMask, struct i2c_slaveConf); 
		
//~ enum i2c_status i2c_write(struct i2c_slaveDevice * slave, uint8_t * data, size_t size);
//~ enum i2c_status i2c_read(struct i2c_slaveDevice * slave, uint8_t * data, size_t size);
//~ enum i2c_status i2c_start(struct i2c_slaveDevice * slave);
//~ enum i2c_status i2c_stop(struct i2c_slaveDevice * slave);

/**
 * @brief Write a specified register to the slave device, 
enum i2c_status i2c_writeRegister(struct i2c_slaveDevice * slave, uint8_t * data, size_t size);
enum i2c_status i2c_readRegister(struct i2c_slaveDevice * slave, uint8_t * data, size_t size);

#endif /* __I2C_H */
