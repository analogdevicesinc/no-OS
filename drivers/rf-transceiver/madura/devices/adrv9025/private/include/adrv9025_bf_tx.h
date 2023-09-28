/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file Automatically generated file: adrv9025_bf_tx.h
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

#ifndef __ADRV9025_BF_TX_H__
#define __ADRV9025_BF_TX_H__
#include "./../../private/include/adrv9025_bf_tx_types.h"
#include "adi_adrv9025.h"
#ifdef __cplusplus
extern "C"{
#endif

int32_t adrv9025_TxArmOverrideControlBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfTxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_TxArmOverrideControlBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfTxChanAddr_e baseAddr,
                                           uint8_t*                bfValue);

int32_t adrv9025_TxCfrBasePulse21bBfSet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfTxChanAddr_e baseAddr,
                                        uint8_t                 bfValue);

int32_t adrv9025_TxCfrBasePulse21bBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfTxChanAddr_e baseAddr,
                                        uint8_t*                bfValue);

int32_t adrv9025_TxCfrHalfPulseLenBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfTxChanAddr_e baseAddr,
                                        uint16_t*               bfValue);

int32_t adrv9025_TxDpdActClkEnableBfSet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfTxChanAddr_e baseAddr,
                                        uint8_t                 bfValue);

int32_t adrv9025_TxDpdActClkEnableBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfTxChanAddr_e baseAddr,
                                        uint8_t*                bfValue);

int32_t adrv9025_TxJesdDfrmMaskBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfTxChanAddr_e baseAddr,
                                     uint8_t                 bfValue);

int32_t adrv9025_TxJesdDfrmMaskBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfTxChanAddr_e baseAddr,
                                     uint8_t*                bfValue);

int32_t adrv9025_TxNcoTestEnableBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t                 bfValue);

int32_t adrv9025_TxNcoTestEnableBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionAprEnBfSet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfTxChanAddr_e baseAddr,
                                          uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionAprEnBfGet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfTxChanAddr_e baseAddr,
                                          uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionAveragePeakRatioBfGet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfTxChanAddr_e baseAddr,
                                                     uint16_t*               bfValue);

int32_t adrv9025_TxPaProtectionAveragePowerBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint16_t*               bfValue);

int32_t adrv9025_TxPaProtectionAvgThresholdBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint16_t                bfValue);

int32_t adrv9025_TxPaProtectionAvgThresholdBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint16_t*               bfValue);

int32_t adrv9025_TxPaProtectionAvgpowerEnBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionAvgpowerEnBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionAvgpowerErrorBfGet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfTxChanAddr_e baseAddr,
                                                  uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionAvgpowerErrorClearBfSet(adi_adrv9025_Device_t*  device,
                                                       adrv9025_BfTxChanAddr_e baseAddr,
                                                       uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionAvgpowerErrorClearRequiredBfSet(adi_adrv9025_Device_t*  device,
                                                               adrv9025_BfTxChanAddr_e baseAddr,
                                                               uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionAvrgDurBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfTxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionAvrgDurBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfTxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionErrorPowerBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint16_t*               bfValue);

int32_t adrv9025_TxPaProtectionGainRampDownEnBfSet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfTxChanAddr_e baseAddr,
                                                   uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionGainRampDownEnBfGet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfTxChanAddr_e baseAddr,
                                                   uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionGainRampUpEnBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionGainRampUpEnBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionGainStepDownEnBfSet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfTxChanAddr_e baseAddr,
                                                   uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionInputSelBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfTxChanAddr_e baseAddr,
                                             uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionInputSelBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfTxChanAddr_e baseAddr,
                                             uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionPeakCountBfSet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfTxChanAddr_e baseAddr,
                                              uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionPeakCountBfGet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfTxChanAddr_e baseAddr,
                                              uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionPeakDurBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfTxChanAddr_e baseAddr,
                                            uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionPeakDurBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfTxChanAddr_e baseAddr,
                                            uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionPeakThresholdBfSet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfTxChanAddr_e baseAddr,
                                                  uint16_t                bfValue);

