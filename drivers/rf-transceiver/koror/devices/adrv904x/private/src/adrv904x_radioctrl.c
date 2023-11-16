/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_radioctrl.c
 * \brief Contains ADRV904X radio control related private function implementations.
 *
 * ADRV904X API Version: 2.9.0.4
 */
#include "../../private/include/adrv904x_radioctrl.h"
#include "../../private/include/adrv904x_rx.h"
#include "../../private/include/adrv904x_tx.h"
#include "../../private/bf/adrv904x_bf_core.h"
#include "../../private/include/adrv904x_init.h"
#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_cpu_cmd_ctrl.h"
#include "../../private/bf/adrv904x_bf_rx_dig.h"
#include "../../private/bf/adrv904x_bf_tx_dig.h"
#include "../../private/include/adrv904x_stream_proc_types.h"
#include "../../private/include/adrv904x_cpu_scratch_registers.h"

#include "../../private/include/adrv904x_dfe_svc_radio_ctrl_sequencer_t.h"
#include "../../private/include/adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_cpu_dfe_init_memory.h"
#include "../../private/include/adrv904x_dfe_memory.h"
#include "../../private/include/adrv904x_gpio.h"
#include "../../private/bf/adrv904x_bf_core.h"

#include "adi_adrv904x_rx_types.h"
#include "adi_adrv904x_tx_types.h"
#include "adi_adrv904x_cals.h"
#include "adi_adrv904x_cpu.h"
#include "adi_adrv904x_types.h"

#include "../../private/bf/adrv904x_bf_sequencer.h"


