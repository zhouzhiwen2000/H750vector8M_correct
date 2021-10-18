/*
 * SCSCL.c
 * 飞特SCSCL系列串行舵机应用层程序
 * 日期: 2019.7.31
 * 作者: 
 */

#include <string.h>
#include "INST.h"
#include "SCS.h"
#include "SCSCL.h"

int WritePosSCS(uint8_t ID, uint16_t Position, uint16_t Time, uint16_t Speed)
{
	uint8_t bBuf[6];
	Host2SCS(bBuf+0, bBuf+1, Position);
	Host2SCS(bBuf+2, bBuf+3, Time);
	Host2SCS(bBuf+4, bBuf+5, Speed);
	
	return genWrite(ID, SCSCL_GOAL_POSITION_L, bBuf, 6);
}
