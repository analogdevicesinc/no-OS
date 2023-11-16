/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/*!
* \file adi_fpgagen6_data_interface.c
*
* \brief    Data Interface Functions for the FPGAGEN6 Device
*
* FPGA API Version: 2.9.0.4
*/
#include "adi_fpgagen6_data_interface.h"
#include "adi_fpgagen6_hal.h"

#include "../../private/include/fpgagen6_data_interface.h"
#include "../../private/bf/fpgagen6_bf_axi_adrv904x_phy.h"
#include "../../private/bf/fpgagen6_bf_axi_adrv904x_rx.h"
#include "../../private/bf/fpgagen6_bf_axi_adrv904x_tx.h"
#include "../../private/bf/fpgagen6_bf_xilinx_jesd.h"
#include "../../private/bf/fpgagen6_bf_adrv904x_system_timing_control.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_DATA_INTERFACE

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SerializerCfgSet( adi_fpgagen6_Device_t* const        device,
                                                                const uint8_t                       chSel,
                                                                const adi_fpgagen6_SerializerCfg_t* const serializerCfg)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, serializerCfg, cleanup);

    if (chSel >= ADI_FPGAGEN6_LANES_PHYSICAL_NUM)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET,
                                                chSel,
                                                recoveryAction,
                                                cleanup);
    }

    /* TODO: need to find rate bitfield */

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPllClkSel_BfSet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    serializerCfg->commonCfg.pllClkSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPolarity_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    serializerCfg->commonCfg.polarity);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPowerdown_BfSet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    serializerCfg->commonCfg.powerDown);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTx8b10bEnable_BfSet(   device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        chSel,
                                                                        serializerCfg->commonCfg.encoder8b10bEnable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPrecursor_BfSet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    serializerCfg->precursor);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPostcursor_BfSet(device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    serializerCfg->postcursor);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxDiffControl_BfSet(   device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        chSel,
                                                                        serializerCfg->diffCtrl);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxOutClkSel_BfSet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    serializerCfg->txOutClkSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SerializerCfgGet( adi_fpgagen6_Device_t* const        device,
                                                                const uint8_t                       chSel,
                                                                adi_fpgagen6_SerializerCfg_t* const serializerCfg)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, serializerCfg, cleanup);

    if (chSel >= ADI_FPGAGEN6_LANES_PHYSICAL_NUM)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET,
                                                chSel,
                                                recoveryAction,
                                                cleanup);
    }

    /* TODO: need to find rate bitfield */

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPllClkSel_BfGet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    (uint8_t *)&serializerCfg->commonCfg.pllClkSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPolarity_BfGet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    &serializerCfg->commonCfg.polarity);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPowerdown_BfGet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    &serializerCfg->commonCfg.powerDown);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTx8b10bEnable_BfGet(   device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        chSel,
                                                                        &serializerCfg->commonCfg.encoder8b10bEnable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPrecursor_BfGet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    &serializerCfg->precursor);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxPostcursor_BfGet(device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    &serializerCfg->postcursor);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxDiffControl_BfGet(   device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        chSel,
                                                                        &serializerCfg->diffCtrl);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxOutClkSel_BfGet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    &serializerCfg->txOutClkSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SerializerResetSet(   adi_fpgagen6_Device_t* const    device,
                                                                    const uint8_t                   chSelMask,
                                                                    const uint8_t                   resetVal)
{
        adi_fpgagen6_ErrAction_e    recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     serializerLaneIdx   = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    for (serializerLaneIdx = 0U; serializerLaneIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; serializerLaneIdx++)
    {
        if ((chSelMask & (1U << serializerLaneIdx)) > 0U)
        {
            recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxReset_BfSet( device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        serializerLaneIdx,
                                                                        (resetVal & 0x1U));
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_RESET_SET,
                                                        chSelMask,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SerializerResetDoneGet(adi_fpgagen6_Device_t* const   device,
                                                                     uint8_t* const                 txResetDone)
{
        adi_fpgagen6_ErrAction_e    recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     serializerLaneIdx   = 0U;
    uint8_t                     chResetReadback     = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, txResetDone, cleanup);

    *txResetDone = 0U;

    for (serializerLaneIdx = 0U; serializerLaneIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; serializerLaneIdx++)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelTxResetDone_BfGet( device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        serializerLaneIdx,
                                                                        &chResetReadback);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_RESET_DONE_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        *txResetDone |= (chResetReadback << serializerLaneIdx);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeserializerCfgSet(   adi_fpgagen6_Device_t* const            device,
                                                                    const uint8_t                           chSel,
                                                                    const adi_fpgagen6_DeserializerCfg_t* const   deserializerCfg)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, deserializerCfg, cleanup);

    if (chSel >= ADI_FPGAGEN6_LANES_PHYSICAL_NUM)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_SET,
                                                chSel,
                                                recoveryAction,
                                                cleanup);
    }

    /* TODO: need to find rate bitfield */

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxPllClkSel_BfSet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    deserializerCfg->commonCfg.pllClkSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxPolarity_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    deserializerCfg->commonCfg.polarity);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxPowerdown_BfSet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    deserializerCfg->commonCfg.powerDown);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRx8b10bEnable_BfSet(   device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        chSel,
                                                                        deserializerCfg->commonCfg.encoder8b10bEnable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxDfeLpmEn_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    deserializerCfg->rxDfeLpmEn);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxOutClkSel_BfSet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    deserializerCfg->rxOutClkSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    /* TBD figure out what cdrSetting is and if it's needed */
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeserializerCfgGet(   adi_fpgagen6_Device_t* const            device,
                                                                    const uint8_t                           chSel,
                                                                    adi_fpgagen6_DeserializerCfg_t* const   deserializerCfg)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, deserializerCfg, cleanup);

    if (chSel >= ADI_FPGAGEN6_LANES_PHYSICAL_NUM)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_GET,
                                                chSel,
                                                recoveryAction,
                                                cleanup);
    }

    /* TODO: need to find rate bitfield */

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxPllClkSel_BfGet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    (uint8_t *)&deserializerCfg->commonCfg.pllClkSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxPolarity_BfGet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    &deserializerCfg->commonCfg.polarity);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxPowerdown_BfGet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    &deserializerCfg->commonCfg.powerDown);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRx8b10bEnable_BfGet(   device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        chSel,
                                                                        &deserializerCfg->commonCfg.encoder8b10bEnable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxDfeLpmEn_BfGet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    &deserializerCfg->rxDfeLpmEn);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxOutClkSel_BfGet( device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    chSel,
                                                                    &deserializerCfg->rxOutClkSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* TBD figure out what cdrSetting is and if it's needed */

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeserializerResetSet( adi_fpgagen6_Device_t* const    device,
                                                                    const uint8_t                   chSelMask,
                                                                    const uint8_t                   resetVal)
{
        adi_fpgagen6_ErrAction_e    recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     deserializerLaneIdx = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    for (deserializerLaneIdx = 0U; deserializerLaneIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; deserializerLaneIdx++)
    {
        if ((chSelMask & (1U << deserializerLaneIdx)) > 0U)
        {
            recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxReset_BfSet( device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        deserializerLaneIdx,
                                                                        (resetVal & 0x1U));
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_RESET_SET,
                                                        chSelMask,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeserializerResetDoneGet( adi_fpgagen6_Device_t* const    device,
                                                                        uint8_t* const                  rxResetDone)
{    
        adi_fpgagen6_ErrAction_e    recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     deserializerLaneIdx = 0U;
    uint8_t                     chResetReadback     = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, rxResetDone, cleanup);

    *rxResetDone = 0U;

    for (deserializerLaneIdx = 0U; deserializerLaneIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; deserializerLaneIdx++)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelRxResetDone_BfGet( device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        deserializerLaneIdx,
                                                                        &chResetReadback);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_RESET_DONE_GET,
                                                    rxResetDone,
                                                    recoveryAction,
                                                    cleanup);
        }
        *rxResetDone |= (chResetReadback << deserializerLaneIdx);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LaneXBarResetSet( adi_fpgagen6_Device_t* const                    device,
                                                                const adi_fpgagen6_LaneXBarResetMask_t* const   laneResetMaskInfo)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;
    uint8_t                     interfaceIdx    = 0U;
    uint8_t                     channelOffset   = 0U;
    uint8_t                     bfValue         = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, laneResetMaskInfo, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    if ((laneResetMaskInfo->deframerSelect  == 0U) &&
        (laneResetMaskInfo->framerSelect    == 0U))
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                laneResetMaskInfo->deframerSelect,
                                "No Deframer/Framer Selection Made");
        goto cleanup;
    }

    if (device->devStateInfo.testLoopbackFlag != 0U)
    {
        channelOffset = ADI_FPGAGEN6_LANES_PHYSICAL_NUM;
    }

    for (channelIdx = (0U + channelOffset); channelIdx < (ADI_FPGAGEN6_LANES_PHYSICAL_NUM + channelOffset); ++channelIdx)
    {
        if ((laneResetMaskInfo->deframerSelect & ADI_FPGAGEN6_DEFRAMER_0) > 0U)
        {
            bfValue = ((uint8_t) ((uint16_t) laneResetMaskInfo->deframer0 >> channelIdx));

            recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneReset_BfSet(device,
                                                                 axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_DEFRAMER0_IDX][interfaceIdx],
                                                                 channelIdx,
                                                                 bfValue);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_ERROR_REPORT(   &device->common,
                                    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER0,
                                    recoveryAction,
                                    channelIdx,
                                    "Rx Lane XBar Link 0 Reset Issue");
                goto cleanup;
            }
        }

        if ((laneResetMaskInfo->deframerSelect & ADI_FPGAGEN6_DEFRAMER_1) > 0U)
        {
            bfValue = ((uint8_t) ((uint16_t) laneResetMaskInfo->deframer1 >> channelIdx));

            recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneReset_BfSet(device,
                                                                 axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_DEFRAMER1_IDX][interfaceIdx],
                                                                 channelIdx,
                                                                 bfValue);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_ERROR_REPORT(   &device->common,
                                    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER1,
                                    recoveryAction,
                                    channelIdx,
                                    "Rx Lane XBar Link 1 Reset Issue");
                goto cleanup;
            }
        }

        if ((laneResetMaskInfo->deframerSelect & ADI_FPGAGEN6_DEFRAMER_2) > 0U)
        {
            bfValue = ((uint8_t) ((uint16_t) laneResetMaskInfo->deframer2 >> channelIdx));

            recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneReset_BfSet(device,
                                                                 axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_DEFRAMER2_IDX][interfaceIdx],
                                                                 channelIdx,
                                                                 bfValue);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_ERROR_REPORT(   &device->common,
                                    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER2,
                                    recoveryAction,
                                    channelIdx,
                                    "Rx Lane XBar Link 2 Reset Issue");
                goto cleanup;
            }
        }

        if ((laneResetMaskInfo->framerSelect & ADI_FPGAGEN6_FRAMER_0) > 0U)
        {
            bfValue = ((uint8_t) ((uint16_t) laneResetMaskInfo->framer0 >> channelIdx));

            recoveryAction = fpgagen6_AxiAdrvgen6Tx_LaneReset_BfSet(device,
                                                                 axiAdrvgen6TxChannelAddr[ADI_FPGAGEN6_FRAMER0_IDX][interfaceIdx],
                                                                 channelIdx,
                                                                 bfValue);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_ERROR_REPORT(   &device->common,
                                    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_FRAMER0,
                                    recoveryAction,
                                    channelIdx,
                                    "Tx Lane XBar Link 0 Reset Issue");
                goto cleanup;
            }
        }

        if ((laneResetMaskInfo->framerSelect & ADI_FPGAGEN6_FRAMER_1) > 0U)
        {
            bfValue = ((uint8_t) ((uint16_t) laneResetMaskInfo->framer1 >> channelIdx));

            recoveryAction = fpgagen6_AxiAdrvgen6Tx_LaneReset_BfSet(device,
                                                                 axiAdrvgen6TxChannelAddr[ADI_FPGAGEN6_FRAMER1_IDX][interfaceIdx],
                                                                 channelIdx,
                                                                 bfValue);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_ERROR_REPORT(   &device->common,
                                    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_FRAMER1,
                                    recoveryAction,
                                    channelIdx,
                                    "Tx Lane XBar Link 1 Reset Issue");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LaneXBarFullResetSet(adi_fpgagen6_Device_t* const device)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;
    uint8_t                     interfaceIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    for (interfaceIdx = 0U; interfaceIdx < 2U; interfaceIdx++)
    {
        for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; ++channelIdx)
        {
            recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneReset_BfSet(   device,
                                                                    axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_DEFRAMER0_IDX][interfaceIdx],
                                                                    channelIdx,
                                                                    ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_ERROR_REPORT(   &device->common,
                                    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER0,
                                    recoveryAction,
                                    channelIdx,
                                    "Rx Lane XBar Link 0 Reset Issue");
                goto cleanup;
            }
            
            recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneReset_BfSet(   device,
                                                                    axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_DEFRAMER1_IDX][interfaceIdx],
                                                                    channelIdx,
                                                                    ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_ERROR_REPORT(   &device->common,
                                    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER1,
                                    recoveryAction,
                                    channelIdx,
                                    "Rx Lane XBar Link 1 Reset Issue");
                goto cleanup;
            }

            recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneReset_BfSet(   device,
                                                                    axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_DEFRAMER2_IDX][interfaceIdx],
                                                                    channelIdx,
                                                                    ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_ERROR_REPORT(   &device->common,
                                    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER2,
                                    recoveryAction,
                                    channelIdx,
                                    "Rx Lane XBar Link 2 Reset Issue");
                goto cleanup;
            }

            recoveryAction = fpgagen6_AxiAdrvgen6Tx_LaneReset_BfSet(   device,
                                                                    axiAdrvgen6TxChannelAddr[ADI_FPGAGEN6_FRAMER0_IDX][interfaceIdx],
                                                                    channelIdx,
                                                                    ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_ERROR_REPORT(   &device->common,
                                    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_FRAMER0,
                                    recoveryAction,
                                    channelIdx,
                                    "Tx Lane XBar Link 0 Reset Issue");
                goto cleanup;
            }

            recoveryAction = fpgagen6_AxiAdrvgen6Tx_LaneReset_BfSet(   device,
                                                                    axiAdrvgen6TxChannelAddr[ADI_FPGAGEN6_FRAMER1_IDX][interfaceIdx],
                                                                    channelIdx,
                                                                    ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_ERROR_REPORT(   &device->common,
                                    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_FRAMER1,
                                    recoveryAction,
                                    channelIdx,
                                    "Tx Lane XBar Link 1 Reset Issue");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LaneXBarResetGet( adi_fpgagen6_Device_t* const            device,
                                                                adi_fpgagen6_LaneXBarResetMask_t* const laneResetMaskInfo)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     interfaceIdx    = 0U;
    uint8_t                     channelIdx      = 0U;
    uint8_t                     channelOffset   = 0U;
    uint8_t                     bfValue         = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, laneResetMaskInfo, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    if ((laneResetMaskInfo->deframerSelect  == 0U) &&
        (laneResetMaskInfo->framerSelect    == 0U))
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                laneResetMaskInfo->deframerSelect,
                                "No Deframer/Framer Selection Made");
        goto cleanup;
    }

    if (device->devStateInfo.testLoopbackFlag != 0U)
    {
        channelOffset = ADI_FPGAGEN6_LANES_PHYSICAL_NUM;
    }

    /* Reset Data Structure */
    laneResetMaskInfo->deframerSelect   = 0U;
    laneResetMaskInfo->framerSelect     = 0U;
    laneResetMaskInfo->deframer0        = 0U;
    laneResetMaskInfo->deframer1        = 0U;
    laneResetMaskInfo->deframer2        = 0U;
    laneResetMaskInfo->framer0          = 0U;
    laneResetMaskInfo->framer1          = 0U;

    for (channelIdx = (0U + channelOffset); channelIdx < (ADI_FPGAGEN6_LANES_PHYSICAL_NUM + channelOffset); ++channelIdx)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneReset_BfGet(   device,
                                                                axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_DEFRAMER0_IDX][interfaceIdx],
                                                                channelIdx,
                                                                &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_ERROR_REPORT(   &device->common,
                                ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER0,
                                recoveryAction,
                                channelIdx,
                                "Rx Lane XBar Link 0 Reset Get Issue");
            goto cleanup;
        }

        laneResetMaskInfo->deframer0 |= (uint16_t) (bfValue << channelIdx);

        recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneReset_BfGet(   device,
                                                                axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_DEFRAMER1_IDX][interfaceIdx],
                                                                channelIdx,
                                                                &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_ERROR_REPORT(   &device->common,
                                ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER1,
                                recoveryAction,
                                channelIdx,
                                "Rx Lane XBar Link 1 Reset Get Issue");
            goto cleanup;
        }

        laneResetMaskInfo->deframer1 |= (uint16_t) (bfValue << channelIdx);

        recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneReset_BfGet(   device,
                                                                axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_DEFRAMER2_IDX][interfaceIdx],
                                                                channelIdx,
                                                                &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_ERROR_REPORT(   &device->common,
                                ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER2,
                                recoveryAction,
                                channelIdx,
                                "Rx Lane XBar Link 2 Reset Get Issue");
            goto cleanup;
        }

        laneResetMaskInfo->deframer2 |= (uint16_t) (bfValue << channelIdx);

        recoveryAction = fpgagen6_AxiAdrvgen6Tx_LaneReset_BfGet(   device,
                                                                axiAdrvgen6TxChannelAddr[ADI_FPGAGEN6_FRAMER0_IDX][interfaceIdx],
                                                                channelIdx,
                                                                &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_ERROR_REPORT(   &device->common,
                                ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_FRAMER0,
                                recoveryAction,
                                channelIdx,
                                "Tx Lane XBar Link 0 Reset Get Issue");
            goto cleanup;
        }

        laneResetMaskInfo->framer0 |= (uint16_t) (bfValue << channelIdx);

        recoveryAction = fpgagen6_AxiAdrvgen6Tx_LaneReset_BfGet(   device,
                                                                axiAdrvgen6TxChannelAddr[ADI_FPGAGEN6_FRAMER1_IDX][interfaceIdx],
                                                                channelIdx,
                                                                &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_ERROR_REPORT(   &device->common,
                                ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_FRAMER1,
                                recoveryAction,
                                channelIdx,
                                "Tx Lane XBar Link 1 Reset Get Issue");
            goto cleanup;
        }

        laneResetMaskInfo->framer1 |= (uint16_t) (bfValue << channelIdx);
    }

    if (laneResetMaskInfo->deframer0 != 0U)
    {
        laneResetMaskInfo->deframerSelect |= (uint8_t) ADI_FPGAGEN6_DEFRAMER_0;
    }

    if (laneResetMaskInfo->deframer1 != 0U)
    {
        laneResetMaskInfo->deframerSelect |= (uint8_t) ADI_FPGAGEN6_DEFRAMER_1;
    }

    if (laneResetMaskInfo->deframer2 != 0U)
    {
        laneResetMaskInfo->deframerSelect |= (uint8_t) ADI_FPGAGEN6_DEFRAMER_2;
    }

    if (laneResetMaskInfo->framer0 != 0U)
    {
        laneResetMaskInfo->framerSelect |= (uint8_t) ADI_FPGAGEN6_FRAMER_0;
    }

    if (laneResetMaskInfo->framer1 != 0U)
    {
        laneResetMaskInfo->deframerSelect |= (uint8_t) ADI_FPGAGEN6_FRAMER_1;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerCfgGet(   adi_fpgagen6_Device_t* const        device,
                                                                const adi_fpgagen6_DeframerSel_e    deframerSel,
                                                                adi_fpgagen6_DeframerCfg_t* const   deframerCfgInfo)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, deframerCfgInfo, cleanup);

    if ((deframerSel != ADI_FPGAGEN6_DEFRAMER_0) &&
        (deframerSel != ADI_FPGAGEN6_DEFRAMER_1) &&
        (deframerSel != ADI_FPGAGEN6_DEFRAMER_2))
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                deframerSel,
                                "Invalid Deframer Selected");
        goto cleanup;
    }

    /* Configure Deframer */
    recoveryAction = fpgagen6_DeframerCfgGet( device, deframerSel, deframerCfgInfo);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerCfgSet(   adi_fpgagen6_Device_t* const            device,
                                                                const adi_fpgagen6_DeframerSel_e        deframerSel,
                                                                const adi_fpgagen6_DeframerCfg_t* const deframerCfgInfo)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, deframerCfgInfo, cleanup);

    if ((deframerSel != ADI_FPGAGEN6_DEFRAMER_0) &&
        (deframerSel != ADI_FPGAGEN6_DEFRAMER_1) &&
        (deframerSel != ADI_FPGAGEN6_DEFRAMER_2))
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                deframerSel,
                                "Invalid Deframer Selected");
        goto cleanup;
    }

    /* Configure Deframer */
    recoveryAction = fpgagen6_DeframerCfgSet( device, deframerSel, deframerCfgInfo);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_FramerCfgGet( adi_fpgagen6_Device_t* const    device,
                                                            const adi_fpgagen6_FramerSel_e  framerSel,
                                                            adi_fpgagen6_FramerCfg_t* const framerCfgInfo)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, framerCfgInfo, cleanup);

    if ((framerSel != ADI_FPGAGEN6_FRAMER_0) &&
        (framerSel != ADI_FPGAGEN6_FRAMER_1))
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                framerSel,
                                "Invalid Framer Selected");
        goto cleanup;
    }

    /* Configure Framer */
    recoveryAction = fpgagen6_FramerCfgGet(device, framerSel, framerCfgInfo);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_FramerCfgSet( adi_fpgagen6_Device_t* const            device,
                                                            const adi_fpgagen6_FramerSel_e          framerSel,
                                                            const adi_fpgagen6_FramerCfg_t* const   framerCfgInfo)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, framerCfgInfo, cleanup);

    if ((framerSel != ADI_FPGAGEN6_FRAMER_0) &&
        (framerSel != ADI_FPGAGEN6_FRAMER_1))
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                framerSel,
                                "Invalid Framer Selected");
        goto cleanup;
    }

    /* Configure Framer */
    recoveryAction = fpgagen6_FramerCfgSet(device, framerSel, framerCfgInfo);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerResetSet( adi_fpgagen6_Device_t* const        device,
                                                                const adi_fpgagen6_DeframerSel_e    deframerSel)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     deframerIdx     = 0U;
    uint8_t                     interfaceIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    FPGAGEN6_DEFRAMER_IDX_SET_GOTO(deframerSel, deframerIdx, recoveryAction, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    recoveryAction = fpgagen6_XilinxJesd_ResetFixed_BfSet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            ADI_ENABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerResetDonePoll(adi_fpgagen6_Device_t* const        device,
                                                                    const adi_fpgagen6_DeframerSel_e    deframerSel)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     deframerIdx     = 0U;
    uint8_t                     interfaceIdx    = 0U;
    uint8_t                     resetBit        = 1U;   /* Initial Condition of Reset Bit after released until process complete */
    uint8_t                     i               = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    FPGAGEN6_DEFRAMER_IDX_SET_GOTO(deframerSel, deframerIdx, recoveryAction, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    if (ADI_FPGAGEN6_ERR_ACT_NONE != fpgagen6_XilinxJesd_ResetFixed_BfSet(  device,
                                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                            ADI_DISABLE))
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_RELEASE,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    for (i = 0U; i < ADI_FPGAGEN6_RESET_FIXED_LOOP; ++i)
    {
        if ((resetBit & 0x1U) > 0U)
        {
            /* Once Released; Reset Bit is set to 1 until Reset Process is complete */
            if (ADI_FPGAGEN6_ERR_ACT_NONE != fpgagen6_XilinxJesd_ResetFixed_BfGet(  device,
                                                                                    xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                                    &resetBit))
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_POLL,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            if (ADI_COMMON_ERR_ACT_NONE != adi_common_hal_Wait_ms(&device->common, ADI_FPGAGEN6_RESET_POLL_STEP))
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_POLL,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
        else
        {
            break;
        }
    }

    if (((resetBit & 0x1U) == 0U) &&
        (i < ADI_FPGAGEN6_RESET_FIXED_LOOP))
    {
        /* Reset Bit has been cleared within the required time */
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_POLL,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_FramerResetSet( adi_fpgagen6_Device_t* const      device,
                                                               const adi_fpgagen6_FramerSel_e   framerSel)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     framerIdx       = 0U;
    uint8_t                     interfaceIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    FPGAGEN6_FRAMER_IDX_SET_GOTO(framerSel, framerIdx, recoveryAction, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    recoveryAction = fpgagen6_XilinxJesd_ResetFixed_BfSet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            ADI_ENABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_RESET_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_FramerResetDonePoll(  adi_fpgagen6_Device_t* const    device,
                                                                    const adi_fpgagen6_FramerSel_e  framerSel)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     framerIdx       = 0U;
    uint8_t                     interfaceIdx    = 0U;
    uint8_t                     resetBit        = 1U;   /* Initial Condition of Reset Bit after released until process complete */
    uint8_t                     i               = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    FPGAGEN6_FRAMER_IDX_SET_GOTO(framerSel, framerIdx, recoveryAction, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    if (ADI_FPGAGEN6_ERR_ACT_NONE != fpgagen6_XilinxJesd_ResetFixed_BfSet(  device,
                                                                                xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                                ADI_DISABLE))
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_RELEASE,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    for (i = 0U; i < ADI_FPGAGEN6_RESET_FIXED_LOOP; ++i)
    {
        if ((resetBit & 0x1U) > 0U)
        {
            /* Once Released; Reset Bit is set to 1 until Reset Process is complete */
            if (ADI_FPGAGEN6_ERR_ACT_NONE != fpgagen6_XilinxJesd_ResetFixed_BfGet(  device,
                                                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                                        &resetBit))
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_RESET_POLL,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            if (ADI_COMMON_ERR_ACT_NONE != adi_common_hal_Wait_ms(&device->common, ADI_FPGAGEN6_RESET_POLL_STEP))
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_RESET_POLL,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
        else
        {
            break;
        }
    }

    if (((resetBit & 0x1U) == 0U) &&
    (i < ADI_FPGAGEN6_RESET_FIXED_LOOP))
    {
        /* Reset Bit has been cleared within the required time */
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_RESET_POLL,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SysRefControlSet( adi_fpgagen6_Device_t* const                device,
                                                                const adi_fpgagen6_SysRefClkCfg_t* const    sysRefCtrlCfg)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, sysRefCtrlCfg, cleanup);

    recoveryAction = fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefPolarity_BfSet( device,
                                                                                FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL,
                                                                                sysRefCtrlCfg->extSysRefPolarity);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefSampleEdge_BfSet(   device,
                                                                                    FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL,
                                                                                    sysRefCtrlCfg->extSysRefSampleEdge);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6SystemTimingControl_IntSysrefEnable_BfSet(   device,
                                                                                FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL,
                                                                                sysRefCtrlCfg->intSysRefEnable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6SystemTimingControl_IntSysrefGenPeriod_BfSet(device,
                                                                                FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL,
                                                                                (uint16_t) (sysRefCtrlCfg->intSysRefGenPeriod - 1U));
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SysRefControlGet( adi_fpgagen6_Device_t* const        device,
                                                                adi_fpgagen6_SysRefClkCfg_t* const  sysRefCtrlCfg)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, sysRefCtrlCfg, cleanup);

    recoveryAction = fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefPolarity_BfGet( device,
                                                                                FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL,
                                                                                &sysRefCtrlCfg->extSysRefPolarity);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefSampleEdge_BfGet(   device,
                                                                                    FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL,
                                                                                    &sysRefCtrlCfg->extSysRefSampleEdge);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6SystemTimingControl_IntSysrefEnable_BfGet(   device,
                                                                                FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL,
                                                                                &sysRefCtrlCfg->intSysRefEnable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6SystemTimingControl_IntSysrefGenPeriod_BfGet(device,
                                                                                FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL,
                                                                                &sysRefCtrlCfg->intSysRefGenPeriod);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    sysRefCtrlCfg->intSysRefGenPeriod = (uint16_t) (sysRefCtrlCfg->intSysRefGenPeriod + 1U);

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SysRefEnableSet(  adi_fpgagen6_Device_t* const    device,
                                                                const uint8_t                   framerSelMask,
                                                                const uint8_t                   deframerSelMask,
                                                                const uint8_t                   enable)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;
    uint8_t                     interfaceIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    if ((framerSelMask  == 0U) &&
        (deframerSelMask    == 0U))
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_ENABLE_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    if (enable > 1U)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_ENABLE_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_MAX_DEFRAMERS; channelIdx++)
    {
        if ((deframerSelMask & (1U << channelIdx)) > 0U)
        {
            recoveryAction = fpgagen6_AxiAdrvgen6Rx_SysrefEnable_BfSet(device,
                                                                    axiAdrvgen6RxChannelAddr[channelIdx][interfaceIdx],
                                                                    enable);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_ENABLE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_MAX_FRAMERS; channelIdx++)
    {
        if ((framerSelMask & (1U << channelIdx)) > 0U)
        {
            recoveryAction = fpgagen6_AxiAdrvgen6Tx_SysrefEnable_BfSet(device,
                                                                    axiAdrvgen6TxChannelAddr[channelIdx][interfaceIdx],
                                                                    enable);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_ENABLE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_InternalSysrefEnableSet(  adi_fpgagen6_Device_t* const    device,
                                                                        const uint8_t                   enable)
{
        adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    if (enable > 1U)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_INTERNAL_SYSREF_ENABLE_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_Adrvgen6SystemTimingControl_IntSysrefEnable_BfSet(   device,
                                                                                FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL,
                                                                                enable);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_INTERNAL_SYSREF_ENABLE_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_InternalSysrefRequest(adi_fpgagen6_Device_t* const device)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    recoveryAction = fpgagen6_Adrvgen6SystemTimingControl_IntSysrefRequest_BfSet(  device,
                                                                                FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_CONTROL,
                                                                                ADI_ENABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_INTERNAL_SYSREF_REQUEST,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LinkStatusGet(adi_fpgagen6_Device_t* const        device,
                                                            const adi_fpgagen6_DeframerSel_e    deframerSel,
                                                            uint8_t* const                      status)
{
        adi_fpgagen6_ErrAction_e    recoveryAction          = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     deframerIdx             = 0U;
    uint8_t                     interfaceIdx            = 0U;
    uint8_t                     mbLockReadback          = 0U;
    uint8_t                     syncHeaderLockReadback  = 0U;
    uint8_t                     bufferOverflowReadback  = 0U;
    uint8_t                     rxStarted8b10bReadback  = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, status, cleanup);

    FPGAGEN6_DEFRAMER_IDX_SET_GOTO(deframerSel, deframerIdx, recoveryAction, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    *status = 0U;
    if (device->devStateInfo.interfaceType == ADI_FPGAGEN6_INTERFACE_JESD204B)
    {
        recoveryAction = fpgagen6_XilinxJesd_RxStarted8b10b_BfGet( device,
                                                                    xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                    &rxStarted8b10bReadback);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        if (rxStarted8b10bReadback != 0U)
        {
            *status |= (1U << ADI_FPGAGEN6_LINK_STATUS_BIT);
        }
    }
    else if (device->devStateInfo.interfaceType == ADI_FPGAGEN6_INTERFACE_JESD204C)
    {
        recoveryAction = fpgagen6_XilinxJesd_MbLock_BfGet( device,
                                                           xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                           &mbLockReadback);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        recoveryAction = fpgagen6_XilinxJesd_SyncHeaderLock_BfGet(  device,
                                                                    xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                    &syncHeaderLockReadback);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        if ((mbLockReadback != 0U) && (syncHeaderLockReadback != 0U))
        {
            *status |= (1U << ADI_FPGAGEN6_LINK_STATUS_BIT);
        }
        
        recoveryAction = fpgagen6_XilinxJesd_BufferOverflow_BfGet(  device,
                                                                    xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                    &bufferOverflowReadback);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        if (bufferOverflowReadback != 0U)
        {
            *status |= (1U << ADI_FPGAGEN6_BUFFER_OVERFLOW_STATUS_BIT);
        }
    }
    else
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SyncXBarCfgSet(   adi_fpgagen6_Device_t* const            device,
                                                                const adi_fpgagen6_SyncXBar_t* const    syncXBarCfgSet)
{
        adi_fpgagen6_ErrAction_e    recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     interfaceIdx        = 0U;
    uint8_t                     i                   = 0U;
    uint8_t                     bfValue             = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, syncXBarCfgSet, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    for (i = 0U; i < ADI_FPGAGEN6_MAX_DEFRAMERS; ++i)
    {
        ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, syncF2pSelect_BfSetFnPtrs[i], cleanup);

        ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, syncF2pPolarity_BfSetFnPtrs[i], cleanup);

        /* Select */

        if (syncXBarCfgSet->deframer[i].select > ADI_FPGAGEN6_SYNCOUT_SYNCINB1)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, syncXBarCfgSet->deframer[i].select, recoveryAction, "Invalid Deframer Sync Out Selection");
            goto cleanup;
        }

        bfValue = (uint8_t) syncXBarCfgSet->deframer[i].select;

        recoveryAction = syncF2pSelect_BfSetFnPtrs[i](  device,
                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                        bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        /* Polarity */

        if (syncXBarCfgSet->deframer[i].polarity > ADI_FPGAGEN6_SYNCPOLARITY_INVERT)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, syncXBarCfgSet->deframer[i].polarity, recoveryAction, "Invalid Deframer Polarity Selection");
            goto cleanup;
        }

        bfValue = (uint8_t) syncXBarCfgSet->deframer[i].polarity;

        recoveryAction = syncF2pPolarity_BfSetFnPtrs[i](device,
                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                        bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    for (i = 0U; i < ADI_FPGAGEN6_MAX_FRAMERS; ++i)
    {
        ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, syncP2fPolarity_BfSetFnPtrs[i], cleanup);

        if (syncXBarCfgSet->framer[i].select > ADI_FPGAGEN6_SYNCSRC_DEFRAMER2)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, syncXBarCfgSet->framer[i].select, recoveryAction, "Invalid Framer SyncB Selection");
            goto cleanup;
        }

        bfValue = (uint8_t) syncXBarCfgSet->framer[i].select;

        recoveryAction = fpgagen6_AxiAdrvgen6Tx_SyncbSourceSel_BfSet(  device,
                                                                    axiAdrvgen6TxChannelAddr[i][interfaceIdx],
                                                                    bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        if (syncXBarCfgSet->framer[i].polarity > ADI_FPGAGEN6_SYNCPOLARITY_INVERT)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, syncXBarCfgSet->framer[i].polarity, recoveryAction, "Invalid Framer Polarity Selection");
            goto cleanup;
        }

        bfValue = (uint8_t) syncXBarCfgSet->framer[i].polarity;

        recoveryAction = syncP2fPolarity_BfSetFnPtrs[i](device,
                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                        bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SyncXBarCfgGet(   adi_fpgagen6_Device_t* const    device,
                                                                adi_fpgagen6_SyncXBar_t* const  syncXBarCfgGet)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     interfaceIdx    = 0U;
    uint8_t                     i               = 0U;
    uint8_t                     bfValue         = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, syncXBarCfgGet, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    for (i = 0U; i < ADI_FPGAGEN6_MAX_DEFRAMERS; ++i)
    {
        ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, syncF2pSelect_BfGetFnPtrs[i], cleanup);

        ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, syncF2pPolarity_BfGetFnPtrs[i], cleanup);

        /* Select */

        recoveryAction = syncF2pSelect_BfGetFnPtrs[i](  device,
                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        syncXBarCfgGet->deframer[i].select = (adi_fpgagen6_SyncOutSel_e) bfValue;

        /* Polarity */

        recoveryAction = syncF2pPolarity_BfGetFnPtrs[i](device,
                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        syncXBarCfgGet->deframer[i].polarity = (adi_fpgagen6_SyncPolaritySel_e) bfValue;
    }

    for (i = 0U; i < ADI_FPGAGEN6_MAX_FRAMERS; ++i)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Tx_SyncbSourceSel_BfGet(  device,
                                                                    axiAdrvgen6TxChannelAddr[i][interfaceIdx],
                                                                    &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        syncXBarCfgGet->framer[i].select = (adi_fpgagen6_SyncSrcSel_e) bfValue;

        ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, syncP2fPolarity_BfGetFnPtrs[i], cleanup);

        recoveryAction = syncP2fPolarity_BfGetFnPtrs[i](device,
                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        syncXBarCfgGet->framer[i].polarity = (adi_fpgagen6_SyncPolaritySel_e) bfValue;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerCmdDataSet(adi_fpgagen6_Device_t* const device,
                                                                const adi_fpgagen6_DeframerSel_e  deframerSel,
                                                                const uint8_t                cmdEnable,
                                                                const uint8_t                dataEnable)
{    adi_fpgagen6_ErrAction_e    recoveryAction          = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     deframerIdx             = 0U;
    uint8_t                     interfaceIdx            = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    FPGAGEN6_DEFRAMER_IDX_SET_GOTO(deframerSel, deframerIdx, recoveryAction, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    if (device->devStateInfo.interfaceType == ADI_FPGAGEN6_INTERFACE_JESD204B)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else if (device->devStateInfo.interfaceType == ADI_FPGAGEN6_INTERFACE_JESD204C)
    {
        recoveryAction = fpgagen6_XilinxJesd_EnableCmdInterface_BfSet(  device,
                                                                        xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                        cmdEnable);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        recoveryAction = fpgagen6_XilinxJesd_EnableDataInterface_BfSet(  device,
                                                                        xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                        dataEnable);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    else
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e  adi_fpgagen6_FramerCmdDataSet(adi_fpgagen6_Device_t* const device,
                                                                const adi_fpgagen6_FramerSel_e  framerSel,
                                                                const uint8_t                cmdEnable,
                                                                const uint8_t                dataEnable)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     framerIdx       = 0U;
    uint8_t                     interfaceIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    FPGAGEN6_FRAMER_IDX_SET_GOTO(framerSel, framerIdx, recoveryAction, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    if (device->devStateInfo.interfaceType == ADI_FPGAGEN6_INTERFACE_JESD204B)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else if (device->devStateInfo.interfaceType == ADI_FPGAGEN6_INTERFACE_JESD204C)
    {
        recoveryAction = fpgagen6_XilinxJesd_EnableCmdInterface_BfSet(  device,
                                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                        cmdEnable);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        recoveryAction = fpgagen6_XilinxJesd_EnableDataInterface_BfSet(  device,
                                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                        dataEnable);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    else
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}