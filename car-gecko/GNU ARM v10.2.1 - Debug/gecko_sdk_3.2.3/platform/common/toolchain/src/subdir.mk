################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v3.2/platform/common/toolchain/src/sl_memory.c 

OBJS += \
./gecko_sdk_3.2.3/platform/common/toolchain/src/sl_memory.o 

C_DEPS += \
./gecko_sdk_3.2.3/platform/common/toolchain/src/sl_memory.d 


# Each subdirectory must supply rules for building sources it contributes
gecko_sdk_3.2.3/platform/common/toolchain/src/sl_memory.o: /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v3.2/platform/common/toolchain/src/sl_memory.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32HG322F64=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"/Users/dafnamargalit/Documents/GitHub/Car-MCU/car-gecko/config" -I"/Users/dafnamargalit/Documents/GitHub/Car-MCU/car-gecko/autogen" -I"/Users/dafnamargalit/Documents/GitHub/Car-MCU/car-gecko" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v3.2//platform/Device/SiliconLabs/EFM32HG/Include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v3.2//hardware/board/inc" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v3.2//platform/CMSIS/Include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v3.2//platform/service/device_init/inc" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v3.2//platform/emlib/inc" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v3.2//platform/common/inc" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v3.2//platform/common/toolchain/inc" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v3.2//platform/service/system/inc" -Os -Wall -Wextra -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_3.2.3/platform/common/toolchain/src/sl_memory.d" -MT"gecko_sdk_3.2.3/platform/common/toolchain/src/sl_memory.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


