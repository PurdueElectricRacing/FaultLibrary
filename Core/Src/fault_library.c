#include "fault_library.h"

TaskHandle_t Faul_Task_Handle;

void twoBitFieldSet(uint64_t *field, uint8_t loc, uint8_t val);
uint8_t twoBitFieldGet(uint64_t *field, uint8_t loc);
void oneBitFieldSet(uint32_t *field, uint8_t loc, uint8_t val);
uint8_t oneBitFieldGet(uint32_t *field, uint8_t loc);
void callTwoBitFunction(uint8_t bit_num, uint64_t *saved_call_types,
                  uint8_t current_call_type, void (*handlers[])());
void callOneBitFunction(uint8_t bit_num, uint32_t *saved_call_types,
                  uint8_t current_call_type, void (*handlers[])());

void twoBitFieldSet(uint64_t *field, uint8_t loc, uint8_t val)
{
  *field &= ~(0b11 << loc * 2);
  *field |= val << loc * 2;
}

uint8_t twoBitFieldGet(uint64_t *field, uint8_t loc)
{
  return ((*field >> loc * 2) & 0b11);
}

void oneBitFieldSet(uint32_t *field, uint8_t loc, uint8_t val) {
  *field &= (uint32_t) ~(0b1 << loc);
  *field |= val << loc;
}

uint8_t oneBitFieldGet(uint32_t *field, uint8_t loc)
{
  return ((*field >> loc ) & 1);
}

//starts the fault task
void faultLibStart()
{

  eepromLoadStruct(FAULT_EEPROM_NAME);

  xTaskCreate(faultTask, "Faults", 256, NULL, 1, &Faul_Task_Handle);
}

//sets up a new fault, gotta love these arguments
void faultCreate(uint8_t bit_num, fault_criticality_t level,
                    uint16_t rise_threshold_ms, uint16_t fall_threshold_ms,
                    fault_historic_t hist,
                    fault_set_t set_type, void (*set_handle),
                    fault_fall_t fall_type, void (*fall_handle),
                    fault_off_t off_type, void (*off_handle))
{
  twoBitFieldSet(&faults.stored.criticality, bit_num, level);
  faults.rise_threshold[bit_num] = rise_threshold_ms / PERIOD_FAULT_TASK;
  faults.fall_threshold[bit_num] = fall_threshold_ms / PERIOD_FAULT_TASK;
  oneBitFieldSet(&faults.historic_type, bit_num, hist);
  twoBitFieldSet(&faults.set_call_type, bit_num, set_type);
  faults.set_handler[bit_num] = set_handle;
  twoBitFieldSet(&faults.fall_call_type, bit_num, fall_type);
  faults.fall_handler[bit_num] = fall_handle;
  oneBitFieldSet(&faults.off_call_type, bit_num, off_type);
  faults.off_handler[bit_num] = off_handle;
}

//signals that a fault has occurred
//NOT set, set requires signal on for a time threshold
void signalFault(uint8_t bit_pos)
{
  faults.stored.signal |= 1 << bit_pos;
}

void faultTask()
{
  while (PER == GREAT)
  {
    TickType_t current_tick_time = xTaskGetTickCount();

    for (int i = 0; i < FAULT_MAX; i++)
    {
      if (oneBitFieldGet(&faults.stored.set, i))
      {
        //fault is currently active
        callTwoBitFunction(i, &faults.set_call_type,
                           SET_CONTINUOUS, faults.set_handler);

        if (oneBitFieldGet(&faults.stored.signal, i))
        {
          //reset cooldown counter if set again
          faults.current_time[i] = 0;
        }
        else if (faults.current_time[i] > faults.fall_threshold[i])
        {
          // It fell for the minimum time, turn fault off
          oneBitFieldSet(&faults.stored.set, i, 0);
          // reset counter for rise
          faults.current_time[i] = 0;

          callOneBitFunction(i, &faults.off_call_type,
                             OFF_ENABLED, faults.off_handler);
        }
        else
        {
          //Currently falling, increment timer

          if (faults.current_time[i] == 0) {
            // initial falling
            callTwoBitFunction(i, &faults.fall_call_type,
                               FALL_SINGLE, faults.fall_handler);
          }

          faults.current_time[i]++;

          callTwoBitFunction(i, &faults.fall_call_type,
                             FALL_CONTINUOUS, faults.fall_handler);
        }
      }
      else if(oneBitFieldGet(&faults.historic_type, i) == HISTORIC_OVERRIDE && oneBitFieldGet(&faults.stored.historic, i))
      {
        // fault not on, but historic says to override
        oneBitFieldSet(&faults.stored.set, i, 1);
        callTwoBitFunction(i, &faults.set_call_type,
                           SET_SINGLE, faults.set_handler);
      }
      else if(oneBitFieldGet(&faults.stored.signal, i))
      {
        // fault is not set, use rise counter

        if (faults.current_time[i] < faults.rise_threshold[i])
        {
          faults.current_time[i]++;
        }
        else
        {
          // fault set, reset timer for cooldown
          oneBitFieldSet(&faults.stored.set, i, 1);
          oneBitFieldSet(&faults.stored.historic, i, 1);
          faults.current_time[i] = 0;

          callTwoBitFunction(i, &faults.set_call_type,
                       SET_SINGLE, faults.set_handler);
        }
      }
      else
      {
        //fault and signal off, reset timer
        faults.current_time[i] = 0;
      }
    }

    //read signals, reset for next cycle
    faults.stored.signal = 0;

    vTaskDelayUntil(&current_tick_time, pdMS_TO_TICKS(PERIOD_FAULT_TASK));
  }

  vTaskDelete(NULL);
}

void callTwoBitFunction(uint8_t bit_num, uint64_t *saved_call_types,
                  uint8_t current_call_type, void (*handlers[])())
{
  if (twoBitFieldGet(saved_call_types, bit_num) == current_call_type &&
      handlers[bit_num] != NULL)
  {
    (*handlers[bit_num])(); //call corresponding function
  }
}

void callOneBitFunction(uint8_t bit_num, uint32_t *saved_call_types,
                  uint8_t current_call_type, void (*handlers[])())
{
  if (oneBitFieldGet(saved_call_types, bit_num) == current_call_type &&
      handlers[bit_num] != NULL)
  {
    (*handlers[bit_num])(); //call corresponding function
  }
}

void faultLibShutdown()
{
  //vTaskDelete(Faul_Task_Handle);
  uint8_t e = eepromSaveStruct(FAULT_EEPROM_NAME);
  if(!e)
  {
    handleNoError();
  }
}


void handleCriticalError()
{
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

}

void handleMediumError()
{
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
}

void handleWarningError()
{
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);

}

void handleNoError()
{
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
}
