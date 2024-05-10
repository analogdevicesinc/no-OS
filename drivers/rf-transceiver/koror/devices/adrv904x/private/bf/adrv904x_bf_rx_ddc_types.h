/** 
 * \file adrv904x_bf_rx_ddc_types.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_RX_DDC_TYPES_H_
#define _ADRV904X_BF_RX_DDC_TYPES_H_

typedef enum adrv904x_BfRxDdcChanAddr
{
    ADRV904X_BF_SLICE_RX_0__RX_DDC_0_    =    0x60060000,
    ADRV904X_BF_SLICE_RX_0__RX_DDC_1_    =    0x60070000,
    ADRV904X_BF_SLICE_RX_1__RX_DDC_0_    =    0x60160000,
    ADRV904X_BF_SLICE_RX_1__RX_DDC_1_    =    0x60170000,
    ADRV904X_BF_SLICE_RX_2__RX_DDC_0_    =    0x60260000,
    ADRV904X_BF_SLICE_RX_2__RX_DDC_1_    =    0x60270000,
    ADRV904X_BF_SLICE_RX_3__RX_DDC_0_    =    0x60360000,
    ADRV904X_BF_SLICE_RX_3__RX_DDC_1_    =    0x60370000,
    ADRV904X_BF_SLICE_RX_4__RX_DDC_0_    =    0x60460000,
    ADRV904X_BF_SLICE_RX_4__RX_DDC_1_    =    0x60470000,
    ADRV904X_BF_SLICE_RX_5__RX_DDC_0_    =    0x60560000,
    ADRV904X_BF_SLICE_RX_5__RX_DDC_1_    =    0x60570000,
    ADRV904X_BF_SLICE_RX_6__RX_DDC_0_    =    0x60660000,
    ADRV904X_BF_SLICE_RX_6__RX_DDC_1_    =    0x60670000,
    ADRV904X_BF_SLICE_RX_7__RX_DDC_0_    =    0x60760000,
    ADRV904X_BF_SLICE_RX_7__RX_DDC_1_    =    0x60770000
} adrv904x_BfRxDdcChanAddr_e;

/** 
 * \brief Enumeration for fpExponentBits
 */

typedef enum adrv904x_Bf_RxDdc_FpExponentBits
{
    ADRV904X_BF_RX_DDC_FP_EXPONENT_BITS_RX1_EXP_2                       =    0,  /*!< No description provided */
    ADRV904X_BF_RX_DDC_FP_EXPONENT_BITS_RX1_EXP_3                       =    1,  /*!< No description provided */
    ADRV904X_BF_RX_DDC_FP_EXPONENT_BITS_RX1_EXP_4                       =    2,  /*!< No description provided */
    ADRV904X_BF_RX_DDC_FP_EXPONENT_BITS_RX1_EXP_5                       =    3   /*!< No description provided */
} adrv904x_Bf_RxDdc_FpExponentBits_e;

/** 
 * \brief Enumeration for fpRoundMode
 */

typedef enum adrv904x_Bf_RxDdc_FpRoundMode
{
    ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTIESTOEVEN                 =    0,  /*!< No description provided */
    ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTOWARDSPOSITIVE            =    1,  /*!< No description provided */
    ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTOWARDSNEGATIVE            =    2,  /*!< No description provided */
    ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTOWARDSZERO                =    3,  /*!< No description provided */
    ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTIESTOAWAY                 =    4   /*!< No description provided */
} adrv904x_Bf_RxDdc_FpRoundMode_e;

#endif // _ADRV904X_BF_RX_DDC_TYPES_H_

