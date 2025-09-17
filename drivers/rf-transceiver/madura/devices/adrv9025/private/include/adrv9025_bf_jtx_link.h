/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file Automatically generated file: adrv9025_bf_jtx_link.h
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

#ifndef __ADRV9025_BF_JTX_LINK_H__
#define __ADRV9025_BF_JTX_LINK_H__
#include "./../../private/include/adrv9025_bf_jtx_link_types.h"
#include "adi_adrv9025.h"
#ifdef __cplusplus
extern "C"{
#endif

int32_t adrv9025_JtxLinkJtxBidCfgBfGet(adi_adrv9025_Device_t*       device,
                                       adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                       uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxConvSelBfSet(adi_adrv9025_Device_t*       device,
                                        adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                        uint8_t                      channelId,
                                        uint8_t                      bfValue);

int32_t adrv9025_JtxLinkJtxConvSelBfGet(adi_adrv9025_Device_t*       device,
                                        adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                        uint8_t                      channelId,
                                        uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxDidCfgBfGet(adi_adrv9025_Device_t*       device,
                                       adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                       uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxDl204bStateBfGet(adi_adrv9025_Device_t*       device,
                                            adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                            uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxDl204bSyncNBfGet(adi_adrv9025_Device_t*       device,
                                            adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                            uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxDl204bSyncNeCountBfGet(adi_adrv9025_Device_t*       device,
                                                  adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                                  uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxDl204cSysrefRcvdGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxECfgBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                     uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxFCfgBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                     uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxKCfgBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                     uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxLaneInvBfSet(adi_adrv9025_Device_t*       device,
                                        adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                        uint8_t                      channelId,
                                        uint8_t                      bfValue);

int32_t adrv9025_JtxLinkJtxLaneInvBfGet(adi_adrv9025_Device_t*       device,
                                        adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                        uint8_t                      channelId,
                                        uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxLaneSelBfGet(adi_adrv9025_Device_t*       device,
                                        adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                        uint8_t                      channelId,
                                        uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxLidCfgBfGet(adi_adrv9025_Device_t*       device,
                                       adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                       uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxMCfgBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                     uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxNpCfgBfGet(adi_adrv9025_Device_t*       device,
                                      adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                      uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxScrCfgBfGet(adi_adrv9025_Device_t*       device,
                                       adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                       uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxSyncNSelBfGet(adi_adrv9025_Device_t*       device,
                                         adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                         uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxSysrefForRelinkBfGet(adi_adrv9025_Device_t*       device,
                                                adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                                uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxSysrefForStartupBfGet(adi_adrv9025_Device_t*       device,
                                                 adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                                 uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxTestGenModeBfSet(adi_adrv9025_Device_t*       device,
                                            adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                            uint8_t                      bfValue);

int32_t adrv9025_JtxLinkJtxTestGenModeBfGet(adi_adrv9025_Device_t*       device,
                                            adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                            uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxTestGenSelBfSet(adi_adrv9025_Device_t*       device,
                                           adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                           uint8_t                      bfValue);

int32_t adrv9025_JtxLinkJtxTestGenSelBfGet(adi_adrv9025_Device_t*       device,
                                           adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                           uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxTplCfgInvalidBfGet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                              uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxTplPhaseAdjustBfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                               uint16_t*                    bfValue);

int32_t adrv9025_JtxLinkJtxTplSysrefIgnoreWhenLinkedBfGet(adi_adrv9025_Device_t*       device,
                                                          adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                                          uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxTplSysrefMaskBfSet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                              uint8_t                      bfValue);

int32_t adrv9025_JtxLinkJtxTplSysrefMaskBfGet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                              uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxTplSysrefNShotCountBfGet(adi_adrv9025_Device_t*       device,
                                                    adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                                    uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxTplSysrefNShotEnableBfGet(adi_adrv9025_Device_t*       device,
                                                     adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                                     uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxTplSysrefPhaseErrBfGet(adi_adrv9025_Device_t*       device,
                                                  adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                                  uint8_t*                     bfValue);

int32_t adrv9025_JtxLinkJtxTplSysrefRcvdBfGet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJtxLinkChanAddr_e baseAddr,
                                              uint8_t*                     bfValue);

#ifdef __cplusplus
}
#endif
#endif
