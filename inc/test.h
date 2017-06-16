/**
 * @brief Test function for development
 * 
 * Those are the test that were in main.c, now moved to clean up for deployment.
 */

#ifndef __TEST_H
#define __TEST_H

void sendTestUart(uint32_t event, void * arg);
void sendTestLog(uint32_t event, void * arg);
void sendTestXbee(uint32_t event, void * arg);
void changeTestLog(uint32_t event, void * arg);
void uartRead(uint32_t event, void * arg);

#endif /* __TEST_H */
