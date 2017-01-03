/**
 * @file stm32f1xx_hal_msp.c
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief MCU specific package callback module
 * 
 * This module includes the HAL driver callback methods used to initialize and deinitilize
 * the low-level peripherals.
 */
 
#include "main.h"
#include "pinmapping.h"

/**
 * @defgroup GPIOx_Clock_Status GPIO port clock status
 * Each macro evaluates to 1 if enabled and 0 otherwise.
 * @{
 */
#define GPIOA_CLOCK_ENABLED (__HAL_RCC_GPIOA_IS_CLK_ENABLED())
#define GPIOB_CLOCK_ENABLED (__HAL_RCC_GPIOB_IS_CLK_ENABLED())
#define GPIOC_CLOCK_ENABLED (__HAL_RCC_GPIOC_IS_CLK_ENABLED())
#define GPIOD_CLOCK_ENABLED (__HAL_RCC_GPIOD_IS_CLK_ENABLED())
#define GPIOE_CLOCK_ENABLED (__HAL_RCC_GPIOE_IS_CLK_ENABLED())

/** @} */ // End of group GPIOx_Clock_Status

static void EnableGpioClock(GPIO_TypeDef * port);

/**
 * @brief Global MSP Initialization
 */
void HAL_MspInit(void) {

}

/**
* @brief Global MSP Initialization.
*/
void HAL_MspDeInit(void)
{

}

/**
 * @brief initialization of the low-level gpio for the UART peripherals.
 */
void HAL_UART_MspInit(UART_HandleTypeDef * huart) {
	if (huart->Instance == USART2) {
		__HAL_RCC_USART2_CLK_ENABLE();
		EnableGpioClock(USART2_RX_PORT);
		EnableGpioClock(USART2_TX_PORT);
		
		GPIO_InitTypeDef gpioInit = {0};
		
		gpioInit.Pin = USART2_TX_PIN;
		gpioInit.Mode = GPIO_MODE_AF_PP;
		gpioInit.Pull = GPIO_PULLUP;
		gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
		
		HAL_GPIO_Init(USART2_TX_PORT, &gpioInit);
		
		gpioInit.Pin = USART2_RX_PIN;
		gpioInit.Mode = GPIO_MODE_AF_INPUT;
		gpioInit.Pull = GPIO_NOPULL;
		
		HAL_GPIO_Init(USART2_RX_PORT, &gpioInit);
		
		// Enable the NVIC for the usart2
		HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}
}


/**
* @brief Enable the given GPIO port clock signal only if it isn't already active.
*/
static void EnableGpioClock(GPIO_TypeDef * port) {

    if ((port == GPIOA) && (!GPIOA_CLOCK_ENABLED)) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if ((port == GPIOB) && (!GPIOB_CLOCK_ENABLED)) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if ((port == GPIOC) && (!GPIOC_CLOCK_ENABLED)) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    } else if ((port == GPIOD) && (!GPIOD_CLOCK_ENABLED)) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    } else if ((port == GPIOE) && (!GPIOE_CLOCK_ENABLED)) {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
}
