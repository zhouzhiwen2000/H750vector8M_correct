extern "C" {//functions&variables imported from C
#include "SCS.h"
#include "SMSBCL.h"
#include "SCSCL.h"
#include "stm32h7xx_hal.h"
}
#include "Servo.h"
#include "StepperMotor.h"
#include <vector>
#include "FreeRTOS.h"
#include "semphr.h"
class _act
{
public:
    uint32_t id;
    uint32_t value;
};

class action_servo
{
public:
    action_servo * Next;
    std::vector<_act>  acts;
    uint32_t time;
    int32_t speed;
};


static action_servo * Current=0;
static action_servo * Last=0;
static _act * ptr_test;
static uint32_t time_start;
static int stepper_lastvalue1=0;
static int stepper_lastvalue2=0;
static int last_mode_servo=-1;
static double servo_speed[10]={0,1,1,1,1,1,1,1,1,1};//{none,1,2,3,4,5,none,none....}
static int servo_lastvalue[10]= {0};
extern SemaphoreHandle_t Servo_Lock;
extern SemaphoreHandle_t Servo_Lock_Upper;
static const bool is_STS[10]={0,true,false,true,true,false};//{none,1,2,3,4,5,none}

uint32_t get_array_seq(uint32_t id)
{
//	if(id>=100)id=id-95;
	return id;
}

int WritePos(uint8_t ID, uint16_t Position, uint16_t Time, uint16_t Speed)
{
	if(!is_STS[get_array_seq(ID)])
	{
		return WritePosSCS(ID,Position,Time,Speed);
	}
	else
	{
		if(Speed!=0)
			return WritePosSTS(ID,Position,Speed,50);
		else
			return WritePosSTS(ID,Position,servo_speed[get_array_seq(ID)]*1250,50);//multiply by 1050 instead of 1000 to ensure enough wait time.

	}
}

/***************freertos new() implementation**************/
void * operator new( size_t size )
{
    return pvPortMalloc(size);
}

void * operator new[]( size_t size )
{
    return pvPortMalloc( size );
}

void operator delete( void * ptr )
{
    vPortFree( ptr );
}

void operator delete[]( void * ptr )
{
    vPortFree( ptr );
}
/***************freertos new() implementation**************/




void Servo_Server()
{
    xSemaphoreTake(Servo_Lock, portMAX_DELAY);
    if(Current!=0)
    {
        if(HAL_GetTick()-time_start>=Current->time)
        {
            action_servo * del=Current;
            Current=Current->Next;
            if(Current==0)Last=0;//queue is empty again
            delete(del);
            if(Current!=0)
            {
                while(!Current->acts.empty())
                {
                    _act  action=(Current->acts.front());;
                    ptr_test=&(Current->acts.front());
                    if(action.id<0xFFF0)
                        WritePos(action.id,action.value,Current->time,0);
                    else
                    	if(action.id==0xFFF0)
                    		{
                    			set_speed_1(Current->speed);
                    			set_stepper_1(action.value);
                    		}
                    	else if(action.id==0xFFF1)
                    		{
                    			set_speed_2(Current->speed);
                    			set_stepper_2(action.value);
                    		}
                    Current->acts.erase(Current->acts.begin());
                }
//				WritePos(Current->id,Current->value,Current->time,0);
                time_start=HAL_GetTick();
            }
        }
    }
    xSemaphoreGive(Servo_Lock);
}

