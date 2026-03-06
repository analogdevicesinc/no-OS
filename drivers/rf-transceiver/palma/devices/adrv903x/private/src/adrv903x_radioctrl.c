/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adrv903x_radioctrl.c
 * \brief Contains ADRV903X radio control related private function implementations.
 *
 * ADRV903X API Version: 2.12.1.4
 */
#include "../../private/include/adrv903x_radioctrl.h"
#include "../../private/include/adrv903x_rx.h"
#include "../../private/include/adrv903x_tx.h"
#include "../../private/bf/adrv903x_bf_core.h"
#include "../../private/include/adrv903x_init.h"
#include "../../private/include/adrv903x_cpu.h"
#include "../../private/include/adrv903x_cpu_cmd_ctrl.h"
#include "../../private/bf/adrv903x_bf_rx_dig.h"
#include "../../private/bf/adrv903x_bf_tx_dig.h"
#include "../../private/include/adrv903x_stream_proc_types.h"
#include "../../private/include/adrv903x_cpu_scratch_registers.h"

#include "adi_adrv903x_rx_types.h"
#include "adi_adrv903x_tx_types.h"
#include "adi_adrv903x_cals.h"
#include "adi_adrv903x_cpu.h"
#include "adi_adrv903x_types.h"

#define ADI_FILE    ADI_ADRV903X_FILE_PRIVATE_RADIOCTRL

