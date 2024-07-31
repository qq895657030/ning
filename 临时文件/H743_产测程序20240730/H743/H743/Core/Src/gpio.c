/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "STM32FCB_gpioCfg.h"

GPIO_TypeDef * OutPort[32]={	PAD0_PORT,	PAD1_PORT,	PAD2_PORT,	PAD3_PORT,	PAD4_PORT,	PAD5_PORT,	PAD6_PORT,	PAD7_PORT,
								PAD8_PORT,	PAD9_PORT,	PAD10_PORT,	PAD11_PORT,	PAD12_PORT,	PAD13_PORT,	PAD14_PORT,	PAD15_PORT,
								PAD16_PORT,	PAD17_PORT,	PAD18_PORT,	PAD19_PORT,	PAD20_PORT,	PAD21_PORT,	PAD22_PORT,	PAD23_PORT,
								PAD24_PORT,	PAD25_PORT,	PAD26_PORT,	PAD27_PORT,	PAD28_PORT,	PAD29_PORT,	PAD30_PORT,	PAD31_PORT,};

uint16_t OutPin[32]={	PAD0_PIN,	PAD1_PIN,	PAD2_PIN,	PAD3_PIN,	PAD4_PIN,	PAD5_PIN,	PAD6_PIN,	PAD7_PIN,
						PAD8_PIN,	PAD9_PIN,	PAD10_PIN,	PAD11_PIN,	PAD12_PIN,	PAD13_PIN,	PAD14_PIN,	PAD15_PIN,
						PAD16_PIN,	PAD17_PIN,	PAD18_PIN,	PAD19_PIN,	PAD20_PIN,	PAD21_PIN,	PAD22_PIN,	PAD23_PIN,
						PAD24_PIN,	PAD25_PIN,	PAD26_PIN,	PAD27_PIN,	PAD28_PIN,	PAD29_PIN,	PAD30_PIN,	PAD31_PIN,};
											
						
/* USER CODE END 0 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(led0_GPIO_Port, led0_Pin|led1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(imu1_cs_GPIO_Port, imu1_cs_Pin|flash_cs_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, imu2_cs_Pin|osd_cs_Pin, GPIO_PIN_SET);

	/*Configure GPIO pins : PCPin PCPin */
	GPIO_InitStruct.Pin = led0_Pin|led1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(led0_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PAPin PAPin */
	GPIO_InitStruct.Pin = imu1_cs_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(imu1_cs_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin =		flash_cs_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(flash_cs_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PBPin PBPin */
	GPIO_InitStruct.Pin = imu2_cs_Pin|osd_cs_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(imu2_cs_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = osd_cs_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(osd_cs_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(imu1_cs_GPIO_Port, imu1_cs_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(imu2_cs_GPIO_Port, imu2_cs_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(flash_cs_GPIO_Port, flash_cs_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(osd_cs_GPIO_Port, osd_cs_Pin, GPIO_PIN_SET);
}


///**
//  * @brief GPIO Initialization Function
//  * @param None
//  * @retval None
//  */
// void MX_GPIO_Init(void)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
///* USER CODE BEGIN MX_GPIO_Init_1 */
///* USER CODE END MX_GPIO_Init_1 */

//  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOC_CLK_ENABLE();
//  __HAL_RCC_GPIOE_CLK_ENABLE();
//  __HAL_RCC_GPIOD_CLK_ENABLE();
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//  __HAL_RCC_GPIOB_CLK_ENABLE();

//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);

//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);

//  /*Configure GPIO pin : PC15 */
//  GPIO_InitStruct.Pin = GPIO_PIN_15;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//  /*Configure GPIO pins : PE3 PE4 */
//  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

///* USER CODE BEGIN MX_GPIO_Init_2 */
///* USER CODE END MX_GPIO_Init_2 */
//}


/* USER CODE BEGIN 2 */
void AT_GPIO_Init(void)
{

	uint8_t i;
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	for(i=0;i<32;i++)
	{
		if(OutPin[i]!=0xffff)
		{
			GPIO_InitStruct.Pin = OutPin[i];
			HAL_GPIO_Init(OutPort[i], &GPIO_InitStruct);
		}
	}
	
#ifdef	INVERTER_PIN
	GPIO_InitStruct.Pin = INVERTER_PIN;
	HAL_GPIO_Init(INVERTER_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(INVERTER_PORT, INVERTER_PIN, GPIO_PIN_RESET);
#endif
	
#ifdef	RESOURCE_PIN
	GPIO_InitStruct.Pin = RESOURCE_PIN;
	HAL_GPIO_Init(RESOURCE_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(RESOURCE_PORT, RESOURCE_PIN, GPIO_PIN_SET);
#endif
	
#ifdef	SBUS_PIN
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pin = SBUS_PIN;
	HAL_GPIO_Init(SBUS_PORT, &GPIO_InitStruct);
#endif
}
