/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
/**
 * \file adrv904x_cpu_cmd_jesd_ser_lane_getset_cfg.h
 * 
 * \brief   Contains ADRV904X JESD Serializer lane data structures.
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADI_ADRV904X_CPU_CMD_JESD_SER_LANE_GETSET_CFG_H__
#define __ADI_ADRV904X_CPU_CMD_JESD_SER_LANE_GETSET_CFG_H__

#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_error_codes_types.h"

/**
 * \brief Serializer lane get configuration command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetJesdSerLaneCfg
{
    uint8_t                     lane;             /*!< Serializer lane number */
} adrv904x_CpuCmd_GetJesdSerLaneCfg_t;)


/**
 * \brief Serializer lane get configuration response command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetJesdSerLaneCfgResp
{
    adrv904x_CpuErrorCode_e     status;           /*!< CPU error status code */
    uint8_t                     lane;             /*!< Serializer lane number */
    uint8_t                     outputDriveSwing; /*!< Serializer Lane output swing level */
    uint8_t                     preEmphasis;      /*!< Serializer Lane pre-emphasis */
    uint8_t                     postEmphasis;     /*!< Serializer Lane post-emphasis */   
} adrv904x_CpuCmd_GetJesdSerLaneCfgResp_t;)


/**
 * \brief Serializer lane set configuration command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetJesdSerLaneCfg
{
    uint8_t                     lane;             /*!< Serializer lane number */ 
    uint8_t                     outputDriveSwing; /*!< Serializer Lane output swing level */
    uint8_t                     preEmphasis;      /*!< Serializer Lane pre-emphasis */	
    uint8_t                     postEmphasis;     /*!< Serializer Lane post-emphasis */   
} adrv904x_CpuCmd_SetJesdSerLaneCfg_t;)


/**
 * \brief Serializer lane set configuration response command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetJesdSerLaneCfgResp
{
    adrv904x_CpuErrorCode_e status;           /*!< CPU error status code */
} adrv904x_CpuCmd_SetJesdSerLaneCfgResp_t;)



#endif /* __ADI_ADRV904X_CPU_CMD_JESD_SER_LANE_GETSET_CFG_H__ */


