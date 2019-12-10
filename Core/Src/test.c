/*
 * test.c
 *
 *  Created on: Nov 25, 2019
 *      Author: pjotr
 */

//#ifdef __GNUC__
///* With GCC, small printf (option LD Linker->Libraries->Small printf
//   set to 'Yes') calls __io_putchar() */
//#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__ */

#include <stdio.h>
#include <string.h>

#include "rtc.h"
#include "usart.h"
#include "i2c.h"

//#include "io.h"
#include "test.h"
#include "display.h"
#include "pot.h"

#define SADr (0x5f << 1)  // slave adress read
#define CTRL_REG1 0x20 // [7] PD: set to activate,
#define CTRL_REG2 0x21 //

#define INIT 0x85       //Power ON, BDU set, Continuous update, Data output rate = 1 Hz
#define CLBR_H0  0x30  //
#define CLBR_H1 0x31
static const uint8_t SAD = (0x5f << 1); // 7bit slave adress HAL lib adds r/w bit

HAL_StatusTypeDef ret;
int16_t H0_T0_out, H1_T0_out, H_T_out;
int16_t H0_rh, H1_rh;
uint8_t buf[4];
int32_t tmp;
uint8_t initFlag= 0;

void getHumidity(void) {

  if (1){

    //uint8_t adr = (SADw << 8) & CTRL_REG1;
    printf("sending to adr 0x%x \n\r ", SAD);
    buf[0] = CTRL_REG1; buf[1] = INIT;
    ret = HAL_I2C_Master_Transmit(&hi2c3, SAD, buf, 2, HAL_MAX_DELAY);
    if(ret != HAL_OK){
      printf("ERROR_TX\n\r");
    }

    printf("OK TX\n\r");


    // get humidity calibration values
    /* 1. Read H0_rH and H1_rH coefficients
     * */
    buf[0] = CLBR_H0;
    ret = HAL_I2C_Master_Transmit(&hi2c3, SAD, buf, 1, HAL_MAX_DELAY);
    if(ret != HAL_OK){
      printf("ERROR_TX clbr_h0\n\r");
    }
    ret = HAL_I2C_Master_Receive(&hi2c3, SAD, buf, 1, HAL_MAX_DELAY);
    if(ret != HAL_OK){
      printf("ERROR_RX clbr_h0\n\r");
    }
    printf("H0_RH recieved\n\r");
    H0_rh = buf[0]>>1;

    buf[0] = CLBR_H1;
    HAL_I2C_Master_Transmit(&hi2c3, SAD, buf, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c3, SAD, buf, 1, HAL_MAX_DELAY);
    H1_rh = buf[0]>>1;
    printf("H0_rH_2x/2 = %d%% rH\n\r", H0_rh);
    printf("H1_rH_2x/2 = %d%% rH\n\r", H1_rh);

    /*2. Read H0_T0_OUT */
    /*3. Read H1_T0_OUT */
    buf[0]=0x36;


  }
  while (1){
    HAL_Delay(100);

  }

}


void test(void) {
  getHumidity();
  //testDisplay();
  //testPot();
  //testIO();

  //setTime();
  //rtcTest();
}

void testIO(void){
  char i[2]={0};
  printf("plz set time & date of systeam \n\r");
//  char biff[8] = {0};
//  //printf("plz set time & date of systeam \n\r");
//  HAL_UART_Transmit(&huart5, (uint8_t *) buff, strlen(buff), 1000);
//  //printf ("Enter Hour: ");
    scanf ("%s", i);
//  HAL_UART_Receive(&huart5, (uint8_t *) &i, 1, 0xffff);

    printf( "you entered: %s \n\r", i);

  while(1);
}

int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart5, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


void testDisplay(void) {
	displayInit();
}

void testBacklight(uint8_t color[]) {

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

  while(1) {
    HAL_Delay(1000);
    writeIns(0x01); //clr dspl

    HAL_RTC_GetTime(&hrtc,&time,RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD); //SUPERWEIRD

    sprintf(str,"%02x:%02x:%02x",time.Hours,time.Minutes,time.Seconds);
    writeString(str);

    HAL_UART_Transmit(&huart5, (uint8_t *) str, strlen(str), 100);

  }
}
void setTime(void){
  RTC_TimeTypeDef Time = {0};
  uint8_t tt[6]={0};

  printf("Set system time in format. Enter hours: \n\r  ");
  HAL_UART_Receive(&huart5, (uint8_t *) &tt, 2, 0xffff);
  Time.Hours = parseChars(tt);

  printf("Enter minutes: \n\r");
  HAL_UART_Receive(&huart5, (uint8_t *) &tt, 2, 0xffff);
  Time.Minutes = parseChars(tt);

  printf("Enter seconds: \n\r");
  HAL_UART_Receive(&huart5, (uint8_t *) &tt, 2, 0xffff);
  Time.Seconds = parseChars(tt);

//  Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//  Time.StoreOperation = RTC_STOREOPERATION_RESET;
  HAL_RTC_SetTime(&hrtc, &Time, RTC_FORMAT_BIN);

//  int j = (i[0] - '0')*10;
//  j += i[1] - '0';
//  Time.Hours = j;
//  printf("%d\n\r",h1);
}

int parseChars(uint8_t *tt) {
  uint16_t t;
  t = (*tt - '0')*10;
  tt++;
  return t + (*tt -'0');
}
