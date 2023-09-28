// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_rx.c
* \brief Contains Rx features related function implementation defined in
* adi_adrv9025_rx.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_rx.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_gpio.h"
#include "adi_adrv9025_radioctrl.h"
#include "../../private/include/adrv9025_radioctrl.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_rx.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_bf_rx.h"
#include "../../private/include/adrv9025_bf_orx.h"
#include "../../private/include/adrv9025_bf_core.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_rx.c"
#endif

#define IF_INVALID_RX_CHANNEL_RETURN(rxChannel); \
{\
        if ((rxChannel != ADI_ADRV9025_RX1) && \
            (rxChannel != ADI_ADRV9025_RX2) && \
            (rxChannel != ADI_ADRV9025_RX3) && \
            (rxChannel != ADI_ADRV9025_RX4)) \
        { \
            ADI_ERROR_REPORT(&device->common,ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, rxChannel, \
                         "Invalid Rx Channel Parameter encountered"); \
            ADI_ERROR_RETURN(device->common.error.newAction); \
        } \
}

#define ADI_ADRV9025_NUM_GPIOS_IN_RX_GAIN_CTRL_PIN_FEATURE 2U
#define ADI_ADRV9025_NUM_GPIOS_IN_RX_AGC_HYBRID_FEATURE 1U
#define ADI_ADRV9025_NUM_GPIOS_IN_RX_EXT_CTRL_WORD_OUTPUT_FEATURE 8U
#define ADI_ADRV9025_NUM_GPIOS_IN_RX_DUALBAND_CTRL_WORD_OUTPUT_FEATURE 8U
#define ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX  8U