void Servo_Add_Action(uint32_t id,uint32_t value,int32_t time)
{
    xSemaphoreTake(Servo_Lock, portMAX_DELAY);
    action_servo * New=new action_servo() ;
//	New->id=id;
//	New->value=value;
    _act action1;
    action1.id=id;
    action1.value=value;
    New->acts.push_back(action1);
    New->time=time;
    New->Next=0;
    New->speed=-1;
    if(action1.id==0xFFF0)
    {
    	if(New->speed==-1)
    	{
    		New->speed=2;//default speed =2
    	}
    	if(time!=-1)
    	{
    		New->speed=time;
    	}
        if(Current==0)
        {
            stepper_lastvalue1=get_steps_1();//update steps
        }
        int stepper_time=fabs((int)value-(int)stepper_lastvalue1)*2.0*New->speed/20;//in ms
        New->time=stepper_time;
        stepper_lastvalue1=value;
    }
    else if (action1.id==0xFFF1)
    {
    	if(New->speed==-1)
    	{
    		New->speed=2;//default speed =2
    	}
    	if(time!=-1)
    	{
    		New->speed=time;
    	}
        if(Current==0)
        {
            stepper_lastvalue2=get_steps_2();//update steps
        }
        int stepper_time=fabs((int)value-(int)stepper_lastvalue2)*2.0*New->speed/20;//in ms
        New->time=stepper_time;
        stepper_lastvalue2=value;
    }
    else
    {
        if(time==-1)
        {
            int servo_time=fabs((int)value-(int)servo_lastvalue[get_array_seq(id)])/servo_speed[get_array_seq(id)];//in ms
            New->time=servo_time+1;
        }
        servo_lastvalue[get_array_seq(id)]=value;
    }
    if(Last==0) Last=New;//queue is empty now
    else
    {
        Last->Next=New;
        Last=New;
    }
    if(Current==0)//initial steps
    {
        Current=New;
        while(!Current->acts.empty())
        {
            _act  action=Current->acts.front();
            if(action.id<0xFFF0)
                WritePos(action.id,action.value,Current->time,0);
            else if(action.id==0xFFF0)
    		{
    			set_speed_1(Current->speed);
    			set_stepper_1(action.value);
    		}
            else if(action.id==0xFFF1)
    		{
    			set_speed_2(Current->speed);
    			set_stepper_2(action.value);
    		}
            Current->acts.erase(Current->acts.begin());
        }
//		WritePos(Current->id,Current->value,Current->time,0);
        time_start=HAL_GetTick();
    }
    xSemaphoreGive(Servo_Lock);
}

void Servo_Add_Action_bunch(std::vector<_act>  acts,int32_t time)
{
    xSemaphoreTake(Servo_Lock, portMAX_DELAY);
    action_servo * New=new(action_servo);
    New->acts=acts;
    New->time=time;
    New->Next=0;
    New->speed=-1;
    std::vector<_act>::iterator it = acts.begin();
    for(; it != acts.end(); ++it)
    {
        _act & act=*it;
        if(act.id==0xFFF0)
        {
        	if(New->speed==-1)
        	{
        		New->speed=2;//default speed =2
        	}
        	if(time!=-1)
        	{
        		New->speed=time;
        	}
            if(Current==0)
            {
                stepper_lastvalue1=get_steps_1();//update steps
            }
            int stepper_time=fabs(act.value-stepper_lastvalue1)*2.0*New->speed/10;//in ms
            New->time=stepper_time;
            stepper_lastvalue1=act.value;
        }
        else if(act.id==0xFFF1)
        {
        	if(New->speed==-1)
        	{
        		New->speed=2;//default speed =2
        	}
        	if(time!=-1)
        	{
        		New->speed=time;
        	}
            if(Current==0)
            {
                stepper_lastvalue2=get_steps_2();//update steps
            }
            int stepper_time=fabs(act.value-stepper_lastvalue2)*2.0*New->speed/10;//in ms
            New->time=stepper_time;
            stepper_lastvalue2=act.value;
        }
        else
        {
            if(time==-1)
            {
                int servo_time=fabs((int)act.value-(int)servo_lastvalue[get_array_seq(act.id)])/servo_speed[get_array_seq(act.id)];//in ms
                New->time=servo_time;
            }
            servo_lastvalue[get_array_seq(act.id)]=act.value;
        }
    }
    if(Last==0) Last=New;//queue is empty now
    else
    {
        Last->Next=New;
        Last=New;
    }
    if(Current==0)//initial steps
    {
        Current=New;
//		WritePos(Current->id,Current->value,Current->time,0);
        while(!Current->acts.empty())
        {
            _act & action=Current->acts.front();
            if(action.id<0xFFF0)
                WritePos(action.id,action.value,Current->time,0);
            else if(action.id==0xFFF0)
    		{
    			set_speed_1(Current->speed);
    			set_stepper_1(action.value);
    		}
            else if(action.id==0xFFF1)
    		{
    			set_speed_2(Current->speed);
    			set_stepper_2(action.value);
    		}
        }
        time_start=HAL_GetTick();
    }
    xSemaphoreGive(Servo_Lock);
}

void Servo_TransPos()//grab&to Middle
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	last_mode_servo = -1;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

