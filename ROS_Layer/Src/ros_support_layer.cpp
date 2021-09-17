#include "ros_support_layer.h"
#include <ros.h>
#include "geometry_msgs/Point.h"
#include "Screen_drv.h"
#include <std_msgs/Float64.h>
#include <std_msgs/UInt8.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include "Servo.h"
#include "StepperMotor.h"
extern "C" {//functions&variables imported from C 
#include "SCSProtocol.h"
}
#define a_PARAMETER          (0.6472324f)
#define b_PARAMETER          (0.7622959f)
#define R 15
extern "C" {//functions&variables exported to and imported from C
	void Set_Move(float x, float y, float z);
	void Set_Run_Flag(unsigned char flag);
	uint8_t Get_Run_Flag();
	void Set_RC_Velocity(double vol);
	void Set_relative(uint8_t flag);
	uint8_t Get_relative();
	void Set_pending_flag(uint8_t flag);
	uint8_t Get_pending_flag();
	void Set_Last_Target(double x, double y, double z);
	void Get_Position_NOW(double *x, double *y, double *z); //Position thread-safe wrapper
	double Get_Error();
}
extern int last_mode_servo;
void callback_pos(const geometry_msgs::Point& msg);
void callback_speed(const geometry_msgs::Point& msg);
void callback_speedlimit(const std_msgs::Float64& msg);
void callback_servo_speed(const geometry_msgs::Point& msg);
void callback_mode(const std_msgs::UInt8& msg);
void callback_servo(const std_msgs::UInt8& msg);
void callback_xy(const geometry_msgs::Point& msg);
void callback_display(const std_msgs::String& msg);
void publish_pos();
ros::NodeHandle nh;
//ros::Subscriber<std_msgs::Float64> carspeed_sub("/car/speed", &callback_speed);
ros::Subscriber<geometry_msgs::Point> carpos_sub("/car/pos", &callback_pos);
ros::Subscriber<geometry_msgs::Point> carspeed_sub("/car/speed", &callback_speed);
ros::Subscriber<std_msgs::Float64> carspeedlimit_sub("/car/speedlimit", &callback_speedlimit);
ros::Subscriber<std_msgs::UInt8> carmode_sub("/car/mode", &callback_mode);
ros::Subscriber<std_msgs::UInt8> servo_sub("/car/servo", &callback_servo);
ros::Subscriber<geometry_msgs::Point> xy_sub("/car/xy", &callback_xy);
ros::Subscriber<std_msgs::String> display_sub("/car/display", &callback_display);
ros::Subscriber<geometry_msgs::Point> servo_speed_sub("/car/servo_speed", &callback_servo_speed);
geometry_msgs::Point pos_msg;
std_msgs::Bool idle;
std_msgs::Float64 error_car;
ros::Publisher current_pos("cur_pos", &pos_msg);
ros::Publisher servo_status("/servo_status", &idle);
ros::Publisher car_status("/car_status", &error_car);

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    nh.getHardware()->flush();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    nh.getHardware()->reset_rbuf();
}



