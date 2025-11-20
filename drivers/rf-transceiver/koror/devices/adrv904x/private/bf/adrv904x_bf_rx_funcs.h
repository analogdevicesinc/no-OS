/** 
 * \file adrv904x_bf_rx_funcs.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_RX_FUNCS_H_
#define _ADRV904X_BF_RX_FUNCS_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_rx_funcs_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcovrgHigh_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcovrgLow_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcovrgLowInt0Counter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcovrgLowInt0Counter_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcovrgLowInt1Counter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcovrgLowInt1Counter_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcHighOvrgExceededCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcHighOvrgExceededCounter_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcLowOvrgExceededCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcLowOvrgExceededCounter_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcResetGainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAdcResetGainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAttackDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcAttackDelay_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcChangeGainIfAdcovrgHigh_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcChangeGainIfAdcovrgHigh_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcChangeGainIfUlbthHigh_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcChangeGainIfUlbthHigh_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDelayCounterBaseRate_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandExtTableLowerIndex_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandExtTableLowerIndex_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandExtTableUpperIndex_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandExtTableUpperIndex_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandHighLnaThreshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandHighLnaThreshold_BfGet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandLnaStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandLnaStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandLowLnaThreshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandLowLnaThreshold_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandMaxIndex_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandMaxIndex_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandPwrMargin_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcDualbandPwrMargin_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcEnableFastRecoveryLoop_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcEnableFastRecoveryLoop_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcEnableGainIndexUpdate_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcEnableSyncPulseForGainCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcEnableSyncPulseForGainCounter_BfGet(adi_adrv904x_Device_t* const device,
                                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcGainIndex_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcGainIndex_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcGainUpdateCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcGainUpdateCounter_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLlbGainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLlbGainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLlbThresholdExceededCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLlbThresholdExceededCounter_BfGet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLlBlocker_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLockLevel_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLockLevel_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLower0Threshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLower0Threshold_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLower0ThresholdExceededGainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLower0ThresholdExceededGainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLower1Threshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLower1Threshold_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLower1ThresholdExceededGainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLower1ThresholdExceededGainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLowThsPreventGainInc_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcLowThsPreventGainInc_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcManualGainIndex_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcManualGainPinControl_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcMaximumGainIndex_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcMaximumGainIndex_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcMinimumGainIndex_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcMinimumGainIndex_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcOvrgHighGainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcOvrgHighGainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcOvrgLowGainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcOvrgLowGainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcOvrgLowInt0GainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcOvrgLowInt0GainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcOvrgLowInt1GainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcOvrgLowInt1GainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcOvrgResetpdHighCount_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcPeakThresholdGainControlMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcPeakThresholdGainControlMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcPeakWaitTime_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcPeakWaitTime_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcResetCounters_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcResetOnRxon_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcResetOnRxon_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcSetup_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcSetup_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcSlowloopFreezeEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcSlowloopFreezeEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcSlowLoopSettlingDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcSlowLoopSettlingDelay_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcSoftReset_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUlbGainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUlbGainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUlbThresholdExceededCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUlbThresholdExceededCounter_BfGet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUlBlocker_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUpper0ThresholdExceededGainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUpper0ThresholdExceededGainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUpper1Threshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUpper1Threshold_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUpper1ThresholdExceededGainStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUpper1ThresholdExceededGainStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUrangeInterval0_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUrangeInterval0_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUrangeInterval1Mult_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUrangeInterval1Mult_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUrangeInterval2Mult_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUrangeInterval2Mult_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_AgcUseCountersForMgc_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_ApdHighSrcSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_ApdHighSrcSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_ApdLowSrcSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_ApdLowSrcSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_BbdcTrackingEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_BbdcTrackingEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerDataSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerDataSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerEnSpiOrAgcSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerEnSpiOrAgcSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerMeasurementDuration_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerMeasurementDuration_BfGet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerReadback_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerStartDelayCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                  const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerTddModeEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DecPowerValue_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DualbandControlBandA_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_DualbandControlBandB_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_Hb2HighSrcSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_Hb2HighSrcSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_Hb2LowSrcSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_Hb2LowSrcSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_Hb2OverloadUseHb2In_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_Hb2OverloadUseHb2In_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_InvertApdLow_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_InvertApdLow_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_InvertHb2Low_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_InvertHb2Low_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadEnAgc_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadEnAgc_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadPowerModeAgc_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadPowerModeAgc_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadThAgcHigh_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                          const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadThAgcHigh_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                          uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadThAgcInt0Low_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadThAgcInt0Low_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadThAgcInt1Low_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadThAgcInt1Low_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                             uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadThAgcLow_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                         const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadThAgcLow_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                         uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_OverloadThAgcPre_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                         const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_PeakCountExpirationAgc_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                               const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_PeakCountSpacingAgcHigh_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_PeakCountSpacingAgcLow_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_PeakCountThresholdAgcHigh_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_PeakCountThresholdAgcLow_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_PeakSampleCountAgc_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_PeakSampleCountAgc_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_PeakWindowSizeAgc_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_PeakWindowSizeAgc_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_ReadGainTable_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_RxdpSlicerPosition_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncs_TiaValidOverride_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxFuncsChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

#endif // _ADRV904X_BF_RX_FUNCS_H_

