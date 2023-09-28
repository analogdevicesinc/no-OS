// SPDX-License-Identifier: GPL-2.0
/**
* \file adrv9025_tx.c
* \brief Contains ADRV9025 transmit related function implementations
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_tx.h"
#include "../../private/include/adrv9025_bf_core.h"
#include "../../private/include/adrv9025_bf_txdac_mem_map.h"
#include "../../private/include/adrv9025_bf_pll_mem_map.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_gpio.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"
#include "../../private/include/adrv9025_bf_txdac_mem_map_types.h"

#define IF_INVALID_TX_CHANNEL_RETURN(txChannel); \
{\
        if ((txChannel != ADI_ADRV9025_TX1) && \
            (txChannel != ADI_ADRV9025_TX2) && \
            (txChannel != ADI_ADRV9025_TX3) && \
            (txChannel != ADI_ADRV9025_TX4) && \
            (txChannel != ADI_ADRV9025_TXALL) && \
            (txChannel > ADI_ADRV9025_TXALL)) \
        { \
            ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, txChannel, \
                         "Invalid Tx Channel Parameter encountered"); \
            ADI_ERROR_RETURN(device->common.error.newAction); \
        } \
}

#define ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_UPDATE_PIN_FEATURE 1
#define ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_CTRL_PIN_FEATURE 2

uint16_t adrv9025_TxDacAddrDecode(adi_adrv9025_Device_t* device,
                                  uint32_t               channelMask)
{
    if (device == NULL)
    {
        return 0;
    }

    if (((device->devStateInfo.initializedChannels >> ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET) & channelMask) != channelMask)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelMask,
                         "Tx channel not in device initializedChannels");
        return 0;
    }

    switch (channelMask)
    {
    case ADI_ADRV9025_TX1:
        return ADRV9025_BF_TXDAC_CH0;
    case ADI_ADRV9025_TX2:
        return ADRV9025_BF_TXDAC_CH1;
    case ADI_ADRV9025_TX3:
        return ADRV9025_BF_TXDAC_CH2;
    case ADI_ADRV9025_TX4:
        return ADRV9025_BF_TXDAC_CH3;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelMask,
                         "Tx channel is invalid");
        return (0);
    }
    }
}

uint16_t adrv9025_TxAddrDecode(adi_adrv9025_Device_t* device,
                               uint32_t               channelMask)
{
    if (device == NULL)
    {
        return 0;
    }

    if (((device->devStateInfo.initializedChannels >> ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET) & channelMask) != channelMask)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelMask,
                         "TX channel not in device initializedChannels");
        return 0;
    }

    switch (channelMask)
    {
    case ADI_ADRV9025_TX1:
        return ADRV9025_ADDR_CH0_TX;
    case ADI_ADRV9025_TX2:
        return ADRV9025_ADDR_CH1_TX;
    case ADI_ADRV9025_TX3:
        return ADRV9025_ADDR_CH2_TX;
    case ADI_ADRV9025_TX4:
        return ADRV9025_ADDR_CH3_TX;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelMask,
                         "Tx channel is invalid");
        return (0);
    }
    }
}

int32_t adrv9025_TxAttenCtrlPinReportError(adi_adrv9025_Device_t* device,
                                           int32_t                sharedResourceArr[],
                                           int32_t                recoveryAction,
                                           const char*            customError)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID                        = ADRV9025_FEATURE_TX_ATTEN_CTRL_PIN;
    static const uint32_t             NUM_GPIOS_IN_TX_ATTEN_CTRL_PIN_FEATURE = 2U;

    int32_t recAct                = ADI_COMMON_ACT_NO_ACTION;
    uint8_t resourceReleaseStatus = ADI_FAILURE;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        sharedResourceArr);

    if (recoveryAction > ADI_COMMON_ACT_NO_ACTION)
    {
        recAct = adrv9025_SharedResourcesRelease(device,
                                                 ADRV9025_SHARED_RESOURCE_GPIO,
                                                 &sharedResourceArr[0],
                                                 NUM_GPIOS_IN_TX_ATTEN_CTRL_PIN_FEATURE,
                                                 THIS_FEATURE_ID,
                                                 &resourceReleaseStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recAct,
                         NULL,
                         "Error while attempting to release GPIO shared resouce for GPIO Tx attenuation ctrl Increment/Decrement Pins");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recAct,
                             resourceReleaseStatus,
                             "Error while attempting to release GPIO shared resouce for GPIO Tx attenutaion ctrl Increment/Decrement Pins");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     customError);

    return recoveryAction;
}

int32_t adrv9025_TxAttenSpi2CtrlPinReportError(adi_adrv9025_Device_t* device,
                                               int32_t                sharedResourceArr[],
                                               int32_t                recoveryAction,
                                               const char*            customError)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID                  = ADRV9025_FEATURE_TX_ATTEN_CTRL_PIN;
    static const uint32_t             NUM_GPIOS_IN_TX_SPI2_PIN_FEATURE = 1U;

    int32_t recAct                = ADI_COMMON_ACT_NO_ACTION;
    uint8_t resourceReleaseStatus = ADI_FAILURE;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(sharedResourceArr);

    if (recoveryAction > ADI_COMMON_ACT_NO_ACTION)
    {
        recAct = adrv9025_SharedResourcesRelease(device,
                                                 ADRV9025_SHARED_RESOURCE_GPIO,
                                                 &sharedResourceArr[0],
                                                 NUM_GPIOS_IN_TX_SPI2_PIN_FEATURE,
                                                 THIS_FEATURE_ID,
                                                 &resourceReleaseStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to release GPIO shared resouce for GPIO Tx attenuation ctrl Spi2 Pins");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error while attempting to release GPIO shared resouce for GPIO Tx attenutaion ctrl Spi2 Pins");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     customError);

    return recAct;
}

int32_t adrv9025_TxAttenPinCtrlCfgSetRangeCheck(adi_adrv9025_Device_t*        device,
                                                adi_adrv9025_TxAttenPinCfg_t* txAttenPinCtrlCfg)
{
    static const uint8_t MAX_ATTEN_STEP = 31;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Check rxGainPinCtrlCfg pointer is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenPinCtrlCfg);

    /*Check that gain increment step is between 0-31*/
    if (txAttenPinCtrlCfg->stepSize > MAX_ATTEN_STEP)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenPinCtrlCfg->stepSize,
                         "Attenuation step exceeds limit for GPIO gain control config. Valid range 0-31");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*GPIOs 0-15 are valid for GPIO gain control*/
    if (txAttenPinCtrlCfg->txAttenIncPin > ADI_ADRV9025_GPIO_15)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenPinCtrlCfg->txAttenIncPin,
                         "Attenuation Increment pin for GPIO ctrl is out of range. Valid values GPIO_0 - GPIO_15");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*GPIOs 0-15 are valid for GPIO gain control*/
    if (txAttenPinCtrlCfg->txAttenDecPin > ADI_ADRV9025_GPIO_15)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenPinCtrlCfg->txAttenDecPin,
                         "Attenuation Decrement pin for GPIO ctrl is out of range. Valid values GPIO_0 - GPIO_15");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that attenuation increment and decrement GPIO pins are not the same*/
    if (txAttenPinCtrlCfg->txAttenDecPin == txAttenPinCtrlCfg->txAttenIncPin)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenPinCtrlCfg,
                         "Attenuation Decrement pin cannot be same as attenuation Increment pin for GPIO attenuation ctrl");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_TxAttenModeSetRangeCheck(adi_adrv9025_Device_t*                device,
                                          adi_adrv9025_TxAttenMode_e            txAttenMode,
                                          adi_adrv9025_TxAttenSimultUpdateCfg_t simultaneousUpdateCfg,
                                          adi_adrv9025_TxAttenPinCfg_t          gpioModePinCfg)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Check that attenuation mode is in range */
    if (txAttenMode > ADI_ADRV9025_TXATTEN_SPI2_MODE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenMode,
                         "Selected Tx attenuation mode is invalid, Valid Values BYPASS/SPI/GPIO/SPI2");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((simultaneousUpdateCfg.simultaneousUpdate != ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_DISABLED) && (txAttenMode != ADI_ADRV9025_TXATTEN_SPI_MODE))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         simultaneousUpdate,
                         "Tx attenuation simultaneous update feature can only be used on Tx attenuation Spi mode");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((simultaneousUpdateCfg.simultaneousUpdate == ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_DISABLED) && (simultaneousUpdateCfg.gpioUpdatePin !=
        ADI_ADRV9025_GPIO_INVALID))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         simultaneousUpdate,
                         "GPIO selection should be invalid, when Tx atteunation simultaneous update feature is not being used.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txAttenMode != ADI_ADRV9025_TXATTEN_GPIO_MODE) && ((gpioModePinCfg.txAttenIncPin != ADI_ADRV9025_GPIO_INVALID) || (gpioModePinCfg.txAttenDecPin !=
        ADI_ADRV9025_GPIO_INVALID)))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         gpioModePinCfg,
                         "Inc/Dec pins should be selected INVALID when GPIO mode is not being used");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txAttenMode == ADI_ADRV9025_TXATTEN_GPIO_MODE) && ((gpioModePinCfg.txAttenIncPin == ADI_ADRV9025_GPIO_INVALID) || (gpioModePinCfg.txAttenDecPin ==
        ADI_ADRV9025_GPIO_INVALID)))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         gpioModePinCfg,
                         "Inc/Dec pins shouldn't be invalid when Tx attenution mode is GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_TxAttenCfgSetRangeCheck(adi_adrv9025_Device_t*     device,
                                         adi_adrv9025_TxAttenCfg_t* txAttenCfg)
{
    static const uint8_t MAX_ATTEN_STEP = 0x03;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Check rxGainPinCtrlCfg pointer is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenCfg);

    /*Check that the requested rxChannel is valid*/
    IF_INVALID_TX_CHANNEL_RETURN(txAttenCfg->txChannelMask);

    /*Check that gain increment step is between 0-7*/
    if (txAttenCfg->txAttenStepSize > MAX_ATTEN_STEP)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenCfg->txAttenStepSize,
                         "Attenuation step exceeds limit for GPIO gain control config. Valid range 0-3");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_TxTestToneCfgSetRangeCheck(adi_adrv9025_Device_t*        device,
                                            adi_adrv9025_TxTestToneCfg_t* txToneCfg)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Check rxGainPinCtrlCfg pointer is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        txToneCfg);

    /*Check that the requested rxChannel is valid*/
    IF_INVALID_TX_CHANNEL_RETURN(txToneCfg->txChannelMask);

    /*Check that gain increment step is between 0-7*/
    /* TODO: Is this range check already covered in the bitfield write function? */
    if (txToneCfg->txToneGain > 3)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txToneCfg->txToneGain,
                         "Tone gain is invalid, valid range 0-3");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Tone Frequency is checked in top level function because it needs to be checked per channel, since
     * each channel can have a different Tx Sample rate */

    return device->common.error.newAction;
}

