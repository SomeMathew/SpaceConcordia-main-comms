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

static void clockConfig(void);
static void initBlinkGPIO(void);
static void initTestUart(void);
static int	initTestXbee(void);


static void blink(uint32_t, void *);
static void sendTestUart(uint32_t event, void * arg);
static void sendTestLog(uint32_t event, void * arg);
static void sendTestXbee(uint32_t event, void * arg);
static void changeTestLog(uint32_t event, void * arg);
static void uartRead(uint32_t event, void * arg);
static int loggingStream(uint8_t * data, size_t size);

uint8_t testData[] = "test\n";
size_t testDataSize = LENGTH_OF_ARRAY(testData) - 1;

bool changeLogTestActive = true;

int main(void) {
	// Call the CSMSIS system clock routine to store the clock frequency
    // in the SystemCoreClock global RAM location.
    SystemCoreClockUpdate();
	// Initialize the HAL Device Library
	HAL_Init();
	clockConfig();

	initBlinkGPIO();
	initTestUart();


	logging_open(loggingStream);
	commands_init(mcuDevice_serialPC);

	if (initTestXbee() == DRIVER_STATUS_ERROR) {
		logging_send("Failed opening Xbee", MODULE_INDEX_XBEE, LOG_WARNING);
	}

	mockDevice_init();
	data_gatherer_init();
        init_pitot();
	//~ initTestXbee();
	//~ struct task * blinkTask = createTask(blink, 0, NULL, 1000, true, 0);

	struct task * logTestTask = createTask(sendTestLog, 0, NULL, 500, true, 0);
	//~ struct task * uartTestTask = createTask(sendTestUart, 0, NULL, 2000, true, 0);
	//~ struct task * logChangeTask = createTask(changeTestLog, 0, &changeLogTestActive, 3000, true, 1);
	//~ struct task * uartReadTask = createTask(uartRead, 0, NULL, 200, true, 0);

	//~ struct task * xbeeTestTask = createTask(sendTestXbee, 0, NULL, 500, true, 0);
	while(1) {
		//~ HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		runScheduler();
		//~ HAL_Delay(5000);
	}
}

static void blink(uint32_t event, void * arg) {
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

static void sendTestUart(uint32_t event, void * arg) {
	uart_write(mcuDevice_serialXBee, testData, testDataSize);
}

static void sendTestLog(uint32_t event, void * arg) {
	logging_send("testLog debug", MODULE_INDEX_MAINTEST, LOG_DEBUG);
	logging_send("testLog warning", MODULE_INDEX_MAINTEST, LOG_WARNING);
	//~ logging_send("testLog critical +1", MODULE_INDEX_MAINTEST + 3, LOG_CRITICAL);
	logging_send("testLog critical", MODULE_INDEX_MAINTEST, LOG_CRITICAL);
	logging_send("testLog critical 24", 24, LOG_CRITICAL);
}

static void sendTestXbee(uint32_t event, void * arg) {
	if (xbee_write(testData, testDataSize) == DRIVER_STATUS_ERROR) {
		logging_send("Xbee ERROR", MODULE_INDEX_XBEE, LOG_WARNING);
	}
	logging_send("Xbee Test", MODULE_INDEX_XBEE, LOG_DEBUG);
}

static void changeTestLog(uint32_t event, void * arg) {
	if (*((bool *) arg)) {
		logging_setVerbosity(LOG_DEBUG | LOG_CRITICAL);
		logging_filterModule(MODULE_INDEX_MAINTEST + 1, false);
	} else {
		logging_setVerbosity(LOG_DEBUG | LOG_WARNING | LOG_CRITICAL);
		logging_filterModule(MODULE_INDEX_MAINTEST + 1, true);
	}
	*((bool *) arg) = !(*((bool *) arg));
}

static void uartRead(uint32_t event, void * arg) {
	uint8_t buffer[50];
	size_t readSize = uart_read(mcuDevice_serialPC, buffer, 49);
	if (readSize > 0) {
		buffer[readSize] = '\0';
		logging_send((char *) buffer, MODULE_INDEX_MAINTEST, LOG_DEBUG);
	}
}

static void initBlinkGPIO(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gpioInit = {
		.Pin = GPIO_PIN_0,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW,
	};

	HAL_GPIO_Init(GPIOA, &gpioInit);
}

static void initTestUart(void) {
	struct uart_ioConf setConfig = {
		.baudrate = SERIALPC_CONF_BAUDRATE,
		.parity = SERIALPC_CONF_PARITY,
		.wordlength = SERIALPC_CONF_WORDLENGTH,
		.stopbits = SERIALPC_CONF_STOPBITS,
	};

	uart_open(mcuDevice_serialPC, &setConfig);
}

static int initTestXbee(void) {
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