void All_Middle()//grab&to Middle
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(3,470,-1);//旋转机械臂归中
	Servo_Add_Action(4,1300,-1);//机械臂展平
	last_mode_servo=0x0E;
    xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

void Servo_InitPos()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(1,2200,-1);//lower disk
	Servo_Add_Action(3,470,-1);//arm to middle
	Servo_Add_Action(4,1300,-1);//lift arm to level
	Servo_Add_Action(2,170,-1);//POS 01
	Servo_Add_Action(5,580,-1);//合上爪子
	last_mode_servo=0x15;
    xSemaphoreGiveRecursive(Servo_Lock_Upper);
}


void Servo_Put3()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(3,940,-1);//1号盘子
	Servo_Add_Action(0xFFF0,37/0.0038,-1);//stub:push stepper
	Servo_Add_Action(0xFFF1,450,-1);//stub:lift stepper
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(3,1500,-1);//机械臂旋转
	Servo_Add_Action(4,1300,-1);//机械臂水平
	Servo_Add_Action(0xFFF1,20/0.0038,-1);//stub:lower stepper
	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(0xFFF1,450,-1);//stub:lift stepper
	last_mode_servo=0x02;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);

}


void Servo_Put2()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(2,550,-1);//1号盘子
	Servo_Add_Action(0xFFF0,37/0.0038,-1);//stub:push stepper
	Servo_Add_Action(0xFFF1,450,-1);//stub:lift stepper
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(3,1500,-1);//机械臂旋转
	Servo_Add_Action(4,1300,-1);//机械臂水平
	Servo_Add_Action(0xFFF1,20/0.0038,-1);//stub:lower stepper
	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(0xFFF1,450,-1);//stub:lift stepper
	last_mode_servo=0x01;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

//1:700 2:380 3: 685 4:190
void Servo_Put1()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(2,170,-1);//1号盘子
	Servo_Add_Action(0xFFF0,37/0.0038,-1);//stub:push stepper
	Servo_Add_Action(0xFFF1,450,-1);//stub:lift stepper
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(3,1500,-1);//机械臂旋转
	Servo_Add_Action(4,1300,-1);//机械臂水平
	Servo_Add_Action(0xFFF1,20/0.0038,-1);//stub:lower stepper
	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(0xFFF1,450,-1);//stub:lift stepper
	last_mode_servo=0x00;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

void Servo_Grab1()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(2,170,-1);//1号盘子
	Servo_Add_Action(0xFFF0,37/0.0038,-1);//stub:push stepper
	Servo_Add_Action(0xFFF1,450,-1);//stub:lift stepper
	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(3,1500,-1);//机械臂旋转
	Servo_Add_Action(4,1300,-1);//机械臂水平
	Servo_Add_Action(0xFFF1,20/0.0038,-1);//stub:lower stepper
	Servo_Add_Action(5,580,-1);//合上爪子
	Servo_Add_Action(0xFFF1,450,-1);//stub:lower stepper
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	last_mode_servo=0x03;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}
void Servo_Grab2()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	
	Servo_Add_Action(2,550,-1);//1号盘子
	Servo_Add_Action(0xFFF0,37/0.0038,-1);//stub:push stepper
	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(3,1500,-1);//机械臂旋转
	Servo_Add_Action(4,1300,-1);//机械臂水平
	Servo_Add_Action(0xFFF1,20/0.0038,-1);//stub:lower stepper
	Servo_Add_Action(5,580,-1);//合上爪子
	Servo_Add_Action(0xFFF1,450,-1);//stub:lower stepper
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	
	last_mode_servo=0x04;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
	
	
}
void Servo_Grab3()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);

	Servo_Add_Action(2,940,-1);//1号盘子
	Servo_Add_Action(0xFFF0,37/0.0038,-1);//stub:push stepper
	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(3,1500,-1);//机械臂旋转
	Servo_Add_Action(4,1300,-1);//机械臂水平
	Servo_Add_Action(0xFFF1,20/0.0038,-1);//stub:lower stepper
	Servo_Add_Action(5,580,-1);//合上爪子
	Servo_Add_Action(0xFFF1,450,-1);//stub:lower stepper
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	last_mode_servo=0x05;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}




