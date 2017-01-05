/**
 * @file commands.c
 * @author Space Concordia Rocket Division
 * @author Mathieu Breault
 * @brief Read commands from a given UART and adds the corresponding task to run.
 * 
 * This module is implemented using a table, which must be modified at compile time to add
 * or remove commands.
 * 
 * For command usage see their associated functions.
 * To add a command see commandTable[] variable.
 */

#include <string.h>
#include <stdlib.h>

#include "commands.h"
#include "logging.h"

#define COMMAND_SIZE 2
#define MAX_ARG_SIZE 16
#define BUFFER_LENGTH COMMAND_SIZE + MAX_ARG_SIZE + 1

struct commandEntry {
	char command[COMMAND_SIZE + 1];
	void (*vector)(uint8_t * args, size_t size);
	uint8_t argSize;
};

/* Functions for the commandTable */
static void logFilter(uint8_t * args, size_t size);
static void logVerbosity(uint8_t * args, size_t size);

static void nextCommands(void * arg);
static struct commandEntry * findCommandEntry(uint8_t * cmd);


static struct task * nextCommandTask; 
static USART_TypeDef * inputUSART;
static uint8_t buffer[BUFFER_LENGTH];

/**
 * Command table that is searched by the module.
 * 
 * Adding a command:
 * 		Add the function to execute and define it in the above section.
 * 			The function must handle the parsing of its arguments.
 * 		Add an entry according to the struct commandEntry.
 * 			The <CMD> string must have a maximum size of COMMAND_SIZE.
 * 			The <arg size> must have a max of MAX_ARG_SIZE.
 * 			{"<CMD>", <function ptr>, <arg size in bytes>}
 */
static struct commandEntry commandTable[] = {
	{"LF", logFilter, 5}, // logging filter module
	{"LV", logVerbosity, 1}, // logging change verbosity
};

void commands_init(USART_TypeDef * USARTx) {
	nextCommandTask = createTask(nextCommands, NULL, 20, true, 2);
	inputUSART = USARTx;
}
//~ void commands_close();

static void nextCommands(void * arg) {
	uint8_t * cursor = buffer;
	// find if there's a command start (#)
	do {
		if (uart_read(inputUSART, cursor, 1) == 0) {
			return;
		}
	} while (*cursor != '#');
	
	// find command and its entry
	if (uart_read(inputUSART, cursor, COMMAND_SIZE) != COMMAND_SIZE) {
		return;
	}
	
	struct commandEntry * entry = findCommandEntry(cursor);
	
	if (entry == NULL) {
		return;
	}
	
	// Error if argSize of entry is too large
	if (entry->argSize > MAX_ARG_SIZE) {
		entry->argSize = MAX_ARG_SIZE;
	}
	
	size_t argReadSize = uart_read(inputUSART, cursor, entry->argSize);
	buffer[argReadSize] = '\0'; // make it a null terminated string
	(entry->vector)(cursor, argReadSize);
}

static struct commandEntry * findCommandEntry(uint8_t * cmd) {
	for (int i = 0; i < LENGTH_OF_ARRAY(commandTable); i++) {
		if (memcmp(commandTable[i].command, (char *) cmd, COMMAND_SIZE) == 0) {
			return (commandTable + i);
		}
	}
	return NULL;
}

/* Commands functions */

/**
 * @brief set filter of the logging system for a module
 * 
 * Usage: #LF<module ID>,<filter bool>
 * 
 * @see logging_filterModule
 */
static void logFilter(uint8_t * args, size_t size) {
	if (size < 3) {
		return;
	}
	char * delim = ",";
	char * id = strtok((char *) args, delim);
	char * filterOn = strtok(NULL, delim);
	
	if (id == NULL || filterOn == NULL) {
		return;
	}
	logging_filterModule((uint8_t) atoi(id), *filterOn - '0');
}

/**
 * @brief set Verbosity of the logging system.
 * 
 * Usage: #LV<flag>
 * 			flag must be of the same form as in logging.h
 * 			Logical OR of the verbosity flags.
 * 		  eg.: #LV7 for LOG_DEBUG | LOG_WARNING | LOG_CRITICAL
 * @see enum logging_level
 */
static void logVerbosity(uint8_t * args, size_t size) {
	logging_setVerbosity(*args);
}
