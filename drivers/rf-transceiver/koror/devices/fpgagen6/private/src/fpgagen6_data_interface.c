/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/*!
* \file fpgagen6_test.c
*
* \brief    Test Interface Functions for the FPGAGEN6 Device
*
* FPGA API Version: 2.9.0.4
*/
#include "../../private/include/fpgagen6_data_interface.h"

#include "adi_library_types.h"
#include "adi_fpgagen6_data_interface.h"


#define ADI_FILE    ADI_FPGAGEN6_FILE_PRIVATE_DATA_INTERFACE

/**
 *  \brief 
 */
const fpgagen6_BfAxiAdrvgen6RxChanAddr_e axiAdrvgen6RxChannelAddr[ADI_FPGAGEN6_MAX_DEFRAMERS][ADI_FPGAGEN6_INTERFACE_TYPES_NUM] = 
{
    { FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_0, FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_0 },
    { FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_1, FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_1 },
    { FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_2, FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_2 }
};

/**
 *  \brief 
 */
const fpgagen6_BfAxiAdrvgen6TxChanAddr_e axiAdrvgen6TxChannelAddr[ADI_FPGAGEN6_MAX_FRAMERS][ADI_FPGAGEN6_INTERFACE_TYPES_NUM] = 
{
    { FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204B_0, FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204C_0 },
    { FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204B_1, FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204C_1 }
};

/**
 *  \brief
 */
const fpgagen6_BfXilinxJesdChanAddr_e xilinxJesdDeframerAddr[ADI_FPGAGEN6_MAX_DEFRAMERS][ADI_FPGAGEN6_INTERFACE_TYPES_NUM] = 
{
    { FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_DEFRAMER_0, FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_DEFRAMER_0 },
    { FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_DEFRAMER_1, FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_DEFRAMER_1 },
    { FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204B_DEFRAMER_2, FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_RX_204C_DEFRAMER_2 }
};

/**
 *  \brief
 */
const fpgagen6_BfXilinxJesdChanAddr_e xilinxJesdFramerAddr[ADI_FPGAGEN6_MAX_FRAMERS][ADI_FPGAGEN6_INTERFACE_TYPES_NUM] = 
{
    { FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204B_FRAMER_0, FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204C_FRAMER_0 },
    { FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204B_FRAMER_1, FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_TX_204C_FRAMER_1 }
};

/**
 *  \brief Sync Select F2P Set Functions
 */
const SyncF2pSelectSetFnPtr_t syncF2pSelect_BfSetFnPtrs[ADI_FPGAGEN6_MAX_DEFRAMERS] =
{
    fpgagen6_AxiAdrvgen6Phy_Syncb0F2pSelect_BfSet,
    fpgagen6_AxiAdrvgen6Phy_Syncb1F2pSelect_BfSet,
    fpgagen6_AxiAdrvgen6Phy_Syncb2F2pSelect_BfSet
};

/**
 *  \brief Sync Select F2P Get Functions
 */
const SyncF2pSelectGetFnPtr_t syncF2pSelect_BfGetFnPtrs[ADI_FPGAGEN6_MAX_DEFRAMERS] =
{
    fpgagen6_AxiAdrvgen6Phy_Syncb0F2pSelect_BfGet,
    fpgagen6_AxiAdrvgen6Phy_Syncb1F2pSelect_BfGet,
    fpgagen6_AxiAdrvgen6Phy_Syncb2F2pSelect_BfGet
};

/**
 *  \brief Sync Polarity F2P Set Functions
 */
const SyncF2pPolaritySetFnPtr_t syncF2pPolarity_BfSetFnPtrs[ADI_FPGAGEN6_MAX_DEFRAMERS] =
{
    fpgagen6_AxiAdrvgen6Phy_Syncb0F2pPolarity_BfSet,
    fpgagen6_AxiAdrvgen6Phy_Syncb1F2pPolarity_BfSet,
    fpgagen6_AxiAdrvgen6Phy_Syncb2F2pPolarity_BfSet

};

/**
 *  \brief Sync Polarity F2P Get Functions
 */
const SyncF2pPolarityGetFnPtr_t syncF2pPolarity_BfGetFnPtrs[ADI_FPGAGEN6_MAX_DEFRAMERS] =
{
    fpgagen6_AxiAdrvgen6Phy_Syncb0F2pPolarity_BfGet,
    fpgagen6_AxiAdrvgen6Phy_Syncb1F2pPolarity_BfGet,
    fpgagen6_AxiAdrvgen6Phy_Syncb2F2pPolarity_BfGet
};