#define ADI_FILE    ADI_ADRV904X_FILE_PRIVATE_RADIOCTRL

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxEnableSet(adi_adrv904x_Device_t* const device,
                                                       const uint32_t rxChannelMask,
                                                       const uint32_t rxChannelEnable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t orxChanMask = rxChannelMask >> ADI_ADRV904X_MAX_RX_ONLY;
    uint8_t orxChanEnable = rxChannelEnable >> ADI_ADRV904X_MAX_RX_ONLY;
    uint8_t bfValue = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction =   adrv904x_Core_RadioControlInterfaceOrxSpiEn_BfGet(device,
                                                                         NULL,
                                                                         (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                         &bfValue);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control Orx channels.");
        return recoveryAction;
    }

    /* Update the bfValue */
    ADRV904X_BF_UPDATE(bfValue, orxChanEnable, orxChanMask, 0U);

    recoveryAction =  adrv904x_Core_RadioControlInterfaceOrxSpiEn_BfSet(device,
                                                                        NULL,
                                                                        (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                        bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control ORX channels.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxEnableSet(adi_adrv904x_Device_t* const device,
                                                      const uint32_t rxChannelMask,
                                                      const uint32_t rxChannelEnable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t bfValue = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction =   adrv904x_Core_RadioControlInterfaceRxSpiEn_BfGet(device,
                                                                        NULL,
                                                                        (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                        &bfValue);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control RX channels.");
        return recoveryAction;
    }

    /* Update the bfValue */
    ADRV904X_BF_UPDATE(bfValue, (uint8_t)rxChannelEnable, (uint8_t)rxChannelMask, 0U);

    recoveryAction =   adrv904x_Core_RadioControlInterfaceRxSpiEn_BfSet(device,
                                                                        NULL,
                                                                        (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                        bfValue);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control RX channels.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxEnableSet(adi_adrv904x_Device_t* const device,
                                                      const uint32_t txChannelMask,
                                                      const uint32_t txChannelEnable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t bfValue = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction =   adrv904x_Core_RadioControlInterfaceTxSpiEn_BfGet(device,
                                                                        NULL,
                                                                        (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                        &bfValue);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control TX channels.");
        return recoveryAction;
    }

    /* Update the bfValue */
    ADRV904X_BF_UPDATE(bfValue, (uint8_t)txChannelEnable, (uint8_t)txChannelMask, 0U);

    recoveryAction =  adrv904x_Core_RadioControlInterfaceTxSpiEn_BfSet(device,
                                                                       NULL,
                                                                       (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                       bfValue);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable Radio Control TX channels.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxTxEnableSetRangeCheck(adi_adrv904x_Device_t* const device,
                                                                  const uint32_t               orxChannelMask,
                                                                  const uint32_t               orxChannelEnable,
                                                                  const uint32_t               rxChannelMask,
                                                                  const uint32_t               rxChannelEnable,
                                                                  const uint32_t               txChannelMask,
                                                                  const uint32_t               txChannelEnable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    /*TODO: update this static const once Tx channels have been numbered from Tx/Rx 0 - Tx/Rx7 */
    static const uint32_t ALL_TX_MASK = (ADI_ADRV904X_TX0 | ADI_ADRV904X_TX1 | ADI_ADRV904X_TX2 | ADI_ADRV904X_TX3 |
                                         ADI_ADRV904X_TX4 | ADI_ADRV904X_TX5 | ADI_ADRV904X_TX6 | ADI_ADRV904X_TX7);
    static const uint32_t ALL_RX_MASK = (ADI_ADRV904X_RX0 | ADI_ADRV904X_RX1 | ADI_ADRV904X_RX2 | ADI_ADRV904X_RX3 |
                                         ADI_ADRV904X_RX4 | ADI_ADRV904X_RX5 | ADI_ADRV904X_RX6 | ADI_ADRV904X_RX7);
    static const uint32_t ALL_ORX_MASK = (ADI_ADRV904X_ORX0 | ADI_ADRV904X_ORX1);

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    (void)orxChannelEnable;
    (void)rxChannelEnable;
    (void)txChannelEnable;

    /* Check that ARM and Stream processors have been loaded before enabling */
    if (device->devStateInfo.devState < ADI_ADRV904X_STATE_ALLCPUSLOADED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Channel Enable/Disable is valid only after loading of CPU(s) & Stream processors");
        return recoveryAction;
    }

    /* Check that ORx channel mask is valid */
    if ((orxChannelMask & (~(uint32_t)ALL_ORX_MASK)) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            NULL,
            "Invalid ORx Channel mask encountered while attempting to enable ORx signal chain in SPI mode.");
        return recoveryAction;
    }

    /* Check that Rx channel mask is valid */
    if (rxChannelMask > ALL_RX_MASK)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               NULL,
                               "Invalid Rx Channel mask encountered while attempting to enable Rx signal chain in SPI mode.");
        return recoveryAction;
    }

    /* Check that Tx channel mask is valid */
    if (txChannelMask > ALL_TX_MASK)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            txChannelMask,
            "Invalid Tx Channel mask encountered while attempting to enable Tx signal chain in SPI mode.");
        return recoveryAction;
    }

    /* Check that requested ORx channels are initialized */
    if ((orxChannelMask & device->devStateInfo.initializedChannels) != orxChannelMask)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            rxChannelMask,
            "Requested ORx channels to enable are not initialized");
        return recoveryAction;
    }

    /* Check that requested Rx/ORx channels are initialized */
    if ((rxChannelMask & device->devStateInfo.initializedChannels) != rxChannelMask)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Requested Rx channels to enable are not initialized");
        return recoveryAction;
    }

    /* Check that requested Tx channels are initialized */
    if ((txChannelMask & (device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET)) != txChannelMask)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            txChannelMask,
            "Requested Tx channels to be enable are not initialized");
        return recoveryAction;
    }

    return ADI_ADRV904X_ERR_ACT_NONE;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioCtrlCfgSetRangeCheck(adi_adrv904x_Device_t* const                    device,
                                                                    const adi_adrv904x_RadioCtrlModeCfg_t* const    radioCtrlCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, radioCtrlCfg);

    static const uint32_t ALL_RX_MASK = (ADI_ADRV904X_RX0 | ADI_ADRV904X_RX1 | ADI_ADRV904X_RX2 | ADI_ADRV904X_RX3 |
                                     ADI_ADRV904X_RX4 | ADI_ADRV904X_RX5 | ADI_ADRV904X_RX6 | ADI_ADRV904X_RX7);
    static const uint32_t ALL_ORX_MASK = (ADI_ADRV904X_ORX0 | ADI_ADRV904X_ORX1);

    if (radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV904X_RX_EN_INVALID_MODE)
    {
        /*check if Rx profile is valid*/
        if ((device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID) != ADI_ADRV904X_RX_PROFILE_VALID)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                device->devStateInfo.profilesValid,
                "No valid Rx profile is loaded.");
            return recoveryAction;
        }

        /*Check that if requested Rx Channel is valid*/
        if (((radioCtrlCfg->rxRadioCtrlModeCfg.rxChannelMask & (~(uint32_t)ALL_RX_MASK)) != 0U))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->rxRadioCtrlModeCfg.rxChannelMask,
                "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
            return recoveryAction;
        }

        if ((radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV904X_RX_EN_SPI_MODE) &&
            (radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV904X_RX_EN_PIN_MODE))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode,
                "Invalid Rx signal chain enable mode selected. Valid values include SPI, Pin mode");
            return recoveryAction;
        }
    }

    if (radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV904X_TX_EN_INVALID_MODE)
    {
        /*check if Tx profile is valid*/
        if ((device->devStateInfo.profilesValid & ADI_ADRV904X_TX_PROFILE_VALID) != ADI_ADRV904X_TX_PROFILE_VALID)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                device->devStateInfo.profilesValid,
                "No valid Tx profile is loaded.");
            return recoveryAction;
        }

        /*Check that if requested Tx Channel is valid*/
        if (((radioCtrlCfg->txRadioCtrlModeCfg.txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->txRadioCtrlModeCfg.txChannelMask,
                "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
            return recoveryAction;

        }

        if ((radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV904X_TX_EN_SPI_MODE) &&
        (radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV904X_TX_EN_PIN_MODE))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode,
                "Invalid Tx signal chain enable mode selected. Valid values include SPI, Pin mode");
            return recoveryAction;
        }
    }

    if (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV904X_ORX_EN_INVALID_MODE)
    {
        if ((device->devStateInfo.profilesValid & ADI_ADRV904X_ORX_PROFILE_VALID) != ADI_ADRV904X_ORX_PROFILE_VALID)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                device->devStateInfo.profilesValid,
                "No valid ORx profile is loaded.");
            return recoveryAction;
        }

        /*Check that if requested ORx Channel is valid*/
        if ((radioCtrlCfg->orxRadioCtrlModeCfg.orxChannelMask & (~(uint32_t)ALL_ORX_MASK)))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->orxRadioCtrlModeCfg.orxChannelMask,
                "Invalid ORx channel is selected. Valid values are any combinations of ORx0/1");
            return recoveryAction;
        }

        if ((radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV904X_ORX_EN_SPI_MODE) &&
        (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV904X_ORX_EN_PIN_MODE))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode,
                "Invalid ORx signal chain enable mode selected. Valid values include SPI, Pin mode");
            return recoveryAction;
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxRadioCtrlCfgSet(adi_adrv904x_Device_t * const               device,
                                                            adi_adrv904x_RxRadioCtrlModeCfg_t* const    rxRadioCtrlModeCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t armModeRxChanMask = 0x0U;  /* Bit N indicates RxN is in ARM mode. ARM mode overrides SPI/Pin-mode. */
    uint8_t spiModeRxChanMask = 0x0U;  /* If RxN is not in ARM mode then bit N here indicates RxN is SPI-mode (1) or Pin-mode (0) */

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxRadioCtrlModeCfg);

    /* Fetch the current Spi-enabled/Pin-enabled and ARM override status for Rx channels */
    recoveryAction = adrv904x_Core_RadioControlInterfaceRxSpiModeSel_BfGet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                           &spiModeRxChanMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get RX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* For both pin or SPI mode we disable ARM MODE for the Rx channels */
    armModeRxChanMask = ((uint8_t) rxRadioCtrlModeCfg->rxChannelMask);
    recoveryAction = adrv904x_Core_RadioControlInterfaceRxArmModeSelClr_BfSet(device,
                                                                              NULL,
                                                                              (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                              armModeRxChanMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    if (rxRadioCtrlModeCfg->rxEnableMode == ADI_ADRV904X_RX_EN_SPI_MODE)
    {
        /* Enable Spi mode for the channels - turn on the relevant bits in SpiModeSel register */
        spiModeRxChanMask |= rxRadioCtrlModeCfg->rxChannelMask;

    }
    else if (rxRadioCtrlModeCfg->rxEnableMode == ADI_ADRV904X_RX_EN_PIN_MODE)
    {
        /* Enable pin-mode for the channels - turn off the relevant bits in SpiModeSel register */
        spiModeRxChanMask &= ~((uint8_t) rxRadioCtrlModeCfg->rxChannelMask);
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            rxRadioCtrlModeCfg->rxEnableMode,
            "Invalid Rx Enable Mode encountered while attempting to configure Rx radio ctrl configuration settings");
        return recoveryAction;
    }

    /* Write the updated SpiMode register */
    recoveryAction = adrv904x_Core_RadioControlInterfaceRxSpiModeSel_BfSet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                           (uint8_t) spiModeRxChanMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxRadioCtrlCfgSet(adi_adrv904x_Device_t* const                device,
                                                            adi_adrv904x_TxRadioCtrlModeCfg_t* const    txRadioCtrlModeCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t txSpiModeSel = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, txRadioCtrlModeCfg);

    /* Disable ARM MODE for Tx ctrl configuration */
    recoveryAction = adrv904x_Core_RadioControlInterfaceTxArmModeSelClr_BfSet(device,
                                                                              NULL,
                                                                              (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                              (uint8_t) txRadioCtrlModeCfg->txChannelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* Read Spi mode */
    recoveryAction = adrv904x_Core_RadioControlInterfaceTxSpiModeSel_BfGet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                           &txSpiModeSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /*For Spi mode set Spi Mode bitfield to 1 and for pin mode set the Spi mode bitfield to 0*/
    if (txRadioCtrlModeCfg->txEnableMode == ADI_ADRV904X_TX_EN_SPI_MODE)
    {
        /* Enable Spi mode */
        txSpiModeSel |= (uint8_t) txRadioCtrlModeCfg->txChannelMask;

    }
    else if (txRadioCtrlModeCfg->txEnableMode == ADI_ADRV904X_TX_EN_PIN_MODE)
    {
        /* Disable Spi mode */
        txSpiModeSel &= ~((uint8_t) txRadioCtrlModeCfg->txChannelMask);
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                                recoveryAction,
                                txRadioCtrlModeCfg->txEnableMode,
                                "Invalid Tx Enable Mode encountered while attempting to configure Tx radio ctrl configuration settings");
        return recoveryAction;
    }

    recoveryAction =  adrv904x_Core_RadioControlInterfaceTxSpiModeSel_BfSet(device,
                                                                            NULL,
                                                                            (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                            txSpiModeSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxRadioCtrlCfgSet(adi_adrv904x_Device_t* const                device,
                                                             adi_adrv904x_ORxRadioCtrlModeCfg_t* const   orxRadioCtrlModeCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t orxSpiModeSel = 0U;
    uint8_t orxChanMask = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, orxRadioCtrlModeCfg);

    orxChanMask = (uint8_t) (orxRadioCtrlModeCfg->orxChannelMask >> ADI_ADRV904X_MAX_RX_ONLY);

    /* Disable ARM override ctrl for ORx ctrl configuration */
    recoveryAction = adrv904x_Core_RadioControlInterfaceOrxArmModeSelClr_BfSet(device,
                                                                               NULL,
                                                                               (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                               orxChanMask);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API ORX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* Read Spi mode sel */
    recoveryAction = adrv904x_Core_RadioControlInterfaceOrxSpiModeSel_BfGet(device,
                                                                            NULL,
                                                                            (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                            &orxSpiModeSel);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API ORX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /*For Spi mode set Spi Mode bitfield to 1 and for pin mode set the Spi mode bitfield to 0*/
    if (orxRadioCtrlModeCfg->orxEnableMode == ADI_ADRV904X_ORX_EN_SPI_MODE)
    {

        /* Enable Spi mode */
        orxSpiModeSel |= orxChanMask;
    }
    else if (orxRadioCtrlModeCfg->orxEnableMode == ADI_ADRV904X_ORX_EN_PIN_MODE)
    {
        /* Disable Spi mode */
        orxSpiModeSel &= ~orxChanMask;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            orxRadioCtrlModeCfg->orxEnableMode,
            "Invalid ORx Enable Mode encountered while attempting to configure Rx radio ctrl configuration settings");
        return recoveryAction;
    }

    recoveryAction = adrv904x_Core_RadioControlInterfaceOrxSpiModeSel_BfSet(device,
                                                                            NULL,
                                                                            (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                            orxSpiModeSel);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API ORX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxRadioCtrlCfgGet(adi_adrv904x_Device_t* const                device,
                                                             const adi_adrv904x_RxChannels_e             rxChannel,
                                                             adi_adrv904x_ORxRadioCtrlModeCfg_t* const   orxRadioCtrlModeCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t  orxArmMode = 0U;
    uint8_t  orxChannel = 0U;
    uint8_t  orxSpiMode = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, orxRadioCtrlModeCfg);

    orxChannel = (uint8_t)(rxChannel >> ADI_ADRV904X_MAX_RX_ONLY);
    /* Verify only one ORX channel is set */
    if (orxChannel & (orxChannel - 1U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid ORX Channel selection.");
        return recoveryAction;
    }

    orxRadioCtrlModeCfg->orxChannelMask = 0;
    orxRadioCtrlModeCfg->orxEnableMode = ADI_ADRV904X_ORX_EN_INVALID_MODE;

    /* Read ORX ARM Override ctrl bitfield*/
    recoveryAction =  adrv904x_Core_RadioControlInterfaceOrxArmModeSel_BfGet(device,
                                                                             NULL,
                                                                             (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                             &orxArmMode);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get ORX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* The Spi mode bitfield is only valid if ARM override ctrl is set to 0 */
    if ((orxArmMode & orxChannel) == 0)
    {
        /* Read Pin ctrl mode */
        recoveryAction =  adrv904x_Core_RadioControlInterfaceOrxSpiModeSel_BfGet(device,
                                                                                 NULL,
                                                                                 (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                                 &orxSpiMode);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get ORX Radio Control Cfg Failed.");
            return recoveryAction;
        }

        /* 'Spi Mode' bit field: 1 (Spi Mode), 0 (Pin Mode) */
        if ((orxChannel & orxSpiMode) == orxChannel)
        {
            orxRadioCtrlModeCfg->orxEnableMode = ADI_ADRV904X_ORX_EN_SPI_MODE;
        }
        else
        {
            orxRadioCtrlModeCfg->orxEnableMode = ADI_ADRV904X_ORX_EN_PIN_MODE;
        }
    }

    /*Update the ORx channel mask*/
    orxRadioCtrlModeCfg->orxChannelMask = rxChannel;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxRadioCtrlCfgGet(adi_adrv904x_Device_t* const                device,
                                                            const adi_adrv904x_RxChannels_e             rxChannel,
                                                            adi_adrv904x_RxRadioCtrlModeCfg_t * const   rxRadioCtrlModeCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t  rxArmMode = 0U;
    uint8_t  rxSpiMode = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxRadioCtrlModeCfg);

    rxRadioCtrlModeCfg->rxChannelMask = 0;
    rxRadioCtrlModeCfg->rxEnableMode = ADI_ADRV904X_RX_EN_INVALID_MODE;

    /* Verify only one ORX channel is set */
    if ((rxChannel & ~ADI_ADRV904X_RX_MASK_ALL) || (rxChannel & (rxChannel - 1U)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid RX Channel selection.");
        return recoveryAction;
    }

    /* Read RX ARM Override ctrl bitfield*/
    recoveryAction =  adrv904x_Core_RadioControlInterfaceRxArmModeSel_BfGet(device,
                                                                            NULL,
                                                                            (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                            &rxArmMode);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get RX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* The Spi mode bitfield is only valid if ARM override ctrl is set to 0 */
    if ((rxArmMode & rxChannel) == 0)
    {
        /* Read Pin ctrl mode */
        recoveryAction =  adrv904x_Core_RadioControlInterfaceRxSpiModeSel_BfGet(device,
                                                                                NULL,
                                                                                (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                                &rxSpiMode);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get RX Radio Control Cfg Failed.");
            return recoveryAction;
        }

        /* 'Spi Mode' bit field: 1 (Spi Mode), 0 (Pin Mode) */
        if ((rxChannel & rxSpiMode) == rxChannel)
        {
            rxRadioCtrlModeCfg->rxEnableMode = ADI_ADRV904X_RX_EN_SPI_MODE;
        }
        else
        {
            rxRadioCtrlModeCfg->rxEnableMode = ADI_ADRV904X_RX_EN_PIN_MODE;
        }
    }

    /*Update the ORx channel mask*/
    rxRadioCtrlModeCfg->rxChannelMask = rxChannel;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxRadioCtrlCfgGet(adi_adrv904x_Device_t * const device,
                                                            const adi_adrv904x_TxChannels_e txChannel,
                                                            adi_adrv904x_TxRadioCtrlModeCfg_t * const txRadioCtrlModeCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t  txArmMode = 0U;
    uint8_t  txSpiMode = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, txRadioCtrlModeCfg);

    txRadioCtrlModeCfg->txChannelMask = 0;
    txRadioCtrlModeCfg->txEnableMode = ADI_ADRV904X_TX_EN_INVALID_MODE;

    /* Verify only one TX channel is set */
    if (txChannel & (txChannel - 1U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid TX Channel selection.");
        return recoveryAction;
    }

    /*Read TX ARM Mode ctrl bitfield*/
    recoveryAction =  adrv904x_Core_RadioControlInterfaceTxArmModeSel_BfGet(device,
                                                                            NULL,
                                                                            (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                            &txArmMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get TX Radio Control Cfg Failed.");
        return recoveryAction;
    }

    /* The Spi mode bitfield is only valid if ARM mode ctrl is not set to 1 */
    if ((txArmMode & txChannel) == 0)
    {
        /* Read TX Pin ctrl mode */
        recoveryAction =  adrv904x_Core_RadioControlInterfaceTxSpiModeSel_BfGet(device,
                                                                                NULL,
                                                                                (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                                &txSpiMode);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get TX Radio Control Cfg Failed.");
            return recoveryAction;
        }
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get TX Radio Control Cfg Failed.");
            return recoveryAction;
        }

        /* 'Spi Mode' bit field: 1 (Spi Mode), 0 (Pin Mode) */
        if ((txChannel & txSpiMode) == txChannel)
        {
            txRadioCtrlModeCfg->txEnableMode = ADI_ADRV904X_TX_EN_SPI_MODE;
        }
        else
        {
            txRadioCtrlModeCfg->txEnableMode = ADI_ADRV904X_TX_EN_PIN_MODE;
        }
    }

    /*Update the Tx Channel mask*/
    txRadioCtrlModeCfg->txChannelMask = txChannel;

    return recoveryAction;
}


adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierConfigSet(adi_adrv904x_Device_t* const device,
                                                                      const adi_adrv904x_RadioCtrlAntCalCarrierCfg_t* const antCalCarrierCfg,
                                                                      uint32_t postCalPatternId,
                                                                      uint8_t useV2)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint16_t SCRATCHPAD_REGISTER_OFFSET_BETWEEN_RS = 54U;
    uint16_t scratchpadOffsetToStoreRsPatternAddress = 0U;
    uint32_t globalPatternId = 0U;
    uint8_t sequencerId = 0U;
    uint32_t patternAddr = 0U;
    int32_t carrierGain = 0;
    uint32_t carrierGainReg = 0U;
    uint32_t i = 0U;
    const int32_t DIG_GAIN_MIN = -90000;
    const int32_t DIG_GAIN_MAX = 36000;
    const uint32_t DIG_GAIN_MULT = 65536U;
    uint32_t extendedCoreScratchRegBaseAddress = 0U;
    uint32_t tempAddress = 0U;
    uint8_t origEccState = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, antCalCarrierCfg, cleanup);

    /* Check that Radio Sequencer Enable */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Sequencer not enabled");
        goto cleanup;
    }

    /* Write pattern ID to scratch reg */
    if (antCalCarrierCfg->patternId > ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NUM_PATTERNS_PER_SEQUENCER)
    {
        /* Invalid pattern ID value */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, antCalCarrierCfg->patternId, "Invalid pattern ID value. Valid value are from 0 to 31");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_PATTERN_ID_TO_SWITCH_BYTE_0,
                                                    antCalCarrierCfg->patternId);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing pattern ID");
        goto cleanup;
    }

    /* For Radio Sequencer 0 and 1 */
    for (sequencerId = 0; sequencerId < 2; sequencerId++)
    {
        scratchpadOffsetToStoreRsPatternAddress = (uint16_t)(ADRV904X_ANTENNA_CAL_RS0_PATTERN0_ADDR_LSB + (sequencerId * SCRATCHPAD_REGISTER_OFFSET_BETWEEN_RS));

        if (useV2 == ADI_FALSE)
        {
            recoveryAction = adrv904x_Sequencer_ExternalJumpPointer_BfGet(device, 
                                                                          NULL, 
                                                                          ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS, 
                                                                          sequencerId,
                                                                          (uint16_t* const)&patternAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get Radio Sequencer pattern address");
                goto cleanup;
            }

            /* Shift to the location expected in the scratchpad */
            patternAddr = patternAddr << 4;
        }
        else
        {
            globalPatternId = ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SEQ_PTRN_ID_TO_GLOBAL_ID(sequencerId, postCalPatternId);
            recoveryAction = adrv904x_RadioSequencerPatternAddrGet(device, globalPatternId, &patternAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get Radio Sequencer default pattern address");
                goto cleanup;
            }
        }



        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        scratchpadOffsetToStoreRsPatternAddress,
                                                        (uint8_t)(patternAddr & 0xFF));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, patternAddr, "Error writing LSB of pattern address");
            goto cleanup;
        }

        scratchpadOffsetToStoreRsPatternAddress++;
        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        scratchpadOffsetToStoreRsPatternAddress,
                                                        (uint8_t)((patternAddr >> 8) & 0xFF));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, patternAddr, "Error writing MSB of pattern address");
            goto cleanup;
        }

        globalPatternId = ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SEQ_PTRN_ID_TO_GLOBAL_ID(sequencerId, antCalCarrierCfg->patternId);

        recoveryAction = adrv904x_RadioSequencerPatternAddrGet(device, globalPatternId, &patternAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get Radio Sequencer default pattern address");
            goto cleanup;
        }

        if ((patternAddr & 0x0F) != 0)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "The pattern addresses should be aligned to 16-byte boundary");
            goto cleanup;
        }

        scratchpadOffsetToStoreRsPatternAddress++;
        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        scratchpadOffsetToStoreRsPatternAddress,
                                                        (uint8_t)(patternAddr & 0xFF));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, patternAddr, "Error writing LSB of pattern address");
            goto cleanup;
        }

        scratchpadOffsetToStoreRsPatternAddress++;
        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        scratchpadOffsetToStoreRsPatternAddress,
                                                        (uint8_t)((patternAddr >> 8) & 0xFF));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, patternAddr, "Error writing MSB of pattern address");
            goto cleanup;
        }
    }

    /* Write GPIO number of toggles to scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_NUMBER_OF_GPIO_TO_SWITCH_CARRIER,
                                                    (uint8_t)(antCalCarrierCfg->numOfGpioToSwitchCarrier));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while write numOfGpioToSwitchCarrier");
        goto cleanup;
    }

    /* Write carrier table to scratch reg */
    for (i = 0U; i < ADI_ADRV904X_CARRIER_TABLE_COUNT; i++)
    {
        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        (uint16_t)(ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_BYTE_0 + i),
                                                        antCalCarrierCfg->carrierTable[i]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, i, "Error while write carrierTable");
            goto cleanup;
        }
    }

    /* Check first pattern number of loops value */
    if (antCalCarrierCfg->firstPatternNumberOfLoops == 0)
    {
        /* Invalid first pattern number of loops value */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, antCalCarrierCfg->firstPatternNumberOfLoops,
                               "Invalid first pattern number of loops value. Valid value are from 1 to 255");
        goto cleanup;
    }

    /* Write first pattern number of loops to scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_FIRST_PATTERN_NUMBER_OF_LOOPS,
                                                    antCalCarrierCfg->firstPatternNumberOfLoops - 1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while write firstPatternNumberOfLoops");
        goto cleanup;
    }

    /* Check second pattern number of loops value */
    if (antCalCarrierCfg->secondPatternNumberOfLoops == 0)
    {
        /* Invalid second pattern number of loops value */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, antCalCarrierCfg->secondPatternNumberOfLoops,
                               "Invalid second pattern number of loops value. Valid value are from 1 to 255");
        goto cleanup;
    }

    /* Write second pattern number of loops to scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_SECOND_PATTERN_NUMBER_OF_LOOPS,
                                                    antCalCarrierCfg->secondPatternNumberOfLoops - 1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while write secondPatternNumberOfLoops");
        goto cleanup;
    }

    /* Read the base address for extended core scratch registers located in M4 heap*/
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, ADRV904X_PM_CORE_SCRATCH_EXT_PTR, &extendedCoreScratchRegBaseAddress, 0xFFFFFFFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading extended core stream scratch base addr");
        goto cleanup;
    }

    /* Read the current ECC scrubbing state in M4 before disabling ECC */
    recoveryAction =  adi_adrv904x_EccEnableGet(device, &origEccState);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading ECC state of M4");
        return recoveryAction;
    }
    
    /* Disable ECC before we write to M4 memory */
    recoveryAction =  adi_adrv904x_EccEnableSet(device, 0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disabling ECC scrubbing on M4");
        return recoveryAction;
    }
    
    for (i = 0U; i < ADI_ADRV904X_MAX_RX_CARRIERS; i++)
    {
        carrierGain = antCalCarrierCfg->rxCarrierGainForAntCal[i];

        if((carrierGain < DIG_GAIN_MIN) || (carrierGain > DIG_GAIN_MAX))
        {
            /* Invalid carrier gain value */
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, carrierGain, "Invalid gain mdB value. Valid value are from -90000mdB to 36000mdB");
            goto cleanup;
        }

        /* Convert from mdB to 7.16. (reg value = 10**(value in mdB/1000/20)) * 2^16) */
        //carrierGainReg = (uint32_t)((double)pow(10, (double)carrierGain / 1000U / 20U) * DIG_GAIN_MULT);
        carrierGainReg = (uint32_t)(12);
        carrierGainReg &= 0x003FFFFF; /* Mask 23 bits of gain */
        tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_ANTENNA_CAL_RX_CARRIER_0_GAIN + i);
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, tempAddress, carrierGainReg, 0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while write Rx carrier gain byte.");
            goto cleanup;
        }
    }

    for (i = 0U; i < ADI_ADRV904X_MAX_TX_CARRIERS; i++)
    {
        carrierGain = antCalCarrierCfg->txCarrierGainForAntCal[i];

        if ((carrierGain < DIG_GAIN_MIN) || (carrierGain > DIG_GAIN_MAX))
        {
            /* Invalid carrier gain value */
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, carrierGain, "Invalid gain mdB value. Valid value are from -90000mdB to 36000mdB");
            goto cleanup;
        }

        /* Convert from mdB to 7.16. (reg value = 10**(value in mdB/1000/20)) * 2^16) */
        //carrierGainReg = (uint32_t)((double)pow(10, (double)carrierGain / 1000U / 20U) * DIG_GAIN_MULT);
        carrierGainReg = (uint32_t)(12);
        carrierGainReg &= 0x003FFFFF; /* Mask 23 bits of gain */
        
        tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_ANTENNA_CAL_TX_CARRIER_0_GAIN + i);
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, tempAddress, carrierGainReg, 0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while write Tx carrier gain byte.");
            goto cleanup;
        }
    }
    
    /* Restore ECC scrubbing before we exit */
    recoveryAction =  adi_adrv904x_EccEnableSet(device, origEccState);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disabling ECC scrubbing on M4");
        return recoveryAction;
    }
