################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include CMSIS/EFM32PG12B/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: SLSTK3402A_EFM32PG12B_leuart_echo.axf

# Tool invocations
SLSTK3402A_EFM32PG12B_leuart_echo.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "SLSTK3402A_EFM32PG12B_leuart_echo.ld" -Xlinker --gc-sections -Xlinker -Map="SLSTK3402A_EFM32PG12B_leuart_echo.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o SLSTK3402A_EFM32PG12B_leuart_echo.axf "./CMSIS/EFM32PG12B/startup_efm32pg12b.o" "./CMSIS/EFM32PG12B/system_efm32pg12b.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_leuart.o" "./emlib/em_system.o" "./src/main_pg12.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: SLSTK3402A_EFM32PG12B_leuart_echo.hex'
	arm-none-eabi-objcopy -O ihex "SLSTK3402A_EFM32PG12B_leuart_echo.axf" "SLSTK3402A_EFM32PG12B_leuart_echo.hex"
	@echo ' '

	@echo 'Building bin file: SLSTK3402A_EFM32PG12B_leuart_echo.bin'
	arm-none-eabi-objcopy -O binary "SLSTK3402A_EFM32PG12B_leuart_echo.axf" "SLSTK3402A_EFM32PG12B_leuart_echo.bin"
	@echo ' '

	@echo 'Building s37 file: SLSTK3402A_EFM32PG12B_leuart_echo.s37'
	arm-none-eabi-objcopy -O srec "SLSTK3402A_EFM32PG12B_leuart_echo.axf" "SLSTK3402A_EFM32PG12B_leuart_echo.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "SLSTK3402A_EFM32PG12B_leuart_echo.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) SLSTK3402A_EFM32PG12B_leuart_echo.axf
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
