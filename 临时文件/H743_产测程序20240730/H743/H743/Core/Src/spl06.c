#include "spl06.h"
#include <math.h> 

uint8_t Baro_ID=0;
static int32_t spl06_pressure_raw = 0;
static int32_t spl06_temperature_raw = 0;

typedef struct {
    int16_t c0;
    int16_t c1;
    int32_t c00;
    int32_t c10;
    int16_t c01;
    int16_t c11;
    int16_t c20;
    int16_t c21;
    int16_t c30;
} spl06_coeffs_t;


spl06_coeffs_t spl06_cal;

uint8_t SPL06_Write_Byte(uint8_t regadd,uint8_t data) 				 
{ 
  uint8_t W_Data=0;

	HAL_Delay(10);
  W_Data = data;
  if(HAL_OK != HAL_I2C_Mem_Write(&hi2c1, SPL06_Write, regadd, I2C_MEMADD_SIZE_8BIT, &W_Data, 1, 0xfff))
		return 0;
	
  HAL_Delay(10);
  
  return 1;
}
uint8_t SPL06_Read_Byte(uint8_t regadd)
{
  uint8_t R_Data=0;
  
HAL_Delay(10); 	
  HAL_I2C_Mem_Read(&hi2c1, SPL06_Read, regadd, I2C_MEMADD_SIZE_8BIT, &R_Data, 1, 0xfff);
  HAL_Delay(10);
  
  return R_Data;		
}

uint8_t SPL06_Read_Buf(uint8_t regadd,uint8_t *buf,uint8_t len)
{  
	HAL_Delay(10);
  if( HAL_OK != HAL_I2C_Mem_Read(&hi2c1, SPL06_Read, regadd, I2C_MEMADD_SIZE_8BIT, buf, len, 0xfff))
		return 0;
	
  HAL_Delay(10);
  return 1;		
}

static uint8_t read_calibration_coefficients() {
    uint8_t sstatus;
    uint8_t caldata[SPL06_CALIB_COEFFS_LEN];	
		uint8_t count=0;

		do{
			sstatus = SPL06_Read_Byte(SPL06_MODE_AND_STATUS_REG);
			HAL_Delay(10); //NO
			//printf("hx 3 0x%x\r\n",sstatus);
			count++;
		}while((sstatus != 0xE0) && (sstatus != 0xC0) && (count<10));
	
    if (!(sstatus && (sstatus & SPL06_MEAS_CFG_COEFFS_RDY)))
        return 0;   // error reading status or coefficients not ready



    if (!SPL06_Read_Buf( SPL06_CALIB_COEFFS_START, (uint8_t *)&caldata, SPL06_CALIB_COEFFS_LEN)) {
        return 0;
    }

    spl06_cal.c0 = (caldata[0] & 0x80 ? 0xF000 : 0) | ((uint16_t)caldata[0] << 4) | (((uint16_t)caldata[1] & 0xF0) >> 4);
    spl06_cal.c1 = ((caldata[1] & 0x8 ? 0xF000 : 0) | ((uint16_t)caldata[1] & 0x0F) << 8) | (uint16_t)caldata[2];
    spl06_cal.c00 = (caldata[3] & 0x80 ? 0xFFF00000 : 0) | ((uint32_t)caldata[3] << 12) | ((uint32_t)caldata[4] << 4) | (((uint32_t)caldata[5] & 0xF0) >> 4);
    spl06_cal.c10 = (caldata[5] & 0x8 ? 0xFFF00000 : 0) | (((uint32_t)caldata[5] & 0x0F) << 16) | ((uint32_t)caldata[6] << 8) | (uint32_t)caldata[7];
    spl06_cal.c01 = ((uint16_t)caldata[8] << 8) | ((uint16_t)caldata[9]);
    spl06_cal.c11 = ((uint16_t)caldata[10] << 8) | (uint16_t)caldata[11];
    spl06_cal.c20 = ((uint16_t)caldata[12] << 8) | (uint16_t)caldata[13];
    spl06_cal.c21 = ((uint16_t)caldata[14] << 8) | (uint16_t)caldata[15];
    spl06_cal.c30 = ((uint16_t)caldata[16] << 8) | (uint16_t)caldata[17];

    return 1;
}

static int8_t spl06_samples_to_cfg_reg_value(uint8_t sample_rate)
{
    switch(sample_rate)
    {
        case 1: return 0;
        case 2: return 1;
        case 4: return 2;
        case 8: return 3;
        case 16: return 4;
        case 32: return 5;
        case 64: return 6;
        case 128: return 7;
        default: return -1; // invalid
    }
}

