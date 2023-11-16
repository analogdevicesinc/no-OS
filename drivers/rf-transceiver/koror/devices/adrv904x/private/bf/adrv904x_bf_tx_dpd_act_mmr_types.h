/** 
 * \file adrv904x_bf_tx_dpd_act_mmr_types.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_TX_DPD_ACT_MMR_TYPES_H_
#define _ADRV904X_BF_TX_DPD_ACT_MMR_TYPES_H_

typedef enum adrv904x_BfTxDpdActMmrChanAddr
{
    ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DPD_ACT_MMR    =    0x608E0000,
    ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DPD_ACT_MMR    =    0x609E0000,
    ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DPD_ACT_MMR    =    0x60AE0000,
    ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DPD_ACT_MMR    =    0x60BE0000,
    ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DPD_ACT_MMR    =    0x60CE0000,
    ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DPD_ACT_MMR    =    0x60DE0000,
    ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DPD_ACT_MMR    =    0x60EE0000,
    ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DPD_ACT_MMR    =    0x60FE0000
} adrv904x_BfTxDpdActMmrChanAddr_e;

/** 
 * \brief Enumeration for dpdGmpDdrMode
 */

typedef enum adrv904x_Bf_TxDpdActMmr_DpdGmpDdrMode
{
    ADRV904X_BF_TX_DPD_ACT_MMR_DPD_GMP_DDR_MODE_MODE0                           =    0,  /*!< GMP0 - 50                                                                                                           */
    ADRV904X_BF_TX_DPD_ACT_MMR_DPD_GMP_DDR_MODE_MODE1                           =    1,  /*!< GMP0 - 32 DDR7 - 6 DDR8 - 6 DDR9 - 6                                                                                */
    ADRV904X_BF_TX_DPD_ACT_MMR_DPD_GMP_DDR_MODE_MODE2                           =    2,  /*!< GMP0 - 38 DDR7 - 6 DDR9 - 6                                                                                         */
    ADRV904X_BF_TX_DPD_ACT_MMR_DPD_GMP_DDR_MODE_MODE3                           =    3,  /*!< GMP0 - 38 DDR7 - 4 DDR8 - 4 DDR9 - 4                                                                                */
    ADRV904X_BF_TX_DPD_ACT_MMR_DPD_GMP_DDR_MODE_MODE4                           =    4,  /*!< GMP0 - 44 DDR7 - 6                                                                                                  */
    ADRV904X_BF_TX_DPD_ACT_MMR_DPD_GMP_DDR_MODE_MODE5                           =    5,  /*!< GMP0 - 44 DDR9 - 6                                                                                                  */
    ADRV904X_BF_TX_DPD_ACT_MMR_DPD_GMP_DDR_MODE_MODE6                           =    6,  /*!< GMP0 - 44 DDR7 - 3 DDR9 - 3                                                                                         */
    ADRV904X_BF_TX_DPD_ACT_MMR_DPD_GMP_DDR_MODE_MODE7                           =    7   /*!< GMP0 - 32 GMP1 - 9 DDR7 - 3 DDR8 - 3 DDR9 - 3                                                                       */
} adrv904x_Bf_TxDpdActMmr_DpdGmpDdrMode_e;

#endif // _ADRV904X_BF_TX_DPD_ACT_MMR_TYPES_H_