void setup(void)
{
	nh.initNode();
	nh.subscribe(carspeed_sub);
	nh.subscribe(carpos_sub);
	nh.subscribe(carspeedlimit_sub);
	nh.subscribe(carmode_sub);
	nh.subscribe(servo_sub);	
	nh.advertise(servo_status);
	nh.advertise(car_status);
	nh.subscribe(display_sub);
	nh.subscribe(xy_sub);
	nh.subscribe(servo_speed_sub);
	set_stepper_1(-10000);
	Servo_Add_Action(0,0,1000);//simply wait
	Servo_TransPos();
	
	
//	Servo_Camera2();
//	Servo_TransPos();
	
	//Servo_Camera_AdjPosLower();
//	Servo_Grab();



//

	

//-
	
//	Servo_Camera_AdjPosLower();
//	Servo_TransPos();

//	Servo_Grab_Pose2_Lower();
//	Servo_PutMiddle();

	
//	Servo_Camera();
//	Servo_Grab_Pose_Lower();
//	Servo_PutMiddle();
	
	
	
	
//	Servo_GrabRight();
//	Servo_Put_Upper();

	
	Screen_printString("helloWorld!");
}
void publish_servo_status()
{
		idle.data=Is_Servo_Idle();
		servo_status.publish(&idle);
}
void publish_car_status()
{
		error_car.data = Get_Error();
		car_status.publish(&error_car);
}
void loop(void)
{

    nh.spinOnce();
	publish_servo_status();
	publish_car_status();
//		publish_pos();
		
    //HAL_Delay(1000);
}
void callback_speed(const geometry_msgs::Point& msg)// cm/s
{
    Set_Run_Flag(0);
    Set_Move(\
		msg.x*8.556169931964706,\
		msg.y*8.556169931964706,\
		(msg.z*R)*8.556169931964706\
		);
	//Run_Flag=0;
	//Move_X=msg.x*8.556169931964706;//0.1168747240823413;
    //Move_Y=msg.y*8.556169931964706;//0.1168747240823413;
    //Move_Z=(msg.z*R)*8.556169931964706;//0.1168747240823413;//waiting
}
void callback_pos(const geometry_msgs::Point& msg)// cm
{
    if(Get_Run_Flag()==0)//mode changed from speed mode
    {
        if(Get_relative()==0)//changed to abs pos
        {
            double NOW_X,NOW_Y,NOW_Z;
			Get_Position_NOW(&NOW_X,&NOW_Y,&NOW_Z);
			Set_Last_Target(NOW_X,NOW_Y,NOW_Z);
            // NOW_X=(Position_B-Position_A)/2.0;//2021-4-20 changed **************************************************
            // NOW_Y=(Position_B+Position_C)/2.0;//2021-4-20 changed **************************************************
            // NOW_Z=(Position_C-Position_A+Position_D-Position_B)/4.0/(a_PARAMETER+b_PARAMETER);
            // Last_Target_X=NOW_X;//����������״̬
            // Last_Target_Y=NOW_Y;//
            // Last_Target_Z=NOW_Z;//
        }
    }
	Set_Run_Flag(1);
	Set_Move(\
	msg.x*1711.23398,\
	msg.y*1711.23398,\
	(msg.z*R)*1711.23398\
	);
    //Run_Flag=1;
    //Move_X=msg.x*1711.23398;
    //Move_Y=msg.y*1711.23398;
    //Move_Z=(msg.z*R)*1711.23398;//waiting
    if(Get_relative()==1)
		Set_pending_flag(1);
        //pending_flag=1;
	publish_car_status();
}

void callback_speedlimit(const std_msgs::Float64& msg)// cm/s
{
	Set_RC_Velocity(msg.data*8.556169931964706*2);
    //RC_Velocity=msg.data*8.556169931964706*2;
}

void callback_servo_speed(const geometry_msgs::Point& msg)// cm/s
{
    change_servo_speed(msg.x,msg.y);
}

