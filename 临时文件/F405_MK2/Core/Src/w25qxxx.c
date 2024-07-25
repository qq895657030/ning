/**
  **************************************************************************
  * @file     spi_flash.c
  * @brief    spi_flash source code
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "w25qxxx.h"
#include <string.h>


#define  FLASH_CS_LOW()   HAL_GPIO_WritePin(flash_cs_GPIO_Port, flash_cs_Pin, GPIO_PIN_RESET)
#define  FLASH_CS_HIGH()  HAL_GPIO_WritePin(flash_cs_GPIO_Port, flash_cs_Pin, GPIO_PIN_SET)

#define FLASH_SPI &hspi3

uint8_t buffer_tx[BUF_SIZE];
uint8_t buffer_rx[BUF_SIZE];

/** @addtogroup 435_SPI_w25q_flash
  * @{
  */

uint8_t spiflash_sector_buf[SPIF_SECTOR_SIZE];


/**
  * @brief  write data to flash
  * @param  pbuffer: the pointer for data buffer
  * @param  write_addr: the address where the data is written
  * @param  length: buffer length
  * @retval none
  */
void spiflash_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
  uint32_t sector_pos;
  uint16_t sector_offset;
  uint16_t sector_remain;
  uint16_t index;
  uint8_t *spiflash_buf;
  spiflash_buf = spiflash_sector_buf;

  /* sector address */
  sector_pos = write_addr / SPIF_SECTOR_SIZE;

  /* address offset in a sector */
  sector_offset = write_addr % SPIF_SECTOR_SIZE;

  /* the remain in a sector */
  sector_remain = SPIF_SECTOR_SIZE - sector_offset;
  if(length <= sector_remain)
  {
    /* smaller than a sector size */
    sector_remain = length;
  }
  while(1)
  {
    /* read a sector */
    spiflash_read(spiflash_buf, sector_pos * SPIF_SECTOR_SIZE, SPIF_SECTOR_SIZE);

    /* validate the read erea */
    for(index = 0; index < sector_remain; index++)
    {
      if(spiflash_buf[sector_offset + index] != 0xFF)
      {
        /* there are some data not equal 0xff, so this secotr needs erased */
        break;
      }
    }
    if(index < sector_remain)
    {
      /* erase the sector */
      spiflash_sector_erase(sector_pos);

      /* copy the write data */
      for(index = 0; index < sector_remain; index++)
      {
        spiflash_buf[index + sector_offset] = pbuffer[index];
      }
      spiflash_write_nocheck(spiflash_buf, sector_pos * SPIF_SECTOR_SIZE, SPIF_SECTOR_SIZE); /* program the sector */
    }
    else
    {
      /* write directly in the erased area */
      spiflash_write_nocheck(pbuffer, write_addr, sector_remain);
    }
    if(length == sector_remain)
    {
      /* write end */
      break;
    }
    else
    {
      /* go on writing */
      sector_pos++;
      sector_offset = 0;

      pbuffer += sector_remain;
      write_addr += sector_remain;
      length -= sector_remain;
      if(length > SPIF_SECTOR_SIZE)
      {
        /* could not write the remain data in the next sector */
        sector_remain = SPIF_SECTOR_SIZE;
      }
      else
      {
        /* could write the remain data in the next sector */
        sector_remain = length;
      }
    }
  }
}

/**
  * @brief  read data from flash
  * @param  pbuffer: the pointer for data buffer
  * @param  read_addr: the address where the data is read
  * @param  length: buffer length
  * @retval none
  */

void spiflash_read(uint8_t *pbuffer, uint32_t read_addr, uint32_t length)
{
	uint8_t temp = SPIF_READDATA;
	
  FLASH_CS_LOW();
  HAL_SPI_Transmit(FLASH_SPI, &temp, 1, 100); /* send instruction */
  HAL_SPI_Transmit(FLASH_SPI, (uint8_t *)((read_addr) >> 16), 1, 100); /* send 24-bit address */
  HAL_SPI_Transmit(FLASH_SPI, (uint8_t *)((read_addr) >> 8), 1, 100);
  HAL_SPI_Transmit(FLASH_SPI, (uint8_t *)read_addr, 1, 100);
  HAL_SPI_Receive(FLASH_SPI, pbuffer, length,100);
  FLASH_CS_HIGH();
}

/**
  * @brief  erase a sector data
  * @param  erase_addr: sector address to erase
  * @retval none
  */
void spiflash_sector_erase(uint32_t erase_addr)
{
	uint8_t temp = SPIF_SECTORERASE;
  //erase_addr *= SPIF_SECTOR_SIZE; /* translate sector address to byte address */
  spiflash_write_enable();
  spiflash_wait_busy();
  FLASH_CS_LOW();
  HAL_SPI_Transmit(FLASH_SPI, &temp, 1, 100);
  HAL_SPI_Transmit(FLASH_SPI, (uint8_t *)((erase_addr) >> 16), 1, 100);
  HAL_SPI_Transmit(FLASH_SPI, (uint8_t *)((erase_addr) >> 8), 1, 100);
  HAL_SPI_Transmit(FLASH_SPI, (uint8_t *)erase_addr, 1, 100);
  FLASH_CS_HIGH();
  spiflash_wait_busy();
}

