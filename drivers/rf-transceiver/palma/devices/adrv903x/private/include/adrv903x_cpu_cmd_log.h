/**
 * \file adrv903x_cpu_cmd_log.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_SET_LOG_FILTERS
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_LOG_H__
#define __ADRV903X_CPU_CMD_LOG_H__

#include "adi_adrv903x_cpu_log_types.h"
#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cpu_error_codes_types.h"


/**
 * \brief SET_LOG_FILTERS command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetLogFilters
{
    adi_adrv903x_CpuLogEvent_t       logEventFilter;
    adi_adrv903x_CpuLogCpuId_t       cpuIdFilter;
    adi_adrv903x_CpuLogObjIdFilter_t objIdFilter;
} adrv903x_CpuCmd_SetLogFilters_t;)

#endif /* __ADRV903X_CPU_CMD_LOG_H__ */

