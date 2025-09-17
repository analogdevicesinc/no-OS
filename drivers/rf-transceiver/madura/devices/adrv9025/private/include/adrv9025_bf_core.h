/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file Automatically generated file: adrv9025_bf_core.h
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

#ifndef __ADRV9025_BF_CORE_H__
#define __ADRV9025_BF_CORE_H__
#include "./../../private/include/adrv9025_bf_core_types.h"
#include "adi_adrv9025.h"
#ifdef __cplusplus
extern "C"{
#endif

int32_t adrv9025_CoreArmCommandBusyBfGet(adi_adrv9025_Device_t*    device,
                                         adrv9025_BfCoreChanAddr_e baseAddr,
                                         uint8_t*                  bfValue);

int32_t adrv9025_CoreArmDCommandBusyBfGet(adi_adrv9025_Device_t*    device,
                                          adrv9025_BfCoreChanAddr_e baseAddr,
                                          uint8_t*                  bfValue);

int32_t adrv9025_CoreArmM3RunBfSet(adi_adrv9025_Device_t*    device,
                                   adrv9025_BfCoreChanAddr_e baseAddr,
                                   uint8_t                   bfValue);

int32_t adrv9025_CoreArmOverrideControlBfSet(adi_adrv9025_Device_t*    device,
                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                             uint8_t                   bfValue);

int32_t adrv9025_CoreArmOverrideControlBfGet(adi_adrv9025_Device_t*    device,
                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                             uint8_t*                  bfValue);

int32_t adrv9025_CoreAuxAdcClkDivide1BfSet(adi_adrv9025_Device_t*    device,
                                           adrv9025_BfCoreChanAddr_e baseAddr,
                                           uint8_t                   bfValue);

int32_t adrv9025_CoreAuxAdcClkDivide1BfGet(adi_adrv9025_Device_t*    device,
                                           adrv9025_BfCoreChanAddr_e baseAddr,
                                           uint8_t*                  bfValue);

int32_t adrv9025_CoreAuxAdcClkDivide2BfSet(adi_adrv9025_Device_t*    device,
                                           adrv9025_BfCoreChanAddr_e baseAddr,
                                           uint8_t                   bfValue);

int32_t adrv9025_CoreAuxAdcClkDivide2BfGet(adi_adrv9025_Device_t*    device,
                                           adrv9025_BfCoreChanAddr_e baseAddr,
                                           uint8_t*                  bfValue);

int32_t adrv9025_CoreAuxAdcClkPd1BfSet(adi_adrv9025_Device_t*    device,
                                       adrv9025_BfCoreChanAddr_e baseAddr,
                                       uint8_t                   bfValue);

int32_t adrv9025_CoreAuxAdcClkPd2BfSet(adi_adrv9025_Device_t*    device,
                                       adrv9025_BfCoreChanAddr_e baseAddr,
                                       uint8_t                   bfValue);

int32_t adrv9025_CoreEnCtrl0BfSet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t                   bfValue);

int32_t adrv9025_CoreEnCtrl0BfGet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t*                  bfValue);

int32_t adrv9025_CoreEnCtrl1BfSet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t                   bfValue);

int32_t adrv9025_CoreEnCtrl1BfGet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t*                  bfValue);

int32_t adrv9025_CoreEnCtrl2BfSet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t                   bfValue);

int32_t adrv9025_CoreEnCtrl2BfGet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t*                  bfValue);

int32_t adrv9025_CoreEnCtrl3BfSet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t                   bfValue);

int32_t adrv9025_CoreEnCtrl3BfGet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t*                  bfValue);

int32_t adrv9025_CoreEnCtrl4BfSet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t                   bfValue);

int32_t adrv9025_CoreEnCtrl4BfGet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t*                  bfValue);

int32_t adrv9025_CoreEnCtrl5BfSet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t                   bfValue);

int32_t adrv9025_CoreEnCtrl5BfGet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t*                  bfValue);

int32_t adrv9025_CoreEnCtrl6BfSet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t                   bfValue);

int32_t adrv9025_CoreEnCtrl6BfGet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t*                  bfValue);

int32_t adrv9025_CoreEnCtrl7BfSet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t                   bfValue);

int32_t adrv9025_CoreEnCtrl7BfGet(adi_adrv9025_Device_t*    device,
                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                  uint8_t*                  bfValue);

int32_t adrv9025_CoreGpInterruptsMaskPin0BfSet(adi_adrv9025_Device_t*    device,
                                               adrv9025_BfCoreChanAddr_e baseAddr,
                                               uint64_t                  bfValue);

int32_t adrv9025_CoreGpInterruptsMaskPin0BfGet(adi_adrv9025_Device_t*    device,
                                               adrv9025_BfCoreChanAddr_e baseAddr,
                                               uint64_t*                 bfValue);

