/**
 * @file LSM303DLHC.c
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Driver for the LSM303DLHC accelerometer functionality.
 * 
 * This driver currently only implements the accelerometer and fills its buffer
 * with the unscaled ADC value.
 */

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "LSM303DLHC.h"
#include "scheduler.h"
#include "logging.h"
#include "acquisitionBuffers.h"

#define LSM303DLHC_ADDRESS_LIN_ACCEL (0b0011001)
#define SEND_BUFFER_SIZE 32

// Credit: Adafruit LSM303DLHC Arduino library
#define	LSM303_REGISTER_ACCEL_CTRL_REG1_A       (0x20) 
#define	LSM303_REGISTER_ACCEL_CTRL_REG2_A       (0x21) 
#define	LSM303_REGISTER_ACCEL_CTRL_REG3_A       (0x22) 
#define	LSM303_REGISTER_ACCEL_CTRL_REG4_A       (0x23) 
#define	LSM303_REGISTER_ACCEL_CTRL_REG5_A       (0x24) 
#define	LSM303_REGISTER_ACCEL_CTRL_REG6_A       (0x25) 
#define	LSM303_REGISTER_ACCEL_REFERENCE_A       (0x26) 
#define	LSM303_REGISTER_ACCEL_STATUS_REG_A      (0x27) 
#define	LSM303_REGISTER_ACCEL_OUT_X_L_A         (0x28)
#define	LSM303_REGISTER_ACCEL_OUT_X_H_A         (0x29)
#define	LSM303_REGISTER_ACCEL_OUT_Y_L_A         (0x2A)
#define	LSM303_REGISTER_ACCEL_OUT_Y_H_A         (0x2B)
#define	LSM303_REGISTER_ACCEL_OUT_Z_L_A         (0x2C)
#define	LSM303_REGISTER_ACCEL_OUT_Z_H_A         (0x2D)
#define	LSM303_REGISTER_ACCEL_FIFO_CTRL_REG_A   (0x2E)
#define	LSM303_REGISTER_ACCEL_FIFO_SRC_REG_A    (0x2F)
#define	LSM303_REGISTER_ACCEL_INT1_CFG_A        (0x30)
#define	LSM303_REGISTER_ACCEL_INT1_SOURCE_A     (0x31)
#define	LSM303_REGISTER_ACCEL_INT1_THS_A        (0x32)
#define	LSM303_REGISTER_ACCEL_INT1_DURATION_A   (0x33)
#define	LSM303_REGISTER_ACCEL_INT2_CFG_A        (0x34)
#define	LSM303_REGISTER_ACCEL_INT2_SOURCE_A     (0x35)
#define	LSM303_REGISTER_ACCEL_INT2_THS_A        (0x36)
#define	LSM303_REGISTER_ACCEL_INT2_DURATION_A   (0x37)
#define	LSM303_REGISTER_ACCEL_CLICK_CFG_A       (0x38)
#define	LSM303_REGISTER_ACCEL_CLICK_SRC_A       (0x39)
#define	LSM303_REGISTER_ACCEL_CLICK_THS_A       (0x3A)
#define	LSM303_REGISTER_ACCEL_TIME_LIMIT_A      (0x3B)
#define	LSM303_REGISTER_ACCEL_TIME_LATENCY_A    (0x3C)
#define	LSM303_REGISTER_ACCEL_TIME_WINDOW_A     (0x3D)

// ODR 50Hz; Normal Mode (Low-power disabled); Z, Y, Z enabled
#define LSM303_CONFIG_CTRL_REG1 0x47

// FS = 11 (+- 16g full scale); HR = 0 (high resolution disabled)
#define LSM303_CONFIG_CTRL_REG4 0x30



static uint8_t buffer[SEND_BUFFER_SIZE];

static struct task * runTask = NULL;
static char testBuffer[128];

static void i2cCallback(uint32_t event, void * args);
static void runLoop(uint32_t event, void * args);

