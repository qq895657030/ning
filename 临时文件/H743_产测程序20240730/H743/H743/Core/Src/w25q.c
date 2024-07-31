#include<stdio.h>
#include <string.h>

#include "w25q.h"

uint16_t Flash_ID=0;

#define  FLASH_SPI_CS_LOW()   HAL_GPIO_WritePin(flash_cs_GPIO_Port, flash_cs_Pin, GPIO_PIN_RESET)
#define  FLASH_SPI_CS_HIGH()  HAL_GPIO_WritePin(flash_cs_GPIO_Port, flash_cs_Pin, GPIO_PIN_SET)

#define flash_spi hspi3

#define flash_SpiNum 2
 /**********************************************************************************
  * 函数功能: 模块初始化
  */
uint8_t BSP_W25Qx_Init(void)
{ 	
//	BSP_W25Qx_Reset();	
	BSP_W25Qx_Read_ID(&Flash_ID);
//	if(Flash_ID == 0xef17)
//		usb_printf("flash ID:0x%x\n",Flash_ID);
//	else
//		usb_printf("flash is not exist\n");	
	return BSP_W25Qx_GetStatus();
}
 
 
static void	BSP_W25Qx_Reset(void)
{
	uint8_t cmd[2] = {RESET_ENABLE_CMD,RESET_MEMORY_CMD};
	
	FLASH_SPI_CS_LOW();
	/* Send the reset command */
	
#if HARD_SPI==1	
	HAL_SPI_Transmit(&flash_spi, cmd, 2, W25Qx_TIMEOUT_VALUE);	
#else
	user_SPI_Transmit(flash_SpiNum, cmd, 2);
#endif
	FLASH_SPI_CS_HIGH();
}
 
 /**********************************************************************************
  * 函数功能: 获取设备状态
  */
static uint8_t BSP_W25Qx_GetStatus(void)
{
	uint8_t cmd[] = {READ_STATUS_REG1_CMD};
	uint8_t status;
	
	FLASH_SPI_CS_LOW();
	/* Send the read status command */
	
#if HARD_SPI==1	
	HAL_SPI_Transmit(&flash_spi, cmd, 1, W25Qx_TIMEOUT_VALUE);	
	/* Reception of the data */
	HAL_SPI_Receive(&flash_spi,&status, 1, W25Qx_TIMEOUT_VALUE);
#else
	user_SPI_Transmit(flash_SpiNum, cmd, 1);
	user_SPI_Receive(flash_SpiNum, &status, 1);
#endif
	
	FLASH_SPI_CS_HIGH();
	
	/* Check the value of the register */
  if((status & W25Q128FV_FSR_BUSY) != 0)
  {
    return W25Qx_BUSY;
  }
	else
	{
		return W25Qx_OK;
	}		
}
 
 /**********************************************************************************
  * 函数功能: 写使能
  */
