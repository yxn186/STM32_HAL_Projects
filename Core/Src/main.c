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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include "joled.h"
#include "Encoder.h"
#include "No_Blocking_Key.h"
#include "servo.h"
#include "Serial.h"
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim4)
  {
    Key_Tick();
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
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  JOLED_Init();
  Serial_Init();
  Serial_Printf("串口上电!\r\n");
  Encoder_Init(&htim3);
  Servo_Init(&htim2,TIM_CHANNEL_1);
  Servo_Init(&htim2,TIM_CHANNEL_2);

  HAL_TIM_Base_Start_IT(&htim4);
  
  int16_t Angle= 0;
  int16_t Serial_Angle = 0;
  int16_t Ozone_Angle = 0;
  Key_State state;
  int16_t Duty;
  uint16_t Serial_len;
  uint8_t Serial_RxData[256+1];

  typedef enum
  {
    k_state_e_1 = 0,
    k_state_e_2,
    k_state_s_1,
    k_state_s_2,
    k_state_o,
    k_state_max
  } Servo_key_state;

 Servo_key_state s_k_s = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    Angle =  Encoder_Get_Angle(&htim3);
    JOLED_ShowSignedNum(1, 1, Angle, 3);
    
    state = Key_Get_Event(1);

    if(state != KEY_FREE)
    {
      s_k_s = (s_k_s + 1) % k_state_max;
    }

    Serial_len = Serial_TryGetPacket(Serial_RxData, 256);

    if(Serial_len > 0)
    {
      if(Serial_len > 256)
      {
        Serial_len = 256;
      }
      Serial_Angle = 0;
      Serial_RxData[Serial_len] = '\0';

      for(uint8_t i = 0;i < Serial_len;i++)
      {
        if(Serial_RxData[0] == '-' && i == 0)
        {
          continue;
        }
        Serial_Angle = Serial_Angle*10 + Serial_RxData[i] - '0';//带进位
      }
      
      if(Serial_RxData[0] == '-')
      {
        Serial_Angle = -Serial_Angle;
      }

      Serial_Printf("串口收到数据！解析为 %d 度\r\n",Serial_Angle);
    }

    if(s_k_s == k_state_e_1)
    {
      Duty = Servo_Set_Angle(&htim2, TIM_CHANNEL_1, -Angle);
    }
    else if (s_k_s == k_state_e_2)
    {
      Duty = Servo_Set_Angle(&htim2, TIM_CHANNEL_2, -Angle);
    }
    else if (s_k_s == k_state_s_1)
    {
      Duty = Servo_Set_Angle(&htim2, TIM_CHANNEL_1, -Serial_Angle);
    }
    else if (s_k_s == k_state_s_2)
    {
       Duty = Servo_Set_Angle(&htim2, TIM_CHANNEL_2, -Serial_Angle);
    }
    else if(s_k_s == k_state_o)
    {
      Duty = Servo_Set_Angle(&htim2, TIM_CHANNEL_2, Ozone_Angle);
    }
      
    JOLED_ShowNum(2, 1, Serial_len, 6);
    JOLED_ShowSignedNum(1, 1, Angle, 3);
    JOLED_ShowSignedNum(1, 6, Serial_Angle, 3);
    JOLED_ShowNum(4, 1, Duty, 6);
    JOLED_ShowSignedNum(3,1,s_k_s,1); 
    
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
