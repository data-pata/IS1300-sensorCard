/*
 * test.h
 *
 *  Created on: Nov 25, 2019
 *      Author: pjotr
 */

#ifndef INC_TEST_H_
#include <stdint.h>
#include <stdio.h>
#include "HTS221_Driver.h"


void humTempCalib(void);
void getHumidity(uint16_t* value);
void tempCal(void);
void Get_Humidity(uint16_t* value);



#define INC_TEST_H_
#endif /* INC_TEST_H_ */


