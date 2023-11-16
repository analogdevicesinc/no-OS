/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_rx.c
* \brief Contains top level rx fpga related functions
*
* FPGA API Version: 2.9.0.4
*/

#include "adi_fpgagen6_rx.h"
#include "adi_fpgagen6_hal.h"

#include "../../private/bf/fpgagen6_bf_axi_rxdma.h"


#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_RX

#define ADI_FPGAGEN6_RX_DMA_STRIDE (FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1 - FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0)

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxCaptureStart(adi_fpgagen6_Device_t* const device)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    recoveryAction = fpgagen6_AxiRxdma_PassToken_BfSet( device,
                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0,
                                                        ADI_TRUE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_CAPTURE_START,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_DatapathEnable_BfSet(device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0,
                                                            ADI_TRUE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_CAPTURE_START,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxCaptureWait(adi_fpgagen6_Device_t* const    device,
                                                            const uint32_t                  timeMsDelay)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    
    //TODO: poll DMA0 DMA_STATUS bit
    uint8_t  dmaStatus = 0U;
    uint32_t i = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    for (i = 0U; i < timeMsDelay; ++i)
    {
        recoveryAction = fpgagen6_AxiRxdma_DmaStatus_BfGet(  device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0,
                                                            &dmaStatus);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                    ADI_FPGAGEN6_ERRCODE_RX_CAPTURE_WAIT,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        if (dmaStatus == ADI_DISABLE)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            goto cleanup;
        }

        recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_Wait_ms(&device->common, 1U);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                    ADI_FPGAGEN6_ERRCODE_RX_CAPTURE_WAIT,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    recoveryAction = adi_fpgagen6_RxDataChainReset(device);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_CAPTURE_WAIT,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    ADI_PARAM_ERROR_REPORT( &device->common,
                            recoveryAction,
                            timeMsDelay,
                            "Capture timed out");
    goto cleanup;

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxDataChainReset(adi_fpgagen6_Device_t* const device)
{
    adi_fpgagen6_ErrAction_e    recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfAxiRxdmaChanAddr_e dmaBaseAddress = FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0;
    uint8_t                     busy          = 0U;
    uint8_t                     haltComplete    = 0U;
    uint32_t                    dmaIdx          = 0U;
    uint32_t                    i               = 0U;
    uint32_t                    dmaAddress = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);
    
    recoveryAction = fpgagen6_AxiRxdma_DatapathEnable_BfSet(device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0,
                                                            ADI_DISABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_RESET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiRxdma_PassToken_BfSet( device,
                                                        FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0,
                                                        ADI_DISABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_RESET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    for (dmaIdx = 0U; dmaIdx < ADI_FPGAGEN6_RX_DMA_COUNT; dmaIdx++)
    {
        dmaAddress = (uint32_t)FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0 + dmaIdx * (uint32_t)ADI_FPGAGEN6_RX_DMA_STRIDE;
        dmaBaseAddress = (fpgagen6_BfAxiRxdmaChanAddr_e)(dmaAddress);
        busy = 0U;
        haltComplete = 0U;

        recoveryAction = fpgagen6_AxiRxdma_Busy_BfGet(  device,
                                                        dmaBaseAddress,
                                                        &busy);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                    ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_RESET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        if (busy != 0U)
        {
            recoveryAction = fpgagen6_AxiRxdma_Stop_BfSet(  device,
                                                            dmaBaseAddress,
                                                            ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                        ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_RESET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            
            for (i = 0U; i < ADI_FPGAGEN6_RX_DMA_HALT_TIMEOUT_MS; ++i)
            {
                recoveryAction = fpgagen6_AxiRxdma_HaltComplete_BfGet(  device,
                                                                        dmaBaseAddress,
                                                                        &haltComplete);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                            ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_RESET,
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
                        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                                ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_RESET,
                                                                ADI_NO_VARIABLE,
                                                                recoveryAction,
                                                                cleanup);
                    }
                }
            }
        }
        
        recoveryAction = fpgagen6_AxiRxdma_Swreset_BfSet(   device,
                                                            dmaBaseAddress,
                                                            ADI_ENABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                    ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_RESET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        if (i >= ADI_FPGAGEN6_RX_DMA_HALT_TIMEOUT_MS)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                ADI_FPGAGEN6_ERRCODE_RX_DMA_HALT_TIMEOUT,
                ADI_NO_VARIABLE,
                recoveryAction,
                cleanup);
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e  adi_fpgagen6_RxDmaCfgSet( adi_fpgagen6_Device_t* const            device,
                                                            const uint8_t                           rxDmaSel,
                                                            const adi_fpgagen6_RxDmaCfg_t* const    rxDmaCfg)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfAxiRxdmaChanAddr_e dmaBaseAddress = FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0;
    uint32_t dmaAddress = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, rxDmaCfg, cleanup);

    if (rxDmaSel >= ADI_FPGAGEN6_RX_DMA_COUNT)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxDmaSel,
                                "Invalid DMA selected. Valid values between 0 : 15");
        goto cleanup;
    }
    dmaAddress = (uint32_t)FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0 + (uint32_t)rxDmaSel * (uint32_t)ADI_FPGAGEN6_RX_DMA_STRIDE;
    dmaBaseAddress = (fpgagen6_BfAxiRxdmaChanAddr_e)(dmaAddress);
    
    device->devStateInfo.rxDmaInUse[rxDmaSel] = rxDmaCfg->inUse;

    recoveryAction = fpgagen6_AxiRxdma_StartAddrLow_BfSet(  device,
                                                            dmaBaseAddress,
                                                            (uint32_t)rxDmaCfg->dma.startingAddress);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_StartAddrHigh_BfSet( device,
                                                            dmaBaseAddress,
                                                            (uint32_t)(rxDmaCfg->dma.startingAddress >> 32U));
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_Qthreshold_BfSet(    device,
                                                            dmaBaseAddress,
                                                            rxDmaCfg->dma.qThreshold);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_AlignToSof_BfSet(    device,
                                                            dmaBaseAddress,
                                                            rxDmaCfg->dma.alignToSof);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_TransSize_BfSet( device,
                                                        dmaBaseAddress,
                                                        rxDmaCfg->dma.transactionSize);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiRxdma_ArbitrationSelect_BfSet( device,
                                                                dmaBaseAddress,
                                                                rxDmaCfg->dma.arbitrationSelect);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_DataMaskSelect_BfSet(    device,
                                                                dmaBaseAddress,
                                                                rxDmaCfg->dma.dataMaskSelect);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e  adi_fpgagen6_RxDmaCfgGet( adi_fpgagen6_Device_t* const device,
                                                            const uint32_t rxDmaSel,
                                                            adi_fpgagen6_RxDmaCfg_t* const rxDmaCfg)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfAxiRxdmaChanAddr_e dmaBaseAddress = FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0;
    uint32_t dmaAddrReadbackLow = 0U;
    uint32_t dmaAddrReadbackHigh = 0U;
    uint32_t dmaAddress = 0U;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, rxDmaCfg, cleanup);
    
    if (rxDmaSel >= ADI_FPGAGEN6_RX_DMA_COUNT)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxDmaSel,
                                "Invalid DMA selected. Valid values between 0 : 15");
        goto cleanup;
    }

    dmaAddress = (uint32_t)FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0 + (uint32_t)rxDmaSel * (uint32_t)ADI_FPGAGEN6_RX_DMA_STRIDE;
    dmaBaseAddress = (fpgagen6_BfAxiRxdmaChanAddr_e)(dmaAddress);
    
    rxDmaCfg->inUse = device->devStateInfo.rxDmaInUse[rxDmaSel];
    
    recoveryAction = fpgagen6_AxiRxdma_StartAddrLow_BfGet(  device,
                                                            dmaBaseAddress,
                                                            &dmaAddrReadbackLow);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiRxdma_StartAddrHigh_BfGet( device,
                                                            dmaBaseAddress,
                                                            &dmaAddrReadbackHigh);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    rxDmaCfg->dma.startingAddress = ((uint64_t)dmaAddrReadbackHigh << 32) + dmaAddrReadbackLow;

    recoveryAction = fpgagen6_AxiRxdma_ValidDataWidthStream_BfGet(  device,
                                                                    dmaBaseAddress,
                                                                    &rxDmaCfg->rxDmaCaptureCfg.validDataWidthStream);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_Qthreshold_BfGet(device,
                                                        dmaBaseAddress,
                                                        &rxDmaCfg->dma.qThreshold);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_AlignToSof_BfGet(device,
                                                        dmaBaseAddress,
                                                        &rxDmaCfg->dma.alignToSof);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_TransSize_BfGet( device,
                                                        dmaBaseAddress,
                                                        &rxDmaCfg->dma.transactionSize);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_ArbitrationSelect_BfGet( device,
                                                                dmaBaseAddress,
                                                                &rxDmaCfg->dma.arbitrationSelect);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiRxdma_DataMaskSelect_BfGet(device,
                                                            dmaBaseAddress,
                                                            &rxDmaCfg->dma.dataMaskSelect);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiRxdma_TriggerSelect_BfGet( device,
                                                            dmaBaseAddress,
                                                            (uint32_t*)&rxDmaCfg->rxDmaCaptureCfg.triggerSelect);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_Length_BfGet(device,
                                                    dmaBaseAddress,
                                                    &rxDmaCfg->rxDmaCaptureCfg.length);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiRxdma_Busy_BfGet(  device,
                                                    dmaBaseAddress,
                                                    &rxDmaCfg->dma.busy);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiRxdma_Complete_BfGet(  device,
                                                        dmaBaseAddress,
                                                        &rxDmaCfg->dma.complete);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiRxdma_Overflow_BfGet(  device,
                                                        dmaBaseAddress,
                                                        &rxDmaCfg->overflow);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxDataChainConfigure( adi_fpgagen6_Device_t* const            device,
                                                                    const uint32_t                          rxDmaSelMask,
                                                                    const adi_fpgagen6_RxDmaCaptureCfg_t    rxDmaCaptureCfgs[],
                                                                    const uint32_t                          numDmasToConfigure)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfAxiRxdmaChanAddr_e dmaBaseAddress = FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0;
    uint32_t dmaIdx = 0U;
    uint32_t numOfDmasConfigured = 0U;
    uint32_t dmaAddress = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    for (dmaIdx = 0U; dmaIdx < ADI_FPGAGEN6_RX_DMA_COUNT; ++dmaIdx)
    {
        if ((rxDmaSelMask & (0x1U << dmaIdx)) > 0U)
        {
            ++numOfDmasConfigured;

            if (numOfDmasConfigured > numDmasToConfigure)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, numDmasToConfigure, recoveryAction, "rxDmaSelMask has more bits set than there are configuration structures.");
                goto cleanup;
            }

            dmaAddress = (uint32_t)FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0 + dmaIdx * (uint32_t)ADI_FPGAGEN6_RX_DMA_STRIDE;
            dmaBaseAddress = (fpgagen6_BfAxiRxdmaChanAddr_e)(dmaAddress);

            recoveryAction = fpgagen6_AxiRxdma_TriggerSelect_BfSet( device,
                                                                    dmaBaseAddress,
                                                                    rxDmaCaptureCfgs[numOfDmasConfigured-1U].triggerSelect);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                        ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
    
            recoveryAction = fpgagen6_AxiRxdma_Length_BfSet(device,
                                                            dmaBaseAddress,
                                                            rxDmaCaptureCfgs[numOfDmasConfigured-1U].length);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                        ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            recoveryAction = fpgagen6_AxiRxdma_ValidDataWidthStream_BfSet(  device,
                                                                            dmaBaseAddress,
                                                                            rxDmaCaptureCfgs[numOfDmasConfigured-1U].validDataWidthStream);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                        ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            recoveryAction = fpgagen6_AxiRxdma_Start_BfSet( device,
                                                            dmaBaseAddress,
                                                            ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_RX,
                                                        ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxTransportCfgSet(adi_fpgagen6_Device_t* const                device,
                                                                const uint8_t                               rxTransportSel,
                                                                const adi_fpgagen6_RxTransportCfg_t* const  rxTransportCfg)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, rxTransportCfg, cleanup);

    if (rxTransportSel >= ADI_FPGAGEN6_RX_DMA_COUNT)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxTransportSel,
                                "Invalid Transport selected.");
        goto cleanup;
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxTransportCfgGet(adi_fpgagen6_Device_t* const            device,
                                                                const uint32_t                          rxTransportSel,
                                                                adi_fpgagen6_RxTransportCfg_t* const    rxTransportCfg)
{    
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, rxTransportCfg, cleanup);

    if (rxTransportSel >= ADI_FPGAGEN6_RX_DMA_COUNT)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxTransportSel,
                                "Invalid Transport selected.");
        goto cleanup;
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}
