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

static void clockConfig(void);
static void initBlinkGPIO(void);
static void initTestUart(void);
static void blink(void *);
static void sendTestUart(void * arg);
static void sendTestLog(void * arg);
static void changeTestLog(void * arg);
static void uartRead(void * arg);
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
	
	struct task * blinkTask = createTask(blink, NULL, 1000, true, 0);
	//~ struct task * uartTestTask = createTask(sendTestUart, NULL, 2000, true, 0);
	struct task * logTestTask = createTask(sendTestLog, NULL, 500, true, 0);
	struct task * logChangeTask = createTask(changeTestLog, &changeLogTestActive, 3000, true, 1);
	struct task * uartReadTask = createTask(uartRead, NULL, 200, true, 0);
	while(1) {
		//~ HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		runScheduler();
		//~ HAL_Delay(5000);
	}
}

static void blink(void * arg) {
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

static void sendTestUart(void * arg) {
	uart_write(SERIALPC_DEVICE, testData, testDataSize);
}

static void sendTestLog(void * arg) {
	logging_send("testLog debug", MODULE_INDEX_MAINTEST, LOG_DEBUG);
	logging_send("testLog warning", MODULE_INDEX_MAINTEST, LOG_WARNING);
	logging_send("testLog critical module disable", MODULE_INDEX_MAINTEST + 1, LOG_CRITICAL);
	logging_send("testLog critical", MODULE_INDEX_MAINTEST, LOG_CRITICAL);
}

static void changeTestLog(void * arg) {
	if (*((bool *) arg)) {
		logging_setVerbosity(LOG_DEBUG | LOG_CRITICAL);
		logging_filterModule(MODULE_INDEX_MAINTEST + 1, false);
	} else {
		logging_setVerbosity(LOG_DEBUG | LOG_WARNING | LOG_CRITICAL);
		logging_filterModule(MODULE_INDEX_MAINTEST + 1, true);
	}
	*((bool *) arg) = !(*((bool *) arg));
}

static void uartRead(void * arg) {
	uint8_t buffer[50];
	size_t readSize = uart_read(SERIALPC_DEVICE, buffer, 49);
	if (readSize > 0) {
		buffer[readSize] = '\0';
		logging_send((char *) buffer, MODULE_INDEX_MAINTEST, LOG_DEBUG);
	}
}

static void initBlinkGPIO(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gpioInit = {
		.Pin = GPIO_PIN_5,
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
	
	uart_open(SERIALPC_DEVICE, &setConfig); 
}

static int loggingStream(uint8_t * data, size_t size) {
	return uart_write(SERIALPC_DEVICE, data, size);
}

/**
 * @brief System clock configuration
 * 
 * Currently configured as:
 *            System Clock source            = PLL (HSE Bypass)
 *            SYSCLK(Hz)                     = 74000000
 *            HCLK(Hz)                       = 74000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            PLLMUL                         = 9
 *            Flash Latency(WS)              = 2
 */
static void clockConfig(void) {
	RCC_OscInitTypeDef oscInit = {
		.OscillatorType = RCC_OSCILLATORTYPE_HSE,
		.HSEState = RCC_HSE_BYPASS,
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
