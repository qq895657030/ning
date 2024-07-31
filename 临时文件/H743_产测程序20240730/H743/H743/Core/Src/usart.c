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
#include "public.h"
Uart_Stru UartMes[6];
USART_TypeDef* UartType[6]={USART1,USART2,USART3,UART4,UART5,USART6};
/* USER CODE END 0 */

UART_HandleTypeDef huart6;

void MX_USART6_Init(void)
{
    huart6.Instance = USART6;
	huart6.Init.BaudRate = 115200;
	huart6.Init.WordLength = UART_WORDLENGTH_8B;
	huart6.Init.StopBits = UART_STOPBITS_1;
	huart6.Init.Parity = UART_PARITY_NONE;
	huart6.Init.Mode = UART_MODE_TX_RX;
	huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart6.Init.OverSampling = UART_OVERSAMPLING_16;
	huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart6.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	
	if (HAL_UART_Init(&huart6) != HAL_OK)
	{
		Error_Handler();
	}
}

//UART发送字符串
void USART_Send_string(USART_TypeDef* usart_x,uint8_t *str,uint32_t length)  
{  
	uint32_t i;
	for(i=0;i<length;i++)
	{
		usart_x->TDR=*str;
		while((usart_x->ISR & UART_FLAG_TXE) == 0);
		str++;
	}
}

//UART接收packet中断回调函数
void UART_Receive_INT_CallBack(USART_TypeDef* usart_x,Uart_Stru* UartMessage)  
{
	uint8_t Res;
	uint16_t crcPayload;
	uint16_t crc_H,crc_L;
	
	if((usart_x->	ISR & UART_FLAG_RXNE) != RESET)
	{
		usart_x->ISR &= ~UART_FLAG_RXNE;
			/* read one byte from the receive data register */
		Res = usart_x->RDR;

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
	
	usart_x->ISR=0;
} 

void USART6_IRQHandler(void)
{
	UART_Receive_INT_CallBack(USART6,&UartMes[5]);
}

/* USER CODE END 1 */
