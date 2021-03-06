/**
* @file logging.h
* @author Space Concordia Rocket Division
* @author Mathieu Breault
* @brief Simple logging system for debugging and tracking.
* 
* When using logging_send() the message should be a string literal directly in the function call,
* this way when LOGGING_DISABLED is defined, the string and function calls won't be compiled.
*  
* TODO :
* 	-clean compile time disabling mechanism for specific modules.
* 	-Way to tag logging message for dynamic pause per module
*/

#ifndef _LOGGING_H
#define _LOGGING_H

#include <stdbool.h>

#include "main.h"

#ifndef LOGGING_DISABLED

#define LOGGING_MODULE_MAX_FILTER 256
#define LOGGING_MODULE_COUNT 32

enum logging_level {
	LOG_DEBUG = 0x1,
	LOG_WARNING = 0x2,
	LOG_CRITICAL = 0x4,
};


/**
 * @brief Initialize the logging system with all events active.
 * 
 * The output is written with the given function pointer.
 * 
 * @param write the logging function called to write messages.
 * @return negative if logging is already active.
 */
int logging_open(int (*write)(uint8_t * data, size_t size));

/**
 * @brief Stop all messages and reset the initialization.
 * 
 * @return negative if logging is already inactive.
 */
int logging_close();

/**
 * @brief Temporarily pause and unpause all events.
 * 
 * @param status true to pause logging.
 */
int logging_pause(bool status);

/**
 * @brief Set the active level of logging. 
 * 
 * @param verbosity logical OR of the required level
 * @return negative if the logging system is inactive.
 */
int logging_setVerbosity(int verbosity);

/**
 * @brief Filter a module output.
 * 
 * @param moduleIndex the index to filter
 * @param filterOn true to stop the output
 * 
 * @return negative if moduleIndex is invalid (>= LOGGING_MODULE_COUNT)
 */
int logging_filterModule(uint8_t moduleIndex, bool filterOn);

/**
 * @brief Select the output function where the logging message are sent.
 */
int logging_setOutput(int (*write)(uint8_t * data, size_t size));

/**
 * @brief Log a new event to the stream, this will only output if the verbosity is set for the given level. 
 * 
 * @return negative if logging is closed or invalid module index, \
 * 			0 if the specified level or module index is inactive, \
 * 			1 on success.
 */
int logging_send(char * message, uint8_t moduleIndex, enum logging_level level);

#else

#define logging_open(__write__) ((void)0)
#define logging_close() ((void)0)
#define logging_pause(__status__) ((void)0)
#define logging_setVerbosity(__verbosity__) ((void)0)
#define logging_filterModule(__moduleIndex__, __filterOn__) ((void)0)
#define logging_setOutput(__write__) ((void)0)
#define logging_send(__message__, __level__) ((void)0)

#endif /* LOGGING_DISABLE */
#endif /* _LOGGING_H */
