/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file Automatically generated file: adrv9025_bf_jrx_link.h
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

#ifndef __ADRV9025_BF_JRX_LINK_H__
#define __ADRV9025_BF_JRX_LINK_H__
#include "./../../private/include/adrv9025_bf_jrx_link_types.h"
#include "adi_adrv9025.h"
#ifdef __cplusplus
extern "C"{
#endif

int32_t adrv9025_JrxLinkJrxChksumCfgBfGet(adi_adrv9025_Device_t*       device,
                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                          uint8_t                      channelId,
                                          uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxConvSelBfSet(adi_adrv9025_Device_t*       device,
                                        adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                        uint8_t                      channelId,
                                        uint8_t                      bfValue);

int32_t adrv9025_JrxLinkJrxConvSelBfGet(adi_adrv9025_Device_t*       device,
                                        adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                        uint8_t                      channelId,
                                        uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxCsCfgBfGet(adi_adrv9025_Device_t*       device,
                                      adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                      uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDidCfgBfGet(adi_adrv9025_Device_t*       device,
                                       adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                       uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bBdCntBfGet(adi_adrv9025_Device_t*       device,
                                            adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                            uint8_t                      channelId,
                                            uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bBdeBfGet(adi_adrv9025_Device_t*       device,
                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                          uint8_t                      channelId,
                                          uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bCgsBfGet(adi_adrv9025_Device_t*       device,
                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                          uint8_t                      channelId,
                                          uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bCksBfGet(adi_adrv9025_Device_t*       device,
                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                          uint8_t                      channelId,
                                          uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bEcntEnaBfSet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                              uint8_t                      channelId,
                                              uint8_t                      bfValue);

int32_t adrv9025_JrxLinkJrxDl204bEcntEnaBfGet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                              uint8_t                      channelId,
                                              uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bEcntRstBfSet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                              uint8_t                      channelId,
                                              uint8_t                      bfValue);

int32_t adrv9025_JrxLinkJrxDl204bEcntTchBfSet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                              uint8_t                      channelId,
                                              uint8_t                      bfValue);

int32_t adrv9025_JrxLinkJrxDl204bEcntTchBfGet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                              uint8_t                      channelId,
                                              uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bEofEventBfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bEomfEventBfGet(adi_adrv9025_Device_t*       device,
                                                adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                uint8_t                      channelId,
                                                uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bEthBfSet(adi_adrv9025_Device_t*       device,
                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                          uint8_t                      bfValue);

int32_t adrv9025_JrxLinkJrxDl204bFsBfGet(adi_adrv9025_Device_t*       device,
                                         adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                         uint8_t                      channelId,
                                         uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bFsLostBfGet(adi_adrv9025_Device_t*       device,
                                             adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                             uint8_t                      channelId,
                                             uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bIldBfGet(adi_adrv9025_Device_t*       device,
                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                          uint8_t                      channelId,
                                          uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bIlsBfGet(adi_adrv9025_Device_t*       device,
                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                          uint8_t                      channelId,
                                          uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bIrqClrBfSet(adi_adrv9025_Device_t*       device,
                                             adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                             uint16_t                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bIrqClrBfGet(adi_adrv9025_Device_t*       device,
                                             adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                             uint16_t*                    bfValue);

int32_t adrv9025_JrxLinkJrxDl204bIrqVecBfGet(adi_adrv9025_Device_t*       device,
                                             adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                             uint8_t                      channelId,
                                             uint16_t*                    bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg0BfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg1BfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg10BfGet(adi_adrv9025_Device_t*       device,
                                                adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                uint8_t                      channelId,
                                                uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg13BfGet(adi_adrv9025_Device_t*       device,
                                                adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                uint8_t                      channelId,
                                                uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg2BfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg3BfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg4BfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg5BfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg6BfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg7BfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg8BfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bL0Rxcfg9BfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bNitBfGet(adi_adrv9025_Device_t*       device,
                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                          uint8_t                      channelId,
                                          uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bNitCntBfGet(adi_adrv9025_Device_t*       device,
                                             adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                             uint8_t                      channelId,
                                             uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bSyncNBfGet(adi_adrv9025_Device_t*       device,
                                            adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                            uint8_t                      channelId,
                                            uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bUekBfGet(adi_adrv9025_Device_t*       device,
                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                          uint8_t                      channelId,
                                          uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bUekCntBfGet(adi_adrv9025_Device_t*       device,
                                             adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                             uint8_t                      channelId,
                                             uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bUserDataBfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint8_t                      channelId,
                                               uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204bValidCksumBfGet(adi_adrv9025_Device_t*       device,
                                                 adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                 uint8_t                      channelId,
                                                 uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204cMbReqdCfgBfGet(adi_adrv9025_Device_t*       device,
                                                adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204cStateGet(adi_adrv9025_Device_t*       device,
                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                          uint8_t                      channelId,
                                          uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204hIrqClrBfSet(adi_adrv9025_Device_t*       device,
                                             adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                             uint16_t                     bfValue);

int32_t adrv9025_JrxLinkJrxDl204hIrqClrBfGet(adi_adrv9025_Device_t*       device,
                                             adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                             uint16_t*                    bfValue);

int32_t adrv9025_JrxLinkJrxDscrCfgBfGet(adi_adrv9025_Device_t*       device,
                                        adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                        uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxFCfgBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                     uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxHdCfgBfGet(adi_adrv9025_Device_t*       device,
                                      adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                      uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxKCfgBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                     uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxLCfgBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                     uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxLidCfgBfGet(adi_adrv9025_Device_t*       device,
                                       adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                       uint8_t                      channelId,
                                       uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxLinkLaneSelBfGet(adi_adrv9025_Device_t*       device,
                                            adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                            uint8_t                      channelId,
                                            uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxLinkTypeBfGet(adi_adrv9025_Device_t*       device,
                                         adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                         uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxMCfgBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                     uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxNCfgBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                     uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxNpCfgBfGet(adi_adrv9025_Device_t*       device,
                                      adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                      uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxSCfgBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                     uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxSyncNSelBfGet(adi_adrv9025_Device_t*       device,
                                         adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                         uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxSysrefForRelinkBfGet(adi_adrv9025_Device_t*       device,
                                                adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxSysrefForStartupBfGet(adi_adrv9025_Device_t*       device,
                                                 adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                 uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxTplPhaseAdjustBfGet(adi_adrv9025_Device_t*       device,
                                               adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                               uint16_t*                    bfValue);

int32_t adrv9025_JrxLinkJrxTplSysrefIgnoreWhenLinkedBfGet(adi_adrv9025_Device_t*       device,
                                                          adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                          uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxTplSysrefMaskBfSet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                              uint8_t                      bfValue);

int32_t adrv9025_JrxLinkJrxTplSysrefMaskBfGet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                              uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxTplSysrefNShotCountBfGet(adi_adrv9025_Device_t*       device,
                                                    adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                    uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxTplSysrefNShotEnableBfGet(adi_adrv9025_Device_t*       device,
                                                     adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                     uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxTplSysrefRcvdBfGet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                              uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxTplUsrDataRdyBfGet(adi_adrv9025_Device_t*       device,
                                              adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                              uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxTplBufDepthBfGet(adi_adrv9025_Device_t       *device,
                                            adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                            uint8_t                      channelId,
                                            uint8_t                     *bfValue);

int32_t adrv9025_JrxLinkJrxTplBufProtectionBfGet(adi_adrv9025_Device_t       *device,
                                                 adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                 uint8_t                     *bfValue);

int32_t adrv9025_JrxLinkJrxTplBufProtectEnBfSet(adi_adrv9025_Device_t       *device,
                                                adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                uint8_t                      bfValue);

int32_t adrv9025_JrxLinkJrxTplBufProtectEnBfGet(adi_adrv9025_Device_t       *device,
                                                adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                                uint8_t*                     bfValue);

int32_t adrv9025_JrxLinkJrxTplPhaseAdjustBfSet(adi_adrv9025_Device_t        *device,
                                               adrv9025_BfJrxLinkChanAddr_e  baseAddr,
                                               uint16_t                      bfValue);

int32_t adrv9025_JrxLinkJrxTplPhaseDiffBfGet(adi_adrv9025_Device_t       *device,
                                             adrv9025_BfJrxLinkChanAddr_e baseAddr,
                                             uint8_t                      channelId,
                                             uint8_t                     *bfValue);

#ifdef __cplusplus
}
#endif
#endif
