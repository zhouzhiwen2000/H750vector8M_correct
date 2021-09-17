################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_conj_tests.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_dot_prod_tests.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mag_squared_tests.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mag_tests.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mult_cmplx_tests.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mult_real_test.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/complex_math_test_common_data.c \
../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/complex_math_test_group.c 

OBJS += \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_conj_tests.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_dot_prod_tests.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mag_squared_tests.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mag_tests.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mult_cmplx_tests.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mult_real_test.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/complex_math_test_common_data.o \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/complex_math_test_group.o 

C_DEPS += \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_conj_tests.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_dot_prod_tests.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mag_squared_tests.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mag_tests.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mult_cmplx_tests.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/cmplx_mult_real_test.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/complex_math_test_common_data.d \
./Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/complex_math_test_group.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/%.o: ../Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/%.c Drivers/CMSIS/DSP/DSP_Lib_TestSuite/Common/src/complex_math_tests/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32H750xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I"C:/Users/zhouz/Documents/GitHub/H750vector8M_correct/Hardware" -I"C:/Users/zhouz/Documents/GitHub/H750vector8M_correct/ROS_Layer/Inc" -I"C:/Users/zhouz/Documents/GitHub/H750vector8M_correct/Control" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

