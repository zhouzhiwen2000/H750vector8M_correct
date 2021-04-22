//#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "Control.h"
#include <math.h>
/*Ȧ���ͱ��������ݶ�Ӧ��ϵ��תһȦ��Ӧ���������ݸı�32256�������������32256����*/
/*���뵥λ�����壬��1/32256.0Ȧ*/
/*�ٶȵ�λ������/5ms����200*����/s,0.0062003968253968Ȧ/s*/
/*����ֱ��6cm,�ܳ�18.849555555cm��*/
/*�ʾ��뵥λ��1/32256.0*18.849555555cm,�ٶȵ�λ��0.1168747240823413cm/s*/
u8 delay_50,delay_flag;                         //��ʱ��ر���
u8 Run_Flag=0;  																//����״̬��־λ 0Ϊ�ٶ�ģʽ��1Ϊλ��ģʽ
int Encoder_A,Encoder_B,Encoder_C,Encoder_D;          //���������������
long int Position_A,Position_B,Position_C,Position_D,Rate_A,Rate_B,Rate_C,Rate_D; //PID������ر���
long int Motor_A,Motor_B,Motor_C,Motor_D;        //���PWM����
long int Target_A,Target_B,Target_C,Target_D;     //���Ŀ��ֵ
//u16 PID_Parameter[10],Flash_Parameter[10];  //Flash�������
float	Position_KP=5,Position_KI=0,Position_KD=25;  //λ�ÿ���PID����
float Velocity_KP=5,Velocity_KI=5              ;	          //�ٶȿ���PID����
double RC_Velocity=140;                             //λ��ģʽ�ٶȣ���λ��Ŀ���ٶȵ�λ��ͬ
float Move_X=0,Move_Y=0,Move_Z=0;   //XYZ��Ŀ���ٶ�
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
        if(++delay_50==10)	 delay_50=0,delay_flag=0;                     //���������ṩ50ms�ľ�׼��ʱ
    }
    Encoder_A=-Read_Encoder(4);                                         		 //===��ȡ��������ֵ
    Position_A+=Encoder_A;                                                 //===���ֵõ��ٶ�
    Encoder_B=-Read_Encoder(5);                                         		 //===��ȡ��������ֵ
    Position_B+=Encoder_B;                                                 //===���ֵõ��ٶ�
    Encoder_C=Read_Encoder(3);                                         		//===��ȡ��������ֵ
    Position_C+=Encoder_C;                                                 //===���ֵõ��ٶ�
    Encoder_D=Read_Encoder(2);                                         		//===��ȡ��������ֵ
    Position_D+=Encoder_D;                                                 //===���ֵõ��ٶ�
//    UART_Control();			//UART�ṩ����Ŀ�������ɰ�������ֵ��
//    if(RC_Velocity>0&&RC_Velocity<15)  RC_Velocity=15;                   //������������ٷ����������������15
    if(Run_Flag==0)//�ٶ�ģʽ
    {
        Motor_A=Incremental_PI_A(Encoder_A,Target_A);                         //===�ٶȱջ����Ƽ�����A����PWM
        Motor_B=Incremental_PI_B(Encoder_B,Target_B);                         //===�ٶȱջ����Ƽ�����B����PWM
        Motor_C=Incremental_PI_C(Encoder_C,Target_C);                         //===�ٶȱջ����Ƽ�����C����PWM
        Motor_D=Incremental_PI_D(Encoder_D,Target_D);                         //===�ٶȱջ����Ƽ�����C����PWM
    }
    else//λ��ģʽ
    {
        Motor_A=Position_PID_A(Position_A,Target_A)>>8;//λ�ñջ����ƣ�������A�ٶ��ڻ���������
        Motor_B=Position_PID_B(Position_B,Target_B)>>8;//λ�ñջ����ƣ�������B�ٶ��ڻ���������
        Motor_C=Position_PID_C(Position_C,Target_C)>>8;//λ�ñջ����ƣ�������C�ٶ��ڻ���������
        Motor_D=Position_PID_D(Position_D,Target_D)>>8;//λ�ñջ����ƣ�������D�ٶ��ڻ���������
        Count_Velocity();   //����н��������٣����������ٶȲ�����RC_Velocity
        Motor_A=Incremental_PI_A(Encoder_A,-Motor_A);         //===�ٶȱջ����Ƽ�����A����PWM
        Motor_B=Incremental_PI_B(Encoder_B,-Motor_B);         //===�ٶȱջ����Ƽ�����B����PWM
        Motor_C=Incremental_PI_C(Encoder_C,-Motor_C);         //===�ٶȱջ����Ƽ�����C����PWM
        Motor_D=Incremental_PI_D(Encoder_D,-Motor_D);         //===�ٶȱջ����Ƽ�����D����PWM
    }
    Xianfu_Pwm(8400);                 //===PWM�޷�
    Set_Pwm(Motor_A,Motor_B,Motor_C,Motor_D);     //===��ֵ��PWM�Ĵ���
}

