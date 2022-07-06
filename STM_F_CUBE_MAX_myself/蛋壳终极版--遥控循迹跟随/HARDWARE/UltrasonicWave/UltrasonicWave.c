/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  : UltrasonicWave.c
 * 描述    ：超声波测距模块，UltrasonicWave_Configuration（）函数
             初始化超声模块，UltrasonicWave_StartMeasure（）函数
			 启动测距，并将测得的数据通过串口1打印出来         
 * 实验平台：野火STM32开发板
 * 硬件连接：------------------
 *          | PB1  - TRIG      |
 *          | PB0  - ECHO      |
 *           ------------------
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
*********************************************************************************/
#include "UltrasonicWave.h"
#include "TIM3.h"

#define	TRIG_PORT      GPIOB		//TRIG       
#define	ECHO_PORT      GPIOB		//ECHO 
#define	TRIG_PIN       GPIO_Pin_8   //TRIG       
#define	ECHO_PIN       GPIO_Pin_9	//ECHO   

uint16_t UltrasonicWave_Distance;      //计算出的距离    



/*
 * 函数名：UltrasonicWave_Configuration
 * 描述  ：超声波模块的初始化
 * 输入  ：无
 * 输出  ：无	
 */
void UltrasonicWave_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	
  	       
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
  GPIO_InitStructure.GPIO_Pin = TRIG_PIN;					 //PB1接TRIG
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     //设为推挽输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
  GPIO_Init(TRIG_PORT, &GPIO_InitStructure);	                 //初始化外设GPIO 

  GPIO_InitStructure.GPIO_Pin = ECHO_PIN;				     //PB0接ECH0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		 //设为输入
  GPIO_Init(ECHO_PORT,&GPIO_InitStructure);						 //初始化GPIOB
}

/*
 * 函数名：UltrasonicWave_CalculateTime
 * 描述  ：计算距离
 * 输入  ：无
 * 输出  ：无	
 */

  
void DelayTime_us(int Time)    
{
   unsigned char i;
   for ( ; Time>0; Time--)
     for ( i = 0; i < 72; i++ );
}


/*
 * 函数名：UltrasonicWave_StartMeasure
 * 描述  ：开始测距，发送一个>10us的脉冲，然后测量返回的高电平时间
 * 输入  ：无
 * 输出  ：无	
 */
void UltrasonicWave_StartMeasure(void)
{
  GPIO_SetBits(TRIG_PORT,TRIG_PIN); 		  //送>10US的高电平
  DelayTime_us(20);		                      //延时20US
  GPIO_ResetBits(TRIG_PORT,TRIG_PIN);
  
  while(!GPIO_ReadInputDataBit(ECHO_PORT,ECHO_PIN));	             //等待高电平
  TIM_Cmd(TIM3, ENABLE);                                             //开启时钟
  while(GPIO_ReadInputDataBit(ECHO_PORT,ECHO_PIN));	                 //等待低电平
  TIM_Cmd(TIM3, DISABLE);			                                 //定时器3失能
  UltrasonicWave_Distance=TIM_GetCounter(TIM3)*5*34/200;		     //计算距离					 //计算距离
  TIM_SetCounter(TIM3,0);
	  
}
/************************END OF FILE************/