int lsm303dlhc_open(McuDevice_I2C bus, struct i2c_slaveDevice * device, uint32_t msInterval) {
	struct i2c_slaveConf config = {
		.address = LSM303DLHC_ADDRESS_LIN_ACCEL,
		.callback = i2cCallback
	};
	i2c_ioctl_setSlave(bus, device, I2C_SLAVESET_ADDRESS | I2C_SLAVESET_CALLBACK, &config);
	
	uint8_t registerVal = LSM303_CONFIG_CTRL_REG1;
	if (i2c_writeRegister_blocking(device, LSM303_REGISTER_ACCEL_CTRL_REG1_A, I2C_ADDRESS_SIZE_8BIT, &registerVal, 1) != I2C_STATUS_OK) {
		logging_send("set creg1", MODULE_INDEX_LSM303, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	
	registerVal = LSM303_CONFIG_CTRL_REG4;
	if (i2c_writeRegister_blocking(device, LSM303_REGISTER_ACCEL_CTRL_REG4_A, I2C_ADDRESS_SIZE_8BIT, &registerVal, 1) != I2C_STATUS_OK) {
		logging_send("set creg4", MODULE_INDEX_LSM303, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	
	runTask = createTask(runLoop, 0, (void *) device, msInterval, true, 1);
	
	return DRIVER_STATUS_OK;
}

/**
 * @param args will contain the struct i2c_slaveDevice *.
 */
static void runLoop(uint32_t event, void * args) {
	UNUSED(event);
	struct i2c_slaveDevice * slaveDevice = (struct i2c_slaveDevice *) args;
	
	uint8_t x_low = 0, x_high = 0;
	i2c_readRegister_blocking(slaveDevice, LSM303_REGISTER_ACCEL_OUT_X_L_A, I2C_ADDRESS_SIZE_8BIT, &x_low, 1);
	i2c_readRegister_blocking(slaveDevice, LSM303_REGISTER_ACCEL_OUT_X_H_A, I2C_ADDRESS_SIZE_8BIT, &x_high, 1);
	
	//~ sprintf(testBuffer, "%hhx %hhx", x_low, x_high);
	//~ logging_send(testBuffer, MODULE_INDEX_LSM303, LOG_DEBUG);
	

	uint8_t y_low = 0, y_high = 0;
	i2c_readRegister_blocking(slaveDevice, LSM303_REGISTER_ACCEL_OUT_Y_L_A, I2C_ADDRESS_SIZE_8BIT, &y_low, 1);
	i2c_readRegister_blocking(slaveDevice, LSM303_REGISTER_ACCEL_OUT_Y_H_A, I2C_ADDRESS_SIZE_8BIT, &y_high, 1);

	//~ sprintf(testBuffer, "%hhx %hhx", y_low, y_high);
	//~ logging_send(testBuffer, MODULE_INDEX_LSM303, LOG_DEBUG);
	
	
	uint8_t z_low = 0, z_high = 0;
	i2c_readRegister_blocking(slaveDevice, LSM303_REGISTER_ACCEL_OUT_Z_L_A, I2C_ADDRESS_SIZE_8BIT, &z_low, 1);
	i2c_readRegister_blocking(slaveDevice, LSM303_REGISTER_ACCEL_OUT_Z_H_A, I2C_ADDRESS_SIZE_8BIT, &z_high, 1);

	//~ sprintf(testBuffer, "%hhx %hhx", z_low, z_high);
	//~ logging_send(testBuffer, MODULE_INDEX_LSM303, LOG_DEBUG);
	
	int16_t x = 0, y = 0, z = 0;
	
	// div by 16 since it is a left-aligned 12 bit number (undocumented) (safe >> 4 signed)
	x = (int16_t) (((uint16_t) x_high << 8) | (x_low))/16;
	y = (int16_t) (((uint16_t) y_high << 8) | (y_low))/16;
	z = (int16_t) (((uint16_t) z_high << 8) | (z_low))/16;
	
	sprintf(testBuffer, "x= %" PRId16 "; y= %" PRId16 "; z= %" PRId16, x, y, z);
	logging_send(testBuffer, MODULE_INDEX_LSM303, LOG_DEBUG);
	
	//~ uint8_t ctrlReg = 0;
	//~ i2c_readRegister_blocking(slaveDevice, LSM303_REGISTER_ACCEL_CTRL_REG1_A, I2C_ADDRESS_SIZE_8BIT, &ctrlReg, 1);
	//~ sprintf(testBuffer, "ctrl1reg : %" 	PRIx8, ctrlReg);
	//~ logging_send(testBuffer, MODULE_INDEX_LSM303, LOG_DEBUG);
	
	//~ ctrlReg = 0;
	//~ i2c_readRegister_blocking(slaveDevice, LSM303_REGISTER_ACCEL_CTRL_REG4_A, I2C_ADDRESS_SIZE_8BIT, &ctrlReg, 1);
	//~ sprintf(testBuffer, "ctrl4reg : %" 	PRIx8, ctrlReg);
	//~ logging_send(testBuffer, MODULE_INDEX_LSM303, LOG_DEBUG);
	
	int i = 0;
	
	if (x < 0) {
		buffer[i++] = '-';
	}
	uint32_t convertValue = (uint32_t) (x < 0) ? -x : x;
	i += ui2ascii(convertValue, buffer + i);
	
	buffer[i++] = '#';
	
	if (y < 0) {
		buffer[i++] = '-';
	}
	convertValue = (uint32_t) (y < 0) ? -y : y;
	i += ui2ascii(convertValue, buffer + i);
	
	buffer[i++] = '#';
	
	if (z < 0) {
		buffer[i++] = '-';
	}
	convertValue = (uint32_t) (z < 0) ? -z : z;
	i += ui2ascii(convertValue, buffer + i);
	
	acqBuff_write(acqbuff_Accelerometer, buffer, i);
}

static void i2cCallback(uint32_t event, void * args) {
	UNUSED(args);
}
