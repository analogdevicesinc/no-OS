/******************************************************************************
* Copyright (c) 2012 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file sd.c
*
* Contains code for the SD card FLASH functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a jz	04/28/11 Initial release
* 7.00a kc  10/18/13 Integrated SD/MMC driver
* 12.00a ssc 12/11/14 Fix for CR# 839182
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xparameters.h"
#include "fsbl.h"

#if defined(XPAR_PS7_SD_0_S_AXI_BASEADDR) || defined(XPAR_XSDPS_0_BASEADDR)

#ifndef XPAR_PS7_SD_0_S_AXI_BASEADDR
#define XPAR_PS7_SD_0_S_AXI_BASEADDR XPAR_XSDPS_0_BASEADDR
#endif

#include "xstatus.h"

#include "ff.h"
#include "sd.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

extern u32 FlashReadBaseAddress;


static FIL fil;		/* File object */
static FATFS fatfs;
static char buffer[32];
static char *boot_file = buffer;

/******************************************************************************/
/******************************************************************************/
/**
*
* This function initializes the controller for the SD FLASH interface.
*
* @param	filename of the file that is to be used
*
* @return
*		- XST_SUCCESS if the controller initializes correctly
*		- XST_FAILURE if the controller fails to initializes correctly
*
* @note		None.
*
****************************************************************************/
u32 InitSD(const char *filename)
{

	FRESULT rc;
	TCHAR *path = "0:/"; /* Logical drive number is 0 */

	/* Register volume work area, initialize device */
	rc = f_mount(&fatfs, path, 0);
	fsbl_printf(DEBUG_INFO,"SD: rc= %.8x\n\r", rc);

	if (rc != FR_OK) {
		return XST_FAILURE;
	}

	strcpy_rom(buffer, filename);
	boot_file = (char *)buffer;
	FlashReadBaseAddress = XPAR_PS7_SD_0_S_AXI_BASEADDR;

	rc = f_open(&fil, boot_file, FA_READ);
	if (rc) {
		fsbl_printf(DEBUG_GENERAL,"SD: Unable to open file %s: %d\n", boot_file, rc);
		return XST_FAILURE;
	}

	return XST_SUCCESS;

}

/******************************************************************************/
/**
*
* This function provides the SD FLASH interface for the Simplified header
* functionality.
*
* @param	SourceAddress is address in FLASH data space
* @param	DestinationAddress is address in OCM data space
* @param	LengthBytes is the number of bytes to move
*
* @return
*		- XST_SUCCESS if the write completes correctly
*		- XST_FAILURE if the write fails to completes correctly
*
* @note		None.
*
****************************************************************************/
u32 SDAccess( u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes)
{

	FRESULT rc;	 /* Result code */
	UINT br;

	rc = f_lseek(&fil, SourceAddress);
	if (rc) {
		fsbl_printf(DEBUG_INFO,"SD: Unable to seek to %lx\n", SourceAddress);
		return XST_FAILURE;
	}

	rc = f_read(&fil, (void*)DestinationAddress, LengthBytes, &br);

	if (rc) {
		fsbl_printf(DEBUG_GENERAL,"*** ERROR: f_read returned %d\r\n", rc);
	}

	return XST_SUCCESS;

} /* End of SDAccess */


/******************************************************************************/
/**
*
* This function closes the file object
*
* @param	None
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void ReleaseSD(void) {

	f_close(&fil);
	return;


}
#endif


