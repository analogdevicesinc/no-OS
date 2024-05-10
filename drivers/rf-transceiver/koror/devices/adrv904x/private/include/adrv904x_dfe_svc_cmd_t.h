/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adrv904x_dfe_svc_cmd_t.h
 *
 * \brief   Contains shared SDK/DFE command typedefs
 *
 * \details Contains DFE error code definitions
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADRV904X_DFE_SVC_CMD_T_H__
#define __ADRV904X_DFE_SVC_CMD_T_H__

#include "adi_adrv904x_dfe_svc_err_codes_t.h"
#include "adi_adrv904x_dfe_osal_t.h"

/**
 *  \brief Maximum command log record size
 */
#define ADRV904X_DFE_SVC_CMD_LOG_SIZE    (12U)

/**
 * \brief Maximum debug payload size
 */
#define MAX_DEBUG_DATA_SIZE      (256u)

/**
 * \brief Maximum debug response size
 */
#define MAX_DEBUG_RESP_SIZE      (256u)

/**
 *  \brief SDK Command enumerations
 */
typedef enum adrv904x_DfeSvcCmdId
{
    ADRV904X_DFE_SVC_CMD_ID_PING                             = 0x00u,                                    /*!< SDK Ping command */
    ADRV904X_DFE_SVC_CMD_ID_RADIO_PING                       = 0x01u,                                    /*!< SDK Radio Ping command */
    ADRV904X_DFE_SVC_CMD_ID_RADIO_START                      = 0x02u,                                    /*!< SDK Radio Start command */
    ADRV904X_DFE_SVC_CMD_ID_RADIO_STOP                       = 0x03u,                                    /*!< SDK Radio Stop command */
    ADRV904X_DFE_SVC_CMD_ID_GET_SYS_STATUS                   = 0x09u,                                    /*!< SDK Sys Status command */
    ADRV904X_DFE_SVC_CMD_ID_PREPARE_RELOAD                   = 0x10u,                                    /*!< SDK Prepare for Reload command */
    ADRV904X_DFE_SVC_CMD_ID_UNLOCK_CONFIG                    = 0x20u,                                    /*!< SDK Unlock Config command */
    ADRV904X_DFE_SVC_CMD_ID_SET_CONFIG                       = 0x21u,                                    /*!< SDK Set Config command */
    ADRV904X_DFE_SVC_CMD_ID_GET_CONFIG                       = 0x22u,                                    /*!< SDK Get Config command */
    ADRV904X_DFE_SVC_CMD_ID_SET_CTRL                         = 0x23u,                                    /*!< SDK Set system or calibration ctrl command */
    ADRV904X_DFE_SVC_CMD_ID_RS_PATTERN_LUT_SET               = 0x24u,                                    /*!< SDK Set RS pattern lookup table */
    ADRV904X_DFE_SVC_CMD_ID_DEBUG                            = 0x25u,                                    /*!< SDK Debug command */
    ADRV904X_DFE_SVC_CMD_ID_LOGTRACE_BUFFER                  = 0x26u,                                    /*!< SDK Logtrace buffer command*/
    ADRV904X_DFE_SVC_CMD_ID_UPDATE_ECC                       = 0x27u,                                    /*!< SDK ECC Update command */
    ADRV904X_DFE_SVC_CMD_ID_DEBUG_CMD                        = 0x28u,                                    /*!< SDK Debug command */
    ADRV904X_DFE_SVC_CMD_ID_WATCHDOG_CONFIG                  = 0x29u,                                    /*!< SDK Configure watchdog command */

    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_CONFIG_SET          = 0x30u,                                    /*!< SDK DFE power meter TSSI config set command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_CONFIG_GET          = 0x31u,                                    /*!< SDK DFE power meter TSSI config get command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_ENABLE              = 0x32u,                                    /*!< SDK DFE power meter TSSI enable command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_CLGC_TSSI_ENABLE         = 0x33u,                                    /*!< SDK DFE power meter CLGC TSSI enable command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_MANUAL_START        = 0x34u,                                    /*!< SDK DFE power meter TSSI manual start command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_READBACK            = 0x35u,                                    /*!< SDK DFE power meter TSSI readback command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_CLGC_TSSI_READBACK       = 0x36u,                                    /*!< SDK DFE power meter CLGC TSSI readback command */

    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_CONFIG_SET        = 0x37u,                                    /*!< SDK DFE power meter DPD In  config set command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_CONFIG_GET        = 0x38u,                                    /*!< SDK DFE power meter DPD In  config get command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_ENABLE            = 0x39u,                                    /*!< SDK DFE power meter DPD In  meas enable command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_CONFIG_SET       = 0x3Au,                                    /*!< SDK DFE power meter DPD Out config set command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_CONFIG_GET       = 0x3Bu,                                    /*!< SDK DFE power meter DPD Out config get command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_ENABLE           = 0x3Cu,                                    /*!< SDK DFE power meter DPD Out meas enable command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_OUT_READBACK      = 0x3Du,                                    /*!< SDK DFE power meter DPD In,Out readback command */

    ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_CONFIG_SET   = 0x3Eu,                                    /*!< SDK DFE power meter Tx carrier meas config set command */
    ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_CONFIG_GET   = 0x3Fu,                                    /*!< SDK DFE power meter Tx carrier meas config get command */
    ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_ENABLE       = 0x40u,                                    /*!< SDK DFE power meter Tx carrier meas enable command */
    ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_MANUAL_START = 0x41u,                                    /*!< SDK DFE power meter Tx carrier manual start command */
    ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_GET          = 0x42u,                                    /*!< SDK DFE power meter Tx carrier meas readback command */

    ADRV904X_DFE_SVC_CMD_ID_LOG_MASK_SET                     = 0x43u,                                    /*!< SDK DFE log level mask set command */
    ADRV904X_DFE_SVC_CMD_ID_LOG_MASK_GET                     = 0x44u,                                    /*!< SDK DFE log level mask get command */

    ADRV904X_DFE_SVC_CMD_ID_CPU_LOAD_GET                     = 0x45u,                                    /*!< SDK Get CPU loading command */
    ADRV904X_DFE_SVC_CMD_ID_CPU_LOAD_INTERVAL_SET            = 0x46u,                                    /*!< SDK Set CPU loading calculation interval command */
    ADRV904X_DFE_SVC_CMD_ID_CPU_LOAD_INTERVAL_GET            = 0x47u,                                    /*!< SDK Get CPU loading calculation interval command */

    ADRV904X_DFE_SVC_CMD_ID_WBREG_ACCESS                     = 0x48u,                                    /*!< SDK WB Regularization access command */
    ADRV904X_DFE_SVC_CMD_ID_TELEMETRY_MASK_SET               = 0x49u,                                    /*!< SDK DFE telemetry  mask set command */
    ADRV904X_DFE_SVC_CMD_ID_TELEMETRY_MASK_GET               = 0x4Au,                                    /*!< SDK DFE telemetry mask get command */
    ADRV904X_DFE_SVC_CMD_ID_SEQUENCER_WAIT_FOR_SSB_SYNC      = 0x4Bu,                                    /*!< SDK wait for SSB SYNC command */

    ADRV904X_DFE_SVC_CMD_ID_ECC_ENABLE_SET                   = 0x4Cu,                                    /*!< SDK ECC Enable set command */
    ADRV904X_DFE_SVC_CMD_ID_ECC_ENABLE_GET                   = 0x4Du,                                    /*!< SDK ECC Enable get command */

    ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_PARAMS_SET           = 0x4Eu,                                    /*!< SDK Set Radio TxLOL correction parameters command */
    ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_PARAMS_GET           = 0x4Fu,                                    /*!< SDK Get Radio TxLOL correction parameters command */
    ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_ENABLE_UPDATES_SET   = 0x50u,                                    /*!< SDK Set Radio TxLOL enable updates command */
    ADRV904X_DFE_SVC_CMD_ID_RADIO_TXLOL_ENABLE_UPDATES_GET   = 0x51u,                                    /*!< SDK Get Radio TxLOL enable updates command */
    ADRV904X_DFE_SVC_CMD_ID_RADIO_TX_PFIR_FRAC_DELAY_SET     = 0x52u,                                    /*!< SDK DFE Radio CTC 2 Frac Delay command */

    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_CONFIG_SET          = 0x53u,                                    /*!< SDK DFE power meter RSSI (DDC) config set command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_CONFIG_GET          = 0x54u,                                    /*!< SDK DFE power meter RSSI (DDC) config get command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_ENABLE              = 0x55u,                                    /*!< SDK DFE power meter RSSI (DDC) enable command */
    ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_READBACK            = 0x56u,                                    /*!< SDK DFE power meter RSSI (DDC) readback command */

    ADRV904X_DFE_SVC_CPU_CMD_ID_DFE_APPLICATION              = 0xFFu,                                    /*!< SDK max command ID */
    ADRV904X_DFE_SVC_CMD_ID_MAX_NUM                          = ADRV904X_DFE_SVC_CPU_CMD_ID_DFE_APPLICATION + 1u, /*!< number of command IDs */
} adrv904x_DfeSvcCmdId_e;

