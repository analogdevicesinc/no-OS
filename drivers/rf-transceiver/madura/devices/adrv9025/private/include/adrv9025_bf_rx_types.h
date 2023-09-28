/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_bf_rx_types.h Automatically generated file with generator ver 0.0.1.0.
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

#ifndef __ADRV9025_BF_RX_TYPES_H__
#define __ADRV9025_BF_RX_TYPES_H__

#ifdef __cplusplus
extern "C"{
#endif

typedef enum adrv9025_BfRxChanAddr
{
     ADRV9025_BF_ALL_RX_CHANNELS =    0x200,
     ADRV9025_BF_RX_CH0          =    0x1200,
     ADRV9025_BF_RX_CH1          =    0x1400,
     ADRV9025_BF_RX_CH2          =    0x1600,
     ADRV9025_BF_RX_CH3          =    0x1800
} adrv9025_BfRxChanAddr_e;

/**
 * \brief Enumeration for fpExponentBits
 */
typedef enum adrv9025_BfRxFpExponentBits
{
    ADRV9025_BF_RX_RX1_EXP_2 =    0,  /*!< Two bits of exponent   */
    ADRV9025_BF_RX_RX1_EXP_3 =    1,  /*!< Three bits of exponent */
    ADRV9025_BF_RX_RX1_EXP_4 =    2,  /*!< Four bits of exponent  */
    ADRV9025_BF_RX_RX1_EXP_5 =    3   /*!< Five bits of exponent  */
} adrv9025_BfRxFpExponentBits_e;
/**
 * \brief Enumeration for fpRoundMode
 */
typedef enum adrv9025_BfRxFpRoundMode
{
    ADRV9025_BF_RX_ROUNDTIESTOEVEN      =    0,  /*!< No description provided */
    ADRV9025_BF_RX_ROUNDTOWARDSPOSITIVE =    1,  /*!< No description provided */
    ADRV9025_BF_RX_ROUNDTOWARDSNEGATIVE =    2,  /*!< No description provided */
    ADRV9025_BF_RX_ROUNDTOWARDSZERO     =    3,  /*!< No description provided */
    ADRV9025_BF_RX_ROUNDTIESTOAWAY      =    4   /*!< No description provided */
} adrv9025_BfRxFpRoundMode_e;
#ifdef __cplusplus
}
#endif

#endif /* __ADRV9025_BF_RX_TYPES_H__ */

/*  EOF: adrv9025_bf_rx_types.h */