int32_t adrv9025_CoreGpInterruptsMaskPin1BfSet(adi_adrv9025_Device_t*    device,
                                               adrv9025_BfCoreChanAddr_e baseAddr,
                                               uint64_t                  bfValue);

int32_t adrv9025_CoreGpInterruptsMaskPin1BfGet(adi_adrv9025_Device_t*    device,
                                               adrv9025_BfCoreChanAddr_e baseAddr,
                                               uint64_t*                 bfValue);

int32_t adrv9025_CoreGpInterruptsStatusWordBfGet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint64_t*                 bfValue);

int32_t adrv9025_CoreGpInterruptsStickyBitMaskBfSet(adi_adrv9025_Device_t*    device,
                                                    adrv9025_BfCoreChanAddr_e baseAddr,
                                                    uint64_t                  bfValue);

int32_t adrv9025_CoreGpInterruptsStickyBitMaskBfGet(adi_adrv9025_Device_t*    device,
                                                    adrv9025_BfCoreChanAddr_e baseAddr,
                                                    uint64_t*                 bfValue);

int32_t adrv9025_CoreGpioAnalogDirectionControlOeBfSet(adi_adrv9025_Device_t*    device,
                                                       adrv9025_BfCoreChanAddr_e baseAddr,
                                                       uint8_t                   bfValue);

int32_t adrv9025_CoreGpioAnalogDirectionControlOeBfGet(adi_adrv9025_Device_t*    device,
                                                       adrv9025_BfCoreChanAddr_e baseAddr,
                                                       uint8_t*                  bfValue);

int32_t adrv9025_CoreGpioAnalogLowerNibbleSourceControlBfSet(adi_adrv9025_Device_t*    device,
                                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                                             uint8_t                   bfValue);

int32_t adrv9025_CoreGpioAnalogLowerNibbleSourceControlBfGet(adi_adrv9025_Device_t*    device,
                                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                                             uint8_t*                  bfValue);

int32_t adrv9025_CoreGpioAnalogUpperNibbleSourceControlBfSet(adi_adrv9025_Device_t*    device,
                                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                                             uint8_t                   bfValue);

int32_t adrv9025_CoreGpioAnalogUpperNibbleSourceControlBfGet(adi_adrv9025_Device_t*    device,
                                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                                             uint8_t*                  bfValue);

int32_t adrv9025_CoreGpioDirectionControlOeBfSet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint32_t                  bfValue);

int32_t adrv9025_CoreGpioDirectionControlOeBfGet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint32_t*                 bfValue);

int32_t adrv9025_CoreGpioExtraBitsSourceControlBfSet(adi_adrv9025_Device_t*    device,
                                                     adrv9025_BfCoreChanAddr_e baseAddr,
                                                     uint8_t                   bfValue);

int32_t adrv9025_CoreGpioExtraBitsSourceControlBfGet(adi_adrv9025_Device_t*    device,
                                                     adrv9025_BfCoreChanAddr_e baseAddr,
                                                     uint8_t*                  bfValue);

int32_t adrv9025_CoreGpioLowerByteLowerNibbleSourceControlBfSet(adi_adrv9025_Device_t*    device,
                                                                adrv9025_BfCoreChanAddr_e baseAddr,
                                                                uint8_t                   bfValue);

int32_t adrv9025_CoreGpioLowerByteLowerNibbleSourceControlBfGet(adi_adrv9025_Device_t*    device,
                                                                adrv9025_BfCoreChanAddr_e baseAddr,
                                                                uint8_t*                  bfValue);

int32_t adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfSet(adi_adrv9025_Device_t*    device,
                                                                adrv9025_BfCoreChanAddr_e baseAddr,
                                                                uint8_t                   bfValue);

int32_t adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfGet(adi_adrv9025_Device_t*    device,
                                                                adrv9025_BfCoreChanAddr_e baseAddr,
                                                                uint8_t*                  bfValue);

int32_t adrv9025_CoreGpioSpiReadBfGet(adi_adrv9025_Device_t*    device,
                                      adrv9025_BfCoreChanAddr_e baseAddr,
                                      uint32_t*                 bfValue);

int32_t adrv9025_CoreGpioSpiSourceBfSet(adi_adrv9025_Device_t*    device,
                                        adrv9025_BfCoreChanAddr_e baseAddr,
                                        uint32_t                  bfValue);

int32_t adrv9025_CoreGpioSpiSourceBfGet(adi_adrv9025_Device_t*    device,
                                        adrv9025_BfCoreChanAddr_e baseAddr,
                                        uint32_t*                 bfValue);

int32_t adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfSet(adi_adrv9025_Device_t*    device,
                                                                adrv9025_BfCoreChanAddr_e baseAddr,
                                                                uint8_t                   bfValue);

int32_t adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfGet(adi_adrv9025_Device_t*    device,
                                                                adrv9025_BfCoreChanAddr_e baseAddr,
                                                                uint8_t*                  bfValue);