/**
 *  \brief Sync Polarity P2F Set Functions
 */
const SyncP2fPolaritySetFnPtr_t syncP2fPolarity_BfSetFnPtrs[ADI_FPGAGEN6_MAX_FRAMERS] =
{
    fpgagen6_AxiAdrvgen6Phy_Syncb0P2fPolarity_BfSet,
    fpgagen6_AxiAdrvgen6Phy_Syncb1P2fPolarity_BfSet
};

/**
 *  \brief Sync Polarity P2F Set Functions
 */
const SyncP2fPolarityGetFnPtr_t syncP2fPolarity_BfGetFnPtrs[ADI_FPGAGEN6_MAX_FRAMERS] =
{
    fpgagen6_AxiAdrvgen6Phy_Syncb0P2fPolarity_BfGet,
    fpgagen6_AxiAdrvgen6Phy_Syncb1P2fPolarity_BfGet
};

ADI_API uint8_t fpgagen6_LCalculate(const uint8_t lanesEnabled)
{
    uint8_t i = 0U;
    uint8_t laneCount = 0U;

    for (i = 0U; i < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; ++i)
    {
        laneCount += ((lanesEnabled >> i) & 0x1U);
    }

    return laneCount;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_RxLaneXBarCfgGet( adi_fpgagen6_Device_t* const        device,
                                                            const adi_fpgagen6_DeframerSel_e    deframerSel,
                                                            uint8_t                             laneXBar[])
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;
    uint8_t                     deframerIdx     = 0U;
    uint8_t                     interfaceIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, laneXBar);

    FPGAGEN6_DEFRAMER_IDX_SET_RETURN(deframerSel, deframerIdx, recoveryAction);

    FPGAGEN6_INTERFACE_IDX_SET_RETURN(device->devStateInfo.interfaceType, recoveryAction);

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_LANES_DEFRAMER_MAX; ++channelIdx)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Rx_LogicalLaneSel_BfGet(  device,
                                                                    axiAdrvgen6RxChannelAddr[deframerIdx][interfaceIdx],
                                                                    channelIdx,
                                                                    &laneXBar[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_RX_LANEXBAR_CFG_GET,
                                                        channelIdx,
                                                        recoveryAction);
        }
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_RxLaneXBarCfgSet( adi_fpgagen6_Device_t* const        device,
                                                            const adi_fpgagen6_DeframerSel_e    deframerSel,
                                                            const uint8_t                       laneXBar[])
{        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;
    uint8_t                     deframerIdx     = 0U;
    uint8_t                     interfaceIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, laneXBar);

    FPGAGEN6_DEFRAMER_IDX_SET_RETURN(deframerSel, deframerIdx, recoveryAction);

    FPGAGEN6_INTERFACE_IDX_SET_RETURN(device->devStateInfo.interfaceType, recoveryAction);

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_LANES_DEFRAMER_MAX; ++channelIdx)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Rx_LogicalLaneSel_BfSet(  device,
                                                                    axiAdrvgen6RxChannelAddr[deframerIdx][interfaceIdx],
                                                                    channelIdx,
                                                                    laneXBar[channelIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                      ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_RX_LANEXBAR_CFG_SET,
                                                      channelIdx,
                                                      recoveryAction);
        }
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TxLaneXBarCfgGet( adi_fpgagen6_Device_t* const    device,
                                                            const adi_fpgagen6_FramerSel_e  framerSel,
                                                            uint8_t                             laneXBar[])
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;
    uint8_t                     channelOffset   = 0U;
    uint8_t                     cfgIdx          = 0U;
    uint8_t                     framerIdx       = 0U;
    uint8_t                     interfaceIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, laneXBar);

    FPGAGEN6_FRAMER_IDX_SET_RETURN(framerSel, framerIdx, recoveryAction);

    FPGAGEN6_INTERFACE_IDX_SET_RETURN(device->devStateInfo.interfaceType, recoveryAction);

    if (device->devStateInfo.testLoopbackFlag != 0U)
    {
        channelOffset = ADI_FPGAGEN6_LANES_PHYSICAL_NUM;
    }

    for (channelIdx = (0U + channelOffset); channelIdx < (ADI_FPGAGEN6_LANES_PHYSICAL_NUM + channelOffset); ++channelIdx)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Tx_PhysicalLaneSel_BfGet( device,
                                                                    axiAdrvgen6TxChannelAddr[framerIdx][interfaceIdx],
                                                                    channelIdx,
                                                                    &laneXBar[cfgIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_TX_LANEXBAR_CFG_GET,
                                                        channelIdx,
                                                        recoveryAction);
        }

        ++cfgIdx;
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_TxLaneXBarCfgSet( adi_fpgagen6_Device_t* const            device,
                                                            const adi_fpgagen6_FramerSel_e          framerSel,
                                                            const uint8_t                       laneXBar[])
{        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;
    uint8_t                     channelOffset   = 0U;
    uint8_t                     cfgIdx          = 0U;
    uint8_t                     framerIdx       = 0U;
    uint8_t                     interfaceIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, laneXBar);

    FPGAGEN6_FRAMER_IDX_SET_RETURN(framerSel, framerIdx, recoveryAction);

    FPGAGEN6_INTERFACE_IDX_SET_RETURN(device->devStateInfo.interfaceType, recoveryAction);

    if (device->devStateInfo.testLoopbackFlag != 0U)
    {
        channelOffset = ADI_FPGAGEN6_LANES_PHYSICAL_NUM;
    }

    for (channelIdx = (0U + channelOffset); channelIdx < (ADI_FPGAGEN6_LANES_PHYSICAL_NUM + channelOffset); ++channelIdx)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Tx_PhysicalLaneSel_BfSet( device,
                                                                    axiAdrvgen6TxChannelAddr[framerIdx][interfaceIdx],
                                                                    channelIdx,
                                                                    laneXBar[cfgIdx]);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                        ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_TX_LANEXBAR_CFG_SET,
                                                        channelIdx,
                                                        recoveryAction);
        }

        ++cfgIdx;
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_DeframerCfgGet(   adi_fpgagen6_Device_t* const        device,
                                                            const adi_fpgagen6_DeframerSel_e    deframerSel,
                                                            adi_fpgagen6_DeframerCfg_t* const   deframerCfgInfo)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     channelIdx      = 0U;
    uint8_t                     cfgIdx          = 0U;
    uint8_t                     deframerIdx     = 0U;
    uint8_t                     interfaceIdx    = 0U;
    uint8_t                     bfValue         = 0U;
    uint8_t                     channelOffset   = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, deframerCfgInfo);

    FPGAGEN6_DEFRAMER_IDX_SET_RETURN(deframerSel, deframerIdx, recoveryAction);

    FPGAGEN6_INTERFACE_IDX_SET_RETURN(device->devStateInfo.interfaceType, recoveryAction);

    /* Common Interface Configuration */

    /* Get Rx Lane XBar */
    recoveryAction = fpgagen6_RxLaneXBarCfgGet( device,
                                                deframerSel,
                                                deframerCfgInfo->laneXBar);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_RX_LANEXBAR_CFG_GET,
                                                    deframerSel,
                                                    recoveryAction);
    }

    /* Get Subclass */
    recoveryAction = fpgagen6_XilinxJesd_CfgSubclassv_BfGet(device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            &deframerCfgInfo->subclassV);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    /* Get Lanes In Use */
    recoveryAction = fpgagen6_XilinxJesd_LaneEnable_BfGet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            &deframerCfgInfo->lanesInUse);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    /* Get Rx Buffer Delay */
    recoveryAction = fpgagen6_XilinxJesd_RxBufferDelay_BfGet(   device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                &deframerCfgInfo->ctrlRxBufAdv);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }


    if (ADI_FPGAGEN6_INTERFACE_JESD204B == device->devStateInfo.interfaceType)
    {
        /* Get ILA Multiframes */
        recoveryAction = fpgagen6_XilinxJesd_IlaMultiFrames_BfGet(  device,
                                                                    xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                    &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        deframerCfgInfo->ilaMultiFrames = (uint16_t) (bfValue + 1U);

        /* Get ILA Support */
        recoveryAction = fpgagen6_XilinxJesd_CfgIla_BfGet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            &deframerCfgInfo->ilaSupportEn);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Scrambling */
        recoveryAction = fpgagen6_XilinxJesd_CfgScr_BfGet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            &deframerCfgInfo->scramblingEn);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get SYSREF Always */
        recoveryAction = fpgagen6_XilinxJesd_SysrefAlways_BfGet(device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                &deframerCfgInfo->sysRefAlways);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get SYSREF Delay */
        recoveryAction = fpgagen6_XilinxJesd_SysrefDelay_BfGet( device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                &deframerCfgInfo->sysRefDelay);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get SYSREF Required On Resync */
        recoveryAction = fpgagen6_XilinxJesd_SysrefOnResync_BfGet(  device,
                                                                    xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                    &deframerCfgInfo->sysRefRequiredOnResync);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Octets per Frame (F) */
        recoveryAction = fpgagen6_XilinxJesd_CfgF_BfGet(device,
                                                        xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        deframerCfgInfo->cfgF = (bfValue + 1U);

        /* Get Frames per Multiframe (K) */
        recoveryAction = fpgagen6_XilinxJesd_CfgK_BfGet(device,
                                                        xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        deframerCfgInfo->cfgK = (bfValue + 1U);

        /* Get Error Report using Sync Disable */
        recoveryAction = fpgagen6_XilinxJesd_CfgErr_BfGet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            &deframerCfgInfo->errReportingEnable);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        if (device->devStateInfo.testLoopbackFlag != 0U)
        {
            channelOffset = ADI_FPGAGEN6_LANES_PHYSICAL_NUM;
        }

        /* Reading ILA Data per Lane */
        for (channelIdx = (0U + channelOffset); channelIdx < (ADI_FPGAGEN6_LANES_PHYSICAL_NUM + channelOffset); ++channelIdx)
        {
            /* Get Bank ID */
            recoveryAction = fpgagen6_XilinxJesd_IlaBid_BfGet(  device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                channelIdx,
                                                                &deframerCfgInfo->bankId[cfgIdx]);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                            ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction);
            }

            /* Get Lane ID */
            recoveryAction = fpgagen6_XilinxJesd_IlaLid_BfGet(  device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                channelIdx,
                                                                &deframerCfgInfo->laneId[cfgIdx]);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                            ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction);
            }

            /* Get Samples per Converter per Frame */
            recoveryAction = fpgagen6_XilinxJesd_IlaS_BfGet(device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            channelIdx,
                                                            &bfValue);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                            ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction);
            }

            deframerCfgInfo->ilaS[channelIdx] = (bfValue + 1U);

            /* Get Converters per Device */
            recoveryAction = fpgagen6_XilinxJesd_IlaM_BfGet(device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            channelIdx,
                                                            &bfValue);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                            ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction);
            }

            deframerCfgInfo->ilaM[channelIdx] = (bfValue + 1U);

            /* Get Converter Resolution */
            recoveryAction = fpgagen6_XilinxJesd_IlaN_BfGet(device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            channelIdx,
                                                            &bfValue);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                            ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction);
            }

            deframerCfgInfo->ilaN[channelIdx] = (bfValue + 1U);

            /* Get Total Bits per Sample */
            recoveryAction = fpgagen6_XilinxJesd_IlaNp_BfGet(   device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                channelIdx,
                                                                &bfValue);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                            ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction);
            }

            deframerCfgInfo->ilaNp[channelIdx] = (bfValue + 1U);

            ++cfgIdx;
        }
    }

    if (ADI_FPGAGEN6_INTERFACE_JESD204C == device->devStateInfo.interfaceType)
    {
        /* Get Command Interface */
        recoveryAction = fpgagen6_XilinxJesd_EnableCmdInterface_BfGet(  device,
                                                                        xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                        &deframerCfgInfo->enableCmdInterface);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Data Interface */
        recoveryAction = fpgagen6_XilinxJesd_EnableDataInterface_BfGet( device,
                                                                        xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                        &deframerCfgInfo->enableDataInterface);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Number of Multi Blocks in an Extended Multi Block */
        recoveryAction = fpgagen6_XilinxJesd_CfgMbInEmb_BfGet(  device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                &deframerCfgInfo->cfgE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Meta Mode */
        recoveryAction = fpgagen6_XilinxJesd_MetaMode_BfGet(device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            &deframerCfgInfo->ctrlMetaMode);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Multiblock Threshold */
        recoveryAction = fpgagen6_XilinxJesd_RxMbTh_BfGet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            (&deframerCfgInfo->ctrlRxMBlockTh - 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
        
        /* Get the lane buffer delay */
        recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneBufferDelay_BfGet( device,
                                                                    axiAdrvgen6RxChannelAddr[deframerIdx][interfaceIdx],
                                                                    &deframerCfgInfo->laneBufferDelay);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_DeframerCfgSet(   adi_fpgagen6_Device_t* const            device,
                                                            const adi_fpgagen6_DeframerSel_e        deframerSel,
                                                            const adi_fpgagen6_DeframerCfg_t* const deframerCfgInfo)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     deframerIdx     = 0U;
    uint8_t                     interfaceIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, deframerCfgInfo);

    FPGAGEN6_DEFRAMER_IDX_SET_RETURN(deframerSel, deframerIdx, recoveryAction);

    FPGAGEN6_INTERFACE_IDX_SET_RETURN(device->devStateInfo.interfaceType, recoveryAction);

    /* Common Interface Configuration First */

    /* Configure Lane XBar */
    recoveryAction = fpgagen6_RxLaneXBarCfgSet( device,
                                                deframerSel,
                                                deframerCfgInfo->laneXBar);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_RX_LANEXBAR_CFG_SET,
                                                    deframerSel,
                                                    recoveryAction);
    }

     /* Configure Lanes In Use */
    recoveryAction = fpgagen6_XilinxJesd_LaneEnable_BfSet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            deframerCfgInfo->lanesInUse);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    /* Configure Subclass */
    recoveryAction = fpgagen6_XilinxJesd_CfgSubclassv_BfSet(device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            deframerCfgInfo->subclassV);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    /* Configure Rx Buffer Delay */
    recoveryAction = fpgagen6_XilinxJesd_RxBufferDelay_BfSet(   device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                deframerCfgInfo->ctrlRxBufAdv);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    if (ADI_FPGAGEN6_INTERFACE_JESD204B == device->devStateInfo.interfaceType)
    {
        /* Set ILA Multiframes */
        recoveryAction = fpgagen6_XilinxJesd_IlaMultiFrames_BfSet(  device,
                                                                    xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                    deframerCfgInfo->ilaMultiFrames -1U);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure ILA Support */
        recoveryAction = fpgagen6_XilinxJesd_CfgIla_BfSet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            deframerCfgInfo->ilaSupportEn);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Scrambling */
        recoveryAction = fpgagen6_XilinxJesd_CfgScr_BfSet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            deframerCfgInfo->scramblingEn);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure SYSREF Always */
        recoveryAction = fpgagen6_XilinxJesd_SysrefAlways_BfSet(device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                deframerCfgInfo->sysRefAlways);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure SYSREF Delay */
        recoveryAction = fpgagen6_XilinxJesd_SysrefDelay_BfSet( device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                deframerCfgInfo->sysRefDelay);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure SYSREF Required On Resync */
        recoveryAction = fpgagen6_XilinxJesd_SysrefOnResync_BfSet(  device,
                                                                    xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                    deframerCfgInfo->sysRefRequiredOnResync);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Octets per Frame (F) */
        recoveryAction = fpgagen6_XilinxJesd_CfgF_BfSet(device,
                                                        xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                        (deframerCfgInfo->cfgF - 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Frames per Multiframe (K) */
        recoveryAction = fpgagen6_XilinxJesd_CfgK_BfSet(device,
                                                        xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                        (deframerCfgInfo->cfgK - 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Error Report using Sync Disable */
        recoveryAction = fpgagen6_XilinxJesd_CfgErr_BfSet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            deframerCfgInfo->errReportingEnable);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
    }

    if (ADI_FPGAGEN6_INTERFACE_JESD204C == device->devStateInfo.interfaceType)
    {
        /* Configure Number of Multi Blocks in an Extended Multi Block */
        recoveryAction = fpgagen6_XilinxJesd_CfgMbInEmb_BfSet(  device,
                                                                xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                                deframerCfgInfo->cfgE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Meta Mode */
        recoveryAction = fpgagen6_XilinxJesd_MetaMode_BfSet(device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            deframerCfgInfo->ctrlMetaMode);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Multiblock Threshold */
        recoveryAction = fpgagen6_XilinxJesd_RxMbTh_BfSet(  device,
                                                            xilinxJesdDeframerAddr[deframerIdx][interfaceIdx],
                                                            (deframerCfgInfo->ctrlRxMBlockTh - 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
        
        /* Get the lane buffer delay */
        recoveryAction = fpgagen6_AxiAdrvgen6Rx_LaneBufferDelay_BfSet( device,
                                                                    axiAdrvgen6RxChannelAddr[deframerIdx][interfaceIdx],
                                                                    deframerCfgInfo->laneBufferDelay);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
    }

    return recoveryAction;
}


ADI_API adi_fpgagen6_ErrAction_e fpgagen6_FramerCfgGet( adi_fpgagen6_Device_t* const    device,
                                                        const adi_fpgagen6_FramerSel_e  framerSel,
                                                        adi_fpgagen6_FramerCfg_t* const framerCfgInfo)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     framerIdx       = 0U;
    uint8_t                     interfaceIdx    = 0U;
    uint8_t                     bfValue         = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, framerCfgInfo);

    FPGAGEN6_FRAMER_IDX_SET_RETURN(framerSel, framerIdx, recoveryAction);

    FPGAGEN6_INTERFACE_IDX_SET_RETURN(device->devStateInfo.interfaceType, recoveryAction);

    /* Common Configuration First */

    /* Get Lane XBar */
    recoveryAction = fpgagen6_TxLaneXBarCfgGet( device,
                                                framerSel,
                                                framerCfgInfo->laneXBar);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_TX_LANEXBAR_CFG_GET,
                                                    framerSel,
                                                    recoveryAction);
    }

    /* Get Lanes In Use */
    recoveryAction = fpgagen6_XilinxJesd_LaneEnable_BfGet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            &framerCfgInfo->lanesInUse);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    /* Get Subclass */
    recoveryAction = fpgagen6_XilinxJesd_CfgSubclassv_BfGet(device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            &framerCfgInfo->subclassV);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    if (ADI_FPGAGEN6_INTERFACE_JESD204B == device->devStateInfo.interfaceType)
    {

        /* Get ILA Support */
        recoveryAction = fpgagen6_XilinxJesd_CfgIla_BfGet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            &framerCfgInfo->ilaSupportEn);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Scrambling */
        recoveryAction = fpgagen6_XilinxJesd_CfgScr_BfGet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            &framerCfgInfo->scramblingEn);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get SYSREF Always */
        recoveryAction = fpgagen6_XilinxJesd_SysrefAlways_BfGet(device,
                                                                xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                &framerCfgInfo->sysRefAlways);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get SYSREF Delay */
        recoveryAction = fpgagen6_XilinxJesd_SysrefDelay_BfGet( device,
                                                                xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                &framerCfgInfo->sysRefDelay);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get SYSREF Required On Resync */
        recoveryAction = fpgagen6_XilinxJesd_SysrefOnResync_BfGet(  device,
                                                                    xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                    &framerCfgInfo->sysRefRequiredOnResync);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Multiframes */
        recoveryAction = fpgagen6_XilinxJesd_IlaMultiFrames_BfGet(  device,
                                                                    xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                    &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        framerCfgInfo->ilaMultiFrames = (uint16_t) (bfValue + 1U);

        /* Get Octets per Frame (F) */
        recoveryAction = fpgagen6_XilinxJesd_CfgF_BfGet(device,
                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        framerCfgInfo->cfgF = (bfValue + 1U);

        /* Get Frames per Multiframe (K) */
        recoveryAction = fpgagen6_XilinxJesd_CfgK_BfGet(device,
                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        framerCfgInfo->cfgK = (bfValue + 1U);

        /* Get Bank ID */
        recoveryAction = fpgagen6_XilinxJesd_IlaBid_BfGet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            0U,
                                                            &framerCfgInfo->bankId);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Device ID */
        recoveryAction = fpgagen6_XilinxJesd_IlaDid_BfGet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            0U,
                                                            &framerCfgInfo->deviceId);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Control Bits per Sample (CS) */
        recoveryAction = fpgagen6_XilinxJesd_CfgCs_BfGet(   device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            &framerCfgInfo->ilaCs);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Total Bits per Sample (Np) */
        recoveryAction = fpgagen6_XilinxJesd_IlaM_BfGet(device,
                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                        0U,
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        framerCfgInfo->ilaNp = (bfValue + 1U);

        /* Get Converter Resolution (N) */
        recoveryAction = fpgagen6_XilinxJesd_IlaN_BfGet(device,
                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                        0U,
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        framerCfgInfo->ilaN = (bfValue + 1U);

        /* Get Converters per Device (M) */
        recoveryAction = fpgagen6_XilinxJesd_IlaM_BfGet(device,
                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                        0U,
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        framerCfgInfo->ilaM = (bfValue + 1U);

        /* Get Control Words per Frame (CF) */
        recoveryAction = fpgagen6_XilinxJesd_IlaCf_BfGet(   device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            0U,
                                                            &framerCfgInfo->ilaCf);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get High Density Format(HD) */
        recoveryAction = fpgagen6_XilinxJesd_IlaHd_BfGet(   device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            0U,
                                                            &framerCfgInfo->ilaHd);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Samples per Converter per Frame (S) */
        recoveryAction = fpgagen6_XilinxJesd_IlaS_BfGet(device,
                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                        0U,
                                                        &bfValue);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        framerCfgInfo->ilaS = (bfValue + 1U);
    }

    if (ADI_FPGAGEN6_INTERFACE_JESD204C == device->devStateInfo.interfaceType)
    {
        /* Get Enable Command Interface */
        recoveryAction = fpgagen6_XilinxJesd_EnableCmdInterface_BfGet(  device,
                                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                        &framerCfgInfo->enableCmdInterface);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Enable Data Interface */
        recoveryAction = fpgagen6_XilinxJesd_EnableDataInterface_BfGet( device,
                                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                        &framerCfgInfo->enableDataInterface);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Number of Multi Blocks in an Extended Multi Block */
        recoveryAction = fpgagen6_XilinxJesd_CfgMbInEmb_BfGet(  device,
                                                                xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                &framerCfgInfo->cfgE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Get Meta Mode */
        recoveryAction = fpgagen6_XilinxJesd_MetaMode_BfGet(device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            &framerCfgInfo->ctrlMetaMode);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_FramerCfgSet( adi_fpgagen6_Device_t* const            device,
                                                        const adi_fpgagen6_FramerSel_e          framerSel,
                                                        const adi_fpgagen6_FramerCfg_t* const   framerCfgInfo)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     framerIdx       = 0U;
    uint8_t                     interfaceIdx    = 0U;
    uint8_t                     laneIdx         = 0U;
    uint8_t                     framerCfgL      = 0U;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    FPGAGEN6_FRAMER_IDX_SET_RETURN(framerSel, framerIdx, recoveryAction);

    FPGAGEN6_INTERFACE_IDX_SET_RETURN(device->devStateInfo.interfaceType, recoveryAction);

    /* Common Configuration First */

    /* Configure Framer LaneXBar */
    recoveryAction = fpgagen6_TxLaneXBarCfgSet( device,
                                                framerSel,
                                                framerCfgInfo->laneXBar);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,
                                                    ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_TX_LANEXBAR_CFG_SET,
                                                    framerSel,
                                                    recoveryAction);
    }

    /* Configure Lanes In Use */
    recoveryAction = fpgagen6_XilinxJesd_LaneEnable_BfSet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            framerCfgInfo->lanesInUse);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    /* Configure Subclass */
    recoveryAction = fpgagen6_XilinxJesd_CfgSubclassv_BfSet(device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            framerCfgInfo->subclassV);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    if (ADI_FPGAGEN6_INTERFACE_JESD204B == device->devStateInfo.interfaceType)
    {
        /* Configure ILA Support */
        recoveryAction = fpgagen6_XilinxJesd_CfgIla_BfSet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            framerCfgInfo->ilaSupportEn);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Scrambling */
        recoveryAction = fpgagen6_XilinxJesd_CfgScr_BfSet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            framerCfgInfo->scramblingEn);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure SYREF Always */
        recoveryAction = fpgagen6_XilinxJesd_SysrefAlways_BfSet(device,
                                                                xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                framerCfgInfo->sysRefAlways);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure SYREF Delay */
        recoveryAction = fpgagen6_XilinxJesd_SysrefDelay_BfSet( device,
                                                                xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                framerCfgInfo->sysRefDelay);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure SYREF Required On Resync */
        recoveryAction = fpgagen6_XilinxJesd_SysrefDelay_BfSet( device,
                                                                xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                framerCfgInfo->sysRefRequiredOnResync);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure ILA Multiframes */
        recoveryAction = fpgagen6_XilinxJesd_IlaMultiFrames_BfSet(  device,
                                                                    xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                    (uint8_t) (framerCfgInfo->ilaMultiFrames - 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Octets per Frame (F) */
        recoveryAction = fpgagen6_XilinxJesd_CfgF_BfSet(device,
                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                        (framerCfgInfo->cfgF - 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Frames per Multiframe (K) */
        recoveryAction = fpgagen6_XilinxJesd_CfgK_BfSet(device,
                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                        (framerCfgInfo->cfgK - 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Bank ID */
        recoveryAction = fpgagen6_XilinxJesd_CfgBid_BfSet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            framerCfgInfo->bankId);

        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Device ID */
        recoveryAction = fpgagen6_XilinxJesd_CfgDid_BfSet(  device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            framerCfgInfo->deviceId);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
        
        /* Configure Lane ID */
        for (laneIdx = 0; laneIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; laneIdx++)
        {
            recoveryAction = fpgagen6_XilinxJesd_CfgLid_BfSet(  device,
                                                                xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                laneIdx, //framer lane number (0-7)
                                                                framerCfgInfo->laneId[laneIdx]);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                    ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                    ADI_NO_VARIABLE,
                    recoveryAction);
            }
        }
        
        

        /* Configure Control Bits Per Sample (CS)*/
        recoveryAction = fpgagen6_XilinxJesd_CfgCs_BfSet(   device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            framerCfgInfo->ilaCs);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Total Bits Per Sample (Np) */
        recoveryAction = fpgagen6_XilinxJesd_CfgNp_BfSet(   device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            (framerCfgInfo->ilaNp == 0U) ? 0U : (framerCfgInfo->ilaNp - 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Converter Resolution (N) */
        recoveryAction = fpgagen6_XilinxJesd_CfgN_BfSet(   device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            (framerCfgInfo->ilaN == 0U) ? 0U: framerCfgInfo->ilaN - 1U);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
        
        /* Configure Number of Converters (M) */
        recoveryAction = fpgagen6_XilinxJesd_CfgM_BfSet(device,
                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                        (framerCfgInfo->ilaM == 0U) ? 0U : framerCfgInfo->ilaM - 1U);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                ADI_NO_VARIABLE,
                recoveryAction);
        }

        /* Configure Control Words per Frame (CF) */
        recoveryAction = fpgagen6_XilinxJesd_CfgCf_BfSet(   device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            framerCfgInfo->ilaCf);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* configure high density format (HD) */
        recoveryAction = fpgagen6_XilinxJesd_CfgHd_BfSet(   device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            framerCfgInfo->ilaHd);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* configure samples per converter per frame (S) */
        recoveryAction = fpgagen6_XilinxJesd_CfgS_BfSet(device,
                                                        xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                        (framerCfgInfo->ilaS - 1U));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
        
        /* Configure number of lanes for selected link in 204B mode */
        framerCfgL = fpgagen6_LCalculate(framerCfgInfo->lanesInUse);
        if (framerCfgL != 0)
        {
            framerCfgL -= 1U; /* Configure value is always L-1 */
        }
        
        for (laneIdx = 0; laneIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; laneIdx++)
        {
            /* Skip Land ID that is not in used */
            if ((framerCfgInfo->lanesInUse & (1 << laneIdx)) == 0)
            {
                continue;
            }
            
            recoveryAction = fpgagen6_XilinxJesd_CfgL_BfSet(device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            laneIdx, 
                                                            framerCfgL);
            if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                          ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                          ADI_NO_VARIABLE,
                                                          recoveryAction);
            }
        }
    }

    if (ADI_FPGAGEN6_INTERFACE_JESD204C == device->devStateInfo.interfaceType)
    {
        /* Configure Number of Multi Blocks in an Extended Multi Block */
        recoveryAction = fpgagen6_XilinxJesd_CfgMbInEmb_BfSet(  device,
                                                                xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                                framerCfgInfo->cfgE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Configure Meta Mode */
        recoveryAction = fpgagen6_XilinxJesd_MetaMode_BfSet(device,
                                                            xilinxJesdFramerAddr[framerIdx][interfaceIdx],
                                                            framerCfgInfo->ctrlMetaMode);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GEN,
                                                        ADI_FPGAGEN6_ERRCODE_REGISTER_IO,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
    }

    return recoveryAction;
}
