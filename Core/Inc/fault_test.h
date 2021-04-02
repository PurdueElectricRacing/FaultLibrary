/*
 * fault_test.h
 *
 *  Created on: Feb 27, 2021
 *      Author: lukeo
 */

#ifndef INC_FAULT_TEST_H_
#define INC_FAULT_TEST_H_

#include "main.h"
#include "stm32l4xx_hal.h"
#include "fault_library.h"

//SAMPLE FUNCTIONS
void setLightRed();
void setLightBlue();
void setLightGreen();
void setLightOff();

#define BUTTON_PERIOD 50

#endif /* INC_FAULT_TEST_H_ */
