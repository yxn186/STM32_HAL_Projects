/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "Serial.h"
#include "dma.h"
#include "i2c.h"
#include "stm32_hal_legacy.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include "joled.h"
#include "MPU6050.h"
#include<math.h>
#include "DWT.h"
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
float Angle_a,Angle_g,Angle;
int16_t AX,AY,AZ,GX,GY,GZ;
volatile uint8_t mpu_flag = 0;

float AX1,AY1,AZ1,GX1,GY1,GZ1;
float AX2,AY2;

float Roll_Angle_6050,Yaw_Angle_6050,Pitch_Angle_6050;
float Real_Pitch_Angle_6050;
uint32_t T1,T2,T4;//dwt
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static float wrap180(float a)
{
    while (a > 180.0f) a -= 360.0f;
    while (a < -180.0f) a += 360.0f;
    return a;
}

static void euler_remap(float* roll, float* pitch, float* yaw)
{
    if (*pitch > 90.0f)
    {
        *pitch = 180.0f - *pitch;
        *roll += 180.0f;
        *yaw  += 180.0f;
    }
    else if (*pitch < -90.0f)
    {
        *pitch = -180.0f - *pitch;
        *roll += 180.0f;
        *yaw  += 180.0f;
    }
    *roll = wrap180(*roll);
    *yaw  = wrap180(*yaw);
}


//欧拉角计算函数（MPU6050）
void MPU6050_Data_Calculate(void)
{
	/*这里是AI 开始*/
	//大概就是转换了下参考系之类的 因为不转换就是有问题 在vofa显示会反过来 还在研究这里的数学过程 
	//float yaw_rad = Yaw_Angle_6050 / 180.0f * 3.1415926f;
	//float cy = cosf(yaw_rad);
	//float sy = sinf(yaw_rad);
	//AX2 =  AX1 * cy - AY1 * sy;
	//AY2 =  AX1 * sy + AY1 * cy;
  AX2 = AX1;
  AY2 = AY1;

	/*这里是AI 结束*/
	
	//Alpha参数 统一设置
	float Alpha = 0.7f;
	
	
	float MPU6050_Data_Calculate_dt = 0.001f * (float)T4/1000.0;
	
	//Roll横滚角--绕x轴转--计算YOZ平面
	float a_Roll_Angle = atan2(AY2,AZ1) / 3.14159f * 180.0f;
	static float g_Roll_Angle = 0.0f;
	g_Roll_Angle = Roll_Angle_6050 + GX1/32768.0f * 1000.0f * MPU6050_Data_Calculate_dt;
	//互补滤波
	Roll_Angle_6050 = Alpha * a_Roll_Angle + (1 - Alpha) * g_Roll_Angle;
	
	/*这里是AI 开始*/
	//大概也是转换了下参考系之类的 因为不转换就是有问题 在vofa显示会反过来 还在研究这里的数学过程 
	
	//Pitch俯仰角--绕y轴转--计算XOZ平面 atan2f(-ax, sqrtf(ay*ay + az*az))
	
	float a_Pitch_Angle = atan2(-AX2,sqrtf(AY2*AY2 + AZ1*AZ1)) / 3.14159f * 180.0f;
	static float g_Pitch_Angle = 0.0f;
	g_Pitch_Angle = Pitch_Angle_6050 + GY1/32768.0f * 1000.0f * MPU6050_Data_Calculate_dt;
	//互补滤波
	
	Pitch_Angle_6050 = Alpha * a_Pitch_Angle + (1 - Alpha) * g_Pitch_Angle;
	/*这里是AI 结束*/
	
//	float a_Pitch_Angle = atan2(AX1,AZ1) / 3.14159f * 180.0f;
//	static float g_Pitch_Angle = 0.0f;
//	g_Pitch_Angle = Pitch_Angle_6050 + GY1/32768.0f * 2000.0f * 0.001f;
//	//互补滤波
//	float Alpha_Pitch = 0.9f;
//	Pitch_Angle_6050 = Alpha_Pitch * a_Pitch_Angle + (1 - Alpha_Pitch) * g_Pitch_Angle;
	
	//Yaw偏航角--绕z轴转--计算XOY平面
	Yaw_Angle_6050 = Yaw_Angle_6050 + GZ1/32768.0f * 1000.0f * MPU6050_Data_Calculate_dt;

	//6050自身pitch
	float Real_a_Pitch_Angle = -atan2(AX1,AZ1) / 3.14159f * 180.0f;
	static float Real_g_Pitch_Angle = 0.0f;
	Real_g_Pitch_Angle = Real_Pitch_Angle_6050 + GY1/32768.0f * 1000.0f * MPU6050_Data_Calculate_dt;
	//互补滤波
	float Alpha_Real_Pitch_Angle_6050 = 0.4;
	Real_Pitch_Angle_6050 = Alpha_Real_Pitch_Angle_6050 * Real_a_Pitch_Angle + (1 - Alpha_Real_Pitch_Angle_6050) * Real_g_Pitch_Angle;
}



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim1)
  {
     mpu_flag = 1;
  }
}
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  JOLED_Init();
  MPU6050_Init(&hi2c2);
  HAL_TIM_Base_Start_IT(&htim1);
  Serial_Init();
  DWT_Init();
  
  uint32_t last_time1 = HAL_GetTick();
  uint32_t last_time2 = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    
    if (mpu_flag)
    {
      mpu_flag = 0;

      T2 = DWT_GetUs();
      T4 = T2 - T1;
      T1 = T2;
      MPU6050_GetData(&hi2c2, &AX,&AY,&AZ,&GX,&GY,&GZ);

      // 校准 + 计算（你原来的代码照搬）
      AX1 = AX - 32;
			AY1 = AY - 0;
			AZ1 = AZ - 0;
			GX1 = GX + 23;
			GY1 = GY - 33;
			GZ1 = GZ + 17;
      
      MPU6050_Data_Calculate();
      euler_remap(&Roll_Angle_6050, &Pitch_Angle_6050, &Yaw_Angle_6050);

      
    }

    if(HAL_GetTick() - last_time1 >= 100)
    {
      //JOLED_ShowSignedNum(1, 1, Roll_Angle_6050, 3);
      //JOLED_ShowSignedNum(1, 6, Yaw_Angle_6050, 3);
      //JOLED_ShowSignedNum(1, 11, Pitch_Angle_6050, 3);
      JOLED_ShowNum(2, 1, T4, 10);
      //JOLED_ShowNum(3, 1, T1, 10);
      //JOLED_ShowNum(4, 1, T2, 10);
      //JOLED_ShowSignedNum(2,1,AX1,5);
      //JOLED_ShowSignedNum(3,1,AY1,5);
      //JOLED_ShowSignedNum(4,1,AZ1,5);
                              
      //JOLED_ShowSignedNum(2,8,GX1,5);
      //JOLED_ShowSignedNum(3,8,GY1,5);
      //JOLED_ShowSignedNum(4,8,GZ1,5);
      
      last_time1 = HAL_GetTick();
    }
    
    if(HAL_GetTick() - last_time2 >= 5)
    {
      Serial_Printf("%f,%f,%f,%d\r\n",Roll_Angle_6050,Yaw_Angle_6050,-Pitch_Angle_6050,T4);
      last_time2 = HAL_GetTick();
    }
    
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
#ifdef USE_FULL_ASSERT
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
