################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/sx126x_driver/src/lr_fhss_mac.c \
../Core/Src/sx126x_driver/src/sx126x.c \
../Core/Src/sx126x_driver/src/sx126x_hal.c \
../Core/Src/sx126x_driver/src/sx126x_lr_fhss.c 

OBJS += \
./Core/Src/sx126x_driver/src/lr_fhss_mac.o \
./Core/Src/sx126x_driver/src/sx126x.o \
./Core/Src/sx126x_driver/src/sx126x_hal.o \
./Core/Src/sx126x_driver/src/sx126x_lr_fhss.o 

C_DEPS += \
./Core/Src/sx126x_driver/src/lr_fhss_mac.d \
./Core/Src/sx126x_driver/src/sx126x.d \
./Core/Src/sx126x_driver/src/sx126x_hal.d \
./Core/Src/sx126x_driver/src/sx126x_lr_fhss.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/sx126x_driver/src/%.o Core/Src/sx126x_driver/src/%.su: ../Core/Src/sx126x_driver/src/%.c Core/Src/sx126x_driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L071xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-sx126x_driver-2f-src

clean-Core-2f-Src-2f-sx126x_driver-2f-src:
	-$(RM) ./Core/Src/sx126x_driver/src/lr_fhss_mac.d ./Core/Src/sx126x_driver/src/lr_fhss_mac.o ./Core/Src/sx126x_driver/src/lr_fhss_mac.su ./Core/Src/sx126x_driver/src/sx126x.d ./Core/Src/sx126x_driver/src/sx126x.o ./Core/Src/sx126x_driver/src/sx126x.su ./Core/Src/sx126x_driver/src/sx126x_hal.d ./Core/Src/sx126x_driver/src/sx126x_hal.o ./Core/Src/sx126x_driver/src/sx126x_hal.su ./Core/Src/sx126x_driver/src/sx126x_lr_fhss.d ./Core/Src/sx126x_driver/src/sx126x_lr_fhss.o ./Core/Src/sx126x_driver/src/sx126x_lr_fhss.su

.PHONY: clean-Core-2f-Src-2f-sx126x_driver-2f-src

