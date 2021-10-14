/*
 * ros_task.cpp
 *
 *  Created on: Oct 12, 2021
 *      Author: ZhouZhiwen
 */
#include "Control.h"
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <uxr/client/transport.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/bool.h>
#include <std_msgs/msg/float64.h>
#include <std_msgs/msg/u_int8.h>
#include <std_msgs/msg/string.h>
#include "Screen_drv.h"
#include "Servo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include <geometry_msgs/msg/Point.h>
#include "usart.h"
#include "Servo.h"
#include "ros_task.hpp"
#include "main.h"
rcl_publisher_t current_pos;
rcl_publisher_t servo_status;
rcl_publisher_t car_status;

rcl_subscription_t carpos_sub;
rcl_subscription_t carspeed_sub;
rcl_subscription_t carspeedlimit_sub;
rcl_subscription_t carmode_sub;
rcl_subscription_t servo_sub;
rcl_subscription_t xy_sub;
rcl_subscription_t display_sub;
rcl_subscription_t servo_speed_sub;

geometry_msgs__msg__Point pos_msg;
std_msgs__msg__Bool idle;
std_msgs__msg__Float64 error_car;

void callback_pos(const void * msg);
void callback_speed(const void * msg);
void callback_speedlimit(const void * msg);
void callback_servo_speed(const void * msg);
void callback_mode(const void * msg);
void callback_servo(const void * msg);
void callback_xy(const void * msg);
void callback_display(const void * msg);
geometry_msgs__msg__Point sub_msg_pos;
geometry_msgs__msg__Point sub_msg_speed;
geometry_msgs__msg__Point sub_msg_servo_speed;
geometry_msgs__msg__Point sub_msg_xy;
std_msgs__msg__Float64 sub_msg_speedlimit;
std_msgs__msg__UInt8 sub_msg_mode;
std_msgs__msg__UInt8 sub_msg_servo;
std_msgs__msg__String sub_msg_display;

#define a_PARAMETER          (0.6472324f)
#define b_PARAMETER          (0.7622959f)
#define R 15
extern "C" {//functions&variables imported from C
bool cubemx_transport_open(struct uxrCustomTransport * transport);
bool cubemx_transport_close(struct uxrCustomTransport * transport);
size_t cubemx_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
size_t cubemx_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

void * microros_allocate(size_t size, void * state);
void microros_deallocate(void * pointer, void * state);
void * microros_reallocate(void * pointer, size_t size, void * state);
void * microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state);
}
void publish_servo_status();
void publish_car_status();
void publish_pos();
/*geometry_msgs::Point*/
void callback_speed(const void * msg)// cm/s
{
	const geometry_msgs__msg__Point * msgin = (const geometry_msgs__msg__Point *)msg;
    Set_Run_Flag(0);
    Set_Move(\
		msgin->x*8.556169931964706,\
		msgin->y*8.556169931964706,\
		(msgin->z*R)*8.556169931964706\
		);
}
void callback_pos(const void * msg)// cm
{
	const geometry_msgs__msg__Point * msgin = (const geometry_msgs__msg__Point *)msg;
    if(Get_Run_Flag()==0)//mode changed from speed mode
    {
        if(Get_relative()==0)//changed to abs pos
        {
            double NOW_X,NOW_Y,NOW_Z;
			Get_Position_NOW(&NOW_X,&NOW_Y,&NOW_Z);
			Set_Last_Target(NOW_X,NOW_Y,NOW_Z);
        }
    }
	Set_Run_Flag(1);
	Set_Move(\
	msgin->x*1711.23398,\
	msgin->y*1711.23398,\
	(msgin->z*R)*1711.23398\
	);
    if(Get_relative()==1)
		Set_pending_flag(1);
	publish_car_status();
}

void callback_speedlimit(const void * msg)// cm/s
{
	const std_msgs__msg__Float64  * msgin = (const std_msgs__msg__Float64 *)msg;
	Set_RC_Velocity(msgin->data*8.556169931964706*2);
}

void callback_servo_speed(const void * msg)// cm/s
{
	const geometry_msgs__msg__Point * msgin = (const geometry_msgs__msg__Point *)msg;
    change_servo_speed(msgin->x,msgin->y);
}

