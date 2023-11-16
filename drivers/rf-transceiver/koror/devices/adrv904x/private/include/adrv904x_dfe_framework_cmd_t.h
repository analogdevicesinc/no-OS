/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file  framework_cmd_t.h
 * \brief Contains DFE Application type definitions for the ADI Command
 *        Handler Service.
 *
 * DFE APP Version: 2.9.0.4
 */

#ifndef _FRAMEWORK_CMD_T_H
#define _FRAMEWORK_CMD_T_H

#include "stdint.h"
#include "stdbool.h"
#include "adi_adrv904x_dfe_svc_err_codes_t.h"
#include "adi_adrv904x_dfe_app_err_codes_t.h"
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_dfe_framework_tracking_cal_t.h"
#include "adi_adrv904x_dfe_app_capture_sequencer_t.h"


#define ADRV904X_DFE_APP_FRAMEWORK_CMD_CALFRMWRK_OBJ_TYPE_MASK           0x03
#define ADRV904X_DFE_APP_FRAMEWORK_CMD_CALFRMWRK_OBJ_TYPE_CAL            0x01
#define ADRV904X_DFE_APP_FRAMEWORK_CMD_CALFRMWRK_OBJ_TYPE_FRMWRK         0x02

#define ADRV904X_DFE_APP_FRAMEWORK_CMD_EVENTCODE                         (0x00u)

#define ADRV904X_DFE_APP_FRAMEWORK_CMD_LOG_ENABLE                        (0)
#define  ADI_ADRV904X_DFE_APP_LOG_DPD_ADAPTATION_LOOP_EVENT_DATA              (0)
#define  ADI_ADRV904X_DFE_APP_LOG_DPD_ADAPTATION_LOOP_CAPTURE_BUF_RELEASES    (0)

typedef enum adrv904x_DfeAppFrameworkCmdOpcode
{
    ADRV904X_DFE_APP_FRAMEWORK_CMD_PING                         = 0x0000u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_SETCONFIG        = 0x0001u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_CTRL             = 0x0002u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_DEBUG            = 0x0003u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_ENABLEDISABLE    = 0x0004u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETCALSTATUS     = 0x0007u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETCONFIG        = 0x0008u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETCALSTATE      = 0x0009u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETENABLEDISABLE = 0x000Au,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_SETFRMWRKCONFIG  = 0x000Du,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETFRMWRKCONFIG  = 0x000Eu,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_SYSTEM_STATUS                = 0x000Fu,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_SETTIMER         = 0x0010u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETTIMER         = 0x0011u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_SETEXITFLAG      = 0x0012u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_SETCAPSEQCFG     = 0x0013u,
    ADRV904X_DFE_APP_FRAMEWORK_CMD_TRACKINGCAL_GETCAPSEQCFG     = 0x0014u
}adrv904x_DfeAppFrameworkCmdOpcode_e;

/*! Calibration status type */
typedef enum adrv904x_DfeAppFrameworkCmdCalStatusType
{
    ADRV904X_DFE_APP_FRAMEWORK_CMD_CAL_STATUS_COMMON,     /*!< Common calibration status */
    ADRV904X_DFE_APP_FRAMEWORK_CMD_CAL_STATUS_SPECIFIC,   /*!< Calibration-specific status */
    ADRV904X_DFE_APP_FRAMEWORK_CMD_CAL_STATUS_PRIVATE,    /*!< Private calibration-specific status */
    ADRV904X_DFE_APP_FRAMEWORK_CMD_CAL_STATUS_NUM_STATUS
} adrv904x_DfeAppFrameworkCmdCalStatusType_e;

ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppCmd
{
    uint16_t appCmdOpcode;                   /*!< Application command opcode */
    /* void*          appCmdData[];             */
} adrv904x_DfeAppFrameworkCmdAppCmd_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdGenericCmd
{
    uint32_t generic;                           /*!< Placeholder data field */
}   adrv904x_DfeAppFrameworkCmdGenericCmd_t;
ADI_ADRV904X_PACK_FINISH

