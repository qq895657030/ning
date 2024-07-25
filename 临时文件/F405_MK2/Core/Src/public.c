#include "public.h"
#include <string.h>
#include "w25qxxx.h"
#include "max7456.h"
#include "icm426xx.h"
//#include "spl06.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"
#include "STM32FCB_gpioCfg.h"

#define	USB_VCP		8
#define	MAIN_UART		5


MotorStru MotorData;			//电机参数
//uint8_t uartTxState;				//uart测试结果，每一个bit代表每一路uart测试结果，1：表示ok，0表示不ok
uint8_t uartRxState=0;				//uart测试结果，每一个bit代表每一路uart测试结果，1：表示ok，0表示不ok
uint8_t CAN_state;				//CAN测试结果，1：表示ok，0表示不ok
uint8_t SPI_state;				//SPI测试结果，每一个bit代表每一路SPI测试结果，1：表示ok，0表示不ok
uint8_t I2C_state;				//I2C测试结果，每一个bit代表每一路I2C测试结果，1：表示ok，0表示不ok
uint16_t GPIO_state;
uint16_t Throttle;
uint16_t Chk_cmd;				//检测命令		
uint8_t flow=0;
uint8_t testMessage[]={'H','A','K','R','C'};
uint8_t baroInitFlag;
uint32_t rxCnt,txCnt;

static uint8_t Get_SerialNum(uint8_t* data)
{ 
	uint8_t i=0;
	uint32_t Device_ID[3];

	Device_ID[0] = *(__IO  uint32_t*)(0x1FFF7A10);
	Device_ID[1] = *(__IO  uint32_t*)(0x1FFF7A14);
	Device_ID[2] = *(__IO  uint32_t*)(0x1FFF7A18);   
	
	data[i++]=Device_ID[0]>>24;
	data[i++]=Device_ID[0]>>16;
	data[i++]=Device_ID[0]>>8;
	data[i++]=Device_ID[0];

	data[i++]=Device_ID[1]>>24;
	data[i++]=Device_ID[1]>>16;
	data[i++]=Device_ID[1]>>8;
	data[i++]=Device_ID[1];

	data[i++]=Device_ID[2]>>24;
	data[i++]=Device_ID[2]>>16;
	data[i++]=Device_ID[2]>>8;
	data[i++]=Device_ID[2];
	
	return i;
}

uint16_t crc16(uint8_t *buf,uint32_t len)
{
	uint16_t temp;
	uint32_t i;
	uint16_t cksum = 0;
	
	for (i = 0; i < len; i++) 
	{
		temp=*buf++;
		cksum += (((temp^0x5a)<<8) + temp^0xa5);
	}
	
	return cksum;
}

void user_memcopy(uint8_t * buf1,uint8_t * buf2,uint8_t len)
{
	uint8_t i;
	
	for(i=0;i<len;i++)
	{
		buf1[i]=buf2[i];
	}
}

//发送一包数据函数
//uart_num:串口号0~7代表uart1~8，串口号8代表USB VCP
//cmd：要发送的命令
//payload：要发送的数据首地址
//lenPay：要发送的数据长度
void Send_packet(uint8_t uart_num,uint8_t cmd,uint8_t *payload,uint8_t lenPay)
{
	uint16_t crcPayload;// = crc16(payload, lenPay);
	uint16_t count = 0;
	uint8_t messageSend[40];

	messageSend[count++] = 0xf0;
	messageSend[count++] = cmd;
	messageSend[count++] = lenPay;
	
	if(lenPay)
	{
//		memcpy(&messageSend[count], payload, lenPay);
		user_memcopy(&messageSend[count], payload, lenPay);
	}
	
	count += lenPay;
	crcPayload = crc16(messageSend, count);
	messageSend[count++] = (uint8_t)(crcPayload >> 8);
	messageSend[count++] = (uint8_t)(crcPayload & 0xFF);
	messageSend[count++] = 0xff;
	
	if(uart_num<8)
	{
		USART_Send_string(UartType[uart_num],messageSend, count);
	}
	else
	{
		//CDC_Transmit_FS(messageSend, count);
	}
}


