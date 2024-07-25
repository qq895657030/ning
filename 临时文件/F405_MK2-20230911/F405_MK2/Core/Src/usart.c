/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include<stdio.h>
#include "public.h"

Uart_Stru UartMes[6];
USART_TypeDef* UartType[6]={USART1,USART2,USART3,UART4,UART5,USART6};
/* USER CODE END 0 */

UART_HandleTypeDef huart6;

/* USART6 init function */

void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
    /* USART6 clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
  }

  __HAL_UART_ENABLE_IT(uartHandle, UART_IT_RXNE);		//开启接收中断	
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspDeInit 0 */

  /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();

    /**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);

    /* USART6 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspDeInit 1 */

  /* USER CODE END USART6_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//UART发送字符串
void USART_Send_string(USART_TypeDef* usart_x,uint8_t *str,uint32_t length)  
{  
	uint32_t i;
	for(i=0;i<length;i++)
	{
		usart_x->DR=*str;
		while((usart_x->SR & UART_FLAG_TXE) == 0);
		str++;
	}
}

//UART接收packet中断回调函数
void UART_Receive_INT_CallBack(USART_TypeDef* usart_x,Uart_Stru* UartMessage)  
{
	uint8_t Res;
	uint16_t crcPayload;
	uint16_t crc_H,crc_L;
	
	if((usart_x->SR & UART_FLAG_RXNE) != RESET)
	{
		usart_x->SR &= ~UART_FLAG_RXNE;
			/* read one byte from the receive data register */
		Res = usart_x->DR;

		if(0==UartMessage->RxState)
		{
			if(0xf0==Res)			//起始帧
			{
				UartMessage->RxIndex=0;
				UartMessage->RxBuf[UartMessage->RxIndex++]=0xf0;
				UartMessage->RxState=1;
			}
		}
		else	if(1==UartMessage->RxState)
		{
			UartMessage->RxBuf[UartMessage->RxIndex++]=Res;
			
			if((0xff==UartMessage->RxBuf[UartMessage->RxIndex-1])&&(UartMessage->RxIndex>=(UartMessage->RxBuf[2]+6)))		//结束帧
			{	
				crcPayload=crc16(UartMessage->RxBuf, UartMessage->RxBuf[2]+3);
				crc_H=(uint8_t)(crcPayload >> 8);
				crc_L=(uint8_t)(crcPayload & 0xFF);
				
				if((crc_H==UartMessage->RxBuf[UartMessage->RxIndex-3])&&(crc_L==UartMessage->RxBuf[UartMessage->RxIndex-2]))	//奇偶校验
				{
					UartMessage->RxState=2;			//校验正确，一包数据接收完成
					UartMessage->dataLen=*(UartMessage->RxBuf+2);
					UartMessage->data=UartMessage->RxBuf+3;
				}
				else
				{
					UartMessage->RxState=0;			//校验失败，通讯错误
				}
				
				UartMessage->RxIndex=0;
			}
		}
	} 
} 

void USART6_IRQHandler(void)
{
	UART_Receive_INT_CallBack(UartType[5],&UartMes[5]);
}

/* support printf function, usemicrolib is unnecessary */
#if (__ARMCC_VERSION > 6000000)
  __asm (".global __use_no_semihosting\n\t");
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
  FILE __stdout;
#else
 #ifdef __CC_ARM
  #pragma import(__use_no_semihosting)
  struct __FILE
  {
    int handle;
  };
  FILE __stdout;
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
 #endif
#endif

#if defined (__GNUC__) && !defined (__clang__)
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/**
  * @brief  retargets the c library printf function to the usart.
  * @param  none
  * @retval none
  */
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart6, (uint8_t *)&ch, 1, 0xFFFF); 
  return ch;
}

/* USER CODE END 1 */