int32_t adrv9025_TxFirWrite(adi_adrv9025_Device_t* device,
                            uint32_t               txChanMask,
                            int8_t                 gain_dB,
                            uint8_t                numFirCoefs,
                            int16_t                coefs[],
                            uint8_t                arraySize)
{
    uint8_t              i             = 0;
    uint8_t              j             = 0;
    uint8_t              numTapsReg    = 0;
    uint8_t              nTapMul       = 0;
    uint8_t              maxNumTaps    = 0;
    uint8_t              filterGain    = 0;
    uint8_t              pfirCoeffCtl  = 0;
    uint16_t             baseaddr      = 0;
    uint8_t              filterConfig  = 0;
    static const uint8_t autoInc       = ADI_ADRV9025_PFIR_COEFF_AUTO_INCR;
    static const uint8_t TXFIR_MAXTAPS = 80;
    static const uint8_t TXFIR_MULTAPS = 20;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        coefs);

    ADRV9025_BUGINFO(__FUNCTION__);

    if (adrv9025_TxAddrDecode(device,
                              txChanMask) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChanMask,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Error checking, #taps less than max number of taps allowed.*/
    if (numFirCoefs == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numFirCoefs,
                         "NumFirCoef is zero");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (arraySize == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "arraySize is zero");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    maxNumTaps = TXFIR_MAXTAPS;
    nTapMul    = TXFIR_MULTAPS;

    /* Verification for the structure numFirCoefs */
    if (numFirCoefs > maxNumTaps)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numFirCoefs,
                         "NumFirCoef is too large");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (numFirCoefs > arraySize)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numFirCoefs,
                         "NumFirCoef is larger than arraySize");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Calculate register value for number of Taps */
    if (((numFirCoefs % nTapMul) == 0) &&
        (numFirCoefs > 0))
    {
        numTapsReg = (numFirCoefs / nTapMul) - 1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numFirCoefs,
                         "Invalid numFirCoefs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    pfirCoeffCtl |= ADRV9025_PFIR_COEFF_PROG_CLK_EN | ADRV9025_PFIR_COEFF_WR_ENABLE;
    if (autoInc != 0)
    {
        pfirCoeffCtl |= ADRV9025_PFIR_COEFF_AUTO_INCR;
    }

    baseaddr = adrv9025_TxAddrDecode(device,
                                     txChanMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    ADRV9025_SPIWRITEBYTE("TX_PFIR_COEFF_CTL",
                          (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_CTL_OFFSET),
                          pfirCoeffCtl);

    j = 0;
    ADRV9025_SPIWRITEBYTE("TX_PFIR_COEFF_ADDR",
                          (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_ADDR_OFFSET),
                          j);

    for (i = 0; i < numFirCoefs; i++)
    {
        //pfir_coeff_data LSB
        ADRV9025_SPIWRITEBYTE("TX_PFIR_COEFF_DATA_LSB",
                              (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_DATA_OFFSET),
                              (uint8_t)(coefs[i]));
        if (autoInc == 0)
        {
            j++;
            ADRV9025_SPIWRITEBYTE("TX_PFIR_COEFF_ADDR",
                                  (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_ADDR_OFFSET),
                                  j);
        }
        //pfir_coeff_data MSB
        ADRV9025_SPIWRITEBYTE("TX_PFIR_COEFF_DATA_MSB",
                              (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_DATA_OFFSET),
                              (uint8_t)(coefs[i] >> 8));
        if (autoInc == 0)
        {
            j++;
            ADRV9025_SPIWRITEBYTE("TX_PFIR_COEFF_ADDR",
                                  (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_ADDR_OFFSET),
                                  j);
        }
    }

    /* disable pfir_coeff_prog_clk_en */
    /* disable pfir_coeff_wr_enable */
    pfirCoeffCtl = 0;

    ADRV9025_SPIWRITEBYTE("TX_PFIR_COEFF_CTL",
                          (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_CTL_OFFSET),
                          pfirCoeffCtl);

    /* Set filter gain to be used */
    switch (gain_dB)
    {
    case ADRV9025_FIR_GAIN_NEG12_DB:
        filterGain = 0x00;
        break;
    case ADRV9025_FIR_GAIN_NEG6_DB:
        filterGain = 0x01;
        break;
    case ADRV9025_FIR_GAIN_0_DB:
        filterGain = 0x02;
        break;
    case ADRV9025_FIR_GAIN_POS6_DB:
        filterGain = 0x03;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         gain_dB,
                         "Invalid FirGain");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    filterConfig = 0;
    /* write filter gain and #taps if TX, else is RX */
    /* tx_pfir_Taps */
    filterConfig |= ADRV9025_BF_ENCODE(numTapsReg,
                                       ADRV9025_TX_PFIR_TAPS_MASK,
                                       ADRV9025_TX_PFIR_TAPS_SHIFT);

    /* tx_pfir_gain */
    filterConfig |= ADRV9025_BF_ENCODE(filterGain,
                                       ADRV9025_TX_PFIR_GAIN_MASK,
                                       ADRV9025_TX_PFIR_GAIN_SHIFT);

    ADRV9025_SPIWRITEBYTE("TX_FILTER_CONFIG",
                          (baseaddr + ADRV9025_ADDR_TX_FILTER_CONFIG_OFFSET),
                          filterConfig);

    return (device->common.error.newAction);
}

uint32_t adrv9025_txAttenAddrLookup(adi_adrv9025_Device_t* device,
                                    uint32_t               channelMask)
{
    if (((device->devStateInfo.initializedChannels >> ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET) & channelMask) != channelMask)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         ChannelMask,
                         "Channel not in device initializedChannels for adrv9025_txAttenAddrLookup() function");
        return 0;
    }

    switch (channelMask)
    {
    case ADI_ADRV9025_TX1:
        return ADRV9025_ADDR_TX1_ATTEN_TABLE;
    case ADI_ADRV9025_TX2:
        return ADRV9025_ADDR_TX2_ATTEN_TABLE;
    case ADI_ADRV9025_TX3:
        return ADRV9025_ADDR_TX3_ATTEN_TABLE;
    case ADI_ADRV9025_TX4:
        return ADRV9025_ADDR_TX4_ATTEN_TABLE;
    default:
        return 0;
    }
}

