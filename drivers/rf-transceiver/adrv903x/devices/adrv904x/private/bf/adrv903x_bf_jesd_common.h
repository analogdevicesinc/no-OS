/** 
 * \file adrv903x_bf_jesd_common.h Automatically generated file with generator ver 0.0.13.0.
 * 
 * \brief Contains BitField functions to support ADRV903X transceiver device.
 * 
 * ADRV903X BITFIELD VERSION: 0.0.0.8
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2025 Analog Devices Inc.
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADRV903X_BF_JESD_COMMON_H_
#define _ADRV903X_BF_JESD_COMMON_H_

#include "adi_adrv903x_core.h"
#include "adi_adrv903x_hal.h"
#include "../../private/bf/adrv903x_bf_jesd_common_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxCrcErrCntThreshold_BfSet(adi_adrv903x_Device_t* const device,
                                                                                 adi_adrv903x_SpiCache_t* const spiCache,
                                                                                 const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bBde_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bBdCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bCgs_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bCks_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEcntEna_BfSet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEcntEna_BfGet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEcntRst_BfSet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEcntTch_BfSet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEcntTch_BfGet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEofEvent_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEomfEvent_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEth_BfSet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bFs_BfGet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bFsLost_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bIld_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bIls_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bIrqClr_BfSet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                           const uint16_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bIrqClr_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint16_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bIrqVec_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint16_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg0_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg1_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg13_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg2_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg4_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg5_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg6_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg8_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg9_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bNit_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bNitCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bSyncN_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bUek_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bUekCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bUserData_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cClrErrCnt_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cCrcErrCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cEmbErrCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cHoldErrCnt_BfSet(adi_adrv903x_Device_t* const device,
                                                                               adi_adrv903x_SpiCache_t* const spiCache,
                                                                               const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cMbErrCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cShErrCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cState_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxLaneLoopback_BfSet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxLinkEn_BfSet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxLinkEn_BfGet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxSampleLoopback_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxSampleLoopback_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneClearErrors_BfSet(adi_adrv903x_Device_t* const device,
                                                                                  adi_adrv903x_SpiCache_t* const spiCache,
                                                                                  const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneErrorCount_BfGet(adi_adrv903x_Device_t* const device,
                                                                                 adi_adrv903x_SpiCache_t* const spiCache,
                                                                                 const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint32_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneErrorFlag_BfGet(adi_adrv903x_Device_t* const device,
                                                                                adi_adrv903x_SpiCache_t* const spiCache,
                                                                                const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneInv_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneInvalidDataFlag_BfGet(adi_adrv903x_Device_t* const device,
                                                                                      adi_adrv903x_SpiCache_t* const spiCache,
                                                                                      const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneUpdateErrorCount_BfSet(adi_adrv903x_Device_t* const device,
                                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                                       const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestMode_BfSet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestMode_BfGet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSampleClearErrors_BfGet(adi_adrv903x_Device_t* const device,
                                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                                    const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                    uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSampleErrorCount_BfGet(adi_adrv903x_Device_t* const device,
                                                                                   adi_adrv903x_SpiCache_t* const spiCache,
                                                                                   const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSampleErrorFlag_BfGet(adi_adrv903x_Device_t* const device,
                                                                                  adi_adrv903x_SpiCache_t* const spiCache,
                                                                                  const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSampleUpdateErrorCount_BfSet(adi_adrv903x_Device_t* const device,
                                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                                         const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                         const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSource_BfSet(adi_adrv903x_Device_t* const device,
                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                         const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSource_BfGet(adi_adrv903x_Device_t* const device,
                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                         const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JtxLinkEn_BfSet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JtxLinkEn_BfGet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JtxSampleLoopback_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JtxSampleLoopback_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

#endif // _ADRV903X_BF_JESD_COMMON_H_

