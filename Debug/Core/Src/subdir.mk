################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/eeprom.c \
../Core/Src/fault_library.c \
../Core/Src/fault_test.c \
../Core/Src/freertos.c \
../Core/Src/main.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/system_stm32l4xx.c 

OBJS += \
./Core/Src/eeprom.o \
./Core/Src/fault_library.o \
./Core/Src/fault_test.o \
./Core/Src/freertos.o \
./Core/Src/main.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/system_stm32l4xx.o 

C_DEPS += \
./Core/Src/eeprom.d \
./Core/Src/fault_library.d \
./Core/Src/fault_test.d \
./Core/Src/freertos.d \
./Core/Src/main.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/system_stm32l4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_HAL_DRIVER -DSTM32L432xx -I"C:/Users/lukeo/OneDrive - purdue.edu/PER/FaultLibrary/Core/Inc" -I"C:/Users/lukeo/OneDrive - purdue.edu/PER/FaultLibrary/Drivers/STM32L4xx_HAL_Driver/Inc" -I"C:/Users/lukeo/OneDrive - purdue.edu/PER/FaultLibrary/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/lukeo/OneDrive - purdue.edu/PER/FaultLibrary/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"C:/Users/lukeo/OneDrive - purdue.edu/PER/FaultLibrary/Drivers/CMSIS/Include" -I"C:/Users/lukeo/OneDrive - purdue.edu/PER/FaultLibrary/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/lukeo/OneDrive - purdue.edu/PER/FaultLibrary/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/lukeo/OneDrive - purdue.edu/PER/FaultLibrary/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


