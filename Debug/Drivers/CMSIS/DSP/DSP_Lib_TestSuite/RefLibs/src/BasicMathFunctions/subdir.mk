################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/abs.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/add.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/dot_prod.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/mult.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/negate.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/offset.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/scale.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/shift.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/sub.c 

OBJS += \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/abs.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/add.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/dot_prod.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/mult.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/negate.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/offset.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/scale.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/shift.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/sub.o 

C_DEPS += \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/abs.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/add.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/dot_prod.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/mult.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/negate.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/offset.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/scale.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/shift.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/sub.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/%.o: ../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/%.c Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32H750xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I"C:/Users/zhouz/Documents/GitHub/H750vector8M_correct/Hardware" -I"C:/Users/zhouz/Documents/GitHub/H750vector8M_correct/ROS_Layer/Inc" -I"C:/Users/zhouz/Documents/GitHub/H750vector8M_correct/Control" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

