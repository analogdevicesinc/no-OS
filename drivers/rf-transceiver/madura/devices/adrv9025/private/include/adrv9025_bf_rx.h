/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file Automatically generated file: adrv9025_bf_rx.h
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

#ifndef __ADRV9025_BF_RX_H__
#define __ADRV9025_BF_RX_H__
#include "./../../private/include/adrv9025_bf_rx_types.h"
#include "./../../private/include/adrv9025_bf_orx_types.h"
#include "adi_adrv9025.h"
#ifdef __cplusplus
extern "C"{
#endif

int32_t adrv9025_RxAdcOverloadResetEnBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxAdcOverloadResetEnBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxAdcovrg2ndHighCounterBfSet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfRxChanAddr_e baseAddr,
                                              uint8_t                 bfValue);

int32_t adrv9025_RxAgcAdcHighOvrgExceededCounterBfSet(adi_adrv9025_Device_t*  device,
                                                      adrv9025_BfRxChanAddr_e baseAddr,
                                                      uint8_t                 bfValue);

int32_t adrv9025_RxAgcAdcHighOvrgExceededCounterBfGet(adi_adrv9025_Device_t*  device,
                                                      adrv9025_BfRxChanAddr_e baseAddr,
                                                      uint8_t*                bfValue);

int32_t adrv9025_RxAgcAdcLowOvrgExceededCounterBfSet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfRxChanAddr_e baseAddr,
                                                     uint8_t                 bfValue);

int32_t adrv9025_RxAgcAdcLowOvrgExceededCounterBfGet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfRxChanAddr_e baseAddr,
                                                     uint8_t*                bfValue);

int32_t adrv9025_RxAgcAdcResetGainStepBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxAgcAdcResetGainStepBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_RxAgcAdcovrgLowInt0CounterBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t                 bfValue);

int32_t adrv9025_RxAgcAdcovrgLowInt0CounterBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t*                bfValue);

int32_t adrv9025_RxAgcAdcovrgLowInt1CounterBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t                 bfValue);

int32_t adrv9025_RxAgcAdcovrgLowInt1CounterBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t*                bfValue);

int32_t adrv9025_RxAgcApdLowFreqErrorMitigationModeBfSet(adi_adrv9025_Device_t*  device,
                                                         adrv9025_BfRxChanAddr_e baseAddr,
                                                         uint8_t                 bfValue);

int32_t adrv9025_RxAgcAttackDelayBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t                 bfValue);

int32_t adrv9025_RxAgcAttackDelayBfGet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t*                bfValue);

int32_t adrv9025_RxAgcChangeGainIfAdcovrgHighBfSet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfRxChanAddr_e baseAddr,
                                                   uint8_t                 bfValue);

int32_t adrv9025_RxAgcChangeGainIfAdcovrgHighBfGet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfRxChanAddr_e baseAddr,
                                                   uint8_t*                bfValue);

int32_t adrv9025_RxAgcChangeGainIfUlbthHighBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t                 bfValue);

int32_t adrv9025_RxAgcChangeGainIfUlbthHighBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t*                bfValue);

int32_t adrv9025_RxAgcDecGainGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t                 bfValue);

int32_t adrv9025_RxAgcDecGainGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t*                bfValue);

int32_t adrv9025_RxAgcDecrGainStepSizeBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxAgcDecrGainStepSizeBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_RxAgcEnableFastRecoveryLoopBfSet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfRxChanAddr_e baseAddr,
                                                  uint8_t                 bfValue);

int32_t adrv9025_RxAgcEnableFastRecoveryLoopBfGet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfRxChanAddr_e baseAddr,
                                                  uint8_t*                bfValue);

int32_t adrv9025_RxAgcEnableGainIndexUpdateBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t*                bfValue);

int32_t adrv9025_OrxAgcEnableGainIndexUpdateBfGet(adi_adrv9025_Device_t*   device,
                                                  adrv9025_BfOrxChanAddr_e baseAddr,
                                                  uint8_t*                 bfValue);

int32_t adrv9025_RxAgcEnableSyncPulseForGainCounterBfSet(adi_adrv9025_Device_t*  device,
                                                         adrv9025_BfRxChanAddr_e baseAddr,
                                                         uint8_t                 bfValue);

int32_t adrv9025_RxAgcEnableSyncPulseForGainCounterBfGet(adi_adrv9025_Device_t*  device,
                                                         adrv9025_BfRxChanAddr_e baseAddr,
                                                         uint8_t*                bfValue);