cleanup:
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_LoUnlockGpInterruptMaskGet(adi_adrv904x_Device_t* const    device,
                                                                     const adi_adrv904x_LoName_e      loName,
                                                                     uint8_t* const                   loGpInterruptPin0Mask,
                                                                     uint8_t* const                   loGpInterruptPin1Mask)
{
    static const uint8_t  LO0_GP_INTERRUPT_SHIFT = 32U;
    uint64_t LO0_GP_INTERRUPT_UINT64_MASK  =  (uint64_t)1U <<  LO0_GP_INTERRUPT_SHIFT; /* east */
    static const uint8_t  LO1_GP_INTERRUPT_SHIFT = 31U;
    uint64_t LO1_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO1_GP_INTERRUPT_SHIFT; /* west */
        adi_adrv904x_ErrAction_e  recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint64_t gpInterruptPin0Mask        = 0U;
    uint64_t gpInterruptPin1Mask        = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, loGpInterruptPin0Mask);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, loGpInterruptPin1Mask);

    /* Read GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin0_BfGet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin0Mask); 
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin1_BfGet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    switch (loName)
    {
        case(ADI_ADRV904X_LO0):
            {
                *loGpInterruptPin0Mask = (uint8_t)ADRV904X_BF_DECODE(gpInterruptPin0Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                *loGpInterruptPin1Mask = (uint8_t)ADRV904X_BF_DECODE(gpInterruptPin1Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);

                break;
            }
        case(ADI_ADRV904X_LO1):
            {
                *loGpInterruptPin0Mask = (uint8_t)ADRV904X_BF_DECODE(gpInterruptPin0Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                *loGpInterruptPin1Mask = (uint8_t)ADRV904X_BF_DECODE(gpInterruptPin1Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                break;
            }
                default:
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid LO selection while attempting to retrieve LO GP Interrupt mask.");
                return recoveryAction;
            }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_LoUnlockGpInterruptMaskSet(adi_adrv904x_Device_t* const    device,
                                                                     const adi_adrv904x_LoName_e      loName,
                                                                     const uint8_t                    loGpInterruptPin0Mask,
                                                                     const uint8_t                    loGpInterruptPin1Mask)
{
    static const uint8_t  LO0_GP_INTERRUPT_SHIFT = 32U;
    uint64_t LO0_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO0_GP_INTERRUPT_SHIFT; /* east */
    static const uint8_t  LO1_GP_INTERRUPT_SHIFT = 31U;
    uint64_t LO1_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO1_GP_INTERRUPT_SHIFT; /* west */
        adi_adrv904x_ErrAction_e  recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint64_t gpInterruptPin0Mask        = 0U;
    uint64_t gpInterruptPin1Mask        = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Read GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin0_BfGet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin0Mask);
        
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin1_BfGet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    switch (loName)
    {
        case(ADI_ADRV904X_LO0):
            {
                /* Update GP Interrupt Mask */
                ADRV904X_BF_UPDATE(gpInterruptPin0Mask, (uint64_t)loGpInterruptPin0Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                ADRV904X_BF_UPDATE(gpInterruptPin1Mask, (uint64_t)loGpInterruptPin1Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                break;
            }
        case(ADI_ADRV904X_LO1):
            {
                /* Update GP Interrupt Mask */
                ADRV904X_BF_UPDATE(gpInterruptPin0Mask, (uint64_t)loGpInterruptPin0Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                ADRV904X_BF_UPDATE(gpInterruptPin1Mask, (uint64_t)loGpInterruptPin1Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                break;
            }
                    default:
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid LO selection while attempting to set LO GP Interrupt mask");
                return recoveryAction;
            }
    }

    /* Write GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin0_BfSet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        gpInterruptPin0Mask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Set GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin1_BfSet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Set GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_LoOverrangeGpInterruptMaskGet(adi_adrv904x_Device_t* const    device,
                                                                        const adi_adrv904x_LoName_e     loName,
                                                                        uint8_t* const                  loGpInterruptPin0Mask,
                                                                        uint8_t* const                  loGpInterruptPin1Mask)
{
    static const uint8_t  LO0_GP_INTERRUPT_SHIFT = 29U;
    uint64_t LO0_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO0_GP_INTERRUPT_SHIFT; /* east */
    static const uint8_t  LO1_GP_INTERRUPT_SHIFT = 28U;
    uint64_t LO1_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO1_GP_INTERRUPT_SHIFT; /* west */
        adi_adrv904x_ErrAction_e  recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint64_t gpInterruptPin0Mask        = 0U;
    uint64_t gpInterruptPin1Mask        = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, loGpInterruptPin0Mask);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, loGpInterruptPin1Mask);

    /* Read GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin0_BfGet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin0Mask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin1_BfGet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    switch (loName)
    {
        case(ADI_ADRV904X_LO0):
            {
                *loGpInterruptPin0Mask = (uint8_t)ADRV904X_BF_DECODE(gpInterruptPin0Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                *loGpInterruptPin1Mask = (uint8_t)ADRV904X_BF_DECODE(gpInterruptPin1Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);

                break;
            }
        case(ADI_ADRV904X_LO1):
            {
                *loGpInterruptPin0Mask = (uint8_t)ADRV904X_BF_DECODE(gpInterruptPin0Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                *loGpInterruptPin1Mask = (uint8_t)ADRV904X_BF_DECODE(gpInterruptPin1Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                break;
            }
                    default:
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid LO selection while attempting to retrieve LO GP Interrupt mask.");
                return recoveryAction;
            }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_LoOverrangeGpInterruptMaskSet(adi_adrv904x_Device_t* const    device,
                                                                        const adi_adrv904x_LoName_e     loName,
                                                                        const uint8_t                   loGpInterruptPin0Mask,
                                                                        const uint8_t                   loGpInterruptPin1Mask)
{
    static const uint8_t  LO0_GP_INTERRUPT_SHIFT = 29U;
    uint64_t LO0_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO0_GP_INTERRUPT_SHIFT; /* east */
    static const uint8_t  LO1_GP_INTERRUPT_SHIFT = 28U;
    uint64_t LO1_GP_INTERRUPT_UINT64_MASK  = (uint64_t)1U <<  LO1_GP_INTERRUPT_SHIFT; /* west */
        adi_adrv904x_ErrAction_e  recoveryAction =  ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint64_t gpInterruptPin0Mask        = 0U;
    uint64_t gpInterruptPin1Mask        = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Read GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin0_BfGet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin0Mask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin1_BfGet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        &gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    switch (loName)
    {
        case(ADI_ADRV904X_LO0):
            {
                /* Update GP Interrupt Mask */
                ADRV904X_BF_UPDATE(gpInterruptPin0Mask, (uint64_t)loGpInterruptPin0Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                ADRV904X_BF_UPDATE(gpInterruptPin1Mask, (uint64_t)loGpInterruptPin1Mask, LO0_GP_INTERRUPT_UINT64_MASK, LO0_GP_INTERRUPT_SHIFT);
                break;
            }
        case(ADI_ADRV904X_LO1):
            {
                /* Update GP Interrupt Mask */
                ADRV904X_BF_UPDATE(gpInterruptPin0Mask, (uint64_t)loGpInterruptPin0Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                ADRV904X_BF_UPDATE(gpInterruptPin1Mask, (uint64_t)loGpInterruptPin1Mask, LO1_GP_INTERRUPT_UINT64_MASK, LO1_GP_INTERRUPT_SHIFT);
                break;
            }
                    default:
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid LO selection while attempting to set LO GP Interrupt mask");
                return recoveryAction;
            }
    }

    /* Write GP Interrupt Pin0 and Pin1 mask */
    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin0_BfSet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        gpInterruptPin0Mask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Set GPIO Interrupt Pin 0 failed.");
        return recoveryAction;
    }

    recoveryAction =  adrv904x_Core_GpInterruptsMaskUpperWordPin1_BfSet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        gpInterruptPin1Mask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Set GPIO Interrupt Pin 1 failed.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_LoFrequencySetRangeCheck(adi_adrv904x_Device_t* const device,
                                                                   const adi_adrv904x_LoConfig_t* const loConfig)
{
    adi_adrv904x_ErrAction_e    recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                   areCalsRunning     = 0U;
    adi_adrv904x_LoSel_e      loSelect           = ADI_ADRV904X_LOSEL_LO1;
    adi_adrv904x_LoSel_e      loSelectGet        = ADI_ADRV904X_LOSEL_LO1;
    uint64_t                  maxRfBandwidth_kHz = 0U;
    uint8_t                   i                  = 0U;
    adi_adrv904x_TxChannels_e txChannel          = ADI_ADRV904X_TXOFF;
    adi_adrv904x_RxChannels_e rxChannel          = ADI_ADRV904X_RXOFF;
    int64_t                   freqValid          = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, loConfig);

        /*Check that LO Name selected is valid */
    if ((loConfig->loName != ADI_ADRV904X_LO0) &&
        (loConfig->loName != ADI_ADRV904X_LO1)
                )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loConfig->loName, "Invalid LO selected for setting LO frequency");
        return recoveryAction;
    }

    /*Check that PLL mode is valid*/
    if ((loConfig->loConfigSel != ADI_ADRV904X_NCO_NO_OPTION_SELECTED) &&
        (loConfig->loConfigSel != ADI_ADRV904X_NCO_AUTO_UPDATE_DISABLE))
    {

        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loConfig->loName, "Invalid LO Config selected for setting LO frequency");
        return recoveryAction;
    }

    /* Determine the PLL Frequency LO Selection */
    if (loConfig->loName == ADI_ADRV904X_LO0)
    {
        loSelect = ADI_ADRV904X_LOSEL_LO0;
    }
    else if (loConfig->loName == ADI_ADRV904X_LO1)
    {
        loSelect = ADI_ADRV904X_LOSEL_LO1;
    }
        /* Find the greatest TX Input Rates value of the same LO */
    maxRfBandwidth_kHz = 0U;
    if ((device->devStateInfo.profilesValid & ADI_ADRV904X_TX_PROFILE_VALID) == ADI_ADRV904X_TX_PROFILE_VALID)
    {
        for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
        {
            /* convert the index to matching the channel */
            txChannel = (adi_adrv904x_TxChannels_e)((uint8_t)(1U << i));
            if (((device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == (uint32_t)txChannel)
            {
                recoveryAction = adrv904x_TxLoSourceGet(device, txChannel, &loSelectGet);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxLoSourceGet Issue");
                    return recoveryAction;
                }
                if (loSelectGet == loSelect)
                {
                    /* Now check if matching LO then compare to get max value */
                    if (device->initExtract.tx.txChannelCfg[i].rfBandwidth_kHz > maxRfBandwidth_kHz)
                    {
                        maxRfBandwidth_kHz = device->initExtract.tx.txChannelCfg[i].rfBandwidth_kHz;
                    }
                }
            }
        }
    }

    /* Find the greatest RX Output Rates value of the same LO */
    if ((device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID) == ADI_ADRV904X_RX_PROFILE_VALID)
    {
        for (i = 0U; i < ADI_ADRV904X_MAX_RX_ONLY; i++)
        {
            /* convert the index to matching the channel */
            rxChannel = (adi_adrv904x_RxChannels_e)((uint8_t)(1U << i));
            if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == (uint32_t)rxChannel)
            {
                recoveryAction = adrv904x_RxLoSourceGet(device, rxChannel, &loSelectGet);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV904X_ERRSRC_RADIOCTRL,
            ADI_ADRV904X_ERRCODE_RADIOCTRL_LO_CFG,
            recoveryAction,
            loConfig->loFrequency_Hz,
            "(loFreq - Bandwidth/2) should be greater than DC.");
        return recoveryAction;
    }

    /*Check that init cals are not running*/
    recoveryAction = (adi_adrv904x_ErrAction_e)adi_adrv904x_InitCalsCheckCompleteGet(device, &areCalsRunning, ADI_FALSE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "InitCalsCheckCompleteGet Issue");
        return recoveryAction;
    }

    if (areCalsRunning == 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, areCalsRunning, "Can not set LO frequency while InitCals are running");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_LoLoopFilterSetRangeCheck(adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_LoName_e loName,
                                                                    const adi_adrv904x_LoLoopFilterCfg_t* const loLoopFilterConfig)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    static const uint16_t MIN_LOOPBANDWIDTH_KHZ = 60;
    static const uint16_t MAX_LOOPBANDWIDTH_KHZ = 1000;
    static const uint8_t MIN_PHASEMARGIN_DEGREE = 45;
    static const uint8_t MAX_PHASEMARGIN_DEGREE = 75;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, loLoopFilterConfig);

    if ((loName != ADI_ADRV904X_LO0) && (loName != ADI_ADRV904X_LO1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid LO selected for setting LO Loop filter");
        return recoveryAction;
    }

    if ((loLoopFilterConfig->loopBandwidth_kHz < MIN_LOOPBANDWIDTH_KHZ) || (loLoopFilterConfig->loopBandwidth_kHz > MAX_LOOPBANDWIDTH_KHZ))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid loop bandwidth for setting LO Loop filter");
        return recoveryAction;
    }

    if ((loLoopFilterConfig->phaseMargin_degrees < MIN_PHASEMARGIN_DEGREE) || (loLoopFilterConfig->phaseMargin_degrees > MAX_PHASEMARGIN_DEGREE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loName, "Invalid phase margin for setting LO Loop filter");
        return recoveryAction;
    }

    return ADI_ADRV904X_ERR_ACT_NONE;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_StreamTrigger(adi_adrv904x_Device_t* const    device,
                                                        const uint8_t                   streamId)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t cpuCommandBusy = 0U;
    uint32_t timeout_us = ADI_ADRV904X_SENDCPUCMD_TIMEOUT_US;
    uint32_t waitInterval_us = ADI_ADRV904X_SENDCPUCMD_INTERVAL_US;
    uint32_t eventCheck = 0U;
    uint32_t numEventChecks = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* setting a 2 second timeout for mailbox busy bit to be clear (can't send an cpu mailbox command until mailbox is ready) */
#if ADI_ADRV904X_SENDCPUCMD_INTERVAL_US > ADI_ADRV904X_SENDCPUCMD_TIMEOUT_US
    waitInterval_us = timeout_us;
#elif ADI_ADRV904X_SENDCPUCMD_INTERVAL_US == 0
    waitInterval_us = timeout_us;
#endif
    numEventChecks = timeout_us / waitInterval_us;

    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0U; eventCheck <= numEventChecks; ++eventCheck)
    {
        recoveryAction = adrv904x_Core_Arm0Spi0CommandBusy_BfGet(device,
                                                                 NULL,
                                                                 (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                 &cpuCommandBusy);
        if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Read (for CPU 0 Command Busy Bit) Issue");
            return recoveryAction;
        }

        if (cpuCommandBusy == ADI_TRUE)
        {
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
            if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
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
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_CPU,
                            ADI_ADRV904X_ERRCODE_CPU_CMD_TIMEOUT,
                            recoveryAction,
                            cpuCommandBusy,
                            "CPU Mailbox Busy; Timeout Occurred");
        return recoveryAction;
    }
    else
    {
        /*Write streamId to payload in arm mailbox*/
        recoveryAction = adrv904x_Core_Arm0Spi0ExtCmdByte1_BfSet(device,
                                                                 NULL,
                                                                 (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                 streamId);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Core Stream Processor stream ID number to trigger.");
            return recoveryAction;
        }

        /*Write opcode to trigger Core Stream Processor rather than send command to ARM*/
        recoveryAction = adrv904x_Core_Arm0Spi0Command_BfSet(device,
                                                             NULL,
                                                             (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                             ADRV904X_CPU_STREAM_TRIGGER_OPCODE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while triggering Core Stream Processor directly from API.");
            return recoveryAction;
        }

    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxStreamTrigger(adi_adrv904x_Device_t* const           device,
                                                          const uint8_t                          txChanMask,
                                                          const uint8_t                          txStreamId)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint32_t NUM_OF_EVENT_CHECKS = 1000U;
    const uint32_t UNUSED_STREAM_ID = 0xFFU; /* The value of unused TX stream ID need to match the value used by core stream */
    uint32_t eventCheck = 0U;
    uint8_t rdStreamId = 0;
    uint8_t byteWriteBuf[2] = { 0U, 0U };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0U; eventCheck <= NUM_OF_EVENT_CHECKS; ++eventCheck)
    {
        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        ADRV904X_TRIGGER_SLICE_STREAM_NUM,
                                                        &rdStreamId);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading stream ID from scratchpad register.");
            return recoveryAction;
        }

        if (rdStreamId != UNUSED_STREAM_ID)
        {
            recoveryAction = (adi_adrv904x_ErrAction_e)adi_common_hal_Wait_us(&device->common, 1);
            if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
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
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Core SP Busy");
        return recoveryAction;
    }
    else
    {
        /* Set requested TX stream ID to be run */
        byteWriteBuf[0] = (uint8_t)txStreamId;

        /* Update Tx channel bitmask */
        byteWriteBuf[1] = txChanMask;

        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_CORE_STREAM_SCRATCH204, byteWriteBuf, 2U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write to Core scratch pad registers");
            return recoveryAction;
        }

        recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_MAIN_TRIGGER_TX_STREAM);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while triggering Core Stream to trigger TX stream");
            return recoveryAction;
        }
    }
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxStreamTrigger(adi_adrv904x_Device_t* const           device,
                                                          const uint8_t                          rxChanMask,
                                                          const uint8_t                          rxStreamId)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint32_t NUM_OF_EVENT_CHECKS = 1000U;
    const uint32_t UNUSED_STREAM_ID = 0xFFU; /* The value of unused RX stream ID need to match the value used by core stream */
    uint32_t eventCheck = 0U;
    uint8_t rdStreamId = 0;
    uint8_t byteWriteBuf[2] = { 0U, 0U };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0U; eventCheck <= NUM_OF_EVENT_CHECKS; ++eventCheck)
    {
        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        ADRV904X_TRIGGER_SLICE_STREAM_NUM,
                                                        &rdStreamId);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading stream ID from scratchpad register.");
            return recoveryAction;
        }

        if (rdStreamId != UNUSED_STREAM_ID)
        {
            recoveryAction = (adi_adrv904x_ErrAction_e)adi_common_hal_Wait_us(&device->common, 1);
            if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
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
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Core SP Busy");
        return recoveryAction;
    }
    else
    {
        /* Set requested RX stream ID to be run */
        byteWriteBuf[0] = (uint8_t)rxStreamId;

        /* Update Tx channel bitmask */
        byteWriteBuf[1] = rxChanMask;

        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_CORE_STREAM_SCRATCH204, byteWriteBuf, 2U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write to Core scratch pad registers");
            return recoveryAction;
        }

        recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_MAIN_TRIGGER_RX_STREAM);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while triggering Core Stream to trigger RX stream");
            return recoveryAction;
        }
    }
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_StreamGpioConfigSetRangeCheck(adi_adrv904x_Device_t* const          device,
                                                                        const adi_adrv904x_StreamGpioPinCfg_t* const streamGpioPinCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    int idx = 0;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, streamGpioPinCfg);


    /* Check all Stream GPIO if valid */
    for (idx = 0U; idx < ADI_ADRV904X_MAX_STREAMGPIO; idx++)
    {
        /* Check if pin is valid and match index */
        if ((streamGpioPinCfg->streamGpInput[idx] != ADI_ADRV904X_GPIO_INVALID) &&
            (streamGpioPinCfg->streamGpInput[idx] != (adi_adrv904x_GpioPinSel_e)idx))
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, idx, "Invalid stream trigger GPIO pin selected. Index must match GPIO value. To disable, please select GPIO_INVALID");
            return recoveryAction;
        }
    }


    /* Check digital output GPIOs */
    for (idx = 0U; idx < ADI_ADRV904X_DFE_CTRL_MAX_GPIO_DIG; idx++)
    {
        /* Check if pin is valid and match index */
        if ((streamGpioPinCfg->streamGpDigOutput[idx] != ADI_ADRV904X_GPIO_INVALID) &&
            (streamGpioPinCfg->streamGpDigOutput[idx] != (adi_adrv904x_GpioPinSel_e)idx))
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, idx, "Invalid stream trigger GPIO pin selected. Index must match GPIO value. To disable, please select GPIO_INVALID");
            return recoveryAction;
        }
    }

    for (idx = 0U; idx < ADI_ADRV904X_DFE_CTRL_MAX_GPIO_ANA; idx++)
    {
        /* Check if pin is valid and match index */
        if ((streamGpioPinCfg->streamGpAnaOutput[idx] != ADI_ADRV904X_GPIO_ANA_INVALID) &&
            (streamGpioPinCfg->streamGpAnaOutput[idx] != (adi_adrv904x_GpioAnaPinSel_e)idx))
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, idx, "Invalid stream trigger GPIO pin selected. Index must match GPIO value. To disable, please select ADI_ADRV904X_GPIO_ANA_INVALID");
            return recoveryAction;
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return (recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_AdcVersionGet(adi_adrv904x_Device_t* const  device,
                                                        const uint8_t                 calType,
                                                        const adi_adrv904x_Channels_e channel,
                                                        adi_adrv904x_Version_t* const adcVersion)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, adcVersion);

    uint32_t lengthResp = 4U;
    uint8_t ctrlData[4] = { 0U };
    uint8_t ctrlResp[16] = { 0U };

    if ((calType != ADRV904X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if ((channel != ADI_ADRV904X_CH0) &&
        (channel != ADI_ADRV904X_CH1) &&
        (channel != ADI_ADRV904X_CH2) &&
        (channel != ADI_ADRV904X_CH3) &&
        (channel != ADI_ADRV904X_CH4) &&
        (channel != ADI_ADRV904X_CH5) &&
        (channel != ADI_ADRV904X_CH6) &&
        (channel != ADI_ADRV904X_CH7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "channel parameter is invalid.");
        return recoveryAction;
    }

    /* Get the ADC module revision number: */
    recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_GET_SW_VERSION,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "Get ADC SW version failed");
        return recoveryAction;
    }

    adcVersion->majorVer       = adrv904x_CpuIntFromBytesGet(&ctrlResp[ 0], 4U);
    adcVersion->minorVer       = adrv904x_CpuIntFromBytesGet(&ctrlResp[ 4], 4U);
    adcVersion->maintenanceVer = adrv904x_CpuIntFromBytesGet(&ctrlResp[ 8], 4U);
    adcVersion->buildVer       = adrv904x_CpuIntFromBytesGet(&ctrlResp[12], 4U);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_AdcGetDataInfo(adi_adrv904x_Device_t* const  device,
                                                         const uint8_t                 calType,
                                                         const adi_adrv904x_Channels_e channel,
                                                         adrv904x_CpuCmd_GetAdcSizeAddrs_t* const adcSizes,
                                                         adrv904x_CpuCmd_GetAdcSizeAddrs_t* const adcAddrs)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, adcSizes);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, adcAddrs);

    uint32_t lengthResp = 4U;
    uint8_t  ctrlData[4] = { 0U };
    uint8_t  ctrlResp[24] = { 0U };

    if ((calType != ADRV904X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if (calType == ADRV904X_CPU_OBJID_IC_ADC_ORX)
    {
        if ((channel != ADI_ADRV904X_CH0) &&
            (channel != ADI_ADRV904X_CH1))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad ORx channel");
            return recoveryAction;
        }
    }
    else
    {
        if ((channel != ADI_ADRV904X_CH0) &&
            (channel != ADI_ADRV904X_CH1) &&
            (channel != ADI_ADRV904X_CH2) &&
            (channel != ADI_ADRV904X_CH3) &&
            (channel != ADI_ADRV904X_CH4) &&
            (channel != ADI_ADRV904X_CH5) &&
            (channel != ADI_ADRV904X_CH6) &&
            (channel != ADI_ADRV904X_CH7))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad Rx/Tx channel");
            return recoveryAction;
        }
    }

    /* Get the ADC module data sizes: */
    recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_GET_DATA_SIZES,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "Get ADC data sizes");
        return recoveryAction;
    }

    /* Store the adcSizes */
    adcSizes->obj      = adrv904x_CpuIntFromBytesGet(&ctrlResp[ 0], 4U);
    adcSizes->init     = adrv904x_CpuIntFromBytesGet(&ctrlResp[ 4], 4U);
    adcSizes->config   = adrv904x_CpuIntFromBytesGet(&ctrlResp[ 8], 4U);
    adcSizes->calObj   = adrv904x_CpuIntFromBytesGet(&ctrlResp[12], 4U);
    adcSizes->calState = adrv904x_CpuIntFromBytesGet(&ctrlResp[16], 4U);
    adcSizes->calData  = adrv904x_CpuIntFromBytesGet(&ctrlResp[20], 4U);

    /* Get the ADC module data addresses: */
    recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_GET_DATA_ADDRS,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "Get ADC data addresses");
        return recoveryAction;
    }

    /* Store the ADC Addresses */
    adcAddrs->obj      = adrv904x_CpuIntFromBytesGet(&ctrlResp[ 0], 4U);
    adcAddrs->init     = adrv904x_CpuIntFromBytesGet(&ctrlResp[ 4], 4U);
    adcAddrs->config   = adrv904x_CpuIntFromBytesGet(&ctrlResp[ 8], 4U);
    adcAddrs->calObj   = adrv904x_CpuIntFromBytesGet(&ctrlResp[12], 4U);
    adcAddrs->calState = adrv904x_CpuIntFromBytesGet(&ctrlResp[16], 4U);
    adcAddrs->calData  = adrv904x_CpuIntFromBytesGet(&ctrlResp[20], 4U);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_AdcRunInit(adi_adrv904x_Device_t* const  device,
                                                     const uint8_t                 calType,
                                                     const adi_adrv904x_Channels_e channel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    uint32_t lengthResp = 4U;
    uint8_t ctrlData[4] = { 0U };
    uint8_t ctrlResp[16] = { 0U };

    if ((calType != ADRV904X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if (calType == ADRV904X_CPU_OBJID_IC_ADC_ORX)
    {
        if ((channel != ADI_ADRV904X_CH0) &&
            (channel != ADI_ADRV904X_CH1))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad ORx channel");
            return recoveryAction;
        }
    }
    else
    {
        if ((channel != ADI_ADRV904X_CH0) &&
            (channel != ADI_ADRV904X_CH1) &&
            (channel != ADI_ADRV904X_CH2) &&
            (channel != ADI_ADRV904X_CH3) &&
            (channel != ADI_ADRV904X_CH4) &&
            (channel != ADI_ADRV904X_CH5) &&
            (channel != ADI_ADRV904X_CH6) &&
            (channel != ADI_ADRV904X_CH7))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad Rx/Tx channel");
            return recoveryAction;
        }
    }

    recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_INIT,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcRunInit failed");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_AdcSetFsmCmd(adi_adrv904x_Device_t* const     device,
                                                       const uint8_t                    calType,
                                                       const adi_adrv904x_Channels_e    channel,
                                                       const uint8_t                    fsmCmd)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    uint32_t lengthResp = 4U;
    uint8_t ctrlData[4] = { 0U };
    uint8_t ctrlResp[16] = { 0U };

    if ((calType != ADRV904X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if (calType == ADRV904X_CPU_OBJID_IC_ADC_ORX)
    {
        if ((channel != ADI_ADRV904X_CH0) &&
            (channel != ADI_ADRV904X_CH1))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcSetFsmCmd failed: bad ORx channel");
            return recoveryAction;
        }
    }
    else
    {
        if ((channel != ADI_ADRV904X_CH0) &&
            (channel != ADI_ADRV904X_CH1) &&
            (channel != ADI_ADRV904X_CH2) &&
            (channel != ADI_ADRV904X_CH3) &&
            (channel != ADI_ADRV904X_CH4) &&
            (channel != ADI_ADRV904X_CH5) &&
            (channel != ADI_ADRV904X_CH6) &&
            (channel != ADI_ADRV904X_CH7))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcSetFsmCmd failed: bad Rx/Tx channel");
            return recoveryAction;
        }
    }

    ctrlData[0] = fsmCmd;

    recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_SET_FSM_CMD,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcSetFsmCmd failed");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_AdcGetFsmState(adi_adrv904x_Device_t* const  device,
                                                         const uint8_t                 calType,
                                                         const adi_adrv904x_Channels_e channel,
                                                         uint8_t* const                fsmState)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, fsmState);
    
    uint32_t lengthResp = 4U;
    uint8_t  ctrlData[4] = { 0U };
    uint8_t  ctrlResp[24] = { 0U };

    if ((calType != ADRV904X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if (calType == ADRV904X_CPU_OBJID_IC_ADC_ORX)
    {
        if ((channel != ADI_ADRV904X_CH0) &&
            (channel != ADI_ADRV904X_CH1))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad ORx channel");
            return recoveryAction;
        }
    }
    else
    {
        if ((channel != ADI_ADRV904X_CH0) &&
            (channel != ADI_ADRV904X_CH1) &&
            (channel != ADI_ADRV904X_CH2) &&
            (channel != ADI_ADRV904X_CH3) &&
            (channel != ADI_ADRV904X_CH4) &&
            (channel != ADI_ADRV904X_CH5) &&
            (channel != ADI_ADRV904X_CH6) &&
            (channel != ADI_ADRV904X_CH7))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "bad Rx/Tx channel");
            return recoveryAction;
        }
    }

    /* Get the ADC module data sizes: */
    recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_GET_FSM_STATE,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "Get FSM state");
        return recoveryAction;
    }

    *fsmState = ctrlResp[0];

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_AdcRunCmd(adi_adrv904x_Device_t* const     device,
                                                    const uint8_t                    calType,
                                                    const adi_adrv904x_Channels_e    channel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    uint32_t lengthResp = 4U;
    uint8_t ctrlData[4] = { 0U };
    uint8_t ctrlResp[16] = { 0U };

    if ((calType != ADRV904X_CPU_OBJID_IC_ADC_RX) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_TXLB) &&
        (calType != ADRV904X_CPU_OBJID_IC_ADC_ORX))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, calType, "Invalid cal type. - must be an ADC cal");
        return recoveryAction;
    }

    if (calType == ADRV904X_CPU_OBJID_IC_ADC_ORX)
    {
        if ((channel != ADI_ADRV904X_CH0) &&
            (channel != ADI_ADRV904X_CH1))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcRunCmd failed: bad ORx channel");
            return recoveryAction;
        }
    }
    else
    {
        if ((channel != ADI_ADRV904X_CH0) &&
            (channel != ADI_ADRV904X_CH1) &&
            (channel != ADI_ADRV904X_CH2) &&
            (channel != ADI_ADRV904X_CH3) &&
            (channel != ADI_ADRV904X_CH4) &&
            (channel != ADI_ADRV904X_CH5) &&
            (channel != ADI_ADRV904X_CH6) &&
            (channel != ADI_ADRV904X_CH7))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcRunCmd failed: bad Rx/Tx channel");
            return recoveryAction;
        }
    }

    ctrlData[0] = 0;

    recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                    (uint32_t)calType,
                                                    (uint16_t)ADC_CTRL_RUN_CMD,
                                                    channel,
                                                    ctrlData,
                                                    sizeof(ctrlData),
                                                    &lengthResp,
                                                    ctrlResp,
                                                    sizeof(ctrlResp));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channel, "AdcRunCmd failed");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SetTrackingPeriod(adi_adrv904x_Device_t* const  device,
                                                            uint8_t objId,
                                                            uint32_t rateMsec)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    uint16_t offset;
    uint32_t configDataLength = 4;
    uint8_t  configDataSet[4] = { 0 };

    if ((objId < ADRV904X_CPU_OBJID_TC_START) ||
        (objId > ADRV904X_CPU_OBJID_TC_END))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Invalid objId");
        return recoveryAction;
    }

    offset = (objId - ADRV904X_CPU_OBJID_TC_START) * sizeof(uint32_t);

    configDataSet[0] = (uint8_t)((rateMsec >>  0) & 0xFFu);
    configDataSet[1] = (uint8_t)((rateMsec >>  8) & 0xFFu);
    configDataSet[2] = (uint8_t)((rateMsec >> 16) & 0xFFu);
    configDataSet[3] = (uint8_t)((rateMsec >> 24) & 0xFFu);

    /* Get the ADC module data sizes: */
    recoveryAction = adi_adrv904x_CpuConfigSet(device,
                                               (uint32_t)ADRV904X_CPU_OBJID_CFG_TRACKING_CALS,
                                               offset,
                                               configDataSet,
                                               configDataLength);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Set tracking period");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GetTrackingPeriod(adi_adrv904x_Device_t* const  device,
                                                            uint8_t objId,
                                                            uint32_t* const rateMsec)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    uint16_t offset;
    uint32_t configDataLength = 4;
    uint8_t  configDataGet[4] = { 0 };

    if ((objId < ADRV904X_CPU_OBJID_TC_START) ||
        (objId > ADRV904X_CPU_OBJID_TC_END))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Invalid objId");
        return recoveryAction;
    }

    offset = (objId - ADRV904X_CPU_OBJID_TC_START) * sizeof(uint32_t);

    /* Get the ADC module data sizes: */
    recoveryAction = adi_adrv904x_CpuConfigGet(device,
                                               (uint32_t)ADRV904X_CPU_OBJID_CFG_TRACKING_CALS,
                                               offset,
                                               configDataGet,
                                               configDataLength);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, objId, "Set tracking period");
        return recoveryAction;
    }

    *rateMsec = adrv904x_CpuIntFromBytesGet(&configDataGet[0], 4U);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioSequencerPatternSet(adi_adrv904x_Device_t* const device, uint32_t sequencerId, uint32_t patternId)
{
         uint32_t patternAddr = 0u;
    uint8_t enableFlag = 1u;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;

    /* Check that Radio Sequencer Enable */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }

    recoveryAction = adrv904x_RadioSequencerPatternAddrGet(device, patternId, &patternAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get Radio patternAddr addresses. \n");
        goto cleanup;
    }

    if ((patternAddr & 0x0F) != 0)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "patternAddr addresses should be aligned to 16-byte boundary.\n");
        goto cleanup;
    }

    recoveryAction = adrv904x_Sequencer_ExternalJumpPointer_BfSet(device, NULL, sequencerBaseAddr, (uint8_t)sequencerId, (uint16_t)(patternAddr>>4));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Radio Sequencer patterns. \n");
        goto cleanup;
    }

    recoveryAction = adrv904x_Sequencer_ExternalJumpEnable_BfSet(device, NULL, sequencerBaseAddr, (uint8_t)sequencerId, enableFlag);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Radio Sequencer ExternalJumpEnable. \n");
        goto cleanup;
    }

