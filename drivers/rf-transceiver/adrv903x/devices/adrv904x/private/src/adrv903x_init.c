/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adrv903x_init.c
*
* ADRV903X API Version: 2.12.1.4
*/

#include "../../private/include/adrv903x_init.h"
#include "../../private/bf/adrv903x_bf_core.h"
#include "../../private/bf/adrv903x_bf_rx_funcs.h"
#include "../../private/bf/adrv903x_bf_orx_dig.h"
#include "../../private/bf/adrv903x_bf_tx_funcs.h"
#include "../../private/bf/adrv903x_bf_tx_dig.h"
#include "../../private/include/adrv903x_rx.h"
#include "../../private/include/adrv903x_tx.h"
#include "../../private/include/adrv903x_gpio.h"
#include "../../private/include/adrv903x_cpu_cmd_tx_to_orx_mapping.h"

#include "adi_adrv903x_radioctrl_types.h"


#define ADI_FILE    ADI_ADRV903X_FILE_PRIVATE_INIT

static uint32_t adrv903x_ClockDivWithFixedDiv(const uint32_t clock, const uint32_t clockMax, const uint32_t fixedDivLog2);
static uint32_t adrv903x_ErrReportConvert(const adi_adrv903x_Version_t version);

/*********************/
/* Private functions */
/*********************/

/**
* \brief Calculates the divide value necessary to get the
*        clock parameter under the clockMax using
*        out = clock / 2 ^ (div + fixedDivLog2)
*
* \param clock Input clock frequency
* \param clockMax Maximum clock frequency
* \param fixedDivLog2 Constant added to the divider
*
* \retval uint32_t divider value calculated or the max value allowed
*/
static uint32_t adrv903x_ClockDivWithFixedDiv(  const uint32_t clock,
                                                const uint32_t clockMax,
                                                const uint32_t fixedDivLog2)
{
    uint32_t              div                 = 0U;
    static const uint32_t ADRV903X_POWER2_MAX = 8U;

    for (div = 0U; div < ADRV903X_POWER2_MAX; div++)
    {
        if (clock <= (clockMax << (div + fixedDivLog2)))
        {
            return div;
        }
    }

    return div;
}

