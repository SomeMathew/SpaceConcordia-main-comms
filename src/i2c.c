/**
 * @file i2c.c
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Interrupt or DMA based i2c device driver.
 * 
 */

#include "i2c.h"
#include "stm32f1xx_hal.h"
#include "circularBuffer.h"

#define DEFAULT_CLOCKSPEED 400000
#define DEFAULT_DUTYCYCLE I2C_DUTYCYCLE_2
#define DEFAULT_ADDRESS 0x30F
#define DEFAULT_ADDRESSMODE I2C_ADDRESSINGMODE_7BIT
#define DEFAULT_DUALADDRESSMODE I2C_DUALADDRESS_DISABLE
#define DEFAULT_ADDRESS2 0xFF
#define DEFAULT_GENCALLMODE I2C_GENERALCALL_DISABLE
#define DEFAULT_NOSTRETCHMODE I2C_NOSTRETCH_DISABLE


// Internal peripheral structure, hi2c Handle should be first to make
// casting possible between i2c_Peripheral and I2C_HandleTypeDef.
struct i2c_Peripheral {
	I2C_HandleTypeDef hi2c;
};

static struct i2c_Peripheral device_i2c2 = {
	.hi2c = {
		.Instance = I2C2,
		.Init = {
			.ClockSpeed = DEFAULT_CLOCKSPEED,
			.DutyCycle = DEFAULT_DUTYCYCLE,
			.OwnAddress1 = DEFAULT_ADDRESS,
			.AddressingMode = DEFAULT_ADDRESSMODE,
			.DualAddressMode = DEFAULT_DUALADDRESSMODE,
			.OwnAddress2 = DEFAULT_ADDRESS2,
			.GeneralCallMode = DEFAULT_GENCALLMODE,
			.NoStretchMode = DEFAULT_NOSTRETCHMODE,
		},
	}
};

McuDevice_I2C mcuDevice_i2cBus2 = &device_i2c2;

static inline setAddressingMode(I2C_HandleTypeDef * device, enum i2c_addressing_mode mode) {
	if (mode == I2C_ADDRESS_10BIT) {
		device->Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
	} else {
		device->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	}
}

enum i2c_status i2c_open(McuDevice_I2C bus, struct i2c_busConf * conf) {
	I2C_HandleTypeDef * device = (I2C_HandleTypeDef *) &((struct i2c_Peripheral *) bus)->hi2c;
	device->Init.ClockSpeed = conf->clockSpeed;
	setAddressingMode(device, conf->addressingMode);
	
	if (HAL_I2C_Init(device) == HAL_OK) {
		return I2C_STATUS_OK;
	} else {
		return I2C_STATUS_ERROR;
	}
}
enum i2c_status i2c_ioctl_setBus(McuDevice_I2C bus, int busSetMask, struct i2c_busConf * conf) {
	I2C_HandleTypeDef * device = (I2C_HandleTypeDef *) &((struct i2c_Peripheral *) bus)->hi2c;
	if (busSetMask & I2C_BUSSET_CLOCKSPEED) {
		device->Init.ClockSpeed = conf->clockSpeed;
	}
	
	if (busSetMask & I2C_BUSSET_ADDRESSINGMODE) {
		setAddressingMode(device, conf->addressingMode);
	}
	return I2C_STATUS_OK;
}
enum i2c_status i2c_ioctl_setSlave(McuDevice_I2C bus, struct i2c_slaveDevice * slave, 
		int slaveSetMask, struct i2c_slaveConf); 

enum i2c_status i2c_writeRegister(struct i2c_slaveDevice * slave, uint8_t * data, size_t size);
enum i2c_status i2c_readRegister(struct i2c_slaveDevice * slave, uint8_t * data, size_t size);
