/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file Automatically generated file: adrv9025_bf_orx.h
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

#ifndef __ADRV9025_BF_ORX_H__
#define __ADRV9025_BF_ORX_H__
#include "./../../private/include/adrv9025_bf_orx_types.h"
#include "adi_adrv9025.h"
#ifdef __cplusplus
extern "C"{
#endif

int32_t adrv9025_OrxAgcEnableFastRecoveryLoopBfSet(adi_adrv9025_Device_t*   device,
                                                   adrv9025_BfOrxChanAddr_e baseAddr,
                                                   uint8_t                  bfValue);

int32_t adrv9025_OrxAuxAdcDecimationCtlBfSet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t                  bfValue);

int32_t adrv9025_OrxAuxAdcPdBfSet(adi_adrv9025_Device_t*   device,
                                  adrv9025_BfOrxChanAddr_e baseAddr,
                                  uint8_t                  bfValue);

int32_t adrv9025_OrxAuxAdcPdBfGet(adi_adrv9025_Device_t*   device,
                                  adrv9025_BfOrxChanAddr_e baseAddr,
                                  uint8_t*                 bfValue);

int32_t adrv9025_OrxAuxAdcReadBfGet(adi_adrv9025_Device_t*   device,
                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                    uint16_t*                bfValue);

int32_t adrv9025_OrxAuxAdcReadbackLockBfSet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t                  bfValue);

int32_t adrv9025_OrxAuxAdcResetBfSet(adi_adrv9025_Device_t*   device,
                                     adrv9025_BfOrxChanAddr_e baseAddr,
                                     uint8_t                  bfValue);

int32_t adrv9025_OrxAuxAdcSelBfSet(adi_adrv9025_Device_t*   device,
                                   adrv9025_BfOrxChanAddr_e baseAddr,
                                   uint8_t                  bfValue);

int32_t adrv9025_OrxAuxAdcSelBfGet(adi_adrv9025_Device_t*   device,
                                   adrv9025_BfOrxChanAddr_e baseAddr,
                                   uint8_t*                 bfValue);

int32_t adrv9025_OrxBbdcLoopback1TrackingEnableBfSet(adi_adrv9025_Device_t*   device,
                                                     adrv9025_BfOrxChanAddr_e baseAddr,
                                                     uint8_t                  bfValue);

int32_t adrv9025_OrxBbdcLoopback1TrackingEnableBfGet(adi_adrv9025_Device_t*   device,
                                                     adrv9025_BfOrxChanAddr_e baseAddr,
                                                     uint8_t*                 bfValue);

int32_t adrv9025_OrxBbdcLoopback2TrackingEnableBfSet(adi_adrv9025_Device_t*   device,
                                                     adrv9025_BfOrxChanAddr_e baseAddr,
                                                     uint8_t                  bfValue);

int32_t adrv9025_OrxBbdcLoopback2TrackingEnableBfGet(adi_adrv9025_Device_t*   device,
                                                     adrv9025_BfOrxChanAddr_e baseAddr,
                                                     uint8_t*                 bfValue);

int32_t adrv9025_OrxBbdcLoopbackMShiftBfSet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t                  bfValue);

int32_t adrv9025_OrxBbdcLoopbackMShiftBfGet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t*                 bfValue);

int32_t adrv9025_OrxBbdcMShiftBfSet(adi_adrv9025_Device_t*   device,
                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                    uint8_t                  bfValue);

int32_t adrv9025_OrxBbdcMShiftBfGet(adi_adrv9025_Device_t*   device,
                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                    uint8_t*                 bfValue);

int32_t adrv9025_OrxBbdcOrx2MShiftBfSet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t                  bfValue);

int32_t adrv9025_OrxBbdcOrx2MShiftBfGet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t*                 bfValue);

int32_t adrv9025_OrxBbdcOrx2TrackingEnableBfSet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t                  bfValue);

int32_t adrv9025_OrxBbdcOrx2TrackingEnableBfGet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t*                 bfValue);

int32_t adrv9025_OrxBbdcTrackingEnableBfSet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t                  bfValue);

int32_t adrv9025_OrxBbdcTrackingEnableBfGet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t*                 bfValue);

int32_t adrv9025_OrxDecPowerBfGet(adi_adrv9025_Device_t*   device,
                                  adrv9025_BfOrxChanAddr_e baseAddr,
                                  uint8_t*                 bfValue);

int32_t adrv9025_OrxDynamicSlicerAgcSyncEnableBfSet(adi_adrv9025_Device_t*   device,
                                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                                    uint8_t                  bfValue);

int32_t adrv9025_OrxDynamicSlicerAgcSyncEnableBfGet(adi_adrv9025_Device_t*   device,
                                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                                    uint8_t*                 bfValue);

