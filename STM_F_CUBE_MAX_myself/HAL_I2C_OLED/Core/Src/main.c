/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define command 0x00
#define data 0x40
#define qijian 0x78

uint8_t suzu[] = {/*--  文字:  蒙  --*/
/*--  幼圆12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x04,0xFC,0xFC,0xFC,0xFE,0xFE,0xFC,0xFC,0xFC,0xFC,0xFE,0xFE,0xFC,0xFC,0x74,0x00,
0x04,0xD4,0xD5,0x5F,0x7F,0xEF,0xBF,0xFF,0xFD,0x7D,0x1F,0x37,0x63,0x63,0xC0,0x00,
};

uint8_t suzu_command_data[] = {0xAE, 0x40, 0xb0, 0xc8, 0x81, 0xff, 0xa1, 0xa6, 
0xa8, 0x1f, 0xd3, 0x00, 0xd5, 0xf0, 0xd9, 0x22, 0xda, 0x02, 0xd8, 0x49, 
0x8d, 0x14, 0xaf};

uint8_t suzu_command_data2[] = {0X8D, 0x14, 0xaf};

void  OLED_Init()
{
	HAL_Delay(1);
	HAL_I2C_Mem_Write(&hi2c1, qijian, command , 8, suzu_command_data, sizeof(suzu_command_data), 5000);
}

void OLED_Display_On(void)
{
	HAL_I2C_Mem_Write(&hi2c1, qijian, command , 8, suzu_command_data2, sizeof(suzu_command_data2), 500);
}

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	uint8_t neibu[] = {0xb0+y, ((x&0xf0)>>4)|0x10, (x&0x0f)};
	HAL_I2C_Mem_Write(&hi2c1, qijian, command , 8, neibu, sizeof (neibu), 500);
}

void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no,uint8_t font_width,uint8_t font_height)
{
	uint8_t t, i;
        for(i=0;i<(font_height/8);i++)				/*font_height朿大忼为32，这张屏只有8个页（行），每页4个位*/
	 {
			OLED_Set_Pos(x,y+i);	
			for(t=0;t<font_width;t++)		/*font_width朿大忼为128，屏幕只有这么大*/
			{	
				HAL_I2C_Mem_Write(&hi2c1, qijian, data, 8, suzu, sizeof(suzu), 500);	
			}		
	 }		
}

void OLED_Clear(void){  
	uint8_t i,n;	
	uint8_t neibu[] = {0x00, 0x10};
	 for(i=0;i<8;i++)  
	 {  
			HAL_I2C_Mem_Write(&hi2c1, qijian, command , 8, (uint8_t *)(0xb0+i), sizeof (0xb0), 500);
			HAL_I2C_Mem_Write(&hi2c1, qijian, command , 8, neibu, sizeof (neibu), 500);
			for(n=0;n<128;n++)
		 HAL_I2C_Mem_Write(&hi2c1, qijian, data, 8, 0, sizeof(0), 5);
	 } 														//更新显示
}
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	OLED_Clear();
	OLED_Init();
	OLED_Display_On();
	OLED_ShowCHinese(0,0,0,16,16);
	HAL_Delay(200);
	OLED_Clear();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

