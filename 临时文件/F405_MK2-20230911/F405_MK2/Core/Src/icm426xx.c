#include "icm426xx.h"


#define  ICM_SPI_CS_LOW()   HAL_GPIO_WritePin(imu_cs_GPIO_Port, imu_cs_Pin, GPIO_PIN_RESET)
#define  ICM_SPI_CS_HIGH()  HAL_GPIO_WritePin(imu_cs_GPIO_Port, imu_cs_Pin, GPIO_PIN_SET)

#define ICM42688DelayMs(_nms)  HAL_Delay(_nms)

static float accSensitivity   = 0.244f;   //加速度的最小分辨率 mg/LSB
static float gyroSensitivity  = 32.8f;    //陀螺仪的最小分辨率

#define IMU_SPI hspi1

static uint8_t icm42688_read_reg(uint8_t reg)
{
	uint8_t regval = 0;

	ICM_SPI_CS_LOW();
	
	reg |= 0x80;
	HAL_SPI_Transmit(&IMU_SPI, &reg, 1, 100);
	HAL_SPI_Receive(&IMU_SPI, &regval, 1, 100);
	
	ICM_SPI_CS_HIGH();

	return regval;
}

static void icm42688_read_regs(uint8_t reg, uint8_t* buf, uint16_t len)
{   
	ICM_SPI_CS_LOW();
	
	reg |= 0x80;
	HAL_SPI_Transmit(&IMU_SPI, &reg, 1, 100);
	HAL_SPI_Receive(&IMU_SPI, buf, len, 100);
	
	ICM_SPI_CS_HIGH();
}

static uint8_t icm42688_write_reg(uint8_t reg, uint8_t value)
{
	uint8_t regval = 0;
	
	ICM_SPI_CS_LOW();	
	//reg |= 0x80;
	HAL_SPI_Transmit(&IMU_SPI, &reg, 1, 100);	
	HAL_SPI_Transmit(&IMU_SPI, &value, 1, 100);	
	ICM_SPI_CS_HIGH();	
	
	return regval;
}

float Icm42688GetGres(uint8_t Gscale)
{
    switch(Gscale)
    {
    case GFS_15_125DPS:
        gyroSensitivity = 15.125f / 32768.0f;
        break;
    case GFS_31_25DPS:
        gyroSensitivity = 31.25f / 32768.0f;
        break;
    case GFS_62_5DPS:
        gyroSensitivity = 62.5f / 32768.0f;
        break;
    case GFS_125DPS:
        gyroSensitivity = 125.0f / 32768.0f;
        break;
    case GFS_250DPS:
        gyroSensitivity = 250.0f / 32768.0f;
        break;
    case GFS_500DPS:
        gyroSensitivity = 500.0f / 32768.0f;
        break;
    case GFS_1000DPS:
        gyroSensitivity = 1000.0f / 32768.0f;
        break;
    case GFS_2000DPS:
        gyroSensitivity = 2000.0f / 32768.0f;
        break;
    }
    return gyroSensitivity;
}

float Icm42688GetAres(uint8_t Ascale)
{
    switch(Ascale)
    {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (11), 4 Gs (10), 8 Gs (01), and 16 Gs  (00).
    case AFS_2G:
        accSensitivity = 2000 / 32768.0f;
        break;
    case AFS_4G:
        accSensitivity = 4000 / 32768.0f;
        break;
    case AFS_8G:
        accSensitivity = 8000 / 32768.0f;
        break;
    case AFS_16G:
        accSensitivity = 16000 / 32768.0f;
        break;
    }

    return accSensitivity;
}

