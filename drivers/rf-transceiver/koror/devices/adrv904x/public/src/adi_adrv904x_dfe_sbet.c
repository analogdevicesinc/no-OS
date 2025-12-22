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
#include "adi_adrv904x_dfe_sbet.h"
#include "adi_adrv904x_dfe_cfr.h"
#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_hal.h"

#include "../../private/include/adrv904x_radioctrl.h"
#include "../../private/include/adrv904x_tx.h"
#include "../../private/include/adrv904x_dfe_cfr.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"
#include "../../private/include/adrv904x_stream_proc_types.h"
#include "../../private/bf/adrv904x_bf_streamproc_channel_types.h"
#include "../../private/include/adrv904x_cpu_scratch_registers.h"

#include "../../private/bf/adrv904x_bf_core.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_DFE_SBET


static adi_adrv904x_ErrAction_e adrv904x_CfrActiveConfigUpdateCheck(adi_adrv904x_Device_t* const    device,
                                                                    const adi_adrv904x_TxChannels_e cfrTxChannelSel,
                                                                    uint32_t                        configSelectMask);





ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SbetCfrThresholdSet(adi_adrv904x_Device_t* const             device,
                                                                  const adi_adrv904x_CfrThrConfig_t* const cfrThrConfig,
                                                                  const uint32_t                           hardClipperThreshold,
                                                                  const uint32_t                           cfrTxChannelMask,
                                                                  const uint32_t                           cfrPowerIndex)
{
            adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    uint32_t configSelectMask = (uint32_t)ADI_ADRV904X_CFR_CFG1;

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

    /* Check that if DPD Model Index is valid*/
    if (cfrPowerIndex >= 2)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               cfrPowerIndex,
                               "Invalid Cfr Power Index selected. Valid values are 0 and 1");
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

            /* Set engine 1 or 2 Thresh Sq*/
            recoveryAction = adrv904x_CfrWritePeakThreshSq(device,
                                                           cfrThrConfig->cfrEngine0PeakThresholdScaler,
                                                           cfrThrConfig->cfrPeakThreshold,
                                                           txCfrBaseAddr,
                                                           configSelectMask,
                                                           cfrPowerIndex);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sq for engine 0");
                goto cleanup;
            }

            /* Set engine 1 or 2 Thresh Sqrt*/
            recoveryAction = adrv904x_CfrWritePeakThreshSqrt(device,
                                                             cfrThrConfig->cfrEngine0PeakThresholdScaler,
                                                             cfrThrConfig->cfrPeakThreshold,
                                                             txCfrBaseAddr,
                                                             configSelectMask,
                                                             cfrPowerIndex);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing threshold sqrt for engine 0");
                goto cleanup;
            }


            /* Set engine 1 or 2 Correction Threshold */
            recoveryAction = adrv904x_CfrWriteCorrectionThresh(device,
                                                               cfrThrConfig->cfrCorrectionThresholdScaler,
                                                               cfrThrConfig->cfrPeakThreshold,
                                                               txCfrBaseAddr,
                                                               configSelectMask,
                                                               cfrPowerIndex);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing correction threshold for engine 0");
                goto cleanup;
            }
            /* the pre-DPD hard clipper threshold is stored in CFR1_THR_SQ2 or CFR1_THR_SQRT2 */
            if (cfrPowerIndex == 0U)
            {
                recoveryAction = adrv904x_TxCfrReg_Cfr1ThrSq_BfSet(device,
                                                                   NULL,
                                                                   txCfrBaseAddr,
                                                                   2u,
                                                                   (uint32_t)hardClipperThreshold);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dpd Powerindex:0 hardClipperThreshold");
                    goto cleanup;
                }
            }
            else //if (cfrPowerIndex == 1U)
            {
                recoveryAction = adrv904x_TxCfrReg_Cfr1ThrSqrt_BfSet(device,
                                                                   NULL,
                                                                   txCfrBaseAddr,
                                                                   2u,
                                                                   (uint32_t)hardClipperThreshold);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dpd Powerindex:1 hardClipperThreshold");
                    goto cleanup;
                }
            }
        }
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SbetTxAttenSet(adi_adrv904x_Device_t* const device,
                                                             adi_adrv904x_SbetTxAtten_t * const configTx)
{
            adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
   
    uint16_t attenRegVal = 0U;
    uint32_t chanSel = 0U;
    uint32_t chanIdx = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configTx, cleanup);

    if ((configTx->txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                                recoveryAction,
                                configTx->txChannelMask,
                                "Invalid channel is selected for Sbet Tx config. Valid values are 0 or any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }


    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(configTx->txChannelMask, chanSel))
        {
            if (configTx->txAttenuation_mdB > ADRV904X_TX_ATTEN_VALUE_MILLI_DB_MAX)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        configTx->txAttenuation_mdB,
                                        "Invalid txAttenuation_mdB value");
                goto cleanup;
            }

            recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txCfrBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine CFR base address");
                goto cleanup;
            }

            /* Conversion from the requested atten level (milli-dB) to equivalent
             * TxAttenTable index is always done based on a step size of 0.05.
             * Other step sizes are not supported. */
            attenRegVal = (configTx->txAttenuation_mdB / ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P05);

            /* setting  Tx attenuation for sbet*/
            recoveryAction = adrv904x_TxCfrReg_Cfr1ThrCor_BfSet(device,
                                                                NULL,
                                                                txCfrBaseAddr,
                                                                2u,
                                                                (uint32_t)attenRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing tx attenuation");
                goto cleanup;
            }
        }
    }
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SbetTxAttenGet(adi_adrv904x_Device_t* const device,
                                                             adi_adrv904x_SbetTxAtten_t * const configTx)
{

            adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;
    uint32_t attenRegVal = 0U;
    uint32_t chanSel = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configTx, cleanup);

    if ((configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TXOFF) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX0) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX1) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX2) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX3) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX4) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX5) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX6) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configTx->txChannelMask,
                               "Invalid Tx channel is selected. User should select one channel or CHOFF only");
        goto cleanup;
    }

    if (configTx->txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF) 
    {
        configTx->txAttenuation_mdB = 0U;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else
    {
        chanSel = configTx->txChannelMask;

        recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txCfrBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanSel, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_TxCfrReg_Cfr1ThrCor_BfGet(device,
                                                            NULL,
                                                            txCfrBaseAddr,
                                                            2u,
                                                            &attenRegVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx slice attenuation");
            goto cleanup;
        }
        /* Conversion from the atten level (milli-dB) to equivalent
         * TxAttenTable index is always done based on a step size of 0.05.
         * Other step sizes are not supported. */
        configTx->txAttenuation_mdB = (uint16_t)(attenRegVal * ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P05);
    }
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SbetCfrMappingSet(adi_adrv904x_Device_t* const             device,
                                                                const uint32_t                           cfrTxChannelMask,
                                                                const uint32_t                           mapping)
{
            adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    uint32_t dpdPowerIndex = 0U;
    uint8_t cfrPowIdxPerChan = 0U;
    uint8_t cfrPowerIndex[4] = { 0U, 0U, 0U, 0U};

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

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

    /* Check that if Mapping is valid*/
    if (mapping > 2)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            mapping,
            "Invalid cfr dpd mapping selected. Valid values are 0..2");
        goto cleanup;
    }

    for (dpdPowerIndex = 0; dpdPowerIndex < 4; dpdPowerIndex++)
    {
        switch (dpdPowerIndex)
        {
            case 0:
                cfrPowIdxPerChan = 0;
                break;
            case 1:
                if (mapping == 0)
                {
                    cfrPowIdxPerChan = 1;
                }
                else
                {
                    cfrPowIdxPerChan = 0;
                }
                break;
            case 2:
                if (mapping == 2)
                {
                    cfrPowIdxPerChan = 0;
                }
                else
                {
                    cfrPowIdxPerChan = 1;
                }
                break;
            case 3:
            cfrPowIdxPerChan = 1;
                break;
        }
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(cfrTxChannelMask, chanSel))
            {
                cfrPowerIndex[dpdPowerIndex] &= ~(0x1 << chanIdx);
                cfrPowerIndex[dpdPowerIndex] |= cfrPowIdxPerChan << chanIdx;
            }
        }
    }

    //for dpd model index 0
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_SBET_DPD_TO_CFR_TRANSLATION_TBL_0,
                                                    cfrPowerIndex[0]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing sbet mapping for dpd model 0");
        goto cleanup;
    }
    //for dpd model index 1
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_SBET_DPD_TO_CFR_TRANSLATION_TBL_1,
                                                    cfrPowerIndex[1]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing sbet mapping for dpd model 1");
        goto cleanup;
    }
    //for dpd model index 2
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_SBET_DPD_TO_CFR_TRANSLATION_TBL_2,
                                                    cfrPowerIndex[2]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing sbet mapping for dpd model 2");
        goto cleanup;
    }
    //for dpd model index 3
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_SBET_DPD_TO_CFR_TRANSLATION_TBL_3,
                                                    cfrPowerIndex[3]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing sbet mapping for dpd model 3");
        goto cleanup;
    }


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



