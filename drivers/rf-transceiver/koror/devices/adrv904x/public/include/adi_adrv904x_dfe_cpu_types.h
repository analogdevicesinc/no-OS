/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv904x_dfe_cpu_types.h
*
* \brief Contains ADRV904X data types for on board dfe cpus feature
*
* ADRV904X API Version: 2.15.0.4
*/


#ifndef _ADI_ADRV904X_DFE_CPU_TYPES_H_
#define _ADI_ADRV904X_DFE_CPU_TYPES_H_



#include "adi_adrv904x_user.h"
#include "adi_library_types.h"
#include "adi_adrv904x_dfe_osal_t.h"
#include "adi_adrv904x_dfe_svc_watchdog_t.h"

/**
* \brief DFE CPU Binary Info Data Structure
*/
typedef struct adi_adrv904x_DfeCpuBinaryInfo
{
    uint8_t filePath[ADI_ADRV904X_DFE_MAX_FILE_LENGTH]; /*!< File Path for DFE CPU Core Binary Image */
} adi_adrv904x_DfeCpuBinaryInfo_t;

/**
 * \brief Enumerated list of DFE Firmware Debug Mode
 */
typedef enum adi_adrv904x_DfeFwDebugMode
{
    ADI_ADRV904X_DFE_FW_DEBUG_MODE_SWD,    /*!< SWD Mode */
    ADI_ADRV904X_DFE_FW_DEBUG_MODE_JTAG,   /*!< Jtag Mode */
    ADI_ADRV904X_DFE_FW_DEBUG_MODE_JTAG4,  /*!< Jtag and 4-bit trace */
    ADI_ADRV904X_DFE_FW_DEBUG_MODE_JTAG8,  /*!< Jtag and 8-bit trace */
} adi_adrv904x_DfeFwDebugMode_e;

#endif /* _ADI_ADRV904X_DFE_CPU_TYPES_H_ */
