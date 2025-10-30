/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_cals.c
* \brief Contains Calibration features related function implementation defined in
* adi_adrv903x_cals.h
*
* ADRV903X API Version: 2.12.1.4
*/

#include "adi_adrv903x_cals.h"
#include "adi_adrv903x_error.h"
#include "adi_adrv903x_cpu.h"

#include "../../private/bf/adrv903x_bf_rx_funcs.h"
#include "../../private/include/adrv903x_rx.h"
#include "../../private/include/adrv903x_cpu.h"

#define ADI_FILE    ADI_ADRV903X_FILE_PUBLIC_CALS

/*****************************************************************************/
/***** Helper functions' definition ******************************************/
/*****************************************************************************/
static adi_adrv903x_ErrAction_e adrv903x_CalStatusGet(adi_adrv903x_Device_t* const    device,
                                                      const adrv903x_CpuObjectId_e    objId,
                                                      const adi_adrv903x_Channels_e   channel,
                                                      adi_adrv903x_CalStatus_t* const status);

static adi_adrv903x_ErrAction_e adrv903x_SendCalStatusCmd(adi_adrv903x_Device_t* const            device,
                                                          const adrv903x_CpuCmd_CalStatusType_e   type,
                                                          const adrv903x_CpuObjectId_e            calObjId,
                                                          const adi_adrv903x_Channels_e           channel);

static adi_adrv903x_ErrAction_e adrv903x_GetCalStatusCmdResp(adi_adrv903x_Device_t* const    device,
                                                             const adrv903x_CpuObjectId_e    objId,
                                                             const adi_adrv903x_Channels_e   channel,
                                                             const size_t                    calStatusSize,
                                                             void* const                     pRxBuf,
                                                             const void** const              pCalStatus);

static adrv903x_CpuObjectId_e adrv903x_InitCalToObjId(const adi_adrv903x_InitCalibrations_e calId);

static adrv903x_CpuObjectId_e adrv903x_TrackingCalToObjId(const adi_adrv903x_TrackingCalibrationMask_e calId);

static uint32_t adrv903x_ChanMaskToNum(const adi_adrv903x_Channels_e mask);

static uint32_t adrv903x_GetBitPosition(const uint32_t mask);

