/******************************************************************************
* Copyright (c) 2017 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_errata.h
*
* @addtogroup a53_errata Cortex A53 64 bit Processor Errata Support
* @{
* Various ARM errata are handled in the standalone BSP. The implementation for
* errata handling follows ARM guidelines and is based on the open source Linux
* support for these errata.
*
* @note
* The errata handling is enabled by default. To disable handling of all the
* errata globally, un-define the macro ENABLE_ARM_ERRATA in xil_errata.h. To
* disable errata on a per-erratum basis, un-define relevant macros in
* xil_errata.h.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 6.4   mus  08/11/17 First release
* </pre>
*
******************************************************************************/
#ifndef XIL_ERRATA_H
#define XIL_ERRATA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name errata_definitions
 *
 * The errata conditions handled in the standalone BSP are listed below
 * @{
 */

#define ENABLE_ARM_ERRATA 1

#ifdef ENABLE_ARM_ERRATA

/**
 *  Errata No: 855873
 *  Description: An eviction might overtake a cache clean operation
 */
#define CONFIG_ARM_ERRATA_855873 1


/*@}*/
#endif  /* ENABLE_ARM_ERRATA */

#ifdef __cplusplus
}
#endif

#endif  /* XIL_ERRATA_H */
/**
* @} End of "addtogroup a53_errata".
*/
