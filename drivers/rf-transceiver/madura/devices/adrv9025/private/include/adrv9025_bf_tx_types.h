/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_bf_tx_types.h Automatically generated file with generator ver 0.0.1.0.
 *
 * \brief Contains BitField functions to support ADRV9025 transceiver device.
 *
 * ADRV9025 BITFIELD VERSION: 0.0.0.1
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the ADRV9025 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef __ADRV9025_BF_TX_TYPES_H__
#define __ADRV9025_BF_TX_TYPES_H__

#ifdef __cplusplus
extern "C"{
#endif

typedef enum adrv9025_BfTxChanAddr
{
    ADRV9025_BF_ALL_TX_CHANNELS = 0x400,
    ADRV9025_BF_TX_CH0 = 0x1E00,
    ADRV9025_BF_TX_CH1 = 0x2000,
    ADRV9025_BF_TX_CH2 = 0x2200,
    ADRV9025_BF_TX_CH3 = 0x2400
} adrv9025_BfTxChanAddr_e;

#ifdef __cplusplus
}
#endif

#endif /* __ADRV9025_BF_TX_TYPES_H__ */

/*  EOF: adrv9025_bf_tx_types.h */
