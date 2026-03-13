/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adrv903x_cpu_health_monitor_types.h
 * 
 * \brief   Contains ADRV903X Private CPU Health Monitor data types
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADRV903X_CPU_HEALTH_MONITOR_TYPES_H__
#define __ADRV903X_CPU_HEALTH_MONITOR_TYPES_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cpu_error_codes_types.h"

/**
 * \brief Health Monitor Private structure for a single CPU. No defined private data to include yet.
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_HealthMonitorPrivateCpuStatusSingle
{
    uint8_t dummyField;         /*!< Dummy field */
} adrv903x_HealthMonitorPrivateCpuStatusSingle_t;)

/**
 * \brief Health Monitor Private structure for all CPUs
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_HealthMonitorPrivateCpuStatus
{
    adrv903x_HealthMonitorPrivateCpuStatusSingle_t  cpu0;   /*!< Private CPU Health Monitor Status info for CPU0 */
    adrv903x_HealthMonitorPrivateCpuStatusSingle_t  cpu1;   /*!< Private CPU Health Monitor Status info for CPU1 */
} adrv903x_HealthMonitorPrivateCpuStatus_t;)

#endif /* __ADRV903X_CPU_HEALTH_MONITOR_TYPES_H__ */

