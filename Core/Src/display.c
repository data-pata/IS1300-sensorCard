 /**
******************************************************************************
@brief EA DOGS104W-A display drivers.
@file display.c
@author Patrik Sjöfors
@version
@date
@brief
******************************************************************************
*/
#include "display.h"

#define RESET_CS HAL_GPIO_WritePin(CS_SPI2_GPIO_Port, CS_SPI2_Pin, 0)
#define SET_CS HAL_GPIO_WritePin(CS_SPI2_GPIO_Port, CS_SPI2_Pin, 1)

/**
 * @brief diplayInit Initializes display
 * @note
 */
void displayInit(void){
  SET_CS;
  HAL_Delay(1);
  RESET_CS;

  displayReset();
  HAL_Delay(10);

  writeIns(0x3A); //8-Bit data length extension Bit RE=1; REV=0

  writeIns(0x09); //4 line display
  writeIns(0x06); //Bottom view
  writeIns(0x1E); //Bias setting BS1=1
  writeIns(0x39); //8-Bit data length extension Bit RE=0; IS=1
  writeIns(0x1B); //BS0=1 -> Bias=1/6
  writeIns(0x6E); //Divider on and set value
  writeIns(0x56); //Booster on and set contrast (BB1=C5, DB0=C4)
  writeIns(0x7A); //Set contrast (DB3-DB0=C3-C0)

  writeIns(0x38); //8-Bit data length extension Bit RE=0; IS=0

  clearDisplay();
  writeIns(0x0F);

  SET_CS;
}

/**
 * @brief printd Prints a string to display
 * @param str String to print
 * @note
 */
void printd(uint8_t * str) {
  do {
    writeD(*str++);
  } while(*str);
}

/**
 * @brief writeD Write data to display.
 * @param byte Databyte to write.
 * @note
 */
void writeD(uint8_t byte) {
  RESET_CS;
  uint8_t data[3];
  data[0] = 0x5f;
  data[1] = byte & 0x0f;
  data[2] = (byte >> 4) & 0x0f;
  HAL_SPI_Transmit(&hspi2, data, 3, 10);
  SET_CS;
}
/**
 * @brief writeD Write data to display.
 * @param byte Databyte to write.
 * @note
 */
void DisplayOnOff(uint8_t data) {
  writeIns(0x08+data);
}
/**
 * @brief writeD Write data to display.
 * @param byte Databyte to write.
 * @note
 */
void writeIns(uint8_t ins) {

  RESET_CS;

  uint8_t outputBuffer[3];
  outputBuffer[0] = 0x1F;
  outputBuffer[1] = ins & 0x0F;                 // mask 4 lsb:s
  outputBuffer[2] = (ins>>4) & 0x0F;            // mask 4 msb:s

  HAL_SPI_Transmit(&hspi2, outputBuffer, 3, 10);

  SET_CS;
}
/**
 * @brief clearDisplay Clears all content.
 * @note
 */
void clearDisplay(){
  writeIns(0x01);
}

/**
 * @brief displayReset Resets display.
 * @note
 */
void displayReset(void){
  HAL_GPIO_WritePin(Display_reset_GPIO_Port, Display_reset_Pin, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(Display_reset_GPIO_Port, Display_reset_Pin, GPIO_PIN_SET);
}

/**
 * @brief printdL Prints a string to specified line on display.
 * @param line: Zero indexed line to print to, min 0 max 3.
 * @param str: Char pointer to str, max 10 characters.
 */
void printdL(int line, char* str) {
  int addr = line * 0x20;
  writeIns(0x80 | addr); //set adr pointer
  printd(str);
}

