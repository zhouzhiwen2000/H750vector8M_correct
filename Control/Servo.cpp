extern "C" {//functions&variables imported from C 
#include "SCSProtocol.h"
#include "stm32h7xx_hal.h"
}
#include "Servo.h"
#include "StepperMotor.h"
#include <vector>
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
};

action_servo * Current=0;
action_servo * Last=0;
_act * ptr_test;
uint32_t time_start;
int stepper_lastvalue=0;
double servo_speed=1;
uint32_t servo_lastvalue[10]={0};
void Servo_Server()
{
	if(Current!=0)
	{
		if(HAL_GetTick()-time_start>=Current->time)
		{
			action_servo * del=Current;
			Current=Current->Next;
			delete(del);
			if(Current!=0)
			{
				while(!Current->acts.empty())
				{
					_act  action=(Current->acts.front());;
					ptr_test=&(Current->acts.front());
					if(action.id!=0xFFF0)
						WritePos(action.id,action.value,Current->time,0);
					else
						set_stepper(action.value);
					Current->acts.erase(Current->acts.begin());
				}
//				WritePos(Current->id,Current->value,Current->time,0);
				time_start=HAL_GetTick();
			}
		}
	}
}

void Servo_Add_Action(uint32_t id,uint32_t value,int32_t time)
{
	action_servo * New=new action_servo() ;
//	New->id=id;
//	New->value=value;
	_act action1;
	action1.id=id;
	action1.value=value;
	New->acts.push_back(action1);
	New->time=time;
	New->Next=0;
	if(action1.id==0xFFF0)
	{
		if(Current==0)
		{
			stepper_lastvalue=get_steps();//update steps
		}
		int stepper_time=fabs((int)value-(int)stepper_lastvalue)*2.0*get_speed()/10;//in ms
		New->time=stepper_time;
		stepper_lastvalue=value;
	}
	
	if(time==-1)
	{
		int servo_time=fabs((int)value-(int)servo_lastvalue[id])/servo_speed;//in ms
		New->time=servo_time;
		servo_lastvalue[id]=value;
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
			if(action.id!=0xFFF0)
				WritePos(action.id,action.value,Current->time,0);
			else
			{
				set_stepper(action.value);
			}
			Current->acts.erase(Current->acts.begin());
		}
//		WritePos(Current->id,Current->value,Current->time,0);
		time_start=HAL_GetTick();
	}
	
}

void Servo_Add_Action_bunch(std::vector<_act>  acts,uint32_t time)
{
	action_servo * New=new(action_servo);
	New->acts=acts;
	New->time=time;
	New->Next=0;
	
	std::vector<_act>::iterator it = acts.begin();
	for(; it != acts.end(); ++it)
	{
		_act & act=*it;
		if(act.id==0xFFF0)
		{
			if(Current==0)
			{
					stepper_lastvalue=get_steps();//update steps
			}
			int stepper_time=fabs(act.value-stepper_lastvalue)*2.0*get_speed()/10;//in ms
			if(stepper_time>time)
			{
				New->time=stepper_time;
			}
			stepper_lastvalue=act.value;
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
			if(action.id!=0xFFF0)
				WritePos(action.id,action.value,Current->time,0);
			else
				set_stepper(action.value);
			Current->acts.erase(Current->acts.begin());
		}
		time_start=HAL_GetTick();
	}
}

void Servo_TransPos()//grab&to Middle
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);
}

void All_Middle()//grab&to Middle
{
	Servo_Add_Action(100,765,500);
	Servo_Add_Action(0xFFF0,462,3000);
}



