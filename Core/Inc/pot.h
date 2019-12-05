/*
 * pot.h
 *
 *  Created on: Dec 4, 2019
 *      Author: Pjotr
 */

#ifndef INC_POT_H_
#define INC_POT_H_


#include "stm32l4xx_hal.h"
#include "stdint.h"
//#include "stm32l4xx_hal_gpio.h"

void potPollTest(void);
void potItTest(void);
void changeLight(uint32_t adcResult);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);



#endif /* INC_POT_H_ */
