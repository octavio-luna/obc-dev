################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../csp-src/atomics/atomics_freertos_gcc.c 

OBJS += \
./csp-src/atomics/atomics_freertos_gcc.o 

C_DEPS += \
./csp-src/atomics/atomics_freertos_gcc.d 


# Each subdirectory must supply rules for building sources it contributes
csp-src/atomics/%.o: ../csp-src/atomics/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=arm926ej-s -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D__ASSEMBLY__ -Dsdram -Dat91sam9g20 -DTRACE_LEVEL=5 -DDEBUG=1 -D'BASE_REVISION_NUMBER=$(REV)' -D'BASE_REVISION_HASH_SHORT=$(REVHASH_SHORT)' -D'BASE_REVISION_HASH=$(REVHASH)' -IC:/ISIS/ISIS-OBC/hal/hal/include -IC:/ISIS/ISIS-OBC/hal/hcc/include -IC:/ISIS/ISIS-OBC/hal/at91/include -IC:/ISIS/ISIS-OBC/hal/freertos/include -IC:/ISIS/ISIS-OBC/mission-support/mission-support/include -IC:/ISIS/ISIS-OBC/satellite-subsystems/satellite-subsystems/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


