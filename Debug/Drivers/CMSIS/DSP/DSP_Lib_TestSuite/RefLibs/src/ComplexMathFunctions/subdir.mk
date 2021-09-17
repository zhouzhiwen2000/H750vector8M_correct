################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_conj.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_dot_prod.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mag.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mag_squared.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mult_cmplx.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mult_real.c 

OBJS += \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_conj.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_dot_prod.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mag.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mag_squared.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mult_cmplx.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mult_real.o 

C_DEPS += \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_conj.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_dot_prod.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mag.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mag_squared.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mult_cmplx.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/cmplx_mult_real.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/%.o: ../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/%.c Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/ComplexMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32H750xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I"C:/Users/zhouz/Documents/GitHub/H750vector8M_correct/Hardware" -I"C:/Users/zhouz/Documents/GitHub/H750vector8M_correct/ROS_Layer/Inc" -I"C:/Users/zhouz/Documents/GitHub/H750vector8M_correct/Control" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

