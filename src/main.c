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

static void clockConfig(void);
static void initBlinkGPIO(void);
static void blink(void *);

int main(void) {
	// Call the CSMSIS system clock routine to store the clock frequency
    // in the SystemCoreClock global RAM location.
    SystemCoreClockUpdate();
	// Initialize the HAL Device Library
	HAL_Init();
	clockConfig();
	
	initBlinkGPIO();
	
	struct task * blinkTask = createTask(blink, NULL, 1000, true, 0);
	while(1) {
		//~ HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		runScheduler();
		//~ HAL_Delay(5000);
	}
}

static void blink(void * arg) {
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
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
