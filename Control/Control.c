//#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "Control.h"
#include <math.h>
/*圈数和编码器数据对应关系：转一圈对应编码器数据改变32256，即编码器输出32256脉冲*/
/*距离单位：脉冲，即1/32256.0圈*/
/*速度单位：脉冲/5ms，即200*脉冲/s,0.0062003968253968圈/s*/
/*轮子直径6cm,周长18.849555555cm。*/
/*故距离单位：1/32256.0*18.849555555cm,速度单位：0.1168747240823413cm/s*/
u8 delay_50,delay_flag;                         //延时相关变量
u8 Run_Flag=0;  																//运行状态标志位 0为速度模式，1为位置模式
int Encoder_A,Encoder_B,Encoder_C,Encoder_D;          //编码器的脉冲计数
long int Position_A,Position_B,Position_C,Position_D,Rate_A,Rate_B,Rate_C,Rate_D; //PID控制相关变量
long int Motor_A,Motor_B,Motor_C,Motor_D;        //电机PWM变量
long int Target_A,Target_B,Target_C,Target_D;     //电机目标值
//u16 PID_Parameter[10],Flash_Parameter[10];  //Flash相关数组
float	Position_KP=5,Position_KI=0,Position_KD=25;  //位置控制PID参数
float Velocity_KP=5,Velocity_KI=5              ;	          //速度控制PID参数
double RC_Velocity=140;                             //位置模式速度，单位和目标速度单位相同
float Move_X=0,Move_Y=0,Move_Z=0;   //XYZ轴目标速度
uint8_t pending_flag=0;
int flag_clear_i=0;

uint8_t relative=1;
#define a_PARAMETER          (0.6472324f)
#define b_PARAMETER          (0.7622959f)
void Control()
{
//    Move_X=0;
//    Move_Y=5000;
//    Move_Z=0;//necessary
//		Target_A=1;
    if(Run_Flag==1&&relative==1)
    {
        if(pending_flag==1)
        {
            Kinematic_Analysis_Relative(Move_X,Move_Y,Move_Z);
            pending_flag=0;
        }
    }
    else
        Kinematic_Analysis(Move_X,Move_Y,Move_Z);
    if(delay_flag==1)
    {
        if(++delay_50==10)	 delay_50=0,delay_flag=0;                     //给主函数提供50ms的精准延时
    }
    Encoder_A=-Read_Encoder(4);                                         		 //===读取编码器的值
    Position_A+=Encoder_A;                                                 //===积分得到速度
    Encoder_B=-Read_Encoder(5);                                         		 //===读取编码器的值
    Position_B+=Encoder_B;                                                 //===积分得到速度
    Encoder_C=Read_Encoder(3);                                         		//===读取编码器的值
    Position_C+=Encoder_C;                                                 //===积分得到速度
    Encoder_D=Read_Encoder(2);                                         		//===读取编码器的值
    Position_D+=Encoder_D;                                                 //===积分得到速度
//    UART_Control();			//UART提供控制目标量（可包含限速值）
//    if(RC_Velocity>0&&RC_Velocity<15)  RC_Velocity=15;                   //避免电机进入低速非线性区，限速最低15
    if(Run_Flag==0)//速度模式
    {
        Motor_A=Incremental_PI_A(Encoder_A,Target_A);                         //===速度闭环控制计算电机A最终PWM
        Motor_B=Incremental_PI_B(Encoder_B,Target_B);                         //===速度闭环控制计算电机B最终PWM
        Motor_C=Incremental_PI_C(Encoder_C,Target_C);                         //===速度闭环控制计算电机C最终PWM
        Motor_D=Incremental_PI_D(Encoder_D,Target_D);                         //===速度闭环控制计算电机C最终PWM
    }
    else//位置模式
    {
        Motor_A=Position_PID_A(Position_A,Target_A)>>8;//位置闭环控制，计算电机A速度内环的输入量
        Motor_B=Position_PID_B(Position_B,Target_B)>>8;//位置闭环控制，计算电机B速度内环的输入量
        Motor_C=Position_PID_C(Position_C,Target_C)>>8;//位置闭环控制，计算电机C速度内环的输入量
        Motor_D=Position_PID_D(Position_D,Target_D)>>8;//位置闭环控制，计算电机D速度内环的输入量
        Count_Velocity();   //针对行进方向限速，能限制总速度不超过RC_Velocity
        Motor_A=Incremental_PI_A(Encoder_A,-Motor_A);         //===速度闭环控制计算电机A最终PWM
        Motor_B=Incremental_PI_B(Encoder_B,-Motor_B);         //===速度闭环控制计算电机B最终PWM
        Motor_C=Incremental_PI_C(Encoder_C,-Motor_C);         //===速度闭环控制计算电机C最终PWM
        Motor_D=Incremental_PI_D(Encoder_D,-Motor_D);         //===速度闭环控制计算电机D最终PWM
    }
    Xianfu_Pwm(8400);                 //===PWM限幅
    Set_Pwm(Motor_A,Motor_B,Motor_C,Motor_D);     //===赋值给PWM寄存器
}

