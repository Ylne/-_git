#ifndef __XUNJI_H
#define	__XUNJI_H

#include "stm32f10x.h"

#define foundroad_L GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)    //��
#define foundroad_R GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)    //��


void foundroad_config(void);

#endif


