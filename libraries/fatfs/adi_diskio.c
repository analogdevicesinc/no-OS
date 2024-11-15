/***************************************************************************//**
*   @file   diskio.c
*   @brief  Implementation of the Low level disk I/O module for FatFs.
*   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "sd.h"
#include "no_os_error.h"
#include <stdio.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define DEV_SD		0	/* Example: Map MMC/SD card to physical drive 0 */
#define DEV_RAM		1	/* Example: Map Ramdisk to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#define ERASE_SECTOR_SIZE	1u
uint8_t			sd_init_var = false;
extern struct sd_desc	*sd_desc;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

DSTATUS SD_disk_status();
DSTATUS SD_disk_initialize();
DRESULT SD_disk_read(BYTE *buff, LBA_t sector, UINT count);
DRESULT SD_disk_write(BYTE *buff, LBA_t sector, UINT count);

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv)	/* Physical drive nmuber to identify the drive */
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
	BYTE pdrv)	/* Physical drive nmuber to identify the drive */
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
	UINT count)		/* Number of sectors to read */
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
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count		/* Number of sectors to write */
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
	void *buff)		/* Buffer to send/receive control data */
{
	switch(pdrv) {
	case DEV_SD:
		switch (cmd){
		case CTRL_SYNC: return RES_OK;
		case GET_SECTOR_COUNT:
			*(LBA_t *)buff = sd_desc->memory_size / DATA_BLOCK_LEN;
			return RES_OK;
		case GET_SECTOR_SIZE:
			/* Sector size in FatFs is the name for
			 * data block size in the SD card specification */
			*(WORD *)buff = DATA_BLOCK_LEN;
			return RES_OK;
		case GET_BLOCK_SIZE:
			/* Block size in FatFs is the name for
			 * sector size in the SD card specification */
			*(DWORD *)buff = ERASE_SECTOR_SIZE;
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

DSTATUS SD_disk_status()
{
	if (sd_init_var)
		return 0;
	return STA_NOINIT;
}

DSTATUS SD_disk_initialize()
{

	if (sd_desc == 0)
		return STA_NOINIT;
	sd_init_var = true;

	return 0;
}

DRESULT SD_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
	if (!sd_init_var)
		return RES_NOTRDY;
	if (0 != sd_read(sd_desc, buff, (uint64_t)sector * 512, (uint64_t)count * 512))
		return RES_ERROR;

	return RES_OK;
}

DRESULT SD_disk_write(BYTE *buff, LBA_t sector, UINT count)
{
	if (!sd_init_var)
		return RES_NOTRDY;
	if (0 != sd_write(sd_desc, buff, (uint64_t)sector * 512, (uint64_t)count * 512))
		return RES_ERROR;

	return RES_OK;
}


