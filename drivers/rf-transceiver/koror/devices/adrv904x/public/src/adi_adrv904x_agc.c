/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_agc.c
* \brief Contains AGC features related function implementation defined in
* adi_adrv904x_agc.h
*
* ADRV904X API Version: 2.9.0.4
*/

#include "adi_adrv904x_agc.h"
#include "adi_adrv904x_rx.h"

#include "../../private/bf/adrv904x_bf_rx_funcs_types.h"
#include "../../private/bf/adrv904x_bf_rx_funcs.h"
#include "../../private/bf/adrv904x_bf_rx_ddc_types.h"
#include "../../private/bf/adrv904x_bf_rx_ddc.h"
#include "../../private/bf/adrv904x_bf_rx_dig_types.h"
#include "../../private/bf/adrv904x_bf_rx_dig.h"
#include "../../private/bf/adrv904x_bf_tdr_dpath_top.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"
#include "../../private/include/adrv904x_agc.h"
#include "../../private/include/adrv904x_rx.h"
#include "../../private/include/adrv904x_gpio.h"


#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_AGC

static const adrv904x_BfRxFuncsChanAddr_e rxChanIdToRxFuncsChanAddr[] = 
{
    ADRV904X_BF_SLICE_RX_0__RX_FUNCS,
    ADRV904X_BF_SLICE_RX_1__RX_FUNCS,
    ADRV904X_BF_SLICE_RX_2__RX_FUNCS,
    ADRV904X_BF_SLICE_RX_3__RX_FUNCS,
    ADRV904X_BF_SLICE_RX_4__RX_FUNCS,
    ADRV904X_BF_SLICE_RX_5__RX_FUNCS,
    ADRV904X_BF_SLICE_RX_6__RX_FUNCS,
    ADRV904X_BF_SLICE_RX_7__RX_FUNCS
};

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcGainIndexRangeSet(adi_adrv904x_Device_t * const device,
                                                                   const adi_adrv904x_AgcGainRange_t agcGainRange[],
                                                                   const uint32_t numOfAgcRangeCfgs)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, agcGainRange, cleanup);
    
    if (numOfAgcRangeCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numOfAgcRangeCfgs, "Invalid Number of AGC Range Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numOfAgcRangeCfgs; ++cfgIdx)
    {
        recoveryAction = adrv904x_AgcGainRangeCfgRangeCheck(device, &agcGainRange[cfgIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AGC Min/Max Gain set Range Check Error Reported");
            goto cleanup;
        }
    }

    /* Write out the configurations */
    for (cfgIdx = 0U; cfgIdx < numOfAgcRangeCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(agcGainRange[cfgIdx].rxChannelMask, chanSel))
            {
                recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &rxFuncsBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid Rx Channel used to determine rx func address");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcMaximumGainIndex_BfSet(device, NULL, rxFuncsBaseAddr, agcGainRange[cfgIdx].maxGainIndex);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing AGC max gain index");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcMinimumGainIndex_BfSet(device, NULL, rxFuncsBaseAddr, agcGainRange[cfgIdx].minGainIndex);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing AGC min gain index");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcGainIndexRangeGet(adi_adrv904x_Device_t * const device,
                                                                   const adi_adrv904x_RxChannels_e rxChannel,
                                                                   adi_adrv904x_AgcGainRange_t * const agcGainConfigReadback)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, agcGainConfigReadback, cleanup);
    
    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel selection");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, rxChannel, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid Rx Channel used to determine rx func address");
        goto cleanup;
    }
                
    recoveryAction = adrv904x_RxFuncs_AgcMaximumGainIndex_BfGet(device, NULL, rxFuncsBaseAddr, &agcGainConfigReadback->maxGainIndex);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading AGC max gain index");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcMinimumGainIndex_BfGet(device, NULL, rxFuncsBaseAddr, &agcGainConfigReadback->minGainIndex);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading AGC min gain index");
        goto cleanup;
    }
    
    agcGainConfigReadback->rxChannelMask = (uint32_t)rxChannel;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcCfgSet(adi_adrv904x_Device_t * const device,
                                                        const adi_adrv904x_AgcCfg_t agcConfig[],
                                                        const uint32_t numOfAgcCfgs)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    adrv904x_BfRxChanAddr_e rxDigBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_DIG;
    adrv904x_BfTdrDpathTopChanAddr_e rxAnalogChannelIBitfieldAddr = ADRV904X_BF_SLICE_RX_0__ADC32_ANALOG_REGS_TDR_DPATH_TOP_0_;
    adrv904x_BfTdrDpathTopChanAddr_e rxAnalogChannelQBitfieldAddr = ADRV904X_BF_SLICE_RX_0__ADC32_ANALOG_REGS_TDR_DPATH_TOP_1_;
    adi_adrv904x_AgcGainRange_t agcGainRange;
    uint8_t rxRefClkCycles = 0U;
    uint8_t hb2OutClkDividerVal = 0U;
    uint8_t bfValue = 0U;
    adi_adrv904x_RxDecimatedPowerCfg_t rxDecPowerCfg;
    uint16_t preHb2Threshold = 0U;
    static const uint8_t ADC_OVLD_PEAK_COUNT_SPACING = 1U;
    static const uint8_t ADC_OVLD_PEAK_COUNT_THRESHOLD = 4U;
    static const uint32_t ADC_OVLD_PEAK_COUNT_EXPIRATION = 10U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, agcConfig, cleanup);

    ADI_LIBRARY_MEMSET(&agcGainRange, 0, sizeof(adi_adrv904x_AgcGainRange_t));
    ADI_LIBRARY_MEMSET(&rxDecPowerCfg, 0, sizeof(adi_adrv904x_RxDecimatedPowerCfg_t));
    
    if (numOfAgcCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numOfAgcCfgs, "Invalid Number of AGC Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numOfAgcCfgs; ++cfgIdx)
    {
        recoveryAction = adrv904x_AgcCfgRangeCheck(device, &agcConfig[cfgIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AGC Range Check Error Reported");
            goto cleanup;
        }
    }

    /* Write out the configurations */
    for (cfgIdx = 0U; cfgIdx < numOfAgcCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(agcConfig[cfgIdx].rxChannelMask, chanSel))
            {
                recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &rxFuncsBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Rx funcs address get issue");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxAnalogBitfieldAddressGet(device,
                                                                     (adi_adrv904x_RxChannels_e)(chanSel),
                                                                     &rxAnalogChannelIBitfieldAddr,
                                                                     &rxAnalogChannelQBitfieldAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Rx analog address get issue");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &rxDigBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Rx digital address get issue");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxDig_ReferenceClockCycles_BfGet(device, NULL, rxDigBaseAddr, &rxRefClkCycles);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Error while reading reference clock cycles");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxFuncs_AgcDelayCounterBaseRate_BfSet(device, NULL, rxFuncsBaseAddr, rxRefClkCycles);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing agc delay counter base rate");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcPeakWaitTime_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeakWaitTime);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing agc peak wait delay");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxFuncs_AgcOvrgResetpdHighCount_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeakWaitTime);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing agc reset pd high count");
                    goto cleanup;
                }

                agcGainRange.maxGainIndex = agcConfig[cfgIdx].agcRxMaxGainIndex;
                agcGainRange.minGainIndex = agcConfig[cfgIdx].agcRxMinGainIndex;
                agcGainRange.rxChannelMask = chanSel;
                
                recoveryAction = adi_adrv904x_AgcGainIndexRangeSet(device, &agcGainRange, 1U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting agc max/min gain");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxFuncs_AgcGainUpdateCounter_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcGainUpdateCounter);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing agc gain update counter");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcAttackDelay_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcRxAttackDelay);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing agc attack delay");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcEnableFastRecoveryLoop_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcEnableFastRecoveryLoop);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing fast recovery enable bit");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxFuncs_AgcLowThsPreventGainInc_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcLowThreshPreventGainInc);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing prevent gain inc when crossing low threshold bit");
                    goto cleanup;
                }
                
                /* Bit0 of agcChangeGainIfThreshHigh is used for ADC overload */
                bfValue = agcConfig[cfgIdx].agcChangeGainIfThreshHigh & 0x01U;
                recoveryAction = adrv904x_RxFuncs_AgcChangeGainIfUlbthHigh_BfSet(device, NULL, rxFuncsBaseAddr, bfValue);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing immediate gain change enable bit for ADC overload");
                    goto cleanup;
                }
                
                /* Bit1 of agcChangeGainIfThreshHigh is used for HB2 peak detector */
                bfValue = (agcConfig[cfgIdx].agcChangeGainIfThreshHigh >> 1U) & 0x01U;
                recoveryAction = adrv904x_RxFuncs_AgcChangeGainIfAdcovrgHigh_BfSet(device, NULL, rxFuncsBaseAddr, bfValue);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing immediate gain change enable bit for Hb2 overload");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcPeakThresholdGainControlMode_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeakThreshGainControlMode);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing peak threshold gain control mode");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcResetOnRxon_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcResetOnRxon);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing reset on rx on bit");
                    goto cleanup;
                }
                    
                recoveryAction = adrv904x_RxFuncs_AgcSlowLoopSettlingDelay_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcSlowLoopSettlingDelay);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing slow loop settling delay field");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcAdcResetGainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcAdcResetGainStep);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing agc reset gain step field");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcUrangeInterval0_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2UnderRangeLowInterval);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing hb2 urange low interval");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcUrangeInterval1Mult_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2UnderRangeMidInterval);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing hb2 urange mid interval");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcUrangeInterval2Mult_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2UnderRangeHighInterval);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing hb2 urange high interval");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TdrDpathTop_FlashThresh_BfSet(device,
                                                                        NULL,
                                                                        rxAnalogChannelIBitfieldAddr,
                                                                        agcConfig[cfgIdx].agcPeak.adcOvldLowThresh);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting ADC overload low threshold for I channel");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TdrDpathTop_FlashThresh_BfSet(device,
                                                                        NULL,
                                                                        rxAnalogChannelQBitfieldAddr,
                                                                        agcConfig[cfgIdx].agcPeak.adcOvldLowThresh);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting ADC overload low threshold for Q channel");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcUlbThresholdExceededCounter_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.adcOvldUpperThreshPeakExceededCnt);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing ADC overload high threshold counter");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcLlbThresholdExceededCounter_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.adcOvldLowerThreshPeakExceededCnt);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing ADC overload low threshold counter");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcUlbGainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.adcOvldGainStepAttack);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing ADC overload high gain step");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcLlbGainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.adcOvldGainStepRecovery);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing ADC overload low gain step");
                    goto cleanup;
                }
                
                if (agcConfig[cfgIdx].agcPeak.enableHb2Overload == 1U) 
                {

                    recoveryAction = adrv904x_RxDig_Hb2OutClkDivideRatio_BfGet(device, NULL, rxDigBaseAddr, &hb2OutClkDividerVal);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Error while reading hb2 out clock divider value");
                        goto cleanup;
                    }
                    
                    recoveryAction = adrv904x_RxDig_PeakDetectClkDivideRatio_BfSet(device, NULL, rxDigBaseAddr, hb2OutClkDividerVal);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Error while setting peak detector clock divider value");
                        goto cleanup;
                    }
                    
                    recoveryAction = adrv904x_RxDig_PeakDetectClkEnable_BfSet(device, NULL, rxDigBaseAddr, ADI_ENABLE);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Error while enabling peak detector clock");
                        goto cleanup;
                    }
                }

                recoveryAction = adrv904x_RxHb2OverloadCfgSet(device,
                                                              (adi_adrv904x_RxChannels_e)(chanSel),
                                                              agcConfig[cfgIdx].agcPeak.enableHb2Overload,
                                                              agcConfig[cfgIdx].agcPeak.hb2OverloadSignalSelection,
                                                              agcConfig[cfgIdx].agcPeak.hb2OverloadPowerMode);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 config");
                    goto cleanup;
                }

                /* Set prethreshold to the half of lowest underrange threshold */
                if (agcConfig[cfgIdx].agcEnableFastRecoveryLoop == 0U)
                {
                    preHb2Threshold = agcConfig[cfgIdx].agcPeak.hb2UnderRangeHighThresh >> 1U;
                }
                else
                {
                    preHb2Threshold = agcConfig[cfgIdx].agcPeak.hb2UnderRangeLowThresh >> 1U;
                }
                
                recoveryAction = adrv904x_RxFuncs_OverloadThAgcPre_BfSet(device, NULL, rxFuncsBaseAddr, preHb2Threshold);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting prethreshold for Hb2");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_PeakWindowSizeAgc_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2OverloadDurationCnt);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 second stage peak window");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_PeakSampleCountAgc_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2OverloadThreshCnt);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 second stage peak threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_OverloadThAgcHigh_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2HighThresh);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 high threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_OverloadThAgcInt0Low_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2UnderRangeLowThresh);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 underrange low threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_OverloadThAgcInt1Low_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2UnderRangeMidThresh);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 underrange mid threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_OverloadThAgcLow_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2UnderRangeHighThresh);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 underrange high threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcAdcHighOvrgExceededCounter_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2UpperThreshPeakExceededCnt);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 high threshold counter");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcAdcLowOvrgExceededCounter_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2UnderRangeHighThreshExceededCnt);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 underrange high threshold counter");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcOvrgLowGainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2GainStepHighRecovery);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 underrange high threshold gain step");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcOvrgLowInt0GainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2GainStepLowRecovery);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 underrange low threshold gain step");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcOvrgLowInt1GainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2GainStepMidRecovery);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 underrange mid threshold gain step");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcOvrgHighGainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2GainStepAttack);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 high threshold gain step");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcAdcovrgLowInt1Counter_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2UnderRangeMidThreshExceededCnt);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 underrange mid threshold counter");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcAdcovrgLowInt0Counter_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPeak.hb2UnderRangeLowThreshExceededCnt);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing HB2 underrange low threshold counter");
                    goto cleanup;
                }
                
                if (agcConfig[cfgIdx].agcPower.powerEnableMeasurement == 1U)
                {
                    rxDecPowerCfg.decPowerControl = ADI_ADRV904X_DEC_POWER_AGC_MEAS;
                    rxDecPowerCfg.measBlockSelectMask = ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK;
                    rxDecPowerCfg.powerInputSelect = agcConfig[cfgIdx].agcPower.powerInputSelect;
                    rxDecPowerCfg.powerMeasurementDuration = agcConfig[cfgIdx].agcPower.powerMeasurementDuration;
                    rxDecPowerCfg.rxChannelMask = chanSel;

                    recoveryAction = adi_adrv904x_RxDecimatedPowerCfgSet(device, &rxDecPowerCfg, 1U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing decimated power configuration");
                        goto cleanup;
                    }
                }
                else
                {
                    recoveryAction = adi_adrv904x_RxDecimatedPowerCfgGet(device,
                                                                         (adi_adrv904x_RxChannels_e)(chanSel),
                                                                         ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK,
                                                                         &rxDecPowerCfg);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Rx decimated power configuration");
                        goto cleanup;
                    }
                    
                    if (rxDecPowerCfg.decPowerControl == ADI_ADRV904X_DEC_POWER_AGC_MEAS)
                    {
                        /* Main path measurement block is being used for AGC - we should disable it */
                        rxDecPowerCfg.decPowerControl = ADI_ADRV904X_DEC_POWER_MEAS_OFF;
                        rxDecPowerCfg.measBlockSelectMask = ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK;
                        rxDecPowerCfg.powerInputSelect = 0;
                        rxDecPowerCfg.powerMeasurementDuration = 0;
                        rxDecPowerCfg.rxChannelMask = chanSel;

                        recoveryAction = adi_adrv904x_RxDecimatedPowerCfgSet(device, &rxDecPowerCfg, 1U);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing Rx decimated power configuration");
                            goto cleanup;
                        }
                    }
                }

                recoveryAction = adrv904x_RxFuncs_AgcLower0Threshold_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPower.underRangeHighPowerThresh);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dec power underrange high threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcLower1Threshold_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPower.underRangeLowPowerThresh);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dec power underrange low threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcLower0ThresholdExceededGainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPower.underRangeHighPowerGainStepRecovery);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dec power underrange high threshold gain step");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcLower1ThresholdExceededGainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPower.underRangeLowPowerGainStepRecovery);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dec power underrange low threshold gain step");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcLockLevel_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPower.overRangeLowPowerThresh);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dec power overrange low threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcUpper1Threshold_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPower.overRangeHighPowerThresh);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dec power overrange high threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcUpper1ThresholdExceededGainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPower.overRangeHighPowerGainStepAttack);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dec power overrange high threshold gain step");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcUpper0ThresholdExceededGainStep_BfSet(device, NULL, rxFuncsBaseAddr, agcConfig[cfgIdx].agcPower.overRangeLowPowerGainStepAttack);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dec power overrange low threshold gain step");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxFuncs_PeakCountSpacingAgcHigh_BfSet(device, NULL, rxFuncsBaseAddr, ADC_OVLD_PEAK_COUNT_SPACING);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing peak count spacing for ADC ovld high");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_PeakCountSpacingAgcLow_BfSet(device, NULL, rxFuncsBaseAddr, ADC_OVLD_PEAK_COUNT_SPACING);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing peak count spacing for ADC ovld low");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxFuncs_PeakCountThresholdAgcHigh_BfSet(device, NULL, rxFuncsBaseAddr, ADC_OVLD_PEAK_COUNT_THRESHOLD);
                if(recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing peak count threshold for ADC ovld high");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_PeakCountThresholdAgcLow_BfSet(device, NULL, rxFuncsBaseAddr, ADC_OVLD_PEAK_COUNT_THRESHOLD);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing peak count threshold for ADC ovld low");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_PeakCountExpirationAgc_BfSet(device, NULL, rxFuncsBaseAddr, ADC_OVLD_PEAK_COUNT_EXPIRATION);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing peak count expiration for ADC ovld");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcCfgGet(adi_adrv904x_Device_t * const device,
                                                        const adi_adrv904x_RxChannels_e rxChannel,
                                                        adi_adrv904x_AgcCfg_t * const agcConfigReadBack)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    adrv904x_BfTdrDpathTopChanAddr_e rxAnalogChannelIBitfieldAddr = ADRV904X_BF_SLICE_RX_0__ADC32_ANALOG_REGS_TDR_DPATH_TOP_0_;
    adrv904x_BfTdrDpathTopChanAddr_e rxAnalogChannelQBitfieldAddr = ADRV904X_BF_SLICE_RX_0__ADC32_ANALOG_REGS_TDR_DPATH_TOP_1_;
    adi_adrv904x_AgcGainRange_t agcGainRange;
    uint8_t bfValue = 0U;
    adi_adrv904x_RxDecimatedPowerCfg_t rxDecPowerCfg;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, agcConfigReadBack, cleanup);

    ADI_LIBRARY_MEMSET(&agcGainRange, 0, sizeof(adi_adrv904x_AgcGainRange_t));
    ADI_LIBRARY_MEMSET(&rxDecPowerCfg, 0, sizeof(adi_adrv904x_RxDecimatedPowerCfg_t));

    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel selection");
        goto cleanup;
    }

    /* Read back Rx func bitfield address for selected channel */
    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, rxChannel, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Rx funcs address get issue");
        goto cleanup;
    }

    /* Read back Rx analog bitfield address for selected channel */
    recoveryAction = adrv904x_RxAnalogBitfieldAddressGet(device,
                                                         rxChannel,
                                                         &rxAnalogChannelIBitfieldAddr,
                                                         &rxAnalogChannelQBitfieldAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Rx analog address get issue");
        goto cleanup;
    }
    
    agcConfigReadBack->rxChannelMask =  (uint32_t)rxChannel;
    
    recoveryAction = adrv904x_RxFuncs_AgcPeakWaitTime_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeakWaitTime);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading agc peak wait delay");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_AgcGainIndexRangeGet(device, rxChannel, &agcGainRange);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading agc max/min gain");
        goto cleanup;
    }

    agcConfigReadBack->agcRxMinGainIndex = agcGainRange.minGainIndex;
    agcConfigReadBack->agcRxMaxGainIndex = agcGainRange.maxGainIndex;

    recoveryAction = adrv904x_RxFuncs_AgcGainUpdateCounter_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcGainUpdateCounter);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading agc gain update counter");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcAttackDelay_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcRxAttackDelay);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading agc attack delay");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcEnableFastRecoveryLoop_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcEnableFastRecoveryLoop);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading fast recovery enable bit");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcLowThsPreventGainInc_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcLowThreshPreventGainInc);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading prevent gain inc when crossing low threshold bit");
        goto cleanup;
    }

    /* Bit0 of agcChangeGainIfThreshHigh is used for ADC overload */
    bfValue = 0U;
    agcConfigReadBack->agcChangeGainIfThreshHigh = 0U;
    recoveryAction = adrv904x_RxFuncs_AgcChangeGainIfUlbthHigh_BfGet(device, NULL, rxFuncsBaseAddr, &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading immediate gain change enable bit for ADC overload");
        goto cleanup;
    }
    agcConfigReadBack->agcChangeGainIfThreshHigh |= (bfValue & 0x01U);

    /* Bit1 of agcChangeGainIfThreshHigh is used for HB2 peak detector */
    recoveryAction = adrv904x_RxFuncs_AgcChangeGainIfAdcovrgHigh_BfGet(device, NULL, rxFuncsBaseAddr, &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading immediate gain change enable bit for Hb2 overload");
        goto cleanup;
    }
    agcConfigReadBack->agcChangeGainIfThreshHigh |= ((bfValue & 0x01U) << 1U);

    recoveryAction = adrv904x_RxFuncs_AgcPeakThresholdGainControlMode_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeakThreshGainControlMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peak threshold gain control mode");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcResetOnRxon_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcResetOnRxon);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading reset on rx on bit");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcSlowLoopSettlingDelay_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcSlowLoopSettlingDelay);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading slow loop settling delay field");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcAdcResetGainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcAdcResetGainStep);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading agc reset gain step field");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcUrangeInterval0_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2UnderRangeLowInterval);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading hb2 urange low interval");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcUrangeInterval1Mult_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2UnderRangeMidInterval);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading hb2 urange mid interval");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcUrangeInterval2Mult_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2UnderRangeHighInterval);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading hb2 urange high interval");
        goto cleanup;
    }

    recoveryAction = adrv904x_TdrDpathTop_FlashThresh_BfGet(device,
                                                            NULL,
                                                            rxAnalogChannelIBitfieldAddr,
                                                            &agcConfigReadBack->agcPeak.adcOvldLowThresh);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading ADC overload low threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcUlbThresholdExceededCounter_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.adcOvldUpperThreshPeakExceededCnt);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading ADC overload high threshold counter");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcLlbThresholdExceededCounter_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.adcOvldLowerThreshPeakExceededCnt);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading ADC overload low threshold counter");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcUlbGainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.adcOvldGainStepAttack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading ADC overload high gain step");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcLlbGainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.adcOvldGainStepRecovery);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading ADC overload low gain step");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxHb2OverloadCfgGet(device,
                                                  rxChannel,
                                                  &agcConfigReadBack->agcPeak.enableHb2Overload,
                                                  &agcConfigReadBack->agcPeak.hb2OverloadSignalSelection,
                                                  &agcConfigReadBack->agcPeak.hb2OverloadPowerMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 config");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_PeakWindowSizeAgc_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2OverloadDurationCnt);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 second stage peak window");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_PeakSampleCountAgc_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2OverloadThreshCnt);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 second stage peak threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_OverloadThAgcHigh_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2HighThresh);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading writing HB2 high threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_OverloadThAgcInt0Low_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2UnderRangeLowThresh);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 underrange low threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_OverloadThAgcInt1Low_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2UnderRangeMidThresh);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 underrange mid threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_OverloadThAgcLow_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2UnderRangeHighThresh);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 underrange high threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcAdcHighOvrgExceededCounter_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2UpperThreshPeakExceededCnt);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 high threshold counter");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcAdcLowOvrgExceededCounter_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2UnderRangeHighThreshExceededCnt);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 underrange high threshold counter");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcOvrgLowGainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2GainStepHighRecovery);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 underrange high threshold gain step");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcOvrgLowInt0GainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2GainStepLowRecovery);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 underrange low threshold gain step");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcOvrgLowInt1GainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2GainStepMidRecovery);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 underrange mid threshold gain step");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcOvrgHighGainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2GainStepAttack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 high threshold gain step");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcAdcovrgLowInt1Counter_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2UnderRangeMidThreshExceededCnt);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 underrange mid threshold counter");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcAdcovrgLowInt0Counter_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPeak.hb2UnderRangeLowThreshExceededCnt);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading HB2 underrange low threshold counter");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_RxDecimatedPowerCfgGet(device, rxChannel, ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK, &rxDecPowerCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading decimated power configuration");
        goto cleanup;
    }

    if (rxDecPowerCfg.decPowerControl == ADI_ADRV904X_DEC_POWER_AGC_MEAS)
    {
        agcConfigReadBack->agcPower.powerEnableMeasurement = 1U;
    }
    else
    {
        agcConfigReadBack->agcPower.powerEnableMeasurement = 0U;
    }

    agcConfigReadBack->agcPower.powerInputSelect = rxDecPowerCfg.powerInputSelect;
    agcConfigReadBack->agcPower.powerMeasurementDuration = rxDecPowerCfg.powerMeasurementDuration;

    recoveryAction = adrv904x_RxFuncs_AgcLower0Threshold_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPower.underRangeHighPowerThresh);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dec power underrange high threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcLower1Threshold_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPower.underRangeLowPowerThresh);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dec power underrange low threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcLower0ThresholdExceededGainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPower.underRangeHighPowerGainStepRecovery);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dec power underrange high threshold gain step");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcLower1ThresholdExceededGainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPower.underRangeLowPowerGainStepRecovery);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dec power underrange low threshold gain step");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcLockLevel_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPower.overRangeLowPowerThresh);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dec power overrange low threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcUpper1Threshold_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPower.overRangeHighPowerThresh);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dec power overrange high threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcUpper1ThresholdExceededGainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPower.overRangeHighPowerGainStepAttack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dec power overrange high threshold gain step");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcUpper0ThresholdExceededGainStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcConfigReadBack->agcPower.overRangeLowPowerGainStepAttack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dec power overrange low threshold gain step");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcFreezeSet(adi_adrv904x_Device_t* const device,
                                                           const uint32_t rxChannelMask,
                                                           const uint32_t freezeEnable)
 {
          adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
     uint32_t chanIdx = 0U;
     uint32_t chanSel = 0U;
         
     ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

     ADI_ADRV904X_API_ENTRY(&device->common);
    
     if ((rxChannelMask > (uint32_t) ADI_ADRV904X_RX_MASK_ALL) || (rxChannelMask == (uint32_t) ADI_ADRV904X_RXOFF))
     {
         recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
         ADI_PARAM_ERROR_REPORT(&device->common,
                                recoveryAction,
                                rxChannelMask,
                                "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
         goto cleanup;
     }

     for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
     {
         chanSel = 1U << chanIdx;
         if (ADRV904X_BF_EQUAL(rxChannelMask, chanSel) == 0)
         {
             /* Do nothing to this channel. */
             continue;
         }

         recoveryAction = adrv904x_RxFuncs_AgcSlowloopFreezeEnable_BfSet(device,
                                                                         NULL,
                                                                         rxChanIdToRxFuncsChanAddr[chanIdx],
                                                                         ADRV904X_BF_EQUAL(freezeEnable, chanSel) ? 1U : 0U);
         
         if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
         {
             ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting slow loop freeze enable bitfield");
             goto cleanup;
         }         
     }
     
cleanup :
     ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);    
        
 }

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcFreezeGet(adi_adrv904x_Device_t* const device,
                                                           uint32_t* const freezeEnable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint8_t bfVal = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, freezeEnable, cleanup);
    *freezeEnable = 0U;
    
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
    {
        recoveryAction = adrv904x_RxFuncs_AgcSlowloopFreezeEnable_BfGet(device, NULL, rxChanIdToRxFuncsChanAddr[chanIdx], &bfVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting bitfield");
            goto cleanup;
        }
        
        *freezeEnable |= (bfVal << chanIdx);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);    
    
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcFreezeOnGpioSet(adi_adrv904x_Device_t* const device,
                                                                 const uint32_t rxChannels,
                                                                 const adi_adrv904x_GpioPinSel_e gpioPin,
                                                                 const uint8_t enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioPinSel_e gpioPinToSet = gpioPin;
    adi_adrv904x_Channels_e rxChan = ADI_ADRV904X_CH0;
    uint8_t i = 0;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    if ((rxChannels > (uint32_t) ADI_ADRV904X_RX_MASK_ALL) || (rxChannels == (uint32_t) ADI_ADRV904X_RXOFF))        
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannels,
                               "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    if (enable != ADI_ENABLE && enable != ADI_DISABLE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               enable,
                               "Enable must be ADI_ENABLE or ADI_DISABLE");
        goto cleanup;
    }
    
    if (enable == ADI_ENABLE) 
    {
        if ((gpioPinToSet < ADI_ADRV904X_GPIO_01) || (gpioPinToSet >= ADI_ADRV904X_GPIO_INVALID))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   gpioPinToSet,
                                   "Invalid GPIO pin. Note ADI_ADRV904X_GPIO_00 cannot be used to control AGC loop");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_GpioSignalSet(device,
                                                gpioPinToSet,
                                                ADI_ADRV904X_GPIO_SIGNAL_AGC_SLOWLOOP_FREEZE_ENABLE,
                                                rxChannels);
    
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Call to adrv904x_GpioSignalSet failed for AGC freeze GPIO");
            goto cleanup;
        }    
    }
    else if (enable == ADI_DISABLE)
    {
        /* To disable Freeze-on-GPIO we need to query for the currently configured GPIO on a channel by channel basis */
        for (i = 0; i < ADI_ADRV904X_MAX_CHANNELS; i++)
        {
            rxChan = (adi_adrv904x_Channels_e) (1 << i);
            if ((rxChan & rxChannels) == 0)
            {
                /* Channel not in rxChannels. Skip it. */
                continue;
            }
            
            /* Find the currently configured GPIO for AGC-freeze this rxChan */
            recoveryAction = adrv904x_GpioSignalFind(device,
                                                     &gpioPinToSet,
                                                     ADI_ADRV904X_GPIO_SIGNAL_AGC_SLOWLOOP_FREEZE_ENABLE,
                                                     rxChan);
        
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                                     recoveryAction,
                                     "Failed to find GPIO_SIGNAL_AGC_SLOWLOOP_FREEZE_ENABLE for Rx channel");
                goto cleanup;
            }
            
            if (gpioPinToSet == ADI_ADRV904X_GPIO_INVALID)
            {
                /* Channel already has AGC-freeze-on-GPIO disabled. Skip it. */
                continue;
            }
        
            recoveryAction = adrv904x_GpioSignalRelease(device,
                                                        gpioPinToSet,
                                                        ADI_ADRV904X_GPIO_SIGNAL_AGC_SLOWLOOP_FREEZE_ENABLE,
                                                        rxChannels);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                                     recoveryAction,
                                     "Failed to release GPIO_SIGNAL_AGC_SLOWLOOP_FREEZE_ENABLE signal");
                goto cleanup;
            }    
        }
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);    
}
    

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcFreezeOnGpioGet(adi_adrv904x_Device_t* const device,
                                                                 const adi_adrv904x_RxChannels_e rxChannel,
                                                                 adi_adrv904x_GpioPinSel_e* const gpioPin)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioPin, cleanup);
    *gpioPin = ADI_ADRV904X_GPIO_INVALID;
    
    if ((rxChannel != (uint32_t)ADI_ADRV904X_RX0) &&
        (rxChannel != (uint32_t)ADI_ADRV904X_RX1) &&
        (rxChannel != (uint32_t)ADI_ADRV904X_RX2) &&
        (rxChannel != (uint32_t)ADI_ADRV904X_RX3) &&
        (rxChannel != (uint32_t)ADI_ADRV904X_RX4) &&
        (rxChannel != (uint32_t)ADI_ADRV904X_RX5) &&
        (rxChannel != (uint32_t)ADI_ADRV904X_RX6) &&
        (rxChannel != (uint32_t)ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannel,
                               "Invalid Rx channel is selected. Valid values are Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    recoveryAction = adrv904x_GpioSignalFind(device,
                                             gpioPin,
                                             ADI_ADRV904X_GPIO_SIGNAL_AGC_SLOWLOOP_FREEZE_ENABLE,
                                             (adi_adrv904x_Channels_e) rxChannel);
        
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        *gpioPin = ADI_ADRV904X_GPIO_INVALID;
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Failed to find GPIO for AGC_SLOWLOOP_FREEZE_ENABLE for Rx channel");
        goto cleanup;
    }    

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);    
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcReset(adi_adrv904x_Device_t* const device,
                                                       const uint32_t rxChannelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    if (((rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || (rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(rxChannelMask, chanSel))
        {
            recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &rxFuncsBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Rx Channel used to determine rx func address");
                goto cleanup;
            }
            
            recoveryAction = adrv904x_RxFuncs_AgcResetCounters_BfSet(device, NULL, rxFuncsBaseAddr, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting AGC reset counters bit");
                goto cleanup;
            }
            
            recoveryAction = adrv904x_RxFuncs_AgcResetCounters_BfSet(device, NULL, rxFuncsBaseAddr, 0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing AGC reset counters bit");
                goto cleanup;
            }

            recoveryAction = adrv904x_RxFuncs_AgcSoftReset_BfSet(device, NULL, rxFuncsBaseAddr, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting AGC soft reset bit");
                goto cleanup;
            }
            
            recoveryAction = adrv904x_RxFuncs_AgcSoftReset_BfSet(device, NULL, rxFuncsBaseAddr, 0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing AGC soft reset bit");
                goto cleanup;
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDualBandLnaGainTableWrite(adi_adrv904x_Device_t* const device,
                                                                          const uint32_t rxChannelMask,
                                                                          const uint8_t gainIndexOffset,
                                                                          const adi_adrv904x_RxDualBandLnaGainTableRow_t gainTableRow[],
                                                                          const uint32_t arraySize)
{
        static const uint8_t MAX_GAIN_INDEX_OFFSET = 3U;
    static const uint8_t MAX_ARRAY_SIZE = 4U;
    static const uint32_t NUM_BYTES_PER_GAIN_ROW = 8U;
    static const uint32_t WRITE_MASK = 0x0FF00000U;  /* Bit 27:20 needs to be written */
    static const uint32_t WRITE_SHIFT = 20U; /* Bit 27:20 needs to be written */
    static const uint8_t MAX_GAIN_CONTROL_WORD = 3U;
    static const uint8_t MAX_GAIN_COMPENSATION_VALUE = 63U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    uint8_t baseIndex = 0U;
    uint8_t i = 0U;
    uint8_t rowCounter = 0U;
    uint32_t dualBandRowValue = 0U;
    uint32_t addressToWrite = 0U;
    uint32_t registerReadVal = 0U;
    uint32_t registerWriteVal = 0U;
    
    const uint32_t rxGainTableBaseAddr[] = 
    { 
        ADI_ADRV904X_RX0_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX1_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX2_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX3_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX4_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX5_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX6_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX7_GAIN_TABLE_BASEADDR
    };

    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gainTableRow, cleanup);

    if (((rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || (rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    if (gainIndexOffset > MAX_GAIN_INDEX_OFFSET) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gainIndexOffset,
                               "Gain index offset cannot be bigger than 3");
        goto cleanup;
    }
    
    if (arraySize > MAX_ARRAY_SIZE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               arraySize,
                               "Array size cannot be bigger than 4");
        goto cleanup;
    }

    if (arraySize > (gainIndexOffset + 1U)) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               arraySize,
                               "Array size cannot be bigger than offset + 1");
        goto cleanup;
    }

    baseIndex = (gainIndexOffset - (arraySize - 1U));
    rowCounter = 0U;
    
    for (i = baseIndex; i <= gainIndexOffset; ++i) 
    {
        if (gainTableRow[rowCounter].externalControlWord  > MAX_GAIN_CONTROL_WORD) 
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   gainTableRow[rowCounter].externalControlWord,
                                   "Gain ctrl word cannot be greater than 3");
            goto cleanup;
        }
        
        if (gainTableRow[rowCounter].gainCompensation  > MAX_GAIN_COMPENSATION_VALUE) 
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   gainTableRow[rowCounter].gainCompensation,
                                   "Gain compensation value cannot be greater than 63");
            goto cleanup;
        }

        dualBandRowValue = ((uint32_t)gainTableRow[rowCounter].externalControlWord & 0x03U) | (((uint32_t)gainTableRow[rowCounter].gainCompensation & 0x3FU) << 0x02U);
        dualBandRowValue <<= WRITE_SHIFT;
        
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(rxChannelMask, chanSel))
            {
                /* Perform a manual read modify write, since this register map is 32b only */
                addressToWrite = rxGainTableBaseAddr[chanIdx] + ((uint32_t)i * NUM_BYTES_PER_GAIN_ROW);
                recoveryAction = adi_adrv904x_Register32Read(device,
                                                             NULL,
                                                             addressToWrite,
                                                             &registerReadVal,
                                                             0xFFFFFFFFU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while reading gain row");
                    goto cleanup;
                }
                
                registerWriteVal = (registerReadVal & (~WRITE_MASK)) | dualBandRowValue;
                recoveryAction = adi_adrv904x_Register32Write(device,
                                                              NULL,
                                                              addressToWrite,
                                                              registerWriteVal,
                                                              0xFFFFFFFFU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing dual band gain row");
                    goto cleanup;
                }
            }
        }

        rowCounter++;
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDualBandLnaGainTableRead(adi_adrv904x_Device_t* const device,
                                                                         const uint32_t rxChannelMask,
                                                                         const uint8_t gainIndexOffset,
                                                                         adi_adrv904x_RxDualBandLnaGainTableRow_t gainTableRow[],
                                                                         const uint32_t arraySize,
                                                                         uint8_t* const numGainIndicesRead)
{
        static const uint8_t MAX_GAIN_INDEX_OFFSET = 3U;
    static const uint8_t MAX_ARRAY_SIZE = 4U;
    static const uint32_t NUM_BYTES_PER_GAIN_ROW = 8U;
    static const uint32_t READ_MASK = 0x0FF00000U; /* Bit 27:20 needs to be read */
    static const uint32_t READ_SHIFT = 20U; /* Bit 27:20 needs to be read */
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint8_t baseIndex = 0U;
    uint8_t i = 0U;
    uint8_t rowCounter = 0U;
    uint32_t dualBandRowValue = 0U;
    uint32_t addressToRead = 0U;
    const uint32_t rxGainTableBaseAddr[] = 
    { 
        ADI_ADRV904X_RX0_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX1_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX2_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX3_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX4_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX5_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX6_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX7_GAIN_TABLE_BASEADDR
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gainTableRow, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, numGainIndicesRead, cleanup);

    if ((rxChannelMask != (uint32_t)ADI_ADRV904X_RX0) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX1) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX2) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX3) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX4) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX5) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX6) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    if (gainIndexOffset > MAX_GAIN_INDEX_OFFSET) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gainIndexOffset,
                               "Gain index offset cannot be bigger than 3");
        goto cleanup;
    }
    
    if (arraySize > MAX_ARRAY_SIZE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               arraySize,
                               "Array size cannot be bigger than 4");
        goto cleanup;
    }

    if (arraySize > (gainIndexOffset + 1U)) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               arraySize,
                               "Array size cannot be bigger than offset + 1");
        goto cleanup;
    }

    baseIndex = (gainIndexOffset - (arraySize - 1U));
    rowCounter = 0U;
    
    chanIdx = adrv904x_RxChannelsToId((adi_adrv904x_RxChannels_e)rxChannelMask);
    
    for (i = baseIndex; i <= gainIndexOffset; ++i) 
    {
        addressToRead = rxGainTableBaseAddr[chanIdx] + ((uint32_t)i * NUM_BYTES_PER_GAIN_ROW);
        recoveryAction = adi_adrv904x_Register32Read(device,
                                                        NULL,
                                                        addressToRead,
                                                        &dualBandRowValue,
                                                        0xFFFFFFFFU);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while reading dual band gain row");
            goto cleanup;
        }

        dualBandRowValue &= READ_MASK;
        dualBandRowValue >>= READ_SHIFT;
        gainTableRow[rowCounter].externalControlWord = (uint8_t)(dualBandRowValue & 0x03U);
        gainTableRow[rowCounter].gainCompensation = (uint8_t)((dualBandRowValue >> 0x02U) & 0x3FU);

        rowCounter++;
    }

    /*Update no. of gain indices read*/
    if (numGainIndicesRead != NULL)
    {
        *numGainIndicesRead = rowCounter;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcDualBandCfgSet(adi_adrv904x_Device_t * const device,
                                                                const adi_adrv904x_AgcDualBandCfg_t agcDualBandConfig[],
                                                                const uint32_t numOfAgcDualBandCfgs) 
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    adrv904x_BfRxDdcChanAddr_e rxDdcBaseAddrBand0 = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    adrv904x_BfRxDdcChanAddr_e rxDdcBaseAddrBand1 = ADRV904X_BF_SLICE_RX_0__RX_DDC_1_;
    adi_adrv904x_RxDecimatedPowerCfg_t rxDecPowerCfg;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, agcDualBandConfig, cleanup);

    ADI_LIBRARY_MEMSET(&rxDecPowerCfg, 0, sizeof(adi_adrv904x_RxDecimatedPowerCfg_t));
    
    if (numOfAgcDualBandCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numOfAgcDualBandCfgs, "Invalid Number of AGC Dual Band Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numOfAgcDualBandCfgs; ++cfgIdx)
    {
        recoveryAction = adrv904x_AgcDualBandCfgRangeCheck(device, &agcDualBandConfig[cfgIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AGC Dual Band Range Check Error Reported");
            goto cleanup;
        }
    }

    /* Write out the configurations */
    for (cfgIdx = 0U; cfgIdx < numOfAgcDualBandCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(agcDualBandConfig[cfgIdx].rxChannelMask, chanSel))
            {
                recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &rxFuncsBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Rx funcs address get issue");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device,
                                                                  (adi_adrv904x_RxChannels_e)chanSel,
                                                                  ADI_ADRV904X_RX_DDC_BAND0,
                                                                  &rxDdcBaseAddrBand0);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanSel, "Invalid Rx Channel used to determine rx ddc0 address");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device,
                                                                  (adi_adrv904x_RxChannels_e)chanSel,
                                                                  ADI_ADRV904X_RX_DDC_BAND1,
                                                                  &rxDdcBaseAddrBand1);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanSel, "Invalid Rx Channel used to determine rx ddc1 address");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxFuncs_AgcDualbandExtTableUpperIndex_BfSet(device, NULL, rxFuncsBaseAddr, agcDualBandConfig[cfgIdx].agcRxDualbandExtTableUpperIndex);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dual band ext table upper index");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcDualbandExtTableLowerIndex_BfSet(device, NULL, rxFuncsBaseAddr, agcDualBandConfig[cfgIdx].agcRxDualbandExtTableLowerIndex);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dual band ext table lower index");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxFuncs_AgcDualbandPwrMargin_BfSet(device, NULL, rxFuncsBaseAddr, agcDualBandConfig[cfgIdx].agcDualbandPwrMargin);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dual band power margin");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcDualbandLnaStep_BfSet(device, NULL, rxFuncsBaseAddr, agcDualBandConfig[cfgIdx].agcDualbandLnaStep);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dual band lna step");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcDualbandHighLnaThreshold_BfSet(device, NULL, rxFuncsBaseAddr, agcDualBandConfig[cfgIdx].agcDualbandHighLnaThreshold);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dual band high lna threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcDualbandLowLnaThreshold_BfSet(device, NULL, rxFuncsBaseAddr, agcDualBandConfig[cfgIdx].agcDualbandLowLnaThreshold);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dual band low lna threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxFuncs_AgcDualbandMaxIndex_BfSet(device, NULL, rxFuncsBaseAddr, agcDualBandConfig[cfgIdx].agcDualBandMaxGainIndex);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dual band max gain index");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxDdc_GainCompForExtGain_BfSet(device, NULL, rxDdcBaseAddrBand0, agcDualBandConfig[cfgIdx].enableGainCompensationForExtLna);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write gain compensation enable bit for ext lna");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_RxDdc_GainCompForExtGain_BfSet(device, NULL, rxDdcBaseAddrBand1, agcDualBandConfig[cfgIdx].enableGainCompensationForExtLna);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write gain compensation enable bit for ext lna");
                    goto cleanup;
                }
                
                if (agcDualBandConfig[cfgIdx].agcDualBandEnable == 1U)
                {
                    rxDecPowerCfg.decPowerControl = ADI_ADRV904X_DEC_POWER_AGC_MEAS;
                    rxDecPowerCfg.measBlockSelectMask = (uint8_t)(ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK | ADI_ADRV904X_DEC_POWER_BAND_B_MEAS_BLOCK);
                    rxDecPowerCfg.powerInputSelect = 0; /* This field is 'Don't Care' when configuring Band decimated power measurements */
                    rxDecPowerCfg.powerMeasurementDuration = agcDualBandConfig[cfgIdx].decPowerDdcMeasurementDuration;
                    rxDecPowerCfg.rxChannelMask = chanSel;

                    recoveryAction = adi_adrv904x_RxDecimatedPowerCfgSet(device, &rxDecPowerCfg, 1U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing decimated power configuration");
                        goto cleanup;
                    }
                }
                else
                {
                    recoveryAction = adi_adrv904x_RxDecimatedPowerCfgGet(device,
                                                                         (adi_adrv904x_RxChannels_e)(chanSel),
                                                                         ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK,
                                                                         &rxDecPowerCfg);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Rx decimated power configuration");
                        goto cleanup;
                    }
                    
                    if (rxDecPowerCfg.decPowerControl == ADI_ADRV904X_DEC_POWER_AGC_MEAS)
                    {
                        /* Main path measurement block is being used for AGC - we should disable it */
                        rxDecPowerCfg.decPowerControl = ADI_ADRV904X_DEC_POWER_MEAS_OFF;
                        rxDecPowerCfg.measBlockSelectMask = (uint8_t)(ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK | ADI_ADRV904X_DEC_POWER_BAND_B_MEAS_BLOCK);
                        rxDecPowerCfg.powerInputSelect = 0;
                        rxDecPowerCfg.powerMeasurementDuration = 0;
                        rxDecPowerCfg.rxChannelMask = chanSel;

                        recoveryAction = adi_adrv904x_RxDecimatedPowerCfgSet(device, &rxDecPowerCfg, 1U);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing Rx decimated power configuration");
                            goto cleanup;
                        }
                    }
                }

                recoveryAction = adrv904x_RxFuncs_AgcDualbandEnable_BfSet(device, NULL, rxFuncsBaseAddr, agcDualBandConfig[cfgIdx].agcDualBandEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dual band enable bit");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcDualBandCfgGet(adi_adrv904x_Device_t * const device,
                                                                adi_adrv904x_AgcDualBandCfg_t * const agcDualBandConfigReadBack) 
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    adrv904x_BfRxDdcChanAddr_e rxDdcBaseAddrBand0 = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    adi_adrv904x_RxDecimatedPowerCfg_t rxDecPowerCfg;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, agcDualBandConfigReadBack, cleanup);

    ADI_LIBRARY_MEMSET(&rxDecPowerCfg, 0, sizeof(adi_adrv904x_RxDecimatedPowerCfg_t));
    
    if ((agcDualBandConfigReadBack->rxChannelMask != ADI_ADRV904X_RX0) &&
        (agcDualBandConfigReadBack->rxChannelMask != ADI_ADRV904X_RX1) &&
        (agcDualBandConfigReadBack->rxChannelMask != ADI_ADRV904X_RX2) &&
        (agcDualBandConfigReadBack->rxChannelMask != ADI_ADRV904X_RX3) &&
        (agcDualBandConfigReadBack->rxChannelMask != ADI_ADRV904X_RX4) &&
        (agcDualBandConfigReadBack->rxChannelMask != ADI_ADRV904X_RX5) &&
        (agcDualBandConfigReadBack->rxChannelMask != ADI_ADRV904X_RX6) &&
        (agcDualBandConfigReadBack->rxChannelMask != ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, agcDualBandConfigReadBack->rxChannelMask, "Invalid Rx channel is selected. Valid values are Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)agcDualBandConfigReadBack->rxChannelMask, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, agcDualBandConfigReadBack->rxChannelMask, "Rx funcs address get issue");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device,
                                                        (adi_adrv904x_RxChannels_e)agcDualBandConfigReadBack->rxChannelMask,
                                                        ADI_ADRV904X_RX_DDC_BAND0,
                                                        &rxDdcBaseAddrBand0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, agcDualBandConfigReadBack->rxChannelMask, "Invalid Rx Channel used to determine rx ddc0 address");
        goto cleanup;
    }


    recoveryAction = adrv904x_RxFuncs_AgcDualbandExtTableUpperIndex_BfGet(device, NULL, rxFuncsBaseAddr, &agcDualBandConfigReadBack->agcRxDualbandExtTableUpperIndex);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dual band ext table upper index");
        goto cleanup;
    }
                
    recoveryAction = adrv904x_RxFuncs_AgcDualbandExtTableLowerIndex_BfGet(device, NULL, rxFuncsBaseAddr, &agcDualBandConfigReadBack->agcRxDualbandExtTableLowerIndex);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dual band ext table lower index");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcDualbandPwrMargin_BfGet(device, NULL, rxFuncsBaseAddr, &agcDualBandConfigReadBack->agcDualbandPwrMargin);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dual band power margin");
        goto cleanup;
    }
                
    recoveryAction = adrv904x_RxFuncs_AgcDualbandLnaStep_BfGet(device, NULL, rxFuncsBaseAddr, &agcDualBandConfigReadBack->agcDualbandLnaStep);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dual band lna step");
        goto cleanup;
    }
                
    recoveryAction = adrv904x_RxFuncs_AgcDualbandHighLnaThreshold_BfGet(device, NULL, rxFuncsBaseAddr, &agcDualBandConfigReadBack->agcDualbandHighLnaThreshold);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dual band high lna threshold");
        goto cleanup;
    }
                
    recoveryAction = adrv904x_RxFuncs_AgcDualbandLowLnaThreshold_BfGet(device, NULL, rxFuncsBaseAddr, &agcDualBandConfigReadBack->agcDualbandLowLnaThreshold);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dual band low lna threshold");
        goto cleanup;
    }
                
    recoveryAction = adrv904x_RxFuncs_AgcDualbandMaxIndex_BfGet(device, NULL, rxFuncsBaseAddr, &agcDualBandConfigReadBack->agcDualBandMaxGainIndex);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dual band max gain index");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxDdc_GainCompForExtGain_BfGet(device, NULL, rxDdcBaseAddrBand0, &agcDualBandConfigReadBack->enableGainCompensationForExtLna);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read gain compensation enable bit for ext lna");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_RxDecimatedPowerCfgGet(device,
                                                         (adi_adrv904x_RxChannels_e)(agcDualBandConfigReadBack->rxChannelMask),
                                                         ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK,
                                                         &rxDecPowerCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Rx decimated power configuration");
        goto cleanup;
    }
    
    agcDualBandConfigReadBack->decPowerDdcMeasurementDuration = rxDecPowerCfg.powerMeasurementDuration;
    
    recoveryAction = adrv904x_RxFuncs_AgcDualbandEnable_BfGet(device, NULL, rxFuncsBaseAddr, &agcDualBandConfigReadBack->agcDualBandEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dual band enable bit");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcDualBandGpioCfgSet(adi_adrv904x_Device_t * const device,
                                                                    const adi_adrv904x_AgcDualBandGpioCfg_t * agcDualBandGpioConfig) 
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t i = 0U;
    uint16_t gpioSelMask = 0U;
    adi_adrv904x_GpioSignal_e option1 = ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_0;
    adi_adrv904x_GpioSignal_e option2 = ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_0;
    adi_adrv904x_GpioAnaPinSel_e gpioAnalogSel = ADI_ADRV904X_GPIO_ANA_00;
    adi_adrv904x_GpioSignal_e gpioSignalReadBack = ADI_ADRV904X_GPIO_SIGNAL_UNUSED;
    uint32_t channelMaskReadBack = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, agcDualBandGpioConfig, cleanup);

    for (i = 0U; i < ADI_ADRV904X_GPIO_ANALOG_COUNT; ++i)
    {
        gpioSelMask = 1U << i;
        if (ADRV904X_BF_EQUAL(agcDualBandGpioConfig->gpioSelectionMask, gpioSelMask))
        {
            switch (i)
            {
            case 0:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_0;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND0_0;
                break;
            case 1:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_1;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND0_1;
                break;
            case 2:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND1_0;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND1_0;
                break;
            case 3:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND1_1;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND1_1;
                break;
            case 4:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND0_0;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND0_0;
                break;
            case 5:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND0_1;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND0_1;
                break;
            case 6:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND1_0;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND1_0;
                break;
            case 7:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND1_1;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND1_1;
                break;
            case 8:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND0_0;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND0_0;
                break;
            case 9:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND0_1;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND0_1;
                break;
            case 10:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND1_0;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND1_0;
                break;
            case 11:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND1_1;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND1_1;
                break;
            case 12:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND0_0;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND0_0;
                break;
            case 13:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND0_1;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND0_1;
                break;
            case 14:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND1_0;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND1_0;
                break;
            case 15:
                option1 = ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND1_1;
                option2 = ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND1_1;
                break;
            }

            if ((agcDualBandGpioConfig->analogGpioMapping[i] != option1) &&
                (agcDualBandGpioConfig->analogGpioMapping[i] != option2) &&
                (agcDualBandGpioConfig->analogGpioMapping[i] != ADI_ADRV904X_GPIO_SIGNAL_UNUSED))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, agcDualBandGpioConfig->analogGpioMapping, "Invalid GPIO selected. Please check the GPIO table given at function documentation");
                goto cleanup;
            }

            gpioAnalogSel = (adi_adrv904x_GpioAnaPinSel_e)i;
            if (agcDualBandGpioConfig->analogGpioMapping[i] == ADI_ADRV904X_GPIO_SIGNAL_UNUSED)
            {
                recoveryAction = adrv904x_GpioAnalogSignalGet(device, gpioAnalogSel, &gpioSignalReadBack, &channelMaskReadBack);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         "Error while retrieving current signal for the selected gpio");
                    goto cleanup;
                }
                
                if ((gpioSignalReadBack != option1) && 
                    (gpioSignalReadBack != option2))
                {
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GPIO selected to release is being used by other feature");
                    goto cleanup;
                }

                recoveryAction = adrv904x_GpioAnalogSignalRelease(device, gpioAnalogSel, gpioSignalReadBack, channelMaskReadBack);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to release gpio");
                    goto cleanup;
                }
            }
            else 
            {
                recoveryAction = adrv904x_GpioAnalogSignalSet(device, gpioAnalogSel, agcDualBandGpioConfig->analogGpioMapping[i], (uint32_t)ADI_ADRV904X_CHOFF);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to assign selected signal to GPIO");
                    goto cleanup;
                }
            }
        }
    }

    /* No error, set recovery action to None */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
    
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcDualBandGpioCfgGet(adi_adrv904x_Device_t * const device,
                                                                    adi_adrv904x_AgcDualBandGpioCfg_t * const agcDualBandGpioConfigReadBack) 
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t i = 0U;
    uint16_t gpioSelMask = 0U;
    adi_adrv904x_GpioAnaPinSel_e gpioAnalogSel = ADI_ADRV904X_GPIO_ANA_00;
    adi_adrv904x_GpioSignal_e gpioSignalReadBack = ADI_ADRV904X_GPIO_SIGNAL_UNUSED;
    uint32_t channelMaskReadBack = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, agcDualBandGpioConfigReadBack, cleanup);

    for (i = 0U; i < ADI_ADRV904X_GPIO_ANALOG_COUNT; ++i)
    {
        gpioSelMask = 1U << i;
        if (ADRV904X_BF_EQUAL(agcDualBandGpioConfigReadBack->gpioSelectionMask, gpioSelMask))
        {
            gpioAnalogSel = (adi_adrv904x_GpioAnaPinSel_e)i;
            recoveryAction = adrv904x_GpioAnalogSignalGet(device, gpioAnalogSel, &gpioSignalReadBack, &channelMaskReadBack);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     "Error while retrieving current signal for the selected gpio");
                goto cleanup;
            }
            
            agcDualBandGpioConfigReadBack->analogGpioMapping[i] = gpioSignalReadBack;
        }
        else
        {
            agcDualBandGpioConfigReadBack->analogGpioMapping[i] = ADI_ADRV904X_GPIO_SIGNAL_INVALID;
        }
    }

    /* No error, set recovery action to None */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcGpioReSyncSet(adi_adrv904x_Device_t * const device,
                                                               uint32_t rxChannelMask,
                                                               adi_adrv904x_GpioPinSel_e gpioSelection,
                                                               uint8_t enable) 
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    adi_adrv904x_GpioPinSel_e gpioReadback = ADI_ADRV904X_GPIO_INVALID;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    /*Check that if requested Rx Channel valid*/
    if (((rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || (rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (enable > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               enable,
                               "enable can be either 0 or 1");
        goto cleanup;
    }
    
    if (gpioSelection == ADI_ADRV904X_GPIO_00) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               enable,
                               "GPIO0 cannot be used for this feature");
        goto cleanup;
    }

    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(rxChannelMask, chanSel))
        {
            recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)chanSel, &rxFuncsBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "Rx funcs address get issue");
                goto cleanup;
            }

            recoveryAction = adrv904x_RxFuncs_AgcEnableSyncPulseForGainCounter_BfSet(device, NULL, rxFuncsBaseAddr, enable);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing AGC re-sync enable bitfield");
                goto cleanup;
            }
            
            if (gpioSelection == ADI_ADRV904X_GPIO_INVALID)
            {
                /* Release the signal from this Rx channel */

                recoveryAction = adrv904x_GpioSignalFind(device,
                                                         &gpioReadback,
                                                         ADI_ADRV904X_GPIO_SIGNAL_AGC_GAIN_CHANGE,
                                                         (adi_adrv904x_Channels_e)(chanSel));
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read gpio for selected signal");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_GpioSignalRelease(device,
                                                            gpioReadback,
                                                            ADI_ADRV904X_GPIO_SIGNAL_AGC_GAIN_CHANGE,
                                                            chanSel);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to release gpio");
                    goto cleanup;
                }
                
            }
            else
            {
                /* Assign the signal for this channel */
                recoveryAction = adrv904x_GpioSignalSet(device,
                                                        gpioSelection,
                                                        ADI_ADRV904X_GPIO_SIGNAL_AGC_GAIN_CHANGE,
                                                        chanSel);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to assign selected signal to GPIO");
                    goto cleanup;
                }
                
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcGpioReSyncGet(adi_adrv904x_Device_t * const device,
                                                               const uint32_t rxChannelMask,
                                                               adi_adrv904x_GpioPinSel_e * const gpioSelection,
                                                               uint8_t * const enable) 
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioSelection, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, enable, cleanup);

    /*Check that if requested Rx Channel valid*/
    if ((rxChannelMask == 0U) || ((rxChannelMask & (rxChannelMask - 1)) != 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)rxChannelMask, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "Rx funcs address get issue");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_RxFuncs_AgcEnableSyncPulseForGainCounter_BfGet(device, NULL, rxFuncsBaseAddr, enable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading AGC re-sync enable bitfield");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_GpioSignalFind(device,
                                             gpioSelection,
                                             ADI_ADRV904X_GPIO_SIGNAL_AGC_GAIN_CHANGE,
                                             (adi_adrv904x_Channels_e)(rxChannelMask));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read gpio for selected signal");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcDualBandActiveExternalLnaGainWordGet(adi_adrv904x_Device_t * const device,
                                                                                      const adi_adrv904x_RxChannels_e rxChannel,
                                                                                      uint8_t * const bandAExternalLnaGainWord,
                                                                                      uint8_t * const bandBExternalLnaGainWord)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, bandAExternalLnaGainWord, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, bandBExternalLnaGainWord, cleanup);

    /*Check that if requested Rx Channel valid*/
    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel selection");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, rxChannel, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Rx funcs address get issue");
        goto cleanup;
    }

    /* Set this field to 0, to readback the active gain index */
    recoveryAction = adrv904x_RxFuncs_ReadGainTable_BfSet(device, NULL, rxFuncsBaseAddr, 0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing read gain table bitfield");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_RxFuncs_DualbandControlBandA_BfGet(device, NULL, rxFuncsBaseAddr, bandAExternalLnaGainWord);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading external control word for band A");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_DualbandControlBandB_BfGet(device, NULL, rxFuncsBaseAddr, bandBExternalLnaGainWord);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading external control word for band B");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcUpperLevelBlockerGet(adi_adrv904x_Device_t * const device,
                                                                      uint8_t * const               agcULBlockerBitMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    static const adrv904x_BfRxFuncsChanAddr_e rxChannelFuncs[] = { 
        ADRV904X_BF_SLICE_RX_0__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_1__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_2__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_3__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_4__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_5__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_6__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_7__RX_FUNCS
    };

    uint8_t channelBitFieldVal = 0U;
    uint8_t channelId          = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, agcULBlockerBitMask, cleanup);

    /* clear return byte before readings */
    *agcULBlockerBitMask = 0U;

    for (channelId = 0U; channelId < ADI_ADRV904X_MAX_RX_ONLY; channelId++)
    {
        recoveryAction = adrv904x_RxFuncs_AgcUlBlocker_BfGet(device, NULL, rxChannelFuncs[channelId], &channelBitFieldVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading agc upper level blocker bitfield");
            goto cleanup;
        }

        *agcULBlockerBitMask = *agcULBlockerBitMask | (channelBitFieldVal << channelId);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcLowerLevelBlockerGet(adi_adrv904x_Device_t * const device,
                                                                      uint8_t * const               agcLLBlockerBitMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    static const adrv904x_BfRxFuncsChanAddr_e rxChannelFuncs[] = { 
        ADRV904X_BF_SLICE_RX_0__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_1__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_2__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_3__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_4__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_5__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_6__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_7__RX_FUNCS
    };

    uint8_t channelBitFieldVal = 0U;
    uint8_t channelId          = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, agcLLBlockerBitMask, cleanup);

    /* clear return byte before readings */
    *agcLLBlockerBitMask = 0U;

    for (channelId = 0U; channelId < ADI_ADRV904X_MAX_RX_ONLY; channelId++)
    {
        recoveryAction = adrv904x_RxFuncs_AgcLlBlocker_BfGet(device, NULL, rxChannelFuncs[channelId], &channelBitFieldVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading agc lower level blocker bitfield");
            goto cleanup;
        }

        *agcLLBlockerBitMask = *agcLLBlockerBitMask | (channelBitFieldVal << channelId);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcHighThresholdPeakDetectorGet(adi_adrv904x_Device_t * const device,
                                                                              uint8_t * const               thresholdPeakDetectorBitMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    static const adrv904x_BfRxFuncsChanAddr_e rxChannelFuncs[] = { 
        ADRV904X_BF_SLICE_RX_0__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_1__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_2__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_3__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_4__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_5__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_6__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_7__RX_FUNCS
    };

    uint8_t channelBitFieldVal = 0U;
    uint8_t channelId          = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, thresholdPeakDetectorBitMask, cleanup);

    /* clear return byte before readings */
    *thresholdPeakDetectorBitMask = 0U;

    for (channelId = 0U; channelId < ADI_ADRV904X_MAX_RX_ONLY; channelId++)
    {
        recoveryAction = adrv904x_RxFuncs_AgcAdcovrgHigh_BfGet(device, NULL, rxChannelFuncs[channelId], &channelBitFieldVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading agc high threshold peak detector exceeded bitfield");
            goto cleanup;
        }

        *thresholdPeakDetectorBitMask = *thresholdPeakDetectorBitMask | (channelBitFieldVal << channelId);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AgcLowThresholdPeakDetectorGet(adi_adrv904x_Device_t * const device,
    uint8_t * const               thresholdPeakDetectorBitMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    static const adrv904x_BfRxFuncsChanAddr_e rxChannelFuncs[] = { 
        ADRV904X_BF_SLICE_RX_0__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_1__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_2__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_3__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_4__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_5__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_6__RX_FUNCS,
        ADRV904X_BF_SLICE_RX_7__RX_FUNCS
    };

    uint8_t channelBitFieldVal = 0U;
    uint8_t channelId          = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, thresholdPeakDetectorBitMask, cleanup);

    /* clear return byte before readings */
    *thresholdPeakDetectorBitMask = 0U;

    for (channelId = 0U; channelId < ADI_ADRV904X_MAX_RX_ONLY; channelId++)
    {
        recoveryAction = adrv904x_RxFuncs_AgcAdcovrgLow_BfGet(device, NULL, rxChannelFuncs[channelId], &channelBitFieldVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading agc upper level blocker bitfield");
            goto cleanup;
        }

        *thresholdPeakDetectorBitMask = *thresholdPeakDetectorBitMask | (channelBitFieldVal << channelId);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}