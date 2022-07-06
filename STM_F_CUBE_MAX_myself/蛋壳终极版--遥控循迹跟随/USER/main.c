#include "stm32f10x.h"
#include "sys.h" 


float Pitch,Roll,Yaw;						//�Ƕ�
short gyrox,gyroy,gyroz;				//������--���ٶ�
short aacx,aacy,aacz;						//���ٶ�
int Encoder_Left,Encoder_Right;	//���������ݣ��ٶȣ�

int PWM_MAX=7200,PWM_MIN=-7200;	//PWM�޷�����
int MOTO1,MOTO2;								//���װ�ر���

extern int Vertical_out,Velocity_out,Turn_out;	  
u8 string[10] = {0};	
u16 time=0;
int main(void)	
{
	
// char buf[16];
	delay_init();
	NVIC_Config();
	uart_init(115200);
	
	
//	uart3_init(38400);//��9600  
//	delay_ms(100);
//	
//	USART3_Send_String("AT\r\n");
//	USART3_Send_String("AT+NAMEYeHuoBalanceCar\r\n");//��������ģ��ָ��--��������
//	delay_ms(100);	
//	USART3_Send_String("AT+BAUD8\r\n"); 		 //��������ģ��ָ��,�����������ó�115200
//	delay_ms(100);
	
	uart3_init(115200);
	
	OLED_Init();
	OLED_Clear();
	
	KEY_Init();
	
	MPU_Init();
	
	mpu_dmp_init();
	MPU6050_EXTI_Init();
//	OLED_ShowString(0,4,"BALANCE CAR",16);
	
	Encoder_TIM2_Init();
	Encoder_TIM4_Init();
	Motor_Init();
	PWM_Init_TIM1(0,7199);
	
	Tracking_Init();          ///ѭ��

  TIM3_NVIC_Configuration();
	TIM3_Configuration();			//��������������Ķ�ʱ��
	UltrasonicWave_Configuration(); //������
//		printf("  %d\r\n",mpu_dmp_init());
//	while(mpu_dmp_init())
// 	{
//		OLED_ShowString(0,0,"MPU6050 error",16);

////		printf(" MPU_ADDR: error\r\n");
//	}  

//		OLED_ShowString(0,2,"MPU6050 ok",16);
//	printf(" MPU_ADDR: ok\r\n");
//			OLED_Clear();

//		OLED_Clear();
		oledshow();
  while(1)	
	{
//    time++;
//		if(time%200==0)
//		printf("L:%d R:%d",Left_Tracking,Right_Tracking);
		
//     Wave();
//			sprintf(buf,"%4d",UltrasonicWave_Distance);
//		OLED_ShowString(0,1,(unsigned char*)buf,16);
//    UltrasonicWave_StartMeasure();
		
//		UltrasonicWave_StartMeasure();
//		oledshow();
//		time=0;
	
//			 UltrasonicWave_StartMeasure();
//				sprintf(buf,"%4d",UltrasonicWave_Distance);
//			OLED_ShowString(0, 6, (unsigned char*)buf, 8);
		//      
 
//if(Mode_flag==3)
//	{	 
//		chaoshengbo();
//	}	
	 
	
//			
		 
//		UltrasonicWave_StartMeasure();
//		UltrasonicWave_StartMeasure();
//	sprintf((char *)string,"distance:%.d",UltrasonicWave_Distance);//0300
//		OLED_ShowString(0,6,string,8);
//		
//    printf(" distance:%d\r\n",UltrasonicWave_Distance);
		 
//		sprintf((char *)string,"Pitch:%.2f",Pitch);//0300
//		OLED_ShowString(0,0,string,8);
//		sprintf((char *)string,"Roll :%.2f",Roll);//0300
//		OLED_ShowString(0,2,string,8);
//		sprintf((char *)string,"Yaw  :%.2f",Yaw);//0300
//		OLED_ShowString(0,4,string,8);
		

		
		
		
//		OLED_Float(0,0,Pitch,1);
//		OLED_Float(50,50,Velocity_out,1);
//		OLED_ShowNumber(30,0,Velocity_out,3,16);//OK
//		if(mpu_dmp_get_data(&Pitch,&Roll,&Yaw)==0)
//		{ 
//			mpu_dmp_get_data(&Pitch,&Roll,&Yaw);			//�Ƕ�
//			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//������
//			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//���ٶ�
//			
//		}
//		delay_ms(100);
printf("\r\npitch:%.f  roll:%.f  yaw:%.f\r\n",Pitch,Roll,Yaw);
		
	} 	
} 






