/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：速度值
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
    int Encoder_TIM;
    switch(TIMX)
    {
    case 2:
        Encoder_TIM= (int)(__HAL_TIM_GET_COUNTER(&htim2));
        __HAL_TIM_GET_COUNTER(&htim2)=0;
        break;
    case 3:
        Encoder_TIM= (short)(__HAL_TIM_GET_COUNTER(&htim3));
        __HAL_TIM_GET_COUNTER(&htim3)=0;
        break;
    case 4:
        Encoder_TIM= (short)(__HAL_TIM_GET_COUNTER(&htim4));
        __HAL_TIM_GET_COUNTER(&htim4)=0;
        break;
    case 5:
        Encoder_TIM= (int)(__HAL_TIM_GET_COUNTER(&htim5));
        __HAL_TIM_GET_COUNTER(&htim5)=0;
        break;
    default:
        Encoder_TIM=0;
    }
    return Encoder_TIM;
}


/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d)
{
    if(motor_d>0)//正转
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,motor_d);
        HAL_GPIO_WritePin(D1AIN1_GPIO_Port,D1AIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D1AIN2_GPIO_Port,D1AIN2_Pin,GPIO_PIN_SET);
    }
    else//Inverse转
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,-motor_d);
        HAL_GPIO_WritePin(D1AIN1_GPIO_Port,D1AIN1_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(D1AIN2_GPIO_Port,D1AIN2_Pin,GPIO_PIN_RESET);
    }

    if(motor_a>0)//正转
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,motor_a);
        HAL_GPIO_WritePin(D1BIN1_GPIO_Port,D1BIN1_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(D1BIN2_GPIO_Port,D1BIN2_Pin,GPIO_PIN_RESET);
    }
    else//Inverse转
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,-motor_a);
        HAL_GPIO_WritePin(D1BIN1_GPIO_Port,D1BIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D1BIN2_GPIO_Port,D1BIN2_Pin,GPIO_PIN_SET);
    }

    if(motor_c>0)//正转
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,motor_c);
        HAL_GPIO_WritePin(D2AIN1_GPIO_Port,D2AIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D2AIN2_GPIO_Port,D2AIN2_Pin,GPIO_PIN_SET);
    }
    else//Inverse转
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,-motor_c);
        HAL_GPIO_WritePin(D2AIN1_GPIO_Port,D2AIN1_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(D2AIN2_GPIO_Port,D2AIN2_Pin,GPIO_PIN_RESET);
    }

    if(motor_b>0)//正转
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,motor_b);
        HAL_GPIO_WritePin(D2BIN1_GPIO_Port,D2BIN1_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(D2BIN2_GPIO_Port,D2BIN2_Pin,GPIO_PIN_RESET);
    }
    else//Inverse转
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,-motor_b);
        HAL_GPIO_WritePin(D2BIN1_GPIO_Port,D2BIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D2BIN2_GPIO_Port,D2BIN2_Pin,GPIO_PIN_SET);
    }
}


