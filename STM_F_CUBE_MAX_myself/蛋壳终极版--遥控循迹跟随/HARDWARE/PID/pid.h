#ifndef __PID_H__
#define __PID_H__
#include "sys.h"
//PID�ṹ��
typedef struct 
{
	signed int  Med_Angle;			//�û��趨ƽ��λ��ֵ
	float Pv;
					/*ֱ���������趨 */
	float  Kd;					//ƽ��΢����ϵ��
	signed int  Kp;			//ƽ�������ϵ��
	

	
					/*�ٶȻ������趨 */
	signed int left;      //Ŀ�����ٶ�
	signed int right;     //Ŀ�����ٶ�
	float  Ki_speed;				//�ٶȻ�������ϵ��
	signed int Kp_speed;		//�ٶȻ�������ϵ��
	signed int EK_speed;  	//�ٶ�ƫ��
	signed int SEK_speed;   //��ʷƫ��֮��
	static int EK_speedLowout;  	//�ٶ�ƫ��
  static int EK_speedLowout_last;  	//�ٶ�ƫ��
					/*ת�򻷲����趨 */
	float Kp_turn;					//ת�򻷱�����ϵ��
	float Angle_turn;				//Ŀ��ת��Ƕ�
	
}PID;

extern PID pid;

int balance(float Angle);
void PID_Init(void);
int velocity(int Target,int encoder_left,int encoder_right);
int turn(float gyro);

#endif