adi_adrv903x_ErrAction_e adrv903x_ClocksSync(   adi_adrv903x_Device_t* const        device,
                                                const adi_adrv903x_Init_t* const    init)
{
    adi_adrv903x_ErrAction_e        recoveryAction          = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t                         idx                     = 0U;
    uint32_t                        digDeviceClockDiv       = 0U;
    uint32_t                        mBiasClockDiv           = 0U;
    uint32_t                        digRefClock_kHz         = 0U;

    static const uint32_t MASTER_BIAS_CLOCK_TARGET_kHz = 1000U;   /* Prefer <1MHz */
    static const uint32_t MASTER_BIAS_CLOCK_MAX_kHz    = 2000U;   /* But 2MHz max */

    static const uint32_t DIGDEVICECLOCKDIVMAX         = 3U;
    static const uint32_t MBIASCLOCKDIVMAX             = 3U;

    static const uint32_t MBIASCLOCKDIVFIXED           = 6U; /* Fixed divide-by-64 before master bias divider */

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, init);

    /* Set Digital device clock divider */
    digDeviceClockDiv = 0U;

    while( digDeviceClockDiv        < DIGDEVICECLOCKDIVMAX &&
           (1 << digDeviceClockDiv) < device->initExtract.clocks.padDivideRatio )
    {
        digDeviceClockDiv++;
    }

    recoveryAction = adrv903x_Core_DevclkDivideRatio_BfSet(device,
                                                           NULL,
                                                           ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                           digDeviceClockDiv);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set devclk divide ratio");
        return recoveryAction;
    }

    if (init->clocks.DevClkOnChipTermResEn > 1U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                init->clocks.DevClkOnChipTermResEn,
                                "DevClkOnChipTermResEn can only be 0 or 1");
        return recoveryAction;
    }

    recoveryAction = adrv903x_Core_DevclkBuffTermEn_BfSet(device,
                                                          NULL,
                                                          ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                          init->clocks.DevClkOnChipTermResEn);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set devclk buffer Termination enable");
        return recoveryAction;
    }

    recoveryAction = adrv903x_Core_DevclkBufferEnable_BfSet(device,
                                                            NULL,
                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            ADI_ENABLE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set devclk buffer enable");
        return recoveryAction;
    }

    digRefClock_kHz = device->initExtract.clocks.deviceClockScaled_kHz;

    /* Set Master Bias clock divider */
    mBiasClockDiv = adrv903x_ClockDivWithFixedDiv(digRefClock_kHz, MASTER_BIAS_CLOCK_TARGET_kHz, MBIASCLOCKDIVFIXED);
    if (mBiasClockDiv > MBIASCLOCKDIVMAX)
    {
        /* We've exceeded target clock frequency, so clamp to max divide value and verify
           that we have not exceeded max clock frequency */
        mBiasClockDiv = MBIASCLOCKDIVMAX;
        if( digRefClock_kHz > (MASTER_BIAS_CLOCK_MAX_kHz << (mBiasClockDiv + MBIASCLOCKDIVFIXED)) )
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    mBiasClockDiv,
                                    "Device Clock too fast!");
            return recoveryAction;
        }
    }

    recoveryAction = adrv903x_Core_MasterBiasClkDivRatio_BfSet(device,
                                                               NULL,
                                                               ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                               mBiasClockDiv);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set dmaster bias clock div ratio");
        return recoveryAction;
    }

    for (idx = 0; idx < 2U; ++idx)
    {
        recoveryAction = adrv903x_Core_MbiasIgenPd_BfSet(device,
                                                         NULL,
                                                         ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                         idx,
                                                         ADI_DISABLE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to clear mbias igen powerdown");
            return recoveryAction;
        }
        
        recoveryAction = adrv903x_Core_MbiasTrimCompPd_BfSet(device,
                                                             NULL,
                                                             ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                             idx,
                                                             ADI_DISABLE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to clear mbias trim comp powerdown");
            return recoveryAction;
        }
        
        recoveryAction = adrv903x_Core_MbiasRtrimResetb_BfSet(device,
                                                              NULL,
                                                              ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                              idx,
                                                              ADI_ENABLE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to clear mbias trim comp powerdown");
            return recoveryAction;
        }
    }

    recoveryAction = adrv903x_Core_DevclkDividerMcsResetb_BfSet(device,
                                                                NULL,
                                                                (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                                ADI_ENABLE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set DevclkDivider MCS ResetB");
        return recoveryAction;
    }

    /* THIS MUST BE done BEFORE starting up the main digital clocks DigitalClockPowerUp */
    recoveryAction = adrv903x_Core_UseDeviceClkAsHsdigclk_BfSet(device,
                                                                NULL,
                                                                ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                ADI_ENABLE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set use device clk as hsdig");
        return recoveryAction;
    }
    
    recoveryAction = adrv903x_Core_DigitalClockPowerUp_BfSet(device,
                                                             NULL,
                                                             ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                             ADI_ENABLE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set digital clock powerup");
        return recoveryAction;
    }
    
    recoveryAction = adrv903x_Core_ArmClkEnable_BfSet(device,
                                                      NULL,
                                                      ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                      ADI_ENABLE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set arm clock enable");
        return recoveryAction;
    }

    recoveryAction = adrv903x_Core_ArmClkDivideRatio_BfSet(device,
                                                           NULL,
                                                           ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                           device->initExtract.clocks.armClkDivideRatio);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set arm clk divide ratio");
        return recoveryAction;
    }

    /* Calculate and set the ARM clock divider when on DEVCLK */
    recoveryAction = adrv903x_Core_ArmClkDivideRatioDevClk_BfSet(device,
                                                                 NULL,
                                                                 (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                                 device->initExtract.clocks.armClkDevClkDivRatio);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set arm clk device clk divide ratio");
        return recoveryAction;
    }

    recoveryAction = adrv903x_Core_AhbSpiBridgeEnable_BfSet(device,
                                                            NULL,
                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            ADI_ENABLE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set ahb spi bridge enable bit");
        return recoveryAction;
    }

    /* Clear clk divider sync bit, then set it */
    recoveryAction = adrv903x_Core_DigitalClockDividerSyncEn_BfSet(device,
                                                                   NULL,
                                                                   ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                   ADI_ENABLE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to clear digital clock divider sync enable bit");
        return recoveryAction;
    }
    recoveryAction = adrv903x_Core_DigitalClockDividerSyncEn_BfSet(device,
                                                                   NULL,
                                                                   ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                   ADI_DISABLE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while attempting to set digital clock divider sync enable bit");
        return recoveryAction;
    }

    return recoveryAction;
}


