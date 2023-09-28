/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file Automatically generated file: adrv9025_bf_pll_mem_map.h
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

#ifndef __ADRV9025_BF_PLL_MEM_MAP_H__
#define __ADRV9025_BF_PLL_MEM_MAP_H__
#include "./../../private/include/adrv9025_bf_pll_mem_map_types.h"
#include "adi_adrv9025.h"
#ifdef __cplusplus
extern "C"{
#endif

int32_t adrv9025_PllMemMapAuxAdcReadyBfGet(adi_adrv9025_Device_t*         device,
                                           adrv9025_BfPllMemMapChanAddr_e baseAddr,
                                           uint8_t*                       bfValue);

int32_t adrv9025_PllMemMapRx12SelLo2BfGet(adi_adrv9025_Device_t*         device,
                                          adrv9025_BfPllMemMapChanAddr_e baseAddr,
                                          uint8_t*                       bfValue);

int32_t adrv9025_PllMemMapRx34SelLo2BfGet(adi_adrv9025_Device_t*         device,
                                          adrv9025_BfPllMemMapChanAddr_e baseAddr,
                                          uint8_t*                       bfValue);

int32_t adrv9025_PllMemMapSynLockBfGet(adi_adrv9025_Device_t*         device,
                                       adrv9025_BfPllMemMapChanAddr_e baseAddr,
                                       uint8_t*                       bfValue);

int32_t adrv9025_PllMemMapTx12SelLo2BfGet(adi_adrv9025_Device_t*         device,
                                          adrv9025_BfPllMemMapChanAddr_e baseAddr,
                                          uint8_t*                       bfValue);

int32_t adrv9025_PllMemMapTx34SelLo2BfGet(adi_adrv9025_Device_t*         device,
                                          adrv9025_BfPllMemMapChanAddr_e baseAddr,
                                          uint8_t*                       bfValue);

#ifdef __cplusplus
}
#endif
#endif