int32_t adrv9025_TxPaProtectionPeakThresholdBfGet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfTxChanAddr_e baseAddr,
                                                  uint16_t*               bfValue);

int32_t adrv9025_TxPaProtectionPeakpowerEnBfSet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionPeakpowerEnBfGet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionPeakpowerErrorBfGet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfTxChanAddr_e baseAddr,
                                                   uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionPeakpowerErrorClearBfSet(adi_adrv9025_Device_t*  device,
                                                        adrv9025_BfTxChanAddr_e baseAddr,
                                                        uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionPeakpowerErrorClearRequiredBfSet(adi_adrv9025_Device_t*  device,
                                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                                uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionRampMaxAttenuationBfSet(adi_adrv9025_Device_t*  device,
                                                       adrv9025_BfTxChanAddr_e baseAddr,
                                                       uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionRampMaxAttenuationBfGet(adi_adrv9025_Device_t*  device,
                                                       adrv9025_BfTxChanAddr_e baseAddr,
                                                       uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionRampStepDurationBfSet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfTxChanAddr_e baseAddr,
                                                     uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionRampStepDurationBfGet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfTxChanAddr_e baseAddr,
                                                     uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionRampStepSizeBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint8_t                 bfValue);

int32_t adrv9025_TxPaProtectionRampStepSizeBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint8_t*                bfValue);

int32_t adrv9025_TxPaProtectionTxAttenStepBfSet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                uint8_t                 bfValue);

int32_t adrv9025_TxPllJesdProtClrBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfTxChanAddr_e baseAddr,
                                       uint8_t                 bfValue);

int32_t adrv9025_TxPllJesdProtClrReqdBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfTxChanAddr_e baseAddr,
                                           uint8_t                 bfValue);

int32_t adrv9025_TxPllJesdProtEventBfGet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfTxChanAddr_e baseAddr,
                                         uint8_t*                bfValue);

int32_t adrv9025_TxPllUnlockMaskBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t                 bfValue);

int32_t adrv9025_TxPllUnlockMaskBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t*                bfValue);

int32_t adrv9025_TxSpi2TxAttenGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfTxChanAddr_e baseAddr,
                                              uint8_t                 bfValue);

int32_t adrv9025_TxSpi2TxAttenGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfTxChanAddr_e baseAddr,
                                              uint8_t*                bfValue);

int32_t adrv9025_TxSrlIrqBfGet(adi_adrv9025_Device_t*  device,
                               adrv9025_BfTxChanAddr_e baseAddr,
                               uint8_t*                bfValue);

int32_t adrv9025_TxSrlIrqEnBfSet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfTxChanAddr_e baseAddr,
                                 uint8_t                 bfValue);

int32_t adrv9025_TxSrlIrqEnBfGet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfTxChanAddr_e baseAddr,
                                 uint8_t*                bfValue);

int32_t adrv9025_TxSrlSlewOffsetBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t                 bfValue);

int32_t adrv9025_TxSrlSlewOffsetBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t*                bfValue);

int32_t adrv9025_TxSrlStatBfGet(adi_adrv9025_Device_t*  device,
                                adrv9025_BfTxChanAddr_e baseAddr,
                                uint16_t*               bfValue);

int32_t adrv9025_TxSrlStatEnBfSet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t                 bfValue);

int32_t adrv9025_TxSrlStatEnBfGet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t*                bfValue);

int32_t adrv9025_TxSrlStatModeBfSet(adi_adrv9025_Device_t*  device,
                                    adrv9025_BfTxChanAddr_e baseAddr,
                                    uint8_t                 bfValue);

int32_t adrv9025_TxSrlTableSelBfSet(adi_adrv9025_Device_t*  device,
                                    adrv9025_BfTxChanAddr_e baseAddr,
                                    uint8_t                 bfValue);

int32_t adrv9025_TxSrlTableSelBfGet(adi_adrv9025_Device_t*  device,
                                    adrv9025_BfTxChanAddr_e baseAddr,
                                    uint8_t*                bfValue);

int32_t adrv9025_TxTpcDecrAttenGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t                 bfValue);

int32_t adrv9025_TxTpcDecrAttenGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t*                bfValue);

