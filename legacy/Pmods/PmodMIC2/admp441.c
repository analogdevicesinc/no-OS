/**************************************************************************//**
*   @file   ADMP441.c
*   @brief  Implementation of driver file for ADMP441.
*   @author ATofan (alexandru.tofan@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "xparameters.h"
#include "xil_io.h"
#include "admp441.h"

/**************************************************************************//**
* @brief  Converts data from big endian to little endian.
*
* @param  Data in big endian.
*
* @return Data in little endian.
******************************************************************************/
long bigToLittle(long bigData)
{
	long littleData = 0;

	littleData = ((bigData & 0xFF000000) >> 24) |
				 ((bigData & 0x00FF0000) >> 8)  |
				 ((bigData & 0x0000FF00) << 8)  |
			     ((bigData & 0x000000FF) << 24);

	return(littleData);
}
