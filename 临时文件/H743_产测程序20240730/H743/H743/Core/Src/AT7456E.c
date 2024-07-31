#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "AT7456E.h"
#include "spi.h"
//#include "usart.h"
#include "BFdefault.h"

#define AUTO_INC_DIS 0 
//#define AUTO_INC_DIS 1 

#define spix hspi2

//#define  OSD_SPI_CS_LOW()   HAL_GPIO_WritePin(OSC_CS_GPIO_Port, OSC_CS_Pin, GPIO_PIN_RESET)
//#define  OSD_SPI_CS_HIGH()  HAL_GPIO_WritePin(OSC_CS_GPIO_Port, OSC_CS_Pin, GPIO_PIN_SET)

#define  OSD_SPI_CS_LOW()   HAL_GPIO_WritePin(osd_cs_GPIO_Port, osd_cs_Pin, GPIO_PIN_RESET)
#define  OSD_SPI_CS_HIGH()  HAL_GPIO_WritePin(osd_cs_GPIO_Port, osd_cs_Pin, GPIO_PIN_SET)

#define delay(n) HAL_Delay(n)

uint8_t max7456_ID;


static uint8_t read_reg(uint8_t reg)
{
	uint8_t regval = 0;

	OSD_SPI_CS_LOW();
	reg |= 0x80;
	HAL_SPI_Transmit(&spix, &reg, 1, 100);
	HAL_SPI_Receive(&spix, &regval, 1, 100);	
	OSD_SPI_CS_HIGH();

	return regval;
}
static uint8_t read_char(void)
{   
	uint8_t regval = 0;
	OSD_SPI_CS_LOW();
	HAL_SPI_Receive(&spix, &regval, 1, 100);
	OSD_SPI_CS_HIGH();

	return regval;
}

/*static void read_regs(uint8_t reg, uint8_t* buf, uint8_t len)
{   
	OSD_SPI_CS_LOW();
	reg |= 0x80;
	HAL_SPI_Transmit(&spix, &reg, 1, 100);
	HAL_SPI_Receive(&spix, buf, len, 100);
	OSD_SPI_CS_HIGH();
}*/

static uint8_t write_reg(uint8_t reg, uint8_t value)
{
	uint8_t regval = 0;

	OSD_SPI_CS_LOW();
	
	//reg |= 0x80;
	HAL_SPI_Transmit(&spix, &reg, 1, 100);	
	HAL_SPI_Transmit(&spix, &value, 1, 100);	
	
	OSD_SPI_CS_HIGH();

	return regval;
}

/*static uint8_t write_regs(uint8_t reg, uint8_t * buf,uint8_t len)
{
	uint8_t regval = 0;

	OSD_SPI_CS_LOW();
	
	//reg |= 0x80;
	HAL_SPI_Transmit(&spix, &reg, 1, 100);	
	HAL_SPI_Transmit(&spix, buf, len, 100);	
	
	OSD_SPI_CS_HIGH();

	return regval;
}*/

static uint8_t write_char( uint8_t onechar)
{
	uint8_t regval = 0;

	OSD_SPI_CS_LOW();
	
	//reg |= 0x80;
	HAL_SPI_Transmit(&spix, &onechar, 1, 100);	
	
	OSD_SPI_CS_HIGH();

	return regval;
}

void configOSDBL(unsigned char autoLevelCtrl)
{
	unsigned char k;

	k = read_reg(OSDBL);
	
	if (autoLevelCtrl)
		k &= ~(1 << 4);
	else
		k |= (1 << 4);
	
	write_reg(OSDBL, k);
}

void max7456Init(void)
{
	write_reg(VM0,RESET); // CA[8] bit
	delay(10);
	configOSDBL(1);
	
	uint8_t count = 10;
	
	while(count--)
	{
		if( max7456_ID == 0x1B )
		{
//			printf("MAX7456 ID: 0x%x\n",max7456_ID);
			count=0;
		}
		else
		{
			max7456_ID = read_reg(OSDM);
			delay(10);
		}
	}
	
//	if( max7456_ID != 0x1B )
//		printf("MAX7456 is not exist\n");	
}


// AT7456 ��ʾ�ַ� 
void DisplayChar(unsigned char row, unsigned char col, unsigned char c)
{
	unsigned int kk;
	
	kk = row * 30 + col;
	write_reg(DMAH, kk / 256); // address
	write_reg(DMAL, kk % 256);
	write_reg(DMDI, c);
}