/** \brief CPU command ID type */
typedef uint16_t adrv904x_DfeSvcCmdId_t;

/**
 * \brief SDK command status enumeration
 */
typedef enum adrv904x_DfeSvcCmdStatus
{
    ADRV904X_DFE_SVC_CMD_STATUS_NO_ERROR,                   /*!< No error */
    ADRV904X_DFE_SVC_CMD_STATUS_GENERIC,                    /*!< Unspecified/unknown error */
    ADRV904X_DFE_SVC_CMD_STATUS_LINK_ERROR,                 /*!< Link (lower level) error */
    ADRV904X_DFE_SVC_CMD_STATUS_UNEXPECTED_TRANSACTION_ID,  /*!< Unexpected/invalid transaction ID received */
    ADRV904X_DFE_SVC_CMD_STATUS_CMD_FAILED,                 /*!< Command-specific failure. See command-specific response for details. */
    ADRV904X_DFE_SVC_CMD_STATUS_CMD_ID_INVALID              /*!< Invalid command ID */
} adrv904x_DfeSvcCmdStatus_e;
typedef uint16_t adrv904x_DfeSvcCmdStatus_t;

/**
 * \brief SDK command Generic structure
 */
typedef struct adrv904x_DfeSvcCmdGenericCmd
{
    uint32_t generic;                           /*!< Placeholder data field */
}   adrv904x_DfeSvcCmdGenericCmd_t;

