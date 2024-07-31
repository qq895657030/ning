#include "mpu6000.h"
#include<stdlib.h>
#include<string.h>


uint8_t Imu_ID[imu_num];

#define true 1
#define false 0	
	
#define  IMU_SPI_CS_LOW()   HAL_GPIO_WritePin(imu1_cs_GPIO_Port, imu1_cs_Pin, GPIO_PIN_RESET)
#define  IMU_SPI_CS_HIGH()  HAL_GPIO_WritePin(imu1_cs_GPIO_Port, imu1_cs_Pin, GPIO_PIN_SET)

#define imu_spi hspi2

static float accSensitivity   = 16000 / 32768.0f;   //加速度的最小分辨率 mg/LSB
static float gyroSensitivity  = 2000.0f / 32768.0f;    //陀螺仪的最小分辨率

#define int16_val(v, idx) ((int16_t)(((uint8_t)v[2 * idx] << 8) | v[2 * idx + 1]))

uint8_t mpu6000_read_reg(uint8_t reg)
{
	uint8_t regval = 0;

	IMU_SPI_CS_LOW();
	HAL_Delay(1);
	reg |= 0x80;
	HAL_SPI_Transmit(&imu_spi, &reg, 1, 100);
	HAL_SPI_Receive(&imu_spi, &regval, 1, 100);
	HAL_Delay(1);
	IMU_SPI_CS_HIGH();

	return regval;
}

static void mpu6000_read_regs( uint8_t reg, uint8_t* buf, uint16_t len)
{   

	IMU_SPI_CS_LOW();
	HAL_Delay(1);	
	reg |= 0x80;
	HAL_SPI_Transmit(&imu_spi, &reg, 1, 100);
	HAL_SPI_Receive(&imu_spi, buf, len, 100);
	HAL_Delay(1);
	IMU_SPI_CS_HIGH();
}

static uint8_t mpu6000_write_reg( uint8_t reg, uint8_t value)
{
	uint8_t regval = 0;
	
	IMU_SPI_CS_LOW();	
	HAL_Delay(1);	
	//reg |= 0x80;
	HAL_SPI_Transmit(&imu_spi, &reg, 1, 100);	
	HAL_SPI_Transmit(&imu_spi, &value, 1, 100);	
	HAL_Delay(1);			
	IMU_SPI_CS_HIGH();	

	return regval;
}

static void mpu6000AccAndGyroInit()
{
    // Device Reset
    mpu6000_write_reg( MPU_RA_PWR_MGMT_1, BIT_H_RESET);
    HAL_Delay(150);

    mpu6000_write_reg( MPU_RA_SIGNAL_PATH_RESET, BIT_GYRO | BIT_ACC | BIT_TEMP);
    HAL_Delay(150);

    // Clock Source PPL with Z axis gyro reference
    mpu6000_write_reg( MPU_RA_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);//
    HAL_Delay(15);

    // Disable Primary I2C Interface
    mpu6000_write_reg( MPU_RA_USER_CTRL, BIT_I2C_IF_DIS);
    HAL_Delay(15);

    mpu6000_write_reg( MPU_RA_PWR_MGMT_2, 0x00);
    HAL_Delay(15);

    // Accel Sample Rate 1kHz
    // Gyroscope Output Rate =  1kHz when the DLPF is enabled
    mpu6000_write_reg( MPU_RA_SMPLRT_DIV, 7);
    HAL_Delay(15);

    // Gyro +/- 2000 DPS Full Scale
    mpu6000_write_reg( MPU_RA_GYRO_CONFIG, INV_FSR_2000DPS << 3);
    HAL_Delay(15);

    // Accel +/- 16 G Full Scale
    mpu6000_write_reg( MPU_RA_ACCEL_CONFIG, INV_FSR_16G << 3);
    HAL_Delay(15);

    // Accel and Gyro DLPF Setting
    mpu6000_write_reg( MPU_RA_CONFIG, 6);
    HAL_Delay(1);
}

static void mpu6000_readID()
{
	mpu6000_write_reg( MPU_RA_PWR_MGMT_1, BIT_H_RESET);
	HAL_Delay(200);
	
	Imu_ID[0] = mpu6000_read_reg(MPU_RA_WHO_AM_I);
}

uint8_t mpu6000_init(void)
{
	mpu6000_readID();
	if(MPU6000_WHO_AM_I_CONST == Imu_ID[0] )
	{
		mpu6000AccAndGyroInit();
		HAL_Delay(1);

		return true;
	}
	return false;
}

static imuRawData_t mpu6000_rawdata={0};

static uint8_t mpu6000_Read(void)
{
	uint8_t buf[14]={0};
	
	mpu6000_read_regs( MPU_RA_ACCEL_XOUT_H, buf, 6 + 2 + 6);
	
	mpu6000_rawdata.accRaw[0] = int16_val(buf,0) * accSensitivity;
	mpu6000_rawdata.accRaw[1] = int16_val(buf,1) * accSensitivity;
	mpu6000_rawdata.accRaw[2] = int16_val(buf,2) * accSensitivity;

	mpu6000_rawdata.tempRaw = int16_val(buf,3)/34 + 365;	
	
	mpu6000_rawdata.gyroRaw[0] = int16_val(buf,4) * gyroSensitivity;
	mpu6000_rawdata.gyroRaw[1] = int16_val(buf,5) * gyroSensitivity;
	mpu6000_rawdata.gyroRaw[2] = int16_val(buf,6) * gyroSensitivity;	
	
	return true;
}

imuData_t IumData[imu_num];

void Imu_Test(void)
{
	mpu6000_Read();
	
	IumData[0].ax = mpu6000_rawdata.accRaw[0];
	IumData[0].ay = mpu6000_rawdata.accRaw[1];	
	IumData[0].az = mpu6000_rawdata.accRaw[2];	
	
	IumData[0].gx = mpu6000_rawdata.gyroRaw[0];
	IumData[0].gy = mpu6000_rawdata.gyroRaw[1];
	IumData[0].gz = mpu6000_rawdata.gyroRaw[2];	
}










