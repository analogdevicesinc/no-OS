/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
/**
 * \file adi_adrv904x_cpu_sw_bkpt_types.h
 * 
 * \brief   Contains the definitions for Software Breakpoints
 *
 * \details Contains the definitions for Software Breakpoints
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADI_ADRV904X_CPU_SW_BKPT_TYPES_H__
#define __ADI_ADRV904X_CPU_SW_BKPT_TYPES_H__

#include <stdint.h>


/**
* \brief Software breakpoint entry for each object
*/
typedef struct adi_adrv904x_SwBreakPointEntry
{
    uint32_t                objId;          /*! Object ID of the module to which the break point belongs */
    uint32_t                chanMask;       /*! Bit mask of channels for which the break point to be enabled */
    uint32_t                bkptMask;       /*! Bit mask of the breakpoints to be enabled */    
} adi_adrv904x_SwBreakPointEntry_t;

#endif /* __ADI_ADRV904X_CPU_SW_BKPT_TYPES_H__ */