/**
 * \brief SDK command Generic response structure
 */
typedef struct adrv904x_DfeSvcCmdGenericResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                   /*!< SDK error status code */
}   adrv904x_DfeSvcCmdGenericResp_t;

/**
 * \brief SDK command Ping structure
 */
typedef struct adrv904x_DfeSvcCmdPing
{
    uint32_t echoData;                          /*!< Data to be echoed back by SDK */
}   adrv904x_DfeSvcCmdPing_t;

/**
 * \brief SDK command Ping Response structure
 */
typedef struct adrv904x_DfeSvcCmdPingResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                   /*!< SDK error status code */
    uint32_t          echoData;                 /*!< Echoed data from SDK */
} adrv904x_DfeSvcCmdPingResp_t;

/** \brief CPU command transaction ID type */
typedef uint16_t adrv904x_DfeSvcCmdTransactionId_t;

/**
 * \brief Common CPU command structure
 */
typedef struct adrv904x_DfeSvcCmd
{
    adrv904x_DfeSvcCmdId_t            cmdId;               /*!< Command ID. Value is from adrv904x_DfeSvcCmnCpuCmdId_e enumeration. */
    adrv904x_DfeSvcCmdTransactionId_t tId;                 /*!< Transaction ID. Unique ID for a command/response pair. */

    /* Command payload follows command header.
     * This can't be declared here due to use of the -Wpedantic compiler option.
     * void* payload[];
     */
}   adrv904x_DfeSvcCmd_t;

