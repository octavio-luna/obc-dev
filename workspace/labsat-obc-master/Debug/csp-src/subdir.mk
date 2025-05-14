################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../csp-src/csp_bridge.c \
../csp-src/csp_buffer.c \
../csp-src/csp_conn.c \
../csp-src/csp_crc32.c \
../csp-src/csp_debug.c \
../csp-src/csp_dedup.c \
../csp-src/csp_hex_dump.c \
../csp-src/csp_id.c \
../csp-src/csp_iflist.c \
../csp-src/csp_init.c \
../csp-src/csp_io.c \
../csp-src/csp_port.c \
../csp-src/csp_promisc.c \
../csp-src/csp_qfifo.c \
../csp-src/csp_rdp.c \
../csp-src/csp_rdp_queue.c \
../csp-src/csp_route.c \
../csp-src/csp_rtable_cidr.c \
../csp-src/csp_rtable_stdio.c \
../csp-src/csp_service_handler.c \
../csp-src/csp_services.c \
../csp-src/csp_sfp.c \
../csp-src/csp_yaml.c 

OBJS += \
./csp-src/csp_bridge.o \
./csp-src/csp_buffer.o \
./csp-src/csp_conn.o \
./csp-src/csp_crc32.o \
./csp-src/csp_debug.o \
./csp-src/csp_dedup.o \
./csp-src/csp_hex_dump.o \
./csp-src/csp_id.o \
./csp-src/csp_iflist.o \
./csp-src/csp_init.o \
./csp-src/csp_io.o \
./csp-src/csp_port.o \
./csp-src/csp_promisc.o \
./csp-src/csp_qfifo.o \
./csp-src/csp_rdp.o \
./csp-src/csp_rdp_queue.o \
./csp-src/csp_route.o \
./csp-src/csp_rtable_cidr.o \
./csp-src/csp_rtable_stdio.o \
./csp-src/csp_service_handler.o \
./csp-src/csp_services.o \
./csp-src/csp_sfp.o \
./csp-src/csp_yaml.o 

C_DEPS += \
./csp-src/csp_bridge.d \
./csp-src/csp_buffer.d \
./csp-src/csp_conn.d \
./csp-src/csp_crc32.d \
./csp-src/csp_debug.d \
./csp-src/csp_dedup.d \
./csp-src/csp_hex_dump.d \
./csp-src/csp_id.d \
./csp-src/csp_iflist.d \
./csp-src/csp_init.d \
./csp-src/csp_io.d \
./csp-src/csp_port.d \
./csp-src/csp_promisc.d \
./csp-src/csp_qfifo.d \
./csp-src/csp_rdp.d \
./csp-src/csp_rdp_queue.d \
./csp-src/csp_route.d \
./csp-src/csp_rtable_cidr.d \
./csp-src/csp_rtable_stdio.d \
./csp-src/csp_service_handler.d \
./csp-src/csp_services.d \
./csp-src/csp_sfp.d \
./csp-src/csp_yaml.d 


# Each subdirectory must supply rules for building sources it contributes
csp-src/%.o: ../csp-src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=arm926ej-s -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D__ASSEMBLY__ -Dsdram -Dat91sam9g20 -DTRACE_LEVEL=5 -DDEBUG=1 -D'BASE_REVISION_NUMBER=$(REV)' -D'BASE_REVISION_HASH_SHORT=$(REVHASH_SHORT)' -D'BASE_REVISION_HASH=$(REVHASH)' -IC:/ISIS/ISIS-OBC/hal/hal/include -IC:/ISIS/ISIS-OBC/hal/hcc/include -IC:/ISIS/ISIS-OBC/hal/at91/include -IC:/ISIS/ISIS-OBC/hal/freertos/include -IC:/ISIS/ISIS-OBC/mission-support/mission-support/include -IC:/ISIS/ISIS-OBC/satellite-subsystems/satellite-subsystems/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


