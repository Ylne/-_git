#ifndef __TRACKING_H__
#define __TRACKING_H__
#include "sys.h"

#define Right_Tracking  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)
#define Left_Tracking   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)

void Tracking_Init(void);
void Tracking_detection(void);



 





#endif 

