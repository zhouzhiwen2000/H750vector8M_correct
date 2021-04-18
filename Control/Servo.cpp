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
double servo_speed[10]={0.5,0.8,0.8,0.8,0.8,0.8,0.8,0.8,0.5,0.5};//{none,1,2,3,4,100,101,none....}
int servo_lastvalue[10]= {0};
uint32_t get_array_seq(uint32_t id)
{
	if(id>=100)id=id-95;
	return id;
}
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

void Servo_Add_Action_bunch(std::vector<_act>  acts,int32_t time)
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
	Servo_Add_Action(1,529,-1);
	Servo_Add_Action(2,266,-1);
	Servo_Add_Action(3,717,-1);
	Servo_Add_Action(4,950,-1);
	Servo_Add_Action(100,765,-1);
}

void All_Middle()//grab&to Middle
{
    Servo_Add_Action(100,765,-1);
    Servo_Add_Action(0xFFF0,462,-1);
}



void Servo_PutLeft()
{
	Servo_Add_Action(2,266,-1);
    Servo_Add_Action(3,717,-1);//transion

    Servo_Add_Action(1,385,-1);
	Servo_Add_Action(2,500,-1);
    Servo_Add_Action(3,685,-1);
	Servo_Add_Action(4,200,-1);
    Servo_Add_Action(2,415,-1);
    Servo_Add_Action(101,850,-1);
	
	Servo_Add_Action(2,500,-1);
	Servo_Add_Action(4,950,-1);
	Servo_Add_Action(3,717,-1);
	Servo_Add_Action(2,266,-1);
	Servo_Add_Action(1,529,-1);

}


void Servo_PutMiddle()
{
	Servo_Add_Action(0xFFF0,850,-1);
	
	Servo_Add_Action(100,765,-1);

	Servo_Add_Action(2,460,500);
    Servo_Add_Action(1,529,500);
    Servo_Add_Action(3,717,500);
	Servo_Add_Action(4,500,500);
    Servo_Add_Action(4,135,1000);
	
	Servo_Add_Action(101,850,-1);
	
	Servo_Add_Action(4,950,1000);
	Servo_Add_Action(0xFFF0,450,-1);//Stepper Middle
	Servo_TransPos();
	
}

//1:685 2:380 3: 685 4:190
void Servo_PutRight()
{	
	Servo_Add_Action(2,266,-1);
    Servo_Add_Action(3,717,-1);//transion

    Servo_Add_Action(1,695,-1);
	Servo_Add_Action(2,500,-1);
    Servo_Add_Action(3,685,-1);
	Servo_Add_Action(4,200,-1);
    Servo_Add_Action(2,415,-1);
    Servo_Add_Action(101,850,-1);
	
	Servo_Add_Action(2,500,-1);
	Servo_Add_Action(4,950,-1);
	Servo_Add_Action(3,717,-1);
	Servo_Add_Action(2,266,-1);
	Servo_Add_Action(1,529,-1);
}

void Servo_GrabLeft()
{
//    Servo_Add_Action(2,266,-1);
//    Servo_Add_Action(3,717,-1);//transion
	Servo_Add_Action(100,765,-1); 
	Servo_Add_Action(101,850,-1);

    Servo_Add_Action(1,695,-1);
	Servo_Add_Action(2,500,-1);
    Servo_Add_Action(3,685,-1);
	Servo_Add_Action(4,200,-1);
    Servo_Add_Action(2,415,-1);
    Servo_Grab();
	
	Servo_Add_Action(2,500,-1);
	Servo_Add_Action(4,950,-1);
	Servo_Add_Action(3,717,-1);
	Servo_Add_Action(2,266,-1);
	Servo_Add_Action(1,529,-1);

}
void Servo_GrabMiddle()
{
	Servo_Add_Action(100,765,-1);
	Servo_Add_Action(101,850,-1);
	Servo_Add_Action(0xFFF0,850,-1);
	
//	Servo_Add_Action(100,765,-1);
//	Servo_Add_Action(101,850,-1);
	
	Servo_Add_Action(2,460,-1);
    Servo_Add_Action(1,529,-1);
    Servo_Add_Action(3,717,-1);
	Servo_Add_Action(4,500,-1);
    Servo_Add_Action(4,135,-1);
	
	Servo_Grab();
	
	Servo_Add_Action(4,950,-1);
	Servo_Add_Action(0xFFF0,450,-1);//Stepper Middle
	Servo_TransPos();
	
	
	
}
void Servo_GrabRight()
{
	//	Servo_Add_Action(2,266,-1);
//    Servo_Add_Action(3,717,-1);//transion
	Servo_Add_Action(100,765,-1);
	Servo_Add_Action(101,850,-1);

    Servo_Add_Action(1,385,-1);
	Servo_Add_Action(2,500,-1);
    Servo_Add_Action(3,685,-1);
	Servo_Add_Action(4,200,-1);
    Servo_Add_Action(2,415,-1);
    Servo_Grab();
	
	Servo_Add_Action(2,500,-1);
	Servo_Add_Action(4,950,-1);
	Servo_Add_Action(3,717,-1);
	Servo_Add_Action(2,266,-1);
	Servo_Add_Action(1,529,-1);
}




