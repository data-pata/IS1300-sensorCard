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

/**
* @brief Read HTS221 Humidity output registers, and calculate humidity.
* @param Pointer to the returned humidity value that must be divided by 10 to get the value in [%].
* @retval Error code [HTS221_OK, HTS221_ERROR].
*/
void getHumidity(uint16_t* value)
{
  int16_t H0_T0_out, H1_T0_out, H_T_out;
  int16_t H0_rh, H1_rh;
  uint8_t buffer[2];
  int32_t tmp;

  uint8_t init = 0x8 | 0x4 | 0x1;
  // PD set, bdu, 1hz
  HAL_I2C_Mem_Write(&hi2c3, HTS221_SAD, CTRL_REG1, 1, &init, 1, 50);

  /* 1. Read H0_rH and H1_rH coefficients*/
  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, 0x80|CLBR_REG, 1, buffer, 2, 10);
  H0_rh = buffer[0]>>1;
  H1_rh = buffer[1]>>1;

  /*2. Read H0_T0_OUT */
  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, 0x80|0x36, 1, buffer, 2, 10);
  H0_T0_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];

  /*3. Read H1_T0_OUT */
  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, 0x80|HTS221_H1_T0_OUT_L, 1, buffer, 2, 10);
  H1_T0_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];

  /*4. Read H_T_OUT */
  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, 0x80|HTS221_HR_OUT_L_REG, 1, buffer, 2, 10);
  H_T_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];

  /*5. Compute the RH [%] value by linear interpolation */
  tmp = ((int32_t)(H_T_out - H0_T0_out)) * ((int32_t)(H1_rh - H0_rh));
  tmp = (tmp/(H1_T0_out - H0_T0_out) + H0_rh);
  char str[10];
  sprintf(str, ".%d rH", (int)tmp);
  printdL(2, str);
  char wbuf[20];
  sprintf((char *) wbuf, "H0_rH = %d%% rH \n\r", H0_rh);
  printu(wbuf);
  sprintf((char *) wbuf, "H1_rH = %d%% rH \n\r", H1_rh);
  printu(wbuf);
  sprintf((char *) wbuf, "H0_T0_out = %d \n\r", H0_T0_out);
  printu(wbuf);
  sprintf((char *) wbuf, "H1_T0_out = %d \n\r", H1_T0_out);
  printu(wbuf);
}


void tempCal() {

  int16_t T0_out, T1_out, T_out, T0_degC_x8_u16, T1_degC_x8_u16;
  int16_t T0_degC, T1_degC;
  uint8_t buffer[4], T1_T2_msb;
  float   tmp_f;

  // Read from 0x32 & 0x33 registers the value of coefficients T0_degC_x8 and T1_degC_x8
  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, 0x80|0x32, 1, buffer, 2, 100);

  /*2. Read from 0x35 register the value of the MSB bits of T1_degC and T0_degC */
  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, 0x35, 1, &T1_T2_msb, 1, 100);

  /*Calculate the T0_degC and T1_degC values*/
  T0_degC_x8_u16 = (((uint16_t)(T1_T2_msb & 0x03)) << 8) | ((uint16_t)buffer[0]); //166
  T1_degC_x8_u16 = (((uint16_t)(T1_T2_msb & 0x0C)) << 6) | ((uint16_t)buffer[1]); //422
  T0_degC = T0_degC_x8_u16 >> 3;  //20
  T1_degC = T1_degC_x8_u16 >> 3;  //52

  /*3. Read from 0x3C & 0x3D registers the value of T0_OUT*/
  /*4. Read from 0x3E & 0x3F registers the value of T1_OUT*/
  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, 0x80|0x3c, 1, buffer, 4, 100);
  T0_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];  //514
  T1_out = (((uint16_t)buffer[3]) << 8) | (uint16_t)buffer[2];  //514

  /* 5.Read from 0x2A & 0x2B registers the value T_OUT (ADC_OUT).*/
  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, 0x80|0x2A, 1, buffer, 2, 100);

  T_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0]; //-14650

  /* 6. Compute the Temperature value by linear interpolation*/
  tmp_f = (float)(T_out - T0_out) * (float)(T1_degC - T0_degC) / (float)(T1_out - T0_out)  +  T0_degC;

  char str[10];
  sprintf((char *) str, "T0_degC = %d C\n\r", T0_degC);
  printu(str);
  sprintf((char *) str, "T1_degC = %d C\n\r", T1_degC);
  printu(str);
  sprintf((char *) str, "T0_out = %d\n\r",T0_out);
  printu(str);
  sprintf((char *) str, "T1_out = %d\n\r", T1_out);
  printu(str);
  sprintf((char *) str, "T_out = %d\n\r", T_out);
  printu(str);

  sprintf(str, "%d\n\r", (int)trunc(tmp_f));
  printu(str);
}

char * ftoi(float f) {
  char str[25];

  char *tmpSign = (f < 0) ? "-" : "";
  float tmpVal = (f < 0) ? -f : f;

  int tmpInt1 = tmpVal;                  // Get the integer (678). implicit cast?
  float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
  int tmpInt2 = trunc(tmpFrac * 10000);  // Turn into integer (123).

  // Print as parts, note that you need 0-padding for fractional bit.

  sprintf (str, "f = %s%d.%04d\n", tmpSign, tmpInt1, tmpInt2);
  printu(str);
  return str;
}