cleanup:
    return recoveryAction;

}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioSequencerApply(adi_adrv904x_Device_t* const device)
{
        adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check that Radio Sequencer Enable */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }
    /* this bit needs to see a 0->1 transition to latch the register changes*/
    /* Clear the apply_on_multi_boundary */
    recoveryAction = adrv904x_Sequencer_ApplyOnMultiframeBoundary_BfSet(device,
        NULL,
        sequencerBaseAddr,
        0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing Radio Sequencer apply_on_multi_boundary. \n");
        goto cleanup;
    }

    /* Set the apply_on_multi_boundary */
    recoveryAction = adrv904x_Sequencer_ApplyOnMultiframeBoundary_BfSet(device,
        NULL,
        sequencerBaseAddr,
        1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Radio Sequencer apply_on_multi_boundary. \n");
        goto cleanup;
    }
cleanup:
    return recoveryAction;

}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioSequencerApplyImmediately(adi_adrv904x_Device_t* const device)
{
        adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check that Radio Sequencer Enable */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }

    /* this bit needs to see a 0->1 transition to latch the register changes*/
    /* Clear the apply_now */
    recoveryAction = adrv904x_Sequencer_ApplyNow_BfSet(device,
        NULL,
        sequencerBaseAddr,
        0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing Radio Sequencer apply_now. \n");
        goto cleanup;
    }

    /* Set the apply_now */
    recoveryAction = adrv904x_Sequencer_ApplyNow_BfSet(device,
        NULL,
        sequencerBaseAddr,
        1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Radio Sequencer apply_now. \n");
        goto cleanup;
    }
