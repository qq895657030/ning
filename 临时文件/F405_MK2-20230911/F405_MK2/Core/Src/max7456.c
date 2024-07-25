/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of printf or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "max7456.h"
#include "spi.h"


#define  OSD_SPI_CS_LOW()   HAL_GPIO_WritePin(osd_cs_GPIO_Port, osd_cs_Pin, GPIO_PIN_RESET)
#define  OSD_SPI_CS_HIGH()  HAL_GPIO_WritePin(osd_cs_GPIO_Port, osd_cs_Pin, GPIO_PIN_SET)

#define osd_spi hspi2

/*
static void read_regs(uint8_t reg, uint8_t* buf, uint16_t len)
{   
	OSD_SPI_CS_LOW();
	reg |= 0x80;
	HAL_SPI_Transmit(&osd_spi, &reg, 1, 100);
	HAL_SPI_Receive(&osd_spi, buf, len, 100);
	OSD_SPI_CS_HIGH();
}*/

static uint8_t read_reg(uint8_t reg)
{
	uint8_t regval = 0;

	OSD_SPI_CS_LOW();
	reg |= 0x80;
	HAL_SPI_Transmit(&osd_spi, &reg, 1, 100);
	HAL_SPI_Receive(&osd_spi, &regval, 1, 100);	
	OSD_SPI_CS_HIGH();

	return regval;
}

static uint8_t write_reg(uint8_t reg, uint8_t value)
{
	uint8_t regval = 0;

	OSD_SPI_CS_LOW();
	
	//reg |= 0x80;
	HAL_SPI_Transmit(&osd_spi, &reg, 1, 100);	
	HAL_SPI_Transmit(&osd_spi, &value, 1, 100);	
	
	OSD_SPI_CS_HIGH();

	return regval;
}


uint8_t OSD_ID;

void max7456Init(void)
{
	//write_reg(MAX7456ADD_VM0,MAX7456_CLOSE); // CA[8] bit
	//HAL_Delay(10);
	
	OSD_ID = read_reg(MAX7456ADD_OSDM);
	
//	if( OSD_ID == 0x1B )
//		printf("MAX7456 ID: 0x%x\n",OSD_ID);
//	else
//		printf("MAX7456 is not exist\n");
	
	write_reg(MAX7456ADD_CMAL,(1<<6)); // CA[8] bit
//	HAL_Delay(5);
	
//	if(read_reg(MAX7456ADD_CMAL) & (1<<6)){
//		printf("AT7456\n");
//	}else{
//		printf("MAX7456\n");
//	}
	
	write_reg(MAX7456ADD_VM0,VERTICAL_SYNC_NEXT_VSYNC); // P制
//	HAL_Delay(200);
	// Wait for reset to complete
	//while ((read_reg( MAX7456ADD_VM0) & VERTICAL_SYNC_NEXT_VSYNC) != 0x00);	
	
	write_reg(MAX7456ADD_DMM,0x40); //8bit
//	HAL_Delay(200);
	
	write_reg(MAX7456ADD_VM0,0x48);
//	HAL_Delay(200);

	write_reg(MAX7456ADD_VM1,0x00);
//	HAL_Delay(200);

	write_reg(MAX7456ADD_OSDBL,(read_reg(MAX7456ADD_OSDRL))|0x10);
//	HAL_Delay(200);

	write_reg(MAX7456ADD_OSDM,0x2d);
//	HAL_Delay(200);	
}


void max7456_write_char(uint16_t add,uint8_t chr)
{
	write_reg(MAX7456ADD_DMAH,(add&0XFF00)>>8);
	write_reg(MAX7456ADD_DMAL,(add&0XFF));
	write_reg(MAX7456ADD_DMDI,chr);
}

void max7456_write_att_char(uint16_t add,uint8_t att)
{
	write_reg(MAX7456ADD_DMAH,((add&0XFF00)>>8) | 0x02);
	write_reg(MAX7456ADD_DMAL,(add&0XFF));
	write_reg(MAX7456ADD_DMDI,att);
}


void max7456_write_char_XY(uint8_t X,uint8_t Y,uint8_t chr) 
{ 
	uint16_t add=0; 
	add=((uint16_t)Y)*30+X; 
	max7456_write_char(add,chr); 
} 

