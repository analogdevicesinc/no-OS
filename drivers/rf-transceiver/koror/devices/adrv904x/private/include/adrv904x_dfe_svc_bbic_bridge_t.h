/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv904x_dfe_svc_bbic_bridge_t.h
 * \brief Contains data types for DFE SDK data information
 *
 * DFE SDK API Version: 2.10.0.4
 */

#ifndef __ADRV904X_DFE_SVC_BBIC_BRIDGE_T_H__
#define __ADRV904X_DFE_SVC_BBIC_BRIDGE_T_H__

#include <stdint.h>
#include "adi_adrv904x_dfe_svc_bbic_bridge_version_t.h"
#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_dfe_svc_cmd_t.h"
#include "adrv904x_dfe_svc_cmd_force_exception.h"
#include "adi_adrv904x_dfe_svc_err_codes_t.h"
#include "adrv904x_dfe_svc_log_trace_t.h"
#include "adrv904x_dfe_osal_util_t.h"
#include "adi_adrv904x_dfe_svc_dfe_capture_cmn_t.h"

#define SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE    (10744)
#define ADI_NUM_CAPTURE_BUFFERS_MAX              64
/**
 * \brief DFE mailbox data area
 */
typedef struct
{
    uint8_t buffer[SVC_BBIC_BRIDGE_MAX_MAILBOX_LINK_SIZE];      /*! mailbox data buffer (both directions) */
} _adi_DfeMailboxBuffer_t;


/**
 * \brief DFE capture data information
 */
typedef struct
{
    uint32_t                 txChannelNo;
    void                     * pCaptureBufferAddr;
    adi_adrv904x_DfeSvcDfeCaptureType_e capType;
}adrv904x_DfeSvcCaptureBufferData_t;

/**
 * \brief DFE SDK data information
 */

ADI_ADRV904X_PACK_START
typedef struct
{
    adi_adrv904x_DfeSvcBbicBridgeVersion_t      sdkVersion;                                 /*!< SDK version number */
    adi_adrv904x_DfeSvcBbicBridgeVersion_t      appVersion;                                 /*!< application version number */
    adrv904x_DfeSvcBbicBridgeSdkRevision_t sdkRevision;                                /*!< SDK revision information */
    _adi_DfeMailboxBuffer_t          dfeMailboxBuffer;                           /*!< DFE mailbox data buffer */
    adrv904x_DfeSvcCmdDfeExceptionData_t   dfeExceptionSnapshotData;                   /*!< DFE exception handler snapshot data */
    adi_adrv904x_DfeSvcLogTraceErrLog_t         errLogData;                                 /*!< DFE err log data */
    adrv904x_DfeSvcCmdCtrlLog_t            cmdLogData;                                 /*!< DFE command log data */
    uint32_t                         appInitStatus;                              /*!< Application initialization status */
    uint32_t                         dfeBootError;                               /*!< DFE boot error code */
    uint32_t                         vswrAlarm;                                  /*!< DFE VSWR: bitmask of TX channels with active VSWR alarm */
    adrv904x_DfeSvcLogTraceBuffer_t        logDataBuffer;                              /*!< DFE uart log data */
    adrv904x_DfeOsalUtilLogData_t          osalLogData;                                /*!< DFE osal log data */
    adrv904x_DfeSvcCaptureBufferData_t     captureBuffer[ADI_NUM_CAPTURE_BUFFERS_MAX]; /*!< DFE capture buffer data*/
    adrv904x_DfeSvcCaptureBufferData_t     accumBuffer[ADI_NUM_CAPTURE_BUFFERS_MAX];   /*!< DFE accumulation buffer data*/
} adrv904x_DfeSvcBbicBridgeSdkData_t;
ADI_ADRV904X_PACK_FINISH


/**
 *  \brief DFE SDK Boot Flag enumerations
 */
typedef enum adrv904x_DfeSvcBbicBridgeSdkBootFlag
{
    ADRV904X_DFE_SVC_BBIC_BRIDGE_SDK_BOOT_FLAG_DEFAULT           = 0x00u,               /*!< SDK Default boot flag */
    ADRV904X_DFE_SVC_BBIC_BRIDGE_SDK_BOOT_FLAG_BBIC_RELOAD       = 0x01u,               /*!< SDK Reload from BBIC flag */
    ADRV904X_DFE_SVC_BBIC_BRIDGE_SDK_BOOT_FLAG_DEBUG_JTAG_ENABLE = 0x55u,               /*!< SDK JTAG Debug enable boot flag */
} adrv904x_DfeSvcBbicBridgeSdkBootFlag_e;

/** \brief DFE SDK Boot Flag type */
typedef uint8_t adrv904x_DfeSvcBbicBridgeSdkBootFlag_t;



#endif /* __ADRV904X_DFE_SVC_BBIC_BRIDGE_T_H__ */


