/*
 * display.c
 *
 *  Created on: Dec 4, 2019
 *      Author: Pjotr
 */
//#include "main.h"
#include "display.h"

void displayInit(void){
  SET_CS;
  HAL_Delay(10);
  RESET_CS;

  displayReset(); // kolla gpio-pin inst!
  HAL_Delay(10);

  HAL_GPIO_WritePin(Display_green_GPIO_Port, Display_green_Pin, 1);

  writeIns(0x3A); //8-Bit data length extension Bit RE=1; REV=0
  writeIns(0x09); //4 line display
  writeIns(0x06); //Bottom view
  writeIns(0x1E); //Bias setting BS1=1
  writeIns(0x39); //8-Bit data length extension Bit RE=0; IS=1
  writeIns(0x1B); //BS0=1 -> Bias=1/6
  writeIns(0x6E); //Devider on and set value
  writeIns(0x56); //Booster on and set contrast (BB1=C5, DB0=C4)
  writeIns(0x7A); //Set contrast (DB3-DB0=C3-C0)
  writeIns(0x38); //8-Bit data length extension Bit RE=0; IS=0
  writeIns(0x3a); // set RE, reset IS

  //writeIns(0x72); //adress

  writeIns(0x06);
  writeIns(0x38); // reset RE, reset IS
  writeIns(0x0f); //display, cursor, blink on
  writeIns(0x01); //clearDisplay

  SET_CS;
  //writeD(0xff); //ascii
  //HAL_Delay(10);

}

void writeString(uint8_t * string) {
  do {
    writeD(*string++);
  } while(*string);
}

void writeD(uint8_t byte) {
  SET_CS;
  HAL_Delay(10);
  RESET_CS;
  uint8_t data[3];
  data[0] = 0x5f;
  data[1] = byte & 0x0f;
  data[2] = (byte >> 4) & 0x0f;
  HAL_SPI_Transmit(&hspi2, data, 3, 10);
}

void DisplayOnOff(uint8_t data) {
  writeIns(0x08+data);
}

void writeIns(uint8_t ins) {

  SET_CS;
  HAL_Delay(10);
  RESET_CS;

  uint8_t outputBuffer[3];
  outputBuffer[0] = 0x1F;
  outputBuffer[1] = ins & 0x0F;                 // mask 4 lsb:s
  outputBuffer[2] = (ins>>4) & 0x0F;            // mask 4 msb:s

  HAL_SPI_Transmit(&hspi2, outputBuffer, 3, 10);
  //HAL_Delay(10);

}

void setCS(uint8_t set){
  HAL_GPIO_WritePin(CS_SPI2_GPIO_Port, CS_SPI2_Pin, set);
}
void displayReset(void){
  HAL_GPIO_WritePin(Display_reset_GPIO_Port, Display_reset_Pin, GPIO_PIN_SET);
}