uint8_t BSP_W25Qx_WriteEnable(void)
{
	uint8_t cmd[] = {WRITE_ENABLE_CMD};
	uint32_t tickstart = HAL_GetTick();
 
	/*Select the FLASH: Chip Select low */
	FLASH_SPI_CS_LOW();
	/* Send the read ID command */
#if HARD_SPI==1		
	HAL_SPI_Transmit(&flash_spi, cmd, 1, W25Qx_TIMEOUT_VALUE);	
#else
	user_SPI_Transmit(flash_SpiNum, cmd, 1);
#endif
	/*Deselect the FLASH: Chip Select high */
	FLASH_SPI_CS_HIGH();
	
	/* Wait the end of Flash writing */
	while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
	{
		/* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE)
    {        
			return W25Qx_TIMEOUT;
    }
	}
	
	return W25Qx_OK;
}
 
 /**********************************************************************************
  * 函数功能: 获取设备ID
  */
void BSP_W25Qx_Read_ID(uint16_t *ID)
{
	uint8_t cmd[4] = {READ_ID_CMD,0x00,0x00,0x00};
	uint8_t temp[2];
	FLASH_SPI_CS_LOW();
	/* Send the read ID command */
#if HARD_SPI==1	
	HAL_SPI_Transmit(&flash_spi, cmd, 4, W25Qx_TIMEOUT_VALUE);	
	/* Reception of the data */
	HAL_SPI_Receive(&flash_spi,temp, 2, W25Qx_TIMEOUT_VALUE);
#else	
	user_SPI_Transmit(flash_SpiNum, cmd, 4);
	user_SPI_Receive(flash_SpiNum, temp, 2);
#endif
	*ID = (temp[0]<<8)|temp[1];
	FLASH_SPI_CS_HIGH();
		
}
 
 /**********************************************************************************
  * 函数功能: 读数据
  * 输入参数: 缓存数组指针、读地址、字节数
  */
uint8_t BSP_W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
	uint8_t cmd[4];
 
	/* Configure the command */
	cmd[0] = READ_CMD;
	cmd[1] = (uint8_t)(ReadAddr >> 16);
	cmd[2] = (uint8_t)(ReadAddr >> 8);
	cmd[3] = (uint8_t)(ReadAddr);
	
	FLASH_SPI_CS_LOW();
	/* Send the read ID command */
	
#if HARD_SPI==1	
	HAL_SPI_Transmit(&flash_spi, cmd, 4, W25Qx_TIMEOUT_VALUE);	
	/* Reception of the data */
	if (HAL_SPI_Receive(&flash_spi, pData,Size,W25Qx_TIMEOUT_VALUE) != HAL_OK)
  {
    return W25Qx_ERROR;
  }
#else
	user_SPI_Transmit(flash_SpiNum, cmd, 4);
	user_SPI_Receive(flash_SpiNum, pData, Size);
#endif

	FLASH_SPI_CS_HIGH();
	return W25Qx_OK;
}
 
 /**********************************************************************************
  * 函数功能: 写数据
  * 输入参数: 缓存数组指针、写地址、字节数
  */
uint8_t BSP_W25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
	uint8_t cmd[4];
	uint32_t end_addr, current_size, current_addr;
	uint32_t tickstart = HAL_GetTick();
	
	/* Calculation of the size between the write address and the end of the page */
  current_addr = 0;
 
  while (current_addr <= WriteAddr)
  {
    current_addr += W25Q128FV_PAGE_SIZE;
  }
  current_size = current_addr - WriteAddr;
 
  /* Check if the size of the data is less than the remaining place in the page */
  if (current_size > Size)
  {
    current_size = Size;
  }
 
  /* Initialize the adress variables */
  current_addr = WriteAddr;
  end_addr = WriteAddr + Size;
	
  /* Perform the write page by page */
  do
  {
		/* Configure the command */
		cmd[0] = PAGE_PROG_CMD;
		cmd[1] = (uint8_t)(current_addr >> 16);
		cmd[2] = (uint8_t)(current_addr >> 8);
		cmd[3] = (uint8_t)(current_addr);
 
		/* Enable write operations */
		BSP_W25Qx_WriteEnable();
	
		FLASH_SPI_CS_LOW();
    /* Send the command */
#if HARD_SPI==1	
    if (HAL_SPI_Transmit(&flash_spi,cmd, 4, W25Qx_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25Qx_ERROR;
    }
    
    /* Transmission of the data */
    if (HAL_SPI_Transmit(&flash_spi, pData,current_size, W25Qx_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25Qx_ERROR;
    }
#else
	user_SPI_Transmit(flash_SpiNum, cmd, 4);
	user_SPI_Transmit(flash_SpiNum, pData, current_size);
#endif
			FLASH_SPI_CS_HIGH();
    	/* Wait the end of Flash writing */
		while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
		{
			/* Check for the Timeout */
			if((HAL_GetTick() - tickstart) > W25Qx_TIMEOUT_VALUE)
			{        
				return W25Qx_TIMEOUT;
			}
		}
    
    /* Update the address and size variables for next page programming */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + W25Q128FV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q128FV_PAGE_SIZE;
  } while (current_addr < end_addr);
 
	
	return W25Qx_OK;
}
 
 /**********************************************************************************
  * 函数功能: 扇区擦除
  * 输入参数: 地址
  */
uint8_t BSP_W25Qx_Erase_Sector(uint32_t Address)
{
	uint8_t cmd[4];
	uint32_t tickstart = HAL_GetTick();
	cmd[0] = SECTOR_ERASE_CMD;
	cmd[1] = (uint8_t)(Address >> 16);
	cmd[2] = (uint8_t)(Address >> 8);
	cmd[3] = (uint8_t)(Address);
	
	/* Enable write operations */
	BSP_W25Qx_WriteEnable();
	
	/*Select the FLASH: Chip Select low */
	FLASH_SPI_CS_LOW();
	/* Send the read ID command */
#if HARD_SPI==1	
	HAL_SPI_Transmit(&flash_spi, cmd, 4, W25Qx_TIMEOUT_VALUE);	
#else
	user_SPI_Transmit(flash_SpiNum, cmd, 4);
#endif	
	/*Deselect the FLASH: Chip Select high */
	FLASH_SPI_CS_HIGH();
	
	/* Wait the end of Flash writing */
	while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
	{
		/* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25Q128FV_SECTOR_ERASE_MAX_TIME)
    {        
			return W25Qx_TIMEOUT;
    }
	}
	return W25Qx_OK;
}
 
 /**********************************************************************************
  * 函数功能: 芯片擦除
  */
uint8_t BSP_W25Qx_Erase_Chip(void)
{
	uint8_t cmd[4];
	uint32_t tickstart = HAL_GetTick();
	cmd[0] = CHIP_ERASE_CMD;
	
	/* Enable write operations */
	BSP_W25Qx_WriteEnable();
	
	/*Select the FLASH: Chip Select low */
	FLASH_SPI_CS_LOW();
	/* Send the read ID command */
#if HARD_SPI==1	
	HAL_SPI_Transmit(&flash_spi, cmd, 1, W25Qx_TIMEOUT_VALUE);	
#else
	user_SPI_Transmit(flash_SpiNum, cmd, 1);
#endif	
	/*Deselect the FLASH: Chip Select high */
	FLASH_SPI_CS_HIGH();
	
	/* Wait the end of Flash writing */
	while(BSP_W25Qx_GetStatus() != W25Qx_BUSY)
	{
		/* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25Q128FV_BULK_ERASE_MAX_TIME)
    {        
			return W25Qx_TIMEOUT;
    }
	}
	return W25Qx_OK;
}

uint8_t flashtest(uint8_t *w,uint8_t *r,uint8_t len)
{
//	static uint8_t writebuf[10]="woshishui",readbuf[10] = {0};

	if(BSP_W25Qx_Erase_Sector(W25Q128FV_FLASH_SIZE-4096)!= W25Qx_OK)
		return 0;
	
	BSP_W25Qx_Write(w, W25Q128FV_FLASH_SIZE-200, len);
	HAL_Delay(500);

	memset(r,'\0',len);
	BSP_W25Qx_Read(r, W25Q128FV_FLASH_SIZE-200,len);
//	printf("readbuf %s\r\n",r);
	HAL_Delay(10);
	
	if( 0 == memcmp(w,r,len) )
		return 1;
	else
		return 0;
}


