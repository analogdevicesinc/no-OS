/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adrv904x_dfe_dpd.c
*
* \brief Contains ADRV904X DFE DPD related private function implementations
* 
* ADRV904X API Version: 2.9.0.4
*/
#include "adi_adrv904x_dfe_cal_dpd_types.h"
#include "../../private/include/adrv904x_dfe_dpd.h"
#define ADI_FILE    ADI_ADRV904X_FILE_PRIVATE_DFE_DPD


adi_adrv904x_ErrAction_e adrv904x_DpdModelConfigDpdSetRangeCheck(adi_adrv904x_Device_t* const                      device,
                                                                 const uint32_t                                    dpdTxChannelMask,
                                                                 const adi_adrv904x_DfeAppCalDpdModelType_e        dpdModelType,
                                                                 const adi_adrv904x_DfeAppCalDpdModelDesc_t* const modelDesc)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, modelDesc);
    
    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }
    
    if (dpdModelType > ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_DPD_3)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdModelType,
                               "Invalid DPD Model is selected. Valid values must be less than 3(ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_DPD_3)");
        return recoveryAction;
    }
    
    if (modelDesc->features == 0u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction,
                               modelDesc->features,
                               "modelDesc->features should be 1 ~ 255.");
        return recoveryAction;
    }
    
    if (modelDesc->dpdPartial.partial > ADI_ADRV904X_DFE_APP_CAL_DPD_TYPE_CTC_2)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction,
                               modelDesc->dpdPartial.partial,
                               "Invalid modelDesc->dpdPartial.partial is selected, modelDesc->dpdPartial should be DPD, CTC1 or CTC2 ");
        return recoveryAction;
    }

    if (modelDesc->mode > ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_GMP_DDR_MODE7)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction,
                               modelDesc->mode,
                               "Invalid modelDesc->mode is selected, modelDesc->mode should be 0~7 ");
        return recoveryAction;
    }
    
    if (modelDesc->actDepth > ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_DEPTH_16)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction,
                               modelDesc->actDepth,
                               "Invalid modelDesc->actDepth is selected, modelDesc->mode should be 0/1/2 ");
        return recoveryAction;
    }
    
    for (uint8_t i = 0; i < modelDesc->features; i++)
    {
        if (modelDesc->feature[i].poly != ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_POLY_GMP0 &&
            modelDesc->feature[i].poly != ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_POLY_GMP1 &&
            modelDesc->feature[i].poly != ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_POLY_DDR7 &&
            modelDesc->feature[i].poly != ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_POLY_DDR8 &&
            modelDesc->feature[i].poly != ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_POLY_DDR9)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, 
                                   recoveryAction,
                                   modelDesc->feature[i].poly,
                                   "Invalid feature[i].poly is selected, feature[i].poly should be 0/1/7/8/9 ");
            return recoveryAction;
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DpdModelConfigCtcSetRangeCheck(adi_adrv904x_Device_t* const                      device,
                                                                 const uint32_t                                    dpdTxChannelMask,
                                                                 const adi_adrv904x_DfeAppCalDpdModelType_e        dpdModelType,
                                                                 const adi_adrv904x_DfeAppCalCtcModelDesc_t* const modelDesc)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, modelDesc);

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    if ((dpdModelType > ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_CTC_4) || (dpdModelType < ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_CTC_0))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdModelType,
                               "Invalid CTC Model is selected. Valid values must be 3/4/5/6/7");
        return recoveryAction;
    }
    
    if (modelDesc->features == 0u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction,
                               modelDesc->features,
                               "modelDesc->features should be 1 ~ 255.");
        return recoveryAction;
    }

    if (modelDesc->ctcPartial.partial > ADI_ADRV904X_DFE_APP_CAL_DPD_TYPE_CTC_2)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction,
                               modelDesc->ctcPartial.partial,
                               "Invalid modelDesc->ctcPartial.partial is selected, modelDesc->dpdPartial should be DPD, CTC1 or CTC2 ");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DpdActuatorGainMonitorConfigSetRangeCheck(adi_adrv904x_Device_t* const                          device,
                                                                            const uint32_t                                        dpdTxChannelMask,
                                                                            const adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const dpdPowerMeterCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint32_t ADI_ADRV904X_DPDINOUT_PWR_MEASDURATION_MAX = 24u;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, dpdPowerMeterCfg);

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }
    
    if (dpdPowerMeterCfg->dpdInOutPwrMeasDuration > ADI_ADRV904X_DPDINOUT_PWR_MEASDURATION_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction,
                               dpdPowerMeterCfg->dpdInOutPwrMeasDuration,
                               "dpdInOutPwrMeasDuration should be 0 ~ 24.");
        return recoveryAction;
    }
    
    if (dpdPowerMeterCfg->dpdInOutPwrMeasEn > 0x1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, 
                               recoveryAction,
                               dpdPowerMeterCfg->dpdInOutPwrMeasEn,
                               "Invalid dpdInOutPwrMeasEn, 0 is for disable and 1 is for enable ");
        return recoveryAction;
    }

    if ((dpdPowerMeterCfg->modelTableForMinGainLim != 1U) &&
         (dpdPowerMeterCfg->modelTableForMinGainLim != 2U) &&
         (dpdPowerMeterCfg->modelTableForMinGainLim != 4U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdPowerMeterCfg->modelTableForMinGainLim,
                               "Invalid model for min gain limit violation. Valid values are 1, 2 or 4 (Unity)");
        return recoveryAction;
    }

    if ((dpdPowerMeterCfg->modelTableForMaxGainLim != 1U) &&
        (dpdPowerMeterCfg->modelTableForMaxGainLim != 2U) &&
        (dpdPowerMeterCfg->modelTableForMaxGainLim != 4U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdPowerMeterCfg->modelTableForMaxGainLim,
                               "Invalid model for max gain limit violation. Valid values are 1, 2 or 4 (Unity)");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DpdCaptureConfigSetRangeCheck(adi_adrv904x_Device_t* const                       device,
                                                                const uint32_t                                     dpdTxChannelMask,
                                                                const adi_adrv904x_DfeAppCalDpdCaptureCfg_t* const dpdCaptureCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint32_t ADI_ADRV904X_DPD_MAX_CAP_SAMPLE = 4096u;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, dpdCaptureCfg);

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    if (dpdCaptureCfg->capPeakDetCfg.windowLength < ADI_ADRV904X_DPD_MAX_CAP_SAMPLE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdCaptureCfg->capPeakDetCfg.windowLength,
                               "Invalid window length is selected. window length has to be greater than maximum capture sample");
        return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DpdTrackingConfigSetRangeCheck(adi_adrv904x_Device_t* const                       device,
                                                                 const uint32_t                                     dpdTxChannelMask,
                                                                 const adi_adrv904x_DfeAppCalDpdTrackCfg_t* const   dpdTrackCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, dpdTrackCfg);

    static const uint32_t MAX_NUMBER_OF_MULTIFRAMES = 600U;
    static const uint32_t MIN_NUMBER_OF_MULTIFRAMES = 1U;

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    if (dpdTrackCfg->adapt.featureFilterEn > 0x1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->adapt.featureFilterEn,
                               "Invalid featureFilterEn is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }
    
    if (dpdTrackCfg->runClgc > 0x1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->runClgc,
                               "Invalid runClgc is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }

    if (dpdTrackCfg->useLegacyGainPhaseComp > 0x1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->useLegacyGainPhaseComp,
                               "Invalid useLegacyGainPhaseComp is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }

    if (dpdTrackCfg->updateMode > ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->updateMode,
                               "Invalid dpdTrackCfg updateMode is selected. updateMode should be 0/1/2");
        return recoveryAction;
    }

    if (dpdTrackCfg->forceDirect > 0x1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->forceDirect,
                               "Invalid forceDirect is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }

    if (dpdTrackCfg->mu > 100u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->mu,
                               "Invalid dpdTrackCfg->mu is selected. It should not be larger than 100 ");
        return recoveryAction;
    }

    if ((dpdTrackCfg->lowPowerActionModel != 1U) &&
         (dpdTrackCfg->lowPowerActionModel != 2U) &&
         (dpdTrackCfg->lowPowerActionModel != 4U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->lowPowerActionModel,
                               "Invalid model for lowPowerActionModel. Valid values are 1, 2 or 4 (Unity)");
        return recoveryAction;
    }

    if ((dpdTrackCfg->numberOfMultiFrames < MIN_NUMBER_OF_MULTIFRAMES) || (dpdTrackCfg->numberOfMultiFrames > MAX_NUMBER_OF_MULTIFRAMES))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                dpdTrackCfg->numberOfMultiFrames,
                                "Invalid dpdTrackCfg->numberOfMultiFrames is selected. Range is [1-600] ");
        return recoveryAction;
    }

    if ((dpdTrackCfg->adapt.numDpdSamples < 0x1000U) ||
        (dpdTrackCfg->adapt.numDpdSamples > 0x8000U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->adapt.numDpdSamples,
                               "Invalid numDpdSamples is selected. Valid value is within 4096 (0x1000) and 32,768 (0x8000).");
        return recoveryAction;
    }

    if (dpdTrackCfg->mThresholdDB > 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->mThresholdDB,
                               "Invalid mThresholdDB is selected. Valid value is negative number.");
        return recoveryAction;
    }


    for(uint8_t i=0; i<ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS; i++)
    {
        if (dpdTrackCfg->indirectRegValue[i] > 63U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   dpdTrackCfg->indirectRegValue[i],
                                   "Invalid indirectRegVal is selected. Valid value is within 0 and 63.");
            return recoveryAction;
        }
    }
    
    if (dpdTrackCfg->directRegValue > 63U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->directRegValue,
                               "Invalid directRegVal is selected. Valid value is within 0 and 63.");
        return recoveryAction;
    }

    if (dpdTrackCfg->ctc1StatsSampleNum > 1112u)  
    {
        /* maximum number of samples (in ctc1 stats Collection) is equal to (140-1) * 8
         * where 140 is the burst count in each capture, 1 is due to the HW bug, 
         * and 8 is the maximum number of the captures.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->ctc1StatsSampleNum, 
                               "Invalid ctc1StatsSampleNum is selected. Maximum value is 1112.");
        return recoveryAction;
    }

    if (dpdTrackCfg->ctc1ManualEnabled > 0x1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->ctc1ManualEnabled, 
                               "Invalid ctc1ManualEnabled is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }


    if (dpdTrackCfg->ctc1DeltaPercentage > 20u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->ctc1DeltaPercentage, 
                               "Invalid ctc1DeltaPercentage is selected. Maximum value is 20.");
        return recoveryAction;
    }

    for(uint8_t i=0; i<3; i++)
    {
        if (dpdTrackCfg->ctc1PeakRankRatio[i] > 100U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   dpdTrackCfg->ctc1PeakRankRatio[i],
                                   "Invalid ctc1PeakRankRatio is selected. Valid value is within 0 and 100.");
            return recoveryAction;
        }
    }

    for(uint8_t i=0; i<3; i++)
    {
        if (dpdTrackCfg->ctc1CaptureWeighting[i] > 8U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   dpdTrackCfg->ctc1CaptureWeighting[i],
                                   "Invalid ctc1CaptureWeighting is selected. Valid value is within 0 and 8.");
            return recoveryAction;
        }
    }

    if ((dpdTrackCfg->wbRegEnable != 0u) &&
        (dpdTrackCfg->wbRegEnable != 1u))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->wbRegEnable,
                               "Invalid wbRegEnable is selected. Valid value is 0/1 ");
        return recoveryAction;
    }

    if ((dpdTrackCfg->wbRegMode != 0U) &&
        (dpdTrackCfg->wbRegMode != 0x1U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->wbRegMode,
                               "Invalid wbRegMode is selected. Valid value is 0/1.");
        return recoveryAction;
    }

    if ((dpdTrackCfg->wbRegAlpha < 0.0f) ||
        (dpdTrackCfg->wbRegAlpha > 1.0f))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->wbRegAlpha,
                               "Invalid wbRegAlpha is selected. Valid value is within 0.0 and 1.0 ");
        return recoveryAction;
    }

    if ((dpdTrackCfg->wbRegBeta < 0.0f) ||
        (dpdTrackCfg->wbRegBeta > 1.0f))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->wbRegBeta,
                               "Invalid wbRegBeta is selected. Valid value is within 0.0 and 1.0 ");
        return recoveryAction;
    }

    if ((dpdTrackCfg->bwDetCfg.bwDetEn != 0u) &&
        (dpdTrackCfg->bwDetCfg.bwDetEn != 1u))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->bwDetCfg.bwDetEn,
                               "Invalid bwDetEn is selected. Valid value is 0/1 ");
        return recoveryAction;
    }

    if (dpdTrackCfg->bwDetCfg.corrLagMax > 4095u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->bwDetCfg.corrLagMax,
                               "Invalid corrLagMax is selected. Valid value is within 0:4095 ");
        return recoveryAction;
    }

    if ((dpdTrackCfg->bwDetCfg.loBwBeta2Thres < 0.0f) ||
        (dpdTrackCfg->bwDetCfg.loBwBeta2Thres > 1.0f))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->bwDetCfg.loBwBeta2Thres,
                               "Invalid loBwBeta2Thres is selected. Valid value is within 0.0 and 1.0 ");
        return recoveryAction;
    }

    if ((dpdTrackCfg->bwDetCfg.hiBwBeta2Thres < 0.0f) ||
        (dpdTrackCfg->bwDetCfg.hiBwBeta2Thres > 1.0f))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->bwDetCfg.hiBwBeta2Thres,
                               "Invalid hiBwBeta2Thres is selected. Valid value is within 0.0 and 1.0 ");
        return recoveryAction;
    }

    if ((dpdTrackCfg->bwDetCfg.alphaTrack < 0.0f) ||
        (dpdTrackCfg->bwDetCfg.alphaTrack > 1.0f))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTrackCfg->bwDetCfg.alphaTrack,
                               "Invalid alphaTrack is selected. Valid value is within 0.0 and 1.0 ");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}

adi_adrv904x_ErrAction_e adrv904x_DpdTddLutSwitchCfgSetRangeCheck(adi_adrv904x_Device_t* const                           device,
                                                                  const uint32_t                                         dpdTxChannelMask,
                                                                  const adi_adrv904x_DfeAppDpdActTddLutSwitch_t* const   tddLutSwitchConfig)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, tddLutSwitchConfig);

    if (((dpdTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dpdTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dpdTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    if (tddLutSwitchConfig->txOnMdl0SwitchEn > 0x1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               tddLutSwitchConfig->txOnMdl0SwitchEn,
                               "Invalid tddLutSwitchConfig->txOnMdl0SwitchEn is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }
       
    if (tddLutSwitchConfig->txOnMdl1SwitchEn > 0x1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               tddLutSwitchConfig->txOnMdl1SwitchEn,
                               "Invalid tddLutSwitchConfig->txOnMdl1SwitchEn is selected. 1 is for enable and 0 is for disable.");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
    return recoveryAction;
}