/**
 * \brief Common CPU command response structure
 */
typedef struct adrv904x_DfeSvcCmdResp
{
    adrv904x_DfeSvcCmdId_t            cmdId;  /*!< Command ID. Value is from adrv904x_DfeSvcCmnCpuCmdId_e enumeration. */
    adrv904x_DfeSvcCmdTransactionId_t tId;    /*!< Transaction ID. Unique ID for a command/response pair. */
    adrv904x_DfeSvcCmdStatus_t        status; /*!< Command status. Value is from adrv904x_DfeSvcCmnCpuCmdStatus_e enumeration. */

    /* Command response payload follows command header.
     * This can't be declared here due to use of the -Wpedantic compiler option
     * void* payload[];
     */
}   adrv904x_DfeSvcCmdResp_t;

/**
 * \brief UNLOCK_CONFIG command structure
 */
typedef struct adrv904x_DfeSvcCmdUnlockConfig
{
    uint32_t configKey;
} adrv904x_DfeSvcCmdUnlockConfig_t;

/**
 * \brief UNLOCK_CONFIG command response structure
 */
typedef struct adrv904x_DfeSvcCmdUnlockConfigResp
{
    adi_adrv904x_DfeSvcErrCode_e cmdStatus;      /*!< Command status */
} adrv904x_DfeSvcCmdUnlockConfigResp_t;

/**
 * \brief SET_CONFIG command structure
 */
typedef struct adrv904x_DfeSvcCmdSetConfig
{
    adi_adrv904x_DfeSvcErrObjId_t objId;                 /*!< Object ID of calibration or system component */
    uint16_t           offset;                /*!< Offset into the configuration structure */
    uint16_t           length;                /*!< Length of the configuration in bytes */

    /* Config data payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t configData[];
     */
} adrv904x_DfeSvcCmdSetConfig_t;

/**
 * \brief SET_CONFIG command response structure
 */
typedef struct adrv904x_DfeSvcCmdSetConfigResp
{
    adi_adrv904x_DfeSvcErrCode_e cmdStatus;  /*!< Command status */
} adrv904x_DfeSvcCmdSetConfigResp_t;

/**
 * \brief ECC_Update command response structure
 */
typedef struct adrv904x_DfeSvcEccUpdateResp
{
    adi_adrv904x_DfeSvcErrCode_e cmdStatus;  /*!< Command status */
} adrv904x_DfeSvcEccUpdateResp_t;

/**
 * \brief GET_CONFIG command structure
 */
typedef struct adrv904x_DfeSvcCmdGetConfig
{
    adi_adrv904x_DfeSvcErrObjId_t objId;                 /*!< Object ID of calibration or system component */
    uint16_t           offset;                /*!< Offset into the configuration structure */
    uint16_t           length;                /*!< Length of the configuration in bytes */
} adrv904x_DfeSvcCmdGetConfig_t;

/**
 * \brief GET_CONFIG command response structure
 */
typedef struct adrv904x_DfeSvcCmdGetConfigResp
{
    adi_adrv904x_DfeSvcErrCode_e cmdStatus;      /*!< Command status */

    /* Config data payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     * uint8_t configData[];
     */
} adrv904x_DfeSvcCmdGetConfigResp_t;

/**
 * \brief Header in the response buff which stores the error status during the message processing
 */
typedef struct
{
    adi_adrv904x_DfeSvcErrCode_e status;                   /*!< Error Code */
} adrv904x_DfeSvcCmdMsgProcessStatus_t;

