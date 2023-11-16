/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_vswr.c
* \brief Contains VSWR feature related function implementations
*
* ADRV904X API Version: 2.9.0.4
*/
#include "adi_adrv904x_dfe_vswr.h"
#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_hal.h"
#include "adi_adrv904x_dfe_cpu.h"
#include "adi_adrv904x_dfe_framework_tracking_cal_t.h"

#include "../../private/include/adrv904x_tx.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"


#include "../../private/include/adrv904x_dfe_svc_err_dfe_boot_status_t.h"
#include "../../private/bf/adrv904x_bf_tx_dfe_dig_regs.h"
#include "../../private/include/adrv904x_dfe_cpu.h"


#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_DFE_VSWR


static adi_adrv904x_ErrAction_e adrv904x_TxVswrPlaybackDataWrite(adi_adrv904x_Device_t* const                    device,
                                                                 const uint32_t                                  txChannelMask,
                                                                 const adi_adrv904x_VswrPlaybackData_t* const    playbackData,
                                                                 const uint8_t                                   a55Running);


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrPlaybackDataWrite(adi_adrv904x_Device_t* const device,
                                                                    const uint32_t txChannelMask,
                                                                    const adi_adrv904x_VswrPlaybackData_t* const playbackData)
{

        adi_adrv904x_ErrAction_e recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);


    recoveryAction = adrv904x_TxVswrPlaybackDataWrite(device,
                                                      txChannelMask,
                                                      playbackData,
                                                      ADI_TRUE);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx VSWR Playback RAM Write Issue");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrPlaybackDataWrite_v2(adi_adrv904x_Device_t* const device,
                                                                       const uint32_t txChannelMask,
                                                                       const adi_adrv904x_VswrPlaybackData_t* const playbackData)
{

        adi_adrv904x_ErrAction_e recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, playbackData, cleanup);


    recoveryAction = adrv904x_TxVswrPlaybackDataWrite(device,
                                                      txChannelMask,
                                                      playbackData,
                                                      ADI_FALSE);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx VSWR Playback RAM Write Issue");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrPlaybackDataRead(adi_adrv904x_Device_t* const device,
                                                                   const uint32_t txChannelSel,
                                                                   adi_adrv904x_VswrPlaybackData_t* const playbackData)
{
        adi_adrv904x_ErrAction_e recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;


    uint32_t i                 = 0U;
    uint16_t bfReadback        = 0U;
    uint32_t baseAddrToRead    = 0U;
    uint8_t playbackEnableRead = 0U;
    adrv904x_BfTxDfeDigRegsChanAddr_e txDfeDigBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS;
    uint32_t dataToReadArr[ADI_ADRV904X_VSWR_PLAYBACK_DATA_MAX_NUM_SAMPLES];
    
    uint32_t vswrPlaybackBaseAddressArr[ADI_ADRV904X_MAX_TXCHANNELS] = {
        ADI_ADVRV904X_TX0_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX1_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX2_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX3_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX4_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX5_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX6_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX7_VSWR_PLAYBACK_RAM_BASEADDR 
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_LIBRARY_MEMSET(&dataToReadArr[0U], 0, sizeof(dataToReadArr));
    
    /*Check that if requested Tx Channel valid*/
    switch (txChannelSel) 
    {
    case ADI_ADRV904X_TX0:
        baseAddrToRead = vswrPlaybackBaseAddressArr[0];
        break;
    case ADI_ADRV904X_TX1:
        baseAddrToRead = vswrPlaybackBaseAddressArr[1];
        break;
    case ADI_ADRV904X_TX2:
        baseAddrToRead = vswrPlaybackBaseAddressArr[2];
        break;
    case ADI_ADRV904X_TX3:
        baseAddrToRead = vswrPlaybackBaseAddressArr[3];
        break;
    case ADI_ADRV904X_TX4:
        baseAddrToRead = vswrPlaybackBaseAddressArr[4];
        break;
    case ADI_ADRV904X_TX5:
        baseAddrToRead = vswrPlaybackBaseAddressArr[5];
        break;
    case ADI_ADRV904X_TX6:
        baseAddrToRead = vswrPlaybackBaseAddressArr[6];
        break;
    case ADI_ADRV904X_TX7:
        baseAddrToRead = vswrPlaybackBaseAddressArr[7];
        break;
    default:
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, baseAddrToRead, "Invalid Tx Channel used to determine vswr playback address");
        goto cleanup;
        break;
    }
    
    ADI_LIBRARY_MEMSET(playbackData->playbackDataReal, 0, sizeof(playbackData->playbackDataReal));
    ADI_LIBRARY_MEMSET(playbackData->playbackDataImag, 0, sizeof(playbackData->playbackDataImag));
    
    recoveryAction = adrv904x_TxDfeDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txDfeDigBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine Tx Dfe Dig base address");
        goto cleanup;
    }

    /* Check if playback data enabled before reading RAM */
    recoveryAction =  adrv904x_TxDfeDigRegs_TxVswrDataGenEnable_BfGet(device,
                                                                        NULL,
                                                                        txDfeDigBaseAddr,
                                                                        &playbackEnableRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading VSWR data playback");
        goto cleanup;
    }
    
    if (playbackEnableRead == ADI_TRUE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Playback generator should be disabled while reading from playback RAM");
        goto cleanup;
    }

    /* Enable AHB access to VSWR playback RAM */
    recoveryAction =  adrv904x_TxDfeDigRegs_TxVswrDataGenAhbAccessEnable_BfSet(device,
                                                                                NULL,
                                                                                txDfeDigBaseAddr,
                                                                                ADI_TRUE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling VSWR RAM access");
        goto cleanup;
    }
    
    /* Read the number of samples */
    recoveryAction =  adrv904x_TxDfeDigRegs_TxVswrDataGenMaxSamples_BfGet(device,
                                                                          NULL,
                                                                          txDfeDigBaseAddr,
                                                                          &bfReadback);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading number of samples");
        goto cleanup;
    }

    playbackData->numSamples = bfReadback;
    
    if (playbackData->numSamples > ADI_ADRV904X_VSWR_PLAYBACK_DATA_MAX_NUM_SAMPLES) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               playbackData->numSamples,
                               "Number of samples read from device is greater than ADI_ADRV904X_VSWR_PLAYBACK_DATA_MAX_NUM_SAMPLES");
        goto cleanup;
        
    }

    recoveryAction = adi_adrv904x_Registers32Read(device,
                                                  NULL,
                                                  baseAddrToRead,
                                                  dataToReadArr,
                                                  NULL,
                                                  playbackData->numSamples);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Write Issue");
        goto cleanup;
    }
    
    for (i = 0; i < playbackData->numSamples; ++i) 
    {
        playbackData->playbackDataReal[i] =  (int16_t)((dataToReadArr[i] >> 0U) & 0x0000FFFFU);
        playbackData->playbackDataImag[i] =  (int16_t)((dataToReadArr[i] >> 16U) & 0x0000FFFFU);
    }

    /* Dsiable AHB access to VSWR playback RAM */
    recoveryAction =  adrv904x_TxDfeDigRegs_TxVswrDataGenAhbAccessEnable_BfSet(device,
                                                                                NULL,
                                                                                txDfeDigBaseAddr,
                                                                                ADI_FALSE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disabling VSWR RAM access");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrTrackingConfigSet(adi_adrv904x_Device_t* const                     device,
                                                                    const uint32_t                                   vswrTxChannelMask,
                                                                    const adi_adrv904x_DfeAppCalVswrCfg_t* const     cfg)
{
        const uint16_t VSWR_CTRL_TRACK_CONFIG_SET = (uint16_t)(ADI_ADRV904X_DFE_APP_CAL_VSWR_CTRL_CONFIG | ADI_ADRV904X_DFE_APP_CAL_VSWR_CTRL_SET_FLAG);
    const uint32_t VSWR_MAX_ACCUM_ITERATIONS = 0x3FFFFFU;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_DfeAppCalVswrCfg_t vswrConfig;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfg, cleanup);

    if (((vswrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (vswrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               vswrTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    if (cfg->accumNumIter > VSWR_MAX_ACCUM_ITERATIONS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cfg->accumNumIter, "Number of iterations cannot be larger than 0x3FFFFFU");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY((void *)&vswrConfig, cfg, sizeof(adi_adrv904x_DfeAppCalVswrCfg_t));
    vswrConfig.accumNumIter = ADRV904X_HTOCL(vswrConfig.accumNumIter);
    vswrConfig.returnLossThreshmdB = ADRV904X_HTOCL(vswrConfig.returnLossThreshmdB);

    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (vswrTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_VSWR,
                                                               VSWR_CTRL_TRACK_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t*)&vswrConfig,
                                                               sizeof(adi_adrv904x_DfeAppCalVswrCfg_t),
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrTrackingConfigGet(adi_adrv904x_Device_t* const                     device,
                                                                    const adi_adrv904x_TxChannels_e                  vswrTxChannelSel,
                                                                    adi_adrv904x_DfeAppCalVswrCfg_t* const           cfg)
{
        const uint16_t VSWR_CTRL_TRACK_CONFIG_GET = (uint16_t)(ADI_ADRV904X_DFE_APP_CAL_VSWR_CTRL_CONFIG);
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfg, cleanup);
    
    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)vswrTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, vswrTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_VSWR,
                                                       VSWR_CTRL_TRACK_CONFIG_GET,
                                                       (adi_adrv904x_Channels_e)vswrTxChannelSel,
                                                       NULL,
                                                       0,
                                                       &lengthResp,
                                                       (uint8_t*)cfg,
                                                       sizeof(adi_adrv904x_DfeAppCalVswrCfg_t));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        cfg->accumNumIter = ADRV904X_CTOHL(cfg->accumNumIter);
        cfg->returnLossThreshmdB = ADRV904X_CTOHL(cfg->returnLossThreshmdB);
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrStatusGet(adi_adrv904x_Device_t* const               device,
                                                            const adi_adrv904x_Channels_e              vswrTxChannelSel,
                                                            adi_adrv904x_DfeAppCalVswrStatus_t* const  vswrStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e calId = ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TX_VSWR_MASK;
    uint32_t i = 0u;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, vswrStatus, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel(vswrTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, vswrTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    /* Use the common cal status get handler to get the data from the CPU */
    recoveryAction = adi_adrv904x_DfeCalSpecificStatusGet(device,
                                                          vswrTxChannelSel,
                                                          adrv904x_DfeTrackingCalToObjId(calId),
                                                          (uint8_t *)vswrStatus,
                                                          sizeof(adi_adrv904x_DfeAppCalVswrStatus_t));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting VSWR tracking cal status.");
        goto cleanup;
    }

    /* Generic header */
    vswrStatus->hdr.errorCode = ADRV904X_CTOHL(vswrStatus->hdr.errorCode);
    vswrStatus->hdr.iterCount = ADRV904X_CTOHL(vswrStatus->hdr.iterCount);
    vswrStatus->hdr.percentComplete = ADRV904X_CTOHL(vswrStatus->hdr.percentComplete);
    vswrStatus->hdr.performanceMetric = ADRV904X_CTOHL(vswrStatus->hdr.performanceMetric);
    vswrStatus->hdr.updateCount = ADRV904X_CTOHL(vswrStatus->hdr.updateCount);

    /* VSWR specific fields */
    for (i = 0u; i < ADI_ADRV904X_DFE_APP_CAL_VSWR_NUM_REFLECTIONS; ++i)
    {
        vswrStatus->reflection[i].returnLossmdB = ADRV904X_CTOHL(vswrStatus->reflection[i].returnLossmdB);
        vswrStatus->reflection[i].sampleDelay = ADRV904X_CTOHS(vswrStatus->reflection[i].sampleDelay);
    }
    vswrStatus->fwdSampleDelay = ADRV904X_CTOHS(vswrStatus->fwdSampleDelay);
    vswrStatus->fwdGainmdB = ADRV904X_CTOHL(vswrStatus->fwdGainmdB);
    vswrStatus->fwdCaptureTimeStamp_us = ADRV904X_CTOHLL(vswrStatus->fwdCaptureTimeStamp_us);
    vswrStatus->revCaptureTimeStamp_us = ADRV904X_CTOHLL(vswrStatus->revCaptureTimeStamp_us);
    vswrStatus->fwdCapturePowermdB = ADRV904X_CTOHL(vswrStatus->fwdCapturePowermdB);
    vswrStatus->revCapturePowermdB = ADRV904X_CTOHL(vswrStatus->revCapturePowermdB);
    vswrStatus->txPlaybackPowermdB = ADRV904X_CTOHL(vswrStatus->txPlaybackPowermdB);
    vswrStatus->captureAccumLength = ADRV904X_CTOHS(vswrStatus->captureAccumLength);
    
    /* Capture related status fields */
    vswrStatus->capStartRetryCount = ADRV904X_CTOHL(vswrStatus->capStartRetryCount);
    vswrStatus->capAbortRetryCount = ADRV904X_CTOHL(vswrStatus->capAbortRetryCount);
    vswrStatus->capInvalidRetryCount = ADRV904X_CTOHL(vswrStatus->capInvalidRetryCount);
    vswrStatus->capAbortErrorCount = ADRV904X_CTOHL(vswrStatus->capAbortErrorCount);
    vswrStatus->capInvalidErrorCount = ADRV904X_CTOHL(vswrStatus->capInvalidErrorCount);
    vswrStatus->capStartErrorCount = ADRV904X_CTOHL(vswrStatus->capStartErrorCount);
    vswrStatus->periodEndedErrorCount = ADRV904X_CTOHL(vswrStatus->periodEndedErrorCount);
    vswrStatus->txAttenChangeCount = ADRV904X_CTOHL(vswrStatus->txAttenChangeCount);
    vswrStatus->fwdPathUnstableCount = ADRV904X_CTOHL(vswrStatus->fwdPathUnstableCount);

    vswrStatus->calExitRequestCount = ADRV904X_CTOHL(vswrStatus->calExitRequestCount);

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrReset(adi_adrv904x_Device_t* const device,
                                                        const uint32_t               vswrTxChannelMask)
{
         const uint16_t VSWR_CTRL_RESET = (uint16_t)(ADI_ADRV904X_DFE_APP_CAL_VSWR_CTRL_RESET);
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (((vswrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (vswrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            vswrTxChannelMask,
            "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (vswrTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device,
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_VSWR,
                                                               VSWR_CTRL_RESET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               NULL,
                                                               0U,
                                                               &lengthResp,
                                                               NULL,
                                                               0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Failed to send request");
                goto cleanup;
            }
        }
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



static adi_adrv904x_ErrAction_e adrv904x_TxVswrPlaybackDataWrite(adi_adrv904x_Device_t* const                    device,
                                                                 const uint32_t                                  txChannelMask,
                                                                 const adi_adrv904x_VswrPlaybackData_t* const    playbackData,
                                                                 const uint8_t                                   a55Running)
{
    adi_adrv904x_ErrAction_e recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_DfeSvcErrDfeBootStatus_e bootStatus = ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_POWER_UP;
    uint32_t chanIdx           = 0U;
    uint32_t chanSel           = 0U;
    uint32_t i                 = 0U;
    uint32_t dataToWrite       = 0U;
    uint8_t playbackEnableRead = 0U;
    uint32_t origEccState      = 0U;
    uint8_t initEcc            = ADI_FALSE;
    adrv904x_BfTxDfeDigRegsChanAddr_e txDfeDigBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS;
    uint32_t dataToWriteArr[ADI_ADRV904X_VSWR_PLAYBACK_DATA_MAX_NUM_SAMPLES];
    uint32_t vswrPlaybackBaseAddressArr[ADI_ADRV904X_MAX_TXCHANNELS] = {
        ADI_ADVRV904X_TX0_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX1_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX2_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX3_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX4_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX5_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX6_VSWR_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX7_VSWR_PLAYBACK_RAM_BASEADDR 
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_RETURN(playbackData);
    ADI_LIBRARY_MEMSET(&dataToWriteArr[0U], 0, sizeof(dataToWriteArr));
                              
    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    if (playbackData->numSamples > ADI_ADRV904X_VSWR_PLAYBACK_DATA_MAX_NUM_SAMPLES) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               playbackData->numSamples,
                               "Number of samples cannot be greater than ADI_ADRV904X_VSWR_PLAYBACK_DATA_MAX_NUM_SAMPLES");
        goto cleanup;
    }

    if (a55Running == ADI_TRUE)
    {
        /* Read Enabled ECC scrubbing memory regions for VSWR playback RAM */
        recoveryAction =  adrv904x_DfeEccEnableGet(device,
                                                   &origEccState);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading VSWR RAM ECC state");
            goto cleanup;
        }
        else
        {
            initEcc = ADI_TRUE;
        }
    }
    else   /* need to confirm A55 is not running so it doesn't interfere with VSWR PLayback RAM access  */
    {
        /* Get the CPU boot status */
        recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                         ADRV904X_DFE_BOOT_STATUS_SCRATCH_REG_ID,
                                                         (uint8_t*) &bootStatus);
        if (bootStatus != ADRV904X_DFE_SVC_ERR_DFE_BOOT_STATUS_POWER_UP)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while accessing VSWR RAM.  A55 running");
            goto cleanup;
        }
    }

    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(txChannelMask, chanSel))
        {
            recoveryAction = adrv904x_TxDfeDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txDfeDigBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanSel, "Invalid Tx Channel used to determine Tx Dfe Dig base address");
                goto cleanup;
            }

            /* Check if playback data is enabled before writing a new dataset*/
            recoveryAction =  adrv904x_TxDfeDigRegs_TxVswrDataGenEnable_BfGet(device,
                                                                              NULL,
                                                                              txDfeDigBaseAddr,
                                                                              &playbackEnableRead);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading VSWR data playback");
                goto cleanup;
            }
            
            if (playbackEnableRead == ADI_TRUE) 
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Playback generator should be disabled while writing to playback RAM");
                goto cleanup;
            }
            
            /* Enable AHB access to VSWR playback RAM */
            recoveryAction =  adrv904x_TxDfeDigRegs_TxVswrDataGenAhbAccessEnable_BfSet(device,
                                                                                       NULL,
                                                                                       txDfeDigBaseAddr,
                                                                                       ADI_TRUE);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling VSWR RAM access");
                goto cleanup;
            }

            if (a55Running == ADI_TRUE)
            {
                /* Disable ECC scrubbing for VSWR playback RAM */
                recoveryAction =  adrv904x_DfeEccEnableSet(device,
                                                           ~(0x00000004u << chanIdx) & origEccState);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disabling VSWR RAM ECC scrubbing");
                    goto cleanup;
                }
            }

            for (i = 0; i < playbackData->numSamples; ++i) 
            {
                dataToWrite = ((((uint32_t)playbackData->playbackDataReal[i] << 0U) & 0x0000FFFFU) |
                               (((uint32_t)playbackData->playbackDataImag[i] << 16U) & 0xFFFF0000U));
                
                dataToWriteArr[i] = dataToWrite;
            }

            recoveryAction = adi_adrv904x_Registers32Write(device,
                                                           NULL,
                                                           &vswrPlaybackBaseAddressArr[chanIdx],
                                                           dataToWriteArr,
                                                           NULL,
                                                           playbackData->numSamples);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Write Issue");
                goto cleanup;
            }
            /* Program the number of samples */
            recoveryAction =  adrv904x_TxDfeDigRegs_TxVswrDataGenMaxSamples_BfSet(device,
                                                                                  NULL,
                                                                                  txDfeDigBaseAddr,
                                                                                  playbackData->numSamples);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing number of samples");
                goto cleanup;
            }
            
            /* Disable AHB access to VSWR playback RAM */
            recoveryAction =  adrv904x_TxDfeDigRegs_TxVswrDataGenAhbAccessEnable_BfSet(device,
                                                                                       NULL,
                                                                                       txDfeDigBaseAddr,
                                                                                       ADI_FALSE);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disabling VSWR RAM access");
                goto cleanup;
            }
            
            device->devStateInfo.vswrWaveformLoaded[chanIdx] = ADI_TRUE;
        }
    }

cleanup:
    if (initEcc == ADI_TRUE)
    {
        /* Restore ECC scrubbing for VSWR playback RAM */
        recoveryAction =  adrv904x_DfeEccEnableSet(device,
                                                   origEccState);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while restoring VSWR RAM ECC scrubbing");
        }
    }

    return recoveryAction;
}
