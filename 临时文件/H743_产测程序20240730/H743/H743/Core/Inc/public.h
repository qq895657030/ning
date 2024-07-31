#ifndef _PUBLIC_H
#define _PUBLIC_H

#include "stm32h7xx_hal.h"

typedef enum 
{
	START_CHK=0,
	SELF_CHK,
	UART_CHK,
	M1_CHK,
	M2_CHK,
	M3_CHK,
	M4_CHK,
	BUZZ_1_CHK,
	BUZZ_2_CHK,
	SW_G_CHK,
	SBUS_CHK,
	GPIO_CHK,
	OSD_CHK,
	GYRO_CHK,
	BARO_CHK,
	PR5V_CHK,
	PRBECV_CHK,
	OSD10V_CHK,
	OSDBAT_CHK,
	CHK_OVER,
	CONNECT,
	TIMEOUT,
}VCP_cmd;


typedef	struct{
	uint8_t 	RxBuf[200];
	uint8_t 	TxBuf[200];
	uint8_t 	RxLen;
	uint8_t 	TxLen;
}USB_dataStru;

typedef	struct{
	uint32_t rpm;			//����ش�ת��
	uint16_t cur;			//����ش�����
	uint16_t vol;			//����ش���ѹ
	uint16_t temperature;	//����ش��¶�
}MotorStru;

//�ɿذ��Բ�ش����ݽṹ�壬�ɿذ��Բ��������IO�ڶ�·���ԣ����ڷɿذ��ϵĸ���ģ��ͨѶ���ԣ���ѹ���������ԣ�
//IO�ڶ�·���������һ�㲻�࣬����ֻ����5�������IO��linkPort1[0]�����·GPIO1��linkPort1[2]�����·GPIO2����4λ����Port����4λ����pin�ţ�0x00����Gnd��0xff����Vcc��
//����linkPort1[0]=0x17��linkPort1[1]=0x20�����ʾPA7��PB0��·��
typedef	struct{
	uint8_t linkPort1[2];	//��·������IO�ڣ�bit7~4����portA��B��C������bit3~0����pin0~15��
	uint8_t linkPort2[2];
	uint8_t linkPort3[2];
	uint8_t linkPort4[2];
	uint8_t linkPort5[2];
	uint8_t osdID;			//OSD״̬����SPIͨѶ�Ƿ�ɹ�
	uint8_t ESCstate;		//���״̬�����ܷ�����ش�����
	uint16_t flashID;		//SPI flash ID
	uint16_t gyroID;		//������ID
	uint16_t baroID;		//��ѹ��ֵ
	uint16_t FCB_VolBat;	//�ɿذ���ĵ�ѹ
	uint16_t FCB_Current;	//�ɿذ���ĵ���
}SelfMessageStru;

uint16_t crc16(uint8_t *buf,uint32_t len);

void decode_MCB_communication_data(void);

#endif