/**
 * \brief Shadow definition of _adi_drv_MailboxData_t
 */
typedef struct
{
    uint32_t handle;
    uint32_t opcode;
    uint32_t data;
} adrv904x_DfeSvcCmdMailboxLogData_t;


/**
 * \brief Set of data that encompasses a command
 */
typedef struct
{
    adrv904x_DfeSvcCmd_t               cmdData;                           /*!< Command ID. Value is from adrv904x_DfeSvcCmnCpuCmdId_e enumeration. */
    adrv904x_DfeSvcCmdMailboxLogData_t mailBoxData;                       /*!< For sending, handle of mailbox to send to. For reception, handle of mailbox received from. */
    uint64_t                     timeStampUs;                       /*!< Timestamp for the command receieved in microseconds(based on OS ticks) */
} adrv904x_DfeSvcCmdLogData_t;

/**
 * \brief Structure contains the opcode and result of executing the command
 */
typedef struct adrv904x_DfeSvcCmdCtrlResult
{
    uint32_t           handle;                  /*!< Handle for the command */
    uint32_t           opcode;                  /*!< Opcode of the command */
    adi_adrv904x_DfeSvcErrCode_e  resCode;                 /*!< Result of executing the command */
    adrv904x_DfeSvcCmdResp_t cmdResp;                 /*!< Command response result */
    uint64_t           timeStampUs;             /*!< Timestamp for the command responded in microseconds(based on OS ticks) */
}adrv904x_DfeSvcCmdCtrlResult_t;

/**
 * \brief Command and result log structure.
 *
 * \details The command and result log are circular buffers.
 *  The last #ADRV904X_DFE_SVC_CMD_LOG_SIZE commands and their results are stored.
 *  Since the arrival of command and reporting the results are asynchronous,
 *  the result at index 'N' does not correspond to command at index 'N'.
 */

typedef struct adrv904x_DfeSvcCmdCtrl_Log
{
    uint8_t                  cmdIdx;                                /*!< Command log index */
    uint8_t                  resultIdx;                             /*!< Result log index */
    adrv904x_DfeSvcCmdLogData_t    cmdLog[ADRV904X_DFE_SVC_CMD_LOG_SIZE];         /*!< Command log */
    adrv904x_DfeSvcCmdCtrlResult_t resultLog[ADRV904X_DFE_SVC_CMD_LOG_SIZE];      /*!< Result log */
} adrv904x_DfeSvcCmdCtrlLog_t;

/**
 * \brief DEBUG_MODE command structure
 */
typedef struct adrv904x_DfeSvcCmdDebugMode
{
    uint32_t svcDebugModeKey;           /*!< Command status */
} adrv904x_DfeSvcCmdDebugMode_t;


/**
 * \brief DEBUG_MODE command response structure
 */
typedef struct adrv904x_DfeSvcCmdDebugModeResp
{
    adi_adrv904x_DfeSvcErrCode_e cmdStatus;        /*!< Error Code */
} adrv904x_DfeSvcCmdDebugModeResp_t;


/**
 * \brief DEBUG command structure
 */
typedef struct adrv904x_DfeSvcCmdDebugCmd
{
    adi_adrv904x_DfeSvcErrObjId_t objId;           /*!< Object ID of calibration or system component */
    uint16_t           debugCmd;        /*!< Command to be executed */
    uint32_t           channelNum;      /*!< Channel number (0 for channel 1, 1 for channel 2, etc.) */
    uint16_t           length;          /*!< Length of the debug data payload in bytes */

    /* Debug data payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     *  uint8_t dbgData[];
     */
} adrv904x_DfeSvcCmdDebugCmd_t;


/**
 * \brief DEBUG command response structure
 */
