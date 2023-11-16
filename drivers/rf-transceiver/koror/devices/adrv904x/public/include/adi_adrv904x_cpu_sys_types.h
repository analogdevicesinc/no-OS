/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_cpu_sys_types.h
 * 
 * \brief   Contains ADRV904X Calibration data types
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADRV904X_CPU_SYS_TYPES_H__
#define __ADRV904X_CPU_SYS_TYPES_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"


/**
 * \brief Cpu System Status Mask
 */
typedef enum adi_adrv904x_CpuSysStatusMask
{
    ADI_ADRV904X_CPU_SS_ID_1_MASK     = 0x01u   /*!< Cpu System Status ID Mask 1       */
 
} adi_adrv904x_CpuSysStatusMask_e;

/**
 * \brief Data structure to hold common cpu system status information
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_CpuSysStatus
{
    adi_adrv904x_CpuErrorCode_t errorCode;          /*!< Current error condition reported by the calibration */
    uint32_t                placeHolder;        /*!< example of status */
} adi_adrv904x_CpuSysStatus_t;)



#ifdef __ICCARM__
#pragma diag_default=Pm009
#endif /* __ICCARM__ */

#endif /* __ADRV904X_CPU_SYS_TYPES_H__ */

