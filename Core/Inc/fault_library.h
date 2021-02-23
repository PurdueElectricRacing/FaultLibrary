/*
 * fault_library.h
 *
 *  Created on: Feb 6, 2020
 *      Author: Luke Oxley
 */

#ifndef FAULT_LIBRARY_H_
#define FAULT_LIBRARY_H_

#include "main.h"
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"
#include "eeprom.h"

#define PER 1
#define GREAT PER

#define PERIOD_FAULT_TASK 50 //ms

#define FAULT_MAX 32

#define FAULT_EEPROM_NAME "flt"

typedef struct{
  uint32_t signal; //reset after each checking cycle
  uint32_t set; //achieved minimum time
  uint32_t historic; //only resets by user
  uint64_t criticality; //2 bit bit field
} fault_stored_t;

typedef struct {
  fault_stored_t stored;
  uint16_t rise_threshold[FAULT_MAX];
  uint16_t fall_threshold[FAULT_MAX];
  uint16_t current_time[FAULT_MAX];
  void (*set_handler[FAULT_MAX])(); //function pointer
  void (*fall_handler[FAULT_MAX])();
  void (*off_handler[FAULT_MAX])();
  uint64_t set_call_type;
  uint64_t fall_call_type;
  uint32_t off_call_type;
  uint32_t historic_type;

} fault_t;

fault_t faults;

typedef enum {
  SET_DISABLED,
  SET_SINGLE,
  SET_CONTINUOUS
} fault_set_t;

typedef enum {
  FALL_DISABLED,
  FALL_SINGLE,
  FALL_CONTINUOUS
} fault_fall_t;

typedef enum {
  OFF_DISABLED,
  OFF_ENABLED
} fault_off_t;

typedef enum {
  HISTORIC_IGNORE,
  HISTORIC_OVERRIDE
} fault_historic_t;

typedef enum {
  FAULT_WARNING,
  FAULT_ERROR,
  FAULT_CRITICAL
} fault_criticality_t;

void faultLibStart();
void faultTask();
void faultCreate( uint8_t bit_num,
                  fault_criticality_t level,
                  uint16_t rise_threshold_ms,
                  uint16_t fall_threshold_ms,
                  fault_historic_t hist,
                  fault_set_t set_type,
                  void (*set_handle),
                  fault_fall_t fall_type,
                  void (*fall_handle),
                  fault_off_t off_type,
                  void (*off_handle));
void signalFault(uint8_t bit_pos);
void faultLibShutdown();

//SAMPLE FUNCTIONS
void handleCriticalError();
void handleMediumError();
void handleWarningError();
void handleNoError();

#endif