// ��ʾ���� 0..9
void DisplayNum(unsigned char row, unsigned char col, unsigned char c)
{
	unsigned int kk;
	
	kk = row * 30 + col;
	write_reg(DMAH, kk / 256); // address
	write_reg(DMAL, kk % 256);
	write_reg(DMDI, (c == 0) ? 10 : c);
}

// ��ʾ���ַ���Ϊ�ַ��������Լ��ո�
void DisplayString(unsigned char row, unsigned char col, unsigned char *s)
{
	unsigned int kk;
	unsigned char c;
	
	kk = row * 30 + col;
	write_reg(DMAH, kk / 256); // address
	write_reg(DMAL, kk % 256);
	write_reg(DMM, 0x01); // Auto Inc
	c = *s++;
	
	while (c != 0)
	{
		if ((c >= '0') && (c <= '9'))
			write_char((c == '0') ? 10 : c - '1' + 1);
		else if ((c >= 'A') && (c <= 'Z'))
			write_char(c - 'A' + 11);
		else if ((c >= 'a') && (c <= 'z'))
			write_char(c - 'a' + 37);
		else if (c == ' ')
			write_char(0x00);
		else if (c == '(')
			write_char(0x3f);
		else if (c == ')')
			write_char(0x40);
		else if (c == '.')
			write_char(0x41);
		else if (c == '?')
			write_char(0x42);
		else if (c == ';')
			write_char(0x43);
		else if (c == ':')
			write_char(0x44);
		else if (c == ',')
			write_char(0x45);
		else if (c == '\'')
			write_char(0x46);
		else if (c == '/')
			write_char(0x47);
		else if (c == '"')
			write_char(0x48);
		else if (c == '-')
			write_char(0x49);
		else if (c == '<')
			write_char(0x4a);
		else if (c == '>')
			write_char(0x4b);
		else if (c == '@')
			write_char(0x4c);
		else
			write_char(c);
		
		c = *s++;
		kk++;
	}
	write_char(0xff); // Exit Auto Inc
}

// AT7456(E)��λ
void at7456_reset(void)
{
// �������λ����Ҫ�����û������ nRESET ���ⲿ 7456
	write_reg(VM0, 2);
	delay(60);// ��λʱ������Ϊ 50ms

//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,0); // nRST = 0
//	delay(60); // ��λ����������Ϊ 50ms
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,1); // nRST = 1
//	delay(60); // ���� 40ms�������޷����ַ�д��оƬ
}

// �ر� OSD
void turnOff_osd(void)
{
	unsigned char k;
	k = read_reg(VM0);
	write_reg(VM0, k & ~(1 << 3)); // VM0[3]=0����ֹ OSD
	delay(1);
}

// �� OSD
void turnOn_osd(void)
{
	unsigned char k;
	k = read_reg(VM0);
	write_reg(VM0, k | (1 << 3)); // VM0[3]=1���� OSD
	delay(1);
}

// ������0 ��ַΪ�ո�
void clear_screen(void)
{
	unsigned int i;
	
	write_reg(DMAH, 0x00); 
	write_reg(DMAL, 0);
	write_reg(DMM, 0x01);
	for (i = 0; i < (16*30); i++)
		write_char(0);
	
	write_char(0xff); 
}

// ͨ��д��/���� VM0.7 ���ж� AT7456 �İ汾���⣬�°���Զ�д VM0.7 λ
// ��ֹ SPI �ӿڿ�·���·����Ҫ�� 0��1 ����״̬
VER7456 checkAT7456(void)
{
	unsigned char r1, r2;
	r1 = read_reg(VM0);
	r2 = (r1 & ~(1 << 1)) | 0x88;
	write_reg(VM0, r2);
	delay(1);
	
	r2 = read_reg(VM0) & 0x88;
	write_reg(VM0, r1);
	delay(1);
	if (r2 == 0x88)
		return NEW7456; 
	else if (r2 == 0x08)
		return OLD7456;
	else
		return BAD7456; 
}

void wait_memory_wr_ok(void)
{
 while ((read_reg(STAT) & (1<<5)) != 0); 
 while ((read_reg(DMM) & (1<<2)) != 0);
}

// �����ֿ�
// addr : �ֿ��ַ��0 .. 255
// dt : ��� 54 �ֽ��ֿ����ݵĵ�ַ
void write_at7456_font(unsigned char addr, unsigned char *dt)
{
	unsigned char n;

	write_reg(CMAH, addr);
	for (n=0; n<54; n++)
	{
		write_reg(CMAL, n);
		write_reg(CMDI, *dt++);
	}
	write_reg(CMM, RAM_NVM);
	// AT7456 = 2.92ms
	wait_memory_wr_ok();
}