int32_t adrv9025_RxAgcGainIndexBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t*                bfValue);

int32_t adrv9025_RxAgcGainUpdateCounterBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint32_t                bfValue);

int32_t adrv9025_RxAgcGainUpdateCounterBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint32_t*               bfValue);

int32_t adrv9025_RxAgcIncGainGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t                 bfValue);

int32_t adrv9025_RxAgcIncGainGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t*                bfValue);

int32_t adrv9025_RxAgcIncrGainStepSizeBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxAgcIncrGainStepSizeBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_RxAgcLlbGainStepBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t                 bfValue);

int32_t adrv9025_RxAgcLlbGainStepBfGet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t*                bfValue);

int32_t adrv9025_RxAgcLlbThresholdExceededCounterBfSet(adi_adrv9025_Device_t*  device,
                                                       adrv9025_BfRxChanAddr_e baseAddr,
                                                       uint8_t                 bfValue);

int32_t adrv9025_RxAgcLlbThresholdExceededCounterBfGet(adi_adrv9025_Device_t*  device,
                                                       adrv9025_BfRxChanAddr_e baseAddr,
                                                       uint8_t*                bfValue);

int32_t adrv9025_RxAgcLockLevelBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 bfValue);

int32_t adrv9025_RxAgcLockLevelBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t*                bfValue);

int32_t adrv9025_RxAgcLowThsPreventGainIncBfSet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                uint8_t                 bfValue);

int32_t adrv9025_RxAgcLowThsPreventGainIncBfGet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                uint8_t*                bfValue);

int32_t adrv9025_RxAgcLower0ThresholdBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxAgcLower0ThresholdBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxAgcLower0ThresholdExceededGainStepBfSet(adi_adrv9025_Device_t*  device,
                                                           adrv9025_BfRxChanAddr_e baseAddr,
                                                           uint8_t                 bfValue);

int32_t adrv9025_RxAgcLower0ThresholdExceededGainStepBfGet(adi_adrv9025_Device_t*  device,
                                                           adrv9025_BfRxChanAddr_e baseAddr,
                                                           uint8_t*                bfValue);

int32_t adrv9025_RxAgcLower1ThresholdBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxAgcLower1ThresholdBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxAgcLower1ThresholdExceededGainStepBfSet(adi_adrv9025_Device_t*  device,
                                                           adrv9025_BfRxChanAddr_e baseAddr,
                                                           uint8_t                 bfValue);

int32_t adrv9025_RxAgcLower1ThresholdExceededGainStepBfGet(adi_adrv9025_Device_t*  device,
                                                           adrv9025_BfRxChanAddr_e baseAddr,
                                                           uint8_t*                bfValue);

int32_t adrv9025_RxAgcManualGainIndexBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxAgcManualGainIndexBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxAgcManualGainPinControlBfSet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                uint8_t                 bfValue);

int32_t adrv9025_RxAgcManualGainPinControlBfGet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                uint8_t*                bfValue);

int32_t adrv9025_RxAgcMaximumGainIndexBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxAgcMaximumGainIndexBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_RxAgcMinimumGainIndexBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxAgcMinimumGainIndexBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_RxAgcOvrgHighGainStepBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxAgcOvrgHighGainStepBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_RxAgcOvrgLowGainStepBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxAgcOvrgLowGainStepBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxAgcOvrgLowInt0GainStepBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint8_t                 bfValue);

int32_t adrv9025_RxAgcOvrgLowInt0GainStepBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint8_t*                bfValue);

int32_t adrv9025_RxAgcOvrgLowInt1GainStepBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint8_t                 bfValue);

int32_t adrv9025_RxAgcOvrgLowInt1GainStepBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint8_t*                bfValue);

int32_t adrv9025_RxAgcPeakThresholdGainControlModeBfSet(adi_adrv9025_Device_t*  device,
                                                        adrv9025_BfRxChanAddr_e baseAddr,
                                                        uint8_t                 bfValue);

int32_t adrv9025_RxAgcPeakThresholdGainControlModeBfGet(adi_adrv9025_Device_t*  device,
                                                        adrv9025_BfRxChanAddr_e baseAddr,
                                                        uint8_t*                bfValue);

int32_t adrv9025_RxAgcPeakWaitTimeBfSet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfRxChanAddr_e baseAddr,
                                        uint8_t                 bfValue);

