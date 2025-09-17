/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file Automatically generated file: adrv9025_bf_analog_tx_mem_map.h
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

#ifndef __ADRV9025_BF_ANALOG_TX_MEM_MAP_H__
#define __ADRV9025_BF_ANALOG_TX_MEM_MAP_H__
#include "./../../private/include/adrv9025_bf_analog_tx_mem_map_types.h"
#include "adi_adrv9025.h"
#ifdef __cplusplus
extern "C"{
#endif

int32_t adrv9025_AnalogTxMemMapAuxdac0BfSet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t                            bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac0BfGet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t*                           bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac0ConfigBfSet(adi_adrv9025_Device_t*              device,
                                                  adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                                  uint8_t                             bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac1BfSet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t                            bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac1BfGet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t*                           bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac1ConfigBfSet(adi_adrv9025_Device_t*              device,
                                                  adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                                  uint8_t                             bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac2BfSet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t                            bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac2BfGet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t*                           bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac2ConfigBfSet(adi_adrv9025_Device_t*              device,
                                                  adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                                  uint8_t                             bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac3BfSet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t                            bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac3BfGet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t*                           bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdac3ConfigBfSet(adi_adrv9025_Device_t*              device,
                                                  adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                                  uint8_t                             bfValue);

int32_t adrv9025_AnalogTxMemMapAuxdacLatchEnBfSet(adi_adrv9025_Device_t*              device,
                                                  adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                                  uint8_t                             bfValue);

#ifdef __cplusplus
}
#endif
#endif
