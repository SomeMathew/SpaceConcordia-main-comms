/**
 * @file pinmapping.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief pin configurations for gpios and peripherals
 * 
 */
 
#ifndef __PINMAP_H
#define __PINMAP_H

#define USART3_DEVICE USART3
#define USART3_TX_PORT GPIOC
#define USART3_RX_PORT GPIOC
#define USART3_TX_PIN GPIO_PIN_10
#define USART3_RX_PIN GPIO_PIN_11

#define USART2_DEVICE USART2
#define USART2_TX_PORT GPIOA
#define USART2_RX_PORT GPIOA
#define USART2_TX_PIN GPIO_PIN_2
#define USART2_RX_PIN GPIO_PIN_3

#define USART1_DEVICE USART1
#define USART1_TX_PORT GPIOA
#define USART1_RX_PORT GPIOA
#define USART1_RTS_PORT GPIOA
#define USART1_TX_PIN GPIO_PIN_9
#define USART1_RX_PIN GPIO_PIN_10
#define USART1_RTS_PIN GPIO_PIN_12

#define I2C2_DEVICE I2C2
#define I2C2_SCL_PORT GPIOB
#define I2C2_SDA_PORT GPIOB
#define I2C2_SCL_PIN GPIO_PIN_10
#define I2C2_SDA_PIN GPIO_PIN_11

#endif /* __PINMAP_H */
