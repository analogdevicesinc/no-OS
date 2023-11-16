/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_tx.c
* \brief Contains top level tx fpga related functions
*
* FPGA API Version: 2.9.0.4
*/

#include "adi_fpgagen6_tx.h"
#include "adi_fpgagen6_hal.h"

#include "../../private/bf/fpgagen6_bf_axi_txdma.h"
#include "../../private/include/fpgagen6_data_interface.h"

#include "../../private/bf/fpgagen6_bf_adrv904x_ads10_shim_tx_data_path.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_TX

#define ADI_FPGAGEN6_TX_DMA_STRIDE (FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_1 - FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0)

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataStart(adi_fpgagen6_Device_t* const device)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    recoveryAction = fpgagen6_AxiTxdma_PassToken_BfSet( device, 
                                                        FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0,
                                                        ADI_TRUE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DATA_START,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_Wait_ms(&device->common, 1U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
            ADI_FPGAGEN6_ERRCODE_TX_DATA_START,
            ADI_NO_VARIABLE,
            recoveryAction,
            cleanup);
    }
    
    recoveryAction = fpgagen6_AxiTxdma_DatapathEnable_BfSet(device,
                                                            FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0,
                                                            ADI_TRUE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DATA_START,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataStop(adi_fpgagen6_Device_t* const device)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    recoveryAction = fpgagen6_AxiTxdma_DatapathEnable_BfSet(device,
                                                            FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0,
                                                            ADI_DISABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DATA_STOP,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_PassToken_BfSet( device,
                                                        FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0,
                                                        ADI_DISABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DATA_STOP,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataChainReset(adi_fpgagen6_Device_t* const device)
{
    adi_fpgagen6_ErrAction_e        recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfAxiTxdmaChanAddr_e   dmaBaseAddress  = FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0;
    uint8_t                         busy            = 0U;
    uint8_t                         haltComplete    = 0U;
    uint32_t                        dmaIdx          = 0U;
    uint32_t                        i               = 0U;
    uint32_t                        dmaBaseAddr     = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);
    
    recoveryAction = adi_fpgagen6_TxDataStop(device);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_RESET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    for (dmaIdx = 0U; dmaIdx < ADI_FPGAGEN6_TX_DMA_COUNT; ++dmaIdx)
    {
        dmaBaseAddr = (uint32_t)FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0 + dmaIdx * (uint32_t)ADI_FPGAGEN6_TX_DMA_STRIDE;
        dmaBaseAddress  = (fpgagen6_BfAxiTxdmaChanAddr_e)(dmaBaseAddr);
        busy            = 0U;
        haltComplete    = 0U;

        recoveryAction = fpgagen6_AxiTxdma_Busy_BfGet(  device,
                                                        dmaBaseAddress,
                                                        &busy);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                    ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_RESET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        if (busy != 0U)
        {
            recoveryAction = fpgagen6_AxiTxdma_Stop_BfSet(  device,
                                                            dmaBaseAddress,
                                                            ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_RESET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            
            for (i = 0U; i < ADI_FPGAGEN6_TX_DMA_HALT_TIMEOUT_MS; ++i)
            {
                recoveryAction = fpgagen6_AxiTxdma_HaltComplete_BfGet(  device,
                                                                        dmaBaseAddress,
                                                                        &haltComplete);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                            ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_RESET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                if (haltComplete == 1U)
                {
                    break;
                }
                else
                {
                    recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_Wait_ms(&device->common, 1U);
                    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                    {
                        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                                ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_RESET,
                                                                ADI_NO_VARIABLE,
                                                                recoveryAction,
                                                                cleanup);
                    }
                }
            }
        }

        recoveryAction = fpgagen6_AxiTxdma_Swreset_BfSet(   device,
                                                            dmaBaseAddress,
                                                            ADI_ENABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                    ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_RESET,
                                                    dmaIdx,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        if ((busy != 0U) && (haltComplete != 1U))
        {
            recoveryAction = fpgagen6_AxiTxdma_Busy_BfGet(  device,
                                                            dmaBaseAddress,
                                                            &busy);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_RESET,
                                                        dmaIdx,
                                                        recoveryAction,
                                                        cleanup);
            }

            if (busy != 0U)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DMA_HALT_TIMEOUT,
                                                        dmaIdx,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

    recoveryAction = fpgagen6_Adrvgen6Ads10ShimTxDataPath_SoftReset_BfSet(device, FPGAGEN6_BF_ADS10_SHIM_TX_DATA_PATH, ADI_ENABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
            ADI_FPGAGEN6_ERRCODE_TX_DMA_HALT_TIMEOUT,
            ADI_NO_VARIABLE,
            recoveryAction,
            cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDmaCfgSet(  adi_fpgagen6_Device_t* const            device,
                                                            const uint8_t                           txDmaSel,
                                                            const adi_fpgagen6_TxDmaCfg_t* const    txDmaCfg)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfAxiTxdmaChanAddr_e dmaBaseAddress = FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0;
    uint32_t dmaBaseAddr     = 0U;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, txDmaCfg, cleanup);

    if (txDmaSel >= ADI_FPGAGEN6_TX_DMA_COUNT)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                txDmaSel,
                                "Invalid DMA selected. Valid values between 0 : 15");
        goto cleanup;
    }

    dmaBaseAddr = (uint32_t)FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0 + (uint32_t)txDmaSel * (uint32_t)ADI_FPGAGEN6_TX_DMA_STRIDE;
    dmaBaseAddress = (fpgagen6_BfAxiTxdmaChanAddr_e)(dmaBaseAddr);

    recoveryAction = fpgagen6_AxiTxdma_StartAddrLow_BfSet(  device,
                                                            dmaBaseAddress,
                                                            (uint32_t)txDmaCfg->dma.startingAddress);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_StartAddrHigh_BfSet( device,
                                                            dmaBaseAddress,
                                                            (uint32_t)(txDmaCfg->dma.startingAddress >> 32U));
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_ValidDataWidthStream_BfSet(  device,
                                                                    dmaBaseAddress,
                                                                    txDmaCfg->validDataWidthStream);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_Qthreshold_BfSet(device,
                                                        dmaBaseAddress,
                                                        txDmaCfg->dma.qThreshold);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_AlignToSof_BfSet(device,
                                                        dmaBaseAddress,
                                                        txDmaCfg->dma.alignToSof);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_TransSize_BfSet( device,
                                                        dmaBaseAddress,
                                                        txDmaCfg->dma.transactionSize);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_ArbitrationSelect_BfSet( device,
                                                                dmaBaseAddress,
                                                                txDmaCfg->dma.arbitrationSelect);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_DataMaskSelect_BfSet(device,
                                                            dmaBaseAddress,
                                                            txDmaCfg->dma.dataMaskSelect);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup :
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e  adi_fpgagen6_TxDmaCfgGet( adi_fpgagen6_Device_t* const    device,
                                                            const uint32_t                  txDmaSel,
                                                            adi_fpgagen6_TxDmaCfg_t* const  txDmaCfg)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfAxiTxdmaChanAddr_e dmaBaseAddress = FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0;
    uint32_t dmaAddrReadbackLow = 0U;
    uint32_t dmaAddrReadbackHigh = 0U;
    uint32_t dmaBaseAddr = 0U;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, txDmaCfg, cleanup);

    if (txDmaSel >= ADI_FPGAGEN6_TX_DMA_COUNT)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                txDmaSel,
                                "Invalid DMA selected. Valid values between 0 : 15");
        goto cleanup;
    }

    dmaBaseAddr = (uint32_t)FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0 + txDmaSel * (uint32_t)ADI_FPGAGEN6_TX_DMA_STRIDE;
    dmaBaseAddress = (fpgagen6_BfAxiTxdmaChanAddr_e)(dmaBaseAddr);

    recoveryAction = fpgagen6_AxiTxdma_StartAddrLow_BfGet(  device,
                                                            dmaBaseAddress,
                                                            &dmaAddrReadbackLow);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_StartAddrHigh_BfGet( device,
                                                            dmaBaseAddress,
                                                            &dmaAddrReadbackHigh);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    txDmaCfg->dma.startingAddress = ((uint64_t)dmaAddrReadbackHigh << 32) + dmaAddrReadbackLow;
    
    recoveryAction = fpgagen6_AxiTxdma_ValidDataWidthStream_BfGet(  device,
                                                                    dmaBaseAddress,
                                                                    &txDmaCfg->validDataWidthStream);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_Qthreshold_BfGet(device,
                                                        dmaBaseAddress,
                                                        &txDmaCfg->dma.qThreshold);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_AlignToSof_BfGet(device,
                                                        dmaBaseAddress,
                                                        &txDmaCfg->dma.alignToSof);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_TransSize_BfGet( device,
                                                        dmaBaseAddress,
                                                        &txDmaCfg->dma.transactionSize);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiTxdma_ArbitrationSelect_BfGet( device,
                                                                dmaBaseAddress,
                                                                &txDmaCfg->dma.arbitrationSelect);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_DataMaskSelect_BfGet(device,
                                                            dmaBaseAddress,
                                                            &txDmaCfg->dma.dataMaskSelect);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_TriggerSelect_BfGet( device,
                                                            dmaBaseAddress,
                                                            &txDmaCfg->txDmaCaptureCfg.triggerSelect);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_Length_BfGet(device,
                                                    dmaBaseAddress,
                                                    &txDmaCfg->txDmaCaptureCfg.length);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_Busy_BfGet(  device,
                                                    dmaBaseAddress,
                                                    &txDmaCfg->dma.busy);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_Complete_BfGet(  device,
                                                        dmaBaseAddress,
                                                        &txDmaCfg->dma.complete);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_Cyclic_BfGet(device,
                                                    dmaBaseAddress,
                                                    &txDmaCfg->txDmaCaptureCfg.continuous);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiTxdma_Underflow_BfGet( device,
                                                        dmaBaseAddress,
                                                        &txDmaCfg->overflow);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }


    {
        /* Read Wave Sequencer */
        uint8_t wsIndex = 0;

        recoveryAction = fpgagen6_AxiTxdma_WsEnable_BfGet(device,
                                                          dmaBaseAddress,
                                                          &txDmaCfg->txDmaCaptureCfg.wsEnable);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                    ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        for (wsIndex = 0; wsIndex < ADI_FPGAGEN6_MAX_WAVE_SEQ_PER_TXDMA; wsIndex++)
        {
            recoveryAction = fpgagen6_AxiTxdma_WsStartAddrLow_BfGet(  device,
                                                                    dmaBaseAddress,
                                                                    wsIndex,
                                                                    &dmaAddrReadbackLow);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            recoveryAction = fpgagen6_AxiTxdma_WsStartAddrHigh_BfGet( device,
                                                                     dmaBaseAddress,
                                                                     wsIndex,
                                                                     &dmaAddrReadbackHigh);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            txDmaCfg->wsAddress[wsIndex] = ((uint64_t)dmaAddrReadbackHigh << 32) + dmaAddrReadbackLow;

            recoveryAction = fpgagen6_AxiTxdma_WsIterations_BfGet(device,
                                                                  dmaBaseAddress,
                                                                  wsIndex,
                                                                  &txDmaCfg->wsIterations[wsIndex]);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            recoveryAction = fpgagen6_AxiTxdma_WsLength_BfGet(device,
                                                              dmaBaseAddress,
                                                              wsIndex,
                                                              &txDmaCfg->wsLength[wsIndex]);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataChainConfigure( adi_fpgagen6_Device_t* const            device,
                                                                    const uint32_t                          txDmaSelMask,
                                                                    const adi_fpgagen6_TxDmaTransmitCfg_t   txDmaCaptureCfgs[],
                                                                    const uint32_t                          numDmasToConfigure)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfAxiTxdmaChanAddr_e dmaBaseAddress = FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0;
    uint32_t dmaIdx = 0U;
    uint32_t numOfDmasConfigured = 0U;
    uint32_t dmaBaseAddr = 0U;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FPGAGEN6_API_ENTRY(&device->common);
    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, txDmaCaptureCfgs, cleanup);

    if (txDmaSelMask == 0)
    {
        /* If mask is zero then perform no work and return no error */
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
        goto cleanup;        
    }
    
    for (dmaIdx = 0; dmaIdx < ADI_FPGAGEN6_TX_DMA_COUNT; ++dmaIdx)
    {
        if ((txDmaSelMask & (0x1U << dmaIdx)) > 0U)
        {
            ++numOfDmasConfigured;

            if (numOfDmasConfigured > numDmasToConfigure)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, numDmasToConfigure, recoveryAction, "txDmaSelMask has more bits set than there are configuration structures.");
                goto cleanup;
            }

            dmaBaseAddr = (uint32_t)FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0 + dmaIdx * (uint32_t)ADI_FPGAGEN6_TX_DMA_STRIDE;
            dmaBaseAddress = (fpgagen6_BfAxiTxdmaChanAddr_e)(dmaBaseAddr);

            recoveryAction = fpgagen6_AxiTxdma_TriggerSelect_BfSet( device,
                                                                    dmaBaseAddress,
                                                                    txDmaCaptureCfgs[numOfDmasConfigured-1U].triggerSelect);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            recoveryAction = fpgagen6_AxiTxdma_Length_BfSet(device,
                                                            dmaBaseAddress,
                                                            txDmaCaptureCfgs[numOfDmasConfigured - 1U].length);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }


            /* Disable Waveform Sequencer */
            recoveryAction = fpgagen6_AxiTxdma_WsEnable_BfSet(device,
                                                              dmaBaseAddress,
                                                              ADI_DISABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            /* Setup Wave Sequencer */
            if (txDmaCaptureCfgs[numOfDmasConfigured - 1U].wsEnable > 0)
            {
                uint8_t wsIndex = txDmaCaptureCfgs[numOfDmasConfigured - 1U].wsIndex;
                if (wsIndex >= ADI_FPGAGEN6_MAX_WAVE_SEQ_PER_TXDMA)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, wsIndex, recoveryAction, "wsIndex is not valid.");
                    goto cleanup;
                }

                uint8_t bfValue = 0;
                recoveryAction = fpgagen6_AxiTxdma_IncludeWs_BfGet(device,
                                                                   dmaBaseAddress,
                                                                   &bfValue);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                            ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                if (bfValue == 0)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, bfValue, recoveryAction, "IncludeWs is not valid.");
                    goto cleanup;
                }

                recoveryAction = fpgagen6_AxiTxdma_WsStartAddrLow_BfSet(  device,
                                                                        dmaBaseAddress,
                                                                        wsIndex,
                                                                        (uint32_t)txDmaCaptureCfgs[numOfDmasConfigured - 1U].wsAddress);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                            ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                recoveryAction = fpgagen6_AxiTxdma_WsStartAddrHigh_BfSet( device,
                                                                         dmaBaseAddress,
                                                                         wsIndex,
                                                                         (uint32_t)(txDmaCaptureCfgs[numOfDmasConfigured - 1U].wsAddress >> 32U));
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                            ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                recoveryAction = fpgagen6_AxiTxdma_WsIterations_BfSet(device,
                                                                      dmaBaseAddress,
                                                                      wsIndex,
                                                                      txDmaCaptureCfgs[numOfDmasConfigured - 1U].wsIterations);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                            ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                recoveryAction = fpgagen6_AxiTxdma_WsLength_BfSet(device,
                                                                  dmaBaseAddress,
                                                                  wsIndex,
                                                                  txDmaCaptureCfgs[numOfDmasConfigured - 1U].length);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                            ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                recoveryAction = fpgagen6_AxiTxdma_WsEnable_BfSet(device,
                                                                  dmaBaseAddress,
                                                                  ADI_ENABLE);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                            ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }


            recoveryAction = fpgagen6_AxiTxdma_Cyclic_BfSet(device,
                                                            dmaBaseAddress,
                                                            txDmaCaptureCfgs[numOfDmasConfigured-1U].continuous);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
 
            recoveryAction = fpgagen6_AxiTxdma_Start_BfSet( device,
                                                           dmaBaseAddress,
                                                           ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataWsMmEnable( adi_fpgagen6_Device_t* const            device,
                                                                const uint32_t                          txDmaSelMask)
{    


    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfAxiTxdmaChanAddr_e dmaBaseAddress = FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0;
    uint32_t dmaIdx = 0U;
    uint32_t dmaBaseAddr = 0U;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FPGAGEN6_API_ENTRY(&device->common);

    if (txDmaSelMask == 0)
    {
        /* If mask is zero then perform no work and return no error */
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
        goto cleanup;        
    }


    for (dmaIdx = 0; dmaIdx < ADI_FPGAGEN6_TX_DMA_COUNT; ++dmaIdx)
    {
        if ((txDmaSelMask & (0x1U << dmaIdx)) > 0U)
        {
            dmaBaseAddr = (uint32_t)FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0 + dmaIdx * (uint32_t)ADI_FPGAGEN6_TX_DMA_STRIDE;
            dmaBaseAddress = (fpgagen6_BfAxiTxdmaChanAddr_e)(dmaBaseAddr);

            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;

                        /* Setup Wave Sequencer Manual Mode Enable */

            recoveryAction = fpgagen6_AxiTxdma_WsMmEnable_BfSet(device,
                                                                dmaBaseAddress,
                                                                ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DATA_MM_ENABLE,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataWsMmAdvance( adi_fpgagen6_Device_t* const            device,
                                                                 const uint32_t                          txDmaSelMask)
{
    

    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfAxiTxdmaChanAddr_e dmaBaseAddress = FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0;
    uint32_t dmaIdx = 0U;
    uint32_t dmaBaseAddr = 0U;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FPGAGEN6_API_ENTRY(&device->common);

    if (txDmaSelMask == 0)
    {
        /* If mask is zero then perform no work and return no error */
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
        goto cleanup;        
    }


    for (dmaIdx = 0; dmaIdx < ADI_FPGAGEN6_TX_DMA_COUNT; ++dmaIdx)
    {
        if ((txDmaSelMask & (0x1U << dmaIdx)) > 0U)
        {
            dmaBaseAddr = (uint32_t)FPGAGEN6_BF_ADS10V1_TOP_AXI_TXDMA_0 + dmaIdx * (uint32_t)ADI_FPGAGEN6_TX_DMA_STRIDE;
            dmaBaseAddress = (fpgagen6_BfAxiTxdmaChanAddr_e)(dmaBaseAddr);


            /* Setup Wave Sequencer Manual Mode Advance */

            recoveryAction = fpgagen6_AxiTxdma_WsMmAdvance_BfSet(device,
                                                                dmaBaseAddress,
                                                                ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_TX,
                                                        ADI_FPGAGEN6_ERRCODE_TX_DATA_MM_ENABLE,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}