void Servo_Grab_Upper()//上层抓取的预备姿势
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(3,470,-1);//机械臂旋转到中间
	Servo_Add_Action(0xFFF0,127/0.0038,-1);//伸出平台
	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(0xFFF1,25/0.0038,-1);//降低平台
	Servo_Add_Action(4,1300,-1);//机械臂水平
	Servo_Add_Action(5,580,-1);//合上爪子
	last_mode_servo=0x06;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
	
}

void Servo_Grab_Lower()//下层抓取姿势 不含爪子闭合 不含平台回升
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(0xFFF0,127/0.0038,-1);//伸出平台
	Servo_Add_Action(3,470,-1);//机械臂旋转到中间
	Servo_Add_Action(4,1300,-1);//机械臂水平
	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(0xFFF1,150/0.0038,-1);//降低平台
	last_mode_servo=0x07;
    xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

void Servo_Grab_Ground()//地面抓取
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(4,1300,-1);//机械臂水平
	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(0xFFF1,150/0.0038,-1);//降低平台
	Servo_Add_Action(5,580,-1);//合上爪子
	Servo_Add_Action(0xFFF1,16/0.0038,-1);//抬高平台
	last_mode_servo=0x07;
    xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

void Servo_Grab()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
    Servo_Add_Action(5,580,-1);
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

void Servo_Release()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
    Servo_Add_Action(5,750,-1);
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}
//2:700 3:450 4:1000
void Servo_Put_Upper()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	last_mode_servo=0x0A;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}



//2: 840  3: 320  4:980  放下的状态
//2: 740  3: 475  4:950  起来的中间状态

//2: 560 3: 140  4: 1000 100: 850  观察圈圈
void Servo_Put_Lower()//地面放置 不含向前移动 建议前移>=100
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(3,470,-1);//机械臂旋转到中间
	Servo_Add_Action(4,1300,-1);//机械臂水平
//	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(0xFFF1,185/0.0038,1);//降低平台
	Servo_Add_Action(5,750,-1);//张开爪子
	Servo_Add_Action(0xFFF1,450,-1);//回升平台
	last_mode_servo=0x0B;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}



//2: 500 3: 140  4: 1000 100: 850
void Servo_Camera()//转向摄像头
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(3,1500,-1);//机械臂右转
	Servo_Add_Action(0xFFF0,127/0.0038,1);//push forward


	last_mode_servo=0x0C;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}
//2: 500 3: 295 4: 870

void Servo_Camera1()//转回来
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	Servo_Add_Action(4,1770,-1);//机械臂抬起
	Servo_Add_Action(3,470,-1);//机械臂左转
//	Servo_Add_Action(0xFFF0,127/0.0038,1);//push backward


	last_mode_servo=0x0F;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}
//670 700 80

void Servo_Camera2()//看二维码
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);

	last_mode_servo=0x10;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

void Servo_Put_Upper_Storage()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);

	xSemaphoreGiveRecursive(Servo_Lock_Upper);

}
void Servo_Put_Lower_Storage()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);

	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

//2:500 3:160 4:900 
void Servo_Camera_AdjPosLower()
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);

	last_mode_servo = 0x14;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

void Servo_Camera3()//看六个物块
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);

	last_mode_servo=0x11;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}

bool Is_Servo_Idle()
{
	xSemaphoreTake(Servo_Lock, portMAX_DELAY);
    bool idle=Current==0;
    xSemaphoreGive(Servo_Lock);
    return idle;
}
void update_Servo_state(uint32_t id,int value)
{
	xSemaphoreTake(Servo_Lock, portMAX_DELAY);
    if(id==0xFFF0)
        stepper_lastvalue1=value;
    else if(id==0xFFF1)
        stepper_lastvalue2=value;
    else
    {
        servo_lastvalue[get_array_seq(id)]=value;
    }
    xSemaphoreGive(Servo_Lock);
}
void change_servo_speed(uint32_t id, double speed)
{
	xSemaphoreTake(Servo_Lock, portMAX_DELAY);
	servo_speed[get_array_seq(id)]=speed;
	xSemaphoreGive(Servo_Lock);
}

void set_last_mode_servo(int value)
{
	xSemaphoreTakeRecursive(Servo_Lock_Upper, portMAX_DELAY);
	last_mode_servo = value;
	xSemaphoreGiveRecursive(Servo_Lock_Upper);
}