int32_t adrv9025_OrxDynamicSlicerModeEnBfSet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t                  bfValue);

int32_t adrv9025_OrxDynamicSlicerModeEnBfGet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t*                 bfValue);

int32_t adrv9025_OrxDynamicSlicerResendBfSet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t                  bfValue);

int32_t adrv9025_OrxDynamicSlicerSyncHeadBfSet(adi_adrv9025_Device_t*   device,
                                               adrv9025_BfOrxChanAddr_e baseAddr,
                                               uint32_t                 bfValue);

int32_t adrv9025_OrxDynamicSlicerSyncHeadBfGet(adi_adrv9025_Device_t*   device,
                                               adrv9025_BfOrxChanAddr_e baseAddr,
                                               uint32_t*                bfValue);

int32_t adrv9025_OrxFpEnBfSet(adi_adrv9025_Device_t*   device,
                              adrv9025_BfOrxChanAddr_e baseAddr,
                              uint8_t                  bfValue);

int32_t adrv9025_OrxFpEnBfGet(adi_adrv9025_Device_t*   device,
                              adrv9025_BfOrxChanAddr_e baseAddr,
                              uint8_t*                 bfValue);

int32_t adrv9025_OrxFpExponentBitsBfSet(adi_adrv9025_Device_t*         device,
                                        adrv9025_BfOrxChanAddr_e       baseAddr,
                                        adrv9025_BfOrxFpExponentBits_e bfValue);

int32_t adrv9025_OrxFpExponentBitsBfGet(adi_adrv9025_Device_t*          device,
                                        adrv9025_BfOrxChanAddr_e        baseAddr,
                                        adrv9025_BfOrxFpExponentBits_e* bfValue);

int32_t adrv9025_OrxFpFloatDataFormatBfSet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t                  bfValue);

int32_t adrv9025_OrxFpFloatDataFormatBfGet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t*                 bfValue);

int32_t adrv9025_OrxFpHideLeadingOnesBfSet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t                  bfValue);

int32_t adrv9025_OrxFpHideLeadingOnesBfGet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t*                 bfValue);

int32_t adrv9025_OrxFpIntDataAttenBfSet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t                  bfValue);

int32_t adrv9025_OrxFpIntDataAttenBfGet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t*                 bfValue);

int32_t adrv9025_OrxFpNanEncEnBfSet(adi_adrv9025_Device_t*   device,
                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                    uint8_t                  bfValue);

int32_t adrv9025_OrxFpNanEncEnBfGet(adi_adrv9025_Device_t*   device,
                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                    uint8_t*                 bfValue);

int32_t adrv9025_OrxFpRoundModeBfSet(adi_adrv9025_Device_t*      device,
                                     adrv9025_BfOrxChanAddr_e    baseAddr,
                                     adrv9025_BfOrxFpRoundMode_e bfValue);

int32_t adrv9025_OrxFpRoundModeBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfOrxChanAddr_e     baseAddr,
                                     adrv9025_BfOrxFpRoundMode_e* bfValue);

int32_t adrv9025_OrxGainCompEnableBfSet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t                  bfValue);

int32_t adrv9025_OrxGainCompEnableBfGet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t*                 bfValue);

int32_t adrv9025_OrxGainCompForExtGainBfSet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t                  bfValue);

int32_t adrv9025_OrxGainCompForExtGainBfGet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t*                 bfValue);

int32_t adrv9025_OrxGainCompForTempGainBfSet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t                  bfValue);

int32_t adrv9025_OrxGainCompForTempGainBfGet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t*                 bfValue);

int32_t adrv9025_OrxIntDataFormatBfSet(adi_adrv9025_Device_t*   device,
                                       adrv9025_BfOrxChanAddr_e baseAddr,
                                       uint8_t                  bfValue);

int32_t adrv9025_OrxIntDataFormatBfGet(adi_adrv9025_Device_t*   device,
                                       adrv9025_BfOrxChanAddr_e baseAddr,
                                       uint8_t*                 bfValue);

int32_t adrv9025_OrxIntDataResolutionBfSet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t                  bfValue);

int32_t adrv9025_OrxIntDataResolutionBfGet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t*                 bfValue);

int32_t adrv9025_OrxIntEmbedSlicerBfSet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t                  bfValue);

int32_t adrv9025_OrxIntEmbedSlicerBfGet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t*                 bfValue);

int32_t adrv9025_OrxIntEmbedSlicerNumberBfSet(adi_adrv9025_Device_t*   device,
                                              adrv9025_BfOrxChanAddr_e baseAddr,
                                              uint8_t                  bfValue);

