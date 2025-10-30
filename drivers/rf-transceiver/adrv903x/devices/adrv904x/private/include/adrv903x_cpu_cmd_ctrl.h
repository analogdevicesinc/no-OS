/**
 * \file adrv903x_cpu_cmd_cal_ctrl.h
 * 
 * \brief   Command definition for ADRV903X_CPU_CMD_ID_SET_CTRL
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_CTRL_H__
#define __ADRV903X_CPU_CMD_CTRL_H__

#include "adi_adrv903x_platform_pack.h"
#include "adrv903x_cpu_error_codes_types.h"
#include "adrv903x_cpu_object_ids_types.h"
#include "adi_adrv903x_cals_types.h"

/**
 * \brief Maximum ctrl payload size
 */
#define MAX_CTRL_DATA_SIZE    (256u)

/**
 * \brief Maximum ctrl response size
 */
#define MAX_CTRL_RESP_SIZE    (256u)

#ifndef CLIENT_IGNORE
/**
 * \brief SET_CTRL command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetCtrl
{
    adrv903x_CpuObjectId_t objId;   /*!< Object ID of the cal or system component */
    uint16_t ctrlCmd;               /*!< Command to be executed */
    uint32_t channelNum;            /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
    uint16_t length;                /*!< Length of the control data in bytes */

    /* Control data payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t ctrlData[];
     */
} adrv903x_CpuCmd_SetCtrl_t;)


/**
 * \brief SET_CTRL command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetCtrlResp
{
    adrv903x_CpuErrorCode_e cmdStatus;  /*!< Command status */
    uint16_t length;                    /*!< Length of the control command response in bytes */

    /* Control data response payload follows command header. 
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t ctrlResp[];
     */
} adrv903x_CpuCmd_SetCtrlResp_t;)

/**
 * \brief Ctrl payload size struct.
 *        Used to determine the maximum ctrl payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv903x_CpuCmd_SetCtrlMaxSize
{
    adrv903x_CpuCmd_SetCtrl_t  setCtrlCmd;
    uint8_t                    ctrlData[MAX_CTRL_DATA_SIZE];   /*!< Ctrl data */ 
} adrv903x_CpuCmd_SetCtrlMaxSize_t;

/**
 * \brief Ctrl payload size struct.
 *        Used to determine the maximum ctrl payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef struct adrv903x_CpuCmd_SetCtrlRespMaxSize
{
    adrv903x_CpuCmd_SetCtrlResp_t  setCtrlCmdResp;
    uint8_t                        ctrlResp[MAX_CTRL_RESP_SIZE];   /*!< Ctrl response */ 
} adrv903x_CpuCmd_SetCtrlRespMaxSize_t;

/**
 * \brief ADC cal control commands
 * \note Not instantiated. 
 */
typedef enum adrv903x_CpuCmd_AdcCtrlCmd
{
    ADC_CTRL_GET_SW_VERSION = 0u,
    ADC_CTRL_INIT           = 1u,
    ADC_CTRL_SET_FSM_CMD    = 2u,
    ADC_CTRL_GET_FSM_STATE  = 3u,
    ADC_CTRL_GET_DATA_SIZES = 4u,
    ADC_CTRL_GET_DATA_ADDRS = 5u,
    ADC_CTRL_RUN_CMD        = 6u,
    ADC_CTRL_CMD_INVALID    = 7u
} adrv903x_CpuCmd_AdcCtrlCmd_e;

typedef enum adrv903x_CpuCmd_AdcCtrlFsmCmd
{
    ADC_FSM_CMD_IDLE_CAL        = 0u,
    ADC_FSM_CMD_INIT_CAL        = 1u, 
    ADC_FSM_CMD_RUN_FG_CAL      = 2u,
    ADC_FSM_CMD_RUN_BG_CAL      = 3u,
    ADC_FSM_CMD_FREEZE_CAL      = 4u,
    ADC_FSM_CMD_ABORT_CAL       = 5u,
    ADC_FSM_CMD_WARM_BOOT_CAL   = 6u
} adrv903x_CpuCmd_AdcCtrlFsmCmd_e;
#endif /* CLIENT_IGNORE */

/**
 * \brief ADC cal get channel object data sizes and addresses
 *        
 * \note 
 */
typedef struct adrv903x_CpuCmd_GetAdcSizeAddrs
{
    uint32_t obj;
    uint32_t init;
    uint32_t config;
    uint32_t calObj;
    uint32_t calState;
    uint32_t calData; 
} adrv903x_CpuCmd_GetAdcSizeAddrs_t;

#endif /* __ADRV903X_CPU_CMD_CTRL_H__ */