void Servo_PutLeft()
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,50,3000);
	Servo_Add_Action(100,840,500);
	Servo_Add_Action(1,336,1500);
	Servo_Add_Action(3,985,1000);	
	Servo_Add_Action(2,22,1000);
	Servo_Add_Action(101,765,1000);
	Servo_TransPos();
}
void Servo_PutMiddle()
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,800,3000);
	Servo_Add_Action(100,765,500);
	Servo_Add_Action(1,529,1500);
	Servo_Add_Action(3,1003,1000);	
	Servo_Add_Action(2,30,1000);
	Servo_Add_Action(101,765,1000);
	Servo_TransPos();
}
void Servo_PutRight()
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,50,3000);
	Servo_Add_Action(100,666,500);
	Servo_Add_Action(1,720,1500);
	Servo_Add_Action(3,980,1000);	
	Servo_Add_Action(2,24,1000);
	Servo_Add_Action(101,765,1000);
	Servo_TransPos();
}
void Servo_GrabLeft()
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,50,3000);
	Servo_Add_Action(101,765,500);
	Servo_Add_Action(100,840,500);
	Servo_Add_Action(1,336,1500);
	Servo_Add_Action(3,985,1000);	
	Servo_Add_Action(2,50,1000);
	Servo_Add_Action(101,580,1000);
	Servo_TransPos();
}
void Servo_GrabMiddle()
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,800,3000);
	Servo_Add_Action(101,765,500);
	Servo_Add_Action(100,765,500);
	Servo_Add_Action(1,529,1500);
	Servo_Add_Action(3,1003,1000);	
	Servo_Add_Action(2,50,1000);
	Servo_Add_Action(101,580,1000);
	Servo_TransPos();
}
void Servo_GrabRight()
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,50,3000);
	Servo_Add_Action(101,765,500);
	Servo_Add_Action(100,666,500);
	Servo_Add_Action(1,720,1500);
	Servo_Add_Action(3,980,1000);	
	Servo_Add_Action(2,50,1000);
	Servo_Add_Action(101,580,1000);
	Servo_TransPos();
}



/******************To be done********************/
void Servo_Grab_Upper()//done
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,900,5000);
	Servo_Add_Action(101,765,500);
	Servo_Add_Action(100,765,500);
	Servo_Add_Action(1,529,1500);
	Servo_Add_Action(3,1023,1000);	
	Servo_Add_Action(2,674,1000);
	Servo_Add_Action(101,580,1000);
//	Servo_TransPos();
}

void Servo_Grab_Pose_Lower()//done
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,50,5000);
	Servo_Add_Action(101,765,500);
	Servo_Add_Action(100,765,500);
	Servo_Add_Action(1,529,1500);
	Servo_Add_Action(3,865,1000);	
	Servo_Add_Action(2,945,1000);
//	Servo_Add_Action(0xFFF0,900,5000);
//	Servo_Add_Action(101,580,1000);
}
void Servo_Grab_Pose2_Lower()//done
{
	Servo_Add_Action(2,955,1000);//was 970
//	Servo_Add_Action(101,580,1000);
}
void Servo_Grab()//done
{
	Servo_Add_Action(101,580,1000);
}

void Servo_Put_Upper()
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,700,3000);
	Servo_Add_Action(100,765,500);
	Servo_Add_Action(1,529,1500);
	Servo_Add_Action(3,977,2000);	
	Servo_Add_Action(2,885,2000);
	Servo_Add_Action(101,765,500);
  Servo_Add_Action(0xFFF0,50,3000);
	Servo_Add_Action(2,500,1000);	
//	Servo_Add_Action(3,940,500);
//	Servo_TransPos();
}

void Servo_Put_Lower()
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,700,3000);
	Servo_Add_Action(100,765,500);
	Servo_Add_Action(1,529,1500);
	Servo_Add_Action(3,799,2000);	
	Servo_Add_Action(2,1021,2000);
	Servo_Add_Action(101,765,500);
  Servo_Add_Action(0xFFF0,50,3000);
	Servo_Add_Action(2,990,500);	
	Servo_Add_Action(3,880,500);
//	Servo_TransPos();
	
}
void Servo_Camera()//done
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,100,3000);
	Servo_Add_Action(101,765,500);
	Servo_Add_Action(100,765,500);
	Servo_Add_Action(1,529,1500);
	Servo_Add_Action(3,1020,100);	
	Servo_Add_Action(2,460,500);
}

void Servo_Camera1()//done
{
	Servo_Add_Action(2,266,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,100,3000);
	Servo_Add_Action(101,765,500);
	Servo_Add_Action(100,765,500);
	Servo_Add_Action(1,529,1500);
	Servo_Add_Action(3,1005,1000);
	Servo_Add_Action(2,460,500);
}
void Servo_Camera2()//done
{
	Servo_Add_Action(2,330,1000);
	Servo_Add_Action(3,1020,1000);//transion
  Servo_Add_Action(0xFFF0,100,3000);
	Servo_Add_Action(101,765,500);
	Servo_Add_Action(100,765,500);
}
