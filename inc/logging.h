/**
* @file logging.h
* @author Space Concordia Rocket Division
* @author Mathieu Breault
* @brief Simple logging system for debugging and tracking.
* 
*/

#ifndef _LOGGING_H
#define _LOGGING_H

#include "main.h"

enum logging_level {
	LOG_DEBUG = 0x1,
	LOG_WARNING = 0x2,
	LOG_CRITICAL = 0x4,
};

/*
 * Initialize the logging system with all events active.
 * 
 * The output is written with the given function pointer.
 * 
 * Return negative if logging is already active.
 */
int logging_open(int (*write)(uint8_t * data, size_t size));

/*
 * Stop all messages and reset the initialization.
 * 
 * Return negative if logging is already inactive.
 */
int logging_close();

/*
 * Temporarily pause and unpause all events.
 */
int logging_pause(bool status);

/*
 * Set the active level of logging, verbosity is the logical OR of the required level. 
 * 
 * return negative if the logging system is inactive.
 */
int logging_setVerbosity(int verbosity);

/*
 * Select the output function where the logging message are sent.
 */
int logging_setOutput(int (*write)(uint8_t * data, size_t size));

/*
 * Log a new event to the stream, this will only output if the verbosity is set for the given level. 
 * 
 * Returns negative if logging is closed, 0 if the specified level is inactive, 1 on success.
 */
int logging_send(char * message, enum logging_level level);

#endif /* _LOGGING_H */
