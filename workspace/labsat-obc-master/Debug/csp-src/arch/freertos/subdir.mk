################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../csp-src/arch/freertos/csp_clock.c \
../csp-src/arch/freertos/csp_queue.c \
../csp-src/arch/freertos/csp_semaphore.c \
../csp-src/arch/freertos/csp_system.c \
../csp-src/arch/freertos/csp_time.c 

OBJS += \
./csp-src/arch/freertos/csp_clock.o \
./csp-src/arch/freertos/csp_queue.o \
./csp-src/arch/freertos/csp_semaphore.o \
./csp-src/arch/freertos/csp_system.o \
./csp-src/arch/freertos/csp_time.o 

C_DEPS += \
./csp-src/arch/freertos/csp_clock.d \
./csp-src/arch/freertos/csp_queue.d \
./csp-src/arch/freertos/csp_semaphore.d \
./csp-src/arch/freertos/csp_system.d \
./csp-src/arch/freertos/csp_time.d 


# Each subdirectory must supply rules for building sources it contributes
csp-src/arch/freertos/%.o: ../csp-src/arch/freertos/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=arm926ej-s -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D__ASSEMBLY__ -Dsdram -Dat91sam9g20 -DTRACE_LEVEL=5 -DDEBUG=1 -D'BASE_REVISION_NUMBER=$(REV)' -D'BASE_REVISION_HASH_SHORT=$(REVHASH_SHORT)' -D'BASE_REVISION_HASH=$(REVHASH)' -IC:/ISIS/ISIS-OBC/hal/hal/include -IC:/ISIS/ISIS-OBC/hal/hcc/include -IC:/ISIS/ISIS-OBC/hal/at91/include -IC:/ISIS/ISIS-OBC/hal/freertos/include -IC:/ISIS/ISIS-OBC/mission-support/mission-support/include -IC:/ISIS/ISIS-OBC/satellite-subsystems/satellite-subsystems/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