ADI_API adi_adrv903x_ErrAction_e adrv903x_OrxEnableSet(adi_adrv903x_Device_t* const device,
                                                       const uint32_t rxChannelMask,
                                                       const uint32_t rxChannelEnable)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t orxChanMask = rxChannelMask >> ADI_ADRV903X_MAX_RX_ONLY;
    uint8_t orxChanEnable = rxChannelEnable >> ADI_ADRV903X_MAX_RX_ONLY;
    uint8_t bfValue = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction =   adrv903x_Core_RadioControlInterfaceOrxSpiEn_BfGet(device,
                                                                         NULL,
                                                                         (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                         &bfValue);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control Orx channels.");
        return recoveryAction;
    }

    /* Update the bfValue */
    ADRV903X_BF_UPDATE(bfValue, orxChanEnable, orxChanMask, 0U);

    recoveryAction =  adrv903x_Core_RadioControlInterfaceOrxSpiEn_BfSet(device,
                                                                        NULL,
                                                                        (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                        bfValue);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control ORX channels.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxEnableSet(adi_adrv903x_Device_t* const device,
                                                      const uint32_t rxChannelMask,
                                                      const uint32_t rxChannelEnable)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t bfValue = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction =   adrv903x_Core_RadioControlInterfaceRxSpiEn_BfGet(device,
                                                                        NULL,
                                                                        (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                        &bfValue);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control RX channels.");
        return recoveryAction;
    }

    /* Update the bfValue */
    ADRV903X_BF_UPDATE(bfValue, (uint8_t)rxChannelEnable, (uint8_t)rxChannelMask, 0U);

    recoveryAction =   adrv903x_Core_RadioControlInterfaceRxSpiEn_BfSet(device,
                                                                        NULL,
                                                                        (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                        bfValue);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control RX channels.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxEnableSet(adi_adrv903x_Device_t* const device,
                                                      const uint32_t txChannelMask,
                                                      const uint32_t txChannelEnable)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t bfValue = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction =   adrv903x_Core_RadioControlInterfaceTxSpiEn_BfGet(device,
                                                                        NULL,
                                                                        (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                        &bfValue);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control TX channels.");
        return recoveryAction;
    }

    /* Update the bfValue */
    ADRV903X_BF_UPDATE(bfValue, (uint8_t)txChannelEnable, (uint8_t)txChannelMask, 0U);

    recoveryAction =  adrv903x_Core_RadioControlInterfaceTxSpiEn_BfSet(device,
                                                                       NULL,
                                                                       (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                       bfValue);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control TX channels.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxTxEnableSetRangeCheck(adi_adrv903x_Device_t* const device,
                                                                  const uint32_t               orxChannelMask,
                                                                  const uint32_t               orxChannelEnable,
                                                                  const uint32_t               rxChannelMask,
                                                                  const uint32_t               rxChannelEnable,
                                                                  const uint32_t               txChannelMask,
                                                                  const uint32_t               txChannelEnable)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    /*TODO: update this static const once Tx channels have been numbered from Tx/Rx 0 - Tx/Rx7 */
    static const uint32_t ALL_TX_MASK = (ADI_ADRV903X_TX0 | ADI_ADRV903X_TX1 | ADI_ADRV903X_TX2 | ADI_ADRV903X_TX3 |
                                         ADI_ADRV903X_TX4 | ADI_ADRV903X_TX5 | ADI_ADRV903X_TX6 | ADI_ADRV903X_TX7);
    static const uint32_t ALL_RX_MASK = (ADI_ADRV903X_RX0 | ADI_ADRV903X_RX1 | ADI_ADRV903X_RX2 | ADI_ADRV903X_RX3 |
                                         ADI_ADRV903X_RX4 | ADI_ADRV903X_RX5 | ADI_ADRV903X_RX6 | ADI_ADRV903X_RX7);
    static const uint32_t ALL_ORX_MASK = (ADI_ADRV903X_ORX0 | ADI_ADRV903X_ORX1);

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    (void)orxChannelEnable;
    (void)rxChannelEnable;
    (void)txChannelEnable;

    /* Check that ARM and Stream processors have been loaded before enabling */
    if (device->devStateInfo.devState < ADI_ADRV903X_STATE_ALLCPUSLOADED)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Channel Enable/Disable is valid only after loading of CPU(s) & Stream processors");
        return recoveryAction;
    }

    /* Check that ORx channel mask is valid */
    if ((orxChannelMask & (~(uint32_t)ALL_ORX_MASK)) != 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            NULL,
            "Invalid ORx Channel mask encountered while attempting to enable ORx signal chain in SPI mode.");
        return recoveryAction;
    }

    /* Check that Rx channel mask is valid */
    if (rxChannelMask > ALL_RX_MASK)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               NULL,
                               "Invalid Rx Channel mask encountered while attempting to enable Rx signal chain in SPI mode.");
        return recoveryAction;
    }

    /* Check that Tx channel mask is valid */
    if (txChannelMask > ALL_TX_MASK)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            txChannelMask,
            "Invalid Tx Channel mask encountered while attempting to enable Tx signal chain in SPI mode.");
        return recoveryAction;
    }

    /* Check that requested ORx channels are initialized */
    if ((orxChannelMask & device->devStateInfo.initializedChannels) != orxChannelMask)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            rxChannelMask,
            "Requested ORx channels to enable are not initialized");
        return recoveryAction;
    }

    /* Check that requested Rx/ORx channels are initialized */
    if ((rxChannelMask & device->devStateInfo.initializedChannels) != rxChannelMask)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Requested Rx channels to enable are not initialized");
        return recoveryAction;
    }

    /* Check that requested Tx channels are initialized */
    if ((txChannelMask & (device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET)) != txChannelMask)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            txChannelMask,
            "Requested Tx channels to be enable are not initialized");
        return recoveryAction;
    }

    return ADI_ADRV903X_ERR_ACT_NONE;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RadioCtrlCfgSetRangeCheck(adi_adrv903x_Device_t* const                    device,
                                                                    const adi_adrv903x_RadioCtrlModeCfg_t* const    radioCtrlCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, radioCtrlCfg);

    static const uint32_t ALL_RX_MASK = (ADI_ADRV903X_RX0 | ADI_ADRV903X_RX1 | ADI_ADRV903X_RX2 | ADI_ADRV903X_RX3 |
                                     ADI_ADRV903X_RX4 | ADI_ADRV903X_RX5 | ADI_ADRV903X_RX6 | ADI_ADRV903X_RX7);
    static const uint32_t ALL_ORX_MASK = (ADI_ADRV903X_ORX0 | ADI_ADRV903X_ORX1);

    if (radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV903X_RX_EN_INVALID_MODE)
    {
        /*check if Rx profile is valid*/
        if ((device->devStateInfo.profilesValid & ADI_ADRV903X_RX_PROFILE_VALID) != ADI_ADRV903X_RX_PROFILE_VALID)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                device->devStateInfo.profilesValid,
                "No valid Rx profile is loaded.");
            return recoveryAction;
        }

        /*Check that if requested Rx Channel is valid*/
        if (((radioCtrlCfg->rxRadioCtrlModeCfg.rxChannelMask & (~(uint32_t)ALL_RX_MASK)) != 0U))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->rxRadioCtrlModeCfg.rxChannelMask,
                "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
            return recoveryAction;
        }

        if ((radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV903X_RX_EN_SPI_MODE) &&
            (radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV903X_RX_EN_PIN_MODE))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode,
                "Invalid Rx signal chain enable mode selected. Valid values include SPI, Pin mode");
            return recoveryAction;
        }
    }

    if (radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV903X_TX_EN_INVALID_MODE)
    {
        /*check if Tx profile is valid*/
        if ((device->devStateInfo.profilesValid & ADI_ADRV903X_TX_PROFILE_VALID) != ADI_ADRV903X_TX_PROFILE_VALID)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                device->devStateInfo.profilesValid,
                "No valid Tx profile is loaded.");
            return recoveryAction;
        }

        /*Check that if requested Tx Channel is valid*/
        if (((radioCtrlCfg->txRadioCtrlModeCfg.txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->txRadioCtrlModeCfg.txChannelMask,
                "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
            return recoveryAction;

        }

        if ((radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV903X_TX_EN_SPI_MODE) &&
        (radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV903X_TX_EN_PIN_MODE))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode,
                "Invalid Tx signal chain enable mode selected. Valid values include SPI, Pin mode");
            return recoveryAction;
        }
    }

    if (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV903X_ORX_EN_INVALID_MODE)
    {
        if ((device->devStateInfo.profilesValid & ADI_ADRV903X_ORX_PROFILE_VALID) != ADI_ADRV903X_ORX_PROFILE_VALID)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                device->devStateInfo.profilesValid,
                "No valid ORx profile is loaded.");
            return recoveryAction;
        }

        /*Check that if requested ORx Channel is valid*/
        if ((radioCtrlCfg->orxRadioCtrlModeCfg.orxChannelMask & (~(uint32_t)ALL_ORX_MASK)))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->orxRadioCtrlModeCfg.orxChannelMask,
                "Invalid ORx channel is selected. Valid values are any combinations of ORx0/1");
            return recoveryAction;
        }

        if ((radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV903X_ORX_EN_SPI_MODE) &&
        (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV903X_ORX_EN_PIN_MODE))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode,
                "Invalid ORx signal chain enable mode selected. Valid values include SPI, Pin mode");
            return recoveryAction;
        }
    }

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxRadioCtrlCfgSet(adi_adrv903x_Device_t * const               device,
                                                            adi_adrv903x_RxRadioCtrlModeCfg_t* const    rxRadioCtrlModeCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t armModeRxChanMask = 0x0U;  /* Bit N indicates RxN is in ARM mode. ARM mode overrides SPI/Pin-mode. */
    uint8_t spiModeRxChanMask = 0x0U;  /* If RxN is not in ARM mode then bit N here indicates RxN is SPI-mode (1) or Pin-mode (0) */

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, rxRadioCtrlModeCfg);

    /* Fetch the current Spi-enabled/Pin-enabled and ARM override status for Rx channels */
    recoveryAction = adrv903x_Core_RadioControlInterfaceRxSpiModeSel_BfGet(device,
                                                                           NULL,
                                                                           (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                           &spiModeRxChanMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get RX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* For both pin or SPI mode we disable ARM MODE for the Rx channels */
    armModeRxChanMask = ((uint8_t) rxRadioCtrlModeCfg->rxChannelMask);
    recoveryAction = adrv903x_Core_RadioControlInterfaceRxArmModeSelClr_BfSet(device,
                                                                              NULL,
                                                                              (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                              armModeRxChanMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    if (rxRadioCtrlModeCfg->rxEnableMode == ADI_ADRV903X_RX_EN_SPI_MODE)
    {
        /* Enable Spi mode for the channels - turn on the relevant bits in SpiModeSel register */
        spiModeRxChanMask |= rxRadioCtrlModeCfg->rxChannelMask;

    }
    else if (rxRadioCtrlModeCfg->rxEnableMode == ADI_ADRV903X_RX_EN_PIN_MODE)
    {
        /* Enable pin-mode for the channels - turn off the relevant bits in SpiModeSel register */
        spiModeRxChanMask &= ~((uint8_t) rxRadioCtrlModeCfg->rxChannelMask);
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            rxRadioCtrlModeCfg->rxEnableMode,
            "Invalid Rx Enable Mode encountered while attempting to configure Rx radio ctrl configuration settings");
        return recoveryAction;
    }

    /* Write the updated SpiMode register */
    recoveryAction = adrv903x_Core_RadioControlInterfaceRxSpiModeSel_BfSet(device,
                                                                           NULL,
                                                                           (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                           (uint8_t) spiModeRxChanMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxRadioCtrlCfgSet(adi_adrv903x_Device_t* const                device,
                                                            adi_adrv903x_TxRadioCtrlModeCfg_t* const    txRadioCtrlModeCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t txSpiModeSel = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, txRadioCtrlModeCfg);

    /* Disable ARM MODE for Tx ctrl configuration */
    recoveryAction = adrv903x_Core_RadioControlInterfaceTxArmModeSelClr_BfSet(device,
                                                                              NULL,
                                                                              (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                              (uint8_t) txRadioCtrlModeCfg->txChannelMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* Read Spi mode */
    recoveryAction = adrv903x_Core_RadioControlInterfaceTxSpiModeSel_BfGet(device,
                                                                           NULL,
                                                                           (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                           &txSpiModeSel);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /*For Spi mode set Spi Mode bitfield to 1 and for pin mode set the Spi mode bitfield to 0*/
    if (txRadioCtrlModeCfg->txEnableMode == ADI_ADRV903X_TX_EN_SPI_MODE)
    {
        /* Enable Spi mode */
        txSpiModeSel |= (uint8_t) txRadioCtrlModeCfg->txChannelMask;

    }
    else if (txRadioCtrlModeCfg->txEnableMode == ADI_ADRV903X_TX_EN_PIN_MODE)
    {
        /* Disable Spi mode */
        txSpiModeSel &= ~((uint8_t) txRadioCtrlModeCfg->txChannelMask);
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                                recoveryAction,
                                txRadioCtrlModeCfg->txEnableMode,
                                "Invalid Tx Enable Mode encountered while attempting to configure Tx radio ctrl configuration settings");
        return recoveryAction;
    }

    recoveryAction =  adrv903x_Core_RadioControlInterfaceTxSpiModeSel_BfSet(device,
                                                                            NULL,
                                                                            (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                            txSpiModeSel);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_OrxRadioCtrlCfgSet(adi_adrv903x_Device_t* const                device,
                                                             adi_adrv903x_ORxRadioCtrlModeCfg_t* const   orxRadioCtrlModeCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t orxSpiModeSel = 0U;
    uint8_t orxChanMask = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, orxRadioCtrlModeCfg);

    orxChanMask = (uint8_t) (orxRadioCtrlModeCfg->orxChannelMask >> ADI_ADRV903X_MAX_RX_ONLY);

    /* Disable ARM override ctrl for ORx ctrl configuration */
    recoveryAction = adrv903x_Core_RadioControlInterfaceOrxArmModeSelClr_BfSet(device,
                                                                               NULL,
                                                                               (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                               orxChanMask);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API ORX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* Read Spi mode sel */
    recoveryAction = adrv903x_Core_RadioControlInterfaceOrxSpiModeSel_BfGet(device,
                                                                            NULL,
                                                                            (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                            &orxSpiModeSel);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API ORX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /*For Spi mode set Spi Mode bitfield to 1 and for pin mode set the Spi mode bitfield to 0*/
    if (orxRadioCtrlModeCfg->orxEnableMode == ADI_ADRV903X_ORX_EN_SPI_MODE)
    {

        /* Enable Spi mode */
        orxSpiModeSel |= orxChanMask;
    }
    else if (orxRadioCtrlModeCfg->orxEnableMode == ADI_ADRV903X_ORX_EN_PIN_MODE)
    {
        /* Disable Spi mode */
        orxSpiModeSel &= ~orxChanMask;
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            orxRadioCtrlModeCfg->orxEnableMode,
            "Invalid ORx Enable Mode encountered while attempting to configure Rx radio ctrl configuration settings");
        return recoveryAction;
    }

    recoveryAction = adrv903x_Core_RadioControlInterfaceOrxSpiModeSel_BfSet(device,
                                                                            NULL,
                                                                            (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                            orxSpiModeSel);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API ORX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_OrxRadioCtrlCfgGet(adi_adrv903x_Device_t* const                device,
                                                             const adi_adrv903x_RxChannels_e             rxChannel,
                                                             adi_adrv903x_ORxRadioCtrlModeCfg_t* const   orxRadioCtrlModeCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t  orxArmMode = 0U;
    uint8_t  orxChannel = 0U;
    uint8_t  orxSpiMode = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, orxRadioCtrlModeCfg);

    orxChannel = (uint8_t)(rxChannel >> ADI_ADRV903X_MAX_RX_ONLY);
    /* Verify only one ORX channel is set */
    if (orxChannel & (orxChannel - 1U))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid ORX Channel selection.");
        return recoveryAction;
    }

    orxRadioCtrlModeCfg->orxChannelMask = 0;
    orxRadioCtrlModeCfg->orxEnableMode = ADI_ADRV903X_ORX_EN_INVALID_MODE;

    /* Read ORX ARM Override ctrl bitfield*/
    recoveryAction =  adrv903x_Core_RadioControlInterfaceOrxArmModeSel_BfGet(device,
                                                                             NULL,
                                                                             (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                             &orxArmMode);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get ORX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* The Spi mode bitfield is only valid if ARM override ctrl is set to 0 */
    if ((orxArmMode & orxChannel) == 0)
    {
        /* Read Pin ctrl mode */
        recoveryAction =  adrv903x_Core_RadioControlInterfaceOrxSpiModeSel_BfGet(device,
                                                                                 NULL,
                                                                                 (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                                 &orxSpiMode);

        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get ORX Radio Control Cfg Failed.");
            return recoveryAction;
        }

        /* 'Spi Mode' bit field: 1 (Spi Mode), 0 (Pin Mode) */
        if ((orxChannel & orxSpiMode) == orxChannel)
        {
            orxRadioCtrlModeCfg->orxEnableMode = ADI_ADRV903X_ORX_EN_SPI_MODE;
        }
        else
        {
            orxRadioCtrlModeCfg->orxEnableMode = ADI_ADRV903X_ORX_EN_PIN_MODE;
        }
    }

    /*Update the ORx channel mask*/
    orxRadioCtrlModeCfg->orxChannelMask = rxChannel;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxRadioCtrlCfgGet(adi_adrv903x_Device_t* const                device,
                                                            const adi_adrv903x_RxChannels_e             rxChannel,
                                                            adi_adrv903x_RxRadioCtrlModeCfg_t * const   rxRadioCtrlModeCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t  rxArmMode = 0U;
    uint8_t  rxSpiMode = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, rxRadioCtrlModeCfg);

    rxRadioCtrlModeCfg->rxChannelMask = 0;
    rxRadioCtrlModeCfg->rxEnableMode = ADI_ADRV903X_RX_EN_INVALID_MODE;

    /* Verify only one ORX channel is set */
    if ((rxChannel & ~ADI_ADRV903X_RX_MASK_ALL) || (rxChannel & (rxChannel - 1U)))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid RX Channel selection.");
        return recoveryAction;
    }

    /* Read RX ARM Override ctrl bitfield*/
    recoveryAction =  adrv903x_Core_RadioControlInterfaceRxArmModeSel_BfGet(device,
                                                                            NULL,
                                                                            (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                            &rxArmMode);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get RX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* The Spi mode bitfield is only valid if ARM override ctrl is set to 0 */
    if ((rxArmMode & rxChannel) == 0)
    {
        /* Read Pin ctrl mode */
        recoveryAction =  adrv903x_Core_RadioControlInterfaceRxSpiModeSel_BfGet(device,
                                                                                NULL,
                                                                                (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                                &rxSpiMode);

        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get RX Radio Control Cfg Failed.");
            return recoveryAction;
        }

        /* 'Spi Mode' bit field: 1 (Spi Mode), 0 (Pin Mode) */
        if ((rxChannel & rxSpiMode) == rxChannel)
        {
            rxRadioCtrlModeCfg->rxEnableMode = ADI_ADRV903X_RX_EN_SPI_MODE;
        }
        else
        {
            rxRadioCtrlModeCfg->rxEnableMode = ADI_ADRV903X_RX_EN_PIN_MODE;
        }
    }

    /*Update the ORx channel mask*/
    rxRadioCtrlModeCfg->rxChannelMask = rxChannel;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxRadioCtrlCfgGet(adi_adrv903x_Device_t * const device,
                                                            const adi_adrv903x_TxChannels_e txChannel,
                                                            adi_adrv903x_TxRadioCtrlModeCfg_t * const txRadioCtrlModeCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t  txArmMode = 0U;
    uint8_t  txSpiMode = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, txRadioCtrlModeCfg);

    txRadioCtrlModeCfg->txChannelMask = 0;
    txRadioCtrlModeCfg->txEnableMode = ADI_ADRV903X_TX_EN_INVALID_MODE;

    /* Verify only one TX channel is set */
    if (txChannel & (txChannel - 1U))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid TX Channel selection.");
        return recoveryAction;
    }

    /*Read TX ARM Mode ctrl bitfield*/
    recoveryAction =  adrv903x_Core_RadioControlInterfaceTxArmModeSel_BfGet(device,
                                                                            NULL,
                                                                            (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                            &txArmMode);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get TX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* The Spi mode bitfield is only valid if ARM mode ctrl is not set to 1 */
    if ((txArmMode & txChannel) == 0)
    {
        /* Read TX Pin ctrl mode */
        recoveryAction =  adrv903x_Core_RadioControlInterfaceTxSpiModeSel_BfGet(device,
                                                                                NULL,
                                                                                (adrv903x_BfCoreChanAddr_e) ADRV903X_BF_CORE_ADDR,
                                                                                &txSpiMode);

        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get TX Radio Control Cfg Failed.");
            return recoveryAction;
        }
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get TX Radio Control Cfg Failed.");
            return recoveryAction;
        }

        /* 'Spi Mode' bit field: 1 (Spi Mode), 0 (Pin Mode) */
        if ((txChannel & txSpiMode) == txChannel)
        {
            txRadioCtrlModeCfg->txEnableMode = ADI_ADRV903X_TX_EN_SPI_MODE;
        }
        else
        {
            txRadioCtrlModeCfg->txEnableMode = ADI_ADRV903X_TX_EN_PIN_MODE;
        }
    }

    /*Update the Tx Channel mask*/
    txRadioCtrlModeCfg->txChannelMask = txChannel;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_LoUnlockGpInterruptMaskGet(adi_adrv903x_Device_t* const    device,
                                                                     const adi_adrv903x_LoName_e      loName,
                                                                     uint8_t* const                   loGpInterruptPin0Mask,
                                                                     uint8_t* const                   loGpInterruptPin1Mask)
{
    static const uint8_t  LO0_GP_INTERRUPT_SHIFT = 32U;
    uint64_t LO0_GP_INTERRUPT_UINT64_MASK  =  (uint64_t)1U <<  LO0_GP_INTERRUPT_SHIFT; /* east */
    static const uint8_t  LO1_GP_INTERRUPT_SHIFT = 31U;
    uint64_t LO1_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO1_GP_INTERRUPT_SHIFT; /* west */
        adi_adrv903x_ErrAction_e  recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint64_t gpInterruptPin0Mask        = 0U;
    uint64_t gpInterruptPin1Mask        = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, loGpInterruptPin0Mask);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, loGpInterruptPin1Mask);

    /* Read GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin0_BfGet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin0Mask); 
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin1_BfGet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    switch (loName)
    {
        case(ADI_ADRV903X_LO0):
            {
                *loGpInterruptPin0Mask = (uint8_t)ADRV903X_BF_DECODE(gpInterruptPin0Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                *loGpInterruptPin1Mask = (uint8_t)ADRV903X_BF_DECODE(gpInterruptPin1Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);

                break;
            }
        case(ADI_ADRV903X_LO1):
            {
                *loGpInterruptPin0Mask = (uint8_t)ADRV903X_BF_DECODE(gpInterruptPin0Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                *loGpInterruptPin1Mask = (uint8_t)ADRV903X_BF_DECODE(gpInterruptPin1Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                break;
            }
                default:
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid LO selection while attempting to retrieve LO GP Interrupt mask.");
                return recoveryAction;
            }
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_LoUnlockGpInterruptMaskSet(adi_adrv903x_Device_t* const    device,
                                                                     const adi_adrv903x_LoName_e      loName,
                                                                     const uint8_t                    loGpInterruptPin0Mask,
                                                                     const uint8_t                    loGpInterruptPin1Mask)
{
    static const uint8_t  LO0_GP_INTERRUPT_SHIFT = 32U;
    uint64_t LO0_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO0_GP_INTERRUPT_SHIFT; /* east */
    static const uint8_t  LO1_GP_INTERRUPT_SHIFT = 31U;
    uint64_t LO1_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO1_GP_INTERRUPT_SHIFT; /* west */
        adi_adrv903x_ErrAction_e  recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint64_t gpInterruptPin0Mask        = 0U;
    uint64_t gpInterruptPin1Mask        = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Read GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin0_BfGet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin0Mask);
        
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin1_BfGet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    switch (loName)
    {
        case(ADI_ADRV903X_LO0):
            {
                /* Update GP Interrupt Mask */
                ADRV903X_BF_UPDATE(gpInterruptPin0Mask, (uint64_t)loGpInterruptPin0Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                ADRV903X_BF_UPDATE(gpInterruptPin1Mask, (uint64_t)loGpInterruptPin1Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                break;
            }
        case(ADI_ADRV903X_LO1):
            {
                /* Update GP Interrupt Mask */
                ADRV903X_BF_UPDATE(gpInterruptPin0Mask, (uint64_t)loGpInterruptPin0Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                ADRV903X_BF_UPDATE(gpInterruptPin1Mask, (uint64_t)loGpInterruptPin1Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                break;
            }
                    default:
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid LO selection while attempting to set LO GP Interrupt mask");
                return recoveryAction;
            }
    }

    /* Write GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin0_BfSet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        gpInterruptPin0Mask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Set GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin1_BfSet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Set GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_LoOverrangeGpInterruptMaskGet(adi_adrv903x_Device_t* const    device,
                                                                        const adi_adrv903x_LoName_e     loName,
                                                                        uint8_t* const                  loGpInterruptPin0Mask,
                                                                        uint8_t* const                  loGpInterruptPin1Mask)
{
    static const uint8_t  LO0_GP_INTERRUPT_SHIFT = 29U;
    uint64_t LO0_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO0_GP_INTERRUPT_SHIFT; /* east */
    static const uint8_t  LO1_GP_INTERRUPT_SHIFT = 28U;
    uint64_t LO1_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO1_GP_INTERRUPT_SHIFT; /* west */
        adi_adrv903x_ErrAction_e  recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint64_t gpInterruptPin0Mask        = 0U;
    uint64_t gpInterruptPin1Mask        = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, loGpInterruptPin0Mask);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, loGpInterruptPin1Mask);

    /* Read GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin0_BfGet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin0Mask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin1_BfGet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    switch (loName)
    {
        case(ADI_ADRV903X_LO0):
            {
                *loGpInterruptPin0Mask = (uint8_t)ADRV903X_BF_DECODE(gpInterruptPin0Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                *loGpInterruptPin1Mask = (uint8_t)ADRV903X_BF_DECODE(gpInterruptPin1Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);

                break;
            }
        case(ADI_ADRV903X_LO1):
            {
                *loGpInterruptPin0Mask = (uint8_t)ADRV903X_BF_DECODE(gpInterruptPin0Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                *loGpInterruptPin1Mask = (uint8_t)ADRV903X_BF_DECODE(gpInterruptPin1Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                break;
            }
                    default:
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid LO selection while attempting to retrieve LO GP Interrupt mask.");
                return recoveryAction;
            }
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_LoOverrangeGpInterruptMaskSet(adi_adrv903x_Device_t* const    device,
                                                                        const adi_adrv903x_LoName_e     loName,
                                                                        const uint8_t                   loGpInterruptPin0Mask,
                                                                        const uint8_t                   loGpInterruptPin1Mask)
{
    static const uint8_t  LO0_GP_INTERRUPT_SHIFT = 29U;
    uint64_t LO0_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO0_GP_INTERRUPT_SHIFT; /* east */
    static const uint8_t  LO1_GP_INTERRUPT_SHIFT = 28U;
    uint64_t LO1_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO1_GP_INTERRUPT_SHIFT; /* west */
        adi_adrv903x_ErrAction_e  recoveryAction =  ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint64_t gpInterruptPin0Mask        = 0U;
    uint64_t gpInterruptPin1Mask        = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Read GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin0_BfGet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin0Mask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin1_BfGet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    switch (loName)
    {
        case(ADI_ADRV903X_LO0):
            {
                /* Update GP Interrupt Mask */
                ADRV903X_BF_UPDATE(gpInterruptPin0Mask, (uint64_t)loGpInterruptPin0Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                ADRV903X_BF_UPDATE(gpInterruptPin1Mask, (uint64_t)loGpInterruptPin1Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                break;
            }
        case(ADI_ADRV903X_LO1):
            {
                /* Update GP Interrupt Mask */
                ADRV903X_BF_UPDATE(gpInterruptPin0Mask, (uint64_t)loGpInterruptPin0Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                ADRV903X_BF_UPDATE(gpInterruptPin1Mask, (uint64_t)loGpInterruptPin1Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                break;
            }
                    default:
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid LO selection while attempting to set LO GP Interrupt mask");
                return recoveryAction;
            }
    }

    /* Write GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin0_BfSet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        gpInterruptPin0Mask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Set GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv903x_Core_GpInterruptsMaskUpperWordPin1_BfSet(device,
                                                                        NULL,
                                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Set GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_LoFrequencySetRangeCheck(adi_adrv903x_Device_t* const device,
                                                                   const adi_adrv903x_LoConfig_t* const loConfig)
{
    adi_adrv903x_ErrAction_e    recoveryAction     = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t                   areCalsRunning     = 0U;
    adi_adrv903x_LoSel_e      loSelect           = ADI_ADRV903X_LOSEL_LO1;
    adi_adrv903x_LoSel_e      loSelectGet        = ADI_ADRV903X_LOSEL_LO1;
    uint64_t                  maxRfBandwidth_kHz = 0U;
    uint8_t                   i                  = 0U;
    adi_adrv903x_TxChannels_e txChannel          = ADI_ADRV903X_TXOFF;
    adi_adrv903x_RxChannels_e rxChannel          = ADI_ADRV903X_RXOFF;
    int64_t                   freqValid          = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, loConfig);

        /*Check that LO Name selected is valid */
    if ((loConfig->loName != ADI_ADRV903X_LO0) &&
        (loConfig->loName != ADI_ADRV903X_LO1)
                )
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loConfig->loName, "Invalid LO selected for setting LO frequency");
        return recoveryAction;
    }

    /*Check that PLL mode is valid*/
    if ((loConfig->loConfigSel != ADI_ADRV903X_NCO_NO_OPTION_SELECTED) &&
        (loConfig->loConfigSel != ADI_ADRV903X_NCO_AUTO_UPDATE_DISABLE))
    {

        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loConfig->loName, "Invalid LO Config selected for setting LO frequency");
        return recoveryAction;
    }

    /* Determine the PLL Frequency LO Selection */
    if (loConfig->loName == ADI_ADRV903X_LO0)
    {
        loSelect = ADI_ADRV903X_LOSEL_LO0;
    }
    else if (loConfig->loName == ADI_ADRV903X_LO1)
    {
        loSelect = ADI_ADRV903X_LOSEL_LO1;
    }
        /* Find the largest bandwidth of all the initialized Tx or Rx chans driven by the LO. */
    maxRfBandwidth_kHz = 0U;
    if ((device->devStateInfo.profilesValid & ADI_ADRV903X_TX_PROFILE_VALID) == ADI_ADRV903X_TX_PROFILE_VALID)
    {
        for (i = 0U; i < ADI_ADRV903X_MAX_TXCHANNELS; i++)
        {
            /* convert the index to matching the channel */
            txChannel = (adi_adrv903x_TxChannels_e)((uint8_t)(1U << i));
            if (((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel)
                  == (uint32_t)txChannel)
            {
                /* Tx chan i is initialized. */
                recoveryAction = adrv903x_TxLoSourceGet(device, txChannel, &loSelectGet);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxLoSourceGet Issue");
                    return recoveryAction;
                }
                
                if (loSelectGet == loSelect)
                {
                    /* Tx chan i is driven by the lo whose freq is being changed */
                    if (device->initExtract.tx.txChannelCfg[i].rfBandwidth_kHz > maxRfBandwidth_kHz)
                    {
                        /* This Tx chan has the largest bandwidth so far; Update max. */
                        maxRfBandwidth_kHz = device->initExtract.tx.txChannelCfg[i].rfBandwidth_kHz;
                    }
                }
            }
        }
    }

    /* Find the greatest RX Output Rates value of the same LO */
    if ((device->devStateInfo.profilesValid & ADI_ADRV903X_RX_PROFILE_VALID) == ADI_ADRV903X_RX_PROFILE_VALID)
    {
        for (i = 0U; i < ADI_ADRV903X_MAX_RX_ONLY; i++)
        {
            /* convert the index to matching the channel */
            rxChannel = (adi_adrv903x_RxChannels_e)((uint8_t)(1U << i));
            if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == (uint32_t)rxChannel)
            {
                recoveryAction = adrv903x_RxLoSourceGet(device, rxChannel, &loSelectGet);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxLoSourceGet Issue");
                    return recoveryAction;
                }
                if (loSelectGet == loSelect)
                {
                    /* Now check if matching LO then compare to get max value */
                    if (device->initExtract.rx.rxChannelCfg[i].rfBandwidth_kHz > maxRfBandwidth_kHz)
                    {
                        maxRfBandwidth_kHz = device->initExtract.rx.rxChannelCfg[i].rfBandwidth_kHz;
                    }
                }
            }
        }
    }

    /* The rule is that the PLL LO frequency must always more than half of the greatest bandwidth. */
    freqValid = (int64_t)loConfig->loFrequency_Hz - (int64_t)((maxRfBandwidth_kHz * 1000) >> 1U);
    if ((freqValid <= 0U) && (maxRfBandwidth_kHz > 0U))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV903X_ERRSRC_RADIOCTRL,
            ADI_ADRV903X_ERRCODE_RADIOCTRL_LO_CFG,
            recoveryAction,
            loConfig->loFrequency_Hz,
            "(loFreq - Bandwidth/2) should be greater than DC.");
        return recoveryAction;
    }

    /*Check that init cals are not running*/
    recoveryAction = (adi_adrv903x_ErrAction_e)adi_adrv903x_InitCalsCheckCompleteGet(device, &areCalsRunning, ADI_FALSE);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "InitCalsCheckCompleteGet Issue");
        return recoveryAction;
    }

    if (areCalsRunning == 1U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, areCalsRunning, "Can not set LO frequency while InitCals are running");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_LoLoopFilterSetRangeCheck(adi_adrv903x_Device_t* const device,
                                                                    const adi_adrv903x_LoName_e loName,
                                                                    const adi_adrv903x_LoLoopFilterCfg_t* const loLoopFilterConfig)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    static const uint16_t MIN_LOOPBANDWIDTH_KHZ = 60;
    static const uint16_t MAX_LOOPBANDWIDTH_KHZ = 1000;
    static const uint8_t MIN_PHASEMARGIN_DEGREE = 45;
    static const uint8_t MAX_PHASEMARGIN_DEGREE = 75;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, loLoopFilterConfig);

    if ((loName != ADI_ADRV903X_LO0) && (loName != ADI_ADRV903X_LO1))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid LO selected for setting LO Loop filter");
        return recoveryAction;
    }

    if ((loLoopFilterConfig->loopBandwidth_kHz < MIN_LOOPBANDWIDTH_KHZ) || (loLoopFilterConfig->loopBandwidth_kHz > MAX_LOOPBANDWIDTH_KHZ))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid loop bandwidth for setting LO Loop filter");
        return recoveryAction;
    }

    if ((loLoopFilterConfig->phaseMargin_degrees < MIN_PHASEMARGIN_DEGREE) || (loLoopFilterConfig->phaseMargin_degrees > MAX_PHASEMARGIN_DEGREE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid phase margin for setting LO Loop filter");
        return recoveryAction;
    }

    return ADI_ADRV903X_ERR_ACT_NONE;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_StreamTrigger(adi_adrv903x_Device_t* const    device,
                                                        const uint8_t                   streamId)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t cpuCommandBusy = 0U;
    uint32_t timeout_us = ADI_ADRV903X_SENDCPUCMD_TIMEOUT_US;
    uint32_t waitInterval_us = ADI_ADRV903X_SENDCPUCMD_INTERVAL_US;
    uint32_t eventCheck = 0U;
    uint32_t numEventChecks = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* setting a 2 second timeout for mailbox busy bit to be clear (can't send an cpu mailbox command until mailbox is ready) */
#if ADI_ADRV903X_SENDCPUCMD_INTERVAL_US > ADI_ADRV903X_SENDCPUCMD_TIMEOUT_US
    waitInterval_us = timeout_us;
#elif ADI_ADRV903X_SENDCPUCMD_INTERVAL_US == 0
    waitInterval_us = timeout_us;
#endif
    numEventChecks = timeout_us / waitInterval_us;

    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0U; eventCheck <= numEventChecks; ++eventCheck)
    {
        recoveryAction = adrv903x_Core_Arm0Spi0CommandBusy_BfGet(device,
                                                                 NULL,
                                                                 (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                                 &cpuCommandBusy);
        if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Read (for CPU 0 Command Busy Bit) Issue");
            return recoveryAction;
        }

        if (cpuCommandBusy == ADI_TRUE)
        {
            recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
            if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue");
                return recoveryAction;
            }
        }
        else
        {
            break;
        }
    }

    /* if busy bit remains set after timeout event loop function is exited, otherwise command is sent */
    if (cpuCommandBusy == ADI_TRUE)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_FEATURE;
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV903X_ERRSRC_CPU,
                            ADI_ADRV903X_ERRCODE_CPU_CMD_TIMEOUT,
                            recoveryAction,
                            cpuCommandBusy,
                            "CPU Mailbox Busy; Timeout Occurred");
        return recoveryAction;
    }
    else
    {
        /*Write streamId to payload in arm mailbox*/
        recoveryAction = adrv903x_Core_Arm0Spi0ExtCmdByte1_BfSet(device,
                                                                 NULL,
                                                                 (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                                 streamId);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Core Stream Processor stream ID number to trigger.");
            return recoveryAction;
        }

        /*Write opcode to trigger Core Stream Processor rather than send command to ARM*/
        recoveryAction = adrv903x_Core_Arm0Spi0Command_BfSet(device,
                                                             NULL,
                                                             (adrv903x_BfCoreChanAddr_e)ADRV903X_BF_CORE_ADDR,
                                                             ADRV903X_CPU_STREAM_TRIGGER_OPCODE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while triggering Core Stream Processor directly from API.");
            return recoveryAction;
        }

    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxStreamTrigger(adi_adrv903x_Device_t* const           device,
                                                          const uint8_t                          txChanMask,
                                                          const uint8_t                          txStreamId)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    const uint32_t NUM_OF_EVENT_CHECKS = 1000U;
    const uint32_t UNUSED_STREAM_ID = 0xFFU; /* The value of unused TX stream ID need to match the value used by core stream */
    uint32_t eventCheck = 0U;
    uint8_t rdStreamId = 0;
    uint8_t byteWriteBuf[2] = { 0U, 0U };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0U; eventCheck <= NUM_OF_EVENT_CHECKS; ++eventCheck)
    {
        recoveryAction = adrv903x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        ADRV903X_TRIGGER_SLICE_STREAM_NUM,
                                                        &rdStreamId);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading stream ID from scratchpad register.");
            return recoveryAction;
        }

        if (rdStreamId != UNUSED_STREAM_ID)
        {
            recoveryAction = (adi_adrv903x_ErrAction_e)adi_common_hal_Wait_us(&device->common, 1);
            if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue");
                return recoveryAction;
            }
        }
        else
        {
            break;
        }
    }

    /* if stream ID indicates that previous request was still not processed, exit with error */
    if (rdStreamId != UNUSED_STREAM_ID)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Core SP Busy");
        return recoveryAction;
    }
    else
    {
        /* Set requested TX stream ID to be run */
        byteWriteBuf[0] = (uint8_t)txStreamId;

        /* Update Tx channel bitmask */
        byteWriteBuf[1] = txChanMask;

        recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_CORE_STREAM_SCRATCH204, byteWriteBuf, 2U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write to Core scratch pad registers");
            return recoveryAction;
        }

        recoveryAction = adrv903x_StreamTrigger(device, ADRV903X_STREAM_MAIN_TRIGGER_TX_STREAM);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while triggering Core Stream to trigger TX stream");
            return recoveryAction;
        }
    }
    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxStreamTrigger(adi_adrv903x_Device_t* const           device,
                                                          const uint8_t                          rxChanMask,
                                                          const uint8_t                          rxStreamId)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    const uint32_t NUM_OF_EVENT_CHECKS = 1000U;
    const uint32_t UNUSED_STREAM_ID = 0xFFU; /* The value of unused RX stream ID need to match the value used by core stream */
    uint32_t eventCheck = 0U;
    uint8_t rdStreamId = 0;
    uint8_t byteWriteBuf[2] = { 0U, 0U };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0U; eventCheck <= NUM_OF_EVENT_CHECKS; ++eventCheck)
    {
        recoveryAction = adrv903x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        ADRV903X_TRIGGER_SLICE_STREAM_NUM,
                                                        &rdStreamId);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading stream ID from scratchpad register.");
            return recoveryAction;
        }

        if (rdStreamId != UNUSED_STREAM_ID)
        {
            recoveryAction = (adi_adrv903x_ErrAction_e)adi_common_hal_Wait_us(&device->common, 1);
            if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Request Issue");
                return recoveryAction;
            }
        }
        else
        {
            break;
        }
    }

    /* if stream ID indicates that previous request was still not processed exit with error */
    if (rdStreamId != UNUSED_STREAM_ID)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Core SP Busy");
        return recoveryAction;
    }
    else
    {
        /* Set requested RX stream ID to be run */
        byteWriteBuf[0] = (uint8_t)rxStreamId;

        /* Update Tx channel bitmask */
        byteWriteBuf[1] = rxChanMask;

        recoveryAction = adi_adrv903x_RegistersByteWrite(device, NULL, ADRV903X_ADDR_CORE_STREAM_SCRATCH204, byteWriteBuf, 2U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write to Core scratch pad registers");
            return recoveryAction;
        }

        recoveryAction = adrv903x_StreamTrigger(device, ADRV903X_STREAM_MAIN_TRIGGER_RX_STREAM);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while triggering Core Stream to trigger RX stream");
            return recoveryAction;
        }
    }
    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_StreamGpioConfigSetRangeCheck(adi_adrv903x_Device_t* const          device,
                                                                        const adi_adrv903x_StreamGpioPinCfg_t* const streamGpioPinCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    int idx = 0;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, streamGpioPinCfg);


    /* Check all Stream GPIO if valid */
    for (idx = 0U; idx < ADI_ADRV903X_MAX_STREAMGPIO; idx++)
    {
        /* Check if pin is valid and match index */
        if ((streamGpioPinCfg->streamGpInput[idx] != ADI_ADRV903X_GPIO_INVALID) &&
            (streamGpioPinCfg->streamGpInput[idx] != (adi_adrv903x_GpioPinSel_e)idx))
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, idx, "Invalid stream trigger GPIO pin selected. Index must match GPIO value. To disable, please select GPIO_INVALID");
            return recoveryAction;
        }
    }

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    return (recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcVersionGet(adi_adrv903x_Device_t* const  device,
                                                        const uint8_t                 calType,
                                                        const adi_adrv903x_Channels_e channel,
                                                        adi_adrv903x_Version_t* const adcVersion)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, adcVersion);

    uint32_t lengthResp = 4U;
    uint8_t ctrlData[4] = { 0U };
    uint8_t ctrlResp[16] = { 0U };

    if ((calType != ADRV903X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if ((channel != ADI_ADRV903X_CH0) &&
        (channel != ADI_ADRV903X_CH1) &&
        (channel != ADI_ADRV903X_CH2) &&
        (channel != ADI_ADRV903X_CH3) &&
        (channel != ADI_ADRV903X_CH4) &&
        (channel != ADI_ADRV903X_CH5) &&
        (channel != ADI_ADRV903X_CH6) &&
        (channel != ADI_ADRV903X_CH7))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        return recoveryAction;
    }

    /* Get the ADC module revision number: */
    recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_GET_SW_VERSION,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "Get ADC SW version failed");
        return recoveryAction;
    }

    adcVersion->majorVer       = adrv903x_CpuIntFromBytesGet(&ctrlResp[ 0], 4U);
    adcVersion->minorVer       = adrv903x_CpuIntFromBytesGet(&ctrlResp[ 4], 4U);
    adcVersion->maintenanceVer = adrv903x_CpuIntFromBytesGet(&ctrlResp[ 8], 4U);
    adcVersion->buildVer       = adrv903x_CpuIntFromBytesGet(&ctrlResp[12], 4U);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcGetDataInfo(adi_adrv903x_Device_t* const  device,
                                                         const uint8_t                 calType,
                                                         const adi_adrv903x_Channels_e channel,
                                                         adrv903x_CpuCmd_GetAdcSizeAddrs_t* const adcSizes,
                                                         adrv903x_CpuCmd_GetAdcSizeAddrs_t* const adcAddrs)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, adcSizes);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, adcAddrs);

    uint32_t lengthResp = 4U;
    uint8_t  ctrlData[4] = { 0U };
    uint8_t  ctrlResp[24] = { 0U };

    if ((calType != ADRV903X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if (calType == ADRV903X_CPU_OBJID_IC_ADC_ORX)
    {
        if ((channel != ADI_ADRV903X_CH0) &&
            (channel != ADI_ADRV903X_CH1))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad ORx channel");
            return recoveryAction;
        }
    }
    else
    {
        if ((channel != ADI_ADRV903X_CH0) &&
            (channel != ADI_ADRV903X_CH1) &&
            (channel != ADI_ADRV903X_CH2) &&
            (channel != ADI_ADRV903X_CH3) &&
            (channel != ADI_ADRV903X_CH4) &&
            (channel != ADI_ADRV903X_CH5) &&
            (channel != ADI_ADRV903X_CH6) &&
            (channel != ADI_ADRV903X_CH7))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad Rx/Tx channel");
            return recoveryAction;
        }
    }

    /* Get the ADC module data sizes: */
    recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_GET_DATA_SIZES,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "Get ADC data sizes");
        return recoveryAction;
    }

    /* Store the adcSizes */
    adcSizes->obj      = adrv903x_CpuIntFromBytesGet(&ctrlResp[ 0], 4U);
    adcSizes->init     = adrv903x_CpuIntFromBytesGet(&ctrlResp[ 4], 4U);
    adcSizes->config   = adrv903x_CpuIntFromBytesGet(&ctrlResp[ 8], 4U);
    adcSizes->calObj   = adrv903x_CpuIntFromBytesGet(&ctrlResp[12], 4U);
    adcSizes->calState = adrv903x_CpuIntFromBytesGet(&ctrlResp[16], 4U);
    adcSizes->calData  = adrv903x_CpuIntFromBytesGet(&ctrlResp[20], 4U);

    /* Get the ADC module data addresses: */
    recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_GET_DATA_ADDRS,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "Get ADC data addresses");
        return recoveryAction;
    }

    /* Store the ADC Addresses */
    adcAddrs->obj      = adrv903x_CpuIntFromBytesGet(&ctrlResp[ 0], 4U);
    adcAddrs->init     = adrv903x_CpuIntFromBytesGet(&ctrlResp[ 4], 4U);
    adcAddrs->config   = adrv903x_CpuIntFromBytesGet(&ctrlResp[ 8], 4U);
    adcAddrs->calObj   = adrv903x_CpuIntFromBytesGet(&ctrlResp[12], 4U);
    adcAddrs->calState = adrv903x_CpuIntFromBytesGet(&ctrlResp[16], 4U);
    adcAddrs->calData  = adrv903x_CpuIntFromBytesGet(&ctrlResp[20], 4U);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcRunInit(adi_adrv903x_Device_t* const  device,
                                                     const uint8_t                 calType,
                                                     const adi_adrv903x_Channels_e channel)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    uint32_t lengthResp = 4U;
    uint8_t ctrlData[4] = { 0U };
    uint8_t ctrlResp[16] = { 0U };

    if ((calType != ADRV903X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if (calType == ADRV903X_CPU_OBJID_IC_ADC_ORX)
    {
        if ((channel != ADI_ADRV903X_CH0) &&
            (channel != ADI_ADRV903X_CH1))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad ORx channel");
            return recoveryAction;
        }
    }
    else
    {
        if ((channel != ADI_ADRV903X_CH0) &&
            (channel != ADI_ADRV903X_CH1) &&
            (channel != ADI_ADRV903X_CH2) &&
            (channel != ADI_ADRV903X_CH3) &&
            (channel != ADI_ADRV903X_CH4) &&
            (channel != ADI_ADRV903X_CH5) &&
            (channel != ADI_ADRV903X_CH6) &&
            (channel != ADI_ADRV903X_CH7))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad Rx/Tx channel");
            return recoveryAction;
        }
    }

    recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_INIT,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcRunInit failed");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcSetFsmCmd(adi_adrv903x_Device_t* const     device,
                                                       const uint8_t                    calType,
                                                       const adi_adrv903x_Channels_e    channel,
                                                       const uint8_t                    fsmCmd)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    uint32_t lengthResp = 4U;
    uint8_t ctrlData[4] = { 0U };
    uint8_t ctrlResp[16] = { 0U };

    if ((calType != ADRV903X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if (calType == ADRV903X_CPU_OBJID_IC_ADC_ORX)
    {
        if ((channel != ADI_ADRV903X_CH0) &&
            (channel != ADI_ADRV903X_CH1))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcSetFsmCmd failed: bad ORx channel");
            return recoveryAction;
        }
    }
    else
    {
        if ((channel != ADI_ADRV903X_CH0) &&
            (channel != ADI_ADRV903X_CH1) &&
            (channel != ADI_ADRV903X_CH2) &&
            (channel != ADI_ADRV903X_CH3) &&
            (channel != ADI_ADRV903X_CH4) &&
            (channel != ADI_ADRV903X_CH5) &&
            (channel != ADI_ADRV903X_CH6) &&
            (channel != ADI_ADRV903X_CH7))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcSetFsmCmd failed: bad Rx/Tx channel");
            return recoveryAction;
        }
    }

    ctrlData[0] = fsmCmd;

    recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_SET_FSM_CMD,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcSetFsmCmd failed");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcGetFsmState(adi_adrv903x_Device_t* const  device,
                                                         const uint8_t                 calType,
                                                         const adi_adrv903x_Channels_e channel,
                                                         uint8_t* const                fsmState)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, fsmState);
    
    uint32_t lengthResp = 4U;
    uint8_t  ctrlData[4] = { 0U };
    uint8_t  ctrlResp[24] = { 0U };

    if ((calType != ADRV903X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if (calType == ADRV903X_CPU_OBJID_IC_ADC_ORX)
    {
        if ((channel != ADI_ADRV903X_CH0) &&
            (channel != ADI_ADRV903X_CH1))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad ORx channel");
            return recoveryAction;
        }
    }
    else
    {
        if ((channel != ADI_ADRV903X_CH0) &&
            (channel != ADI_ADRV903X_CH1) &&
            (channel != ADI_ADRV903X_CH2) &&
            (channel != ADI_ADRV903X_CH3) &&
            (channel != ADI_ADRV903X_CH4) &&
            (channel != ADI_ADRV903X_CH5) &&
            (channel != ADI_ADRV903X_CH6) &&
            (channel != ADI_ADRV903X_CH7))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad Rx/Tx channel");
            return recoveryAction;
        }
    }

    /* Get the ADC module data sizes: */
    recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_GET_FSM_STATE,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "Get FSM state");
        return recoveryAction;
    }

    *fsmState = ctrlResp[0];

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_AdcRunCmd(adi_adrv903x_Device_t* const     device,
                                                    const uint8_t                    calType,
                                                    const adi_adrv903x_Channels_e    channel)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    uint32_t lengthResp = 4U;
    uint8_t ctrlData[4] = { 0U };
    uint8_t ctrlResp[16] = { 0U };

    if ((calType != ADRV903X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV903X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if (calType == ADRV903X_CPU_OBJID_IC_ADC_ORX)
    {
        if ((channel != ADI_ADRV903X_CH0) &&
            (channel != ADI_ADRV903X_CH1))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcRunCmd failed: bad ORx channel");
            return recoveryAction;
        }
    }
    else
    {
        if ((channel != ADI_ADRV903X_CH0) &&
            (channel != ADI_ADRV903X_CH1) &&
            (channel != ADI_ADRV903X_CH2) &&
            (channel != ADI_ADRV903X_CH3) &&
            (channel != ADI_ADRV903X_CH4) &&
            (channel != ADI_ADRV903X_CH5) &&
            (channel != ADI_ADRV903X_CH6) &&
            (channel != ADI_ADRV903X_CH7))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcRunCmd failed: bad Rx/Tx channel");
            return recoveryAction;
        }
    }

    ctrlData[0] = 0;

    recoveryAction = adi_adrv903x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_RUN_CMD,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcRunCmd failed");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_SetTrackingPeriod(adi_adrv903x_Device_t* const  device,
                                                            uint8_t objId,
                                                            uint32_t rateMsec)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    uint16_t offset;
    uint32_t configDataLength = 4;
    uint8_t  configDataSet[4] = { 0 };

    if ((objId < ADRV903X_CPU_OBJID_TC_START) ||
        (objId > ADRV903X_CPU_OBJID_TC_END))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Invalid objId");
        return recoveryAction;
    }

    offset = (objId - ADRV903X_CPU_OBJID_TC_START) * sizeof(uint32_t);

    configDataSet[0] = (uint8_t)((rateMsec >>  0) & 0xFFu);
    configDataSet[1] = (uint8_t)((rateMsec >>  8) & 0xFFu);
    configDataSet[2] = (uint8_t)((rateMsec >> 16) & 0xFFu);
    configDataSet[3] = (uint8_t)((rateMsec >> 24) & 0xFFu);

    /* Get the ADC module data sizes: */
    recoveryAction = adi_adrv903x_CpuConfigSet(device,
                                               (uint32_t)ADRV903X_CPU_OBJID_CFG_TRACKING_CALS,
                                               offset,
                                               configDataSet,
                                               configDataLength);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Set tracking period");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_GetTrackingPeriod(adi_adrv903x_Device_t* const  device,
                                                            uint8_t objId,
                                                            uint32_t* const rateMsec)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    uint16_t offset;
    uint32_t configDataLength = 4;
    uint8_t  configDataGet[4] = { 0 };

    if ((objId < ADRV903X_CPU_OBJID_TC_START) ||
        (objId > ADRV903X_CPU_OBJID_TC_END))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Invalid objId");
        return recoveryAction;
    }

    offset = (objId - ADRV903X_CPU_OBJID_TC_START) * sizeof(uint32_t);

    /* Get the ADC module data sizes: */
    recoveryAction = adi_adrv903x_CpuConfigGet(device,
                                               (uint32_t)ADRV903X_CPU_OBJID_CFG_TRACKING_CALS,
                                               offset,
                                               configDataGet,
                                               configDataLength);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Set tracking period");
        return recoveryAction;
    }

    *rateMsec = adrv903x_CpuIntFromBytesGet(&configDataGet[0], 4U);

    return recoveryAction;
}