typedef struct adrv904x_DfeAppFrameworkCmdGenericResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                   /*!< App error status code */
}   adrv904x_DfeAppFrameworkCmdGenericResp_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppPing
{
    uint32_t echoData;                      /*!< Data to be echoed back by CPU */
} adrv904x_DfeAppFrameworkCmdAppPing_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppPingResp
{
    adi_adrv904x_DfeAppErrCode_e status;               /*!< CPU error status code */
    uint32_t          data;                 /*!< Echoed data from CPU */
} adrv904x_DfeAppFrameworkCmdAppPingResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SET_CONFIG command structure
 */

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetConfig
{
    uint32_t objId;                        /*!< Object ID of calibration or system component */
    uint16_t offset;                       /*!< Offset into the configuration structure */
    uint16_t length;                       /*!< Length of the configuration in bytes */

    /* Config data payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t configData[];
     */
} adrv904x_DfeAppFrameworkCmdAppSetConfig_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SET_CONFIG command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetConfigResp
{
    adi_adrv904x_DfeAppErrCode_e cmdStatus;  /*!< Command status */
} adrv904x_DfeAppFrameworkCmdAppSetConfigResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief GET_CONFIG command structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppGetConfig
{
    uint32_t objId;                        /*!< Object ID of calibration or system component */
    uint16_t offset;                       /*!< Offset into the configuration structure */
    uint16_t length;                       /*!< Length of the configuration in bytes */
} adrv904x_DfeAppFrameworkCmdAppGetConfig_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief GET_CONFIG command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppGetConfigResp
{
    adi_adrv904x_DfeAppErrCode_e cmdStatus;      /*!< Command status */

    /* Config data payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t configData[];
     */
} adrv904x_DfeAppFrameworkCmdAppGetConfigResp_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetEnabledCals
{
    uint32_t calMask;          /*!< Cal mask to enable or disable */
    uint32_t channelMask;      /*!< Channel mask to apply the command */
    bool     bEnable;          /*!< false:Disable selected cal/s    true:Enable selected cal/s */
} adrv904x_DfeAppFrameworkCmdAppSetEnabledCals_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetEnabledCalsResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                   /*!< CPU error status code */
} adrv904x_DfeAppFrameworkCmdAppSetEnabledCalsResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief GET_CAL_STATUS command structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppGetCalStatus
{
    adi_adrv904x_DfeAppFrameworkTrackingCalStatusType_e type;       /*!< Calibration status type to be retrieved */
    uint32_t                                 calObjId;   /*!< Object ID of calibration */
    uint32_t                                 channelNum; /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
} adrv904x_DfeAppFrameworkCmdAppGetCalStatus_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief GET_CAL_STATUS command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp
{
    adi_adrv904x_DfeAppErrCode_e cmdStatus;  /*!< Command status */

    /* Cal status payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* calStatus[];
     */
} adrv904x_DfeAppFrameworkCmdAppGetCalStatusResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SETEXITFLAG command structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetExitFlag
{
    uint32_t calObjId;                                    /*!< Object ID of calibration */
    uint32_t channelNum;                                  /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
    uint8_t  exitFlagVal;                                 /*!< Value to set */
} adrv904x_DfeAppFrameworkCmdAppSetExitFlag_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SETEXITFLAG command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetExitFlagResp
{
    adi_adrv904x_DfeAppErrCode_e cmdStatus;  /*!< Command status */

    /* Cal status payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* calStatus[];
     */
} adrv904x_DfeAppFrameworkCmdAppSetExitFlagResp_t;
ADI_ADRV904X_PACK_FINISH


