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
#include "logging.h"

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
	EnableGpioClock(PITOT_CS_PORT);
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
	if (huart->Instance == USART2_DEVICE) {
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
		HAL_NVIC_SetPriority(USART2_IRQn, 0, 2);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
		
	} else if (huart->Instance == USART1_DEVICE) {
		__HAL_RCC_USART1_CLK_ENABLE();
		EnableGpioClock(USART1_RX_PORT);
		EnableGpioClock(USART1_TX_PORT);
		
		GPIO_InitTypeDef gpioInit = {0};
		
		gpioInit.Pin = USART1_TX_PIN;
		gpioInit.Mode = GPIO_MODE_AF_PP;
		gpioInit.Pull = GPIO_PULLUP;
		gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
		
		HAL_GPIO_Init(USART1_TX_PORT, &gpioInit);
		
		gpioInit.Pin = USART1_RX_PIN;
		gpioInit.Mode = GPIO_MODE_AF_INPUT;
		gpioInit.Pull = GPIO_NOPULL;
		
		HAL_GPIO_Init(USART1_RX_PORT, &gpioInit);
		
		gpioInit.Pin = USART1_RTS_PIN;
		gpioInit.Mode = GPIO_MODE_AF_INPUT;
		gpioInit.Pull = GPIO_NOPULL;
		gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
		
		HAL_GPIO_Init(USART1_RTS_PORT, &gpioInit);
		
		// Enable the NVIC for the usart1
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
}

/**
 * @brief initialization of the low-level gpio and DMA for the I2C peripherals.
 * 
 * This callback function configures the following hardware resources:
 * 		-Peripheral Clock
 * 		-Peripheral GPIO (see pinmapping.h)
 * 		-DMA
 * 		-NVIC for DMA and IT transfer
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C2_DEVICE) {
		__HAL_RCC_I2C2_CLK_ENABLE();
		EnableGpioClock(I2C2_SCL_PORT);
		EnableGpioClock(I2C2_SDA_PORT);
		
		// Initialize the SCL pin
		GPIO_InitTypeDef gpioInit = {0};
		
		gpioInit.Pin = I2C2_SCL_PIN;
		gpioInit.Mode = GPIO_MODE_AF_OD;
		gpioInit.Pull = GPIO_PULLUP;
		gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
		
		HAL_GPIO_Init(I2C2_SCL_PORT, &gpioInit);
		
		// Initialize the SDA pin, settings the same as SCL (OD, Pullup)
		gpioInit.Pin = I2C2_SDA_PIN;
		
		HAL_GPIO_Init(I2C2_SDA_PORT, &gpioInit);
		
		// No DMA for I2C2 Set for interrupt based transfer
		// TODO: Add possibility for a DMA define in pinmapping to activate it.
		HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 1);
		HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 2);
		HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
		HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
	} else if (hi2c->Instance == I2C1_DEVICE) {
		__HAL_RCC_I2C1_CLK_ENABLE();
		EnableGpioClock(I2C1_SCL_PORT);
		EnableGpioClock(I2C1_SDA_PORT);
		
		// Initialize the SCL pin
		GPIO_InitTypeDef gpioInit = {0};
		
		gpioInit.Pin = I2C1_SCL_PIN;
		gpioInit.Mode = GPIO_MODE_AF_OD;
		gpioInit.Pull = GPIO_PULLUP;
		gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
		
		HAL_GPIO_Init(I2C1_SCL_PORT, &gpioInit);
		
		// Initialize the SDA pin, settings the same as SCL (OD, Pullup)
		gpioInit.Pin = I2C1_SDA_PIN;
		
		HAL_GPIO_Init(I2C1_SDA_PORT, &gpioInit);
		
		// No DMA for I2C2 Set for interrupt based transfer
		// TODO: Add possibility for a DMA define in pinmapping to activate it.
		HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 1);
		HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 2);
		HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
		HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
	}
}

/**
 * @brief initialization of the low-level gpio and DMA for the SPI peripherals.
 * 
 * This callback function configures the following hardware resources:
 * 		-Peripheral Clock
 * 		-Peripheral GPIO (see pinmapping.h)
 * 		-DMA
 * 		-NVIC for DMA and IT transfer
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == SPI2_DEVICE) {
		__HAL_RCC_SPI2_CLK_ENABLE();
		EnableGpioClock(SPI2_MOSI_PORT);
		EnableGpioClock(SPI2_MISO_PORT);
		EnableGpioClock(SPI2_SCK_PORT);
		
		
		GPIO_InitTypeDef gpioInit = {0};
		
		// Initialize the MISO pin
		gpioInit.Pin       = SPI2_MISO_PIN;
		gpioInit.Mode      = GPIO_MODE_AF_INPUT;
		gpioInit.Pull      = GPIO_NOPULL;
		gpioInit.Speed     = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(SPI2_MISO_PORT, &gpioInit);
		
		//~ /* SPI MOSI GPIO pin configuration  */
		//~ gpioInit.Pin = SPI2_MOSI_PIN;
		//~ gpioInit.Mode = GPIO_MODE_AF_PP;
		//~ gpioInit.Pull = GPIO_NOPULL;
		//~ HAL_GPIO_Init(SPI2_MOSI_PORT, &gpioInit);
		
		/* SPI SCK GPIO pin configuration  */
		gpioInit.Pin = SPI2_SCK_PIN;
		gpioInit.Mode = GPIO_MODE_AF_PP;
		gpioInit.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(SPI2_SCK_PORT, &gpioInit);
		logging_send("test spi", MODULE_INDEX_SPI, LOG_DEBUG);
		
		// No DMA for I2C2 Set for interrupt based transfer
		// TODO: Add possibility for a DMA define in pinmapping to activate it.
		HAL_NVIC_SetPriority(SPI2_IRQn, 1, 1);
		HAL_NVIC_EnableIRQ(SPI2_IRQn);
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