int32_t adrv9025_OrxIntEmbedSlicerNumberBfGet(adi_adrv9025_Device_t*   device,
                                              adrv9025_BfOrxChanAddr_e baseAddr,
                                              uint8_t*                 bfValue);

int32_t adrv9025_OrxIntEmbedSlicerPosBfSet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t                  bfValue);

int32_t adrv9025_OrxIntEmbedSlicerPosBfGet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t*                 bfValue);

int32_t adrv9025_OrxIntEvenParityBfSet(adi_adrv9025_Device_t*   device,
                                       adrv9025_BfOrxChanAddr_e baseAddr,
                                       uint8_t                  bfValue);

int32_t adrv9025_OrxIntEvenParityBfGet(adi_adrv9025_Device_t*   device,
                                       adrv9025_BfOrxChanAddr_e baseAddr,
                                       uint8_t*                 bfValue);

int32_t adrv9025_OrxIntParitySupportBfSet(adi_adrv9025_Device_t*   device,
                                          adrv9025_BfOrxChanAddr_e baseAddr,
                                          uint8_t                  bfValue);

int32_t adrv9025_OrxIntParitySupportBfGet(adi_adrv9025_Device_t*   device,
                                          adrv9025_BfOrxChanAddr_e baseAddr,
                                          uint8_t*                 bfValue);

int32_t adrv9025_OrxIntSlicerLsbOnQBfSet(adi_adrv9025_Device_t*   device,
                                         adrv9025_BfOrxChanAddr_e baseAddr,
                                         uint8_t                  bfValue);

int32_t adrv9025_OrxIntSlicerLsbOnQBfGet(adi_adrv9025_Device_t*   device,
                                         adrv9025_BfOrxChanAddr_e baseAddr,
                                         uint8_t*                 bfValue);

int32_t adrv9025_OrxMaxSlicerOverrideBfSet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t                  bfValue);

int32_t adrv9025_OrxOrx1AgcGainIndexBfGet(adi_adrv9025_Device_t*   device,
                                          adrv9025_BfOrxChanAddr_e baseAddr,
                                          uint8_t*                 bfValue);

int32_t adrv9025_OrxOrx1AgcManualGainIndexBfSet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t                  bfValue);

int32_t adrv9025_OrxOrx1AgcManualGainIndexBfGet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t*                 bfValue);

int32_t adrv9025_OrxOrx1AgcSetupBfSet(adi_adrv9025_Device_t*   device,
                                      adrv9025_BfOrxChanAddr_e baseAddr,
                                      uint8_t                  bfValue);

int32_t adrv9025_OrxOrx1AgcSetupBfGet(adi_adrv9025_Device_t*   device,
                                      adrv9025_BfOrxChanAddr_e baseAddr,
                                      uint8_t*                 bfValue);

int32_t adrv9025_OrxOrx2AgcGainIndexBfGet(adi_adrv9025_Device_t*   device,
                                          adrv9025_BfOrxChanAddr_e baseAddr,
                                          uint8_t*                 bfValue);

int32_t adrv9025_OrxOrx2AgcManualGainIndexBfSet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t                  bfValue);

int32_t adrv9025_OrxOrx2AgcManualGainIndexBfGet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t*                 bfValue);

int32_t adrv9025_OrxOrx2AgcSetupBfSet(adi_adrv9025_Device_t*   device,
                                      adrv9025_BfOrxChanAddr_e baseAddr,
                                      uint8_t                  bfValue);

int32_t adrv9025_OrxOrx2AgcSetupBfGet(adi_adrv9025_Device_t*   device,
                                      adrv9025_BfOrxChanAddr_e baseAddr,
                                      uint8_t*                 bfValue);

int32_t adrv9025_OrxRxTempGainCompBfSet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t                  bfValue);

int32_t adrv9025_OrxRxTempGainCompBfGet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t*                 bfValue);

int32_t adrv9025_OrxRxdpSlicerPinCntrlGpioSelectBfGet(adi_adrv9025_Device_t*   device,
                                                      adrv9025_BfOrxChanAddr_e baseAddr,
                                                      uint8_t*                 bfValue);

int32_t adrv9025_OrxRxdpSlicerPositionBfGet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t*                 bfValue);

int32_t adrv9025_OrxSlicerPinControlStepBfSet(adi_adrv9025_Device_t*   device,
                                              adrv9025_BfOrxChanAddr_e baseAddr,
                                              uint8_t                  bfValue);

int32_t adrv9025_OrxSlicerPinControlStepBfGet(adi_adrv9025_Device_t*   device,
                                              adrv9025_BfOrxChanAddr_e baseAddr,
                                              uint8_t*                 bfValue);

int32_t adrv9025_OrxStatic3bitSlicerModeEnBfSet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t                  bfValue);

#ifdef __cplusplus
}
#endif
#endif