void callback_mode(const void * msg)// pos_mode 0:absolute 1:relative
{
	const std_msgs__msg__UInt8  * msgin = (const std_msgs__msg__UInt8 *)msg;
    if(Get_relative()!=msgin->data)//mode changed
    {
        if(Get_Run_Flag()==1&&Get_relative()==1)//in position mode,want to change to absolute mode from relative mode
        {
            double NOW_X,NOW_Y,NOW_Z;
			Get_Position_NOW(&NOW_X,&NOW_Y,&NOW_Z);
			Set_Last_Target(NOW_X,NOW_Y,NOW_Z);
			Set_Move(NOW_X,NOW_Y,NOW_Z);
        }
		Set_relative(msgin->data);
        //relative=msg.data;
    }
		publish_car_status();
}
void callback_servo(const void * msg)
{
	const std_msgs__msg__UInt8  * msgin = (const std_msgs__msg__UInt8 *)msg;
	switch(msgin->data)
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
		case 0x14:
			Servo_Camera_AdjPosLower();
			break;
	}

	publish_servo_status();
}
void callback_display(const void * msg)
{
	const std_msgs__msg__String  * msgin = (const std_msgs__msg__String *)msg;
	Screen_printString(std::string(msgin->data.data));
}
void callback_xy(const void * msg)
{
	const geometry_msgs__msg__Point * msgin = (const geometry_msgs__msg__Point *)msg;
	if(msgin->y!=0)
	{
//			set_stepper(msg.y);//25-900
//			update_Servo_state(0xFFF0,msg.y);
			Servo_Add_Action(0xFFF0,msgin->y,-1);
	}
	if(msgin->x!=0)
	{
//			WritePos(100,1024-msg.x,100,50);//200-370 259middle
//			update_Servo_state(100,msg.y);
			Servo_Add_Action(100,1024-msgin->x,-1);
	}
	publish_servo_status();
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
	(void) last_call_time;
	if (timer != NULL) {
		publish_servo_status();
		publish_car_status();
	}
}

void ros_init()
{
	  // micro-ROS configuration
	  rmw_uros_set_custom_transport(
		true,
		(void *) &huart1,
		cubemx_transport_open,
		cubemx_transport_close,
		cubemx_transport_write,
		cubemx_transport_read);

	  rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
	  freeRTOS_allocator.allocate = microros_allocate;
	  freeRTOS_allocator.deallocate = microros_deallocate;
	  freeRTOS_allocator.reallocate = microros_reallocate;
	  freeRTOS_allocator.zero_allocate =  microros_zero_allocate;

	  if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
		  printf("Error on default allocators (line %d)\n", __LINE__);
	  }

	  // micro-ROS app

	  rclc_support_t support;
	  rcl_allocator_t allocator;
	  rcl_node_t node;

	  allocator = rcl_get_default_allocator();

	  //create init_options
	  rclc_support_init(&support, 0, NULL, &allocator);

	  // create node
	  rclc_node_init_default(&node, "car_node", "", &support);

	  // create publisher
	  /*rclc_publisher_init_best_effort*/
	  rclc_publisher_init_best_effort(
		&current_pos,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Point),
		"current_pos");

	  rclc_publisher_init_best_effort(
		&servo_status,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
		"servo_status");

	  rclc_publisher_init_best_effort(
		&car_status,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
		"car_status");

	  rclc_subscription_init_default(
	  		&carpos_sub,
	  		&node,
	  		ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Point),
	  		"/car/pos");

	  rclc_subscription_init_default(
	  		&carspeed_sub,
	  		&node,
	  		ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Point),
	  		"/car/speed");

	  rclc_subscription_init_default(
	  		&carspeedlimit_sub,
	  		&node,
	  		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
	  		"/car/speedlimit");

	  rclc_subscription_init_default(
	  		&carmode_sub,
	  		&node,
	  		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
	  		"/car/mode");

	  rclc_subscription_init_default(
	  		&servo_sub,
	  		&node,
	  		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
	  		"/car/servo");

	  rclc_subscription_init_default(
	  		&xy_sub,
	  		&node,
	  		ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Point),
	  		"/car/xy");

	  rclc_subscription_init_default(
	  		&display_sub,
	  		&node,
	  		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
	  		"/car/display");

	  rcl_ret_t temp_rc1 = rclc_subscription_init_default(
	  		&servo_speed_sub,
	  		&node,
	  		ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Point),
	  		"/car/servo_speed");

