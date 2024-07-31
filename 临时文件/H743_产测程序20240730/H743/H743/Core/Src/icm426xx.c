#include "icm426xx.h"

#define  ICM1_SPI_CS_LOW()   HAL_GPIO_WritePin(imu1_cs_GPIO_Port, imu1_cs_Pin, GPIO_PIN_RESET)
#define  ICM1_SPI_CS_HIGH()  HAL_GPIO_WritePin(imu1_cs_GPIO_Port, imu1_cs_Pin, GPIO_PIN_SET)

#define  ICM2_SPI_CS_LOW()   HAL_GPIO_WritePin(imu2_cs_GPIO_Port, imu2_cs_Pin, GPIO_PIN_RESET)
#define  ICM2_SPI_CS_HIGH()  HAL_GPIO_WritePin(imu2_cs_GPIO_Port, imu2_cs_Pin, GPIO_PIN_SET)

#define imu_spi1 hspi1
#define imu_spi2 hspi4

#define ICM42688DelayMs(_nms)  HAL_Delay(_nms)

static float accSensitivity   = 0.244f;   //���ٶȵ���С�ֱ��� mg/LSB
static float gyroSensitivity  = 32.8f;    //�����ǵ���С�ֱ���

static uint8_t icm42688_read_reg(uint8_t imu_x,uint8_t reg)
{
	uint8_t regval = 0;

	if(imu_x >= max_imu_num)
		return 0;
	
	if(imu_x==0)
	{
		ICM1_SPI_CS_LOW();
		
		reg |= 0x80;
		HAL_SPI_Transmit(&imu_spi1, &reg, 1, 100);
		HAL_SPI_Receive(&imu_spi1, &regval, 1, 100);
		
		ICM1_SPI_CS_HIGH();
	}
	else
	{
		ICM2_SPI_CS_LOW();
		
		reg |= 0x80;
		HAL_SPI_Transmit(&imu_spi2, &reg, 1, 100);
		HAL_SPI_Receive(&imu_spi2, &regval, 1, 100);
		
		ICM2_SPI_CS_HIGH();
	}
	
	return regval;
}

static void icm42688_read_regs(uint8_t imu_x, uint8_t reg, uint8_t* buf, uint16_t len)
{   
	if(imu_x >= max_imu_num)
		return;

	if(imu_x==0)
	{
		ICM1_SPI_CS_LOW();
		
		reg |= 0x80;
		HAL_SPI_Transmit(&imu_spi1, &reg, 1, 100);
		HAL_SPI_Receive(&imu_spi1, buf, len, 100);
		
		ICM1_SPI_CS_HIGH();
	}
	else
	{
		ICM2_SPI_CS_LOW();
		
		reg |= 0x80;
		HAL_SPI_Transmit(&imu_spi2, &reg, 1, 100);
		HAL_SPI_Receive(&imu_spi2, buf, len, 100);
		
		ICM2_SPI_CS_HIGH();
	}
}

