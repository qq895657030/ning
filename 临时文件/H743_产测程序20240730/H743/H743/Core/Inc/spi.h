/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;

extern SPI_HandleTypeDef hspi2;

extern SPI_HandleTypeDef hspi3;

extern SPI_HandleTypeDef hspi4;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_SPI1_Init(void);
void MX_SPI2_Init(void);
void MX_SPI3_Init(void);
void MX_SPI4_Init(void);

/* USER CODE BEGIN Prototypes */
//SPI1
#define	SPI1_CS_PORT	GPIOC
#define	SPI1_CS_PIN	GPIO_PIN_15

#define	SPI1_SCLK_PORT	GPIOA
#define	SPI1_SCLK_PIN	GPIO_PIN_5

#define	SPI1_MISO_PORT	GPIOA
#define	SPI1_MISO_PIN	GPIO_PIN_6

#define	SPI1_MOSI_PORT	GPIOD
#define	SPI1_MOSI_PIN	GPIO_PIN_7

//SPI2
#define	SPI2_CS_PORT	GPIOB
#define	SPI2_CS_PIN	GPIO_PIN_12

#define	SPI2_SCLK_PORT	GPIOB
#define	SPI2_SCLK_PIN	GPIO_PIN_13

#define	SPI2_MISO_PORT	GPIOB
#define	SPI2_MISO_PIN	GPIO_PIN_14

#define	SPI2_MOSI_PORT	GPIOB
#define	SPI2_MOSI_PIN	GPIO_PIN_15

//SPI3
#define	SPI3_CS_PORT	GPIOE
#define	SPI3_CS_PIN	GPIO_PIN_2

#define	SPI3_SCLK_PORT	GPIOB
#define	SPI3_SCLK_PIN	GPIO_PIN_3

#define	SPI3_MISO_PORT	GPIOB
#define	SPI3_MISO_PIN	GPIO_PIN_4

#define	SPI3_MOSI_PORT	GPIOB
#define	SPI3_MOSI_PIN	GPIO_PIN_5

//SPI4
#define	SPI4_CS_PORT	GPIOE
#define	SPI4_CS_PIN	GPIO_PIN_11

#define	SPI4_SCLK_PORT	GPIOE
#define	SPI4_SCLK_PIN	GPIO_PIN_12

#define	SPI4_MISO_PORT	GPIOE
#define	SPI4_MISO_PIN	GPIO_PIN_13

#define	SPI4_MOSI_PORT	GPIOE
#define	SPI4_MOSI_PIN	GPIO_PIN_14
/* USER CODE END Prototypes */

void MySPI_GPIO_Init(void);
uint8_t MySPI_SwapByte(uint8_t spiNum,uint8_t dat);
void user_SPI_Transmit(uint8_t spiNum, uint8_t *pData, uint16_t Size);
void user_SPI_Receive(uint8_t spiNum, uint8_t *pData, uint16_t Size);

#define HARD_SPI	1

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

