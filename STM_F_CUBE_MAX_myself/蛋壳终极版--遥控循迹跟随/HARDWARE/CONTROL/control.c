#include "control.h"
#include "motor.h"

/*
0x00：刹车
0x01：前进
0x05：后退
0x03：左转
0x07：右转
*/
u8 A;
u8 Mode_flag;					//模式选择标志位
float Med_Angle=-12;	//机械中值。---在这里修改你的机械中值即可。
float Target_Speed=0;	//期望速度（俯仰）。---二次开发接口，用于控制小车前进后退及其速度。
float Turn_Speed=0;		//期望速度（偏航）
float 
Vertical_Kp=-630//420*0.6=252     770*0.6=462   -1300*0.6=-780    -870  -1050*0.6
,//直立环KP、KD
Vertical_Kd=-1.85;    //1*0.6     2.3*0.6=1.38  -3*0.6=-1.8   -4.2*0.6  -3*0.6
float 
	Velocity_Kp=0.24,//速度环KP、KI  0.21   0.45
	Velocity_Ki=0.0012;      ////   0.0015   0.004
float 
	Turn_Kd=-0.7,//转向环KP、KD -0.7
	Turn_Kp=20;//20
int Vertical_out,Velocity_out,Turn_out;//直立环&速度环&转向环 的输出变量

int Vertical(float Med,float Angle,float gyro_Y);//函数声明
int Velocity(int Target,int encoder_left,int encoder_right);
int Turn(int gyro_Z,int RC); 

//void chaoshengbo(void);

#define SPEED_X 30 //俯仰(前后)最大设定速度
#define SPEED_Z 100//偏航(左右)最大设定速度 




void EXTI9_5_IRQHandler(void)
{
	int PWM_out;
	if(EXTI_GetITStatus(EXTI_Line5)!=0)//一级判定
	{
		if(PBin(5)==0)//二级判定
		{
			EXTI_ClearITPendingBit(EXTI_Line5);//清除中断标志位
			
			//1.采集编码器数据&MPU6050角度信息。
			Encoder_Left=-Read_Speed(2);//电机是相对安装，刚好相差180度，为了编码器输出极性一致，就需要对其中一个取反。
			Encoder_Right=Read_Speed(4);
			
	 		mpu_dmp_get_data(&Pitch,&Roll,&Yaw);			//角度
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//陀螺仪
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//加速度
			
/******************************************************避障**********************************************/

 	if(Mode_flag==3)
	{	 
		UltrasonicWave_StartMeasure();
		
	 	if(UltrasonicWave_Distance<=70)Target_Speed=10;
		else if(UltrasonicWave_Distance>100&&UltrasonicWave_Distance<=200)Target_Speed=-10;
		else Target_Speed=0;
	}	
/*********************************************************************************************************/		
			
			
			
			
/******************************************************循迹***********************************************/
//	  if(Left_Tracking==0&&Right_Tracking==1){Turn_Kd=0;Turn_Speed=15;Target_Speed=10;}
//    if(Left_Tracking==1&&Right_Tracking==0){Turn_Kd=0;Turn_Speed=-15;Target_Speed=10;}
//		else Turn_Kd=-0.6;Target_Speed=8;

//    Turn_Speed=Turn_Speed>SPEED_Z?SPEED_Z:(Turn_Speed<-SPEED_Z?(-SPEED_Z):Turn_Speed);//限幅( (20*100) * 100   )
//      
   if(Mode_flag==2)
    Tracking_detection();
//			
		  
/*********************************************************************************************************/			
		 
			
			
/******************************************************蓝牙***********************************************/	
	else if(Mode_flag==1)
		/*前后*/
	{		if((Fore==0)&&(Back==0))Target_Speed=0;//未接受到前进后退指令-->速度清零，稳在原地
			if(Fore==1)Target_Speed=-25;// 前进1标志位拉高-->需要前进
			if(Back==1)Target_Speed=25;//
			Target_Speed=Target_Speed>SPEED_X?SPEED_X:(Target_Speed<-SPEED_X?(-SPEED_X):Target_Speed);//限幅
			
			/*左右*/
			if((Left==0)&&(Right==0))Turn_Speed=0;
			if(Left==1)Turn_Speed-=50;	//左转
			if(Right==1)Turn_Speed+=50;	//右转
			Turn_Speed=Turn_Speed>SPEED_Z?SPEED_Z:(Turn_Speed<-SPEED_Z?(-SPEED_Z):Turn_Speed);//限幅( (20*100) * 100   )
			
			/*转向约束*/
			if((Left==0)&&(Right==0))Turn_Kd=-0.7;//若无左右转向指令，则开启转向约束
			else if((Left==1)||(Right==1))Turn_Kd=0;//若左右转向指令接收到，则去掉转向约束
	}
/******************************************************蓝牙*************************************************/
		
			//2.将数据压入闭环控制中，计算出控制输出量。
		 
			Velocity_out=Velocity(Target_Speed,Encoder_Left,Encoder_Right);	//速度环
			Vertical_out=Vertical(Velocity_out+Med_Angle,Roll,gyrox);			//直立环
			Turn_out=Turn(gyroz,Turn_Speed);																//转向环												
			//转向环

			PWM_out=Vertical_out;//最终输出
			//3.把控制输出量加载到电机上，完成最终的的控制。
			MOTO1=PWM_out-Turn_out;//左电机
			MOTO2=PWM_out+Turn_out;//右电机
			Limit(&MOTO1,&MOTO2);	 //PWM限幅			
			Load(MOTO1,MOTO2);		 //加载到电机上。
			
//			Stop(&Med_Angle,&Roll);
			
		}
	}
}




