/** 
 * \file adrv904x_bf_tx_funcs.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_TX_FUNCS_H_
#define _ADRV904X_BF_TX_FUNCS_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_tx_funcs_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_AnaRampHoldSampleEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_AnaRampHoldSampleEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_JesdDfrmMask_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_JesdDfrmMask_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAprEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAprEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAverageDur_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAverageErrorPower_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                      uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAveragePeakRatio_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                     uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAveragePower_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                 uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAverageThreshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                     const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAverageThreshold_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                     uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerError_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfSet(adi_adrv904x_Device_t* const device,
                                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                                               const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfGet(adi_adrv904x_Device_t* const device,
                                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                                               const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerIrqEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerIrqEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerRdnEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerRdnEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionGainRampUpEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionInputSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionInputSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionOverloadCountTh_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionOverloadThPre_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                  const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionOverloadWindowSize_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerError_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfSet(adi_adrv904x_Device_t* const device,
                                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfGet(adi_adrv904x_Device_t* const device,
                                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerIrqEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerIrqEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerRdnEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerRdnEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakCount_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakCount_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakDur_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakDur_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakErrorPower_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                   uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakPower_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                              uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakThreshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                  const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakThreshold_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                  uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionRampMaxAttenuation_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionRampStepDuration_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionRampStepSize_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionReadbackUpdate_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllJesdProtClr_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllJesdProtClrReqd_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllJesdProtClrReqd_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllJesdProtEvent_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllUnlockMask_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllUnlockMask_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvTxonQual_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvTxonQual_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvWait_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvWait_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdDinSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdDinSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdDisAnalogDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdEn_BfSet(adi_adrv904x_Device_t* const device,
                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                              const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdEn_BfGet(adi_adrv904x_Device_t* const device,
                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                              const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdError_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdIrqEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdIrqEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdRdnEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdRdnEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdSlewOffset_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                      const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdSlewOffset_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                      uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdStat_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdStatEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdStatEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdStatMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdStatMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenuation_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                      const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenuation_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                      uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenConfig_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenConfig_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenPapDisAnalogDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenUpdGpioEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenUpdGpioEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenUpdTrgSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPeakToPowerMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPeakToPowerMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPower_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerInputSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerInputSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerLargestPeak_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerMeasurementDuration_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerMeasurementDuration_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerMeasurementEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerMeasurementEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerMeasurementReadback_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_UseSliceAttenValue_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

#endif // _ADRV904X_BF_TX_FUNCS_H_

