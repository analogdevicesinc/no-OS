/** 
 * \file adrv904x_bf_tx_dfe_dig_regs_types.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support ADRV904X transceiver device.
 * 
 * ADRV904X BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2021 Analog Devices Inc.
 * 
 * Released under the ADRV904X API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _ADRV904X_BF_TX_DFE_DIG_REGS_TYPES_H_
#define _ADRV904X_BF_TX_DFE_DIG_REGS_TYPES_H_

typedef enum adrv904x_BfTxDfeDigRegsChanAddr
{
    ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS    =    0x608C0000,
    ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS    =    0x609C0000,
    ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS    =    0x60AC0000,
    ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS    =    0x60BC0000,
    ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS    =    0x60CC0000,
    ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS    =    0x60DC0000,
    ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS    =    0x60EC0000,
    ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS    =    0x60FC0000
} adrv904x_BfTxDfeDigRegsChanAddr_e;

/** 
 * \brief Enumeration for rxCapAltSel
 */

typedef enum adrv904x_Bf_TxDfeDigRegs_RxCapAltSel
{
    ADRV904X_BF_TX_DFE_DIG_REGS_RX_CAP_ALT_SEL_DC_OFFSET                       =    0,  /*!< DC Offset Data                                                                                                      */
    ADRV904X_BF_TX_DFE_DIG_REGS_RX_CAP_ALT_SEL_HB2_INPUT                       =    1,  /*!< HB2 Input Data                                                                                                      */
    ADRV904X_BF_TX_DFE_DIG_REGS_RX_CAP_ALT_SEL_QFIR_OUTPUT                     =    2,  /*!< QFIR Output Data                                                                                                    */
    ADRV904X_BF_TX_DFE_DIG_REGS_RX_CAP_ALT_SEL_QFIR_INPUT                      =    3   /*!< QFIR Input Data                                                                                                     */
} adrv904x_Bf_TxDfeDigRegs_RxCapAltSel_e;

/** 
 * \brief Enumeration for rxCapSel
 */

typedef enum adrv904x_Bf_TxDfeDigRegs_RxCapSel
{
    ADRV904X_BF_TX_DFE_DIG_REGS_RX_CAP_SEL_DC_OFFSET                       =    0,  /*!< DC Offset Data                                                                                                      */
    ADRV904X_BF_TX_DFE_DIG_REGS_RX_CAP_SEL_HB2_INPUT                       =    1,  /*!< HB2 Input Data                                                                                                      */
    ADRV904X_BF_TX_DFE_DIG_REGS_RX_CAP_SEL_QFIR_OUTPUT                     =    2,  /*!< QFIR Output Data                                                                                                    */
    ADRV904X_BF_TX_DFE_DIG_REGS_RX_CAP_SEL_QFIR_INPUT                      =    3   /*!< QFIR Input Data                                                                                                     */
} adrv904x_Bf_TxDfeDigRegs_RxCapSel_e;

/** 
 * \brief Enumeration for txDpdCapAltSel
 */

typedef enum adrv904x_Bf_TxDfeDigRegs_TxDpdCapAltSel
{
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_ALT_SEL_PRE_CFR                         =    0,  /*!< Select Pre CFR Data                                                                                                 */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_ALT_SEL_PRE_DPD_HB1_OUT                 =    1,  /*!< Select Pre DPD HB1 Out                                                                                              */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_ALT_SEL_PRE_DPD_HB2_OUT                 =    2,  /*!< Select Pre DPD HB2 Output                                                                                           */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_ALT_SEL_POST_DPD_HB1_OUT                =    3,  /*!< Select Post DPD HB1 Output                                                                                          */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_ALT_SEL_CT_DPD_LG_DATA                  =    4,  /*!< CT_DPD Capture Mode                                                                                                 */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_ALT_SEL_NO_CAPTURE                      =    7   /*!< No Capture Mode, Zeros on Bus to Save Power                                                                         */
} adrv904x_Bf_TxDfeDigRegs_TxDpdCapAltSel_e;

/** 
 * \brief Enumeration for txDpdCapSel
 */

typedef enum adrv904x_Bf_TxDfeDigRegs_TxDpdCapSel
{
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_SEL_PRE_CFR                         =    0,  /*!< Select Pre CFR Data                                                                                                 */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_SEL_PRE_DPD_HB1_OUT                 =    1,  /*!< Select Pre DPD HB1 Output                                                                                           */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_SEL_PRE_DPD_HB2_OUT                 =    2,  /*!< Select Pre DPD HB2 Output                                                                                           */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_SEL_POST_DPD_HB1_OUT                =    3,  /*!< Select Post DPD HB1 Output                                                                                          */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_SEL_CT_DPD_LG_DATA                  =    4,  /*!< CT_DPD Capture Mode                                                                                                 */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_DPD_CAP_SEL_NO_CAPTURE                      =    7   /*!< No Capture Mode, Zeros on Bus to Save Power                                                                         */
} adrv904x_Bf_TxDfeDigRegs_TxDpdCapSel_e;

/** 
 * \brief Enumeration for txVswrDpdMuxSel
 */

typedef enum adrv904x_Bf_TxDfeDigRegs_TxVswrDpdMuxSel
{
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_VSWR_DPD_MUX_SEL_DPD_SEL                         =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_VSWR_DPD_MUX_SEL_VSWR_SEL                        =    1   /*!< No description provided                                                                                             */
} adrv904x_Bf_TxDfeDigRegs_TxVswrDpdMuxSel_e;

/** 
 * \brief Enumeration for txVswrFwdRev
 */

typedef enum adrv904x_Bf_TxDfeDigRegs_TxVswrFwdRev
{
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_VSWR_FWD_REV_VSWR_REV_MODE                   =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_TX_DFE_DIG_REGS_TX_VSWR_FWD_REV_VSWR_FWD_MODE                   =    1   /*!< No description provided                                                                                             */
} adrv904x_Bf_TxDfeDigRegs_TxVswrFwdRev_e;

#endif // _ADRV904X_BF_TX_DFE_DIG_REGS_TYPES_H_

