/**
* @file logging.c
* @author Space Concordia Rocket Division
* @author Mathieu Breault
* @brief Simple logging system for debugging and tracking.
* 
*/


#include <stdbool.h>
#include <string.h> 

#include "logging.h"
 
#define LOG_NONE (0x00)
#define LOG_ALL  (0xFF)

#define BUFFER_MAX_SIZE 200

static int (*logStream)(uint8_t * data, size_t size);

static int logLevel = LOG_NONE;
static bool logActive = false;
static bool logPause = false;

static char debugMsg[] = "DEBUG: ";
static uint8_t debugMsgLength = LENGTH_OF_ARRAY(debugMsg) - 1;
static char warningMsg[] = "WARNING: ";
static uint8_t warningMsgLength = LENGTH_OF_ARRAY(warningMsg) - 1;
static char criticalMsg[] = "CRITICAL: ";
static uint8_t criticalMsgLength = LENGTH_OF_ARRAY(criticalMsg) - 1;

static char msgBuffer[BUFFER_MAX_SIZE];
static size_t msgBufferCount;

static inline void clearBuffer() {
	msgBufferCount = 0;
}

static inline void addToBuffer(char * string, size_t count) {
	memcpy(msgBuffer + msgBufferCount, string, count);
	msgBufferCount += count;
} 

/*
 * Returns true if the given eventLevel is currently active
 */
static inline bool logEventIsActive(enum logging_level eventLevel) {
	if (logPause) {
		return false;
	} else {
		return (logLevel & eventLevel);
	}
}

static inline void writeLog(char * levelMsg, uint8_t levelMsgLength, char * msg, size_t msgLength) {
	clearBuffer();
	addToBuffer(levelMsg, levelMsgLength);
	addToBuffer(msg, msgLength);
	addToBuffer("\n", 1);
	(*logStream)((uint8_t *) msgBuffer, msgBufferCount);
}

/*
 * Write the DEBUG log message to the stream.
 * 
 * returns 1 on success, 0 if the level is inactive.
 */
static inline int writeLogDebug(char * message) {
	if (logEventIsActive(LOG_DEBUG)) {
		writeLog(debugMsg, debugMsgLength, message, strlen(message));
		return 1;
	} else {
		return 0;
	}
}

/*
 * Write the WARNING log message to the stream.
 * 
 * returns 1 on success, 0 if the level is inactive.
 */
static inline int writeLogWarning(char * message) {
	if (logEventIsActive(LOG_WARNING)) {
		writeLog(warningMsg, warningMsgLength, message, strlen(message));
		return 1;
	} else {
		return 0;
	}
}

/*
 * Write the CRITICAL log message to the stream.
 * 
 * returns 1 on success, 0 if the level is inactive.
 */
static inline int writeLogCritical(char * message) {
	if (logEventIsActive(LOG_CRITICAL)) {
		writeLog(criticalMsg, criticalMsgLength, message, strlen(message));
		return 1;
	} else {
		return 0;
	}
}

/*
 * Initialize the logging system with all events active.
 * 
 * Return negative if logging is already active.
 */
int logging_open(int (*write)(uint8_t * data, size_t size)) {
	if (logActive) {
		return -1;
	}
	logStream = write;
	logLevel = LOG_ALL;
	logActive = true;
	return 0;
}

/*
 * Stop all events and reset the initialization.
 * 
 * Return negative if logging is already inactive.
 */
int logging_close() {
	if (!logActive) {
		return -1;
	}
	logLevel = LOG_NONE;
	logActive = false;
	return 0;
}

/*
 * Temporarily pause and unpause all events.
 */
int logging_pause(bool status) {
	logPause = status;
	return 0;
}

/*
 * Set the active level of logging, verbosity is the logical OR of the required level. 
 * 
 * return negative if the logging system is inactive.
 */
int logging_setVerbosity(int verbosity) {
	if (!logActive) {
		return -1;
	}
	logLevel = verbosity;
	return 0;
}

int logging_setOutput(int (*write)(uint8_t * data, size_t size)) {
	logStream = write;
	return 0;
}

/*
 * Log and event of type level, with the given message string.
 * 
 * The Event will only be sent if the log level is currently active.
 * 
 * Returns negative if logging is closed, 0 if the specified level is inactive, 1 on success.
 */
int logging_send(char * message, enum logging_level level) {
	if (!logActive) {
		return -1;
	} else if (logPause) {
		return 0;
	}
	switch(level) {
	case LOG_CRITICAL:
				return writeLogCritical(message);
				break;
	case LOG_WARNING:
				return writeLogWarning(message);
				break;
	case LOG_DEBUG:
				return writeLogDebug(message);
				break;
	default:
				logging_send("Invalid LOG level sent", LOG_WARNING); 
				return 0;
	}
}