int32_t adrv9025_RxAgcPeakWaitTimeBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfRxChanAddr_e baseAddr,
                                        uint8_t*                bfValue);

int32_t adrv9025_RxAgcResetOnRxonBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t                 bfValue);

int32_t adrv9025_RxAgcResetOnRxonBfGet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t*                bfValue);

int32_t adrv9025_RxAgcSetupBfSet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfRxChanAddr_e baseAddr,
                                 uint8_t                 bfValue);

int32_t adrv9025_RxAgcSetupBfGet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfRxChanAddr_e baseAddr,
                                 uint8_t*                bfValue);

int32_t adrv9025_RxAgcSlowLoopSettlingDelayBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t                 bfValue);

int32_t adrv9025_RxAgcSlowLoopSettlingDelayBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t*                bfValue);

int32_t adrv9025_RxAgcSlowloopFastGainChangeBlockEnableBfSet(adi_adrv9025_Device_t*  device,
                                                             adrv9025_BfRxChanAddr_e baseAddr,
                                                             uint8_t                 bfValue);

int32_t adrv9025_RxAgcSoftResetBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 bfValue);

int32_t adrv9025_RxAgcUlSigPowerMeasDelayBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint16_t                bfValue);

int32_t adrv9025_RxAgcUlSigPowerMeasDelayBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint16_t*               bfValue);

int32_t adrv9025_RxAgcUlSigPowerMeasDurationBfSet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfRxChanAddr_e baseAddr,
                                                  uint16_t                bfValue);

int32_t adrv9025_RxAgcUlSigPowerMeasDurationBfGet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfRxChanAddr_e baseAddr,
                                                  uint16_t*               bfValue);

int32_t adrv9025_RxAgcUlbGainStepBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t                 bfValue);

int32_t adrv9025_RxAgcUlbGainStepBfGet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t*                bfValue);

int32_t adrv9025_RxAgcUlbThresholdExceededCounterBfSet(adi_adrv9025_Device_t*  device,
                                                       adrv9025_BfRxChanAddr_e baseAddr,
                                                       uint8_t                 bfValue);

int32_t adrv9025_RxAgcUlbThresholdExceededCounterBfGet(adi_adrv9025_Device_t*  device,
                                                       adrv9025_BfRxChanAddr_e baseAddr,
                                                       uint8_t*                bfValue);

int32_t adrv9025_RxAgcUpper0ThresholdExceededGainStepBfSet(adi_adrv9025_Device_t*  device,
                                                           adrv9025_BfRxChanAddr_e baseAddr,
                                                           uint8_t                 bfValue);

int32_t adrv9025_RxAgcUpper0ThresholdExceededGainStepBfGet(adi_adrv9025_Device_t*  device,
                                                           adrv9025_BfRxChanAddr_e baseAddr,
                                                           uint8_t*                bfValue);

int32_t adrv9025_RxAgcUpper1ThresholdBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxAgcUpper1ThresholdBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxAgcUpper1ThresholdExceededGainStepBfSet(adi_adrv9025_Device_t*  device,
                                                           adrv9025_BfRxChanAddr_e baseAddr,
                                                           uint8_t                 bfValue);

int32_t adrv9025_RxAgcUpper1ThresholdExceededGainStepBfGet(adi_adrv9025_Device_t*  device,
                                                           adrv9025_BfRxChanAddr_e baseAddr,
                                                           uint8_t*                bfValue);

int32_t adrv9025_RxAgcUrangeInterval0BfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint16_t                bfValue);

int32_t adrv9025_RxAgcUrangeInterval0BfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint16_t*               bfValue);

int32_t adrv9025_RxAgcUrangeInterval1MultBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint8_t                 bfValue);

int32_t adrv9025_RxAgcUrangeInterval1MultBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint8_t*                bfValue);

int32_t adrv9025_RxAgcUrangeInterval2MultBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint8_t                 bfValue);

int32_t adrv9025_RxAgcUrangeInterval2MultBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint8_t*                bfValue);

int32_t adrv9025_RxApdHighSrcSelectBfSet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfRxChanAddr_e baseAddr,
                                         uint8_t                 bfValue);

int32_t adrv9025_RxApdHighSrcSelectBfGet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfRxChanAddr_e baseAddr,
                                         uint8_t*                bfValue);