typedef struct adrv904x_DfeSvcCmdDebugCmdResp
{
    adi_adrv904x_DfeSvcErrCode_e cmdStatus;        /*!< Command status */
    uint16_t          length;           /*!< Length of the response data in bytes */

    /* Debug data response payload follows command header.
     * This can't be declared here due to the API's use of the -Wpedantic compiler option.
     *  uint8_t respData[];
     */
} adrv904x_DfeSvcCmdDebugCmdResp_t;

/**
 * \brief LogTrace_Buffer command structure
 */
typedef struct adrv904x_DfeSvcCmdLogTraceBuffer
{
    uint8_t svcLogTraceBufferStatus;           /*!< LogTrace Buffer status */
} adrv904x_DfeSvcCmdLogTraceBuffer_t;

/**
 * \brief LogTrace_Buffer command response structure
 */
typedef struct adrv904x_DfeSvcCmdLogTraceBufferResp
{
    adi_adrv904x_DfeSvcErrCode_t cmdStatus;        /*!< Command status */
} adrv904x_DfeSvcCmdLogTraceBufferResp_t;

/**
 * \brief Watchdog_config command structure
 */
typedef struct adrv904x_DfeSvcCmdWatchdogConfig
{
    uint32_t timeout_ms; /*!< Watchdog timeout value */
    uint8_t  wdt;        /*!< Watchdog instance */
} adrv904x_DfeSvcCmdWatchdogConfig_t;

/**
 * \brief Watchdog_config command response structure
 */
typedef struct adrv904x_DfeSvcCmdWatchdogConfigResp
{
    adi_adrv904x_DfeSvcErrCode_t cmdStatus;        /*!< Command status */
} adrv904x_DfeSvcCmdWatchdogConfigResp_t;

/**
 * \brief PREPARE_RELOAD command structure
 */
typedef struct adrv904x_DfeSvcCmdPrepareReload
{
    uint32_t reload;           /*!< Unused */
} adrv904x_DfeSvcCmdPrepareReload_t;

/**
 * \brief PREPARE_RELOAD command response structure
 */
typedef struct adrv904x_DfeSvcCmdPrepareReloadResp
{
    adi_adrv904x_DfeSvcErrCode_e cmdStatus;        /*!< Command status */
} adrv904x_DfeSvcCmdPrepareReloadResp_t;

/**
 * \brief SDK CPU and memory loading command structure
 */
typedef struct adrv904x_DfeSvcCmdCpuAndMemLoadGet
{
    uint32_t generic;                          /*!< Placeholder */
}   adrv904x_DfeSvcCmdCpuAndMemLoadGet_t;

/**
 * \brief SDK CPU loading command response structure
 */
typedef struct adrv904x_DfeSvcCmdCpuAndMemLoadGetResp
{
    adi_adrv904x_DfeSvcErrCode_e        status;            /*!< SDK error status code */
    adi_adrv904x_DfeCpuAndMemLoad_t load;              /*!< CPU Loading information */
} adrv904x_DfeSvcCmdCpuAndMemLoadGetResp_t;

/**
 * \brief SDK CPU loading interval set command structure
 */
typedef struct adrv904x_DfeSvcCmdCpuLoadIntervalSet
{
    uint32_t interval_ms;                      /*!< CPU Loading sampling interval */
}   adrv904x_DfeSvcCmdCpuLoadIntervalSet_t;

/**
 * \brief SDK CPU loading interval set command response structure
 */
typedef struct adrv904x_DfeSvcCmdCpuLoadIntervalSetResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                   /*!< SDK error status code */
} adrv904x_DfeSvcCmdCpuLoadIntervalSetResp_t;

/**
 * \brief SDK CPU loading interval get command structure
 */
typedef struct adrv904x_DfeSvcCmdCpuLoadIntervalGet
{
    uint32_t generic;                          /*!< Placeholder */
}   adrv904x_DfeSvcCmdCpuLoadIntervalGet_t;

/**
 * \brief SDK CPU loading interval get command response structure
 */
