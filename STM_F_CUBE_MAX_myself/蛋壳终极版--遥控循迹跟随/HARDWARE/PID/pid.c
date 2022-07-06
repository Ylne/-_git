#include "pid.h"
 PID pid;

/**************************************************************************
�������ܣ�PID���ݳ�ʼ��
��ڲ�������
����  ֵ����
**************************************************************************/
void PID_Init()
{
	          /*ƽ��PID�����Ʋ�����ʼ��*/
		pid.Med_Angle =  0;		
		pid.Kp = 90;
		pid.Kd = 4;  
											 // 35 2
							/*�ٶ�PID�����Ʋ�����ʼ��*/		
		pid.Kp_speed = 155;    
		pid.Ki_speed = 0.4;
		pid.EK_speed = 0;
		pid.SEK_speed = 1;
	  pid.EK_speedLowout=0;
	  pid.EK_speedLowout_last=0;
		pid.left = 0;
		pid.right = 0;				//�ٶȳ�ʼֵ
		                  //65 2
							/*ת��PID�����Ʋ�����ʼ��*/
		pid.Kp_turn =35;
		pid.Angle_turn = 0;
}

/**************************************************************************
�������ܣ����������������ֱ�����������PIDֵ��������
��ڲ�����δ֪
����  ֵ����
**************************************************************************/


				/*С��ƽ�⻷���֣�΢��+��������
	        ΢�ֱ���Ϊֱ�Ӷ�ȡ�ļ��ٶ�*/
extern short gyrox;
int balance(float Angle)
{  

    signed  int  Bias;
	  int balance;
	  Bias=(Angle-pid.Med_Angle);
	  balance=pid.Kp*Bias+gyrox*pid.Kd;  
	  return balance;     
}

				/*С���ٶȻ����֣� ����+��������*/
int velocity(int Target,int encoder_left,int encoder_right)
{    
	    
		int velocity;			
		pid.EK_speed = ((encoder_left+encoder_right)-Target);
	  pid.EK_speedLowout=(1-a)*pid.EK_speed+a*pid.EK_speedLowout_last;
  	pid.EK_speedLowout_last
	 	pid.SEK_speed += pid.EK
	_speed;
		velocity = pid.Kp_speed*pid.EK_speed +(int)(pid.Ki_speed*pid.SEK_speed);
		return velocity;
}

				/*С��ת�򻷲��֣���������*/
int turn(float gyro)//ת�����
{
    int Turn;
	  float Bias;
			
			if(pid.Angle_turn <= -180&&gyro>0)
				Bias = (gyro-(pid.Angle_turn+360))*10;	
			else if(pid.Angle_turn <= -360&&gyro<0)
				Bias = (gyro-(pid.Angle_turn+360))*10;	
			else if(pid.Angle_turn >= 180&&gyro<0)
				 Bias = (gyro-(pid.Angle_turn-360))*10;
			else if(pid.Angle_turn >= 360&&gyro>0)
					Bias = (gyro-(pid.Angle_turn-360))*10;
		  else
				 Bias = (gyro-pid.Angle_turn)*10;	

	  Turn = (int)(Bias*pid.Kp_turn); 
	
				/*����ת���ٶȵĵ����޷�*/
		if(Turn>=1500)	Turn =1500;
		else if(Turn<=-1500)Turn =-1500;
			
		return Turn;
}
