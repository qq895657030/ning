#ifndef _PUBLIC_H
#define _PUBLIC_H

#include "main.h"

typedef enum 
{
	START_CHK = 0, 
	SELF_CHK,
	UART_CHK,
	M1_CHK,
	M2_CHK,
	M3_CHK,
	M4_CHK,
	M5_8_CHK,
	CAN_CHK,
	SPI_CHK,
	SBUS_CHK,
	GPIO_CHK,
	OSD_CHK,
	GYRO_CHK,
	BARO_CHK,
	PR5V_CHK,
	PR10V_CHK,
	OSD10V_CHK,
	OSDBAT_CHK,
}VCP_cmd;


typedef	struct{
	uint8_t 	RxBuf[200];
	uint8_t 	TxBuf[200];
	uint8_t 	RxLen;
	uint8_t 	TxLen;
}USB_dataStru;

typedef	struct{
	uint32_t rpm;			//电调回传转速
	uint16_t cur;			//电调回传电流
	uint16_t vol;			//电调回传电压
	uint16_t temperature;	//电调回传温度
}MotorStru;

//飞控板自测回传数据结构体，飞控板自测包括相邻IO口短路测试，挂在飞控板上的各个模块通讯测试，电压、电流测试；
//IO口短路关联情况的一般不多，这里只留出5组关联的IO，linkPort1[0]代表短路GPIO1，linkPort1[2]代表短路GPIO2，高4位代表Port，低4位代表pin脚，0x00代表Gnd，0xff代表Vcc；
//例如linkPort1[0]=0x17，linkPort1[1]=0x20，则表示PA7和PB0短路；
typedef	struct{
	uint8_t linkPort1[2];	//短路的两个IO口，bit7~4代表portA、B、C……，bit3~0代表pin0~15，
	uint8_t linkPort2[2];
	uint8_t linkPort3[2];
	uint8_t linkPort4[2];
	uint8_t linkPort5[2];
	uint8_t osdID;			//OSD状态，即SPI通讯是否成功
	uint8_t ESCstate;		//电调状态，即能否读到回传数据
	uint16_t flashID;		//SPI flash ID
	uint16_t gyroID;		//陀螺仪ID
	uint16_t baroID;		//气压计值
	uint16_t FCB_VolBat;	//飞控板检测的电压
	uint16_t FCB_Current;	//飞控板检测的电流
}SelfMessageStru;

uint16_t crc16(uint8_t *buf,uint32_t len);

void decode_MCB_communication_data(void);
void Send_packet(uint8_t uart_num,uint8_t cmd,uint8_t *payload,uint8_t lenPay);
void test_tx(void);
#endif

