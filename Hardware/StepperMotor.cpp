#include "StepperMotor.h"
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

extern "C" {//functions&variables imported from C

}
using namespace std;
int current_speed=10;//1 for 0.1ms per halfstep
int current_halfstep_left=0;
int current_halfstep_total=0;
int clear_count=0;
bool clear_flag=false;


void stepper_ISR(void)
{
    static uint32_t counter=0;
    static bool state=false;
    if(++counter>=current_speed)
    {
        if(current_halfstep_left!=0)
        {
            if(current_halfstep_left>0)
            {
                HAL_GPIO_WritePin(DIR_GPIO_Port,DIR_Pin,GPIO_PIN_SET);
                current_halfstep_left--;
                current_halfstep_total++;
            }
            else
            {
                HAL_GPIO_WritePin(DIR_GPIO_Port,DIR_Pin,GPIO_PIN_RESET);
                current_halfstep_left++;
                current_halfstep_total--;
            }
            HAL_GPIO_WritePin(STEP_GPIO_Port,STEP_Pin,(GPIO_PinState)state);
            state=!state;
        }
        counter=0;
    }
    if(clear_flag)
    {
        clear_count++;
        if(clear_count>=1)
        {
            if(HAL_GPIO_ReadPin(SW_GPIO_Port,SW_Pin)!=0)
            {
                current_halfstep_left=0;
                current_halfstep_total=0;
                clear_count=0;
                clear_flag=false;
                set_stepper(450);
            }
            else
            {
                clear_count=0;
                clear_flag=false;
            }
        }
    }
}

void set_stepper(int steps)
{
    current_halfstep_left=(2*steps-current_halfstep_total);
}

void set_speed(int speed)
{
    current_speed=speed;
}

int get_speed(void)
{
    return current_speed;
}

int get_steps(void)
{
    return current_halfstep_total/2;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == SW_Pin)
    {
        clear_flag=true;
    }
}