/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
#include "main.h"
#include "stdint.h"
#include "usart.h"
__IO uint32_t CAN_RxFlag = 0;		 //用于标志是否接收到数据，在中断函数中赋�??

CAN_TxHeaderTypeDef TxMes;		//发�?�结构体
CAN_RxHeaderTypeDef RxMes;		//接收结构�?

uint8_t CAN_TxDate[9]="CAN LOOP";								//发�?�缓冲区
uint8_t CAN_RxDate[9];								//接收缓冲�?

uint32_t TxMailbox;

/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 6;
  hcan.Init.Mode = CAN_MODE_LOOPBACK;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_10TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_7TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = ENABLE;
  hcan.Init.AutoWakeUp = ENABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */





  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
static void CAN_Filter_Config(void)
{
	CAN_FilterTypeDef  CAN_FilterTypeDef;

	/*CAN筛鿉器初始匿*/
	CAN_FilterTypeDef.FilterBank=0;						//筛鿉器绿0
	CAN_FilterTypeDef.FilterMode=CAN_FILTERMODE_IDLIST;	//工作在列表模�?
	CAN_FilterTypeDef.FilterScale=CAN_FILTERSCALE_32BIT;	//筛鿉器位宽为单丿32位㿿
	/* 使能筛鿉器，按照标志的内容进行比对筛鿉，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0�? */

	CAN_FilterTypeDef.FilterIdHigh= ((((uint32_t)0x2233<<3)|
										 CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;		//要筛选的ID高位 
	CAN_FilterTypeDef.FilterIdLow= (((uint32_t)0x2233<<3)|
									     CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要筛选的ID低位 
	CAN_FilterTypeDef.FilterMaskIdHigh= 0;		//第二个ID的高�?
	CAN_FilterTypeDef.FilterMaskIdLow= 0;			//第二个ID的低�?
	CAN_FilterTypeDef.FilterFIFOAssignment=CAN_FILTER_FIFO0 ;	//筛鿉器被关联到FIFO0
	CAN_FilterTypeDef.FilterActivation=ENABLE;			//使能筛鿉�?
	HAL_CAN_ConfigFilter(&hcan,&CAN_FilterTypeDef);
}

void CAN_RxMesInit(CAN_RxHeaderTypeDef* RxMessage)
{
  
	/*把接收结构体清零*/
  (*RxMessage).StdId = 0x00;
  (*RxMessage).ExtId = 0x00;
  (*RxMessage).IDE = CAN_ID_STD;
  (*RxMessage).DLC = 0;
  (*RxMessage).RTR = 0;
  (*RxMessage).FilterMatchIndex = 0;
	(*RxMessage).Timestamp = 0;
}

void CAN_TxMsgInit(CAN_TxHeaderTypeDef* TxMessage)
{	  
  (*TxMessage).StdId=0x00;						 
  (*TxMessage).ExtId=0x2233;					 //使用的扩展ID
  (*TxMessage).IDE=CAN_ID_EXT;				  //扩展模式
  (*TxMessage).RTR=CAN_RTR_DATA;				 //发鿁的是数捿
  (*TxMessage).DLC=8;							 //数据长度�?8字节	
}

void CAN_Config(void)
{
  CAN_Filter_Config();
  CAN_TxMsgInit(&TxMes); 
  CAN_RxMesInit(&RxMes);
	HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);				//使能FIFO0接收到数据中�?
	HAL_CAN_Start(&hcan);		//弿启CAN1
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
	/* 比较ID是否�?0x2233 */
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxMes,CAN_RxDate);
	if((RxMes.ExtId==0x2233) && (RxMes.IDE==CAN_ID_EXT) && (RxMes.DLC==8) )
	{
		CAN_RxFlag = 1; //接收成功  
	}
	else
	{
		CAN_RxFlag = 0; //接收失败
	}
}
/**
  * @brief  CAN错误回调函数
  * @param  hcan: CAN句柄指针
  * @retval �?
  */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	printf("\r\nCAN出错\r\n");
}

int fputc(int ch, FILE *f)
{
    uint8_t temp[1] = {ch};
    HAL_UART_Transmit(&huart1 , temp ,1, 2);
    
    return ch;    
}
/* USER CODE END 1 */