int32_t adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfSet(adi_adrv9025_Device_t*    device,
                                                                adrv9025_BfCoreChanAddr_e baseAddr,
                                                                uint8_t                   bfValue);

int32_t adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfGet(adi_adrv9025_Device_t*    device,
                                                                adrv9025_BfCoreChanAddr_e baseAddr,
                                                                uint8_t*                  bfValue);

int32_t adrv9025_CoreMonitorOutBfSet(adi_adrv9025_Device_t*    device,
                                     adrv9025_BfCoreChanAddr_e baseAddr,
                                     uint8_t                   bfValue);

int32_t adrv9025_CoreMonitorOutBfGet(adi_adrv9025_Device_t*    device,
                                     adrv9025_BfCoreChanAddr_e baseAddr,
                                     uint8_t*                  bfValue);

int32_t adrv9025_CoreOrxEnableBfSet(adi_adrv9025_Device_t*    device,
                                    adrv9025_BfCoreChanAddr_e baseAddr,
                                    uint8_t                   bfValue);

int32_t adrv9025_CoreOrxEnableBfGet(adi_adrv9025_Device_t*    device,
                                    adrv9025_BfCoreChanAddr_e baseAddr,
                                    uint8_t*                  bfValue);

int32_t adrv9025_CoreOrxPinModeEnableBfSet(adi_adrv9025_Device_t*    device,
                                           adrv9025_BfCoreChanAddr_e baseAddr,
                                           uint8_t                   bfValue);

int32_t adrv9025_CoreOrxPinModeEnableBfGet(adi_adrv9025_Device_t*    device,
                                           adrv9025_BfCoreChanAddr_e baseAddr,
                                           uint8_t*                  bfValue);

int32_t adrv9025_CoreOrxPinModeUsingSpiSelectBfSet(adi_adrv9025_Device_t*    device,
                                                   adrv9025_BfCoreChanAddr_e baseAddr,
                                                   uint8_t                   bfValue);

int32_t adrv9025_CoreOrxPinModeUsingSpiSelectBfGet(adi_adrv9025_Device_t*    device,
                                                   adrv9025_BfCoreChanAddr_e baseAddr,
                                                   uint8_t*                  bfValue);

int32_t adrv9025_CoreOrxSelectInPinModeBfSet(adi_adrv9025_Device_t*    device,
                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                             uint8_t                   bfValue);

int32_t adrv9025_CoreOrxSelectInPinModeBfGet(adi_adrv9025_Device_t*    device,
                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                             uint8_t*                  bfValue);

int32_t adrv9025_CoreOrxSingleChannelModeBfSet(adi_adrv9025_Device_t*    device,
                                               adrv9025_BfCoreChanAddr_e baseAddr,
                                               uint8_t                   bfValue);

int32_t adrv9025_CoreOrxSingleChannelModeBfGet(adi_adrv9025_Device_t*    device,
                                               adrv9025_BfCoreChanAddr_e baseAddr,
                                               uint8_t*                  bfValue);

int32_t adrv9025_CoreOrxSingleChannelMode1aBfSet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t                   bfValue);

int32_t adrv9025_CoreOrxSingleChannelMode1aBfGet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t*                  bfValue);

int32_t adrv9025_CoreOrxSingleChannelMode1aHighChannelEnableBfSet(adi_adrv9025_Device_t*    device,
                                                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                                                  uint8_t                   bfValue);

int32_t adrv9025_CoreOrxSingleChannelMode1aHighChannelEnableBfGet(adi_adrv9025_Device_t*    device,
                                                                  adrv9025_BfCoreChanAddr_e baseAddr,
                                                                  uint8_t*                  bfValue);

int32_t adrv9025_CoreOrxSingleChannelMode1aLowChannelEnableBfSet(adi_adrv9025_Device_t*    device,
                                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                                 uint8_t                   bfValue);

int32_t adrv9025_CoreOrxSingleChannelMode1aLowChannelEnableBfGet(adi_adrv9025_Device_t*    device,
                                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                                 uint8_t*                  bfValue);

int32_t adrv9025_CorePdAuxdacBfSet(adi_adrv9025_Device_t*    device,
                                   adrv9025_BfCoreChanAddr_e baseAddr,
                                   uint8_t                   bfValue);

int32_t adrv9025_CorePdAuxdacBfGet(adi_adrv9025_Device_t*    device,
                                   adrv9025_BfCoreChanAddr_e baseAddr,
                                   uint8_t*                  bfValue);

int32_t adrv9025_CorePinSelectSettlingDelayBfSet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t                   bfValue);

int32_t adrv9025_CorePinSelectSettlingDelayBfGet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t*                  bfValue);

int32_t adrv9025_CoreRxOrxAnySourceMuxSel10BfSet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t                   bfValue);