static adi_adrv903x_ErrAction_e adrv903x_VerifyChannel(const adi_adrv903x_Channels_e channel);


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitCalsRun(adi_adrv903x_Device_t* const            device,
                                                          const adi_adrv903x_InitCals_t* const    initCals)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_RunInit_t runInitCmd;
    adrv903x_CpuCmd_RunInitResp_t runInitCmdRsp;
    uint32_t cpuTypeIdx = 0U;
    adi_adrv903x_InitCalibrations_t validCalMask = ADI_ADRV903X_IC_ALL_CALS;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adi_adrv903x_CpuErrorCode_t cpuErrorCode = 0U;


    validCalMask |= ADI_ADRV903X_IC_RXSPUR;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, initCals, cleanup);
    ADI_LIBRARY_MEMSET(&runInitCmd, 0, sizeof(adrv903x_CpuCmd_RunInit_t));
    ADI_LIBRARY_MEMSET(&runInitCmdRsp, 0, sizeof(adrv903x_CpuCmd_RunInitResp_t));

    if (initCals->calMask == 0U)
    {
        /* Don't report an error as this is a valid value. Nothing to do since the calMask is 0 so return. */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        goto cleanup;
    }

    if ((initCals->calMask & ~validCalMask) != 0U)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid calMask provided.");
        goto cleanup;
    }

    if ((initCals->rxChannelMask == 0U) && (initCals->txChannelMask == 0U) && (initCals->orxChannelMask == 0U))
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "At least one channel mask must be non-zero.");
        goto cleanup;
    }

    if ((initCals->rxChannelMask & ~0xFFU) != 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, initCals->rxChannelMask, "Invalid rxChannelMask provided.");
        goto cleanup;
    }

    if ((initCals->txChannelMask & ~0xFFU) != 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, initCals->txChannelMask, "Invalid txChannelMask provided.");
        goto cleanup;
    }

    if ((initCals->orxChannelMask & ~0x3U) != 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, initCals->orxChannelMask, "Invalid orxChannelMask provided.");
        goto cleanup;
    }

    /* Fill out run init cmd params with user-provided params */
    runInitCmd.config.calMask = ADRV903X_HTOCLL(initCals->calMask);
    runInitCmd.config.rxChannelMask = ADRV903X_HTOCL(initCals->rxChannelMask);
    runInitCmd.config.txChannelMask = ADRV903X_HTOCL(initCals->txChannelMask);
    runInitCmd.config.orxChannelMask = ADRV903X_HTOCL(initCals->orxChannelMask);

    runInitCmd.config.warmBoot = initCals->warmBoot;


    runInitCmd.config.warmBoot = (initCals->warmBoot & (~ADI_ADRV903X_JRXREPAIR_INIT_ALL));

    if ((device->devStateInfo.devState & ADI_ADRV903X_STATE_STANDBY) == ADI_ADRV903X_STATE_STANDBY)
    {
        /* Serdes Initcal required for jesd link bring up */
        runInitCmd.config.calMask = ADRV903X_HTOCLL(initCals->calMask &
                                                    (ADI_ADRV903X_IC_SERDES |
                                                     ADI_ADRV903X_IC_ADC_RX |   /* Run RX ADC  */
                                                     ADI_ADRV903X_IC_ADC_ORX)); /* Run ORX ADC */

        /* Skip initcals during Standby Recover sequence */
        if (runInitCmd.config.calMask == 0U)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            goto cleanup;
        }
    }

    /* For each CPU, send the run init command, wait for a response, and process any errors. */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_RUN_INIT,
                                                (void*)&runInitCmd,
                                                sizeof(runInitCmd),
                                                (void*)&runInitCmdRsp,
                                                sizeof(runInitCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(runInitCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    }

    device->devStateInfo.devState = (adi_adrv903x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV903X_STATE_INITCALS_RUN);

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitCalsDetailedStatusGet(adi_adrv903x_Device_t* const        device,
                                                                        adi_adrv903x_InitCalStatus_t* const initStatus)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_RunInitGetDetailedStatusResp_t runInitStatusRsp = { { { 0U }, { 0U }, 0U, { 0U }, { 0U } } };
    uint32_t i = 0U;
    uint32_t cpuTypeIdx = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, initStatus, cleanup);
    ADI_LIBRARY_MEMSET(initStatus, 0, sizeof(adi_adrv903x_InitCalStatus_t));

    /* For each CPU send the run init detailed status cmd and process the response */
    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_RUN_INIT_GET_DETAILED_STATUS,
                                                NULL,
                                                0U,
                                                (void*)&runInitStatusRsp,
                                                sizeof(runInitStatusRsp),
                                                NULL);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_ERROR_INFO_GET_REPORT( ADI_ADRV903X_ERRSRC_CPU,
                                                ADI_ADRV903X_ERRCODE_CPU_CMD_RESPONSE,
                                                ADI_NO_VARIABLE,
                                                recoveryAction);
            goto cleanup;
        }

        /* Merge the CPU-specific response into the initStatus structure */

        /* Calibration duration is MAX(CPU0_duration, CPU1_duration). Select the longest duration here. */
        if (initStatus->calsDurationMsec < ADRV903X_CTOHL(runInitStatusRsp.status.calsDurationMsec))
        {
            initStatus->calsDurationMsec = ADRV903X_CTOHL(runInitStatusRsp.status.calsDurationMsec);
        }
 
        /* Go through each channel-specific status item in the response */
        for (i = 0U; i < ADI_ADRV903X_NUM_INIT_CAL_CHANNELS; ++i)
        {
            /* For channel-specific errors, we can only set one error code. If there are multiple
             * errors on this channel, we simply choose the first CPU's error. */
            if (initStatus->initErrCodes[i] == ADRV903X_CPU_NO_ERROR)
            {
                initStatus->initErrCodes[i] = ADRV903X_CTOHL(runInitStatusRsp.status.initErrCodes[i]);
            }
            
            /* Setup the channel-specific cal mask items.
             * Since these are channel masks, it is safe to simply
             * OR in the results from each CPU (a CPU that does not run a
             * particular cal will have a value of 0 here). */
            initStatus->initErrCals[i]      |= ADRV903X_CTOHLL(runInitStatusRsp.status.initErrCals[i]);
            initStatus->calsSincePowerUp[i] |= ADRV903X_CTOHLL(runInitStatusRsp.status.calsSincePowerUp[i]);
            initStatus->calsLastRun[i]      |= ADRV903X_CTOHLL(runInitStatusRsp.status.calsLastRun[i]);
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitCalsDetailedStatusGet_v2( adi_adrv903x_Device_t* const            device,
                                                                            adi_adrv903x_InitCalErrData_t* const    initCalErrData)
{
        adi_adrv903x_ErrAction_e apiRecoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_ErrAction_e calRecoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_RunInitGetDetailedStatusResp_t runInitStatusRsp = { { { 0U }, { 0U }, 0U, { 0U }, { 0U } } };
    adi_adrv903x_InitCalStatus_t initCalStatus;
    adi_adrv903x_ErrorInfo_t error = { 0, NULL, NULL, ADI_ADRV903X_ERR_ACT_NONE, NULL };
    uint32_t idx = 0U;
    uint32_t cpuTypeIdx = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, initCalErrData, cleanup);

    ADI_LIBRARY_MEMSET(initCalErrData, 0, sizeof(adi_adrv903x_InitCalErrData_t));

    ADI_LIBRARY_MEMSET(&initCalStatus, 0, sizeof(adi_adrv903x_InitCalStatus_t));

    /* For each CPU send the run init detailed status cmd and process the response */
    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        apiRecoveryAction = adrv903x_CpuCmdSend(device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_RUN_INIT_GET_DETAILED_STATUS,
                                                NULL,
                                                0U,
                                                (void*)&runInitStatusRsp,
                                                sizeof(runInitStatusRsp),
                                                NULL);
        if (apiRecoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_ERROR_INFO_GET_REPORT( ADI_ADRV903X_ERRSRC_CPU,
                                                ADI_ADRV903X_ERRCODE_CPU_CMD_RESPONSE,
                                                ADI_NO_VARIABLE,
                                                apiRecoveryAction);
            goto cleanup;
        }

        /* Merge the CPU-specific response into the initStatus structure */

        /* Go through each channel-specific status item in the response */
        for (idx = 0U; idx < ADI_ADRV903X_NUM_INIT_CAL_CHANNELS; ++idx)
        {
            /* For channel-specific errors, we can only set one error code. If there are multiple
             * errors on this channel, we simply choose the first CPU's error. */
            if (initCalStatus.initErrCodes[idx] == ADRV903X_CPU_NO_ERROR)
            {
                initCalStatus.initErrCodes[idx] = ADRV903X_CTOHL(runInitStatusRsp.status.initErrCodes[idx]);
            }
            
            /* Setup the channel-specific cal mask items.
             * Since these are channel masks, it is safe to simply
             * OR in the results from each CPU (a CPU that does not run a
             * particular cal will have a value of 0 here). */
            initCalStatus.initErrCals[idx]      |= ADRV903X_CTOHLL(runInitStatusRsp.status.initErrCals[idx]);
            initCalStatus.calsSincePowerUp[idx] |= ADRV903X_CTOHLL(runInitStatusRsp.status.calsSincePowerUp[idx]);
            initCalStatus.calsLastRun[idx]      |= ADRV903X_CTOHLL(runInitStatusRsp.status.calsLastRun[idx]);
        }

        /* Pass InitCals Duration on CPU Cores Directly to adi_adrv903x_InitCalErrData_t Structure */
        initCalErrData->calsDurationMsec[cpuTypeIdx] = ADRV903X_CTOHL(runInitStatusRsp.status.calsDurationMsec);
    }

    for (idx = 0U; idx < ADI_ADRV903X_NUM_INIT_CAL_CHANNELS; ++idx)
    {
        /* Error Detail Lookup */
        ADI_ADRV903X_ERROR_INFO_GET(ADI_ADRV903X_ERRSRC_CPU_RUNTIME,
                                    initCalStatus.initErrCodes[idx],
                                    ADI_NO_VARIABLE,
                                    calRecoveryAction,
                                    error);

        /* Populate Cal Error Information Structure */
        initCalErrData->channel[idx].initErrCals = initCalStatus.initErrCals[idx];
        initCalErrData->channel[idx].errCode = initCalStatus.initErrCodes[idx];
        (void) ADI_LIBRARY_STRNCPY((char*) &initCalErrData->channel[idx].errMsg[0U], error.errMsg, ADI_ADRV903X_CHAR_ARRAY_MAX);
        (void) ADI_LIBRARY_STRNCPY((char*) &initCalErrData->channel[idx].errCause[0U], error.errCause, ADI_ADRV903X_CHAR_ARRAY_MAX);
        initCalErrData->channel[idx].action = (int64_t) error.actionCode;
        (void) ADI_LIBRARY_STRNCPY((char*) &initCalErrData->channel[idx].actionMsg[0U], error.actionMsg, ADI_ADRV903X_CHAR_ARRAY_MAX);
        initCalErrData->channel[idx].calsSincePowerUp = initCalStatus.calsSincePowerUp[idx];
        initCalErrData->channel[idx].calsLastRun = initCalStatus.calsLastRun[idx];
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, apiRecoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitCalsWait( adi_adrv903x_Device_t* const    device,
                                                            const uint32_t                  timeoutMs)
{ 
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t areCalsRunning = 0U;
    uint32_t timeElapsedUs = 0U;
    uint32_t timeoutUs = timeoutMs * 1000U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    /* While timeoutMs has not expired, execute adi_adrv903x_InitCalsCheckCompleteGet() until completion (or timeout). */
    for (timeElapsedUs = 0U; timeElapsedUs < timeoutUs; timeElapsedUs += ADI_ADRV903X_INITCALSWAIT_INTERVAL_US)
    {
        recoveryAction = adi_adrv903x_InitCalsCheckCompleteGet(device, &areCalsRunning, ADI_TRUE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            /* Failure indicates one or more init cals failed (and init cals are now complete) */
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Init Cals Check Complete Issue");
            goto cleanup;
        }

        /* Break out here if init cals have completed */
        if (areCalsRunning == 0U)
        {
            break;
        }
 
        /* Init cals are still in progress. Wait the specified wait interval, then check again for status. */
        recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV903X_INITCALSWAIT_INTERVAL_US);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
            goto cleanup;
        }
    }

    /* Check for timeout */
    if (timeElapsedUs >= timeoutUs)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_FEATURE;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                timeoutUs,
                                "Initialization Calibrations Timeout");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitCalsWait_v2(  adi_adrv903x_Device_t* const            device,
                                                                const uint32_t                          timeoutMs,
                                                                adi_adrv903x_InitCalErrData_t* const    initCalErrData)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t areCalsRunning = 0U;
    uint32_t timeElapsedUs = 0U;
    uint32_t timeoutUs = timeoutMs * 1000U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    /* While timeoutMs has not expired, execute adi_adrv903x_InitCalsCheckCompleteGet() until completion (or timeout). */
    for (timeElapsedUs = 0U; timeElapsedUs < timeoutUs; timeElapsedUs += ADI_ADRV903X_INITCALSWAIT_INTERVAL_US)
    {
        /* Do Not Check Init Cals Status via this function */
        recoveryAction = adi_adrv903x_InitCalsCheckCompleteGet_v2(device, &areCalsRunning, initCalErrData);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Init Cals Check Complete Issue");
            goto cleanup;
        }

        /* Break out here if init cals have completed */
        if (areCalsRunning == 0U)
        {
            break;
        }
 
        /* Init cals are still in progress. Wait the specified wait interval, then check again for status. */
        recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV903X_INITCALSWAIT_INTERVAL_US);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
            goto cleanup;
        }
    }

    /* Check for timeout */
    if (timeElapsedUs >= timeoutUs)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_FEATURE;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                timeoutUs,
                                "Initialization Calibrations Timeout");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitCalsCheckCompleteGet(adi_adrv903x_Device_t* const device,
                                                                       uint8_t* const               areCalsRunning,
                                                                       const uint8_t                calErrorCheck)
{ 
        adi_adrv903x_ErrAction_e                            recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_RunInitGetCompletionStatusResp_t    runInitCompleteRsp;
    uint8_t                                             inProgress          = 0U;
    uint8_t                                             calSuccess          = 1U;
    uint32_t                                            cpuTypeIdx          = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, areCalsRunning, cleanup);
    ADI_LIBRARY_MEMSET(&runInitCompleteRsp, 0, sizeof(adrv903x_CpuCmd_RunInitGetCompletionStatusResp_t));
    
    *areCalsRunning = 0U;

    /* For each CPU, check the status of initial calibrations */
    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_RUN_INIT_GET_COMPLETION_STATUS,
                                                NULL,
                                                0U,
                                                (void*)&runInitCompleteRsp,
                                                sizeof(runInitCompleteRsp),
                                                NULL);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            /* Other Command Response Issue */
            ADI_ADRV903X_ERROR_INFO_GET_REPORT( ADI_ADRV903X_ERRSRC_CPU,
                                                ADI_ADRV903X_ERRCODE_CPU_CMD_RESPONSE,
                                                ADI_NO_VARIABLE,
                                                recoveryAction);
            goto cleanup;
        }

        /* Update the inProgress flag with this CPU's init cal status.
         * If any CPU is in progress, the global state is set to 'in progress' */
        inProgress |= runInitCompleteRsp.inProgress;

        /* Update the calSuccess flag with this CPU's init cal status.
         * Only if both CPUs are successful is the global status set to 'success' */
        calSuccess &= runInitCompleteRsp.success; 
    }

    /* Update status flags for caller. */
    if (inProgress == 1U)
    {
        /* We do not check calSuccess here because it is not valid
         * if inProgress == 1 (per interface spec)*/
        *areCalsRunning = 1U;
    } 
    else
    {
        *areCalsRunning = 0U;

        if (calErrorCheck == ADI_TRUE)
        {
            /* If cals failed, error here */
            if (calSuccess == 0U)
            {
                ADI_ADRV903X_ERROR_INFO_GET_REPORT( ADI_ADRV903X_ERRSRC_CALS,
                                                    ADI_ADRV903X_ERRCODE_CALS_INIT_ERROR_FLAG,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitCalsCheckCompleteGet_v2(  adi_adrv903x_Device_t* const            device,
                                                                            uint8_t* const                          areCalsRunning,
                                                                            adi_adrv903x_InitCalErrData_t* const    initCalErrData)
{
        adi_adrv903x_ErrAction_e                            recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_RunInitGetCompletionStatusResp_t    runInitCompleteRsp;
    uint8_t                                             inProgress          = 0U;
    uint32_t                                            cpuTypeIdx          = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, areCalsRunning, cleanup);
    ADI_LIBRARY_MEMSET(&runInitCompleteRsp, 0, sizeof(adrv903x_CpuCmd_RunInitGetCompletionStatusResp_t));

    *areCalsRunning = 0U;

    /* For each CPU, check the status of initial calibrations */
    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_RUN_INIT_GET_COMPLETION_STATUS,
                                                NULL,
                                                0U,
                                                (void*)&runInitCompleteRsp,
                                                sizeof(runInitCompleteRsp),
                                                NULL);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            /* Other Command Response Issue */
            ADI_ADRV903X_ERROR_INFO_GET_REPORT( ADI_ADRV903X_ERRSRC_CPU,
                                                ADI_ADRV903X_ERRCODE_CPU_CMD_RESPONSE,
                                                ADI_NO_VARIABLE,
                                                recoveryAction);
            goto cleanup;
        }

        /* Update the inProgress flag with this CPU's init cal status.
         * If any CPU is in progress, the global state is set to 'in progress'*/
        inProgress |= runInitCompleteRsp.inProgress;
    }

    /* Update status flags for caller. */
    if (inProgress == 1U)
    {
        /* We do not check calSuccess here because it is not valid
         * if inProgress == 1 (per interface spec)*/
        *areCalsRunning = 1U;
    } 
    else
    {
        *areCalsRunning = 0U;

        if (initCalErrData != NULL)
        {
            ADI_LIBRARY_MEMSET(initCalErrData, 0, sizeof(adi_adrv903x_InitCalErrData_t));

            /* Detailed Status As an Out Parameter is Optional but we'll report any error information into the error structure either way */
            recoveryAction = adi_adrv903x_InitCalsDetailedStatusGet_v2(device, initCalErrData);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Initial Calibration Detailed Status Get Issue");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitCalsAbort(adi_adrv903x_Device_t* const device)
{ 
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t cpuTypeIdx = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    /* For each CPU, send the abort command and verify the response */
    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_RUN_INIT_ABORT,
                                                NULL,
                                                0U,
                                                NULL,
                                                0U,
                                                NULL);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Run Init Abort Command Send Issue");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TrackingCalsEnableSet(adi_adrv903x_Device_t* const device,
                                                                    const adi_adrv903x_TrackingCalibrationMask_t calMask,
                                                                    const uint32_t channelMask,
                                                                    const adi_adrv903x_TrackingCalEnableDisable_e enableDisableFlag)
{ 
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t cpuTypeIdx = 0U;
    adrv903x_CpuCmd_SetEnabledTrackingCals_t setTrackingCalCmd;
    adrv903x_CpuCmd_SetEnabledTrackingCalsResp_t setTrackingCalCmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adi_adrv903x_CpuErrorCode_t cpuErrorCode = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_LIBRARY_MEMSET(&setTrackingCalCmd, 0, sizeof(adrv903x_CpuCmd_SetEnabledTrackingCals_t));
    ADI_LIBRARY_MEMSET(&setTrackingCalCmdRsp, 0, sizeof(adrv903x_CpuCmd_SetEnabledTrackingCalsResp_t));
    
    if ((enableDisableFlag != ADI_ADRV903X_TRACKING_CAL_DISABLE) && (enableDisableFlag != ADI_ADRV903X_TRACKING_CAL_ENABLE))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, enableDisableFlag, "Invalid enableDisableFlag provided.");
        goto cleanup;
    }

    if (calMask == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calMask, "Invalid calMask provided.");
        goto cleanup;
    }

    if ((calMask & ~ADI_ADRV903X_TC_ALL_CALS) != 0U)
    {

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calMask, "Invalid calMask provided.");
        goto cleanup;
    }

    /* channelMask == 0 is a valid input for system calibrations (e.g. SERDES cal), so it is not checked. */
    if ((channelMask & ~0xFFU) != 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask, "Invalid channelMask provided.");
        goto cleanup;
    }

    /* Build the command */
    setTrackingCalCmd.enableDisable = (adrv903x_TrackingCalEnableDisable_t)enableDisableFlag;
    setTrackingCalCmd.calMask       = (adi_adrv903x_TrackingCalibrationMask_t)ADRV903X_HTOCL(calMask);
    setTrackingCalCmd.channelMask   = ADRV903X_HTOCL(channelMask);

    /* For each CPU, send the set-tracking-cal-enable command and verify the response */
    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_SET_ENABLED_TRACKING_CALS,
                                                &setTrackingCalCmd,
                                                sizeof(setTrackingCalCmd),
                                                &setTrackingCalCmdRsp,
                                                sizeof(setTrackingCalCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(setTrackingCalCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TrackingCalsEnableSet_v2(adi_adrv903x_Device_t* const                     device,
                                                                       const adi_adrv903x_TrackingCalibrationMask_e     calMask,
                                                                       const adi_adrv903x_ChannelTrackingCals_t* const  channelMask,
                                                                       const adi_adrv903x_TrackingCalEnableDisable_e    enableDisableFlag)
{ 
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t cpuTypeIdx = 0U;
    adrv903x_CpuCmd_SetEnabledTrackingCals_v2_t setTrackingCalCmd;
    adrv903x_CpuCmd_SetEnabledTrackingCalsResp_t setTrackingCalCmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adi_adrv903x_CpuErrorCode_t cpuErrorCode = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, channelMask, cleanup);
    ADI_LIBRARY_MEMSET(&setTrackingCalCmd, 0, sizeof(adrv903x_CpuCmd_SetEnabledTrackingCals_t));
    ADI_LIBRARY_MEMSET(&setTrackingCalCmdRsp, 0, sizeof(adrv903x_CpuCmd_SetEnabledTrackingCalsResp_t));
    
    if ((enableDisableFlag != ADI_ADRV903X_TRACKING_CAL_DISABLE) && (enableDisableFlag != ADI_ADRV903X_TRACKING_CAL_ENABLE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, enableDisableFlag, "Invalid enableDisableFlag provided.");
        goto cleanup;
    }

    /* calMask cannot be zero nor having invalid bits */
    if ((calMask == 0U) ||
        ((calMask & ~ADI_ADRV903X_TC_ALL_CALS) != 0U))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calMask, "Invalid calMask provided.");
        goto cleanup;
    }

    /* channelMask == 0 is a valid input, check for invalid bits. */
    if ((channelMask->rxChannel & ~0xFFU) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask->rxChannel, "Invalid rxChannelMask provided.");
        goto cleanup;
    }

    /* txChannelMask == 0 is a valid input, check for invalid bits. */
    if ((channelMask->txChannel & ~0xFFU) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask->txChannel, "Invalid txChannelMask provided.");
        goto cleanup;
    }
    
    /* txChannelMask == 0 is a valid input, check for invalid bits. */
    if ((channelMask->orxChannel & ~0x03U) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask->orxChannel, "Invalid orxChannelMask provided.");
        goto cleanup;
    }
    
    /* laneSerdesMask == 0 is a valid input, check for invalid bits. */
    if ((channelMask->laneSerdes & ~0xFFU) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask->laneSerdes, "Invalid laneSerdesMask provided.");
        goto cleanup;
    }
    
    /* Build the command */
    setTrackingCalCmd.enableDisable = (adrv903x_TrackingCalEnableDisable_t)enableDisableFlag;
    setTrackingCalCmd.calMask       = (adi_adrv903x_TrackingCalibrationMask_t)ADRV903X_HTOCL(calMask);
    setTrackingCalCmd.rxChannel   = ADRV903X_HTOCL(channelMask->rxChannel);
    setTrackingCalCmd.orxChannel   = ADRV903X_HTOCL(channelMask->orxChannel);
    setTrackingCalCmd.txChannel   = ADRV903X_HTOCL(channelMask->txChannel);
    setTrackingCalCmd.laneSerdes   = ADRV903X_HTOCL(channelMask->laneSerdes);
 
    /* For each CPU, send the set-tracking-cal-enable command and verify the response */
    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                             (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                             ADRV903X_LINK_ID_0,
                                             ADRV903X_CPU_CMD_ID_SET_ENABLED_TRACKING_CALS_V2,
                                             &setTrackingCalCmd,
                                             sizeof(setTrackingCalCmd),
                                             &setTrackingCalCmdRsp,
                                             sizeof(setTrackingCalCmdRsp),
                                             &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(setTrackingCalCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TrackingCalsEnableGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_TrackingCalEnableMasks_t* const enableMasks)
{ 
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t cpuTypeIdx = 0U;
    uint32_t channelIdx = 0U;
    adrv903x_CpuCmd_GetEnabledTrackingCalsResp_t getTrackingCalCmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adi_adrv903x_CpuErrorCode_t cpuErrorCode = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, enableMasks, cleanup);
    ADI_LIBRARY_MEMSET(&getTrackingCalCmdRsp, 0, sizeof(adrv903x_CpuCmd_GetEnabledTrackingCalsResp_t));

    /* Initialize the caller's buffer */
    ADI_LIBRARY_MEMSET(enableMasks, 0, sizeof(*enableMasks));

    /* For each CPU, send the get-tracking-cal-enable command and verify the response */
    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_GET_ENABLED_TRACKING_CALS,
                                                NULL,
                                                0U,
                                                &getTrackingCalCmdRsp,
                                                sizeof(getTrackingCalCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(getTrackingCalCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        /* If the command was successful, OR the result with the existing result in enableMask.
         * Each CPU will only set the enable bit for a particular cal/channel if it "owns" that
         * channel.
         */
        for (channelIdx = 0U; channelIdx < ADI_ADRV903X_NUM_TRACKING_CAL_CHANNELS; channelIdx++)
        {
            enableMasks->enableMask[channelIdx] |= ADRV903X_CTOHL(getTrackingCalCmdRsp.enableMasks.enableMask[channelIdx]);
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TrackingCalAllStateGet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_TrackingCalState_t* const calState)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t cpuTypeIdx = 0U;
    uint32_t channelIdx = 0U;
    uint32_t calIdx = 0U;
    adrv903x_CpuCmd_GetTrackingCalStateResp_t getStateCmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adi_adrv903x_CpuErrorCode_t cpuErrorCode = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, calState, cleanup);

    /* Initialize the caller's buffer */
    ADI_LIBRARY_MEMSET(calState, 0, sizeof(*calState));

    ADI_LIBRARY_MEMSET(&getStateCmdRsp, 0, sizeof(adrv903x_CpuCmd_GetTrackingCalStateResp_t));

    /* For each CPU, send the get-tracking-cal-state command and verify the response */
    for (cpuTypeIdx = (uint32_t) ADI_ADRV903X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_GET_TRACKING_CAL_STATE,
                                                NULL,
                                                0U,
                                                &getStateCmdRsp,
                                                sizeof(getStateCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(getStateCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        /* If the command was successful, OR the result with the existing result in calState.
         * Each CPU will only set the state information for a particular cal/channel if it "owns" that
         * channel (otherwise it will be set to 0).
         */
        for (channelIdx = 0U; channelIdx < ADI_ADRV903X_NUM_TRACKING_CAL_CHANNELS; channelIdx++)
        {
            calState->calError[channelIdx] |= ADRV903X_CTOHL(getStateCmdRsp.calState.calError[channelIdx]);

            for (calIdx = 0U; calIdx < ADI_ADRV903X_TC_NUM_CALS; calIdx++)
            {
                calState->calState[channelIdx][calIdx] |= getStateCmdRsp.calState.calState[channelIdx][calIdx];
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_InitCalStatusGet(adi_adrv903x_Device_t* const            device,
                                                               const adi_adrv903x_InitCalibrations_e   calId,
                                                               const adi_adrv903x_Channels_e           channel,
                                                               adi_adrv903x_CalStatus_t* const         calStatus)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, calStatus, cleanup);

    if(((calId & (calId - 1U)) != 0U) ||/* Make sure only one bit is set*/
        (calId == 0U) ||/* No bit set */
        ((ADI_ADRV903X_IC_ALL_CALS & calId) != calId /* bit must be in ADI_ADRV903X_IC_ALL_CALS range */
        && (calId != ADI_ADRV903X_IC_RXSPUR)
        ))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calId, "Invalid calId provided.");
        goto cleanup;
    }
    
    recoveryAction = adrv903x_VerifyChannel(channel);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup;
    }

    /* Use the common cal status get handler to get the data from the CPU */
    recoveryAction = adrv903x_CalStatusGet(device, adrv903x_InitCalToObjId(calId), channel, calStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting init cal status.");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TrackingCalStatusGet(adi_adrv903x_Device_t* const                 device,
                                                                   const adi_adrv903x_TrackingCalibrationMask_e calId,
                                                                   const adi_adrv903x_Channels_e                channel,
                                                                   adi_adrv903x_CalStatus_t* const             calStatus)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, calStatus, cleanup);
    
    if (calId == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calId, "calId cannot be zero.");
        goto cleanup; 
    }

    recoveryAction = adrv903x_VerifyChannel(channel);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup;
    }

    /* Use the common cal status get handler to get the data from the CPU */
    recoveryAction = adrv903x_CalStatusGet(device, adrv903x_TrackingCalToObjId(calId), channel, calStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting tracking cal status.");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxHrmDataGet(adi_adrv903x_Device_t* const device,
                                                           const uint8_t channelMask,
                                                           adi_adrv903x_TxHrmData_t  txHrmDataArray[],
                                                           uint32_t arrayLength)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint8_t  i           = 0U;
    uint8_t  indexBitSet = 0U;
    uint32_t lengthResp  = 0U;
    uint8_t  numBitsSet  = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txHrmDataArray, cleanup);

    static const uint16_t CMD_READ_UP_HRM_COEFF = 3U;

    if (channelMask == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask, "channelMask cannot be zero.");
        goto cleanup;
    }

    for (i = 0U; i < ADI_ADRV903X_MAX_CHANNELS; i++)
    {
        if((channelMask & (1U << i)) == (1U << i))
        {
            ++numBitsSet;
        }
    }

    if (numBitsSet != arrayLength)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, arrayLength, "arrayLength must match number of bits high in channelMask");
        goto cleanup;
    }

    for (i = 0U; i < ADI_ADRV903X_MAX_CHANNELS; i++)
    {
        if ((channelMask & (1U << i)) == (1U << i))
        {
            recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                            ADRV903X_CPU_OBJID_IC_HRM,
                                                            CMD_READ_UP_HRM_COEFF,
                                                            (adi_adrv903x_Channels_e)(1U << i),
                                                            NULL,
                                                            0U,
                                                            &lengthResp,
                                                            (uint8_t *)&txHrmDataArray[indexBitSet],
                                                            48U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask, "Couldn't retrieve Hrm Data for this channelMask");
                goto cleanup;
            }
            ++indexBitSet;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxHrmDataSet(adi_adrv903x_Device_t* const device,
                                                           const uint8_t channelMask,
                                                           const adi_adrv903x_TxHrmData_t txHrmDataArray[],
                                                           uint32_t arrayLength)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint8_t  i           = 0U;
    uint8_t  indexBitSet = 0U;
    uint32_t lengthResp  = 0U;
    uint8_t  numBitsSet  = 0U;
    
    adi_adrv903x_TxHrmData_t txHrmDataArrayResp[8U];
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txHrmDataArray, cleanup);
    ADI_LIBRARY_MEMSET(txHrmDataArrayResp, 0U, 8U*sizeof(adi_adrv903x_TxHrmData_t));
    
    static const uint16_t CMD_APPLY_HRM_COEFF = 2U;

    if (channelMask == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask, "channelMask cannot be zero.");
        goto cleanup;
    }

    for (i = 0U; i < ADI_ADRV903X_MAX_CHANNELS; i++)
    {
        if ((channelMask & (1U << i)) == (1U << i))
        {
            ++numBitsSet;
        }
    }

    if (numBitsSet != arrayLength)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, arrayLength, "arrayLength must match number of bits high in channelMask");
        goto cleanup;
    }

    for (i = 0U; i < ADI_ADRV903X_MAX_CHANNELS; i++)
    {
        if ((channelMask & (1U << i)) == (1U << i))
        {
            recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                            ADRV903X_CPU_OBJID_IC_HRM,
                                                            CMD_APPLY_HRM_COEFF,
                                                            (adi_adrv903x_Channels_e)(1U << i),
                                                            (uint8_t*)&txHrmDataArray[indexBitSet],
                                                            48U,
                                                            &lengthResp,
                                                            (uint8_t*)&txHrmDataArrayResp[indexBitSet],
                                                            48U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask, "Couldn't retrieve Hrm Data for this channelMask");
                goto cleanup;
            }
            ++indexBitSet;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