/**************************************************************************
�������ܣ���λʱ���ȡ����������
��ڲ�������ʱ��
����  ֵ���ٶ�ֵ
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
�������ܣ���ֵ��PWM�Ĵ���
��ڲ�����PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d)
{
    if(motor_d>0)//��ת
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,motor_d);
        HAL_GPIO_WritePin(D1AIN1_GPIO_Port,D1AIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D1AIN2_GPIO_Port,D1AIN2_Pin,GPIO_PIN_SET);
    }
    else//Inverseת
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,-motor_d);
        HAL_GPIO_WritePin(D1AIN1_GPIO_Port,D1AIN1_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(D1AIN2_GPIO_Port,D1AIN2_Pin,GPIO_PIN_RESET);
    }

    if(motor_a>0)//��ת
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,motor_a);
        HAL_GPIO_WritePin(D1BIN1_GPIO_Port,D1BIN1_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(D1BIN2_GPIO_Port,D1BIN2_Pin,GPIO_PIN_RESET);
    }
    else//Inverseת
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,-motor_a);
        HAL_GPIO_WritePin(D1BIN1_GPIO_Port,D1BIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D1BIN2_GPIO_Port,D1BIN2_Pin,GPIO_PIN_SET);
    }

    if(motor_c>0)//��ת
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,motor_c);
        HAL_GPIO_WritePin(D2AIN1_GPIO_Port,D2AIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D2AIN2_GPIO_Port,D2AIN2_Pin,GPIO_PIN_SET);
    }
    else//Inverseת
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,-motor_c);
        HAL_GPIO_WritePin(D2AIN1_GPIO_Port,D2AIN1_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(D2AIN2_GPIO_Port,D2AIN2_Pin,GPIO_PIN_RESET);
    }

    if(motor_b>0)//��ת
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,motor_b);
        HAL_GPIO_WritePin(D2BIN1_GPIO_Port,D2BIN1_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(D2BIN2_GPIO_Port,D2BIN2_Pin,GPIO_PIN_RESET);
    }
    else//Inverseת
    {
        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,-motor_b);
        HAL_GPIO_WritePin(D2BIN1_GPIO_Port,D2BIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(D2BIN2_GPIO_Port,D2BIN2_Pin,GPIO_PIN_SET);
    }
}


//void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d)
//{
//    if(motor_c>0)//��ת
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,motor_c);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);
//    }
//    else//Inverseת
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,-motor_c);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);
//    }

//    if(motor_b>0)//��ת
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,motor_b);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET);
//    }
//    else//Inverseת
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,-motor_b);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
//    }

//    if(motor_d>0)//��ת
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,motor_d);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);
//    }
//    else//Inverseת
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,-motor_d);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
//    }

//    if(motor_a>0)//��ת
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,motor_a);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
//    }
//    else//Inverseת
//    {
//        __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,-motor_a);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
//    }
//}
/**************************************************************************
�������ܣ�����PWM��ֵ
��ڲ�������ֵ
����  ֵ����
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
�������ܣ�λ��PID���ƹ������ٶȵ�����
��ڲ������ޡ���ֵ
����  ֵ����
**************************************************************************/
void Xianfu_Velocity(int amplitude_A,int amplitude_B,int amplitude_C,int amplitude_D)
{
    if(Motor_A<-amplitude_A) Motor_A=-amplitude_A;	//λ�ÿ���ģʽ�У�A����������ٶ�
    if(Motor_A>amplitude_A)  Motor_A=amplitude_A;	  //λ�ÿ���ģʽ�У�A����������ٶ�
    if(Motor_B<-amplitude_B) Motor_B=-amplitude_B;	//λ�ÿ���ģʽ�У�B����������ٶ�
    if(Motor_B>amplitude_B)  Motor_B=amplitude_B;		//λ�ÿ���ģʽ�У�B����������ٶ�
    if(Motor_C<-amplitude_C) Motor_C=-amplitude_C;	//λ�ÿ���ģʽ�У�C����������ٶ�
    if(Motor_C>amplitude_C)  Motor_C=amplitude_C;		//λ�ÿ���ģʽ�У�C����������ٶ�
    if(Motor_D<-amplitude_D) Motor_D=-amplitude_D;	//λ�ÿ���ģʽ�У�C����������ٶ�
    if(Motor_D>amplitude_D)  Motor_D=amplitude_D;		//λ�ÿ���ģʽ�У�C����������ٶ�
}

