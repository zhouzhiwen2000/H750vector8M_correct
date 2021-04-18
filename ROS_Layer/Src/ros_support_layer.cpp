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
extern "C" {
    extern float Move_X,Move_Y,Move_Z;   //XYZ轴目标速度
    extern uint8_t Run_Flag;
    extern int RC_Velocity;
    extern uint8_t relative;
    extern uint8_t pending_flag;
    extern double Last_Target_X,Last_Target_Y,Last_Target_Z;
    extern long int Position_A,Position_B,Position_C,Position_D; //PID控制相关变量
		extern long int Target_A,Target_B,Target_C,Target_D;
		
}
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
	set_stepper(-10000);
	Servo_Add_Action(0,0,1000);//simply wait
	Servo_TransPos();
	
	

//	
//	//970 430 735
//	
//	Servo_Add_Action(0xFFF0,25,-1);
//	Servo_Add_Action(101,850,-1);
//	
//	//480 650 1000
//	Servo_Add_Action(2,480,-1);
//	Servo_Add_Action(4,1000,-1);
//    Servo_Add_Action(3,650,-1);
//	
//	
//	//520 670 1000
//	Servo_Add_Action(2,520,-1);
//    Servo_Add_Action(3,670,-1);

//	
//	//560 650 1000
//	Servo_Add_Action(2,560,-1);
//    Servo_Add_Action(3,650,-1);
//	
//	
//	//600 630 1000
//	Servo_Add_Action(2,600,-1);
//    Servo_Add_Action(3,630,-1);
//	
//	//640 640 1000
//	Servo_Add_Action(2,640,-1);
//    Servo_Add_Action(3,640,-1);
//	
//	//660 600 1000
//	Servo_Add_Action(2,660,-1);
//    Servo_Add_Action(3,600,-1);
//	
//	//700 520 1000
//	Servo_Add_Action(2,700,-1);
//    Servo_Add_Action(3,520,-1);
//	
//	//720 430 1000
//	Servo_Add_Action(3,430,-1);
//	Servo_Add_Action(2,720,-1);
//	
//	
//	Servo_Add_Action(0xFFF0,850,-1);
//    
//	
//	//740 430 940
//	Servo_Add_Action(2,740,-1);
//	Servo_Add_Action(4,940,-1);
//	
//	//800 430 860
//	Servo_Add_Action(2,800,-1);
//	Servo_Add_Action(4,860,-1);
//	
//	
//	//860 430 780
//	Servo_Add_Action(2,860,-1);
//	Servo_Add_Action(4,780,-1);
//	
//	//900 430 735
//	Servo_Add_Action(2,900,-1);
//	Servo_Add_Action(4,735,-1);
//	
//	//920 430 735
//	Servo_Add_Action(2,920,-1);

//	
//	Servo_Grab();
//	
//	Servo_Add_Action(2,900,-1);
//	Servo_Add_Action(4,780,-1);
//	Servo_Add_Action(2,860,-1);
//	Servo_Add_Action(4,860,-1);
//	Servo_Add_Action(2,800,-1);
//	Servo_Add_Action(4,940,-1);
//	Servo_Add_Action(2,740,-1);
//	Servo_Add_Action(0xFFF0,25,-1);
//	
//	
//	//***********************************************************
//	Servo_Add_Action(2,720,-1);
//	Servo_Add_Action(3,430,-1);
//    Servo_Add_Action(3,520,-1);
//	Servo_Add_Action(2,700,-1);
//    Servo_Add_Action(3,600,-1);
//	Servo_Add_Action(2,660,-1);
//    Servo_Add_Action(3,640,-1);
//	Servo_Add_Action(2,640,-1);
//    Servo_Add_Action(3,630,-1);
//	Servo_Add_Action(2,600,-1);
//    Servo_Add_Action(3,650,-1);
//	Servo_Add_Action(2,560,-1);
//    Servo_Add_Action(3,670,-1);
//	Servo_Add_Action(2,520,-1);
//    Servo_Add_Action(3,650,-1);
//	Servo_Add_Action(4,1000,-1);
//	Servo_Add_Action(2,480,-1);
//	
//	Servo_Add_Action(0xFFF0,450,-1);
//	
//	
//	Servo_PutMiddle();
	

	
	
//	Servo_GrabRight();
//	Servo_Put_Upper();

	
	
