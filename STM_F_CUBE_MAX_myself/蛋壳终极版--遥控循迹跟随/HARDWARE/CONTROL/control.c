#include "control.h"
#include "motor.h"

/*
0x00��ɲ��
0x01��ǰ��
0x05������
0x03����ת
0x07����ת
*/
u8 A;
u8 Mode_flag;					//ģʽѡ���־λ
float Med_Angle=-12;	//��е��ֵ��---�������޸���Ļ�е��ֵ���ɡ�
float Target_Speed=0;	//�����ٶȣ���������---���ο����ӿڣ����ڿ���С��ǰ�����˼����ٶȡ�
float Turn_Speed=0;		//�����ٶȣ�ƫ����
float 
Vertical_Kp=-630//420*0.6=252     770*0.6=462   -1300*0.6=-780    -870  -1050*0.6
,//ֱ����KP��KD
Vertical_Kd=-1.85;    //1*0.6     2.3*0.6=1.38  -3*0.6=-1.8   -4.2*0.6  -3*0.6
float 
	Velocity_Kp=0.24,//�ٶȻ�KP��KI  0.21   0.45
	Velocity_Ki=0.0012;      ////   0.0015   0.004
float 
	Turn_Kd=-0.7,//ת��KP��KD -0.7
	Turn_Kp=20;//20
int Vertical_out,Velocity_out,Turn_out;//ֱ����&�ٶȻ�&ת�� ���������

int Vertical(float Med,float Angle,float gyro_Y);//��������
int Velocity(int Target,int encoder_left,int encoder_right);
int Turn(int gyro_Z,int RC); 

//void chaoshengbo(void);

#define SPEED_X 30 //����(ǰ��)����趨�ٶ�
#define SPEED_Z 100//ƫ��(����)����趨�ٶ� 




void EXTI9_5_IRQHandler(void)
{
	int PWM_out;
	if(EXTI_GetITStatus(EXTI_Line5)!=0)//һ���ж�
	{
		if(PBin(5)==0)//�����ж�
		{
			EXTI_ClearITPendingBit(EXTI_Line5);//����жϱ�־λ
			
			//1.�ɼ�����������&MPU6050�Ƕ���Ϣ��
			Encoder_Left=-Read_Speed(2);//�������԰�װ���պ����180�ȣ�Ϊ�˱������������һ�£�����Ҫ������һ��ȡ����
			Encoder_Right=Read_Speed(4);
			
	 		mpu_dmp_get_data(&Pitch,&Roll,&Yaw);			//�Ƕ�
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//������
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//���ٶ�
			
/******************************************************����**********************************************/

 	if(Mode_flag==3)
	{	 
		UltrasonicWave_StartMeasure();
		
	 	if(UltrasonicWave_Distance<=70)Target_Speed=10;
		else if(UltrasonicWave_Distance>100&&UltrasonicWave_Distance<=200)Target_Speed=-10;
		else Target_Speed=0;
	}	
/*********************************************************************************************************/		
			
			
			
			
/******************************************************ѭ��***********************************************/
//	  if(Left_Tracking==0&&Right_Tracking==1){Turn_Kd=0;Turn_Speed=15;Target_Speed=10;}
//    if(Left_Tracking==1&&Right_Tracking==0){Turn_Kd=0;Turn_Speed=-15;Target_Speed=10;}
//		else Turn_Kd=-0.6;Target_Speed=8;

//    Turn_Speed=Turn_Speed>SPEED_Z?SPEED_Z:(Turn_Speed<-SPEED_Z?(-SPEED_Z):Turn_Speed);//�޷�( (20*100) * 100   )
//      
   if(Mode_flag==2)
    Tracking_detection();
//			
		  
/*********************************************************************************************************/			
		 
			
			
/******************************************************����***********************************************/	
	else if(Mode_flag==1)
		/*ǰ��*/
	{		if((Fore==0)&&(Back==0))Target_Speed=0;//δ���ܵ�ǰ������ָ��-->�ٶ����㣬����ԭ��
			if(Fore==1)Target_Speed=-25;// ǰ��1��־λ����-->��Ҫǰ��
			if(Back==1)Target_Speed=25;//
			Target_Speed=Target_Speed>SPEED_X?SPEED_X:(Target_Speed<-SPEED_X?(-SPEED_X):Target_Speed);//�޷�
			
			/*����*/
			if((Left==0)&&(Right==0))Turn_Speed=0;
			if(Left==1)Turn_Speed-=50;	//��ת
			if(Right==1)Turn_Speed+=50;	//��ת
			Turn_Speed=Turn_Speed>SPEED_Z?SPEED_Z:(Turn_Speed<-SPEED_Z?(-SPEED_Z):Turn_Speed);//�޷�( (20*100) * 100   )
			
			/*ת��Լ��*/
			if((Left==0)&&(Right==0))Turn_Kd=-0.7;//��������ת��ָ�����ת��Լ��
			else if((Left==1)||(Right==1))Turn_Kd=0;//������ת��ָ����յ�����ȥ��ת��Լ��
	}
/******************************************************����*************************************************/
		
			//2.������ѹ��ջ������У�����������������
		 
			Velocity_out=Velocity(Target_Speed,Encoder_Left,Encoder_Right);	//�ٶȻ�
			Vertical_out=Vertical(Velocity_out+Med_Angle,Roll,gyrox);			//ֱ����
			Turn_out=Turn(gyroz,Turn_Speed);																//ת��												
			//ת��

			PWM_out=Vertical_out;//�������
			//3.�ѿ�����������ص�����ϣ�������յĵĿ��ơ�
			MOTO1=PWM_out-Turn_out;//����
			MOTO2=PWM_out+Turn_out;//�ҵ��
			Limit(&MOTO1,&MOTO2);	 //PWM�޷�			
			Load(MOTO1,MOTO2);		 //���ص�����ϡ�
			
//			Stop(&Med_Angle,&Roll);
			
		}
	}
}




