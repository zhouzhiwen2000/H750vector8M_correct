#include "Screen_drv.h"
#include "usart.h"
extern "C" {//functions&variables imported from C

}

void Screen_printString(std::string str)
{
	uint8_t EOL []={0xff,0xff,0xff};
	HAL_UART_Transmit(&huart7,(uint8_t *)"t0.txt=\"",8,0xffff);
	
	HAL_UART_Transmit(&huart7,(uint8_t *)str.c_str(),str.length(),0xffff);
	
	HAL_UART_Transmit(&huart7,(uint8_t *)"\"",1,0xffff);
	
	HAL_UART_Transmit(&huart7,EOL,3,0xffff);
}