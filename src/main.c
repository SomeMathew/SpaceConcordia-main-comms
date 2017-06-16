/**
 * @file main.c
 * @author Space Concordia Rocket division
 * @author Mathieu Breault
 * @brief main project loop and setup
 *
 * This module sets up the system and event scheduler. Contains the work loop.
 *
 */



#include "main.h"
#include "uart.h"
#include "logging.h"
#include "commands.h"
#include "xbee.h"
#include "mockDevice.h"
#include "pitot.h"
#include "dataGatherer.h"
#include "LSM303DLHC.h"
#include "i2c.h"
#include "MPL3115A2.h"

static void clockConfig(void);
static void initBlinkGPIO(void);
static int initLoggingUART(void);
static int initXbeeUART(void);
static int loggingStream(uint8_t * data, size_t size);

static void blink(uint32_t, void *);


int main(void) {
	// Call the CSMSIS system clock routine to store the clock frequency
    // in the SystemCoreClock global RAM location.
    SystemCoreClockUpdate();
	// Initialize the HAL Device Library
	HAL_Init();
	clockConfig();
	
	
	initBlinkGPIO();
	createTask(blink, 0, NULL, 1000, true, 2);
	
	initLoggingUART();
	logging_open(loggingStream);
	commands_init(mcuDevice_serialPC);

	if (initXbeeUART() == DRIVER_STATUS_ERROR) {
		logging_send("Failed opening Xbee", MODULE_INDEX_XBEE, LOG_WARNING);
	} else {
		logging_send("Xbee opened", MODULE_INDEX_XBEE, LOG_DEBUG);
	}

	mockDevice_init();
	data_gatherer_init();

	struct i2c_busConf i2cBus1Config = {
		.clockSpeed = 400000,
		.addressingMode = I2C_ADDRESSINGMODE_7BIT
	};
	struct i2c_slaveDevice lsm303_accel_slaveDevice = {0};
	if (i2c_open(mcuDevice_i2cBus1, &i2cBus1Config) != I2C_STATUS_OK) {
		logging_send("Error opening i2c bus1", MODULE_INDEX_I2C, LOG_WARNING);
	} else {
		logging_send("I2C1 Opened", MODULE_INDEX_I2C, LOG_DEBUG);
	}
	lsm303dlhc_open(mcuDevice_i2cBus1, &lsm303_accel_slaveDevice, POLLING_RATE_ACCEL);
	

	struct i2c_busConf i2cBus2Config = {
		.clockSpeed = 400000,
		.addressingMode = I2C_ADDRESSINGMODE_7BIT
	};
	struct i2c_slaveDevice mpl311_barometer = {0};
	
	if (i2c_open(mcuDevice_i2cBus2, &i2cBus2Config) != I2C_STATUS_OK) {
		logging_send("Error opening I2C2", MODULE_INDEX_I2C, LOG_WARNING);
	} else {
		logging_send("I2C2 Opened", MODULE_INDEX_I2C, LOG_DEBUG);
	}
	
	mpl3115a2_open(mcuDevice_i2cBus2, &mpl311_barometer, POLLING_RATE_BAROMETER);
	
	if (init_pitot(POLLING_RATE_PITOT) != DRIVER_STATUS_OK) {
		logging_send("Error opening SPI", MODULE_INDEX_SPI, LOG_WARNING);
	} else {
		logging_send("SPI Opened", MODULE_INDEX_SPI, LOG_DEBUG);
	}

	while(1) {
		runScheduler();
	}
}

size_t ui2ascii(uint32_t n, uint8_t* buffer) {
	uint8_t reverse_digits[10];
	size_t  i = 0;
	do {
		reverse_digits[i] = '0' + n % 10;
		n /= 10;
		++i;
	} while (n);

	for (size_t j = 0; j < i; ++j) {
		buffer[j] = reverse_digits[i - j - 1];
	}

	return i;
}

static void blink(uint32_t event, void * arg) {
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
}



static void initBlinkGPIO(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gpioInit = {
		.Pin = GPIO_PIN_0,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW,
	};

	return HAL_GPIO_Init(GPIOA, &gpioInit);
}

static int initLoggingUART(void) {
	struct uart_ioConf setConfig = {
		.baudrate = SERIALPC_CONF_BAUDRATE,
		.parity = SERIALPC_CONF_PARITY,
		.wordlength = SERIALPC_CONF_WORDLENGTH,
		.stopbits = SERIALPC_CONF_STOPBITS,
	};

	return uart_open(mcuDevice_serialPC, &setConfig);
}

static int initXbeeUART(void) {
	struct uart_ioConf setConfig = {
		.baudrate = XBEE_CONF_BAUDRATE,
		.parity = XBEE_CONF_PARITY,
		.wordlength = XBEE_CONF_WORDLENGTH,
		.stopbits = XBEE_CONF_STOPBITS,
	};

	uart_open(mcuDevice_serialXBee, &setConfig);
	return xbee_open(mcuDevice_serialXBee);
}

static int loggingStream(uint8_t * data, size_t size) {
	return uart_write(mcuDevice_serialPC, data, size);
}

/**
 * @brief System clock configuration
 *
 * Currently configured as:
 *            System Clock source            = PLL (HSE Bypass)
 *            SYSCLK(Hz)                     = 72000000
 *            HCLK(Hz)                       = 72000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            PLLMUL                         = 9
 *            Flash Latency(WS)              = 2
 */
static void clockConfig(void) {
	RCC_OscInitTypeDef oscInit = {
		.OscillatorType = RCC_OSCILLATORTYPE_HSE,
		.HSEState = RCC_HSE_ON,
		.HSEPredivValue = RCC_HSE_PREDIV_DIV1,
		.LSEState = RCC_LSE_OFF,
		.HSIState = RCC_HSI_OFF,
		.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
		.LSIState = RCC_LSI_OFF,
		.PLL = {
			.PLLState = RCC_PLL_ON,
			.PLLSource = RCC_PLLSOURCE_HSE,
			.PLLMUL = RCC_PLL_MUL9,
		},
	};

	HAL_RCC_OscConfig(&oscInit);

	RCC_ClkInitTypeDef clkInit = {
		.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
		.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
		.AHBCLKDivider = RCC_SYSCLK_DIV1,
		.APB1CLKDivider = RCC_HCLK_DIV2,
		.APB2CLKDivider = RCC_HCLK_DIV1,
	};

	HAL_RCC_ClockConfig(&clkInit, FLASH_LATENCY_2);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