typedef struct adrv904x_DfeSvcCmdCpuLoadIntervalGetResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                   /*!< SDK error status code */
    uint32_t          interval_ms;              /*!< CPU Loading information */
} adrv904x_DfeSvcCmdCpuLoadIntervalGetResp_t;

/**
 * \brief SDK command Set Log Mask structure
 */
typedef struct adrv904x_DfeSvcCmdSetLogMaskCmd
{
    uint32_t logMask;                           /*!< Placeholder data field */
}   adrv904x_DfeSvcCmdSetLogMaskCmd_t;

/**
 * \brief LOG_MASK_GET command response structure
 */
typedef struct adrv904x_DfeSvcCmdGetLogMaskResp
{
    uint32_t          logMask; /*!< SDK log mask */
    adi_adrv904x_DfeSvcErrCode_e status;  /*!< Command status */
} adrv904x_DfeSvcCmdGetLogMaskResp_t;

/**
 * \brief SDK command Set Telemetry Mask structure
 */
typedef struct adrv904x_DfeSvcCmdSetTelemetryMaskCmd
{
    uint64_t sdkTelemetryMask0;                                     /* !<Bitmask to enable first 64 SDK telemetry events */
    uint64_t sdkTelemetryMask1;                                     /* !<Bitmask to enable second  64 SDK telemetry events */
    uint64_t appTelemetryMask0;                                     /* !<Bitmask to enable first 64 APP telemetry events */
    uint64_t appTelemetryMask1;                                     /* !<Bitmask to enable second  64 APP telemetry events */
}   adrv904x_DfeSvcCmdSetTelemetryMaskCmd_t;

/**
 * \brief SDK command Get Telemetry Mask response structure
 */
typedef struct adrv904x_DfeSvcCmdGetTelemetryMaskResp
{
    uint64_t          sdkTelemetryMask0; /* !<Bitmask to enable first 64 SDK telemetry events */
    uint64_t          sdkTelemetryMask1; /* !<Bitmask to enable second  64 SDK telemetry events */
    uint64_t          appTelemetryMask0; /* !<Bitmask to enable first 64 APP telemetry events */
    uint64_t          appTelemetryMask1; /* !<Bitmask to enable second  64 APP telemetry events */
    adi_adrv904x_DfeSvcErrCode_e status;            /*!< Command status */
} adrv904x_DfeSvcCmdGetTelemetryMaskResp_t;

/**
 * \brief SDK CPU loading command response structure
 */
typedef struct adrv904x_DfeSvcCmdWbRegBufferAccessResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                   /*!< SDK error status code */
    uint64_t          wbRegStartAddress;        /*!< start address for wb regularization buffer */
    uint32_t          wbRegBufferSize;          /*!< size of wb regularization buffer */
} adrv904x_DfeSvcCmdWbRegBufferAccessResp_t;

/**
 * \brief SDK ECC enable/disable set command structure
 */
typedef struct adrv904x_DfeSvcCmdEccEnableSet
{
    uint32_t eccEnableMemRegion;                /*!< bitfield of memory region(s) for ecc enable(1) or disable(0) */
}   adrv904x_DfeSvcCmdEccEnableSet_t;

/**
 * \brief SDK ECC enable/disable set command response structure
 */
typedef struct adrv904x_DfeSvcCmdEccEnableSetResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                     /*!< SDK error status code */
} adrv904x_DfeSvcCmdEccEnableSetResp_t;

/**
 * \brief SDK CPU loading command response structure
 */
typedef struct adrv904x_DfeSvcCmdEccEnableGetResp
{
    adi_adrv904x_DfeSvcErrCode_e status;                     /*!< SDK error status code */
    uint32_t          eccEnabledMemRegion;        /*!< bitfield of memory region(s) for ecc enabled(1) or disabled(0) */
} adrv904x_DfeSvcCmdEccEnableGetResp_t;

#endif /* __ADRV904X_DFE_SVC_CMD_T_H__ */