int32_t adi_adrv9025_RxGainPinCtrlCfgSetRangeCheck(adi_adrv9025_Device_t*       device,
                                                   adi_adrv9025_RxChannels_e    rxChannel,
                                                   adi_adrv9025_RxGainPinCfg_t* rxGainPinCtrlCfg)
{
    static const uint8_t MAX_GAIN_STEP = 7;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check rxGainPinCtrlCfg pointer is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxGainPinCtrlCfg);

    /*Check that the requested rxChannel is valid*/
    IF_INVALID_RX_CHANNEL_RETURN(rxChannel);

    /*Perform range check on config parameters only on enable*/
    if (rxGainPinCtrlCfg->enable > 0)
    {
        /*Check that gain increment step is between 0-7*/
        if (rxGainPinCtrlCfg->incStep > MAX_GAIN_STEP)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainPinCtrlCfg->incStep,
                             "Gain Increment Step exceeds limit for GPIO gain control config. Valid range 0-7");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Check that gain decrement step is between 0-7*/
        if (rxGainPinCtrlCfg->decStep > MAX_GAIN_STEP)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainPinCtrlCfg->decStep,
                             "Gain Decrement Step exceeds limit for GPIO gain control config. Valid range 0-7");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*GPIOs 0-15 are valid for GPIO gain control*/
        if (rxGainPinCtrlCfg->rxGainIncPin > ADI_ADRV9025_GPIO_15)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainPinCtrlCfg->decStep,
                             "Gain Increment pin for GPIO gain ctrl is out of range. Valid values GPIO_0 - GPIO_15");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*GPIOs 0-15 are valid for GPIO gain control*/
        if (rxGainPinCtrlCfg->rxGainDecPin > ADI_ADRV9025_GPIO_15)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainPinCtrlCfg->decStep,
                             "Gain Decrement pin for GPIO gain ctrl is out of range. Valid values GPIO_0 - GPIO_15");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Check that gain increment and decrement GPIO pins are not the same*/
        if (rxGainPinCtrlCfg->rxGainIncPin == rxGainPinCtrlCfg->rxGainDecPin)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGainPinCtrlCfg->decStep,
                             "Gain Decrement pin cannot be same as Gain Increment pin for GPIO gain ctrl");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainPinCtrlCfgSet(adi_adrv9025_Device_t*       device,
                                         adi_adrv9025_RxChannels_e    rxChannel,
                                         adi_adrv9025_RxGainPinCfg_t* rxGainPinCtrlCfg)
{
    static const uint8_t              MANUAL_GAIN_PIN_CTRL_ENABLE  = 1;
    static const uint8_t              MANUAL_GAIN_PIN_CTRL_DISABLE = 0;
    static const adrv9025_FeatureID_e THIS_FEATURE_ID              = ADRV9025_FEATURE_RX_GAIN_CTRL_PIN;
    static const uint32_t             RX_GAIN_CTRL_INC_PIN_INDEX   = 0;
    static const uint32_t             RX_GAIN_CTRL_DEC_PIN_INDEX   = 1;

    int32_t                 recoveryAction                                                        = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfRxChanAddr_e rxChannelBaseAddr                                                     = ADRV9025_BF_ALL_RX_CHANNELS;
    int32_t                 sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_IN_RX_GAIN_CTRL_PIN_FEATURE] = {0};
    uint8_t                 resourceAcquisitionStatus                                             = ADI_FAILURE;
    uint8_t                 resourceReleaseStatus                                                 = ADI_FAILURE;
    uint8_t                 currentRxGainCtrlIncPin                                               = 0;
    uint8_t                 currentRxGainCtrlDecPin                                               = 0;
    uint8_t                 rxGainCtrlPinModeEn                                                   = 0;
    uint32_t                incGpioPinMask                                                        = 0;
    uint32_t                decGpioPinMask                                                        = 0;
    uint32_t                i                                                                     = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxGainPinCtrlCfg);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adi_adrv9025_RxGainPinCtrlCfgSetRangeCheck(device,
                                               rxChannel,
                                               rxGainPinCtrlCfg);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Initialize the local shared resource array to invalid shared resource ID. This array is used to acquire multiple shared resources*/
    for (i = 0; i < ADI_ADRV9025_NUM_GPIOS_IN_RX_GAIN_CTRL_PIN_FEATURE; i++)
    {
        sharedResourceArr[i] = ADRV9025_SHARED_RESOURCE_INVALID;
    }

    /*Resolve Rx Channel SPI address*/
    adrv9025_RxBitfieldAddressGet(device,
                                  rxChannel,
                                  &rxChannelBaseAddr);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Read the current status of GPIO gain ctrl enable*/
    adrv9025_RxAgcManualGainPinControlBfGet(device,
                                            rxChannelBaseAddr,
                                            &rxGainCtrlPinModeEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error while attempting to read manual GPIO gain control config bitfield");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Disable Rx Manual Gain Ctrl GPIO feature by default and release currently used GPIOs if Rx Gain Pin ctrl feature is enabled*/
    if (rxGainCtrlPinModeEn == MANUAL_GAIN_PIN_CTRL_ENABLE)
    {
        adrv9025_RxAgcManualGainPinControlBfSet(device,
                                                rxChannelBaseAddr,
                                                MANUAL_GAIN_PIN_CTRL_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set manual GPIO gain control config bitfield");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Read Rx Gain Ctrl Decrement GPIO select */
        adrv9025_RxAgcDecGainGpioSelectBfGet(device,
                                             rxChannelBaseAddr,
                                             &currentRxGainCtrlDecPin);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to read manual gain control decrement GPIO select register");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Read Rx Gain Ctrl Increment GPIO select*/
        adrv9025_RxAgcIncGainGpioSelectBfGet(device,
                                             rxChannelBaseAddr,
                                             &currentRxGainCtrlIncPin);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to read manual gain control increment GPIO select register");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Attempt to Release Gain Ctrl Increment GPIO since it is a shared resource. This is to ensure that this feature is not accidentally releasing the GPIO
        which is actively in use by another feature */
        sharedResourceArr[RX_GAIN_CTRL_INC_PIN_INDEX] = currentRxGainCtrlIncPin;
        sharedResourceArr[RX_GAIN_CTRL_DEC_PIN_INDEX] = currentRxGainCtrlDecPin;
        adrv9025_SharedResourcesRelease(device,
                                        ADRV9025_SHARED_RESOURCE_GPIO,
                                        &sharedResourceArr[0],
                                        ADI_ADRV9025_NUM_GPIOS_IN_RX_GAIN_CTRL_PIN_FEATURE,
                                        THIS_FEATURE_ID,
                                        &resourceReleaseStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to release GPIO shared resouce for GPIO gain ctrl Increment/Decrement Pins");
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
        incGpioPinMask = ((uint32_t)0x00000001 << (uint8_t)currentRxGainCtrlIncPin);
        decGpioPinMask = ((uint32_t)0x00000001 << (uint8_t)currentRxGainCtrlDecPin);
        recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                      (incGpioPinMask | decGpioPinMask));
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         recoveryAction,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set GPIO Increment/Decrement Pin inputs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (rxGainPinCtrlCfg->enable > 0)
    {
        /* Attempt to Acquire Gain Ctrl Increment GPIO since it is a shared resource. This is to ensure that this feature is not accidentally acquiring the GPIO
        which is actively in use by another feature */
        sharedResourceArr[RX_GAIN_CTRL_INC_PIN_INDEX] = (uint32_t)rxGainPinCtrlCfg->rxGainIncPin;
        sharedResourceArr[RX_GAIN_CTRL_DEC_PIN_INDEX] = (uint32_t)rxGainPinCtrlCfg->rxGainDecPin;
        adrv9025_SharedResourcesAcquire(device,
                                        ADRV9025_SHARED_RESOURCE_GPIO,
                                        &sharedResourceArr[0],
                                        ADI_ADRV9025_NUM_GPIOS_IN_RX_GAIN_CTRL_PIN_FEATURE,
                                        THIS_FEATURE_ID,
                                        &resourceAcquisitionStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to acquire GPIO shared resouce for GPIO gain ctrl Increment/Decrement Pins");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceAcquisitionStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceAcquisitionStatus,
                             "Error while attempting to acquire GPIO shared resouce for GPIO gain ctrl Increment/Decrement Pins");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Configure GPIO gain increment step*/
        recoveryAction = adrv9025_RxAgcIncrGainStepSizeBfSet(device,
                                                             rxChannelBaseAddr,
                                                             rxGainPinCtrlCfg->incStep);
        adrv9025_RxGainCtrlPinReportError(device,
                                          &sharedResourceArr[0],
                                          recoveryAction,
                                          "Error while attempting to set manual GPIO gain control Increment Step Size");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Configure GPIO gain decrement step*/
        adrv9025_RxAgcDecrGainStepSizeBfSet(device,
                                            rxChannelBaseAddr,
                                            rxGainPinCtrlCfg->decStep);
        adrv9025_RxGainCtrlPinReportError(device,
                                          &sharedResourceArr[0],
                                          recoveryAction,
                                          "Error while attempting to set manual GPIO gain control Decrement Step Size");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Set ADRV9025 GPIO Directions for Gain ctrl GPIO Increment and Decrement pins*/
        incGpioPinMask = ((uint32_t)0x00000001 << (uint8_t)rxGainPinCtrlCfg->rxGainIncPin);
        decGpioPinMask = ((uint32_t)0x00000001 << (uint8_t)rxGainPinCtrlCfg->rxGainDecPin);
        recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                      (incGpioPinMask | decGpioPinMask));
        recoveryAction = adrv9025_RxGainCtrlPinReportError(device,
                                                           &sharedResourceArr[0],
                                                           recoveryAction,
                                                           "Error while attempting to set manual GPIO gain control pin");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Set Rx Gain Ctrl Decrement GPIO select */
        adrv9025_RxAgcDecGainGpioSelectBfSet(device,
                                             rxChannelBaseAddr,
                                             rxGainPinCtrlCfg->rxGainDecPin);
        adrv9025_RxGainCtrlPinReportError(device,
                                          &sharedResourceArr[0],
                                          recoveryAction,
                                          "Error while attempting to set manual gain control decrement GPIO select register");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Set Rx Gain Ctrl Increment GPIO select*/
        adrv9025_RxAgcIncGainGpioSelectBfSet(device,
                                             rxChannelBaseAddr,
                                             rxGainPinCtrlCfg->rxGainIncPin);
        adrv9025_RxGainCtrlPinReportError(device,
                                          &sharedResourceArr[0],
                                          recoveryAction,
                                          "Error while attempting to set manual gain control increment GPIO select register");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Enable Manual GPIO gain control*/
        adrv9025_RxAgcManualGainPinControlBfSet(device,
                                                rxChannelBaseAddr,
                                                MANUAL_GAIN_PIN_CTRL_ENABLE);
        adrv9025_RxGainCtrlPinReportError(device,
                                          &sharedResourceArr[0],
                                          recoveryAction,
                                          "Error while attempting to set manual GPIO gain control config bitfield");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainPinCtrlCfgGet(adi_adrv9025_Device_t*       device,
                                         adi_adrv9025_RxChannels_e    rxChannel,
                                         adi_adrv9025_RxGainPinCfg_t* rxGainPinCtrlCfg)
{
    adrv9025_BfRxChanAddr_e rxChannelBaseAddr = ADRV9025_BF_ALL_RX_CHANNELS;
    uint8_t                 rxGainDecPinBf    = 0;
    uint8_t                 rxGainIncPinBf    = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check rxGainPinCtrlCfg pointer is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxGainPinCtrlCfg);

    /*Check that the requested rxChannel is valid*/
    IF_INVALID_RX_CHANNEL_RETURN(rxChannel);

    /*Resolve Rx Channel SPI address*/
    adrv9025_RxBitfieldAddressGet(device,
                                  rxChannel,
                                  &rxChannelBaseAddr);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Read the Rx Manual Gain Ctrl GPIO Enable status*/
    adrv9025_RxAgcManualGainPinControlBfGet(device,
                                            rxChannelBaseAddr,
                                            &rxGainPinCtrlCfg->enable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error while attempting to read manual GPIO gain control config bitfield");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Read GPIO gain increment step*/
    adrv9025_RxAgcIncrGainStepSizeBfGet(device,
                                        rxChannelBaseAddr,
                                        &rxGainPinCtrlCfg->incStep);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error while attempting to read manual GPIO gain control Increment Step Size");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Read GPIO gain decrement step*/
    adrv9025_RxAgcDecrGainStepSizeBfGet(device,
                                        rxChannelBaseAddr,
                                        &rxGainPinCtrlCfg->decStep);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error while attempting to read manual GPIO gain control Decrement Step Size");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Read Rx Gain Ctrl Decrement GPIO select */
    adrv9025_RxAgcDecGainGpioSelectBfGet(device,
                                         rxChannelBaseAddr,
                                         &rxGainDecPinBf);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error while attempting to read manual gain control decrement GPIO select register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Read Rx Gain Ctrl Increment GPIO select*/
    adrv9025_RxAgcIncGainGpioSelectBfGet(device,
                                         rxChannelBaseAddr,
                                         &rxGainIncPinBf);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error while attempting to read manual gain control increment GPIO select register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Update rxGainIncPin and rxGainDecPin enums with read back value*/
    rxGainPinCtrlCfg->rxGainDecPin = (adi_adrv9025_GpioPinSel_e)rxGainDecPinBf;
    rxGainPinCtrlCfg->rxGainIncPin = (adi_adrv9025_GpioPinSel_e)rxGainIncPinBf;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainTableExtCtrlPinsSetRangeCheck(adi_adrv9025_Device_t*                 device,
                                                         adi_adrv9025_RxExtCtrlPinOuputEnable_e extCtrlGpioChannelEnable)
{
    static const uint8_t GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_DEFAULT = 0x00;
    static const uint8_t GPIO_ANA_UPPER_NIBBLE_SRC_CTRL_MASK    = 0x0F;
    static const uint8_t GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_MASK    = 0x0F;
    static const uint8_t GPIO_ANA_RX1_RX2_EXT_CTRL_GPIO_ENABLE  = 0x04;
    static const uint8_t GPIO_ANA_RX3_RX4_EXT_CTRL_GPIO_ENABLE  = 0x04;

    uint8_t gpioAnalogSrcCtrl = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((extCtrlGpioChannelEnable != ADI_ADRV9025_DISABLE_RX1_RX2_EXT_CTRL_GPIOS) &&
        (extCtrlGpioChannelEnable != ADI_ADRV9025_ENABLE_RX1_RX2_EXT_CTRL_GPIOS) &&
        (extCtrlGpioChannelEnable != ADI_ADRV9025_DISABLE_RX3_RX4_EXT_CTRL_GPIOS) &&
        (extCtrlGpioChannelEnable != ADI_ADRV9025_ENABLE_RX3_RX4_EXT_CTRL_GPIOS) &&
        (extCtrlGpioChannelEnable != ADI_ADRV9025_DISABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS) &&
        (extCtrlGpioChannelEnable != ADI_ADRV9025_ENABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extCtrlGpioChannelEnable,
                         "Invalid Rx Channel enable Requested for external ctrl word output Analog GPIOs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX1_RX2_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX1_RX2_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS))
    {
        adrv9025_CoreGpioAnalogLowerNibbleSourceControlBfGet(device,
                                                             ADRV9025_BF_CORE,
                                                             &gpioAnalogSrcCtrl);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Mask Upper Nibble Src Ctrl*/
        gpioAnalogSrcCtrl &= GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_MASK;

        /*If Analog GPIO Source Ctrl is set to a feature other than Ext Attenuator Ctrl Output, throw an exception*/
        if ((gpioAnalogSrcCtrl != GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_DEFAULT) &&
            (gpioAnalogSrcCtrl != GPIO_ANA_RX1_RX2_EXT_CTRL_GPIO_ENABLE))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gpioAnalogSrcCtrl,
                             "Gain Table Ext Ctrl word output for channels Rx1, Rx2 selected but Analog GPIO Src Ctrl set to another feature");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if ((extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX3_RX4_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX3_RX4_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS))
    {
        adrv9025_CoreGpioAnalogUpperNibbleSourceControlBfGet(device,
                                                             ADRV9025_BF_CORE,
                                                             &gpioAnalogSrcCtrl);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Mask Lower Nibble Src Ctrl*/
        gpioAnalogSrcCtrl &= GPIO_ANA_UPPER_NIBBLE_SRC_CTRL_MASK;

        /*If Analog GPIO Source Ctrl is set to a feature other than Ext Attenuator Ctrl Output, throw an exception*/
        if ((gpioAnalogSrcCtrl != GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_DEFAULT) &&
            (gpioAnalogSrcCtrl != GPIO_ANA_RX3_RX4_EXT_CTRL_GPIO_ENABLE))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gpioAnalogSrcCtrl,
                             "Gain Table Ext Ctrl word output for channels Rx3, Rx4 selected but Analog GPIO Src Ctrl set to another feature");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainTableExtCtrlPinsSet(adi_adrv9025_Device_t*                 device,
                                               adi_adrv9025_RxExtCtrlPinOuputEnable_e extCtrlGpioChannelEnable)
{
    static const uint8_t              GPIO_ANA_UPPER_NIBBLE_SRC_CTRL_DEFAULT = 0x00;
    static const uint8_t              GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_DEFAULT = 0x00;
    static const uint8_t              GPIO_ANA_RX1_RX2_EXT_CTRL_GPIO_ENABLE  = 0x04;
    static const uint8_t              GPIO_ANA_RX3_RX4_EXT_CTRL_GPIO_ENABLE  = 0x04;
    static const adrv9025_FeatureID_e THIS_FEATURE_ID                        = ADRV9025_FEATURE_RX_EXT_CTRL_WORD_OUTPUT;

    int32_t recoveryAction                                                               = ADI_COMMON_ACT_NO_ACTION;
    uint8_t gpioAnalogInputOutputMask                                                    = 0;
    int32_t sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_IN_RX_EXT_CTRL_WORD_OUTPUT_FEATURE] = {0};
    uint8_t resourceAcquisitionStatus                                                    = ADI_FAILURE;
    uint8_t resourceReleaseStatus                                                        = ADI_FAILURE;
    uint8_t numSharedResources                                                           = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adi_adrv9025_RxGainTableExtCtrlPinsSetRangeCheck(device,
                                                     extCtrlGpioChannelEnable);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Resolve Shared Analog GPIO Resources depending on channels for which ext ctrl output enable/disable is requested*/
    if ((extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS))
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_ANA_00;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_ANA_01;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_ANA_02;
        sharedResourceArr[3] = ADI_ADRV9025_GPIO_ANA_03;
        sharedResourceArr[4] = ADI_ADRV9025_GPIO_ANA_04;
        sharedResourceArr[5] = ADI_ADRV9025_GPIO_ANA_05;
        sharedResourceArr[6] = ADI_ADRV9025_GPIO_ANA_06;
        sharedResourceArr[7] = ADI_ADRV9025_GPIO_ANA_07;
        numSharedResources   = 8;
    }
    else if ((extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX1_RX2_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX1_RX2_EXT_CTRL_GPIOS))
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_ANA_00;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_ANA_01;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_ANA_02;
        sharedResourceArr[3] = ADI_ADRV9025_GPIO_ANA_03;
        numSharedResources   = 4;
    }
    else if ((extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX3_RX4_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX3_RX4_EXT_CTRL_GPIOS))
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_ANA_04;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_ANA_05;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_ANA_06;
        sharedResourceArr[3] = ADI_ADRV9025_GPIO_ANA_07;
        numSharedResources   = 4;
    }

    if ((extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX1_RX2_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS))
    {
        /*Release Shared Analog GPIOs*/
        adrv9025_SharedResourcesRelease(device,
                                        ADRV9025_SHARED_RESOURCE_GPIO_ANALOG,
                                        &sharedResourceArr[0],
                                        numSharedResources,
                                        THIS_FEATURE_ID,
                                        &resourceReleaseStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to release shared Analog GPIO during disabling of Rx1, Rx2 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error while attempting to release shared Analog GPIO during disabling of Rx1, Rx2 ext ctrl output");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Set GPIO_ANA[3:0] to input on disable request*/
        gpioAnalogInputOutputMask = (((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_00) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_01) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_02) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_03));
        adi_adrv9025_GpioAnalogInputDirSet(device,
                                           gpioAnalogInputOutputMask);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set Analog GPIO lower nibble direction ctrl to default during disabling of Rx1, Rx2 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Set Analog GPIO Src Ctrl Lower Nibble to default*/
        adrv9025_CoreGpioAnalogLowerNibbleSourceControlBfSet(device,
                                                             ADRV9025_BF_CORE,
                                                             GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_DEFAULT);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set Analog GPIO lower nibble source ctrl to default during disabling of Rx1, Rx2 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX3_RX4_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_DISABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS))
    {
        /*Release Shared Analog GPIOs*/
        adrv9025_SharedResourcesRelease(device,
                                        ADRV9025_SHARED_RESOURCE_GPIO_ANALOG,
                                        &sharedResourceArr[0],
                                        numSharedResources,
                                        THIS_FEATURE_ID,
                                        &resourceReleaseStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to release shared Analog GPIO during disabling of Rx3, Rx4 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error while attempting to release shared Analog GPIO during disabling of Rx3, Rx4 ext ctrl output");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Set GPIO_ANA[7:4] to input on disabling*/
        gpioAnalogInputOutputMask = (((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_04) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_05) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_06) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_07));
        adi_adrv9025_GpioAnalogInputDirSet(device,
                                           gpioAnalogInputOutputMask);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set Analog GPIO lower nibble direction ctrl to default during disabling of Rx3, Rx4 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Set Analog GPIO Src Ctrl Lower Nibble to default*/
        adrv9025_CoreGpioAnalogUpperNibbleSourceControlBfSet(device,
                                                             ADRV9025_BF_CORE,
                                                             GPIO_ANA_UPPER_NIBBLE_SRC_CTRL_DEFAULT);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set Analog GPIO lower nibble source ctrl to default during disabling of Rx3, Rx4 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX1_RX2_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS))
    {
        /*Acquire Shared Analog GPIOs*/
        adrv9025_SharedResourcesAcquire(device,
                                        ADRV9025_SHARED_RESOURCE_GPIO_ANALOG,
                                        &sharedResourceArr[0],
                                        numSharedResources,
                                        THIS_FEATURE_ID,
                                        &resourceAcquisitionStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to acquire shared Analog GPIO during enabling of Rx1, Rx2 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceAcquisitionStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceAcquisitionStatus,
                             "Error while attempting to acquire shared Analog GPIO during enabling of Rx1, Rx2 ext ctrl output");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Set GPIO_ANA[3:0] to output on enable request*/
        gpioAnalogInputOutputMask = (((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_00) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_01) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_02) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_03));
        adi_adrv9025_GpioAnalogOutputDirSet(device,
                                            gpioAnalogInputOutputMask);
        adrv9025_RxExtCtrlPinReportError(device,
                                         &sharedResourceArr[0],
                                         numSharedResources,
                                         recoveryAction,
                                         "Error while attempting to set Analog GPIO lower nibble direction ctrl to output during enabling of Rx1, Rx2 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Set Analog GPIO Src Ctrl Lower Nibble to Ext Attenuator Ctrl Output*/
        adrv9025_CoreGpioAnalogLowerNibbleSourceControlBfSet(device,
                                                             ADRV9025_BF_CORE,
                                                             GPIO_ANA_RX1_RX2_EXT_CTRL_GPIO_ENABLE);
        adrv9025_RxExtCtrlPinReportError(device,
                                         &sharedResourceArr[0],
                                         numSharedResources,
                                         recoveryAction,
                                         "Error while attempting to set Analog GPIO lower nibble source ctrl to ext attenutaor ctrl during enabling of Rx1, Rx2 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX3_RX4_EXT_CTRL_GPIOS) ||
        (extCtrlGpioChannelEnable == ADI_ADRV9025_ENABLE_RX1_RX2_RX3_RX4_EXT_CTRL_GPIOS))
    {
        /*Acquire Shared Analog GPIOs*/
        adrv9025_SharedResourcesAcquire(device,
                                        ADRV9025_SHARED_RESOURCE_GPIO_ANALOG,
                                        &sharedResourceArr[0],
                                        numSharedResources,
                                        THIS_FEATURE_ID,
                                        &resourceAcquisitionStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to acquire shared Analog GPIO during enabling of Rx3, Rx4 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceAcquisitionStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceAcquisitionStatus,
                             "Error while attempting to acquire shared Analog GPIO during enabling of Rx3, Rx4 ext ctrl output. Please check if the GPIOs in use by another feature");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Set GPIO_ANA[7:4] to output on enabling*/
        gpioAnalogInputOutputMask = (((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_04) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_05) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_06) |
            ((uint8_t)1 << (uint8_t)ADI_ADRV9025_GPIO_ANA_07));
        adi_adrv9025_GpioAnalogOutputDirSet(device,
                                            gpioAnalogInputOutputMask);
        adrv9025_RxExtCtrlPinReportError(device,
                                         &sharedResourceArr[0],
                                         numSharedResources,
                                         recoveryAction,
                                         "Error while attempting to set Analog GPIO lower nibble direction ctrl to ext attenuator ctrl during enabling of Rx3, Rx4 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Set Analog GPIO Src Ctrl Lower Nibble to default*/
        adrv9025_CoreGpioAnalogUpperNibbleSourceControlBfSet(device,
                                                             ADRV9025_BF_CORE,
                                                             GPIO_ANA_RX3_RX4_EXT_CTRL_GPIO_ENABLE);
        adrv9025_RxExtCtrlPinReportError(device,
                                         &sharedResourceArr[0],
                                         numSharedResources,
                                         recoveryAction,
                                         "Error while attempting to set Analog GPIO lower nibble source ctrl to ext attenuator ctrl during enabling of Rx3, Rx4 ext ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainTableWriteRangeCheck(adi_adrv9025_Device_t*        device,
                                                uint32_t                      rxChannelMask,
                                                uint8_t                       gainIndexOffset,
                                                adi_adrv9025_RxGainTableRow_t gainTableRow[],
                                                uint16_t                      arraySize)
{
    static const uint8_t MAX_EXT_CTRL_WORD = 3;
    static const int16_t MIN_DIG_GAIN      = -360; /*Dig gain is in the range -18dB to 50db*/
    static const int16_t MAX_DIG_GAIN      = 1000; /*Dig gain is in the range -18dB to 50db*/

    uint16_t gainIndex = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check gainTableRow is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        gainTableRow);

    /*Check the no. of gain indices parameter is not 0*/
    if (arraySize == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "arraySize parameter is 0. Valid values from 1 - 256");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that the gain index offset is within range*/
    if ((gainIndexOffset < ADI_ADRV9025_MIN_RX_GAIN_TABLE_INDEX) ||
        (gainIndexOffset > ADI_ADRV9025_START_RX_GAIN_INDEX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         gainIndexOffset,
                         "Invalid gainIndexOffset. Parameter gainIndexOffset should be in the range ADI_ADRV9025_MIN_RX_GAIN_TABLE_INDEX to ADI_ADRV9025_START_RX_GAIN_INDEX");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check the no. of gain indices parameter is valid*/
    if (arraySize > ((gainIndexOffset - ADI_ADRV9025_MIN_RX_GAIN_TABLE_INDEX) + 1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "gainTableRow arraySize exceeds the limit. Valid range ADI_ADRV9025_MIN_RX_GAIN_TABLE_INDEX to gainIndexOffset");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that rxChannelMask is valid*/
    if (rxChannelMask == ADI_ADRV9025_RXOFF)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Invalid Rx Channel mask parameter. Valid Rx channel masks include Rx1-Rx4 and ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (rxChannelMask > (ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4 |
        ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Invalid Rx Channel mask parameter. Valid Rx channel masks include Rx1-Rx4 and ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that if Rx gain table is required to be loaded, Rx Profile is valid*/
    if (((rxChannelMask & (uint32_t)ADI_ADRV9025_RX1) == (uint32_t)ADI_ADRV9025_RX1) ||
        ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX2) == (uint32_t)ADI_ADRV9025_RX2) ||
        ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX3) == (uint32_t)ADI_ADRV9025_RX3) ||
        ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX4) == (uint32_t)ADI_ADRV9025_RX4))
    {
        if ((device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannelMask,
                             "Rx gain table selected for programming but Rx profile is invalid in device structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /*Check that if ORx gain table is required to be loaded, ORx Profile is valid*/
    if (((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX1) == (uint32_t)ADI_ADRV9025_ORX1) ||
        ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX2) == (uint32_t)ADI_ADRV9025_ORX2) ||
        ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX3) == (uint32_t)ADI_ADRV9025_ORX3) ||
        ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX4) == (uint32_t)ADI_ADRV9025_ORX4))
    {
        if ((device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannelMask,
                             "ORx gain table selected for programming but ORx profile is invalid in device structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /*Range check individual gain table row entries*/
    for (gainIndex = 0; gainIndex < arraySize; gainIndex++)
    {
        /*Check that {TIA GAIN, ADC CTRL} are 0*/
        if (gainTableRow[gainIndex].adcTiaGain != 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainTableRow[gainIndex].adcTiaGain,
                             "Rx Gain Table adcTiaGain parameter exceeds the limit. The only allowed value is 0");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Check that EXT_CTRL is in the range {0,1,2,3}*/
        if (gainTableRow[gainIndex].extControl > MAX_EXT_CTRL_WORD)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainTableRow[gainIndex].extControl,
                             "Rx Gain Table extControl parameter exceeds the limit. Allowed values are 0,1,2,3");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Check that digital gain is in the range -18dB to 50dB*/
        if ((gainTableRow[gainIndex].digGain < MIN_DIG_GAIN) ||
            (gainTableRow[gainIndex].digGain > MAX_DIG_GAIN))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainTableRow[gainIndex].digGain,
                             "Rx Gain Table digGain parameter exceeds the limit. Allowed values between -18dB to 50dB(-360 to +1000)");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainTableWrite(adi_adrv9025_Device_t*        device,
                                      uint32_t                      rxChannelMask,
                                      uint8_t                       gainIndexOffset,
                                      adi_adrv9025_RxGainTableRow_t gainTableRow[],
                                      uint16_t                      arraySize)
{
    uint32_t baseIndex             = 0;
    uint32_t baseAddress           = 0;
    uint16_t numGainIndicesToWrite = arraySize;
    /*Maximum Array Size = Max Gain Table Size x Bytes Per Gain Table Entry*/
    static uint8_t armDmaData[((ADI_ADRV9025_MAX_GAIN_TABLE_INDEX - ADI_ADRV9025_MIN_GAIN_TABLE_INDEX) + 1) * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX] =
    {0};
    static const uint8_t DEC_POWER_CONFIG1 =
    0x09; /* dec_power_log_shift | dec_power_enable_meas */
    static const uint8_t DIGITAL_GAIN_CONFIG2 =
    0x01; /* digital_gain_enable */

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check gainTableRow is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        gainTableRow);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adi_adrv9025_RxGainTableWriteRangeCheck(device,
                                            rxChannelMask,
                                            gainIndexOffset,
                                            gainTableRow,
                                            numGainIndicesToWrite);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Calculate base index for the config*/
    baseIndex = (gainIndexOffset - (numGainIndicesToWrite - 1));

    /*Format Gain Table Entries*/
    adrv9025_RxGainTableFormat(device,
                               gainTableRow,
                               &armDmaData[0],
                               numGainIndicesToWrite);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Resolve the RX Channel SRAM to program*/
    /*If Rx1 Channel Mask Set by user for this config, load Rx1 gain table*/
    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX1) == (uint32_t)ADI_ADRV9025_RX1)
    {
        /*Resolve Rx1 Gain Table SRAM load start address*/
        baseAddress = (uint32_t)ADI_ADRV9025_RX1_GAIN_TABLE_BASEADDR + (baseIndex * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX);
        /*Write to the SRAM via ARM DMA*/
        adrv9025_CpuDmaMemWrite(device,
                                baseAddress,
                                &armDmaData[0],
                                (uint32_t)(numGainIndicesToWrite * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX),
                                ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error writing to ARM DMA while loading Rx Gain Table @ base address 0x80800000");
        ADI_ERROR_RETURN(device->common.error.newAction);

        ADRV9025_SPIWRITEBYTE("RX1_DEC_POWR_CONFIG_1",
                              (ADRV9025_ADDR_CH0_RX + ADRV9025_ADDR_RX_DEC_POWER_CONFIG1_OFFSET),
                              DEC_POWER_CONFIG1);

        ADRV9025_SPIWRITEBYTE("RX1_DIGITAL_GAIN_CONFIG2",
                              (ADRV9025_ADDR_CH0_RX + ADRV9025_ADDR_RX_DIGITAL_GAIN_CONFIG2_OFFSET),
                              DIGITAL_GAIN_CONFIG2);
    }

    /*If Rx2 Channel Mask Set by user for this config, load Rx2 gain table*/
    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX2) == (uint32_t)ADI_ADRV9025_RX2)
    {
        /*Resolve Rx2 Gain Table SRAM load start address*/
        baseAddress = (uint32_t)ADI_ADRV9025_RX2_GAIN_TABLE_BASEADDR + (baseIndex * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX);
        /*Write to the SRAM via ARM DMA*/
        adrv9025_CpuDmaMemWrite(device,
                                baseAddress,
                                &armDmaData[0],
                                (uint32_t)(numGainIndicesToWrite * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX),
                                ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error writing to ARM DMA while loading Rx Gain Table @ base address 0x88800000");
        ADI_ERROR_RETURN(device->common.error.newAction);

        ADRV9025_SPIWRITEBYTE("RX2_DEC_POWR_CONFIG_1",
                              (ADRV9025_ADDR_CH1_RX + ADRV9025_ADDR_RX_DEC_POWER_CONFIG1_OFFSET),
                              DEC_POWER_CONFIG1);

        ADRV9025_SPIWRITEBYTE("RX2_DIGITAL_GAIN_CONFIG2",
                              (ADRV9025_ADDR_CH1_RX + ADRV9025_ADDR_RX_DIGITAL_GAIN_CONFIG2_OFFSET),
                              DIGITAL_GAIN_CONFIG2);
    }

    /*If Rx3 Channel Mask Set by user for this config, load Rx3 gain table*/
    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX3) == (uint32_t)ADI_ADRV9025_RX3)
    {
        /*Resolve Rx3 Gain Table SRAM load start address*/
        baseAddress = (uint32_t)ADI_ADRV9025_RX3_GAIN_TABLE_BASEADDR + (baseIndex * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX);
        /*Write to the SRAM via ARM DMA*/
        adrv9025_CpuDmaMemWrite(device,
                                baseAddress,
                                &armDmaData[0],
                                (uint32_t)(numGainIndicesToWrite * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX),
                                ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error writing to ARM DMA while loading Rx Gain Table @ base address 0x90800000");
        ADI_ERROR_RETURN(device->common.error.newAction);

        ADRV9025_SPIWRITEBYTE("RX3_DEC_POWR_CONFIG_1",
                              (ADRV9025_ADDR_CH2_RX + ADRV9025_ADDR_RX_DEC_POWER_CONFIG1_OFFSET),
                              DEC_POWER_CONFIG1);

        ADRV9025_SPIWRITEBYTE("RX3_DIGITAL_GAIN_CONFIG2",
                              (ADRV9025_ADDR_CH2_RX + ADRV9025_ADDR_RX_DIGITAL_GAIN_CONFIG2_OFFSET),
                              DIGITAL_GAIN_CONFIG2);
    }

    /*If Rx4 Channel Mask Set by user for this config, load Rx4 gain table*/
    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX4) == (uint32_t)ADI_ADRV9025_RX4)
    {
        /*Resolve Rx4 Gain Table SRAM load start address*/
        baseAddress = (uint32_t)ADI_ADRV9025_RX4_GAIN_TABLE_BASEADDR + (baseIndex * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX);
        /*Write to the SRAM via ARM DMA*/
        adrv9025_CpuDmaMemWrite(device,
                                baseAddress,
                                &armDmaData[0],
                                (uint32_t)(numGainIndicesToWrite * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX),
                                ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error writing to ARM DMA while loading Rx Gain Table @ base address 0x98800000");
        ADI_ERROR_RETURN(device->common.error.newAction);

        ADRV9025_SPIWRITEBYTE("RX4_DEC_POWR_CONFIG_1",
                              (ADRV9025_ADDR_CH3_RX + ADRV9025_ADDR_RX_DEC_POWER_CONFIG1_OFFSET),
                              DEC_POWER_CONFIG1);

        ADRV9025_SPIWRITEBYTE("RX3_DIGITAL_GAIN_CONFIG2",
                              (ADRV9025_ADDR_CH3_RX + ADRV9025_ADDR_RX_DIGITAL_GAIN_CONFIG2_OFFSET),
                              DIGITAL_GAIN_CONFIG2);
    }

    /*If ORx1/ORx2 Channel Mask Set by user for this config, load ORx1/ORx2 gain table*/
    if (((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX1) == (uint32_t)ADI_ADRV9025_ORX1) ||
        ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX2) == (uint32_t)ADI_ADRV9025_ORX2))
    {
        /*Resolve ORx1 Gain Table SRAM load start address*/
        baseAddress = (uint32_t)ADI_ADRV9025_ORX1_ORX2_GAIN_TABLE_BASEADDR + (baseIndex * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX);
        /*Write to the SRAM via ARM DMA*/
        adrv9025_CpuDmaMemWrite(device,
                                baseAddress,
                                &armDmaData[0],
                                (uint32_t)(numGainIndicesToWrite * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX),
                                ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error writing to ARM DMA while loading ORx Gain Table @ base address 0xC0800000");
        ADI_ERROR_RETURN(device->common.error.newAction);

        ADRV9025_SPIWRITEBYTE("ORX1_DEC_POWR_CONFIG_1",
                              (ADRV9025_ADDR_CH0_ORX + ADRV9025_ADDR_ORX_DEC_POWER_CONFIG1_OFFSET),
                              DEC_POWER_CONFIG1);

        ADRV9025_SPIWRITEBYTE("ORX1_DIGITAL_GAIN_CONFIG2",
                              (ADRV9025_ADDR_CH0_ORX + ADRV9025_ADDR_ORX_DIGITAL_GAIN_CONFIG2_OFFSET),
                              DIGITAL_GAIN_CONFIG2);
    }

    /*If ORx3/ORx4 Channel Mask Set by user for this config, load ORx3/ORx4 gain table*/
    if (((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX3) == (uint32_t)ADI_ADRV9025_ORX3) ||
        ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX4) == (uint32_t)ADI_ADRV9025_ORX4))
    {
        /*Resolve ORx2 Gain Table SRAM load start address*/
        baseAddress = (uint32_t)ADI_ADRV9025_ORX3_ORX4_GAIN_TABLE_BASEADDR + (baseIndex * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX);
        /*Write to the SRAM via ARM DMA*/
        adrv9025_CpuDmaMemWrite(device,
                                baseAddress,
                                &armDmaData[0],
                                (uint32_t)(numGainIndicesToWrite * ADI_ADRV9025_NUM_BYTES_PER_RX_GAIN_INDEX),
                                ADI_ADRV9025_CPU_MEM_AUTO_INCR);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error writing to ARM DMA while loading ORx Gain Table @ base address 0xE0800000");
        ADI_ERROR_RETURN(device->common.error.newAction);

        ADRV9025_SPIWRITEBYTE("ORX2_DEC_POWR_CONFIG_1",
                              (ADRV9025_ADDR_CH1_ORX + ADRV9025_ADDR_ORX_DEC_POWER_CONFIG1_OFFSET),
                              DEC_POWER_CONFIG1);

        ADRV9025_SPIWRITEBYTE("ORX2_DIGITAL_GAIN_CONFIG2",
                              (ADRV9025_ADDR_CH1_ORX + ADRV9025_ADDR_ORX_DIGITAL_GAIN_CONFIG2_OFFSET),
                              DIGITAL_GAIN_CONFIG2);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxMinMaxGainIndexSetRangeCheck(adi_adrv9025_Device_t* device,
                                                    uint32_t               rxChannelMask,
                                                    uint8_t                minGainIndex,
                                                    uint8_t                maxGainIndex)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check that rxChannelMask is valid*/
    if (rxChannelMask == ADI_ADRV9025_RXOFF)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Invalid Rx Channel mask parameter. Valid Rx channel masks include Rx1-Rx4 and ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (rxChannelMask > (ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4 |
        ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Invalid Rx Channel mask parameter. Valid Rx channel masks include Rx1-Rx4 and ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that requested min gain index does not exceed max gain index*/
    if (minGainIndex >= maxGainIndex)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "minGainIndex should be less than maxGainIndex");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that requested min gain index is not less than the absolute min gain index possible for this device*/
    if (minGainIndex < ADI_ADRV9025_MIN_GAIN_TABLE_INDEX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "minGainIndex should be greater than or equal to ADI_ADRV9025_MIN_GAIN_TABLE_INDEX");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that requested max gain index is not greater than the absolute max gain index possible for this device*/
    if (maxGainIndex > ADI_ADRV9025_MAX_GAIN_TABLE_INDEX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "maxGainIndex should be less than or equal to ADI_ADRV9025_MAX_GAIN_TABLE_INDEX");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxMinMaxGainIndexSet(adi_adrv9025_Device_t* device,
                                          uint32_t               rxChannelMask,
                                          uint8_t                minGainIndex,
                                          uint8_t                maxGainIndex)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adi_adrv9025_RxMinMaxGainIndexSetRangeCheck(device,
                                                rxChannelMask,
                                                minGainIndex,
                                                maxGainIndex);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Update device gain table min and max gain indices*/
    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX1) == (uint32_t)ADI_ADRV9025_RX1)
    {
        device->devStateInfo.gainIndexes.rx1MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx1MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX2) == (uint32_t)ADI_ADRV9025_RX2)
    {
        device->devStateInfo.gainIndexes.rx2MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx2MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX3) == (uint32_t)ADI_ADRV9025_RX3)
    {
        device->devStateInfo.gainIndexes.rx3MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx3MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX4) == (uint32_t)ADI_ADRV9025_RX4)
    {
        device->devStateInfo.gainIndexes.rx4MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx4MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX1) == (uint32_t)ADI_ADRV9025_ORX1)
    {
        device->devStateInfo.gainIndexes.orx1orx2MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.orx1orx2MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX2) == (uint32_t)ADI_ADRV9025_ORX2)
    {
        device->devStateInfo.gainIndexes.orx1orx2MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.orx1orx2MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX3) == (uint32_t)ADI_ADRV9025_ORX3)
    {
        device->devStateInfo.gainIndexes.orx3orx4MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.orx3orx4MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX4) == (uint32_t)ADI_ADRV9025_ORX4)
    {
        device->devStateInfo.gainIndexes.orx3orx4MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.orx3orx4MinGainIndex = minGainIndex;
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainTableReadRangeCheck(adi_adrv9025_Device_t*        device,
                                               adi_adrv9025_RxChannels_e     rxChannel,
                                               uint8_t                       gainIndexOffset,
                                               adi_adrv9025_RxGainTableRow_t gainTableRow[],
                                               uint16_t                      arraySize,
                                               uint16_t*                     numGainIndicesRead)
{
    UNUSED_PARA(numGainIndicesRead);
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check that gainTableRowPtr is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        gainTableRow);

    /*Check that the channel requested is valid*/
    if ((rxChannel != ADI_ADRV9025_RX1) &&
        (rxChannel != ADI_ADRV9025_RX2) &&
        (rxChannel != ADI_ADRV9025_RX3) &&
        (rxChannel != ADI_ADRV9025_RX4) &&
        (rxChannel != ADI_ADRV9025_ORX1) &&
        (rxChannel != ADI_ADRV9025_ORX2) &&
        (rxChannel != ADI_ADRV9025_ORX3) &&
        (rxChannel != ADI_ADRV9025_ORX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel Requested for gain table read. Valid Rx channels include Rx1-Rx4, ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((arraySize > ((ADI_ADRV9025_MAX_GAIN_TABLE_INDEX - ADI_ADRV9025_MIN_GAIN_TABLE_INDEX) + 1)) ||
        (arraySize == 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid arraySize parameter requested for Rx Channel Gain Table Read. Valid range is 1-256");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that gainIndexOffset is correct*/
    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
    {
        if ((gainIndexOffset > device->devStateInfo.gainIndexes.rx1MaxGainIndex) ||
            (gainIndexOffset < device->devStateInfo.gainIndexes.rx1MinGainIndex))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainIndexOffset,
                             "Invalid gainIndexOffset. Parameter gainIndexOffset should be within Rx1 min and max gain index range");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX2:
    {
        if ((gainIndexOffset > device->devStateInfo.gainIndexes.rx2MaxGainIndex) ||
            (gainIndexOffset < device->devStateInfo.gainIndexes.rx2MinGainIndex))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainIndexOffset,
                             "Invalid gainIndexOffset. Parameter gainIndexOffset should be within Rx2 min and max gain index range");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        break;
    }

    case ADI_ADRV9025_RX3:
    {
        if ((gainIndexOffset > device->devStateInfo.gainIndexes.rx3MaxGainIndex) ||
            (gainIndexOffset < device->devStateInfo.gainIndexes.rx3MinGainIndex))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainIndexOffset,
                             "Invalid gainIndexOffset. Parameter gainIndexOffset should be within Rx3 min and max gain index range");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX4:
    {
        if ((gainIndexOffset > device->devStateInfo.gainIndexes.rx4MaxGainIndex) ||
            (gainIndexOffset < device->devStateInfo.gainIndexes.rx4MinGainIndex))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainIndexOffset,
                             "Invalid gainIndexOffset. Parameter gainIndexOffset should be within Rx4 min and max gain index range");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_ORX1: /*Fall Through*/
    case ADI_ADRV9025_ORX2:
    {
        if ((gainIndexOffset > device->devStateInfo.gainIndexes.orx1orx2MaxGainIndex) ||
            (gainIndexOffset < device->devStateInfo.gainIndexes.orx1orx2MinGainIndex))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainIndexOffset,
                             "Invalid gainIndexOffset. Parameter gainIndexOffset should be within ORx1/ORx2 min and max gain index range");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_ORX3: /*Fall Through*/
    case ADI_ADRV9025_ORX4:
    {
        if ((gainIndexOffset > device->devStateInfo.gainIndexes.orx3orx4MaxGainIndex) ||
            (gainIndexOffset < device->devStateInfo.gainIndexes.orx3orx4MinGainIndex))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gainIndexOffset,
                             "Invalid gainIndexOffset. Parameter gainIndexOffset should be within ORx3/ORx4 min and max gain index range");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel Requested for gain table read. Valid Rx channels include Rx1-Rx4, ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainTableRead(adi_adrv9025_Device_t*        device,
                                     adi_adrv9025_RxChannels_e     rxChannel,
                                     uint8_t                       gainIndexOffset,
                                     adi_adrv9025_RxGainTableRow_t gainTableRow[],
                                     uint16_t                      arraySize,
                                     uint16_t*                     numGainIndicesRead)
{
    static const uint32_t NUM_BYTES_PER_GAIN_INDEX = 8;
    static const uint32_t ARM_DMA_AUTO_INCR        = 1;

    uint32_t baseAddress           = 0;
    uint16_t maxReadGainIndices    = arraySize;
    uint16_t numGainIndicesReadVal = 0;
    /*Maximum Array Size = Max Gain Table Size x Bytes Per Gain Table Entry*/
    static uint8_t armDmaData[((ADI_ADRV9025_MAX_GAIN_TABLE_INDEX - ADI_ADRV9025_MIN_GAIN_TABLE_INDEX) + 1) * 8] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check that gainTableRowPtr is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        gainTableRow);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adi_adrv9025_RxGainTableReadRangeCheck(device,
                                           rxChannel,
                                           gainIndexOffset,
                                           gainTableRow,
                                           maxReadGainIndices,
                                           numGainIndicesRead);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Calculate no. of indices to read and the base address for the config*/
    adrv9025_RxGainTableReadParamsCompute(device,
                                          rxChannel,
                                          maxReadGainIndices,
                                          gainIndexOffset,
                                          &numGainIndicesReadVal,
                                          &baseAddress);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Read Gain Table Data for the requested channel via ARM DMA*/
    adrv9025_CpuDmaMemRead(device,
                           baseAddress,
                           &armDmaData[0],
                           (numGainIndicesReadVal * NUM_BYTES_PER_GAIN_INDEX),
                           ARM_DMA_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error Reading Gain Table ARM DMA");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Parse gain table data obtained in ARM DMA data format to an rx gain table row entry datastructure memory*/
    adrv9025_RxGainTableParse(device,
                              &gainTableRow[0],
                              &armDmaData[0],
                              numGainIndicesReadVal);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error parsing gain table data");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Update no. of gain indices read*/
    if (numGainIndicesRead != NULL)
    {
        *numGainIndicesRead = numGainIndicesReadVal;
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainSetRangeCheck(adi_adrv9025_Device_t* device,
                                         adi_adrv9025_RxGain_t  rxGain[],
                                         uint8_t                arraySize)
{
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);
    static const uint32_t ORX12_MASK  = (uint32_t)(ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2);
    static const uint32_t ORX34_MASK  = (uint32_t)(ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4);

    uint32_t configIndex = 0;

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check device for null pointer */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    /*Check for null rxGain pointer*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxGain);

    /*Check that no. of Rx gain settings is valid*/
    if (arraySize == 0 || arraySize > 8)
    {
        ADI_ERROR_REPORT(&device->common,ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid Parameter arraySize. Number of configs should be between 1 and 8");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Range check manual gain index setting for the requested channel*/
    for (configIndex = 0; configIndex < arraySize; configIndex++)
    {
        /*Check that Rx profile is valid*/
        if ((rxGain[configIndex].rxChannelMask & ALL_RX_MASK) > 0)
        {
            if ((device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID) == 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxGain[configIndex].rxChannelMask,
                                 "Rx gain index set requested for an Rx Channel but Rx profile is invalid in the device structure");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        /*Check that ORx profile is valid*/
        if ((rxGain[configIndex].rxChannelMask & (ORX12_MASK | ORX34_MASK)) > 0)
        {
            if ((device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID) == 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxGain[configIndex].rxChannelMask,
                                 "Rx gain index set requested for an ORx channel but ORx profile is invalid in the device structure");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        /*Check that rxChannelMask is not out of range*/
        if ((rxGain[configIndex].rxChannelMask > (ALL_RX_MASK | ORX12_MASK | ORX34_MASK)) ||
            (rxGain[configIndex].rxChannelMask == ADI_ADRV9025_RXOFF))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGain[configIndex].rxChannelMask,
                             "Invalid Rx Channel Mask encountered for manual rx gain index setting");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Check that gain indices are within range for the channel selected*/
        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV9025_RX1) == (uint32_t)ADI_ADRV9025_RX1)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx1MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx1MinGainIndex))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxGain[configIndex].gainIndex,
                                 "Rx1 manual gain index setting is out of range");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV9025_RX2) == (uint32_t)ADI_ADRV9025_RX2)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx2MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx2MinGainIndex))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxGain[configIndex].gainIndex,
                                 "Rx2 manual gain index setting is out of range");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV9025_RX3) == (uint32_t)ADI_ADRV9025_RX3)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx3MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx3MinGainIndex))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxGain[configIndex].gainIndex,
                                 "Rx3 manual gain index setting is out of range");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV9025_RX4) == (uint32_t)ADI_ADRV9025_RX4)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx4MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx4MinGainIndex))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxGain[configIndex].gainIndex,
                                 "Rx4 manual gain index setting is out of range");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        if (((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV9025_ORX1) == (uint32_t)ADI_ADRV9025_ORX1) ||
            ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV9025_ORX2) == (uint32_t)ADI_ADRV9025_ORX2))
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.orx1orx2MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.orx1orx2MinGainIndex))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxGain[configIndex].gainIndex,
                                 "ORx1/ORx2 manual gain index setting is out of range");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        if (((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV9025_ORX3) == (uint32_t)ADI_ADRV9025_ORX3) ||
            ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV9025_ORX4) == (uint32_t)ADI_ADRV9025_ORX4))
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.orx3orx4MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.orx3orx4MinGainIndex))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxGain[configIndex].gainIndex,
                                 "ORx3/ORx4 manual gain index setting is out of range");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return device->common.error.newAction;
}





