/*
 * SCServo.c
 * 飞特舵机硬件接口层程序
 * 日期: 2020.7.9
 * 作者: 
 */
#include "usart.h"

uint32_t IOTimeOut = 100;//输入输出超时
uint8_t wBuf[128];
uint8_t wLen = 0;

void Uart_Send(uint8_t *buf , uint16_t len)
{
	HAL_UART_Transmit(&huart5, buf, len, HAL_MAX_DELAY);
}

int16_t Uart_Read(uint8_t *buf , uint16_t len, uint32_t timeout)
{
	if(HAL_UART_Receive(&huart5, buf, len, timeout)==HAL_OK){
		return len;
	}else{
		return 0;
	}
}


//UART 接收数据接口
int readSCS(unsigned char *nDat, int nLen)
{
	return Uart_Read(nDat, nLen, IOTimeOut);
}

//UART 发送数据接口
int writeSCS(unsigned char *nDat, int nLen)
{
	while(nLen--){
		if(wLen<sizeof(wBuf)){
			wBuf[wLen] = *nDat;
			wLen++;
			nDat++;
		}
	}
	return wLen;
}

//接收缓冲区刷新
void rFlushSCS()
{
}

//发送缓冲区刷新
void wFlushSCS()
{
	if(wLen){
		Uart_Send(wBuf, wLen);
		wLen = 0;
	}
}

