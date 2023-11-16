/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adrv904x_dfe_cfr.c
*
* ADRV904X API Version: 2.9.0.4
*/

#include "../../private/include/adrv904x_dfe_cfr.h"
#include "adi_adrv904x_dfe_cfr_types.h"


#include "adi_adrv904x_hal.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PRIVATE_DFE_CFR

ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrRegsBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_TxChannels_e txChannel,
                                                                    adrv904x_BfTxCfrRegChanAddr_e* const cfrChannelBitfieldAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cfrChannelBitfieldAddr);

    if (((device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               device->devStateInfo.initializedChannels,
                               "Selected Tx channel is not initialized for this device.");
        return recoveryAction;
    }

    switch (txChannel)
    {
    case ADI_ADRV904X_TX0:
        *cfrChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
        break;

    case ADI_ADRV904X_TX1:
        *cfrChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG;
        break;

    case ADI_ADRV904X_TX2:
        *cfrChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG;
        break;
    case ADI_ADRV904X_TX3:
        *cfrChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG;
        break;

    case ADI_ADRV904X_TX4:
        *cfrChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG;
        break;

    case ADI_ADRV904X_TX5:
        *cfrChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG;
        break;

    case ADI_ADRV904X_TX6:
        *cfrChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG;
        break;

    case ADI_ADRV904X_TX7:
        *cfrChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG;
        break;

    default:
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   txChannel,
                                   "Invalid txChannel parameter ");
            return recoveryAction;
        }
    }

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrWritePeakThreshSq(adi_adrv904x_Device_t* const device,
                                                               const uint32_t cfrPeakThresholdScaler,
                                                               const uint32_t cfrPeakThreshold,
                                                               const adrv904x_BfTxCfrRegChanAddr_e engineBaseAddr,
                                                               const uint32_t configSelectMask,
                                                               const uint8_t engineId)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    static const uint32_t CFR_PEAK_THR_MAX = 64U * (1U << 16U);
    static const uint32_t CFR_PEAK_THRSCALER_MAX = 1U << 16U;

    uint64_t tempScalerVal = ((uint64_t)cfrPeakThresholdScaler * (uint64_t)cfrPeakThresholdScaler) >> (uint64_t)16U;
    uint64_t tempThreshVal = ((uint64_t)cfrPeakThreshold * (uint64_t)cfrPeakThreshold) >> (uint64_t)16U;
    uint64_t detThresholdSq = (tempScalerVal * tempThreshVal) >> (uint64_t)16U;

    if((cfrPeakThreshold >= CFR_PEAK_THR_MAX) ||
       (cfrPeakThresholdScaler >= CFR_PEAK_THRSCALER_MAX))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrPeakThreshold,
                               "CFR threshold is out of range");
        return recoveryAction;
    }

    if (ADRV904X_BF_EQUAL(configSelectMask, (uint32_t)ADI_ADRV904X_CFR_CFG0))
    {
        recoveryAction = adrv904x_TxCfrReg_Cfr0ThrSq_BfSet(device,
                                                           NULL,
                                                           engineBaseAddr,
                                                           engineId,
                                                           (uint32_t)detThresholdSq);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sq - Cfg0");
            return recoveryAction;
        }
    }

    if (ADRV904X_BF_EQUAL(configSelectMask, (uint32_t)ADI_ADRV904X_CFR_CFG1))
    {
        recoveryAction = adrv904x_TxCfrReg_Cfr1ThrSq_BfSet(device,
                                                           NULL,
                                                           engineBaseAddr,
                                                           engineId,
                                                           (uint32_t)detThresholdSq);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sq - Cfg1");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrWritePeakThreshSqrt(adi_adrv904x_Device_t* const device,
                                                                 const uint32_t cfrPeakThresholdScaler,
                                                                 const uint32_t cfrPeakThreshold,
                                                                 const adrv904x_BfTxCfrRegChanAddr_e engineBaseAddr,
                                                                 const uint32_t configSelectMask,
                                                                 const uint8_t engineId)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    static const uint32_t CFR_PEAK_THR_MAX = 64U * (1U << 16U);
    static const uint32_t CFR_PEAK_THRSCALER_MAX = 1U << 16U;

    static const uint64_t TWO_TO_16_OVER_SQRT_TWO = 46341U;
    uint64_t detThresholdSq = (uint64_t)cfrPeakThreshold * (uint64_t)cfrPeakThresholdScaler * TWO_TO_16_OVER_SQRT_TWO;
    detThresholdSq >>= (uint64_t)31U;

    if ((cfrPeakThreshold >= CFR_PEAK_THR_MAX) ||
        (cfrPeakThresholdScaler >= CFR_PEAK_THRSCALER_MAX))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrPeakThreshold,
                               "CFR threshold is out of range");
        return recoveryAction;
    }

    if (ADRV904X_BF_EQUAL(configSelectMask, (uint32_t)ADI_ADRV904X_CFR_CFG0))
    {
        recoveryAction = adrv904x_TxCfrReg_Cfr0ThrSqrt_BfSet(device,
                                                           NULL,
                                                           engineBaseAddr,
                                                           engineId,
                                                           (uint32_t)detThresholdSq);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sqrt - Cfg0");
            return recoveryAction;
        }
    }

    if (ADRV904X_BF_EQUAL(configSelectMask, (uint32_t)ADI_ADRV904X_CFR_CFG1))
    {
        recoveryAction = adrv904x_TxCfrReg_Cfr1ThrSqrt_BfSet(device,
                                                           NULL,
                                                           engineBaseAddr,
                                                           engineId,
                                                           (uint32_t)detThresholdSq);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sqrt - Cfg1");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrWriteCorrectionThresh(adi_adrv904x_Device_t* const device,
                                                                   const uint32_t cfrCorrectionThresholdScaler,
                                                                   const uint32_t cfrPeakThreshold,
                                                                   const adrv904x_BfTxCfrRegChanAddr_e engineBaseAddr,
                                                                   const uint32_t configSelectMask,
                                                                   const uint8_t engineId)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    static const uint32_t CFR_PEAK_THR_MAX = 64U * (1U << 16U);
    static const uint32_t CFR_CORR_THRSCALER_MAX = 1U << 16U;

    uint64_t corThresholdSq = (uint64_t)cfrPeakThreshold * (uint64_t)cfrCorrectionThresholdScaler;
    corThresholdSq >>= (uint64_t)15U;

    if ((cfrPeakThreshold >= CFR_PEAK_THR_MAX) ||
        (cfrCorrectionThresholdScaler >= CFR_CORR_THRSCALER_MAX))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrPeakThreshold,
                               "CFR threshold is out of range");
        return recoveryAction;
    }

    if (ADRV904X_BF_EQUAL(configSelectMask, (uint32_t)ADI_ADRV904X_CFR_CFG0))
    {
        recoveryAction = adrv904x_TxCfrReg_Cfr0ThrCor_BfSet(device,
                                                            NULL,
                                                            engineBaseAddr,
                                                            engineId,
                                                            (uint32_t)corThresholdSq);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing correction threshold - Cfg0");
            return recoveryAction;
        }
    }

    if (ADRV904X_BF_EQUAL(configSelectMask, (uint32_t)ADI_ADRV904X_CFR_CFG1))
    {
        recoveryAction = adrv904x_TxCfrReg_Cfr1ThrCor_BfSet(device,
                                                             NULL,
                                                             engineBaseAddr,
                                                             engineId,
                                                             (uint32_t)corThresholdSq);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing correction threshold - Cfg1");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e writeCfrPulseRam(adi_adrv904x_Device_t* const   device,
                                                 const uint8_t                  interpolationRateMult,
                                                 const int16_t* const           coeffRealHalfPulse,
                                                 const int16_t* const           coeffImagHalfPulse,
                                                 const uint16_t                 numCoeffs,
                                                 const uint32_t                 baseAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint32_t rowMinSampleIdx = 0U;
    uint32_t rowMaxSampleIdx = 0U;
    uint32_t rowIdx = 0U;

    uint32_t rowToWrite[ADI_ADRV904X_CFR_PULSE_RAM_ROW_SAMPLE_SIZE] = { 0U };
    uint32_t sampleCounter = 0U;
    uint32_t rowCounter = 0U;
    uint32_t addressToWrite[ADI_ADRV904X_CFR_PULSE_RAM_ROW_SAMPLE_SIZE] = { 0U };
    uint32_t maskToWrite[ADI_ADRV904X_CFR_PULSE_RAM_ROW_SAMPLE_SIZE] = { 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, coeffRealHalfPulse);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, coeffImagHalfPulse);

    for (rowIdx = 0; rowIdx < ADI_ADRV904X_CFR_PULSE_RAM_NUM_OF_ROWS; rowIdx++)
    {
        rowMaxSampleIdx = rowMinSampleIdx + interpolationRateMult * 3U;

        if (numCoeffs > rowMaxSampleIdx)
        {
            rowCounter = 0U;
            for (sampleCounter = rowMinSampleIdx;
                 sampleCounter < (rowMinSampleIdx + (ADI_ADRV904X_CFR_PULSE_RAM_ROW_SAMPLE_SIZE * interpolationRateMult));
                 sampleCounter += interpolationRateMult)
            {
                rowToWrite[rowCounter++] = ((((uint32_t)coeffRealHalfPulse[sampleCounter] << 0U) & 0x0000FFFFU) |
                                            (((uint32_t)coeffImagHalfPulse[sampleCounter] << 16U) & 0xFFFF0000U));
            }

            addressToWrite[0] = baseAddr + rowIdx * ADI_ADRV904X_CFR_PULSE_RAM_ROW_SIZE;
            addressToWrite[1] = addressToWrite[0] + sizeof(uint32_t);
            addressToWrite[2] = addressToWrite[1] + sizeof(uint32_t);
            addressToWrite[3] = addressToWrite[2] + sizeof(uint32_t);


            recoveryAction = adi_adrv904x_Registers32Write(device,
                                                           NULL,
                                                           (const uint32_t*)&addressToWrite[0],
                                                           (const uint32_t*)&rowToWrite[0U],
                                                           (const uint32_t*)&maskToWrite[0U],
                                                           ADI_ADRV904X_CFR_PULSE_RAM_ROW_SAMPLE_SIZE);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Write Issue");
                return recoveryAction;
            }
        }

        rowMinSampleIdx = (rowMaxSampleIdx + interpolationRateMult) % 1023U;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrCorrectionPulseWrite(adi_adrv904x_Device_t* const              device,
                                                                  const adi_adrv904x_CfrPulseSel_e          pulseSelect,
                                                                  const uint32_t                            txChanIdx,
                                                                  const uint8_t                             interpolationRate,
                                                                  const adi_adrv904x_CfrCorrectionPulse_t* const  cfrCorrectionPulse)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    const uint32_t pulseBaseAddresses[ADI_ADRV904X_MAX_TXCHANNELS] =
    {
        ADI_ADVRV904X_TX0_PULSE_RAM_BASEADDR,
        ADI_ADVRV904X_TX1_PULSE_RAM_BASEADDR,
        ADI_ADVRV904X_TX2_PULSE_RAM_BASEADDR,
        ADI_ADVRV904X_TX3_PULSE_RAM_BASEADDR,
        ADI_ADVRV904X_TX4_PULSE_RAM_BASEADDR,
        ADI_ADVRV904X_TX5_PULSE_RAM_BASEADDR,
        ADI_ADVRV904X_TX6_PULSE_RAM_BASEADDR,
        ADI_ADVRV904X_TX7_PULSE_RAM_BASEADDR
    };

    uint8_t interpolationRateMult = 0U;
    uint32_t pulseLengthCheck = 0U;
    uint32_t ramIdx = 0U;
    uint32_t addressOffset = 0U;
    uint32_t pulseOffset = 0U;
    uint32_t chanSel = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cfrCorrectionPulse);

    /* Interpolation rate in HW  0=1x // 1=2x // 2=4x*/
    interpolationRateMult = 1U << interpolationRate;
    if ((cfrCorrectionPulse->numCoeffs % interpolationRateMult) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrCorrectionPulse->numCoeffs,
                               "Number of coefficients should be multiple of interpolation rate");
        return recoveryAction;
    }

    pulseLengthCheck = cfrCorrectionPulse->numCoeffs / interpolationRateMult;
    if ((pulseLengthCheck % 4U) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrCorrectionPulse->numCoeffs,
                               "Number of coefficients/interpolation rate should be multiple of 4");
        return recoveryAction;
    }

    chanSel = 1U << txChanIdx;
    recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txCfrBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChanIdx, "Invalid Tx Channel used to determine CFR base address");
        return recoveryAction;
    }

    for (ramIdx = 0U; ramIdx < ADI_ADRV904X_CFR_NUM_OF_PULSE_RAMS; ++ramIdx)
    {
        if (ADRV904X_BF_EQUAL(cfrCorrectionPulse->ramSelectMask, (1U << ramIdx)))
        {
            if (ramIdx > 0U)
            {
                /* Set this RAM bank as shadow RAM */
                recoveryAction = adrv904x_TxCfrReg_CfrRamShadowRamSel_BfSet(device,
                                                                            NULL,
                                                                            txCfrBaseAddr,
                                                                            (ramIdx - 1U));
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing Shadow RAM bf");
                    return recoveryAction;
                }
            }

            /* Each RAM has 2 pulse data and we're adding pulse offset  */
            if (pulseSelect == ADI_ADRV904X_CFR_PULSE0)
            {
                pulseOffset = 0U;
            }
            else
            {
                pulseOffset = ADI_ADRV904X_CFR_PULSE_RAM_SIZE;
            }

            addressOffset = ramIdx * ADI_ADRV904X_CFR_PULSE_RAM_SIZE * 2U + pulseOffset;
            recoveryAction = writeCfrPulseRam(device,
                                              interpolationRateMult,
                                              cfrCorrectionPulse->coeffRealHalfPulse,
                                              cfrCorrectionPulse->coeffImagHalfPulse,
                                              cfrCorrectionPulse->numCoeffs,
                                              pulseBaseAddresses[txChanIdx] + addressOffset);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CFR pulse write failed");
                return recoveryAction;
            }
        }
    }

    /* Write Half pulse length. Programmed pulse length = numOfCoefficents / interpolationRate - 1 */
    if (pulseSelect == ADI_ADRV904X_CFR_PULSE0)
    {
        recoveryAction = adrv904x_TxCfrReg_Cfr0PulHpl_BfSet(device,
                                                            NULL,
                                                            txCfrBaseAddr,
                                                            (pulseLengthCheck - 1U));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing pulse 0 half pulse length");
            return recoveryAction;
        }

    }
    else
    {
        recoveryAction = adrv904x_TxCfrReg_Cfr1PulHpl_BfSet(device,
                                                            NULL,
                                                            txCfrBaseAddr,
                                                            (pulseLengthCheck - 1U));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing pulse 1 half pulse length");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrPulseWriteRangeCheck(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_CfrCorrectionPulse_t* const cfrPulseData)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cfrPulseData);

    if (cfrPulseData->numCoeffs > ADI_ADRV904X_MAX_CFR_CORRECTION_PULSE_LEN)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrPulseData->numCoeffs,
                               "Number of pulses cannot be greater than ADI_ADRV904X_MAX_CFR_CORRECTION_PULSE_LEN");
        return recoveryAction;
    }

    if (((cfrPulseData->pulseSelectMask & (~(uint32_t)ADI_ADRV904X_CFR_PULSEALL)) != 0U) ||
         (cfrPulseData->pulseSelectMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrPulseData->pulseSelectMask,
                               "Invalid pulse is selected. Valid values are any combinations of Pulse0/1");
        return recoveryAction;
    }

    if (((cfrPulseData->ramSelectMask & (~(uint32_t)(ADI_ADRV904X_CFR_PULSE_RAM_ALL | ADI_ADRV904X_CFR_PULSE_RAM_SINGLESHOT))) != 0U) ||
        (cfrPulseData->ramSelectMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrPulseData->ramSelectMask,
                               "Invalid ram is selected. User can either select any combinations of RAM0/1/2/3/4 or SINGLESHOT");
        return recoveryAction;
    }

    if(ADRV904X_BF_EQUAL(cfrPulseData->ramSelectMask, ADI_ADRV904X_CFR_PULSE_RAM_SINGLESHOT) &&
      ((cfrPulseData->ramSelectMask & (~(uint32_t)ADI_ADRV904X_CFR_PULSE_RAM_SINGLESHOT)) != 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrPulseData->ramSelectMask,
                               "Invalid ram is selected. User cannot select SINGLESHOT and individual RAM blocks at the same time");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrDatapathBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                        const adi_adrv904x_TxChannels_e txChannel,
                                                                        adrv904x_BfTxDatapathChanAddr_e* const txDatapathChannelBitfieldAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, txDatapathChannelBitfieldAddr);

    if (((device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               device->devStateInfo.initializedChannels,
                               "Selected Tx channel is not initialized for this device.");
        return recoveryAction;
    }

    switch (txChannel)
    {
    case ADI_ADRV904X_TX0:
        *txDatapathChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_0__TX_DATAPATH;
        break;

    case ADI_ADRV904X_TX1:
        *txDatapathChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_1__TX_DATAPATH;
        break;

    case ADI_ADRV904X_TX2:
        *txDatapathChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_2__TX_DATAPATH;
        break;
    case ADI_ADRV904X_TX3:
        *txDatapathChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_3__TX_DATAPATH;
        break;

    case ADI_ADRV904X_TX4:
        *txDatapathChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_4__TX_DATAPATH;
        break;

    case ADI_ADRV904X_TX5:
        *txDatapathChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_5__TX_DATAPATH;
        break;

    case ADI_ADRV904X_TX6:
        *txDatapathChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_6__TX_DATAPATH;
        break;

    case ADI_ADRV904X_TX7:
        *txDatapathChannelBitfieldAddr = ADRV904X_BF_SLICE_TX_7__TX_DATAPATH;
        break;

    default:
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   txChannel,
                                   "Invalid txChannel parameter ");
            return recoveryAction;
        }
    }

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}
