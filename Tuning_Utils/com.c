#include "DataScope_DP.h"
#include "usart.h"
#include "com.h"
extern int Encoder_A,Encoder_B,Encoder_C,Encoder_D;//real speed
extern long int Position_A,Position_B,Position_C,Position_D; //real pos
extern long int Target_A,Target_B,Target_C,Target_D; //target
void DataScope(void)
{   
		unsigned char Send_Count;
		DataScope_Get_Channel_Data(Position_A, 1 );       //显示目标值
		DataScope_Get_Channel_Data(Position_B, 2 );       //显示实际值，便于PID参数调节  
		DataScope_Get_Channel_Data(Position_C, 3 );                
		DataScope_Get_Channel_Data(Position_D, 4 );
		DataScope_Get_Channel_Data(Target_A, 5 );       //显示目标值
		DataScope_Get_Channel_Data(Target_B, 6 );       //显示实际值，便于PID参数调节  
		DataScope_Get_Channel_Data(Target_C, 7 );                
		DataScope_Get_Channel_Data(Target_D, 8 );	
	
//		DataScope_Get_Channel_Data(0, 9 );  
//		DataScope_Get_Channel_Data( 0 , 10);
		Send_Count = DataScope_Data_Generate(8);
		HAL_UART_Transmit(&huart1,DataScope_OutPut_Buffer,Send_Count,0xffff);
}