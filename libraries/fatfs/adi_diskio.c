/***************************************************************************//**
*   @file   diskio.c
*   @brief  Implementation of the Low level disk I/O module for FatFs.
*   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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


