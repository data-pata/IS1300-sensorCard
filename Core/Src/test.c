/*
 * test.c
 *
 *  Created on: Nov 25, 2019
 *      Author: pjotr
 */

#include <string.h>

#include "cmsis_os2.h"
#include "rtc.h"
#include "usart.h"
#include "i2c.h"

#include "io.h"
#include "test.h"
#include "display.h"
#include "pot.h"



#define HTS221_SAD (0x5f << 1)  // slave adress [SHIFTED]
#define CTRL_REG1 0x20 // [7] PD: set to activate,
#define DATA_REG 0x28 // reg to hum and temp data (2bytes each)

#define CLBR_REG  0x30  //

HAL_StatusTypeDef ret;
int16_t H0_T0_out, H1_T0_out;
int16_t H0_rh, H1_rh;


int16_t T0_degC, T1_degC;

int16_t T0_out;
int16_t T1_out;


int32_t tmp;
uint8_t initFlag= 0;

void humTempCalib(void) {
  uint8_t wbuf[25];
  uint8_t rbuf[15];

  int16_t T0_degC_x8_u16, T1_degC_x8_u16;
  uint8_t T1_T0_msb;

// READ HUMIDITY CALIBRATION VALUES

    HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, CLBR_REG, 1, rbuf, 15, 100);

    H0_rh = rbuf[0]>>1;      // calibration values must be divided by 2
    H1_rh = rbuf[1]>>1;
    H0_T0_out = (uint16_t)rbuf[0x6] | (((uint16_t)rbuf[0x7]) << 8); // concatenate to correct int16_t values
    H1_T0_out = (uint16_t)rbuf[0xA] | (((uint16_t)rbuf[0xB]) << 8);

//    sprintf((char *) wbuf, "rbuf[0x6] = %x rbuf[0x7] = %x \n\r", rbuf[0x6], rbuf[0x7]);
//    printu(wbuf);
//    sprintf((char *) wbuf, "rbuf[0xA] = %x rbuf[0xB] = %x \n\r", rbuf[0xA], rbuf[0xB]);
//    printu(wbuf);

//    H0_T0_out = ~H0_T0_out + 1; // two's complement
//    H1_T0_out = ~H1_T0_out + 1; // but since datatype is signed it will display corr values?
//   0  1  2  3  4  5  6  7  8  9  A  B
//0x30 31 32 33 34 35 36 37 38 39 3A 3B

//    sprintf((char *) wbuf, "H0_rH = %d%% rH \n\r", H0_rh);
//    printu(wbuf);
//    sprintf((char *) wbuf, "H1_rH = %d%% rH \n\r", H1_rh);
//    printu(wbuf);
//    sprintf((char *) wbuf, "H0_T0_out = %d \n\r", H0_T0_out);
//    printu(wbuf);
//    sprintf((char *) wbuf, "H1_T0_out = %d \n\r", H1_T0_out);
//    printu(wbuf);

// READ TEMP CALIBRATION VALUES

    T1_T0_msb = rbuf[0x5];
    T0_degC_x8_u16 = (((uint16_t)(T1_T0_msb & 0x03)) << 8) | ((uint16_t)rbuf[0x2]);
    T1_degC_x8_u16 = (((uint16_t)(T1_T0_msb & 0x0C)) << 6) | ((uint16_t)rbuf[0x3]);

    T0_degC = T0_degC_x8_u16>>3;
    T1_degC = T1_degC_x8_u16>>3;

//    T0_degC = rbuf[0x2] >> 3;
//    T1_degC = rbuf[0x3] >> 3;
//    T1_T0_msb = rbuf[0x5];
//    T0_degC = ((T1_T0_msb & 0x3) << 8) | T0_degC; // 3
//    T1_degC = ((T1_T0_msb & 0xC) << 6) | T1_degC;

    T0_out = (uint16_t)rbuf[0xC] | (((uint16_t)rbuf[0xD]) << 8);
    T1_out = (uint16_t)rbuf[0xE] | (((uint16_t)rbuf[0xF]) <<8);

//    sprintf((char *) wbuf, "T0_degC = %d C \n\r", T0_degC);
//    printu(wbuf);
//    sprintf((char *) wbuf, "T1_degC = %d C \n\r", T1_degC);
//    printu(wbuf);
//    sprintf((char *) wbuf, "T0_out = %d \n\r",T0_out);
//    printu(wbuf);
//    sprintf((char *) wbuf, "T1_out = %d \n\r\n\r", T1_out);
//    printu(wbuf);

}

void humTemp(uint16_t* value) {
  int16_t H_T_out, T_out;
  uint8_t rbuf[4];
  uint8_t wbuf[25];
  int32_t tmp, TC, HrH;

  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, DATA_REG, 1, rbuf, 4, 100);

  H_T_out = (uint16_t)rbuf[0] | (((uint16_t)rbuf[1])<<8);
  T_out = (uint16_t)rbuf[2] | (((uint16_t)rbuf[3])<<8);

  tmp = ((int32_t)(T_out - T0_out)) * ((int32_t)(T1_degC - T0_degC)*10);
  TC = tmp /(T1_out - T0_out) + T0_degC*10;

  tmp = ((int32_t)(H_T_out - H0_T0_out)) * ((int32_t)(H1_rh - H0_rh)*10);
  *value = (uint16_t) (tmp/(H1_T0_out - H0_T0_out) + H0_rh);

}
/**
 * @brief Convert integer to string.
 * @param i The integer.
 * @return Pointer to statically allocated string.
 * @note The string is overwritten upon the next call.
 */
char *itoa(int i)
{
  static char buf[16];
  char *p = buf;
  int n = 1000000000;
  int onlyzero = 1;
  if (i < 0) {
    *p++ = '-';
    i = -i;
  }
  while (n) {
    int k = i / n;
    i = i % n;
    n = n / 10;
    if (onlyzero) {
      if (k == 0)
        continue;
      else
        onlyzero = 0;
    }
    *p = '0' + k;
    p++;
  }
  if (onlyzero)
    *p++ = '0';
  *p = 0;
  return buf;
}



//sprintf((char *) wbuf, "T1_out = %d \n\r\n\r", T1_out);
//printu(wbuf);
//sprintf((char *) wbuf, "TC = %d C \n\r", TC);
//printu((char *) wbuf);
//sprintf((char *) wbuf, "HrH = %d%% rH", HrH);
//printu((char *) wbuf);




////printu("sending to adr 0x%x \n\r ", SAD);
//readBuf[0] = CTRL_REG1; buf[1] = INIT;
//ret = HAL_I2C_Master_Transmit(&hi2c3, SAD, buf, 2, HAL_MAX_DELAY);
//if(ret != HAL_OK){
//  //printu("ERROR_TX\n\r");
//}
//
////printu("OK TX\n\r");
//
//

//buf[0] = CLBR_H0;
//   ret = HAL_I2C_Master_Transmit(&hi2c3, SAD, buf, 1, HAL_MAX_DELAY);
//   if(ret != HAL_OK){
//     //printu("ERROR_TX clbr_h0\n\r");
//   }
//   ret = HAL_I2C_Master_Receive(&hi2c3, SAD, buf, 1, HAL_MAX_DELAY);
//   if(ret != HAL_OK){
//     //printu("ERROR_RX clbr_h0\n\r");
//   }
//   //printu("H0_RH recieved\n\r");