int32_t adi_adrv9025_RxGainSet(adi_adrv9025_Device_t* device,
                               adi_adrv9025_RxGain_t  rxGain[],
                               uint8_t                arraySize)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t configIndex    = 0;

    /*Check for null device pointer*/
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check for null rxGain pointer*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxGain);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adi_adrv9025_RxGainSetRangeCheck(device,
                                     rxGain,
                                     arraySize);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Update manual gain index setting for the requested channel */
    for (configIndex = 0; configIndex < arraySize; configIndex++)
    {
        if ((rxGain[configIndex].rxChannelMask & ADI_ADRV9025_RX1) == ADI_ADRV9025_RX1)
        {
            adrv9025_RxAgcManualGainIndexBfSet(device,
                                               ADRV9025_BF_RX_CH0,
                                               rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGain[configIndex].gainIndex,
                             "Error while attemtpting to set Rx manual gain index for channel Rx1");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((rxGain[configIndex].rxChannelMask & ADI_ADRV9025_RX2) == ADI_ADRV9025_RX2)
        {
            adrv9025_RxAgcManualGainIndexBfSet(device,
                                               ADRV9025_BF_RX_CH1,
                                               rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGain[configIndex].gainIndex,
                             "Error while attemtpting to set Rx manual gain index for channel Rx2");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((rxGain[configIndex].rxChannelMask & ADI_ADRV9025_RX3) == ADI_ADRV9025_RX3)
        {
            adrv9025_RxAgcManualGainIndexBfSet(device,
                                               ADRV9025_BF_RX_CH2,
                                               rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGain[configIndex].gainIndex,
                             "Error while attemtpting to set Rx manual gain index for channel Rx3");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((rxGain[configIndex].rxChannelMask & ADI_ADRV9025_RX4) == ADI_ADRV9025_RX4)
        {
            adrv9025_RxAgcManualGainIndexBfSet(device,
                                               ADRV9025_BF_RX_CH3,
                                               rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGain[configIndex].gainIndex,
                             "Error while attemtpting to set Rx manual gain index for channel Rx4");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((rxGain[configIndex].rxChannelMask & ADI_ADRV9025_ORX1) == ADI_ADRV9025_ORX1)
        {
            adrv9025_OrxOrx1AgcManualGainIndexBfSet(device,
                                                    ADRV9025_BF_ORX_CH0,
                                                    rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGain[configIndex].gainIndex,
                             "Error while attemtpting to set manual gain index for channel ORx1");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Write gain index to streamproc_debug_reg10  bitfield for this register erroneously masked bits 6 and 7 so using absolute address */
            recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                                       0x1B88,
                                                       rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recoveryAction,
                             NULL,
                             "Error while writing spi byte, rxGainIndex to streamproc_debug_reg10");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((rxGain[configIndex].rxChannelMask & ADI_ADRV9025_ORX2) == ADI_ADRV9025_ORX2)
        {
            adrv9025_OrxOrx2AgcManualGainIndexBfSet(device,
                                                    ADRV9025_BF_ORX_CH0,
                                                    rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGain[configIndex].gainIndex,
                             "Error while attemtpting to set manual gain index for channel ORx2");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Write gain index to streamproc_debug_reg12  bitfield for this register erroneously masked bits 6 and 7 so using absolute address */
            recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                                       0x1B8A,
                                                       rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recoveryAction,
                             NULL,
                             "Error while writing spi byte, rxGainIndex to streamproc_debug_reg12");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((rxGain[configIndex].rxChannelMask & ADI_ADRV9025_ORX3) == ADI_ADRV9025_ORX3)
        {
            adrv9025_OrxOrx1AgcManualGainIndexBfSet(device,
                                                    ADRV9025_BF_ORX_CH1,
                                                    rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGain[configIndex].gainIndex,
                             "Error while attemtpting to set manual gain index for channel ORx3");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Write gain index to streamproc_debug_reg10  bitfield for this register erroneously masked bits 6 and 7 so using absolute address */
            recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                                       0x1D88,
                                                       rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recoveryAction,
                             NULL,
                             "Error while writing spi byte, rxGainIndex to streamproc_debug_reg10");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((rxGain[configIndex].rxChannelMask & ADI_ADRV9025_ORX4) == ADI_ADRV9025_ORX4)
        {
            adrv9025_OrxOrx2AgcManualGainIndexBfSet(device,
                                                    ADRV9025_BF_ORX_CH1,
                                                    rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxGain[configIndex].gainIndex,
                             "Error while attemtpting to set manual gain index for channel ORx4");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Write gain index to streamproc_debug_reg12  bitfield for this register erroneously masked bits 6 and 7 so using absolute address */
            recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                                       0x1D8A,
                                                       rxGain[configIndex].gainIndex);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recoveryAction,
                             NULL,
                             "Error while writing spi byte, rxGainIndex to streamproc_debug_reg12");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainGetRangeCheck(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel,
                                         adi_adrv9025_RxGain_t*    rxGain)
{
    UNUSED_PARA(rxGain);
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);
    static const uint32_t ORX12_MASK  = (uint32_t)(ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2);
    static const uint32_t ORX34_MASK  = (uint32_t)(ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4);

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, rxGain);

    /*Check that the requested rxChannel is valid*/
    if ((rxChannel != ADI_ADRV9025_RX1) &&
        (rxChannel != ADI_ADRV9025_RX2) &&
        (rxChannel != ADI_ADRV9025_RX3) &&
        (rxChannel != ADI_ADRV9025_RX4) &&
        (rxChannel != ADI_ADRV9025_ORX1) &&
        (rxChannel != ADI_ADRV9025_ORX2) &&
        (rxChannel != ADI_ADRV9025_ORX3) &&
        (rxChannel != ADI_ADRV9025_ORX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel Requested for manual rx gain read");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that rx profile is valid in current config*/
    if (((uint32_t)rxChannel & ALL_RX_MASK) > 0)
    {
        if ((device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Gain index read requested for an ORx channel but Rx profile is invalid in device structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /*Check that orx profile is valid in current config*/
    if (((uint32_t)rxChannel & (ORX12_MASK | ORX34_MASK)) > 0)
    {
        if ((device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Gain index read requested for an ORx channel but ORx profile is invalid in device structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxMgcGainGet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_RxChannels_e rxChannel,
                                  adi_adrv9025_RxGain_t*    rxGain)
{
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);

    adrv9025_BfRxChanAddr_e rxChannelBitFieldAddr = ADRV9025_BF_RX_CH0;

    /* Check device for null pointer */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

	/*Check for null rxGain pointer*/
	ADI_NULL_PTR_RETURN(&device->common, rxGain);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adi_adrv9025_RxGainGetRangeCheck(device,
                                     rxChannel,
                                     rxGain);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     rxChannel,
                     "Invalid RX Channel or RX Gain value.");
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Check for null rxGain pointer*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxGain);

    /*Read the manual gain index setting*/
    if ((rxChannel & ALL_RX_MASK) > 0)
    {
        /*Resolve the Rx channel bitfield address*/
        adrv9025_RxBitfieldAddressGet(device,
                                      rxChannel,
                                      &rxChannelBitFieldAddr);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Error while attempting to decode Rx channel bitfield address");
        ADI_ERROR_RETURN(device->common.error.newAction);

        //Get Rx Manual gain in SPI register at offset 0x53
        adrv9025_RxAgcManualGainIndexBfGet(device,
                                           rxChannelBitFieldAddr,
                                           &rxGain->gainIndex);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Error while attempting to get Rx AGC gain index");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        /*Resolve the ORx channel bitfield address*/
        if (rxChannel == ADI_ADRV9025_ORX1)
        {
            /* Get ORx gain in SPI register at offset 0xBB */
            adrv9025_OrxOrx1AgcManualGainIndexBfGet(device, ADRV9025_BF_ORX_CH0, &rxGain->gainIndex);
        }
        else if (rxChannel == ADI_ADRV9025_ORX2)
        {
            /* Get ORx gain in SPI register at offset 0xBC */
            adrv9025_OrxOrx2AgcManualGainIndexBfGet(device, ADRV9025_BF_ORX_CH0, &rxGain->gainIndex);
        }
        else if (rxChannel == ADI_ADRV9025_ORX3)
        {
            adrv9025_OrxOrx1AgcManualGainIndexBfGet(device, ADRV9025_BF_ORX_CH1, &rxGain->gainIndex);
        }
        else if (rxChannel == ADI_ADRV9025_ORX4)
        {
            adrv9025_OrxOrx2AgcManualGainIndexBfGet(device, ADRV9025_BF_ORX_CH1, &rxGain->gainIndex);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Invalif ORX Channel parameter.");
        }

        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Error while attempting to read ORx manual gain index");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_RxChannels_e rxChannel,
                               adi_adrv9025_RxGain_t*    rxGain)
{
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);

    adrv9025_BfRxChanAddr_e rxChannelBitFieldAddr    = ADRV9025_BF_RX_CH0;
    uint8_t                 agcEnableGainIndexUpdate = 0;
    uint32_t                rxChannelStatus          = 0;
    uint32_t                txChannelStatus          = 0;

    /* Check device for null pointer */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adi_adrv9025_RxGainGetRangeCheck(device,
                                     rxChannel,
                                     rxGain);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     rxChannel,
                     "Invalid RX Channel or RX Gain value.");
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Check for null rxGain pointer*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxGain);

    /*Read the manual gain index setting*/
    if ((rxChannel & ALL_RX_MASK) > 0)
    {
        /*Resolve the Rx channel bitfield address*/
        adrv9025_RxBitfieldAddressGet(device,
                                      rxChannel,
                                      &rxChannelBitFieldAddr);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Error while attempting to decode Rx channel bitfield address");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Need to check agc_enable_gain_index_update. If this is enabled, then need to write dummy first before getting agc_gain_index. */
        adrv9025_RxAgcEnableGainIndexUpdateBfGet(device,
                                                 rxChannelBitFieldAddr,
                                                 &agcEnableGainIndexUpdate);
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (agcEnableGainIndexUpdate == ADI_TRUE)
        {
            adi_adrv9025_RxTxEnableGet(device,
                                       &rxChannelStatus,
                                       &txChannelStatus);
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             device->common.error.newAction,
                             device->common.error.varName,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if ((rxChannel & rxChannelStatus) != 0)
            {
                adrv9025_RxAgcEnableGainIndexUpdateLatch(device,
                                                         rxChannelBitFieldAddr);
                ADI_ERROR_REPORT(&device->common,
                                 device->common.error.errSource,
                                 device->common.error.errCode,
                                 device->common.error.newAction,
                                 device->common.error.varName,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxChannel,
                                 "AGC gain index set in write to latch mode hence corresponding RxChannel must be enabled to read AGC gain index value.");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        //Get Rx Auto gain in SPI register at offset 0x9E
        adrv9025_RxAgcGainIndexBfGet(device,
                                     rxChannelBitFieldAddr,
                                     &rxGain->gainIndex);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Error while attempting to set Rx AGC gain index");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        /* Need to check agc_enable_gain_index_update. If this is enabled, then need to write dummy first before getting agc_gain_index. */
        if (rxChannel == ADI_ADRV9025_ORX1 || rxChannel == ADI_ADRV9025_ORX2)
        {
            adrv9025_OrxAgcEnableGainIndexUpdateBfGet(device,
                                                      ADRV9025_BF_ORX_CH0,
                                                      &agcEnableGainIndexUpdate);
        }
        else if (rxChannel == ADI_ADRV9025_ORX3 || rxChannel == ADI_ADRV9025_ORX4)
        {
            adrv9025_OrxAgcEnableGainIndexUpdateBfGet(device,
                                                      ADRV9025_BF_ORX_CH1,
                                                      &agcEnableGainIndexUpdate);
        }

        // if the agc_enable_gain_index_update returns true, do the dummy write to latch the gain index
        if (agcEnableGainIndexUpdate == ADI_TRUE)
        {
            if (rxChannel == ADI_ADRV9025_ORX1)
            {
                adrv9025_OrxOrx1AgcEnableGainIndexUpdateLatch(device,
                                                              ADRV9025_BF_ORX_CH0);
            }
            else if (rxChannel == ADI_ADRV9025_ORX2)
            {
                adrv9025_OrxOrx2AgcEnableGainIndexUpdateLatch(device,
                                                              ADRV9025_BF_ORX_CH0);
            }
            else if (rxChannel == ADI_ADRV9025_ORX3)
            {
                adrv9025_OrxOrx1AgcEnableGainIndexUpdateLatch(device,
                                                              ADRV9025_BF_ORX_CH1);
            }
            else if (rxChannel == ADI_ADRV9025_ORX4)
            {
                adrv9025_OrxOrx2AgcEnableGainIndexUpdateLatch(device,
                                                              ADRV9025_BF_ORX_CH1);
            }
            else
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxChannel,
                                 "Invalid ORX Channel parameter.");
            }
        }

        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
        /*Resolve the ORx channel bitfield address*/
        if (rxChannel == ADI_ADRV9025_ORX1)
        {
            /* Get ORx gain in SPI register at offset 0xBB */
            adrv9025_OrxOrx1AgcGainIndexBfGet(device,
                                              ADRV9025_BF_ORX_CH0,
                                              &rxGain->gainIndex);
        }
        else if (rxChannel == ADI_ADRV9025_ORX2)
        {
            /* Get ORx gain in SPI register at offset 0xBC */
            adrv9025_OrxOrx2AgcGainIndexBfGet(device,
                                              ADRV9025_BF_ORX_CH0,
                                              &rxGain->gainIndex);
        }
        else if (rxChannel == ADI_ADRV9025_ORX3)
        {
            adrv9025_OrxOrx1AgcGainIndexBfGet(device,
                                              ADRV9025_BF_ORX_CH1,
                                              &rxGain->gainIndex);
        }
        else if (rxChannel == ADI_ADRV9025_ORX4)
        {
            adrv9025_OrxOrx2AgcGainIndexBfGet(device,
                                              ADRV9025_BF_ORX_CH1,
                                              &rxGain->gainIndex);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Invalid ORX Channel parameter.");
        }
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Error while attempting to read ORx manual gain index");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxDecPowerGet(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_RxChannels_e rxChannel,
                                   uint16_t*                 rxDecPower_mdBFS)
{
    adrv9025_BfRxChanAddr_e  rxChannelBitFieldAddr  = ADRV9025_BF_RX_CH0;
    adrv9025_BfOrxChanAddr_e orxChannelBitFieldAddr = ADRV9025_BF_ORX_CH0;
    uint8_t                  bfValue                = 0;

    static const uint32_t ALL_RX_MASK       = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);
    static const uint8_t  RX_DEC_POWER_MULT = 250; /* 250 = 1000 * 0.25dB */
    static const uint8_t  DEC_MAX_POWER     = 252;

    /* Check device for null pointer */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adrv9025_RxDecPowerGetRangeCheck(device,
                                     rxChannel);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /*Check for null rxDecPower pointer*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxDecPower_mdBFS);

    /*Read the manual gain index setting*/
    if ((rxChannel & ALL_RX_MASK) > 0)
    {
        /*Resolve the Rx channel bitfield address*/
        adrv9025_RxBitfieldAddressGet(device,
                                      rxChannel,
                                      &rxChannelBitFieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*
         * This register contains the decimated power readback value
         * for the Ch1 Rx datapath.Resolution is 0.25dB.
         * This register must be written(any value) to generate a strobe
         * that will latch the value to be read back.
         */
        ADRV9025_SPIWRITEBYTE("RX_DECIMATED_PWR",
                              (rxChannelBitFieldAddr + ADRV9025_ADDR_RX_DECIMATED_PWR_OFFSET),
                              0x0F);

        adrv9025_RxDecPowerBfGet(device,
                                 rxChannelBitFieldAddr,
                                 &bfValue);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        /*Resolve the ORx channel bitfield address*/
        adrv9025_OrxBitfieldAddressGet(device,
                                       rxChannel,
                                       &orxChannelBitFieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*
         * This register contains the decimated power readback value
         * for the Ch1 Rx datapath.Resolution is 0.25dB.
         * This register must be written(any value) to generate a strobe
         * that will latch the value to be read back.
         */
        ADRV9025_SPIWRITEBYTE("RX_DECIMATED_PWR",
                              (orxChannelBitFieldAddr + ADRV9025_ADDR_ORX_DECIMATED_PWR_OFFSET),
                              0x0F);

        adrv9025_OrxDecPowerBfGet(device,
                                  orxChannelBitFieldAddr,
                                  &bfValue);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*
     * Checking the DECIMATED Power range. The valid range is from 0 to -63 dB.
     * With 0.25 dB step, the bfValue can't greater than 252.
     * (i.e. with bfValue == 100 -> DEC power is (100 / 4) = 25 dB,
     * negative sign is implicit so it is -25 dB. */
    if (bfValue > DEC_MAX_POWER)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_SPI_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         rxChannel,
                         "Decimated Power is out of range.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    *rxDecPower_mdBFS = (((uint16_t)bfValue) * RX_DEC_POWER_MULT);

    return device->common.error.newAction;
}

/****************************************************************************
 * Initialization functions
 ****************************************************************************/

int32_t adi_adrv9025_RxGainTableDualBandLnaWrite(adi_adrv9025_Device_t*                device,
                                                 adi_adrv9025_DualBandRxGainTableCfg_t gainTableCfg[],
                                                 uint8_t                               arraySize)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, gainTableCfg);

    UNUSED_PARA(device);
    UNUSED_PARA(gainTableCfg);
    UNUSED_PARA(arraySize);
    return 0;
}

/* TODO: Needs doxygen header */
int32_t adi_adrv9025_RxGainTableDualBandLnaRead(adi_adrv9025_Device_t*                 device,
                                                adi_adrv9025_RxChannels_e              rxChannel,
                                                adi_adrv9025_DualBandRxGainTableCfg_t* gainTableCfg)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, gainTableCfg);

    UNUSED_PARA(device);
    UNUSED_PARA(rxChannel);
    UNUSED_PARA(gainTableCfg);
    return 0;
}

int32_t adi_adrv9025_RxFirWrite(adi_adrv9025_Device_t* device,
                                uint32_t               rxChanMask,
                                int8_t                 gain_dB,
                                uint8_t                numFirCoefs,
                                int16_t                coefs[],
                                uint8_t                arraySize)
{
    uint8_t               j                                      = 0;
    uint32_t              maskBit                                = 0;
    static const uint32_t maskArray[ADI_ADRV9025_MAX_RXCHANNELS] = {
        ADI_ADRV9025_RX1,
        ADI_ADRV9025_RX2,
        ADI_ADRV9025_RX3,
        ADI_ADRV9025_RX4,
        ADI_ADRV9025_ORX1,
        ADI_ADRV9025_ORX2,
        ADI_ADRV9025_ORX3,
        ADI_ADRV9025_ORX4,
        ADI_ADRV9025_LB12,
        ADI_ADRV9025_LB34
    };
    static const uint32_t VALID_RXCHANMASKALL  = 0x000003FF;
    static const uint32_t VALID_RXCHANMASK1234 = 0x0000000F;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        coefs);

    ADRV9025_BUGINFO(__FUNCTION__);

    if ((rxChanMask & VALID_RXCHANMASKALL) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChanMask,
                         "Invalid RxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (rxChanMask == VALID_RXCHANMASK1234)
    {
        adrv9025_RxFirWrite(device,
                            rxChanMask,
                            gain_dB,
                            numFirCoefs,
                            coefs,
                            arraySize);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        /* Program FIR filters */
        for (j = 0; j < ADI_ADRV9025_MAX_RXCHANNELS; j++)
        {
            maskBit = maskArray[j];
            if (ADRV9025_BF_EQUAL(rxChanMask,
                                  maskBit))
            {
                adrv9025_RxFirWrite(device,
                                    maskBit,
                                    gain_dB,
                                    numFirCoefs,
                                    coefs,
                                    arraySize);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RxFirRead(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_RxChannels_e rxChannel,
                               int8_t*                   gain_dB,
                               uint8_t*                  numFirCoefs,
                               int16_t                   coefs[],
                               uint8_t                   arraySize)
{
    uint8_t  firBankSel    = 0;
    uint8_t  i             = 0;
    uint8_t  j             = 0;
    uint8_t  numTapsReg    = 0;
    uint8_t  nTapMul       = 0;
    uint8_t  filterGain    = 0;
    uint8_t  pfirCoeffCtl  = 0;;
    uint16_t baseaddr      = 0;
    uint8_t  rxPfirBankSel = 0;
    uint8_t  byteLo        = 0;
    uint8_t  byteHi        = 0;
    uint8_t  filterConfig  = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        coefs);
    ADI_NULL_PTR_RETURN(&device->common,
                        gain_dB);
    ADI_NULL_PTR_RETURN(&device->common,
                        numFirCoefs);

    ADRV9025_BUGINFO(__FUNCTION__);

    static const uint8_t autoInc = ADI_ADRV9025_PFIR_COEFF_AUTO_INCR;

    static const uint8_t RXFIR_BANKA  = 0x01;
    static const uint8_t ORXFIR_BANKA = 0x02;
    static const uint8_t ORXFIR_BANKB = 0x04;
    static const uint8_t ORXFIR_BANKC = 0x08;

    static const uint8_t BANKA = 0;
    static const uint8_t BANKB = 1;
    static const uint8_t BANKC = 2;

    static const uint8_t RXFIR_MULTAPS = 24;

    if (adrv9025_RxAddrDecode(device,
                              rxChannel) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid RxChannel");
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

    /* firBankSelect and assignment of nTapMul and address firAddr.*/
    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1: /* fall through */
    case ADI_ADRV9025_RX2: /* fall through */
    case ADI_ADRV9025_RX3: /* fall through */
    case ADI_ADRV9025_RX4:
        firBankSel = RXFIR_BANKA;
        nTapMul = RXFIR_MULTAPS;
        break;

    case ADI_ADRV9025_ORX1: /* fall through */
    case ADI_ADRV9025_ORX3:
        firBankSel = ORXFIR_BANKA;
        nTapMul = RXFIR_MULTAPS;
        break;

    case ADI_ADRV9025_ORX2: /* fall through */
    case ADI_ADRV9025_ORX4:
        firBankSel = ORXFIR_BANKB;
        nTapMul = RXFIR_MULTAPS;
        break;

    case ADI_ADRV9025_LB12: /* fall through */
    case ADI_ADRV9025_LB34:
        firBankSel = ORXFIR_BANKC;
        nTapMul = RXFIR_MULTAPS;
        break;

    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Undefined Rx rxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (firBankSel == RXFIR_BANKA)
    {
        pfirCoeffCtl = ADRV9025_PFIR_COEFF_PROG_CLK_EN;
        if (autoInc != 0)
        {
            pfirCoeffCtl |= ADRV9025_PFIR_COEFF_AUTO_INCR;
        }

        baseaddr = adrv9025_RxAddrDecode(device,
                                         rxChannel);
        ADRV9025_SPIREADBYTE("RX_PFIR_SETTINGS",
                             (baseaddr + ADRV9025_ADDR_RX_PFIR_SETTINGS_OFFSET),
                             &filterConfig);
        numTapsReg = ADRV9025_BF_DECODE(filterConfig,
                                        ADRV9025_RX_PFIR_TAPS_MASK,
                                        ADRV9025_RX_PFIR_TAPS_SHIFT);
        *numFirCoefs = (numTapsReg + 1) * nTapMul;

        filterGain = ADRV9025_BF_DECODE(filterConfig,
                                        ADRV9025_RX_PFIR_GAIN_MASK,
                                        ADRV9025_RX_PFIR_GAIN_SHIFT);

        /* Set filter gain to be used */
        switch (filterGain)
        {
        case 0:
            *gain_dB = ADRV9025_FIR_GAIN_NEG12_DB;
            break;
        case 1:
            *gain_dB = ADRV9025_FIR_GAIN_NEG6_DB;
            break;
        case 2:
            *gain_dB = ADRV9025_FIR_GAIN_0_DB;
            break;
        case 3:
        default:
            *gain_dB = ADRV9025_FIR_GAIN_POS6_DB;
            break;
        }

        ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_CTL",
                              (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_CTL_OFFSET),
                              pfirCoeffCtl);

        /* pfir_coeff_addr */
        j = 0;
        ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_ADDR",
                              (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_ADDR_OFFSET),
                              j);

        if (arraySize > *numFirCoefs)
        {
            arraySize = *numFirCoefs;
        }

        for (i = 0; i < arraySize; i++)
        {
            /* pfir_coeff_data LSB */
            byteLo = 0;
            byteHi = 0;
            ADRV9025_SPIREADBYTE("RX_PFIR_COEFF_DATA_LSB",
                                 (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_DATA_OFFSET),
                                 &byteLo);

            if (autoInc == 0)
            {
                j++;
                ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_ADDR",
                                      (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_ADDR_OFFSET),
                                      j);
            }
            /* pfir_coeff_data MSB */
            ADRV9025_SPIREADBYTE("RX_PFIR_COEFF_DATA_MSB",
                                 (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_DATA_OFFSET),
                                 &byteHi);
            coefs[i] = (int16_t)((((uint16_t)byteHi) << 8) | byteLo);

            if (autoInc == 0)
            {
                j++;
                ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_ADDR",
                                      (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_ADDR_OFFSET),
                                      j);
            }
        }

        /* disable pfir_coeff_prog_clk_en */
        /* disable pfir_coeff_wr_enable */

        pfirCoeffCtl = 0;
        ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_CTL",
                              (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_CTL_OFFSET),
                              pfirCoeffCtl);
    }

    if (firBankSel & (ORXFIR_BANKA | ORXFIR_BANKB | ORXFIR_BANKC))
    {
        pfirCoeffCtl = ADRV9025_PFIR_COEFF_PROG_CLK_EN;
        if (autoInc != 0)
        {
            pfirCoeffCtl |= ADRV9025_PFIR_COEFF_AUTO_INCR;
        }

        /* pfir_coeff_rx_b_sel */
        if (firBankSel & ORXFIR_BANKA)
        {
            pfirCoeffCtl |= ADRV9025_PFIR_COEFF_RX_A_SEL;
        }
        else if (firBankSel & ORXFIR_BANKB)
        {
            pfirCoeffCtl |= ADRV9025_PFIR_COEFF_RX_B_SEL;
        }
        else if (firBankSel & ORXFIR_BANKC)
        {
            pfirCoeffCtl |= ADRV9025_PFIR_COEFF_RX_C_SEL;
        }

        baseaddr = adrv9025_RxAddrDecode(device,
                                         rxChannel);

        /* Write FIR #taps and gain */
        /* orx_pfir_Taps */
        /* rx_pfir_Taps */

        ADRV9025_SPIREADBYTE("ORX_PFIR_BANK_SEL",
                             (baseaddr + ADRV9025_ADDR_RX_PFIR_BANK_SEL_OFFSET),
                             &rxPfirBankSel);

        if (firBankSel & ORXFIR_BANKA)
        {
            ADRV9025_SPIREADBYTE("ORX_PFIR_CONFIG_SETTING_BANK_A",
                                 (baseaddr + ADRV9025_ADDR_RX_PFIR_CONFIG_SETTINGSN_OFFSET + BANKA),
                                 &filterConfig);
            /* Verify mapping for Obs Rx1/3 to FIR  Bank A */
            if (BANKA != ADRV9025_BF_DECODE(rxPfirBankSel,
                                            ADRV9025_PFIR_CONFIG_FOR_ORX_1_MASK,
                                            ADRV9025_PFIR_CONFIG_FOR_ORX_1_SHIFT))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxPfirBankSel,
                                 "Invalid rxPfirBankSel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
        else if (firBankSel & ORXFIR_BANKB)
        {
            ADRV9025_SPIREADBYTE("ORX_PFIR_CONFIG_SETTING_BANK_B",
                                 (baseaddr + ADRV9025_ADDR_RX_PFIR_CONFIG_SETTINGSN_OFFSET + BANKB),
                                 &filterConfig);
            /* Verify mapping for Obs Rx2/4 to FIR  Bank B */
            if (BANKB != ADRV9025_BF_DECODE(rxPfirBankSel,
                                            ADRV9025_PFIR_CONFIG_FOR_ORX_2_MASK,
                                            ADRV9025_PFIR_CONFIG_FOR_ORX_2_SHIFT))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxPfirBankSel,
                                 "Invalid rxPfirBankSel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
        else if (firBankSel & ORXFIR_BANKC)
        {
            ADRV9025_SPIREADBYTE("ORX_PFIR_CONFIG_SETTING_BANK_C",
                                 (baseaddr + ADRV9025_ADDR_RX_PFIR_CONFIG_SETTINGSN_OFFSET + BANKC),
                                 &filterConfig);
            /* Verify mapping for internal loopback1 to Bank C */
            if (BANKC != ADRV9025_BF_DECODE(rxPfirBankSel,
                                            ADRV9025_PFIR_CONFIG_FOR_LOOPBACK_1_MASK,
                                            ADRV9025_PFIR_CONFIG_FOR_LOOPBACK_1_SHIFT))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxPfirBankSel,
                                 "Invalid rxPfirBankSel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Verify mapping for internal loopback2 to Bank C */
            if (BANKC != ADRV9025_BF_DECODE(rxPfirBankSel,
                                            ADRV9025_PFIR_CONFIG_FOR_LOOPBACK_2_MASK,
                                            ADRV9025_PFIR_CONFIG_FOR_LOOPBACK_2_SHIFT))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxPfirBankSel,
                                 "Invalid rxPfirBankSel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        numTapsReg = ADRV9025_BF_DECODE(filterConfig,
                                        ADRV9025_RX_PFIR_TAPS_MASK,
                                        ADRV9025_RX_PFIR_TAPS_SHIFT);
        *numFirCoefs = (numTapsReg + 1) * nTapMul;

        filterGain = ADRV9025_BF_DECODE(filterConfig,
                                        ADRV9025_RX_PFIR_GAIN_MASK,
                                        ADRV9025_RX_PFIR_GAIN_SHIFT);

        /* Set filter gain to be used */
        switch (filterGain)
        {
        case 0:
            *gain_dB = ADRV9025_FIR_GAIN_NEG12_DB;
            break;
        case 1:
            *gain_dB = ADRV9025_FIR_GAIN_NEG6_DB;
            break;
        case 2:
            *gain_dB = ADRV9025_FIR_GAIN_0_DB;
            break;
        case 3:
        default:
            *gain_dB = ADRV9025_FIR_GAIN_POS6_DB;
            break;
        }

        ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_CTL",
                              (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_CTL_OFFSET),
                              pfirCoeffCtl);

        /* pfir_coeff_addr */
        j = 0;
        ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_ADDR",
                              (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_ADDR_OFFSET),
                              j);

        if (arraySize > *numFirCoefs)
        {
            arraySize = *numFirCoefs;
        }

        for (i = 0; i < arraySize; i++)
        {
            /* pfir_coeff_data LSB */
            byteLo = 0;
            byteHi = 0;
            ADRV9025_SPIREADBYTE("ORX_PFIR_COEFF_DATA_LSB",
                                 (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_DATA_OFFSET),
                                 &byteLo);

            if (autoInc == 0)
            {
                j++;
                ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_ADDR",
                                      (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_ADDR_OFFSET),
                                      j);
            }

            /* pfir_coeff_data MSB */
            ADRV9025_SPIREADBYTE("ORX_PFIR_COEFF_DATA_MSB",
                                 (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_DATA_OFFSET),
                                 &byteHi);
            coefs[i] = (int16_t)((((uint16_t)byteHi) << 8) | byteLo);

            if (autoInc == 0)
            {
                j++;
                ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_ADDR",
                                      (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_ADDR_OFFSET),
                                      j);
            }
        }

        /* disable pfir_coeff_prog_clk_en */
        /* disable pfir_coeff_wr_enable */

        pfirCoeffCtl = 0;
        ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_CTL",
                              (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_CTL_OFFSET),
                              pfirCoeffCtl);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RxGainCtrlModeSet(adi_adrv9025_Device_t*   device,
                                       adi_adrv9025_RxAgcMode_t gainMode[],
                                       uint8_t                  arraySize)
{
    static const uint32_t                  ORX12_MASK                              = (uint32_t)(ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2);
    static const uint8_t                   AGC_ENABLE_FAST_RECOVERY_LOOP_BITFIELD  = 1;
    static const uint8_t                   AGC_DISABLE_FAST_RECOVERY_LOOP_BITFIELD = 0;
    static const adi_adrv9025_RxChannels_e rxChannelArr[]                          = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};
    static const adi_adrv9025_RxChannels_e orxChannelArr[]                         = {
        ADI_ADRV9025_ORX1,
        ADI_ADRV9025_ORX2,
        ADI_ADRV9025_ORX3,
        ADI_ADRV9025_ORX4
    };

    uint32_t                 gainModeIndex  = 0;
    uint32_t                 rxChannelIndex = 0;
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddr;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        gainMode);

    for (gainModeIndex = 0; gainModeIndex < arraySize; gainModeIndex++)
    {
        for (rxChannelIndex = 0; rxChannelIndex < (sizeof(rxChannelArr) / sizeof(rxChannelArr[0])); rxChannelIndex++)
        {
            if ((gainMode[gainModeIndex].rxChannelMask & (uint32_t)rxChannelArr[rxChannelIndex]) ==
                (uint32_t)rxChannelArr[rxChannelIndex])
            {
                adrv9025_RxBitfieldAddressGet(device,
                                              rxChannelArr[rxChannelIndex],
                                              &rxChannelBitfieldAddr);
                ADI_ERROR_RETURN(device->common.error.newAction);

                adrv9025_RxAgcSetupBfSet(device,
                                         rxChannelBitfieldAddr,
                                         (uint8_t)gainMode[gainModeIndex].agcMode);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (gainMode[gainModeIndex].agcMode == ADI_ADRV9025_MGC)
                {
                    adrv9025_RxAgcEnableFastRecoveryLoopBfSet(device,
                                                              rxChannelBitfieldAddr,
                                                              AGC_DISABLE_FAST_RECOVERY_LOOP_BITFIELD);
                }
                else
                {
                    if ((device->devStateInfo.rxAgcPeakModeMask & rxChannelArr[rxChannelIndex]) > 0)
                    {
                        adrv9025_RxAgcEnableFastRecoveryLoopBfSet(device,
                                                                  rxChannelBitfieldAddr,
                                                                  AGC_ENABLE_FAST_RECOVERY_LOOP_BITFIELD);
                    }
                    else
                    {
                        adrv9025_RxAgcEnableFastRecoveryLoopBfSet(device,
                                                                  rxChannelBitfieldAddr,
                                                                  AGC_DISABLE_FAST_RECOVERY_LOOP_BITFIELD);
                    }
                }
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        for (rxChannelIndex = 0; rxChannelIndex < (sizeof(orxChannelArr) / sizeof(orxChannelArr[0])); rxChannelIndex++)
        {
            if ((gainMode[gainModeIndex].rxChannelMask & (uint32_t)orxChannelArr[rxChannelIndex]) ==
                (uint32_t)orxChannelArr[rxChannelIndex])
            {
                adrv9025_OrxBitfieldAddressGet(device,
                                               orxChannelArr[rxChannelIndex],
                                               &orxChannelBitfieldAddr);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if ((gainMode[gainModeIndex].rxChannelMask & ORX12_MASK) > 0)
                {
                    adrv9025_OrxOrx1AgcSetupBfSet(device,
                                                  orxChannelBitfieldAddr,
                                                  (uint8_t)gainMode[gainModeIndex].agcMode);
                }
                else
                {
                    adrv9025_OrxOrx2AgcSetupBfSet(device,
                                                  orxChannelBitfieldAddr,
                                                  (uint8_t)gainMode[gainModeIndex].agcMode);
                }
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (gainMode[gainModeIndex].agcMode == ADI_ADRV9025_MGC)
                {
                    adrv9025_OrxAgcEnableFastRecoveryLoopBfSet(device,
                                                               orxChannelBitfieldAddr,
                                                               AGC_DISABLE_FAST_RECOVERY_LOOP_BITFIELD);
                }
                else
                {
                    if ((device->devStateInfo.rxAgcPeakModeMask & orxChannelArr[rxChannelIndex]) > 0)
                    {
                        adrv9025_OrxAgcEnableFastRecoveryLoopBfSet(device,
                                                                   orxChannelBitfieldAddr,
                                                                   AGC_ENABLE_FAST_RECOVERY_LOOP_BITFIELD);
                    }
                    else
                    {
                        adrv9025_OrxAgcEnableFastRecoveryLoopBfSet(device,
                                                                   orxChannelBitfieldAddr,
                                                                   AGC_DISABLE_FAST_RECOVERY_LOOP_BITFIELD);
                    }
                }
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxGainCtrlModeGet(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_RxChannels_e rxChannel,
                                       adi_adrv9025_RxAgcMode_t* gainMode)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        gainMode);

    gainMode->agcMode       = (adi_adrv9025_RxAgcMode_e)0;
    gainMode->rxChannelMask = 0;

    switch (rxChannel)
    {
    case(ADI_ADRV9025_RX1):
    {
        gainMode->rxChannelMask = (uint32_t)ADI_ADRV9025_RX1;
        adrv9025_RxAgcSetupBfGet(device,
                                 ADRV9025_BF_RX_CH0,
                                 (uint8_t*)&gainMode->agcMode);
        break;
    }
    case(ADI_ADRV9025_RX2):
    {
        gainMode->rxChannelMask = (uint32_t)ADI_ADRV9025_RX2;
        adrv9025_RxAgcSetupBfGet(device,
                                 ADRV9025_BF_RX_CH1,
                                 (uint8_t*)&gainMode->agcMode);
        break;
    }
    case(ADI_ADRV9025_RX3):
    {
        gainMode->rxChannelMask = (uint32_t)ADI_ADRV9025_RX3;
        adrv9025_RxAgcSetupBfGet(device,
                                 ADRV9025_BF_RX_CH2,
                                 (uint8_t*)&gainMode->agcMode);
        break;
    }
    case(ADI_ADRV9025_RX4):
    {
        gainMode->rxChannelMask = (uint32_t)ADI_ADRV9025_RX4;
        adrv9025_RxAgcSetupBfGet(device,
                                 ADRV9025_BF_RX_CH3,
                                 (uint8_t*)&gainMode->agcMode);
        break;
    }
    case(ADI_ADRV9025_ORX1):
    {
        gainMode->rxChannelMask = (uint32_t)ADI_ADRV9025_ORX1;
        adrv9025_OrxOrx1AgcSetupBfGet(device,
                                      ADRV9025_BF_ORX_CH0,
                                      (uint8_t*)&gainMode->agcMode);
        break;
    }
    case(ADI_ADRV9025_ORX2):
    {
        gainMode->rxChannelMask = (uint32_t)ADI_ADRV9025_ORX2;
        adrv9025_OrxOrx1AgcSetupBfGet(device,
                                      ADRV9025_BF_ORX_CH0,
                                      (uint8_t*)&gainMode->agcMode);
        break;
    }
    case(ADI_ADRV9025_ORX3):
    {
        gainMode->rxChannelMask = (uint32_t)ADI_ADRV9025_ORX3;
        adrv9025_OrxOrx2AgcSetupBfGet(device,
                                      ADRV9025_BF_ORX_CH1,
                                      (uint8_t*)&gainMode->agcMode);
        break;
    }
    case(ADI_ADRV9025_ORX4):
    {
        gainMode->rxChannelMask = (uint32_t)ADI_ADRV9025_ORX4;
        adrv9025_OrxOrx2AgcSetupBfGet(device,
                                      ADRV9025_BF_ORX_CH1,
                                      (uint8_t*)&gainMode->agcMode);
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid rxChannel for reading Rx Gain Ctrl Mode");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxDataFormatSetRangeCheck(adi_adrv9025_Device_t*      device,
                                               adi_adrv9025_RxDataFormat_t rxDataFormat[],
                                               uint8_t                     arraySize)
{
    static const uint32_t ALL_RX_MASK            = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);
    static const uint32_t ALL_ORX_MASK           = (uint32_t)(ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4);
    static const uint8_t  MINIMUM_NUM_FORMATTERS = 1;
    static const uint8_t  MAXIMUM_NUM_FORMATTERS = 4;

    uint8_t rxDataFormatConfigIndex = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxDataFormat);

    /* Check that arraySize is in valid range */
    if (arraySize < MINIMUM_NUM_FORMATTERS)
    {
        /* invalid number of formatters */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid number of Rx formatters");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that arraySize is in valid range */
    if (arraySize > MAXIMUM_NUM_FORMATTERS)
    {
        /* invalid number of formatters */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_WARN_CHECK_PARAM,
                         arraySize,
                         "WARNING: Number of Rx formatters is larger than expected.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (rxDataFormatConfigIndex = 0; rxDataFormatConfigIndex < arraySize; rxDataFormatConfigIndex++)
    {
        if (rxDataFormat[rxDataFormatConfigIndex].rxChannelMask == ADI_ADRV9025_RXOFF)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             device->devStateInfo.profilesValid,
                             "Empty Rx channel mask selected for Rx data format configuration");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if (((rxDataFormat[rxDataFormatConfigIndex].rxChannelMask & ALL_RX_MASK) > 0) &&
            ((device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID) != ADI_ADRV9025_RX_PROFILE_VALID))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             device->devStateInfo.profilesValid,
                             "Rx channel selected for Rx data format configuration but Rx profile is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if (((rxDataFormat[rxDataFormatConfigIndex].rxChannelMask & ALL_ORX_MASK) > 0) &&
            ((device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID) != ADI_ADRV9025_ORX_PROFILE_VALID))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             device->devStateInfo.profilesValid,
                             "ORx channel selected for Rx data format configuration but ORx profile is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Check the format selected is valid*/
        if ((rxDataFormat[rxDataFormatConfigIndex].formatSelect != ADI_ADRV9025_GAIN_COMPENSATION_DISABLED) &&
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect != ADI_ADRV9025_GAIN_WITH_FLOATING_POINT) &&
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect != ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO) &&
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect != ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE) &&
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect != ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) &&
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect != ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER) &&
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect != ADI_ADRV9025_EMBED_OVERLOAD_MONITOR_DATA))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxDataFormat[rxDataFormatConfigIndex].formatSelect,
                             "Incorrect Rx Data Format selected");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Range Check params for Gain Compensation Mode Disabled*/
        if (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_COMPENSATION_DISABLED)
        {
            if ((rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings.intSampleResolution != ADI_ADRV9025_INTEGER_12BIT_2SCOMP) &&
                (rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings.intSampleResolution != ADI_ADRV9025_INTEGER_12BIT_SIGNED) &&
                (rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings.intSampleResolution != ADI_ADRV9025_INTEGER_16BIT_2SCOMP) &&
                (rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings.intSampleResolution != ADI_ADRV9025_INTEGER_16BIT_SIGNED) &&
                (rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings.intSampleResolution != ADI_ADRV9025_INTEGER_24BIT_2SCOMP) &&
                (rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings.intSampleResolution != ADI_ADRV9025_INTEGER_24BIT_SIGNED))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings.intSampleResolution,
                                 "Incorrect Integer format and resolution selected while attempting to disable gain compensation");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
            /*Check that floating point config is valid*/
        else if (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_WITH_FLOATING_POINT)
        {
            adrv9025_RxDataFormatSetFloatingPointRangeCheck(device,
                                                            &rxDataFormat[rxDataFormatConfigIndex].floatingPointConfig);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
            /*Check Integer and Slicer format configs if integer format selected*/
        else if ((rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER) ||
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE) ||
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) ||
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO))
        {
            adrv9025_RxDataFormatSetIntegerRangeCheck(device,
                                                      rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                                      rxDataFormat[rxDataFormatConfigIndex].formatSelect,
                                                      &rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings,
                                                      &rxDataFormat[rxDataFormatConfigIndex].slicerConfigSettings);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
            /* Check that embedded overload monitor config is valid */
        else if (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_EMBED_OVERLOAD_MONITOR_DATA)
        {
            if ((rxDataFormat[rxDataFormatConfigIndex].rxChannelMask & ALL_ORX_MASK) > 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                 "Embedded overload indicator on Rx data is only valid for Rx1-Rx4 channels");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            adrv9025_RxDataFormatSetEmbOvldMonitorRangeCheck(device,
                                                             &rxDataFormat[rxDataFormatConfigIndex].embOvldMonitorSettings,
                                                             rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings.intSampleResolution);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxDataFormatSet(adi_adrv9025_Device_t*      device,
                                     adi_adrv9025_RxDataFormat_t rxDataFormat[],
                                     uint8_t                     arraySize)
{
    static const uint8_t GAIN_COMP_EXT_LNA_DISABLE = 0;
    static const uint8_t GAIN_COMP_TEMP_DISABLE    = 0;

    uint8_t rxDataFormatConfigIndex = 0;
    uint8_t intDataResolution       = 0;
    uint8_t intDataFormat           = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check Rx data format config pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxDataFormat);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adi_adrv9025_RxDataFormatSetRangeCheck(device,
                                           rxDataFormat,
                                           arraySize);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (rxDataFormatConfigIndex = 0; rxDataFormatConfigIndex < arraySize; rxDataFormatConfigIndex++)
    {
        /*If Floating Point mode selected, Configure floating point*/
        if (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_WITH_FLOATING_POINT)
        {
            adrv9025_RxDataFormatFloatingPointSet(device,
                                                  rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                                  &rxDataFormat[rxDataFormatConfigIndex].floatingPointConfig);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxGainCompExtLnaSet(device,
                                         rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                         rxDataFormat[rxDataFormatConfigIndex].externalLnaGain);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxGainCompTempEnableSet(device,
                                             rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                             rxDataFormat[rxDataFormatConfigIndex].tempCompensationEnable);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
            /*Integer and slicer config settings*/
        else if ((rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER) ||
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE) ||
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) ||
            (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO))
        {
            adrv9025_RxDataFormatIntegerSet(device,
                                            rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                            rxDataFormat[rxDataFormatConfigIndex].formatSelect,
                                            &rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings,
                                            &rxDataFormat[rxDataFormatConfigIndex].slicerConfigSettings);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxGainCompExtLnaSet(device,
                                         rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                         rxDataFormat[rxDataFormatConfigIndex].externalLnaGain);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxGainCompTempEnableSet(device,
                                             rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                             rxDataFormat[rxDataFormatConfigIndex].tempCompensationEnable);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
            /* Embedded overload indicator settings */
        else if (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_EMBED_OVERLOAD_MONITOR_DATA)
        {
            adrv9025_RxDataFormatEmbOvldMonitorSet(device,
                                                   rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                                   &rxDataFormat[rxDataFormatConfigIndex].embOvldMonitorSettings);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxGainCompExtLnaSet(device,
                                         rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                         rxDataFormat[rxDataFormatConfigIndex].externalLnaGain);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxGainCompTempEnableSet(device,
                                             rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                             rxDataFormat[rxDataFormatConfigIndex].tempCompensationEnable);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
            /*If gain compensation disable is requested set floating point and integer foramatter regs to default*/
        else if (rxDataFormat[rxDataFormatConfigIndex].formatSelect == ADI_ADRV9025_GAIN_COMPENSATION_DISABLED)
        {
            /*Resolve Sample Resolution and format*/
            adrv9025_RxIntSampleResFormatResolve(device,
                                                 &rxDataFormat[rxDataFormatConfigIndex].integerConfigSettings,
                                                 &intDataResolution,
                                                 &intDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Disable gain compensation and reset the formatter settings to default */
            adrv9025_RxDataFormatGainCompDisable(device,
                                                 rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                                 intDataFormat,
                                                 intDataResolution);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxGainCompTempEnableSet(device,
                                             rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                             GAIN_COMP_TEMP_DISABLE);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxGainCompExtLnaSet(device,
                                         rxDataFormat[rxDataFormatConfigIndex].rxChannelMask,
                                         GAIN_COMP_EXT_LNA_DISABLE);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxDataFormatGet(adi_adrv9025_Device_t*       device,
                                     adi_adrv9025_RxChannels_e    rxChannel,
                                     adi_adrv9025_RxDataFormat_t* rxDataFormat)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check Rx data format config pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxDataFormat);

    /*Retrieve Rx Data Format select*/
    adrv9025_RxDataFormatSelectGet(device,
                                   rxChannel,
                                   &rxDataFormat->formatSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Fetch floating point config and update rxDataFormat with floating point config*/
    adrv9025_RxDataFormatFloatingPointGet(device,
                                          rxChannel,
                                          &rxDataFormat->floatingPointConfig);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get integer format config and update rxDataFormat with integer slicer config*/
    adrv9025_RxDataFormatIntegerGet(device,
                                    rxChannel,
                                    &rxDataFormat->integerConfigSettings,
                                    &rxDataFormat->slicerConfigSettings);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get embedded overload indicator config and update rxDataFormat with embedded overload monitor config*/
    adrv9025_RxDataFormatEmbOvldMonitorGet(device,
                                           rxChannel,
                                           &rxDataFormat->embOvldMonitorSettings);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxGainCompExtLnaGet(device,
                                 rxChannel,
                                 &rxDataFormat->externalLnaGain);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxGainCompTempEnableGet(device,
                                     rxChannel,
                                     &rxDataFormat->tempCompensationEnable);
    ADI_ERROR_RETURN(device->common.error.newAction);

    rxDataFormat->rxChannelMask = (uint32_t)rxChannel;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxSlicerPositionGet(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel,
                                         uint8_t*                  slicerPosition)
{
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddr  = ADRV9025_BF_ALL_RX_CHANNELS;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr = ADRV9025_BF_ALL_ORX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check slicerPosition pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        slicerPosition);

    if ((rxChannel == ADI_ADRV9025_RX1) ||
        (rxChannel == ADI_ADRV9025_RX2) ||
        (rxChannel == ADI_ADRV9025_RX3) ||
        (rxChannel == ADI_ADRV9025_RX4))
    {
        adrv9025_RxBitfieldAddressGet(device,
                                      rxChannel,
                                      &rxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxRxdpSlicerPositionBfGet(device,
                                           rxChannelBitfieldAddr,
                                           slicerPosition);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if ((rxChannel == ADI_ADRV9025_ORX1) ||
        (rxChannel == ADI_ADRV9025_ORX2) ||
        (rxChannel == ADI_ADRV9025_ORX3) ||
        (rxChannel == ADI_ADRV9025_ORX4))
    {
        adrv9025_OrxBitfieldAddressGet(device,
                                       rxChannel,
                                       &orxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_OrxRxdpSlicerPositionBfGet(device,
                                            orxChannelBitfieldAddr,
                                            slicerPosition);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Incorrect channel encountered while attempting to read slicer position. Valid Rx channels are Rx1-Rx4, ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxLoSourceGet(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_RxChannels_e rxChannel,
                                   adi_adrv9025_LoSel_e*     rxLoSource)
{
    static const uint32_t ALL_RX_MASK  = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);
    static const uint32_t ALL_ORX_MASK = (uint32_t)(ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4);

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check Rx data format config pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxLoSource);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adrv9025_RxLoSourceGetRangeCheck(device,
                                     rxChannel,
                                     rxLoSource);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    if (((uint32_t)rxChannel & ALL_RX_MASK) > 0)
    {
        adrv9025_RxLoSourceGet(device,
                               rxChannel,
                               rxLoSource);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (((uint32_t)rxChannel & ALL_ORX_MASK) > 0)
    {
        adrv9025_OrxLoSourceGet(device,
                                rxChannel,
                                rxLoSource);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxTempGainCompSet(adi_adrv9025_Device_t* device,
                                       uint32_t               rxChannelMask,
                                       int8_t                 gainValue)
{
    static const uint32_t ALL_RX_MASK  = ((uint32_t)ADI_ADRV9025_RX1 | (uint32_t)ADI_ADRV9025_RX2 | (uint32_t)ADI_ADRV9025_RX3 | (uint32_t)ADI_ADRV9025_RX4);
    static const uint32_t ALL_ORX_MASK = ((uint32_t)ADI_ADRV9025_ORX1 | (uint32_t)ADI_ADRV9025_ORX2 | (uint32_t)ADI_ADRV9025_ORX3 | (uint32_t)ADI_ADRV9025_ORX4
    );

    int32_t                  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfRxChanAddr_e  bfRxArray[]    = {ADRV9025_BF_RX_CH0, ADRV9025_BF_RX_CH1, ADRV9025_BF_RX_CH2, ADRV9025_BF_RX_CH3};
    adrv9025_BfOrxChanAddr_e bfOrxArray[]   = {ADRV9025_BF_ORX_CH0, ADRV9025_BF_ORX_CH1};
    uint32_t                 chanIndex      = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check Rx channel mask is valid.  It can represent both Rx and ORx channels. Recal it is a bitfield.*/
    if ((rxChannelMask > (ALL_RX_MASK | ALL_ORX_MASK)) ||
        (rxChannelMask == ADI_ADRV9025_RXOFF))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Channel provided is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that gainValue is within allowable range.*/
    if ((gainValue < -63) || (gainValue > 63))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         gainValue,
                         "Gain value must be within range -63 to +63.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        /*Register expects sign-magnitude value hence if value is negative, need to convert out of two's compliment and set the sign bit (bit 7) to 1*/
        if (gainValue < 0)
        {
            gainValue = (~gainValue + 1) | 0x40;
        }
    }

    /* Program up the Rx channels */
    for (chanIndex = 0; chanIndex < (sizeof(bfRxArray) / sizeof(bfRxArray[0])); ++chanIndex)
    {
        if ((rxChannelMask & (0x1 << chanIndex)) > 0)
        {
            recoveryAction = adrv9025_RxRxTempGainCompBfSet(device,
                                                            bfRxArray[chanIndex],
                                                            (uint8_t)gainValue);
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             recoveryAction,
                             rxChannelMask,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* Program up the ORx channels */
    for (chanIndex = 0; chanIndex < (sizeof(bfOrxArray) / sizeof(bfOrxArray[0])); ++chanIndex)
    {
        /* ORx analog channels 1 and 2 map to the same digital channel ORx 0 */
        /* ORx analog channels 3 and 4 map to the same digital channel ORx 1 */
        /* chanIndex is multiplied by 2 (i.e. << 1) in order to skip two bits at a time.*/
        if ((rxChannelMask & (0x30 << (chanIndex << 1))) > 0)
        {
            recoveryAction = adrv9025_OrxRxTempGainCompBfSet(device,
                                                             bfOrxArray[chanIndex],
                                                             (uint8_t)gainValue);
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             recoveryAction,
                             rxChannelMask,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxTempGainCompGet(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_RxChannels_e rxChannelSel,
                                       int8_t*                   gainValue)
{
    static const uint32_t ALL_RX_MASK = ((uint32_t)ADI_ADRV9025_RX1 | (uint32_t)ADI_ADRV9025_RX2 | (uint32_t)ADI_ADRV9025_RX3 | (uint32_t)ADI_ADRV9025_RX4);

    int32_t                  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfRxChanAddr_e  bfRxArray[]    = {ADRV9025_BF_RX_CH0, ADRV9025_BF_RX_CH1, ADRV9025_BF_RX_CH2, ADRV9025_BF_RX_CH3};
    adrv9025_BfOrxChanAddr_e bfOrxArray[]   = {ADRV9025_BF_ORX_CH0, ADRV9025_BF_ORX_CH1};
    uint32_t                 chanIndex      = 0;
    uint8_t                  gainRead       = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* Check gain value pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        gainValue);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check Rx channel mask is valid.  It can represent both Rx and ORx channels. Recal it is a bitfield.*/
    if ((rxChannelSel != ADI_ADRV9025_RX1) &&
        (rxChannelSel != ADI_ADRV9025_RX2) &&
        (rxChannelSel != ADI_ADRV9025_RX3) &&
        (rxChannelSel != ADI_ADRV9025_RX4) &&
        (rxChannelSel != ADI_ADRV9025_ORX1) &&
        (rxChannelSel != ADI_ADRV9025_ORX2) &&
        (rxChannelSel != ADI_ADRV9025_ORX3) &&
        (rxChannelSel != ADI_ADRV9025_ORX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Channel provided is out of range or multiple channels specified (only one channel can be read at a time).");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Retrieve the value from the Rx/ORx channel specified */
    if ((uint32_t)rxChannelSel < ALL_RX_MASK)
    {
        for (chanIndex = 0; chanIndex < (sizeof(bfRxArray) / sizeof(bfRxArray[0])); ++chanIndex)
        {
            if ((rxChannelSel & (0x1 << chanIndex)) > 0)
            {
                recoveryAction = adrv9025_RxRxTempGainCompBfGet(device,
                                                                bfRxArray[chanIndex],
                                                                &gainRead);
                ADI_ERROR_REPORT(&device->common,
                                 device->common.error.errSource,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 rxChannelSel,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);
                break;
            }
        }
    }
    else
    {
        for (chanIndex = 0; chanIndex < (sizeof(bfOrxArray) / sizeof(bfOrxArray[0])); ++chanIndex)
        {
            /* ORx virtual channels 1 and 3 map to the same physical channel ORx 0 */
            /* ORx virtual channels 2 and 4 map to the same physical channel ORx 1 */
            /* chanIndex is multiplied by 2 (i.e. << 1) in order to skip two bits at a time.*/
            if ((rxChannelSel & (0x30 << (chanIndex << 1))) > 0)
            {
                recoveryAction = adrv9025_OrxRxTempGainCompBfGet(device,
                                                                 bfOrxArray[chanIndex],
                                                                 &gainRead);
                ADI_ERROR_REPORT(&device->common,
                                 device->common.error.errSource,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 rxChannelSel,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);
                break;
            }
        }
    }

    /* Convert from 7-bit sign magnitude format to 8-bit two's compliment */
    if ((gainRead & 0x40) > 0)
    {
        /* Handle the negative case */
        *gainValue = (int8_t)(~((gainRead & 0x3F) - 1));
    }
    else
    {
        /* Handle the positive case */
        *gainValue = (int8_t)gainRead;
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_GpioForAnalogDualBandOperationSet(adi_adrv9025_Device_t* device,
                                                       uint8_t                enable)
{
    static const uint8_t              GPIO_ANA_UPPER_NIBBLE_SRC_CTRL_DEFAULT  = 0x00;
    static const uint8_t              GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_DEFAULT  = 0x00;
    static const uint8_t              GPIO_ANA_UPPER_NIBBLE_SRC_CTRL_DUALBAND = 0x07;
    static const uint8_t              GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_DUALBAND = 0x07;
    static const adrv9025_FeatureID_e THIS_FEATURE_ID                         = ADRV9025_FEATURE_RX_DUALBAND_LNA_CTRL_WORD_OUTPUT;

    int32_t recoveryAction                                                                    = ADI_COMMON_ACT_NO_ACTION;
    uint8_t gpioAnalogInputOutputMask                                                         = 0xFF;
    uint8_t resourceAcquisitionStatus                                                         = ADI_FAILURE;
    uint8_t resourceReleaseStatus                                                             = ADI_FAILURE;
    uint8_t numSharedResources                                                                = 8;
    int32_t sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_IN_RX_DUALBAND_CTRL_WORD_OUTPUT_FEATURE] =
    {
        ADI_ADRV9025_GPIO_ANA_00,
        ADI_ADRV9025_GPIO_ANA_01,
        ADI_ADRV9025_GPIO_ANA_02,
        ADI_ADRV9025_GPIO_ANA_03,
        ADI_ADRV9025_GPIO_ANA_04,
        ADI_ADRV9025_GPIO_ANA_05,
        ADI_ADRV9025_GPIO_ANA_06,
        ADI_ADRV9025_GPIO_ANA_07
    };

    /* Sanity checks */
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (enable == ADI_DISABLE)
    {
        /*Release All Shared Analog GPIOs*/
        adrv9025_SharedResourcesRelease(device,
                                        ADRV9025_SHARED_RESOURCE_GPIO_ANALOG,
                                        &sharedResourceArr[0],
                                        numSharedResources,
                                        THIS_FEATURE_ID,
                                        &resourceReleaseStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to release shared Analog GPIO during disabling of Rx1, Rx2, R3, R4 dualband ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);
        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error while attempting to release shared Analog GPIO during disabling of Rx1, Rx2, R3, R4 dualband ctrl output");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        /*Set GPIO_ANA[7:0] to input on disable request*/
        adi_adrv9025_GpioAnalogInputDirSet(device,
                                           gpioAnalogInputOutputMask);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set Analog GPIO direction ctrl to default during disabling of Rx1, Rx2, R3, R4 dualband ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);
        /*Set Analog GPIO Src Ctrl Lower Nibble to default*/
        adrv9025_CoreGpioAnalogLowerNibbleSourceControlBfSet(device,
                                                             ADRV9025_BF_CORE,
                                                             GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_DEFAULT);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set Analog GPIO lower nibble source ctrl to default during disabling of Rx1, Rx2 dualband ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Set Analog GPIO Src Ctrl Upper Nibble to default*/
        adrv9025_CoreGpioAnalogUpperNibbleSourceControlBfSet(device,
                                                             ADRV9025_BF_CORE,
                                                             GPIO_ANA_UPPER_NIBBLE_SRC_CTRL_DEFAULT);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set Analog GPIO lower nibble source ctrl to default during disabling of Rx3, Rx4 dualband ctrl output");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Finish and return.*/
        return device->common.error.newAction;
    }

    /*Acquire Shared Analog GPIOs*/
    adrv9025_SharedResourcesAcquire(device,
                                    ADRV9025_SHARED_RESOURCE_GPIO_ANALOG,
                                    &sharedResourceArr[0],
                                    numSharedResources,
                                    THIS_FEATURE_ID,
                                    &resourceAcquisitionStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error while attempting to acquire shared Analog GPIO during enabling of R1, R2, Rx3, Rx4 dualband ctrl output");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (resourceAcquisitionStatus == ADI_FAILURE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         resourceAcquisitionStatus,
                         "Error while attempting to acquire shared Analog GPIO during enabling of Rx3, Rx4 dualband ctrl output. Please check if the GPIOs in use by another feature");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Set GPIO_ANA[7:0] to output on enabling*/
    recoveryAction = adi_adrv9025_GpioAnalogOutputDirSet(device,
                                                         gpioAnalogInputOutputMask);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDualbandLnaCtrlPinReportError(device,
                                             &sharedResourceArr[0],
                                             numSharedResources,
                                             recoveryAction,
                                             "Error while attempting to set Analog GPIO lower nibble direction ctrl to dualband lna ctrl during enabling of Rx3, Rx4 dualband ctrl output");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Set Analog GPIO Src Ctrl Lower Nibble to dual band LNA ctrl */
    recoveryAction = adrv9025_CoreGpioAnalogLowerNibbleSourceControlBfSet(device,
                                                                          ADRV9025_BF_CORE,
                                                                          GPIO_ANA_LOWER_NIBBLE_SRC_CTRL_DUALBAND);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_RxDualbandLnaCtrlPinReportError(device,
                                             &sharedResourceArr[0],
                                             numSharedResources,
                                             recoveryAction,
                                             "Error while attempting to set Analog GPIO lower nibble source ctrl to dualband lna ctrl during enabling of Rx1, Rx2 dualband ctrl output");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Set Analog GPIO Src Ctrl Lower Nibble to dual band LNA ctrl */
    recoveryAction = adrv9025_CoreGpioAnalogUpperNibbleSourceControlBfSet(device,
                                                                          ADRV9025_BF_CORE,
                                                                          GPIO_ANA_UPPER_NIBBLE_SRC_CTRL_DUALBAND);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_RxDualbandLnaCtrlPinReportError(device,
                                             &sharedResourceArr[0],
                                             numSharedResources,
                                             recoveryAction,
                                             "Error while attempting to set Analog GPIO lower nibble source ctrl to dualband lna ctrl during enabling of Rx3, Rx4 dualband ctrl output");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_RxNcoShifterSet(adi_adrv9025_Device_t         *device,
                                     adi_adrv9025_RxChannels_e      rxChannel,
                                     adi_adrv9025_RxNcoShifterCfg_t rxShiftConfig,
                                     adi_adrv9025_RxDdc_e           rxDdcMode,
                                     uint32_t                       enableMask)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    uint8_t              enable        = 0x00;
    uint8_t              extData[]     = {ADRV9025_CPU_OBJECTID_GS_RX_DDC, (uint8_t)rxChannel, (uint8_t)rxDdcMode, enable};
    uint8_t              armData[32u]  = {0u};
    uint8_t              cmdStatusByte = 0u;
    int32_t              errHdl        = 0;

    static const uint8_t CPU_ERR_CODE = 0x0E;
    static const uint8_t bandANco1    = 0x0A;
    static const uint8_t bandBNco1    = 0x09;
    static const uint8_t bandANco2    = 0x06;
    static const uint8_t bandBNco2    = 0x05;
    static const uint8_t bandABEnable = 0x80;

    uint8_t rxNcoEnableMax = (ADI_ADRV9025_BAND_A_NCO_1 | ADI_ADRV9025_BAND_B_NCO_1 | ADI_ADRV9025_BAND_A_NCO_2 | ADI_ADRV9025_BAND_B_NCO_2);

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    /* Check if enable mask is valid */
    if (enableMask > rxNcoEnableMax)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Invalid enableMask");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Calculate byte to be passed to the ARM */
    if ((enableMask & ADI_ADRV9025_BAND_A_NCO_1) == ADI_ADRV9025_BAND_A_NCO_1)
    {
        enable = bandANco1;
    }
    if ((enableMask & ADI_ADRV9025_BAND_B_NCO_1) == ADI_ADRV9025_BAND_B_NCO_1)
    {
        enable |= bandBNco1;
    }
    if ((enableMask & ADI_ADRV9025_BAND_A_NCO_2) == ADI_ADRV9025_BAND_A_NCO_2)
    {
        enable |= bandANco2;
    }
    if ((enableMask & ADI_ADRV9025_BAND_B_NCO_2) == ADI_ADRV9025_BAND_B_NCO_2)
    {
        enable |= bandBNco2;
    }

    extData[3] = enable;

    /* Check if rxDdcMode is valid */
    if (rxDdcMode > ADI_ADRV9025_RXDDC_DEC2_REALIF)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Invalid rxDdcMode value");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* set the band combine bit in extended data if bandAbCombinedEnable = 1 */
    if (rxShiftConfig.bandAbCombinedEnable == 1)
    {
        extData[2] |= bandABEnable;
    }

    /* byte pack the shifter configuration in the arm mailbox array */
    rxShiftConfig.bandAInputBandWidth_kHz = rxShiftConfig.bandAInputBandWidth_kHz * 1000;

    armData[0] = (uint8_t)(rxShiftConfig.bandAInputBandWidth_kHz & 0xFF);
    armData[1] = (uint8_t)((rxShiftConfig.bandAInputBandWidth_kHz >> 8) & 0xFF);
    armData[2] = (uint8_t)((rxShiftConfig.bandAInputBandWidth_kHz >> 16) & 0xFF);
    armData[3] = (uint8_t)((rxShiftConfig.bandAInputBandWidth_kHz >> 24) & 0xFF);

    rxShiftConfig.bandAInputCenterFreq_kHz = rxShiftConfig.bandAInputCenterFreq_kHz * 1000;

    armData[4] = (uint8_t)(rxShiftConfig.bandAInputCenterFreq_kHz & 0xFF);
    armData[5] = (uint8_t)((rxShiftConfig.bandAInputCenterFreq_kHz >> 8) & 0xFF);
    armData[6] = (uint8_t)((rxShiftConfig.bandAInputCenterFreq_kHz >> 16) & 0xFF);
    armData[7] = (uint8_t)((rxShiftConfig.bandAInputCenterFreq_kHz >> 24) & 0xFF);

    rxShiftConfig.bandANco1Freq_kHz = rxShiftConfig.bandANco1Freq_kHz * 1000;

    armData[8]  = (uint8_t)(rxShiftConfig.bandANco1Freq_kHz & 0xFF);
    armData[9]  = (uint8_t)((rxShiftConfig.bandANco1Freq_kHz >> 8) & 0xFF);
    armData[10] = (uint8_t)((rxShiftConfig.bandANco1Freq_kHz >> 16) & 0xFF);
    armData[11] = (uint8_t)((rxShiftConfig.bandANco1Freq_kHz >> 24) & 0xFF);

    rxShiftConfig.bandANco2Freq_kHz = rxShiftConfig.bandANco2Freq_kHz * 1000;

    armData[12] = (uint8_t)(rxShiftConfig.bandANco2Freq_kHz & 0xFF);
    armData[13] = (uint8_t)((rxShiftConfig.bandANco2Freq_kHz >> 8) & 0xFF);
    armData[14] = (uint8_t)((rxShiftConfig.bandANco2Freq_kHz >> 16) & 0xFF);
    armData[15] = (uint8_t)((rxShiftConfig.bandANco2Freq_kHz >> 24) & 0xFF);

    rxShiftConfig.bandBInputBandWidth_kHz = rxShiftConfig.bandBInputBandWidth_kHz * 1000;

    armData[16] = (uint8_t)(rxShiftConfig.bandBInputBandWidth_kHz & 0xFF);
    armData[17] = (uint8_t)((rxShiftConfig.bandBInputBandWidth_kHz >> 8) & 0xFF);
    armData[18] = (uint8_t)((rxShiftConfig.bandBInputBandWidth_kHz >> 16) & 0xFF);
    armData[19] = (uint8_t)((rxShiftConfig.bandBInputBandWidth_kHz >> 24) & 0xFF);

    rxShiftConfig.bandBInputCenterFreq_kHz = rxShiftConfig.bandBInputCenterFreq_kHz * 1000;

    armData[20] = (uint8_t)(rxShiftConfig.bandBInputCenterFreq_kHz & 0xFF);
    armData[21] = (uint8_t)((rxShiftConfig.bandBInputCenterFreq_kHz >> 8) & 0xFF);
    armData[22] = (uint8_t)((rxShiftConfig.bandBInputCenterFreq_kHz >> 16) & 0xFF);
    armData[23] = (uint8_t)((rxShiftConfig.bandBInputCenterFreq_kHz >> 24) & 0xFF);

    rxShiftConfig.bandBNco1Freq_kHz = rxShiftConfig.bandBNco1Freq_kHz * 1000;

    armData[24] = (uint8_t)(rxShiftConfig.bandBNco1Freq_kHz & 0xFF);
    armData[25] = (uint8_t)((rxShiftConfig.bandBNco1Freq_kHz >> 8) & 0xFF);
    armData[26] = (uint8_t)((rxShiftConfig.bandBNco1Freq_kHz >> 16) & 0xFF);
    armData[27] = (uint8_t)((rxShiftConfig.bandBNco1Freq_kHz >> 24) & 0xFF);

    rxShiftConfig.bandBNco2Freq_kHz = rxShiftConfig.bandBNco2Freq_kHz * 1000;

    armData[28] = (uint8_t)(rxShiftConfig.bandBNco2Freq_kHz & 0xFF);
    armData[29] = (uint8_t)((rxShiftConfig.bandBNco2Freq_kHz >> 8) & 0xFF);
    armData[30] = (uint8_t)((rxShiftConfig.bandBNco2Freq_kHz >> 16) & 0xFF);
    armData[31] = (uint8_t)((rxShiftConfig.bandBNco2Freq_kHz >> 24) & 0xFF);

    /* Write data to ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                              &armData[0],
                                              sizeof(armData),
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Executing Arm set Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              ADRV9025_CPU_SET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     extData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_SETDEFAULTCONDITION_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETDEFAULTCONDITION_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & CPU_ERR_CODE) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     errHdl,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                              extData[0],
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RxNcoShifterGet(adi_adrv9025_Device_t             *device,
                                     adi_adrv9025_RxChannels_e          rxChannel,
                                     adi_adrv9025_RxNcoBandSelect_e     ncoBandSelect,
                                     adi_adrv9025_RxNcoShifterCfg_t    *rxShiftConfigGet,
                                     adi_adrv9025_RxDdc_e              *rxDdcMode,
                                     uint32_t                          *enableMask)
{
    int32_t              recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    uint8_t              extData[]         = {ADRV9025_CPU_OBJECTID_GS_RX_DDC, 0};
    uint8_t              armData[34u]      = {0u};
    uint8_t              cmdStatusByte     = 0u;
    uint8_t              ddcModeRead       = 0x00;
    int32_t              errHdl            = 0;
    uint8_t              armEnableMask     = 0x00;

    static const uint8_t ARM_ERR_CODE  = 0x0E;
    static const uint8_t ncoOff        = 0x00;
    static const uint8_t bandANco1     = 0x0A;
    static const uint8_t bandBNco1     = 0x09;
    static const uint8_t bandANco2     = 0x06;
    static const uint8_t bandBNco2     = 0x05;
    static const uint8_t bandABNco_1_2 = 0x0F;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* Check rxShiftConfigGet pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common, rxShiftConfigGet);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    if ((ncoBandSelect != ADI_ADRV9025_NCO_BANDA) && (ncoBandSelect != ADI_ADRV9025_NCO_BANDB))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NcoBandSelect,
                         "NCO Band Selection provided is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    extData[0] = ADRV9025_CPU_OBJECTID_GS_RX_DDC;
    extData[1] = (uint8_t)rxChannel;

    /* Executing Arm set Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     extData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_SETDEFAULTCONDITION_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETDEFAULTCONDITION_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     errHdl,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                                              extData[0],
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Read the data from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                             &armData[0],
                                             sizeof(armData),
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    rxShiftConfigGet->bandAInputBandWidth_kHz  = (uint32_t)armData[0];
    rxShiftConfigGet->bandAInputBandWidth_kHz |= (uint32_t)(armData[1] << 8 );
    rxShiftConfigGet->bandAInputBandWidth_kHz |= (uint32_t)(armData[2] << 16);
    rxShiftConfigGet->bandAInputBandWidth_kHz |= (uint32_t)(armData[3] << 24);
    rxShiftConfigGet->bandAInputBandWidth_kHz  = rxShiftConfigGet->bandAInputBandWidth_kHz / 1000;

    rxShiftConfigGet->bandAInputCenterFreq_kHz  = (uint32_t)armData[4];
    rxShiftConfigGet->bandAInputCenterFreq_kHz |= (uint32_t)(armData[5] << 8 );
    rxShiftConfigGet->bandAInputCenterFreq_kHz |= (uint32_t)(armData[6] << 16);
    rxShiftConfigGet->bandAInputCenterFreq_kHz |= (uint32_t)(armData[7] << 24);
    rxShiftConfigGet->bandAInputCenterFreq_kHz  = rxShiftConfigGet->bandAInputCenterFreq_kHz / 1000;

    rxShiftConfigGet->bandANco1Freq_kHz  = (uint32_t)armData[8];
    rxShiftConfigGet->bandANco1Freq_kHz |= (uint32_t)(armData[9]  << 8 );
    rxShiftConfigGet->bandANco1Freq_kHz |= (uint32_t)(armData[10] << 16);
    rxShiftConfigGet->bandANco1Freq_kHz |= (uint32_t)(armData[11] << 24);
    rxShiftConfigGet->bandANco1Freq_kHz  = rxShiftConfigGet->bandANco1Freq_kHz / 1000;

    rxShiftConfigGet->bandANco2Freq_kHz  = (uint32_t)armData[12];
    rxShiftConfigGet->bandANco2Freq_kHz |= (uint32_t)(armData[13] << 8 );
    rxShiftConfigGet->bandANco2Freq_kHz |= (uint32_t)(armData[14] << 16);
    rxShiftConfigGet->bandANco2Freq_kHz |= (uint32_t)(armData[15] << 24);
    rxShiftConfigGet->bandANco2Freq_kHz = rxShiftConfigGet->bandANco2Freq_kHz / 1000;

    rxShiftConfigGet->bandBInputBandWidth_kHz  = (uint32_t)armData[16];
    rxShiftConfigGet->bandBInputBandWidth_kHz |= (uint32_t)(armData[17] << 8 );
    rxShiftConfigGet->bandBInputBandWidth_kHz |= (uint32_t)(armData[18] << 16);
    rxShiftConfigGet->bandBInputBandWidth_kHz |= (uint32_t)(armData[19] << 24);
    rxShiftConfigGet->bandBInputBandWidth_kHz  = rxShiftConfigGet->bandBInputBandWidth_kHz / 1000;

    rxShiftConfigGet->bandBInputCenterFreq_kHz  = (uint32_t)armData[20];
    rxShiftConfigGet->bandBInputCenterFreq_kHz |= (uint32_t)(armData[21] << 8 );
    rxShiftConfigGet->bandBInputCenterFreq_kHz |= (uint32_t)(armData[22] << 16);
    rxShiftConfigGet->bandBInputCenterFreq_kHz |= (uint32_t)(armData[23] << 24);
    rxShiftConfigGet->bandBInputCenterFreq_kHz  = rxShiftConfigGet->bandBInputCenterFreq_kHz / 1000;

    rxShiftConfigGet->bandBNco1Freq_kHz  = (uint32_t)armData[24];
    rxShiftConfigGet->bandBNco1Freq_kHz |= (uint32_t)(armData[25] << 8 );
    rxShiftConfigGet->bandBNco1Freq_kHz |= (uint32_t)(armData[26] << 16);
    rxShiftConfigGet->bandBNco1Freq_kHz |= (uint32_t)(armData[27] << 24);
    rxShiftConfigGet->bandBNco1Freq_kHz  = rxShiftConfigGet->bandBNco1Freq_kHz / 1000;

    rxShiftConfigGet->bandBNco2Freq_kHz  = (uint32_t)armData[28];
    rxShiftConfigGet->bandBNco2Freq_kHz |= (uint32_t)(armData[29] << 8 );
    rxShiftConfigGet->bandBNco2Freq_kHz |= (uint32_t)(armData[30] << 16);
    rxShiftConfigGet->bandBNco2Freq_kHz |= (uint32_t)(armData[31] << 24);
    rxShiftConfigGet->bandBNco2Freq_kHz  = rxShiftConfigGet->bandBNco2Freq_kHz / 1000;

    rxShiftConfigGet->bandAbCombinedEnable = ((uint32_t)(armData[32] & 0x80) >> 7);

    ddcModeRead  = (uint32_t)(armData[32] & 0x7F);

    *rxDdcMode = (adi_adrv9025_RxDdc_e)ddcModeRead;

    /* Check if rxDdcMode returned value is valid */
    if (*rxDdcMode > ADI_ADRV9025_RXDDC_DEC2_REALIF)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Invalid rxDdcMode value returned");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    armEnableMask = armData[33];

    if (armEnableMask <= bandABNco_1_2)
    {
        if (armEnableMask == ncoOff)
        {
            *enableMask = ADI_ADRV9025_NCO_OFF;
        }
        /* Calculate byte to be passed to the ARM */
        if ((armEnableMask & bandANco1) == bandANco1)
        {
            *enableMask = ADI_ADRV9025_BAND_A_NCO_1;
        }
        if ((armEnableMask & bandBNco1) == bandBNco1)
        {
            *enableMask |= ADI_ADRV9025_BAND_B_NCO_1;
        }
        if ((armEnableMask & bandANco2) == bandANco2)
        {
            *enableMask |= ADI_ADRV9025_BAND_A_NCO_2;
        }
        if ((armEnableMask & bandBNco2) == bandBNco2)
        {
            *enableMask |= ADI_ADRV9025_BAND_B_NCO_2;
        }
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Invalid RX NCO Enable value returned");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RxAgcClkModeSet(adi_adrv9025_Device_t     *device,
                                     uint32_t                   rxChannelMask,
                                     adi_adrv9025_RxAgcMode_e   agcMode)
{
    /* Check that the passed device pointer is not NULL */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* Add entry to the API log */
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    uint8_t                   rxChannelListPosition = 0;
    int32_t                   recoveryAction        = ADI_COMMON_ACT_ERR_RESET_FULL;
    adi_adrv9025_RxChannels_e rxChannel             = ADI_ADRV9025_RXOFF;
    adrv9025_BfRxChanAddr_e   rxChannelBfAddr       = ADRV9025_BF_ALL_RX_CHANNELS;

    static const uint8_t                   rxChannelMax    = 0x0F;
    static const adi_adrv9025_RxChannels_e rxChannelList[] = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};
    static const uint8_t rxChannelListSize                 = (adi_adrv9025_RxChannels_e)( sizeof(rxChannelList) / sizeof(rxChannelList[0]));

    if ((rxChannelMask > rxChannelMax) || (rxChannelMask == 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Invalid rxChannelMask. Valid range (1-15)");
                         ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Iterate through channel mask */
    for (rxChannelListPosition = 0; rxChannelListPosition < rxChannelListSize; rxChannelListPosition++)
    {
        if ((rxChannelMask & (uint32_t)rxChannelList[rxChannelListPosition]) == (uint32_t)rxChannelList[rxChannelListPosition])
        {
            rxChannel = (adi_adrv9025_RxChannels_e)rxChannelList[rxChannelListPosition];

            switch (rxChannel)
            {
                case ADI_ADRV9025_RX1:
                    rxChannelBfAddr = ADRV9025_BF_RX_CH0;
                break;
                case ADI_ADRV9025_RX2:
                    rxChannelBfAddr = ADRV9025_BF_RX_CH1;
                    break;
                case ADI_ADRV9025_RX3:
                    rxChannelBfAddr = ADRV9025_BF_RX_CH2;
                    break;
                case ADI_ADRV9025_RX4:
                    rxChannelBfAddr = ADRV9025_BF_RX_CH3;
                    break;
                case ADI_ADRV9025_ORX1:
                case ADI_ADRV9025_ORX2:
                case ADI_ADRV9025_ORX3:
                case ADI_ADRV9025_ORX4:
                case ADI_ADRV9025_LB12:
                case ADI_ADRV9025_LB34:
                default:
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     rxChannel,
                                     "Invalid Rx Channel selected");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                break;
            }
            recoveryAction = adrv9025_RxAgcClkDivideRatioBfSet(device, rxChannelBfAddr, agcMode);

            if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                                 bfValue,
                                 "Failed to set AGC clock ratio mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RxAgcClkModeGet(adi_adrv9025_Device_t     *device,
                                     adi_adrv9025_RxChannels_e  rxChannel,
                                     uint8_t                   *agcMode)
{
    /* Check that the passed device pointer is not NULL */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* Check agcMode is not null */
    ADI_NULL_PTR_RETURN(&device->common, agcMode);

    /* Add entry to the API log */
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    adrv9025_BfRxChanAddr_e rxChannelBfAddr = ADRV9025_BF_ALL_RX_CHANNELS;

    int32_t recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;

    switch (rxChannel)
    {
        case ADI_ADRV9025_RX1:
            rxChannelBfAddr = ADRV9025_BF_RX_CH0;
            break;
        case ADI_ADRV9025_RX2:
            rxChannelBfAddr = ADRV9025_BF_RX_CH1;
            break;
        case ADI_ADRV9025_RX3:
            rxChannelBfAddr = ADRV9025_BF_RX_CH2;
            break;
        case ADI_ADRV9025_RX4:
            rxChannelBfAddr = ADRV9025_BF_RX_CH3;
            break;
        case ADI_ADRV9025_ORX1:
        case ADI_ADRV9025_ORX2:
        case ADI_ADRV9025_ORX3:
        case ADI_ADRV9025_ORX4:
        case ADI_ADRV9025_LB12:
        case ADI_ADRV9025_LB34:
        default:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Invalid Rx Channel selected");
            ADI_ERROR_RETURN(device->common.error.newAction);
            break;
    }

    recoveryAction = adrv9025_RxAgcClkDivideRatioBfGet(device, rxChannelBfAddr, agcMode);

    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         bfValue,
                         "Failed to get AGC clock ratio mode status");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RxAgcSyncGpioPinSet(adi_adrv9025_Device_t     *device,
                                         uint8_t                   rxChannelMask,
                                         adi_adrv9025_GpioPinSel_e hybridSelPin1)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID                           = ADRV9025_FEATURE_RX_AGC_HYBRID_MODE;
    int32_t  retVal                                                             = ADI_COMMON_ACT_NO_ACTION;
    int32_t  sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_IN_RX_AGC_HYBRID_FEATURE] = { 0 };
    uint32_t gpioInputDirMask                                                   = 0;
    uint8_t  resourceAcqReleaseStatus                                           = ADI_FAILURE;
    static uint16_t channel[4]                                                  = { 0x13CB, 0x15CB, 0x17CB, 0x19CB };
    uint8_t i, j;

    /* Check device pointer is not null */

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /* Setup Hybrid mode select pin0 */
    sharedResourceArr[0] = hybridSelPin1;
    gpioInputDirMask     = ((uint32_t)0x00000001 << hybridSelPin1);

    /*Acquire Shared GPIOs to ensure that they are not in use by another feature*/
    retVal = adrv9025_SharedResourcesRelease(device,
                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                             &sharedResourceArr[0],
                                             1,
                                             THIS_FEATURE_ID,
                                             &resourceAcqReleaseStatus);

    if (resourceAcqReleaseStatus == ADI_FAILURE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Unable to release requested GPIOs for Hybrid Mode use. Please check if the GPIOs are in use by another feature");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Set the direction as input for selected Rx AGC Hybrid Mode on release*/
    if ((retVal = adi_adrv9025_GpioInputDirSet(device, gpioInputDirMask)) != ADI_COMMON_ACT_NO_ACTION)
    {
        /*Clear the error so that clean up functions execute*/
        retVal = adrv9025_ErrorClear(&device->common);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Re-Acquire Shared GPIOs if setting of GPIO Input Dir is unsuccessful*/
        retVal = adrv9025_SharedResourcesAcquire(device,
                                                 ADRV9025_SHARED_RESOURCE_GPIO,
                                                 &sharedResourceArr[0],
                                                 1,
                                                 THIS_FEATURE_ID,
                                                 &resourceAcqReleaseStatus);

        if (resourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Unable to re-acquire GPIOs for Rx AGC Hybrid mode while attempting to exit the function due to GPIO dir set error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             device->common.error.newAction,
                             device->common.error.varName,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    for (i = 0, j=1; i < 4; i++, j = j*2)
    {

        if ((rxChannelMask&j) != 0)
        {
            retVal = adi_adrv9025_SpiByteWrite(device, channel[i], hybridSelPin1);
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             device->common.error.newAction,
                             device->common.error.varName,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return retVal;
}
