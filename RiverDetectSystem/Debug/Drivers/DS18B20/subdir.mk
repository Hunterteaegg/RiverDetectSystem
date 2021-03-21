################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/DS18B20/ds18b20.c 

OBJS += \
./Drivers/DS18B20/ds18b20.o 

C_DEPS += \
./Drivers/DS18B20/ds18b20.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/DS18B20/ds18b20.o: ../Drivers/DS18B20/ds18b20.c Drivers/DS18B20/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F103xB -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"E:/MyCode/RiverDetectSystem/RiverDetectSystem/Drivers/ESP8266" -I"E:/MyCode/RiverDetectSystem/RiverDetectSystem/Drivers/DS18B20" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DS18B20/ds18b20.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