uint8_t Imu_ID = 0;
void Icm42688Init(void)
{
    uint8_t reg_val = 0;
    /* 读取 who am i 寄存器 */
    Imu_ID = icm42688_read_reg(ICM42688_WHO_AM_I);
	
//		if(Imu_ID == 0x47){
//			usb_printf("Icm42688 ID:0x%x\n",Imu_ID);
//		}else{
//			usb_printf("Icm42688 is not exist\n");
//		}
		
    icm42688_write_reg(ICM42688_REG_BANK_SEL, 0); //设置bank 0区域寄存器
    icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x01); //软复位传感器
    ICM42688DelayMs(100);


    if(Imu_ID == ICM42688_ID)
    {
        icm42688_write_reg(ICM42688_REG_BANK_SEL, 1); //设置bank 1区域寄存器
        icm42688_write_reg(ICM42688_INTF_CONFIG4, 0x02); //设置为4线SPI通信

        icm42688_write_reg(ICM42688_REG_BANK_SEL, 0); //设置bank 0区域寄存器
        icm42688_write_reg(ICM42688_FIFO_CONFIG, 0x40); //Stream-to-FIFO Mode(page63)
				ICM42688DelayMs(10);

        reg_val = icm42688_read_reg(ICM42688_INT_SOURCE0);
        icm42688_write_reg(ICM42688_INT_SOURCE0, 0x00);
        icm42688_write_reg(ICM42688_FIFO_CONFIG2, 0x00); // watermark
        icm42688_write_reg(ICM42688_FIFO_CONFIG3, 0x02); // watermark
        icm42688_write_reg(ICM42688_INT_SOURCE0, reg_val);
        icm42688_write_reg(ICM42688_FIFO_CONFIG1, 0x63); // Enable the accel and gyro to the FIFO
				ICM42688DelayMs(10);
			
        icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00);
        icm42688_write_reg(ICM42688_INT_CONFIG, 0x36);

        icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00);
        reg_val = icm42688_read_reg(ICM42688_INT_SOURCE0);
        reg_val |= (1 << 2); //FIFO_THS_INT1_ENABLE
        icm42688_write_reg(ICM42688_INT_SOURCE0, reg_val);
				ICM42688DelayMs(10);
				
        Icm42688GetAres(AFS_8G);
        icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00);
        reg_val = icm42688_read_reg(ICM42688_ACCEL_CONFIG0);//page74
        reg_val |= (AFS_8G << 5);   //量程 ±8g
        reg_val |= (AODR_50Hz);     //输出速率 50HZ
        icm42688_write_reg(ICM42688_ACCEL_CONFIG0, reg_val);
				ICM42688DelayMs(10);
				
        Icm42688GetGres(GFS_1000DPS);
        icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00);
        reg_val = icm42688_read_reg(ICM42688_GYRO_CONFIG0);//page73
        reg_val |= (GFS_1000DPS << 5);   //量程 ±1000dps
        reg_val |= (GODR_50Hz);     //输出速率 50HZ
        icm42688_write_reg(ICM42688_GYRO_CONFIG0, reg_val);
				ICM42688DelayMs(10);
				
        icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00);
        reg_val = icm42688_read_reg(ICM42688_PWR_MGMT0); //读取PWR—MGMT0当前寄存器的值(page72)
        reg_val &= ~(1 << 5);//使能温度测量
        reg_val |= ((3) << 2);//设置GYRO_MODE  0:关闭 1:待机 2:预留 3:低噪声
        reg_val |= (3);//设置ACCEL_MODE 0:关闭 1:关闭 2:低功耗 3:低噪声
        icm42688_write_reg(ICM42688_PWR_MGMT0, reg_val);
        ICM42688DelayMs(10); //操作完PWR—MGMT0寄存器后 200us内不能有任何读写寄存器的操作
    }
}


int8_t IcmGetRawData(imuData_t* RawData)
{
    uint8_t buffer[12] = {0};

    icm42688_read_regs(ICM42688_ACCEL_DATA_X1, buffer, 12);

    RawData->ax  = ((uint16_t)buffer[0] << 8)  | buffer[1];
    RawData->ay  = ((uint16_t)buffer[2] << 8)  | buffer[3];
    RawData->az  = ((uint16_t)buffer[4] << 8)  | buffer[5];
    RawData->gx = ((uint16_t)buffer[6] << 8)  | buffer[7];
    RawData->gy = ((uint16_t)buffer[8] << 8)  | buffer[9];
    RawData->gz = ((uint16_t)buffer[10] << 8) | buffer[11];


    RawData->ax = (int16_t)(RawData->ax * accSensitivity);
    RawData->ay = (int16_t)(RawData->ay * accSensitivity);
    RawData->az = (int16_t)(RawData->az * accSensitivity);

    RawData->gx = (int16_t)(RawData->gx * gyroSensitivity);
    RawData->gy = (int16_t)(RawData->gy * gyroSensitivity);
    RawData->gz = (int16_t)(RawData->gz * gyroSensitivity);

    return 0;
}

imuData_t IumRawData;

void Imu_Test(void)
{
	IcmGetRawData(&IumRawData);

//	printf("acc :ax=%d ay=%d az=%d  gyro:gx=%d gy=%d gz=%d \n", RawData.ax, RawData.ay, RawData.az,RawData.gx,RawData.gy,RawData.gz);
//	printf("\r\n");	
}


