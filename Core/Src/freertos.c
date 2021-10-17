/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "Control.h"
#include "StepperMotor.h"
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <uxr/client/transport.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/int32.h>

#include "usart.h"
#include "Servo.h"
#include "ros_task.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t controlTaskHandle;
const osThreadAttr_t controlTask_attributes = {
  .name = "controlTask",
  .stack_size = 1000 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
SemaphoreHandle_t Control_Lock = NULL;
osThreadId_t servoTaskHandle;
const osThreadAttr_t servoTask_attributes = {
  .name = "servoTask",
  .stack_size = 1000 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
SemaphoreHandle_t Servo_Lock = NULL;
SemaphoreHandle_t Servo_Lock_Upper = NULL;
osThreadId_t ROSTaskHandle;
const osThreadAttr_t ROSTask_attributes = {
  .name = "ROSTask",
  .stack_size = 1000 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t LEDTaskHandle;
const osThreadAttr_t LEDTask_attributes = {
  .name = "LEDTask",
  .stack_size = 64 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartControlTask(void *argument);
void StartServoTask(void *argument);
void StartROSTask(void *argument);
void StartLEDTask(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  controlTaskHandle = osThreadNew(StartControlTask, NULL, &controlTask_attributes);



  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void StartControlTask(void *argument)
{
  Control_Lock = xSemaphoreCreateMutex();//create lock used for control loop
  servoTaskHandle = osThreadNew(StartServoTask, NULL, &servoTask_attributes);
//  Set_relative(0);
//  Set_Run_Flag(1);
  for(;;)
  {
    TickType_t tcnt=xTaskGetTickCount();
	Control();
    osDelayUntil(tcnt+10);//100Hz
  }
}

void StartServoTask(void *argument)
{
  Servo_Lock = xSemaphoreCreateMutex();//create lock used for control loop
  Servo_Lock_Upper = xSemaphoreCreateRecursiveMutex();
  ROSTaskHandle = osThreadNew(StartROSTask, NULL, &ROSTask_attributes);
  LEDTaskHandle = osThreadNew(StartLEDTask, NULL, &LEDTask_attributes);
  set_stepper_2(0xFFFF);
  set_stepper_1(-0xFFFF);
  Servo_InitPos();

  for(;;)
  {
    TickType_t tcnt=xTaskGetTickCount();
    Servo_Server();
    osDelayUntil(tcnt+50);//20Hz
  }
}

void StartROSTask(void *argument)
{
	ros_init();
	for(;;)
	{
		osDelay(10);
	}
}

void StartLEDTask(void *argument)
{
	static bool ledstate=false;
	while(1)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,ledstate);
		ledstate=!ledstate;
		osDelay(500);
	}
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