ADI_API adi_adrv903x_ErrAction_e adrv903x_RxOverloadProtectionSet(adi_adrv903x_Device_t* const    device,
                                                                  const adi_adrv903x_RxChannels_e rxChannel)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxFuncsChanAddr_e rxFuncsChannelBitfieldAddr = (adrv903x_BfRxFuncsChanAddr_e) 0U;
    adrv903x_BfOrxDigChanAddr_e orxDigChannelBitfieldAddr = (adrv903x_BfOrxDigChanAddr_e) 0U;

    /* Hardcoded values for ONLY Rx Overload Protection */
    static const uint8_t  AGC_USE_COUNTERS_FOR_MGC                  = 0x1U;
    static const uint8_t  AGC_MANUAL_GAIN_PIN_CONTROL               = 0x0U;
    static const uint8_t  AGC_ULB_THRESHOLD_EXCEEDED_COUNTER        = 0X3U;
    static const uint8_t  AGC_ADC_HIGH_OVRG_EXCEEDED_COUNTER        = 0x3U;
    static const uint32_t AGC_GAIN_UPDATE_COUNTER                   = 0x3D090U;
    static const uint8_t  AGC_SLOW_LOOP_SETTLING_DELAY              = 0x10U;
    static const uint8_t  AGC_RESET_ON_RXON                         = 0x0U;
    static const uint8_t  AGC_ENABLE_SYNC_PULSE_FOR_GAIN_COUNTER    = 0x0U;
    static const uint8_t  HB2_OVERLOAD_USE_HB2_IN                   = 0x0U;

    /* Hardcoded values to set for BOTH Rx and ORx Overload Protection */
    static const uint8_t  PEAK_SAMPLE_COUNT_AGC                     = 0x1U;
    static const uint8_t  PEAK_WINDOW_SIZE_AGC                      = 0x2U;
    static const uint8_t  OVERLOAD_EN_AGC                           = 0x1U;
    static const uint8_t  OVERLOAD_POWER_MODE_AGC                   = 0x1U;
    static const uint16_t OVERLOAD_TH_AGC_PRE                       = 0x32D6U;
    static const uint16_t OVERLOAD_TH_AGC_HIGH                      = 0x2862U;
    
    /* Check for valid device handle */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /*Check that the requested rxChannel is valid*/
    if ((rxChannel != ADI_ADRV903X_RX0)  &&
        (rxChannel != ADI_ADRV903X_RX1)  &&
        (rxChannel != ADI_ADRV903X_RX2)  &&
        (rxChannel != ADI_ADRV903X_RX3)  &&
        (rxChannel != ADI_ADRV903X_RX4)  &&
        (rxChannel != ADI_ADRV903X_RX5)  &&
        (rxChannel != ADI_ADRV903X_RX6)  &&
        (rxChannel != ADI_ADRV903X_RX7)  &&
        (rxChannel != ADI_ADRV903X_ORX0) &&
        (rxChannel != ADI_ADRV903X_ORX1))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_ERROR_REPORT( &device->common,
                           ADI_ADRV903X_ERRSRC_API,
                           ADI_COMMON_ERRCODE_API,
                           recoveryAction,
                           rxChannel,
                           "Invalid Rx Channel Requested for Rx Overload Protection");
        return recoveryAction;
    }
    
    /* If rxChannel is a Rx Channel, use Rx_Funcs base addr and bitfields */
    if (((uint32_t)ADI_ADRV903X_RX_MASK_ALL & (uint32_t)rxChannel) > 0U)
    {
        /* Get base address for the Rx Channel */
        recoveryAction = (adi_adrv903x_ErrAction_e) adrv903x_RxFuncsBitfieldAddressGet(  device,
                                                                                       rxChannel,
                                                                                      &rxFuncsChannelBitfieldAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to get Rx Funcs Base address for given Rx channel");
            return recoveryAction;
        }
        
        /* Set bitfields */
        recoveryAction =  adrv903x_RxFuncs_AgcUseCountersForMgc_BfSet(device,
                                                                      NULL,
                                                                      rxFuncsChannelBitfieldAddr,
                                                                      AGC_USE_COUNTERS_FOR_MGC);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set AgcUseCountersForMgc for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_AgcManualGainPinControl_BfSet(device,
                                                                         NULL,
                                                                         rxFuncsChannelBitfieldAddr,
                                                                         AGC_MANUAL_GAIN_PIN_CONTROL);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set AgcManualGainPinControl for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_AgcUlbThresholdExceededCounter_BfSet(device,
                                                                                NULL,
                                                                                rxFuncsChannelBitfieldAddr,
                                                                                AGC_ULB_THRESHOLD_EXCEEDED_COUNTER);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set AgcUlbThresholdExceededCounter for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_AgcAdcHighOvrgExceededCounter_BfSet(device,
                                                                               NULL,
                                                                               rxFuncsChannelBitfieldAddr,
                                                                               AGC_ADC_HIGH_OVRG_EXCEEDED_COUNTER);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set AgcAdcHighOvrgExceededCounter for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_AgcGainUpdateCounter_BfSet(device,
                                                                      NULL,
                                                                      rxFuncsChannelBitfieldAddr,
                                                                      AGC_GAIN_UPDATE_COUNTER);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set AgcGainUpdateCounter for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_AgcSlowLoopSettlingDelay_BfSet(device,
                                                                          NULL,
                                                                          rxFuncsChannelBitfieldAddr,
                                                                          AGC_SLOW_LOOP_SETTLING_DELAY);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set AgcSlowLoopSettlingDelay for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_PeakSampleCountAgc_BfSet(device,
                                                                    NULL,
                                                                    rxFuncsChannelBitfieldAddr,
                                                                    PEAK_SAMPLE_COUNT_AGC);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set PeakSampleCountAgc for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_PeakWindowSizeAgc_BfSet(device,
                                                                   NULL,
                                                                   rxFuncsChannelBitfieldAddr,
                                                                   PEAK_WINDOW_SIZE_AGC);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set PeakWindowSizeAgc for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_OverloadEnAgc_BfSet(device,
                                                               NULL,
                                                               rxFuncsChannelBitfieldAddr,
                                                               OVERLOAD_EN_AGC);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set OverloadEnAgc for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_OverloadPowerModeAgc_BfSet(device,
                                                                      NULL,
                                                                      rxFuncsChannelBitfieldAddr,
                                                                      OVERLOAD_POWER_MODE_AGC);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set OverloadPowerModeAgc for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_Hb2OverloadUseHb2In_BfSet(device,
                                                                     NULL,
                                                                     rxFuncsChannelBitfieldAddr,
                                                                     HB2_OVERLOAD_USE_HB2_IN);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set Hb2OverloadUseHb2In for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_OverloadThAgcPre_BfSet(device,
                                                                  NULL,
                                                                  rxFuncsChannelBitfieldAddr,
                                                                  OVERLOAD_TH_AGC_PRE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set OverloadThAgcPre for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_OverloadThAgcHigh_BfSet(device,
                                                                   NULL,
                                                                   rxFuncsChannelBitfieldAddr,
                                                                   OVERLOAD_TH_AGC_HIGH);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set OverloadThAgcHigh for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_AgcResetOnRxon_BfSet(device,
                                                                NULL,
                                                                rxFuncsChannelBitfieldAddr,
                                                                AGC_RESET_ON_RXON);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set AgcResetOnRxon for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_AgcEnableSyncPulseForGainCounter_BfSet(device,
                                                                                  NULL,
                                                                                  rxFuncsChannelBitfieldAddr,
                                                                                  AGC_ENABLE_SYNC_PULSE_FOR_GAIN_COUNTER);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set AgcEnableSyncPulseForGainCounter for given Rx channel");
            return recoveryAction;
        }

        /* Set and clear this bit to latch new threshold values */
        recoveryAction =  adrv903x_RxFuncs_TiaValidOverride_BfSet(device,
                                                                  NULL,
                                                                  rxFuncsChannelBitfieldAddr,
                                                                  ADI_ENABLE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set TiaValidOverride for given Rx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_RxFuncs_TiaValidOverride_BfSet(device,
                                                                  NULL,
                                                                  rxFuncsChannelBitfieldAddr,
                                                                  ADI_DISABLE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set TiaValidOverride for given Rx channel");
            return recoveryAction;
        }
    }

    /* If rxChannel is a ORx Channel, use ORx_Dig base addr and bitfields */
    /* Note: AGC bitfields from Rx Channels do not exist here, just overload protection bitfields.*/
    if (((uint32_t)ADI_ADRV903X_ORX_MASK_ALL & (uint32_t)rxChannel) > 0U)
    {
        /* Get base address for the ORx Channel */
        recoveryAction = adrv903x_OrxBitfieldAddressGet(  device,
                                                          rxChannel,
                                                         &orxDigChannelBitfieldAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to get ORx Dig Base address for given ORx channel");
            return recoveryAction;
        }

        /* Set bitfields */
        recoveryAction =  adrv903x_OrxDig_OverloadCountTh_BfSet(device,
                                                                NULL,
                                                                orxDigChannelBitfieldAddr,
                                                                PEAK_SAMPLE_COUNT_AGC);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set OverloadCountTh for given ORx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_OrxDig_OverloadDurationCount_BfSet(device,
                                                                      NULL,
                                                                      orxDigChannelBitfieldAddr,
                                                                      PEAK_WINDOW_SIZE_AGC);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set verloadDurationCount for given ORx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_OrxDig_OverloadEn_BfSet(device,
                                                           NULL,
                                                           orxDigChannelBitfieldAddr,
                                                           OVERLOAD_EN_AGC);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set OverloadEn for given ORx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_OrxDig_OverloadPowerMode_BfSet(device,
                                                                  NULL,
                                                                  orxDigChannelBitfieldAddr,
                                                                  OVERLOAD_POWER_MODE_AGC);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set OverloadPowerMode for given ORx channel");
            return recoveryAction;
        }

        recoveryAction =  adrv903x_OrxDig_OverloadThPre_BfSet(device,
                                                              NULL,
                                                              orxDigChannelBitfieldAddr,
                                                              OVERLOAD_TH_AGC_PRE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set OverloadThPre for given ORx channel");
            return recoveryAction;
        }
        recoveryAction =  adrv903x_OrxDig_OverloadThHigh_BfSet(device,
                                                               NULL,
                                                               orxDigChannelBitfieldAddr,
                                                               OVERLOAD_TH_AGC_HIGH);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT( &device->common,
                               ADI_ADRV903X_ERRSRC_API,
                               ADI_COMMON_ERRCODE_API,
                               recoveryAction,
                               rxChannel,
                               "Error while attempting to set OverloadThHigh for given ORx channel");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxPowerMonitorInitialize(adi_adrv903x_Device_t* const device,
                                                                   const uint32_t               txChannelMask,
                                                                   const uint8_t                totalInputInterpoloationRate)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    static const uint8_t PROTECTION_RAMP_STEP_SIZE_IN_NS = 20U; /* Calculate the step size field for 20ns*/
    static const uint8_t PROTECTION_RAMP_STEP_SIZE_MAX = 15U;
    static const uint8_t PROTECTION_RAMP_STEP_SIZE_MIN = 10U;
    static const uint8_t PROTECTION_RAMP_MAX_ATTENUATION = 0x64U;
    static const uint8_t PROTECTION_RAMP_STEP_SIZE = 0x02U;
    static const uint8_t PROTECTION_DISABLE_ANALOG_DELAY_FOR_SRD_RAMP = 0x01U;
    static const uint8_t PROTECTION_DISABLE_ANALOG_DELAY_FOR_OTHER_RAMPS = 0x01U;
    static const uint8_t PROTECTION_RAMP_UP_ENABLE = 0x01U;
    static const uint8_t MAX_INTERPOLATION_RATE = 8U;
    uint32_t chanIdx = 0;
    uint8_t attenClkDivRatio = 0U;
    uint32_t stepSize = 0U;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    adrv903x_BfTxChanAddr_e txDigBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_DIG;
    uint32_t txChanSel = 0U;

    if ((totalInputInterpoloationRate == 0) || 
        (totalInputInterpoloationRate > MAX_INTERPOLATION_RATE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               totalInputInterpoloationRate,
                               "Total interpolation rate cannot be 0 or higher than 8");
        return recoveryAction;
    }

    /* Write the same init config to all Tx channels */
    for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
    {
        txChanSel = 1U << chanIdx;
        if (ADRV903X_BF_EQUAL(txChannelMask, txChanSel))
        {
            recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(txChanSel), &txBaseAddr);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                return recoveryAction;
            }

            recoveryAction = adrv903x_TxBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(txChanSel), &txDigBaseAddr);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid Tx Channel used while reading tx dig address");
                return recoveryAction;
            }

            recoveryAction = adrv903x_TxDig_TxAttenClkDivideRatio_BfGet(device,
                                                                        NULL,
                                                                        txDigBaseAddr,
                                                                        &attenClkDivRatio);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Overload Window Size");
                return recoveryAction;
            }

            recoveryAction = adrv903x_TxFuncs_PaProtectionOverloadWindowSize_BfSet(device,
                                                                                   NULL,
                                                                                   txBaseAddr,
                                                                                   totalInputInterpoloationRate * 2U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Overload Window Size");
                return recoveryAction;
            }

            recoveryAction = adrv903x_TxFuncs_PaProtectionOverloadCountTh_BfSet(device,
                                                                                NULL,
                                                                                txBaseAddr,
                                                                                totalInputInterpoloationRate);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Overload Count");
                return recoveryAction;
            }

            recoveryAction = adrv903x_TxFuncs_PaProtectionRampMaxAttenuation_BfSet(device,
                                                                                   NULL,
                                                                                   txBaseAddr,
                                                                                   PROTECTION_RAMP_MAX_ATTENUATION);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Power Protect Ramp Max Attenuation");
                return recoveryAction;
            }

            recoveryAction = adrv903x_TxFuncs_PaProtectionRampStepSize_BfSet(device,
                                                                             NULL,
                                                                             txBaseAddr,
                                                                             PROTECTION_RAMP_STEP_SIZE);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Power Protect Ramp Step Size");
                return recoveryAction;
            }

            /* Calculate step size as number of tx atten block clock cycles */
            stepSize = ((uint32_t)PROTECTION_RAMP_STEP_SIZE_IN_NS * device->initExtract.clocks.hsDigClk_kHz) / ((uint32_t)1U << (uint32_t)attenClkDivRatio);
            stepSize = stepSize / 1000000U;

            /* Saturate step size before writing it to register */
            if (stepSize < PROTECTION_RAMP_STEP_SIZE_MIN)
            {
                stepSize = PROTECTION_RAMP_STEP_SIZE_MIN;
            }
            else if (stepSize > PROTECTION_RAMP_STEP_SIZE_MAX)
            {
                stepSize = PROTECTION_RAMP_STEP_SIZE_MAX;
            }

            recoveryAction = adrv903x_TxFuncs_PaProtectionRampStepDuration_BfSet(device,
                                                                                 NULL,
                                                                                 txBaseAddr,
                                                                                 stepSize);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Power Protect Ramp Step Duration");
                return recoveryAction;
            }

            recoveryAction = adrv903x_TxFuncs_TxAttenPapDisAnalogDelay_BfSet(device,
                                                                             NULL,
                                                                             txBaseAddr,
                                                                             PROTECTION_DISABLE_ANALOG_DELAY_FOR_OTHER_RAMPS);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Power Protect Ramp Analog Delay");
                return recoveryAction;
            }

            recoveryAction = adrv903x_TxFuncs_SrdDisAnalogDelay_BfSet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      PROTECTION_DISABLE_ANALOG_DELAY_FOR_SRD_RAMP);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD Ramp Analog Delay");
                return recoveryAction;
            }

            recoveryAction = adrv903x_TxFuncs_PaProtectionGainRampUpEn_BfSet(device,
                                                                             NULL,
                                                                             txBaseAddr,
                                                                             PROTECTION_RAMP_UP_ENABLE);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Ramp Up Enable");
                return recoveryAction;
            }
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxToOrxMappingInit(adi_adrv903x_Device_t* const                       device,
                                                             const adi_adrv903x_TxToOrxMappingConfig_t* const   mappingConfig)
{
    static const adi_adrv903x_GpioSignal_e streamSig[ADI_ADRV903X_MAX_STREAMGPIO] = {
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_0, 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_1, 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_2, 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_3, 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_4, 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_5, 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_6, 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_7, 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_8, 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_9, 
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_10,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_11,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_12,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_13,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_14,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_15,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_16,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_17,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_18,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_19,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_20,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_21,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_22,
        ADI_ADRV903X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_23
    };
    
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t numBits = 0U;
    uint8_t idx = 0U;
    uint8_t allInvalid = ADI_TRUE;
    uint8_t allValid = ADI_TRUE;
    adi_adrv903x_GpioPinSel_e pin = ADI_ADRV903X_GPIO_INVALID;
    uint8_t pinIdx = 0U; 

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, mappingConfig);

    /*Range check tx Observability does NOT allow a Tx Channel to be observable on two ORx channels*/
    if (( ((mappingConfig->txObservability >> 0u)  & 0x3u) == 0x3u) ||
        ( ((mappingConfig->txObservability >> 2u)  & 0x3u) == 0x3u) ||
        ( ((mappingConfig->txObservability >> 4u)  & 0x3u) == 0x3u) ||
        ( ((mappingConfig->txObservability >> 6u)  & 0x3u) == 0x3u) ||
        ( ((mappingConfig->txObservability >> 8u)  & 0x3u) == 0x3u) ||
        ( ((mappingConfig->txObservability >> 10u) & 0x3u) == 0x3u) ||
        ( ((mappingConfig->txObservability >> 12u) & 0x3u) == 0x3u) ||
        ( ((mappingConfig->txObservability >> 14u) & 0x3u) == 0x3u))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 mappingConfig->txObservability,
                                 "Invalid Tx observability on ORx channels. A Tx channel cannot be observable by two ORx channels.");
        return recoveryAction;
    }
    
    /*Range check mapping mode*/
    switch (mappingConfig->mode)
    {
    case ADI_ADRV903X_TX_ORX_MAPPING_MODE_2BIT:
        numBits = 2U;
        break;
    case ADI_ADRV903X_TX_ORX_MAPPING_MODE_3BIT:
        numBits = 3U;
        break;
    case ADI_ADRV903X_TX_ORX_MAPPING_MODE_4BIT:
        numBits = 4U;
        break;
    case ADI_ADRV903X_TX_ORX_MAPPING_MODE_6BIT:
        numBits = 6U;
        break;
    case ADI_ADRV903X_TX_ORX_MAPPING_MODE_8BIT:
        numBits = 8U;
        break;
    default:
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 mappingConfig->mode,
                                 "Invalid Tx To ORx Mapping mode selected. Must select 2/3/4/6/8 bit mode");
        return recoveryAction;
        break;
    }
    
    /*Range check Auto Switching Enable/Disable: Orx Atten*/
    if ((mappingConfig->autoSwitchOrxAttenEnable != ADI_ENABLE) &&
        (mappingConfig->autoSwitchOrxAttenEnable != ADI_DISABLE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 mappingConfig->autoSwitchOrxAttenEnable,
                                 "Invalid Tx To ORx Mapping Auto Switching Enable setting for ORx Atten. Must select 0 or 1.");
        return recoveryAction;
    }
    
    /*Range check Auto Switching Enable/Disable: Orx Atten*/
    if ((mappingConfig->autoSwitchOrxNcoEnable != ADI_ENABLE) &&
        (mappingConfig->autoSwitchOrxNcoEnable != ADI_DISABLE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 mappingConfig->autoSwitchOrxNcoEnable,
                                 "Invalid Tx To ORx Mapping Auto Switching Enable setting for ORx NCO. Must select 0 or 1.");
        return recoveryAction;
    }

    /* Range check GPIO LUT entries */
    for (idx = 0U; idx < ADI_ADRV903X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE; idx++)
    {

        if ((mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX0)         &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX1)         &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX2)         &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX3)         &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX4)         &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX5)         &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX6)         &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX7)         &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_NONE)        &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_NO_CHANGE)   &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_0)     &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_1)     &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_2)     &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_3)     &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_4)     &&
            (mappingConfig->pinTableOrx0[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_5))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     mappingConfig->pinTableOrx0[idx],
                                     "Invalid Tx To ORx Mapping GPIO Table entry for ORx0. Must be valid Mapping selection Tx0-7, Extended Mapping Selection, NONE, or NO CHANGE");
            return recoveryAction;
        }

        if ((mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX0)         &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX1)         &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX2)         &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX3)         &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX4)         &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX5)         &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX6)         &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX7)         &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_NONE)        &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_NO_CHANGE)   &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_0)     &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_1)     &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_2)     &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_3)     &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_4)     &&
            (mappingConfig->pinTableOrx1[idx] != ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_5))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     mappingConfig->pinTableOrx1[idx],
                                     "Invalid Tx To ORx Mapping GPIO Table entry for ORx1. Must be valid Mapping selection Tx0-7, Extended Mapping Selection, NONE, or NO CHANGE");
            return recoveryAction;
        }

    }

    /*Range check GPIO select*/
    for (idx = 0U; idx < numBits; idx++)
    {
        if ((mappingConfig->gpioSelect[idx] <  ADI_ADRV903X_GPIO_00) ||
            (mappingConfig->gpioSelect[idx] >= ADI_ADRV903X_GPIO_INVALID))
        {
            allValid = ADI_FALSE;
        }
        else
        {
            allInvalid = ADI_FALSE;
        }
    }

    if ((allValid   == ADI_FALSE) &&
        (allInvalid == ADI_FALSE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 mappingConfig->mode,
                                 "Invalid GPIO selections for Tx To ORx Mapping mode. Must select a pin for all Mode bits or 0 Mode bits.");
        return recoveryAction;
    }
    
    /* If all selected GPIOs were valid, connect pins*/
    else if (allValid == ADI_TRUE)
    {
        for (idx = 0U; idx < numBits; idx++)
        {
            pin = mappingConfig->gpioSelect[idx];
            pinIdx = (uint8_t)pin;
            recoveryAction = adrv903x_GpioSignalSet(device, pin, streamSig[pinIdx], 0U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "GPIO setup error while initializing Tx to ORx Mapping Configuration.");
                return recoveryAction;
            }
        }

    }

    /* If all invalid, no GPIO setup functions to call */
    else if(allInvalid)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    }

    /*Store tx to orx mapping config in device handle for later readback*/
    /*Note that simply writing to the device handle does not affect the pre-loaded Stream Image.*/
    /*The device handle is originally updated in adi_adrv903x_StreamImageWrite */
    ADI_LIBRARY_MEMCPY(&device->devStateInfo.txToOrxMappingConfig, mappingConfig, sizeof(adi_adrv903x_TxToOrxMappingConfig_t));

    /* Translate Extended Flag */

    for (idx = 0U; idx < ADI_ADRV903X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE; idx++)
    {
        if ((device->devStateInfo.txToOrxMappingConfig.pinTableOrx0[idx] > ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_NONE) &&
            (device->devStateInfo.txToOrxMappingConfig.pinTableOrx0[idx] < ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_NO_CHANGE))
        {
            device->devStateInfo.txToOrxMappingConfig.pinTableOrx0[idx] = (adi_adrv903x_TxToOrxMappingPinTable_e) ((uint32_t) (device->devStateInfo.txToOrxMappingConfig.pinTableOrx0[idx] & ~TX_TO_ORX_EXTENDED_MAPPING_FLAG));
        }

        if ((device->devStateInfo.txToOrxMappingConfig.pinTableOrx1[idx] > ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_NONE) &&
            (device->devStateInfo.txToOrxMappingConfig.pinTableOrx1[idx] < ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_NO_CHANGE))
        {
            device->devStateInfo.txToOrxMappingConfig.pinTableOrx1[idx] = (adi_adrv903x_TxToOrxMappingPinTable_e) ((uint32_t) (device->devStateInfo.txToOrxMappingConfig.pinTableOrx1[idx] & ~TX_TO_ORX_EXTENDED_MAPPING_FLAG));
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_UartCfg(adi_adrv903x_Device_t* const      device,
                                                  const adi_adrv903x_GpioPinSel_e   pinSelect)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_GpioSignal_e signal = ADI_ADRV903X_GPIO_SIGNAL_UNUSED;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Range check gpio */
    if ((pinSelect < ADI_ADRV903X_GPIO_00) ||
        (pinSelect >= ADI_ADRV903X_GPIO_INVALID))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pinSelect,
                               "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }

    switch (pinSelect)
    {
        case ADI_ADRV903X_GPIO_12:
            signal = ADI_ADRV903X_GPIO_SIGNAL_UART_PADTXSOUT;
            break;
        default:
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   pinSelect,
                                   "Invalid GPIO selected.");
            return recoveryAction;
            break;
    }

    /* Set the signal for this gpio */
    recoveryAction = adrv903x_GpioSignalSet(device, pinSelect, signal, 0x00U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "GpioSignalSet issue");
        return recoveryAction;
    }

    return recoveryAction;
}