int32_t adrv9025_RxApdLowSrcSelectBfSet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfRxChanAddr_e baseAddr,
                                        uint8_t                 bfValue);

int32_t adrv9025_RxApdLowSrcSelectBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfRxChanAddr_e baseAddr,
                                        uint8_t*                bfValue);

int32_t adrv9025_RxArmOverrideControlBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxArmOverrideControlBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxBbdcMShiftBfSet(adi_adrv9025_Device_t*  device,
                                   adrv9025_BfRxChanAddr_e baseAddr,
                                   uint8_t                 bfValue);

int32_t adrv9025_RxBbdcMShiftBfGet(adi_adrv9025_Device_t*  device,
                                   adrv9025_BfRxChanAddr_e baseAddr,
                                   uint8_t*                bfValue);

int32_t adrv9025_RxBbdcTrackingEnableBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxBbdcTrackingEnableBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxControlOutMuxAddressBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t                 bfValue);

int32_t adrv9025_RxDecOverloadDurationCountBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t                 bfValue);

int32_t adrv9025_RxDecOverloadDurationCountBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e baseAddr,
                                                 uint8_t*                bfValue);

int32_t adrv9025_RxDecOverloadPowerModeBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t                 bfValue);

int32_t adrv9025_RxDecOverloadPowerModeBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t*                bfValue);

int32_t adrv9025_RxDecOverloadThresholdCountBfSet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfRxChanAddr_e baseAddr,
                                                  uint8_t                 bfValue);

int32_t adrv9025_RxDecOverloadThresholdCountBfGet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfRxChanAddr_e baseAddr,
                                                  uint8_t*                bfValue);

int32_t adrv9025_RxDecPowerBfGet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfRxChanAddr_e baseAddr,
                                 uint8_t*                bfValue);

int32_t adrv9025_RxDecPowerEnableMeasBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxDecPowerEnableMeasBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxDecPowerInputSelectBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxDecPowerInputSelectBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_RxDecPowerLogShiftBfSet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfRxChanAddr_e baseAddr,
                                         uint8_t                 bfValue);

int32_t adrv9025_RxDecPowerLogShiftBfGet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfRxChanAddr_e baseAddr,
                                         uint8_t*                bfValue);

int32_t adrv9025_RxDecPowerMeasurementDurationBfSet(adi_adrv9025_Device_t*  device,
                                                    adrv9025_BfRxChanAddr_e baseAddr,
                                                    uint8_t                 bfValue);

int32_t adrv9025_RxDecPowerMeasurementDurationBfGet(adi_adrv9025_Device_t*  device,
                                                    adrv9025_BfRxChanAddr_e baseAddr,
                                                    uint8_t*                bfValue);

int32_t adrv9025_RxDecThresholdConfigBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxDecThresholdConfigBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxDecimatedDataOverloadInt0LowerThresholdBfSet(adi_adrv9025_Device_t*  device,
                                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                                uint16_t                bfValue);

int32_t adrv9025_RxDecimatedDataOverloadInt0LowerThresholdBfGet(adi_adrv9025_Device_t*  device,
                                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                                uint16_t*               bfValue);

int32_t adrv9025_RxDecimatedDataOverloadInt1LowerThresholdBfSet(adi_adrv9025_Device_t*  device,
                                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                                uint16_t                bfValue);

int32_t adrv9025_RxDecimatedDataOverloadInt1LowerThresholdBfGet(adi_adrv9025_Device_t*  device,
                                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                                uint16_t*               bfValue);

int32_t adrv9025_RxDecimatedDataOverloadLowerThresholdBfSet(adi_adrv9025_Device_t*  device,
                                                            adrv9025_BfRxChanAddr_e baseAddr,
                                                            uint16_t                bfValue);

int32_t adrv9025_RxDecimatedDataOverloadLowerThresholdBfGet(adi_adrv9025_Device_t*  device,
                                                            adrv9025_BfRxChanAddr_e baseAddr,
                                                            uint16_t*               bfValue);

int32_t adrv9025_RxDecimatedDataOverloadSecondaryUpperThresholdBfSet(adi_adrv9025_Device_t*  device,
                                                                     adrv9025_BfRxChanAddr_e baseAddr,
                                                                     uint16_t                bfValue);

int32_t adrv9025_RxDecimatedDataOverloadUpperThresholdBfSet(adi_adrv9025_Device_t*  device,
                                                            adrv9025_BfRxChanAddr_e baseAddr,
                                                            uint16_t                bfValue);

