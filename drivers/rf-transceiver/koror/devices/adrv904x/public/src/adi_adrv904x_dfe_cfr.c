/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_cfr.c
* \brief Contains CFR features related function implementations
*
* ADRV904X API Version: 2.10.0.4
*/
#include "adi_adrv904x_dfe_cfr.h"
#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_hal.h"

#include "../../private/include/adrv904x_radioctrl.h"
#include "../../private/include/adrv904x_tx.h"
#include "../../private/include/adrv904x_dfe_cfr.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"
#include "../../private/include/adrv904x_stream_proc_types.h"
#include "../../private/bf/adrv904x_bf_streamproc_channel_types.h"
#include "../../private/include/adrv904x_dfe_cfr.h"
#include "../../private/include/adrv904x_cpu_scratch_registers.h"

#include "../../private/bf/adrv904x_bf_core.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_DFE_CFR


static adi_adrv904x_ErrAction_e adrv904x_CfrActiveConfigUpdateCheck(adi_adrv904x_Device_t* const    device,
                                                                    const adi_adrv904x_TxChannels_e cfrTxChannelSel,
                                                                    uint32_t                        configSelectMask);

static adi_adrv904x_ErrAction_e adrv904x_CfrActivePulseUpdateCheck(adi_adrv904x_Device_t* const    device,
                                                                   const adi_adrv904x_TxChannels_e cfrTxChannelSel,
                                                                   uint32_t                        pulseSelectMask);


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrThresholdSet(adi_adrv904x_Device_t* const             device, 
                                                              const adi_adrv904x_CfrThrConfig_t* const cfrThrConfig,
                                                              const uint32_t                           cfrTxChannelMask,
                                                              const uint32_t                           configSelectMask)
{
            adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrThrConfig, cleanup);

    /* Check that if requested Tx Channel valid*/
    if (((cfrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (cfrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /* Check that if requested config set is valid*/
    if (((configSelectMask & (~(uint32_t)ADI_ADRV904X_CFR_CFGALL)) != 0U) ||
         (configSelectMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configSelectMask,
                               "Invalid configuration selected. Valid values are any combinations of CFG0/1");
        goto cleanup;
    }
    
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(cfrTxChannelMask, chanSel))
        {
            recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txCfrBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine CFR base address");
                goto cleanup;
            }

            recoveryAction = adrv904x_CfrActiveConfigUpdateCheck(device,
                                                                 (adi_adrv904x_TxChannels_e)(chanSel),
                                                                 configSelectMask);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, configSelectMask, "Requested configuration is actively being used by CFR engine");
                goto cleanup;
            }

            /* Set engine 0 Thresh Sq*/
            recoveryAction = adrv904x_CfrWritePeakThreshSq(device,
                                                           cfrThrConfig->cfrEngine0PeakThresholdScaler,
                                                           cfrThrConfig->cfrPeakThreshold,
                                                           txCfrBaseAddr,
                                                           configSelectMask,
                                                           0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sq for engine 0");
                goto cleanup;
            }
            
            /* Set engine 1 Thresh Sq*/
            recoveryAction = adrv904x_CfrWritePeakThreshSq(device,
                                                           cfrThrConfig->cfrEngine1PeakThresholdScaler,
                                                           cfrThrConfig->cfrPeakThreshold,
                                                           txCfrBaseAddr,
                                                           configSelectMask,
                                                           1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sq for engine 1");
                goto cleanup;
            }

            /* Set engine 2 Thresh Sq*/
            recoveryAction = adrv904x_CfrWritePeakThreshSq(device,
                                                           cfrThrConfig->cfrEngine2PeakThresholdScaler,
                                                           cfrThrConfig->cfrPeakThreshold,
                                                           txCfrBaseAddr,
                                                           configSelectMask,
                                                           2U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sq for engine 2");
                goto cleanup;
            }

            /* Set engine 0 Thresh Sqrt*/
            recoveryAction = adrv904x_CfrWritePeakThreshSqrt(device,
                                                           cfrThrConfig->cfrEngine0PeakThresholdScaler,
                                                           cfrThrConfig->cfrPeakThreshold,
                                                           txCfrBaseAddr,
                                                           configSelectMask,
                                                           0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sqrt for engine 0");
                goto cleanup;
            }

            /* Set engine 1 Thresh Sqrt*/
            recoveryAction = adrv904x_CfrWritePeakThreshSqrt(device,
                                                             cfrThrConfig->cfrEngine1PeakThresholdScaler,
                                                             cfrThrConfig->cfrPeakThreshold,
                                                             txCfrBaseAddr,
                                                             configSelectMask,
                                                             1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sqrt for engine 1");
                goto cleanup;
            }

            /* Set engine 2 Thresh Sqrt*/
            recoveryAction = adrv904x_CfrWritePeakThreshSqrt(device,
                                                             cfrThrConfig->cfrEngine2PeakThresholdScaler,
                                                             cfrThrConfig->cfrPeakThreshold,
                                                             txCfrBaseAddr,
                                                             configSelectMask,
                                                             2U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sqrt for engine 2");
                goto cleanup;
            }

            /* Set engine 0 Correction Threshold */
            recoveryAction = adrv904x_CfrWriteCorrectionThresh(device,
                                                             cfrThrConfig->cfrCorrectionThresholdScaler,
                                                             cfrThrConfig->cfrPeakThreshold,
                                                             txCfrBaseAddr,
                                                             configSelectMask,
                                                             0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing correction threshold for engine 0");
                goto cleanup;
            }

            /* Set engine 1 Correction Threshold */
            recoveryAction = adrv904x_CfrWriteCorrectionThresh(device,
                                                               cfrThrConfig->cfrCorrectionThresholdScaler,
                                                               cfrThrConfig->cfrPeakThreshold,
                                                               txCfrBaseAddr,
                                                               configSelectMask,
                                                               1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing correction threshold for engine 1");
                goto cleanup;
            }
            
            /* Set engine 2 Correction Threshold */
            recoveryAction = adrv904x_CfrWriteCorrectionThresh(device,
                                                               cfrThrConfig->cfrCorrectionThresholdScaler,
                                                               cfrThrConfig->cfrPeakThreshold,
                                                               txCfrBaseAddr,
                                                               configSelectMask,
                                                               2U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing correction threshold for engine 2");
                goto cleanup;
            }
            
            /* Update device info struct with new thresholds */
            if (ADRV904X_BF_EQUAL(configSelectMask, (uint32_t)ADI_ADRV904X_CFR_CFG0)) 
            {
                device->devStateInfo.cfrThresholdsCfg0[chanIdx].cfrPeakThreshold = cfrThrConfig->cfrPeakThreshold;
                device->devStateInfo.cfrThresholdsCfg0[chanIdx].cfrEngine0PeakThresholdScaler = cfrThrConfig->cfrEngine0PeakThresholdScaler;
                device->devStateInfo.cfrThresholdsCfg0[chanIdx].cfrEngine1PeakThresholdScaler = cfrThrConfig->cfrEngine1PeakThresholdScaler;
                device->devStateInfo.cfrThresholdsCfg0[chanIdx].cfrEngine2PeakThresholdScaler = cfrThrConfig->cfrEngine2PeakThresholdScaler;
                device->devStateInfo.cfrThresholdsCfg0[chanIdx].cfrCorrectionThresholdScaler = cfrThrConfig->cfrCorrectionThresholdScaler;
            }

            if (ADRV904X_BF_EQUAL(configSelectMask, (uint32_t)ADI_ADRV904X_CFR_CFG1)) 
            {
                device->devStateInfo.cfrThresholdsCfg1[chanIdx].cfrPeakThreshold = cfrThrConfig->cfrPeakThreshold;
                device->devStateInfo.cfrThresholdsCfg1[chanIdx].cfrEngine0PeakThresholdScaler = cfrThrConfig->cfrEngine0PeakThresholdScaler;
                device->devStateInfo.cfrThresholdsCfg1[chanIdx].cfrEngine1PeakThresholdScaler = cfrThrConfig->cfrEngine1PeakThresholdScaler;
                device->devStateInfo.cfrThresholdsCfg1[chanIdx].cfrEngine2PeakThresholdScaler = cfrThrConfig->cfrEngine2PeakThresholdScaler;
                device->devStateInfo.cfrThresholdsCfg1[chanIdx].cfrCorrectionThresholdScaler = cfrThrConfig->cfrCorrectionThresholdScaler;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrThresholdGet(adi_adrv904x_Device_t* const          device,
                                                              adi_adrv904x_CfrThrConfig_t* const    cfrThrConfig,
                                                              const uint32_t                        cfrTxChannelSel,
                                                              const uint32_t                        configSelect)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrThrConfig, cleanup);

    switch (cfrTxChannelSel)
    {
        case ADI_ADRV904X_TX0:
            chanIdx    = 0U;
            break;
        case ADI_ADRV904X_TX1:
            chanIdx    = 1U;
            break;
        case ADI_ADRV904X_TX2:
            chanIdx    = 2U;
            break;
        case ADI_ADRV904X_TX3:
            chanIdx    = 3U;
            break;
        case ADI_ADRV904X_TX4:
            chanIdx    = 4U;
            break;
        case ADI_ADRV904X_TX5:
            chanIdx    = 5U;
            break;
        case ADI_ADRV904X_TX6:
            chanIdx    = 6U;
            break;
        case ADI_ADRV904X_TX7:
            chanIdx    = 7U;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   cfrTxChannelSel,
                                   "Invalid Tx channel is selected. Valid values are Tx0/1/2/3/4/5/6/7");
            goto cleanup;
            break;
    }

    /* Read threshold configuration from device struct */
    if (configSelect == (uint32_t)ADI_ADRV904X_CFR_CFG0)
    {
        cfrThrConfig->cfrPeakThreshold              = device->devStateInfo.cfrThresholdsCfg0[chanIdx].cfrPeakThreshold;
        cfrThrConfig->cfrEngine0PeakThresholdScaler = device->devStateInfo.cfrThresholdsCfg0[chanIdx].cfrEngine0PeakThresholdScaler;
        cfrThrConfig->cfrEngine1PeakThresholdScaler = device->devStateInfo.cfrThresholdsCfg0[chanIdx].cfrEngine1PeakThresholdScaler;
        cfrThrConfig->cfrEngine2PeakThresholdScaler = device->devStateInfo.cfrThresholdsCfg0[chanIdx].cfrEngine2PeakThresholdScaler;
        cfrThrConfig->cfrCorrectionThresholdScaler  = device->devStateInfo.cfrThresholdsCfg0[chanIdx].cfrCorrectionThresholdScaler;
        recoveryAction                              = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else if (configSelect == (uint32_t)ADI_ADRV904X_CFR_CFG1)
    {
        cfrThrConfig->cfrPeakThreshold              = device->devStateInfo.cfrThresholdsCfg1[chanIdx].cfrPeakThreshold;
        cfrThrConfig->cfrEngine0PeakThresholdScaler = device->devStateInfo.cfrThresholdsCfg1[chanIdx].cfrEngine0PeakThresholdScaler;
        cfrThrConfig->cfrEngine1PeakThresholdScaler = device->devStateInfo.cfrThresholdsCfg1[chanIdx].cfrEngine1PeakThresholdScaler;
        cfrThrConfig->cfrEngine2PeakThresholdScaler = device->devStateInfo.cfrThresholdsCfg1[chanIdx].cfrEngine2PeakThresholdScaler;
        cfrThrConfig->cfrCorrectionThresholdScaler  = device->devStateInfo.cfrThresholdsCfg1[chanIdx].cfrCorrectionThresholdScaler;
        recoveryAction                              = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configSelect,
                               "Invalid Cfr config selected. User should select either CFG0 or CFG1");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrEnableSet(adi_adrv904x_Device_t* const device,
                                                           const uint32_t               cfrEngineEnable[],
                                                           const uint32_t               cfrEngineBypass[],
                                                           const uint32_t               arraySize,
                                                           const uint32_t               cfrTxChannelMask,
                                                           const uint32_t               configSelectMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;

    uint32_t chanIdx   = 0U;
    uint32_t chanSel   = 0U;
    uint32_t engineIdx = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrEngineEnable, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrEngineBypass, cleanup);

    /*Check that if requested Tx Channel valid*/
    if (((cfrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (cfrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /*Check that if requested config set is valid*/
    if (((configSelectMask & (~(uint32_t)ADI_ADRV904X_CFR_CFGALL)) != 0U) ||
         (configSelectMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configSelectMask,
                               "Invalid configuration selected. Valid values are any combinations of CFG0/1");
        goto cleanup;
    }
    
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(cfrTxChannelMask, chanSel))
        {
            recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txCfrBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine CFR base address");
                goto cleanup;
            }

            recoveryAction = adrv904x_CfrActiveConfigUpdateCheck(device,
                                                                 (adi_adrv904x_TxChannels_e)(chanSel),
                                                                 configSelectMask);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, configSelectMask, "Requested configuration is actively being used by CFR engine");
                goto cleanup;
            }
            
            if (ADRV904X_BF_EQUAL(configSelectMask, (uint32_t)ADI_ADRV904X_CFR_CFG0)) 
            {
                for (engineIdx = 0U; engineIdx < arraySize; ++engineIdx) 
                {
                    /* Write engine enable - Cfg0 */
                    recoveryAction = adrv904x_TxCfrReg_Cfr0CtrlEn_BfSet(device,
                                                                        NULL,
                                                                        txCfrBaseAddr,
                                                                        engineIdx,
                                                                        cfrEngineEnable[engineIdx]);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing engine enable Cfg0");
                        goto cleanup;
                    }
                    
                    /* Write engine bypass - Cfg0 */
                    recoveryAction = adrv904x_TxCfrReg_Cfr0CtrlByp_BfSet(device,
                                                                         NULL,
                                                                         txCfrBaseAddr,
                                                                         engineIdx,
                                                                         cfrEngineBypass[engineIdx]);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing engine bypass Cfg0");
                        goto cleanup;
                    }
                }
            }

            if (ADRV904X_BF_EQUAL(configSelectMask, (uint32_t)ADI_ADRV904X_CFR_CFG1)) 
            {
                for (engineIdx = 0U; engineIdx < arraySize; ++engineIdx) 
                {
                    /* Write engine enable - Cfg1 */
                    recoveryAction = adrv904x_TxCfrReg_Cfr1CtrlEn_BfSet(device,
                                                                        NULL,
                                                                        txCfrBaseAddr,
                                                                        engineIdx,
                                                                        cfrEngineEnable[engineIdx]);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing engine enable Cfg1");
                        goto cleanup;
                    }
                    
                    /* Write engine bypass - Cfg1 */
                    recoveryAction = adrv904x_TxCfrReg_Cfr1CtrlByp_BfSet(device,
                                                                         NULL,
                                                                         txCfrBaseAddr,
                                                                         engineIdx,
                                                                         cfrEngineBypass[engineIdx]);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing engine bypass Cfg1");
                        goto cleanup;
                    }
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrEnableGet(adi_adrv904x_Device_t* const device,
                                                           uint32_t                     cfrEngineEnable[],
                                                           uint32_t                     cfrEngineBypass[],
                                                           const uint32_t               arraySize,
                                                           const uint32_t               cfrTxChannelSel,
                                                           const uint32_t               configSelect)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    uint8_t tmpReadBack = 0U;
    uint32_t engineIdx  = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrEngineEnable, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrEngineBypass, cleanup);

    recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(cfrTxChannelSel), &txCfrBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cfrTxChannelSel, "Invalid Tx Channel used to determine CFR base address");
        goto cleanup;
    }

    if (configSelect == (uint32_t)ADI_ADRV904X_CFR_CFG0)
    {
        for (engineIdx = 0U; engineIdx < arraySize; ++engineIdx) 
        {
            /* Read engine enable - Cfg0 */
            recoveryAction = adrv904x_TxCfrReg_Cfr0CtrlEn_BfGet(device,
                                                                NULL,
                                                                txCfrBaseAddr,
                                                                engineIdx,
                                                                &tmpReadBack);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading engine enable Cfg0");
                goto cleanup;
            }

            cfrEngineEnable[engineIdx] = tmpReadBack;

            /* Read engine bypass - Cfg0 */
            recoveryAction = adrv904x_TxCfrReg_Cfr0CtrlByp_BfGet(device,
                                                                 NULL,
                                                                 txCfrBaseAddr,
                                                                 engineIdx,
                                                                 &tmpReadBack);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading engine bypass Cfg0");
                goto cleanup;
            }

            cfrEngineBypass[engineIdx] = tmpReadBack;
        }
    }
    else if (configSelect == (uint32_t)ADI_ADRV904X_CFR_CFG1)
    {
        for (engineIdx = 0U; engineIdx < arraySize; ++engineIdx) 
        {
            /* Read engine enable - Cfg1 */
            recoveryAction = adrv904x_TxCfrReg_Cfr1CtrlEn_BfGet(device,
                                                                NULL,
                                                                txCfrBaseAddr,
                                                                engineIdx,
                                                                &tmpReadBack);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading engine enable Cfg1");
                goto cleanup;
            }

            cfrEngineEnable[engineIdx] = tmpReadBack;

            /* Read engine bypass - Cfg1 */
            recoveryAction = adrv904x_TxCfrReg_Cfr1CtrlByp_BfGet(device,
                                                                 NULL,
                                                                 txCfrBaseAddr,
                                                                 engineIdx,
                                                                 &tmpReadBack);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading engine bypass Cfg1");
                goto cleanup;
            }

            cfrEngineBypass[engineIdx] = tmpReadBack;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configSelect,
                               "Invalid Cfr config selected. User should select either CFG0 or CFG1");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrConfigSet(adi_adrv904x_Device_t* const             device,
                                                           const adi_adrv904x_CfrConfig_t* const    cfrCtrlConfig,
                                                           const uint32_t                           cfrTxChannelMask,
                                                           const uint32_t                           configSelectMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrCtrlConfig, cleanup);

    /* Check that if requested Tx Channel valid*/
    if (((cfrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (cfrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /* Check that if requested config set is valid*/
    if (((configSelectMask & (~(uint32_t)ADI_ADRV904X_CFR_CFGALL)) != 0U) ||
         (configSelectMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configSelectMask,
                               "Invalid configuration selected. Valid values are any combinations of CFG0/1");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_CfrThresholdSet(device, 
                                                  &cfrCtrlConfig->cfrThreshold,
                                                  cfrTxChannelMask,
                                                  configSelectMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CFR thresholds");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_CfrEnableSet(device,
                                               &cfrCtrlConfig->cfrEngineEnable[0],
                                               &cfrCtrlConfig->cfrEngineBypass[0],
                                               ADI_ADRV904X_NUM_OF_CFR_ENGINES,
                                               cfrTxChannelMask,
                                               configSelectMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CFR engine/bypass configuration");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrConfigGet(adi_adrv904x_Device_t* const     device,
                                                           adi_adrv904x_CfrConfig_t* const  cfrCtrlConfig,
                                                           const uint32_t                   cfrTxChannelSel,
                                                           const uint32_t                   configSelect)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrCtrlConfig, cleanup);

    recoveryAction = adi_adrv904x_CfrThresholdGet(device, 
                                                  &cfrCtrlConfig->cfrThreshold,
                                                  cfrTxChannelSel,
                                                  configSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading CFR thresholds");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_CfrEnableGet(device,
                                               &cfrCtrlConfig->cfrEngineEnable[0],
                                               &cfrCtrlConfig->cfrEngineBypass[0],
                                               ADI_ADRV904X_NUM_OF_CFR_ENGINES,
                                               cfrTxChannelSel,
                                               configSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading CFR engine/bypass configuration");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrActiveCfgSet(adi_adrv904x_Device_t* const      device,
                                                              const uint32_t                    cfrTxChannelMask,
                                                              const adi_adrv904x_CfrConfigSel_e configSelect,
                                                              const uint8_t                     immediateUpdate)
{
        static const uint32_t txScratchPadAddr[] = { (uint32_t)ADRV904X_ADDR_TX0_STREAM_SCRATCH0,
                                                 (uint32_t)ADRV904X_ADDR_TX1_STREAM_SCRATCH0,
                                                 (uint32_t)ADRV904X_ADDR_TX2_STREAM_SCRATCH0,
                                                 (uint32_t)ADRV904X_ADDR_TX3_STREAM_SCRATCH0,
                                                 (uint32_t)ADRV904X_ADDR_TX4_STREAM_SCRATCH0,
                                                 (uint32_t)ADRV904X_ADDR_TX5_STREAM_SCRATCH0,
                                                 (uint32_t)ADRV904X_ADDR_TX6_STREAM_SCRATCH0,
                                                 (uint32_t)ADRV904X_ADDR_TX7_STREAM_SCRATCH0 };

     static const uint32_t CFR_MODE_CHANGE_REQ = 0x00001000U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;

    uint32_t chanIdx     = 0U;
    uint32_t chanSel     = 0U;
    uint8_t activeCfgSet = 0U;
    uint32_t txScratchValue = 0U;
    uint8_t actTxChannelMask = 0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /*Check that if requested Tx Channel valid*/
    if (((cfrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (cfrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /*Check that if requested config set is valid*/
    if ((configSelect != ADI_ADRV904X_CFR_CFG0) &&
        (configSelect != ADI_ADRV904X_CFR_CFG1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configSelect,
                               "Invalid configuration selected. Valid values are CFG0/1");
        goto cleanup;
    }

    /* Check if immediateUpdate parameter is valid */
    if (immediateUpdate > 2U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               immediateUpdate,
                               "immediateUpdate value is invalid. Valid values 0-2");
        goto cleanup;
    }

    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(cfrTxChannelMask, chanSel))
        {
            recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txCfrBaseAddr);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine CFR base address");
                goto cleanup;
            }

            /* Read active configuration */
            recoveryAction = adrv904x_TxCfrReg_CfrCtrlActiveSet_BfGet(device,
                                                                      NULL,
                                                                      txCfrBaseAddr,
                                                                      &activeCfgSet);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading active config set");
                goto cleanup;
            }

            if (activeCfgSet != ((uint8_t)configSelect - 1))
            {
                if (immediateUpdate == 2U)
                {
                    /* Read the scratchpad register */
                    recoveryAction = adi_adrv904x_Register32Read(device,
                                                                 NULL,
                                                                 txScratchPadAddr[chanIdx],
                                                                 &txScratchValue,
                                                                 0xFFFFFFFF);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register");
                        goto cleanup;
                    }

                    if (txScratchValue & CFR_MODE_CHANGE_REQ)
                    {
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Another request already active");
                        goto cleanup;
                    }

                    /* Update Tx channel bitmask */
                    actTxChannelMask |= (uint8_t)chanSel;
                }
                else if (immediateUpdate == 1U)
                {
                    /* Assert config select toggle bit to switch to requested configuration */
                    adrv904x_TxCfrReg_CfrCtrlChSet_BfSet(device,
                                                         NULL,
                                                         txCfrBaseAddr,
                                                         ADI_TRUE);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing CFR ch set bit");
                        goto cleanup;
                    }
                }
                else
                {
                    /* TODO: Trigger stream to change active configuration on next carrier configuration. We may not need to trigger this stream 
                     * based on the solution for TRDIG-2870 */
                }
            }
        }
    }

    if (immediateUpdate == 2U)
    {
        /* Set CFR active config set change request */
        recoveryAction = adrv904x_TxStreamTrigger(device, actTxChannelMask, (uint8_t)ADRV904X_STREAM_TX_SET_CFR_CFG_SET_CHANGE_REQ);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while triggering TX stream");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrActiveCfgGet(adi_adrv904x_Device_t* const          device,
                                                              const adi_adrv904x_TxChannels_e       txChannelSel,
                                                              adi_adrv904x_CfrConfigSel_e* const    configSelectRead)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    uint8_t tmpReadbackVal = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configSelectRead, cleanup);

    recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txCfrBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine CFR base address");
        goto cleanup;
    }

    /* Read active configuration */
    recoveryAction = adrv904x_TxCfrReg_CfrCtrlActiveSet_BfGet(device,
                                                              NULL,
                                                              txCfrBaseAddr,
                                                              &tmpReadbackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading active config set");
        goto cleanup;
    }

    *configSelectRead = (adi_adrv904x_CfrConfigSel_e)(tmpReadbackVal + 1U);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrActivePulseSet(adi_adrv904x_Device_t* const     device,
                                                                const uint32_t                   cfrTxChannelMask,
                                                                const adi_adrv904x_CfrPulseSel_e pulseSelect,
                                                                const uint8_t                    immediateUpdate)
{
        adi_adrv904x_ErrAction_e recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;

    uint32_t chanIdx      = 0U;
    uint32_t chanSel      = 0U;
    uint8_t activePulse   = 0U;
    uint8_t pulseSelBfVal = 0U;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /*Check that if requested Tx Channel valid*/
    if (((cfrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (cfrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /*Check that if requested config set is valid*/
    if ((pulseSelect != ADI_ADRV904X_CFR_PULSE0) &&
        (pulseSelect != ADI_ADRV904X_CFR_PULSE1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pulseSelect,
                               "Invalid pulse selected. Valid values are Pulse0/1");
        goto cleanup;
    }

    /* Check if immediateUpdate parameter is valid */
    if (immediateUpdate > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               immediateUpdate,
                               "immediateUpdate value is invalid. Valid values 0-1");
        goto cleanup;
    }

    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(cfrTxChannelMask, chanSel))
        {
            recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txCfrBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine CFR base address");
                goto cleanup;
            }

            /* Read active pulse */
            recoveryAction = adrv904x_TxCfrReg_CfrCtrlPulSel_BfGet(device,
                                                                   NULL,
                                                                   txCfrBaseAddr,
                                                                   &activePulse);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading active pulse");
                goto cleanup;
            }
            
            /* Pulse select bf value is 0 and 1*/
            pulseSelBfVal = (uint8_t)pulseSelect - 1U; 
            if (activePulse != (uint8_t)pulseSelBfVal)
            {
                if (immediateUpdate == ADI_TRUE) 
                {
                    /* Write the pulse selection immediately */
                    adrv904x_TxCfrReg_CfrCtrlPulSel_BfSet(device,
                                                         NULL,
                                                         txCfrBaseAddr,
                                                         pulseSelBfVal);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing CFR pulse set bit");
                        goto cleanup;
                    }
                }
                else
                {
                    /* TODO: Trigger stream to change pulse on next carrier configuration. We may not need to trigger this stream 
                     *      based on the solution for TRDIG-2870 */
                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrActivePulseGet(adi_adrv904x_Device_t* const        device,
                                                                const adi_adrv904x_TxChannels_e     txChannelSel,
                                                                adi_adrv904x_CfrPulseSel_e* const   pulseSelectRead)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    uint8_t tmpReadbackVal = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pulseSelectRead, cleanup);

    recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txCfrBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine CFR base address");
        goto cleanup;
    }

    /* Read active configuration */
    recoveryAction = adrv904x_TxCfrReg_CfrCtrlPulSel_BfGet(device,
                                                              NULL,
                                                              txCfrBaseAddr,
                                                              &tmpReadbackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading active pulse");
        goto cleanup;
    }

    *pulseSelectRead = (adi_adrv904x_CfrPulseSel_e)(tmpReadbackVal + 1U);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrStatisticsCfgSet(adi_adrv904x_Device_t* const                  device,
                                                                  const adi_adrv904x_CfrStatisticsCfg_t* const  cfrStatisticsConfig,
                                                                  const uint32_t                                cfrTxChannelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    static const uint8_t WINDOW_SIZE_MAX = 15U;

    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    uint8_t engineIdx = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrStatisticsConfig, cleanup);

    /*Check that if requested Tx Channel valid*/
    if (((cfrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (cfrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /* Check if window enable parameter is valid */
    if (cfrStatisticsConfig->windowEn > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrStatisticsConfig->windowEn,
                               "windowEn value is invalid. Valid values 0-1");
        goto cleanup;
    }
    
    /* Check if window size parameter is valid */
    if (cfrStatisticsConfig->windowSize > WINDOW_SIZE_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrStatisticsConfig->windowSize,
                               "windowSize value is invalid. Valid range [0-15]");
        goto cleanup;
    }
    
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(cfrTxChannelMask, chanSel))
        {
            recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txCfrBaseAddr);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine CFR base address");
                goto cleanup;
            }

            for (engineIdx = 0U; engineIdx < ADI_ADRV904X_NUM_OF_CFR_ENGINES; ++engineIdx) 
            {
                /* Write window enable bit */
                recoveryAction = adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfSet(device,
                                                                              NULL,
                                                                              txCfrBaseAddr,
                                                                              engineIdx,
                                                                              cfrStatisticsConfig->windowEn);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing statistics window enable");
                    goto cleanup;
                }
                
                /* Write window size bit */
                recoveryAction = adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfSet(device,
                                                                              NULL,
                                                                              txCfrBaseAddr,
                                                                              engineIdx,
                                                                              cfrStatisticsConfig->windowSize);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing statistics window size");
                    goto cleanup;
                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrStatisticsCfgGet(adi_adrv904x_Device_t* const              device,
                                                                  adi_adrv904x_CfrStatisticsCfg_t* const    cfrStatisticsConfig,
                                                                  const uint32_t                            cfrTxChannelSel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    uint8_t tmpReadbackVal = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrStatisticsConfig, cleanup);

    recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(cfrTxChannelSel), &txCfrBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cfrTxChannelSel, "Invalid Tx Channel used to determine CFR base address");
        goto cleanup;
    }

    /* Read window enable from engine 0, since we configure the same setting to all engines */
    recoveryAction = adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfGet(device,
                                                                  NULL,
                                                                  txCfrBaseAddr,
                                                                  0U,
                                                                  &tmpReadbackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading statistics window enable");
        goto cleanup;
    }
    cfrStatisticsConfig->windowEn = tmpReadbackVal;

    /* Read window size from engine 0, since we configure the same setting to all engines */
    recoveryAction = adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfGet(device,
                                                                  NULL,
                                                                  txCfrBaseAddr,
                                                                  0U,
                                                                  &tmpReadbackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading statistics window size");
        goto cleanup;
    }

    cfrStatisticsConfig->windowSize = tmpReadbackVal;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrStatisticsGet(adi_adrv904x_Device_t* const         device,
                                                               adi_adrv904x_CfrStatistics_t* const  cfrStatistics,
                                                               const adi_adrv904x_TxChannels_e      cfrTxChannelSel,
                                                               const uint8_t                        prevWindow,
                                                               const uint8_t                        clearStats)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    uint32_t engineIdx = 0U;

    uint16_t* const cfrPeaksDetected[ADI_ADRV904X_NUM_OF_CFR_ENGINES] = { &cfrStatistics->cfrEngine1PeaksDetected, &cfrStatistics->cfrEngine2PeaksDetected, &cfrStatistics->cfrEngine3PeaksDetected };
    uint8_t* const cfrPeaksSkippedWFifo[ADI_ADRV904X_NUM_OF_CFR_ENGINES] = { &cfrStatistics->cfrEngine1PeaksSkippedWFifo, &cfrStatistics->cfrEngine2PeaksSkippedWFifo, &cfrStatistics->cfrEngine3PeaksSkippedWFifo };
    uint8_t* const cfrPeaksSkippedPFifo[ADI_ADRV904X_NUM_OF_CFR_ENGINES] = { &cfrStatistics->cfrEngine1PeaksSkippedPFifo, &cfrStatistics->cfrEngine2PeaksSkippedPFifo, &cfrStatistics->cfrEngine3PeaksSkippedPFifo };
    uint16_t* const cfrPeaksSkippedCPC[ADI_ADRV904X_NUM_OF_CFR_ENGINES] = { &cfrStatistics->cfrEngine1PeaksSkippedCPC, &cfrStatistics->cfrEngine2PeaksSkippedCPC, &cfrStatistics->cfrEngine3PeaksSkippedCPC };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrStatistics, cleanup);

    /* Check if prevWindow parameter is valid */
    if (prevWindow > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               prevWindow,
                               "prevWindow value is invalid. Valid values 0-1");
        goto cleanup;
    }
    
    /* Check if clearStats parameter is valid */
    if (clearStats > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clearStats,
                               "clearStats value is invalid. Valid values 0-1");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(cfrTxChannelSel), &txCfrBaseAddr);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cfrTxChannelSel, "Invalid Tx Channel used to determine CFR base address");
        goto cleanup;
    }

    for (engineIdx = 0U; engineIdx < ADI_ADRV904X_NUM_OF_CFR_ENGINES; ++engineIdx) 
    {
        recoveryAction = adrv904x_TxCfrReg_CfrDbgPkReadStat_BfSet(device,
                                                                  NULL,
                                                                  txCfrBaseAddr,
                                                                  engineIdx,
                                                                  ADI_TRUE);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while updating CFR statistics");
            goto cleanup;
        }
        
        if (prevWindow == ADI_TRUE)
        {
            /* Read number of peaks detected */
            recoveryAction = adrv904x_TxCfrReg_CfrDbgPkFoundPrvWin_BfGet(device,
                                                                         NULL,
                                                                         txCfrBaseAddr,
                                                                         engineIdx,
                                                                         cfrPeaksDetected[engineIdx]);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peaks detected");
                goto cleanup;
            }

            /* Read number of peaks skipped at weight FIFO */
            recoveryAction = adrv904x_TxCfrReg_CfrDbgPkLostWfPrvWin_BfGet(device,
                                                                          NULL,
                                                                          txCfrBaseAddr,
                                                                          engineIdx,
                                                                          cfrPeaksSkippedWFifo[engineIdx]);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peaks skipped at weight FIFO");
                goto cleanup;
            }
            
            /* Read number of peaks skipped at peak FIFO */
            recoveryAction = adrv904x_TxCfrReg_CfrDbgPkLostPfPrvWin_BfGet(device,
                                                                          NULL,
                                                                          txCfrBaseAddr,
                                                                          engineIdx,
                                                                          cfrPeaksSkippedPFifo[engineIdx]);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peaks skipped at peak FIFO");
                goto cleanup;
            }

            /* Read number of peaks skipped at CPC */
            recoveryAction = adrv904x_TxCfrReg_CfrDbgPkLostCpcPrvWin_BfGet(device,
                                                                          NULL,
                                                                          txCfrBaseAddr,
                                                                          engineIdx,
                                                                          cfrPeaksSkippedCPC[engineIdx]);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peaks skipped at CPC");
                goto cleanup;
            }
        }
        else
        {
            /* Read number of peaks detected */
            recoveryAction = adrv904x_TxCfrReg_CfrDbgPkFoundCurWin_BfGet(device,
                                                                         NULL,
                                                                         txCfrBaseAddr,
                                                                         engineIdx,
                                                                         cfrPeaksDetected[engineIdx]);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peaks detected");
                goto cleanup;
            }

            /* Read number of peaks skipped at weight FIFO */
            recoveryAction = adrv904x_TxCfrReg_CfrDbgPkLostWfCurWin_BfGet(device,
                                                                          NULL,
                                                                          txCfrBaseAddr,
                                                                          engineIdx,
                                                                          cfrPeaksSkippedWFifo[engineIdx]);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peaks skipped at weight FIFO");
                goto cleanup;
            }
            
            /* Read number of peaks skipped at peak FIFO */
            recoveryAction = adrv904x_TxCfrReg_CfrDbgPkLostPfCurWin_BfGet(device,
                                                                          NULL,
                                                                          txCfrBaseAddr,
                                                                          engineIdx,
                                                                          cfrPeaksSkippedPFifo[engineIdx]);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peaks skipped at peak FIFO");
                goto cleanup;
            }

            /* Read number of peaks skipped at CPC */
            recoveryAction = adrv904x_TxCfrReg_CfrDbgPkLostCpcCurWin_BfGet(device,
                                                                           NULL,
                                                                           txCfrBaseAddr,
                                                                           engineIdx,
                                                                           cfrPeaksSkippedCPC[engineIdx]);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peaks skipped at CPC");
                goto cleanup;
            }
        }
    }
    
    if (clearStats == ADI_TRUE)
    {
        /* Clear CFR statistics for all engines*/
        for (engineIdx = 0U; engineIdx < ADI_ADRV904X_NUM_OF_CFR_ENGINES; ++engineIdx) 
        {
            recoveryAction = adrv904x_TxCfrReg_CfrDbgPkClrStat_BfSet(device,
                                                                     NULL,
                                                                     txCfrBaseAddr,
                                                                     engineIdx,
                                                                     ADI_TRUE);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing CFR statistics");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

static adi_adrv904x_ErrAction_e readCfrPulseRam(adi_adrv904x_Device_t* const    device,
                                                const uint8_t                   interpolationRateMult,
                                                int16_t* const                  coeffRealHalfPulse,
                                                int16_t* const                  coeffImagHalfPulse,
                                                const uint16_t                  numCoeffs,
                                                const uint32_t                  baseAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint32_t rowMinSampleIdx    = 0U;
    uint32_t rowMaxSampleIdx    = 0U;
    uint32_t rowIdx             = 0U;
    uint32_t rowToRead[1]       = { 0U };
    uint32_t sampleCounter      = 0U;
    uint32_t addressToRead      = 0U;
    uint32_t rowCounter         = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, coeffRealHalfPulse);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, coeffImagHalfPulse);

    for (rowIdx = 0; rowIdx < ADI_ADRV904X_CFR_PULSE_RAM_NUM_OF_ROWS; rowIdx++) 
    {
        rowMaxSampleIdx = rowMinSampleIdx + interpolationRateMult * 3;

        if (numCoeffs > rowMaxSampleIdx) 
        {
            rowCounter = 0U;
            for (sampleCounter = rowMinSampleIdx; 
                 sampleCounter < (rowMinSampleIdx + (ADI_ADRV904X_CFR_PULSE_RAM_ROW_SAMPLE_SIZE * interpolationRateMult));
                 sampleCounter += interpolationRateMult) 
            {
                addressToRead = baseAddr + rowIdx * ADI_ADRV904X_CFR_PULSE_RAM_ROW_SIZE + rowCounter;

                recoveryAction = adi_adrv904x_Register32Read(device,
                                                             NULL,
                                                             addressToRead,
                                                             (uint32_t*)&rowToRead[0U],
                                                             0xFFFFFFFF);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Read Issue");
                    return recoveryAction;
                }

                coeffRealHalfPulse[sampleCounter] =  (int16_t)((rowToRead[0] >> 0U) & 0x0000FFFFU);
                coeffImagHalfPulse[sampleCounter] =  (int16_t)((rowToRead[0] >> 16U) & 0x0000FFFFU);
                rowCounter += ADI_ADRV904X_CFR_PULSE_RAM_ROW_SAMPLE_SIZE;
            }
        }

        rowMinSampleIdx = (rowMaxSampleIdx + interpolationRateMult) % 1023U;
    }

    return recoveryAction;
}
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrCorrectionPulseWrite(adi_adrv904x_Device_t* const              device,
                                                                      const uint32_t                            cfrTxChannelMask,
                                                                      const adi_adrv904x_CfrCorrectionPulse_t   cfrCorrectionPulses[],
                                                                      const uint32_t                            numCorrectionPulses)
{
        adi_adrv904x_ErrAction_e recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    uint32_t pulseIdx         = 0U;
    uint32_t chanIdx          = 0U;
    uint32_t chanSel          = 0U;
    uint8_t interpolationRate = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrCorrectionPulses, cleanup);
    
    /*Check that if requested Tx Channel valid*/
    if (((cfrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (cfrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /* Check that if pulse data is valid */
    for (pulseIdx = 0U; pulseIdx < numCorrectionPulses; ++pulseIdx)
    {
        recoveryAction =  adrv904x_CfrPulseWriteRangeCheck(device, &cfrCorrectionPulses[pulseIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CFR pulse range check failed");
            goto cleanup;
        }
    }

    for (pulseIdx = 0U; pulseIdx < numCorrectionPulses; ++pulseIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(cfrTxChannelMask, chanSel))
            {
                recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txCfrBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine CFR base address");
                    goto cleanup;
                }

                /* Check if we're writing to the pulse which is being used actively */
                recoveryAction = adrv904x_CfrActivePulseUpdateCheck(device,
                                                                    (adi_adrv904x_TxChannels_e)(chanSel),
                                                                    cfrCorrectionPulses[pulseIdx].pulseSelectMask);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cfrCorrectionPulses[pulseIdx].pulseSelectMask, "Requested pulse is actively being used by CFR engine");
                    goto cleanup;
                }

                /* Write Pulse 0 */
                if (ADRV904X_BF_EQUAL(cfrCorrectionPulses[pulseIdx].pulseSelectMask, ADI_ADRV904X_CFR_PULSE0))
                {
                    recoveryAction = adrv904x_TxCfrReg_Cfr0PulInterp_BfGet(device,
                                                                           NULL,
                                                                           txCfrBaseAddr,
                                                                           &interpolationRate);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading interpolation rate");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_CfrCorrectionPulseWrite(device,
                                                                      ADI_ADRV904X_CFR_PULSE0,
                                                                      chanIdx,
                                                                      interpolationRate,
                                                                      &cfrCorrectionPulses[pulseIdx]);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing the pulse 0");
                        goto cleanup;
                    }
                }

                /* Write Pulse 1*/
                if (ADRV904X_BF_EQUAL(cfrCorrectionPulses[pulseIdx].pulseSelectMask, ADI_ADRV904X_CFR_PULSE1))
                {
                    recoveryAction = adrv904x_TxCfrReg_Cfr1PulInterp_BfGet(device,
                                                                           NULL,
                                                                           txCfrBaseAddr,
                                                                           &interpolationRate);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading interpolation rate");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_CfrCorrectionPulseWrite(device,
                                                                      ADI_ADRV904X_CFR_PULSE1,
                                                                      chanIdx,
                                                                      interpolationRate,
                                                                      &cfrCorrectionPulses[pulseIdx]);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing the pulse 1");
                        goto cleanup;
                    }
                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrCorrectionPulseRead(adi_adrv904x_Device_t* const               device,
                                                                     const adi_adrv904x_TxChannels_e            cfrTxChannelSel,
                                                                     adi_adrv904x_CfrCorrectionPulse_t* const   cfrCorrectionPulse,
                                                                     const adi_adrv904x_CfrPulseSel_e           pulseSelect)
{
        adi_adrv904x_ErrAction_e recoveryAction                  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr              = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    uint32_t pulseBaseAddresses[ADI_ADRV904X_MAX_TXCHANNELS] = {ADI_ADVRV904X_TX0_PULSE_RAM_BASEADDR,
                                                                ADI_ADVRV904X_TX1_PULSE_RAM_BASEADDR,
                                                                ADI_ADVRV904X_TX2_PULSE_RAM_BASEADDR,
                                                                ADI_ADVRV904X_TX3_PULSE_RAM_BASEADDR,
                                                                ADI_ADVRV904X_TX4_PULSE_RAM_BASEADDR,
                                                                ADI_ADVRV904X_TX5_PULSE_RAM_BASEADDR,
                                                                ADI_ADVRV904X_TX6_PULSE_RAM_BASEADDR,
                                                                ADI_ADVRV904X_TX7_PULSE_RAM_BASEADDR};

    uint8_t interpolationRate     = 0U;
    uint8_t interpolationRateMult = 0U;
    uint32_t baseAddrToRead       = 0U;
    uint32_t addrOffset           = 0U;
    uint16_t halfPulseLength      = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfrCorrectionPulse, cleanup);

    ADI_LIBRARY_MEMSET(&cfrCorrectionPulse->coeffImagHalfPulse, 0, (sizeof(int16_t) * ADI_ADRV904X_MAX_CFR_CORRECTION_PULSE_LEN));
    ADI_LIBRARY_MEMSET(&cfrCorrectionPulse->coeffRealHalfPulse, 0, (sizeof(int16_t) * ADI_ADRV904X_MAX_CFR_CORRECTION_PULSE_LEN));

    recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, cfrTxChannelSel, &txCfrBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cfrTxChannelSel, "Invalid Tx Channel used to determine CFR base address");
        goto cleanup;
    }

    switch (cfrTxChannelSel) 
    {
        case ADI_ADRV904X_TX0:
            baseAddrToRead = pulseBaseAddresses[0];
        break;
        case ADI_ADRV904X_TX1:
            baseAddrToRead = pulseBaseAddresses[1];
            break;
        case ADI_ADRV904X_TX2:
            baseAddrToRead = pulseBaseAddresses[2];
            break;
        case ADI_ADRV904X_TX3:
            baseAddrToRead = pulseBaseAddresses[3];
            break;
        case ADI_ADRV904X_TX4:
            baseAddrToRead = pulseBaseAddresses[4];
            break;
        case ADI_ADRV904X_TX5:
            baseAddrToRead = pulseBaseAddresses[5];
            break;
        case ADI_ADRV904X_TX6:
            baseAddrToRead = pulseBaseAddresses[6];
            break;
        case ADI_ADRV904X_TX7:
            baseAddrToRead = pulseBaseAddresses[7];
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, cfrTxChannelSel, "Invalid Tx Channel used to determine pulse base address");
            goto cleanup;
            break;
    }
    
    if ((pulseSelect != ADI_ADRV904X_CFR_PULSE0) &&
        (pulseSelect != ADI_ADRV904X_CFR_PULSE1)) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pulseSelect, "Invalid pulseSelect. Valid values are PULSE0/1");
        goto cleanup;
    }

    /* Read Half pulse length*/
    if (pulseSelect == ADI_ADRV904X_CFR_PULSE0)
    {
        recoveryAction = adrv904x_TxCfrReg_Cfr0PulHpl_BfGet(device,
                                                            NULL,
                                                            txCfrBaseAddr,
                                                            &halfPulseLength);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading half pulse length");
            goto cleanup;
        }

        recoveryAction = adrv904x_TxCfrReg_Cfr0PulInterp_BfGet(device,
                                                               NULL,
                                                               txCfrBaseAddr,
                                                               &interpolationRate);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading interpolation rate");
            goto cleanup;
        }
    }
    else 
    {
        recoveryAction = adrv904x_TxCfrReg_Cfr1PulHpl_BfGet(device,
                                                            NULL,
                                                            txCfrBaseAddr,
                                                            &halfPulseLength);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading half pulse length");
            goto cleanup;
        }

        recoveryAction = adrv904x_TxCfrReg_Cfr1PulInterp_BfGet(device,
                                                               NULL,
                                                               txCfrBaseAddr,
                                                               &interpolationRate);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading interpolation rate");
            goto cleanup;
        }
    }

    /* Interpolation rate in HW  0=1x // 1=2x // 2=4x*/
    interpolationRateMult = 1 << interpolationRate;

    /* Set RAM0 bank as shadow RAM */
    recoveryAction = adrv904x_TxCfrReg_CfrRamShadowRamSel_BfSet(device,
                                                                NULL,
                                                                txCfrBaseAddr,
                                                                0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writin Shadow RAM bf");
        goto cleanup;
    }

    /* Programmed half pulse length = Number of coefficents / interpolationRate - 1*/
    halfPulseLength = (halfPulseLength + 1U) * interpolationRateMult;
    if (halfPulseLength != cfrCorrectionPulse->numCoeffs) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, halfPulseLength, "Half pulse length doesn't match the num of coefficients user requested");
        goto cleanup;
    }

    addrOffset = (ADI_ADRV904X_CFR_PULSE_RAM_SIZE * 2U) + (ADI_ADRV904X_CFR_PULSE_RAM_SIZE * ((uint32_t)pulseSelect - 1U));
    recoveryAction = readCfrPulseRam(device,
                                     interpolationRateMult,
                                     cfrCorrectionPulse->coeffRealHalfPulse,
                                     cfrCorrectionPulse->coeffImagHalfPulse,
                                     cfrCorrectionPulse->numCoeffs,
                                     (baseAddrToRead + addrOffset));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CFR pulse read failed");
        goto cleanup;
    }

    cfrCorrectionPulse->pulseSelectMask = (uint8_t)pulseSelect;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



