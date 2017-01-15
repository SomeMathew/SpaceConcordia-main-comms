/**
 * @file pinmapping.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief pin configurations for gpios and peripherals
 * 
 */
 
#ifndef __PINMAP_H
#define __PINMAP_H

#define USART2_DEVICE USART2
#define USART2_TX_PORT GPIOA
#define USART2_RX_PORT GPIOA
#define USART2_TX_PIN GPIO_PIN_2
#define USART2_RX_PIN GPIO_PIN_3

#define I2C2_DEVICE I2C2
#define I2C2_SCL_PORT GPIOB
#define I2C2_SDA_PORT GPIOB
#define I2C2_SCL_PIN GPIO_PIN_10
#define I2C2_SDA_PIN GPIO_PIN_11

#endif /* __PINMAP_H */
