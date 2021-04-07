#ifndef _STEPPERMOTOR_H
#define _STEPPERMOTOR_H
#ifdef __cplusplus
#include <vector>//only for c++ file

 extern "C" {//functions&variables exported to C
#endif
	 void stepper_ISR(void);
	 void set_stepper(int steps);
	 void set_speed(int speed);//实际上是时间，越大越慢
	 int get_speed(void);
	 int get_steps(void);
#ifdef __cplusplus
}

#endif

#endif
