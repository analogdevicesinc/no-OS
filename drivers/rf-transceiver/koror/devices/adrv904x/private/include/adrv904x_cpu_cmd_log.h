/**
 * \file adrv904x_cpu_cmd_log.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_SET_LOG_FILTERS
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_LOG_H__
#define __ADRV904X_CPU_CMD_LOG_H__

#include "adi_adrv904x_cpu_log_types.h"
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"


/**
 * \brief SET_LOG_FILTERS command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetLogFilters
{
    adi_adrv904x_CpuLogEvent_t       logEventFilter;
    adi_adrv904x_CpuLogCpuId_t       cpuIdFilter;
    adi_adrv904x_CpuLogObjIdFilter_t objIdFilter;
} adrv904x_CpuCmd_SetLogFilters_t;)

#endif /* __ADRV904X_CPU_CMD_LOG_H__ */

