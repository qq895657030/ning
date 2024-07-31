#include "bmp280.h"

uint8_t Baro_ID=0;

#define bmp280_i2c hi2c2

static void bmp280_Read_Bytes(uint8_t reg,uint8_t *dat,uint8_t n)
{
	HAL_I2C_Mem_Read(&bmp280_i2c, BMP280_Raddr, reg, I2C_MEMADD_SIZE_8BIT, dat, n, 0xffff);
}

static uint8_t bmp280_Read_Byte(uint8_t reg)
{
	uint8_t rec_data;
	
	HAL_I2C_Mem_Read(&bmp280_i2c, BMP280_Raddr, reg, I2C_MEMADD_SIZE_8BIT, &rec_data, 1, 0xffff);
	return rec_data;
}

static void bmp280_Write_Byte(uint8_t reg,uint8_t data)
{
	HAL_I2C_Mem_Write(&bmp280_i2c, BMP280_Waddr, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff);
}


uint8_t bmp280_ReadID(void)
{
	return bmp280_Read_Byte(BMP280_CHIPID_REG);
}


//获取BMP当前状态
uint8_t  BMP280_GetStatus(uint8_t status_flag)
{
	uint8_t flag;
	flag = bmp280_Read_Byte(BMP280_STATUS_REG);
	if(flag&status_flag)	return SET;
	else return RESET;
}


BMP280 bmp280_inst;
BMP280* bmp280 = &bmp280_inst;		//这个全局结构体变量用来保存存在芯片内ROM补偿参数
void bmp280_init(void)
{
	uint8_t buf[24];
	
	bmp280_Read_Bytes(BMP280_DIG_T1_LSB_REG,buf,24);
  bmp280->T1 = ((int16_t)buf[1] << 8) | buf[0];
	bmp280->T2 = ((int16_t)buf[3] << 8) | buf[2];
	bmp280->T3 = ((int16_t)buf[5] << 8) | buf[4];
	
	bmp280->P1 = ((int16_t)buf[7] << 8) | buf[6];
	bmp280->P2 = ((int16_t)buf[9] << 8) | buf[8];
	bmp280->P3 = ((int16_t)buf[11] << 8) | buf[10];
	bmp280->P4 = ((int16_t)buf[13] << 8) | buf[12];
	bmp280->P5 = ((int16_t)buf[15] << 8) | buf[14];
	bmp280->P6 = ((int16_t)buf[17] << 8) | buf[16];
	bmp280->P7 = ((int16_t)buf[19] << 8) | buf[18];
	bmp280->P8 = ((int16_t)buf[21] << 8) | buf[20];
	bmp280->P9 = ((int16_t)buf[23] << 8) | buf[22];
		
	/******************************************************/
	bmp280_Write_Byte(BMP280_RESET_REG,BMP280_RESET_VALUE);	//往复位寄存器写入给定值
//	HAL_Delay(10);
	
	bmp280_Write_Byte(BMP280_CTRLMEAS_REG,(BMP280_OVERSAMPLING_T << 5) | (BMP280_OVERSAMPLING_P << 2) | BMP280_MODE);	
//	HAL_Delay(10);
	
	bmp280_Write_Byte(BMP280_CONFIG_REG,BMP280_FILTER_COEFFICIENT_2 << 2);
//	HAL_Delay(10);
	
	Baro_ID = bmp280_ReadID();
}

// calculate pressure
float _update_pressure(int32_t press_raw)
{
	int64_t var1, var2, p;
	int32_t _t_fine;
	
	// according to datasheet page 22
	var1 = ((int64_t)_t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)bmp280->P6;
	var2 = var2 + ((var1 * (int64_t)bmp280->P5) << 17);
	var2 = var2 + (((int64_t)bmp280->P4) << 35);
	var1 = ((var1 * var1 * (int64_t)bmp280->P3) >> 8) + ((var1 * (int64_t)bmp280->P2) << 12);
	var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)bmp280->P1) >> 33;

	if (var1 == 0) 
		return 0;

	p = 1048576 - press_raw;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t)bmp280->P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t)bmp280->P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280->P7) << 4);

	const float press = (float)p / 256.0f;
	
	return press;
}


//大气压值-Pa
float  BMP280_Get_Pressure(void)
{
	float press = 0;
	uint8_t buf[6];
	
	bmp280_Read_Bytes(BMP280_PRESSURE_MSB_REG,buf,6);
//	HAL_Delay(10);
	
	press = _update_pressure((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));	
	
//	printf("P = %f\r\n",press);
//	HAL_Delay(10);
	return press;
}


uint32_t baro_test(void)
{
	//printf("P = %d\r\n",baro_press);
	return (uint32_t)BMP280_Get_Pressure();
}