uint8_t ICname[6]={'S','7','2','2','R','E'};	//STM32F722RETX
uint8_t IOtestIx;
uint32_t baro_pres=0;
//解码测试板（MCB）通讯（uart，SPI，iic，can）传递过来的数据
void decode_MCB_communication_data(void)	
{
	uint8_t buf[30];
	uint8_t i=0;
	uint8_t Temp;
//	uint8_t tmp,STmp;
	
	if(UartMes[MAIN_UART].RxState==2)
	{
		UartMes[MAIN_UART].RxState=0;
		Chk_cmd=UartMes[MAIN_UART].RxBuf[1];
		rxCnt++;
		
		switch(Chk_cmd)
		{
			case	START_CHK:
				i=Get_SerialNum(buf);
				memcpy(buf+i,ICname,6);		//UniqueID
				i+=6;
				Send_packet(MAIN_UART,START_CHK,buf,i);
				break;
			
			case	SELF_CHK:
//				for(uint8_t i=0; i<10; i++)
//				{
//					buf[i] = w25_R[i];
//				}
		
				i=0;
				
				buf[i++] = 0;//OSD_ID;
				buf[i++] = 0x00; // 预留
				buf[i++] = 0;//Flash_ID;
				buf[i++] = 0x00;
				buf[i++] = 0x00;	
				buf[i++] = Imu_ID;	
				buf[i++] = 0;	
				buf[i++] = 0;//Baro_ID;	

				buf[i++] = VolBat>>8;
				buf[i++] = VolBat&0xFF;

				buf[i++] = 0;
				buf[i++] = 0;
			
				Send_packet(MAIN_UART,SELF_CHK,buf,i);
			
				AT_GPIO_Init();
				break;
				
			case	GYRO_CHK:
				Imu_Test();
				memcpy(buf,&IumRawData,sizeof(imuData_t));
				Send_packet(MAIN_UART,GYRO_CHK,buf,sizeof(imuData_t));
				break;
			
			case	BARO_CHK:
				#if 0
				if(baroInitFlag==0)
				{
					baroInitFlag=1;
					hi2c1.i2cx = I2Cx_PORT;	
					i2c_config(&hi2c1);
					delay_ms(1);
				}
				
				baro_pres = baro_test();
				
				i=0;
				buf[i++]=baro_pres>>24;
				buf[i++]=baro_pres>>16;
				buf[i++]=baro_pres>>8;
				buf[i++]=baro_pres;
				
				Send_packet(MAIN_UART,BARO_CHK,buf,i);
			#endif
				break;
				
			case	SBUS_CHK:
				Temp=*UartMes[MAIN_UART].data;
				buf[0]=HAL_GPIO_ReadPin(SBUS_PORT,SBUS_PIN);
				Send_packet(MAIN_UART,SBUS_CHK,buf,1);
				break;
			
			case	GPIO_CHK:
				baroInitFlag=0;
			
				for(i=0;i<32;i++)
				{
					if(i<29)
					{
						if(OutPin[i]!=0xffff)
						{
							HAL_GPIO_WritePin(OutPort[i], OutPin[i], GPIO_PIN_RESET);
						}
					}
					else	if(i>29)
					{
						if(OutPin[i]!=0xffff)
						{
							HAL_GPIO_WritePin(OutPort[i], OutPin[i], GPIO_PIN_SET);
						}
					}
				}
				
				IOtestIx=0;
				Send_packet(MAIN_UART,GPIO_CHK,&IOtestIx,1);
				
				for(i=0;i<32;i++)
				{
					HAL_Delay(40);
					
					if(i<29)
					{
						if(OutPin[i]!=0xffff)
						{
							HAL_GPIO_WritePin(OutPort[i], OutPin[i], GPIO_PIN_SET);
						}
					}		
					else	if(i>29)
					{
						if(OutPin[i]!=0xffff)
						{
							HAL_GPIO_WritePin(OutPort[i], OutPin[i], GPIO_PIN_RESET);
						}
					}
					
					IOtestIx=i+1;
					Send_packet(MAIN_UART,GPIO_CHK,&IOtestIx,1);
				}
				break;

		}
	}
}

void test_tx(void)
{
	uint8_t buf[7]={0xF0, 0x0B,0x01,0x00,0xb2, 0x4c, 0xFF};
	
//	if(userTimer1==0)
	{
		txCnt++;
//		userTimer1=10;
		
		USART_Send_string(USART1,buf, 7);
		HAL_Delay(2);
	}
}

