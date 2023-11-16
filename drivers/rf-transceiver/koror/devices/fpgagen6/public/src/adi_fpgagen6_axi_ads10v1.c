/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/*!
* \file adi_fpgagen6_axi_ads10v1.c
*
* \brief    AXI ADS10v1 Functions for the FPGAGEN6 Device
*
* FPGA API Version: 2.9.0.4
*/

#include "adi_fpgagen6_axi_ads10v1.h"
#include "adi_fpgagen6_hal.h"

#include "../../private/bf/fpgagen6_bf_axi_ads10v1.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_AXI_ADS10V1


ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_Ads10v1GlobalReset( adi_fpgagen6_Device_t* const device)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    /* Global Reset is Self Clearing */
    recoveryAction = fpgagen6_AxiAds10v1_GlobalReset_BfSet( device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                            ADI_ENABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_GLOBAL_RESET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* TODO: DRP Writes */

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SmaPinCfgGet( adi_fpgagen6_Device_t* const            device,
                                                            adi_fpgagen6_Ads10v1SmaPinCfg_t* const  smaPinInfo)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     smaPinIdx       = 0U;
    uint8_t                     destinationIdx  = 0U;
    uint8_t                     smaDir          = 0xFFU;
    uint8_t                     bfValue         = 0x0U;
    uint32_t                    smaPinValue     = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    for (smaPinIdx = 0U; smaPinIdx < ADI_FPGAGEN_AXI_ADS10V1_SMA_PIN_NUM; ++smaPinIdx)
    {
        recoveryAction = fpgagen6_AxiAds10v1_SmaDir_BfGet(  device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                            smaPinIdx,
                                                            &smaDir);

        switch (smaDir)
        {
            case ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_OUTPUT:
                recoveryAction = fpgagen6_AxiAds10v1_SmaSrc_BfGet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    destinationIdx,
                                                                    &bfValue);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                            ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_GET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
                /* Offset from Output Index 0 (See User Guide) */
                smaPinValue = (uint32_t)ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_CONSTANT_LOW + (uint32_t)bfValue;
                smaPinInfo->smaPin[smaPinIdx] = (adi_fpgagen6_AxiAds10v1SmaSel_e)(smaPinValue);
                break;

            case ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_INPUT:
                /* Scan all inputs for matching pin configuration */
                for (destinationIdx = 0U; destinationIdx < ADI_FPGAGEN_AXI_ADS10V1_SMA_OUTPUT_MAX_NUM; ++destinationIdx)
                {
                    recoveryAction = fpgagen6_AxiAds10v1_DestSmaSel_BfGet(  device,
                                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                            destinationIdx,
                                                                            &bfValue);
                    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                    {
                        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_GET,
                                                                destinationIdx,
                                                                recoveryAction,
                                                                cleanup);
                    }

                    if (bfValue == smaPinIdx)
                    {
                        /* Offset from Input Index 0 (See User Guide) */
                        smaPinValue = (uint32_t)ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_RX_DMA_EXT_LINK0 + (uint32_t)destinationIdx;
                        smaPinInfo->smaPin[smaPinIdx] = (adi_fpgagen6_AxiAds10v1SmaSel_e)(smaPinValue);
                    }
                }
                break;


            default:
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, smaPinInfo->smaPin, "Invalid SMA Pin Direction");
                goto cleanup;
                break;
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SmaPinCfgSet( adi_fpgagen6_Device_t* const                    device,
                                                            const adi_fpgagen6_Ads10v1SmaPinCfg_t* const    smaPinInfo)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     smaPinIdx       = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    for (smaPinIdx = 0U; smaPinIdx < ADI_FPGAGEN_AXI_ADS10V1_SMA_PIN_NUM; ++smaPinIdx)
    {
        /* Configure Each SMA Pin */
        recoveryAction = fpgagen6_AxiAds10v1_SmaDir_BfSet(  device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                            smaPinIdx,
                                                            ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_INPUT);

        switch (smaPinInfo->smaPin[smaPinIdx])
        {
            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_NO_SELECTION:
                /* Do Nothing */
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_CONSTANT_LOW:
                recoveryAction = fpgagen6_AxiAds10v1_SmaDir_BfSet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    smaPinIdx,
                                                                    ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_OUTPUT);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                            ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_SET,
                                                            smaPinInfo->smaPin[smaPinIdx],
                                                            recoveryAction,
                                                            cleanup);
                }

                recoveryAction = fpgagen6_AxiAds10v1_SmaSrc_BfSet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    smaPinIdx,
                                                                    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_IDX_CONSTANT_LOW);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_CONSTANT_HIGH:
                recoveryAction = fpgagen6_AxiAds10v1_SmaDir_BfSet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    smaPinIdx,
                                                                    ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_OUTPUT);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                            ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_SET,
                                                            smaPinInfo->smaPin[smaPinIdx],
                                                            recoveryAction,
                                                            cleanup);
                }

                recoveryAction = fpgagen6_AxiAds10v1_SmaSrc_BfSet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    smaPinIdx,
                                                                    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_IDX_CONSTANT_HIGH);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_TDD_EXT:
                recoveryAction = fpgagen6_AxiAds10v1_SmaDir_BfSet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    smaPinIdx,
                                                                    ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_OUTPUT);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                            ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_SET,
                                                            smaPinInfo->smaPin[smaPinIdx],
                                                            recoveryAction,
                                                            cleanup);
                }
                recoveryAction = fpgagen6_AxiAds10v1_SmaSrc_BfSet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    smaPinIdx,
                                                                    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_IDX_TDD_EXT);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_TX_WAVEFORM_SRT_PULSE:
                recoveryAction = fpgagen6_AxiAds10v1_SmaDir_BfSet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    smaPinIdx,
                                                                    ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_OUTPUT);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                            ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_SET,
                                                            smaPinInfo->smaPin[smaPinIdx],
                                                            recoveryAction,
                                                            cleanup);
                }

                recoveryAction = fpgagen6_AxiAds10v1_SmaSrc_BfSet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    smaPinIdx,
                                                                    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_IDX_TX_WAVEFORM_SRT_PULSE);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_MULTI_PLATFORM_SYNC:
                recoveryAction = fpgagen6_AxiAds10v1_SmaDir_BfSet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    smaPinIdx,
                                                                    ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_OUTPUT);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                            ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_SET,
                                                            smaPinInfo->smaPin[smaPinIdx],
                                                            recoveryAction,
                                                            cleanup);
                }
                recoveryAction = fpgagen6_AxiAds10v1_SmaSrc_BfSet(  device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    smaPinIdx,
                                                                    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_IDX_MULTI_PLATFORM_SYNC);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_GPIO_SRC:
                recoveryAction = fpgagen6_AxiAds10v1_SmaDir_BfSet(  device,
                                                                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                  smaPinIdx,
                                                                  ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_OUTPUT);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                            ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_SET,
                                                            smaPinInfo->smaPin[smaPinIdx],
                                                            recoveryAction,
                                                            cleanup);
                }
                
                recoveryAction = fpgagen6_AxiAds10v1_SmaSrc_BfSet(device,
                                                                  FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                  smaPinIdx,
                                                                  ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_SMA_SRC);
    
                break;
                
            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_RX_DMA_EXT_LINK0:
                recoveryAction = fpgagen6_AxiAds10v1_DestSmaSel_BfSet(  device,
                                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                        ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_RX_DMA_EXT_LINK0,
                                                                        smaPinIdx);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_RX_DMA_EXT_LINK1:
                recoveryAction = fpgagen6_AxiAds10v1_DestSmaSel_BfSet(  device,
                                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                        ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_RX_DMA_EXT_LINK1,
                                                                        smaPinIdx);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_RX_DMA_EXT_LINK2:
                recoveryAction = fpgagen6_AxiAds10v1_DestSmaSel_BfSet(  device,
                                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                        ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_RX_DMA_EXT_LINK2,
                                                                        smaPinIdx);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_TX_DMA_EXT_LINK0:
                recoveryAction = fpgagen6_AxiAds10v1_DestSmaSel_BfSet(  device,
                                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                        ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_TX_DMA_EXT_LINK0,
                                                                        smaPinIdx);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_TX_DMA_EXT_LINK1:
                recoveryAction = fpgagen6_AxiAds10v1_DestSmaSel_BfSet(  device,
                                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                        ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_TX_DMA_EXT_LINK1,
                                                                        smaPinIdx);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_TDD_FSM_EXT:
                recoveryAction = fpgagen6_AxiAds10v1_DestSmaSel_BfSet(  device,
                                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                        ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_TDD_FSM_EXT,
                                                                        smaPinIdx);
                break;

            case ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_MULTI_PLATFORM_SYNC:
                recoveryAction = fpgagen6_AxiAds10v1_DestSmaSel_BfSet(  device,
                                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                        ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_MULTI_PLATFORM_SYNC,
                                                                        smaPinIdx);
                break;

            default:
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, smaPinInfo->smaPin, "Invalid SMA Pin Selection");
                goto cleanup;
                break;
        }

        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                    ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_SET,
                                                    smaPinInfo->smaPin[smaPinIdx],
                                                    recoveryAction,
                                                    cleanup);
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_Ads10v1LedCfgGet( adi_fpgagen6_Device_t* const        device,
                                                                adi_fpgagen6_Ads10v1LedCfg_t* const ledCfg)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     bfValue         = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    recoveryAction = fpgagen6_AxiAds10v1_LedMode_BfGet(device,
                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                        &bfValue);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    ledCfg->mode = (adi_fpgagen6_AxiAds10v1LedMode_e) bfValue;

    recoveryAction = fpgagen6_AxiAds10v1_LedDelayToModeChange_BfGet(device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    &ledCfg->delayMs);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAds10v1_LedBlinkDuration_BfGet(device,
                                                                FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                &ledCfg->blinkDuration);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAds10v1_LedNumBlinks_BfGet(device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                            &ledCfg->blinkNum);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAds10v1_LedMask_BfGet( device,
                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                        &ledCfg->mask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAds10v1_LedSwDefinedValue_BfGet(   device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    &ledCfg->swDefined);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_Ads10v1LedCfgSet( adi_fpgagen6_Device_t* const                device,
                                                                const adi_fpgagen6_Ads10v1LedCfg_t* const   ledCfg)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    /* LED Mode */
    recoveryAction = fpgagen6_AxiAds10v1_LedMode_BfSet( device,
                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                        ledCfg->mode);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAds10v1_LedDelayToModeChange_BfSet(device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    ledCfg->delayMs);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAds10v1_LedBlinkDuration_BfSet(device,
                                                                FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                ledCfg->blinkDuration);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAds10v1_LedNumBlinks_BfSet(device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                            ledCfg->blinkNum);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAds10v1_LedMask_BfSet( device,
                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                        ledCfg->mask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAds10v1_LedSwDefinedValue_BfSet(   device,
                                                                    FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                                    ledCfg->swDefined);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,
                                                ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}