int32_t adrv9025_RxDecimatedDataOverloadUpperThresholdBfGet(adi_adrv9025_Device_t*  device,
                                                            adrv9025_BfRxChanAddr_e baseAddr,
                                                            uint16_t*               bfValue);

int32_t adrv9025_RxDynamicSlicerAgcSyncEnableBfSet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfRxChanAddr_e baseAddr,
                                                   uint8_t                 bfValue);

int32_t adrv9025_RxDynamicSlicerAgcSyncEnableBfGet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfRxChanAddr_e baseAddr,
                                                   uint8_t*                bfValue);

int32_t adrv9025_RxDynamicSlicerModeEnBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxDynamicSlicerModeEnBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_RxDynamicSlicerResendBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxDynamicSlicerSyncHeadBfSet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfRxChanAddr_e baseAddr,
                                              uint32_t                bfValue);

int32_t adrv9025_RxDynamicSlicerSyncHeadBfGet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfRxChanAddr_e baseAddr,
                                              uint32_t*               bfValue);

int32_t adrv9025_RxEnableDecOverloadBfSet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t                 bfValue);

int32_t adrv9025_RxEnableDecOverloadBfGet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t*                bfValue);

int32_t adrv9025_RxExternalSlicerPinControlStepBfSet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfRxChanAddr_e baseAddr,
                                                     uint8_t                 bfValue);

int32_t adrv9025_RxExternalSlicerPinControlStepBfGet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfRxChanAddr_e baseAddr,
                                                     uint8_t*                bfValue);

int32_t adrv9025_RxFpEnBfSet(adi_adrv9025_Device_t*  device,
                             adrv9025_BfRxChanAddr_e baseAddr,
                             uint8_t                 bfValue);

int32_t adrv9025_RxFpEnBfGet(adi_adrv9025_Device_t*  device,
                             adrv9025_BfRxChanAddr_e baseAddr,
                             uint8_t*                bfValue);

int32_t adrv9025_RxFpExponentBitsBfSet(adi_adrv9025_Device_t*        device,
                                       adrv9025_BfRxChanAddr_e       baseAddr,
                                       adrv9025_BfRxFpExponentBits_e bfValue);

int32_t adrv9025_RxFpExponentBitsBfGet(adi_adrv9025_Device_t*         device,
                                       adrv9025_BfRxChanAddr_e        baseAddr,
                                       adrv9025_BfRxFpExponentBits_e* bfValue);

int32_t adrv9025_RxFpFloatDataFormatBfSet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t                 bfValue);

int32_t adrv9025_RxFpFloatDataFormatBfGet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t*                bfValue);

int32_t adrv9025_RxFpHideLeadingOnesBfSet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t                 bfValue);

int32_t adrv9025_RxFpHideLeadingOnesBfGet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t*                bfValue);

int32_t adrv9025_RxFpIntDataAttenBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t                 bfValue);

int32_t adrv9025_RxFpIntDataAttenBfGet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t*                bfValue);

int32_t adrv9025_RxFpNanEncEnBfSet(adi_adrv9025_Device_t*  device,
                                   adrv9025_BfRxChanAddr_e baseAddr,
                                   uint8_t                 bfValue);

int32_t adrv9025_RxFpNanEncEnBfGet(adi_adrv9025_Device_t*  device,
                                   adrv9025_BfRxChanAddr_e baseAddr,
                                   uint8_t*                bfValue);

int32_t adrv9025_RxFpRoundModeBfSet(adi_adrv9025_Device_t*     device,
                                    adrv9025_BfRxChanAddr_e    baseAddr,
                                    adrv9025_BfRxFpRoundMode_e bfValue);

int32_t adrv9025_RxFpRoundModeBfGet(adi_adrv9025_Device_t*      device,
                                    adrv9025_BfRxChanAddr_e     baseAddr,
                                    adrv9025_BfRxFpRoundMode_e* bfValue);

int32_t adrv9025_RxGainCompEnableBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t                 bfValue);

int32_t adrv9025_RxGainCompEnableBfGet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t*                bfValue);

int32_t adrv9025_RxGainCompForExtGainBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxGainCompForExtGainBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxGainCompForTempGainBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxGainCompForTempGainBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_RxHb2HighSrcSelectBfSet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfRxChanAddr_e baseAddr,
                                         uint8_t                 bfValue);