/*********************
ֱ����PD��������Kp*Ek+Kd*Ek_D

��ڣ������Ƕȡ���ʵ�Ƕȡ���ʵ���ٶ�
���ڣ�ֱ�������
*********************/
int Vertical(float Med,float Angle,float gyro_X)
{
	int PWM_out;
	
	PWM_out=Vertical_Kp*(Angle-Med)+Vertical_Kd*(gyro_X-0);
	return PWM_out;
}


/*********************
�ٶȻ�PI��Kp*Ek+Ki*Ek_S
*********************/
int Velocity(int Target,int encoder_left,int encoder_right)
{
	static int Encoder_S,EnC_Err_Lowout_last,PWM_out,Encoder_Err,EnC_Err_Lowout;
	float a=0.7;
	
	//1.�����ٶ�ƫ��
	Encoder_Err=((encoder_left+encoder_right)-Target);//��ȥ���--�ҵ���⣺�ܹ����ٶ�Ϊ"0"�ĽǶȣ����ǻ�е��ֵ��
	//2.���ٶ�ƫ����е�ͨ�˲�
	//low_out=(1-a)*Ek+a*low_out_last;
	EnC_Err_Lowout=(1-a)*Encoder_Err+a*EnC_Err_Lowout_last;//ʹ�ò��θ���ƽ�����˳���Ƶ���ţ���ֹ�ٶ�ͻ�䡣
	EnC_Err_Lowout_last=EnC_Err_Lowout;//��ֹ�ٶȹ����Ӱ��ֱ����������������
	//3.���ٶ�ƫ����֣����ֳ�λ��
	Encoder_S+=EnC_Err_Lowout;
	//4.�����޷�
	Encoder_S=Encoder_S>10000?10000:(Encoder_S<(-10000)?(-10000):Encoder_S);
	
	//5.�ٶȻ������������
	PWM_out=Velocity_Kp*EnC_Err_Lowout+Velocity_Ki*Encoder_S;
	return PWM_out;
}


/*********************
ת�򻷣�ϵ��*Z����ٶ�+ϵ��*ң������
*********************/
int Turn(int gyro_Z,int RC)
{
	int PWM_out;
	//�ⲻ��һ���ϸ��PD��������Kd��Ե���ת���Լ������Kp��Ե���ң�ص�ת��
	PWM_out=Turn_Kd*gyro_Z + Turn_Kp*RC;
	return PWM_out;
}









/**************************************************************************
�������ܣ�ѭ��ģ�飬�߼�����
��ڲ�������
����  ֵ����
Turn_Speed+ ��ת  Turn_Speed- ��ת
**************************************************************************/	

