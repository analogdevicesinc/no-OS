 /**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
 /**
 * \file adi_adrv904x_cpu_log_types.h
 * 
 * \brief   Contains CPU log types
 *
 * \details Contains CPU log types
 *
 * ADRV904X API Version: 2.10.0.4
 */
 
#ifndef __ADI_ADRV904X_CPU_LOG_TYPES_H__
#define __ADI_ADRV904X_CPU_LOG_TYPES_H__

#include <stdint.h>

/**
 * \brief CPU log events
 */
typedef enum adi_adrv904x_CpuLogEvent
{
    /* LOG_EVENT = 0 is reserved */
    ADI_ADRV904X_CPU_LOG_EVENT_ERROR      = 1, /*!< Error messages */
    ADI_ADRV904X_CPU_LOG_EVENT_WARNING    = 2, /*!< Warning messages */ 
    ADI_ADRV904X_CPU_LOG_EVENT_INFO       = 3, /*!< Informational messages, least verbose */
    ADI_ADRV904X_CPU_LOG_EVENT_INFO_2     = 4, /*!< Informational messages, most verbose */
    ADI_ADRV904X_CPU_LOG_EVENT_INVALID    = 5  /*!< Invalid setting (ignore) */
} adi_adrv904x_CpuLogEvent_e;
typedef uint8_t adi_adrv904x_CpuLogEvent_t;

/**
 * \brief CPU log CPU ID
 */
typedef enum adi_adrv904x_CpuLogCpuId
{
    ADI_ADRV904X_CPU_LOG_CPU_ID_NONE    = 0x0,  /*!< No CPUs */
    ADI_ADRV904X_CPU_LOG_CPU_ID_0       = 0x1,  /*!< CPU 0 (Primary CPU) */
    ADI_ADRV904X_CPU_LOG_CPU_ID_1       = 0x2,  /*!< CPU 1 */
    ADI_ADRV904X_CPU_LOG_CPU_ID_ALL     = 0x3,  /*!< All CPUs */
    ADI_ADRV904X_CPU_LOG_CPU_ID_INVALID = 0xFF  /*!< Invalid setting (ignore) */
} adi_adrv904x_CpuLogCpuId_e;
typedef uint8_t adi_adrv904x_CpuLogCpuId_t;

/**
 * \brief CPU log object ID
 */
typedef uint32_t adi_adrv904x_CpuLogObjId_t;

/**
 * \brief CPU log object ID filter enable
 */
typedef enum adi_adrv904x_CpuLogObjIdEnable
{
    ADI_ADRV904X_CPU_LOG_OBJ_ID_DISABLE = 0x0,  /*!< Disable */
    ADI_ADRV904X_CPU_LOG_OBJ_ID_ENABLE  = 0x1,  /*!< Enable */
    ADI_ADRV904X_CPU_LOG_OBJ_ID_INVALID = 0xFF  /*!< Invalid setting (ignore) */
} adi_adrv904x_CpuLogObjIdEnable_e;
typedef uint8_t adi_adrv904x_CpuLogObjIdEnable_t;

/**
 * \brief CPU log object ID filter settings
 */
typedef struct adi_adrv904x_CpuLogObjIdFilter
{
    adi_adrv904x_CpuLogObjId_t       objId;              /*!< Object ID */
    adi_adrv904x_CpuLogObjIdEnable_t objIdFilterEnable;  /*!< Filter enable/disable */
} adi_adrv904x_CpuLogObjIdFilter_t;

#endif /* __ADI_ADRV904X_CPU_LOG_TYPES_H__*/