cleanup:
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioSequencerPhaseSetRangeCheck(adi_adrv904x_Device_t* const device, uint32_t phaseAdjustment)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check phaseAdjustment is valid */
    if ((phaseAdjustment & (~(uint32_t)(0x1FFFFFFU))) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               NULL,
                               "Invalid phaseAdjustment value encountered while attempting to write multiframe phaseAdjustment.");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;

}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioSequencerPatternAddrGet(adi_adrv904x_Device_t* const device,
                                                                       const uint32_t               patternId,
                                                                       uint32_t* const              patternAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check patternAddr address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, patternAddr);

    if (device->devStateInfo.rsImageHeader.numberPatterns == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "The lookup table has not been populated. \n");
        return recoveryAction;
    }

    if ((patternId >= device->devStateInfo.rsImageHeader.numberPatterns) || (patternId >= ADI_ADRV904X_RADIO_SEQ_LOOKUP_TAB_MAX_SIZE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid input patternId. \n");
        return recoveryAction;
    }

    *patternAddr = device->devStateInfo.lookupTable[patternId];

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioSequencerDfePtrnLookupTableSet(adi_adrv904x_Device_t* const device)
{
    uint32_t i = 0U;
    uint32_t address = 0U;
    uint32_t temp = 0U;
    uint32_t rsInitStructAddr = ADRV904X_DFE_CPU_INIT_ADDR_GET(radioSeqInitData);
    uint32_t sequenceLookupTableAddr = rsInitStructAddr + ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcRadioCtrlSequencerInitData_t, sequenceLookupTable);
    uint32_t numPatternsAddr = rsInitStructAddr + ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcRadioCtrlSequencerInitData_t, numberPatterns);
    uint32_t sequenceLookupTableCrcAddr = rsInitStructAddr + ADI_LIBRARY_OFFSETOF(adrv904x_DfeSvcRadioCtrlSequencerInitData_t, sequenceLookupTableCrc);
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check that the radio sequencer is enabled */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        return recoveryAction;
    }
    
    /* Add number of RS patterns to DFE init region */
    temp = device->devStateInfo.rsImageHeader.numberPatterns;
    recoveryAction = adi_adrv904x_Registers32Write(device, NULL, &numPatternsAddr, &(temp), NULL, sizeof(temp) / 4U);                                 
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write radio sequencer numberPatterns to DFE init region");
        return recoveryAction;
    }

    /* Add pattern lookup table CRC to DFE init region */
    temp = device->devStateInfo.rsImageHeader.lookupCrc;
    recoveryAction = adi_adrv904x_Registers32Write(device, NULL, &sequenceLookupTableCrcAddr, &(temp), NULL, sizeof(temp) / 4U);                                 
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write radio sequencer sequenceLookupTableCrc to DFE init region");
        return recoveryAction;
    }

    /* Add pattern lookup table to the DFE init region */
    for (i = 0U; i < device->devStateInfo.rsImageHeader.numberPatterns; i++)
    {
        address = sequenceLookupTableAddr + (i * sizeof(device->devStateInfo.lookupTable[0U]));
        temp = device->devStateInfo.lookupTable[i];
        recoveryAction = adi_adrv904x_Registers32Write(device, NULL, &address, &temp, NULL, sizeof(temp) / 4U);                                 
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write radio sequencer sequenceLookupTableAddr to DFE init region");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioSequencerResetHoldSet(adi_adrv904x_Device_t* const device,
                                                                     const uint32_t               resetHold)
{
    const uint32_t RADIO_SEQUENCER_CONFIG_0_REG_OFFSET = 0x20U;
    const uint32_t RADIO_SEQUENCER_NUM_SEQUENCERS = 16U;
    const uint32_t RESET_HOLD_BITP = 28U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;
    uint8_t channelId = 0x0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    for (channelId = 0U; channelId < RADIO_SEQUENCER_NUM_SEQUENCERS; channelId++)
    {
        uint32_t regVal = 0U;
        recoveryAction = adi_adrv904x_Register32Read(device,
            NULL,
            ((uint32_t)sequencerBaseAddr + RADIO_SEQUENCER_CONFIG_0_REG_OFFSET + channelId * 4U),
            &regVal,
            0xFFFFFFFFU);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading RS reset_hold bitfield");
            goto cleanup;
        }

        regVal = (resetHold == 0U) ? regVal & (~(1U << RESET_HOLD_BITP)) : regVal | (1U << RESET_HOLD_BITP);

        recoveryAction = adi_adrv904x_Register32Write(device,
            NULL,
            ((uint32_t)sequencerBaseAddr + RADIO_SEQUENCER_CONFIG_0_REG_OFFSET + channelId * 4U),
            regVal,
            0xFFFFFFFFU);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS reset_hold bitfield");
            goto cleanup;
        }
    }
