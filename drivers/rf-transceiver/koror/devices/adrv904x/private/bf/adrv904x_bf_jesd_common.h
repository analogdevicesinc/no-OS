/** 
 * \file adrv904x_bf_jesd_common.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support ADRV904X transceiver device.
 * 
 * ADRV904X BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2021 Analog Devices Inc.
 * 
 * Released under the ADRV904X API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _ADRV904X_BF_JESD_COMMON_H_
#define _ADRV904X_BF_JESD_COMMON_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_jesd_common_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_CddcBypassMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_CddcBypassMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_CducBypassMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_CducBypassMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxChannelSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxChannelSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxChannelSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxChannelSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxCrcErrCntThreshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bBde_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bBdCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bCgs_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bCks_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEcntEna_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEcntEna_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEcntRst_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEcntTch_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEcntTch_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEofEvent_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEomfEvent_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEth_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bFs_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bFsLost_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bIld_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bIls_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bIrqClr_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                           const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bIrqClr_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bIrqVec_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg0_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg1_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg13_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg2_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg4_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg5_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg6_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg8_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg9_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bNit_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bNitCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bUek_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bUekCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bUserData_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cClrErrCnt_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cCrcErrCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cEmbErrCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cMbErrCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cShErrCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cState_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxLaneLoopback_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxLinkEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxLinkEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxSampleLoopback_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxSampleLoopback_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneClearErrors_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneErrorCount_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneErrorFlag_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneInv_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneInvalidDataFlag_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneUpdateErrorCount_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSampleClearErrors_BfGet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSampleErrorCount_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSampleErrorFlag_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSampleUpdateErrorCount_BfSet(adi_adrv904x_Device_t* const device,
                                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                                         const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSource_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSource_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxChannelSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxChannelSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxChannelSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxChannelSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxLinkEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxLinkEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxSampleLoopback_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxSampleLoopback_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_OrxSampleSelLink2_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_OrxSampleSelLink2_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_RxSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_RxSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_RxSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_RxSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_TxSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_TxSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_TxSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_TxSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);
#endif // _ADRV904X_BF_JESD_COMMON_H_

