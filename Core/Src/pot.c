/*
 * pot.c
 *
 *  Created on: Dec 4, 2019
 *      Author: Pjotr
 * The ADC peripheral result in STM32 is 12 bit.
 * Thus, the maximum value (when the input voltage equals 3.3 V)
 * is 4095 (0b111111111111).
 *
 *  */
#include "pot.h"
#include "pwm.h"
#include "tim.h"
#include "gpio.h"
#include "adc.h"
#include "stdio.h"

#define ADC_0V_VALUE  0
#define ADC_1V_VALUE  1241
#define ADC_2V_VALUE  2482
#define ADC_3V_VALUE  3723

uint32_t adcResult = 0;

void potItTest(void) {

  HAL_ADC_Start_IT(&hadc1);

//  while(1) {
//    asm("NOP");
//  }
}

void changeLight(uint32_t adcResult) {

  if ((adcResult >= ADC_0V_VALUE) && (adcResult < ADC_1V_VALUE)) {
    HAL_GPIO_WritePin(Display_green_GPIO_Port, Display_green_Pin, 0);
    HAL_GPIO_WritePin(Display_white_GPIO_Port, Display_white_Pin, 0);
    //HAL_GPIO_WritePin(Display_red_GPIO_Port, Display_red_Pin, 0);
    HAL_GPIO_WritePin(Display_green_GPIO_Port, Display_green_Pin, 1);

  }

 if ((adcResult >= ADC_1V_VALUE) && (adcResult < ADC_2V_VALUE)) {
   HAL_GPIO_WritePin(Display_green_GPIO_Port, Display_green_Pin, 0);
   HAL_GPIO_WritePin(Display_white_GPIO_Port, Display_white_Pin, 0);
  // HAL_GPIO_WritePin(Display_red_GPIO_Port, Display_red_Pin, 0);
   //HAL_GPIO_WritePin(Display_red_GPIO_Port, Display_red_Pin, 1);

 }

 if ((adcResult >= ADC_2V_VALUE) && (adcResult < ADC_3V_VALUE)) {
   HAL_GPIO_WritePin(Display_green_GPIO_Port, Display_green_Pin, 0);
   HAL_GPIO_WritePin(Display_white_GPIO_Port, Display_white_Pin, 0);
   //HAL_GPIO_WritePin(Display_red_GPIO_Port, Display_red_Pin, 0);
   HAL_GPIO_WritePin(Display_white_GPIO_Port, Display_white_Pin, 1);
 }

 if (adcResult >= ADC_3V_VALUE) {
   HAL_GPIO_WritePin(Display_green_GPIO_Port, Display_green_Pin, 0);
   HAL_GPIO_WritePin(Display_white_GPIO_Port, Display_white_Pin, 0);
   //HAL_GPIO_WritePin(Display_red_GPIO_Port, Display_red_Pin, 0);
 }

}

void potPollTest(void) {
  while (1) {

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    adcResult = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    changeLight(adcResult);
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if(hadc->Instance == ADC1){
    dimBackLight(HAL_ADC_GetValue(&hadc1));
  }
}