//
//int16_t Hx_rh; // = (H1_rh - H0_rh)
//int16_t H0_T0_out, H1_T0_out, H0_rh, H1_rh;
//int16_t T0_degC, T1_degC, T0_out, T1_out;
//
//int32_t tmp;
//uint8_t initFlag= 0;
//
//void humTempCalib(void) {
//
//  uint8_t wbuf[18];
//  uint8_t rbuf[15];
//
//  if(!initFlag) {
//    initFlag=1;
//
//  int16_t T0_degC_x8_u16, T1_degC_x8_u16;
//  uint8_t T1_T0_msb;
//
//// READ HUMIDITY CALIBRATION VALUES
//
//    HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, CLBR_REG, 1, rbuf, 15, 50);
//
//    H0_rh = rbuf[0]>>1;      // calibration values must be divided by 2
//    H1_rh = rbuf[1]>>1;
//    H0_T0_out = (uint16_t)rbuf[0x6] | (((uint16_t)rbuf[0x7]) << 8); // concatenate to correct int16_t values
//    H1_T0_out = (uint16_t)rbuf[0xA] | (((uint16_t)rbuf[0xB]) << 8);
//
////    sprintf((char *) wbuf, "rbuf[0x6] = %x rbuf[0x7] = %x \n\r", rbuf[0x6], rbuf[0x7]);
////    printu(wbuf);
////    sprintf((char *) wbuf, "rbuf[0xA] = %x rbuf[0xB] = %x \n\r", rbuf[0xA], rbuf[0xB]);
////    printu(wbuf);
//
////    H0_T0_out = ~H0_T0_out + 1; // two's complement
////    H1_T0_out = ~H1_T0_out + 1; // but since datatype is signed it will display corr values?
////   0  1  2  3  4  5  6  7  8  9  A  B
////0x30 31 32 33 34 35 36 37 38 39 3A 3B
//
////    sprintf((char *) wbuf, "H0_rH = %d%% rH \n\r", H0_rh);
////    printu(wbuf);
////    sprintf((char *) wbuf, "H1_rH = %d%% rH \n\r", H1_rh);
////    printu(wbuf);
////    sprintf((char *) wbuf, "H0_T0_out = %d \n\r", H0_T0_out);
////    printu(wbuf);
////    sprintf((char *) wbuf, "H1_T0_out = %d \n\r", H1_T0_out);
////    printu(wbuf);
//
//// READ TEMP CALIBRATION VALUES
//
//    T1_T0_msb = rbuf[0x5];
//    T0_degC_x8_u16 = (((uint16_t)(T1_T0_msb & 0x03)) << 8) | ((uint16_t)rbuf[0x2]);
//    T1_degC_x8_u16 = (((uint16_t)(T1_T0_msb & 0x0C)) << 6) | ((uint16_t)rbuf[0x3]);
//
//    T0_degC = T0_degC_x8_u16>>3;
//    T1_degC = T1_degC_x8_u16>>3;
//
////    T0_degC = rbuf[0x2] >> 3;
////    T1_degC = rbuf[0x3] >> 3;
////    T1_T0_msb = rbuf[0x5];
////    T0_degC = ((T1_T0_msb & 0x3) << 8) | T0_degC; // 3
////    T1_degC = ((T1_T0_msb & 0xC) << 6) | T1_degC;
//
//    T0_out = (uint16_t)rbuf[0xC] | (((uint16_t)rbuf[0xD]) << 8);
//    T1_out = (uint16_t)rbuf[0xE] | (((uint16_t)rbuf[0xF]) <<8);
//
////    sprintf((char *) wbuf, "T0_degC = %d C\n\r", T0_degC);
////    printu(wbuf);
////    sprintf((char *) wbuf, "T1_degC = %d C\n\r", T1_degC);
////    printu(wbuf);
////    sprintf((char *) wbuf, "T0_out = %d\n\r",T0_out);
////    printu(wbuf)
////    sprintf((char *) wbuf, "T1_out = %d\n\r", T1_out);
////    printu(wbuf);
//  }
//
//  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, DATA_REG, 1, rbuf, 4, 50);
//
//  int16_t H_T_out = (uint16_t)rbuf[0] | (((uint16_t)rbuf[1])<<8);
//
//  int32_t tmp = ((int32_t)(H_T_out - H0_T0_out)) * ((int32_t)(H1_rh - H0_rh)*10);
//  int32_t H = (uint16_t) (tmp/(H1_T0_out - H0_T0_out) + H0_rh);
//
//  int16_t T_out = (uint16_t)rbuf[0] | (((uint16_t)rbuf[1])<<8);
//
//  tmp = ((int32_t)(T_out - T0_out)) * ((int32_t)(T1_degC - T0_degC)*10);
//  int32_t TC = tmp /(T1_out - T0_out) + T0_degC*10;
//
//}


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
