################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../csp-src/interfaces/csp_if_can.c \
../csp-src/interfaces/csp_if_i2c.c \
../csp-src/interfaces/csp_if_kiss.c \
../csp-src/interfaces/csp_if_lo.c \
../csp-src/interfaces/csp_if_tun.c 

OBJS += \
./csp-src/interfaces/csp_if_can.o \
./csp-src/interfaces/csp_if_i2c.o \
./csp-src/interfaces/csp_if_kiss.o \
./csp-src/interfaces/csp_if_lo.o \
./csp-src/interfaces/csp_if_tun.o 

C_DEPS += \
./csp-src/interfaces/csp_if_can.d \
./csp-src/interfaces/csp_if_i2c.d \
./csp-src/interfaces/csp_if_kiss.d \
./csp-src/interfaces/csp_if_lo.d \
./csp-src/interfaces/csp_if_tun.d 


# Each subdirectory must supply rules for building sources it contributes
csp-src/interfaces/%.o: ../csp-src/interfaces/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=arm926ej-s -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D__ASSEMBLY__ -Dsdram -Dat91sam9g20 -DTRACE_LEVEL=5 -DDEBUG=1 -D'BASE_REVISION_NUMBER=$(REV)' -D'BASE_REVISION_HASH_SHORT=$(REVHASH_SHORT)' -D'BASE_REVISION_HASH=$(REVHASH)' -IC:/ISIS/ISIS-OBC/hal/hal/include -IC:/ISIS/ISIS-OBC/hal/hcc/include -IC:/ISIS/ISIS-OBC/hal/at91/include -IC:/ISIS/ISIS-OBC/hal/freertos/include -IC:/ISIS/ISIS-OBC/mission-support/mission-support/include -IC:/ISIS/ISIS-OBC/satellite-subsystems/satellite-subsystems/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


