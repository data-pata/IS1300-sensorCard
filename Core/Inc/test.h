/*
 * test.h
 *
 *  Created on: Nov 25, 2019
 *      Author: pjotr
 */

#ifndef INC_TEST_H_
#include <stdint.h>
#include <stdio.h>

int fputc(int ch, FILE *f);

void getHumidity(void);

void runTests(void);
void testDisplay(void);
void testBacklight(uint8_t[]);
void testPot(void);
void testIO(void);



#define INC_TEST_H_
#endif /* INC_TEST_H_ */