/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����long int
����  ֵ��unsigned int
**************************************************************************/
u32 myabs(long int a)
{
    u32 temp;
    if(a<0)  temp=-a;
    else temp=a;
    return temp;
}
/**************************************************************************
�������ܣ�����PI������
��ڲ���������������ֵ��Ŀ���ٶ�
����  ֵ�����PWM
��������ʽ��ɢPID��ʽ
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)������ƫ��
e(k-1)������һ�ε�ƫ��  �Դ�����
pwm�����������
�����ǵ��ٶȿ��Ʊջ�ϵͳ���棬ֻʹ��PI����
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI_A (int Encoder,int Target)
{
    static int Bias,Pwm,Last_bias;
    Bias=Encoder-Target;                //����ƫ��
    Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //����ʽPI������
    if(Pwm>8400)Pwm=8400;
    if(Pwm<-8400)Pwm=-8400;
    Last_bias=Bias;	                   //������һ��ƫ��
    return Pwm;                         //�������
}
int Incremental_PI_B (int Encoder,int Target)
{
    static int Bias,Pwm,Last_bias;
    Bias=Encoder-Target;                //����ƫ��
    Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //����ʽPI������
    if(Pwm>8400)Pwm=8400;
    if(Pwm<-8400)Pwm=-8400;
    Last_bias=Bias;	                   //������һ��ƫ��
    return Pwm;                         //�������
}
int Incremental_PI_C (int Encoder,int Target)
{
    static int Bias,Pwm,Last_bias;
    Bias=Encoder-Target;                                  //����ƫ��
    Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //����ʽPI������
    if(Pwm>8400)Pwm=8400;
    if(Pwm<-8400)Pwm=-8400;
    Last_bias=Bias;	                   //������һ��ƫ��
    return Pwm;                         //�������
}
int Incremental_PI_D (int Encoder,int Target)
{
    static int Bias,Pwm,Last_bias;
    Bias=Encoder-Target;                                  //����ƫ��
    Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //����ʽPI������
    if(Pwm>8400)Pwm=8400;
    if(Pwm<-8400)Pwm=-8400;
    Last_bias=Bias;	                   //������һ��ƫ��
    return Pwm;                         //�������
}
/**************************************************************************
�������ܣ�λ��ʽPID������
��ڲ���������������λ����Ϣ��Ŀ��λ��
����  ֵ�����PWM
����λ��ʽ��ɢPID��ʽ
pwm=Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]
e(k)������ƫ��
e(k-1)������һ�ε�ƫ��
��e(k)����e(k)�Լ�֮ǰ��ƫ����ۻ���;����kΪ1,2,,k;
pwm�������
**************************************************************************/
int Position_PID_A (int Encoder,int Target)
{
    static float Bias,Pwm,Integral_bias,Last_Bias;
    if(flag_clear_i)
    {
        Integral_bias=0;
        flag_clear_i=0;
    }
    Bias=Encoder-Target;                                  //����ƫ��
    Integral_bias+=Bias;	                                 //���ƫ��Ļ���
    if(Integral_bias>100000)Integral_bias=10000;
    if(Integral_bias<-100000)Integral_bias=-10000;
    Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //λ��ʽPID������
    Last_Bias=Bias;                                       //������һ��ƫ��
    return Pwm;                                           //�������
}
int Position_PID_B (int Encoder,int Target)
{
    static float Bias,Pwm,Integral_bias,Last_Bias;
    if(flag_clear_i)
    {
        Integral_bias=0;
        flag_clear_i=0;
    }
    Bias=Encoder-Target;                                  //����ƫ��
    Integral_bias+=Bias;	                                 //���ƫ��Ļ���
    if(Integral_bias>100000)Integral_bias=10000;
    if(Integral_bias<-100000)Integral_bias=-10000;
    Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //λ��ʽPID������
    Last_Bias=Bias;                                       //������һ��ƫ��
    return Pwm;                                           //�������
}
int Position_PID_C (int Encoder,int Target)
{
    static float Bias,Pwm,Integral_bias,Last_Bias;
    if(flag_clear_i)
    {
        Integral_bias=0;
        flag_clear_i=0;
    }
    Bias=Encoder-Target;                                  //����ƫ��
    Integral_bias+=Bias;	                                 //���ƫ��Ļ���
    if(Integral_bias>100000)Integral_bias=10000;
    if(Integral_bias<-100000)Integral_bias=-10000;
    Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //λ��ʽPID������
    Last_Bias=Bias;                                       //������һ��ƫ��
    return Pwm;                                           //�������
}
int Position_PID_D (int Encoder,int Target)
{
    static float Bias,Pwm,Integral_bias,Last_Bias;
    if(flag_clear_i)
    {
        Integral_bias=0;
        flag_clear_i=0;
    }
    Bias=Encoder-Target;                                  //����ƫ��
    Integral_bias+=Bias;	                                 //���ƫ��Ļ���
    if(Integral_bias>100000)Integral_bias=10000;
    if(Integral_bias<-100000)Integral_bias=-10000;
    Pwm=Position_KP*Bias+Position_KI/100*Integral_bias+Position_KD*(Bias-Last_Bias);       //λ��ʽPID������
    Last_Bias=Bias;                                       //������һ��ƫ��
    return Pwm;                                           //�������
}
double Last_Target_X,Last_Target_Y,Last_Target_Z;
/**************************************************************************
�������ܣ�ÿ�����λ�ÿ��ƹ����ٶȼ���
��ڲ�������
����  ֵ����
**************************************************************************/
void Count_Velocity(void) //���ڿ���λ��ģʽ�µ��ٶ� ����λ��ģʽ�£�����ִ�����ǰ�ı�Ŀ�꣬������⡣
{
    static double Divider;
    double Bias_X,Bias_Y,Bias_Z;
    if(relative==0)
    {
        Bias_X=(Move_X-Last_Target_X);  //��X��λ����
        Bias_Y=(Move_Y-Last_Target_Y);	//��Y��λ����
        Bias_Z=(Move_Z-Last_Target_Z);	//��Z��λ����
    }
    else
    {
        Bias_X=Move_X;  //��X��λ����
        Bias_Y=Move_Y;	//��Y��λ����
        Bias_Z=Move_Z;	//��Z��λ����
    }

    if(Bias_X!=0||Bias_Y!=0||Bias_Z!=0)Divider=sqrt(Bias_X*Bias_X+Bias_Y*Bias_Y+Bias_Z*Bias_Z);
    if(Bias_X!=0||Bias_Y!=0||Bias_Z!=0) Kinematic_Analysis2(Bias_X,Bias_Y,Bias_Z);

    Xianfu_Velocity(RC_Velocity*myabs(Rate_A)/Divider,RC_Velocity*myabs(Rate_B)/Divider,RC_Velocity*myabs(Rate_C)/Divider,RC_Velocity*myabs(Rate_D)/Divider);//�������٣����������ٶȲ�����RC_Velocity
    /*��Bias_X��Y,Z����Kinematic_Analysis�ٳ���Bias��ģȻ�����RC_Velocity�൱�ڶԷ���Ϊ�н�����Bias���򣩣���СΪRC_Velocity��ʸ������Kinematic_Analysis����*/
    if(relative==0)
    {
        Last_Target_X=Move_X;   //����X����һ�ε�λ����Ϣ�����ڵ���
        Last_Target_Y=Move_Y;   //����Y����һ�ε�λ����Ϣ�����ڵ���
        Last_Target_Z=Move_Z;   //����Z����һ�ε�λ����Ϣ�����ڵ���
    }
}

/**************************************************************************
�������ܣ�С���˶���ѧģ��,�����ٶȻ����λ��
��ڲ�����X Y Z �����ٶȻ���λ��
����  ֵ����
**************************************************************************/
void Kinematic_Analysis(float Vx,float Vy,float Vz)
{
    Target_A   = -Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Target_B   = +Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Target_C   = -Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
    Target_D   = +Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
}

/**************************************************************************
�������ܣ�С���˶���ѧģ��,���λ����
��ڲ�����X Y Z �����ٶȻ���λ��
����  ֵ����
**************************************************************************/
void Kinematic_Analysis_Relative(float Vx,float Vy,float Vz)
{
    Target_A   =Position_A-Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Target_B   =Position_B+Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Target_C   =Position_C-Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
    Target_D   =Position_D+Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
}

/**************************************************************************
�������ܣ���ȡλ�ÿ��ƹ����ٶ�ֵ
��ڲ�����X Y Z ����λ�ñ仯��
����  ֵ����
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