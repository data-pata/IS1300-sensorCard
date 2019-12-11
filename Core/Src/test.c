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

uint8_t strbuf[60];

#define HTS221_SAD (0x5f << 1)  // slave adress [SHIFTED]
#define CTRL_REG1 0x20 // [7] PD: set to activate,
#define CTRL_REG2 0x21 //

#define INIT 0x85       //Power ON, BDU set, Continuous update, Data output rate = 1 Hz
#define CLBR_REG  0x30  //
static const uint8_t SAD = (0x5f << 1); // 7bit slave adress HAL lib adds r/w bit

HAL_StatusTypeDef ret;
int16_t H0_T0_out, H1_T0_out, H_T_out;
int16_t H0_rh, H1_rh;

uint8_t buf[15];

int32_t tmp;
uint8_t initFlag= 0;

void getHumidity(void) {

  if (1){

    // get humidity calibration values
    /* 1. Read H0_rH and H1_rH coefficients
     * */

    HAL_I2C_Mem_Read(&hi2c3, HTS221_SAD, CLBR_REG, 1, buf, 15, 100);
    //HAL_I2C_Master_Transmit(&hi2c3, SAD, buf, 1, HAL_MAX_DELAY);
    //HAL_I2C_Master_Receive(&hi2c3, SAD, buf, 1, HAL_MAX_DELAY);

    // calibration values must be divided by 2
    H0_rh = buf[0]>>1;
    H1_rh = buf[1]>>1;

    sprintf(strbuf, "H0_rH_2x/2 = %d%% rH\n\r", H0_rh);
    printu(strbuf);
    sprintf(strbuf, "H1_rH_2x/2 = %d%% rH\n\r", H1_rh);
    printu(strbuf);

    /*2. Read H0_T0_OUT */
    /*3. Read H1_T0_OUT */
    //buf[0]=0x36;

  }

  while (1){
    osDelay(500);

  }

}


void runTests(void) {
  //getHumidity();
  displayInit();
  testPot();

  //setTime();
  //rtcTest();
}

void testBacklight(uint8_t color[]) {

}

void testPot(){
  potItTest();
  //potPollTest();
}


////printu("sending to adr 0x%x \n\r ", SAD);
//buf[0] = CTRL_REG1; buf[1] = INIT;
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
