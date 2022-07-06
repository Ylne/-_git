#include "usart3.h"	  


void uart3_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʱ��GPIOB��USART3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//USART1_TX   PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//USART1_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
}



u8 Fore,Back,Left,Right;

void USART3_IRQHandler(void) 
{
	int Bluetooth_data;
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)//�����жϱ�־λ����
	{
		
		Bluetooth_data=USART_ReceiveData(USART3);//������յ�����
	
//		if(Bluetooth_data==0x00)Fore=0,Back=0,Left=0,Right=0;//ɲ
//		else if(Bluetooth_data==0x01)Fore=1,Back=0,Left=0,Right=0;//ǰ
//		else if(Bluetooth_data==0x05)Fore=0,Back=1,Left=0,Right=0;//��
//		else if(Bluetooth_data==0x03)Fore=0,Back=0,Left=1,Right=0;//��
//		else if(Bluetooth_data==0x07)Fore=0,Back=0,Left=0,Right=1;//��
//		else if(Bluetooth_data==0x09)Mode_flag=1;//��������ģʽ
//		else if(Bluetooth_data==0x11)Mode_flag=2;//ѭ������ģʽ
//		else if(Bluetooth_data==0x13)Mode_flag=3;//����ģʽ
//		else										Fore=0,Back=0,Left=0,Right=0,Mode_flag=0;//ɲ
		
		
			if(Bluetooth_data=='Z')Fore=0,Back=0,Left=0,Right=0;//ɲ
		else if(Bluetooth_data=='A')Fore=1,Back=0,Left=0,Right=0;//ǰ
		else if(Bluetooth_data=='E')Fore=0,Back=1,Left=0,Right=0;//��
		else if(Bluetooth_data=='C')Fore=0,Back=0,Left=1,Right=0;//��
		else if(Bluetooth_data=='G')Fore=0,Back=0,Left=0,Right=1;//��
		else if(Bluetooth_data=='L'){Mode_flag=1;OLED_Clear();OLED_ShowCHinese(15,3,0);OLED_ShowCHinese(42,3,1);
																							OLED_ShowCHinese(69,3,6);OLED_ShowCHinese(96,3,7);}//��������ģʽ
		else if(Bluetooth_data=='M'){Mode_flag=2;OLED_Clear();OLED_ShowCHinese(15,3,2);OLED_ShowCHinese(42,3,3);
																							OLED_ShowCHinese(69,3,6);OLED_ShowCHinese(96,3,7);}//ѭ������ģʽ
		else if(Bluetooth_data=='N'){Mode_flag=3;OLED_Clear();OLED_ShowCHinese(15,3,4);OLED_ShowCHinese(42,3,5);
																							OLED_ShowCHinese(69,3,6);OLED_ShowCHinese(96,3,7);}//����ģʽ
		else										Fore=0,Back=0,Left=0,Right=0,Mode_flag=0;//ɲ
	}
}

//һ��
void USART3_Send_Data(char data)
{
	USART_SendData(USART3,data);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=1);
}

//һ��
void USART3_Send_String(char *String)
{
	u16 len,j;
	
	len=strlen(String);
	for(j=0;j<len;j++)
	{
		USART3_Send_Data(*String++);
	}
}





