/******************************************************************************
* Copyright (c) 2012 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file qspi.h
*
* This file contains the interface for the QSPI FLASH functionality
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm	01/10/10 Initial release
* 3.00a mb	01/09/12  Added the Delay Values defines for qspi
* 5.00a sgd	05/17/13 Added Flash Size > 128Mbit support
* 					 Dual Stack support
* 6.00a bsv	09/04/20 Added support for 2Gb flash parts
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___QSPI_H___
#define ___QSPI_H___

#include "fsbl.h"
#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "fsbl.h"

/************************** Constant Definitions *****************************/
#define SINGLE_FLASH_CONNECTION			0
#define DUAL_STACK_CONNECTION			1
#define DUAL_PARALLEL_CONNECTION		2
#define FLASH_SIZE_16MB					0x1000000

/*
 * Bank mask
 */
#define BANKMASK 0xF000000

/*
 * Identification of Flash
 * Micron:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is first byte of Device ID - 0xBB or 0xBA
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 * Spansion:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is Device ID - Memory Interface type - 0x20 or 0x02
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 */

#define MICRON_ID		0x20
#define SPANSION_ID		0x01
#define WINBOND_ID		0xEF
#define MACRONIX_ID		0xC2
#define ISSI_ID			0x9D

#define FLASH_SIZE_ID_8M		0x14
#define FLASH_SIZE_ID_16M		0x15
#define FLASH_SIZE_ID_32M		0x16
#define FLASH_SIZE_ID_64M		0x17
#define FLASH_SIZE_ID_128M		0x18
#define FLASH_SIZE_ID_256M		0x19
#define FLASH_SIZE_ID_512M		0x20
#define FLASH_SIZE_ID_1G		0x21
#define FLASH_SIZE_ID_2G		0x22
/* Macronix size constants are different for 512M and 1G */
#define MACRONIX_FLASH_SIZE_ID_512M		0x1A
#define MACRONIX_FLASH_SIZE_ID_1G		0x1B
#define MACRONIX_FLASH_SIZE_ID_2G		0x1C
#define MACRONIX_FLASH_1_8_V_MX66_ID_512        (0x3A)
/*
 * Size in bytes
 */
#define FLASH_SIZE_8M			0x0100000
#define FLASH_SIZE_16M			0x0200000
#define FLASH_SIZE_32M			0x0400000
#define FLASH_SIZE_64M			0x0800000
#define FLASH_SIZE_128M			0x1000000
#define FLASH_SIZE_256M			0x2000000
#define FLASH_SIZE_512M			0x4000000
#define FLASH_SIZE_1G			0x8000000
#define FLASH_SIZE_2G			0x10000000

/************************** Function Prototypes ******************************/
u32 InitQspi(void);

u32 QspiAccess( u32 SourceAddress,
		u32 DestinationAddress,
		u32 LengthBytes);

u32 FlashReadID(void);
u32 SendBankSelect(u8 BankSel);
/************************** Variable Definitions *****************************/


#ifdef __cplusplus
}
#endif


#endif /* ___QSPI_H___ */

