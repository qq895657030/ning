#ifndef   STM32FCB_gpioCfg_H
#define   STM32FCB_gpioCfg_H
  
//  164_H743
  
#define   PAD0_PORT      GPIOA    //RX1_1
#define   PAD0_PIN      GPIO_PIN_10
  
#define   PAD1_PORT      GPIOA    //TX1_1
#define   PAD1_PIN      GPIO_PIN_9
  
#define   PAD2_PORT      GPIOB    //SDA2_1
#define   PAD2_PIN      GPIO_PIN_11
  
#define   PAD3_PORT      GPIOB    //SCL2_1
#define   PAD3_PIN      GPIO_PIN_10
  
#define   PAD4_PORT      GPIOE    //RX8_1
#define   PAD4_PIN      GPIO_PIN_0
  
#define   PAD5_PORT      GPIOE    //TX8_1
#define   PAD5_PIN      GPIO_PIN_1
  
#define   PAD6_PORT      GPIOB    //SDA1_1
#define   PAD6_PIN      GPIO_PIN_7
  
#define   PAD7_PORT      GPIOB    //SCL1_1
#define   PAD7_PIN      GPIO_PIN_6
  
#define   PAD8_PORT      GPIOD    //PWM4_1
#define   PAD8_PIN      GPIO_PIN_14
  
#define   PAD9_PORT      GPIOE    //PWM1_1
#define   PAD9_PIN      GPIO_PIN_5
  
#define   PAD10_PORT      GPIOE    //PWM2_1
#define   PAD10_PIN      GPIO_PIN_6
  
#define   PAD11_PORT      GPIOD    //PWM3_1
#define   PAD11_PIN      GPIO_PIN_15
  
#define   PAD12_PORT      GPIOA    //E5_1
#define   PAD12_PIN      GPIO_PIN_2
  
#define   PAD13_PORT      GPIOA    //E6_1
#define   PAD13_PIN      GPIO_PIN_3
  
#define   PAD14_PORT      GPIOD    //E7_1
#define   PAD14_PIN      GPIO_PIN_12
  
#define   PAD15_PORT      GPIOD    //E8_1
#define   PAD15_PIN      GPIO_PIN_13
  
#define   PAD16_PORT      GPIOD    //RX2_1
#define   PAD16_PIN      GPIO_PIN_6
  
#define   PAD17_PORT      GPIOD    //TX2_1
#define   PAD17_PIN      GPIO_PIN_5
  
#define   PAD18_PORT      GPIOB    //RX4_1
#define   PAD18_PIN      GPIO_PIN_8
  
#define   PAD19_PORT      GPIOB    //TX4_1
#define   PAD19_PIN      GPIO_PIN_9
  
#define   PAD20_PORT      GPIOA     //NULL
#define   PAD20_PIN      0xffff
  
#define   PAD21_PORT      GPIOC    //CUR_1
#define   PAD21_PIN      GPIO_PIN_1
  
#define   PAD22_PORT      GPIOD    //RX3_1
#define   PAD22_PIN      GPIO_PIN_9
  
#define   PAD23_PORT      GPIOB    //E1_1
#define   PAD23_PIN      GPIO_PIN_0
  
#define   PAD24_PORT      GPIOB    //E2_1
#define   PAD24_PIN      GPIO_PIN_1
  
#define   PAD25_PORT      GPIOA    //E3_1
#define   PAD25_PIN      GPIO_PIN_0
  
#define   PAD26_PORT      GPIOA    //E4_1
#define   PAD26_PIN      GPIO_PIN_1
  
#define   PAD27_PORT      GPIOE    //RX7_1
#define   PAD27_PIN      GPIO_PIN_7
  
#define   PAD28_PORT      GPIOE    //TX7_1
#define   PAD28_PIN      GPIO_PIN_8
  
#define   PAD29_PORT      GPIOA     //NULL
#define   PAD29_PIN      0xffff
  
#define   PAD30_PORT      GPIOA     //NULL
#define   PAD30_PIN      0xffff
  
#define   PAD31_PORT      GPIOA     //NULL
#define   PAD31_PIN      0xffff
  
#define   PAD_MASK     0x1FEFFFFF
  
#define	BAT_ADC_PORT	GPIOC        //BAT_ADC
#define  BAT_ADC_PIN     GPIO_PIN_0
  
#define	BARO_TYPE	BMP280        //气压计类型
#define	GYRO_TYPE	ICM42688        //陀螺仪类型
#define	GYRO_TYPE2      ICM42688        //陀螺仪2类型
  
  
#endif
  
