################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: arm
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/VL53L0X/vl53l0x_api.c \
../Drivers/VL53L0X/vl53l0x_api_calibration.c \
../Drivers/VL53L0X/vl53l0x_api_core.c \
../Drivers/VL53L0X/vl53l0x_api_ranging.c \
../Drivers/VL53L0X/vl53l0x_api_strings.c \
../Drivers/VL53L0X/vl53l0x_init.c \
../Drivers/VL53L0X/vl53l0x_platform_log.c \
../Drivers/VL53L0X/vl53l0x_tof.c 

C_DEPS += \
./Drivers/VL53L0X/vl53l0x_api.d \
./Drivers/VL53L0X/vl53l0x_api_calibration.d \
./Drivers/VL53L0X/vl53l0x_api_core.d \
./Drivers/VL53L0X/vl53l0x_api_ranging.d \
./Drivers/VL53L0X/vl53l0x_api_strings.d \
./Drivers/VL53L0X/vl53l0x_init.d \
./Drivers/VL53L0X/vl53l0x_platform_log.d \
./Drivers/VL53L0X/vl53l0x_tof.d 

OBJS += \
./Drivers/VL53L0X/vl53l0x_api.o \
./Drivers/VL53L0X/vl53l0x_api_calibration.o \
./Drivers/VL53L0X/vl53l0x_api_core.o \
./Drivers/VL53L0X/vl53l0x_api_ranging.o \
./Drivers/VL53L0X/vl53l0x_api_strings.o \
./Drivers/VL53L0X/vl53l0x_init.o \
./Drivers/VL53L0X/vl53l0x_platform_log.o \
./Drivers/VL53L0X/vl53l0x_tof.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/VL53L0X/%.o Drivers/VL53L0X/%.su Drivers/VL53L0X/%.cyclo: ../Drivers/VL53L0X/%.c Drivers/VL53L0X/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu18 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/VL53L0X -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-VL53L0X

clean-Drivers-2f-VL53L0X:
	-$(RM) ./Drivers/VL53L0X/vl53l0x_api.cyclo ./Drivers/VL53L0X/vl53l0x_api.d ./Drivers/VL53L0X/vl53l0x_api.o ./Drivers/VL53L0X/vl53l0x_api.su ./Drivers/VL53L0X/vl53l0x_api_calibration.cyclo ./Drivers/VL53L0X/vl53l0x_api_calibration.d ./Drivers/VL53L0X/vl53l0x_api_calibration.o ./Drivers/VL53L0X/vl53l0x_api_calibration.su ./Drivers/VL53L0X/vl53l0x_api_core.cyclo ./Drivers/VL53L0X/vl53l0x_api_core.d ./Drivers/VL53L0X/vl53l0x_api_core.o ./Drivers/VL53L0X/vl53l0x_api_core.su ./Drivers/VL53L0X/vl53l0x_api_ranging.cyclo ./Drivers/VL53L0X/vl53l0x_api_ranging.d ./Drivers/VL53L0X/vl53l0x_api_ranging.o ./Drivers/VL53L0X/vl53l0x_api_ranging.su ./Drivers/VL53L0X/vl53l0x_api_strings.cyclo ./Drivers/VL53L0X/vl53l0x_api_strings.d ./Drivers/VL53L0X/vl53l0x_api_strings.o ./Drivers/VL53L0X/vl53l0x_api_strings.su ./Drivers/VL53L0X/vl53l0x_init.cyclo ./Drivers/VL53L0X/vl53l0x_init.d ./Drivers/VL53L0X/vl53l0x_init.o ./Drivers/VL53L0X/vl53l0x_init.su ./Drivers/VL53L0X/vl53l0x_platform_log.cyclo ./Drivers/VL53L0X/vl53l0x_platform_log.d ./Drivers/VL53L0X/vl53l0x_platform_log.o ./Drivers/VL53L0X/vl53l0x_platform_log.su ./Drivers/VL53L0X/vl53l0x_tof.cyclo ./Drivers/VL53L0X/vl53l0x_tof.d ./Drivers/VL53L0X/vl53l0x_tof.o ./Drivers/VL53L0X/vl53l0x_tof.su

.PHONY: clean-Drivers-2f-VL53L0X

