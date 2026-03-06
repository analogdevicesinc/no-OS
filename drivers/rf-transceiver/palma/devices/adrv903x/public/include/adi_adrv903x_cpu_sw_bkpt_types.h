/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**
 * \file adi_adrv903x_cpu_sw_bkpt_types.h
 * 
 * \brief   Contains the definitions for Software Breakpoints
 *
 * \details Contains the definitions for Software Breakpoints
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADI_ADRV903X_CPU_SW_BKPT_TYPES_H__
#define __ADI_ADRV903X_CPU_SW_BKPT_TYPES_H__


#include "adi_library_types.h"



/**
* \brief Software breakpoint entry for each object
*/
typedef struct adi_adrv903x_SwBreakPointEntry
{
    uint32_t                objId;          /*! Object ID of the module to which the break point belongs */
    uint32_t                chanMask;       /*! Bit mask of channels for which the break point to be enabled */
    uint32_t                bkptMask;       /*! Bit mask of the breakpoints to be enabled */    
} adi_adrv903x_SwBreakPointEntry_t;

#endif /* __ADI_ADRV903X_CPU_SW_BKPT_TYPES_H__ */


