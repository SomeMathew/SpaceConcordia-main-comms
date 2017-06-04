/**
 * @file MPL3115A2.c
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Driver for the LSM303DLHC barometer functionality.
 * 
 * This driver currently only implements the barometer and fills its buffer
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
#include "sysTimer.h"

#define SEND_BUFFER_SIZE 32

// Credit: Adafruit MPL3115A2 Arduino library
/*=========================================================================
I2C ADDRESS/BITS
-----------------------------------------------------------------------*/
#define MPL3115A2_ADDRESS                       (0x60)    // 1100000
/*=========================================================================*/

/*=========================================================================
REGISTERS
-----------------------------------------------------------------------*/
#define MPL3115A2_REGISTER_STATUS               (0x00)
#define MPL3115A2_REGISTER_STATUS_TDR           (0x02)
#define MPL3115A2_REGISTER_STATUS_PDR           (0x04)
#define MPL3115A2_REGISTER_STATUS_PTDR          (0x08)

#define MPL3115A2_REGISTER_PRESSURE_MSB         (0x01)
#define MPL3115A2_REGISTER_PRESSURE_CSB         (0x02)
#define MPL3115A2_REGISTER_PRESSURE_LSB         (0x03)

#define MPL3115A2_REGISTER_TEMP_MSB             (0x04)
#define MPL3115A2_REGISTER_TEMP_LSB             (0x05)

#define MPL3115A2_REGISTER_DR_STATUS            (0x06)

#define MPL3115A2_OUT_P_DELTA_MSB               (0x07)
#define MPL3115A2_OUT_P_DELTA_CSB               (0x08)
#define MPL3115A2_OUT_P_DELTA_LSB               (0x09)

#define MPL3115A2_OUT_T_DELTA_MSB               (0x0A)
#define MPL3115A2_OUT_T_DELTA_LSB               (0x0B)

#define MPL3115A2_WHOAMI                        (0x0C)

#define MPL3115A2_PT_DATA_CFG                   (0x13)
#define MPL3115A2_PT_DATA_CFG_TDEFE             (0x01)
#define MPL3115A2_PT_DATA_CFG_PDEFE             (0x02)
#define MPL3115A2_PT_DATA_CFG_DREM              (0x04)

#define MPL3115A2_CTRL_REG1                     (0x26)
#define MPL3115A2_CTRL_REG1_SBYB                (0x01)
#define MPL3115A2_CTRL_REG1_OST                 (0x02)
#define MPL3115A2_CTRL_REG1_RST                 (0x04)
#define MPL3115A2_CTRL_REG1_OS1                 (0x00)
#define MPL3115A2_CTRL_REG1_OS2                 (0x08)
#define MPL3115A2_CTRL_REG1_OS4                 (0x10)
#define MPL3115A2_CTRL_REG1_OS8                 (0x18)
#define MPL3115A2_CTRL_REG1_OS16                (0x20)
#define MPL3115A2_CTRL_REG1_OS32                (0x28)
#define MPL3115A2_CTRL_REG1_OS64                (0x30)
#define MPL3115A2_CTRL_REG1_OS128               (0x38)
#define MPL3115A2_CTRL_REG1_RAW                 (0x40)
#define MPL3115A2_CTRL_REG1_ALT                 (0x80)
#define MPL3115A2_CTRL_REG1_BAR                 (0x00)
#define MPL3115A2_CTRL_REG2                     (0x27)
#define MPL3115A2_CTRL_REG3                     (0x28)
#define MPL3115A2_CTRL_REG4                     (0x29)
#define MPL3115A2_CTRL_REG5                     (0x2A)

#define MPL3115A2_REGISTER_STARTCONVERSION      (0x12)

// Default Bar mode, non-active, ost 16x mode
#define MPL3115A2_CTRL_REG1_VALUE (MPL3115A2_CTRL_REG1_BAR | MPL3115A2_CTRL_REG1_OS16)
//Data flags enabled
#define MPL3115A2_PT_DATA_CFG_VALUE	(MPL3115A2_PT_DATA_CFG_DREM |  MPL3115A2_PT_DATA_CFG_TDEFE | MPL3115A2_PT_DATA_CFG_PDEFE)
// TimeOut for reset
#define TIME_OUT_RESET 5000


static uint8_t buffer[32];

static struct task * runTask = NULL;
static char testBuffer[128];

static void i2cCallback(uint32_t event, void * args);
static void runMPLLoop(uint32_t event, void * args);

static inline bool timeIsAfter(uint32_t a, uint32_t b) {
    return ((int32_t) (b - a) < 0);

}

