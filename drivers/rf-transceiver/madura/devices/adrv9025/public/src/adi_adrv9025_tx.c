// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_tx.c
* \brief Contains Tx features related function implementation defined in
* adi_adrv9025_tx.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_arm.h"
#include "adi_adrv9025_tx.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_gpio.h"
#include "adi_adrv9025.h"
#include "adi_adrv9025_cpu.h"
#include "../../private/include/adrv9025_bf_tx.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_tx.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"
#include "../../private/include/adrv9025_bf_core.h"
#include "../../private/include/adrv9025_bf_txdac_mem_map.h"

#include "no_os_util.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_tx.c"
#endif

#define ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_CTRL_SPI2_PIN_FEATURE 1U

int32_t adi_adrv9025_TxAttenCfgSet(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_TxAttenCfg_t txAttenCfg[],
                                   uint8_t                   attenCfgs)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    int8_t  i              = 0;
    int8_t  j              = 0;
    /* uint16_t regData = 0;*/
    adi_adrv9025_TxChannels_e txChannel         = ADI_ADRV9025_TXOFF;
    adrv9025_BfTxChanAddr_e   txChannelBaseAddr = ADRV9025_BF_ALL_TX_CHANNELS;
    uint32_t                  channelLoop       = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenCfg);

    /* check configs is > 0 */
    if (attenCfgs == 0)
    {
        /* no valid configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         attenCfgs,
                         "Invalid number of configs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Loop through the number of configurations passed */
    for (i = 0; i < attenCfgs; i++)
    {
#if ADI_ADRV9025_TX_RANGE_CHECK > 0
        recoveryAction = adrv9025_TxAttenCfgSetRangeCheck(device,
                                                          &txAttenCfg[i]);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         txAttenCfg[i],
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
#endif

        /* Write configuration */
        channelLoop = txAttenCfg[i].txChannelMask;
        for (j = 0; j < ADI_ADRV9025_MAX_TXCHANNELS; j++)
        {
            if (ADRV9025_BF_EQUAL(channelLoop,
                                  ADI_ADRV9025_TX1))
            {
                txChannel                               = ADI_ADRV9025_TX1;
                txChannelBaseAddr                       = ADRV9025_BF_TX_CH0;
                channelLoop                             = channelLoop & ~ADI_ADRV9025_TX1;
                device->devStateInfo.txAttenStepSize[0] = txAttenCfg[i].txAttenStepSize;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX2))
            {
                txChannel                               = ADI_ADRV9025_TX2;
                txChannelBaseAddr                       = ADRV9025_BF_TX_CH1;
                channelLoop                             = channelLoop & ~ADI_ADRV9025_TX2;
                device->devStateInfo.txAttenStepSize[1] = txAttenCfg[i].txAttenStepSize;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX3))
            {
                txChannel                               = ADI_ADRV9025_TX3;
                txChannelBaseAddr                       = ADRV9025_BF_TX_CH2;
                channelLoop                             = channelLoop & ~ADI_ADRV9025_TX3;
                device->devStateInfo.txAttenStepSize[2] = txAttenCfg[i].txAttenStepSize;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX4))
            {
                txChannel                               = ADI_ADRV9025_TX4;
                txChannelBaseAddr                       = ADRV9025_BF_TX_CH3;
                channelLoop                             = channelLoop & ~ADI_ADRV9025_TX4;
                device->devStateInfo.txAttenStepSize[3] = txAttenCfg[i].txAttenStepSize;
            }

                /* error condition break loop and report error */
            else if (channelLoop == 0)
            {
                /* All channels are set up */
                break;
            }

            else
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 txAttenPinCfg[i].txChannelMask,
                                 "Error while accessing selected Tx channel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Configure part */
            /* Attenuation step size */
            recoveryAction = adrv9025_TxTxAttenConfigBfSet(device,
                                                           txChannelBaseAddr,
                                                           (uint8_t)(txAttenCfg[i].txAttenStepSize));
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             txAttenCfg[i].txAttenStepSize,
                             "Error while setting Tx atten step size bitfield");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Attenuation mode */
            recoveryAction = adi_adrv9025_TxAttenModeSet(device,
                                                         (uint32_t)txChannel,
                                                         txAttenCfg[i].attenMode,
                                                         txAttenCfg[i].simultaneousUpdateCfg,
                                                         txAttenCfg[i].gpioModePinCfg);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             txAttenCfg[i].attenMode,
                             "Error while setting Tx atten mode bitfield");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenCfgGet(adi_adrv9025_Device_t*     device,
                                   adi_adrv9025_TxChannels_e  txChannel,
                                   adi_adrv9025_TxAttenCfg_t* txAttenCfg)
{
    int32_t                               recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e               txChannelBaseAddr     = ADRV9025_BF_ALL_TX_CHANNELS;
    adi_adrv9025_TxAttenMode_e            txModeRead            = ADI_ADRV9025_TXATTEN_SPI_MODE;
    adi_adrv9025_TxDataIfUnlock_e         txJesdData            = ADI_ADRV9025_TXUNLOCK_TX_NOT_DISABLED;
    adi_adrv9025_TxDataIfUnlock_e         txRfPllData           = ADI_ADRV9025_TXUNLOCK_TX_NOT_DISABLED;
    adi_adrv9025_TxAttenStepSize_e        txAttenStepRead       = ADI_ADRV9025_TXATTEN_0P05_DB;
    adi_adrv9025_TxAttenSimultUpdateCfg_t simultaneousUpdateCfg = {ADI_ADRV9025_GPIO_INVALID, ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_DISABLED};
    adi_adrv9025_TxAttenPinCfg_t          gpioModePinCfg        = {0, 0, ADI_ADRV9025_GPIO_INVALID, ADI_ADRV9025_GPIO_INVALID};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenCfg);

    if ((txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                            txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Read Configure part */
    /* Attenuation step size */
    recoveryAction = adrv9025_TxTxAttenConfigBfGet(device,
                                                   txChannelBaseAddr,
                                                   (uint8_t*)&txAttenStepRead);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     txAttenCfg[].attenMode,
                     "Error while attempting to set Attenuation step");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* RFPLL unlock setting for Tx attanuation ramp down - Silicon B*/
    recoveryAction = adrv9025_TxTxDataRampDownOnPllUnlockBfGet(device,
                                                               txChannelBaseAddr,
                                                               (uint8_t*)&txRfPllData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read RfPll unlock settings mode - Silicon B");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* retrieve JESD unlock setting for Tx attanuation ramp down - Silicon B*/
    recoveryAction = adrv9025_TxTxDataRampDownOnJesdDfrmIrqBfGet(device,
                                                                 txChannelBaseAddr,
                                                                 (uint8_t*)&txJesdData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read Jesd unlock settings mode - Silicon B");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Retrieve attenuation mode */
    recoveryAction = adi_adrv9025_TxAttenModeGet(device,
                                                 txChannel,
                                                 &txModeRead,
                                                 &simultaneousUpdateCfg,
                                                 &gpioModePinCfg);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to get attenuation mode");
    ADI_ERROR_RETURN(device->common.error.newAction);

    txAttenCfg->txChannelMask                            = txChannel;
    txAttenCfg->attenMode                                = txModeRead;
    txAttenCfg->disTxDataIfPllUnlock                     = txRfPllData;
    txAttenCfg->rampJesdDfrm                             = txJesdData;
    txAttenCfg->txAttenStepSize                          = txAttenStepRead;
    txAttenCfg->simultaneousUpdateCfg.gpioUpdatePin      = simultaneousUpdateCfg.gpioUpdatePin;
    txAttenCfg->simultaneousUpdateCfg.simultaneousUpdate = simultaneousUpdateCfg.simultaneousUpdate;
    txAttenCfg->gpioModePinCfg.txChannelMask             = gpioModePinCfg.txChannelMask;
    txAttenCfg->gpioModePinCfg.stepSize                  = gpioModePinCfg.stepSize;
    txAttenCfg->gpioModePinCfg.txAttenIncPin             = gpioModePinCfg.txAttenIncPin;
    txAttenCfg->gpioModePinCfg.txAttenDecPin             = gpioModePinCfg.txAttenDecPin;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenModeSet(adi_adrv9025_Device_t*                device,
                                    uint32_t                              txChannelMask,
                                    adi_adrv9025_TxAttenMode_e            txAttenMode,
                                    adi_adrv9025_TxAttenSimultUpdateCfg_t simultaneousUpdateCfg,
                                    adi_adrv9025_TxAttenPinCfg_t          gpioModePinCfg)
{
    int32_t                 recoveryAction                   = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr                = ADRV9025_BF_ALL_TX_CHANNELS;
    uint8_t                 k                                = 0;
    uint8_t                 spi2Enable                       = 0;
    uint8_t                 currentTxAttenUpdateGpioEn       = 0;
    uint8_t                 gpioSimultaneousUpdateEn         = 0;
    uint8_t                 spiSimultaneousUpdateEn          = 0;
    uint8_t                 releaseCurrentSimultUpdGpioPin   = 0;
    uint8_t                 acquireNewSimultUpdGpioPin       = 0;
    uint8_t                 releaseCurrentGpioModeIncDecPins = 0;
    uint8_t                 acquireNewGpioModeIncDecPins     = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

#if ADI_ADRV9025_TX_RANGE_CHECK > 0
    recoveryAction = adrv9025_TxAttenModeSetRangeCheck(device,
                                                       txAttenMode,
                                                       simultaneousUpdateCfg,
                                                       gpioModePinCfg);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     txAttenModeCfg,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (k = 0; k < ADI_ADRV9025_MAX_TXCHANNELS; k++)
    {
        if (ADRV9025_BF_EQUAL(txChannelMask,
                              ((uint32_t)1 << (uint32_t)k)))
        {
            if ((txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                                    ((uint32_t)1 << (uint32_t)k))) == 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 txChannel,
                                 "Invalid TxChannel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Read Spi2 status */
            recoveryAction = adi_adrv9025_Spi2CfgGet(device,
                                                     &spi2Enable);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             spi2Enable,
                             "Error while reading Spi2 enabled status");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Read Tx Attenuation GPIO Update Enable Status */
            recoveryAction = adrv9025_TxTxAttenUpdGpioEnBfGet(device,
                                                              txChannelBaseAddr,
                                                              &currentTxAttenUpdateGpioEn);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while attempting to read Tx attenuation update enable");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* If GPIO update mode is being used prior to this function call, we need to release the GPIO pin */
            if (currentTxAttenUpdateGpioEn == ADI_TRUE)
            {
                releaseCurrentSimultUpdGpioPin = 1;
            }

            /* If Tx attenuation mode is GPIO, release current inc/dec pins */
            if (device->devStateInfo.txAttenMode[k] == ADI_ADRV9025_TXATTEN_GPIO_MODE)
            {
                releaseCurrentGpioModeIncDecPins = 1;
            }

            /* New Attenuation mode to be set */
            if (txAttenMode == ADI_ADRV9025_TXATTEN_SPI2_MODE)
            {
                /* Report error if Spi2 is not enabled but user wants to use Spi2 for Tx attenuation control */
                if (spi2Enable == ADI_FALSE)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     spi2Enable,
                                     "Spi2 should be enabled to set Tx Attenation control mode to Spi2");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                /* To set Spi2 as Tx attenuation mode, tx_atten_mode should be set to 1 and simultaneous update should be disabled */
                recoveryAction = adrv9025_TxTxAttenModeBfSet(device,
                                                             txChannelBaseAddr,
                                                             0x01);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 0x01,
                                 "Error while attempting to set control mode");
                ADI_ERROR_RETURN(device->common.error.newAction);

                spiSimultaneousUpdateEn  = 0;
                gpioSimultaneousUpdateEn = 0;
            }
            else
            {
                if (txAttenMode == ADI_ADRV9025_TXATTEN_SPI_MODE)
                {
                    /* Report error if Spi2 is enabled but user wants to use Spi for Tx attenuation control */
                    if (spi2Enable == ADI_TRUE)
                    {
                        ADI_ERROR_REPORT(&device->common,
                                         ADI_COMMON_ERRSRC_API,
                                         ADI_COMMON_ERR_INV_PARAM,
                                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                         spi2Enable,
                                         "Spi2 shouldn't be enabled to set Tx Attenation control mode to Spi");
                        ADI_ERROR_RETURN(device->common.error.newAction);
                    }

                    if (simultaneousUpdateCfg.simultaneousUpdate == ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_SPI)
                    {
                        gpioSimultaneousUpdateEn = 0;
                        spiSimultaneousUpdateEn  = 1;
                    }
                    else if (simultaneousUpdateCfg.simultaneousUpdate == ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_GPIO)
                    {
                        gpioSimultaneousUpdateEn   = 1;
                        spiSimultaneousUpdateEn    = 0;
                        acquireNewSimultUpdGpioPin = 1;
                    }
                }

                else if (txAttenMode == ADI_ADRV9025_TXATTEN_GPIO_MODE)
                {
                    /* If new Tx attenuation mode will be GPIO mode, acquire new inc/dec pins */
                    acquireNewGpioModeIncDecPins = 1;
                }

                recoveryAction = adrv9025_TxTxAttenModeBfSet(device,
                                                             txChannelBaseAddr,
                                                             txAttenMode);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 txAttenMode,
                                 "Error while attempting to set control mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            if (releaseCurrentGpioModeIncDecPins == 1)
            {
                /* First release inc/dec pins */

                /* Release/Acquire Tx attenuation inc/dec GPIO pins */
                recoveryAction = adrv9025_TxAttenPinCtrlCfgSet(device,
                                                               txChannelBaseAddr,
                                                               gpioModePinCfg,
                                                               releaseCurrentGpioModeIncDecPins,
                                                               acquireNewGpioModeIncDecPins);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 "Error while attempting to set GPIO configuration for Tx attenuation control");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Release/Acquire simultaneous update GPIO pin */
                recoveryAction = adrv9025_TxAttenUpdateGpioPinCtrlCfgSet(device,
                                                                         txChannelBaseAddr,
                                                                         simultaneousUpdateCfg.gpioUpdatePin,
                                                                         releaseCurrentSimultUpdGpioPin,
                                                                         acquireNewSimultUpdGpioPin);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 "Error while configuring GPIO for Tx attenuation simultaneous update");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                /* First release simultaneous update pins */

                /* Release/Acquire simultaneous update GPIO pin */
                recoveryAction = adrv9025_TxAttenUpdateGpioPinCtrlCfgSet(device,
                                                                         txChannelBaseAddr,
                                                                         simultaneousUpdateCfg.gpioUpdatePin,
                                                                         releaseCurrentSimultUpdGpioPin,
                                                                         acquireNewSimultUpdGpioPin);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 "Error while configuring GPIO for Tx attenuation simultaneous update");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Release/Acquire Tx attenuation inc/dec GPIO pins */
                recoveryAction = adrv9025_TxAttenPinCtrlCfgSet(device,
                                                               txChannelBaseAddr,
                                                               gpioModePinCfg,
                                                               releaseCurrentGpioModeIncDecPins,
                                                               acquireNewGpioModeIncDecPins);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 "Error while attempting to set GPIO configuration for Tx attenuation control");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Set Tx attenuation Spi simultaneous update enable bit */
            recoveryAction = adrv9025_CoreTxAttenUpdCoreSpiEnBfSet(device,
                                                                   ADRV9025_BF_CORE,
                                                                   spiSimultaneousUpdateEn);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while attempting to set Tx attenuation Spi update enable");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Set Tx attenuation Gpio simultaneous update enable bit */
            recoveryAction = adrv9025_TxTxAttenUpdGpioEnBfSet(device,
                                                              txChannelBaseAddr,
                                                              gpioSimultaneousUpdateEn);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while attempting to set Tx attenuation GPIO update enable");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Update state device info*/
            device->devStateInfo.txAttenMode[k] = txAttenMode;
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenModeGet(adi_adrv9025_Device_t*                 device,
                                    adi_adrv9025_TxChannels_e              txChannel,
                                    adi_adrv9025_TxAttenMode_e*            txAttenMode,
                                    adi_adrv9025_TxAttenSimultUpdateCfg_t* simultaneousUpdateCfg,
                                    adi_adrv9025_TxAttenPinCfg_t*          gpioModePinCfg)
{
    int32_t                 recoveryAction           = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr        = ADRV9025_BF_ALL_TX_CHANNELS;
    uint8_t                 spiSimultaneousUpdateEn  = 0;
    uint8_t                 gpioSimultaneousUpdateEn = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                            txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        *txAttenMode = device->devStateInfo.txAttenMode[0];
        break;
    case ADI_ADRV9025_TX2:
        *txAttenMode = device->devStateInfo.txAttenMode[1];
        break;
    case ADI_ADRV9025_TX3:
        *txAttenMode = device->devStateInfo.txAttenMode[2];
        break;
    case ADI_ADRV9025_TX4:
        *txAttenMode = device->devStateInfo.txAttenMode[3];
        break;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    recoveryAction = adi_adrv9025_TxAttenPinCtrlCfgGet(device,
                                                       txChannel,
                                                       gpioModePinCfg);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read Tx attenuation GPIO mode pin configuration");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adi_adrv9025_TxAttenUpdateGpioPinCtrlCfgGet(device,
                                                                 txChannel,
                                                                 &simultaneousUpdateCfg->gpioUpdatePin);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read Tx attenuation GPIO update pin");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Tx attenuation Spi simultaneous update enable bit */
    recoveryAction = adrv9025_CoreTxAttenUpdCoreSpiEnBfGet(device,
                                                           ADRV9025_BF_CORE,
                                                           &spiSimultaneousUpdateEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to get Tx attenuation Spi update enable");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Tx attenuation Gpio simultaneous update enable bit */
    recoveryAction = adrv9025_TxTxAttenUpdGpioEnBfGet(device,
                                                      txChannelBaseAddr,
                                                      &gpioSimultaneousUpdateEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to get Tx attenuation GPIO update enable");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if ((gpioSimultaneousUpdateEn == ADI_TRUE) && (spiSimultaneousUpdateEn == ADI_FALSE))
    {
        simultaneousUpdateCfg->simultaneousUpdate = ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_GPIO;
    }
    else if ((gpioSimultaneousUpdateEn == ADI_FALSE) && (spiSimultaneousUpdateEn == ADI_TRUE))
    {
        simultaneousUpdateCfg->simultaneousUpdate = ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_SPI;
    }
    else if ((gpioSimultaneousUpdateEn == ADI_FALSE) && (spiSimultaneousUpdateEn == ADI_FALSE))
    {
        simultaneousUpdateCfg->simultaneousUpdate = ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_DISABLED;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Simultaneous update mode");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenSet(adi_adrv9025_Device_t* device,
                                adi_adrv9025_TxAtten_t txAttenuation[],
                                uint8_t                numTxAttenConfigs)
{
    static const uint8_t MINIMUM_NUM_TX_ATTEN_CONFIGS = 1;
    static const uint8_t MAXIMUM_NUM_TX_ATTEN_CONFIGS = 4;

    int32_t                 recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    int8_t                  i                 = 0;
    int8_t                  j                 = 0;
    uint32_t                channelLoop       = 0;
    uint16_t                regData           = 0;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr = ADRV9025_BF_ALL_TX_CHANNELS;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenuation);

    /* Check that numTxAttenConfigs is in valid range - ERROR */
    if (numTxAttenConfigs < MINIMUM_NUM_TX_ATTEN_CONFIGS)
    {
        /* invalid number of configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numTxAttenConfigs,
                         "Invalid number of Tx Attenuation configs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that numTxAttenConfigs is in valid range - WARNING */
    if (numTxAttenConfigs > MAXIMUM_NUM_TX_ATTEN_CONFIGS)
    {
        /* too many configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_WARN_CHECK_PARAM,
                         numTxAttenConfigs,
                         "WARNING: Number of Tx Attenuation configurations is larger than maximum (4) expected. The last configuration included for any channel will be applied to the device.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Loop through the number of configurations and check attenuation mode */
    for (i = 0; i < numTxAttenConfigs; i++)
    {
        if (txAttenuation[i].txAttenuation_mdB > ADRV9025_TX_ATTEN_VALUE_MAX_MDB)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txAttenuation[i].txAttenuation_mdB,
                             "Invalid txAttenuation_mdB value");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        /* Check for a valid txChannel and check attenuation mode */
        channelLoop = txAttenuation[i].txChannelMask;
        for (j = 0; j < ADI_ADRV9025_MAX_TXCHANNELS; j++)
        {
            if (ADRV9025_BF_EQUAL(channelLoop,
                                  ((uint32_t)0x1 << j)))
            {
                if (device->devStateInfo.txAttenMode[j] != ADI_ADRV9025_TXATTEN_SPI_MODE)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_ADRV9025_ERR_INV_TX_ATTEN_MODE,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     device->devStateInfo.txAttenMode[j],
                                     "Invalid Tx attenuation control mode");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    /* Loop through the number of configurations passed */
    for (i = 0; i < numTxAttenConfigs; i++)
    {
        /* Write configuration */
        /* Check for a valid txChannel and perform the Bitfield access*/
        channelLoop = txAttenuation[i].txChannelMask;
        for (j = 0; j < ADI_ADRV9025_MAX_TXCHANNELS; j++)
        {
            if (ADRV9025_BF_EQUAL(channelLoop,
                                  ADI_ADRV9025_TX1))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH0;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX1;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX2))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH1;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX2;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX3))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH2;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX3;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX4))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH3;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX4;
            }
                /* error condition break loop and report error */
            else if (channelLoop == 0)
            {
                /* All channels setup */
                break;
            }
            else
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 txAttenuation[i].txChannelMask,
                                 "Error while accessing selected Tx channel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* set step size depending on tx->txAttenStepSize */
            switch (device->devStateInfo.txAttenStepSize[j])
            {
            case ADI_ADRV9025_TXATTEN_0P05_DB:
                regData = (txAttenuation[i].txAttenuation_mdB / 50);
                break;
            case ADI_ADRV9025_TXATTEN_0P1_DB:
                regData = (txAttenuation[i].txAttenuation_mdB / 100);
                break;
            case ADI_ADRV9025_TXATTEN_0P2_DB:
                regData = (txAttenuation[i].txAttenuation_mdB / 200);
                break;
            case ADI_ADRV9025_TXATTEN_0P4_DB:
                regData = (txAttenuation[i].txAttenuation_mdB / 400);
                break;
            default:
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 device->devStateInfo.txAttenStepSize,
                                 "Invalid Tx step size in structure");
                ADI_ERROR_RETURN(recoveryAction);
            }
            }
            recoveryAction = adrv9025_TxTxAttenuationBfSet(device,
                                                           txChannelBaseAddr,
                                                           regData);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recoveryAction,
                             txAttenuation->txAttenuation_mdB,
                             "Invalid Tx attenuation setting in structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenGet(adi_adrv9025_Device_t*    device,
                                adi_adrv9025_TxChannels_e txChannel,
                                adi_adrv9025_TxAtten_t*   txAttenuation)
{
    int32_t                        recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e        txBfChannel      = ADRV9025_BF_TX_CH0;
    uint16_t                       txAttenReadBack  = 0;
    uint16_t                       attenStepSizeDiv = 50;
    adi_adrv9025_TxAttenStepSize_e attenStepSize    = ADI_ADRV9025_TXATTEN_0P05_DB;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenuation);

    if ((txBfChannel = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                      txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_TxTxAttenuationBfGet(device,
                                                   txBfChannel,
                                                   &txAttenReadBack);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Failure to Read Attenuation");
    ADI_ERROR_RETURN(device->common.error.newAction);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        attenStepSize = device->devStateInfo.txAttenStepSize[0];
        break;
    case ADI_ADRV9025_TX2:
        attenStepSize = device->devStateInfo.txAttenStepSize[1];
        break;
    case ADI_ADRV9025_TX3:
        attenStepSize = device->devStateInfo.txAttenStepSize[2];
        break;
    case ADI_ADRV9025_TX4:
        attenStepSize = device->devStateInfo.txAttenStepSize[3];
        break;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    switch (attenStepSize)
    {
    case ADI_ADRV9025_TXATTEN_0P05_DB:
        attenStepSizeDiv = 50;
        break;
    case ADI_ADRV9025_TXATTEN_0P1_DB:
        attenStepSizeDiv = 100;
        break;
    case ADI_ADRV9025_TXATTEN_0P2_DB:
        attenStepSizeDiv = 200;
        break;
    case ADI_ADRV9025_TXATTEN_0P4_DB:
        attenStepSizeDiv = 400;
        break;
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         attenStepSizeDiv,
                         "Invalid Tx step size in structure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Readback word always reads back with 0.05dB resolution */
    txAttenuation->txAttenuation_mdB = (txAttenReadBack * attenStepSizeDiv);
    txAttenuation->txChannelMask     = txChannel;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_DacFullScaleSet(adi_adrv9025_Device_t*      device,
                                     adi_adrv9025_TxChannels_e   txChannel,
                                     adi_adrv9025_DacFullScale_e dacFullScale)
{
    static const uint8_t DAC_FSC_TUNE_VAL = 0x10;
    int32_t              recoveryAction   = ADI_COMMON_ACT_NO_ACTION;

    adrv9025_BfTxdacMemMapChanAddr_e       txDacChannelBaseAddr = ADRV9025_BF_ALL_TXDAC;
    adrv9025_BfTxdacMemMapTxdacFscBoostI_e boostDacPowerI       = ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_FSC4MA;
    adrv9025_BfTxdacMemMapTxdacFscBoostQ_e boostDacPowerQ       = ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_FSC4MA;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that if init cals has run */
    if (ADRV9025_BF_EQUAL(device->devStateInfo.devState,
                          ADI_ADRV9025_STATE_INITCALS_RUN))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         devState,
                         "Init cals has already run. Cannot change Tx Dac Full scale settings anymore");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that dacFullScale is in range */
    if (dacFullScale > ADI_ADRV9025_TX_DACFS_3DB)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dacFullScale,
                         "Dac Full scale selection is out of range, Valid values are 0dB and 3dB");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txDacChannelBaseAddr = (adrv9025_BfTxdacMemMapChanAddr_e)adrv9025_TxDacAddrDecode(device,
                                                                                           (uint32_t)txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_TxdacMemMapTxdacFscTuneIBfSet(device,
                                                            txDacChannelBaseAddr,
                                                            DAC_FSC_TUNE_VAL);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     DAC_FSC_TUNE_VAL,
                     "Error while setting FSC Tune IDAC full scale current bitfield");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_TxdacMemMapTxdacFscTuneQBfSet(device,
                                                            txDacChannelBaseAddr,
                                                            DAC_FSC_TUNE_VAL);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     DAC_FSC_TUNE_VAL,
                     "Error while setting FSC Tune QDAC full scale current bitfield");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (dacFullScale == ADI_ADRV9025_TX_DACFS_3DB)
    {
        boostDacPowerI = ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_FSC5P6MA;
        boostDacPowerQ = ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_FSC5P6MA;
    }
    else
    {
        boostDacPowerI = ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_FSC4MA;
        boostDacPowerQ = ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_FSC4MA;
    }

    recoveryAction = adrv9025_TxdacMemMapTxdacFscBoostIBfSet(device,
                                                             txDacChannelBaseAddr,
                                                             boostDacPowerI);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     boostDacPowerI,
                     "Error while setting FSC Boost IDAC output power bitfield");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_TxdacMemMapTxdacFscBoostQBfSet(device,
                                                             txDacChannelBaseAddr,
                                                             boostDacPowerQ);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     boostDacPowerQ,
                     "Error while setting FSC Boost QDAC output power bitfield");
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adi_adrv9025_DacFullScaleGet(adi_adrv9025_Device_t*       device,
                                     adi_adrv9025_TxChannels_e    txChannel,
                                     adi_adrv9025_DacFullScale_e* dacFullScale)
{
    int32_t                                recoveryAction       = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxdacMemMapTxdacFscBoostI_e txDacBoostRead       = ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_FSC4MA;
    adrv9025_BfTxdacMemMapChanAddr_e       txDacChannelBaseAddr = ADRV9025_BF_ALL_TXDAC;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        dacFullScale);

    if ((txDacChannelBaseAddr = (adrv9025_BfTxdacMemMapChanAddr_e)adrv9025_TxDacAddrDecode(device,
                                                                                           txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* DAC full scale setting */
    recoveryAction = adrv9025_TxdacMemMapTxdacFscBoostIBfGet(device,
                                                             txDacChannelBaseAddr,
                                                             &txDacBoostRead);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to set Dac fulls scale");
    ADI_ERROR_RETURN(device->common.error.newAction);

    *dacFullScale = (adi_adrv9025_DacFullScale_e)txDacBoostRead;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxTestToneSet(adi_adrv9025_Device_t*       device,
                                   adi_adrv9025_TxTestToneCfg_t txNcoTestToneCfg[],
                                   uint8_t                      toneCfgs)
{
    int32_t                 recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    int8_t                  i                   = 0;
    int8_t                  j                   = 0;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr   = ADRV9025_BF_ALL_TX_CHANNELS;
    uint32_t                channelLoop         = 0;
    uint32_t                tuneWord            = 0;
    int64_t                 txSampleRateDiv2_Hz = 245760000; // TODO: get profiles IQ rate from device data structure

    ADI_NULL_DEVICE_PTR_RETURN(device);

    //    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);
    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                    ADI_COMMON_LOG_API,
                                    "%s(0x%04X, 0x%04X, 0x%02X)",
                                    *device,
                                    txNcoTestToneCfg,
                                    toneCfgs);

    ADI_NULL_PTR_RETURN(&device->common,
                        txNcoTestToneCfg);

    /* check configs is > 0 and <= 4*/
    if (toneCfgs == 0 || toneCfgs > 4)
    {
        /* no valid configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         attenCfgs,
                         "Invalid number of configs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Loop through the number of configurations passed */
    for (i = 0; i < toneCfgs; i++)
    {
#if ADI_ADRV9025_TX_RANGE_CHECK > 0
        recoveryAction = adrv9025_TxTestToneCfgSetRangeCheck(device,
                                                             &txNcoTestToneCfg[i]);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         txAttenCfg[i],
                         "Invalid configuration found");
        ADI_ERROR_RETURN(device->common.error.newAction);
#endif

        /* Write configuration */
        channelLoop = txNcoTestToneCfg[i].txChannelMask;
        for (j = 0; j < ADI_ADRV9025_MAX_TXCHANNELS; j++)
        {
            if (ADRV9025_BF_EQUAL(channelLoop,
                                  ADI_ADRV9025_TX1))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH0;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX1;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX2))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH1;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX2;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX3))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH2;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX3;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX4))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH3;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX4;
            }
                /* error condition break loop and report error */
            else if (channelLoop == 0)
            {
                /* All channels setup */
                break;
            }
            else
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 txNcoTestToneCfg[i].txChannelMask,
                                 "Error while accessing selected Tx channel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            txSampleRateDiv2_Hz = (((int64_t)device->devStateInfo.txInputRate_kHz[j]) * 1000) >> 1;

            if ((txNcoTestToneCfg[i].txToneFreq_Hz > txSampleRateDiv2_Hz) ||
                (txNcoTestToneCfg[i].txToneFreq_Hz < -(txSampleRateDiv2_Hz)))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 txNcoTestToneCfg[i].txToneFreq_Hz,
                                 "Tx Test Tone Frequency is too large for the Tx Profile's sample rate");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            tuneWord = (uint32_t)((no_os_div_s64(((int64_t)((0 - txNcoTestToneCfg[i].txToneFreq_Hz)) << 32), txSampleRateDiv2_Hz) + 1) >> 1);
            /* +1 >>1 is round to nearest */

            /* Configure part */
            /* Disable Tx NCO complex multiplier */
            recoveryAction = adrv9025_TxTxDpNcoEnableBfSet(device,
                                                           txChannelBaseAddr,
                                                           0);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             txNcoTestToneCfg[i].enable,
                             "Error while attempting to disable the Tx Nco complex multipler");
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_TxTxNcoGainBfSet(device,
                                                       txChannelBaseAddr,
                                                       txNcoTestToneCfg[i].txToneGain);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             txNcoTestToneCfg[i].txToneGain,
                             "Error while attempting to set the Nco tone gain");
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_TxTxDpNcoFtwBfSet(device,
                                                        txChannelBaseAddr,
                                                        tuneWord);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             txNcoTestToneCfg[i].txToneFreq_Hz,
                             "Error while attempting to set the Nco tone frequency");
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_TxTxDpNcoFtwUpdateBfSet(device,
                                                              txChannelBaseAddr,
                                                              ADI_ON);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             txNcoTestToneCfg[i].txToneFreq_Hz,
                             "Error while attempting to update the Nco tone frequency");
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_TxNcoTestEnableBfSet(device,
                                                           txChannelBaseAddr,
                                                           (txNcoTestToneCfg[i].enable > 0) ? 1 : 0);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             txNcoTestToneCfg[i].enable,
                             "Error while attempting to set the Nco test tone enable");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxTestToneGet(adi_adrv9025_Device_t*        device,
                                   adi_adrv9025_TxChannels_e     txChannel,
                                   adi_adrv9025_TxTestToneCfg_t* txNcoTestToneCfg)
{
    int32_t                 recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr   = ADRV9025_BF_ALL_TX_CHANNELS;
    uint8_t                 enable              = 0;
    uint8_t                 txToneGain          = 0;
    uint32_t                tuneWord            = 0;
    int64_t                 txSampleRateDiv2_Hz = 245760000; // TODO: get profiles IQ rate from device data structure

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txNcoTestToneCfg);

    if ((txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                            txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Configure part */
    recoveryAction = adrv9025_TxNcoTestEnableBfGet(device,
                                                   txChannelBaseAddr,
                                                   &enable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to get the Nco test tone enable");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_TxTxNcoGainBfGet(device,
                                               txChannelBaseAddr,
                                               &txToneGain);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to get the Nco tone gain");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_TxTxDpNcoFtwBfGet(device,
                                                txChannelBaseAddr,
                                                &tuneWord);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to get the Nco tone frequency");
    ADI_ERROR_RETURN(device->common.error.newAction);

    txSampleRateDiv2_Hz = (((int64_t)device->devStateInfo.txInputRate_kHz[0]) * 1000) >> 1;

    txNcoTestToneCfg->txChannelMask = txChannel;
    txNcoTestToneCfg->enable        = enable;
    txNcoTestToneCfg->txToneGain    = (adi_adrv9025_TxNcoGain_e)txToneGain;
    txNcoTestToneCfg->txToneFreq_Hz = (int32_t)(((int64_t)(0 - (((((tuneWord | 0xFFFFFFFF00000000) << 1) - 1) * txSampleRateDiv2_Hz) >> 32))));

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenPinCtrlCfgSet(adi_adrv9025_Device_t*       device,
                                          adi_adrv9025_TxAttenPinCfg_t txAttenPinCfg[],
                                          uint8_t                      numTxAttenPinConfigs)
{
    int32_t                 recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 i                 = 0;
    uint8_t                 j                 = 0;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr = ADRV9025_BF_ALL_TX_CHANNELS;
    uint32_t                channelLoop       = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenPinCfg);

    /* check configs is > 0 */
    if (numTxAttenPinConfigs == 0)
    {
        /* no valid configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numTxAttenPinConfigs,
                         "Invalid number of configs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Loop through the number of configurations passed */
    for (i = 0; i < numTxAttenPinConfigs; i++)
    {
        channelLoop = txAttenPinCfg[i].txChannelMask;

        for (j = 0; j < ADI_ADRV9025_MAX_TXCHANNELS; j++)
        {
            if (ADRV9025_BF_EQUAL(channelLoop, ((uint32_t)1 << (uint32_t)j)))
            {
                if (device->devStateInfo.txAttenMode[j] == ADI_ADRV9025_TXATTEN_GPIO_MODE)
                {
                    txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device, ((uint32_t)1 << (uint32_t)j));
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    /* Device is already at GPIO mode, so release old pins and acquire new pins */
                    recoveryAction = adrv9025_TxAttenPinCtrlCfgSet(device,
                                                                   txChannelBaseAddr,
                                                                   txAttenPinCfg[i],
                                                                   ADI_TRUE,
                                                                   ADI_TRUE);
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     recoveryAction,
                                     resourceAcquisitionStatus,
                                     "Error while attempting to acquire GPIO shared resouce for GPIO Attenuation ctrl Increment/Decrement Pins");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
                else
                {
                    /* GPIO Tx attenuation control pins can only be changed when Tx attenuation mode is GPIO */
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     device->devStateInfo.txAttenMode[j],
                                     "GPIO Tx attenuation control pins can only be changed when Tx attenuation mode is GPIO");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenPinCtrlCfgGet(adi_adrv9025_Device_t*        device,
                                          adi_adrv9025_TxChannels_e     txChannel,
                                          adi_adrv9025_TxAttenPinCfg_t* txAttenPinCfg)
{
    int32_t                 recoveryAction           = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 stepRead                 = 0;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr        = ADRV9025_BF_ALL_TX_CHANNELS;
    uint8_t                 currentTxAttenCtrlIncPin = 0;
    uint8_t                 currentTxAttenCtrlDecPin = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenPinCfg);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        txChannelBaseAddr = ADRV9025_BF_TX_CH0;
        break;
    case ADI_ADRV9025_TX2:
        txChannelBaseAddr = ADRV9025_BF_TX_CH1;
        break;
    case ADI_ADRV9025_TX3:
        txChannelBaseAddr = ADRV9025_BF_TX_CH2;
        break;
    case ADI_ADRV9025_TX4:
        txChannelBaseAddr = ADRV9025_BF_TX_CH3;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenPinCfg[i].txChannelMask,
                         "Error while accessing selected Tx channel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

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

    /* Get GPIO Attenuation step size */
    recoveryAction = adrv9025_TxTxIncrDecrWordBfGet(device,
                                                    txChannelBaseAddr,
                                                    &stepRead);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to get GPIO Attenaution step size");
    ADI_ERROR_RETURN(device->common.error.newAction);

    txAttenPinCfg->stepSize      = stepRead;
    txAttenPinCfg->txAttenDecPin = (adi_adrv9025_GpioPinSel_e)currentTxAttenCtrlDecPin;
    txAttenPinCfg->txAttenIncPin = (adi_adrv9025_GpioPinSel_e)currentTxAttenCtrlIncPin;
    txAttenPinCfg->txChannelMask = txChannel;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenUpdateGpioPinCtrlCfgSet(adi_adrv9025_Device_t*    device,
                                                    uint32_t                  txChannelMask,
                                                    adi_adrv9025_GpioPinSel_e gpioSelection)
{
    static const uint32_t ALL_TX_CHANNELS = (uint32_t)(ADI_ADRV9025_TXALL);

    int32_t                 recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr      = ADRV9025_BF_ALL_TX_CHANNELS;
    uint8_t                 i                      = 0;
    uint8_t                 currentTxAttenUpdateEn = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((txChannelMask == 0) || ((txChannelMask & (~ALL_TX_CHANNELS)) != 0))
    {
        /* Tx channel selection is not valid */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelMask,
                         "Invalid Tx channel selection, any combination of Tx1/2/3/4 channels are valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < ADI_ADRV9025_MAX_TXCHANNELS; i++)
    {
        if (ADRV9025_BF_EQUAL(txChannelMask,
                              ((uint32_t)1 << (uint32_t)i)))
        {
            txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                               ((uint32_t)1 << (uint32_t)i));
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Read Tx Attenuation GPIO Update Enable Status */
            recoveryAction = adrv9025_TxTxAttenUpdGpioEnBfGet(device,
                                                              txChannelBaseAddr,
                                                              &currentTxAttenUpdateEn);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while attempting to read Tx attenuation update enable");
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (currentTxAttenUpdateEn == ADI_TRUE)
            {
                /* We're actively using Tx attenuation with GPIO feature, release the current pin and acquire the new pin */
                recoveryAction = adrv9025_TxAttenUpdateGpioPinCtrlCfgSet(device,
                                                                         txChannelBaseAddr,
                                                                         gpioSelection,
                                                                         ADI_TRUE,
                                                                         ADI_TRUE);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 device->common.error.errormessage);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                /* GPIO Tx attenuation simultaneous update is not enabled for selected Tx channel */
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 currentTxAttenUpdateEn,
                                 "GPIO simultaneous update is not enabled for selected Tx channel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenUpdateGpioPinCtrlCfgGet(adi_adrv9025_Device_t*     device,
                                                    adi_adrv9025_TxChannels_e  txChannelSelect,
                                                    adi_adrv9025_GpioPinSel_e* gpioSelection)
{
    int32_t                 recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr = ADRV9025_BF_ALL_TX_CHANNELS;
    uint8_t                 fieldVal          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        gpioSelection);

    if ((txChannelSelect != ADI_ADRV9025_TX1) && (txChannelSelect != ADI_ADRV9025_TX2) &&
        (txChannelSelect != ADI_ADRV9025_TX3) && (txChannelSelect != ADI_ADRV9025_TX4))
    {
        /* Tx channel selection is not valid */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelSelect,
                         "Invalid Tx channel selection, Tx1/2/3/4 channels are valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                       (uint32_t)txChannelSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Get Tx attenuation Update GPIO select */
    recoveryAction = adrv9025_TxTxAttenUpdGpioSelectBfGet(device,
                                                          txChannelBaseAddr,
                                                          &fieldVal);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     fieldVal,
                     "Error while attempting to read GPIO Tx attenuation update pin");
    ADI_ERROR_RETURN(device->common.error.newAction);

    *gpioSelection = (adi_adrv9025_GpioPinSel_e)fieldVal;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenUpdate(adi_adrv9025_Device_t* device,
                                   uint32_t               txChannelMask)
{
    static const uint32_t ALL_TX_CHANNELS = (uint32_t)ADI_ADRV9025_TX1 | (uint32_t)ADI_ADRV9025_TX2 |
    (uint32_t)ADI_ADRV9025_TX3 | (uint32_t)ADI_ADRV9025_TX4;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t i              = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((txChannelMask == 0) || ((txChannelMask & (~ALL_TX_CHANNELS)) != 0))
    {
        /* Tx channel selection is not valid */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelSelect,
                         "Invalid Tx channel selection, any combination of Tx1/2/3/4 channels are valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < ADI_ADRV9025_MAX_TXCHANNELS; i++)
    {
        if (ADRV9025_BF_EQUAL(txChannelMask,
                              ((uint32_t)1 << (uint32_t)i)))
        {
            if (device->devStateInfo.txAttenMode[i] != ADI_ADRV9025_TXATTEN_SPI_MODE)
            {
                /* Selected Tx channel is not at SPI mode */
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 txAttenMode,
                                 "Selected Tx channel for attenuation update is not working at SPI mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    /* First clear the field, it's not autoclear */
    recoveryAction = adrv9025_CoreTxAttenUpdCoreSpiBfSet(device,
                                                         ADRV9025_BF_CORE,
                                                         0x00);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     0x00,
                     "Error while attempting to clear Tx attenuation update spi register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Write Tx attenuation update register to trigger Tx attenuation update*/
    recoveryAction = adrv9025_CoreTxAttenUpdCoreSpiBfSet(device,
                                                         ADRV9025_BF_CORE,
                                                         (uint8_t)txChannelMask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     0x00,
                     "Error while attempting to write Tx attenuation update spi register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxFirRead(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_TxChannels_e txChannel,
                               int8_t*                   gain_dB,
                               uint8_t*                  numFirCoefs,
                               int16_t                   coefs[],
                               uint8_t                   arraySize)
{
    uint8_t              i             = 0;
    uint8_t              j             = 0;
    uint8_t              numTapsReg    = 0;
    uint8_t              nTapMul       = 0;
    uint8_t              filterGain    = 0;
    uint8_t              pfirCoeffCtl  = 0;
    uint16_t             baseaddr      = 0;
    uint8_t              byteLo        = 0;
    uint8_t              byteHi        = 0;
    uint8_t              filterConfig  = 0;
    static const uint8_t autoInc       = ADI_ADRV9025_PFIR_COEFF_AUTO_INCR;
    static const uint8_t TXFIR_MULTAPS = 20;
    static const uint8_t TXFIR_MAXTAPS = 80;

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

    if (adrv9025_TxAddrDecode(device,
                              txChannel) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (arraySize == 0 || arraySize > TXFIR_MAXTAPS)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid arraySize. Number of coefficients should be between 1 and 80.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    nTapMul = TXFIR_MULTAPS;

    pfirCoeffCtl |= ADRV9025_PFIR_COEFF_PROG_CLK_EN;
    if (autoInc != 0)
    {
        pfirCoeffCtl |= ADRV9025_PFIR_COEFF_AUTO_INCR;
    }

    baseaddr = adrv9025_TxAddrDecode(device,
                                     txChannel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    filterConfig = 0;
    ADRV9025_SPIREADBYTE("TX_FILTER_CONFIG",
                         baseaddr + ADRV9025_ADDR_TX_FILTER_CONFIG_OFFSET,
                         &filterConfig);

    filterGain = ADRV9025_BF_DECODE(filterConfig,
                                    ADRV9025_TX_PFIR_GAIN_MASK,
                                    ADRV9025_TX_PFIR_GAIN_SHIFT);
    numTapsReg = ADRV9025_BF_DECODE(filterConfig,
                                    ADRV9025_TX_PFIR_TAPS_MASK,
                                    ADRV9025_TX_PFIR_TAPS_SHIFT);
    *numFirCoefs = (numTapsReg + 1) * nTapMul;

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

    ADRV9025_SPIWRITEBYTE("TX_PFIR_COEFF_CTL",
                          (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_CTL_OFFSET),
                          pfirCoeffCtl);

    //pfir_coeff_addr
    j = 0;
    ADRV9025_SPIWRITEBYTE("TX_PFIR_COEFF_ADDR",
                          (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_ADDR_OFFSET),
                          j);

    /* Read up to arraySize */
    if (arraySize > *numFirCoefs)
    {
        arraySize = *numFirCoefs;
    }

    for (i = 0; i < arraySize; i++)
    {
        byteLo = 0;
        byteHi = 0;
        ADRV9025_SPIREADBYTE("TX_PFIR_COEFF_DATA_LSB",
                             (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_DATA_OFFSET),
                             &byteLo);
        if (autoInc == 0)
        {
            j++;
            ADRV9025_SPIWRITEBYTE("TX_PFIR_COEFF_ADDR",
                                  (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_ADDR_OFFSET),
                                  j);
        }

        ADRV9025_SPIREADBYTE("TX_PFIR_COEFF_DATA_MSB",
                             (baseaddr + ADRV9025_ADDR_TX_PFIR_COEFF_DATA_OFFSET),
                             &byteHi);
        coefs[i] = (int16_t)((((uint16_t)byteHi) << 8) | byteLo);

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

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TxFirWrite(adi_adrv9025_Device_t* device,
                                uint32_t               txChannelMask,
                                int8_t                 gain_dB,
                                uint8_t                numFirCoefs,
                                int16_t                coefs[],
                                uint8_t                arraySize)
{
    uint8_t  j                                        = 0;
    uint32_t maskBit                                  = 0;
    uint32_t txFirLookup[ADI_ADRV9025_MAX_TXCHANNELS] = {ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TX3, ADI_ADRV9025_TX4};

    static const uint32_t VALID_TXCHANMASK = 0x0000000F;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        coefs);

    ADRV9025_BUGINFO(__FUNCTION__);

    if ((txChannelMask & VALID_TXCHANMASK) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelMask,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Program FIR filters */
    for (j = 0; j < ADI_ADRV9025_MAX_TXCHANNELS; j++)
    {
        maskBit = txFirLookup[j];
        if (ADRV9025_BF_EQUAL(txChannelMask,
                              maskBit))
        {
            adrv9025_TxFirWrite(device,
                                maskBit,
                                gain_dB,
                                numFirCoefs,
                                coefs,
                                arraySize);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TxAttenTableWrite(adi_adrv9025_Device_t*         device,
                                       uint32_t                       txChannelMask,
                                       uint32_t                       txAttenIndexOffset,
                                       adi_adrv9025_TxAttenTableRow_t txAttenTableRows[],
                                       uint32_t                       numTxAttenEntries)
{
    int32_t              recoveryAction                                      = ADI_COMMON_ACT_NO_ACTION;
    uint32_t             i                                                   = 0;
    uint32_t             j                                                   = 0;
    uint32_t             baseAddr                                            = 0;
    uint32_t             maskBit                                             = 0;
    uint8_t              cfgData[ADI_ADRV9025_TX_ATTEN_TABLE_CACHE_MAX + 10] = {0};
    uint32_t             start                                               = 0;
    uint32_t             stop                                                = 0;
    uint32_t             data                                                = 0;
    uint32_t             offset                                              = 0;
    static const uint8_t TX_ENTRY_SIZE                                       = 4;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenTableRows);

    ADRV9025_BUGINFO(__FUNCTION__);

#if ADI_ADRV9025_TX_RANGE_CHECK > 0
    adrv9025_TxAttenTableWriteRangeCheck(device,
                                         txChannelMask,
                                         txAttenIndexOffset,
                                         txAttenTableRows,
                                         numTxAttenEntries);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (j = 0; j < ADI_ADRV9025_MAX_TXCHANNELS; j++)
    {
        maskBit = (1UL << j);
        if (ADRV9025_BF_EQUAL(txChannelMask,
                              maskBit))
        {
            baseAddr = adrv9025_txAttenAddrLookup(device,
                                                  maskBit);

            offset = (txAttenIndexOffset * TX_ENTRY_SIZE) + baseAddr;
            start  = 0;
            stop   = 0;
            for (i = 0; i < numTxAttenEntries; i++)
            {
                data = ADRV9025_BF_ENCODE((uint32_t)(txAttenTableRows[i].txAttenHp),
                                          ADRV9025_TX_ATTEN_TABLE_HP_MASK,
                                          ADRV9025_TX_ATTEN_TABLE_HP_SHIFT);
                data |= ADRV9025_BF_ENCODE((uint32_t)(txAttenTableRows[i].txAttenMult),
                                           ADRV9025_TX_ATTEN_TABLE_MULT_MASK,
                                           ADRV9025_TX_ATTEN_TABLE_MULT_SHIFT);
                cfgData[start]     = (uint8_t)(data & 0x000000FF);
                cfgData[start + 1] = (uint8_t)((data >> 8) & 0x000000FF);
                cfgData[start + 2] = (uint8_t)((data >> 16) & 0x000000FF);
                cfgData[start + 3] = (uint8_t)((data >> 24) & 0x000000FF);

                start += TX_ENTRY_SIZE;
                if (start >= ADI_ADRV9025_TX_ATTEN_TABLE_CACHE_MAX)
                {
                    adrv9025_CpuDmaMemWrite(device,
                                            offset + stop,
                                            &cfgData[0],
                                            start,
                                            ADI_ADRV9025_CPU_MEM_AUTO_INCR);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                    stop += start;
                    start = 0;
                }
            }

            if (start > 0)
            {
                recoveryAction = adrv9025_CpuDmaMemWrite(device,
                                                         offset + stop,
                                                         &cfgData[0],
                                                         start,
                                                         ADI_ADRV9025_CPU_MEM_AUTO_INCR);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 NULL,
                                 "Error from DmaMemWrite");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TxAttenTableRead(adi_adrv9025_Device_t*         device,
                                      adi_adrv9025_TxChannels_e      txChannel,
                                      uint32_t                       txAttenIndexOffset,
                                      adi_adrv9025_TxAttenTableRow_t txAttenTableRows[],
                                      uint32_t                       numTxAttenEntries)
{
    int32_t  recoveryAction                                      = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i                                                   = 0;
    uint32_t baseAddr                                            = 0;
    uint8_t  cfgData[ADI_ADRV9025_TX_ATTEN_TABLE_CACHE_MAX + 10] = {0};
    uint32_t start                                               = 0;
    uint32_t stop                                                = 0;
    uint32_t data                                                = 0;
    uint32_t offset;
    uint32_t numTxAttenEntriesRead = 0;
    uint32_t maxAttenIndex         = 0;

    static const uint8_t TX_ENTRY_SIZE = 4;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenTableRows);

    ADRV9025_BUGINFO(__FUNCTION__);

    maxAttenIndex = ADRV9025_TX_ATTEN_TABLE_MAX;

    if (adrv9025_txAttenAddrLookup(device,
                                   txChannel) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannelMask");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((numTxAttenEntries == 0) || (numTxAttenEntries > maxAttenIndex))
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
                         txAttenIndexOffset,
                         "Invalid txAttenIndexOffset + numTxAttenEntries");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    baseAddr = adrv9025_txAttenAddrLookup(device,
                                          txChannel);

    start  = 0;
    stop   = 0;
    offset = (txAttenIndexOffset * TX_ENTRY_SIZE) + baseAddr;

    numTxAttenEntriesRead = numTxAttenEntries * TX_ENTRY_SIZE;
    if (numTxAttenEntriesRead > ADI_ADRV9025_TX_ATTEN_TABLE_CACHE_MAX)
    {
        numTxAttenEntriesRead = ADI_ADRV9025_TX_ATTEN_TABLE_CACHE_MAX;
    }

    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            offset + stop,
                                            (uint8_t *)&cfgData[0],
                                            numTxAttenEntriesRead,
                                            0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error from DmaMemRead");
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < numTxAttenEntries; i++)
    {
        data = (((uint32_t)cfgData[start + 3] << 24) | ((uint32_t)cfgData[start + 2] << 16) |
            ((uint32_t)cfgData[start + 1] << 8) | (uint32_t)cfgData[start + 0]);

        txAttenTableRows[i].txAttenHp = ADRV9025_BF_DECODE(data,
                                                           ADRV9025_TX_ATTEN_TABLE_HP_MASK,
                                                           ADRV9025_TX_ATTEN_TABLE_HP_SHIFT);
        txAttenTableRows[i].txAttenMult = ADRV9025_BF_DECODE(data,
                                                             ADRV9025_TX_ATTEN_TABLE_MULT_MASK,
                                                             ADRV9025_TX_ATTEN_TABLE_MULT_SHIFT);

        start += TX_ENTRY_SIZE;

        if ((start >= ADI_ADRV9025_TX_ATTEN_TABLE_CACHE_MAX) &&
            ((i + 1) < numTxAttenEntries))
        {
            numTxAttenEntriesRead = (numTxAttenEntries - (i + 1)) * TX_ENTRY_SIZE;
            if (numTxAttenEntriesRead > ADI_ADRV9025_TX_ATTEN_TABLE_CACHE_MAX)
            {
                numTxAttenEntriesRead = ADI_ADRV9025_TX_ATTEN_TABLE_CACHE_MAX;
            }

            stop += start;
            adrv9025_CpuDmaMemRead(device,
                                   offset + stop,
                                   &cfgData[0],
                                   numTxAttenEntriesRead,
                                   0);
            ADI_ERROR_RETURN(device->common.error.newAction);
            start = 0;
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_TxPaProtectionCfgSet(adi_adrv9025_Device_t*        device,
                                          adi_adrv9025_TxPaProtectCfg_t txPaProtectCfg[],
                                          uint8_t                       arraySize)
{
    int32_t                   recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    int8_t                    i              = 0;
    int8_t                    j              = 0;
    uint32_t                  channelLoop    = 0;
    adi_adrv9025_TxChannels_e txChannel      = ADI_ADRV9025_TXALL;
    adrv9025_BfTxChanAddr_e   baseAddress    = ADRV9025_BF_ALL_TX_CHANNELS;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txPaProtectCfg);

    /* check configs is > 0 and <= 4 */
    if (arraySize == 0 || arraySize > 4)
    {
        /* no valid configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid number of channels");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < arraySize; i++)
    {
        /* range check for all the values*/
        if (adrv9025_TxPaProtectCfgRangeCheck(device,
                                              &txPaProtectCfg[i]) != 0)
        {
            /* no valid configs */
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             arraySize,
                             device->common.error.errormessage);

            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            channelLoop = txPaProtectCfg[i].txChannel;

            for (j = 0; j < ADI_ADRV9025_MAX_TXCHANNELS; j++)
            {
                if (ADRV9025_BF_EQUAL(channelLoop,
                                      ADI_ADRV9025_TX1))
                {
                    txChannel   = ADI_ADRV9025_TX1;
                    baseAddress = ADRV9025_BF_TX_CH0;
                    channelLoop = channelLoop & ~ADI_ADRV9025_TX1;
                }
                else if (ADRV9025_BF_EQUAL(channelLoop,
                                           ADI_ADRV9025_TX2))
                {
                    txChannel   = ADI_ADRV9025_TX2;
                    baseAddress = ADRV9025_BF_TX_CH1;
                    channelLoop = channelLoop & ~ADI_ADRV9025_TX2;
                }
                else if (ADRV9025_BF_EQUAL(channelLoop,
                                           ADI_ADRV9025_TX3))
                {
                    txChannel   = ADI_ADRV9025_TX3;
                    baseAddress = ADRV9025_BF_TX_CH2;
                    channelLoop = channelLoop & ~ADI_ADRV9025_TX3;
                }
                else if (ADRV9025_BF_EQUAL(channelLoop,
                                           ADI_ADRV9025_TX4))
                {
                    txChannel   = ADI_ADRV9025_TX4;
                    baseAddress = ADRV9025_BF_TX_CH3;
                    channelLoop = channelLoop & ~ADI_ADRV9025_TX4;
                }
                    /* error condition break loop and report error */
                else if (channelLoop == 0)
                {
                    /* All channels setup */
                    break;
                }
                    /* error condition break loop and report error */
                else
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     txAttenPinCfg[i].txChannelMask,
                                     "Error while accessing selected Tx channel. Only Tx1-Tx2-Tx3-Tx4 channels are supported");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                /* Set average duration */
                recoveryAction = adrv9025_TxPaProtectionAvrgDurBfSet(device,
                                                                     baseAddress,
                                                                     txPaProtectCfg[i].avgDuration);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg[i].avgDuration,
                                 "Error while setting Pa protection average field");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Set peak Duration */
                recoveryAction = adrv9025_TxPaProtectionPeakDurBfSet(device,
                                                                     baseAddress,
                                                                     txPaProtectCfg[i].peakDuration);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg[i].peakDuration,
                                 "Error while setting Pa peakDuration field");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Set Tx Attenuation Step. Gain step down is not allowed. Always assign attenuation step to 0 */
                recoveryAction = adrv9025_TxPaProtectionTxAttenStepBfSet(device,
                                                                         baseAddress,
                                                                         0);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 0,
                                 "Error while setting Pa txAttenStep field");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Set Gain Step Down Enable bit. Gain step down is not allowed. Always assign this bit to 0 */
                recoveryAction = adrv9025_TxPaProtectionGainStepDownEnBfSet(device,
                                                                            baseAddress,
                                                                            0);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg[i].gainStepDownEn,
                                 "Error while setting Pa gainStepDownEn field");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Set Average Power Threshold */
                recoveryAction = adrv9025_TxPaProtectionAvgThresholdBfSet(device,
                                                                          baseAddress,
                                                                          txPaProtectCfg[i].powerThreshold);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg[i].powerThreshold,
                                 "Error while setting Pa powerThreshold field");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Set Peak Count value */
                recoveryAction = adrv9025_TxPaProtectionPeakCountBfSet(device,
                                                                       baseAddress,
                                                                       txPaProtectCfg[i].peakCount);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg[i].peakCount,
                                 "Error while setting Pa peakCount field");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Set Peak Threshold */
                adrv9025_TxPaProtectionPeakThresholdBfSet(device,
                                                          baseAddress,
                                                          txPaProtectCfg[i].peakThreshold);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg[i].peakThreshold,
                                 "Error while setting Pa peakThreshold field");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Set Input Selection */
                adrv9025_TxPaProtectionInputSelBfSet(device,
                                                     baseAddress,
                                                     (uint8_t)txPaProtectCfg[i].inputSel);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg[i].inputSel,
                                 "Error while setting Pa protection input selection");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Set Average Power Ratio Calculation Enable bit */
                adrv9025_TxPaProtectionAprEnBfSet(device,
                                                  baseAddress,
                                                  txPaProtectCfg[i].avgPeakRatioEnable);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg[i].avgPeakRatioEnable,
                                 "Error while setting pa protection average peak power ratio calculation enable field");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Set Average Power Enable bit */
                adrv9025_TxPaProtectionAvgpowerEnBfSet(device,
                                                       baseAddress,
                                                       txPaProtectCfg[i].avgPowerEnable);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg[i].avgPowerEnable,
                                 "Error while setting pa protection average power enable field");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Set Peak Power Enable bit */
                adrv9025_TxPaProtectionPeakpowerEnBfSet(device,
                                                        baseAddress,
                                                        txPaProtectCfg[i].peakPowerEnable);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg[i].peakPowerEnable,
                                 "Error while setting pa protection peak power enable field");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Clear Error Flags */
                adi_adrv9025_TxPaProtectionErrFlagsReset(device,
                                                         txChannel,
                                                         ADI_ADRV9025_TXPA_PROTECT_FLAGS_ALL);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 txPaProtectCfg,
                                 "Error while clearing error flags");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxPaProtectionCfgGet(adi_adrv9025_Device_t*         device,
                                          adi_adrv9025_TxChannels_e      txChannel,
                                          adi_adrv9025_TxPaProtectCfg_t* txPaProtectCfg)
{
    int32_t  recoveryAction     = ADI_COMMON_ACT_NO_ACTION;
    uint16_t baseAddress        = 0;
    uint8_t  avgDuration        = 0; /*!< PA Protection Average Power Measurement Duration. */
    uint8_t  peakDuration       = 0; /*!< PA Protection Peak Power Measurement Duration */
    uint16_t powerThreshold     = 0; /*!< PA Protection Average Power Threshold. */
    uint8_t  peakCount          = 0; /*!< Peak Count Causing PA Error. */
    uint16_t peakThreshold      = 0; /*!< PA Protection Peak Power Threshold. */
    uint8_t  rampStepDuration   = 0; /*!< PA Protection Ramp Step duration. */
    uint8_t  rampStepSize       = 0; /*!< PA Protection Ramp Step Size */
    uint8_t  rampMaxAtten       = 0; /*!< PA Protection Ramp Max Attenuation. */
    uint8_t  avgPowerEnable     = 0; /*!< PA Protection Avg Power Enable */
    uint8_t  peakPowerEnable    = 0; /*!< PA Protection Peak Power Enable */
    uint8_t  inputSel           = 0; /*!< Pa Protection block input selection */
    uint8_t  avgPeakRatioEnable = 0; /*!< Pa Protection Average to Peak Ratio Calculation Enable */

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txPaProtectCfg);

    if ((baseAddress = adrv9025_TxAddrDecode(device,
                                             txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Get average duration for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionAvrgDurBfGet(device,
                                                         (adrv9025_BfTxChanAddr_e)baseAddress,
                                                         &avgDuration);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa protection average field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Peak duration for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionPeakDurBfGet(device,
                                                         (adrv9025_BfTxChanAddr_e)baseAddress,
                                                         &peakDuration);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa peak duration field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Average Power Enable bit */
    recoveryAction = adrv9025_TxPaProtectionAvgpowerEnBfGet(device,
                                                            (adrv9025_BfTxChanAddr_e)baseAddress,
                                                            &avgPowerEnable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     avgPowerEnable,
                     "Error while reading pa protection average power enable field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Peak Power Enable bit */
    recoveryAction = adrv9025_TxPaProtectionPeakpowerEnBfGet(device,
                                                             (adrv9025_BfTxChanAddr_e)baseAddress,
                                                             &peakPowerEnable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     peakPowerEnable,
                     "Error while reading pa protection peak power enable field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Average Power Threshold for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionAvgThresholdBfGet(device,
                                                              (adrv9025_BfTxChanAddr_e)baseAddress,
                                                              &powerThreshold);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa average threshold field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Peak Count for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionPeakCountBfGet(device,
                                                           (adrv9025_BfTxChanAddr_e)baseAddress,
                                                           &peakCount);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa peak count field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Average Power Ratio Calculation Enable bit */
    recoveryAction = adrv9025_TxPaProtectionAprEnBfGet(device,
                                                       (adrv9025_BfTxChanAddr_e)baseAddress,
                                                       &avgPeakRatioEnable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     avgPeakRatioEnable,
                     "Error while reading pa protection average peak power ratio calculation enable field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Peak Threshold for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionPeakThresholdBfGet(device,
                                                               (adrv9025_BfTxChanAddr_e)baseAddress,
                                                               &peakThreshold);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa peak threshold field - Silicon B");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Ramp Step Duration for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionRampStepDurationBfGet(device,
                                                                  (adrv9025_BfTxChanAddr_e)baseAddress,
                                                                  &rampStepDuration);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa ramp step duration field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Ramp Step Size for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionRampStepSizeBfGet(device,
                                                              (adrv9025_BfTxChanAddr_e)baseAddress,
                                                              &rampStepSize);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa ramp step size field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Max Ramp attenuation for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionRampMaxAttenuationBfGet(device,
                                                                    (adrv9025_BfTxChanAddr_e)baseAddress,
                                                                    &rampMaxAtten);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa max ramp attenuation field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Input Selection */
    recoveryAction = adrv9025_TxPaProtectionInputSelBfGet(device,
                                                          (adrv9025_BfTxChanAddr_e)baseAddress,
                                                          &inputSel);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     inputSel,
                     "Error while reading Pa protection input selection");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assignment of all the readback values to the structure object*/
    txPaProtectCfg->txChannel          = txChannel;
    txPaProtectCfg->avgDuration        = avgDuration;
    txPaProtectCfg->peakDuration       = peakDuration;
    txPaProtectCfg->txAttenStep        = 0; /* Gain step down is not allowed for Madura device. This field is not being used actively.*/
    txPaProtectCfg->gainStepDownEn     = 0; /* Gain step down is not allowed for Madura device. This field is not being used actively.*/
    txPaProtectCfg->powerThreshold     = powerThreshold;
    txPaProtectCfg->peakCount          = peakCount;
    txPaProtectCfg->peakThreshold      = peakThreshold;
    txPaProtectCfg->rampStepDuration   = rampStepDuration;
    txPaProtectCfg->rampStepSize       = rampStepSize;
    txPaProtectCfg->rampMaxAtten       = rampMaxAtten;
    txPaProtectCfg->avgPowerEnable     = avgPowerEnable;
    txPaProtectCfg->peakPowerEnable    = peakPowerEnable;
    txPaProtectCfg->inputSel           = (adi_adrv9025_PaProtectionInputSel_e)inputSel;
    txPaProtectCfg->avgPeakRatioEnable = avgPeakRatioEnable;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxPaProtectionErrFlagsGet(adi_adrv9025_Device_t*            device,
                                               adi_adrv9025_TxChannels_e         txChannel,
                                               adi_adrv9025_TxPaProtectionErr_t* errorFlags)
{
    static const uint16_t AVERAGE_POWER_ADDRESS = 0x00A3;
    int32_t               recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint16_t              baseAddress           = 0;
    uint8_t               avgPowerErr           = 0;
    uint16_t              powerErr              = 0;
    uint8_t               peakPowerErr          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        errorFlags);

    /* obtain the base address for the Tx Channel */
    if ((baseAddress = adrv9025_TxAddrDecode(device,
                                             txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Write a random byte to average power error to trigger the update, bf function is not available for this byte set */
    recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                               baseAddress + AVERAGE_POWER_ADDRESS,
                                               0x00);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while writing spi byte, average power error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Average Power Err for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionAvgpowerErrorBfGet(device,
                                                               (adrv9025_BfTxChanAddr_e)baseAddress,
                                                               &avgPowerErr);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa error field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Power Err for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionErrorPowerBfGet(device,
                                                            (adrv9025_BfTxChanAddr_e)baseAddress,
                                                            &powerErr);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa error field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get Peak Power Err for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionPeakpowerErrorBfGet(device,
                                                                (adrv9025_BfTxChanAddr_e)baseAddress,
                                                                &peakPowerErr);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa error field - Silicon B");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assignment of the readback values to the structure object */
    errorFlags->avgPowerErr  = avgPowerErr;
    errorFlags->powerErr     = powerErr;
    errorFlags->peakPowerErr = peakPowerErr;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxPaProtectionStatusGet(adi_adrv9025_Device_t*            device,
                                             adi_adrv9025_TxChannels_e         txChannel,
                                             adi_adrv9025_TxPaProtectStatus_t* status)
{
    static uint16_t AVERAGE_POWER_ADDRESS = 0x00A3;
    int32_t         recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint16_t        avgPower              = 0;
    uint16_t        avgPeakRatio          = 0;
    uint16_t        avgErrorPower         = 0;
    uint16_t        baseAddress           = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        status);

    /* obtain the base address for the Tx Channel */
    if ((baseAddress = adrv9025_TxAddrDecode(device,
                                             txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Write a random byte to average power error to trigger the update, bf function is not available for this byte set */
    recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                               baseAddress + AVERAGE_POWER_ADDRESS,
                                               0x00);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while writing spi byte, average power error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get average power status for the tx channel. */
    recoveryAction = adrv9025_TxPaProtectionAveragePowerBfGet(device,
                                                              (adrv9025_BfTxChanAddr_e)baseAddress,
                                                              &avgPower);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa average power status field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get average peak ratio status for the tx channel. */
    recoveryAction = adrv9025_TxPaProtectionAveragePeakRatioBfGet(device,
                                                                  (adrv9025_BfTxChanAddr_e)baseAddress,
                                                                  &avgPeakRatio);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing Pa average peak ratio status field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get error average power for the tx channel */
    recoveryAction = adrv9025_TxPaProtectionErrorPowerBfGet(device,
                                                            (adrv9025_BfTxChanAddr_e)baseAddress,
                                                            &avgErrorPower);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while accessing error average power status field");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assignment of the readback values to the structure. */
    status->avgPower      = avgPower;
    status->avgPeakRatio  = avgPeakRatio;
    status->avgErrorPower = avgErrorPower;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxPaProtectionErrFlagsReset(adi_adrv9025_Device_t*             device,
                                                 adi_adrv9025_TxChannels_e          txChannel,
                                                 adi_adrv9025_TxPaProtectErrFlags_e errorFlags)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint16_t baseAddress    = 0;
    uint8_t  resetValue     = 1;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* obtain the base address for the Tx Channel */
    if ((baseAddress = adrv9025_TxAddrDecode(device,
                                             txChannel)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check whether to clear average power error for the tx channel, if yes reset it */
    if ((errorFlags & ADI_ADRV9025_TXPA_PROTECT_FLAGS_AVG_POWER_ERR) == ADI_ADRV9025_TXPA_PROTECT_FLAGS_AVG_POWER_ERR)
    {
        recoveryAction = adrv9025_TxPaProtectionAvgpowerErrorClearBfSet(device,
                                                                        (adrv9025_BfTxChanAddr_e)baseAddress,
                                                                        resetValue);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         NULL,
                         "Error while clearing average power error field");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check whether to clear peak power error for the tx channel, if yes reset it */
    if ((errorFlags & ADI_ADRV9025_TXPA_PROTECT_FLAGS_PEAK_POWER_ERR) == ADI_ADRV9025_TXPA_PROTECT_FLAGS_PEAK_POWER_ERR)
    {
        recoveryAction = adrv9025_TxPaProtectionPeakpowerErrorClearBfSet(device,
                                                                         (adrv9025_BfTxChanAddr_e)baseAddress,
                                                                         resetValue);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         NULL,
                         "Error while clearing peak power error field - Silicon B");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenSpi2PinCtrlCfgSet(adi_adrv9025_Device_t*           device,
                                              adi_adrv9025_TxAttenSpi2PinCfg_t txAttenSpi2PinCfg[],
                                              uint8_t                          numTxAttenSpi2PinConfigs)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID = ADRV9025_FEATURE_TX_ATTEN_CTRL_PIN;

    int32_t                 recoveryAction                                                              = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 i                                                                           = 0;
    uint8_t                 j                                                                           = 0;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr                                                           = ADRV9025_BF_ALL_TX_CHANNELS;
    int32_t                 sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_CTRL_SPI2_PIN_FEATURE] = {0};
    uint8_t                 resourceAcquisitionStatus                                                   = ADI_FAILURE;
    uint8_t                 resourceReleaseStatus                                                       = ADI_FAILURE;
    uint8_t                 currentTxAttenCtrlSpi2Pin                                                   = 0;
    uint32_t                spi2GpioPinMask                                                             = 0;
    uint32_t                channelLoop                                                                 = 0;

    static const uint32_t SPI2_GPIO_OFFSET = 3;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenSpi2PinCfg);

    /* check configs is > 0 */
    if (numTxAttenSpi2PinConfigs == 0)
    {
        /* no valid configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numTxAttenSpi2PinConfigs,
                         "Invalid number of configs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Loop through the number of configurations passed */
    for (i = 0; i < numTxAttenSpi2PinConfigs; i++)
    {
        if (txAttenSpi2PinCfg[i].txAttenSpi2Pin > ADI_ADRV9025_SPI2_TXATTEN_GPIO18)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txAttenSpi2PinCfg,
                             "Invalid configuration found");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        channelLoop = txAttenSpi2PinCfg[i].txChannelMask;
        for (j = 0; j < ADI_ADRV9025_MAX_TXCHANNELS; j++)
        {
            if (ADRV9025_BF_EQUAL(channelLoop,
                                  ADI_ADRV9025_TX1))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH0;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX1;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX2))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH1;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX2;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX3))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH2;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX3;
            }
            else if (ADRV9025_BF_EQUAL(channelLoop,
                                       ADI_ADRV9025_TX4))
            {
                txChannelBaseAddr = ADRV9025_BF_TX_CH3;
                channelLoop       = channelLoop & ~ADI_ADRV9025_TX4;
            }
                /* error condition break loop and report error */
            else if (channelLoop == 0)
            {
                /* All channels setup */
                break;
            }
                /* error condition break loop and report error */
            else
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 txAttenPinCfg[i].txChannelMask,
                                 "Error while accessing selected Tx channel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /*Read Tx Attenuation Spi2 GPIO select */
            recoveryAction = adrv9025_TxSpi2TxAttenGpioSelectBfGet(device,
                                                                   txChannelBaseAddr,
                                                                   &currentTxAttenCtrlSpi2Pin);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while attempting to read Attenuation Spi2 GPIO select register");
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (currentTxAttenCtrlSpi2Pin > ADI_ADRV9025_SPI2_TXATTEN_DISABLE)
            {
                /* Attempt to Release Attenuation Ctrl Spi2 GPIO since it is a shared resource.
                * This is to ensure that this feature is not accidentally releasing the GPIO
                * which is actively in use by another feature */
                sharedResourceArr[0] = (uint8_t)currentTxAttenCtrlSpi2Pin + SPI2_GPIO_OFFSET;
                recoveryAction       = adrv9025_SharedResourcesRelease(device,
                                                                       ADRV9025_SHARED_RESOURCE_GPIO,
                                                                       &sharedResourceArr[0],
                                                                       ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_CTRL_SPI2_PIN_FEATURE,
                                                                       THIS_FEATURE_ID,
                                                                       &resourceReleaseStatus);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 "Error while attempting to release GPIO shared resouce for GPIO attenuation Spi2 Pins");
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
                spi2GpioPinMask = ((uint32_t)0x00000001 << ((uint8_t)currentTxAttenCtrlSpi2Pin + SPI2_GPIO_OFFSET));
                recoveryAction  = adi_adrv9025_GpioInputDirSet(device,
                                                               (spi2GpioPinMask));
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 recoveryAction,
                                 NULL,
                                 "Error while attempting to set GPIO Spi2 Pin inputs");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /*Set Tx attenuation Ctrl Spi2 Pint select */
                currentTxAttenCtrlSpi2Pin = 0;
                adrv9025_TxSpi2TxAttenGpioSelectBfSet(device,
                                                      txChannelBaseAddr,
                                                      currentTxAttenCtrlSpi2Pin);
                recoveryAction = adrv9025_TxAttenSpi2CtrlPinReportError(device,
                                                                        &sharedResourceArr[0],
                                                                        recoveryAction,
                                                                        "Error while attempting to set GPIO for attenuation Spi2");
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 txAttenPinCfg,
                                 "Error while attempting to set GPIO Tx attenuation Spi2 pin control");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (txAttenSpi2PinCfg[i].txAttenSpi2Pin > ADI_ADRV9025_SPI2_TXATTEN_DISABLE)
            {
                /* Attempt to Acquire Gain Ctrl Increment GPIO since it is a shared resource. This is to ensure that this feature is not accidentally acquiring the GPIO
                which is actively in use by another feature */
                sharedResourceArr[0] = (uint32_t)txAttenSpi2PinCfg[i].txAttenSpi2Pin + SPI2_GPIO_OFFSET;
                recoveryAction       = adrv9025_SharedResourcesAcquire(device,
                                                                       ADRV9025_SHARED_RESOURCE_GPIO,
                                                                       &sharedResourceArr[0],
                                                                       ADI_ADRV9025_NUM_GPIOS_IN_TX_ATTEN_CTRL_SPI2_PIN_FEATURE,
                                                                       THIS_FEATURE_ID,
                                                                       &resourceAcquisitionStatus);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 "Error while attempting to acquire GPIO shared resouce for GPIO Attenuation Spi2 Pins");
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (resourceAcquisitionStatus == ADI_FAILURE)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     resourceAcquisitionStatus,
                                     "Error while attempting to acquire GPIO shared resouce for GPIO Attenuation ctrl Spi2 Pins");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                /* Set ADRV9025 GPIO Directions for Gain ctrl GPIO Spi2 pins*/
                spi2GpioPinMask = ((uint32_t)0x00000001 << ((uint8_t)txAttenSpi2PinCfg[i].txAttenSpi2Pin + SPI2_GPIO_OFFSET));
                adi_adrv9025_GpioInputDirSet(device,
                                             (spi2GpioPinMask));
                recoveryAction = adrv9025_TxAttenSpi2CtrlPinReportError(device,
                                                                        &sharedResourceArr[0],
                                                                        recoveryAction,
                                                                        "Error while attempting to set GPIO direction");
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 txAttenSpi2PinCfg,
                                 "Error while attempting to set GPIO pin control");
                ADI_ERROR_RETURN(device->common.error.newAction);

                /*Set Tx attenuation Ctrl Spi2 GPIO select */
                adrv9025_TxSpi2TxAttenGpioSelectBfSet(device,
                                                      txChannelBaseAddr,
                                                      txAttenSpi2PinCfg[i].txAttenSpi2Pin);
                recoveryAction = adrv9025_TxAttenSpi2CtrlPinReportError(device,
                                                                        &sharedResourceArr[0],
                                                                        recoveryAction,
                                                                        "Error while attempting to set GPIO for attenuation Spi2");
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 txAttenPinCfg,
                                 "Error while attempting to set GPIO Tx attenuation Spi2 pin control");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenSpi2PinCtrlCfgGet(adi_adrv9025_Device_t*            device,
                                              adi_adrv9025_TxChannels_e         txChannel,
                                              adi_adrv9025_TxAttenSpi2PinCfg_t* txAttenSpi2PinCfg)
{
    int32_t                 recoveryAction            = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr         = ADRV9025_BF_ALL_TX_CHANNELS;
    uint8_t                 currentTxAttenSpi2CtrlPin = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        txAttenSpi2PinCfg);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        txChannelBaseAddr = ADRV9025_BF_TX_CH0;
        break;
    case ADI_ADRV9025_TX2:
        txChannelBaseAddr = ADRV9025_BF_TX_CH1;
        break;
    case ADI_ADRV9025_TX3:
        txChannelBaseAddr = ADRV9025_BF_TX_CH2;
        break;
    case ADI_ADRV9025_TX4:
        txChannelBaseAddr = ADRV9025_BF_TX_CH3;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txAttenSpi2PinCfg[i].txChannelMask,
                         "Error while accessing selected Tx channel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Read Tx Attenuation Spi2 GPIO select */
    recoveryAction = adrv9025_TxSpi2TxAttenGpioSelectBfGet(device,
                                                           txChannelBaseAddr,
                                                           &currentTxAttenSpi2CtrlPin);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read Attenuation Spi2 GPIO select register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    txAttenSpi2PinCfg->txAttenSpi2Pin = (adi_adrv9025_Spi2TxAttenGpioSel_e)currentTxAttenSpi2CtrlPin;
    txAttenSpi2PinCfg->txChannelMask  = txChannel;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxLoSourceGet(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_TxChannels_e txChannel,
                                   adi_adrv9025_LoSel_e*     txLoSource)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check Rx data format config pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        txLoSource);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    recoveryAction = adrv9025_TxLoSourceGetRangeCheck(device,
                                                      txChannel,
                                                      txLoSource);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     txChannel,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    recoveryAction = adrv9025_TxLoSourceGet(device,
                                            txChannel,
                                            txLoSource);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     txChannel,
                     "Failure to get Tx Lo source");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PaPllDfrmEventRampDownEnableSet(adi_adrv9025_Device_t* device,
                                                     uint32_t               txChannelMask,
                                                     uint32_t               irqMask,
                                                     uint8_t                enable)
{
    static uint8_t PA_PROTECTION_RAMP_STEP_DURATION   = 0x0A;
    static uint8_t PA_PROTECTION_RAMP_MAX_ATTENUATION = 0x64;
    static uint8_t PA_PROTECTION_RAMP_STEP_SIZE       = 0x02;

    static const uint32_t ALL_TX_MASK = (uint32_t)ADI_ADRV9025_TX1 | (uint32_t)ADI_ADRV9025_TX2 |
    (uint32_t)ADI_ADRV9025_TX3 | (uint32_t)ADI_ADRV9025_TX4;

    static const uint32_t ALL_DFRM_IRQ_MASK = (uint32_t)ADI_ADRV9025_RAMP_DOWN_ON_DFRMIRQ0 | (uint32_t)ADI_ADRV9025_RAMP_DOWN_ON_DFRMIRQ1;

    static const uint32_t ALL_PLL_UNLOCK_MASK = (uint32_t)ADI_ADRV9025_RAMP_DOWN_ON_RF1PLLUNLOCK | (uint32_t)ADI_ADRV9025_RAMP_DOWN_ON_RF2PLLUNLOCK |
    (uint32_t)ADI_ADRV9025_RAMP_DOWN_ON_AUXPLLUNLOCK | (uint32_t)ADI_ADRV9025_RAMP_DOWN_ON_CLKPLLUNLOCK |
    (uint32_t)ADI_ADRV9025_RAMP_DOWN_ON_SERDESPLLUNLOCK;

    static const uint32_t PLL_UNLOCK_MASK_SHIFT = 2;

    int32_t recoveryAction       = ADI_COMMON_ACT_NO_ACTION;
    uint8_t pllSelect            = 0;
    uint8_t dfrmIrqSelect        = 0;
    uint8_t pllReadBackValue     = 0;
    uint8_t dfrmIrqReadBackValue = 0;
    uint8_t k                    = 0;

    adrv9025_BfTxChanAddr_e baseAddr = ADRV9025_BF_ALL_TX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & (~ALL_TX_MASK)) != 0) || txChannelMask == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelMask,
                         "Invalid Tx Channel is selected to enable/disable ramp-down. Valid values are different combinations of TX1-2-3-4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that if requested Irq event valid*/
    if (((irqMask & (~(ALL_DFRM_IRQ_MASK | ALL_PLL_UNLOCK_MASK | (uint32_t)ADI_ADRV9025_RAMP_DOWN_ON_PA_PROT_ERROR))) != 0) || irqMask == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         irqMask,
                         "Invalid Irq event is selected to enable/disable ramp-down. Valid values are different combinations of DFRM IRQ 0-1/RF1-RF2-AUX-CLK-SERDES PLL UNLOCK/PA PROT ERROR.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    enable = (enable == 0) ? ADI_FALSE : ADI_TRUE;

    for (k = 0; k < ADI_ADRV9025_MAX_TXCHANNELS; k++)
    {
        if (ADRV9025_BF_EQUAL(txChannelMask,
                              ((uint32_t)1 << (uint32_t)k)))
        {
            if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                           ((uint32_t)1 << (uint32_t)k))) == 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "Invalid TxChannel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Set Ramp Step Duration */
            recoveryAction = adrv9025_TxPaProtectionRampStepDurationBfSet(device,
                                                                          baseAddr,
                                                                          PA_PROTECTION_RAMP_STEP_DURATION);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recoveryAction,
                             PA_PROTECTION_RAMP_STEP_DURATION,
                             "Error while setting Pa rampStepDuration field");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Set Ramp Step Size */
            recoveryAction = adrv9025_TxPaProtectionRampStepSizeBfSet(device,
                                                                      baseAddr,
                                                                      PA_PROTECTION_RAMP_STEP_SIZE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recoveryAction,
                             PA_PROTECTION_RAMP_STEP_SIZE,
                             "Error while setting Pa rampStepSize field");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Set Max Ramp Attenuation */
            recoveryAction = adrv9025_TxPaProtectionRampMaxAttenuationBfSet(device,
                                                                            baseAddr,
                                                                            PA_PROTECTION_RAMP_MAX_ATTENUATION);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recoveryAction,
                             PA_PROTECTION_RAMP_MAX_ATTENUATION,
                             "Error while setting Pa rampMaxAtten field");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* 1) Pa Protection Errors Ramp Down */
            if (ADRV9025_BF_EQUAL(irqMask,
                                  (uint32_t)ADI_ADRV9025_RAMP_DOWN_ON_PA_PROT_ERROR))
            {
                recoveryAction = adrv9025_TxPaProtectionGainRampDownEnBfSet(device,
                                                                            baseAddr,
                                                                            enable);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 enable,
                                 "Could not enable/disable ramp-down for pa protection errors");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* 2) PLL Unlock and Dfrm Irq Ramp Down */
            if ((irqMask & (ALL_DFRM_IRQ_MASK | ALL_PLL_UNLOCK_MASK)) > 0)
            {
                pllSelect     = (uint8_t)((irqMask & ALL_PLL_UNLOCK_MASK) >> PLL_UNLOCK_MASK_SHIFT);
                dfrmIrqSelect = (uint8_t)(irqMask & ALL_DFRM_IRQ_MASK);

                recoveryAction = adrv9025_TxPllUnlockMaskBfGet(device,
                                                               baseAddr,
                                                               &pllReadBackValue);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 pllReadBackValue,
                                 "Could not read Pll Unlock Mask");
                ADI_ERROR_RETURN(device->common.error.newAction);

                recoveryAction = adrv9025_TxJesdDfrmMaskBfGet(device,
                                                              baseAddr,
                                                              &dfrmIrqReadBackValue);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 dfrmIrqReadBackValue,
                                 "Could not read Dfrm Irq Mask");
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (enable == ADI_FALSE)
                {
                    /* Set selected mask bits to ignore error */
                    pllReadBackValue |= pllSelect;
                    dfrmIrqReadBackValue |= dfrmIrqSelect;
                }
                else
                {
                    /* Clear selected mask bits to enable ramp down */
                    pllReadBackValue &= (~pllSelect);
                    dfrmIrqReadBackValue &= (~dfrmIrqSelect);
                }

                recoveryAction = adrv9025_TxPllUnlockMaskBfSet(device,
                                                               baseAddr,
                                                               pllReadBackValue);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 pllReadBackValue,
                                 "Could not write Pll Unlock Mask");
                ADI_ERROR_RETURN(device->common.error.newAction);

                recoveryAction = adrv9025_TxJesdDfrmMaskBfSet(device,
                                                              baseAddr,
                                                              dfrmIrqReadBackValue);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 dfrmIrqReadBackValue,
                                 "Could not write Dfrm Irq Mask");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PaPllDfrmEventRampDownEnableGet(adi_adrv9025_Device_t*                device,
                                                     adi_adrv9025_TxChannels_e             txChannelSelect,
                                                     adi_adrv9025_PaPllDfrmRampDownEnSel_e irqSelect,
                                                     uint8_t*                              enable)
{
    static const uint32_t   PLL_MASK_SHIFT       = 2;
    int32_t                 recoveryAction       = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 pllReadBackValue     = 0;
    uint8_t                 dfrmIrqReadBackValue = 0;
    uint8_t                 mergedReadBackValue  = 0;
    adrv9025_BfTxChanAddr_e baseAddr             = ADRV9025_BF_ALL_TX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that if enable pointer is not NULL */
    ADI_NULL_PTR_RETURN(&device->common,
                        enable);

    /* Check that if requested Tx Channel is valid */
    if ((txChannelSelect != ADI_ADRV9025_TX1) &&
        (txChannelSelect != ADI_ADRV9025_TX2) &&
        (txChannelSelect != ADI_ADRV9025_TX3) &&
        (txChannelSelect != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelSelect,
                         "Invalid Tx Channel is selected to read ramp-down status. Valid values are TX1-2-3-4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that if requested Irq event is valid*/
    if ((irqSelect != ADI_ADRV9025_RAMP_DOWN_ON_DFRMIRQ0) &&
        (irqSelect != ADI_ADRV9025_RAMP_DOWN_ON_DFRMIRQ1) &&
        (irqSelect != ADI_ADRV9025_RAMP_DOWN_ON_RF1PLLUNLOCK) &&
        (irqSelect != ADI_ADRV9025_RAMP_DOWN_ON_RF2PLLUNLOCK) &&
        (irqSelect != ADI_ADRV9025_RAMP_DOWN_ON_AUXPLLUNLOCK) &&
        (irqSelect != ADI_ADRV9025_RAMP_DOWN_ON_CLKPLLUNLOCK) &&
        (irqSelect != ADI_ADRV9025_RAMP_DOWN_ON_SERDESPLLUNLOCK) &&
        (irqSelect != ADI_ADRV9025_RAMP_DOWN_ON_PA_PROT_ERROR))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         irqSelect,
                         "Invalid Irq event is selected to read ramp-down status. Valid values are DFRM IRQ 0-1/RF1-RF2-AUX-CLK-SERDES PLL UNLOCK/PA-PROT-ERROR.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                   (uint32_t)txChannelSelect)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelSelect,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (irqSelect == ADI_ADRV9025_RAMP_DOWN_ON_PA_PROT_ERROR)
    {
        recoveryAction = adrv9025_TxPaProtectionGainRampDownEnBfGet(device,
                                                                    baseAddr,
                                                                    enable);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         irqSelect,
                         "Could not read ramp-down status for pa protection error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        recoveryAction = adrv9025_TxPllUnlockMaskBfGet(device,
                                                       baseAddr,
                                                       &pllReadBackValue);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         pllReadBackValue,
                         "Could not read Pll Unlock Mask");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_TxJesdDfrmMaskBfGet(device,
                                                      baseAddr,
                                                      &dfrmIrqReadBackValue);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dfrmIrqReadBackValue,
                         "Could not read Dfrm Irq Mask");
        ADI_ERROR_RETURN(device->common.error.newAction);

        mergedReadBackValue = dfrmIrqReadBackValue | (pllReadBackValue << PLL_MASK_SHIFT);
        mergedReadBackValue &= (uint8_t)irqSelect;
        if (mergedReadBackValue > 0)
        {
            *enable = 0;
        }
        else
        {
            *enable = 1;
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PaPllDfrmEventGet(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_TxChannels_e txChannelSelect,
                                       uint8_t*                  eventBits)
{
    int32_t                 recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 mergedReadVal  = 0;
    uint8_t                 readVal        = 0;
    adrv9025_BfTxChanAddr_e baseAddr       = ADRV9025_BF_ALL_TX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that if eventBits pointer is not NULL */
    ADI_NULL_PTR_RETURN(&device->common,
                        eventBits);

    /* Check that if requested Tx Channel is valid */
    if ((txChannelSelect != ADI_ADRV9025_TX1) &&
        (txChannelSelect != ADI_ADRV9025_TX2) &&
        (txChannelSelect != ADI_ADRV9025_TX3) &&
        (txChannelSelect != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelSelect,
                         "Invalid Tx Channel is selected to read pa protection event bits. Valid values are TX1-2-3-4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                   (uint32_t)txChannelSelect)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_TxPllJesdProtEventBfGet(device,
                                                      baseAddr,
                                                      &readVal);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while reading Pll Jesd error field - Silicon B");
    ADI_ERROR_RETURN(device->common.error.newAction);
    mergedReadVal |= readVal;
    mergedReadVal <<= 1;

    recoveryAction = adrv9025_TxPaProtectionPeakpowerErrorBfGet(device,
                                                                baseAddr,
                                                                &readVal);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while reading peak power error field - Silicon B");
    ADI_ERROR_RETURN(device->common.error.newAction);
    mergedReadVal |= readVal;
    mergedReadVal <<= 1;

    recoveryAction = adrv9025_TxPaProtectionAvgpowerErrorBfGet(device,
                                                               baseAddr,
                                                               &readVal);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while reading average power error field");
    ADI_ERROR_RETURN(device->common.error.newAction);
    mergedReadVal |= readVal;

    *eventBits = mergedReadVal;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PaPllDfrmEventClear(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_TxChannels_e txChannelSelect,
                                         uint8_t                   eventBits)
{
    static const uint8_t ALL_EVENTS = (uint8_t)ADI_ADRV9025_AVGPOWER_ERR | (uint8_t)ADI_ADRV9025_PEAKPOWER_ERR | (uint8_t)ADI_ADRV9025_PLLDFRM_ERR;

    int32_t                 recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e baseAddr       = ADRV9025_BF_ALL_TX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that if requested Tx Channel is valid */
    if ((txChannelSelect != ADI_ADRV9025_TX1) &&
        (txChannelSelect != ADI_ADRV9025_TX2) &&
        (txChannelSelect != ADI_ADRV9025_TX3) &&
        (txChannelSelect != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelSelect,
                         "Invalid Tx Channel is selected to clear sticky pa protection event bits. Valid values are TX1-2-3-4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (((eventBits & (~ALL_EVENTS)) != 0) || (eventBits == 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         eventBits,
                         "Invalid eventBits - Valid Values are combinations of AVGPOWER_ERR/PEAKPOWER_ERR/PLLDFRM_ERR");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                   (uint32_t)txChannelSelect)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (ADRV9025_BF_EQUAL(eventBits,
                          (uint8_t)ADI_ADRV9025_PLLDFRM_ERR))
    {
        recoveryAction = adrv9025_TxPllJesdProtClrBfSet(device,
                                                        baseAddr,
                                                        1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         NULL,
                         "Error while setting clear bit for Pll Jesd error field - Silicon B");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (ADRV9025_BF_EQUAL(eventBits,
                          (uint8_t)ADI_ADRV9025_PEAKPOWER_ERR))
    {
        recoveryAction = adrv9025_TxPaProtectionPeakpowerErrorClearBfSet(device,
                                                                         baseAddr,
                                                                         1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         NULL,
                         "Error while setting clear bit for peak power error field - Silicon B");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (ADRV9025_BF_EQUAL(eventBits,
                          (uint8_t)ADI_ADRV9025_AVGPOWER_ERR))
    {
        recoveryAction = adrv9025_TxPaProtectionAvgpowerErrorClearBfSet(device,
                                                                        baseAddr,
                                                                        1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         NULL,
                         "Error while setting clear bit for average power error field");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAttenuationRampUpStickyModeEnable(adi_adrv9025_Device_t* device,
                                                         uint32_t               channelMask,
                                                         uint8_t                txPllJesdProtClrReqd,
                                                         uint8_t                txPaProtectionAvgpowerErrorClearRequired,
                                                         uint8_t                txPaProtectionPeakpowerErrorClearRequired)
{
    int32_t                 recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBitfieldAddr = (adrv9025_BfTxChanAddr_e)ADRV9025_ADDR_CH0_TX;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((txChannelBitfieldAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                                channelMask)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (txPllJesdProtClrReqd != ADI_TRUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         TxPllJesdProtClrReqd,
                         "Invalid TxPllJesdProtClrReqd, txPllJesdProtClrReqd should always be ADI_TRUE");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txPaProtectionAvgpowerErrorClearRequired != ADI_TRUE) && (txPaProtectionAvgpowerErrorClearRequired != ADI_FALSE))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         TxPaProtectionAvgpowerErrorClearRequired,
                         "Invalid TxPaProtectionAvgpowerErrorClearRequired");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txPaProtectionPeakpowerErrorClearRequired != ADI_TRUE) && (txPaProtectionPeakpowerErrorClearRequired != ADI_FALSE))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         TxPaProtectionPeakpowerErrorClearRequired,
                         "Invalid TxPaProtectionPeakpowerErrorClearRequired");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_TxPllJesdProtClrReqdBfSet(device,
                                                        txChannelBitfieldAddr,
                                                        txPllJesdProtClrReqd);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_TxPaProtectionAvgpowerErrorClearRequiredBfSet(device,
                                                                            txChannelBitfieldAddr,
                                                                            txPaProtectionAvgpowerErrorClearRequired);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_TxPaProtectionPeakpowerErrorClearRequiredBfSet(device,
                                                                             txChannelBitfieldAddr,
                                                                             txPaProtectionPeakpowerErrorClearRequired);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SrlIrqGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_TxChannels_e txChannel,
                               uint8_t*                  enable)
{
    int32_t                 recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr = ADRV9025_BF_TX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check enable pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        enable);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        txChannelBaseAddr = ADRV9025_BF_TX_CH0;
        break;
    case ADI_ADRV9025_TX2:
        txChannelBaseAddr = ADRV9025_BF_TX_CH1;
        break;
    case ADI_ADRV9025_TX3:
        txChannelBaseAddr = ADRV9025_BF_TX_CH2;
        break;
    case ADI_ADRV9025_TX4:
        txChannelBaseAddr = ADRV9025_BF_TX_CH3;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Error while accessing selected Tx channel - Supported values TX1-TX2-TX3-TX4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_TxSrlIrqBfGet(device,
                                            txChannelBaseAddr,
                                            enable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read SRL IRQ status of selected Tx channel");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SrlIrqEnableSet(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_TxChannels_e txChannel,
                                     uint8_t                   enable)
{
    int32_t                 recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr = ADRV9025_BF_TX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        txChannelBaseAddr = ADRV9025_BF_TX_CH0;
        break;
    case ADI_ADRV9025_TX2:
        txChannelBaseAddr = ADRV9025_BF_TX_CH1;
        break;
    case ADI_ADRV9025_TX3:
        txChannelBaseAddr = ADRV9025_BF_TX_CH2;
        break;
    case ADI_ADRV9025_TX4:
        txChannelBaseAddr = ADRV9025_BF_TX_CH3;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Error while accessing selected Tx channel - Supported values TX1-TX2-TX3-TX4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    enable = (enable == 0) ? 0 : 1;

    recoveryAction = adrv9025_TxSrlIrqEnBfSet(device,
                                              txChannelBaseAddr,
                                              enable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to enable/disable SRL interrupt for selected Tx channel");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SrlIrqEnableGet(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_TxChannels_e txChannel,
                                     uint8_t*                  enable)
{
    int32_t                 recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr = ADRV9025_BF_TX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check enable pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        enable);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        txChannelBaseAddr = ADRV9025_BF_TX_CH0;
        break;
    case ADI_ADRV9025_TX2:
        txChannelBaseAddr = ADRV9025_BF_TX_CH1;
        break;
    case ADI_ADRV9025_TX3:
        txChannelBaseAddr = ADRV9025_BF_TX_CH2;
        break;
    case ADI_ADRV9025_TX4:
        txChannelBaseAddr = ADRV9025_BF_TX_CH3;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Error while accessing selected Tx channel - Supported values TX1-TX2-TX3-TX4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_TxSrlIrqEnBfGet(device,
                                              txChannelBaseAddr,
                                              enable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read SRL IRQ enabled status of selected Tx channel");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SrlStatisticsRead(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_TxChannels_e txChannel,
                                       uint8_t                   clearStats,
                                       uint16_t*                 statisticsReadBack)
{
    static const uint8_t    ARM_ERR_SHIFT           = 1;
    static const uint8_t    clrBitPositionInPayload = 0x40;
    int32_t                 recoveryAction          = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr       = ADRV9025_BF_TX_CH0;
    uint8_t                 statsEnabled            = 0;
    uint8_t                 extData[2]              = {0};
    uint8_t                 srlCfgPayload           = 0;
    uint8_t                 armData[2]              = {0};
    uint8_t                 cmdStatusByte           = 0;
    int32_t                 errHdl                  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check statisticsReadBack pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        statisticsReadBack);

    switch (txChannel)
    {
    case ADI_ADRV9025_TX1:
        txChannelBaseAddr = ADRV9025_BF_TX_CH0;
        break;
    case ADI_ADRV9025_TX2:
        txChannelBaseAddr = ADRV9025_BF_TX_CH1;
        break;
    case ADI_ADRV9025_TX3:
        txChannelBaseAddr = ADRV9025_BF_TX_CH2;
        break;
    case ADI_ADRV9025_TX4:
        txChannelBaseAddr = ADRV9025_BF_TX_CH3;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Error while accessing selected Tx channel - Supported values TX1-TX2-TX3-TX4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_TxSrlStatEnBfGet(device,
                                               txChannelBaseAddr,
                                               &statsEnabled);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read SRL stats enabled status of selected Tx channel");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (statsEnabled == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         statsEnabled,
                         "SRL statistics are not enabled for selected Tx channel, please enable statistics before reading. ");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    clearStats = (clearStats == 0) ? ADI_FALSE : ADI_TRUE;

    /*Read statistics from FW through mailbox */

    /* Command ARM to return the statistics */
    srlCfgPayload = (uint8_t)txChannel;
    if (clearStats == ADI_TRUE)
    {
        srlCfgPayload |= clrBitPositionInPayload;
    }

    extData[0]     = ADRV9025_CPU_OBJECTID_SRL_STATISTICS;
    extData[1]     = srlCfgPayload;
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Cannot write ARM command to set SRL statistics");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETSRLSTATS_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETSRLSTATS_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte >> ARM_ERR_SHIFT) > 0)
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

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Cannot wait Arm Command Status");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read SRL configuration from mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ADI_TRUE);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Slr statistics MSB on mailboxGetBuf[0] and LSB on mailboxGetBuf[1] */
    *statisticsReadBack = ((uint16_t)armData[0] << 8) | (uint16_t)armData[1];

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SrlCfgSet(adi_adrv9025_Device_t*            device,
                               adi_adrv9025_SlewRateLimiterCfg_t slewRateLimiterCfg)
{
    static const uint8_t    MAX_SLEW_RATE_OFFSET       = 0x0F;
    static const uint8_t    SRL_CONFIG_MODE_BITS_SHIFT = 4;
    int32_t                 recoveryAction             = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                channelLoop                = 0;
    uint8_t                 j                          = 0;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr          = ADRV9025_BF_ALL_TX_CHANNELS;
    uint8_t                 extData[2]                 = {0};
    uint8_t                 cmdStatusByte              = 0;
    int32_t                 errHdl                     = 0;
    uint8_t                 srlCfgPayload              = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* checking channel mask validity */
    channelLoop = slewRateLimiterCfg.txChannelMask;
    if ((channelLoop == 0) || ((channelLoop & ~ADI_ADRV9025_TXALL) > 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         slewRateLimiterCfg.txChannelMask,
                         "Slew rate limiter txChannelMask value is invalid.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* checking slew rate limitation */
    if ((device->devStateInfo.txSrlRateInvalidMask & ADI_ADRV9025_TXALL) > 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->devStateInfo.txSrlRateInvalidMask,
                         "Slew rate limiter rate value is invalid.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (slewRateLimiterCfg.srlOffset > MAX_SLEW_RATE_OFFSET)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         slewRateLimiterCfg.srlOffset,
                         "Slew rate limiter offset value is over max value which is: 0x0F");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that the requested mode is valid */
    if ((slewRateLimiterCfg.modeSel != ADI_ADRV9025_SRL_DISABLED) &&
        (slewRateLimiterCfg.modeSel != ADI_ADRV9025_SRL_BASIC) &&
        (slewRateLimiterCfg.modeSel != ADI_ADRV9025_SRL_RAMP_DOWN) &&
        (slewRateLimiterCfg.modeSel != ADI_ADRV9025_SRL_RAMP_DOWN_WITH_BBIC_INTERRUPT))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         slewRateLimiterCfg.modeSel,
                         "Invalid mode selection for Slew rate limiter. Valid modes are DISABLED/BASIC/RAMPDOWN/RAMPDOWNWITHBBICINTERRUPT.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that the requested mode is valid */
    if ((slewRateLimiterCfg.tableSel != ADI_ADRV9025_SRL_TABLE0) &&
        (slewRateLimiterCfg.tableSel != ADI_ADRV9025_SRL_TABLE1) &&
        (slewRateLimiterCfg.tableSel != ADI_ADRV9025_SRL_TABLE2) &&
        (slewRateLimiterCfg.tableSel != ADI_ADRV9025_SRL_TABLE3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         slewRateLimiterCfg.tableSel,
                         "Invalid table selection for Slew rate limiter. Valid tables are TABLE0-1-2-3.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Write configuration */
    channelLoop = slewRateLimiterCfg.txChannelMask;
    for (j = 0; j < ADI_ADRV9025_MAX_TXCHANNELS; j++)
    {
        if (ADRV9025_BF_EQUAL(channelLoop,
                              ADI_ADRV9025_TX1))
        {
            txChannelBaseAddr = ADRV9025_BF_TX_CH0;
            channelLoop       = channelLoop & ~ADI_ADRV9025_TX1;
            srlCfgPayload |= ADI_ADRV9025_TX1;
        }
        else if (ADRV9025_BF_EQUAL(channelLoop,
                                   ADI_ADRV9025_TX2))
        {
            txChannelBaseAddr = ADRV9025_BF_TX_CH1;
            channelLoop       = channelLoop & ~ADI_ADRV9025_TX2;
            srlCfgPayload |= ADI_ADRV9025_TX2;
        }
        else if (ADRV9025_BF_EQUAL(channelLoop,
                                   ADI_ADRV9025_TX3))
        {
            txChannelBaseAddr = ADRV9025_BF_TX_CH2;
            channelLoop       = channelLoop & ~ADI_ADRV9025_TX3;
            srlCfgPayload |= ADI_ADRV9025_TX3;
        }
        else if (ADRV9025_BF_EQUAL(channelLoop,
                                   ADI_ADRV9025_TX4))
        {
            txChannelBaseAddr = ADRV9025_BF_TX_CH3;
            channelLoop       = channelLoop & ~ADI_ADRV9025_TX4;
            srlCfgPayload |= ADI_ADRV9025_TX4;
        }
        else if (channelLoop == 0)
        {
            /* All channels are set up */
            break;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             slewRateLimiterCfg.txChannelMask,
                             "Error while accessing selected Tx channel");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        recoveryAction = adrv9025_TxSrlTableSelBfSet(device,
                                                     txChannelBaseAddr,
                                                     (uint8_t)slewRateLimiterCfg.tableSel);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while writing SRL table selection for selected Tx channel");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_TxSrlSlewOffsetBfSet(device,
                                                       txChannelBaseAddr,
                                                       slewRateLimiterCfg.srlOffset);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while writing SRL offset for selected Tx channel");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Command ARM to set rest of the configuration for slew rate limiter of selected Tx channel */
        srlCfgPayload |= ((uint8_t)slewRateLimiterCfg.modeSel << SRL_CONFIG_MODE_BITS_SHIFT);
        extData[0] = ADRV9025_CPU_OBJECTID_SRL_CONFIG;
        extData[1] = srlCfgPayload;

        recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                                  ADI_ADRV9025_CPU_TYPE_C,
                                                  ADRV9025_CPU_SET_OPCODE,
                                                  &extData[0],
                                                  sizeof(extData));
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Cannot write ARM command to set SRL configuration");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Wait for command to finish executing */
        recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                       ADI_ADRV9025_CPU_TYPE_C,
                                                       (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                       &cmdStatusByte,
                                                       (uint32_t)ADI_ADRV9025_SETSRLCONFIG_TIMEOUT_US,
                                                       (uint32_t)ADI_ADRV9025_SETSRLCONFIG_INTERVAL_US);

        /* If cmdStatusByte is non-zero then flag an ARM error */
        if ((cmdStatusByte >> 1) > 0)
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
                         NULL,
                         "Cannot wait Arm Command Status");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Clear payload byte for next loop */
        srlCfgPayload = 0;
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SrlCfgGet(adi_adrv9025_Device_t*             device,
                               adi_adrv9025_TxChannels_e          txChannel,
                               adi_adrv9025_SlewRateLimiterCfg_t* slewRateLimiterCfg)
{
    static const uint8_t    SRL_CONFIG_MODE_BITS_SHIFT = 4;
    int32_t                 recoveryAction             = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfTxChanAddr_e txChannelBaseAddr          = ADRV9025_BF_ALL_TX_CHANNELS;
    uint8_t                 extData[2]                 = {0};
    uint8_t                 armData[1]                 = {0};
    uint8_t                 cmdStatusByte              = 0;
    int32_t                 errHdl                     = 0;
    uint8_t                 srlCfgPayload              = 0;
    uint8_t                 modeSel                    = 0;
    uint8_t                 tableSel                   = 0;
    uint8_t                 srlOffset                  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check slewRateLimiterCfg pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        slewRateLimiterCfg);

    /* Check that the requested mode is valid */
    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         slewRateLimiterCfg.modeSel,
                         "Invalid Tx Channel. Valid values are Tx1/Tx2/Tx3/Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    txChannelBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                       (uint32_t)txChannel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_TxSrlTableSelBfGet(device,
                                                 txChannelBaseAddr,
                                                 &tableSel);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while reading SRL table selection for selected Tx channel");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_TxSrlSlewOffsetBfGet(device,
                                                   txChannelBaseAddr,
                                                   &srlOffset);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while reading SRL offset for selected Tx channel");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Command ARM to return the currently assigned GPIO for the requested signal ID */
    srlCfgPayload  = (uint8_t)txChannel;
    extData[0]     = ADRV9025_CPU_OBJECTID_SRL_CONFIG;
    extData[1]     = srlCfgPayload;
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Cannot write ARM command to get SRL configuration");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETSRLCONFIG_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETSRLCONFIG_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
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

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Cannot wait Arm Command Status");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read SRL configuration from mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            ADI_TRUE);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Check whether channel mask is same for both command and readBack bytes */
    if (!ADRV9025_BF_EQUAL(armData[0],
                           srlCfgPayload))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         srlCfgPayload,
                         "There is a mismatch between channel masks sent and read back from mail box, configuration which is read, will be ignored.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    modeSel = armData[0] >> SRL_CONFIG_MODE_BITS_SHIFT;

    slewRateLimiterCfg->modeSel       = (adi_adrv9025_SrlModeSel_e)modeSel;
    slewRateLimiterCfg->srlOffset     = srlOffset;
    slewRateLimiterCfg->tableSel      = (adi_adrv9025_SrlTableSel_e)tableSel;
    slewRateLimiterCfg->txChannelMask = (uint32_t)txChannel;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SrlRampTimerSet(adi_adrv9025_Device_t* device,
                                     uint32_t               timerMicroSec)
{
    static const uint32_t RAMP_TIMER_MIN = 80;      // 80 microseconds
    static const uint32_t RAMP_TIMER_MAX = 5000000; // 5 seconds
    int32_t               recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t               timerData[5]   = {0};
    uint8_t               cmdStatusByte  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((timerMicroSec < RAMP_TIMER_MIN) || (timerMicroSec > RAMP_TIMER_MAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         timerMicroSec,
                         "Ramp timer value is out of range (80 usec -- 5000000 usec)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Writing SRL timer value to ARM */
    timerData[0] = 4;
    timerData[1] = timerMicroSec & 0xFF;
    timerData[2] = (timerMicroSec >> 8) & 0xFF;
    timerData[3] = (timerMicroSec >> 16) & 0xFF;
    timerData[4] = (timerMicroSec >> 24) & 0xFF;
    /* Write to the ARM Mem */
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                              &timerData[0],
                                              sizeof(timerData),
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "timerValue failed writing ARM memory");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Notify ARM: ARM Object id, byte offset LSB, offset MSB = 0, copy 2 bytes */
    timerData[0] = ADRV9025_CPU_OBJECTID_CONFIG;
    timerData[1] = ADRV9025_CPU_OBJECTID_SYSTEM_INFO;
    timerData[2] = 0;
    timerData[3] = 0;
    timerData[4] = 0;

    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             ADRV9025_CPU_SET_OPCODE,
                             &timerData[0],
                             4);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "ARM command failed to set SRL timer value configuration");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   ADI_ADRV9025_WRITECPUCFG_TIMEOUT_US,
                                                   ADI_ADRV9025_READCPUCFG_TIMEOUT_US);

    if ((cmdStatusByte >> 1) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     ADI_ADRV9025_SRC_CPUCMD,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                              ADRV9025_CPU_OBJECTID_CONFIG,
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Cannot wait Arm Command Status");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SrlRampTimerGet(adi_adrv9025_Device_t* device,
                                     uint32_t*              timerMicroSec)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;
    int32_t              recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint8_t              timerData[4]          = {0};
    uint8_t              cmdStatusByte         = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check timerMicroSec pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        timerMicroSec);

    /* Notify ARM for how many byte to read */
    timerData[0]   = 4;
    timerData[1]   = 0;
    timerData[2]   = 0;
    timerData[3]   = 0;
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                              &timerData[0],
                                              1,
                                              ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "timerValue failed writing ARM memory");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Notify ARM for reading timer value */
    timerData[0] = ADRV9025_CPU_OBJECTID_CONFIG;
    timerData[1] = ADRV9025_CPU_OBJECTID_SYSTEM_INFO;
    timerData[2] = 0;
    timerData[3] = 0;

    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              ADRV9025_CPU_GET_OPCODE,
                                              &timerData[0],
                                              sizeof(timerData));
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     timerData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   ADI_ADRV9025_WRITECPUCFG_TIMEOUT_US,
                                                   ADI_ADRV9025_READCPUCFG_TIMEOUT_US);
    if ((cmdStatusByte >> 1) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     ADI_ADRV9025_SRC_CPUCMD,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                              ADRV9025_CPU_OBJECTID_CONFIG,
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Cannot wait Arm Command Status");
    ADI_ERROR_RETURN(device->common.error.newAction);

    timerData[0] = 0;
    timerData[1] = 0;
    timerData[2] = 0;
    timerData[3] = 0;

    /* Read the timerValue per channel */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                             &timerData[0],
                                             sizeof(timerData),
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "ARM command failed to read SRL timer value");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing Tracking Enable/Disable status (first 4 bytes) */
    *timerMicroSec = (uint32_t)(((uint32_t)timerData[0]) |
        ((uint32_t)timerData[1] << 8) |
        ((uint32_t)timerData[2] << 16) |
        ((uint32_t)timerData[3] << 24));

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PaProtectionRampDownEnableSet(adi_adrv9025_Device_t* device,
                                                   uint32_t               txChannelMask,
                                                   uint8_t                enable)
{
    static const uint32_t ALL_TX_MASK = (uint32_t)ADI_ADRV9025_TX1 | (uint32_t)ADI_ADRV9025_TX2 |
    (uint32_t)ADI_ADRV9025_TX3 | (uint32_t)ADI_ADRV9025_TX4;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t k              = 0;

    adrv9025_BfTxChanAddr_e baseAddr = ADRV9025_BF_ALL_TX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & (~ALL_TX_MASK)) != 0) || txChannelMask == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelMask,
                         "Invalid Tx Channel is selected to enable/disable ramp-down. Valid values are different combinations of TX1-2-3-4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    enable = (enable == 0) ? ADI_FALSE : ADI_TRUE;

    for (k = 0; k < ADI_ADRV9025_MAX_TXCHANNELS; k++)
    {
        if (ADRV9025_BF_EQUAL(txChannelMask,
                              ((uint32_t)1 << (uint32_t)k)))
        {
            if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                           ((uint32_t)1 << (uint32_t)k))) == (adrv9025_BfTxChanAddr_e)0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "Invalid TxChannel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            recoveryAction = adrv9025_TxPaProtectionGainRampDownEnBfSet(device,
                                                                        baseAddr,
                                                                        enable);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             enable,
                             "Could not enable/disable ramp-down for pa protection");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PaProtectionRampDownEnableGet(adi_adrv9025_Device_t*    device,
                                                   adi_adrv9025_TxChannels_e txChannelSelect,
                                                   uint8_t*                  enable)
{
    int32_t                 recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 readVal        = 0;
    adrv9025_BfTxChanAddr_e baseAddr       = ADRV9025_BF_ALL_TX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that if eventBits pointer is not NULL */
    ADI_NULL_PTR_RETURN(&device->common,
                        enable);

    /* Check that if requested Tx Channel is valid */
    if ((txChannelSelect != ADI_ADRV9025_TX1) &&
        (txChannelSelect != ADI_ADRV9025_TX2) &&
        (txChannelSelect != ADI_ADRV9025_TX3) &&
        (txChannelSelect != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelSelect,
                         "Invalid Tx Channel is selected to read pa protection ramp down enable bit. Valid values are TX1-2-3-4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                   (uint32_t)txChannelSelect)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_TxPaProtectionGainRampDownEnBfGet(device,
                                                                baseAddr,
                                                                &readVal);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while reading pa protection ramp down enable bit");
    ADI_ERROR_RETURN(device->common.error.newAction);
    *enable = readVal;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PaProtectionRampUpEnableSet(adi_adrv9025_Device_t* device,
                                                 uint32_t               txChannelMask,
                                                 uint8_t                enable)
{
    static const uint32_t ALL_TX_MASK = (uint32_t)ADI_ADRV9025_TX1 | (uint32_t)ADI_ADRV9025_TX2 |
    (uint32_t)ADI_ADRV9025_TX3 | (uint32_t)ADI_ADRV9025_TX4;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t k              = 0;

    adrv9025_BfTxChanAddr_e baseAddr = ADRV9025_BF_ALL_TX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & (~ALL_TX_MASK)) != 0) || txChannelMask == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelMask,
                         "Invalid Tx Channel is selected to enable/disable ramp-up. Valid values are different combinations of TX1-2-3-4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    enable = (enable == 0) ? ADI_FALSE : ADI_TRUE;

    for (k = 0; k < ADI_ADRV9025_MAX_TXCHANNELS; k++)
    {
        if (ADRV9025_BF_EQUAL(txChannelMask,
                              ((uint32_t)1 << (uint32_t)k)))
        {
            if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                           ((uint32_t)1 << (uint32_t)k))) == (adrv9025_BfTxChanAddr_e)0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "Invalid TxChannel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            recoveryAction = adrv9025_TxPaProtectionGainRampUpEnBfSet(device,
                                                                      baseAddr,
                                                                      enable);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             enable,
                             "Could not enable/disable ramp-up for pa protection");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_PaProtectionRampUpEnableGet(adi_adrv9025_Device_t*    device,
                                                 adi_adrv9025_TxChannels_e txChannelSelect,
                                                 uint8_t*                  enable)
{
    int32_t                 recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 readVal        = 0;
    adrv9025_BfTxChanAddr_e baseAddr       = ADRV9025_BF_ALL_TX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that if eventBits pointer is not NULL */
    ADI_NULL_PTR_RETURN(&device->common,
                        enable);

    /* Check that if requested Tx Channel is valid */
    if ((txChannelSelect != ADI_ADRV9025_TX1) &&
        (txChannelSelect != ADI_ADRV9025_TX2) &&
        (txChannelSelect != ADI_ADRV9025_TX3) &&
        (txChannelSelect != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelSelect,
                         "Invalid Tx Channel is selected to read pa protection ramp up enable bit. Valid values are TX1-2-3-4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                   (uint32_t)txChannelSelect)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_TxPaProtectionGainRampUpEnBfGet(device,
                                                              baseAddr,
                                                              &readVal);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while reading pa protection ramp up enable bit");
    ADI_ERROR_RETURN(device->common.error.newAction);
    *enable = readVal;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxNcoShifterSet(adi_adrv9025_Device_t*         device,
                                     adi_adrv9025_TxNcoShifterCfg_t shifterCfg)
{
    int32_t               recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    uint8_t               extData[2]        = {ADRV9025_CPU_OBJECTID_TX_NCO, 0};
    uint8_t               armData[5]        = {0, 0, 0, 0, 0};
    uint8_t               cmdStatusByte     = 0u;
    int32_t               errHdl            = 0;
    static const uint8_t  ARM_ERR_CODE      = 0x0E;
    static const uint8_t  SHIFT_ONE_BYTE    = 8;
    static const uint8_t  SHIFT_TWO_BYTES   = 16;
    static const uint8_t  SHIFT_THREE_BYTES = 24;
    static const uint8_t  SHIFT_6_BITS      = 6;
    static const uint8_t  SHIFT_7_BITS      = 7;
    static const uint8_t  TXCHANNELMASK     = 0x0F;
    static const uint32_t BYTE_0            = 0x000000FF;
    static const uint32_t BYTE_1            = 0x0000FF00;
    static const uint32_t BYTE_2            = 0x00FF0000;
    static const uint32_t BYTE_3            = 0xFF000000;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    extData[1] = ((shifterCfg.enable) << SHIFT_7_BITS) | (((uint8_t)(shifterCfg.shifterMode)) << SHIFT_6_BITS) | (shifterCfg.txChannelMask & TXCHANNELMASK);

    shifterCfg.shiftFrequency_kHz = shifterCfg.shiftFrequency_kHz * 1000;
    armData[0]                    = shifterCfg.shiftFrequency_kHz & BYTE_0;
    armData[1]                    = (shifterCfg.shiftFrequency_kHz & BYTE_1) >> SHIFT_ONE_BYTE;
    armData[2]                    = (shifterCfg.shiftFrequency_kHz & BYTE_2) >> SHIFT_TWO_BYTES;
    armData[3]                    = (shifterCfg.shiftFrequency_kHz & BYTE_3) >> SHIFT_THREE_BYTES;
    armData[4]                    = (uint8_t)shifterCfg.shiftGain;

    /* Write offset and size to ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                              (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                              &armData[0],
                                              sizeof(armData),
                                              ADI_TRUE);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Executing Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_SET_OPCODE,
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
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
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

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxNcoShifterGet(adi_adrv9025_Device_t          *device,
                                     adi_adrv9025_TxNcoShifterCfg_t *shifterCfg)
{
    int32_t              recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t              extData[2]     = {ADRV9025_CPU_OBJECTID_TX_NCO, 0};
    uint8_t              armData[8]     = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t              cmdStatusByte  = 0u;
    int32_t              errHdl         = 0;
    static const uint8_t ARM_ERR_CODE   = 0x0E;
    static const uint8_t AUTO_INC       = 1;
    static const uint8_t SHIFT_ONE_BYTE = 0x08;
    static const uint8_t SHIFT_4_BITS   = 0x04;
    static const uint8_t SHIFT_7_BITS   = 0x07;
    static const uint8_t NCO_ENABLE     = 0x80;
    static const uint8_t NCO_MODE       = 0x10;
    static const uint8_t TXCHANNELMASK  = 0x0F;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, shifterCfg);

    extData[1] = (shifterCfg->txChannelMask & 0x0F);

    /* Executing Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_GET_OPCODE,
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

    /* Read the Tx NCO Configuration */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData[0],
                            sizeof(armData),
                            AUTO_INC);
    ADI_ERROR_RETURN(device->common.error.newAction);

    shifterCfg->txChannelMask      = armData[0] & TXCHANNELMASK;
    shifterCfg->shifterMode        = (adi_adrv9025_TxNcoShifterMode_e)((armData[0] & NCO_MODE) >> SHIFT_4_BITS);
    shifterCfg->enable             = ((armData[0] & NCO_ENABLE) >> SHIFT_7_BITS);
    shifterCfg->shiftGain          = (adi_adrv9025_TxNcoToneGain_e)(armData[1]);
    shifterCfg->shiftFrequency_kHz = armData[7];
    shifterCfg->shiftFrequency_kHz = (shifterCfg->shiftFrequency_kHz << SHIFT_ONE_BYTE) | armData[6];
    shifterCfg->shiftFrequency_kHz = (shifterCfg->shiftFrequency_kHz << SHIFT_ONE_BYTE) | armData[5];
    shifterCfg->shiftFrequency_kHz = (shifterCfg->shiftFrequency_kHz << SHIFT_ONE_BYTE) | armData[4];
    shifterCfg->shiftFrequency_kHz = shifterCfg->shiftFrequency_kHz / 1000;

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAnalogAttenEarlyDelaySet(adi_adrv9025_Device_t* device,
                                                uint32_t               txChannelMask,
                                                uint8_t                delayValue)
{
    static const uint32_t ALL_TX_MASK = (uint32_t)ADI_ADRV9025_TX1 | (uint32_t)ADI_ADRV9025_TX2 |
    (uint32_t)ADI_ADRV9025_TX3 | (uint32_t)ADI_ADRV9025_TX4;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t k              = 0;

    adrv9025_BfTxChanAddr_e baseAddr = ADRV9025_BF_ALL_TX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & ~ALL_TX_MASK) != 0) || (txChannelMask == 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelMask,
                         "Invalid Tx Channel is selected for Tx atten early delay. Valid values are different combinations of TX1-2-3-4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (k = 0; k < ADI_ADRV9025_MAX_TXCHANNELS; k++)
    {
        if (ADRV9025_BF_EQUAL(txChannelMask,
                              ((uint32_t)1 << (uint32_t)k)))
        {
            if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                           ((uint32_t)1 << (uint32_t)k))) == (adrv9025_BfTxChanAddr_e)0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "Invalid TxChannel");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            recoveryAction = adrv9025_TxTxAttenuationEarlyDelayCounterForAnalogBfSet(device,
                                                                                     baseAddr,
                                                                                     delayValue);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             enable,
                             "Could not set Tx analog atten early delay");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_TxAnalogAttenEarlyDelayGet(adi_adrv9025_Device_t*    device,
                                                adi_adrv9025_TxChannels_e txChannelSelect,
                                                uint8_t*                  delayValue)
{
    int32_t                 recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                 readVal        = 0;
    adrv9025_BfTxChanAddr_e baseAddr       = ADRV9025_BF_ALL_TX_CHANNELS;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that if eventBits pointer is not NULL */
    ADI_NULL_PTR_RETURN(&device->common,
                        delayValue);

    /* Check that if requested Tx Channel is valid */
    if ((txChannelSelect != ADI_ADRV9025_TX1) &&
        (txChannelSelect != ADI_ADRV9025_TX2) &&
        (txChannelSelect != ADI_ADRV9025_TX3) &&
        (txChannelSelect != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelSelect,
                         "Invalid Tx Channel is selected to read Tx analog atten early delay. Valid values are TX1-2-3-4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((baseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                   (uint32_t)txChannelSelect)) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid TxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_TxTxAttenuationEarlyDelayCounterForAnalogBfGet(device,
                                                                             baseAddr,
                                                                             &readVal);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error while reading Tx analog atten early delay");
    ADI_ERROR_RETURN(device->common.error.newAction);
    *delayValue = readVal;

    return device->common.error.newAction;
}
