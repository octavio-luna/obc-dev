################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include csp-src/interfaces/subdir.mk
-include csp-src/drivers/usart/subdir.mk
-include csp-src/crypto/subdir.mk
-include csp-src/atomics/subdir.mk
-include csp-src/arch/freertos/subdir.mk
-include csp-src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
labsat-obc-master.hex \

SECONDARY_SIZE += \
labsat-obc-master.siz \


# All Target
all: labsat-obc-master.elf secondary-outputs

# Tool invocations
labsat-obc-master.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=arm926ej-s -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Xlinker --gc-sections -L"C:\ISIS\ISIS-OBC\hal\at91\lib" -L"C:\ISIS\ISIS-OBC\hal\freertos\lib" -L"C:\ISIS\ISIS-OBC\hal\hal\lib" -L"C:\ISIS\ISIS-OBC\hal\hcc\lib" -L"C:\ISIS\ISIS-OBC\mission-support\mission-support\lib" -L"C:\ISIS\ISIS-OBC\satellite-subsystems\satellite-subsystems\lib" -Wl,-Map,"labsat-obc-master.map" -o "labsat-obc-master.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

labsat-obc-master.hex: labsat-obc-master.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "labsat-obc-master.elf"  "labsat-obc-master.hex"
	@echo 'Finished building: $@'
	@echo ' '

labsat-obc-master.siz: labsat-obc-master.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "labsat-obc-master.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(C++_DEPS)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(CC_DEPS)$(SECONDARY_FLASH)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS)$(S_UPPER_DEPS) labsat-obc-master.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
