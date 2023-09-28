// SPDX-License-Identifier: GPL-2.0
/**
 * \file adrv9025_radioctrl.c
 * \brief Contains ADRV9025 radio control related private function implementations
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "../devices/adrv9025/private/include/adrv9025_radioctrl.h"
#include "../devices/adrv9025/private/include/adrv9025_tx.h"
#include "../devices/adrv9025/private/include/adrv9025_rx.h"
#include "../devices/adrv9025/private/include/adrv9025_bf_tx.h"
#include "../devices/adrv9025/private/include/adrv9025_bf_rx.h"
#include "../devices/adrv9025/private/include/adrv9025_bf_core.h"
#include "../devices/adrv9025/private/include/adrv9025_cpu.h"
#include "../devices/adrv9025/private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_gpio.h"

int32_t adrv9025_TxEnableSet(adi_adrv9025_Device_t* device,
                             uint32_t               txChannelMask)
{
    static const uint8_t                   TX_ENABLE       = 1;
    static const uint8_t                   TX_DISABLE      = 0;
    static const adi_adrv9025_TxChannels_e txChannelsArr[] = {ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TX3, ADI_ADRV9025_TX4};
    static const uint32_t                  ALL_TX_CHANNELS = (uint32_t)(ADI_ADRV9025_TXALL);

    int32_t                 recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                channelIndex          = 0;
    uint8_t                 txEnable              = 0;
    adrv9025_BfTxChanAddr_e txChannelBitfieldAddr = (adrv9025_BfTxChanAddr_e)ADRV9025_ADDR_CH0_TX;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    if ((txChannelMask & ~ALL_TX_CHANNELS) != 0)
    {
        /* Tx channel selection is not valid */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannelMask,
                         "Invalid Tx channel selection, any combination of TxOff/Tx1/2/3/4 channels are valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (channelIndex = 0; channelIndex < (sizeof(txChannelsArr) / sizeof(txChannelsArr[0])); channelIndex++)
    {
        if (((device->devStateInfo.initializedChannels >> ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannelsArr[channelIndex]) == 0)
        {
            continue;
        }

        txChannelBitfieldAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                               txChannelsArr[channelIndex]);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if ((txChannelMask & (uint32_t)txChannelsArr[channelIndex]) == (uint32_t)txChannelsArr[channelIndex])
        {
            txEnable = TX_ENABLE;
        }
        else
        {
            txEnable = TX_DISABLE;
        }

        recoveryAction = adrv9025_TxTxEnableBfSet(device,
                                                  txChannelBitfieldAddr,
                                                  txEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_RxEnableSet(adi_adrv9025_Device_t* device,
                             uint32_t               rxChannelMask)
{
    static const uint8_t                   RX_ENABLE       = 1;
    static const uint8_t                   RX_DISABLE      = 0;
    static const adi_adrv9025_RxChannels_e rxChannelsArr[] = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};

    int32_t                 recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                channelIndex          = 0;
    uint8_t                 rxEnable              = 0;
    adrv9025_BfRxChanAddr_e rxChannelBitfieldAddr = ADRV9025_BF_RX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    for (channelIndex = 0; channelIndex < (sizeof(rxChannelsArr) / sizeof(rxChannelsArr[0])); channelIndex++)
    {
        if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannelsArr[channelIndex]) != (uint32_t)rxChannelsArr[channelIndex])
        {
            continue;
        }

        recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                       rxChannelsArr[channelIndex],
                                                       &rxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if ((rxChannelMask & (uint32_t)rxChannelsArr[channelIndex]) == (uint32_t)rxChannelsArr[channelIndex])
        {
            rxEnable = RX_ENABLE;
        }
        else
        {
            rxEnable = RX_DISABLE;
        }

        recoveryAction = adrv9025_RxRxEnableBfSet(device,
                                                  rxChannelBitfieldAddr,
                                                  rxEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_OrxEnableSet(adi_adrv9025_Device_t* device,
                              uint32_t               rxChannelMask)
{
    static const uint8_t ORX1_ENABLE = 0x01;
    static const uint8_t ORX2_ENABLE = 0x02;
    static const uint8_t ORX3_ENABLE = 0x04;
    static const uint8_t ORX4_ENABLE = 0x08;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t orxEnable      = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    if ((rxChannelMask & ADI_ADRV9025_ORX1) == ADI_ADRV9025_ORX1)
    {
        orxEnable |= ORX1_ENABLE;
    }

    if ((rxChannelMask & ADI_ADRV9025_ORX2) == ADI_ADRV9025_ORX2)
    {
        orxEnable |= ORX2_ENABLE;
    }

    if ((rxChannelMask & ADI_ADRV9025_ORX3) == ADI_ADRV9025_ORX3)
    {
        orxEnable |= ORX3_ENABLE;
    }

    if ((rxChannelMask & ADI_ADRV9025_ORX4) == ADI_ADRV9025_ORX4)
    {
        orxEnable |= ORX4_ENABLE;
    }

    recoveryAction = adrv9025_CoreOrxEnableBfSet(device,
                                                 ADRV9025_BF_CORE,
                                                 orxEnable);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_TxEnableGet(adi_adrv9025_Device_t* device,
                             uint32_t*              txChannelMask)
{
    static const adi_adrv9025_TxChannels_e txChannelsArr[] = {ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TX3, ADI_ADRV9025_TX4};

    int32_t                 recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                channelIndex          = 0;
    uint8_t                 txEnable              = 0;
    adrv9025_BfTxChanAddr_e txChannelBitfieldAddr = (adrv9025_BfTxChanAddr_e)ADRV9025_ADDR_CH0_TX;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        txChannelMask);

    for (channelIndex = 0; channelIndex < (sizeof(txChannelsArr) / sizeof(txChannelsArr[0])); channelIndex++)
    {
        if (((device->devStateInfo.initializedChannels >> ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannelsArr[channelIndex]) != (uint32_t)
            txChannelsArr[channelIndex])
        {
            continue;
        }

        txChannelBitfieldAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                               (uint32_t)txChannelsArr[channelIndex]);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_TxTxEnableBfGet(device,
                                                  txChannelBitfieldAddr,
                                                  &txEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (txEnable > 0)
        {
            *txChannelMask |= (uint32_t)txChannelsArr[channelIndex];
        }
        else
        {
            *txChannelMask &= ~(uint32_t)txChannelsArr[channelIndex];
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxEnableGet(adi_adrv9025_Device_t* device,
                             uint32_t*              rxChannelMask)
{
    static const adi_adrv9025_RxChannels_e rxChannelsArr[] = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};

    int32_t                 recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                channelIndex          = 0;
    uint8_t                 rxEnable              = 0;
    adrv9025_BfRxChanAddr_e rxChannelBitfieldAddr = ADRV9025_BF_RX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    for (channelIndex = 0; channelIndex < (sizeof(rxChannelsArr) / sizeof(rxChannelsArr[0])); channelIndex++)
    {
        if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannelsArr[channelIndex]) == 0)
        {
            continue;
        }

        recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                       rxChannelsArr[channelIndex],
                                                       &rxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxRxEnableBfGet(device,
                                                  rxChannelBitfieldAddr,
                                                  &rxEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (rxEnable > 0)
        {
            *rxChannelMask |= (uint32_t)rxChannelsArr[channelIndex];
        }
        else
        {
            *rxChannelMask &= ~(uint32_t)rxChannelsArr[channelIndex];
        }
    }

    return recoveryAction;
}

int32_t adrv9025_OrxEnableGet(adi_adrv9025_Device_t* device,
                              uint32_t*              rxChannelMask)
{
    static const uint8_t ORX1_ENABLE = 0x01;
    static const uint8_t ORX2_ENABLE = 0x02;
    static const uint8_t ORX3_ENABLE = 0x04;
    static const uint8_t ORX4_ENABLE = 0x08;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t orxEnable      = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    recoveryAction = adrv9025_CoreOrxEnableBfGet(device,
                                                 ADRV9025_BF_CORE,
                                                 &orxEnable);
    ADI_ERROR_RETURN(device->common.error.newAction);

    //Update enable status of ORx1
    if ((orxEnable & ORX1_ENABLE) == ORX1_ENABLE)
    {
        *rxChannelMask |= ADI_ADRV9025_ORX1;
    }
    else
    {
        *rxChannelMask &= ~ADI_ADRV9025_ORX1;
    }

    //Update enable status of ORx2
    if ((orxEnable & ORX2_ENABLE) == ORX2_ENABLE)
    {
        *rxChannelMask |= ADI_ADRV9025_ORX2;
    }
    else
    {
        *rxChannelMask &= ~ADI_ADRV9025_ORX2;
    }

    //Update enable status of ORx3
    if ((orxEnable & ORX3_ENABLE) == ORX3_ENABLE)
    {
        *rxChannelMask |= ADI_ADRV9025_ORX3;
    }
    else
    {
        *rxChannelMask &= ~ADI_ADRV9025_ORX3;
    }

    //Update enable status of ORx4
    if ((orxEnable & ORX4_ENABLE) == ORX4_ENABLE)
    {
        *rxChannelMask |= ADI_ADRV9025_ORX4;
    }
    else
    {
        *rxChannelMask &= ~ADI_ADRV9025_ORX4;
    }

    return recoveryAction;
}

int32_t adrv9025_PllUnlockGpInterruptMaskGet(adi_adrv9025_Device_t* device,
                                             adi_adrv9025_PllName_e pllName,
                                             uint8_t*               pllGpInterruptPin0Mask,
                                             uint8_t*               pllGpInterruptPin1Mask)
{
    static const uint8_t  PLL_LO1_GP_INTERRUPT_MASK   = 0x80;
    static const uint8_t  PLL_LO2_GP_INTERRUPT_MASK   = 0x40;
    static const uint8_t  PLL_AUX_GP_INTERRUPT_MASK   = 0x20;
    static const uint8_t  PLL_LO1_GP_INTERRUPT_SHIFT  = 7;
    static const uint8_t  PLL_LO2_GP_INTERRUPT_SHIFT  = 6;
    static const uint8_t  PLL_AUX_GP_INTERRUPT_SHIFT  = 5;
    static const uint64_t PLL_GP_INTERRUPT_BYTE_MASK  = 0x0000FF0000000000;
    static const uint8_t  PLL_GP_INTERRUPT_BYTE_SHIFT = 40;

    int32_t  recoveryAction             = ADI_COMMON_ACT_NO_ACTION;
    uint64_t gpInterruptPin0Mask        = 0;
    uint64_t gpInterruptPin1Mask        = 0;
    uint8_t  pllGpInterruptPin0MaskByte = 0;
    uint8_t  pllGpInterruptPin1MaskByte = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        pllGpInterruptPin0Mask);

    ADI_NULL_PTR_RETURN(&device->common,
                        pllGpInterruptPin1Mask);

    /*Read GP Interrupt Pin0 and Pin1 mask*/
    recoveryAction = adrv9025_CoreGpInterruptsMaskPin0BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInterruptPin0Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreGpInterruptsMaskPin1BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*PLL mask status is the 5th Byte*/
    pllGpInterruptPin0MaskByte = (uint8_t)((gpInterruptPin0Mask & PLL_GP_INTERRUPT_BYTE_MASK) >> PLL_GP_INTERRUPT_BYTE_SHIFT);
    pllGpInterruptPin1MaskByte = (uint8_t)((gpInterruptPin1Mask & PLL_GP_INTERRUPT_BYTE_MASK) >> PLL_GP_INTERRUPT_BYTE_SHIFT);

    switch (pllName)
    {
    case(ADI_ADRV9025_LO1_PLL):
    {
        *pllGpInterruptPin0Mask = (pllGpInterruptPin0MaskByte & PLL_LO1_GP_INTERRUPT_MASK) >> PLL_LO1_GP_INTERRUPT_SHIFT;
        *pllGpInterruptPin1Mask = (pllGpInterruptPin1MaskByte & PLL_LO1_GP_INTERRUPT_MASK) >> PLL_LO1_GP_INTERRUPT_SHIFT;
        break;
    }
    case(ADI_ADRV9025_LO2_PLL):
    {
        *pllGpInterruptPin0Mask = (pllGpInterruptPin0MaskByte & PLL_LO2_GP_INTERRUPT_MASK) >> PLL_LO2_GP_INTERRUPT_SHIFT;
        *pllGpInterruptPin1Mask = (pllGpInterruptPin1MaskByte & PLL_LO2_GP_INTERRUPT_MASK) >> PLL_LO2_GP_INTERRUPT_SHIFT;
        break;
    }
    case(ADI_ADRV9025_AUX_PLL):
    {
        *pllGpInterruptPin0Mask = (pllGpInterruptPin0MaskByte & PLL_AUX_GP_INTERRUPT_MASK) >> PLL_AUX_GP_INTERRUPT_SHIFT;
        *pllGpInterruptPin1Mask = (pllGpInterruptPin1MaskByte & PLL_AUX_GP_INTERRUPT_MASK) >> PLL_AUX_GP_INTERRUPT_SHIFT;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllName,
                         "Invalid PLL encountered while attempting to retrieve PLL GP Interrupt mask");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    return recoveryAction;
}

int32_t adrv9025_PllUnlockGpInterruptMaskSet(adi_adrv9025_Device_t* device,
                                             adi_adrv9025_PllName_e pllName,
                                             uint8_t                pllGpInterruptPin0Mask,
                                             uint8_t                pllGpInterruptPin1Mask)
{
    static const uint8_t  PLL_LO1_GP_INTERRUPT_MASK   = 0x80;
    static const uint8_t  PLL_LO2_GP_INTERRUPT_MASK   = 0x40;
    static const uint8_t  PLL_AUX_GP_INTERRUPT_MASK   = 0x20;
    static const uint8_t  PLL_LO1_GP_INTERRUPT_SHIFT  = 7;
    static const uint8_t  PLL_LO2_GP_INTERRUPT_SHIFT  = 6;
    static const uint8_t  PLL_AUX_GP_INTERRUPT_SHIFT  = 5;
    static const uint64_t PLL_GP_INTERRUPT_BYTE_MASK  = 0x0000FF0000000000;
    static const uint8_t  PLL_GP_INTERRUPT_BYTE_SHIFT = 40;

    int32_t  recoveryAction             = ADI_COMMON_ACT_NO_ACTION;
    uint64_t gpInterruptPin0Mask        = 0;
    uint64_t gpInterruptPin1Mask        = 0;
    uint8_t  pllGpInterruptPin0MaskByte = 0;
    uint8_t  pllGpInterruptPin1MaskByte = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Read GP Interrupt Pin0 and Pin1 mask*/
    recoveryAction = adrv9025_CoreGpInterruptsMaskPin0BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInterruptPin0Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreGpInterruptsMaskPin1BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*PLL mask status is the 5th Byte*/
    pllGpInterruptPin0MaskByte = (uint8_t)((gpInterruptPin0Mask & PLL_GP_INTERRUPT_BYTE_MASK) >> PLL_GP_INTERRUPT_BYTE_SHIFT);
    pllGpInterruptPin1MaskByte = (uint8_t)((gpInterruptPin1Mask & PLL_GP_INTERRUPT_BYTE_MASK) >> PLL_GP_INTERRUPT_BYTE_SHIFT);

    /*Clear Byte 5 of the GP Interrupt Mask*/
    gpInterruptPin0Mask &= ~PLL_GP_INTERRUPT_BYTE_MASK;
    gpInterruptPin1Mask &= ~PLL_GP_INTERRUPT_BYTE_MASK;

    switch (pllName)
    {
    case(ADI_ADRV9025_LO1_PLL):
    {
        /*Clear LO1 Interrupt Mask bit in GP Interrupt byte5*/
        pllGpInterruptPin0MaskByte &= ~PLL_LO1_GP_INTERRUPT_MASK;
        pllGpInterruptPin1MaskByte &= ~PLL_LO1_GP_INTERRUPT_MASK;

        /*Update PLL GP Interrupt Byte*/
        pllGpInterruptPin0MaskByte |= (pllGpInterruptPin0Mask << PLL_LO1_GP_INTERRUPT_SHIFT);
        pllGpInterruptPin1MaskByte |= (pllGpInterruptPin1Mask << PLL_LO1_GP_INTERRUPT_SHIFT);

        break;
    }
    case(ADI_ADRV9025_LO2_PLL):
    {
        /*Clear LO2 Interrupt Mask bit in GP Interrupt byte5*/
        pllGpInterruptPin0MaskByte &= ~PLL_LO2_GP_INTERRUPT_MASK;
        pllGpInterruptPin1MaskByte &= ~PLL_LO2_GP_INTERRUPT_MASK;

        /*Update PLL GP Interrupt Byte*/
        pllGpInterruptPin0MaskByte |= (pllGpInterruptPin0Mask << PLL_LO2_GP_INTERRUPT_SHIFT);
        pllGpInterruptPin1MaskByte |= (pllGpInterruptPin1Mask << PLL_LO2_GP_INTERRUPT_SHIFT);

        break;
    }
    case(ADI_ADRV9025_AUX_PLL):
    {
        /*Clear Aux PLL Interrupt Mask bit in GP Interrupt byte5*/
        pllGpInterruptPin0MaskByte &= ~PLL_AUX_GP_INTERRUPT_MASK;
        pllGpInterruptPin1MaskByte &= ~PLL_AUX_GP_INTERRUPT_MASK;

        /*Update PLL GP Interrupt Byte*/
        pllGpInterruptPin0MaskByte |= (pllGpInterruptPin0Mask << PLL_AUX_GP_INTERRUPT_SHIFT);
        pllGpInterruptPin1MaskByte |= (pllGpInterruptPin1Mask << PLL_AUX_GP_INTERRUPT_SHIFT);

        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllName,
                         "Invalid PLL encountered while attempting to set PLL GP Interrupt mask");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /*Update GP Interrupt Mask Byte 5*/
    gpInterruptPin0Mask |= ((uint64_t)pllGpInterruptPin0MaskByte << PLL_GP_INTERRUPT_BYTE_SHIFT);
    gpInterruptPin1Mask |= ((uint64_t)pllGpInterruptPin1MaskByte << PLL_GP_INTERRUPT_BYTE_SHIFT);

    /*Write GP Interrupt Pin0 and Pin1 mask*/
    recoveryAction = adrv9025_CoreGpInterruptsMaskPin0BfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            gpInterruptPin0Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreGpInterruptsMaskPin1BfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            gpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_PllOverrangeGpInterruptMaskGet(adi_adrv9025_Device_t* device,
                                                adi_adrv9025_PllName_e pllName,
                                                uint8_t*               pllGpInterruptPin0Mask,
                                                uint8_t*               pllGpInterruptPin1Mask)
{
    static const uint8_t  PLL_LO1_GP_INTERRUPT_MASK   = 0x08;
    static const uint8_t  PLL_LO2_GP_INTERRUPT_MASK   = 0x04;
    static const uint8_t  PLL_AUX_GP_INTERRUPT_MASK   = 0x02;
    static const uint8_t  PLL_LO1_GP_INTERRUPT_SHIFT  = 3;
    static const uint8_t  PLL_LO2_GP_INTERRUPT_SHIFT  = 2;
    static const uint8_t  PLL_AUX_GP_INTERRUPT_SHIFT  = 1;
    static const uint64_t PLL_GP_INTERRUPT_BYTE_MASK  = 0x0000FF0000000000;
    static const uint8_t  PLL_GP_INTERRUPT_BYTE_SHIFT = 40;

    int32_t  recoveryAction             = ADI_COMMON_ACT_NO_ACTION;
    uint64_t gpInterruptPin0Mask        = 0;
    uint64_t gpInterruptPin1Mask        = 0;
    uint8_t  pllGpInterruptPin0MaskByte = 0;
    uint8_t  pllGpInterruptPin1MaskByte = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        pllGpInterruptPin0Mask);

    ADI_NULL_PTR_RETURN(&device->common,
                        pllGpInterruptPin1Mask);

    /*Read GP Interrupt Pin0 and Pin1 mask*/
    recoveryAction = adrv9025_CoreGpInterruptsMaskPin0BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInterruptPin0Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreGpInterruptsMaskPin1BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*PLL mask status is the 5th Byte*/
    pllGpInterruptPin0MaskByte = (uint8_t)((gpInterruptPin0Mask & PLL_GP_INTERRUPT_BYTE_MASK) >> PLL_GP_INTERRUPT_BYTE_SHIFT);
    pllGpInterruptPin1MaskByte = (uint8_t)((gpInterruptPin1Mask & PLL_GP_INTERRUPT_BYTE_MASK) >> PLL_GP_INTERRUPT_BYTE_SHIFT);

    switch (pllName)
    {
    case(ADI_ADRV9025_LO1_PLL):
    {
        *pllGpInterruptPin0Mask = (pllGpInterruptPin0MaskByte & PLL_LO1_GP_INTERRUPT_MASK) >> PLL_LO1_GP_INTERRUPT_SHIFT;
        *pllGpInterruptPin1Mask = (pllGpInterruptPin1MaskByte & PLL_LO1_GP_INTERRUPT_MASK) >> PLL_LO1_GP_INTERRUPT_SHIFT;
        break;
    }
    case(ADI_ADRV9025_LO2_PLL):
    {
        *pllGpInterruptPin0Mask = (pllGpInterruptPin0MaskByte & PLL_LO2_GP_INTERRUPT_MASK) >> PLL_LO2_GP_INTERRUPT_SHIFT;
        *pllGpInterruptPin1Mask = (pllGpInterruptPin1MaskByte & PLL_LO2_GP_INTERRUPT_MASK) >> PLL_LO2_GP_INTERRUPT_SHIFT;
        break;
    }
    case(ADI_ADRV9025_AUX_PLL):
    {
        *pllGpInterruptPin0Mask = (pllGpInterruptPin0MaskByte & PLL_AUX_GP_INTERRUPT_MASK) >> PLL_AUX_GP_INTERRUPT_SHIFT;
        *pllGpInterruptPin1Mask = (pllGpInterruptPin1MaskByte & PLL_AUX_GP_INTERRUPT_MASK) >> PLL_AUX_GP_INTERRUPT_SHIFT;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllName,
                         "Invalid PLL encountered while attempting to retrieve PLL overrange GP Interrupt mask");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    return recoveryAction;
}

int32_t adrv9025_PllOverrangeGpInterruptMaskSet(adi_adrv9025_Device_t* device,
                                                adi_adrv9025_PllName_e pllName,
                                                uint8_t                pllGpInterruptPin0Mask,
                                                uint8_t                pllGpInterruptPin1Mask)
{
    static const uint8_t  PLL_LO1_GP_INTERRUPT_MASK   = 0x08;
    static const uint8_t  PLL_LO2_GP_INTERRUPT_MASK   = 0x04;
    static const uint8_t  PLL_AUX_GP_INTERRUPT_MASK   = 0x02;
    static const uint8_t  PLL_LO1_GP_INTERRUPT_SHIFT  = 3;
    static const uint8_t  PLL_LO2_GP_INTERRUPT_SHIFT  = 2;
    static const uint8_t  PLL_AUX_GP_INTERRUPT_SHIFT  = 1;
    static const uint64_t PLL_GP_INTERRUPT_BYTE_MASK  = 0x0000FF0000000000;
    static const uint8_t  PLL_GP_INTERRUPT_BYTE_SHIFT = 40;

    int32_t  recoveryAction             = ADI_COMMON_ACT_NO_ACTION;
    uint64_t gpInterruptPin0Mask        = 0;
    uint64_t gpInterruptPin1Mask        = 0;
    uint8_t  pllGpInterruptPin0MaskByte = 0;
    uint8_t  pllGpInterruptPin1MaskByte = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Read GP Interrupt Pin0 and Pin1 mask*/
    recoveryAction = adrv9025_CoreGpInterruptsMaskPin0BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInterruptPin0Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreGpInterruptsMaskPin1BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*PLL mask status is the 5th Byte*/
    pllGpInterruptPin0MaskByte = (uint8_t)((gpInterruptPin0Mask & PLL_GP_INTERRUPT_BYTE_MASK) >> PLL_GP_INTERRUPT_BYTE_SHIFT);
    pllGpInterruptPin1MaskByte = (uint8_t)((gpInterruptPin1Mask & PLL_GP_INTERRUPT_BYTE_MASK) >> PLL_GP_INTERRUPT_BYTE_SHIFT);

    /*Clear Byte 5 of the GP Interrupt Mask*/
    gpInterruptPin0Mask &= ~PLL_GP_INTERRUPT_BYTE_MASK;
    gpInterruptPin1Mask &= ~PLL_GP_INTERRUPT_BYTE_MASK;

    switch (pllName)
    {
    case(ADI_ADRV9025_LO1_PLL):
    {
        /*Clear LO1 Interrupt Mask bit in GP Interrupt byte5*/
        pllGpInterruptPin0MaskByte &= ~PLL_LO1_GP_INTERRUPT_MASK;
        pllGpInterruptPin1MaskByte &= ~PLL_LO1_GP_INTERRUPT_MASK;

        /*Update PLL GP Interrupt Byte*/
        pllGpInterruptPin0MaskByte |= (pllGpInterruptPin0Mask << PLL_LO1_GP_INTERRUPT_SHIFT);
        pllGpInterruptPin1MaskByte |= (pllGpInterruptPin1Mask << PLL_LO1_GP_INTERRUPT_SHIFT);

        break;
    }
    case(ADI_ADRV9025_LO2_PLL):
    {
        /*Clear LO2 Interrupt Mask bit in GP Interrupt byte5*/
        pllGpInterruptPin0MaskByte &= ~PLL_LO2_GP_INTERRUPT_MASK;
        pllGpInterruptPin1MaskByte &= ~PLL_LO2_GP_INTERRUPT_MASK;

        /*Update PLL GP Interrupt Byte*/
        pllGpInterruptPin0MaskByte |= (pllGpInterruptPin0Mask << PLL_LO2_GP_INTERRUPT_SHIFT);
        pllGpInterruptPin1MaskByte |= (pllGpInterruptPin1Mask << PLL_LO2_GP_INTERRUPT_SHIFT);

        break;
    }
    case(ADI_ADRV9025_AUX_PLL):
    {
        /*Clear Aux PLL Interrupt Mask bit in GP Interrupt byte5*/
        pllGpInterruptPin0MaskByte &= ~PLL_AUX_GP_INTERRUPT_MASK;
        pllGpInterruptPin1MaskByte &= ~PLL_AUX_GP_INTERRUPT_MASK;

        /*Update PLL GP Interrupt Byte*/
        pllGpInterruptPin0MaskByte |= (pllGpInterruptPin0Mask << PLL_AUX_GP_INTERRUPT_SHIFT);
        pllGpInterruptPin1MaskByte |= (pllGpInterruptPin1Mask << PLL_AUX_GP_INTERRUPT_SHIFT);

        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pllName,
                         "Invalid PLL encountered while attempting to set PLL overrange GP Interrupt mask");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /*Update GP Interrupt Mask Byte 5*/
    gpInterruptPin0Mask |= ((uint64_t)pllGpInterruptPin0MaskByte << PLL_GP_INTERRUPT_BYTE_SHIFT);
    gpInterruptPin1Mask |= ((uint64_t)pllGpInterruptPin1MaskByte << PLL_GP_INTERRUPT_BYTE_SHIFT);

    /*Write GP Interrupt Pin0 and Pin1 mask*/
    recoveryAction = adrv9025_CoreGpInterruptsMaskPin0BfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            gpInterruptPin0Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreGpInterruptsMaskPin1BfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            gpInterruptPin1Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_RxRadioCtrlCfgSet(adi_adrv9025_Device_t*             device,
                                   adi_adrv9025_RxRadioCtrlModeCfg_t* rxRadioCtrlModeCfg)
{
    static const uint8_t RX_PIN_MODE_ENABLE        = 1;
    static const uint8_t RX_PIN_MODE_DISABLE       = 0;
    static const uint8_t ARM_OVERRIDE_CTRL_DISABLE = 0;

    int32_t                   recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                  rxChannelIndex        = 0;
    adrv9025_BfRxChanAddr_e   rxChannelBitfieldAddr = ADRV9025_BF_RX_CH0;
    adi_adrv9025_RxChannels_e rxChannelArr[]        = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxRadioCtrlModeCfg);

    for (rxChannelIndex = 0; rxChannelIndex < (sizeof(rxChannelArr) / sizeof(rxChannelArr[0])); rxChannelIndex++)
    {
        if ((rxRadioCtrlModeCfg->rxChannelMask & rxChannelArr[rxChannelIndex]) == rxChannelArr[rxChannelIndex])
        {
            /*Retrieve Rx Channel Bitfield Address*/
            recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                           rxChannelArr[rxChannelIndex],
                                                           &rxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*For Non-pin mode set Pin Mode bitfield to 0 and for pin mode set the Pin mode bitfield to 1*/
            if (rxRadioCtrlModeCfg->rxEnableMode == ADI_ADRV9025_RX_EN_SPI_MODE)
            {
                /* Disable ARM override ctrl for Rx ctrl configuration */
                recoveryAction = adrv9025_RxArmOverrideControlBfSet(device,
                                                                    rxChannelBitfieldAddr,
                                                                    ARM_OVERRIDE_CTRL_DISABLE);
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Disable Pin ctrl mode */
                recoveryAction = adrv9025_RxRxPinModeBfSet(device,
                                                           rxChannelBitfieldAddr,
                                                           RX_PIN_MODE_DISABLE);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else if (rxRadioCtrlModeCfg->rxEnableMode == ADI_ADRV9025_RX_EN_PIN_MODE)
            {
                /* Disable ARM override ctrl for Rx ctrl configuration */
                recoveryAction = adrv9025_RxArmOverrideControlBfSet(device,
                                                                    rxChannelBitfieldAddr,
                                                                    ARM_OVERRIDE_CTRL_DISABLE);
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Enable Pin ctrl mode */
                recoveryAction = adrv9025_RxRxPinModeBfSet(device,
                                                           rxChannelBitfieldAddr,
                                                           RX_PIN_MODE_ENABLE);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxRadioCtrlModeCfg->rxEnableMode,
                                 "Invalid Rx Enable Mode encountered while attempting to configure Rx radio ctrl configuration settings");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return recoveryAction;
}

int32_t adrv9025_TxRadioCtrlCfgSet(adi_adrv9025_Device_t*             device,
                                   adi_adrv9025_TxRadioCtrlModeCfg_t* txRadioCtrlModeCfg)
{
    static const uint8_t TX_PIN_MODE_ENABLE        = 1;
    static const uint8_t TX_PIN_MODE_DISABLE       = 0;
    static const uint8_t ARM_OVERRIDE_CTRL_DISABLE = 0;

    int32_t                   recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                  txChannelIndex        = 0;
    adrv9025_BfTxChanAddr_e   txChannelBitfieldAddr = ADRV9025_BF_TX_CH0;
    adi_adrv9025_TxChannels_e txChannelArr[]        = {ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TX3, ADI_ADRV9025_TX4};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        txRadioCtrlModeCfg);

    for (txChannelIndex = 0; txChannelIndex < (sizeof(txChannelArr) / sizeof(txChannelArr[0])); txChannelIndex++)
    {
        if ((txRadioCtrlModeCfg->txChannelMask & txChannelArr[txChannelIndex]) == txChannelArr[txChannelIndex])
        {
            /*Retrieve Tx Channel Bitfield Address*/
            txChannelBitfieldAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                                   (uint32_t)txChannelArr[txChannelIndex]);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*For Non-pin mode set Pin Mode bitfield to 0 and for pin mode set the Pin mode bitfield to 1*/
            if (txRadioCtrlModeCfg->txEnableMode == ADI_ADRV9025_TX_EN_SPI_MODE)
            {
                /* Disable ARM override ctrl for Tx ctrl configuration */
                recoveryAction = adrv9025_TxArmOverrideControlBfSet(device,
                                                                    txChannelBitfieldAddr,
                                                                    ARM_OVERRIDE_CTRL_DISABLE);
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Disable Pin Mode ctrl */
                recoveryAction = adrv9025_TxTxPinModeBfSet(device,
                                                           txChannelBitfieldAddr,
                                                           TX_PIN_MODE_DISABLE);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else if (txRadioCtrlModeCfg->txEnableMode == ADI_ADRV9025_TX_EN_PIN_MODE)
            {
                /* Disable ARM override ctrl for Tx ctrl configuration */
                recoveryAction = adrv9025_TxArmOverrideControlBfSet(device,
                                                                    txChannelBitfieldAddr,
                                                                    ARM_OVERRIDE_CTRL_DISABLE);
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Enable pin mode ctrl */
                recoveryAction = adrv9025_TxTxPinModeBfSet(device,
                                                           txChannelBitfieldAddr,
                                                           TX_PIN_MODE_ENABLE);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 txRadioCtrlModeCfg->txEnableMode,
                                 "Invalid Tx Enable Mode encountered while attempting to configure Tx radio ctrl configuration settings");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return recoveryAction;
}

int32_t adrv9025_OrxRadioCtrlCfgSet(adi_adrv9025_Device_t*              device,
                                    adi_adrv9025_ORxRadioCtrlModeCfg_t* orxRadioCtrlModeCfg)
{
    static const uint8_t ORX_PIN_MODE_ENABLE                   = 1;
    static const uint8_t ORX_PIN_MODE_DISABLE                  = 0;
    static const uint8_t ORX_SINGLE_CH_PIN_MODE_ENABLE         = 1;
    static const uint8_t ORX_SINGLE_CH_PIN_MODE_DISABLE        = 0;
    static const uint8_t ORX_SINGLE_CH_PIN_MODE1A_ENABLE       = 1;
    static const uint8_t ORX_SINGLE_CH_PIN_MODE1A_DISABLE      = 0;
    static const uint8_t ORX_PIN_MODE_USING_SPI_SELECT_ENABLE  = 1;
    static const uint8_t ORX_PIN_MODE_USING_SPI_SELECT_DISABLE = 0;
    static const uint8_t ARM_OVERRIDE_CTRL_DISABLE             = 0;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        orxRadioCtrlModeCfg);

    if (orxRadioCtrlModeCfg->orxEnableMode == ADI_ADRV9025_ORX_EN_SPI_MODE)
    {
        recoveryAction = adrv9025_CoreArmOverrideControlBfSet(device,
                                                              ADRV9025_BF_CORE,
                                                              ARM_OVERRIDE_CTRL_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeEnableBfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            ORX_PIN_MODE_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (orxRadioCtrlModeCfg->orxEnableMode == ADI_ADRV9025_ORX_EN_SINGLE_CH_3PIN_MODE)
    {
        recoveryAction = adrv9025_CoreArmOverrideControlBfSet(device,
                                                              ADRV9025_BF_CORE,
                                                              ARM_OVERRIDE_CTRL_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeEnableBfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            ORX_PIN_MODE_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelMode1aBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  ORX_SINGLE_CH_PIN_MODE1A_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelModeBfSet(device,
                                                                ADRV9025_BF_CORE,
                                                                ORX_SINGLE_CH_PIN_MODE_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeUsingSpiSelectBfSet(device,
                                                                    ADRV9025_BF_CORE,
                                                                    ORX_PIN_MODE_USING_SPI_SELECT_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CorePinSelectSettlingDelayBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  orxRadioCtrlModeCfg->orxPinSelectSettlingDelay_armClkCycles);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (orxRadioCtrlModeCfg->orxEnableMode == ADI_ADRV9025_ORX_EN_SINGLE_CH_2PIN_MODE)
    {
        recoveryAction = adrv9025_CoreArmOverrideControlBfSet(device,
                                                              ADRV9025_BF_CORE,
                                                              ARM_OVERRIDE_CTRL_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeEnableBfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            ORX_PIN_MODE_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelMode1aBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  ORX_SINGLE_CH_PIN_MODE1A_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelModeBfSet(device,
                                                                ADRV9025_BF_CORE,
                                                                ORX_SINGLE_CH_PIN_MODE_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeUsingSpiSelectBfSet(device,
                                                                    ADRV9025_BF_CORE,
                                                                    ORX_PIN_MODE_USING_SPI_SELECT_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelMode1aLowChannelEnableBfSet(device,
                                                                                  ADRV9025_BF_CORE,
                                                                                  (uint8_t)orxRadioCtrlModeCfg->singleChannel2PinModeLowOrxSel);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelMode1aHighChannelEnableBfSet(device,
                                                                                   ADRV9025_BF_CORE,
                                                                                   (uint8_t)orxRadioCtrlModeCfg->singleChannel2PinModeHighOrxSel);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CorePinSelectSettlingDelayBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  orxRadioCtrlModeCfg->orxPinSelectSettlingDelay_armClkCycles);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (orxRadioCtrlModeCfg->orxEnableMode == ADI_ADRV9025_ORX_EN_SINGLE_CH_1PIN_MODE)
    {
        recoveryAction = adrv9025_CoreArmOverrideControlBfSet(device,
                                                              ADRV9025_BF_CORE,
                                                              ARM_OVERRIDE_CTRL_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeEnableBfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            ORX_PIN_MODE_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelMode1aBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  ORX_SINGLE_CH_PIN_MODE1A_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelModeBfSet(device,
                                                                ADRV9025_BF_CORE,
                                                                ORX_SINGLE_CH_PIN_MODE_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeUsingSpiSelectBfSet(device,
                                                                    ADRV9025_BF_CORE,
                                                                    ORX_PIN_MODE_USING_SPI_SELECT_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSelectInPinModeBfSet(device,
                                                              ADRV9025_BF_CORE,
                                                              (uint8_t)orxRadioCtrlModeCfg->singleChannel1PinModeOrxSel);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CorePinSelectSettlingDelayBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  orxRadioCtrlModeCfg->orxPinSelectSettlingDelay_armClkCycles);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (orxRadioCtrlModeCfg->orxEnableMode == ADI_ADRV9025_ORX_EN_DUAL_CH_4PIN_MODE)
    {
        recoveryAction = adrv9025_CoreArmOverrideControlBfSet(device,
                                                              ADRV9025_BF_CORE,
                                                              ARM_OVERRIDE_CTRL_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeEnableBfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            ORX_PIN_MODE_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelMode1aBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  ORX_SINGLE_CH_PIN_MODE1A_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelModeBfSet(device,
                                                                ADRV9025_BF_CORE,
                                                                ORX_SINGLE_CH_PIN_MODE_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeUsingSpiSelectBfSet(device,
                                                                    ADRV9025_BF_CORE,
                                                                    ORX_PIN_MODE_USING_SPI_SELECT_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CorePinSelectSettlingDelayBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  orxRadioCtrlModeCfg->orxPinSelectSettlingDelay_armClkCycles);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (orxRadioCtrlModeCfg->orxEnableMode == ADI_ADRV9025_ORX_EN_DUAL_CH_2PIN_MODE)
    {
        recoveryAction = adrv9025_CoreArmOverrideControlBfSet(device,
                                                              ADRV9025_BF_CORE,
                                                              ARM_OVERRIDE_CTRL_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeEnableBfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            ORX_PIN_MODE_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelMode1aBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  ORX_SINGLE_CH_PIN_MODE1A_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSingleChannelModeBfSet(device,
                                                                ADRV9025_BF_CORE,
                                                                ORX_SINGLE_CH_PIN_MODE_DISABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxPinModeUsingSpiSelectBfSet(device,
                                                                    ADRV9025_BF_CORE,
                                                                    ORX_PIN_MODE_USING_SPI_SELECT_ENABLE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreOrxSelectInPinModeBfSet(device,
                                                              ADRV9025_BF_CORE,
                                                              (uint8_t)orxRadioCtrlModeCfg->dualChannel2PinModeOrxSel);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CorePinSelectSettlingDelayBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  orxRadioCtrlModeCfg->orxPinSelectSettlingDelay_armClkCycles);
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
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         orxRadioCtrlModeCfg->orxEnableMode,
                         "Invalid ORx Enable Mode encountered while attempting to configure ORx radio ctrl configuration settings");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_RxRadioCtrlCfgGet(adi_adrv9025_Device_t*             device,
                                   adi_adrv9025_RxChannels_e          rxChannel,
                                   adi_adrv9025_RxRadioCtrlModeCfg_t* rxRadioCtrlModeCfg)
{
    int32_t                   recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                  rxChannelIndex        = 0;
    uint8_t                   rxEnableMode          = (uint8_t)ADI_ADRV9025_RX_EN_INVALID_MODE;
    adrv9025_BfRxChanAddr_e   rxChannelBitfieldAddr = ADRV9025_BF_RX_CH0;
    uint8_t                   armOverrideCtrl       = 0;
    adi_adrv9025_RxChannels_e rxChannelArr[]        = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxRadioCtrlModeCfg);

    rxRadioCtrlModeCfg->rxChannelMask = 0;

    for (rxChannelIndex = 0; rxChannelIndex < (sizeof(rxChannelArr) / sizeof(rxChannelArr[0])); rxChannelIndex++)
    {
        if ((rxChannel & rxChannelArr[rxChannelIndex]) == rxChannelArr[rxChannelIndex])
        {
            /*Retrieve Rx Channel Bitfield Address*/
            recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                           rxChannelArr[rxChannelIndex],
                                                           &rxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Read ARM Override ctrl bitfield*/
            recoveryAction = adrv9025_RxArmOverrideControlBfGet(device,
                                                                rxChannelBitfieldAddr,
                                                                &armOverrideCtrl);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* The pin mode bitfield is only valid if ARM override ctrl is not set to 1 */
            if (armOverrideCtrl == 0)
            {
                /*For Non-pin mode Pin Mode bitfield is set to 0 and for pin mode the 'Pin mode' bitfield is set to 1*/
                recoveryAction = adrv9025_RxRxPinModeBfGet(device,
                                                           rxChannelBitfieldAddr,
                                                           &rxEnableMode);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /*Update the Rx channel mask*/
            rxRadioCtrlModeCfg->rxChannelMask |= rxChannel;

            break;
        }
    }

    /*Update the enable mode struct member*/
    rxRadioCtrlModeCfg->rxEnableMode = (adi_adrv9025_RxEnableMode_e)rxEnableMode;

    return recoveryAction;
}

int32_t adrv9025_TxRadioCtrlCfgGet(adi_adrv9025_Device_t*             device,
                                   adi_adrv9025_TxChannels_e          txChannel,
                                   adi_adrv9025_TxRadioCtrlModeCfg_t* txRadioCtrlModeCfg)
{
    int32_t                   recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                  txChannelIndex        = 0;
    uint8_t                   armOverrideCtrl       = 0;
    uint8_t                   txEnableMode          = (uint8_t)ADI_ADRV9025_TX_EN_INVALID_MODE;
    adrv9025_BfTxChanAddr_e   txChannelBitfieldAddr = ADRV9025_BF_TX_CH0;
    adi_adrv9025_TxChannels_e txChannelArr[]        = {ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TX3, ADI_ADRV9025_TX4};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        txRadioCtrlModeCfg);

    txRadioCtrlModeCfg->txChannelMask = 0;

    for (txChannelIndex = 0; txChannelIndex < (sizeof(txChannelArr) / sizeof(txChannelArr[0])); txChannelIndex++)
    {
        if ((txChannel & txChannelArr[txChannelIndex]) == txChannelArr[txChannelIndex])
        {
            /*Retrieve Tx Channel Bitfield Address*/
            txChannelBitfieldAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device,
                                                                                   (uint32_t)txChannelArr[txChannelIndex]);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Read ARM Override ctrl bitfield*/
            recoveryAction = adrv9025_TxArmOverrideControlBfGet(device,
                                                                txChannelBitfieldAddr,
                                                                &armOverrideCtrl);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* The pin mode bitfield is only valid if ARM override ctrl is not set to 1 */
            if (armOverrideCtrl == 0)
            {
                /*For Non-pin mode Pin Mode bitfield is set to 0 and for pin mode the 'Pin mode' bitfield is set to 1*/
                recoveryAction = adrv9025_TxTxPinModeBfGet(device,
                                                           txChannelBitfieldAddr,
                                                           &txEnableMode);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /*Update the Rx channel mask*/
            txRadioCtrlModeCfg->txChannelMask |= txChannel;

            break;
        }
    }

    /*Update the enable mode struct member*/
    txRadioCtrlModeCfg->txEnableMode = (adi_adrv9025_TxEnableMode_e)txEnableMode;

    return recoveryAction;
}

int32_t adrv9025_OrxRadioCtrlCfgGet(adi_adrv9025_Device_t*              device,
                                    adi_adrv9025_ORxRadioCtrlModeCfg_t* orxRadioCtrlModeCfg)
{
    int32_t                      recoveryAction                  = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                      singleChannelPinModeSel         = 0;
    uint8_t                      singleChannelPinMode1aSel       = 0;
    uint8_t                      orxPinModeSel                   = 0;
    uint8_t                      armOverrideCtrl                 = 0;
    uint8_t                      orxPinModeUsingSpiSelect        = 0;
    uint8_t                      singleChannel1PinModeOrxSel     = (uint8_t)ADI_ADRV9025_SINGLE_CH_PIN_MODE_INVALID_ORX_SEL;
    uint8_t                      singleChannel2PinModeHighOrxSel = (uint8_t)ADI_ADRV9025_SINGLE_CH_PIN_MODE_INVALID_ORX_SEL;
    uint8_t                      singleChannel2PinModeLowOrxSel  = (uint8_t)ADI_ADRV9025_SINGLE_CH_PIN_MODE_INVALID_ORX_SEL;
    uint8_t                      dualChannel2PinModeOrxSel       = (uint8_t)ADI_ADRV9025_DUAL_CH_PIN_MODE_INVALID_ORX_SEL;
    adi_adrv9025_ORxEnableMode_e orxEnableMode                   = ADI_ADRV9025_ORX_EN_INVALID_MODE;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        orxRadioCtrlModeCfg);

    recoveryAction = adrv9025_CoreArmOverrideControlBfGet(device,
                                                          ADRV9025_BF_CORE,
                                                          &armOverrideCtrl);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreOrxPinModeEnableBfGet(device,
                                                        ADRV9025_BF_CORE,
                                                        &orxPinModeSel);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreOrxSingleChannelModeBfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &singleChannelPinModeSel);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreOrxSingleChannelMode1aBfGet(device,
                                                              ADRV9025_BF_CORE,
                                                              &singleChannelPinMode1aSel);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreOrxPinModeUsingSpiSelectBfGet(device,
                                                                ADRV9025_BF_CORE,
                                                                &orxPinModeUsingSpiSelect);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (armOverrideCtrl == 1)
    {
        orxEnableMode = ADI_ADRV9025_ORX_EN_INVALID_MODE;
    }
    else
    {
        if (orxPinModeSel == 0)
        {
            orxEnableMode = ADI_ADRV9025_ORX_EN_SPI_MODE;
        }
        else
        {
            if ((singleChannelPinModeSel == 1) &&
                (singleChannelPinMode1aSel == 0) &&
                (orxPinModeUsingSpiSelect == 0))
            {
                orxEnableMode = ADI_ADRV9025_ORX_EN_SINGLE_CH_3PIN_MODE;
            }
            else if ((singleChannelPinModeSel == 1) &&
                (singleChannelPinMode1aSel == 0) &&
                (orxPinModeUsingSpiSelect == 1))
            {
                orxEnableMode = ADI_ADRV9025_ORX_EN_SINGLE_CH_1PIN_MODE;

                recoveryAction = adrv9025_CoreOrxSelectInPinModeBfGet(device,
                                                                      ADRV9025_BF_CORE,
                                                                      &singleChannel1PinModeOrxSel);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else if ((singleChannelPinModeSel == 1) &&
                (singleChannelPinMode1aSel == 1) &&
                (orxPinModeUsingSpiSelect == 0))
            {
                orxEnableMode = ADI_ADRV9025_ORX_EN_SINGLE_CH_2PIN_MODE;

                recoveryAction = adrv9025_CoreOrxSingleChannelMode1aHighChannelEnableBfGet(device,
                                                                                           ADRV9025_BF_CORE,
                                                                                           &singleChannel2PinModeHighOrxSel);
                ADI_ERROR_RETURN(device->common.error.newAction);

                recoveryAction = adrv9025_CoreOrxSingleChannelMode1aLowChannelEnableBfGet(device,
                                                                                          ADRV9025_BF_CORE,
                                                                                          &singleChannel2PinModeLowOrxSel);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else if ((singleChannelPinModeSel == 0) &&
                (singleChannelPinMode1aSel == 0) &&
                (orxPinModeUsingSpiSelect == 1))
            {
                orxEnableMode = ADI_ADRV9025_ORX_EN_DUAL_CH_2PIN_MODE;

                recoveryAction = adrv9025_CoreOrxSelectInPinModeBfGet(device,
                                                                      ADRV9025_BF_CORE,
                                                                      &dualChannel2PinModeOrxSel);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else if ((singleChannelPinModeSel == 0) &&
                (singleChannelPinMode1aSel == 0) &&
                (orxPinModeUsingSpiSelect == 0))
            {
                orxEnableMode = ADI_ADRV9025_ORX_EN_DUAL_CH_4PIN_MODE;
            }
        }
    }

    /* Update the orxRadioCtrlModeCfg structure members with readback/decoded values */
    recoveryAction = adrv9025_CorePinSelectSettlingDelayBfGet(device,
                                                              ADRV9025_BF_CORE,
                                                              &orxRadioCtrlModeCfg->orxPinSelectSettlingDelay_armClkCycles);
    ADI_ERROR_RETURN(device->common.error.newAction);

    orxRadioCtrlModeCfg->orxEnableMode                   = orxEnableMode;
    orxRadioCtrlModeCfg->singleChannel1PinModeOrxSel     = (adi_adrv9025_SingleChannelPinModeOrxSel_e)singleChannel1PinModeOrxSel;
    orxRadioCtrlModeCfg->singleChannel2PinModeHighOrxSel = (adi_adrv9025_SingleChannelPinModeOrxSel_e)singleChannel2PinModeHighOrxSel;
    orxRadioCtrlModeCfg->singleChannel2PinModeLowOrxSel  = (adi_adrv9025_SingleChannelPinModeOrxSel_e)singleChannel2PinModeLowOrxSel;
    orxRadioCtrlModeCfg->dualChannel2PinModeOrxSel       = (adi_adrv9025_DualChannelPinModeOrxSel_e)dualChannel2PinModeOrxSel;

    return recoveryAction;
}

int32_t adrv9025_ArmGpioPinSet(adi_adrv9025_Device_t*        device,
                               uint8_t                       signalId,
                               adi_adrv9025_ArmGpioPinCfg_t* armGpioPin)
{
#define NUM_GPIOS_PER_CMD 1U
    static const uint8_t GPIO_ENABLE            = 0x80;
    static const uint8_t GPIO_DISABLE           = 0x00;
    static const uint8_t GPIO_POLARITY_NORMAL   = 0x00;
    static const uint8_t GPIO_POLARITY_INVERTED = 0x20;

    int32_t                   recoveryAction                       = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                   resourceAcqReleaseStatus             = ADI_FAILURE;
    adi_adrv9025_GpioPinSel_e currentGpioPinSel                    = ADI_ADRV9025_GPIO_INVALID;
    int32_t                   sharedResourceArr[NUM_GPIOS_PER_CMD] = {0};
    uint32_t                  gpioPinMask                          = 0;
    uint8_t                   extData[3]                           = {0};
    uint8_t                   cmdStatusByte                        = 0;
    int32_t                   errHdl                               = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        armGpioPin);

    /* Retrieve the currently assigned GPIO */
    recoveryAction = adrv9025_ArmGpioPinGet(device,
                                            signalId,
                                            &currentGpioPinSel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Disassociate currently assigned GPIO to the signal and release the shared resource */
    if (currentGpioPinSel < ADI_ADRV9025_GPIO_INVALID)
    {
        /* Command ARM to disassociate the currently assigned GPIO for the requested signal ID */
        extData[0]     = ADRV9025_CPU_OBJECTID_GPIO_CTRL;
        extData[1]     = signalId;
        extData[2]     = ((uint8_t)currentGpioPinSel | GPIO_POLARITY_NORMAL | GPIO_DISABLE);
        recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                                  ADI_ADRV9025_CPU_TYPE_C,
                                                  ADRV9025_CPU_SET_OPCODE,
                                                  &extData[0],
                                                  sizeof(extData));
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Wait for command to finish executing */
        recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                       ADI_ADRV9025_CPU_TYPE_C,
                                                       (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                       &cmdStatusByte,
                                                       (uint32_t)ADI_ADRV9025_SETARMGPIO_TIMEOUT_US,
                                                       (uint32_t)ADI_ADRV9025_SETARMGPIO_INTERVAL_US);

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

        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Release the shared GPIO resource */
        sharedResourceArr[0] = (int32_t)currentGpioPinSel;
        recoveryAction       = adrv9025_SharedResourcesRelease(device,
                                                               ADRV9025_SHARED_RESOURCE_GPIO,
                                                               &sharedResourceArr[0],
                                                               NUM_GPIOS_PER_CMD,
                                                               ADRV9025_FEATURE_ARM_GPIO_PIN,
                                                               &resourceAcqReleaseStatus);

        if (resourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             currentGpioPinSel,
                             "Unable to release GPIO pin currently assigned to requested signal ID. Please check if it is in use by another feature");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Set the direction of Released GPIO to input */
        gpioPinMask    = ((uint32_t)0x00000001 << (uint8_t)currentGpioPinSel);
        recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                      gpioPinMask);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Associate requested signal ID with the GPIO pin selected and acquire the shared GPIO resource */
    if ((armGpioPin->enable > 0) &&
        (armGpioPin->gpioPinSel < ADI_ADRV9025_GPIO_INVALID))
    {
        /* Acquire shared GPIO resource */
        sharedResourceArr[0] = (int32_t)armGpioPin->gpioPinSel;
        recoveryAction       = adrv9025_SharedResourcesAcquire(device,
                                                               ADRV9025_SHARED_RESOURCE_GPIO,
                                                               &sharedResourceArr[0],
                                                               NUM_GPIOS_PER_CMD,
                                                               ADRV9025_FEATURE_ARM_GPIO_PIN,
                                                               &resourceAcqReleaseStatus);

        if (resourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             armGpioPin->gpioPinSel,
                             "Unable to acquire GPIO pin currently assigned to requested signal ID. Please check if it is in use by another feature");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Set the direction of Acquired GPIO to input */
        gpioPinMask    = ((uint32_t)0x00000001 << (uint8_t)armGpioPin->gpioPinSel);
        recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                      gpioPinMask);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Command ARM to associate the currently assigned GPIO for the requested signal ID */
        extData[0] = ADRV9025_CPU_OBJECTID_GPIO_CTRL;
        extData[1] = signalId;
        extData[2] = ((uint8_t)armGpioPin->gpioPinSel | GPIO_ENABLE);

        if (armGpioPin->polarity > 0)
        {
            extData[2] |= GPIO_POLARITY_INVERTED;
        }
        else
        {
            extData[2] &= ~GPIO_POLARITY_INVERTED;
        }

        recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                                  ADI_ADRV9025_CPU_TYPE_C,
                                                  ADRV9025_CPU_SET_OPCODE,
                                                  &extData[0],
                                                  sizeof(extData));
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Wait for command to finish executing */
        recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                       ADI_ADRV9025_CPU_TYPE_C,
                                                       (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                       &cmdStatusByte,
                                                       (uint32_t)ADI_ADRV9025_SETARMGPIO_TIMEOUT_US,
                                                       (uint32_t)ADI_ADRV9025_SETARMGPIO_INTERVAL_US);

        /* If cmdStatusByte is non-zero then flag an ARM error and release the acquired shared resource */
        if ((cmdStatusByte >> 1) > 0)
        {
            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                                             &sharedResourceArr[0],
                                                             NUM_GPIOS_PER_CMD,
                                                             ADRV9025_FEATURE_ARM_GPIO_PIN,
                                                             &resourceAcqReleaseStatus);

            if (resourceAcqReleaseStatus == ADI_FAILURE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_ADRV9025_ACT_ERR_RESET_CPU,
                                 armGpioPin->gpioPinSel,
                                 "Unable to release currently acquired GPIO shared resource for ARM GPIO functionality due to an ARM error");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Set the direction of Released GPIO to input */
            gpioPinMask    = ((uint32_t)0x00000001 << (uint8_t)armGpioPin->gpioPinSel);
            recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                          gpioPinMask);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                         ADI_ADRV9025_CPU_TYPE_C,
                                                         errHdl,
                                                         ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                                  extData[0],
                                                                                  cmdStatusByte),
                                                         recoveryAction);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ArmGpioPinGet(adi_adrv9025_Device_t*     device,
                               uint8_t                    signalId,
                               adi_adrv9025_GpioPinSel_e* gpioPinSel)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;
    static const uint8_t ARM_GPIO_MASK         = 0x1F;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[2]     = {0};
    uint8_t armData[1]     = {0};
    uint8_t cmdStatusByte  = 0;
    int32_t errHdl         = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        gpioPinSel);

    /* Command ARM to return the currently assigned GPIO for the requested signal ID */
    extData[0]     = ADRV9025_CPU_OBJECTID_GPIO_CTRL;
    extData[1]     = signalId;
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETARMGPIO_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETARMGPIO_INTERVAL_US);

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

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read GPIO Pin sel from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                             &armData[0],
                                             sizeof(armData),
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Return the GPIO data read back from ARM mailbox */
    *gpioPinSel = (adi_adrv9025_GpioPinSel_e)(armData[0] & ARM_GPIO_MASK);

    return recoveryAction;
}

int32_t adrv9025_StreamTrigger(adi_adrv9025_Device_t* device,
                               uint8_t                streamId)
{
    static const uint8_t CMD_STATUS_BYTE_ERROR = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[1]     = {0};
    uint8_t cmdStatusByte  = 0;
    int32_t errHdl         = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Command ARM to execute the stream */
    extData[0]     = streamId;
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              ADRV9025_CPU_STREAM_TRIGGER_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_STREAM_TRIGGER_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_SETTXTOORXMAP_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETTXTOORXMAP_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & CMD_STATUS_BYTE_ERROR) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     errHdl,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_STREAM_TRIGGER_OPCODE,
                                                                              extData[0],
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_StreamGpioPinSet(adi_adrv9025_Device_t*    device,
                                  uint8_t                   signalId,
                                  adi_adrv9025_GpioPinSel_e streamTriggerGpio)
{
#define NUM_ADRV9025_STREAM_GPIOS_PER_CMD 1
    int32_t                   recoveryAction                                       = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                   resourceAcqReleaseStatus                             = ADI_FAILURE;
    uint16_t                  streamTriggerPinMaskRegVal                           = 0;
    uint16_t                  streamTriggerPinMask                                 = 0;
    adi_adrv9025_GpioPinSel_e activeStreamTriggerGpio                              = ADI_ADRV9025_GPIO_INVALID;
    uint32_t                  gpioInputMask                                        = 0;
    int32_t                   sharedResourceArr[NUM_ADRV9025_STREAM_GPIOS_PER_CMD] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Range check on signalId */
    if (signalId > (uint8_t)ADI_ADRV9025_GPIO_15)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         signalId,
                         "SignalId provided has invalid range. Must be within [0-15]");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Retrieve the current GPIO pins programmed to be stream triggers */
    recoveryAction = adrv9025_StreamGpioPinGet(device,
                                               signalId,
                                               &activeStreamTriggerGpio);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (activeStreamTriggerGpio != ADI_ADRV9025_GPIO_INVALID)
    {
        /* The GPIO identified by signalId is to be masked from auto-trigggering streams */

        /* Release the shared GPIO resource */
        sharedResourceArr[0] = (int32_t)signalId;
        recoveryAction       = adrv9025_SharedResourcesRelease(device,
                                                               ADRV9025_SHARED_RESOURCE_GPIO,
                                                               &sharedResourceArr[0],
                                                               NUM_ADRV9025_STREAM_GPIOS_PER_CMD,
                                                               ADRV9025_FEATURE_GPIO_STREAM_TRIGGER,
                                                               &resourceAcqReleaseStatus);

        if (resourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_ADRV9025_ERR_SHARED_RESOURCE_RELEASE,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             signalId,
                             "Error while attempting to release the active stream trigger GPIO. Please check if the GPIO is in use by another feature.");
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

        /* Retrieve the current Stream Processor GPIO trigger mask */
        recoveryAction = adrv9025_CoreStreamProcGpioPinMaskBfGet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 &streamTriggerPinMaskRegVal);
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* At this point GPIO pin shared resource released, mask it from triggering streams. Setting to 1 masks the pin. */
        streamTriggerPinMaskRegVal |= ((uint16_t)1 << (uint8_t)activeStreamTriggerGpio);
        recoveryAction = adrv9025_CoreStreamProcGpioPinMaskBfSet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 streamTriggerPinMaskRegVal);
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (streamTriggerGpio != ADI_ADRV9025_GPIO_INVALID)
    {
        /* Pin is currently masked */

        /* Acquire shared resource first to make sure this GPio pin isn't being used for anything else */
        sharedResourceArr[0] = (int32_t)signalId;
        recoveryAction       = adrv9025_SharedResourcesAcquire(device,
                                                               ADRV9025_SHARED_RESOURCE_GPIO,
                                                               &sharedResourceArr[0],
                                                               NUM_ADRV9025_STREAM_GPIOS_PER_CMD,
                                                               ADRV9025_FEATURE_GPIO_STREAM_TRIGGER,
                                                               &resourceAcqReleaseStatus);

        if (resourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_ADRV9025_ERR_SHARED_RESOURCE_RELEASE,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             signalId,
                             "Error while attempting to acquire the req stream trigger GPIO. Please check if the GPIO is in use by another feature.");
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

        /* Read the current stream trigger pin mask value in the device */
        recoveryAction = adrv9025_CoreStreamProcGpioPinMaskBfGet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 &streamTriggerPinMaskRegVal);
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* At this point GPIO pin shared resource acquired, unmask it. Setting to 0 unmasks the pin. */
        streamTriggerPinMask = ((uint16_t)1 << (uint8_t)signalId);
        streamTriggerPinMaskRegVal &= ~streamTriggerPinMask;
        recoveryAction = adrv9025_CoreStreamProcGpioPinMaskBfSet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 streamTriggerPinMaskRegVal);
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Set the GPIO direction to input */
        gpioInputMask  = ((uint32_t)1 << (uint32_t)signalId);
        recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                      gpioInputMask);
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_StreamGpioPinGet(adi_adrv9025_Device_t*     device,
                                  uint8_t                    signalId,
                                  adi_adrv9025_GpioPinSel_e* streamTriggerGpio)
{
    int32_t  recoveryAction       = ADI_COMMON_ACT_NO_ACTION;
    uint16_t streamTriggerPinMask = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        streamTriggerGpio);

    /* Retrieve the current GPIO pins programmed to be stream triggers */
    recoveryAction = adrv9025_CoreStreamProcGpioPinMaskBfGet(device,
                                                             ADRV9025_BF_CORE,
                                                             &streamTriggerPinMask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (ADRV9025_BF_EQUAL(streamTriggerPinMask,
                          ((uint16_t)1 << (uint16_t)signalId)))
    {
        /* corresponding pin in streamTriggerPinMask is equal to 1. Means it is masked, return ADI_ADRV9025_GPIO_INVALID */
        *streamTriggerGpio = ADI_ADRV9025_GPIO_INVALID;
    }
    else
    {
        /* corresponding pin in streamTriggerPinMAsk is not 1 (i.e. zero).  Means it is unmasked, return the same pin code to indicate it is active.*/
        *streamTriggerGpio = (adi_adrv9025_GpioPinSel_e)signalId;
    }

    return recoveryAction;
}

int32_t adrv9025_StreamGpioConfigSetRangeCheck(adi_adrv9025_Device_t*           device,
                                               adi_adrv9025_StreamGpioPinCfg_t* streamGpioPinCfg)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        streamGpioPinCfg);

    if ((streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_INVALID) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_00) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_01) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_02) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_03) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_04) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_05) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_06) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_07) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_08) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_09) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_10) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_11) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_12) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_13) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_14) &&
        (streamGpioPinCfg->streamGpInput0 != ADI_ADRV9025_GPIO_15))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamGpioPinCfg->streamGpInput0,
                         "Invalid stream trigger GPIO pin selected for stream GP0 input. GPIO00-GPIO15 are the valid values. To disable, please select GPIO_INVALID");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_INVALID) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_00) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_01) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_02) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_03) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_04) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_05) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_06) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_07) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_08) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_09) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_10) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_11) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_12) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_13) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_14) &&
        (streamGpioPinCfg->streamGpInput1 != ADI_ADRV9025_GPIO_15))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamGpioPinCfg->streamGpInput1,
                         "Invalid stream trigger GPIO pin selected for stream GP1 input. GPIO00-GPIO15 are valid values. To disable, please select GPIO_INVALID");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_INVALID) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_00) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_01) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_02) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_03) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_04) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_05) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_06) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_07) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_08) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_09) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_10) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_11) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_12) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_13) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_14) &&
        (streamGpioPinCfg->streamGpInput2 != ADI_ADRV9025_GPIO_15))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamGpioPinCfg->streamGpInput2,
                         "Invalid stream trigger GPIO pin selected for stream GP2 input. GPIO00-GPIO15 are valid values. To disable, please select GPIO_INVALID");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_INVALID) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_00) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_01) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_02) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_03) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_04) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_05) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_06) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_07) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_08) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_09) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_10) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_11) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_12) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_13) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_14) &&
        (streamGpioPinCfg->streamGpInput3 != ADI_ADRV9025_GPIO_15))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         streamGpioPinCfg->streamGpInput3,
                         "Invalid stream trigger GPIO pin selected for stream GP3 input. GPIO00-GPIO15 are valid values. To disable, please select GPIO_INVALID");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_TxToOrxMappingSetRangeCheck(adi_adrv9025_Device_t*    device,
                                             adi_adrv9025_RxChannels_e orxChannel,
                                             adi_adrv9025_TxChannels_e txChannel)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    if (device->devStateInfo.devState < ADI_ADRV9025_STATE_CPULOADED)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         orxChannel,
                         "Tx-ORx mapping requires that the Stream and ARM binaries have been loaded");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((orxChannel != ADI_ADRV9025_ORX1) &&
        (orxChannel != ADI_ADRV9025_ORX2) &&
        (orxChannel != ADI_ADRV9025_ORX3) &&
        (orxChannel != ADI_ADRV9025_ORX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         orxChannel,
                         "Invalid ORx Front end selected for Tx-ORx mapping. Valid ORx Front ends are ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4) &&
        (txChannel != ADI_ADRV9025_TXOFF))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx channel selected for Tx-ORx mapping. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((orxChannel == ADI_ADRV9025_ORX1) || (orxChannel == ADI_ADRV9025_ORX2))
    {
        if ((txChannel != ADI_ADRV9025_TX1) &&
            (txChannel != ADI_ADRV9025_TX2) &&
            (txChannel != ADI_ADRV9025_TXOFF))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             orxChannel,
                             "Invalid Tx channel selected for Tx-ORx mapping on ORx1/2 Front End. Valid Tx channels are Tx1 and Tx2. To disable Tx-ORx mapping select TXOFF");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if ((orxChannel == ADI_ADRV9025_ORX3) || (orxChannel == ADI_ADRV9025_ORX4))
    {
        if ((txChannel != ADI_ADRV9025_TX3) &&
            (txChannel != ADI_ADRV9025_TX4) &&
            (txChannel != ADI_ADRV9025_TXOFF))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             orxChannel,
                             "Invalid Tx channel selected for Tx-ORx mapping on ORx3/4 Front End. Valid Tx channels are Tx3 and Tx4. To disable Tx-ORx mapping select TXOFF");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_OrxMapDecode(adi_adrv9025_Device_t*     device,
                              uint16_t                   encodedTxToORxMapping,
                              adi_adrv9025_RxChannels_e  orxChannel,
                              adi_adrv9025_TxChannels_e* txChannel)
{
    static const uint16_t TXMAP_ORX1_ENA_MASK = 0x0002;
    static const uint16_t TXMAP_ORX1_SEL_MASK = 0x0001;
    static const uint16_t TXMAP_ORX2_ENA_MASK = 0x0020;
    static const uint16_t TXMAP_ORX2_SEL_MASK = 0x0010;
    static const uint16_t TXMAP_ORX3_ENA_MASK = 0x0200;
    static const uint16_t TXMAP_ORX3_SEL_MASK = 0x0100;
    static const uint16_t TXMAP_ORX4_ENA_MASK = 0x2000;
    static const uint16_t TXMAP_ORX4_SEL_MASK = 0x1000;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    switch (orxChannel)
    {
    case(ADI_ADRV9025_ORX1):
    {
        if ((encodedTxToORxMapping & TXMAP_ORX1_ENA_MASK) == 0)
        {
            *txChannel = ADI_ADRV9025_TXOFF;
        }
        else
        {
            if ((encodedTxToORxMapping & TXMAP_ORX1_SEL_MASK) == 0)
            {
                *txChannel = ADI_ADRV9025_TX1;
            }
            else
            {
                *txChannel = ADI_ADRV9025_TX2;
            }
        }

        break;
    }
    case(ADI_ADRV9025_ORX2):
    {
        if ((encodedTxToORxMapping & TXMAP_ORX2_ENA_MASK) == 0)
        {
            *txChannel = ADI_ADRV9025_TXOFF;
        }
        else
        {
            if ((encodedTxToORxMapping & TXMAP_ORX2_SEL_MASK) == 0)
            {
                *txChannel = ADI_ADRV9025_TX1;
            }
            else
            {
                *txChannel = ADI_ADRV9025_TX2;
            }
        }

        break;
    }
    case(ADI_ADRV9025_ORX3):
    {
        if ((encodedTxToORxMapping & TXMAP_ORX3_ENA_MASK) == 0)
        {
            *txChannel = ADI_ADRV9025_TXOFF;
        }
        else
        {
            if ((encodedTxToORxMapping & TXMAP_ORX3_SEL_MASK) == 0)
            {
                *txChannel = ADI_ADRV9025_TX3;
            }
            else
            {
                *txChannel = ADI_ADRV9025_TX4;
            }
        }

        break;
    }
    case(ADI_ADRV9025_ORX4):
    {
        if ((encodedTxToORxMapping & TXMAP_ORX4_ENA_MASK) == 0)
        {
            *txChannel = ADI_ADRV9025_TXOFF;
        }
        else
        {
            if ((encodedTxToORxMapping & TXMAP_ORX4_SEL_MASK) == 0)
            {
                *txChannel = ADI_ADRV9025_TX3;
            }
            else
            {
                *txChannel = ADI_ADRV9025_TX4;
            }
        }

        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         orxChannel,
                         "Invalid ORx channel selected for Tx-ORx mapping get.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    return recoveryAction;
}
