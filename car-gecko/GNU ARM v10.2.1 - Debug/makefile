################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include gecko_sdk_3.2.3/platform/service/system/src/subdir.mk
-include gecko_sdk_3.2.3/platform/service/device_init/src/subdir.mk
-include gecko_sdk_3.2.3/platform/emlib/src/subdir.mk
-include gecko_sdk_3.2.3/platform/common/toolchain/src/subdir.mk
-include gecko_sdk_3.2.3/platform/Device/SiliconLabs/EFM32HG/Source/GCC/subdir.mk
-include gecko_sdk_3.2.3/platform/Device/SiliconLabs/EFM32HG/Source/subdir.mk
-include gecko_sdk_3.2.3/hardware/board/src/subdir.mk
-include autogen/subdir.mk
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
all: car-gecko.axf

# Tool invocations
car-gecko.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -T "/Users/dafnamargalit/Documents/GitHub/Car-MCU/car-gecko/autogen/linkerfile.ld" -Xlinker --gc-sections -Xlinker -Map="car-gecko.map" --specs=nano.specs -o car-gecko.axf -Wl,--start-group "./app.o" "./main.o" "./autogen/sl_board_default_init.o" "./autogen/sl_device_init_clocks.o" "./autogen/sl_event_handler.o" "./gecko_sdk_3.2.3/hardware/board/src/sl_board_control_gpio.o" "./gecko_sdk_3.2.3/hardware/board/src/sl_board_init.o" "./gecko_sdk_3.2.3/platform/Device/SiliconLabs/EFM32HG/Source/GCC/startup_efm32hg.o" "./gecko_sdk_3.2.3/platform/Device/SiliconLabs/EFM32HG/Source/system_efm32hg.o" "./gecko_sdk_3.2.3/platform/common/toolchain/src/sl_memory.o" "./gecko_sdk_3.2.3/platform/emlib/src/em_assert.o" "./gecko_sdk_3.2.3/platform/emlib/src/em_cmu.o" "./gecko_sdk_3.2.3/platform/emlib/src/em_core.o" "./gecko_sdk_3.2.3/platform/emlib/src/em_emu.o" "./gecko_sdk_3.2.3/platform/emlib/src/em_gpio.o" "./gecko_sdk_3.2.3/platform/emlib/src/em_system.o" "./gecko_sdk_3.2.3/platform/service/device_init/src/sl_device_init_emu_s0.o" "./gecko_sdk_3.2.3/platform/service/device_init/src/sl_device_init_hfxo_s0.o" "./gecko_sdk_3.2.3/platform/service/device_init/src/sl_device_init_lfxo_s0.o" "./gecko_sdk_3.2.3/platform/service/device_init/src/sl_device_init_nvic.o" "./gecko_sdk_3.2.3/platform/service/system/src/sl_system_init.o" "./gecko_sdk_3.2.3/platform/service/system/src/sl_system_process_action.o" -lgcc -lc -lm -lnosys -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: car-gecko.hex'
	arm-none-eabi-objcopy -O ihex "car-gecko.axf" "car-gecko.hex"
	@echo ' '

	@echo 'Building bin file: car-gecko.bin'
	arm-none-eabi-objcopy -O binary "car-gecko.axf" "car-gecko.bin"
	@echo ' '

	@echo 'Building s37 file: car-gecko.s37'
	arm-none-eabi-objcopy -O srec "car-gecko.axf" "car-gecko.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "car-gecko.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) car-gecko.axf
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
