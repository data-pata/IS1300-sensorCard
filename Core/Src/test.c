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
  testDisplay();
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
//  char err[] = "rtc error";
  char str[10];
  RTC_TimeTypeDef time = {0};
  RTC_DateTypeDef date = {0};
//  sTime.Hours = 0x23;
//  sTime.Minutes = 0x59;
//  sTime.Seconds = 0x55;
//  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
//  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
//  {
//    HAL_UART_Transmit(&huart5, (uint8_t *) err, strlen(err), 100);
//  }

  while(1) {
    HAL_Delay(1000);
    writeIns(0x01); //clr dspl

    HAL_RTC_GetTime(&hrtc,&time,RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD); //SUPERWEIRD
    sprintf(str,"%x:%x:%x",time.Hours,time.Minutes,time.Seconds);
    writeString(str);

    HAL_UART_Transmit(&huart5, (uint8_t *) str, strlen(str), 100);

  }
}
