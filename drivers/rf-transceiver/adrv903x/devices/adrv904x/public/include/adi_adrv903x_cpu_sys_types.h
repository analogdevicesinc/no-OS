/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adi_adrv903x_cpu_sys_types.h
 * 
 * \brief   Contains ADRV903X Calibration data types
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADRV903X_CPU_SYS_TYPES_H__
#define __ADRV903X_CPU_SYS_TYPES_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cpu_error_codes_types.h"


/**
 * \brief Cpu System Status Mask
 */
typedef enum adi_adrv903x_CpuSysStatusMask
{
    ADI_ADRV903X_CPU_SS_ID_1_MASK     = 0x01u   /*!< Cpu System Status ID Mask 1       */
 
} adi_adrv903x_CpuSysStatusMask_e;

/**
 * \brief Data structure to hold common cpu system status information
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_CpuSysStatus
{
    adi_adrv903x_CpuErrorCode_t errorCode;          /*!< Current error condition reported by the calibration */
    uint32_t                placeHolder;        /*!< example of status */
} adi_adrv903x_CpuSysStatus_t;)



#ifdef __ICCARM__
#pragma diag_default=Pm009
#endif /* __ICCARM__ */

#endif /* __ADRV903X_CPU_SYS_TYPES_H__ */

