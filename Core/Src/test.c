 /**
******************************************************************************
@brief Test library for sensorCard functionality and peripherals.
@file test.c
@author Patrik Sjöfors
@version
@date
@brief Uses sensorCard library functions.
******************************************************************************
*/
#include <test.h>
#include <math.h>
#include <string.h>

#include "cmsis_os2.h"
#include "rtc.h"
#include "usart.h"
#include "tim.h"
#include "i2c.h"

#include "display.h"
#include "utilio.h"
#include "humtemp.h"

#include "time.h"
#include "gpio.h"
#include "main.h"
#include "backlight.h"

#include "adc.h"



/**
 * @brief Test PWM backlight dimming.
 * @note
 */
void dimBackLightTest(void){
  uint16_t pwm_value=0;
  uint16_t step=0;

  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);

  while(1) {
    HAL_Delay(50);
    if(pwm_value == 0) step = 100;
    if(pwm_value == 4000) step = -100;
    pwm_value += step;
    dimBackLight(pwm_value);
  }
}

/**
 * @brief tests the adc in from potentiometer.
 */
void potTest() {
  backLightInit();
}
/**
 * @brief Tests print to different display lines.
 */
void displayTest() {
  printdL(0, "hello 0");
  printdL(1, "hello 1");
  printdL(2, "hello 2");
  printdL(3, "hello 3");
}

/**
 * @brief tests setting and printing the real time clock.
 */
void rtcTest() {
  setTime();
  while(1){
    timeToDisplay();
    HAL_Delay(1000);
  }
}
/**
 * @brief Tests io library functions.
 */
void ioTest() {
  printu("Hello from testprogram");
}