//void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d)
//{
//    if(motor_c>0)//正转
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,motor_c);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);
//    }
//    else//Inverse转
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,-motor_c);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);
//    }

//    if(motor_b>0)//正转
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,motor_b);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET);
//    }
//    else//Inverse转
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,-motor_b);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
//    }

//    if(motor_d>0)//正转
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,motor_d);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);
//    }
//    else//Inverse转
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,-motor_d);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
//    }

//    if(motor_a>0)//正转
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,motor_a);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
//    }
//    else//Inverse转
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,-motor_a);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
//    }
//}
/**************************************************************************
函数功能：限制PWM赋值
入口参数：幅值
返回  值：无
**************************************************************************/
void Xianfu_Pwm(int amplitude)
{
    if(Motor_A<-amplitude) Motor_A=-amplitude;
    if(Motor_A>amplitude)  Motor_A=amplitude;
    if(Motor_B<-amplitude) Motor_B=-amplitude;
    if(Motor_B>amplitude)  Motor_B=amplitude;
    if(Motor_C<-amplitude) Motor_C=-amplitude;
    if(Motor_C>amplitude)  Motor_C=amplitude;
    if(Motor_D<-amplitude) Motor_D=-amplitude;
    if(Motor_D>amplitude)  Motor_D=amplitude;
}
/**************************************************************************
函数功能：位置PID控制过程中速度的设置
入口参数：无、幅值
返回  值：无
**************************************************************************/
void Xianfu_Velocity(int amplitude_A,int amplitude_B,int amplitude_C,int amplitude_D)
{
    if(Motor_A<-amplitude_A) Motor_A=-amplitude_A;	//位置控制模式中，A电机的运行速度
    if(Motor_A>amplitude_A)  Motor_A=amplitude_A;	  //位置控制模式中，A电机的运行速度
    if(Motor_B<-amplitude_B) Motor_B=-amplitude_B;	//位置控制模式中，B电机的运行速度
    if(Motor_B>amplitude_B)  Motor_B=amplitude_B;		//位置控制模式中，B电机的运行速度
    if(Motor_C<-amplitude_C) Motor_C=-amplitude_C;	//位置控制模式中，C电机的运行速度
    if(Motor_C>amplitude_C)  Motor_C=amplitude_C;		//位置控制模式中，C电机的运行速度
    if(Motor_D<-amplitude_D) Motor_D=-amplitude_D;	//位置控制模式中，C电机的运行速度
    if(Motor_D>amplitude_D)  Motor_D=amplitude_D;		//位置控制模式中，C电机的运行速度
}

