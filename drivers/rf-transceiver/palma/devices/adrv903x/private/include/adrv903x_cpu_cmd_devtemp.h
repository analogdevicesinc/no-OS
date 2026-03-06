/**
 * \file adrv903x_cpu_cmd_devtemp.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE,
 *                                 ADRV903X_CPU_CMD_ID_GET_ENABLED_TEMPSENSORS,
 *                                 ADRV903X_CPU_CMD_ID_SET_ENABLED_TEMPSENSORS
 *
 * \details Command definition for ADRV903X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE,
 *                                 ADRV903X_CPU_CMD_ID_GET_ENABLED_TEMPSENSORS,
 *                                 ADRV903X_CPU_CMD_ID_SET_ENABLED_TEMPSENSORS
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_DEVTEMP_H__
#define __ADRV903X_CPU_CMD_DEVTEMP_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_dev_temp_types.h"
#include "adrv903x_cpu_error_codes_types.h"

/**
 * \brief ADRV903X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetDevTemp
{
    uint16_t avgMask;  /*!< Bitmask of adi_adrv903x_DevTempSensorMask_e values indicating
                            which temperature sensor readings should be averaged. See
                            definition of adi_adrv903x_DevTempData_t. */
} adrv903x_CpuCmd_GetDevTemp_t;)

/**
 * \brief ADRV903X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetDevTempResp
{
    adrv903x_CpuErrorCode_e     status;     /*!< CPU error status code */
    adi_adrv903x_DevTempData_t  tempData;   /*!< Temperature data returned by CPU */
} adrv903x_CpuCmd_GetDevTempResp_t;)

/**
 * \brief ADRV903X_CPU_CMD_ID_GET_ENABLED_TEMPSENSORS command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetDevTempSnsEnResp
{
    adrv903x_CpuErrorCode_e status;     /*!< CPU error status code */
    uint16_t tempEnData;                /*!<  Bitmask of adi_adrv903x_DevTempSensorMask_e values indicating 
                                                Temperature sensor enable bitfield returned by CPU */
} adrv903x_CpuCmd_GetDevTempSnsEnResp_t;)

/**
 * \brief ADRV903X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetDevTempSnsEn
{
    uint16_t tempEnData;  /*!< Bitmask of adi_adrv903x_DevTempSensorMask_e values indicating
                            which temperature sensors to enable/disable. */
} adrv903x_CpuCmd_SetDevTempSnsEn_t;)

/**
 * \brief ADRV903X_CPU_CMD_ID_SET_ENABLED_TEMPSENSORS command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetDevTempSnsEnResp
{
    adrv903x_CpuErrorCode_e status;         /*!< CPU error status code */
    uint16_t tempEnData;                    /*!< Bitmask of adi_adrv903x_DevTempSensorMask_e values indicating
                                                  which temperature sensors are actually enabled/disabled. */
} adrv903x_CpuCmd_SetDevTempSnsEnResp_t;)

#endif /* __ADRV903X_CPU_CMD_DEVTEMP_H__ */

