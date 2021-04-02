#ifndef _STEPPERMOTOR_H
#define _STEPPERMOTOR_H
#ifdef __cplusplus
#include <vector>//only for c++ file

 extern "C" {//functions&variables exported to C
#endif
	 void stepper_ISR(void);
	 void set_stepper(int time,int steps);
	 void set_stepper_abs(int steps);
#ifdef __cplusplus
}

#endif

#endif
