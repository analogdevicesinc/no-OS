/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**
 * \file adrv903x_cpu_cmd_jesd_ser_lane_getset_cfg.h
 * 
 * \brief   Contains ADRV903X JESD Serializer lane data structures.
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADI_ADRV903X_CPU_CMD_JESD_SER_LANE_GETSET_CFG_H__
#define __ADI_ADRV903X_CPU_CMD_JESD_SER_LANE_GETSET_CFG_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"

/**
 * \brief Serializer lane get configuration command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetJesdSerLaneCfg
{
    uint8_t                     lane;             /*!< Serializer lane number */
} adrv903x_CpuCmd_GetJesdSerLaneCfg_t;)


/**
 * \brief Serializer lane get configuration response command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetJesdSerLaneCfgResp
{
    adrv903x_CpuErrorCode_e     status;           /*!< CPU error status code */
    uint8_t                     lane;             /*!< Serializer lane number */
    uint8_t                     outputDriveSwing; /*!< Serializer Lane output swing level */
    uint8_t                     preEmphasis;      /*!< Serializer Lane pre-emphasis */
    uint8_t                     postEmphasis;     /*!< Serializer Lane post-emphasis */   
} adrv903x_CpuCmd_GetJesdSerLaneCfgResp_t;)


/**
 * \brief Serializer lane set configuration command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetJesdSerLaneCfg
{
    uint8_t                     lane;             /*!< Serializer lane number */ 
    uint8_t                     outputDriveSwing; /*!< Serializer Lane output swing level */
    uint8_t                     preEmphasis;      /*!< Serializer Lane pre-emphasis */	
    uint8_t                     postEmphasis;     /*!< Serializer Lane post-emphasis */   
} adrv903x_CpuCmd_SetJesdSerLaneCfg_t;)


/**
 * \brief Serializer lane set configuration response command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetJesdSerLaneCfgResp
{
    adrv903x_CpuErrorCode_e status;           /*!< CPU error status code */
} adrv903x_CpuCmd_SetJesdSerLaneCfgResp_t;)



#endif /* __ADI_ADRV903X_CPU_CMD_JESD_SER_LANE_GETSET_CFG_H__ */