cleanup:
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioSequencerStart(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ErrAction_e tmpRecoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint32_t RADIO_SEQUENCER_NUM_SEQUENCERS = 16U;
    const uint32_t RADIO_SEQUENCER_CONFIG_0_REG_OFFSET = 0x20U;
    const uint32_t DBG_PROGRAM_COUNTER_BITP = 0U;
    const uint32_t DBG_PROGRAM_COUNTER_BITM = 0x0000FFFFU;
    const uint32_t RS_START_WAIT_INTERVAL_US = 1000U;
    const uint32_t RS_START_WAIT_CNT = 1000U;

    uint32_t pointerInit = 0x0U;
    uint8_t resetHold = 0x0U;
    adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;
    uint8_t channelId = 0x0U;
    uint32_t patternId = ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_STARTUP_PATTERN_GLOBAL_PTRN_ID;
    uint32_t curPc = 0U;
    uint8_t startComplete = 0U;
    uint32_t waitCounter = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check that Radio Sequencer Enable */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }

    /* Check that Radio Sequencer has been loaded before getting RS error info */
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_RSLOADED) != ADI_ADRV904X_STATE_RSLOADED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Radio Sequencer should have been loaded before starting");
        goto cleanup;
    }

    /* Check if RS was already initialized and started */
    if (device->devStateInfo.rsStarted == ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }
    else
    {
        if (device->initExtract.rs.ssbSyncMode == 0)
        {
            /* Clear the reset_hold for all the 16 radio sequencers */
            resetHold = 0x0U;
            recoveryAction = adrv904x_RadioSequencerResetHoldSet(device, resetHold);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to clear reset hold");
                goto cleanup;
            }

            /* Get the default pattern address */
            recoveryAction = adrv904x_RadioSequencerPatternAddrGet(device, patternId, &pointerInit);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get address of RS pattern");
                goto cleanup;
            }

            /* Set the pointerInit and clear ExternalJumpEnable for all the 16 radio sequencers */
            for (channelId = 0; channelId < RADIO_SEQUENCER_NUM_SEQUENCERS; channelId++)
            {
                recoveryAction = adrv904x_Sequencer_PointerInit_BfSet(device,
                                                                      NULL,
                                                                      sequencerBaseAddr, 
                                                                      channelId,
                                                                      (pointerInit >> 4U));
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS pointer_init bitfield");
                    goto cleanup;
                }

                recoveryAction = adrv904x_Sequencer_ExternalJumpEnable_BfSet(device, NULL, sequencerBaseAddr, (uint8_t)channelId, 0);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Radio Sequencer ExternalJumpEnable. \n");
                    goto cleanup;
                }
            }
        }
        else
        {
            /* 1.1 Clear the apply_on_multi_boundary */
            recoveryAction = adrv904x_Sequencer_ApplyOnMultiframeBoundary_BfSet(device,
                                                                                NULL,
                                                                                sequencerBaseAddr,
                                                                                0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing Radio Sequencer apply_on_multi_boundary. \n");
                goto cleanup;
            }

            /* 1.2 Clear the apply_now */
            recoveryAction = adrv904x_Sequencer_ApplyNow_BfSet(device,
                                                               NULL,
                                                               sequencerBaseAddr,
                                                               0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing Radio Sequencer apply_now. \n");
                goto cleanup;
            }

            /* 2.1 Set the error_clear */
            recoveryAction = adrv904x_Sequencer_ErrorClear_BfSet(device,
                                                                 NULL,
                                                                 sequencerBaseAddr,
                                                                 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Radio Sequencer error_clear. \n");
                goto cleanup;
            }

            /* 2.2 Set the ssb_sync_phase_error_clear */
            recoveryAction = adrv904x_Sequencer_SsbSyncPhaseErrorClear_BfSet(device,
                                                                             NULL,
                                                                             sequencerBaseAddr,
                                                                             1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Radio Sequencer ssb_sync_phase_error_clear. \n");
                goto cleanup;
            }

            /* 3.1 Set the reset_hold for all the 16 radio sequencers */
            resetHold = 0x1U;
            recoveryAction = adrv904x_RadioSequencerResetHoldSet(device, resetHold);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set reset hold");
                goto cleanup;
            }

            /* 3.2 Get the default pattern address */
            recoveryAction = adrv904x_RadioSequencerPatternAddrGet(device, patternId, &pointerInit);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get address of RS pattern");
                goto cleanup;
            }

            /* 3.3 Set the pointerInit and clear ExternalJumpEnable for all the 16 radio sequencers */
            for (channelId = 0; channelId < RADIO_SEQUENCER_NUM_SEQUENCERS; channelId++)
            {
                recoveryAction = adrv904x_Sequencer_PointerInit_BfSet(device,
                                                                      NULL,
                                                                      sequencerBaseAddr,
                                                                      channelId,
                                                                      (pointerInit >> 4U));
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS pointer_init bitfield");
                    goto cleanup;
                }

                recoveryAction = adrv904x_Sequencer_ExternalJumpEnable_BfSet(device, NULL, sequencerBaseAddr, (uint8_t)channelId, 0);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Radio Sequencer ExternalJumpEnable. \n");
                    goto cleanup;
                }
            }

            /* 4. Clear global_reset */
            recoveryAction = adrv904x_Sequencer_GlobalReset_BfSet(device,
                                                                  NULL,
                                                                  sequencerBaseAddr,
                                                                  0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing RS Global Reset. \n");
                goto cleanup;
            }

            // 5.1 Enable SP events trigger by SSB_SYNC GPIO
            recoveryAction = adrv904x_GpioDestStreamTrigSet(device, device->initExtract.radioCtrlPreInit.radioSequencerSsbSyncGpioCtrl, ADI_DISABLE);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling stream trigger");
                goto cleanup;
            }

            // 5.2 Wait for new SSB_SYNC to re-align multiframe boundary
            recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                                    ADRV904X_LINK_ID_0,
                                                    ADRV904X_DFE_SVC_CMD_ID_SEQUENCER_WAIT_FOR_SSB_SYNC,
                                                    NULL,
                                                    0,
                                                    NULL,
                                                    0,
                                                    NULL);

            // 5.3 Always disable SP events trigger by SSB_SYNC GPIO, even if WAIT_FOR_SSB_SYNC cmd returned an error
            tmpRecoveryAction = adrv904x_GpioDestStreamTrigSet(device, device->initExtract.radioCtrlPreInit.radioSequencerSsbSyncGpioCtrl, ADI_ENABLE);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while waiting for SSB SYNC. \n");
                goto cleanup;
            }

            recoveryAction = tmpRecoveryAction;
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disabling stream trigger");
                goto cleanup;
            }

            /* 6.1 Clear the error_clear bitfield */
            recoveryAction = adrv904x_Sequencer_ErrorClear_BfSet(device,
                                                                 NULL,
                                                                 sequencerBaseAddr,
                                                                 0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing Radio Sequencer error_clear. \n");
                goto cleanup;
            }

            /* 6.2 Clear the ssb_sync_phase_error_clear bitfield */
            recoveryAction = adrv904x_Sequencer_SsbSyncPhaseErrorClear_BfSet(device,
                                                                             NULL,
                                                                             sequencerBaseAddr,
                                                                             0U);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing Radio Sequencer ssb_sync_phase_error_clear. \n");
                goto cleanup;
            }

            /* 7. Clear the reset_hold for all the 16 radio sequencers */
            resetHold = 0x0U;
            recoveryAction = adrv904x_RadioSequencerResetHoldSet(device, resetHold);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to clear reset hold");
                goto cleanup;
            }
        }

        /* Apply changes on multi-frame boundary */
        recoveryAction = adrv904x_RadioSequencerApply(device);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS Apply.\n");
            goto cleanup;
        }

        startComplete = 0U;
        for (waitCounter = 0U; waitCounter < RS_START_WAIT_CNT; waitCounter++)
        {
            /* Assume that all have started until proven otherwise below */
            startComplete = 1U;

            /* Set the RS status to started */
            device->devStateInfo.rsStarted = ADI_TRUE;

            /* Verify the the current PC == pointerInit */
            for (channelId = 0; channelId < RADIO_SEQUENCER_NUM_SEQUENCERS; channelId++)
            {
                recoveryAction = adi_adrv904x_Register32Read(device,
                                                             NULL,
                                                             ((uint32_t) sequencerBaseAddr + RADIO_SEQUENCER_CONFIG_0_REG_OFFSET + channelId * 4),
                                                             &curPc,
                                                             0xFFFFFFFFU);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading RS dbg_program_counter bitfield");
                    goto cleanup;
                }

                curPc = (curPc & DBG_PROGRAM_COUNTER_BITM) >> DBG_PROGRAM_COUNTER_BITP;
                if (curPc != (pointerInit & DBG_PROGRAM_COUNTER_BITM))
                {
                    /* One of the radio sequencers hasn't started executing the startup pattern yet.
                     * Continue checking for completion.
                     */
                    startComplete = 0U;
                }
            }

            if (startComplete == 0U)
            {
                /* Wait a bit before running the check again */
                recoveryAction = (adi_adrv904x_ErrAction_e)adi_common_hal_Wait_us(&device->common, RS_START_WAIT_INTERVAL_US);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while waiting for Radio Sequencer to start");
                    goto cleanup;
                }
            }
            else
            {
                /* All radio sequencers have started, exit the wait loop */
                break;
            }
        }

        /* Radio sequencer didn't start in the specified amount of time. */
        if (startComplete == 0U)
        {
            /* Set the RS status to not started */
            device->devStateInfo.rsStarted = ADI_FALSE;
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV904X_ERRSRC_API,
                             ADI_COMMON_ERRCODE_BOOT_TIMEOUT,
                             recoveryAction,
                             ADI_NO_VARIABLE,
                             "Radio Sequencer Start Timeout");
            goto cleanup;
        }
    }
    cleanup:
        if ((device->initExtract.rs.ssbSyncMode != 0) && (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE))
    {
        /* Set global_reset */
        adrv904x_Sequencer_GlobalReset_BfSet(device,
                                             NULL,
                                             sequencerBaseAddr,
                                             1U);
    }
        return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeCtrlTxToOrxMappingBaseSet(adi_adrv904x_Device_t* const           device,
                                                                       const adi_adrv904x_RxChannels_e        orxChannel,
                                                                       const adi_adrv904x_TxChannels_e        txChannel,
                                                                       const uint8_t                          isVswrState,
                                                                       const adi_adrv904x_DfeVswrDirection_e  vswrDirection)
{

    const uint8_t VSWR_FLAG = 0x8U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_TxToOrxMappingMode_e lclMode = device->devStateInfo.dfeCtrlTxToOrxMappingConfig.mode;
    uint8_t streamTxOff = 0;
    uint8_t streamTxCfg = 0;
    uint8_t streamTxAct = 0;
    uint8_t txIdx = 0;
    adi_adrv904x_DfeVswrDirection_e lclDir = ADI_ADRV904X_DFE_VSWR_FWD;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check that Mapping Mode DFE CTRL. Otherwise throw an error */
    if (lclMode != ADI_ADRV904X_TX_ORX_MAPPING_MODE_DFE_CTRL)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 lclMode,
                                 "Device Tx to Orx Mapping Mode is a RCI mode. This function can only be used for DFE Control mode.");
        goto cleanup;
    }
    
    /* Range check orxChannel enum */
    if (orxChannel == ADI_ADRV904X_ORX0)
    {
        streamTxOff = ADRV904X_STREAM_MAIN_ORX0_ALL_OFF_PIN_STATE_IN_RS_MODE;
        streamTxCfg = ADRV904X_STREAM_MAIN_CFG_TX_ORX0_MAPPING_IN_RS_MODE;
        streamTxAct = ADRV904X_STREAM_MAIN_ACT_TX_ORX0_MAPPING_IN_RS_MODE;
    }
    else if (orxChannel == ADI_ADRV904X_ORX1)
    {
        streamTxOff = ADRV904X_STREAM_MAIN_ORX1_ALL_OFF_PIN_STATE_IN_RS_MODE;
        streamTxCfg = ADRV904X_STREAM_MAIN_CFG_TX_ORX1_MAPPING_IN_RS_MODE;
        streamTxAct = ADRV904X_STREAM_MAIN_ACT_TX_ORX1_MAPPING_IN_RS_MODE;
    }
    else
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 orxChannel,
                                 "ORx Channel selected is invalid. Must select ORx0-1.") ;
        goto cleanup;
    }
    
    /* Range check txChannel enum */
    switch (txChannel)
    {
        case (ADI_ADRV904X_TX0):
            txIdx = 0;
            break;
        case (ADI_ADRV904X_TX1):
            txIdx = 1;
            break;
        case (ADI_ADRV904X_TX2):
            txIdx = 2;
            break;
        case (ADI_ADRV904X_TX3):
            txIdx = 3;
            break;
        case (ADI_ADRV904X_TX4):
            txIdx = 4;
            break;
        case (ADI_ADRV904X_TX5):
            txIdx = 5;
            break;
        case (ADI_ADRV904X_TX6):
            txIdx = 6;
            break;
        case (ADI_ADRV904X_TX7):
            txIdx = 7;
            break;
        case (ADI_ADRV904X_TXOFF):
            break;
            
        default:
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     txChannel,
                                     "Tx Channel selected is invalid. Must select Tx0-7 or TXOFF") ;
            goto cleanup;
    }
    
    /* If VSWR state, then check vswr direction provided set MSB (bit3) of txIdx mapping in shadow reg */
    if (isVswrState != 0U)
    {
        /* Range Check vswrDirection */
        if ((vswrDirection != ADI_ADRV904X_DFE_VSWR_REV) &&
            (vswrDirection != ADI_ADRV904X_DFE_VSWR_FWD))
        {
            ADI_PARAM_ERROR_REPORT( &device->common,
                             recoveryAction,
                             vswrDirection,
                             "VSWR direction selected is invalid. Must select FWD or REV") ;
            goto cleanup;
        }
        
        txIdx |= VSWR_FLAG;
        lclDir = vswrDirection;
    }
    
    /* If txChannel = TXOFF, trigger the approp stream to apply a preset TX OFF state to GPIO outputs */
    if (txChannel == ADI_ADRV904X_TXOFF)
    {
        recoveryAction = adrv904x_StreamTrigger(device, streamTxOff);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   "Error while triggering Core Stream to apply ALL OFF state to DFE Ctrl Mapping output GPIOs.");
            goto cleanup;
        }
    }
    /* Otherwise, select Tx channel mapping and trigger streams to store and apply new GPIO output states */
    else
    {
        if (orxChannel == ADI_ADRV904X_ORX0)
        {
            /* Set Orx0 mapping to DPD/VSWR txIdx */
            recoveryAction = adrv904x_Core_RadioControlInterfaceOrx0MapShadow_BfSet(device,
                                                                                    NULL,
                                                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                                    txIdx);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     "Error while setting new mapping for Orx0.");
                goto cleanup;
            }

            /* Set ORx0 VSWR dir */
            recoveryAction = adrv904x_Core_VswrFwdRev0SelShadow_BfSet(device,
                                                                      NULL,
                                                                      (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                      lclDir);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     "Error while setting new mapping vswr direction for Orx0.");
                goto cleanup;
            }
        }
        else
        {
            /* Set Orx1 mapping to DPD/VSWR txIdx */
            recoveryAction = adrv904x_Core_RadioControlInterfaceOrx1MapShadow_BfSet(device,
                                                                                    NULL,
                                                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                                    txIdx);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     "Error while setting new mapping for Orx1.");
                goto cleanup;
            }

            /* Set ORx1 VSWR dir */
            recoveryAction = adrv904x_Core_VswrFwdRev1SelShadow_BfSet(device,
                                                                      NULL,
                                                                      (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                      lclDir);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     "Error while setting new mapping vswr direction for Orx1.");
                goto cleanup;
            }
        }

        recoveryAction = adrv904x_StreamTrigger(device, streamTxCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   "Error while triggering Core Stream to store DFE Ctrl Mapping output GPIO states to apply next.");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_StreamTrigger(device, streamTxAct);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   "Error while triggering Core Stream to apply DFE Ctrl Mapping output GPIO states.");
            goto cleanup;
        }
    }

