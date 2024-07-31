/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "public.h"
//#include "spl06.h"
#include "bmp280.h"
#include "stmflash.h"
#include "AT7456E.h"
#include "w25q.h"
#include "Io_test.h"
#include "public.h"
#include "icm426xx.h"
//#include "mpu6000.h"


#define	USER_FLASH_ADDR		(STM32_FLASH_BASE+0XF000)
#define	WARNING_BZ_PORT	PAD31_PORT
#define	WARNING_BZ_PIN	PAD31_PIN  

void SystemClock_Config(void);
uint32_t osd_flag;
uint8_t w25_R[IO_buf_len+2];
uint32_t clk;
int main(void)
{
	uint8_t w25_W[IO_buf_len+2] = "0123456789";

	HAL_Init();

	SystemClock_Config();

	AT_GPIO_Init();
	
	MX_GPIO_Init();
	MX_SPI1_Init();
	MX_SPI2_Init();
	MX_SPI3_Init();
	MX_SPI4_Init();
////	
	MX_I2C2_Init();
	MX_ADC1_Init();
//	
	MX_USART6_Init();

	Icm42688Init();
	//mpu6000_init();



//	spl06_init();
	bmp280_init();
	
	max7456Init();

	BSP_W25Qx_Init();

//	if( (flashtest(w25_W,w25_R,(IO_buf_len+2))) && (Flash_ID == 0xEF17))
//	{
//		Flash_ID = 0xE7;
//		//printf("flash ok\n");
//	}
//	else
//	{
//		Flash_ID = 0x00;
//		//printf("flash error\n");
//	}

//		
	osd_flag=STMFLASH_ReadWord(USER_FLASH_ADDR);
	
//	if(osd_flag==0x0000001B)
//	{
//		HAL_GPIO_WritePin(WARNING_BZ_PORT, WARNING_BZ_PIN, GPIO_PIN_SET);
//		HAL_Delay(1000);
//		HAL_GPIO_WritePin(WARNING_BZ_PORT, WARNING_BZ_PIN, GPIO_PIN_RESET);
//	}
//	else
//	{
//		update_AT7456E_font();
//		
//		osd_flag=0x0000001B;
//		
//		STMFLASH_Write(USER_FLASH_ADDR,&osd_flag,1);
//		
//		HAL_GPIO_WritePin(WARNING_BZ_PORT, WARNING_BZ_PIN, GPIO_PIN_SET);
//		HAL_Delay(1000);
//		HAL_GPIO_WritePin(WARNING_BZ_PORT, WARNING_BZ_PIN, GPIO_PIN_RESET);
//	}
	
	while (1)
	{
//		HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_3);
//		HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_4);
//		HAL_Delay(500);
		decode_MCB_communication_data();
		HAL_GPIO_TogglePin(led1_GPIO_Port,led0_Pin|led1_Pin);
		ADC1_Test();	
		
//		test_tx();
	}
}

///**
//  * @brief System Clock Configuration
//  * @retval None
//  */
//void SystemClock_Config(void)
//{
//  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

//  /** Supply configuration update enable
//  */
//  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
//  /** Configure the main internal regulator output voltage
//  */
//  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

//  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
//  /** Initializes the RCC Oscillators according to the specified parameters
//  * in the RCC_OscInitTypeDef structure.
//  */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
//  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /** Initializes the CPU, AHB and APB buses clocks
//  */
//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
//                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
//  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
//  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
//  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART6;
//  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
//  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
//}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 9;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
  RCC_OscInitStruct.PLL.PLLFRACN = 3072;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}


/* USER CODE BEGIN 4 */

#if 1
/* USER CODE BEGIN 4 */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  USART_Send_string(USART1, (uint8_t *)&ch, 1);

  return ch;
}

#endif
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