static uint32_t adrv903x_ErrReportConvert(const adi_adrv903x_Version_t version)
{
    uint32_t convertedVersion = 0U;

    /* \note Data Type Limitation due to static analysis issue with Variable Reporting; 32-bit Limit */

    convertedVersion =  ( uint32_t) ((uint8_t) version.buildVer)                |   /* Build Version (LSB) */
                        ((uint32_t) ((uint8_t) version.maintenanceVer)  << 8U)  |   /* Maintenance Version */
                        ((uint32_t) ((uint8_t) version.minorVer)        << 16U) |   /* Minor Version */
                        ((uint32_t) ((uint8_t) version.majorVer)        << 24U);    /* Major Version (MSB) */

    return convertedVersion;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_ApiVersionRangeCheck( adi_adrv903x_Device_t* const            device,
                                                                const adi_adrv903x_Version_t* const     version,
                                                                const adi_adrv903x_Version_t* const     minVersion,
                                                                const adi_adrv903x_Version_t* const     maxVersion)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    uint32_t actualVersion = 0U;
    uint32_t minimumVersion = 0U;
    uint32_t maximumVersion = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, version);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, minVersion);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, maxVersion);

    if ((version->majorVer < minVersion->majorVer) || (version->majorVer > maxVersion->majorVer))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, version->majorVer, "Unexpected Major Version");
    }

    if ((version->minorVer < minVersion->minorVer) || (version->minorVer > maxVersion->minorVer))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, version->minorVer, "Unexpected Minor Version");
    }

    if ((version->maintenanceVer < minVersion->maintenanceVer) || (version->maintenanceVer > maxVersion->maintenanceVer))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, version->maintenanceVer, "Unexpected Maintenance Version");
    }

    if ((version->buildVer < minVersion->buildVer) || (version->buildVer > maxVersion->buildVer))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, version->buildVer, "Unexpected Build Version");
    }

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        actualVersion = adrv903x_ErrReportConvert(*version);
        minimumVersion = adrv903x_ErrReportConvert(*minVersion);
        maximumVersion = adrv903x_ErrReportConvert(*maxVersion);
         
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, actualVersion,  "Actual Version (Note: For Illustrative Purposes Only. Packing For Error Reporting Restricts Each Version to 8-Bits)");
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, minimumVersion, "Minimum Version Expected (Note: For Illustrative Purposes Only. Packing For Error Reporting Restricts Each Version to 8-Bits)");
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, maximumVersion, "Maximum Version Expected (Note: For Illustrative Purposes Only. Packing For Error Reporting Restricts Each Version to 8-Bits)");
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_SpiStreamingEntry( adi_adrv903x_Device_t* const device)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t spiConfigB = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Mask with ADI_TRUE here to check bit 0 */
    /* Each function that turns on SPI streaming should also be turning it off, however,
     * if it happens that SPI streaming is On we do not need to do anything. 
     * This if-statement only evaluates to TRUE when spiStreaming is NOT currently ON
     * and when allowSpiStreaming is TRUE.
     */
    if (((device->devStateInfo.spiStreamingOn & ADI_TRUE) != ADI_TRUE) && 
        ((device->devStateInfo.spiOptions.allowSpiStreaming & ADI_TRUE) == ADI_TRUE))
    {
        /* Writing CONFFIG_B to zero puts the part in streaming mode. */
        recoveryAction = adi_adrv903x_Register32Write(device, NULL, ADRV903X_ADDR_SPI_INTERFACE_CONFIG_B, spiConfigB, 0xFFU);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue writing to CONFIG_B register");
            return recoveryAction;
        }

        device->devStateInfo.spiStreamingOn = ADI_TRUE;

        /* Force an address word on the next SPI call by giving an invalid address here.
         * adrv903x_DirectSpiDataPack checks this value as part of the logic to determine if an address word must
         * be inserted.
         * */
        device->devStateInfo.previousSpiStreamingAddress = 0xFFFFFFFFU;
    }
    else
    {
        /* No Action Required */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_SpiStreamingExit(adi_adrv903x_Device_t* const device)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t spiConfigB = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Mask with ADI_TRUE here to check bit 0 */
    /* Only perform the write when it's needed, ie Spi streaming is ON.
     * */
    if ((device->devStateInfo.spiStreamingOn & ADI_TRUE) == ADI_TRUE)
    {
        spiConfigB |= ADRV903X_CONFIG_B_SINGLE_INSTRUCTION;
        recoveryAction = adi_adrv903x_Register32Write(device, NULL, ADRV903X_ADDR_SPI_INTERFACE_CONFIG_B, spiConfigB, 0xFFU);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue writing to CONFIG_B register");
            return recoveryAction;
        }

        device->devStateInfo.spiStreamingOn = ADI_FALSE;
    }
    else
    {
        /* No Action Required */
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    }

    return recoveryAction;
}