/**************************************************************************
函数功能：绝对值函数
入口参数：long int
返回  值：unsigned int
**************************************************************************/
u32 myabs(long int a)
{
    u32 temp;
    if(a<0)  temp=-a;
    else temp=a;
    return temp;
}
/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差
e(k-1)代表上一次的偏差  以此类推
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI_A (int Encoder,int Target)
{
    static int Bias,Pwm,Last_bias;
    Bias=Encoder-Target;                //计算偏差
    Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
    if(Pwm>8400)Pwm=8400;
    if(Pwm<-8400)Pwm=-8400;
    Last_bias=Bias;	                   //保存上一次偏差
    return Pwm;                         //增量输出
}
int Incremental_PI_B (int Encoder,int Target)
{
    static int Bias,Pwm,Last_bias;
    Bias=Encoder-Target;                //计算偏差
    Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
    if(Pwm>8400)Pwm=8400;
    if(Pwm<-8400)Pwm=-8400;
    Last_bias=Bias;	                   //保存上一次偏差
    return Pwm;                         //增量输出
}
int Incremental_PI_C (int Encoder,int Target)
{
    static int Bias,Pwm,Last_bias;
    Bias=Encoder-Target;                                  //计算偏差
    Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
    if(Pwm>8400)Pwm=8400;
    if(Pwm<-8400)Pwm=-8400;
    Last_bias=Bias;	                   //保存上一次偏差
    return Pwm;                         //增量输出
}
int Incremental_PI_D (int Encoder,int Target)
{
    static int Bias,Pwm,Last_bias;
    Bias=Encoder-Target;                                  //计算偏差
    Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
    if(Pwm>8400)Pwm=8400;
    if(Pwm<-8400)Pwm=-8400;
    Last_bias=Bias;	                   //保存上一次偏差
    return Pwm;                         //增量输出
}
/**************************************************************************
函数功能：位置式PID控制器
入口参数：编码器测量位置信息，目标位置
返回  值：电机PWM
根据位置式离散PID公式
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差
e(k-1)代表上一次的偏差
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
pwm代表输出
**************************************************************************/
int Position_PID_A (int Encoder,int Target)
{
    static float Bias,Pwm,Integral_bias,Last_Bias;
    if(flag_clear_i)
    {
        Integral_bias=0;
        flag_clear_i=0;
    }
    Bias=Encoder-Target;                                  //计算偏差
    Integral_bias+=Bias;	                                 //求出偏差的积分
    if(Integral_bias>100000)Integral_bias=10000;
    if(Integral_bias<-100000)Integral_bias=-10000;
    Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
    Last_Bias=Bias;                                       //保存上一次偏差
    return Pwm;                                           //增量输出
}
int Position_PID_B (int Encoder,int Target)
{
    static float Bias,Pwm,Integral_bias,Last_Bias;
    if(flag_clear_i)
    {
        Integral_bias=0;
        flag_clear_i=0;
    }
    Bias=Encoder-Target;                                  //计算偏差
    Integral_bias+=Bias;	                                 //求出偏差的积分
    if(Integral_bias>100000)Integral_bias=10000;
    if(Integral_bias<-100000)Integral_bias=-10000;
    Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
    Last_Bias=Bias;                                       //保存上一次偏差
    return Pwm;                                           //增量输出
}
int Position_PID_C (int Encoder,int Target)
{
    static float Bias,Pwm,Integral_bias,Last_Bias;
    if(flag_clear_i)
    {
        Integral_bias=0;
        flag_clear_i=0;
    }
    Bias=Encoder-Target;                                  //计算偏差
    Integral_bias+=Bias;	                                 //求出偏差的积分
    if(Integral_bias>100000)Integral_bias=10000;
    if(Integral_bias<-100000)Integral_bias=-10000;
    Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
    Last_Bias=Bias;                                       //保存上一次偏差
    return Pwm;                                           //增量输出
}
int Position_PID_D (int Encoder,int Target)
{
    static float Bias,Pwm,Integral_bias,Last_Bias;
    if(flag_clear_i)
    {
        Integral_bias=0;
        flag_clear_i=0;
    }
    Bias=Encoder-Target;                                  //计算偏差
    Integral_bias+=Bias;	                                 //求出偏差的积分
    if(Integral_bias>100000)Integral_bias=10000;
    if(Integral_bias<-100000)Integral_bias=-10000;
    Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
    Last_Bias=Bias;                                       //保存上一次偏差
    return Pwm;                                           //增量输出
}
double Last_Target_X,Last_Target_Y,Last_Target_Z;
/**************************************************************************
函数功能：每个电机位置控制过程速度计算
入口参数：无
返回  值：无
**************************************************************************/
void Count_Velocity(void) //用于控制位置模式下的速度 绝对位置模式下，若在执行完成前改变目标，会出问题。
{
    static double Divider;
    double Bias_X,Bias_Y,Bias_Z;
    if(relative==0)
    {
        Bias_X=(Move_X-Last_Target_X);  //求X轴位移量
        Bias_Y=(Move_Y-Last_Target_Y);	//求Y轴位移量
        Bias_Z=(Move_Z-Last_Target_Z);	//求Z轴位移量
    }
    else
    {
        Bias_X=Move_X;  //求X轴位移量
        Bias_Y=Move_Y;	//求Y轴位移量
        Bias_Z=Move_Z;	//求Z轴位移量
    }

    if(Bias_X!=0||Bias_Y!=0||Bias_Z!=0)Divider=sqrt(Bias_X*Bias_X+Bias_Y*Bias_Y+Bias_Z*Bias_Z);
    if(Bias_X!=0||Bias_Y!=0||Bias_Z!=0) Kinematic_Analysis2(Bias_X,Bias_Y,Bias_Z);

    Xianfu_Velocity(RC_Velocity*myabs(Rate_A)/Divider,RC_Velocity*myabs(Rate_B)/Divider,RC_Velocity*myabs(Rate_C)/Divider,RC_Velocity*myabs(Rate_D)/Divider);//方向限速，能限制总速度不超过RC_Velocity
    /*对Bias_X，Y,Z进行Kinematic_Analysis再除以Bias的模然后乘以RC_Velocity相当于对方向为行进方向（Bias方向），大小为RC_Velocity的矢量进行Kinematic_Analysis）。*/
    if(relative==0)
    {
        Last_Target_X=Move_X;   //保存X轴上一次的位置信息，便于调用
        Last_Target_Y=Move_Y;   //保存Y轴上一次的位置信息，便于调用
        Last_Target_Z=Move_Z;   //保存Z轴上一次的位置信息，便于调用
    }
}

