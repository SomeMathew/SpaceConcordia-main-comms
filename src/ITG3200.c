/**
 * @file ITG3200.c
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Driver for the ITG3200 gyroscope functionality.
 * 
 * This driver currently implements the gyroscope in polling mode and 
 * fills its buffer with the unscaled ADC value.
 */

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "ITG3200.h"
#include "scheduler.h"
#include "logging.h"
#include "acquisitionBuffers.h"
#include "sysTimer.h"

#define SEND_BUFFER_SIZE 32

#define ITG3200_ADDR_AD0_HIGH  0x69   //AD0=1 0x69 I2C address when AD0 is connected to HIGH (VCC) - default for sparkfun breakout
#define ITG3200_ADDR_AD0_LOW   0x68   //AD0=0 0x68 I2C address when AD0 is connected to LOW (GND)
// "The LSB bit of the 7 bit address is determined by the logic level on pin 9. 
// This allows two ITG-3200 devices to be connected to the same I2C bus.
// One device should have pin9 (or bit0) LOW and the other should be HIGH." source: ITG3200 datasheet
// Note that pin9 (AD0 - I2C Slave Address LSB) may not be available on some breakout boards so check 
// the schematics of your breakout board for the correct address to use.


#define GYROSTART_UP_DELAY  70    // 50ms from gyro startup + 20ms register r/w startup

/* ---- Registers ---- */
#define ITG3200_WHO_AM_I           0x00  // RW   SETUP: I2C address   
#define ITG3200_SMPLRT_DIV         0x15  // RW   SETUP: Sample Rate Divider       
#define ITG3200_DLPF_FS            0x16  // RW   SETUP: Digital Low Pass Filter/ Full Scale range
#define ITG3200_INT_CFG            0x17  // RW   Interrupt: Configuration
#define ITG3200_INT_STATUS         0x1A  // R	Interrupt: Status
#define ITG3200_TEMP_OUT           0x1B  // R	SENSOR: Temperature 2bytes
#define ITG3200_GYRO_XOUT          0x1D  // R	SENSOR: Gyro X 2bytes  
#define ITG3200_GYRO_YOUT          0x1F  // R	SENSOR: Gyro Y 2bytes
#define ITG3200_GYRO_ZOUT          0x21  // R	SENSOR: Gyro Z 2bytes
#define ITG3200_PWR_MGM            0x3E  // RW	Power Management

/* ---- bit maps ---- */
#define ITG3200_DLPFFS_FS_SEL             0x18  // 00011000
#define ITG3200_DLPFFS_DLPF_CFG           0x07  // 00000111
#define ITG3200_INTCFG_ACTL               0x80  // 10000000
#define ITG3200_INTCFG_OPEN               0x40  // 01000000
#define ITG3200_INTCFG_LATCH_INT_EN       0x20  // 00100000
#define ITG3200_INTCFG_INT_ANYRD_2CLEAR   0x10  // 00010000
#define ITG3200_INTCFG_ITG_RDY_EN         0x04  // 00000100
#define ITG3200_INTCFG_RAW_RDY_EN         0x01  // 00000001
#define ITG3200_INTSTATUS_ITG_RDY         0x04  // 00000100
#define ITG3200_INTSTATUS_RAW_DATA_RDY    0x01  // 00000001
#define ITG3200_PWRMGM_HRESET             0x80  // 10000000
#define ITG3200_PWRMGM_SLEEP              0x40  // 01000000
#define ITG3200_PWRMGM_STBY_XG            0x20  // 00100000
#define ITG3200_PWRMGM_STBY_YG            0x10  // 00010000
#define ITG3200_PWRMGM_STBY_ZG            0x08  // 00001000
#define ITG3200_PWRMGM_CLK_SEL            0x07  // 00000111

