/**
 * Disclaimer Legal Disclaimer
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_cpu_health_monitor_types.h
 * 
 * \brief   Contains ADRV904X CPU Health Monitor data types
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADI_ADRV904X_CPU_HEALTH_MONITOR_TYPES_H__
#define __ADI_ADRV904X_CPU_HEALTH_MONITOR_TYPES_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"

#ifdef __ICCARM__
/*
 * Error[Pm009]: Identifiers (internal and external) shall not rely on
 * significance of more than 31 characters (MISRA C 2004 rule 5.1)
 *
 * To satisfy API naming conventions it is necessary to have this long common
 * macro prefix.
 */
#pragma diag_suppress=Pm009
#endif /* __ICCARM__ */

/**
 * \brief CPU Usage enumeration and type
 */
typedef enum adi_adrv904x_CpuUsage
{
    ADI_ADRV904X_CPU_USAGE_NORMAL,       /*!< CPU usage is normal */
    ADI_ADRV904X_CPU_USAGE_STARVING,     /*!< Some of the cals may be starving */
    ADI_ADRV904X_CPU_USAGE_CRITICAL      /*!< Cals are not getting chance to run */
} adi_adrv904x_CpuUsage_e;
typedef uint8_t adi_adrv904x_CpuUsage_t;

/**
 * \brief Health Monitor structure for a single CPU
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_HealthMonitorCpuStatusSingle
{
    uint64_t                    initCalStatusMask;      /*!< Status bits for all init calibrations, where bits correspond to calibrations as defined in adi_adrv904x_InitCalibrations_e. A bit will be 0 if the corresponding calibration indicated no error. A bit set to 1 indicates the corresponding calibration generated an error. The bit is cleared when the corresponding calibration is re-run. */
    uint32_t                    numInitCalErrors;       /*!< Total number of Init Cal errors generated since powerup */
    uint64_t                    trackingCalStatusMask;  /*!< Status bits for all tracking calibrations, where bits correspond to calibrations as defined in adi_adrv904x_TrackingCalibrationMask_e. A bit will be 0 if the corresponding calibration indicated no error. A bit set to 1 indicates the corresponding calibration generated an error. The bit is cleared when the corresponding calibration is re-run. */
    uint32_t                    numTrackingCalErrors;   /*!< Total number of Tracking Cal errors generated since powerup */
    uint32_t                    numSystemErrors;        /*!< Total number of System Errors that have occured since bootup. */
    uint32_t                    numSystemWarnings;      /*!< Total number of System Warnings that have occured since bootup */
    adi_adrv904x_CpuUsage_t     cpuUsage;               /*!< CPU Usage level */
} adi_adrv904x_HealthMonitorCpuStatusSingle_t;)

/**
 * \brief Health Monitor structure for all CPUs
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_HealthMonitorCpuStatus
{
   adi_adrv904x_HealthMonitorCpuStatusSingle_t  cpu0;   /*!< CPU Health Monitor Status info for CPU0 */
   adi_adrv904x_HealthMonitorCpuStatusSingle_t  cpu1;   /*!< CPU Health Monitor Status info for CPU1 */
} adi_adrv904x_HealthMonitorCpuStatus_t;)

#ifdef __ICCARM__
/* Restore checking for MISRA 5.1*/
#pragma diag_default=Pm009
#endif

#endif /* __ADI_ADRV904X_CPU_HEALTH_MONITOR_TYPES_H__ */