/*********************
直立环PD控制器：Kp*Ek+Kd*Ek_D

入口：期望角度、真实角度、真实角速度
出口：直立环输出
*********************/
int Vertical(float Med,float Angle,float gyro_X)
{
	int PWM_out;
	
	PWM_out=Vertical_Kp*(Angle-Med)+Vertical_Kd*(gyro_X-0);
	return PWM_out;
}


/*********************
速度环PI：Kp*Ek+Ki*Ek_S
*********************/
int Velocity(int Target,int encoder_left,int encoder_right)
{
	static int Encoder_S,EnC_Err_Lowout_last,PWM_out,Encoder_Err,EnC_Err_Lowout;
	float a=0.7;
	
	//1.计算速度偏差
	Encoder_Err=((encoder_left+encoder_right)-Target);//舍去误差--我的理解：能够让速度为"0"的角度，就是机械中值。
	//2.对速度偏差进行低通滤波
	//low_out=(1-a)*Ek+a*low_out_last;
	EnC_Err_Lowout=(1-a)*Encoder_Err+a*EnC_Err_Lowout_last;//使得波形更加平滑，滤除高频干扰，防止速度突变。
	EnC_Err_Lowout_last=EnC_Err_Lowout;//防止速度过大的影响直立环的正常工作。
	//3.对速度偏差积分，积分出位移
	Encoder_S+=EnC_Err_Lowout;
	//4.积分限幅
	Encoder_S=Encoder_S>10000?10000:(Encoder_S<(-10000)?(-10000):Encoder_S);
	
	//5.速度环控制输出计算
	PWM_out=Velocity_Kp*EnC_Err_Lowout+Velocity_Ki*Encoder_S;
	return PWM_out;
}


/*********************
转向环：系数*Z轴角速度+系数*遥控数据
*********************/
int Turn(int gyro_Z,int RC)
{
	int PWM_out;
	//这不是一个严格的PD控制器，Kd针对的是转向的约束，但Kp针对的是遥控的转向。
	PWM_out=Turn_Kd*gyro_Z + Turn_Kp*RC;
	return PWM_out;
}









/**************************************************************************
函数功能：循迹模块，逻辑运算
入口参数：无
返回  值：无
Turn_Speed+ 左转  Turn_Speed- 右转
**************************************************************************/	

void Tracking_detection(void)
{

//	if(Left_Tracking==0&&Right_Tracking==0)      //直走
//	{
//	  Turn_Kd=-0.7,Target_Speed=-20;
//	}
//	else if(Left_Tracking==0&&Right_Tracking==1)   //右转
//	{
//		 Turn_Kd=0,Turn_Speed=-20;Target_Speed=-20;
//	}
//	else if(Left_Tracking==1&&Right_Tracking==0)   //左转
//		
//	{
//		Turn_Kd=0,Turn_Speed=20;Target_Speed=-20;
//	}
//	else if(Left_Tracking==1&&Right_Tracking==1)  //直走
//	{
//		Turn_Kd=-0.7,Target_Speed=-20;
//	}

	
	
	if(Left_Tracking==0&&Right_Tracking==0)      //直走
	{
	  Turn_Kd=-0.7,Target_Speed=-15;
	}
	else if(Left_Tracking==0&&Right_Tracking==1)   //右转
	{
		 Turn_Kd=0,Turn_Speed=-30;Target_Speed=-15;
	}
	else if(Left_Tracking==1&&Right_Tracking==0)   //左转
		
	{
		Turn_Kd=0,Turn_Speed=30;Target_Speed=-15;
	}
	else if(Left_Tracking==1&&Right_Tracking==1)  //直走
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
函数功能：按键刷新，检测，使用函数
入口参数：未知
返回  值：无
**************************************************************************/

void keyboard()
{
		u8 key;

		key=KEY_Scan(0);		//得到键值
		if(key)
		{						   
			switch(key)
			{				 
				case 1:	//上
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
//				case 3:	//下
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
//				case 4:	//右
//					Key_flag--;
//					break;
			}
		}
//		if(Key_flag>=6)	Key_flag =5;
//		if(Key_flag<=0)	Key_flag =1;
}








/**********************超声波距离计算***************************/
//void chaoshengbo(void)
//{  
//	
//	

////	
//		
//		   if(UltrasonicWave_Distance <= 200)								  //判断若距离小于8cm，小车输出向后PWM值。
//		 
//	      A=1;	
//	    else 
//		    A= 0;
//	
		

 	  
//	//寄生此上报数据
//	//增加自动上报  10ms进一次，故10*50ms = 500ms
//    //SendAutoUp();
// }


//void Wave(void)

//{	 
//	 UltrasonicWave_StartMeasure();	//超声波	
//		if(UltrasonicWave_Distance<10)
//			Target_Speed=10;
//}

