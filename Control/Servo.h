#ifndef _SERVO_H
#define _SERVO_H
#ifdef __cplusplus
extern "C" {
#endif
void Servo_Server();
void Servo_Add_Action(uint32_t id,uint32_t value,int32_t time);
void Servo_TransPos();
void Servo_Put1();
void Servo_Put2();
void Servo_Put3();
void Servo_Grab1();
void Servo_Grab2();
void Servo_Grab3();
void Servo_Grab_Upper();
void Servo_Grab_Lower();
void Servo_Grab();
void Servo_Release();
void Servo_Put_Upper();
void Servo_Put_Lower();
void All_Middle();
void Servo_Camera();
void Servo_Camera1();
void Servo_Camera2();
void Servo_Camera3();
void Servo_Put_Upper_Storage();
void Servo_Put_Lower_Storage();
void Servo_Camera_AdjPosLower();
bool Is_Servo_Idle();
void update_Servo_state(uint32_t id,int value);
void change_servo_speed(uint32_t id, double speed);
void Servo_InitPos();
#ifdef __cplusplus
}
#endif
#endif