int32_t adrv9025_RxHb2HighSrcSelectBfGet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfRxChanAddr_e baseAddr,
                                         uint8_t*                bfValue);

int32_t adrv9025_RxHb2LowSrcSelectBfSet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfRxChanAddr_e baseAddr,
                                        uint8_t                 bfValue);

int32_t adrv9025_RxHb2LowSrcSelectBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfRxChanAddr_e baseAddr,
                                        uint8_t*                bfValue);

int32_t adrv9025_RxHb2OverloadUseHb2InBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_RxHb2OverloadUseHb2InBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfRxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_RxIntDataFormatBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfRxChanAddr_e baseAddr,
                                      uint8_t                 bfValue);

int32_t adrv9025_RxIntDataFormatBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfRxChanAddr_e baseAddr,
                                      uint8_t*                bfValue);

int32_t adrv9025_RxIntDataResolutionBfSet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t                 bfValue);

int32_t adrv9025_RxIntDataResolutionBfGet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t*                bfValue);

int32_t adrv9025_RxIntEmbedSlicerBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t                 bfValue);

int32_t adrv9025_RxIntEmbedSlicerBfGet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t*                bfValue);

int32_t adrv9025_RxIntEmbedSlicerNumberBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t                 bfValue);

int32_t adrv9025_RxIntEmbedSlicerNumberBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t*                bfValue);

int32_t adrv9025_RxIntEmbedSlicerPosBfSet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t                 bfValue);

int32_t adrv9025_RxIntEmbedSlicerPosBfGet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t*                bfValue);

int32_t adrv9025_RxIntEvenParityBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfRxChanAddr_e baseAddr,
                                      uint8_t                 bfValue);

int32_t adrv9025_RxIntEvenParityBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfRxChanAddr_e baseAddr,
                                      uint8_t*                bfValue);

int32_t adrv9025_RxIntParitySupportBfSet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfRxChanAddr_e baseAddr,
                                         uint8_t                 bfValue);

int32_t adrv9025_RxIntParitySupportBfGet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfRxChanAddr_e baseAddr,
                                         uint8_t*                bfValue);

int32_t adrv9025_RxIntSlicerLsbOnQBfSet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfRxChanAddr_e baseAddr,
                                        uint8_t                 bfValue);

int32_t adrv9025_RxIntSlicerLsbOnQBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfRxChanAddr_e baseAddr,
                                        uint8_t*                bfValue);

int32_t adrv9025_RxInvertApdLowBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 bfValue);

int32_t adrv9025_RxInvertApdLowBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t*                bfValue);

int32_t adrv9025_RxInvertHb2LowBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 bfValue);

int32_t adrv9025_RxInvertHb2LowBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t*                bfValue);

int32_t adrv9025_RxMaxSlicerOverrideBfSet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t                 bfValue);

int32_t adrv9025_RxPcaGainControlModeBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxPcaGainControlModeBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxPcaGainIndexOffsetBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxPcaGainIndexOffsetBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxPcaGainStepBfSet(adi_adrv9025_Device_t*  device,
                                    adrv9025_BfRxChanAddr_e baseAddr,
                                    uint8_t                 bfValue);

int32_t adrv9025_RxPcaGainStepBfGet(adi_adrv9025_Device_t*  device,
                                    adrv9025_BfRxChanAddr_e baseAddr,
                                    uint8_t*                bfValue);

int32_t adrv9025_RxPcaPinSelBfSet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfRxChanAddr_e baseAddr,
                                  uint8_t                 bfValue);

int32_t adrv9025_RxPcaPinSelBfGet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfRxChanAddr_e baseAddr,
                                  uint8_t*                bfValue);

int32_t adrv9025_RxPcaUpdateManualGainModeBfSet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                uint8_t                 bfValue);

int32_t adrv9025_RxRxEnableBfSet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfRxChanAddr_e baseAddr,
                                 uint8_t                 bfValue);

int32_t adrv9025_RxRxEnableBfGet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfRxChanAddr_e baseAddr,
                                 uint8_t*                bfValue);

int32_t adrv9025_RxRxFdDwellThreshBfSet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfRxChanAddr_e baseAddr,
                                        uint16_t                bfValue);

int32_t adrv9025_RxRxFdDwellThreshBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfRxChanAddr_e baseAddr,
                                        uint16_t*               bfValue);