void max7456_write_att_char_XY(uint8_t X,uint8_t Y,uint8_t chr) 
{ 
	uint16_t add=0; 
	add=((uint16_t)Y)*30+X; 
	max7456_write_att_char(add,chr); 
} 

void max7456_Clear(void) 
{ 
	uint16_t memory_address = 0,i=0; 
	for (i = 0; i < 480; i++)  
		max7456_write_char(memory_address++, 0); 
} 

void max7456_Write_ASCII_Char(uint16_t address, uint8_t c)  
{ 
    if (c == 32) c = 0; // remap space 
    else if (c > 48 && c <= 57) c -= 48; // remap numbers 
    else if (c == '0') c = 10; // remap zero 
    else if (c >= 65 && c <= 90) c -= 54; // remap big letters 
    else if (c >= 97 && c <= 122) c -= 60; // remap small letters 
    else if (c == '(') c = 63; // remap 
    else if (c == ')') c = 64; // remap 
    else if (c == '.') c = 65; // remap 
    else if (c == '?') c = 66; // remap 
    else if (c == ';') c = 67; // remap 
    else if (c == ':') c = 68; // remap 
    else if (c == ',') c = 69; // remap 
    else if (c == '\'') c = 70; // remap 
    else if (c == '/') c = 71; // remap 
    else if (c == '"') c = 72; // remap 
    else if (c == '-') c = 73; // remap minus 
    else if (c == '<') c = 74; // remap 
    else if (c == '>') c = 75; // remap 
    else if (c == '@') c = 76; // remap 
    max7456_write_char(address, c); 
} 

void max7456_Write_ASCII_Chr(uint8_t X,uint8_t Y,uint8_t chr) 
{ 
	max7456_Write_ASCII_Char(((X++)+(((uint16_t)Y*30))),chr); 
} 

void max7456_Write_ASCII_String(uint8_t X,uint8_t Y,uint8_t *Str) 
{ 
	while(*Str) 
	{ 
		max7456_Write_ASCII_Char(((X++)+(((uint16_t)Y*30))),*Str); 
		Str++; 
	} 
} 

void max7456_Learn_Char(uint8_t number, uint8_t *data)  
{ 
	uint8_t i=0; 
	// select character to write (CMAH) 
	write_reg(MAX7456ADD_CMAH, number); 

	for (i = 0; i < 54; i++) { 
		// select 4pixel byte of char (CMAL) 
		write_reg(MAX7456ADD_CMAL, i); 

		// write 4pixel byte of char (CMDI) 
		write_reg(MAX7456ADD_CMDI, data[i]); 
	} 

	// write to the NVM array from the shadow RAM (CMM) 
	write_reg(MAX7456ADD_CMM, 0xa0); 

	// according to maxim writing to nvram takes about 12ms, lets wait longer 
//	HAL_Delay(500);//修改延时看能否写入 
} 


//从字库文件中提取字符写入MAX7456 
/*void Max7456_Write_FileChar(void) 
{ 
	uint16_t i=0; 
	for(i=0;i<=255;i++) 
	max7456_Learn_Char(i,chars[i]); 
} */

//将字库中的文件显示出来，以测试使用 
void Max7456_Display_AllChar(void) 
{ 
	uint16_t i=0; 
	for(i=0;i<25;i++) 
	max7456_write_char_XY(i, 1, i); 
	for(i=25;i<50;i++) 
	max7456_write_char_XY(i-25, 2, i); 
	for(i=50;i<75;i++) 
	max7456_write_char_XY(i-50, 3, i); 

	for(i=75;i<100;i++) 
	max7456_write_char_XY(i-75, 4, i); 
	for(i=100;i<125;i++) 
	max7456_write_char_XY(i-100, 5, i); 
	for(i=125;i<150;i++) 
	max7456_write_char_XY(i-125, 6, i); 

	for(i=150;i<175;i++) 
	max7456_write_char_XY(i-150, 7, i); 
	for(i=175;i<200;i++) 
	max7456_write_char_XY(i-175, 8, i); 
	for(i=200;i<225;i++) 
	max7456_write_char_XY(i-200, 9, i); 

	for(i=225;i<250;i++) 
	max7456_write_char_XY(i-225, 10, i); 

	for(i=250;i<=255;i++) 
	max7456_write_char_XY(i-250, 11, i); 
}












