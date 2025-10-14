/******************************************************************************
* Copyright (c) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_cache.c
*
* @addtogroup microblaze_cache_apis Microblaze Cache APIs
* @{
*
* This contains implementation of cache related driver functions.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  hbm  07/28/09 Initial release
* 3.10  asa  05/04/13 This version of MicroBlaze BSP adds support for system
*					  cache/L2 cache. Existing APIs in this file are modified
*					  to add support for L2 cache.
*					  These changes are done for implementing PR #697214.
* </pre>
*
*
******************************************************************************/

#include "xil_cache.h"


/****************************************************************************/
/**
*
* @brief    Disable the data cache.
*
* @return   None.
*
****************************************************************************/
void Xil_DCacheDisable(void)
{
	Xil_DCacheFlush();
	Xil_DCacheInvalidate();
	Xil_L1DCacheDisable();
}

/****************************************************************************/
/**
*
* @brief    Disable the instruction cache.
*
* @return   None.
*
*
****************************************************************************/
void Xil_ICacheDisable(void)
{
	Xil_ICacheInvalidate();
	Xil_L1ICacheDisable();
}
