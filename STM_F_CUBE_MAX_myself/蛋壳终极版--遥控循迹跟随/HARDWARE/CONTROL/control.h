#ifndef  _CONTROL_H
#define  _CONTROL_H

#include "sys.h" 




int Vertical(float Med,float Angle,float gyro_Y);
int Velocity(int Target,int encoder_left,int encoder_right);
int Turn(int gyro_Z,int RC); 
void EXTI9_5_IRQHandler(void);

void keyboard(void);
void Tracking_detection(void);
void oledshow(void);
void Wave(void);
void chaoshengbo(void);
#endif

