/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_bf_deser_types.h Automatically generated file with generator ver 0.0.1.0.
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

#ifndef __ADRV9025_BF_DESER_TYPES_H__
#define __ADRV9025_BF_DESER_TYPES_H__

#ifdef __cplusplus
extern "C"{
#endif

typedef enum adrv9025_BfDeserChanAddr
{
     ADRV9025_BF_JESD_DESERIALIZER    =    0x6800
} adrv9025_BfDeserChanAddr_e;

/**
 * \brief Enumeration for rxdivrateLcpllRc
 */
typedef enum adrv9025_BfDeserRxdivrateLcpllRc
{
    ADRV9025_BF_DESER_RXDIVRATE_LCPLL_RC_8          =    8   /*!< Half-rate CDR; 1X oversampling                                                                                                             */
} adrv9025_BfDeserRxdivrateLcpllRc_e;
/**
 * \brief Enumeration for rxdivrateLcpllRs
 */
typedef enum adrv9025_BfDeserRxdivrateLcpllRs
{
    ADRV9025_BF_DESER_RXDIVRATE_LCPLL_RS_8          =    8   /*!< Half-rate CDR; 1X oversampling                                                                                                             */
} adrv9025_BfDeserRxdivrateLcpllRs_e;
#ifdef __cplusplus
}
#endif

#endif // __ADRV9025_BF_DESER_TYPES_H__

/*  EOF: adrv9025_bf_deser_types.h */
