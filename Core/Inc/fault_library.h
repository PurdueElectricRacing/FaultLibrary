/*
 * fault_library.h
 *
 *  Created on: Feb 6, 2020
 *      Author: Luke Oxley
 */

#ifndef FAULT_LIBRARY_H_
#define FAULT_LIBRARY_H_

#include <string.h>
#include "main.h"
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"
#include "eeprom.h"

// Be sure to include headers with locations to functions!
#include "fault_test.h"

#define PER 1
#define GREAT PER

#define PERIOD_FAULT_TASK 50 //ms

#define FAULT_MAX 32

#define FAULT_EEPROM_NAME "flt"

typedef struct {
  uint32_t signal; //reset after each checking cycle
  uint32_t set; //achieved minimum time
  uint32_t historic; //only resets by user
  uint64_t criticality; //2 bit bit field
} fault_stored_t;

typedef struct {
  volatile fault_stored_t stored;
  uint16_t rise_threshold[FAULT_MAX];
  uint16_t fall_threshold[FAULT_MAX];
  uint16_t current_time[FAULT_MAX];
  void (*set_handler[FAULT_MAX])(); //function pointer
  void (*cont_handler[FAULT_MAX])();
  void (*off_handler[FAULT_MAX])();
  uint32_t historic_type;

} fault_t;

fault_t faults;

typedef enum {
  HISTORIC_IGNORE,
  HISTORIC_OVERRIDE
} fault_historic_t;

typedef enum {
  FAULT_WARNING,
  FAULT_ERROR,
  FAULT_CRITICAL
} fault_criticality_t;

void faultLibInitialize();
void signalFault(uint8_t bit_pos);
void faultLibShutdown();
void clearHistory();
uint8_t getFaultSet(uint8_t loc);
uint8_t getFaultSignal(uint8_t loc);
uint8_t getHistoricOverriding(uint8_t loc);
fault_criticality_t getCriticality(uint8_t loc);

//Generic criticality functions
//Called once when fault set
void handleCriticalFault();
void handleErrorFault();
void handleWarningFault();

#endif
