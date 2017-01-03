/**
 * @file uart.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Basic interupt based uart driver to send logging data.
 * 
 */

#ifndef __UART_H
#define __UART_H

#include <stdint.h> 
#include "stm32f1xx_hal.h"
#include "main.h"

enum uart_ioSetMask {
    UART_IOSET_BAUDRATE = 0x01,
    UART_IOSET_PARITY = 0x02,
    UART_IOSET_WORDLENGTH = 0x04,
    UART_IOSET_STOPBITS = 0x08,

    SERIAL_IOSET_INITALL = 0xFF,
};

struct uart_ioConf {
    uint32_t baudrate;
    uint32_t parity;
    uint32_t wordlength;
    uint32_t stopbits;
};

int uart_open(USART_TypeDef * USARTx, struct uart_ioConf * conf);
int uart_ioctl_set(USART_TypeDef * USARTx, int ioSetMask, struct uart_ioConf * conf);
int uart_close(USART_TypeDef * USARTx);
int uart_write(USART_TypeDef * USARTx, uint8_t * data, uint16_t size);

void USART2_IRQHandler(void);

#endif /* __UART_H */
