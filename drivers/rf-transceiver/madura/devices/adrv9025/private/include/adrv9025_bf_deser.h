/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file Automatically generated file: adrv9025_bf_deser.h
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

#ifndef __ADRV9025_BF_DESER_H__
#define __ADRV9025_BF_DESER_H__
#include "./../../private/include/adrv9025_bf_deser_types.h"
#include "adi_adrv9025.h"
#ifdef __cplusplus
extern "C"{
#endif

int32_t adrv9025_DeserCbusAddrDesRcBfSet(adi_adrv9025_Device_t*     device,
                                         adrv9025_BfDeserChanAddr_e baseAddr,
                                         uint8_t                    bfValue);

int32_t adrv9025_DeserCbusRdataDesRsBfGet(adi_adrv9025_Device_t*     device,
                                          adrv9025_BfDeserChanAddr_e baseAddr,
                                          uint8_t*                   bfValue);

int32_t adrv9025_DeserCbusRenDesRcChBfSet(adi_adrv9025_Device_t*     device,
                                          adrv9025_BfDeserChanAddr_e baseAddr,
                                          uint8_t                    bfValue);

int32_t adrv9025_DeserCbusWdataDesRcBfSet(adi_adrv9025_Device_t*     device,
                                          adrv9025_BfDeserChanAddr_e baseAddr,
                                          uint8_t                    bfValue);

int32_t adrv9025_DeserCbusWstrobeDesRcChBfSet(adi_adrv9025_Device_t*     device,
                                              adrv9025_BfDeserChanAddr_e baseAddr,
                                              uint8_t                    bfValue);

#ifdef __cplusplus
}
#endif
#endif
