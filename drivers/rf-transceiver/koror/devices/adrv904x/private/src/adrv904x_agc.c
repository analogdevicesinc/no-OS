/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_agc.c
 * \brief Contains ADRV904X AGC related private function implementations
 *
 * ADRV904X API Version: 2.10.0.4
 */

#include "../../private/include/adrv904x_agc.h"
#include "../../private/include/adrv904x_rx.h"


#define ADI_FILE    ADI_ADRV904X_FILE_PRIVATE_AGC

ADI_API adi_adrv904x_ErrAction_e adrv904x_AgcGainRangeCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_AgcGainRange_t * const agcGainRangeCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint8_t chanIdx = 0U;
    uint8_t rxMinIndex = 0U;
    uint8_t rxMaxIndex = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check agcGainRangeCfg address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, agcGainRangeCfg);
    
    if (((agcGainRangeCfg->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || (agcGainRangeCfg->rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcGainRangeCfg->rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }
    
    if (agcGainRangeCfg->minGainIndex > agcGainRangeCfg->maxGainIndex)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcGainRangeCfg->minGainIndex,
                               "minGainIndex cannot be larger than maxGainIndex");
        return recoveryAction;
    }
    
    /* AGC config range check */
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
    {
        if (ADRV904X_BF_EQUAL(agcGainRangeCfg->rxChannelMask, (uint32_t)(1U << chanIdx)))
        {
            if (chanIdx == 0U)
            {
                rxMinIndex = device->devStateInfo.gainIndexes.rx0MinGainIndex;
                rxMaxIndex = device->devStateInfo.gainIndexes.rx0MaxGainIndex;
            }
            else if (chanIdx == 1U)
            {
                rxMinIndex = device->devStateInfo.gainIndexes.rx1MinGainIndex;
                rxMaxIndex = device->devStateInfo.gainIndexes.rx1MaxGainIndex;
            }
            else if (chanIdx == 2U)
            {
                rxMinIndex = device->devStateInfo.gainIndexes.rx2MinGainIndex;
                rxMaxIndex = device->devStateInfo.gainIndexes.rx2MaxGainIndex;
            }
            else if (chanIdx == 3U)
            {
                rxMinIndex = device->devStateInfo.gainIndexes.rx3MinGainIndex;
                rxMaxIndex = device->devStateInfo.gainIndexes.rx3MaxGainIndex;
            }
            else if (chanIdx == 4U)
            {
                rxMinIndex = device->devStateInfo.gainIndexes.rx4MinGainIndex;
                rxMaxIndex = device->devStateInfo.gainIndexes.rx4MaxGainIndex;
            }
            else if (chanIdx == 5U)
            {
                rxMinIndex = device->devStateInfo.gainIndexes.rx5MinGainIndex;
                rxMaxIndex = device->devStateInfo.gainIndexes.rx5MaxGainIndex;
            }
            else if (chanIdx == 6U)
            {
                rxMinIndex = device->devStateInfo.gainIndexes.rx6MinGainIndex;
                rxMaxIndex = device->devStateInfo.gainIndexes.rx6MaxGainIndex;
            }
            else
            {
                rxMinIndex = device->devStateInfo.gainIndexes.rx7MinGainIndex;
                rxMaxIndex = device->devStateInfo.gainIndexes.rx7MaxGainIndex;
            }

            if (agcGainRangeCfg->maxGainIndex > rxMaxIndex)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       agcGainRangeCfg->maxGainIndex,
                                       "agcGainRangeCfg->maxGainIndex cannot be larger than Rx max index of device");
                return recoveryAction;
            }
            
            if (agcGainRangeCfg->minGainIndex < rxMinIndex)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       agcGainRangeCfg->minGainIndex,
                                       "agcGainRangeCfg->minGainIndex cannot be smaller than Rx min index of device");
                return recoveryAction;
            }
        }
    }
    
    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_AgcCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                           const adi_adrv904x_AgcCfg_t * const agcCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Valid ranges for the AGC configuration values */
    static const uint8_t  MAX_GAIN_STEP_SIZE  = 31U;
    static const uint8_t MAX_ADCOVLD_LOW_THRESHOLD = 8U;
    static const uint16_t  MAX_THRESHOLD_VAL  = 16383U;
    static const uint8_t HB2_FIRST_STAGE_DURATION_MAX = 63U;
    static const uint8_t HB2_FIRST_STAGE_THRESHOLD_MAX = 7U;
    static const uint8_t FAST_RECOVERY_MAX_INTERVAL = 63U;
    static const uint8_t POWER_TRIGGER_MAX_THRESHOLD = 127U;
    static const uint8_t POWER_TRIGGER_UNDERRANGE_LOW_POWER_MAX_THRESHOLD = 31U;
    static const uint8_t POWER_TRIGGER_OVERRANGE_HIGH_POWER_MAX_THRESHOLD = 15U;
    static const uint8_t DEC_POWER_INPUT_SELECT_MAX = 3U;
    static const uint8_t DEC_POWER_MEAS_DURATION_MAX = 31U;
    static const uint32_t GAIN_UPDATE_COUNTER_MAX = 4194303U;
    static const uint8_t IMMEDIATE_GAIN_CHANGE_MAX_VAL = 3U;
    static const uint8_t MAX_ATTACK_DELAY = 63U;
    static const uint8_t MAX_ADC_RESET_GAIN_STEP = 31U;
    static const uint8_t MAX_PEAK_WAIT_TIME = 3U;
    adi_adrv904x_AgcGainRange_t agcGainRange;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check agcCfg address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, agcCfg);

    ADI_LIBRARY_MEMSET(&agcGainRange, 0, sizeof(adi_adrv904x_AgcGainRange_t));

    /*Check that if requested Rx Channel valid*/
    if(((agcCfg->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || (agcCfg->rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    /* Peak detector config range check */
    if (agcCfg->agcPeak.adcOvldGainStepAttack > MAX_GAIN_STEP_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.adcOvldGainStepAttack,
                               "agcCfg->agcPeak.adcOvldGainStepAttack is out of range");
        return recoveryAction;
    }

    if (agcCfg->agcPeak.adcOvldLowThresh > MAX_ADCOVLD_LOW_THRESHOLD)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.adcOvldLowThresh,
                               "agcCfg->agcPeak.adcOvldLowThresh is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.adcOvldGainStepRecovery > MAX_GAIN_STEP_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.adcOvldGainStepRecovery,
                               "agcCfg->agcPeak.adcOvldGainStepRecovery is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2OverloadDurationCnt > HB2_FIRST_STAGE_DURATION_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2OverloadDurationCnt,
                               "agcCfg->agcPeak.hb2OverloadDurationCnt is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2OverloadThreshCnt > HB2_FIRST_STAGE_THRESHOLD_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2OverloadThreshCnt,
                               "agcCfg->agcPeak.hb2OverloadThreshCnt is out of range");
        return recoveryAction;
    }

    if (agcCfg->agcPeak.hb2HighThresh > MAX_THRESHOLD_VAL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2HighThresh,
                               "agcCfg->agcPeak.hb2HighThresh is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2GainStepAttack > MAX_GAIN_STEP_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2GainStepAttack,
                               "agcCfg->agcPeak.hb2GainStepAttack is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2UnderRangeHighThresh > MAX_THRESHOLD_VAL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2UnderRangeHighThresh,
                               "agcCfg->agcPeak.hb2UnderRangeHighThresh is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2GainStepHighRecovery > MAX_GAIN_STEP_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2GainStepHighRecovery,
                               "agcCfg->agcPeak.hb2GainStepHighRecovery is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2UnderRangeMidThresh > MAX_THRESHOLD_VAL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2UnderRangeMidThresh,
                               "agcCfg->agcPeak.hb2UnderRangeMidThresh is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2GainStepMidRecovery > MAX_GAIN_STEP_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2GainStepMidRecovery,
                               "agcCfg->agcPeak.hb2GainStepMidRecovery is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2UnderRangeLowThresh > MAX_THRESHOLD_VAL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2UnderRangeLowThresh,
                               "agcCfg->agcPeak.hb2UnderRangeLowThesh is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2GainStepLowRecovery > MAX_GAIN_STEP_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2GainStepLowRecovery,
                               "agcCfg->agcPeak.hb2GainStepLowRecovery is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2UnderRangeMidInterval > FAST_RECOVERY_MAX_INTERVAL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2UnderRangeMidInterval,
                               "agcCfg->agcPeak.hb2UnderRangeMidInterval is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2UnderRangeHighInterval > FAST_RECOVERY_MAX_INTERVAL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2UnderRangeHighInterval,
                               "agcCfg->agcPeak.hb2UnderRangeHighInterval is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.enableHb2Overload > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.enableHb2Overload,
                               "agcCfg->agcPeak.enableHb2Overload is out of range");
        return recoveryAction;
    }
        
    if (agcCfg->agcPeak.hb2OverloadPowerMode > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2OverloadPowerMode,
                               "agcCfg->agcPeak.hb2OverloadPowerMode is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeak.hb2OverloadSignalSelection > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.hb2OverloadSignalSelection,
                               "agcCfg->agcPeak.hb2OverloadSignalSelection is out of range");
        return recoveryAction;
    }

    if (agcCfg->agcPeak.adcOvldUpperThreshPeakExceededCnt < 2U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.adcOvldUpperThreshPeakExceededCnt,
                               "ADC Overload Upper Threshold Peak Exceeded Count Between 2 and 255");
        return recoveryAction;
    }

    if (agcCfg->agcPeak.adcOvldLowerThreshPeakExceededCnt < 2U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeak.adcOvldLowerThreshPeakExceededCnt,
                               "ADC Overload Lower Threshold Peak Exceeded Count Between 2 and 255");
        return recoveryAction;
    }

    /* Power measurement detector config range check */
    if (agcCfg->agcPower.underRangeHighPowerThresh > POWER_TRIGGER_MAX_THRESHOLD)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.underRangeHighPowerThresh,
                               "agcCfg->agcPower.underRangeHighPowerThresh is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPower.underRangeHighPowerGainStepRecovery > MAX_GAIN_STEP_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.underRangeHighPowerGainStepRecovery,
                               "agcCfg->agcPower.underRangeHighPowerGainStepRecovery is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPower.underRangeLowPowerThresh > POWER_TRIGGER_UNDERRANGE_LOW_POWER_MAX_THRESHOLD)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.underRangeLowPowerThresh,
                               "agcCfg->agcPower.underRangeLowPowerThresh is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPower.underRangeLowPowerGainStepRecovery > MAX_GAIN_STEP_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.underRangeLowPowerGainStepRecovery,
                               "agcCfg->agcPower.underRangeLowPowerGainStepRecovery is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPower.overRangeLowPowerThresh > POWER_TRIGGER_MAX_THRESHOLD)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.overRangeLowPowerThresh,
                               "agcCfg->agcPower.overRangeLowPowerThresh is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPower.overRangeLowPowerGainStepAttack > MAX_GAIN_STEP_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.overRangeLowPowerGainStepAttack,
                               "agcCfg->agcPower.overRangeLowPowerGainStepAttack is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPower.overRangeHighPowerThresh > POWER_TRIGGER_OVERRANGE_HIGH_POWER_MAX_THRESHOLD)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.overRangeHighPowerThresh,
                               "agcCfg->agcPower.overRangeHighPowerThresh is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPower.overRangeHighPowerGainStepAttack > MAX_GAIN_STEP_SIZE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.overRangeHighPowerGainStepAttack,
                               "agcCfg->agcPower.overRangeHighPowerGainStepAttack is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPower.powerEnableMeasurement > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.powerEnableMeasurement,
                               "agcCfg->agcPower.powerEnableMeasurement is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPower.powerInputSelect > DEC_POWER_INPUT_SELECT_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.powerInputSelect,
                               "agcCfg->agcPower.powerInputSelect is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPower.powerMeasurementDuration > DEC_POWER_MEAS_DURATION_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPower.powerMeasurementDuration,
                               "agcCfg->agcPower.powerMeasurementDuration is out of range");
        return recoveryAction;
    }
    
    agcGainRange.rxChannelMask = agcCfg->rxChannelMask;
    agcGainRange.maxGainIndex = agcCfg->agcRxMaxGainIndex;
    agcGainRange.minGainIndex = agcCfg->agcRxMinGainIndex;
    
    recoveryAction = adrv904x_AgcGainRangeCfgRangeCheck(device, &agcGainRange);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, &agcGainRange, "Invalid Rx AGC Gain range");
        return recoveryAction;
    }
    
    if (agcCfg->agcGainUpdateCounter > GAIN_UPDATE_COUNTER_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcGainUpdateCounter,
                               "agcCfg->agcGainUpdateCounter is out of range");
        return recoveryAction;
    }

    if (agcCfg->agcChangeGainIfThreshHigh > IMMEDIATE_GAIN_CHANGE_MAX_VAL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcChangeGainIfThreshHigh,
                               "agcCfg->agcChangeGainIfThreshHigh is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcEnableFastRecoveryLoop > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcEnableFastRecoveryLoop,
                               "agcCfg->agcEnableFastRecoveryLoop is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcPeakThreshGainControlMode > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeakThreshGainControlMode,
                               "agcCfg->agcPeakThreshGainControlMode is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcLowThreshPreventGainInc > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcLowThreshPreventGainInc,
                               "agcCfg->agcLowThreshPreventGainInc is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcResetOnRxon > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcResetOnRxon,
                               "agcCfg->agcResetOnRxon is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcRxAttackDelay > MAX_ATTACK_DELAY)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcRxAttackDelay,
                               "agcCfg->agcRxAttackDelay is out of range");
        return recoveryAction;
    }
    
    if (agcCfg->agcAdcResetGainStep > MAX_ADC_RESET_GAIN_STEP)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcAdcResetGainStep,
                               "agcCfg->agcAdcResetGainStep is out of range");
        return recoveryAction;
    }

    if (agcCfg->agcPeakWaitTime > MAX_PEAK_WAIT_TIME)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcCfg->agcPeakWaitTime,
                               "agcCfg->agcPeakWaitTime is out of range");
        return recoveryAction;
    }
    
    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_AgcDualBandCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                                   const adi_adrv904x_AgcDualBandCfg_t * const agcDualBandCfg) 
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Valid ranges for the AGC configuration values */
    static const uint8_t  MAX_POWER_MARGIN  = 31U;
    static const uint8_t  POWER_MEASUREMENT_DURATION  = 31U;
    static const uint8_t  DUALBAND_MAX_GAIN_INDEX  = 3U;
    adi_adrv904x_AgcGainRange_t agcGainRange;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check agcCfg address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, agcDualBandCfg);

    ADI_LIBRARY_MEMSET(&agcGainRange, 0, sizeof(adi_adrv904x_AgcGainRange_t));

    /*Check that if requested Rx Channel valid*/
    if (((agcDualBandCfg->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || (agcDualBandCfg->rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcDualBandCfg->rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    if (agcDualBandCfg->agcDualBandEnable > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcDualBandCfg->agcDualBandEnable,
                               "agcDualBandCfg->agcDualBandEnable is out of range");
        return recoveryAction;
    }

    agcGainRange.rxChannelMask = agcDualBandCfg->rxChannelMask;
    agcGainRange.maxGainIndex = agcDualBandCfg->agcRxDualbandExtTableUpperIndex;
    agcGainRange.minGainIndex = agcDualBandCfg->agcRxDualbandExtTableLowerIndex;

    recoveryAction = adrv904x_AgcGainRangeCfgRangeCheck(device, &agcGainRange);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, &agcGainRange, "Invalid Rx Dual Band AGC Gain range");
        return recoveryAction;
    }

    if (agcDualBandCfg->agcDualbandPwrMargin > MAX_POWER_MARGIN)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcDualBandCfg->agcDualbandPwrMargin,
                               "agcDualBandCfg->agcDualbandPwrMargin is out of range");
        return recoveryAction;
    }

    if (agcDualBandCfg->decPowerDdcMeasurementDuration > POWER_MEASUREMENT_DURATION)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcDualBandCfg->decPowerDdcMeasurementDuration,
                               "agcDualBandCfg->decPowerDdcMeasurementDuration is out of range");
        return recoveryAction;
    }

    if (agcDualBandCfg->agcDualBandMaxGainIndex > DUALBAND_MAX_GAIN_INDEX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcDualBandCfg->agcDualBandMaxGainIndex,
                               "agcDualBandCfg->agcDualBandMaxGainIndex is out of range");
        return recoveryAction;
    }

    if (agcDualBandCfg->enableGainCompensationForExtLna > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               agcDualBandCfg->enableGainCompensationForExtLna,
                               "agcDualBandCfg->enableGainCompensationForExtLna is out of range");
        return recoveryAction;
    }

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
    return recoveryAction;

}