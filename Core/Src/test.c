/*
 * test.c
 *
 *  Created on: Nov 25, 2019
 *      Author: samfl
 */
#include <test.h>

void test(void) {
	//Test_display_background("red");
	testDisplay();
	testPot();

}

void testDisplay(void) {
	displayInit();
//	Write_data(0x61);
}

void testBacklight(uint8_t color[]) {
	Display_color(color);
}

void testPot(){
  potPollTest();
}
