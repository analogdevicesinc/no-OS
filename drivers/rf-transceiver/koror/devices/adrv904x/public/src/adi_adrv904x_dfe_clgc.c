/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_clgc.c
* \brief Contains CLGC features related function implementations
*
* ADRV904X API Version: 2.10.0.4
*/
#include "adi_library_types.h"
#include "adi_adrv904x_dfe_clgc.h"
#include "adi_adrv904x_core.h"
#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_hal.h"
#include "adi_adrv904x_dfe_cpu.h"
#include "adi_adrv904x_dfe_cal_clgc_types.h"
#include "adi_adrv904x_dfe_framework_tracking_cal_t.h"
#include "../../private/include/adrv904x_platform_byte_order.h"
#include "../../private/include/adrv904x_dfe_cpu.h"


#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_DFE_CLGC

static adi_adrv904x_ErrAction_e adrv904x_ClgcCaptureConfigSetRangeCheck(adi_adrv904x_Device_t* const                       device,
                                                                        const uint32_t                                     clgcTxChannelMask,
                                                                        const adi_adrv904x_DfeAppCalClgcCaptureCfg_t* const clgcCaptureCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, clgcCaptureCfg);

    if (((clgcTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (clgcTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clgcTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }
    
    if (clgcCaptureCfg->capMode > ADI_ADRV904X_DFE_APP_CAL_CLGC_CAPTURE_MODE_PEAK_DET)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clgcCaptureCfg->capMode,
                               "Invalid capMode is selected");
        return recoveryAction;
    }

    if (clgcCaptureCfg->numCapBatches < 1)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clgcCaptureCfg->numCapBatches,
                               "Invalid numCapBatches is selected. It must be greater than 0");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_ClgcTrackConfigSetRangeCheck(adi_adrv904x_Device_t* const                       device,
                                                                      const uint32_t                                     clgcTxChannelMask,
                                                                      const adi_adrv904x_DfeAppCalClgcTrackCfg_t* const  clgcTrackCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, clgcTrackCfg);

    if (((clgcTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (clgcTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clgcTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }
    
    if (clgcTrackCfg->enClgc > 0x1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clgcTrackCfg->enClgc,
                               "Invalid enClgc is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }

    if (clgcTrackCfg->enPaProtect > 0x1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clgcTrackCfg->enPaProtect,
                               "Invalid enPaProtect is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcCaptureConfigSet(adi_adrv904x_Device_t* const                         device,
                                                                   const uint32_t                                       clgcTxChannelMask,
                                                                   const adi_adrv904x_DfeAppCalClgcCaptureCfg_t* const  clgcCaptureCfg)
{
        const uint16_t CLGC_CTRL_CAPTURE_CONFIG_SET =   ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_CAPTURE_CONFIG   |
                                                    ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_COMMAND_FLAG     |
                                                    ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_SET_FLAG;
    adi_adrv904x_DfeAppCalClgcCaptureCfg_t tmpClgcCaptureCfg;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, clgcCaptureCfg, cleanup);

    recoveryAction = adrv904x_ClgcCaptureConfigSetRangeCheck(device, clgcTxChannelMask, clgcCaptureCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Clgc Capture Config range check failed.");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY((void *)&tmpClgcCaptureCfg, clgcCaptureCfg, sizeof(adi_adrv904x_DfeAppCalClgcCaptureCfg_t));

    tmpClgcCaptureCfg.numCapBatches = ADRV904X_HTOCL(tmpClgcCaptureCfg.numCapBatches);
    tmpClgcCaptureCfg.capDurationUs = ADRV904X_HTOCL(tmpClgcCaptureCfg.capDurationUs);
    tmpClgcCaptureCfg.capMode = ADRV904X_HTOCL(tmpClgcCaptureCfg.capMode);
    tmpClgcCaptureCfg.capPeriodUs = ADRV904X_HTOCL(tmpClgcCaptureCfg.capPeriodUs);
    tmpClgcCaptureCfg.minOrxPowThres = ADRV904X_HTOCL(tmpClgcCaptureCfg.minOrxPowThres);
    tmpClgcCaptureCfg.minTxPowThres = ADRV904X_HTOCL(tmpClgcCaptureCfg.minTxPowThres);
    
    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (clgcTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device, 
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               CLGC_CTRL_CAPTURE_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t*)&tmpClgcCaptureCfg,
                                                               sizeof(tmpClgcCaptureCfg),
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

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcCaptureConfigGet(adi_adrv904x_Device_t* const                     device,
                                                                   const adi_adrv904x_TxChannels_e                  clgcTxChannelSel,
                                                                   adi_adrv904x_DfeAppCalClgcCaptureCfg_t* const    clgcCaptureCfg)
{
        const uint16_t CLGC_CTRL_CAPTURE_CONFIG_GET =   ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_CAPTURE_CONFIG |
                                                    ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_COMMAND_FLAG;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, clgcCaptureCfg, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)clgcTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, clgcTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device, 
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       CLGC_CTRL_CAPTURE_CONFIG_GET,
                                                       (adi_adrv904x_Channels_e)clgcTxChannelSel,
                                                       NULL,
                                                       0,
                                                       &lengthResp,
                                                       (uint8_t*)clgcCaptureCfg,
                                                       sizeof(*clgcCaptureCfg));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, clgcTxChannelSel, "Failed to send request");
        goto cleanup;
    }

    clgcCaptureCfg->numCapBatches = ADRV904X_CTOHL(clgcCaptureCfg->numCapBatches);
    clgcCaptureCfg->capDurationUs = ADRV904X_CTOHL(clgcCaptureCfg->capDurationUs);
    clgcCaptureCfg->capMode = ADRV904X_CTOHL(clgcCaptureCfg->capMode);
    clgcCaptureCfg->capPeriodUs = ADRV904X_CTOHL(clgcCaptureCfg->capPeriodUs);
    clgcCaptureCfg->minOrxPowThres = ADRV904X_CTOHL(clgcCaptureCfg->minOrxPowThres);
    clgcCaptureCfg->minTxPowThres = ADRV904X_CTOHL(clgcCaptureCfg->minTxPowThres);

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcTrackingConfigSet(adi_adrv904x_Device_t* const                        device,
                                                                   const uint32_t                                       clgcTxChannelMask,
                                                                   const adi_adrv904x_DfeAppCalClgcTrackCfg_t* const    clgcTrackCfg)
{
        const uint16_t CLGC_CTRL_TRACK_CONFIG_SET = ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_TRACK_CONFIG |   
                                                 ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_COMMAND_FLAG |  
                                                 ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_SET_FLAG;
    adi_adrv904x_DfeAppCalClgcTrackCfg_t tmpClgcTrackCfg;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, clgcTrackCfg, cleanup);

    recoveryAction = adrv904x_ClgcTrackConfigSetRangeCheck(device, clgcTxChannelMask, clgcTrackCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Clgc Tracking Config range check failed.");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY((void *)&tmpClgcTrackCfg, clgcTrackCfg, sizeof(adi_adrv904x_DfeAppCalClgcTrackCfg_t));

    tmpClgcTrackCfg.enClgc = tmpClgcTrackCfg.enClgc;
    tmpClgcTrackCfg.enPaProtect = tmpClgcTrackCfg.enPaProtect;
    tmpClgcTrackCfg.expLoopPowGain = ADRV904X_HTOCL(tmpClgcTrackCfg.expLoopPowGain);
    tmpClgcTrackCfg.maxLoopGainAdjustdB = ADRV904X_HTOCL(tmpClgcTrackCfg.maxLoopGainAdjustdB);
    tmpClgcTrackCfg.maxTxAttenLimitdB = ADRV904X_HTOCL(tmpClgcTrackCfg.maxTxAttenLimitdB);
    tmpClgcTrackCfg.minTxAttenLimitdB = ADRV904X_HTOCL(tmpClgcTrackCfg.minTxAttenLimitdB);
    
    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (clgcTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device, 
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               CLGC_CTRL_TRACK_CONFIG_SET,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               (const uint8_t*)&tmpClgcTrackCfg,
                                                               sizeof(tmpClgcTrackCfg),
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

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcTrackingConfigGet(adi_adrv904x_Device_t* const                device,
                                                                   const adi_adrv904x_TxChannels_e              clgcTxChannelSel,
                                                                   adi_adrv904x_DfeAppCalClgcTrackCfg_t* const  clgcTrackCfg)
{
        const uint16_t CLGC_CTRL_TRACK_CONFIG_GET = ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_TRACK_CONFIG |
                                                ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_COMMAND_FLAG;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
        
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, clgcTrackCfg, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)clgcTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, clgcTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device, 
                                                       ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                       CLGC_CTRL_TRACK_CONFIG_GET,
                                                       (adi_adrv904x_Channels_e)clgcTxChannelSel,
                                                       NULL,
                                                       0,
                                                       &lengthResp,
                                                       (uint8_t*)clgcTrackCfg,
                                                       sizeof(*clgcTrackCfg));

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, clgcTxChannelSel, "Failed to send request");
        goto cleanup;
    }

    clgcTrackCfg->enClgc = clgcTrackCfg->enClgc;
    clgcTrackCfg->enPaProtect = clgcTrackCfg->enPaProtect;
    clgcTrackCfg->expLoopPowGain = ADRV904X_CTOHL(clgcTrackCfg->expLoopPowGain);
    clgcTrackCfg->maxLoopGainAdjustdB = ADRV904X_CTOHL(clgcTrackCfg->maxLoopGainAdjustdB);
    clgcTrackCfg->maxTxAttenLimitdB = ADRV904X_CTOHL(clgcTrackCfg->maxTxAttenLimitdB);
    clgcTrackCfg->minTxAttenLimitdB = ADRV904X_CTOHL(clgcTrackCfg->minTxAttenLimitdB);

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcTrackingRun(adi_adrv904x_Device_t* const  device,
                                                              const uint32_t                clgcTxChannelMask)
{
        const uint16_t CLGC_CTRL_TRACK_RUN_CMD = ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_TRACK_RUN       |
                                             ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_COMMAND_FLAG;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (((clgcTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (clgcTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clgcTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        
        goto cleanup;
    }
    
    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (clgcTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device, 
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               CLGC_CTRL_TRACK_RUN_CMD,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               NULL,
                                                               0,
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

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcTrackingReset(adi_adrv904x_Device_t* const    device,
                                                                const uint32_t                  clgcTxChannelMask)
{
        const uint16_t CLGC_CTRL_TRACK_RESET_CMD =  ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_TRACK_RESET |
                                                ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_COMMAND_FLAG;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lengthResp = 0;
    uint32_t i = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (((clgcTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (clgcTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clgcTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        
        goto cleanup;
    }
    
    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        uint32_t txChan = (1 << i);
        if (clgcTxChannelMask & txChan)
        {
            recoveryAction = adi_adrv904x_DfeAppControlCmdExec(device, 
                                                               ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
                                                               CLGC_CTRL_TRACK_RESET_CMD,
                                                               (adi_adrv904x_Channels_e)txChan,
                                                               NULL,
                                                               0,
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

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeClgcCaptureBuffersAccessSet(adi_adrv904x_Device_t* const                                    device,
                                                                             const uint32_t                                                  clgcTxChannelMask,
                                                                             adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessSet_t* const clgcCapBufAccess)
{
        const uint16_t CLGC_CTRL_CAPTURE_BUF_ACCESS_SET =   ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_CAPTURE_BUF_ACCESS   |
                                                        ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_COMMAND_FLAG     |
                                                        ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_SET_FLAG;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, clgcCapBufAccess, cleanup);

    recoveryAction = adrv904x_DfeCaptureBuffersAccessSet(device,
        clgcTxChannelMask,
        ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
        CLGC_CTRL_CAPTURE_BUF_ACCESS_SET,
        clgcCapBufAccess);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to send request");
        goto cleanup;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeClgcCaptureBuffersAccessGet(adi_adrv904x_Device_t* const                                    device,
                                                                             const adi_adrv904x_TxChannels_e                                 clgcTxChannelSel,
                                                                             adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessGet_t* const clgcCapBufAccess)
{
        const uint16_t CLGC_CTRL_CAPTURE_BUF_ACCESS_GET =   ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_CAPTURE_BUF_ACCESS   |
                                                        ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_COMMAND_FLAG;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, clgcCapBufAccess, cleanup);

    recoveryAction =  adrv904x_DfeCaptureBuffersAccessGet(device,
        clgcTxChannelSel,
        ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD,
        CLGC_CTRL_CAPTURE_BUF_ACCESS_GET,
        clgcCapBufAccess);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to send request");
        goto cleanup;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeClgcStatusGet(adi_adrv904x_Device_t* const device,
                                                               const adi_adrv904x_TxChannels_e clgcTxChannelSel,
                                                               adi_adrv904x_DfeAppCalClgcStatus_t* const clgcStatus)
{
            adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e calId = ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TX_DPD_MASK;
    adi_adrv904x_DfeAppCalClgcStatus_t clgcCalStatus;

    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, clgcStatus, cleanup);

    /*Check that if requested Tx Channel valid*/
    /* Channel must contain a single channel number (0-7) */
    recoveryAction = adrv904x_DfeVerifyChannel((adi_adrv904x_Channels_e)clgcTxChannelSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, clgcTxChannelSel, "channel parameter is invalid.");
        goto cleanup;
    }

    /* Use the common cal status get handler to get the data from the CPU */
    recoveryAction = adi_adrv904x_DfeCalExtendedStatusGet(device,
                                                          (adi_adrv904x_Channels_e)clgcTxChannelSel,
                                                          adrv904x_DfeTrackingCalToObjId(calId),
                                                          (uint8_t *)&clgcCalStatus,
                                                          sizeof(clgcCalStatus));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting tracking cal status.");
        goto cleanup;
    }

    /* Translate the response from the CPU */
    clgcStatus->hdr.errorCode = ADRV904X_CTOHL(clgcCalStatus.hdr.errorCode);
    clgcStatus->hdr.iterCount = ADRV904X_CTOHL(clgcCalStatus.hdr.iterCount);
    clgcStatus->hdr.percentComplete = ADRV904X_CTOHL(clgcCalStatus.hdr.percentComplete);
    clgcStatus->hdr.performanceMetric = ADRV904X_CTOHL(clgcCalStatus.hdr.performanceMetric);
    clgcStatus->hdr.updateCount = ADRV904X_CTOHL(clgcCalStatus.hdr.updateCount);

    clgcStatus->loopPowGain = ADRV904X_CTOHL(clgcCalStatus.loopPowGain);
    clgcStatus->orxPwr = ADRV904X_CTOHL(clgcCalStatus.orxPwr);
    clgcStatus->txAttenAdjdB = ADRV904X_CTOHL(clgcCalStatus.txAttenAdjdB);
    clgcStatus->txPwr = ADRV904X_CTOHL(clgcCalStatus.txPwr);
	clgcStatus->numCapBatchesRun = ADRV904X_CTOHL(clgcCalStatus.numCapBatchesRun);
	clgcStatus->actCapDurationUs = ADRV904X_CTOHL(clgcCalStatus.actCapDurationUs);
	clgcStatus->capStartRetryCount = ADRV904X_CTOHL(clgcCalStatus.capStartRetryCount);
	clgcStatus->capAbortRetryCount = ADRV904X_CTOHL(clgcCalStatus.capAbortRetryCount);
	clgcStatus->capInvalidRetryCount = ADRV904X_CTOHL(clgcCalStatus.capInvalidRetryCount);
	clgcStatus->lowSigRetryCount = ADRV904X_CTOHL(clgcCalStatus.lowSigRetryCount);
	clgcStatus->txAttenChangeCount = ADRV904X_CTOHL(clgcCalStatus.txAttenChangeCount);
	clgcStatus->orxGainChangeCount = ADRV904X_CTOHL(clgcCalStatus.orxGainChangeCount);
	clgcStatus->txAttenLimitCount = ADRV904X_CTOHL(clgcCalStatus.txAttenLimitCount);
	clgcStatus->paProtectAssertCount = ADRV904X_CTOHL(clgcCalStatus.paProtectAssertCount);
	clgcStatus->lowSigErrorCount = ADRV904X_CTOHL(clgcCalStatus.lowSigErrorCount);
	clgcStatus->capAbortErrorCount = ADRV904X_CTOHL(clgcCalStatus.capAbortErrorCount);
	clgcStatus->capInvalidErrorCount = ADRV904X_CTOHL(clgcCalStatus.capInvalidErrorCount);
	clgcStatus->capStartErrorCount = ADRV904X_CTOHL(clgcCalStatus.capStartErrorCount);
	clgcStatus->appExitRequestCount = ADRV904X_CTOHL(clgcCalStatus.appExitRequestCount);
	clgcStatus->gainMonAssertedCount = ADRV904X_CTOHL(clgcCalStatus.gainMonAssertedCount);
    clgcStatus->periodEndedErrorCount = ADRV904X_CTOHL(clgcCalStatus.periodEndedErrorCount);

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


