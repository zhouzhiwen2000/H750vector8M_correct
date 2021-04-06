#ifndef _CONTROL_H
#define _CONTROL_H
#include <stdbool.h>
typedef		char			s8;
typedef		unsigned char	u8;	
typedef		unsigned short	u16;	
typedef		short			s16;
typedef		unsigned long	u32;	
typedef		long			s32;

void Control();
void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d);
void Xianfu_Pwm(int amplitude);
void Xianfu_Velocity(int amplitude_A,int amplitude_B,int amplitude_C,int amplitude_D);
u32 myabs(long int a);
int Incremental_PI_A (int Encoder,int Target);
int Incremental_PI_B (int Encoder,int Target);
int Incremental_PI_C (int Encoder,int Target);
int Incremental_PI_D (int Encoder,int Target);
int Position_PID_A (int Encoder,int Target);
int Position_PID_B (int Encoder,int Target);
int Position_PID_C (int Encoder,int Target);
int Position_PID_D (int Encoder,int Target);
void Get_RC(u8 mode);
void Count_Velocity(void);
void Kinematic_Analysis(float Vx,float Vy,float Vz);
void Kinematic_Analysis2(float Vx,float Vy,float Vz);
void Kinematic_Analysis_Relative(float Vx,float Vy,float Vz);
int Read_Encoder(u8 TIMX);
void Enable_Motors(bool enable);
void Lock_Motors(u8 id);
#endif