static adi_adrv903x_ErrAction_e adrv903x_CalStatusGet(adi_adrv903x_Device_t* const    device,
                                                      const adrv903x_CpuObjectId_e    objId,
                                                      const adi_adrv903x_Channels_e   channel,
                                                      adi_adrv903x_CalStatus_t* const status)
{
    const adi_adrv903x_CalStatus_t* pCalStatus = NULL;
    const adi_adrv903x_CalStatus_t** const ppCalStatus = &pCalStatus;
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    char rxBuf[ADRV903X_CPU_CMD_RESP_MAX_SIZE_BYTES] = { 0 };

    /* Send the cal status get command to the CPU */
    recoveryAction = adrv903x_SendCalStatusCmd(device, ADRV903X_CPU_CMD_CAL_STATUS_COMMON, objId, channel);
    if (recoveryAction == ADI_ADRV903X_ERR_ACT_NONE)
    {
        /* Get the response from the CPU */
        recoveryAction = adrv903x_GetCalStatusCmdResp(device,
                                                      objId,
                                                      channel,
                                                      sizeof(adi_adrv903x_CalStatus_t),
                                                      (void*)&rxBuf[0],
                                                      (const void** const)ppCalStatus);
        if (recoveryAction == ADI_ADRV903X_ERR_ACT_NONE)
        {
            if (pCalStatus != NULL)
            {
                /* Translate the response from the CPU */
                status->errorCode = ADRV903X_CTOHL(pCalStatus->errorCode);
                status->percentComplete = ADRV903X_CTOHL(pCalStatus->percentComplete);
                status->performanceMetric = ADRV903X_CTOHL(pCalStatus->performanceMetric);
                status->iterCount = ADRV903X_CTOHL(pCalStatus->iterCount);
                status->updateCount = ADRV903X_CTOHL(pCalStatus->updateCount);
            }
        }
    }

    return recoveryAction;
}