static uint8_t spl06_configure_measurements()
{
    uint8_t reg_value;

    reg_value = SPL06_TEMP_USE_EXT_SENSOR | spl06_samples_to_cfg_reg_value(SPL06_TEMPERATURE_OVERSAMPLING);
    if (!SPL06_Write_Byte( SPL06_TEMPERATURE_CFG_REG, reg_value)) {
        return 0;
    }

    reg_value = spl06_samples_to_cfg_reg_value(SPL06_PRESSURE_OVERSAMPLING);
    if (!SPL06_Write_Byte( SPL06_PRESSURE_CFG_REG, reg_value)) {
        return 0;
    }

    reg_value = 0;
    if (SPL06_TEMPERATURE_OVERSAMPLING > 8) {
        reg_value |= SPL06_TEMPERATURE_RESULT_BIT_SHIFT;
    }
		
    if (SPL06_PRESSURE_OVERSAMPLING > 8) {
        reg_value |= SPL06_PRESSURE_RESULT_BIT_SHIFT;
    }
		
    if (!SPL06_Write_Byte(SPL06_INT_AND_FIFO_CFG_REG, reg_value)) {
        return 0;
    }

    return 1;
}

static uint8_t spl06_start_pressure_measurement()
{
    return SPL06_Write_Byte(SPL06_MODE_AND_STATUS_REG, SPL06_MEAS_PRESSURE);
}

static uint8_t spl06_read_pressure()
{
    uint8_t data[SPL06_PRESSURE_LEN];
    int32_t spl06_pressure;

    uint8_t ack = SPL06_Read_Buf( SPL06_PRESSURE_START_REG, data, SPL06_PRESSURE_LEN);

    if (ack) {
        spl06_pressure = (int32_t)((data[0] & 0x80 ? 0xFF000000 : 0) | (((uint32_t)(data[0])) << 16) | (((uint32_t)(data[1])) << 8) | ((uint32_t)data[2]));
        spl06_pressure_raw = spl06_pressure;
    }

    return ack;
}

static uint8_t spl06_start_temperature_measurement()
{
    return SPL06_Write_Byte( SPL06_MODE_AND_STATUS_REG, SPL06_MEAS_TEMPERATURE);
}

static uint8_t spl06_read_temperature()
{
    uint8_t data[SPL06_TEMPERATURE_LEN];
    int32_t spl06_temperature;

    uint8_t ack = SPL06_Read_Buf( SPL06_TEMPERATURE_START_REG, data, SPL06_TEMPERATURE_LEN);

    if (ack) {
        spl06_temperature = (int32_t)((data[0] & 0x80 ? 0xFF000000 : 0) | (((uint32_t)(data[0])) << 16) | (((uint32_t)(data[1])) << 8) | ((uint32_t)data[2]));
        spl06_temperature_raw = spl06_temperature;
    }

    return ack;
}

static int32_t spl06_raw_value_scale_factor(uint8_t oversampling_rate)
{
    switch(oversampling_rate)
    {
        case 1: return 524288;
        case 2: return 1572864;
        case 4: return 3670016;
        case 8: return 7864320;
        case 16: return 253952;
        case 32: return 516096;
        case 64: return 1040384;
        case 128: return 2088960;
        default: return -1; // invalid
    }
}

static float spl06_compensate_pressure(int32_t pressure_raw, int32_t temperature_raw)
{
    const float p_raw_sc = (float)pressure_raw / spl06_raw_value_scale_factor(SPL06_PRESSURE_OVERSAMPLING);
    const float t_raw_sc = (float)temperature_raw / spl06_raw_value_scale_factor(SPL06_TEMPERATURE_OVERSAMPLING);

    const float pressure_cal = (float)spl06_cal.c00 + p_raw_sc * ((float)spl06_cal.c10 + p_raw_sc * ((float)spl06_cal.c20 + p_raw_sc * spl06_cal.c30));
    const float p_temp_comp = t_raw_sc * ((float)spl06_cal.c01 + p_raw_sc * ((float)spl06_cal.c11 + p_raw_sc * spl06_cal.c21));

    return pressure_cal + p_temp_comp;
}

// Returns temperature in degrees centigrade
static float spl06_compensate_temperature(int32_t temperature_raw)
{
    const float t_raw_sc = (float)temperature_raw / spl06_raw_value_scale_factor(SPL06_TEMPERATURE_OVERSAMPLING);
    const float temp_comp = (float)spl06_cal.c0 / 2 + t_raw_sc * spl06_cal.c1;
    return temp_comp;
}

uint8_t spl06_calculate( int32_t * pressure, int32_t * temperature )
{

    if (pressure) {
        *pressure = lrintf(spl06_compensate_pressure(spl06_pressure_raw, spl06_temperature_raw));
    }

    if (temperature) {
        *temperature = lrintf(spl06_compensate_temperature(spl06_temperature_raw) * 100);
    }

    return 1;
}

uint8_t spl06_init(void)
{
	Baro_ID = SPL06_Read_Byte(SPL06_CHIP_ID_REG);//Read the SPL06's ID
  HAL_Delay(10);
	
	if( !read_calibration_coefficients())
		return 0;
	
	if( !spl06_configure_measurements())
		return 0;

	return 1;
}

int32_t baro_test(void)
{
	int32_t pressure, temperature;
	
//	if(spl06_start_temperature_measurement())
//		spl06_read_temperature();
	
	if(spl06_start_pressure_measurement())
		spl06_read_pressure();
	
	spl06_calculate( &pressure, &temperature);
	
	return pressure;
}