int32_t adrv9025_CoreRxOrxAnySourceMuxSel10BfGet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t*                  bfValue);

int32_t adrv9025_CoreRxOrxAnySourceMuxSel32BfSet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t                   bfValue);

int32_t adrv9025_CoreRxOrxAnySourceMuxSel32BfGet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t*                  bfValue);

int32_t adrv9025_CoreRxOrxAnySourceMuxSel54BfSet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t                   bfValue);

int32_t adrv9025_CoreRxOrxAnySourceMuxSel54BfGet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t*                  bfValue);

int32_t adrv9025_CoreRxOrxAnySourceMuxSel76BfSet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t                   bfValue);

int32_t adrv9025_CoreRxOrxAnySourceMuxSel76BfGet(adi_adrv9025_Device_t*    device,
                                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                                 uint8_t*                  bfValue);

int32_t adrv9025_CoreRxSync1PadLvdsModeBfGet(adi_adrv9025_Device_t*    device,
                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                             uint8_t*                  bfValue);

int32_t adrv9025_CoreRxSync1PadLvdsPnInvBfGet(adi_adrv9025_Device_t*    device,
                                              adrv9025_BfCoreChanAddr_e baseAddr,
                                              uint8_t*                  bfValue);

int32_t adrv9025_CoreRxSync2PadLvdsModeBfGet(adi_adrv9025_Device_t*    device,
                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                             uint8_t*                  bfValue);

int32_t adrv9025_CoreRxSync2PadLvdsPnInvBfGet(adi_adrv9025_Device_t*    device,
                                              adrv9025_BfCoreChanAddr_e baseAddr,
                                              uint8_t*                  bfValue);

int32_t adrv9025_CoreRxSync3PadLvdsModeBfGet(adi_adrv9025_Device_t*    device,
                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                             uint8_t*                  bfValue);

int32_t adrv9025_CoreRxSync3PadLvdsPnInvBfGet(adi_adrv9025_Device_t*    device,
                                              adrv9025_BfCoreChanAddr_e baseAddr,
                                              uint8_t*                  bfValue);

int32_t adrv9025_CoreSpi2EnBfSet(adi_adrv9025_Device_t*    device,
                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                 uint8_t                   bfValue);

int32_t adrv9025_CoreSpi2EnBfGet(adi_adrv9025_Device_t*    device,
                                 adrv9025_BfCoreChanAddr_e baseAddr,
                                 uint8_t*                  bfValue);

int32_t adrv9025_CoreStreamProcGpioPinMaskBfSet(adi_adrv9025_Device_t*    device,
                                                adrv9025_BfCoreChanAddr_e baseAddr,
                                                uint16_t                  bfValue);

int32_t adrv9025_CoreStreamProcGpioPinMaskBfGet(adi_adrv9025_Device_t*    device,
                                                adrv9025_BfCoreChanAddr_e baseAddr,
                                                uint16_t*                 bfValue);

int32_t adrv9025_CoreTxAttenUpdCoreSpiBfSet(adi_adrv9025_Device_t*    device,
                                            adrv9025_BfCoreChanAddr_e baseAddr,
                                            uint8_t                   bfValue);

int32_t adrv9025_CoreTxAttenUpdCoreSpiEnBfSet(adi_adrv9025_Device_t*    device,
                                              adrv9025_BfCoreChanAddr_e baseAddr,
                                              uint8_t                   bfValue);

int32_t adrv9025_CoreTxAttenUpdCoreSpiEnBfGet(adi_adrv9025_Device_t*    device,
                                              adrv9025_BfCoreChanAddr_e baseAddr,
                                              uint8_t*                  bfValue);

int32_t adrv9025_CoreTxSync1PadLvdsModeBfGet(adi_adrv9025_Device_t*    device,
                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                             uint8_t*                  bfValue);

int32_t adrv9025_CoreTxSync1PadLvdsPnInvBfGet(adi_adrv9025_Device_t*    device,
                                              adrv9025_BfCoreChanAddr_e baseAddr,
                                              uint8_t*                  bfValue);

int32_t adrv9025_CoreTxSync2PadLvdsModeBfGet(adi_adrv9025_Device_t*    device,
                                             adrv9025_BfCoreChanAddr_e baseAddr,
                                             uint8_t*                  bfValue);

int32_t adrv9025_CoreTxSync2PadLvdsPnInvBfGet(adi_adrv9025_Device_t*    device,
                                              adrv9025_BfCoreChanAddr_e baseAddr,
                                              uint8_t*                  bfValue);

int32_t adrv9025_CoreTxSyncDrvSelectBfGet(adi_adrv9025_Device_t*    device,
                                          adrv9025_BfCoreChanAddr_e baseAddr,
                                          uint8_t*                  bfValue);

#ifdef __cplusplus
}
#endif
#endif