static adi_adrv904x_ErrAction_e adrv904x_CfrActiveConfigUpdateCheck(adi_adrv904x_Device_t* const    device,
                                                                    const adi_adrv904x_TxChannels_e cfrTxChannelSel,
                                                                    const uint32_t                  configSelectMask) 
{
    adi_adrv904x_ErrAction_e recoveryAction       = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_CfrConfigSel_e activeCfgReadBack = ADI_ADRV904X_CFR_CFG0;
    adi_adrv904x_CfrConfig_t activeCfrConfig;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_LIBRARY_MEMSET(&activeCfrConfig, 0, sizeof(adi_adrv904x_CfrConfig_t));

    /* Readback the active config for this channel */
    recoveryAction = adi_adrv904x_CfrActiveCfgGet(device,
                                                  cfrTxChannelSel,
                                                  &activeCfgReadBack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, activeCfgReadBack, "Error while reading active config");
        return recoveryAction;
    }

    /* Read the current CFR config */
    recoveryAction = adi_adrv904x_CfrEnableGet(device,
                                               &activeCfrConfig.cfrEngineEnable[0],
                                               &activeCfrConfig.cfrEngineBypass[0],
                                               ADI_ADRV904X_NUM_OF_CFR_ENGINES,
                                               cfrTxChannelSel,
                                               (uint32_t)activeCfgReadBack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, activeCfgReadBack, "Error while reading CFR config");
        return recoveryAction;
    }

    if ((activeCfrConfig.cfrEngineEnable[0] == ADI_TRUE) ||
        (activeCfrConfig.cfrEngineEnable[1] == ADI_TRUE) ||
        (activeCfrConfig.cfrEngineEnable[2] == ADI_TRUE))
    {
        /* Report error if any of the CFR engines are enabled and user is writing to active config */
        if (ADRV904X_BF_EQUAL(configSelectMask, activeCfgReadBack))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   configSelectMask,
                                   "Selected config is actively being used by CFR block");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_CfrActivePulseUpdateCheck(adi_adrv904x_Device_t* const     device,
                                                                   const adi_adrv904x_TxChannels_e  cfrTxChannelSel,
                                                                   const uint32_t                   pulseSelectMask)
{
    adi_adrv904x_ErrAction_e recoveryAction        = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_CfrPulseSel_e activePulseReadBack = ADI_ADRV904X_CFR_PULSE0;
    adi_adrv904x_CfrConfigSel_e activeCfgReadBack  = ADI_ADRV904X_CFR_CFG0;
    adi_adrv904x_CfrConfig_t activeCfrConfig;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_LIBRARY_MEMSET(&activeCfrConfig, 0, sizeof(adi_adrv904x_CfrConfig_t));

    /* Readback the active pulse for this channel */
    recoveryAction = adi_adrv904x_CfrActivePulseGet(device,
                                                    cfrTxChannelSel,
                                                    &activePulseReadBack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, activePulseReadBack, "Error while reading active pulse");
        return recoveryAction;
    }

    /* Readback the active config for this channel */
    recoveryAction = adi_adrv904x_CfrActiveCfgGet(device,
                                                  cfrTxChannelSel,
                                                  &activeCfgReadBack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, activeCfgReadBack, "Error while reading active config");
        return recoveryAction;
    }

    /* Read the current CFR config */
    recoveryAction = adi_adrv904x_CfrEnableGet(device,
                                               &activeCfrConfig.cfrEngineEnable[0],
                                               &activeCfrConfig.cfrEngineBypass[0],
                                               ADI_ADRV904X_NUM_OF_CFR_ENGINES,
                                               cfrTxChannelSel,
                                               (uint32_t)activeCfgReadBack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, activeCfgReadBack, "Error while reading CFR config");
        return recoveryAction;
    }

    if ((activeCfrConfig.cfrEngineEnable[0] == ADI_TRUE) ||
        (activeCfrConfig.cfrEngineEnable[1] == ADI_TRUE) ||
        (activeCfrConfig.cfrEngineEnable[2] == ADI_TRUE))
    {
        /* Report error if any of the CFR engines are enabled and user is writing to active pulse */
        if (ADRV904X_BF_EQUAL(pulseSelectMask, activePulseReadBack))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   pulseSelectMask,
                                   "Selected pulse is actively being used by CFR block");
            return recoveryAction;
        }
    }

    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrControlPeakDurationSet(adi_adrv904x_Device_t* const     device,
                                                                        const uint32_t                   cfrTxChannelMask,
                                                                        const uint32_t                   configSelectMask,
                                                                        const uint8_t                    samplesCount)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint8_t txChanIdx = 0U;
    uint8_t txChanSel = 0U;
    uint8_t NUMBER_OF_ENGINES = 3U;
    uint8_t engineIdx = 0U;
    adrv904x_BfTxCfrRegChanAddr_e txCfrRegAddr[ADI_ADRV904X_MAX_TXCHANNELS] =
    {
        ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG,
        ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG,
        ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG,
        ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG,
        ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG,
        ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG,
        ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG,
        ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Check that if requested Tx Channel valid */
    if (((cfrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (cfrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /* Check that if requested config set is valid */
    if (((configSelectMask & (~(uint32_t)ADI_ADRV904X_CFR_CFGALL)) != 0U) ||
         (configSelectMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configSelectMask,
                               "Invalid configuration selected. Valid values are any combinations of CFG0/1");
        goto cleanup;
    }

    for (txChanIdx = 0U; txChanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++txChanIdx)
    {
        txChanSel = 1U << txChanIdx;

        /* Skipping unselected channels */
        if ((txChanSel & cfrTxChannelMask) == 0U)
        {
            continue;
        }

        /* Verify that the selected CFG is not currently active */
        recoveryAction = adrv904x_CfrActiveConfigUpdateCheck(device,
                                                             (adi_adrv904x_TxChannels_e)(txChanSel),
                                                             configSelectMask);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, configSelectMask, "Requested configuration is actively being used by CFR engine");
            goto cleanup;
        }

        for (engineIdx = 0; engineIdx < NUMBER_OF_ENGINES; engineIdx++)
        {
            if ((configSelectMask & ADI_ADRV904X_CFR_CFG0) != 0u)
            {
                recoveryAction = adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfSet(device,
                                                                       NULL,
                                                                       txCfrRegAddr[txChanIdx],
                                                                       engineIdx,
                                                                       samplesCount);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, engineIdx, "Error while setting CFR 0 Peak Duration for the selected CFR engine");
                    goto cleanup;
                }
            }
            else if ((configSelectMask & ADI_ADRV904X_CFR_CFG1) != 0u)
            {
                recoveryAction = adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfSet(device,
                                                                       NULL,
                                                                       txCfrRegAddr[txChanIdx],
                                                                       engineIdx,
                                                                       samplesCount);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, engineIdx, "Error while setting CFR 1 Peak Duration for the selected CFR engine");
                    goto cleanup;
                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrControlPeakDurationGet(adi_adrv904x_Device_t* const        device,
                                                                        const adi_adrv904x_TxChannels_e     txChannelSel,
                                                                        const adi_adrv904x_CfrConfigSel_e   configSelect,
                                                                        uint8_t* const                      samplesCount)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrRegAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, samplesCount, cleanup);

    /* Check that if requested Tx Channel valid and get address */
    switch (txChannelSel)
    {
        case ADI_ADRV904X_TX0:
            txCfrRegAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
            break;
        case ADI_ADRV904X_TX1:
            txCfrRegAddr = ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG;
            break;
        case ADI_ADRV904X_TX2:
            txCfrRegAddr = ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG;
            break;
        case ADI_ADRV904X_TX3:
            txCfrRegAddr = ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG;
            break;
        case ADI_ADRV904X_TX4:
            txCfrRegAddr = ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG;
            break;
        case ADI_ADRV904X_TX5:
            txCfrRegAddr = ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG;
            break;
        case ADI_ADRV904X_TX6:
            txCfrRegAddr = ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG;
            break;
        case ADI_ADRV904X_TX7:
            txCfrRegAddr = ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   txChannelSel,
                                   "Invalid Tx channel is selected. Valid values are Tx0/1/2/3/4/5/6/7");
            goto cleanup;
            break;
    }

    /* Use engine 0 to read value since all of the engines configured the same value */
    switch (configSelect)
    {
        case ADI_ADRV904X_CFR_CFG0:
            recoveryAction = adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfGet(device,
                                                                   NULL,
                                                                   txCfrRegAddr,
                                                                   0u,
                                                                   samplesCount);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, configSelect, "Error while setting CFR Peak Duration for the selected CFG and CFR engine");
                goto cleanup;
            }
            break;

        case ADI_ADRV904X_CFR_CFG1:  
            recoveryAction = adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfGet(device,
                                                                   NULL,
                                                                   txCfrRegAddr,
                                                                   0u,
                                                                   samplesCount);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, configSelect, "Error while setting CFR Peak Duration for the selected CFG and CFR engine");
                goto cleanup;
            }
            break;

        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   configSelect,
                                   "Invalid CFG selected.");
            goto cleanup;
            break;
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrControlDelaySet(adi_adrv904x_Device_t* const device,
                                                                 const uint32_t               cfrTxChannelMask,
                                                                 const uint16_t               delay0,
                                                                 const uint16_t               delay1,
                                                                 const uint16_t               delay2)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    const uint16_t maxDelay = 1023;
    uint32_t txChanIdx = 0U;
    uint32_t txChanSel = 0U;
    uint32_t packedDelay = 0U;

    static const uint32_t txScratchPadAddr[] = { (uint32_t)ADRV904X_ADDR_TX0_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX1_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX2_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX3_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX4_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX5_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX6_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX7_STREAM_SCRATCH5 };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Check that if requested Tx Channel valid */
    if (((cfrTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (cfrTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /* Check that all 3 delay values are valid */
    if (delay0 > maxDelay)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               delay0,
                               "CFR delay is out of range. delay0");
        goto cleanup;
    }

    if (delay1 > maxDelay)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               delay1,
                               "CFR delay is out of range. delay1");
        goto cleanup;
    }

    if (delay2 > maxDelay)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               delay2,
                               "CFR delay is out of range. delay2");
        goto cleanup;
    }

    packedDelay  = (uint32_t) delay0;
    packedDelay |= (uint32_t) delay1 << 10u;
    packedDelay |= (uint32_t) delay2 << 20u;

    for (txChanIdx = 0U; txChanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++txChanIdx)
    {
        txChanSel = 1U << txChanIdx;

        if ((txChanSel & cfrTxChannelMask) != 0U)
        {
            /* Write to Tx stream proc scratch5 register (bits 29-0)
             * Note: Not using BfSet because it writes the 4 bytes individually.
             *       We want to write the 30 bit parameter at once.
             *       Otherwise, if TX fall stream runs during that time,
             *       the delays written to the CFR block may be incorrect.
             */
            recoveryAction = adi_adrv904x_Register32Write(device,
                                                          NULL,
                                                          txScratchPadAddr[txChanIdx],
                                                          packedDelay,
                                                          0x3FFFFFFFU);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       txScratchPadAddr[txChanIdx],
                                       "Register32Write failed");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrControlDelayGet(adi_adrv904x_Device_t* const       device,
                                                                 const adi_adrv904x_TxChannels_e    txChannelSel,
                                                                 uint16_t* const                    delay0,
                                                                 uint16_t* const                    delay1,
                                                                 uint16_t* const                    delay2)
{        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t txChanIdx = 0U;
    uint32_t txChanSel = 0U;
    uint32_t packedDelay = 0U;
    const uint32_t delayMask = 0x3FFU;

    static const uint32_t txScratchPadAddr[] = { (uint32_t)ADRV904X_ADDR_TX0_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX1_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX2_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX3_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX4_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX5_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX6_STREAM_SCRATCH5,
                                                 (uint32_t)ADRV904X_ADDR_TX7_STREAM_SCRATCH5 };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, delay0, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, delay1, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, delay2, cleanup);

    /* Check that if requested Tx Channel valid */
    if ((txChannelSel == 0U) ||
        ((txChannelSel & (txChannelSel - 1)) != 0u))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelSel,
                               "Invalid Tx channel is selected. Channel must be != 0 and only one channel allowed");
        goto cleanup;
    }

    for (txChanIdx = 0U; txChanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++txChanIdx)
    {
        txChanSel = 1U << txChanIdx;

        if ((txChanSel & txChannelSel) != 0U)
        {
            /* Write to Tx stream proc scratch5 register (bits 29-0)
             * Note: Not using BfSet because it writes the 4 bytes individually.
             *       We want to write the 30 bit parameter at once.
             *       Otherwise, if TX fall stream runs during that time,
             *       the delays written to the CFR block may be incorrect.
             */
            recoveryAction = adi_adrv904x_Register32Read(device,
                                                         NULL,
                                                         txScratchPadAddr[txChanIdx],
                                                         &packedDelay,
                                                         0xFFFFFFFF);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       txScratchPadAddr[txChanIdx],
                                       "Register32Read failed");
                goto cleanup;
            }

            *delay0 = (uint16_t)(packedDelay & delayMask);
            *delay1 = (uint16_t)((packedDelay >> 10) & delayMask);
            *delay2 = (uint16_t)((packedDelay >> 20) & delayMask);
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