void callback_mode(const std_msgs::UInt8& msg)// pos_mode 0:absolute 1:relative
{
    if(Get_relative()!=msg.data)//mode changed
    {
        if(Get_Run_Flag()==1&&Get_relative()==1)//in position mode,want to change to absolute mode from relative mode
        {
            double NOW_X,NOW_Y,NOW_Z;
			Get_Position_NOW(&NOW_X,&NOW_Y,&NOW_Z);
			Set_Last_Target(NOW_X,NOW_Y,NOW_Z);
			Set_Move(NOW_X,NOW_Y,NOW_Z);
            // NOW_X=(Position_B-Position_A)/2.0;
            // NOW_Y=(Position_B+Position_C)/2.0;
            // NOW_Z=(Position_C-Position_A+Position_D-Position_B)/4.0/(a_PARAMETER+b_PARAMETER);
            // Last_Target_X=NOW_X;//����������״̬
            // Last_Target_Y=NOW_Y;
            // Last_Target_Z=NOW_Z;
            // Move_X=NOW_X;//����Ŀ������
            // Move_Y=NOW_Y;
            // Move_Z=NOW_Z;
        }
		Set_relative(msg.data);
        //relative=msg.data;
    }
		publish_car_status();
}
void publish_pos()
{
    double NOW_X,NOW_Y,NOW_Z;
	Get_Position_NOW(&NOW_X,&NOW_Y,&NOW_Z);
    // NOW_X=(Position_B-Position_A)/2.0;
    // NOW_Y=(Position_B+Position_C)/2.0;
    // NOW_Z=(Position_C-Position_A+Position_D-Position_B)/4.0/(a_PARAMETER+b_PARAMETER);
    pos_msg.x=NOW_X/1711.23398;
    pos_msg.y=NOW_Y/1711.23398;
    pos_msg.z=NOW_Z/1711.23398/R;//waiting
    current_pos.publish(&pos_msg);
}
void callback_servo(const std_msgs::UInt8& msg)
{
	switch(msg.data)
	{
		case 0x00:
			Servo_PutRight();
			last_mode_servo=0x00;
			break;
		case 0x01:
			Servo_PutMiddle();
			last_mode_servo=0x01;
			break;		
		case 0x02:
			Servo_PutLeft();
			last_mode_servo=0x02;
			break;		
		case 0x03:
			Servo_GrabLeft();
			last_mode_servo=0x03;
			break;
		case 0x04:
			Servo_GrabMiddle();
			last_mode_servo=0x04;
			break;		
		case 0x05:
			Servo_GrabRight();
			last_mode_servo=0x05;
			break;
		case 0x06:
			Servo_Grab_Upper();
			last_mode_servo=0x06;
			break;		
		case 0x07:
			Servo_Grab_Pose_Lower();
			last_mode_servo=0x07;
			break;
		case 0x08:
			Servo_Grab_Pose2_Lower();
			last_mode_servo=0x08;
			break;		
		case 0x09:
			Servo_Grab();
			last_mode_servo=0x09;
			break;		
		
		case 0x0A:
			Servo_Put_Upper();
			last_mode_servo=0x0A;
			break;		
		case 0x0B:
			Servo_Put_Lower();
			last_mode_servo=0x0B;
			break;
		case 0x0C:
			Servo_Camera();
			last_mode_servo=0x0C;
			break;
		case 0x0D:
			Servo_TransPos();
			last_mode_servo=0x0D;
			break;
		case 0x0E:
			All_Middle();
			last_mode_servo=0x0E;
			break;
		case 0x0F:
			Servo_Camera1();
			last_mode_servo=0x0F;
			break;
		case 0x10:
			Servo_Camera2();
			last_mode_servo=0x10;
			break;
		case 0x11:
			Servo_Camera3();
			last_mode_servo=0x11;
			break;
		case 0x12:
			Servo_Put_Upper_Storage();
			last_mode_servo=0x12;
			break;
		case 0x13:
			Servo_Put_Lower_Storage();
			last_mode_servo=0x13;
			break;
		case 0x14:
			Servo_Camera_AdjPosLower();//Ԥ��ץ�²����ʱ����Ϊֹ�Ŀ�
			last_mode_servo = 0x14;
			break;
		
		
		
	}
	
	publish_servo_status();
}
void callback_display(const std_msgs::String& msg)
{
	Screen_printString(msg.data);
}	
void callback_xy(const geometry_msgs::Point& msg)
{
		if(msg.y!=0)
		{
//			set_stepper(msg.y);//25-900 
//			update_Servo_state(0xFFF0,msg.y);
				Servo_Add_Action(0xFFF0,msg.y,-1);
		}
		if(msg.x!=0)
		{
//			WritePos(100,1024-msg.x,100,50);//200-370 259middle
//			update_Servo_state(100,msg.y);
				Servo_Add_Action(100,1024-msg.x,-1);
		}
	publish_servo_status();
}