/**************************************************************************
函数功能：小车运动数学模型,用于速度或绝对位置
入口参数：X Y Z 三轴速度或者位置
返回  值：无
**************************************************************************/
void Kinematic_Analysis(float Vx,float Vy,float Vz)
{
    Target_A   = -Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Target_B   = +Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Target_C   = -Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
    Target_D   = +Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
}

/**************************************************************************
函数功能：小车运动数学模型,相对位置型
入口参数：X Y Z 三轴速度或者位置
返回  值：无
**************************************************************************/
void Kinematic_Analysis_Relative(float Vx,float Vy,float Vz)
{
    Target_A   =Position_A-Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Target_B   =Position_B+Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Target_C   =Position_C-Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
    Target_D   =Position_D+Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
}

/**************************************************************************
函数功能：获取位置控制过程速度值
入口参数：X Y Z 三轴位置变化量
返回  值：无
**************************************************************************/
void Kinematic_Analysis2(float Vx,float Vy,float Vz)
{
    Rate_A   = -Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Rate_B   = +Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Rate_C   = -Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
    Rate_D   = +Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
}
void Enable_Motors(bool enable)
{
    if(enable)
    {
        HAL_GPIO_WritePin(D1EN_GPIO_Port,D1EN_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(D2EN_GPIO_Port,D2EN_Pin,GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(D1EN_GPIO_Port,D1EN_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D2EN_GPIO_Port,D2EN_Pin,GPIO_PIN_RESET);
    }
}
void Lock_Motors(u8 id)
{
    Enable_Motors(true);
    switch(id)
    {
    case 1: {
        HAL_GPIO_WritePin(D1AIN1_GPIO_Port,D1AIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D1AIN2_GPIO_Port,D1AIN2_Pin,GPIO_PIN_RESET);
    }
    case 2: {
        HAL_GPIO_WritePin(D1BIN1_GPIO_Port,D1BIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D1BIN2_GPIO_Port,D1BIN2_Pin,GPIO_PIN_RESET);
    }
    case 3: {
        HAL_GPIO_WritePin(D2AIN1_GPIO_Port,D2AIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D2AIN2_GPIO_Port,D2AIN2_Pin,GPIO_PIN_RESET);
    }
    case 4: {
        HAL_GPIO_WritePin(D2BIN1_GPIO_Port,D2BIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D2BIN2_GPIO_Port,D2BIN2_Pin,GPIO_PIN_RESET);
    }
    }

}