// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025.c
* \brief Contains Adrv9025 features related function implementation defined in
* adi_adrv9025.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_version.h"
#include "adi_adrv9025_tx.h"
#include "adi_adrv9025_rx.h"
#include "adi_adrv9025_cpu.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_rx.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_bf_core.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025.c"
#endif

int32_t adi_adrv9025_HwOpen(adi_adrv9025_Device_t*      device,
                            adi_adrv9025_SpiSettings_t* spiSettings)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* Check for Hal layer functions */
    /* TODO: move section to common layer (add new function) */
    if ((adi_hal_HwOpen == NULL) || (adi_hal_HwClose == NULL) ||
        (adi_hal_HwReset == NULL) || (adi_hal_SpiInit == NULL) ||
        (adrv9025_hal_SpiWrite == NULL) || (adrv9025_hal_SpiRead == NULL) ||
        (adi_hal_LogFileOpen == NULL) || (adi_hal_LogLevelSet == NULL) ||
        (adi_hal_LogLevelGet == NULL) || (adi_hal_LogWrite == NULL) ||
        (adi_hal_Wait_us == NULL) || (adi_hal_Wait_ms == NULL) ||
        (adi_hal_HwVerify == NULL))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         ADI_HAL_LIBRARY_NOT_AVAILABLE,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Hal library Function pointers not set properlly");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adi_common_LogLevelSet(&device->common,
                           ADI_ADRV9025_LOGGING);


    recoveryAction = adi_common_hal_HwOpen(&device->common);



    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        switch (recoveryAction)
        {
        case ADI_COMMON_HAL_SPI_FAIL:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             NULL,
                             "SPI error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        case ADI_COMMON_HAL_GPIO_FAIL:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_RESET_FEATURE,
                             NULL,
                             "GPIO error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        case ADI_COMMON_HAL_TIMER_FAIL:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_TIMER,
                             NULL,
                             "Timer error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        case ADI_COMMON_HAL_GEN_SW: /* fall through */
        default:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Param error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    device->spiSettings.autoIncAddrUp = spiSettings->autoIncAddrUp;
    /* Following line is where fpga9025Devic->common.devHalInfo pointer is overwritten */
    device->spiSettings.cmosPadDrvStrength = spiSettings->cmosPadDrvStrength;
    device->spiSettings.enSpiStreaming     = spiSettings->enSpiStreaming;
    device->spiSettings.fourWireMode       = spiSettings->fourWireMode;
    device->spiSettings.msbFirst           = spiSettings->msbFirst;

    /* Toggle RESETB pin, Configure and Verify SPI */

    recoveryAction = adi_adrv9025_HwReset(device);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to reset device and set SPI Config");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_HwClose(adi_adrv9025_Device_t* device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADRV9025_BUGINFO(__FUNCTION__);

    recoveryAction = adi_common_hal_HwClose(&device->common);


    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        switch (recoveryAction)
        {
        case ADI_COMMON_HAL_SPI_FAIL:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             NULL,
                             "SPI error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        case ADI_COMMON_HAL_GPIO_FAIL:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_RESET_FEATURE,
                             NULL,
                             "GPIO error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        case ADI_COMMON_HAL_TIMER_FAIL:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_TIMER,
                             NULL,
                             "Timer error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        case ADI_COMMON_HAL_GEN_SW: /* fall through */
        default:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Param error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_HwReset(adi_adrv9025_Device_t* device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    static const uint8_t RESETB_LEVEL_LOW  = 0;
    static const uint8_t RESETB_LEVEL_HIGH = 1;
    static const uint8_t RESETB_WAIT_MS    = 1;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* check that the hardware is available */
    recoveryAction = adi_adrv9025_HwVerify(device);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        return device->common.error.newAction;
    }
    /* toggle RESETB on device with matching spi chip select index */
    recoveryAction = adi_common_hal_HwReset(&device->common,
                                            RESETB_LEVEL_LOW);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         NULL,
                         "GPIO Reset error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_common_hal_Wait_ms(&device->common,
                                            RESETB_WAIT_MS);




    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_TIMER,
                         NULL,
                         "Timer error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_common_hal_HwReset(&device->common,
                                            RESETB_LEVEL_HIGH);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         NULL,
                         "GPIO Reset error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Configure and Verify Spi */
    recoveryAction = adi_adrv9025_SpiCfgSet(device,
                                            &device->spiSettings);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to set SPI Config");
    ADI_ERROR_RETURN(device->common.error.newAction);

    device->devStateInfo.devState = ADI_ADRV9025_STATE_POWERONRESET;

    return (device->common.error.newAction);
}

