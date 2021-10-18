/*
 * SMSBCL.c
 * 飞特SMS/STS系列串行舵机应用层程序
 * 日期: 2020.11.8
 * 作者: 
 */

#include <string.h>
#include "INST.h"
#include "SCS.h"
#include "SMSBCL.h"


int WritePosSTS(uint8_t ID, int16_t Position, uint16_t Speed, uint8_t ACC)
{
	uint8_t bBuf[7];
	if(Position<0){
		Position = -Position;
		Position |= (1<<15);
	}

	bBuf[0] = ACC;
	Host2SCS(bBuf+1, bBuf+2, Position);
	Host2SCS(bBuf+3, bBuf+4, 0);
	Host2SCS(bBuf+5, bBuf+6, Speed);
	
	return genWrite(ID, SMSBCL_ACC, bBuf, 7);
}
