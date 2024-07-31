/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart5;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART6_Init(void);

/* USER CODE BEGIN Prototypes */
typedef	struct{
	uint8_t 	RxBuf[50];
	uint8_t 	RxIndex;
	uint8_t 	RxState;
	uint8_t		dataLen;
	uint8_t		*data;
}Uart_Stru;

void USART_Send_string(USART_TypeDef* usart_x,uint8_t *str,uint32_t length);


extern Uart_Stru UartMes[6];
extern USART_TypeDef* UartType[6];
void UART_Receive_INT_CallBack(USART_TypeDef* usart_x,Uart_Stru* UartMessage);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

