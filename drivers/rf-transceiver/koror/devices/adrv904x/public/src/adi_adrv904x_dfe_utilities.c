/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_utilities.c
* \brief Contains ADI Transceiver CPU functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*        
* ADRV904X API Version: 2.10.0.4
*/

#include "adi_adrv904x_dfe_utilities.h"
#include "adi_adrv904x_dfe_cfr.h"
#include "adi_adrv904x_hal.h"
#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_rx.h"
#include "adi_library.h"

#include "../../private/include/adrv904x_dfe_svc_log_trace_t.h"
#include "../../private/include/adrv904x_dfe_svc_bbic_bridge_t.h"
#include "../../private/include/adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_radioctrl.h"
#include "../../private/bf/adrv904x_bf_sequencer.h"

#define ADI_FILE ADI_ADRV904X_FILE_PUBLIC_DFE_UTILITIES

/*****************************************************************************/
/***** Helper functions' prototypes ******************************************/
/*****************************************************************************/

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSdkVersionGet(adi_adrv904x_Device_t* const         device,
                                                               adi_adrv904x_CpuFwVersion_t* const   dfeSdkVersion)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t address = 0U;
    adi_adrv904x_DfeSvcBbicBridgeVersion_t ver = { 0, 0, 0, 0, 0 };
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfeSdkVersion, cleanup);
    
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_DFECPULOADED) != ADI_ADRV904X_STATE_DFECPULOADED)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE CPU bin must be loaded before getting SDK Lib version");
        goto cleanup;
    }
    
    /* Get dfe sdk version address from CPU */
    recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &address);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK LIB version address");
        goto cleanup;
    }
    
    address += ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t, sdkVersion);
    /* Read and Store the SDK lib version */
    recoveryAction = adi_adrv904x_Registers32Read(device, NULL, address, (uint32_t*)&ver, NULL, sizeof(adi_adrv904x_DfeSvcBbicBridgeVersion_t) / 4);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK Lib Version registers");
        goto cleanup;
    }

    dfeSdkVersion->commVer.majorVer       = adrv904x_CpuIntFromBytesGet((uint8_t*)&ver.majorRev, 4);
    dfeSdkVersion->commVer.minorVer       = adrv904x_CpuIntFromBytesGet((uint8_t*)&ver.minorRev, 4);
    dfeSdkVersion->commVer.maintenanceVer = adrv904x_CpuIntFromBytesGet((uint8_t*)&ver.maintRev, 4);
    dfeSdkVersion->commVer.buildVer       = adrv904x_CpuIntFromBytesGet((uint8_t*)&ver.buildNum, 4);
    dfeSdkVersion->cpuFwBuildType         = (adi_adrv904x_CpuFwBuildType_e)(adrv904x_CpuIntFromBytesGet((uint8_t*)&ver.buildType, 4) &
                                                                   (ADI_ADRV904X_CPU_FW_BUILD_DEBUG | ADI_ADRV904X_CPU_FW_BUILD_RELEASE | ADI_ADRV904X_CPU_FW_TRBLSHOOT));

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppVersionGet(adi_adrv904x_Device_t* const         device,
                                                               adi_adrv904x_CpuFwVersion_t* const   dfeAppVersion)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t address = 0U;
    adi_adrv904x_DfeSvcBbicBridgeVersion_t ver = { 0, 0, 0, 0, 0 };
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfeAppVersion, cleanup);
    
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_DFECPULOADED) != ADI_ADRV904X_STATE_DFECPULOADED)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE CPU bin must be loaded before getting SDK Lib version");
        goto cleanup;
    }

    /* Get dfe sdk version address from CPU */
    recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &address);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK LIB version address");
        goto cleanup;
    }
    
    address += ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t, appVersion);
    /* Read and Store the DFE app version */
    recoveryAction = adi_adrv904x_Registers32Read(device, NULL, address, (uint32_t*)&ver, NULL, sizeof(adi_adrv904x_DfeSvcBbicBridgeVersion_t) / 4);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK Lib Version registers");
        goto cleanup;
    }
    dfeAppVersion->commVer.majorVer       = adrv904x_CpuIntFromBytesGet((uint8_t*)&ver.majorRev, 4);
    dfeAppVersion->commVer.minorVer       = adrv904x_CpuIntFromBytesGet((uint8_t*)&ver.minorRev, 4);
    dfeAppVersion->commVer.maintenanceVer = adrv904x_CpuIntFromBytesGet((uint8_t*)&ver.maintRev, 4);
    dfeAppVersion->commVer.buildVer       = adrv904x_CpuIntFromBytesGet((uint8_t*)&ver.buildNum, 4);
    dfeAppVersion->cpuFwBuildType         = (adi_adrv904x_CpuFwBuildType_e)(adrv904x_CpuIntFromBytesGet((uint8_t*)&ver.buildType, 4) &
                                                                   (ADI_ADRV904X_CPU_FW_BUILD_DEBUG | ADI_ADRV904X_CPU_FW_BUILD_RELEASE | ADI_ADRV904X_CPU_FW_TRBLSHOOT));

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerErrorInfoGet(adi_adrv904x_Device_t* const                               device,
                                                                         adi_adrv904x_DfeSvcRadioCtrlSequencerErrorInfo_t* const    errorInfo)
{
       adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;
    const uint32_t RADIO_SEQUENCER_NUM_SEQUENCERS = 16U;
    const uint32_t RADIO_SEQUENCER_CONFIG_0_REG_OFFSET = 0x20U;
    const uint32_t BUS_ERROR_BITP = 20U;
    const uint32_t BUS_ERROR_BITM = 0x00100000U;
    const uint32_t FETCH_ERROR_BITP = 16U;
    const uint32_t FETCH_ERROR_BITM = 0x00010000U;
    uint8_t channelId = 0U;
    uint8_t regData = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, errorInfo, cleanup);

    ADI_LIBRARY_MEMSET(errorInfo, 0, sizeof(adi_adrv904x_DfeSvcRadioCtrlSequencerErrorInfo_t));
    
    /* Check that Radio Sequencer has been loaded before getting RS error info */
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_RSLOADED) != ADI_ADRV904X_STATE_RSLOADED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Radio Sequencer should have been loaded before getting RS error info");
        goto cleanup;
    }

    /* Get ssbSyncPhaseError */
    recoveryAction = adrv904x_Sequencer_SsbSyncPhaseError_BfGet(device,
                                                                NULL,
                                                                sequencerBaseAddr,
                                                                &errorInfo->ssbSyncPhaseError);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting radio sequencer SSB Sync Phase Error. ");
        goto cleanup;
    }

    /* Get busErrorMask and fetchErrorMask */
    for (channelId = 0; channelId < RADIO_SEQUENCER_NUM_SEQUENCERS; channelId++)
    {
        uint32_t regVal = 0;
        recoveryAction = adi_adrv904x_Register32Read(device,
                                                     NULL,
                                                     ((uint32_t) sequencerBaseAddr + RADIO_SEQUENCER_CONFIG_0_REG_OFFSET + channelId * 4),
                                                     &regVal,
                                                     0xFFFFFFFFU);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting radio sequencer Bus Error Mask. ");
            goto cleanup;
        }

        regData = (regVal & BUS_ERROR_BITM) >> BUS_ERROR_BITP;
        errorInfo->busErrorMask |= ((regData & 0x1U) << channelId);

        regData = (regVal & FETCH_ERROR_BITM) >> FETCH_ERROR_BITP;
        errorInfo->fetchErrorMask |= ((regData & 0x1U) << channelId);
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerSsbSyncPhaseErrorClear(adi_adrv904x_Device_t* const device)
{
	       adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;
    uint8_t bfValue;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Get tracking mode */
    recoveryAction = adrv904x_Sequencer_SsbSyncTrackingMode_BfGet(device,
                                                                  NULL,
                                                                  sequencerBaseAddr,
                                                                  &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting radio sequencer SSB SYNC Tracking Mode. ");
        goto cleanup;
    }

    /* Valid ssb_sync_tracking_mode value: 0, 1, 2 */
    if(bfValue == 0)
    {
        /* do nothing but return */
        goto cleanup;
    }

    if (bfValue > 3U)
    {
        /* Potential hardware error needs to be reported. */
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid bit field value for SSB SYNC Tracking Mode. ");
        goto cleanup;
    }

    /* ssb_sync_tracking_mode > 0 */
    /* Reset SSB_SYNC phase error (requre a 0-1-0 transition sequence) */
    bfValue = 0x00;
    recoveryAction = adrv904x_Sequencer_SsbSyncPhaseErrorClear_BfSet(device,
                                                                     NULL,
                                                                     sequencerBaseAddr,
                                                                     bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting radio sequencer SSB Sync Phase Error. ");
        goto cleanup;
    }

    bfValue = 0x01;
    recoveryAction = adrv904x_Sequencer_SsbSyncPhaseErrorClear_BfSet(device,
                                                                     NULL,
                                                                     sequencerBaseAddr,
                                                                     bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting radio sequencer SSB Sync Phase Error. ");
        goto cleanup;
    }

    bfValue = 0x00;
    recoveryAction = adrv904x_Sequencer_SsbSyncPhaseErrorClear_BfSet(device,
                                                                     NULL,
                                                                     sequencerBaseAddr,
                                                                     bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting radio sequencer SSB Sync Phase Error. ");
        goto cleanup;
    }
cleanup:
 	ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeLogTraceBufferGet(adi_adrv904x_Device_t* const             device,
                                                                   adi_adrv904x_DfeLogTraceBuffer_t* const  logTrace)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_CpuErrorCode_t cpuErrorCode = 0U;
    uint32_t address = 0U;
    uint32_t timeout_us = ADI_ADRV904X_LOGTRACE_TIMEOUT_US;
    uint8_t waitInterval_us = ADI_ADRV904X_LOGTRACE_INTERVAL_US;
    uint8_t waitCount = 0;
    
    uint32_t getDfeLogtraceData[ADI_ADRV904X_MAX_LOGTRACE_SIZE / sizeof(uint32_t)] = { 0 };
    uint8_t sortLogTraceData[ADI_ADRV904X_MAX_LOGTRACE_SIZE] = { 0 };
    
    adrv904x_DfeSvcCmdLogTraceBuffer_t logTraceCmd;
    adrv904x_DfeSvcCmdLogTraceBufferResp_t logTraceBufferResp;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    
    adrv904x_DfeSvcLogTraceBuffer_t logTraceBuffer = { 0, 0, 0, 0, 0, 0, 0, 0};
    adrv904x_DfeSvcLogTraceBuffer_t* pLogTraceBuffer;

    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, logTrace, cleanup);
    
    ADI_LIBRARY_MEMSET(logTrace, 0, sizeof(adi_adrv904x_DfeLogTraceBuffer_t));

    ADI_LIBRARY_MEMSET(&logTraceCmd, 0, sizeof(adrv904x_DfeSvcCmdLogTraceBuffer_t));
    
    ADI_LIBRARY_MEMSET(&logTraceBufferResp, 0, sizeof(adrv904x_DfeSvcCmdLogTraceBufferResp_t));
    
    recoveryAction = adrv904x_DfeSdkDataAddrGet(device, &address);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE SDK Data address");
        goto cleanup;
    }
    
    /*Get logTraceBuffer address*/
    address += ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcBbicBridgeSdkData_t, logDataBuffer);
        
    logTraceCmd.svcLogTraceBufferStatus = ADI_ADRV904X_DFE_SVC_LOGTRACE_BUSY;
    
    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_LOGTRACE_BUFFER,
                                            (void*)&logTraceCmd,
                                            sizeof(adrv904x_DfeSvcCmdLogTraceBuffer_t),
                                            (void*)&logTraceBufferResp,
                                            sizeof(adrv904x_DfeSvcCmdLogTraceBufferResp_t),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(logTraceBufferResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }
    
    /* Read the exception data */
    recoveryAction = adi_adrv904x_Registers32Read(device, NULL, address, (uint32_t*)&logTraceBuffer, NULL, sizeof(adrv904x_DfeSvcLogTraceBuffer_t) / sizeof(uint32_t));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception data registers");
        goto cleanup;
    }
    
    /* Read logbuffer start */
    logTrace->size = logTraceBuffer.size;
    logTrace->overFlowFlag = logTraceBuffer.overflowFlag;
    
    /* Read the exception data */
    recoveryAction = adi_adrv904x_Registers32Read(device, NULL, (uint32_t)logTraceBuffer.pData, (uint32_t*)&getDfeLogtraceData, NULL, logTraceBuffer.size / sizeof(uint32_t));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception data registers");
        goto cleanup;
    }
    
    do
    {
        if (waitCount < (timeout_us / waitInterval_us))
        {
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue");
                return recoveryAction;
            }
        }
        else
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "The logTrace buffer write timeout");
            return recoveryAction;
        }
        
        /* update logTraceBuffer */
        recoveryAction = adi_adrv904x_Registers32Read(device, NULL, address, (uint32_t*)&logTraceBuffer, NULL, sizeof(adrv904x_DfeSvcLogTraceBuffer_t) / sizeof(uint32_t));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading DFE exception data registers");
            goto cleanup;
        }
        
        /* waiting for the number of 100us*/
        waitCount++;
        
    } while (logTraceBuffer.writeFlag);
    
    for (unsigned int i = 0; i < (logTraceBuffer.size / sizeof(uint32_t)); i++)
    {
        for (unsigned int j = 0; j < sizeof(uint32_t); j++)
        {
            sortLogTraceData[j + i * 4] = (uint8_t)(getDfeLogtraceData[i] >> (j * 8) & 0xff);
        }
    }

    pLogTraceBuffer = &logTraceBuffer;

    /* Handle the log data*/
    if (ADI_ADRV904X_DFE_SVC_LOGTRACE_BUFFER_EMPTY(pLogTraceBuffer))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "The logTrace buffer is empty.");
        return recoveryAction;
    }
    else
    {
        /* sort logtrace to memory buffer */
        for (unsigned int i = 0; i < logTraceBuffer.size; i++)
        {
            if ((logTraceBuffer.rdIndex + i) < logTraceBuffer.size)
            {
                logTrace->logTraceData[i] = sortLogTraceData[logTraceBuffer.rdIndex + i];
            }
            else
            {
                logTrace->logTraceData[i] = sortLogTraceData[(logTraceBuffer.rdIndex + i) % logTraceBuffer.size];
            }
        }
    }
    
    /* Read logBuffer finish */
    logTraceCmd.svcLogTraceBufferStatus = ADI_ADRV904X_DFE_SVC_LOGTRACE_RELEASE;
    
    cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    /* Send command and receive response */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_LOGTRACE_BUFFER,
                                            /*waiting dfe ticket merge and change to new macro*/
                                            (void*)&logTraceCmd,
                                            sizeof(adrv904x_DfeSvcCmdLogTraceBuffer_t),
                                            (void*)&logTraceBufferResp,
                                            sizeof(adrv904x_DfeSvcCmdLogTraceBufferResp_t),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(logTraceBufferResp.cmdStatus, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCaptureTypeGet(adi_adrv904x_Device_t* const            device,
                                                                adi_adrv904x_DfeCaptureType_e* const    orx0CaptureType,
                                                                adi_adrv904x_DfeCaptureType_e* const    orx1CaptureType)
{
        static const uint8_t CAPTURE_TYPE_MASK   = 0x08U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t readbackVal = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, orx0CaptureType, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, orx1CaptureType, cleanup);

    /* Readback capture type for Orx0*/
    recoveryAction = adrv904x_Core_RadioControlInterfaceOrx0MapShadow_BfGet(device,
                                                                            NULL,
                                                                            (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                            &readbackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while retrieving current capture type for Orx0.");
        goto cleanup;
    }

    *orx0CaptureType = ((readbackVal & CAPTURE_TYPE_MASK) > 0U) ? ADI_ADRV904X_DFE_CAPTURE_TYPE_VSWR : ADI_ADRV904X_DFE_CAPTURE_TYPE_DPD;

    /* Readback capture type for Orx1*/
    recoveryAction = adrv904x_Core_RadioControlInterfaceOrx1MapShadow_BfGet(device,
                                                                            NULL,
                                                                            (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                            &readbackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while retrieving current capture type for Orx1.");
        goto cleanup;
    }

    *orx1CaptureType = ((readbackVal & CAPTURE_TYPE_MASK) > 0U) ? ADI_ADRV904X_DFE_CAPTURE_TYPE_VSWR : ADI_ADRV904X_DFE_CAPTURE_TYPE_DPD;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
