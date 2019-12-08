/*
 * test.c
 *
 *  Created on: Nov 25, 2019
 *      Author: samfl
 */
#include "test.h"
#include "display.h"
#include "pot.h"
#include "rtc.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

void test(void) {
//  Test_display_background("red");
//  testDisplay();
//  testPot();
  rtcTest();
}

void testDisplay(void) {
	displayInit();
//	Write_data(0x61);
}

void testBacklight(uint8_t color[]) {
//	Display_color(color);
}

void testPot(){
  potItTest();
  //potPollTest();
}

void rtcTest() {
  char str[10];
  RTC_TimeTypeDef Time = {0};

  while(1) {
    HAL_Delay(1000);
    HAL_RTC_GetTime(&hrtc,&Time,RTC_FORMAT_BCD);
    sprintf(str,"%x:%x:%x/r/n",Time.Hours,Time.Minutes,Time.Seconds);
    HAL_UART_Transmit(&huart5, (uint8_t *) str, strlen(str), 100);
    memset(str,0,sizeof(str));
  }
}