int32_t adrv9025_TxAttenTableWriteRangeCheck(adi_adrv9025_Device_t*         device,
                                             uint32_t                       txChannelMask,
                                             uint32_t                       txAttenIndexOffset,
                                             adi_adrv9025_TxAttenTableRow_t txAttenTableRows[],
                                             uint32_t                       numTxAttenEntries)
{
    static const uint8_t  TX_ATTEN_HP_MIN   = 0;
    static const uint8_t  TX_ATTEN_HP_MAX   = 63;
    static const uint16_t TX_ATTEN_MULT_MIN = 0;
    static const uint16_t TX_ATTEN_MULT_MAX = 4095;

    uint32_t j             = 0;
    uint32_t maxAttenIndex = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenTableRows);

    ADRV9025_BUGINFO(__FUNCTION__);

    maxAttenIndex = ADRV9025_TX_ATTEN_TABLE_MAX;

    if ((txChannelMask < 1) || (txChannelMask > 0xF))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid TxChannelMask");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((numTxAttenEntries < 1) || (numTxAttenEntries > maxAttenIndex))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid numTxAttenEntries");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (txAttenIndexOffset + numTxAttenEntries > maxAttenIndex)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid txAttenIndexOffset + numTxAttenEntries");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (j = 0; j < numTxAttenEntries; j++)
    {
        if ((txAttenTableRows[j].txAttenHp < TX_ATTEN_HP_MIN) ||
            (txAttenTableRows[j].txAttenHp > TX_ATTEN_HP_MAX))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             device,
                             "Invalid txAttenHp");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((txAttenTableRows[j].txAttenMult < TX_ATTEN_MULT_MIN) ||
            (txAttenTableRows[j].txAttenMult > TX_ATTEN_MULT_MAX))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             device,
                             "Invalid txAttenMult");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_TxPaProtectCfgRangeCheck(adi_adrv9025_Device_t*         device,
                                          adi_adrv9025_TxPaProtectCfg_t* txPaProtectCfg)
{
    /* Valid ranges for the pa protection configuration values */
    static const uint8_t  AVG_DURATION_MAX  = 0x0F;
    static const uint8_t  PEAK_DURATION_MAX = 0x0F;
    static const uint16_t POWER_THRESH_MAX  = 0x1FFF;
    static const uint16_t PEAK_THRESH_MAX   = 0x1FFF;
    static const uint8_t  PEAK_COUNT_MAX    = 0x1F;
    static const uint8_t  SINGLE_BIT_MAX    = 0x01;

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    if (txPaProtectCfg->avgDuration > AVG_DURATION_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txPaProtectCfg->avgDuration,
                         "Average duration exceeds maximum limit. Valid range 0-15");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (txPaProtectCfg->peakDuration > PEAK_DURATION_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txPaProtectCfg->peakDuration,
                         "Peak duration exceeds maximum limit. Valid range 0-15");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (txPaProtectCfg->powerThreshold > POWER_THRESH_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txPaProtectCfg->powerThreshold,
                         "Power Threshold exceeds maximum limit. Valid range 0-8191, [0:12] bits");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (txPaProtectCfg->peakThreshold > PEAK_THRESH_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txPaProtectCfg->peakThreshold,
                         "Peak threshold exceeds maximum limit. Valid range 0-8191 for Silicon B");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (txPaProtectCfg->peakCount > PEAK_COUNT_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txPaProtectCfg->peakCount,
                         "Peak count exceeds maximum limit. Valid range 0-31 / Silicon B");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (txPaProtectCfg->avgPowerEnable > SINGLE_BIT_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txPaProtectCfg->avgPowerEnable,
                         "Avg power enable value is invalid. Valid values 0-1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (txPaProtectCfg->peakPowerEnable > SINGLE_BIT_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txPaProtectCfg->peakPowerEnable,
                         "Peak power enable value is invalid. Valid values 0-1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (txPaProtectCfg->avgPeakRatioEnable > SINGLE_BIT_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txPaProtectCfg->avgPeakRatioEnable,
                         "Avg Peak power ratio enable value is invalid. Valid values 0-1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txPaProtectCfg->inputSel != ADI_ADRV9025_COMPLEX_MULT_OUTPUT) && (txPaProtectCfg->inputSel != ADI_ADRV9025_TXQEC_ACTUATOR_OUTPUT))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txPaProtectCfg->inputSel,
                         "Pa protection input selection is invalid. COMPLEX_MULT_OUTPUT and TXQEC_ACTUATOR_OUTPUT");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_TxLoSourceGetRangeCheck(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_TxChannels_e txChannel,
                                         adi_adrv9025_LoSel_e*     txLoSource)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        txLoSource);

    /*Check that the requested txChannel is valid*/
    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx channel selected for LO source mapping read back. Valid Tx channels are Tx1-Tx4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that tx profile is valid in current config*/
    if (((device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID) == 0)
        || ((device->devStateInfo.initializedChannels >> ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "LO source read back requested for Tx channel but Tx profile is invalid or channel not initialized in the device structure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_TxLoSourceGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_TxChannels_e txChannel,
                               adi_adrv9025_LoSel_e*     txLoSource)
{
    static const uint8_t TX_LO2_MASK = 0x01;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t rxtxLoMuxReg   = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        txLoSource);

    if ((txChannel == ADI_ADRV9025_TX1) ||
        (txChannel == ADI_ADRV9025_TX2))
    {
        /*Read the Rx Tx Lo Mux reg from the west side of the chip (Aux PLL regs)*/
        recoveryAction = adrv9025_PllMemMapTx12SelLo2BfGet(device,
                                                           ADRV9025_BF_AUX_PLL,
                                                           &rxtxLoMuxReg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if ((txChannel == ADI_ADRV9025_TX3) ||
        (txChannel == ADI_ADRV9025_TX4))
    {
        /*Read the Rx Tx Lo Mux reg from the west side of the chip (Aux PLL regs)*/
        recoveryAction = adrv9025_PllMemMapTx34SelLo2BfGet(device,
                                                           ADRV9025_BF_AUX_PLL,
                                                           &rxtxLoMuxReg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxtxLoMuxReg & TX_LO2_MASK) == 1)
    {
        *txLoSource = ADI_ADRV9025_LOSEL_LO2;
    }
    else
    {
        *txLoSource = ADI_ADRV9025_LOSEL_LO1;
    }

    return recoveryAction;
}

int32_t adrv9025_TxAttenPinCtrlCfgSet(adi_adrv9025_Device_t*       device,
                                      adrv9025_BfTxChanAddr_e      txChannelBaseAddr,
                                      adi_adrv9025_TxAttenPinCfg_t txAttenPinCfg,
                                      uint8_t                      releaseCurrentPins,
                                      uint8_t                      acquireNewPins)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID = ADRV9025_FEATURE_TX_ATTEN_CTRL_PIN;

    int32_t  recoveryAction                                                         = ADI_COMMON_ACT_NO_ACTION;
    uint32_t TX_GAIN_CTRL_INC_PIN_INDEX                                             = 0;
    uint32_t TX_GAIN_CTRL_DEC_PIN_INDEX                                             = 1;
    int32_t  sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_CTRL_PIN_FEATURE] = {0};
    uint8_t  resourceAcquisitionStatus                                              = ADI_FAILURE;
    uint8_t  resourceReleaseStatus                                                  = ADI_FAILURE;
    uint8_t  currentTxAttenCtrlIncPin                                               = 0;
    uint8_t  currentTxAttenCtrlDecPin                                               = 0;
    uint32_t incGpioPinMask                                                         = 0;
    uint32_t decGpioPinMask                                                         = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (releaseCurrentPins != ADI_FALSE)
    {
        /*Read Tx Attenuation Decrement GPIO select */
        recoveryAction = adrv9025_TxTpcDecrAttenGpioSelectBfGet(device,
                                                                txChannelBaseAddr,
                                                                &currentTxAttenCtrlDecPin);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while attempting to read Attenuation decrement GPIO select register");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Read Rx Attenuation Increment GPIO select*/
        recoveryAction = adrv9025_TxTpcIncrAttenGpioSelectBfGet(device,
                                                                txChannelBaseAddr,
                                                                &currentTxAttenCtrlIncPin);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while attempting to read Attenuation increment GPIO select register");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Attempt to Release Attenuation Ctrl GPIO Pins */
        sharedResourceArr[TX_GAIN_CTRL_INC_PIN_INDEX] = currentTxAttenCtrlIncPin;
        sharedResourceArr[TX_GAIN_CTRL_DEC_PIN_INDEX] = currentTxAttenCtrlDecPin;
        recoveryAction                                = adrv9025_SharedResourcesRelease(device,
                                                                                        ADRV9025_SHARED_RESOURCE_GPIO,
                                                                                        &sharedResourceArr[0],
                                                                                        ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_CTRL_PIN_FEATURE,
                                                                                        THIS_FEATURE_ID,
                                                                                        &resourceReleaseStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while attempting to release GPIO shared resouce for GPIO attenuation Increment/Decrement Pins");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error while attempting to release GPIO shared resouce for GPIO gain ctrl Increment/Decrement Pins");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Set released GPIO pins as inputs since unused pins are supposed to be inputs*/
        incGpioPinMask = ((uint32_t)0x00000001 << (uint8_t)currentTxAttenCtrlIncPin);
        decGpioPinMask = ((uint32_t)0x00000001 << (uint8_t)currentTxAttenCtrlDecPin);
        recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                      (incGpioPinMask | decGpioPinMask));
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         NULL,
                         "Error while attempting to set GPIO Increment/Decrement Pin inputs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (acquireNewPins != ADI_FALSE)
    {
#if ADI_ADRV9025_TX_RANGE_CHECK > 0
        recoveryAction = adrv9025_TxAttenPinCtrlCfgSetRangeCheck(device,
                                                                 &txAttenPinCfg);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         txAttenPinCfg,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
#endif

        /* Attempt to Acquire Gain Ctrl Increment GPIO pins */
        sharedResourceArr[TX_GAIN_CTRL_INC_PIN_INDEX] = (int32_t)txAttenPinCfg.txAttenIncPin;
        sharedResourceArr[TX_GAIN_CTRL_DEC_PIN_INDEX] = (int32_t)txAttenPinCfg.txAttenDecPin;
        recoveryAction                                = adrv9025_SharedResourcesAcquire(device,
                                                                                        ADRV9025_SHARED_RESOURCE_GPIO,
                                                                                        &sharedResourceArr[0],
                                                                                        ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_CTRL_PIN_FEATURE,
                                                                                        THIS_FEATURE_ID,
                                                                                        &resourceAcquisitionStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while attempting to acquire GPIO shared resouce for GPIO Attenaution Increment/Decrement Pins");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceAcquisitionStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceAcquisitionStatus,
                             "Error while attempting to acquire GPIO shared resouce for GPIO Attenuation ctrl Increment/Decrement Pins");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Set ADRV9025 GPIO Directions for Gain ctrl GPIO Increment and Decrement pins*/
        incGpioPinMask = ((uint32_t)0x00000001 << (uint8_t)txAttenPinCfg.txAttenIncPin);
        decGpioPinMask = ((uint32_t)0x00000001 << (uint8_t)txAttenPinCfg.txAttenDecPin);

        if ((recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                           (incGpioPinMask | decGpioPinMask))) != ADI_COMMON_ACT_NO_ACTION)
        {
            /*Clear the error so that clean up functions execute */
            recoveryAction = adrv9025_ErrorClear(&device->common);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while clearing error");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Release Shared GPIOs if setting of GPIO Output Dir is unsuccessful */
            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                                             &sharedResourceArr[0],
                                                             ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_CTRL_PIN_FEATURE,
                                                             THIS_FEATURE_ID,
                                                             &resourceReleaseStatus);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while releasing shared GPIO pins");
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (resourceReleaseStatus == ADI_FAILURE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_ADRV9025_ERR_SHARED_RESOURCE_RELEASE,
                                 ADI_ADRV9025_ACT_ERR_RESET_GPIO,
                                 resourceReleaseStatus,
                                 "Unable to release GPIOs for Tx attenuation pins");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_ADRV9025_ACT_ERR_RESET_SPI,
                             gpioAnalogInputOutputMask,
                             "Unable to set GPIOs input direction for Tx attenuation pins");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Configure GPIO Attenuation step size */
        recoveryAction = adrv9025_TxTxIncrDecrWordBfSet(device,
                                                        txChannelBaseAddr,
                                                        txAttenPinCfg.stepSize);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         txAttenPinCfg.stepSize,
                         "Error while attempting to set GPIO Attenaution step size");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Set Tx attenuation Ctrl Decrement GPIO select */
        recoveryAction = adrv9025_TxTpcDecrAttenGpioSelectBfSet(device,
                                                                txChannelBaseAddr,
                                                                txAttenPinCfg.txAttenDecPin);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         txAttenPinCfg,
                         "Error while attempting to set GPIO Tx attenuation decrement pin control");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Set Tx attenuation Ctrl Increment GPIO select*/
        recoveryAction = adrv9025_TxTpcIncrAttenGpioSelectBfSet(device,
                                                                txChannelBaseAddr,
                                                                txAttenPinCfg.txAttenIncPin);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         txAttenPinCfg,
                         "Error while attempting to set GPIO Tx attenuation increment pin control");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_TxAttenUpdateGpioPinCtrlCfgSetRangeCheck(adi_adrv9025_Device_t*    device,
                                                          adi_adrv9025_GpioPinSel_e gpioSelection)
{
    if ((gpioSelection != ADI_ADRV9025_GPIO_00) && (gpioSelection != ADI_ADRV9025_GPIO_01) &&
        (gpioSelection != ADI_ADRV9025_GPIO_02) && (gpioSelection != ADI_ADRV9025_GPIO_03) &&
        (gpioSelection != ADI_ADRV9025_GPIO_04) && (gpioSelection != ADI_ADRV9025_GPIO_05) &&
        (gpioSelection != ADI_ADRV9025_GPIO_06) && (gpioSelection != ADI_ADRV9025_GPIO_07) &&
        (gpioSelection != ADI_ADRV9025_GPIO_08) && (gpioSelection != ADI_ADRV9025_GPIO_09) &&
        (gpioSelection != ADI_ADRV9025_GPIO_10) && (gpioSelection != ADI_ADRV9025_GPIO_11) &&
        (gpioSelection != ADI_ADRV9025_GPIO_12) && (gpioSelection != ADI_ADRV9025_GPIO_13) &&
        (gpioSelection != ADI_ADRV9025_GPIO_14) && (gpioSelection != ADI_ADRV9025_GPIO_15))
    {
        /* Gpio selection is not valid */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         gpioSelection,
                         "Invalid GPIO selection, Valid range(GPIO_00-GPIO_15)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adrv9025_TxAttenUpdateGpioPinCtrlCfgSet(adi_adrv9025_Device_t*    device,
                                                adrv9025_BfTxChanAddr_e   txChannelBaseAddr,
                                                adi_adrv9025_GpioPinSel_e gpioSelection,
                                                uint8_t                   releaseCurrentPin,
                                                uint8_t                   acquireNewPin)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID = ADRV9025_FEATURE_TX_ATTEN_UPDATE_PIN;

    int32_t  recoveryAction                                                           = ADI_COMMON_ACT_NO_ACTION;
    int32_t  sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_UPDATE_PIN_FEATURE] = {0};
    uint8_t  resourceAcquisitionStatus                                                = ADI_FAILURE;
    uint8_t  resourceReleaseStatus                                                    = ADI_FAILURE;
    uint8_t  currentTxAttenUpdatePin                                                  = 0;
    uint32_t updateGpioPinMask                                                        = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (releaseCurrentPin != ADI_FALSE)
    {
        /*Read Tx Attenuation Update PIN GPIO select */
        recoveryAction = adrv9025_TxTxAttenUpdGpioSelectBfGet(device,
                                                              txChannelBaseAddr,
                                                              &currentTxAttenUpdatePin);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while attempting to read Tx attenuation update GPIO selection");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Attempt to Release Attenuation Update GPIO since it is a shared resource. */
        sharedResourceArr[0] = (int32_t)currentTxAttenUpdatePin;
        recoveryAction       = adrv9025_SharedResourcesRelease(device,
                                                               ADRV9025_SHARED_RESOURCE_GPIO,
                                                               &sharedResourceArr[0],
                                                               ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_UPDATE_PIN_FEATURE,
                                                               THIS_FEATURE_ID,
                                                               &resourceReleaseStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while attempting to release GPIO shared resource for Tx attenuation update Pin");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error while attempting to release GPIO shared resource for Tx attenuation update Pin");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Set released GPIO pins as inputs since unused pins are supposed to be inputs*/
        updateGpioPinMask = ((uint32_t)0x00000001 << currentTxAttenUpdatePin);
        recoveryAction    = adi_adrv9025_GpioInputDirSet(device,
                                                         updateGpioPinMask);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         NULL,
                         "Error while attempting to set Tx attenuation update GPIO pin as input");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (acquireNewPin != ADI_FALSE)
    {
        recoveryAction = adrv9025_TxAttenUpdateGpioPinCtrlCfgSetRangeCheck(device,
                                                                           gpioSelection);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         NULL,
                         "Invalid GPIO selection. Valid values GPIO00-GPIO15");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Attempt to Acquire Tx attenuation update GPIO since it is a shared resource. */
        sharedResourceArr[0] = (int32_t)gpioSelection;

        recoveryAction = adrv9025_SharedResourcesAcquire(device,
                                                         ADRV9025_SHARED_RESOURCE_GPIO,
                                                         &sharedResourceArr[0],
                                                         ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_UPDATE_PIN_FEATURE,
                                                         THIS_FEATURE_ID,
                                                         &resourceAcquisitionStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while attempting to acquire GPIO shared resouce for Tx attenuation update GPIO pin");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceAcquisitionStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceAcquisitionStatus,
                             "Error while attempting to acquire GPIO shared resouce for Tx attenuation update GPIO pin");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Set ADRV9025 GPIO Direction for attenuation update pin*/
        updateGpioPinMask = ((uint32_t)0x00000001 << (uint8_t)gpioSelection);

        /* Set GPIO analog pins as output */
        if ((recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                           updateGpioPinMask)) != ADI_COMMON_ACT_NO_ACTION)
        {
            /*Clear the error so that clean up functions execute */
            recoveryAction = adrv9025_ErrorClear(&device->common);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while clearing error ");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Release Shared GPIOs if setting of GPIO Output Dir is unsuccessful */
            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                                             &sharedResourceArr[0],
                                                             ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_UPDATE_PIN_FEATURE,
                                                             THIS_FEATURE_ID,
                                                             &resourceReleaseStatus);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while releasing shared GPIO pins ");
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (resourceReleaseStatus == ADI_FAILURE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_ADRV9025_ERR_SHARED_RESOURCE_RELEASE,
                                 ADI_ADRV9025_ACT_ERR_RESET_GPIO,
                                 resourceReleaseStatus,
                                 "Unable to release GPIO for Tx attenuation pin");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_ADRV9025_ACT_ERR_RESET_SPI,
                             gpioAnalogInputOutputMask,
                             "Unable to set GPIO output direction for Tx attenuation pin");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Set Tx attenuation Update GPIO select */
        recoveryAction = adrv9025_TxTxAttenUpdGpioSelectBfSet(device,
                                                              txChannelBaseAddr,
                                                              (uint8_t)gpioSelection);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         txAttenPinCfg,
                         "Error while attempting to set GPIO Tx attenuation update pin");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_TxRampDownInit(adi_adrv9025_Device_t*            device,
                                adi_adrv9025_TxRampDownInitCfg_t* txRampDownInitCfg)
{
    static const uint32_t   TX_RAMPDOWN_ALL_MASK = 0x00FF;
    int32_t                 recoveryAction       = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e baseAddr             = ADRV9025_BF_ALL_TX_CHANNELS;
    uint32_t                rampDownMask         = TX_RAMPDOWN_ALL_MASK;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txRampDownInitCfg);

    if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                   (uint32_t)txRampDownInitCfg->txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Mask All ramp down events by default */
    recoveryAction = adi_adrv9025_PaPllDfrmEventRampDownEnableSet(device,
                                                                  (uint32_t)txRampDownInitCfg->txChannel,
                                                                  rampDownMask,
                                                                  ADI_FALSE);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to set Tx ramp down masks during initialization");
    ADI_ERROR_RETURN(device->common.error.newAction);

    rampDownMask = 0;
    if (ADRV9025_BF_EQUAL(txRampDownInitCfg->txRampDownEvents,
                          (uint32_t)ADI_ADRV9025_TX_RAMP_DOWN_AUTOSELECT))
    {
        if (txRampDownInitCfg->txLoSel == ADI_ADRV9025_LOSEL_LO1)
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_RF1PLLUNLOCK;
        }
        else if (txRampDownInitCfg->txLoSel == ADI_ADRV9025_LOSEL_LO2)
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_RF2PLLUNLOCK;
        }

        if (txRampDownInitCfg->deserializerLanesEnabledDfrm0 > 0)
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_DFRMIRQ0;
        }

        if (txRampDownInitCfg->deserializerLanesEnabledDfrm1 > 0)
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_DFRMIRQ1;
        }

        /* Unmask Serdes(if it's being used) and Clk Pll unlock events in auto mode */
        if (txRampDownInitCfg->serdesPllVcoFreq_kHz != 0)
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_SERDESPLLUNLOCK;
        }
        rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_CLKPLLUNLOCK;
    }
    else
    {
        if (!(ADRV9025_BF_EQUAL(txRampDownInitCfg->txRampDownEvents,
                                (uint32_t)ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_DFRMIRQ0)))
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_DFRMIRQ0;
        }
        if (!(ADRV9025_BF_EQUAL(txRampDownInitCfg->txRampDownEvents,
                                (uint32_t)ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_DFRMIRQ1)))
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_DFRMIRQ1;
        }
        if (!(ADRV9025_BF_EQUAL(txRampDownInitCfg->txRampDownEvents,
                                (uint32_t)ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_RF1PLLUNLOCK)))
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_RF1PLLUNLOCK;
        }
        if (!(ADRV9025_BF_EQUAL(txRampDownInitCfg->txRampDownEvents,
                                (uint32_t)ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_RF2PLLUNLOCK)))
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_RF2PLLUNLOCK;
        }
        if (!(ADRV9025_BF_EQUAL(txRampDownInitCfg->txRampDownEvents,
                                (uint32_t)ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_SERDESPLLUNLOCK)))
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_SERDESPLLUNLOCK;
        }
        if (!(ADRV9025_BF_EQUAL(txRampDownInitCfg->txRampDownEvents,
                                (uint32_t)ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_CLKPLLUNLOCK)))
        {
            rampDownMask |= ADI_ADRV9025_RAMP_DOWN_ON_CLKPLLUNLOCK;
        }
    }

    if (rampDownMask != 0)
    {
        /* Unmask selected Tx power ramp down events */
        recoveryAction = adi_adrv9025_PaPllDfrmEventRampDownEnableSet(device,
                                                                      (uint32_t)txRampDownInitCfg->txChannel,
                                                                      rampDownMask,
                                                                      ADI_TRUE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to set Tx ramp down masks during initialization");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Enable Tx ramp down on Pll unlock */
    recoveryAction = adrv9025_TxTxDataRampDownOnPllUnlockBfSet(device,
                                                               baseAddr,
                                                               ADI_TRUE);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error while setting ramp down enable on Pll Unlock Event bitfield");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Enable Tx ramp down on Jesd Irq */
    recoveryAction = adrv9025_TxTxDataRampDownOnJesdDfrmIrqBfSet(device,
                                                                 baseAddr,
                                                                 ADI_TRUE);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Error while setting ramp down enable on Jesd Irq Event bitfield");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}