static uint8_t icm42688_write_reg(uint8_t imu_x, uint8_t reg, uint8_t value)
{
	uint8_t regval = 0;
	
	if(imu_x==0)
	{
		ICM1_SPI_CS_LOW();
		
		HAL_SPI_Transmit(&imu_spi1, &reg, 1, 100);	
		HAL_SPI_Transmit(&imu_spi1, &value, 1, 100);	
		
		ICM1_SPI_CS_HIGH();
	}
	else
	{
		ICM2_SPI_CS_LOW();
		
		HAL_SPI_Transmit(&imu_spi2, &reg, 1, 100);	
		HAL_SPI_Transmit(&imu_spi2, &value, 1, 100);	
		
		ICM2_SPI_CS_HIGH();
	}
	
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

uint8_t Imu_ID[max_imu_num] = {0};
void Icm42688Init(void)
{
    uint8_t reg_val = 0;
    /* ��ȡ who am i �Ĵ��� */
	
	for(uint8_t i=0;i<max_imu_num;i++)
	{
    Imu_ID[i] = icm42688_read_reg(i,ICM42688_WHO_AM_I);
	
//		if(Imu_ID[i] == 0x47){
//			printf("Icm42688 ID:0x%x\n",Imu_ID[i]);
//		}else{
//			printf("Icm42688 is not exist\n");
//		}
		
    icm42688_write_reg(i,ICM42688_REG_BANK_SEL, 0); //����bank 0����Ĵ���
    icm42688_write_reg(i,ICM42688_REG_BANK_SEL, 0x01); //��λ������
    ICM42688DelayMs(100);

    if(Imu_ID[i] == ICM42688_ID)
    {
        icm42688_write_reg(i,ICM42688_REG_BANK_SEL, 1); //����bank 1����Ĵ���
        icm42688_write_reg(i,ICM42688_INTF_CONFIG4, 0x02); //����Ϊ4��SPIͨ��

        icm42688_write_reg(i,ICM42688_REG_BANK_SEL, 0); //����bank 0����Ĵ���
        icm42688_write_reg(i,ICM42688_FIFO_CONFIG, 0x40); //Stream-to-FIFO Mode(page63)
				ICM42688DelayMs(10);

        reg_val = icm42688_read_reg(i,ICM42688_INT_SOURCE0);
        icm42688_write_reg(i,ICM42688_INT_SOURCE0, 0x00);
        icm42688_write_reg(i,ICM42688_FIFO_CONFIG2, 0x00); // watermark
        icm42688_write_reg(i,ICM42688_FIFO_CONFIG3, 0x02); // watermark
        icm42688_write_reg(i,ICM42688_INT_SOURCE0, reg_val);
        icm42688_write_reg(i,ICM42688_FIFO_CONFIG1, 0x63); // Enable the accel and gyro to the FIFO
				ICM42688DelayMs(10);
			
        icm42688_write_reg(i,ICM42688_REG_BANK_SEL, 0x00);
        icm42688_write_reg(i,ICM42688_INT_CONFIG, 0x36);

        icm42688_write_reg(i,ICM42688_REG_BANK_SEL, 0x00);
        reg_val = icm42688_read_reg(i,ICM42688_INT_SOURCE0);
        reg_val |= (1 << 2); //FIFO_THS_INT1_ENABLE
        icm42688_write_reg(i,ICM42688_INT_SOURCE0, reg_val);
				ICM42688DelayMs(10);
				
        Icm42688GetAres(AFS_8G);
        icm42688_write_reg(i,ICM42688_REG_BANK_SEL, 0x00);
        reg_val = icm42688_read_reg(i,ICM42688_ACCEL_CONFIG0);//page74
        reg_val |= (AFS_8G << 5);   //���� ��8g
        reg_val |= (AODR_50Hz);     //������� 50HZ
        icm42688_write_reg(i,ICM42688_ACCEL_CONFIG0, reg_val);
				ICM42688DelayMs(10);
				
        Icm42688GetGres(GFS_1000DPS);
        icm42688_write_reg(i,ICM42688_REG_BANK_SEL, 0x00);
        reg_val = icm42688_read_reg(i,ICM42688_GYRO_CONFIG0);//page73
        reg_val |= (GFS_1000DPS << 5);   //���� ��1000dps
        reg_val |= (GODR_50Hz);     //������� 50HZ
        icm42688_write_reg(i,ICM42688_GYRO_CONFIG0, reg_val);
				ICM42688DelayMs(10);
				
        icm42688_write_reg(i,ICM42688_REG_BANK_SEL, 0x00);
        reg_val = icm42688_read_reg(i,ICM42688_PWR_MGMT0); //��ȡPWR��MGMT0��ǰ�Ĵ�����ֵ(page72)
        reg_val &= ~(1 << 5);//ʹ���¶Ȳ���
        reg_val |= ((3) << 2);//����GYRO_MODE  0:�ر� 1:���� 2:Ԥ�� 3:������
        reg_val |= (3);//����ACCEL_MODE 0:�ر� 1:�ر� 2:�͹��� 3:������
        icm42688_write_reg(i,ICM42688_PWR_MGMT0, reg_val);
        ICM42688DelayMs(10); //������PWR��MGMT0�Ĵ����� 200us�ڲ������κζ�д�Ĵ����Ĳ���
    }
	}
}

imuData_t IumData[max_imu_num];

static void IcmGetRawData()
{
    uint8_t buffer[12] = {0};
		
	for(uint8_t i=0;i<max_imu_num;i++)
	{
		icm42688_read_regs(i,ICM42688_ACCEL_DATA_X1, buffer, 12);

		IumData[i].ax  = ((uint16_t)buffer[0] << 8)  | buffer[1];
		IumData[i].ay  = ((uint16_t)buffer[2] << 8)  | buffer[3];
		IumData[i].az  = ((uint16_t)buffer[4] << 8)  | buffer[5];
		IumData[i].gx = ((uint16_t)buffer[6] << 8)  | buffer[7];
		IumData[i].gy = ((uint16_t)buffer[8] << 8)  | buffer[9];
		IumData[i].gz = ((uint16_t)buffer[10] << 8) | buffer[11];


		IumData[i].ax = (int16_t)(IumData[i].ax * accSensitivity);
		IumData[i].ay = (int16_t)(IumData[i].ay * accSensitivity);
		IumData[i].az = (int16_t)(IumData[i].az * accSensitivity);
																	
		IumData[i].gx = (int16_t)(IumData[i].gx * gyroSensitivity);
		IumData[i].gy = (int16_t)(IumData[i].gy * gyroSensitivity);
		IumData[i].gz = (int16_t)(IumData[i].gz * gyroSensitivity);
	}
}


void Imu_Test(void)
{
	IcmGetRawData();
	
//	printf("imu1 :ax=%d ay=%d az=%d  gyro:gx=%d gy=%d gz=%d \n", IumData[0].ax, IumData[0].ay, IumData[0].az,IumData[0].gx,IumData[0].gy,IumData[0].gz);
//	HAL_Delay(10);
//	printf("imu2 :ax=%d ay=%d az=%d  gyro:gx=%d gy=%d gz=%d \n", IumData[1].ax, IumData[1].ay, IumData[1].az,IumData[1].gx,IumData[1].gy,IumData[1].gz);
}