// ���ֿ�����
// addr : �ֿ��ַ��0 .. 255
// dt : ��� 54 �ֽ��ֿ����ݵĵ�ַ
void read_at7456_font(unsigned char addr, unsigned char *dt)
{
	unsigned char n;
	write_reg(CMAH, addr);
	
	write_reg(CMM, NVM_RAM);
	delay(1); 
	for (n=0; n<54; n++)
	{
		write_reg(CMAL, n);
		*dt++ = read_reg(CMDO);
	}
}

/*
 AT7456E �ֿ��д��512 ���ֿ�
*/

// �����ֿ�
// addr : �ֿ��ַ��0 .. 511
// dt : ��� 54 �ֽ��ֿ����ݵĵ�ַ
void write_at7456E_font(unsigned short addr, unsigned char *dt)
{
	unsigned char addr_h, n;
	addr_h = (addr >> 8);
	write_reg(CMAH, addr);
	
	if (AUTO_INC_DIS)
		write_reg(DMM, 0); 
	else
	{
		write_reg(CMAL, (addr_h << 6)); 
		write_reg(DMM, 0x80); 
	}
	
	for (n = 0; n < 54; n++)
	{
		if (AUTO_INC_DIS) 
		{
			write_reg(CMAL, n | (addr_h << 6));
			write_reg(CMDI, *dt++); 
		}
		else
		write_char(*dt++);
	}
	
	write_reg(CMM, RAM_NVM);
	// AT7456 = 2.92ms
	wait_memory_wr_ok(); 
}

// ���ֿ�����
// addr : �ֿ��ַ��0 .. 511
// dt : ��� 54 �ֽ��ֿ����ݵĵ�ַ
void read_at7456E_font(unsigned short addr, unsigned char *dt)
{
	unsigned char addr_h, n;
	addr_h = (addr >> 8);
	write_reg(CMAH, addr);
	write_reg(CMAL, (addr_h << 6)); 
	
	
	write_reg(CMM, NVM_RAM);
	delay(1);
	for (n = 0; n < 54; n++)
	{
		write_reg(CMAL, n);
		*dt++ = read_reg(CMDO);
	}
}

/*
 �ַ���д���������ڲ���ǰ 256 ���ֿ���ַ�
*/
// row����ʾ�ַ��кţ�0..15����col����ʾ�ַ����кţ�0..29��
// ca����ȡ�����ַ�����
void at7456_read_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;

	k = row * 30 + col;
	write_reg(DMM, (0 << 6));
	write_reg(DMAH, k >> 8); 
	write_reg(DMAL, k); 
	ca->index = read_reg(DMDO); 
	ca->attrib = read_char() >> 4;
}

// row����ʾ�ַ��кţ�0..15����col����ʾ�ַ����кţ�0..29��
// ca����д�ַ�����
void at7456_write_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;

	write_reg(DMM, ca->attrib << 3);
	k = row * 30 + col;
	write_reg(DMAH, k >> 8);
	write_reg(DMAL, k);
	write_reg(DMDI, ca->index); 
}

/*
 AT7456E �ַ���д��Ҳ������ AT7456
*/
// row����ʾ�ַ��кţ�0..15����col����ʾ�ַ����кţ�0..29��
// ca����ȡ�����ַ�����
void at7456E_read_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;
	unsigned char addr_h, j;

	k = row * 30 + col;
	addr_h = k >> 8;

	write_reg(DMM, (1 << 6));    
	write_reg(DMAH, addr_h);     
	write_reg(DMAL, k);          
	ca->index = read_reg(DMDO);   
	
	write_reg(DMAH, addr_h | 2);        
	write_reg(DMAL, k);                
	j = read_reg(DMDO);                
	ca->attrib = j >> 5;
	
	if (j & (1 << 4))
		ca->index |= 0x100;
}

// row:��ʾ�ַ��к�(0..15);col:��ʾ�ַ��к�(0..29)
// ca:��д�ַ�����
void at7456E_write_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;
	unsigned char addr_h, j;

	k = row * 30 + col;
	addr_h = k >> 8;

	write_reg(DMM, (1 << 6)); 
	write_reg(DMAH, addr_h | 0x2); 
	write_reg(DMAL, k);
	
	j = ca->attrib << 5; 
	if ((ca->index >> 8) != 0)
		j |= 0x10; 
	write_reg(DMDI, j);
	
	write_reg(DMAH, addr_h);
	write_reg(DMAL, k);
	write_reg(DMDI, ca->index); 
}


void update_AT7456E_font(void)
{

	for (uint16_t chr=0; chr < 256; chr++) 
	{
		write_at7456E_font(chr,BFDefault[chr]);	
		delay(5);
	}

}