//	  servo_speed_sub = rcl_get_zero_initialized_subscription();
//	  rcl_subscription_options_t sub_opt = rcl_subscription_get_default_options();
//	  sub_opt.qos = rmw_qos_profile_default;
//	  rcl_ret_t rc = rcl_subscription_init(
//		&servo_speed_sub,
//	    &node,
//		ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Point),
//		"/car/servo_speed",
//		 &sub_opt);
//	  //rc returned 1
//	  if (rc != RCL_RET_OK) {
//	    PRINT_RCLC_ERROR(rclc_subscription_init_best_effort, rcl_subscription_init);
//	  }

		// create timer,
		rcl_timer_t timer;
		const unsigned int timer_timeout = 100;//10hz
		rclc_timer_init_default(
			&timer,
			&support,
			RCL_MS_TO_NS(timer_timeout),
			timer_callback);
		rclc_executor_t executor = rclc_executor_get_zero_initialized_executor();
		rclc_executor_init(&executor, &support.context, 10, &allocator);//8subs+1timer
		rclc_executor_add_timer(&executor, &timer);

		rclc_executor_add_subscription(&executor, &carpos_sub, &sub_msg_pos, &callback_pos, ON_NEW_DATA);
		rclc_executor_add_subscription(&executor, &carspeed_sub, &sub_msg_speed, &callback_speed, ON_NEW_DATA);
		rclc_executor_add_subscription(&executor, &carspeedlimit_sub, &sub_msg_speedlimit, &callback_speedlimit, ON_NEW_DATA);
		rclc_executor_add_subscription(&executor, &servo_speed_sub, &sub_msg_servo_speed, &callback_servo_speed, ON_NEW_DATA);
		rclc_executor_add_subscription(&executor, &carmode_sub, &sub_msg_mode, &callback_mode, ON_NEW_DATA);
		rclc_executor_add_subscription(&executor, &servo_sub, &sub_msg_servo, &callback_servo, ON_NEW_DATA);
		rclc_executor_add_subscription(&executor, &xy_sub, &sub_msg_xy, &callback_xy, ON_NEW_DATA);
		rclc_executor_add_subscription(&executor, &display_sub, &sub_msg_display, &callback_display, ON_NEW_DATA);
		while(1)
		{
		    TickType_t tcnt=xTaskGetTickCount();
		    rcl_ret_t temp_rc = rclc_executor_spin_some(&executor,RCL_MS_TO_NS(100));
		    if(temp_rc==RCL_RET_ERROR)break;
		    osDelayUntil(tcnt+20);//100Hz
		}
		rcl_subscription_fini(&carpos_sub, &node);
		rcl_subscription_fini(&carspeed_sub, &node);
		rcl_subscription_fini(&carspeedlimit_sub, &node);
		rcl_subscription_fini(&servo_speed_sub, &node);
		rcl_subscription_fini(&carmode_sub, &node);
		rcl_subscription_fini(&servo_sub, &node);
		rcl_subscription_fini(&xy_sub, &node);
		rcl_subscription_fini(&display_sub, &node);

		rcl_publisher_fini(&current_pos, &node);
		rcl_publisher_fini(&servo_status, &node);
		rcl_publisher_fini(&car_status, &node);
		rcl_node_fini(&node);
}
void publish_servo_status()
{
		idle.data=Is_Servo_Idle();
		rcl_publish(&servo_status, &idle, NULL);
}
void publish_car_status()
{
		error_car.data = Get_Error();
		rcl_publish(&car_status, &error_car, NULL);
}
void publish_pos()
{
    double NOW_X,NOW_Y,NOW_Z;
	Get_Position_NOW(&NOW_X,&NOW_Y,&NOW_Z);
    pos_msg.x=NOW_X/1711.23398;
    pos_msg.y=NOW_Y/1711.23398;
    pos_msg.z=NOW_Z/1711.23398/R;//waiting
	rcl_publish(&current_pos, &pos_msg, NULL);
}