int32_t adrv9025_RxRxFdFineEnBfSet(adi_adrv9025_Device_t*  device,
                                   adrv9025_BfRxChanAddr_e baseAddr,
                                   uint8_t                 bfValue);

int32_t adrv9025_RxRxFdLowThreshBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfRxChanAddr_e baseAddr,
                                      uint16_t                bfValue);

int32_t adrv9025_RxRxFdLowThreshBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfRxChanAddr_e baseAddr,
                                      uint16_t*               bfValue);

int32_t adrv9025_RxRxFdUpThreshBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint16_t                bfValue);

int32_t adrv9025_RxRxFdUpThreshBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint16_t*               bfValue);

int32_t adrv9025_RxRxFdUseEnvelopMeasBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_RxRxFdUseEnvelopMeasBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxRxMonFormatIBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 channelId,
                                     uint8_t                 bfValue);

int32_t adrv9025_RxRxMonFormatIBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 channelId,
                                     uint8_t*                bfValue);

int32_t adrv9025_RxRxMonFormatQBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 channelId,
                                     uint8_t                 bfValue);

int32_t adrv9025_RxRxMonFormatQBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 channelId,
                                     uint8_t*                bfValue);

int32_t adrv9025_RxRxPinModeBfSet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfRxChanAddr_e baseAddr,
                                  uint8_t                 bfValue);

int32_t adrv9025_RxRxPinModeBfGet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfRxChanAddr_e baseAddr,
                                  uint8_t*                bfValue);

int32_t adrv9025_RxRxSmonOffsetShiftBfSet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t                 bfValue);

int32_t adrv9025_RxRxSmonPeakEnBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 bfValue);

int32_t adrv9025_RxRxSmonPeakEnBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t*                bfValue);

int32_t adrv9025_RxRxSmonPeriodBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint32_t                bfValue);

int32_t adrv9025_RxRxSmonPeriodBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint32_t*               bfValue);

int32_t adrv9025_RxRxSmonSourceBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 bfValue);

int32_t adrv9025_RxRxSmonSourceBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t*                bfValue);

int32_t adrv9025_RxRxSmonSqrtEnBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t                 bfValue);

int32_t adrv9025_RxRxSmonSqrtEnBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfRxChanAddr_e baseAddr,
                                     uint8_t*                bfValue);

int32_t adrv9025_RxRxTempGainCompBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t                 bfValue);

int32_t adrv9025_RxRxTempGainCompBfGet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfRxChanAddr_e baseAddr,
                                       uint8_t*                bfValue);

int32_t adrv9025_RxRxdpSlicerPinCntrlGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfRxChanAddr_e baseAddr,
                                                     uint8_t                 bfValue);

int32_t adrv9025_RxRxdpSlicerPinCntrlGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfRxChanAddr_e baseAddr,
                                                     uint8_t*                bfValue);

int32_t adrv9025_RxRxdpSlicerPositionBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfRxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_RxSlicerPinControlModeBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t                 bfValue);

int32_t adrv9025_RxSlicerPinControlModeBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t*                bfValue);

int32_t adrv9025_RxSlicerPinControlStepBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t                 bfValue);

int32_t adrv9025_RxSlicerPinControlStepBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t*                bfValue);

int32_t adrv9025_RxStatic3bitSlicerModeEnBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfRxChanAddr_e baseAddr,
                                               uint8_t                 bfValue);

int32_t adrv9025_RxAgcClkDivideRatioBfSet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t                 bfValue);

int32_t adrv9025_RxAgcClkDivideRatioBfGet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfRxChanAddr_e baseAddr,
                                          uint8_t*                bfValue);

int32_t adrv9025_RxReferenceClockCyclesBfSet(adi_adrv9025_Device_t  *device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t                 bfValue);
int32_t adrv9025_RxReferenceClockCyclesBfGet(adi_adrv9025_Device_t   *device,
                                             adrv9025_BfRxChanAddr_e baseAddr,
                                             uint8_t                 *bfValue);

int32_t adrv9025_RxAgcDelayCounterBaseRateBfSet(adi_adrv9025_Device_t  *device,
                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                uint8_t                 bfValue);
int32_t adrv9025_RxAgcDelayCounterBaseRateBfGet(adi_adrv9025_Device_t   *device,
                                                adrv9025_BfRxChanAddr_e baseAddr,
                                                uint8_t                 *bfValue);

#ifdef __cplusplus
}
#endif
#endif
