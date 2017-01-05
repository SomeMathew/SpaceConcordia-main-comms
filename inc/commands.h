/**
 * @file commands.h
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Read commands from a given UART and adds the corresponding task to run.
 * 
 * The commands must be in the format: #<2 byte command><byte arg1><byte arg2>...<byte argn>.
 * The commands list and vector must be implemented in commands.c.
 * 
 * Opted for a static compile time implementation to simplify the module, dynamic modification of
 * the command list isn't useful as this is mainly for testing purpose. 
 */

#ifndef __COMMANDS_H
#define __COMMANDS_H

#include "scheduler.h"
#include "uart.h"

/**
 * @brief Start the command listener on the given UART.
 * 
 * @param USARTx an opened uart device.
 */
void commands_init(USART_TypeDef * USARTx);
//~ void commands_close();

#endif /* __COMMANDS_H */
