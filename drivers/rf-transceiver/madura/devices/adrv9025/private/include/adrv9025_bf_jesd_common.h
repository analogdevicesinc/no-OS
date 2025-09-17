/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file Automatically generated file: adrv9025_bf_jesd_common.h
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

#ifndef __ADRV9025_BF_JESD_COMMON_H__
#define __ADRV9025_BF_JESD_COMMON_H__
#include "./../../private/include/adrv9025_bf_jesd_common_types.h"
#include "adi_adrv9025.h"
#ifdef __cplusplus
extern "C"{
#endif

int32_t adrv9025_JesdCommonJrxTestLaneClearErrorsBfSet(adi_adrv9025_Device_t*          device,
                                                       adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                       uint8_t                         bfValue);

int32_t adrv9025_JesdCommonJrxTestLaneErrorCountBfGet(adi_adrv9025_Device_t*          device,
                                                      adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                      uint8_t                         channelId,
                                                      uint32_t*                       bfValue);

int32_t adrv9025_JesdCommonJrxTestLaneErrorFlagBfGet(adi_adrv9025_Device_t*          device,
                                                     adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                     uint8_t                         channelId,
                                                     uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJrxTestLaneInvBfGet(adi_adrv9025_Device_t*          device,
                                               adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                               uint8_t                         channelId,
                                               uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJrxTestLaneInvalidDataFlagBfGet(adi_adrv9025_Device_t*          device,
                                                           adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                           uint8_t                         channelId,
                                                           uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJrxTestLaneUpdateErrorCountBfSet(adi_adrv9025_Device_t*          device,
                                                            adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                            uint8_t                         bfValue);

int32_t adrv9025_JesdCommonJrxTestModeBfSet(adi_adrv9025_Device_t*          device,
                                            adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                            uint8_t                         bfValue);

int32_t adrv9025_JesdCommonJrxTestModeBfGet(adi_adrv9025_Device_t*          device,
                                            adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                            uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJrxTestSampleClearErrorsBfSet(adi_adrv9025_Device_t*          device,
                                                         adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                         uint8_t                         bfValue);

int32_t adrv9025_JesdCommonJrxTestSampleClearErrorsBfGet(adi_adrv9025_Device_t*          device,
                                                         adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                         uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJrxTestSampleErrorCountBfGet(adi_adrv9025_Device_t*          device,
                                                        adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                        uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJrxTestSampleErrorFlagBfGet(adi_adrv9025_Device_t*          device,
                                                       adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                       uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJrxTestSampleUpdateErrorCountBfSet(adi_adrv9025_Device_t*          device,
                                                              adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                              uint8_t                         bfValue);

int32_t adrv9025_JesdCommonJrxTestSourceBfSet(adi_adrv9025_Device_t*          device,
                                              adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                              uint8_t                         bfValue);

int32_t adrv9025_JesdCommonJrxTestSourceBfGet(adi_adrv9025_Device_t*          device,
                                              adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                              uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJtxLinkEnBfGet(adi_adrv9025_Device_t*          device,
                                          adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                          uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJtxOverrideSrstEnBfSet(adi_adrv9025_Device_t*          device,
                                                  adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                  uint8_t                         bfValue);

int32_t adrv9025_JesdCommonJtxOverrideSrstEnBfGet(adi_adrv9025_Device_t*          device,
                                                  adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                  uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJtxTriggerSrstNBfSet(adi_adrv9025_Device_t*          device,
                                                adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                uint8_t                         bfValue);

int32_t adrv9025_JesdCommonJtxTriggerSrstNBfGet(adi_adrv9025_Device_t*          device,
                                                adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                uint8_t*                        bfValue);

int32_t adrv9025_JesdCommonJrxLinkEnBfGet(adi_adrv9025_Device_t*          device,
                                          adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                          uint8_t*                        bfValue);

#ifdef __cplusplus
}
#endif
#endif