int32_t adrv9025_TxTpcIncrAttenGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t                 bfValue);

int32_t adrv9025_TxTpcIncrAttenGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t*                bfValue);

int32_t adrv9025_TxTxAttenConfigBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t                 bfValue);

int32_t adrv9025_TxTxAttenConfigBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t*                bfValue);

int32_t adrv9025_TxTxAttenModeBfSet(adi_adrv9025_Device_t*  device,
                                    adrv9025_BfTxChanAddr_e baseAddr,
                                    uint8_t                 bfValue);

int32_t adrv9025_TxTxAttenUpdGpioEnBfSet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfTxChanAddr_e baseAddr,
                                         uint8_t                 bfValue);

int32_t adrv9025_TxTxAttenUpdGpioEnBfGet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfTxChanAddr_e baseAddr,
                                         uint8_t*                bfValue);

int32_t adrv9025_TxTxAttenUpdGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfTxChanAddr_e baseAddr,
                                             uint8_t                 bfValue);

int32_t adrv9025_TxTxAttenUpdGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfTxChanAddr_e baseAddr,
                                             uint8_t*                bfValue);

int32_t adrv9025_TxTxAttenuationBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint16_t                bfValue);

int32_t adrv9025_TxTxAttenuationBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint16_t*               bfValue);

int32_t adrv9025_TxTxDataRampDownOnJesdDfrmIrqBfSet(adi_adrv9025_Device_t*  device,
                                                    adrv9025_BfTxChanAddr_e baseAddr,
                                                    uint8_t                 bfValue);

int32_t adrv9025_TxTxDataRampDownOnJesdDfrmIrqBfGet(adi_adrv9025_Device_t*  device,
                                                    adrv9025_BfTxChanAddr_e baseAddr,
                                                    uint8_t*                bfValue);

int32_t adrv9025_TxTxDataRampDownOnPllUnlockBfSet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfTxChanAddr_e baseAddr,
                                                  uint8_t                 bfValue);

int32_t adrv9025_TxTxDataRampDownOnPllUnlockBfGet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfTxChanAddr_e baseAddr,
                                                  uint8_t*                bfValue);

int32_t adrv9025_TxTxDpNcoEnableBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t                 bfValue);

int32_t adrv9025_TxTxDpNcoFtwBfSet(adi_adrv9025_Device_t*  device,
                                   adrv9025_BfTxChanAddr_e baseAddr,
                                   uint32_t                bfValue);

int32_t adrv9025_TxTxDpNcoFtwBfGet(adi_adrv9025_Device_t*  device,
                                   adrv9025_BfTxChanAddr_e baseAddr,
                                   uint32_t*               bfValue);

int32_t adrv9025_TxTxDpNcoFtwUpdateBfSet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfTxChanAddr_e baseAddr,
                                         uint8_t                 bfValue);

int32_t adrv9025_TxTxEnableBfSet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfTxChanAddr_e baseAddr,
                                 uint8_t                 bfValue);

int32_t adrv9025_TxTxEnableBfGet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfTxChanAddr_e baseAddr,
                                 uint8_t*                bfValue);

int32_t adrv9025_TxTxIncrDecrWordBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfTxChanAddr_e baseAddr,
                                       uint8_t                 bfValue);

int32_t adrv9025_TxTxIncrDecrWordBfGet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfTxChanAddr_e baseAddr,
                                       uint8_t*                bfValue);

int32_t adrv9025_TxTxNcoGainBfSet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t                 bfValue);

int32_t adrv9025_TxTxNcoGainBfGet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t*                bfValue);

int32_t adrv9025_TxTxPinModeBfSet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t                 bfValue);

int32_t adrv9025_TxTxPinModeBfGet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t*                bfValue);

int32_t adrv9025_TxTxAttenuationEarlyDelayCounterForAnalogBfSet(adi_adrv9025_Device_t*  device,
                                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                                uint8_t                 bfValue);

int32_t adrv9025_TxTxAttenuationEarlyDelayCounterForAnalogBfGet(adi_adrv9025_Device_t*  device,
                                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                                uint8_t*                bfValue);

#ifdef __cplusplus
}
#endif
#endif