/*
Set Masterbias
*/
static int32_t adrv9025_MasterBiasSet(adi_adrv9025_Device_t* device)
{
    uint8_t masterbiasConfig0 = 0;
    uint8_t masterbiasConfig1 = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* core.masterbias_config_0 */
    masterbiasConfig0 &= ~ADRV9025_CONFIG_0_PD_MASTERBIAS;
    /* trm_vbg_abs */
    masterbiasConfig0 |= ADRV9025_CONFIG_0_TRM_VBG_ABS;
    ADRV9025_SPIWRITEBYTE("MASTERBIAS_CONFIG_0",
                          ADRV9025_ADDR_MASTERBIAS_CONFIG_0,
                          masterbiasConfig0);

    /* core.masterbias_config_1 */
    /* trm_vbg_tempco */
    masterbiasConfig1 |= ADRV9025_CONFIG_1_TRM_VBG_TEMPCO;
    ADRV9025_SPIWRITEBYTE("MASTERBIAS_CONFIG_1",
                          ADRV9025_ADDR_MASTERBIAS_CONFIG_1,
                          masterbiasConfig1);

    return (device->common.error.newAction);
}

/*
Set pad configurations for SYSREF, REFCLK and SYNCB pins
*/
int32_t adrv9025_PadConfigsSet(adi_adrv9025_Device_t* device,
                               adi_adrv9025_Init_t*   init)
{
    uint8_t sysrefPad         = 0;
    uint8_t sysrefControl1    = 0;
    uint8_t txSyncPad         = 0;
    uint8_t syncPadSelect     = 0;
    uint8_t syncPadLvdsMode   = 0;
    uint8_t syncPadLvdsPnInv  = 0;
    uint8_t syncDrvSelect     = 0;
    uint8_t syncPadOnchipTerm = 0;
    uint8_t rxSyncPad         = 0;
    uint8_t rxSync3Pad        = 0;
    int     i                 = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        init);

    ADRV9025_BUGINFO(__FUNCTION__);

    /* SysRef Configuration */
    /*ADDR_bf.sysref_pad_pd.set(1'b0); 0=Power-up */
    sysrefPad &= ~ADRV9025_SYSREF_PAD_PD;
    /*ADDR_bf.sysref_pad_lvds_mode.set(1'b1);  LVDS only */
    sysrefPad |= ADRV9025_SYSREF_PAD_LVDS_MODE;
    ADRV9025_SPIWRITEBYTE("SYSREF_PAD_CONFIG",
                          ADRV9025_ADDR_SYSREF_PAD_CONFIG,
                          sysrefPad);

    /*ADDR_bf.sysref_register_enable.set(1'b1); */
    sysrefControl1 |= ADRV9025_SYSREF_REGISTER_ENABLE;
    /*ADDR_bf.sysref_sample_enable.set(1'b0);*/
    sysrefControl1 |= ADRV9025_SYSREF_SAMPLE_ENABLE;
    /*ADDR_bf.sysref_buffer_enable.set(1'b1);       Enable sysref buffer      */
    sysrefControl1 |= ADRV9025_SYSREF_BUFFER_ENABLE;
    ADRV9025_SPIWRITEBYTE("SYSREF_CONTROL_1",
                          ADRV9025_ADDR_SYSREF_CONTROL_1,
                          sysrefControl1);

    /*SyncB Config */
    /*tx_sync_pad_config */
    for (i = 0; i < ADI_ADRV9025_NUM_DEFRAMERS; i++)
    {
        if (init->dataInterface.deframer[i].deserializerLanesEnabled != 0)
        {
            syncPadSelect    = init->dataInterface.deframer[i].syncbOutSelect;
            syncPadLvdsMode  = ((init->dataInterface.deframer[i].syncbOutLvdsMode == 0) ? 0 : 1);
            syncPadLvdsPnInv = ((init->dataInterface.deframer[i].syncbOutLvdsPnInvert == 0) ? 0 : 1);
            syncDrvSelect    = init->dataInterface.deframer[i].syncbOutCmosDriveLevel;

            if (syncPadSelect == 0)
            {
                /*core.tx_sync1_pad_pd */
                txSyncPad &= ~ADRV9025_TX_SYNC1_PAD_PD;
                /*core.tx_sync1_pad_lvds_mode */
                if (syncPadLvdsMode)
                {
                    txSyncPad |= ADRV9025_TX_SYNC1_PAD_LVDS_MODE;
                }

                /*core.tx_sync1_pad_lvds_pn_inv */
                if (syncPadLvdsPnInv)
                {
                    txSyncPad |= ADRV9025_TX_SYNC1_PAD_LVSD_PN_INV;
                }

                /*core.tx_sync_drv_select */
                txSyncPad |= ADRV9025_BF_ENCODE(syncDrvSelect,
                                                ADRV9025_TX_SYNC_DRV_SELECT_MASK,
                                                ADRV9025_TX_SYNC_DRV_SELECT_SHIFT);
            }
            else
            {
                /*core.tx_sync2_pad_pd */
                txSyncPad &= ~ADRV9025_TX_SYNC2_PAD_PD;
                /*core.tx_sync2_pad_lvds_mode */
                if (syncPadLvdsMode)
                {
                    txSyncPad |= ADRV9025_TX_SYNC2_PAD_LVDS_MODE;
                }
                /*core.tx_sync2_pad_lvds_pn_inv */
                if (syncPadLvdsPnInv)
                {
                    txSyncPad |= ADRV9025_TX_SYNC2_PAD_LVSD_PN_INV;
                }

                /*core.tx_sync_drv_select */
                txSyncPad |= ADRV9025_BF_ENCODE(syncDrvSelect,
                                                ADRV9025_TX_SYNC_DRV_SELECT_MASK,
                                                ADRV9025_TX_SYNC_DRV_SELECT_SHIFT);
            }
        }
    }

    ADRV9025_SPIWRITEBYTE("TX_SYNC_PAD_CONFIG",
                          ADRV9025_ADDR_TX_SYNC_PAD_CONFIG,
                          txSyncPad);

    /*SyncB Config */
    /*Register: core.rx_sync1_2_3_pad_config */
    for (i = 0; i < ADI_ADRV9025_NUM_FRAMERS; i++)
    {
        if (init->dataInterface.framer[i].serializerLanesEnabled != 0)
        {
            syncPadSelect     = init->dataInterface.framer[i].syncbInSelect;
            syncPadLvdsMode   = ((init->dataInterface.framer[i].syncbInLvdsMode == 0) ? 0 : 1);
            syncPadLvdsPnInv  = ((init->dataInterface.framer[i].syncbInLvdsPnInvert == 0) ? 0 : 1);
            syncPadOnchipTerm = ((init->dataInterface.framer[i].syncbInLvdsMode == 0) ? 0 : 1);

            if (syncPadSelect == 0)
            {
                /*core.rx_sync1_pad_pd */
                rxSyncPad &= ~ADRV9025_RX_SYNC1_PAD_PD;
                /*core.rx_sync1_pad_lvds_mode */
                if (syncPadLvdsMode)
                {
                    rxSyncPad |= ADRV9025_RX_SYNC1_PAD_LVDS_MODE;
                }

                /*core.rx_sync1_pad_lvds_pn_inv */
                if (syncPadLvdsPnInv)
                {
                    rxSyncPad |= ADRV9025_RX_SYNC1_PAD_LVSD_PN_INV;
                }

                /*core.rx_sync1_onchip_term */
                if (syncPadOnchipTerm)
                {
                    rxSyncPad |= ADRV9025_RX_SYNC1_PAD_ONCHIP_TERM;
                }
            }
            else if (syncPadSelect == 1)
            {
                /*core.rx_sync2_pad_pd */
                rxSyncPad &= ~ADRV9025_RX_SYNC2_PAD_PD;
                /*core.rx_sync2_pad_lvds_mode */
                if (syncPadLvdsMode)
                {
                    rxSyncPad |= ADRV9025_RX_SYNC2_PAD_LVDS_MODE;
                }

                /*core.rx_sync2_pad_lvds_pn_inv */
                if (syncPadLvdsPnInv)
                {
                    rxSyncPad |= ADRV9025_RX_SYNC2_PAD_LVSD_PN_INV;
                }

                /*core.rx_sync2_onchip_term */
                if (syncPadOnchipTerm)
                {
                    rxSyncPad |= ADRV9025_RX_SYNC2_PAD_ONCHIP_TERM;
                }
            }
            else
            {
                /*core.rx_sync3_pad_pd */
                rxSync3Pad &= ~ADRV9025_RX_SYNC3_PAD_PD;
                /*core.rx_sync3_pad_lvds_mode */
                if (syncPadLvdsMode)
                {
                    rxSync3Pad |= ADRV9025_RX_SYNC3_PAD_LVDS_MODE;
                }

                /*core.rx_sync3_pad_lvds_pn_inv */
                if (syncPadLvdsPnInv)
                {
                    rxSync3Pad |= ADRV9025_RX_SYNC3_PAD_LVSD_PN_INV;
                }

                /*core.rx_sync3_onchip_term */
                if (syncPadOnchipTerm)
                {
                    rxSync3Pad |= ADRV9025_RX_SYNC3_PAD_ONCHIP_TERM;
                }
            }
        }
    }

    ADRV9025_SPIWRITEBYTE("RX_SYNC12_PAD_CONFIG",
                          ADRV9025_ADDR_RX_SYNC12_PAD_CONFIG,
                          rxSyncPad);

    ADRV9025_SPIWRITEBYTE("RX_SYNC3_PAD_CONFIG",
                          ADRV9025_ADDR_RX_SYNC3_PAD_CONFIG,
                          rxSync3Pad);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_Initialize(adi_adrv9025_Device_t* device,
                                adi_adrv9025_Init_t*   init)
{
    uint32_t i                                        = 0;
    uint32_t txMaskArray[ADI_ADRV9025_MAX_TXCHANNELS] = {ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TX3, ADI_ADRV9025_TX4};
    uint32_t rxMaskArray[ADI_ADRV9025_MAX_RXCHANNELS] = {
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

    adi_adrv9025_Info_t devStateInfoClear = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        init);

    /* Restore silicon revision from current device structure */
    devStateInfoClear.deviceSiRev = device->devStateInfo.deviceSiRev;

    ADRV9025_BUGINFO(__FUNCTION__);

    device->devStateInfo = devStateInfoClear;

    adrv9025_CpuInitialize(device);



    adrv9025_MasterBiasSet(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Disable stream pin mode until after streams are loaded */
    /* Disable Tx pin mode for all Tx and Rx channels, ORx was defaulted with pin mode disabled */
    ADRV9025_SPIWRITEBYTE("TXALL_BBIC_ENABLE",
                          ADRV9025_ADDR_TXALL_BBIC_ENABLE,
                          0x00);
    ADRV9025_SPIWRITEBYTE("RXALL_BBIC_ENABLE",
                          ADRV9025_ADDR_RXALL_BBIC_ENABLE,
                          0x00);

    adrv9025_LdoEnable(device,
                       init->clocks.ldoSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_PadConfigsSet(device,
                           init);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adi_adrv9025_ProfilesVerify(device,
                                init);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_ClocksSet(device,
                       init);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Program FIR filters */
    if (device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID)
    {
        for (i = 0; i < ADI_ADRV9025_MAX_TXCHANNELS; i++)
        {
            if (ADRV9025_BF_EQUAL(init->tx.txInitChannelMask,
                                  txMaskArray[i]))
            {
                device->devStateInfo.initializedChannels |= (txMaskArray[i] << ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET);

                if (init->tx.txChannelCfg[i].profile.txFir.numFirCoefs > 0)
                {
                    adi_adrv9025_TxFirWrite(device,
                                            txMaskArray[i],
                                            init->tx.txChannelCfg[i].profile.txFir.gain_dB,
                                            init->tx.txChannelCfg[i].profile.txFir.numFirCoefs,
                                            &init->tx.txChannelCfg[i].profile.txFir.coefs[0],
                                            ADI_ADRV9025_MAX_TXPFIR_COEFS);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID)
    {
        for (i = 0; i < ADI_ADRV9025_MAX_RX_ONLY; i++)
        {
            if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                                  rxMaskArray[i]))
            {
                device->devStateInfo.initializedChannels |= rxMaskArray[i];

                if (init->rx.rxChannelCfg[i].profile.rxFir.numFirCoefs > 0)
                {
                    adi_adrv9025_RxFirWrite(device,
                                            rxMaskArray[i],
                                            init->rx.rxChannelCfg[i].profile.rxFir.gain_dB,
                                            init->rx.rxChannelCfg[i].profile.rxFir.numFirCoefs,
                                            &init->rx.rxChannelCfg[i].profile.rxFir.coefs[0],
                                            ADI_ADRV9025_MAX_RXPFIR_COEFS);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }

        for (i = 0; i < ADI_ADRV9025_MAX_RXCHANNELS; i++)
        {
            device->devStateInfo.rxOutputRate_kHz[i]  = init->rx.rxChannelCfg[i].profile.rxOutputRate_kHz;
            device->devStateInfo.rxRfBandWidth_kHz[i] = init->rx.rxChannelCfg[i].profile.rfBandwidth_kHz;
        }
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID)
    {
        for (i = ADI_ADRV9025_MAX_RX_ONLY; i < ADI_ADRV9025_MAX_LB_CHANNEL_START; i++)
        {
            if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                                  rxMaskArray[i]))
            {
                device->devStateInfo.initializedChannels |= rxMaskArray[i];

                if (init->rx.rxChannelCfg[i].profile.rxFir.numFirCoefs > 0)
                {
                    adi_adrv9025_RxFirWrite(device,
                                            rxMaskArray[i],
                                            init->rx.rxChannelCfg[i].profile.rxFir.gain_dB,
                                            init->rx.rxChannelCfg[i].profile.rxFir.numFirCoefs,
                                            &init->rx.rxChannelCfg[i].profile.rxFir.coefs[0],
                                            ADI_ADRV9025_MAX_RXPFIR_COEFS);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_LB_PROFILE_VALID)
    {
        for (i = ADI_ADRV9025_MAX_LB_CHANNEL_START; i < ADI_ADRV9025_MAX_RXCHANNELS; i++)
        {
            if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                                  rxMaskArray[i]) &&
                (init->rx.rxChannelCfg[i].profile.rxFir.numFirCoefs > 0))
            {
                device->devStateInfo.initializedChannels |= rxMaskArray[i];
                adi_adrv9025_RxFirWrite(device,
                                        rxMaskArray[i],
                                        init->rx.rxChannelCfg[i].profile.rxFir.gain_dB,
                                        init->rx.rxChannelCfg[i].profile.rxFir.numFirCoefs,
                                        &init->rx.rxChannelCfg[i].profile.rxFir.coefs[0],
                                        ADI_ADRV9025_MAX_RXPFIR_COEFS);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    device->devStateInfo.devState = (adi_adrv9025_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV9025_STATE_INITIALIZED);

    device->devStateInfo.linkSharingEnabled = init->dataInterface.linkSharingCfg.linkSharingEnabled;

    if (device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID)
    {
        for (i = 0; i < ADI_ADRV9025_MAX_TXCHANNELS; i++)
        {
            device->devStateInfo.txInputRate_kHz[i]   = init->tx.txChannelCfg[i].profile.txInputRate_kHz;
            device->devStateInfo.txRfBandWidth_kHz[i] = init->tx.txChannelCfg[i].profile.rfBandwidth_kHz;
        }
    }

    /*
     * Save some JESD Profile configuration into devStateInfo
     */
    for (i = 0; i < ADI_ADRV9025_NUM_FRAMERS; i++)
    {
        device->devStateInfo.jesdCfg.framerCfg[i].enableJesd204C         = init->dataInterface.framer[i].enableJesd204C;
        device->devStateInfo.jesdCfg.framerCfg[i].serializerLanesEnabled = init->dataInterface.framer[i].serializerLanesEnabled;
    }

    for (i = 0; i < ADI_ADRV9025_NUM_DEFRAMERS; i++)
    {
        device->devStateInfo.jesdCfg.deframerCfg[i].enableJesd204C           = init->dataInterface.deframer[i].enableJesd204C;
        device->devStateInfo.jesdCfg.deframerCfg[i].deserializerLanesEnabled = init->dataInterface.deframer[i].deserializerLanesEnabled;
    }

    device->devStateInfo.extDpdCaptureTriggerGpio = ADI_ADRV9025_GPIO_INVALID;

    device->devStateInfo.extDpdLutSwapModeEn = (uint8_t)0;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_Shutdown(adi_adrv9025_Device_t* device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADRV9025_BUGINFO(__FUNCTION__);

    recoveryAction = adi_adrv9025_HwReset(device);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "Failed to reset device");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_MultichipSyncSet(adi_adrv9025_Device_t* device,
                                      uint8_t                enableSync)
{
    int32_t              recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t              payload[4]     = {0};
    uint8_t              cmdStatusByte  = 0;
    static const uint8_t ARM_ERR_MASK   = 0x0E;
    uint8_t              mcsDonePayload[1] = { ADRV9025_CPU_OBJECTID_MCS_DONE };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADRV9025_BUGINFO(__FUNCTION__);

    payload[0] = ADRV9025_CPU_OBJECTID_MCS;
    payload[1] = 0;
    payload[2] = 0;
    payload[3] = 0;

    if (enableSync > 0)
    {
        recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                                  ADI_ADRV9025_CPU_TYPE_C,
                                                  ADRV9025_CPU_SET_OPCODE,
                                                  &payload[0],
                                                  4);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to send ARM command");
        ADI_ERROR_RETURN(device->common.error.newAction);

        adi_adrv9025_CpuCmdStatusWait(device,
                                      ADI_ADRV9025_CPU_TYPE_C,
                                      ADRV9025_CPU_SET_OPCODE,
                                      &cmdStatusByte,
                                      ADI_ADRV9025_SETCPUBOOTUPMCS_TIMEOUT_US,
                                      ADI_ADRV9025_SETCPUBOOTUPMCS_INTERVAL_US);



        ADI_ERROR_RETURN(device->common.error.newAction);

        if ((cmdStatusByte & ARM_ERR_MASK) > 0)
        {
            adrv9025_CpuCmdErrorHandler(device,
                                        ADI_ADRV9025_CPU_TYPE_C,
                                        ADI_ADRV9025_SRC_CPUCMD,
                                        ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                 ADRV9025_CPU_OBJECTID_MCS,
                                                                 cmdStatusByte),
                                        recoveryAction);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* MCS Fix Clear ARM_M3_RUN */
        adrv9025_CoreArmM3RunBfSet(device,
                                   ADRV9025_BF_CORE,
                                   0);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        /* MCS Fix Set ARM_M3_RUN */
        adrv9025_CoreArmM3RunBfSet(device,
                                   ADRV9025_BF_CORE,
                                   1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Send MCS done command to ARM */
        recoveryAction = adi_adrv9025_CpuCmdWrite(device, ADI_ADRV9025_CPU_TYPE_C, (uint8_t)ADRV9025_CPU_SET_OPCODE, &mcsDonePayload[0], sizeof(mcsDonePayload));
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, mcsDonePayload, device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Wait for command to finish executing */
        recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
            ADI_ADRV9025_CPU_TYPE_C,
            (uint8_t)ADRV9025_CPU_SET_OPCODE,
            &cmdStatusByte,
            (uint32_t)ADI_ADRV9025_SETMCS_DONE_TIMEOUT_US,
            (uint32_t)ADI_ADRV9025_SETMCS_DONE_INTERVAL_US);

        /* If cmdStatusByte is non-zero then flag an ARM error */
        if ((cmdStatusByte & ARM_ERR_MASK) > 0)
        {
            recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                ADI_ADRV9025_CPU_TYPE_C,
                ADI_ADRV9025_SRC_CPUCMD,
                ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE, mcsDonePayload[0], cmdStatusByte),
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
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_MultichipSyncStatusGet(adi_adrv9025_Device_t* device,
                                            uint32_t*              mcsStatus)
{
    uint8_t mcsStatusRead = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        mcsStatus);

    ADRV9025_BUGINFO(__FUNCTION__);

    ADRV9025_SPIREADBYTE("CLK_PLL_MCS_CONTROL_3",
                         ADRV9025_ADDR_CLK_PLL + ADRV9025_ADDR_MCS_CONTROL_3_OFFSET,
                         &mcsStatusRead);

    *mcsStatus = (uint32_t)mcsStatusRead;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ProfilesVerify(adi_adrv9025_Device_t* device,
                                    adi_adrv9025_Init_t*   init)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        init);

    recoveryAction = adrv9025_ProfilesVerify(device,
                                             init);
    ADI_ERROR_RETURN(device->common.error.newAction);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "ADRV9025 Profiles are invalid");

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_SpiCfgSet(adi_adrv9025_Device_t*      device,
                               adi_adrv9025_SpiSettings_t* spi)
{
    int32_t              recoveryAction            = ADI_COMMON_ACT_NO_ACTION;
    uint8_t              spiConfigA                = 0;
    uint8_t              spiConfigB                = 0;
    uint8_t              ioControl                 = 0;
    static const uint8_t SPICFG_MSBFIRST_OFF       = 0;
    static const uint8_t SPICFG_AUTOINC_DOWN       = 0;
    static const uint8_t SPICFG_FOURWIREMODE_OFF   = 0;
    static const uint8_t SPICFG_ENSPISTREAMING_OFF = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        spi);

    ADRV9025_BUGINFO(__FUNCTION__);

    if ((spi->cmosPadDrvStrength != ADI_ADRV9025_CMOSPAD_DRV_WEAK) &&
        (spi->cmosPadDrvStrength != ADI_ADRV9025_CMOSPAD_DRV_STRONG))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         spi->cmosPadDrvStrength,
                         "Invalid Cmos Pad Drive Strength\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* core.spi_interface_config_A */
    /* SPI bit is 1 = LSB first */
    if (spi->msbFirst == SPICFG_MSBFIRST_OFF)
    {
        spiConfigA |= ADRV9025_CONFIG_A_SPI_LSB_FIRST;
    }

    if (spi->autoIncAddrUp != SPICFG_AUTOINC_DOWN)
    {
        spiConfigA |= ADRV9025_CONFIG_A_SPI_ADDR_ASCENSION;
    }

    if (spi->fourWireMode != SPICFG_FOURWIREMODE_OFF)
    {
        spiConfigA |= ADRV9025_CONFIG_A_SPI_SDO_ACTIVE;
    }

    ADRV9025_SPIWRITEBYTE("SPI_INTERFACE_CONFIG_A",
                          ADRV9025_ADDR_SPI_INTERFACE_CONFIG_A,
                          spiConfigA);

    if (spi->cmosPadDrvStrength == ADI_ADRV9025_CMOSPAD_DRV_STRONG)
    {
        ioControl |= ADRV9025_IO_CONTROL_SPI_OUTS_DRV_SEL;
    }

    /* Force PAD mode */
    ADRV9025_SPIWRITEBYTE("DIGITAL_IO_CONTROL",
                          ADRV9025_ADDR_DIGITAL_IO_CONTROL,
                          ioControl);

    /* core.spi_interface_config_B */
    if (spi->enSpiStreaming == SPICFG_ENSPISTREAMING_OFF)
    {
        spiConfigB |= ADRV9025_CONFIG_B_SINGLE_INSTRUCTION;
    }

    /* Force single instruction mode */
    ADRV9025_SPIWRITEBYTE("SPI_INTERFACE_CONFIG_B",
                          ADRV9025_ADDR_SPI_INTERFACE_CONFIG_B,
                          spiConfigB);

    recoveryAction = adi_adrv9025_SpiVerify(device);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     recoveryAction,
                     NULL,
                     "SPI Verify failed");

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_SpiCfgGet(adi_adrv9025_Device_t*      device,
                               adi_adrv9025_SpiSettings_t* spi)
{
    uint8_t              spiConfigA                = 0;
    uint8_t              spiConfigB                = 0;
    uint8_t              ioControl                 = 0;
    static const uint8_t SPICFG_MSBFIRST_OFF       = 0;
    static const uint8_t SPICFG_AUTOINC_DOWN       = 0;
    static const uint8_t SPICFG_FOURWIREMODE_OFF   = 0;
    static const uint8_t SPICFG_ENSPISTREAMING_OFF = 0;
    static const uint8_t SPICFG_MSBFIRST_ON        = 1;
    static const uint8_t SPICFG_AUTOINC_UP         = 1;
    static const uint8_t SPICFG_FOURWIREMODE_ON    = 1;
    static const uint8_t SPICFG_ENSPISTREAMING_ON  = 1;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        spi);

    ADRV9025_BUGINFO(__FUNCTION__);

    ADRV9025_SPIREADBYTE("SPI_INTERFACE_CONFIG_A",
                         ADRV9025_ADDR_SPI_INTERFACE_CONFIG_A,
                         &spiConfigA);

    /* core.spi_interface_config_A */
    /* SPI bit is 1 = LSB first */
    if (ADRV9025_BF_EQUAL(spiConfigA,
                          ADRV9025_CONFIG_A_SPI_LSB_FIRST))
    {
        spi->msbFirst = SPICFG_MSBFIRST_OFF;
    }
    else
    {
        spi->msbFirst = SPICFG_MSBFIRST_ON;
    }

    if (ADRV9025_BF_EQUAL(spiConfigA,
                          ADRV9025_CONFIG_A_SPI_ADDR_ASCENSION))
    {
        spi->autoIncAddrUp = SPICFG_AUTOINC_UP;
    }
    else
    {
        spi->autoIncAddrUp = SPICFG_AUTOINC_DOWN;
    }

    if (ADRV9025_BF_EQUAL(spiConfigA,
                          ADRV9025_CONFIG_A_SPI_SDO_ACTIVE))
    {
        spi->fourWireMode = SPICFG_FOURWIREMODE_ON;
    }
    else
    {
        spi->fourWireMode = SPICFG_FOURWIREMODE_OFF;
    }

    /* Read PAD mode */
    ADRV9025_SPIREADBYTE("DIGITAL_IO_CONTROL",
                         ADRV9025_ADDR_DIGITAL_IO_CONTROL,
                         &ioControl);

    if (ADRV9025_BF_EQUAL(ioControl,
                          ADRV9025_IO_CONTROL_SPI_OUTS_DRV_SEL))
    {
        spi->cmosPadDrvStrength = ADI_ADRV9025_CMOSPAD_DRV_STRONG;
    }
    else
    {
        spi->cmosPadDrvStrength = ADI_ADRV9025_CMOSPAD_DRV_WEAK;
    }

    /* Read single instruction mode */
    ADRV9025_SPIREADBYTE("SPI_INTERFACE_CONFIG_B",
                         ADRV9025_ADDR_SPI_INTERFACE_CONFIG_B,
                         &spiConfigB);

    /* core.spi_interface_config_B */
    if (ADRV9025_BF_EQUAL(spiConfigB,
                          ADRV9025_CONFIG_B_SINGLE_INSTRUCTION))
    {
        spi->enSpiStreaming = SPICFG_ENSPISTREAMING_OFF;
    }
    else
    {
        spi->enSpiStreaming = SPICFG_ENSPISTREAMING_ON;
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_SpiVerify(adi_adrv9025_Device_t* device)
{
    uint8_t spiReg = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADRV9025_BUGINFO(__FUNCTION__);

    static const uint8_t SCRATCH_PAD_1 = 0xB6; /* DATA 10110110 */
    static const uint8_t SCRATCH_PAD_2 = 0x49; /* DATA 01001001*/
    static const uint8_t VENDOR_ID_0   = 0x56;
    static const uint8_t VENDOR_ID_1   = 0x04;

    /* check that the hardware is available */
    int32_t recoveryAction = adi_adrv9025_HwVerify(device);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        return device->common.error.newAction;
    }

    /* Check SPI read - VENDOR_ID_0 */
    ADRV9025_SPIREADBYTE("VENDOR_ID_0",
                         ADRV9025_ADDR_VENDOR_ID_0,
                         &spiReg);

    if (spiReg != VENDOR_ID_0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         spiReg,
                         "Cannot read from a low SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI read - VENDOR_ID_1 */
    spiReg = 0;
    ADRV9025_SPIREADBYTE("VENDOR_ID_1",
                         ADRV9025_ADDR_VENDOR_ID_1,
                         &spiReg);



    if (spiReg != VENDOR_ID_1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         spiReg,
                         "Cannot read from a low SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI write - SCRATCHPAD : Data = 10110110 */
    spiReg = 0;
    ADRV9025_SPIWRITEBYTE("SCRATCH_PAD",
                          ADRV9025_ADDR_SCRATCH_PAD,
                          SCRATCH_PAD_1);

    ADRV9025_SPIREADBYTE("SCRATCH_PAD",
                         ADRV9025_ADDR_SCRATCH_PAD,
                         &spiReg);



    if (spiReg != SCRATCH_PAD_1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         spiReg,
                         "Cannot write to a low SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI write - SCRATCHPAD : Data = 01001001 */
    spiReg = 0;
    ADRV9025_SPIWRITEBYTE("SCRATCH_PAD",
                          ADRV9025_ADDR_SCRATCH_PAD,
                          SCRATCH_PAD_2);

    ADRV9025_SPIREADBYTE("SCRATCH_PAD",
                         ADRV9025_ADDR_SCRATCH_PAD,
                         &spiReg);



    if (spiReg != SCRATCH_PAD_2)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         spiReg,
                         "Cannot write to a low SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI write - SCRATCH_PAD_UPPER_ADDRESS_SPACE : Data = 10110110 */
    spiReg = 0;
    ADRV9025_SPIWRITEBYTE("SCRATCH_PAD_UPPER",
                          ADRV9025_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE,
                          SCRATCH_PAD_1);

    ADRV9025_SPIREADBYTE("SCRATCH_PAD_UPPER",
                         ADRV9025_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE,
                         &spiReg);



    if (spiReg != SCRATCH_PAD_1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         spiReg,
                         "Cannot write to a high SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI write - SCRATCH_PAD_UPPER_ADDRESS_SPACE : Data = 01001001 */
    spiReg = 0;
    ADRV9025_SPIWRITEBYTE("SCRATCH_PAD_UPPER_ADDR",
                          ADRV9025_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE,
                          SCRATCH_PAD_2);

    ADRV9025_SPIREADBYTE("SCRATCH_PAD_UPPER_ADDR",
                         ADRV9025_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE,
                         &spiReg);



    if (spiReg != SCRATCH_PAD_2)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         spiReg,
                         "Cannot write to a high SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* TODO: Add code to check Spi Streaming when HAL support is available. */

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ApiVersionGet(adi_adrv9025_Device_t*     device,
                                   adi_adrv9025_ApiVersion_t* apiVersion)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        apiVersion);

    ADRV9025_BUGINFO(__FUNCTION__);

    apiVersion->majorVer       = ADI_ADRV9025_CURRENT_MAJOR_VERSION;
    apiVersion->minorVer       = ADI_ADRV9025_CURRENT_MINOR_VERSION;
    apiVersion->maintenanceVer = ADI_ADRV9025_CURRENT_MAINTENANCE_VERSION;
    apiVersion->buildVer       = ADI_ADRV9025_CURRENT_BUILD_VERSION;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DeviceRevGet(adi_adrv9025_Device_t* device,
                                  uint8_t*               siRevision)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        siRevision);

    ADRV9025_BUGINFO(__FUNCTION__);

    ADRV9025_SPIREADBYTE("PRODUCT_ID_1",
                         ADRV9025_ADDR_PRODUCT_ID_1,
                         siRevision);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ProductIdGet(adi_adrv9025_Device_t* device,
                                  uint8_t*               productId)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        productId);

    ADRV9025_BUGINFO(__FUNCTION__);

    ADRV9025_SPIREADBYTE("PRODUCT_ID_0",
                         ADRV9025_ADDR_PRODUCT_ID_0,
                         productId);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_DeviceCapabilityGet(adi_adrv9025_Device_t*          device,
                                         adi_adrv9025_CapabilityModel_t* devCapability)
{
    uint8_t productId  = 0;
    uint8_t siRevision = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        devCapability);

    ADRV9025_SPIREADBYTE("PRODUCT_ID_0",
                         ADRV9025_ADDR_PRODUCT_ID_0,
                         &productId);
    ADRV9025_SPIREADBYTE("PRODUCT_ID_1",
                         ADRV9025_ADDR_PRODUCT_ID_1,
                         &siRevision);

    /* set devCapability to default settings */
    devCapability->productId                  = productId;
    devCapability->siRevision                 = siRevision;
    devCapability->txNumber                   = 4;
    devCapability->rxNumber                   = 4;
    devCapability->orxNumber                  = 4;
    devCapability->txChannelMask              = ADI_ADRV9025_TXALL;
    devCapability->rxChannelMask              = ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4;
    devCapability->orxChannelMask             = ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4;
    devCapability->featureMask                = ADI_ADRV9025_FEATUREALL;
    devCapability->txMaxBw_kHz                = 450000;
    devCapability->rxMaxBw_kHz                = 200000;
    devCapability->orxMaxBw_kHz               = 450000;
    devCapability->rfFreqRangeMin_kHz         = 400000;
    devCapability->rfFreqRangeMax_kHz         = 6000000;
    devCapability->serializerLaneEnableMask   = ADI_ADRV9025_ALL_LANES_ENABLED;
    devCapability->deserializerLaneEnableMask = ADI_ADRV9025_ALL_LANES_ENABLED;

    /* productId mapping */
    switch (productId)
    {
    case 0x80:
        devCapability->featureMask = ADI_ADRV9025_FEATUREALL;
        break;
    case 0x81:
        devCapability->featureMask = devCapability->featureMask & ~(ADI_ADRV9025_FEATURE_RESERVED1 | ADI_ADRV9025_FEATURE_RESERVED2);
        break;
    case 0x82:
        devCapability->featureMask = devCapability->featureMask & ~(ADI_ADRV9025_FEATURE_CFR | ADI_ADRV9025_FEATURE_DPD | ADI_ADRV9025_FEATURE_RESERVED1 |
            ADI_ADRV9025_FEATURE_RESERVED2);
        break;
    case 0x83:
        devCapability->featureMask = devCapability->featureMask & ~(ADI_ADRV9025_FEATURE_CFR | ADI_ADRV9025_FEATURE_RESERVED2);
        break;
    case 0x84:
        devCapability->featureMask = devCapability->featureMask & ~(ADI_ADRV9025_FEATURE_RESERVED1 | ADI_ADRV9025_FEATURE_RESERVED2);
        break;
    case 0x85:
        devCapability->featureMask = devCapability->featureMask & ~(ADI_ADRV9025_FEATURE_RESERVED1 | ADI_ADRV9025_FEATURE_RESERVED2);
        break;
    case 0x86:
        devCapability->featureMask = devCapability->featureMask & ~(ADI_ADRV9025_FEATURE_RESERVED1 | ADI_ADRV9025_FEATURE_RESERVED2);
        break;
    case 0x87:
        devCapability->featureMask = devCapability->featureMask & ~(ADI_ADRV9025_FEATURE_RESERVED1 | ADI_ADRV9025_FEATURE_RESERVED2 | ADI_ADRV9025_FEATURE_CFR | ADI_ADRV9025_FEATURE_DPD);
        break;
    case 0x88:
        devCapability->featureMask = devCapability->featureMask & ~(ADI_ADRV9025_FEATURE_RESERVED2);
        break;

    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "No ProductId found in Capability Model");
        break;
    }

    return (device->common.error.newAction);
}