int mpl3115a2_open(McuDevice_I2C bus, struct i2c_slaveDevice * device, uint32_t msInterval) {
	struct i2c_slaveConf config = {
		.address = MPL3115A2_ADDRESS,
		.callback = i2cCallback
	};
	i2c_ioctl_setSlave(bus, device, I2C_SLAVESET_ADDRESS | I2C_SLAVESET_CALLBACK, &config);
	
	//~ // Reset the device for known value
	uint8_t registerVal = MPL3115A2_CTRL_REG1_RST;
	//~ i2c_writeRegister_blocking(device, MPL3115A2_CTRL_REG1, I2C_ADDRESS_SIZE_8BIT, &registerVal, 1);
	// Wait for the RST bit to clear
	uint32_t timeOutLimit = sysTimer_GetTick() + TIME_OUT_RESET;
	do {
		registerVal = i2c_readRegister_blocking(device, MPL3115A2_CTRL_REG1, I2C_ADDRESS_SIZE_8BIT, &registerVal, 1);
		
		sprintf(testBuffer, "ctrlReg1 %" PRIx8, registerVal);
		logging_send(testBuffer, MODULE_INDEX_MPL311, LOG_DEBUG);
		
		registerVal &= MPL3115A2_CTRL_REG1_RST;
		
		sprintf(testBuffer, "ctrlReg1 new %" PRIx8, registerVal);
		logging_send(testBuffer, MODULE_INDEX_MPL311, LOG_DEBUG);

		if (timeIsAfter(sysTimer_GetTick(), timeOutLimit)) {
			logging_send("timeOut rst MPL311", MODULE_INDEX_MPL311, LOG_CRITICAL);
			return DRIVER_STATUS_ERROR;
		} 
	} while (registerVal);
	
	
	registerVal = MPL3115A2_CTRL_REG1_VALUE;
	if (i2c_writeRegister_blocking(device, MPL3115A2_CTRL_REG1, I2C_ADDRESS_SIZE_8BIT, &registerVal, 1) != I2C_STATUS_OK) {
		logging_send("set creg1 err", MODULE_INDEX_MPL311, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	
	registerVal = MPL3115A2_PT_DATA_CFG_VALUE;
	if (i2c_writeRegister_blocking(device, MPL3115A2_PT_DATA_CFG, I2C_ADDRESS_SIZE_8BIT, &registerVal, 1) != I2C_STATUS_OK) {
		logging_send("set ptData err", MODULE_INDEX_MPL311, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	
	registerVal = MPL3115A2_CTRL_REG1_SBYB;
	if (i2c_writeRegister_blocking(device, MPL3115A2_CTRL_REG1, I2C_ADDRESS_SIZE_8BIT, &registerVal, 1) != I2C_STATUS_OK) {
		logging_send("set SBYB Active", MODULE_INDEX_MPL311, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	
	runTask = createTask(runMPLLoop, 0, (void *) device, msInterval, true, 1);
	
	return DRIVER_STATUS_OK;
}

/**
 * @param args will contain the struct i2c_slaveDevice *.
 */
static void runMPLLoop(uint32_t event, void * args) {
	UNUSED(event);
	struct i2c_slaveDevice * slaveDevice = (struct i2c_slaveDevice *) args;
	
	// Check if Data is ready
	uint8_t registerData = 0;
	i2c_readRegister_blocking(slaveDevice, MPL3115A2_REGISTER_STATUS,  I2C_ADDRESS_SIZE_8BIT, &registerData, 1);
	
	if (!(registerData & MPL3115A2_REGISTER_STATUS_PDR)) {
		logging_send("data nrdy", MODULE_INDEX_MPL311, LOG_WARNING);
		return;
	}
	
	
	uint8_t pMSB = 0, pCSB = 0, pLSB = 0;
	i2c_readRegister_blocking(slaveDevice, MPL3115A2_REGISTER_PRESSURE_MSB, I2C_ADDRESS_SIZE_8BIT, &pMSB, 1);
	i2c_readRegister_blocking(slaveDevice, MPL3115A2_REGISTER_PRESSURE_CSB, I2C_ADDRESS_SIZE_8BIT, &pCSB, 1);	
	i2c_readRegister_blocking(slaveDevice, MPL3115A2_REGISTER_PRESSURE_LSB, I2C_ADDRESS_SIZE_8BIT, &pLSB, 1);
	
	sprintf(testBuffer, "msb: %" PRIx8", csb: %" PRIx8 ", lsb: %" PRIx8, pMSB, pCSB, pLSB);
	logging_send(testBuffer, MODULE_INDEX_MPL311, LOG_DEBUG);
	
	uint32_t intPart = 0;
	intPart = (pMSB << 10) | (pCSB << 2) | (pLSB >> 6);
	
	// Complement to a 32 bit neg number if it's negative
	if (intPart & 0x20000) {
		intPart |= 0xFFFE0000;
	}
	uint32_t fracPart = (pLSB >> 4) & 0x3;
	
	int32_t decValue = (int32_t) intPart;
	
	
	
	int i = 0;
	
	if (decValue < 0) {
		buffer[i++] = '-';
	}
	uint32_t convertValue = (uint32_t) (decValue < 0) ? -decValue : decValue;
	i += ui2ascii(convertValue, buffer + i);
	
	buffer[i++] = '.';
	
	fracPart = 25 * fracPart;
	i += ui2ascii(fracPart, buffer + i);
	
	sprintf(testBuffer, "bar val %" PRId32, convertValue);
	logging_send(testBuffer, MODULE_INDEX_MPL311, LOG_DEBUG);
	
	acqBuff_write(acqbuff_Barometer, buffer, i);
}

static void i2cCallback(uint32_t event, void * args) {
	UNUSED(args);
}