cleanup:
    return recoveryAction;
}



ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioCtrlDbgGpioOutputRangeCheck( adi_adrv904x_Device_t* const device,
                                                                           const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                                           const uint8_t signalMask,
                                                                           const uint8_t setOp)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint8_t MAX_TDD_DRV = 1U;
    
    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    switch (gpioSigType)
    {
        
        case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN:
        case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN:
        case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN:
        case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EXT_EN:
        case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_AUX_GRP0_OUT:
        case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_AUX_GRP1_OUT:
            if (setOp == ADI_TRUE)
            {
                if (signalMask == 0U)
                {
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                }
            }
            else 
            {
                /* if multiple bits are slected or no bit then it is error */
                if (((signalMask & (signalMask - 1)) != 0) || (signalMask == 0U))
                {
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                }
            }
            break; 
            
        case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_TDD_SW_EN:
        case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_DRV_PA_EN:
            if (signalMask != MAX_TDD_DRV)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            }
            break;
        
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    }
    
    /* Flag an error for any invalid parameter */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid combination of GPIO signal type and its mask value.");
        return recoveryAction;
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioCtrlDbgGpioOutputFind( adi_adrv904x_Device_t* const device,
                                                                      const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                                      const uint8_t signalOffset,
                                                                      uint8_t * gpioPin,
                                                                      uint8_t * isDigital)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    int32_t  gpioCfgIdx;
    uint8_t  fbSwIdx = 0U;
    adi_adrv904x_StreamGpioPinCfg_t streamGpioPinCfg;
    
    
    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, gpioPin);
    ADI_LIBRARY_MEMSET(&streamGpioPinCfg, 0, sizeof(streamGpioPinCfg));

    /* Assume it is GPIO analog.*/
    *isDigital = ADI_FALSE;
    
    switch (gpioSigType)
    {
    case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            if (device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[signalOffset] != ADI_ADRV904X_GPIO_INVALID)
            {
                *gpioPin = device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[signalOffset];
                *isDigital = ADI_TRUE;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }
            else if (device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[signalOffset] != ADI_ADRV904X_GPIO_ANA_INVALID)
            {
                *gpioPin = device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[signalOffset];
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }
        break;
        
    case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            if (device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[signalOffset] != ADI_ADRV904X_GPIO_INVALID)
            {
                *gpioPin = device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[signalOffset];
                *isDigital = ADI_TRUE;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }
            else if (device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[signalOffset] != ADI_ADRV904X_GPIO_ANA_INVALID)
            {
                *gpioPin = device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[signalOffset];
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }
        break;

    case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_AUX_GRP0_OUT:
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        if (device->devStateInfo.alarmGpioConfig.auxGrp0OutDigGpio[signalOffset] != ADI_ADRV904X_GPIO_INVALID)
        {
            *gpioPin = device->devStateInfo.alarmGpioConfig.auxGrp0OutDigGpio[signalOffset];
            *isDigital = ADI_TRUE;
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        }
        else if (device->devStateInfo.alarmGpioConfig.auxGrp0OutAnaGpio[signalOffset] != ADI_ADRV904X_GPIO_ANA_INVALID)
        {
            *gpioPin = device->devStateInfo.alarmGpioConfig.auxGrp0OutAnaGpio[signalOffset];
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        }
        break;

    case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_AUX_GRP1_OUT:
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        if (device->devStateInfo.alarmGpioConfig.auxGrp1OutDigGpio[signalOffset] != ADI_ADRV904X_GPIO_INVALID)
        {
            *gpioPin = device->devStateInfo.alarmGpioConfig.auxGrp1OutDigGpio[signalOffset];
            *isDigital = ADI_TRUE;
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        }
        else if (device->devStateInfo.alarmGpioConfig.auxGrp1OutAnaGpio[signalOffset] != ADI_ADRV904X_GPIO_ANA_INVALID)
        {
            *gpioPin = device->devStateInfo.alarmGpioConfig.auxGrp1OutAnaGpio[signalOffset];
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        }
        break;

    case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN:
    case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EXT_EN:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            if (signalOffset > 3U)
            {

                if (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EXT_EN)
                {
                    fbSwIdx = signalOffset; /* adjust index to access rows 4..7 */
                }
                else
                {
                    /* upper nibble */
                    fbSwIdx = signalOffset - 4U; /* adjust index to access rows 0..3 */
                }
                gpioCfgIdx = device->devStateInfo.dfeCtrlTxToOrxMappingConfig.gpioSelectOrx1[fbSwIdx];
            }
            else
            {
                if (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EXT_EN)
                {
                    fbSwIdx = signalOffset + 4U; /* adjust index to access rows 4..7 */
                }
                else
                {
                    fbSwIdx = signalOffset; /* adjust index to access rows 0..3 */
                }
                gpioCfgIdx = device->devStateInfo.dfeCtrlTxToOrxMappingConfig.gpioSelectOrx0[fbSwIdx];
            }
            

            if ((gpioCfgIdx >= ADI_ADRV904X_GPIO_GENERIC_ANA_START) && (gpioCfgIdx <= ADI_ADRV904X_GPIO_GENERIC_ANA_END))
            {
                gpioCfgIdx -= ADI_ADRV904X_GPIO_GENERIC_ANA_START;
                *gpioPin = gpioCfgIdx;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }

            if ((gpioCfgIdx >= ADI_ADRV904X_GPIO_GENERIC_DIG_START) && (gpioCfgIdx <= ADI_ADRV904X_GPIO_GENERIC_DIG_END))
            {
                *isDigital = ADI_TRUE;
                gpioCfgIdx -= ADI_ADRV904X_GPIO_GENERIC_DIG_START;
                *gpioPin = gpioCfgIdx;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }
        break;

    case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_TDD_SW_EN:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            if (device->devStateInfo.digGpioTddSw != ADI_ADRV904X_GPIO_INVALID)
            {
                *gpioPin = device->devStateInfo.digGpioTddSw;
                *isDigital = ADI_TRUE;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }
            else if (device->devStateInfo.anaGpioTddSw != ADI_ADRV904X_GPIO_ANA_INVALID)
            {
                *gpioPin = device->devStateInfo.anaGpioTddSw;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }
        break;
               
    case ADI_ADRV904X_OUT_GPIO_SIG_TYPE_DRV_PA_EN:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            if (device->devStateInfo.digGpioPredrive != ADI_ADRV904X_GPIO_INVALID)
            {
                *gpioPin = device->devStateInfo.digGpioPredrive;
                *isDigital = ADI_TRUE;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }
            else if (device->devStateInfo.anaGpioPredrive != ADI_ADRV904X_GPIO_ANA_INVALID)
            {
                *gpioPin = device->devStateInfo.anaGpioPredrive;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }
        break;
        
    default:
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid GPIO signal type.");
        return recoveryAction;
    }

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while finding GPIO index.");
        return recoveryAction;
    }  

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_DbgGpioOutputPinMode( adi_adrv904x_Device_t* const device,
                                                               const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                               const adi_adrv904x_OutGpioPinStates_e gpioPinstate,
                                                               uint8_t * gpioMode,
                                                               uint8_t isDigital)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    const uint8_t GPIO_DIGITAL_RS_OUTPUT_MODE     = 0x0D;
    const uint8_t GPIO_DIGITAL_MANUAL_OUTPUT_MODE = 0x04;
    const uint8_t GPIO_DIGITAL_MANUAL_INPUT_MODE  = 0x05;

    const uint8_t GPIO_ANALOG_RS_OUTPUT_MODE      = 0x08;
    const uint8_t GPIO_ANALOG_MANUAL_OUTPUT_MODE  = 0x03;
    const uint8_t GPIO_ANALOG_MANUAL_INPUT_MODE   = 0x04;
    
    switch (gpioPinstate)
    {
        case ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF:
        case ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON:
            *gpioMode = (isDigital == ADI_TRUE) ? GPIO_DIGITAL_MANUAL_OUTPUT_MODE : GPIO_ANALOG_MANUAL_OUTPUT_MODE;
            break;
        
        case ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z:
            *gpioMode = (isDigital == ADI_TRUE) ? GPIO_DIGITAL_MANUAL_INPUT_MODE : GPIO_ANALOG_MANUAL_INPUT_MODE;
            break;
        
        case ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP:
            if ((gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN) || (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EXT_EN))
            {
                *gpioMode = (isDigital == ADI_TRUE) ? GPIO_DIGITAL_MANUAL_OUTPUT_MODE : GPIO_ANALOG_MANUAL_OUTPUT_MODE; 
            }
            else
            {
                *gpioMode = (isDigital == ADI_TRUE) ? GPIO_DIGITAL_RS_OUTPUT_MODE : GPIO_ANALOG_RS_OUTPUT_MODE;
            }
            break;
        
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid GPIO pin state.");
            return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioCtrlDbgGpioOutputSet( adi_adrv904x_Device_t* const device,
                                                                     const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                                     const uint8_t signalMask,
                                                                     const adi_adrv904x_OutGpioPinStates_e gpioPinstate,
                                                                     uint8_t isDigital,
                                                                     uint8_t updateDbgFlag)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint8_t MAX_SIGNAL = 8U;

    uint8_t gpioMode = 0U;
    uint32_t regValue = 0U;
    uint8_t  regByte = 0U;
    uint8_t  signalOffset = 0U;
    uint8_t  gpioPin = 0U;
    uint8_t  bitSet = 0U;
    uint8_t outputUpdated = 0U;
    uint16_t channelId = 0U;

    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Loop through and process every signal selected in the signal mask */
    for (signalOffset = 0; signalOffset < MAX_SIGNAL; signalOffset++)
    {
        if ((signalMask & (1 << signalOffset)) == 0)
        {
            /* Signal not selected in signal mask */
            continue;
        }

        if (updateDbgFlag == 0)
        {
            /* The debug flag don't need to be updated */
        }
        else if (updateDbgFlag == 1)
        {
            /* The debug flag needs to be cleared */
            recoveryAction = adrv904x_RadioCtrlDbgGpioDebugFlagSet(device, gpioSigType, signalOffset, 0);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                                     recoveryAction,
                                     "Failed to clear debug flag.");
                return recoveryAction;
            }
        }
        else if (updateDbgFlag == 2)
        {
            /* The debug flag needs to be set */
            recoveryAction = adrv904x_RadioCtrlDbgGpioDebugFlagSet(device, gpioSigType, signalOffset, 1);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                                     recoveryAction,
                                     "Failed to set debug flag.");
                return recoveryAction;
            }
        }

        if (outputUpdated == 0)
        {
            /* Based on the input parameters, find the GPIO Index in the GPIO Pins Configuration */
            recoveryAction = adrv904x_RadioCtrlDbgGpioOutputFind(device, gpioSigType, signalOffset, &gpioPin, &isDigital);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Failed to find GPIO based on the input parameters.");
                return recoveryAction;
            }

            recoveryAction = adrv904x_DbgGpioOutputPinMode(device,
                                                           gpioSigType,
                                                           gpioPinstate,
                                                           &gpioMode,
                                                           isDigital);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting the GPIO pin mode.");
                return recoveryAction;
            }

            /* Set the gpioMode to the correct GPIO Source Control register nibble based on gpioPin */
            if (isDigital == ADI_TRUE)
            {
                recoveryAction = adrv904x_GpioPinmuxStg3Set(device, (adi_adrv904x_GpioPinSel_e)gpioPin, gpioMode);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting the GPIO Source Control bitfield.");
                    return recoveryAction;
                }
            }
            else
            {
                recoveryAction = adrv904x_GpioAnalogPinmuxSet(device, (adi_adrv904x_GpioAnaPinSel_e)gpioPin, gpioMode);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting the GPIO Analog Source Control bitfield.");
                    return recoveryAction;
                }
            }

            if ((gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN) || (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EXT_EN))
            {
                /* The bit position 0,1,2,3 is uses to configure pins assigned to TX-to-ORx0 mapping
                 * The bit position 4,5,6,7 is uses to configure pins assigned to TX-to-ORx1 mapping
                 */
                if (signalOffset < 4)
                {
                    channelId = ADRV904X_FORCED_FB_SW_STATE_BITMAP_ORX0;
                }
                else
                {
                    channelId = ADRV904X_FORCED_FB_SW_STATE_BITMAP_ORX1;
                }

                /* Read the scratch register */
                recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                                NULL,
                                                                ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                channelId,
                                                                &regByte);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common,
                                         recoveryAction,
                                         "Error while read scratch reg FW SW Bitmap.");
                    return recoveryAction;
                }

                if (gpioPinstate == ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP)
                {
                    if (channelId == ADRV904X_FORCED_FB_SW_STATE_BITMAP_ORX0)
                    {
                        if (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN)
                        {
                            bitSet = regByte & (1 << (signalOffset + 0));

                            /* Clear the bit if it is set */
                            regByte &= ~(1 << (signalOffset + 0));
                        }
                        else
                        {
                            bitSet = regByte & (1 << (signalOffset + 4));

                            /* Clear the bit if it is set */
                            regByte &= ~(1 << (signalOffset + 4));
                        }
                    }
                    else
                    {
                        if (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN)
                        {
                            bitSet = regByte & (1 << (signalOffset - 4));

                            /* Clear the bit if it is set */
                            regByte &= ~(1 << (signalOffset - 4));
                        }
                        else
                        {
                            bitSet = regByte & (1 << (signalOffset + 0));

                            /* Clear the bit if it is set */
                            regByte &= ~(1 << (signalOffset + 0));
                        }
                    }
                                             
                    if (bitSet != 0U)
                    {
                        /* Write to the scratch reg */
                        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                                        NULL,
                                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                        channelId,
                                                                        regByte);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing FB SW State bitfield.");
                        }

                        /* Trigger stream for both events */
                        recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_FB_SW_00);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common,
                                                 recoveryAction,
                                                "Error while triggering Core Stream to apply FB SW clear.");
                            return recoveryAction;
                        }

                        recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_FB_SW_01);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common,
                                                 recoveryAction,
                                                 "Error while triggering Core Stream to apply FB SW clear.");
                            return recoveryAction;
                        }
                    }
                }
                else
                {
                    if (channelId == ADRV904X_FORCED_FB_SW_STATE_BITMAP_ORX0)
                    {
                        if (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN)
                        {
                            regByte |= (1 << (signalOffset + 0));
                        }
                        else
                        {
                            regByte |= (1 << (signalOffset + 4));
                        }
                    }
                    else
                    {
                        if (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN)
                        {
                            regByte |= (1 << (signalOffset - 4));
                        }
                        else
                        {
                            regByte |= (1 << (signalOffset + 0));
                        }
                    }

                    /* Write to the scratch reg */
                    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                                    NULL,
                                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                    channelId,
                                                                    regByte);

                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common,
                            recoveryAction,
                            "Error while write scratch reg FW SW Bitmap.");
                        return recoveryAction;
                    }
                }
            }

            /*
             * - Set the corresponding GPIO pin
             * - Write the value to the GPIO From Master
             */
            if (gpioPinstate == ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON)
            {
                /* Update the corresponding GPIO Pin */
                regValue = (1U << gpioPin);

                /* Write back the modified value */
                if (isDigital == ADI_TRUE)
                {
                    recoveryAction = adrv904x_Core_GpioFromMasterSet_BfSet(device,
                        NULL,
                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                        regValue);
                }
                else
                {
                    recoveryAction = adrv904x_Core_GpioAnalogFromMasterSet_BfSet(device,
                        NULL,
                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                        (uint16_t)regValue);
                }

                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while Setting the GPIO From Master Set bitfield.");
                    return recoveryAction;
                }
            }
            else if (gpioPinstate == ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF)
            {
                /* Update the corresponding GPIO Pin */
                regValue = (1U << gpioPin);

                /* Write back the modified value */
                if (isDigital == ADI_TRUE)
                {
                    recoveryAction = adrv904x_Core_GpioFromMasterClear_BfSet(device,
                        NULL,
                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                        regValue);
                }
                else
                {
                    recoveryAction = adrv904x_Core_GpioAnalogFromMasterClear_BfSet(device,
                        NULL,
                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                        (uint16_t)regValue);
                }

                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting the GPIO From Master Clear bitfield.");
                    return recoveryAction;
                }
            }
        }
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioCtrlDbgGpioOutputGet( adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                                    const uint8_t signalOffset,
                                                                    adi_adrv904x_OutGpioPinStates_e * const gpioPinstate,
                                                                    uint8_t gpioPin,
                                                                    uint8_t isDigital)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t gpioMode = 0U;
    uint8_t regByte = 0U;
    uint32_t fromMaster = 0U;
    uint16_t channelId = 0U;
	uint32_t mask = 0U;

    const uint8_t GPIO_DIGITAL_MANUAL_OUTPUT_MODE = 0x04;
    const uint8_t GPIO_DIGITAL_MANUAL_INPUT_MODE  = 0x05;

    const uint8_t GPIO_ANALOG_MANUAL_OUTPUT_MODE  = 0x03;
    const uint8_t GPIO_ANALOG_MANUAL_INPUT_MODE   = 0x04;

    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, gpioPinstate);

    /* Set the gpioMode to the correct GPIO Source Control register nibble based on gpioPin */
    if (isDigital == ADI_TRUE)
    {
        recoveryAction = adrv904x_GpioPinmuxStg3Get(device, (adi_adrv904x_GpioPinSel_e) gpioPin, &gpioMode);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting the GPIO Source Control bitfield.");
            return recoveryAction;
        } 
    }
    else
    {
        recoveryAction = adrv904x_GpioAnalogPinmuxGet(device, (adi_adrv904x_GpioAnaPinSel_e) gpioPin, &gpioMode);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting the GPIO Analog Source Control bitfield.");
            return recoveryAction;
        }  
    }

    if (((isDigital == ADI_TRUE) && (gpioMode == GPIO_DIGITAL_MANUAL_OUTPUT_MODE)) ||
        ((isDigital != ADI_TRUE) && (gpioMode == GPIO_ANALOG_MANUAL_OUTPUT_MODE)))
    {
        if (isDigital == ADI_TRUE)
        {
            recoveryAction = adrv904x_Core_GpioFromMaster_BfGet(device,
                                                                NULL,
                                                                (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                &fromMaster);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, 
                                     recoveryAction, 
                                     "Error while reading back GPIO Manual Mode Input Word values.");
                return recoveryAction;
            }
        }
        else 
        {
            recoveryAction = adrv904x_Core_GpioAnalogFromMaster_BfGet(device,
                                                                      NULL,
                                                                      (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                      (uint16_t*)&fromMaster);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, 
                                     recoveryAction, 
                                     "Error while reading back GPIO Analog Manual Mode Input Word values.");
                return recoveryAction;
            }
        }
    }

    /* FB SW needs to be handled differently */
    if ((gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN) || (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EXT_EN))
    {
        /* For manual output case, need to read scratch register */
        if (((isDigital == ADI_TRUE) && (gpioMode == GPIO_DIGITAL_MANUAL_OUTPUT_MODE)) ||
            ((isDigital != ADI_TRUE) && (gpioMode == GPIO_ANALOG_MANUAL_OUTPUT_MODE)))
        {
            /* Read the scratch register */
            if (signalOffset < 4)
            {
                channelId = ADRV904X_FORCED_FB_SW_STATE_BITMAP_ORX0;
            }
            else
            {
                channelId = ADRV904X_FORCED_FB_SW_STATE_BITMAP_ORX1;
            }
            recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                             NULL,
                                                             ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                             channelId,
                                                             &regByte);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     "Error while read scratch reg FW SW Bitmap.");
                return recoveryAction;
            }
            
	        if ((signalOffset < 4)  && (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EXT_EN))
	        {
		        mask = (1 << (signalOffset + 4));
	        }
	        else if ((signalOffset >= 4)  && (gpioSigType == ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN))
		    {
			    mask = (1 << (signalOffset - 4));
		    }
	        else
	        {
		        mask = (1 << signalOffset);
	        }

            if ((regByte & mask) == 0U)
            {
                *gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;
            }
            else
            {
                if ((fromMaster & (1 << gpioPin)) == 0U)
                {
                    *gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF;
                }
                else
                {
                    *gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON;
                }
                
            }
        }
        else if (((isDigital == ADI_TRUE) && (gpioMode == GPIO_DIGITAL_MANUAL_INPUT_MODE)) ||
                 ((isDigital != ADI_TRUE) && (gpioMode == GPIO_ANALOG_MANUAL_INPUT_MODE)))
        {
            *gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z;
        }
        else 
        {
            *gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;
        }
    }
    else /* For all other signal types */
    {
        if (((isDigital == ADI_TRUE) && (gpioMode == GPIO_DIGITAL_MANUAL_OUTPUT_MODE)) ||
            ((isDigital != ADI_TRUE) && (gpioMode == GPIO_ANALOG_MANUAL_OUTPUT_MODE)))
        {
            /* Check if bit is set */
            if ((fromMaster & (1 << gpioPin)) != 0U)
            {
                *gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON;
            }
            else
            {
                *gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF;
            }
            
        }
        else if (((isDigital == ADI_TRUE) && (gpioMode == GPIO_DIGITAL_MANUAL_INPUT_MODE)) ||
                 ((isDigital != ADI_TRUE) && (gpioMode == GPIO_ANALOG_MANUAL_INPUT_MODE)))
        {
            *gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z;
        }
        else 
        {
            *gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;
        }
    }
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting the GPIO Pin state.");
        return recoveryAction;
    } 
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioCtrlDbgGpioIsSignalTypeAffectedByAlarms(adi_adrv904x_Device_t* const device,
                                                                                       const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                                                       uint8_t* isAffected)
{
    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, isAffected);

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    /* Set pin state to the value selected in the device profile */
    switch (gpioSigType)
    {
        case (ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN):        /* Fallthrough */
        case (ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN):         /* Fallthrough */
        case (ADI_ADRV904X_OUT_GPIO_SIG_TYPE_AUX_GRP0_OUT):    /* Fallthrough */
        case (ADI_ADRV904X_OUT_GPIO_SIG_TYPE_AUX_GRP1_OUT):   /* Fallthrough */
            *isAffected = 1;
            break;
        default:
            *isAffected = 0;
            break;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioCtrlDbgGpioGetPaEnAffectedByAlarms(adi_adrv904x_Device_t* const device,
                                                                                  uint8_t* paEnAffectedByAlarm)
{ 
    uint32_t i = 0U;
    uint8_t regByte = 0U; 
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, paEnAffectedByAlarm);

    *paEnAffectedByAlarm = 0;

    for (i = 0; i < ADI_ADRV904X_ALARM_MAX; i++)
    {
        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        (ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_0 + i),
                                                        &regByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while getting item from ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM table.");
            break;
        }

        *paEnAffectedByAlarm |= regByte;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioCtrlDbgGpioGetLnaEnAffectedByAlarms(adi_adrv904x_Device_t* const device,
    uint8_t* lnaEnAffectedByAlarm)
{
    uint32_t i = 0U;
    uint8_t regByte = 0U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, lnaEnAffectedByAlarm);

    *lnaEnAffectedByAlarm = 0;

    for (i = 0; i < ADI_ADRV904X_ALARM_MAX; i++)
    {
        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        (ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_0 + i),
                                                        &regByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while getting item from ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM table.");
            break;
        }

        *lnaEnAffectedByAlarm |= regByte;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RadioCtrlDbgGpioDebugFlagSet(adi_adrv904x_Device_t* const device,
                                                                       const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                                       const uint8_t signalOffset,
                                                                       uint8_t flag)
{
    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    //adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    uint8_t regByte = 0U;
    uint16_t channelId = ADRV904X_EA_LNA_EN_ALWAYS_OFF; // fix this


    switch (gpioSigType)
    {
        case (ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN):
            channelId = ADRV904X_EA_LNA_EN_OUTPUT_FROZEN;
            break;
        case (ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN):
            channelId = ADRV904X_EA_PA_EN_OUTPUT_FROZEN;
            break;

        case (ADI_ADRV904X_OUT_GPIO_SIG_TYPE_AUX_GRP0_OUT):
            channelId = ADRV904X_EA_AUX_GRP0_OUTPUT_FROZEN;
            break;
        case (ADI_ADRV904X_OUT_GPIO_SIG_TYPE_AUX_GRP1_OUT):
            channelId = ADRV904X_EA_AUX_GRP1_OUTPUT_FROZEN;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   gpioSigType,
                                   "Unexpected GPIO signal type.");
            return recoveryAction;
    }

    /* Get the current value of debug flags */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    channelId,
                                                    &regByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while getting debug flags.");
        return recoveryAction;
    }

    if (flag != 0)
    {
        regByte |= (1 << signalOffset);
    }
    else
    {

        regByte &= ~(1 << signalOffset);
    }

    /* Update debug flags */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    channelId,
                                                    regByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Error while getting debug flags.");
        return recoveryAction;
    }

    return recoveryAction;
}