static adi_adrv903x_ErrAction_e adrv903x_SendCalStatusCmd(adi_adrv903x_Device_t* const            device,
                                                          const adrv903x_CpuCmd_CalStatusType_e   type,
                                                          const adrv903x_CpuObjectId_e            calObjId,
                                                          const adi_adrv903x_Channels_e           channel)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    char txBuf[ADRV903X_CPU_CMD_MAX_SIZE_BYTES];
    adrv903x_CpuCmd_t* pCmd = NULL;
    adrv903x_CpuCmd_GetCalStatus_t* pCalStatusCmd = NULL;
    adi_adrv903x_CpuType_e cpuType = ADI_ADRV903X_CPU_TYPE_UNKNOWN;

    /* Get the CPU that is responsible for the requested channel */
    recoveryAction = adrv903x_CpuChannelMappingGet(device, channel, calObjId, &cpuType);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid channel-to-CPU mapping");
        return recoveryAction;
    }

    /* Build the CPU command */
    pCmd = (adrv903x_CpuCmd_t*)txBuf;
    pCalStatusCmd = (adrv903x_CpuCmd_GetCalStatus_t*)((uint8_t*)pCmd + sizeof(adrv903x_CpuCmd_t));
    pCalStatusCmd->type = (adrv903x_CpuCmd_CalStatusType_t) type;
    pCalStatusCmd->calObjId = ADRV903X_HTOCL(calObjId);
    pCalStatusCmd->channelNum = ADRV903X_HTOCL(adrv903x_ChanMaskToNum(channel));

    /* Then send it */
    recoveryAction = adrv903x_CpuCmdWrite(device, cpuType, ADRV903X_LINK_ID_0, ADRV903X_CPU_CMD_ID_GET_CAL_STATUS, pCmd, sizeof(adrv903x_CpuCmd_GetCalStatus_t));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error sending GET_CAL_STATUS command.");
    }
    
    return recoveryAction;
}

