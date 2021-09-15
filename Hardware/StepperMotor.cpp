#include "StepperMotor.h"
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

extern "C" {//functions&variables imported from C

}
using namespace std;
int current_speed_1=10;//1 for 0.1ms per halfstep
int current_halfstep_left_1=0;
int current_halfstep_total_1=0;
int clear_count_1=0;
bool clear_flag_1=false;
int current_speed_2=10;//1 for 0.1ms per halfstep
int current_halfstep_left_2=0;
int current_halfstep_total_2=0;
int clear_count_2=0;
bool clear_flag_2=false;

void stepper1_ISR(void)
{
    static uint32_t counter=0;
    static bool state=false;
    if(++counter>=current_speed_1)
    {
        if(current_halfstep_left_1!=0)
        {
            if(current_halfstep_left_1>0)
            {
                HAL_GPIO_WritePin(DIR_STEP1_GPIO_Port,DIR_STEP1_Pin,GPIO_PIN_SET);
                current_halfstep_left_1--;
                current_halfstep_total_1++;
            }
            else
            {
                HAL_GPIO_WritePin(DIR_STEP1_GPIO_Port,DIR_STEP1_Pin,GPIO_PIN_RESET);
                current_halfstep_left_1++;
                current_halfstep_total_1--;
            }
            HAL_GPIO_WritePin(STEP_STEP1_GPIO_Port,STEP_STEP1_Pin,(GPIO_PinState)state);
            state=!state;
        }
        counter=0;
    }
    if(clear_flag_1)
    {
        clear_count_1++;
        if(clear_count_1>=1)
        {
            if(HAL_GPIO_ReadPin(SW_STEP1_GPIO_Port,SW_STEP1_Pin)!=0)
            {
                current_halfstep_left_1=0;
                current_halfstep_total_1=0;
                clear_count_1=0;
                clear_flag_1=false;
                set_stepper_1(450);
            }
            else
            {
                clear_count_1=0;
                clear_flag_1=false;
            }
        }
    }
}

void set_stepper_1(int steps)
{
    current_halfstep_left_1=(2*steps-current_halfstep_total_1);
}

void set_speed_1(int speed)
{
    current_speed_1=speed;
}

int get_speed_1(void)
{
    return current_speed_1;
}

int get_steps_1(void)
{
    return current_halfstep_total_1/2;
}


void stepper2_ISR(void)
{
    static uint32_t counter=0;
    static bool state=false;
    if(++counter>=current_speed_2)
    {
        if(current_halfstep_left_2!=0)
        {
            if(current_halfstep_left_2>0)
            {
                HAL_GPIO_WritePin(DIR_STEP2_GPIO_Port,DIR_STEP2_Pin,GPIO_PIN_SET);
                current_halfstep_left_2--;
                current_halfstep_total_2++;
            }
            else
            {
                HAL_GPIO_WritePin(DIR_STEP2_GPIO_Port,DIR_STEP2_Pin,GPIO_PIN_RESET);
                current_halfstep_left_2++;
                current_halfstep_total_2--;
            }
            HAL_GPIO_WritePin(STEP_STEP2_GPIO_Port,STEP_STEP2_Pin,(GPIO_PinState)state);
            state=!state;
        }
        counter=0;
    }
    if(clear_flag_2)
    {
        clear_count_2++;
        if(clear_count_2>=1)
        {
            if(HAL_GPIO_ReadPin(SW_STEP2_GPIO_Port,SW_STEP2_Pin)!=0)
            {
                current_halfstep_left_2=0;
                current_halfstep_total_2=0;
                clear_count_2=0;
                clear_flag_2=false;
                set_stepper_2(450);
            }
            else
            {
                clear_count_2=0;
                clear_flag_2=false;
            }
        }
    }
}

void set_stepper_2(int steps)
{
    current_halfstep_left_2=(2*steps-current_halfstep_total_2);
}

void set_speed_2(int speed)
{
    current_speed_2=speed;
}

int get_speed_2(void)
{
    return current_speed_2;
}

int get_steps_2(void)
{
    return current_halfstep_total_2/2;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == SW_STEP1_Pin)
    {
        clear_flag_1=true;
    }
    if(GPIO_Pin == SW_STEP2_Pin)
    {
        clear_flag_2=true;
    }   
}