/*
 * SCServo.cpp
 * Ӳ��ͨ�Žӿ�
 * ����: 2016.11.11
 * ����: ̷����
 */

#include "SCServo.h"
#include "usart.h"
uint32_t IOTimeOut = 2;//���������ʱ


//UART �������ݽӿ�
int writeSCS(unsigned char *nDat, int nLen)
{
	HAL_UART_Transmit(&huart5,nDat,nLen,0xffff);
	return 0;
}