static adi_adrv903x_ErrAction_e adrv903x_GetCalStatusCmdResp(adi_adrv903x_Device_t* const    device,
                                                             const adrv903x_CpuObjectId_e    objId,
                                                             const adi_adrv903x_Channels_e   channel,
                                                             const size_t                    calStatusSize,
                                                             void* const                     pRxBuf,
                                                             const void** const              pCalStatus)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmdId_t cmdId = 0U;
    adrv903x_CpuCmdResp_t* pCmd = (adrv903x_CpuCmdResp_t*)pRxBuf;
    adrv903x_CpuCmd_GetCalStatusResp_t* pCmdResp = (adrv903x_CpuCmd_GetCalStatusResp_t*)((uint8_t*)pCmd + sizeof(adrv903x_CpuCmdResp_t));
    adi_adrv903x_CpuType_e cpuType = ADI_ADRV903X_CPU_TYPE_UNKNOWN;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cmdErrorCode = ADRV903X_CPU_NO_ERROR;

    /* Get the CPU that is responsible for the requested channel */
    recoveryAction = adrv903x_CpuChannelMappingGet(device, channel, objId, &cpuType);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid channel-to-CPU mapping");
        return recoveryAction;
    }

    /* Read the response from the CPU */
    recoveryAction = adrv903x_CpuCmdRespRead(device, cpuType, ADRV903X_LINK_ID_0, &cmdId, pCmd, sizeof(adrv903x_CpuCmd_GetCalStatusResp_t) + calStatusSize, &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        if (cmdStatus == ADRV903X_CPU_CMD_STATUS_CMD_FAILED)
        {
            /* If the command failed for a command-specific reason, extract the command status code and log the error. */
            cmdErrorCode = (adrv903x_CpuErrorCode_e)ADRV903X_CTOHL((uint32_t)pCmdResp->cmdStatus);
            ADI_ERROR_REPORT(&device->common, ADI_ADRV903X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, cmdErrorCode, "GET_CAL_STATUS command failed.");
            return recoveryAction;
        }
        else
        {
            /* Otherwise log a generic command failed error */
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting response for GET_CAL_STATUS command.");
            return recoveryAction;
        }
    }

    /* Find the cal status in the response payload, and set the caller's pointer to it. */
    *pCalStatus = (void*)((uint8_t*)pCmdResp + sizeof(adrv903x_CpuCmd_GetCalStatusResp_t));

    return recoveryAction;
}

