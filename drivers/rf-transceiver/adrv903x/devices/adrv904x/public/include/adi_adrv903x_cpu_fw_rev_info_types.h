 /**
 * \file adi_adrv903x_cpu_fw_rev_info_types.h
 * 
 * \brief   Contains device revision information.
 *
 * \details Contains device revision information.
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
 
#ifndef __ADI_ADRV903X_CPU_FW_REV_INFO_TYPES_H__
#define __ADI_ADRV903X_CPU_FW_REV_INFO_TYPES_H__

/**
* \brief Enumerated list of FW image build options.
*/
typedef enum adi_adrv903x_CpuFwBuildType
{
    ADI_ADRV903X_CPU_FW_BUILD_RELEASE = 0, /*!< FW release mode */
    ADI_ADRV903X_CPU_FW_BUILD_DEBUG   = 1, /*!< FW debug mode */
    ADI_ADRV903X_CPU_FW_TRBLSHOOT     = 8 /*!< FW troubleshoot mode */
} adi_adrv903x_CpuFwBuildType_e;

#endif /* __ADI_ADRV903X_CPU_FW_REV_INFO_TYPES_H__*/

