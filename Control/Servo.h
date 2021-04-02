#ifndef _SERVO_H
#define _SERVO_H
#ifdef __cplusplus
extern "C" {
#endif
void Servo_Server();
void Servo_Add_Action(uint32_t id,uint32_t value,uint32_t time);
void Servo_InitPos();
void Servo_TransPos();
void Servo_PutLeft();
void Servo_PutMiddle();
void Servo_PutRight();
void Servo_GrabLeft();
void Servo_GrabMiddle();
void Servo_GrabRight();
void Servo_Grab_Upper();
void Servo_Grab_Pose_Lower();
void Servo_Grab();
void Servo_Put_Upper();
void Servo_Put_Lower();
void Servo_Grab_Pose2_Lower();
void All_Middle();
void Servo_Camera();
#ifdef __cplusplus
}
#endif
#endif