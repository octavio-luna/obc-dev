################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
isis-obc-first-project.bin \

SECONDARY_SIZE += \
isis-obc-first-project.siz \


# All Target
all: isis-obc-first-project.elf secondary-outputs

# Tool invocations
isis-obc-first-project.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=arm926ej-s -Os -fmessage-length=0 -ffunction-sections -Wall -Wextra -T C:/ISIS/ISIS-OBC/isis-obc-first-project/..//hal/at91/linker-scripts/sdram.lds -nostartfiles -Xlinker --gc-sections -L"C:/ISIS/ISIS-OBC/isis-obc-first-project/..//hal/at91/lib" -L"C:/ISIS/ISIS-OBC/isis-obc-first-project/..//hal/freertos/lib" -L"C:/ISIS/ISIS-OBC/isis-obc-first-project/..//hal/hal/lib" -Wl,-Map,"isis-obc-first-project.map" -lc -o "isis-obc-first-project.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

isis-obc-first-project.bin: isis-obc-first-project.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary "isis-obc-first-project.elf"  "isis-obc-first-project.bin"
	@echo 'Finished building: $@'
	@echo ' '

isis-obc-first-project.siz: isis-obc-first-project.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley --totals "isis-obc-first-project.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(S_UPPER_DEPS) isis-obc-first-project.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
