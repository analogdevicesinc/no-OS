/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_bf_orx_types.h Automatically generated file with generator ver 0.0.1.0.
 *
 * \brief Contains BitField functions to support ADRV9025 transciever device.
 *
 * ADRV9025 BITFIELD VERSION: 0.0.0.1
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the ADRV9025 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef __ADRV9025_BF_ORX_TYPES_H__
#define __ADRV9025_BF_ORX_TYPES_H__

#ifdef __cplusplus
extern "C"{
#endif

typedef enum adrv9025_BfOrxChanAddr
{
     ADRV9025_BF_ALL_ORX_CHANNELS    =    0x600,
     ADRV9025_BF_ORX_CH0    =    0x1A00,
     ADRV9025_BF_ORX_CH1    =    0x1C00
} adrv9025_BfOrxChanAddr_e;

/**
 * \brief Enumeration for fpExponentBits
 */
typedef enum adrv9025_BfOrxFpExponentBits
{
    ADRV9025_BF_ORX_RX1_EXP_2            =    0,  /*!< Two bits of exponent    */
    ADRV9025_BF_ORX_RX1_EXP_3            =    1,  /*!< Three bits of exponent  */
    ADRV9025_BF_ORX_RX1_EXP_4            =    2,  /*!< Four bits of exponent   */
    ADRV9025_BF_ORX_RX1_EXP_5            =    3   /*!< Five bits of exponent   */
} adrv9025_BfOrxFpExponentBits_e;
/**
 * \brief Enumeration for fpRoundMode
 */
typedef enum adrv9025_BfOrxFpRoundMode
{
    ADRV9025_BF_ORX_ROUNDTIESTOEVEN      =    0,  /*!< No description provided */
    ADRV9025_BF_ORX_ROUNDTOWARDSPOSITIVE =    1,  /*!< No description provided */
    ADRV9025_BF_ORX_ROUNDTOWARDSNEGATIVE =    2,  /*!< No description provided */
    ADRV9025_BF_ORX_ROUNDTOWARDSZERO     =    3,  /*!< No description provided */
    ADRV9025_BF_ORX_ROUNDTIESTOAWAY      =    4   /*!< No description provided */
} adrv9025_BfOrxFpRoundMode_e;
#ifdef __cplusplus
}
#endif

#endif /* __ADRV9025_BF_ORX_TYPES_H__ */

/*  EOF: adrv9025_bf_orx_types.h */
