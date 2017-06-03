/**
 * @file i2c.c
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Interrupt or DMA based i2c device driver.
 * 
 */

#include <stddef.h>
#include "i2c.h"
#include "stm32f1xx_hal.h"
#include "circularBuffer.h"
#include "scheduler.h"

#define DEFAULT_CLOCKSPEED 400000
#define DEFAULT_DUTYCYCLE I2C_DUTYCYCLE_2
#define DEFAULT_ADDRESS 0x30F
#define DEFAULT_ADDRESSMODE I2C_ADDRESSINGMODE_7BIT
#define DEFAULT_DUALADDRESSMODE I2C_DUALADDRESS_DISABLE
#define DEFAULT_ADDRESS2 0xFE
#define DEFAULT_GENCALLMODE I2C_GENERALCALL_DISABLE
#define DEFAULT_NOSTRETCHMODE I2C_NOSTRETCH_DISABLE

#define BLOCKING_TIMEOUT_MS 500


// Internal peripheral structure, hi2c Handle should be first to make
// casting possible between i2c_Peripheral and I2C_HandleTypeDef.
struct i2c_Peripheral {
	I2C_HandleTypeDef hi2c;
	void (*callback)(uint32_t, void *);
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
	},
	.callback = NULL,
};

static struct i2c_Peripheral device_i2c1 = {
	.hi2c = {
		.Instance = I2C1,
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
	},
	.callback = NULL,
};

McuDevice_I2C mcuDevice_i2cBus1 = &device_i2c1;
McuDevice_I2C mcuDevice_i2cBus2 = &device_i2c2;


static inline void setAddressingMode(I2C_HandleTypeDef * device, enum i2c_addressing_mode mode) {
	if (mode == I2C_ADDRESS_10BIT) {
		device->Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
	} else {
		device->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	}
}

static inline int mapStatusFromHAL(HAL_StatusTypeDef status) {
	if (status == HAL_BUSY) {
		return I2C_STATUS_BUSY;
	} else if (status != HAL_OK) {
		return I2C_STATUS_ERROR;
	} else {
		return I2C_STATUS_OK;
	}
}

int i2c_open(McuDevice_I2C bus, struct i2c_busConf * conf) {
	struct i2c_Peripheral * device = (struct i2c_Peripheral *) bus;
	//~ device->hi2c.Init.ClockSpeed = conf->clockSpeed;
	//~ setAddressingMode(&(device->hi2c), conf->addressingMode);
	
	if (HAL_I2C_Init(&(device->hi2c)) == HAL_OK) {
		return I2C_STATUS_OK;
	} else {
		return I2C_STATUS_ERROR;
	}
}

//~ int i2c_ioctl_setBus(McuDevice_I2C bus, int busSetMask, struct i2c_busConf * conf) {
	//~ I2C_HandleTypeDef * device = (I2C_HandleTypeDef *) &((struct i2c_Peripheral *) bus)->hi2c;
	//~ if (busSetMask & I2C_BUSSET_CLOCKSPEED) {
		//~ device->Init.ClockSpeed = conf->clockSpeed;
	//~ }
	
	//~ if (busSetMask & I2C_BUSSET_ADDRESSINGMODE) {
		//~ setAddressingMode(device, conf->addressingMode);
	//~ }
	//~ return I2C_STATUS_OK;
//~ }

int i2c_ioctl_setSlave(McuDevice_I2C bus, struct i2c_slaveDevice * slave, 
		int slaveSetMask, struct i2c_slaveConf * conf) {
	slave->bus = bus;
	if (slaveSetMask & I2C_SLAVESET_ADDRESS) {
		slave->address = ((conf->address) << 1); 
	}
	
	if (slaveSetMask & I2C_SLAVESET_CALLBACK) {
		slave->callback = conf->callback;
	}
	
	return I2C_STATUS_OK;
}

int i2c_writeRegister(struct i2c_slaveDevice * slave, uint16_t memoryAddress, 
		enum i2c_addressSize addSize, uint8_t * data, size_t size) {
	struct i2c_Peripheral * device = (struct i2c_Peripheral *) slave->bus;
	
	int status = HAL_I2C_Mem_Write_IT(&(device->hi2c), slave->address, memoryAddress, addSize, data, size);
	return mapStatusFromHAL(status);
}

int i2c_readRegister(struct i2c_slaveDevice * slave, uint16_t memoryAddress, enum i2c_addressSize addSize, 
		uint8_t * data, size_t size) {
	struct i2c_Peripheral * device = (struct i2c_Peripheral *) slave->bus;

	device->callback = slave->callback;
	
	int status = HAL_I2C_Mem_Read_IT(&(device->hi2c), slave->address, memoryAddress, addSize, data, size);
	return mapStatusFromHAL(status);
}

int i2c_writeRegister_blocking(struct i2c_slaveDevice * slave, uint16_t memoryAddress, 
		enum i2c_addressSize addSize, uint8_t * data, size_t size) {
	struct i2c_Peripheral * device = (struct i2c_Peripheral *) slave->bus;
	
	int status = HAL_I2C_Mem_Write(&(device->hi2c), slave->address, memoryAddress, addSize, data, size, BLOCKING_TIMEOUT_MS);
	return mapStatusFromHAL(status);
}

int i2c_readRegister_blocking(struct i2c_slaveDevice * slave, uint16_t memoryAddress, enum i2c_addressSize addSize, 
		uint8_t * data, size_t size) {
	struct i2c_Peripheral * device = (struct i2c_Peripheral *) slave->bus;

	device->callback = slave->callback;
	
	int status = HAL_I2C_Mem_Read(&(device->hi2c), slave->address, memoryAddress, addSize, data, size, BLOCKING_TIMEOUT_MS);
	return mapStatusFromHAL(status);
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	struct i2c_Peripheral * device = (struct i2c_Peripheral *) hi2c;
	createTask(device->callback, I2C_EVENT_RX_TRANSFER_DONE, NULL,	0, false, 0);
}

void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&device_i2c1.hi2c);
}

void I2C1_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&device_i2c1.hi2c);
}

void I2C2_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&device_i2c2.hi2c);
}

void I2C2_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&device_i2c2.hi2c);
}

