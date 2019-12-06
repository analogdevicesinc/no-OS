/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "sd.h"
#include "spi.h"
#include "spi_extra.h"
#include "error.h"
#include <stdio.h>
#include "../system/adi_initialize.h"
#include <drivers/pwr/adi_pwr.h>

/* Definitions of physical drive number for each drive */
#define DEV_SD		0	/* Example: Map MMC/SD card to physical drive 0 */
#define DEV_RAM		1	/* Example: Map Ramdisk to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


uint8_t		sd_init_var = false;
struct spi_desc	*my_spi_desc;
struct sd_desc	*sd_desc;

DSTATUS SD_disk_status();
DSTATUS SD_disk_initialize();
DRESULT SD_disk_read(BYTE *buff, LBA_t sector, UINT count);
DRESULT SD_disk_write(BYTE *buff, LBA_t sector, UINT count);

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
	case DEV_SD :
		return SD_disk_status();;
	case DEV_RAM :
		return STA_NODISK;
	case DEV_USB :
		return STA_NODISK;
	default:
		return STA_NODISK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
	case DEV_SD :
		return SD_disk_initialize();
	case DEV_RAM :
		return STA_NODISK;
	case DEV_USB :
		return STA_NODISK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,		/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{

	switch (pdrv) {
	case DEV_SD :
		return SD_disk_read(buff, sector, count);
	case DEV_RAM :
		return RES_NOTRDY;
	case DEV_USB :
		return RES_NOTRDY;
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	switch (pdrv) {
	case DEV_SD:
		return SD_disk_write(buff, sector, count);
	case DEV_RAM :
		return RES_NOTRDY;
	case DEV_USB :
		return RES_NOTRDY;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch(pdrv) {
	case DEV_SD:
		switch (cmd){
		case CTRL_SYNC: return RES_OK;
		case GET_SECTOR_COUNT:
			*(LBA_t *)buff = sd_desc->memory_size / DATA_BLOCK_LEN;
			return RES_OK;
		case GET_SECTOR_SIZE:
			*(WORD *)buff = DATA_BLOCK_LEN;
			return RES_OK;
		case GET_BLOCK_SIZE:
			*(DWORD *)buff = 1u;
			return RES_OK;
		default: return RES_OK;
		}
		return RES_PARERR;
	case DEV_RAM:
		return RES_NOTRDY;
	case DEV_USB:
		return RES_NOTRDY;
	}
	return RES_PARERR;
}

static int32_t init_power()
{
	if (adi_pwr_Init() != ADI_PWR_SUCCESS)
		printf("\n Failed to initialize the power service \n");

	if (ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_HCLK, 1))
		printf("Failed to set clock divider for HCLK\n");

	if (ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_PCLK, 1))
		printf("Failed to set clock divider for PCLK\n");

	return 1;
}

static int32_t config_spi(struct spi_desc **spi_desc)
{
	struct spi_init_param		init_desc;
	struct adicup_spi_init_param	adicup_init_param;

	adicup_init_param.master_mode = MASTER;
	adicup_init_param.spi_channel = SPI0;
	adicup_init_param.continuous_mode = true;

	init_desc.chip_select = ADI_SPI_CS2;
	init_desc.max_speed_hz = 250000;
	init_desc.mode = SPI_MODE_0;
	init_desc.extra = &adicup_init_param;

	if (SUCCESS != spi_init(spi_desc, &init_desc)){
		printf("Fail to init spi\n");
		return 0;
	}

	return 1;
}


DSTATUS SD_disk_status(){
	if (sd_init_var)
		return 0;
	return STA_NOINIT;
}

DSTATUS SD_disk_initialize(){

	if (sd_init_var == true)
		return 0;
	adi_initComponents();
	if (!init_power())
		return STA_NOINIT;
	if (!config_spi(&my_spi_desc))
		return STA_NOINIT;
	struct sd_init_param init_param;
	init_param.spi_desc = my_spi_desc;
	if (SUCCESS != sd_init(&sd_desc, &init_param))
		return STA_NOINIT;
	sd_init_var = true;
	return 0;
}

DRESULT SD_disk_read(BYTE *buff, LBA_t sector, UINT count){
	if (!sd_init_var)
		return RES_NOTRDY;
	if (SUCCESS != sd_read(sd_desc, buff, (uint64_t)sector * 512, (uint64_t)count * 512))
		return RES_ERROR;
	return RES_OK;
}

DRESULT SD_disk_write(BYTE *buff, LBA_t sector, UINT count){
	if (!sd_init_var)
		return RES_NOTRDY;
	if (SUCCESS != sd_write(sd_desc, buff, (uint64_t)sector * 512, (uint64_t)count * 512))
		return RES_ERROR;
	return RES_OK;
}


