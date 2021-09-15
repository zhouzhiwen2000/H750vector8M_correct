#ifndef _STEPPERMOTOR_H
#define _STEPPERMOTOR_H
#ifdef __cplusplus
#include <vector>//only for c++ file

 extern "C" {//functions&variables exported to C
#endif
	 void stepper1_ISR(void);
	 void set_stepper_1(int steps);
	 void set_speed_1(int speed);//实际上是时间，越大越慢
	 int get_speed_1(void);
	 int get_steps_1(void);
	 void stepper2_ISR(void);
	 void set_stepper_2(int steps);
	 void set_speed_2(int speed);//实际上是时间，越大越慢
	 int get_speed_2(void);
	 int get_steps_2(void);
#ifdef __cplusplus
}

#endif

#endif