//		Servo_Grab_Upper();
//		Servo_TransPos();
//		Servo_Grab_Upper();
//	  Servo_TransPos();
//		Servo_Grab();
//		Servo_Put_Lower();
//		Servo_Grab();		
//		Servo_Put_Upper();
//	  Servo_Grab_Pose_Lower();
//		Servo_Add_Action(0,0,3000);
//	  Servo_Grab_Pose2_Lower();
//	  Servo_Grab();
//	  All_Middle();
//	  Servo_PutLeft();
//		Servo_PutMiddle();
//		Servo_PutRight();
//		Servo_Camera1();
//		Servo_Camera2();
//		Servo_Camera();
//		Servo_Grab_Pose_Lower();
//		Servo_Grab_Pose2_Lower();
		Screen_printString("helloWorld!");
}
void publish_servo_status()
{
		idle.data=Is_Servo_Idle();
		servo_status.publish(&idle);
}
void publish_car_status()
{
		error_car.data = sqrt((Position_A-Target_A)*(Position_A-Target_A)+(Position_B-Target_B)*(Position_B-Target_B)+(Position_C-Target_C)*(Position_C-Target_C)+(Position_D-Target_D)*(Position_D-Target_D));
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
void callback_speed(const geometry_msgs::Point& msg)//cm/s
{
    Run_Flag=0;
    Move_X=msg.x*8.556169931964706;//0.1168747240823413;
    Move_Y=msg.y*8.556169931964706;//0.1168747240823413;
    Move_Z=(msg.z*R)*8.556169931964706;//0.1168747240823413;//waiting
}
void callback_pos(const geometry_msgs::Point& msg)//cm
{
    if(Run_Flag==0)//mode changed from speed mode
    {
        if(relative==0)//changed to abs pos
        {
            double NOW_X,NOW_Y,NOW_Z;
            NOW_X=(Position_B-Position_A)/2.0;
            NOW_Y=(Position_B+Position_C)/2.0;
            NOW_Z=(Position_C-Position_A+Position_D-Position_B)/4.0/(a_PARAMETER+b_PARAMETER);
            Last_Target_X=NOW_X;//纠正限速器状态
            Last_Target_Y=NOW_Y;//
            Last_Target_Z=NOW_Z;//
        }
    }
    Run_Flag=1;
    Move_X=msg.x*1711.23398;
    Move_Y=msg.y*1711.23398;
    Move_Z=(msg.z*R)*1711.23398;//waiting
    if(relative==1)
        pending_flag=1;
		publish_car_status();
}

void callback_speedlimit(const std_msgs::Float64& msg)//cm/s
{
    RC_Velocity=msg.data*8.556169931964706;
}

void callback_servo_speed(const geometry_msgs::Point& msg)//cm/s
{
    change_servo_speed(msg.x,msg.y);
}

void callback_mode(const std_msgs::UInt8& msg)//pos_mode 0:absolute 1:relative
{
    if(relative!=msg.data)//mode changed
    {
        if(Run_Flag==1&&relative==1)//in position mode,want to change to absolute mode from relative mode
        {
            double NOW_X,NOW_Y,NOW_Z;
            NOW_X=(Position_B-Position_A)/2.0;
            NOW_Y=(Position_B+Position_C)/2.0;
            NOW_Z=(Position_C-Position_A+Position_D-Position_B)/4.0/(a_PARAMETER+b_PARAMETER);
            Last_Target_X=NOW_X;//纠正限速器状态
            Last_Target_Y=NOW_Y;
            Last_Target_Z=NOW_Z;
            Move_X=NOW_X;//纠正目标坐标
            Move_Y=NOW_Y;
            Move_Z=NOW_Z;
        }
        relative=msg.data;
    }
		publish_car_status();
}
void publish_pos()
{
    double NOW_X,NOW_Y,NOW_Z;
    NOW_X=(Position_B-Position_A)/2.0;
    NOW_Y=(Position_B+Position_C)/2.0;
    NOW_Z=(Position_C-Position_A+Position_D-Position_B)/4.0/(a_PARAMETER+b_PARAMETER);
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
			break;
		case 0x01:
			Servo_PutMiddle();
			break;		
		case 0x02:
			Servo_PutLeft();
			break;		
		case 0x03:
			Servo_GrabLeft();
			break;
		case 0x04:
			Servo_GrabMiddle();
			break;		
		case 0x05:
			Servo_GrabRight();
			break;
		case 0x06:
			Servo_Grab_Upper();
			break;		
		case 0x07:
			Servo_Grab_Pose_Lower();
			break;
		case 0x08:
			Servo_Grab_Pose2_Lower();
			break;		
		case 0x09:
			Servo_Grab();
			break;		
		
		case 0x0A:
			Servo_Put_Upper();
			break;		
		case 0x0B:
			Servo_Put_Lower();
			break;
		case 0x0C:
			Servo_Camera();
			break;
		case 0x0D:
		  Servo_TransPos();
			break;
		case 0x0E:
		  All_Middle();
			break;
		case 0x0F:
		  Servo_Camera1();
			break;
		case 0x10:
		  Servo_Camera2();
			break;
		case 0x11:
			Servo_Camera3();
			break;
		case 0x12:
			Servo_Put_Upper_Storage();
			break;
		case 0x13:
			Servo_Put_Lower_Storage();
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