void Servo_Grab_Upper()//done
{
	
	Servo_Add_Action(2,266,-1);
    Servo_Add_Action(3,717,-1);//transion
    Servo_Add_Action(0xFFF0,450,-1);
    Servo_Add_Action(101,850,-1);
    Servo_Add_Action(100,765,-1);
    Servo_Add_Action(1,529,-1);
    Servo_Add_Action(3,660,-1);
	Servo_Add_Action(4,888,-1);
    Servo_Add_Action(2,590,-1);
    Servo_Grab();
	
	Servo_TransPos();
	
	
}

void Servo_Grab_Pose_Lower()//抓地上的物块
{
	Servo_Add_Action(100,765,-1);
    Servo_Add_Action(101,850,-1);
	Servo_Add_Action(3,717,-1);
	Servo_Add_Action(4,950,-1);
	Servo_Add_Action(2,266,-1);
	Servo_Add_Action(3,475,-1);
	Servo_Add_Action(2,740,-1);
	Servo_Add_Action(3,400,-1);
	Servo_Add_Action(2,790,-1);
	Servo_Add_Action(2,840,-1);
    Servo_Add_Action(4,980,-1);
	Servo_Add_Action(3,320,-1);
	
    Servo_Grab();
	
    Servo_Add_Action(2,266,-1);
	Servo_Add_Action(4,950,-1);
    Servo_Add_Action(3,717,-1);
}
void Servo_Grab_Pose2_Lower()//done
{
    Servo_Add_Action(2,955,-1);//was 970
//	Servo_Add_Action(101,580,1000);
}
void Servo_Grab()//done
{
    Servo_Add_Action(101,580,-1);
}

//2:700 3:450 4:1000
void Servo_Put_Upper()
{
	Servo_Add_Action(4,1000,-1);
    Servo_Add_Action(3,450,-1);
    Servo_Add_Action(2,700,-1);
    Servo_Add_Action(101,850,-1);

	Servo_Add_Action(3,600,-1);
    Servo_Add_Action(2,266,-1);
	Servo_Add_Action(3,717,-1);
	Servo_Add_Action(4,950,-1);

}

//2: 840  3: 320  4:980  放下的状态
//2: 740  3: 475  4:950  起来的中间状态
void Servo_Put_Lower()
{
	
//    Servo_Add_Action(2,266,-1);
//    Servo_Add_Action(3,717,-1);//transion
	
//    Servo_Add_Action(0xFFF0,700,-1);
//    Servo_Add_Action(100,765,-1);
    Servo_Add_Action(1,529,-1);
	
    Servo_Add_Action(3,320,-1);
	Servo_Add_Action(4,980,-1);
    Servo_Add_Action(2,840,-1);
    Servo_Add_Action(101,765,-1);

	Servo_Add_Action(2,790,-1);
	Servo_Add_Action(3,400,-1);
	Servo_Add_Action(2,740,-1);
	Servo_Add_Action(3,475,-1);
	
    Servo_Add_Action(2,266,-1);
	Servo_Add_Action(4,950,-1);
    Servo_Add_Action(3,717,-1);

}

//2: 560 3: 140  4: 1000 100: 850
void Servo_Camera()//done
{
    Servo_Add_Action(2,266,-1);
    Servo_Add_Action(3,717,-1);//transion
//    Servo_Add_Action(0xFFF0,100,-1);
//    Servo_Add_Action(101,765,-1);
//    Servo_Add_Action(100,765,-1);
    Servo_Add_Action(1,529,-1);
	
	Servo_Add_Action(4,1000,-1);
	Servo_Add_Action(3,420,-1);
	Servo_Add_Action(2,560,-1);
    Servo_Add_Action(3,140,-1);
    
	Servo_Add_Action(100,765,-1);
}
//2: 500 3: 295 4: 870

void Servo_Camera1()//看台子上面的物块
{
//    Servo_Add_Action(2,266,-1);
//    Servo_Add_Action(3,717,-1);//transion
	
	
//    Servo_Add_Action(0xFFF0,100,-1);
//    Servo_Add_Action(101,850,-1);
//    Servo_Add_Action(100,765,-1);
//    Servo_Add_Action(1,529,-1);
	Servo_Add_Action(4,870,-1);
    Servo_Add_Action(3,295,-1);
	Servo_Add_Action(2,500,-1);
	
	Servo_Add_Action(100,765,-1);

}

void Servo_Camera2()//done
{
    Servo_Add_Action(1,110,-1);
	
	
	Servo_Add_Action(4,1000,-1);
	Servo_Add_Action(3,420,-1);
	Servo_Add_Action(2,560,-1);
  Servo_Add_Action(3,140,-1);
}

void Servo_Put_Upper_Storage()
{

}
void Servo_Put_Lower_Storage()
{

}

void Servo_Camera3()
{

}

bool Is_Servo_Idle()
{
    return Current==0;
}
void update_Servo_state(uint32_t id,int value)
{
    if(id!=0xFFF0)
        stepper_lastvalue=value;
    else
    {
        servo_lastvalue[get_array_seq(id)]=value;
    }
}
void change_servo_speed(uint32_t id, double speed)
{
	servo_speed[id]=speed;
}
