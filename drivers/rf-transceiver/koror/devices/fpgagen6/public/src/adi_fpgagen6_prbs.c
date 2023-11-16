/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file: adi_fpgagen6_prbs.c
 *
 * \brief Contains top level fpgagen6 prbs related functions
 *
 * FPGA API Version: 2.9.0.4
 */

#include "adi_fpgagen6_prbs.h"
#include "adi_fpgagen6_hal.h"
#include "adi_fpgagen6_core.h"

#include "../../private/bf/fpgagen6_bf_axi_adrv904x_phy.h"
#include "../../private/include/fpgagen6_data_interface.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_PRBS

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_PrbsSerializerEnable( adi_fpgagen6_Device_t* const        device,
                                                                    const uint8_t                       chSelMask,
                                                                    const adi_fpgagen6_PrbsTestModes_e  prbsModeSel)
{
        adi_fpgagen6_ErrAction_e    recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx          = 0U;
    adi_fpgagen6_DrpInterface_e drpSel              = ADI_FPGAGEN6_DRP_INTERFACE_CH0;
    uint8_t                     gearboxEnableVal    = 0U;
    uint8_t                     bufferEnableVal     = 0U;
    uint8_t                     outClkSelVal        = 0U;
    uint16_t                    drpReadWord         = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; channelIdx++)
    {
        if ((chSelMask & (1U << channelIdx)) > 0U)
        {
            drpSel = (adi_fpgagen6_DrpInterface_e)channelIdx;
            
            /* If disabling PRBS, restore the previous values */
            if (prbsModeSel == ADI_FPGAGEN6_PRBS_OFF)
            {
                if (device->devStateInfo.serializerPrbsStoredData[channelIdx].prbsEnabled == ADI_DISABLE)
                {
                    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                            ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ENABLE,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                device->devStateInfo.serializerPrbsStoredData[channelIdx].prbsEnabled = ADI_DISABLE;

                gearboxEnableVal    = device->devStateInfo.serializerPrbsStoredData[channelIdx].gearboxEnable;
                bufferEnableVal     = device->devStateInfo.serializerPrbsStoredData[channelIdx].bufferEnable;
                outClkSelVal        = device->devStateInfo.serializerPrbsStoredData[channelIdx].outClkSel;
            }
            /* If enabling PRBS, save off the current values */
            else
            {
                /* Read the gearbox */
                recoveryAction = adi_fpgagen6_DrpFieldRead(device,
                                                            ADI_FPGAGEN6_DRP_ADDRESS_TXPRBS_SETTINGS,
                                                            drpSel,
                                                            &drpReadWord,
                                                            ADI_FPGAGEN6_DRP_MASK_TXGEARBOX_EN);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                            ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ENABLE,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
                device->devStateInfo.serializerPrbsStoredData[channelIdx].gearboxEnable = (uint8_t)drpReadWord;
                
                /* Read the buffer enable */
                recoveryAction = adi_fpgagen6_DrpFieldRead(device,
                                                            ADI_FPGAGEN6_DRP_ADDRESS_TXPRBS_SETTINGS,
                                                            drpSel,
                                                            &drpReadWord,
                                                            ADI_FPGAGEN6_DRP_MASK_TXBUF_EN);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                            ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ENABLE,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
                device->devStateInfo.serializerPrbsStoredData[channelIdx].bufferEnable = (uint8_t)drpReadWord;
                
            
                /* Set the clocking scheme */
                recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxOutClkSel_BfGet( device,
                                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                                channelIdx,
                                                                                &device->devStateInfo.serializerPrbsStoredData[channelIdx].outClkSel);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                            ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ENABLE,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
                
                device->devStateInfo.serializerPrbsStoredData[channelIdx].prbsEnabled = ADI_ENABLE;
                
                /* Set the hardcoded values to enable PRBS */
                gearboxEnableVal = ADI_DISABLE;
                bufferEnableVal = ADI_ENABLE;
                outClkSelVal = ADI_FPGAGEN6_CLKPMA;
            }

            /* Set the gearbox */
            recoveryAction = adi_fpgagen6_DrpFieldWrite(device,
                                                        ADI_FPGAGEN6_DRP_ADDRESS_TXPRBS_SETTINGS,
                                                        drpSel,
                                                        gearboxEnableVal,
                                                        ADI_FPGAGEN6_DRP_MASK_TXGEARBOX_EN);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                        ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ENABLE,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            
            /* Set the buffer */
            recoveryAction = adi_fpgagen6_DrpFieldWrite(device,
                                                        ADI_FPGAGEN6_DRP_ADDRESS_TXPRBS_SETTINGS,
                                                        drpSel,
                                                        bufferEnableVal,
                                                        ADI_FPGAGEN6_DRP_MASK_TXBUF_EN);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                        ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ENABLE,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            
            /* Set the clocking scheme */
            recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxOutClkSel_BfSet( device,
                                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                            channelIdx,
                                                                            outClkSelVal);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                        ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ENABLE,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            
            /* Set the PRBS mode */
            recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPrbsSel_BfSet(   device,
                                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                            channelIdx,
                                                                            (uint8_t)prbsModeSel);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                        ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ENABLE,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_PrbsDeserializerEnable(   adi_fpgagen6_Device_t* const        device,
                                                                        const uint8_t                       chSelMask,
                                                                        const adi_fpgagen6_PrbsTestModes_e  prbsModeSel)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;
    uint8_t                     outClkSelVal    = 0U;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);
    
    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; channelIdx++)
    {
        if ((chSelMask & (1U << channelIdx)) > 0U)
        {
            /* If disabling PRBS, restore the previous values */
            if (prbsModeSel == ADI_FPGAGEN6_PRBS_OFF)
            {
                if (device->devStateInfo.deserializerPrbsStoredData[channelIdx].prbsEnabled == ADI_DISABLE)
                {
                    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                            ADI_FPGAGEN6_ERRCODE_PRBS_DESERIALIZER_ENABLE,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                device->devStateInfo.deserializerPrbsStoredData[channelIdx].prbsEnabled = ADI_DISABLE;

                outClkSelVal = device->devStateInfo.deserializerPrbsStoredData[channelIdx].outClkSel;
            }
            /* If enabling PRBS, save off the current values */
            else
            {
                /* Set the clocking scheme */
                recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxOutClkSel_BfGet( device,
                                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                                channelIdx,
                                                                                &device->devStateInfo.deserializerPrbsStoredData[channelIdx].outClkSel);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                            ADI_FPGAGEN6_ERRCODE_PRBS_DESERIALIZER_ENABLE,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
                device->devStateInfo.deserializerPrbsStoredData[channelIdx].prbsEnabled = ADI_ENABLE;

                /* Set the hardcoded values to enable PRBS */
                outClkSelVal = ADI_FPGAGEN6_CLKPMA;
            }
            /* Change the clocking scheme */
            recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxOutClkSel_BfSet( device,
                                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                            channelIdx,
                                                                            outClkSelVal);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                        ADI_FPGAGEN6_ERRCODE_PRBS_DESERIALIZER_ENABLE,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            
            /* Set the PRBS mode */
            recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxPrbsSel_BfSet(   device,
                                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                            channelIdx,
                                                                            (uint8_t)prbsModeSel);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                        ADI_FPGAGEN6_ERRCODE_PRBS_DESERIALIZER_ENABLE,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_PrbsSerializerErrorInject(adi_fpgagen6_Device_t* const    device,
                                                                        const uint8_t                   chSelMask)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; channelIdx++)
    {
        if ((chSelMask & (1U << channelIdx)) > 0U)
        {
            /* Inject errors on the desired channels */
            recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPrbsInjErr_BfSet(device,
                                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                            channelIdx,
                                                                            ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                        ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ERROR_INJECT,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_PrbsErrorCountRead(   adi_fpgagen6_Device_t* const            device,
                                                                    adi_fpgagen6_PrbsErrorCount_t* const    prbsErrorCounts)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, prbsErrorCounts, cleanup);

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; channelIdx++)
    {
        /* Inject errors on the desired channels */
        recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxPrbsErrCnt_BfGet(device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        channelIdx,
                                                                        &prbsErrorCounts->errorCount[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                    ADI_FPGAGEN6_ERRCODE_PRBS_ERROR_COUNT_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_PrbsErrorClear(   adi_fpgagen6_Device_t* const    device,
                                                                const uint8_t                   chSelMask)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; channelIdx++)
    {
        if ((chSelMask & (1U << channelIdx)) > 0U)
        {
            /* Inject errors on the desired channels */
            recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxPrbsErrCntRst_BfSet( device,
                                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                                channelIdx,
                                                                                ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_PRBS,
                                                        ADI_FPGAGEN6_ERRCODE_PRBS_ERROR_CLEAR,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

