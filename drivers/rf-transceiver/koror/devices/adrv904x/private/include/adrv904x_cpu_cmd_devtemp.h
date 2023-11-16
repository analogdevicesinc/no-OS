/**
 * \file adrv904x_cpu_cmd_devtemp.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE,
 *                                 ADRV904X_CPU_CMD_ID_GET_ENABLED_TEMPSENSORS,
 *                                 ADRV904X_CPU_CMD_ID_SET_ENABLED_TEMPSENSORS
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE,
 *                                 ADRV904X_CPU_CMD_ID_GET_ENABLED_TEMPSENSORS,
 *                                 ADRV904X_CPU_CMD_ID_SET_ENABLED_TEMPSENSORS
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_DEVTEMP_H__
#define __ADRV904X_CPU_CMD_DEVTEMP_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_dev_temp_types.h"
#include "adrv904x_cpu_error_codes_types.h"

/**
 * \brief ADRV904X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetDevTemp
{
    uint16_t avgMask;  /*!< Bitmask of adi_adrv904x_DevTempSensorMask_e values indicating
                            which temperature sensor readings should be averaged. See
                            definition of adi_adrv904x_DevTempData_t. */
} adrv904x_CpuCmd_GetDevTemp_t;)

/**
 * \brief ADRV904X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetDevTempResp
{
    adrv904x_CpuErrorCode_e     status;     /*!< CPU error status code */
    adi_adrv904x_DevTempData_t  tempData;   /*!< Temperature data returned by CPU */
} adrv904x_CpuCmd_GetDevTempResp_t;)

/**
 * \brief ADRV904X_CPU_CMD_ID_GET_ENABLED_TEMPSENSORS command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetDevTempSnsEnResp
{
    adrv904x_CpuErrorCode_e status;     /*!< CPU error status code */
    uint16_t tempEnData;                /*!<  Bitmask of adi_adrv904x_DevTempSensorMask_e values indicating 
                                                Temperature sensor enable bitfield returned by CPU */
} adrv904x_CpuCmd_GetDevTempSnsEnResp_t;)

/**
 * \brief ADRV904X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetDevTempSnsEn
{
    uint16_t tempEnData;  /*!< Bitmask of adi_adrv904x_DevTempSensorMask_e values indicating
                            which temperature sensors to enable/disable. */
} adrv904x_CpuCmd_SetDevTempSnsEn_t;)

/**
 * \brief ADRV904X_CPU_CMD_ID_SET_ENABLED_TEMPSENSORS command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetDevTempSnsEnResp
{
    adrv904x_CpuErrorCode_e status;         /*!< CPU error status code */
    uint16_t tempEnData;                    /*!< Bitmask of adi_adrv904x_DevTempSensorMask_e values indicating
                                                  which temperature sensors are actually enabled/disabled. */
} adrv904x_CpuCmd_SetDevTempSnsEnResp_t;)

#endif /* __ADRV904X_CPU_CMD_DEVTEMP_H__ */

