/*
 * test.c
 *
 *  Created on: Nov 25, 2019
 *      Author: pjotr
 */
#include <math.h>
#include <string.h>

#include "cmsis_os2.h"
#include "rtc.h"
#include "usart.h"
#include "i2c.h"

#include "test.h"
#include "display.h"
#include "pot.h"
#include "utilio.h"

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
  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, 0x80|0x3a, 1, buffer, 2, 10);
  H1_T0_out = (((uint16_t)buffer[1])<<8) | (uint16_t)buffer[0];

  /*4. Read H_T_OUT */
  HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, 0x80|0x28, 1, buffer, 2, 10);
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

  char str[21];
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

  sprintf(str, "%d\n\r", (int)trunc(tmp_f)); //FIXA TRUNC!
  printu(str);
}