void Tracking_detection(void)
{

//	if(Left_Tracking==0&&Right_Tracking==0)      //ֱ��
//	{
//	  Turn_Kd=-0.7,Target_Speed=-20;
//	}
//	else if(Left_Tracking==0&&Right_Tracking==1)   //��ת
//	{
//		 Turn_Kd=0,Turn_Speed=-20;Target_Speed=-20;
//	}
//	else if(Left_Tracking==1&&Right_Tracking==0)   //��ת
//		
//	{
//		Turn_Kd=0,Turn_Speed=20;Target_Speed=-20;
//	}
//	else if(Left_Tracking==1&&Right_Tracking==1)  //ֱ��
//	{
//		Turn_Kd=-0.7,Target_Speed=-20;
//	}

	
	
	if(Left_Tracking==0&&Right_Tracking==0)      //ֱ��
	{
	  Turn_Kd=-0.7,Target_Speed=-15;
	}
	else if(Left_Tracking==0&&Right_Tracking==1)   //��ת
	{
		 Turn_Kd=0,Turn_Speed=-30;Target_Speed=-15;
	}
	else if(Left_Tracking==1&&Right_Tracking==0)   //��ת
		
	{
		Turn_Kd=0,Turn_Speed=30;Target_Speed=-15;
	}
	else if(Left_Tracking==1&&Right_Tracking==1)  //ֱ��
	{
		Turn_Kd=-0.7,Target_Speed=-15;
	}
		
}

//u8 string[10] = {0};	
void oledshow(void)

{	 
//	  sprintf((char *)string,"Roll :%.2f",Roll);//0300
//		OLED_ShowString(0,0,string,8);
//  	sprintf((char *)string,"Kp:%.2f",Vertical_Kp);//0300
//		OLED_ShowString(0,3,string,8);
//		sprintf((char *)string,"Vertical_Kd:%.2f",Vertical_Kd);//0300
//		OLED_ShowString(0,4,string,8);
//		sprintf((char *)string,"Velocity_Kp:%.2f",Velocity_Kp);//0300
//		OLED_ShowString(0,5,string,8);
//		sprintf((char *)string,"Turn_Kd :%.2f",Turn_Kd);//0300
//		OLED_ShowString(0,6,string,8);
	OLED_ShowCHinese(25,0,8);
	OLED_ShowCHinese(45,0,9);
	OLED_ShowCHinese(65,0,10);
	OLED_ShowCHinese(85,0,11);
	OLED_ShowCHinese(5,3,12);
	OLED_ShowCHinese(25,3,13);
	OLED_ShowCHinese(45,3,14);
	OLED_ShowCHinese(65,3,15);
	OLED_ShowCHinese(85,3,16);
	OLED_ShowCHinese(105,3,17);

}


/**************************************************************************
�������ܣ�����ˢ�£���⣬ʹ�ú���
��ڲ�����δ֪
����  ֵ����
**************************************************************************/

void keyboard()
{
		u8 key;

		key=KEY_Scan(0);		//�õ���ֵ
		if(key)
		{						   
			switch(key)
			{				 
				case 1:	//��
				{
			
//					if(Key_flag==1)
//					Vertical_Kp+=5;
//					else if(Key_flag==2)
//					Vertical_Kd+=0.05;
//					else if(Key_flag==3)
//					{
//					Velocity_Kp+=0.05;Velocity_Ki+=0.00025;
//					}
//					else if(Key_flag==4)
//					Turn_Kd+=0.1;
//					else if(Key_flag==5)
//					Turn_Kp++;		
					break;
				}
				case 2:	
				 
//			  	Key_flag++;
					break;
//				case 3:	//��
//				{
//				if(Key_flag==1)
//					Vertical_Kp-=5;
//					else if(Key_flag==2)
//					Vertical_Kd-=0.05;
//					else if(Key_flag==3)
//					{
//					Velocity_Kp-=0.05;Velocity_Ki-=0.00025;
//					}
//					else if(Key_flag==4)
//					Turn_Kd-=0.1;
//					else if(Key_flag==5)
//					Turn_Kp--;		
//					break;
//				}
//				case 4:	//��
//					Key_flag--;
//					break;
			}
		}
//		if(Key_flag>=6)	Key_flag =5;
//		if(Key_flag<=0)	Key_flag =1;
}








/**********************�������������***************************/
//void chaoshengbo(void)
//{  
//	
//	

////	
//		
//		   if(UltrasonicWave_Distance <= 200)								  //�ж�������С��8cm��С��������PWMֵ��
//		 
//	      A=1;	
//	    else 
//		    A= 0;
//	
		

 	  
//	//�������ϱ�����
//	//�����Զ��ϱ�  10ms��һ�Σ���10*50ms = 500ms
//    //SendAutoUp();
// }


//void Wave(void)

//{	 
//	 UltrasonicWave_StartMeasure();	//������	
//		if(UltrasonicWave_Distance<10)
//			Target_Speed=10;
//}

