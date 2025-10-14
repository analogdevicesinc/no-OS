/******************************************************************************
* Copyright (c) 2012 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file fsbl_debug.h
*
* This file contains the debug verbose information for FSBL print functionality
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 3.00a mb	01/09/12 Initial release
*
* </pre>
*
* @note
*
******************************************************************************/

#ifndef _FSBL_DEBUG_H
#define _FSBL_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif


#define DEBUG_GENERAL	0x00000001    /* general debug  messages */
#define DEBUG_INFO	0x00000002    /* More debug information */

#if defined (FSBL_DEBUG_INFO)
#define fsbl_dbg_current_types ((DEBUG_INFO) | (DEBUG_GENERAL))
#elif defined (FSBL_DEBUG)
#define fsbl_dbg_current_types (DEBUG_GENERAL)
#else
#define fsbl_dbg_current_types 0
#endif

#ifdef STDOUT_BASEADDRESS
#define fsbl_printf(type,...) \
		if (((type) & fsbl_dbg_current_types))  {xil_printf (__VA_ARGS__); }
#else
#define fsbl_printf(type, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _FSBL_DEBUG_H */
