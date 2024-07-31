#include "spi.h"

GPIO_TypeDef * Spi_CS_Port[4]= {	SPI1_CS_PORT,	SPI2_CS_PORT,	SPI3_CS_PORT,	SPI4_CS_PORT,};
GPIO_TypeDef * Spi_SCLK_Port[4]={	SPI1_SCLK_PORT, SPI2_SCLK_PORT, SPI3_SCLK_PORT, SPI4_SCLK_PORT,};
GPIO_TypeDef * Spi_MISO_Port[4]={	SPI1_MISO_PORT, SPI2_MISO_PORT, SPI3_MISO_PORT, SPI4_MISO_PORT,};
GPIO_TypeDef * Spi_MOSI_Port[4]={	SPI1_MOSI_PORT, SPI2_MOSI_PORT, SPI3_MOSI_PORT, SPI4_MOSI_PORT};

uint16_t Spi_CS_Pin[4]=  {SPI1_CS_PIN,   SPI2_CS_PIN,   SPI3_CS_PIN,   SPI4_CS_PIN,};
uint16_t Spi_SCLK_Pin[4]={SPI1_SCLK_PIN, SPI2_SCLK_PIN, SPI3_SCLK_PIN, SPI4_SCLK_PIN,};
uint16_t Spi_MISO_Pin[4]={SPI1_MISO_PIN, SPI2_MISO_PIN, SPI3_MISO_PIN, SPI4_MISO_PIN,};
uint16_t Spi_MOSI_Pin[4]={SPI1_MOSI_PIN, SPI2_MOSI_PIN, SPI3_MOSI_PIN, SPI4_MOSI_PIN};

#define  SPI_CS_LOW(n)   HAL_GPIO_WritePin(Spi_CS_Port[n], Spi_CS_Pin[n], GPIO_PIN_RESET)
#define  SPI_CS_HIGH(n)  HAL_GPIO_WritePin(Spi_CS_Port[n], Spi_CS_Pin[n], GPIO_PIN_SET)

#define  SPI_SCLK_LOW(n)   HAL_GPIO_WritePin(Spi_SCLK_Port[n], Spi_SCLK_Pin[n], GPIO_PIN_RESET)
#define  SPI_SCLK_HIGH(n)  HAL_GPIO_WritePin(Spi_SCLK_Port[n], Spi_SCLK_Pin[n], GPIO_PIN_SET)

#define  SPI_MOSI_LOW(n)   HAL_GPIO_WritePin(Spi_MOSI_Port[n], Spi_MOSI_Pin[n], GPIO_PIN_RESET)
#define  SPI_MOSI_HIGH(n)  HAL_GPIO_WritePin(Spi_MOSI_Port[n], Spi_MOSI_Pin[n], GPIO_PIN_SET)

#define  SPI_MISO_HIGH(n)  HAL_GPIO_ReadPin(Spi_MISO_Port[n], Spi_MISO_Pin[n])

void MySPI_GPIO_Init(void)
{
	uint8_t i;
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	for(i=0;i<4;i++)
	{
		GPIO_InitStruct.Pin = Spi_CS_Pin[i];
		HAL_GPIO_Init(Spi_CS_Port[i], &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = Spi_SCLK_Pin[i];
		HAL_GPIO_Init(Spi_SCLK_Port[i], &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = Spi_MOSI_Pin[i];
		HAL_GPIO_Init(Spi_MOSI_Port[i], &GPIO_InitStruct);
	}
	
	for(i=0;i<4;i++)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pin = Spi_MISO_Pin[i];
		HAL_GPIO_Init(Spi_MISO_Port[i], &GPIO_InitStruct);
	}
}

uint8_t MySPI_SwapByte(uint8_t spiNum,uint8_t dat)
{
    uint8_t i,ByteReceive = 0x00;
	
    for(i=0;i<8;i++)
    {
		SPI_SCLK_HIGH(spiNum); //SCK置低电平
		
		if(dat&0x80)
        {
            SPI_MOSI_HIGH(spiNum);
        }
		else
        {
            SPI_MOSI_LOW(spiNum);
        }
        
		dat<<=1;
        SPI_SCLK_LOW(spiNum); //SCK置高电平
		
		ByteReceive|=SPI_MISO_HIGH(spiNum);
		ByteReceive<<=1; 
    }
	
    return ByteReceive;
}

void MySPI_TransmitByte(uint8_t spiNum,uint8_t dat)
{
    uint8_t i;
	
    for(i=0;i<8;i++)
    {
		if(dat&0x80)
        {
            SPI_MOSI_HIGH(spiNum);
        }
		else
        {
            SPI_MOSI_LOW(spiNum);
        }
		
        SPI_SCLK_HIGH(spiNum); //SCK置低电平
		dat<<=1;
		HAL_Delay(1);
		SPI_SCLK_LOW(spiNum); //SCK置高电平
    }
}

uint8_t MySPI_ReceiveByte(uint8_t spiNum)
{
    uint8_t i,ByteReceive = 0x00;
	
    for(i=0;i<8;i++)
    {
		SPI_SCLK_HIGH(spiNum); //SCK置低电平
		HAL_Delay(1);
		SPI_SCLK_LOW(spiNum); //SCK置高电平
		ByteReceive|=SPI_MISO_HIGH(spiNum);
		ByteReceive<<=1; 
    }
	
    return ByteReceive;
}

void user_SPI_Transmit(uint8_t spiNum, uint8_t *pData, uint16_t Size)
{
	uint16_t i;
	
	for(i=0;i<Size;i++)
	{
		MySPI_TransmitByte(spiNum,*pData++);
	}
}

void user_SPI_Receive(uint8_t spiNum, uint8_t *pData, uint16_t Size)
{
	uint16_t i;
	
	for(i=0;i<Size;i++)
	{
		*pData++=MySPI_ReceiveByte(spiNum);
	}
}