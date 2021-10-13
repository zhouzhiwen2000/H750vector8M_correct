################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hardware/SCSProtocol.c \
../Hardware/SCServo.c 

CPP_SRCS += \
../Hardware/Screen_drv.cpp \
../Hardware/StepperMotor.cpp 

C_DEPS += \
./Hardware/SCSProtocol.d \
./Hardware/SCServo.d 

OBJS += \
./Hardware/SCSProtocol.o \
./Hardware/SCServo.o \
./Hardware/Screen_drv.o \
./Hardware/StepperMotor.o 

CPP_DEPS += \
./Hardware/Screen_drv.d \
./Hardware/StepperMotor.d 


# Each subdirectory must supply rules for building sources it contributes
Hardware/%.o: ../Hardware/%.c Hardware/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H750xx -DSTM32_THREAD_SAFE_STRATEGY=4 -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I"C:/Users/ZhouZhiwen/Desktop/H750vector8M_correct/H750vector8M_correct/Control" -I"C:/Users/ZhouZhiwen/Desktop/H750vector8M_correct/H750vector8M_correct/Hardware" -I"C:/Users/ZhouZhiwen/Desktop/H750vector8M_correct/H750vector8M_correct/Tuning_Utils" -I"C:/Users/ZhouZhiwen/Desktop/H750vector8M_correct/H750vector8M_correct/ROS2_Layer/libmicroros/microros_include" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Hardware/%.o: ../Hardware/%.cpp Hardware/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H750xx -DSTM32_THREAD_SAFE_STRATEGY=4 -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I"C:/Users/ZhouZhiwen/Desktop/H750vector8M_correct/H750vector8M_correct/Control" -I"C:/Users/ZhouZhiwen/Desktop/H750vector8M_correct/H750vector8M_correct/Hardware" -I"C:/Users/ZhouZhiwen/Desktop/H750vector8M_correct/H750vector8M_correct/Tuning_Utils" -I"C:/Users/ZhouZhiwen/Desktop/H750vector8M_correct/H750vector8M_correct/ROS2_Layer/libmicroros/microros_include" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

