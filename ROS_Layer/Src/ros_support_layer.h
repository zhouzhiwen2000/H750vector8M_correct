#ifndef ROS_LAYER_H
#define ROS_LAYER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "usart.h"
void setup(void);
void loop(void);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
#ifdef __cplusplus
}
#endif

#endif