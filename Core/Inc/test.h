/*
 * test.h
 *
 *  Created on: Nov 25, 2019
 *      Author: pjotr
 */

#ifndef INC_TEST_H_
#include <stdint.h>

void rtcTest(void);
void test(void);
void testDisplay(void);
void testBacklight(uint8_t[]);
void testPot(void);
void testIO(void);
void setTime(void);
int parseChars(uint8_t *tt);
#define INC_TEST_H_
#endif /* INC_TEST_H_ */