/**
  * @brief  write data without check
  * @param  pbuffer: the pointer for data buffer
  * @param  write_addr: the address where the data is written
  * @param  length: buffer length
  * @retval none
  */
void spiflash_write_nocheck(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
  uint16_t page_remain;

  /* remain bytes in a page */
  page_remain = SPIF_PAGE_SIZE - write_addr % SPIF_PAGE_SIZE;
  if(length <= page_remain)
  {
    /* smaller than a page size */
    page_remain = length;
  }
  while(1)
  {
    spiflash_page_write(pbuffer, write_addr, page_remain);
    if(length == page_remain)
    {
      /* all data are programmed */
      break;
    }
    else
    {
      /* length > page_remain */
      pbuffer += page_remain;
      write_addr += page_remain;

      /* the remain bytes to be prorammed */
      length -= page_remain;
      if(length > SPIF_PAGE_SIZE)
      {
        /* can be progrmmed a page at a time */
        page_remain = SPIF_PAGE_SIZE;
      }
      else
      {
        /* smaller than a page size */
        page_remain = length;
      }
    }
  }
}

/**
  * @brief  write a page data
  * @param  pbuffer: the pointer for data buffer
  * @param  write_addr: the address where the data is written
  * @param  length: buffer length
  * @retval none
  */
void spiflash_page_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
		uint8_t temp = SPIF_PAGEPROGRAM;
  if((0 < length) && (length <= SPIF_PAGE_SIZE))
  {
    /* set write enable */
    spiflash_write_enable();

    FLASH_CS_LOW();

    /* send instruction */
    HAL_SPI_Transmit(FLASH_SPI, &temp, 1, 100);

    /* send 24-bit address */
    HAL_SPI_Transmit(FLASH_SPI, (uint8_t *)((write_addr) >> 16), 1, 100);
    HAL_SPI_Transmit(FLASH_SPI, (uint8_t *)((write_addr) >> 8), 1, 100);
    HAL_SPI_Transmit(FLASH_SPI, (uint8_t *)write_addr, 1, 100);
    HAL_SPI_Transmit(FLASH_SPI, pbuffer,length, 100);

    FLASH_CS_HIGH();

    /* wait for program end */
    spiflash_wait_busy();
  }
}



/**
  * @brief  wait program done
  * @param  none
  * @retval none
  */
void spiflash_wait_busy(void)
{
  while((spiflash_read_sr1() & 0x01) == 0x01);
}

/**
  * @brief  read sr1 register
  * @param  none
  * @retval none
  */
uint8_t spiflash_read_sr1(void)
{
  uint8_t breadbyte = 0,temp =SPIF_READSTATUSREG1;
  FLASH_CS_LOW();
  HAL_SPI_Transmit(FLASH_SPI, &temp, 1, 100);
	HAL_SPI_Receive(FLASH_SPI, &breadbyte, 1,100);
  FLASH_CS_HIGH();
  return (breadbyte);
}

/**
  * @brief  enable write operation
  * @param  none
  * @retval none
  */
void spiflash_write_enable(void)
{
	uint8_t temp =SPIF_WRITEENABLE;
  FLASH_CS_LOW();
  HAL_SPI_Transmit(FLASH_SPI, &temp, 1, 100);
  FLASH_CS_HIGH();
}

/**
  * @brief  read device id
  * @param  none
  * @retval device id
  */
uint16_t w25qxx_read_id(void)
{
	uint8_t temp[2],cmd[4] = {SPIF_MANUFACTDEVICEID,0x00,0x00,0x00};	
  uint16_t wreceivedata = 0;
	
  FLASH_CS_LOW();
	/* Send the read ID command */
	HAL_SPI_Transmit(FLASH_SPI, cmd, 4, 100);	
	/* Reception of the data */
	HAL_SPI_Receive(FLASH_SPI,temp, 2, 100);
	wreceivedata = (temp[0]<<8)|temp[1];
  FLASH_CS_HIGH();
  return wreceivedata;
}

uint8_t Flash_ID = 0;

void spiflash_test(void)
{
	uint16_t flash_id_index = 0;
	
  flash_id_index = w25qxx_read_id();

  spiflash_sector_erase(FLASH_TEST_ADDR); // erase sector 
  spiflash_write(buffer_tx, FLASH_TEST_ADDR, BUF_SIZE);// write data 
  spiflash_read(buffer_rx, FLASH_TEST_ADDR, BUF_SIZE);// read data 
	
	if( (0 == memcmp(buffer_tx,buffer_rx,BUF_SIZE)) && (flash_id_index == W25Q128))
		Flash_ID = 0xEE;
	else
		Flash_ID = 0x0;	
}