/**
 * \brief SETCAPSEQCFG command structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetCapSeqCfg
{
    adi_adrv904x_DfeAppCaptureSequencerCfg_t capSeqCfgSet;                                  /*!< Config to set */
} adrv904x_DfeAppFrameworkCmdAppSetCapSeqCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SETCAPSEQCFG command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetCapSeqCfgResp
{
    adi_adrv904x_DfeAppErrCode_e cmdStatus;  /*!< Command status */

    /* Cal status payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* calStatus[];
     */
} adrv904x_DfeAppFrameworkCmdAppSetCapSeqCfgResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief GETCAPSEQCFG command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppGetCapSeqCfgResp
{
    adi_adrv904x_DfeAppErrCode_e             cmdStatus;    /*!< Command status */

    adi_adrv904x_DfeAppCaptureSequencerCfg_t capSeqCfgGet; /*!< Config to get */

    /* Cal status payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * void* calStatus[];
     */
} adrv904x_DfeAppFrameworkCmdAppGetCapSeqCfgResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief GET_TRACKING_CAL_STATE command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppGetTrackingCalStateResp
{
    adi_adrv904x_DfeAppErrCode_e                   status;
    adi_adrv904x_DfeAppFrameworkTrackingCalState_t calState;
} adrv904x_DfeAppFrameworkCmdAppGetTrackingCalStateResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief GET_ENABLED_TRACKING_CALS command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppGetEnabledTrackingCalsResp
{
    adi_adrv904x_DfeAppErrCode_e                         status;
    adi_adrv904x_DfeAppFrameworkTrackingCalEnableMasks_t enableMasks;
} adrv904x_DfeAppFrameworkCmdAppGetEnabledTrackingCalsResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SET_CTRL command structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetCtrl
{
    uint32_t objId;                 /*!< Object ID of the cal or system component */
    uint16_t ctrlCmd;               /*!< Command to be executed */
    uint32_t channelNum;            /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
    uint16_t length;                /*!< Length of the control data in bytes */

    /* Control data payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t ctrlData[];
     */
} adrv904x_DfeAppFrameworkCmdAppSetCtrl_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief SET_CTRL command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetCtrlResp
{
    adi_adrv904x_DfeAppErrCode_e cmdStatus;        /*!< Error Code */
    uint16_t          length;           /*!< Length of the control command response in bytes */

    /* Control data response payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t ctrlResp[];
     */
} adrv904x_DfeAppFrameworkCmdAppSetCtrlResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief DEBUG command structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppDebug
{
    uint32_t objId;                 /*!< Object ID of the cal or system component */
    uint16_t debugCmd;              /*!< Command to be executed */
    uint32_t channelNum;            /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
    uint16_t length;                /*!< Length of the debug data payload in bytes */

    /* Debug data payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t dbgData[];
     */
} adrv904x_DfeAppFrameworkCmdAppDebug_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief DEBUG command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppDebugResp
{
    adi_adrv904x_DfeAppErrCode_e cmdStatus;        /*!< Error Code */
    uint16_t          length;           /*!< Length of the response data in bytes */

    /* Debug data response payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t respData[];
     */
} adrv904x_DfeAppFrameworkCmdAppDebugResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief TRACKINGCAL_SETTIMER command structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetTimer
{
    adi_adrv904x_DfeAppFrameworkTrackingCalType_t calType;          /*!< cal type                      */
    uint32_t                           calTimerSetValue; /*!< value of calTimer in mS       */
} adrv904x_DfeAppFrameworkCmdAppSetTimer_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief TRACKINGCAL_SETTIMER command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppSetTimerResp
{
    adi_adrv904x_DfeAppErrCode_e cmdStatus;        /*!< Error Code */
} adrv904x_DfeAppFrameworkCmdAppSetTimerResp_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief TRACKINGCAL_GETTIMER command structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppGetTimer
{
    adi_adrv904x_DfeAppFrameworkTrackingCalType_t calType;       /*!< cal timer type                */
} adrv904x_DfeAppFrameworkCmdAppGetTimer_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief TRACKINGCAL_SETTIMER command response structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeAppFrameworkCmdAppGetTimerResp
{
    adi_adrv904x_DfeAppErrCode_e cmdStatus;              /*!< Error Code                                                               */
    uint32_t          calTimerGetValue;       /*!< value of calTimer specified in adrv904x_DfeAppFrameworkCmdAppGetTimer_t in mS  */
} adrv904x_DfeAppFrameworkCmdAppGetTimerResp_t;
ADI_ADRV904X_PACK_FINISH

#endif /* _FRAMEWORK_CMD_T_H */


