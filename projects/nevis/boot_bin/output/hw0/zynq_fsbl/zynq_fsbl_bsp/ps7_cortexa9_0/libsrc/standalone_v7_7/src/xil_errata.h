/******************************************************************************
* Copyright (c) 2013 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_errata.h
*
* @addtogroup a9_errata Cortex A9 Processor and pl310 Errata Support
* @{
* Various ARM errata are handled in the standalone BSP. The implementation for
* errata handling follows ARM guidelines and is based on the open source Linux
* support for these errata.
*
* @note The errata handling is enabled by default. To disable handling of all the
* errata globally, un-define the macro ENABLE_ARM_ERRATA in xil_errata.h. To
* disable errata on a per-erratum basis, un-define relevant macros in
* xil_errata.h.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a srt  04/18/13 First release
* 6.6   mus  12/07/17 Removed errata 753970, It fixes CR#989132
* 7.7   asa  01/06/22 Removed errata 742230 and 743622. These are
*                     already fixed in Cortex-A9 r3p0, the revision
*                     that is being used in Zynq platforms.
* </pre>
*
******************************************************************************/
#ifndef XIL_ERRATA_H
#define XIL_ERRATA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@cond nocomments
 */
/**
 * @name errata_definitions
 *
 * The errata conditions handled in the standalone BSP are listed below
 * @{
 */

#define ENABLE_ARM_ERRATA 1

/**
 *@endcond
 */

#ifdef ENABLE_ARM_ERRATA

/**
 *  Errata No: 	 775420
 *  Description: A data cache maintenance operation which aborts,
 *		 		 might lead to deadlock
 */
#define CONFIG_ARM_ERRATA_775420 1

/**
 *  Errata No: 	 794073
 *  Description: Speculative instruction fetches with MMU disabled
 *               might not comply with architectural requirements
 */
#define CONFIG_ARM_ERRATA_794073 1


/** PL310 L2 Cache Errata */

/**
 *  Errata No: 	 588369
 *  Description: Clean & Invalidate maintenance operations do not
 *	   	 		 invalidate clean lines
 */
#define CONFIG_PL310_ERRATA_588369 1

/**
 *  Errata No: 	 727915
 *  Description: Background Clean and Invalidate by Way operation
 *		 can cause data corruption
 */
#define CONFIG_PL310_ERRATA_727915 1

/*@}*/
#endif  /* ENABLE_ARM_ERRATA */

#ifdef __cplusplus
}
#endif

#endif  /* XIL_ERRATA_H */
/**
* @} End of "addtogroup a9_errata".
*/
