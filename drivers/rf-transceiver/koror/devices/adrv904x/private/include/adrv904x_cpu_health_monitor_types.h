/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adrv904x_cpu_health_monitor_types.h
 * 
 * \brief   Contains ADRV904X Private CPU Health Monitor data types
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADRV904X_CPU_HEALTH_MONITOR_TYPES_H__
#define __ADRV904X_CPU_HEALTH_MONITOR_TYPES_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"

/**
 * \brief Health Monitor Private structure for a single CPU. No defined private data to include yet.
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_HealthMonitorPrivateCpuStatusSingle
{
    uint8_t dummyField;         /*!< Dummy field */
} adrv904x_HealthMonitorPrivateCpuStatusSingle_t;)

/**
 * \brief Health Monitor Private structure for all CPUs
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_HealthMonitorPrivateCpuStatus
{
    adrv904x_HealthMonitorPrivateCpuStatusSingle_t  cpu0;   /*!< Private CPU Health Monitor Status info for CPU0 */
    adrv904x_HealthMonitorPrivateCpuStatusSingle_t  cpu1;   /*!< Private CPU Health Monitor Status info for CPU1 */
} adrv904x_HealthMonitorPrivateCpuStatus_t;)

#endif /* __ADRV904X_CPU_HEALTH_MONITOR_TYPES_H__ */

