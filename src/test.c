/**
 * @brief Test function for development
 * 
 * Those are the test that were in main.c, now moved to clean up for deployment.
 */
 
 
 
static uint8_t testData[] = "test\n";
static size_t testDataSize = LENGTH_OF_ARRAY(testData) - 1;
static bool changeLogTestActive = true;

void sendTestUart(uint32_t event, void * arg) {
	uart_write(mcuDevice_serialXBee, testData, testDataSize);
}

void sendTestLog(uint32_t event, void * arg) {
	logging_send("testLog debug", MODULE_INDEX_MAINTEST, LOG_DEBUG);
	logging_send("testLog warning", MODULE_INDEX_MAINTEST, LOG_WARNING);
	//~ logging_send("testLog critical +1", MODULE_INDEX_MAINTEST + 3, LOG_CRITICAL);
	logging_send("testLog critical", MODULE_INDEX_MAINTEST, LOG_CRITICAL);
	logging_send("testLog critical 24", 24, LOG_CRITICAL);
}

void sendTestXbee(uint32_t event, void * arg) {
	if (xbee_write(testData, testDataSize) == DRIVER_STATUS_ERROR) {
		logging_send("Xbee ERROR", MODULE_INDEX_XBEE, LOG_WARNING);
	}
	logging_send("Xbee Test", MODULE_INDEX_XBEE, LOG_DEBUG);
}

void changeTestLog(uint32_t event, void * arg) {
	if (*((bool *) arg)) {
		logging_setVerbosity(LOG_DEBUG | LOG_CRITICAL);
		logging_filterModule(MODULE_INDEX_MAINTEST + 1, false);
	} else {
		logging_setVerbosity(LOG_DEBUG | LOG_WARNING | LOG_CRITICAL);
		logging_filterModule(MODULE_INDEX_MAINTEST + 1, true);
	}
	*((bool *) arg) = !(*((bool *) arg));
}

void uartRead(uint32_t event, void * arg) {
	uint8_t buffer[50];
	size_t readSize = uart_read(mcuDevice_serialPC, buffer, 49);
	if (readSize > 0) {
		buffer[readSize] = '\0';
		logging_send((char *) buffer, MODULE_INDEX_MAINTEST, LOG_DEBUG);
	}
}
