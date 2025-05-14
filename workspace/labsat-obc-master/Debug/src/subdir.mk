################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CSPManager.c \
../src/DevelTest.c \
../src/LogManager.c \
../src/PowerManager.c \
../src/PowerManagerUart.c \
../src/SDManager.c \
../src/TimerManager.c \
../src/main.c 

OBJS += \
./src/CSPManager.o \
./src/DevelTest.o \
./src/LogManager.o \
./src/PowerManager.o \
./src/PowerManagerUart.o \
./src/SDManager.o \
./src/TimerManager.o \
./src/main.o 

C_DEPS += \
./src/CSPManager.d \
./src/DevelTest.d \
./src/LogManager.d \
./src/PowerManager.d \
./src/PowerManagerUart.d \
./src/SDManager.d \
./src/TimerManager.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=arm926ej-s -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D__ASSEMBLY__ -Dsdram -Dat91sam9g20 -DTRACE_LEVEL=5 -DDEBUG=1 -D'BASE_REVISION_NUMBER=$(REV)' -D'BASE_REVISION_HASH_SHORT=$(REVHASH_SHORT)' -D'BASE_REVISION_HASH=$(REVHASH)' -IC:/ISIS/ISIS-OBC/hal/hal/include -IC:/ISIS/ISIS-OBC/hal/hcc/include -IC:/ISIS/ISIS-OBC/hal/at91/include -IC:/ISIS/ISIS-OBC/hal/freertos/include -IC:/ISIS/ISIS-OBC/mission-support/mission-support/include -IC:/ISIS/ISIS-OBC/satellite-subsystems/satellite-subsystems/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