static adrv903x_CpuObjectId_e adrv903x_InitCalToObjId(const adi_adrv903x_InitCalibrations_e calId)
{
    return (adrv903x_CpuObjectId_e)adrv903x_GetBitPosition((uint32_t)calId);
}

static adrv903x_CpuObjectId_e adrv903x_TrackingCalToObjId(const adi_adrv903x_TrackingCalibrationMask_e calId)
{
    adrv903x_CpuObjectId_t objId = 0U;

    objId = adrv903x_GetBitPosition((uint32_t)calId);
    objId = ADRV903X_CPU_TRACKING_CAL_IDX_TO_OBJ(objId);

    return (adrv903x_CpuObjectId_e)objId;
}

static uint32_t adrv903x_ChanMaskToNum(const adi_adrv903x_Channels_e mask)
{
    return adrv903x_GetBitPosition((uint32_t)mask);
}

static uint32_t adrv903x_GetBitPosition(const uint32_t mask)
{
    uint32_t bitPos = 0U;
 
    if (mask != 0U)
    {
        for (bitPos = 0U; ((1U << bitPos) & mask) == 0U; bitPos++)
        {
        }
    }

    return bitPos;
}

static adi_adrv903x_ErrAction_e adrv903x_VerifyChannel(const adi_adrv903x_Channels_e channel)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Channel must not be set to OFF */
    if (channel != ADI_ADRV903X_CHOFF)
    {
        /* Channel must contain a single channel number (0-7) */
        if (channel == ADI_ADRV903X_CH0 ||
            channel == ADI_ADRV903X_CH1 ||
            channel == ADI_ADRV903X_CH2 ||
            channel == ADI_ADRV903X_CH3 ||
            channel == ADI_ADRV903X_CH4 ||
            channel == ADI_ADRV903X_CH5 ||
            channel == ADI_ADRV903X_CH6 ||
            channel == ADI_ADRV903X_CH7)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CalPvtStatusGet(adi_adrv903x_Device_t* const device,
                                                              const adi_adrv903x_Channels_e channel,
                                                              const uint32_t objId,
                                                              uint8_t  calStatusGet[],
                                                              uint32_t length)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    const void* pCalStatus = NULL;
    const void** const ppCalStatus = &pCalStatus;
    char rxBuf[ADRV903X_CPU_CMD_RESP_MAX_SIZE_BYTES];

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, calStatusGet, cleanup);

    recoveryAction = adrv903x_VerifyChannel(channel);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup; 
    }

    if (((objId > ADRV903X_CPU_OBJID_IC_END) &&
         (objId < ADRV903X_CPU_OBJID_TC_START)) ||
        (objId > ADRV903X_CPU_OBJID_TC_END))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, length, "objId parameter is invalid.");
        goto cleanup;
    }
    
    if (length == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, length, "Length is zero.");
        goto cleanup;
    }

    if ((length + sizeof(adrv903x_CpuCmd_GetCalStatusResp_t) + sizeof(adrv903x_CpuCmdResp_t)) > sizeof(rxBuf))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, length, "Length exceeds maximum response size.");
        goto cleanup;
    }

    /* Send the cal status get command to the CPU */
    recoveryAction = adrv903x_SendCalStatusCmd(device, ADRV903X_CPU_CMD_CAL_STATUS_PRIVATE, (adrv903x_CpuObjectId_e)objId, channel);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SendCalStatusCmd failed");
        goto cleanup; 
    }

    /* Get the response from the CPU */
    recoveryAction = adrv903x_GetCalStatusCmdResp(device,
                                                  (adrv903x_CpuObjectId_e)objId,
                                                  channel,
                                                  length,
                                                  (void*)&rxBuf[0],
                                                  ppCalStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GetCalStatusCmd failed");
        goto cleanup; 
    }

    if (pCalStatus == NULL)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GET_CAL_STATUS command failed");
        goto cleanup; 
    }

    ADI_LIBRARY_MEMCPY(calStatusGet, pCalStatus, length);

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_CalSpecificStatusGet(adi_adrv903x_Device_t* const device,
                                                                   const adi_adrv903x_Channels_e channel,
                                                                   const uint32_t objId,
                                                                   uint8_t  calStatusGet[],
                                                                   uint32_t length)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    const void* pCalStatus = NULL;
    const void** const ppCalStatus = &pCalStatus;
    char rxBuf[ADRV903X_CPU_CMD_RESP_MAX_SIZE_BYTES];

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, calStatusGet, cleanup);

    recoveryAction = adrv903x_VerifyChannel(channel);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        goto cleanup; 
    }

    if (((objId > ADRV903X_CPU_OBJID_IC_END) &&
         (objId < ADRV903X_CPU_OBJID_TC_START)) ||
        (objId > ADRV903X_CPU_OBJID_TC_END))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, length, "objId parameter is invalid.");
        goto cleanup;
    }
    
    if (length == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, length, "Length is zero.");
        goto cleanup;
    }

    if ((length + sizeof(adrv903x_CpuCmd_GetCalStatusResp_t) + sizeof(adrv903x_CpuCmdResp_t)) > sizeof(rxBuf))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, length, "Length exceeds maximum response size.");
        goto cleanup;
    }

    /* Send the cal status get command to the CPU */
    recoveryAction = adrv903x_SendCalStatusCmd(device, ADRV903X_CPU_CMD_CAL_STATUS_SPECIFIC, (adrv903x_CpuObjectId_e)objId, channel);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SendCalStatusCmd failed");
        goto cleanup; 
    }

    /* Get the response from the CPU */
    recoveryAction = adrv903x_GetCalStatusCmdResp(device,
                                                  (adrv903x_CpuObjectId_e)objId,
                                                  channel,
                                                  length,
                                                  (void*)&rxBuf[0],
                                                  ppCalStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GetCalStatusCmd failed");
        goto cleanup; 
    }  

    if (pCalStatus == NULL)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GET_CAL_STATUS command failed");
        goto cleanup; 
    }

    ADI_LIBRARY_MEMCPY(calStatusGet, pCalStatus, length);

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DigDcOffsetEnableSet(adi_adrv903x_Device_t* const device,
                                                                   const uint32_t rxChannelMask,
                                                                   const uint32_t rxChannelEnableDisable)
{ 
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxFuncsChanAddr_e baseAddr = ADRV903X_BF_SLICE_RX_0__RX_FUNCS;
    adi_adrv903x_RxChannels_e rxChannel =  ADI_ADRV903X_RXOFF;
    uint32_t i = 0U;
    uint8_t bfValue = ADI_DISABLE;
    uint32_t chanSel = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);

    /* Validate rxChannelMask */
    if (rxChannelMask > 0xFFU)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "Invalid channelDisableEnableMask");
        goto cleanup;
    }

    for (i = 0U; i < ADI_ADRV903X_MAX_RX_ONLY; ++i)
    {
        chanSel = 1U << i;
        rxChannel = (adi_adrv903x_RxChannels_e)(chanSel);

        if ((rxChannel & rxChannelMask) != rxChannel)
        {
            /* Channel is not in mask. Skip it. */
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            continue;
        }

        /* Get Rx Funcs Bitfield Address */
        recoveryAction = adrv903x_RxFuncsBitfieldAddressGet(device,
                                                            rxChannel,
                                                            &baseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx Funcs base address");
            goto cleanup;
        }

        if ((rxChannelEnableDisable & (uint32_t) rxChannel) == rxChannel)
        {
            bfValue = ADI_ENABLE;
        }
        else
        {
            bfValue = ADI_DISABLE;
        }

        recoveryAction = adrv903x_RxFuncs_BbdcTrackingEnable_BfSet(device,
                                                                   NULL,
                                                                   baseAddr,
                                                                   bfValue);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "BbdcTrackingEnable Bf Set failed.");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DigDcOffsetEnableGet(adi_adrv903x_Device_t* const device,
                                                                   const adi_adrv903x_RxChannels_e rxChannel,
                                                                   uint8_t* const isEnabled)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;       
    adrv903x_BfRxFuncsChanAddr_e baseAddr = ADRV903X_BF_SLICE_RX_0__RX_FUNCS;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, isEnabled, cleanup);

    /* Get Rx Funcs Bitfield Address */
    recoveryAction = adrv903x_RxFuncsBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &baseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx Funcs base address");
        goto cleanup;
    }

    recoveryAction = adrv903x_RxFuncs_BbdcTrackingEnable_BfGet(device,
                                                               NULL,
                                                               baseAddr,
                                                               isEnabled);
    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "BbdcTrackingEnable Bf Get failed.");
        goto cleanup; 
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxLolReset(adi_adrv903x_Device_t* const           device,
                                                         const adi_adrv903x_TxLolReset_t* const txLolReset)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t                 i              = 0U;
    uint32_t                 txChannel      = 0U;
    uint32_t                 lengthResp     = 0U;
    uint8_t                  ctrlResp[1]    = { 0U };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txLolReset, cleanup);

    /* Tx channel mask range check */
    if (txLolReset->channelMask >  (uint32_t) ADI_ADRV903X_TXALL)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txLolReset->channelMask, "Invalid Tx LOL Channel Mask");
        goto cleanup;
    }

    if ((txLolReset->resetType != ADI_ADRV903X_TX_LOL_SOFT_RESET) &&
        (txLolReset->resetType != ADI_ADRV903X_TX_LOL_HARD_RESET))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txLolReset->resetType, "Invalid Tx LOL Reset Type");
        goto cleanup;
    }

    for (i = 0U; i < ADI_ADRV903X_MAX_TXCHANNELS; ++i)
    {
        txChannel = (1U << i);

        if ((txLolReset->channelMask & txChannel) == txChannel)
        {
            /* Sending cpu control command with length of ctrlData set to 0 and ctrlData set to NULL.
             * This way the default firmware response will never be overridden by this API function 
             * and no assumption can be made about firmware response.
             */
            recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                            (uint32_t)ADRV903X_CPU_OBJID_TC_TX_LOL,
                                                            (uint16_t)(txLolReset->resetType),
                                                            (adi_adrv903x_Channels_e)txChannel,
                                                            NULL,
                                                            0U, 
                                                            &lengthResp,
                                                            ctrlResp,
                                                            1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Tx LOL Reset Issue");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxQecReset(adi_adrv903x_Device_t* const           device,
                                                         const adi_adrv903x_TxQecReset_t* const txQecReset)
{
        adi_adrv903x_ErrAction_e recoveryAction     = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    const uint16_t           TXQEC_CMD_RESET    = 5U;
    uint32_t                 i                  = 0U;
    uint32_t                 txChannel          = 0U;
    uint32_t                 lengthResp         = 0U;
    uint8_t                  ctrlData[1]        = { 0U };
    uint8_t                  ctrlResp[1]        = { 0U };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txQecReset, cleanup);

    /* Tx channel mask range check */
    if (txQecReset->channelMask >  (uint32_t) ADI_ADRV903X_TXALL)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txQecReset->channelMask, "Invalid Tx QEC Channel Mask");
        goto cleanup;
    }

    if ((txQecReset->resetType != ADI_ADRV903X_TX_QEC_TRACKING_HARD_RESET)      &&
        (txQecReset->resetType != ADI_ADRV903X_TX_QEC_TRACKING_QEC_RESET)       &&
        (txQecReset->resetType != ADI_ADRV903X_TX_QEC_TRACKING_CHANNEL_RESET))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txQecReset->resetType, "Invalid Tx QEC Reset Type");
        goto cleanup;
    }

    ctrlData[0U] = txQecReset->resetType;

    for (i = 0U; i < ADI_ADRV903X_MAX_TXCHANNELS; ++i)
    {
        txChannel = (1U << i);

        if ((txQecReset->channelMask & txChannel) == txChannel)
        {
            recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                            (uint32_t)ADRV903X_CPU_OBJID_TC_TXQEC,
                                                            TXQEC_CMD_RESET,
                                                            (adi_adrv903x_Channels_e)txChannel,
                                                            ctrlData,
                                                            1U,
                                                            &lengthResp,
                                                            ctrlResp,
                                                            1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Tx QEC Reset Issue");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DigDcOffsetCfgSet(adi_adrv903x_Device_t* const                    device,
                                                                const adi_adrv903x_CpuCmd_SetDcOffset_t* const  dcOffSetCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_CpuCmd_SetDcOffset_t dcOffsetCfgToWrite;
    adi_adrv903x_CpuCmd_SetDcOffsetResp_t cmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adi_adrv903x_CpuErrorCode_t cpuErrorCode = 0U;
    static const uint32_t MAX_CORNER_FREQ = 11513U;
    static const uint32_t MIN_CORNER_FREQ = 3U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, dcOffSetCfg, cleanup);

    ADI_LIBRARY_MEMSET(&dcOffsetCfgToWrite, 0, sizeof(adi_adrv903x_CpuCmd_SetDcOffset_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adi_adrv903x_CpuCmd_SetDcOffsetResp_t));
    
    if (dcOffSetCfg->chanSelect == ADI_ADRV903X_RXOFF)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dcOffSetCfg->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    if (dcOffSetCfg->multEnb > 1U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dcOffSetCfg->multEnb, "Invalid enable selection. Valid values are 0 and 1");
        goto cleanup;
    }

    if ((dcOffSetCfg->dcFilterBw_kHz > MAX_CORNER_FREQ) ||
        (dcOffSetCfg->dcFilterBw_kHz < MIN_CORNER_FREQ))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dcOffSetCfg->dcFilterBw_kHz, "Invalid corner frequency. Valid range [3-11513 kHz]");
        goto cleanup;
    }

    /* Prepare the command payload */
    dcOffsetCfgToWrite.chanSelect = dcOffSetCfg->chanSelect;
    dcOffsetCfgToWrite.dcFilterBw_kHz = ADRV903X_HTOCL(dcOffSetCfg->dcFilterBw_kHz);
    dcOffsetCfgToWrite.multEnb = dcOffSetCfg->multEnb;

    /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend(   device,
                                            ADI_ADRV903X_CPU_TYPE_0,
                                            ADRV903X_LINK_ID_0,
                                            ADRV903X_CPU_CMD_ID_SET_DCOFFSET,
                                            (void*)&dcOffsetCfgToWrite,
                                            sizeof(dcOffsetCfgToWrite),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DigDcOffsetCfgGet(adi_adrv903x_Device_t* const                    device,
                                                                adi_adrv903x_CpuCmd_GetDcOffsetResp_t* const    dcOffSetCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_CpuCmd_GetDcOffsetResp_t cmdRsp;
    adi_adrv903x_CpuCmd_GetDcOffset_t getCmd;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adi_adrv903x_CpuErrorCode_t cpuErrorCode = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, dcOffSetCfg, cleanup);
    
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adi_adrv903x_CpuCmd_GetDcOffsetResp_t));
    ADI_LIBRARY_MEMSET(&getCmd, 0, sizeof(adi_adrv903x_CpuCmd_GetDcOffset_t));

    if ((dcOffSetCfg->chanSelect != (uint8_t)ADI_ADRV903X_RX0) && 
        (dcOffSetCfg->chanSelect != (uint8_t)ADI_ADRV903X_RX1) && 
        (dcOffSetCfg->chanSelect != (uint8_t)ADI_ADRV903X_RX2) && 
        (dcOffSetCfg->chanSelect != (uint8_t)ADI_ADRV903X_RX3) && 
        (dcOffSetCfg->chanSelect != (uint8_t)ADI_ADRV903X_RX4) && 
        (dcOffSetCfg->chanSelect != (uint8_t)ADI_ADRV903X_RX5) && 
        (dcOffSetCfg->chanSelect != (uint8_t)ADI_ADRV903X_RX6) && 
        (dcOffSetCfg->chanSelect != (uint8_t)ADI_ADRV903X_RX7))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, dcOffSetCfg->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    getCmd.chanSelect = dcOffSetCfg->chanSelect;

    /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend(   device,
                                            ADI_ADRV903X_CPU_TYPE_0,
                                            ADRV903X_LINK_ID_0,
                                            ADRV903X_CPU_CMD_ID_GET_DCOFFSET,
                                            (void*)&getCmd,
                                            sizeof(getCmd),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Extract the command-specific response from the response payload */
    dcOffSetCfg->chanSelect  = cmdRsp.chanSelect;
    dcOffSetCfg->mShift = ADRV903X_CTOHL(cmdRsp.mShift);
    dcOffSetCfg->multEnb  = cmdRsp.multEnb;
    dcOffSetCfg->mult = ADRV903X_CTOHL(cmdRsp.mult);

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}
