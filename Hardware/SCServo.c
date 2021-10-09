/*
 * SCServo.cpp
 * 硬件通信接口
 * 日期: 2016.11.11
 * 作者: 谭雄乐
 */

#include "SCServo.h"
#include "usart.h"
uint32_t IOTimeOut = 2;//输入输出超时


//UART 发送数据接口
int writeSCS(unsigned char *nDat, int nLen)
{
	HAL_UART_Transmit(&huart5,nDat,nLen,0xffff);
	return 0;
}