/************************************/
/*    REGISTERS PARAMETERS    */
/************************************/
// Sample Rate Divider
#define ITG3200_NOSRDIVIDER         0 // default    FsampleHz=SampleRateHz/(divider+1)
// Gyro Full Scale Range
#define ITG3200_RANGE2000           3   // default
// Digital Low Pass Filter BandWidth and SampleRate
#define ITG3200_BW256_SR8           0   // default    256Khz BW and 8Khz SR
#define ITG3200_BW188_SR1           1
#define ITG3200_BW098_SR1           2
#define ITG3200_BW042_SR1           3
#define ITG3200_BW020_SR1           4
#define ITG3200_BW010_SR1           5
#define ITG3200_BW005_SR1           6
// Interrupt Active logic lvl
#define ITG3200_ACTIVE_ONHIGH       0 // default
#define ITG3200_ACTIVE_ONLOW        1
// Interrupt drive type
#define ITG3200_PUSH_PULL           0 // default
#define ITG3200_OPEN_DRAIN          1
// Interrupt Latch mode
#define ITG3200_PULSE_50US          0 // default
#define ITG3200_UNTIL_INT_CLEARED   1
// Interrupt Latch clear method
#define ITG3200_READ_STATUSREG      0 // default
#define ITG3200_READ_ANYREG         1
// Power management
#define ITG3200_NORMAL              0 // default
#define ITG3200_STANDBY             1
// Clock Source - user parameters
#define ITG3200_INTERNALOSC         0   // default
#define ITG3200_PLL_XGYRO_REF       1
#define ITG3200_PLL_YGYRO_REF       2
#define ITG3200_PLL_ZGYRO_REF       3
#define ITG3200_PLL_EXTERNAL32      4   // 32.768 kHz
#define ITG3200_PLL_EXTERNAL19      5   // 19.2 Mhz

#define ITG3200_ADDRESS (ITG3200_ADDR_AD0_LOW)
#define ITG3200_CONF_SMPLRT_DIV (0)
#define ITG3200_CONF_DLPF_FS ((ITG3200_RANGE2000 << 3) | ITG3200_BW256_SR8)
#define ITG3200_CONF_PWR_MGM (ITG3200_PLL_XGYRO_REF)


static uint8_t buffer[32];

static struct task * runTask = NULL;
static char testBuffer[128];

static void i2cCallback(uint32_t event, void * args);
static void runLoop(uint32_t event, void * args);

static inline bool timeIsAfter(uint32_t a, uint32_t b) {
    return ((int32_t) (b - a) < 0);

}

int itg3200_open(McuDevice_I2C bus, struct i2c_slaveDevice * device, uint32_t msInterval) {
	struct i2c_slaveConf config = {
		.address = ITG3200_ADDRESS,
		.callback = i2cCallback
	};
	i2c_ioctl_setSlave(bus, device, I2C_SLAVESET_ADDRESS | I2C_SLAVESET_CALLBACK, &config);
	
	uint8_t registerVal =  ITG3200_CONF_SMPLRT_DIV;
	if (i2c_writeRegister_blocking(device, ITG3200_SMPLRT_DIV, I2C_ADDRESS_SIZE_8BIT, &registerVal, 1) != I2C_STATUS_OK) {
		logging_send("set smplrt_div err", MODULE_INDEX_ITG3200, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	
	registerVal = ITG3200_CONF_DLPF_FS;
	if (i2c_writeRegister_blocking(device, ITG3200_DLPF_FS, I2C_ADDRESS_SIZE_8BIT, &registerVal, 1) != I2C_STATUS_OK) {
		logging_send("set DLPF_FS err", MODULE_INDEX_ITG3200, LOG_WARNING);
		return DRIVER_STATUS_ERROR;
	}
	
	registerVal = ITG3200_CONF_PWR_MGM;
	if (i2c_writeRegister_blocking(device, ITG3200_PWR_MGM, I2C_ADDRESS_SIZE_8BIT, &registerVal, 1) != I2C_STATUS_OK) {
		logging_send("set pwr_mgm err", MODULE_INDEX_ITG3200, LOG_WARNING);
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
	
	uint8_t registerData[6];
	i2c_readRegister_blocking(slaveDevice, ITG3200_GYRO_XOUT,  I2C_ADDRESS_SIZE_8BIT, registerData, 6);
	int32_t x = ((registerData[0] << 8) | registerData[1]);
	int32_t y = ((registerData[2] << 8) | registerData[3]);
	int32_t z = ((registerData[4] << 8) | registerData[5]);
	
	
	sprintf(testBuffer, "Raw data: %" PRIx8 ", %"PRIx8", %"PRIx8", %"PRIx8", %"PRIx8 ", %"PRIx8, 
						registerData[0], registerData[1], registerData[2], 
						registerData[3], registerData[4], registerData[5]);
	logging_send(testBuffer, MODULE_INDEX_ITG3200, LOG_DEBUG);
	
	sprintf(testBuffer, "x: %" PRId32", y: %" PRId32 ", z: %" PRId32, x, y, z);
	logging_send(testBuffer, MODULE_INDEX_ITG3200, LOG_DEBUG);
	
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
	
	acqBuff_write(acqbuff_Gyroscope, buffer, i);
}

static void i2cCallback(uint32_t event, void * args) {
	UNUSED(args);
}
