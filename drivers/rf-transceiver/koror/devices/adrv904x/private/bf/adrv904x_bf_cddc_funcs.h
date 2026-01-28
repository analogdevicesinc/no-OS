/** 
 * \file adrv904x_bf_cddc_funcs.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_CDDC_FUNCS_H_
#define _ADRV904X_BF_CDDC_FUNCS_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_cddc_funcs_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcJesdClkDiv_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         const adrv904x_Bf_CddcFuncs_CddcJesdClkDiv_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPeakToPowerMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPeakToPowerMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPower_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerInputSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerInputSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerLargestPeak_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                                   uint8_t channelId, 
                                                                                                   const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfGet(adi_adrv904x_Device_t* const device,
                                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                                   uint8_t channelId, 
                                                                                                   uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfGet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementPause_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementReadback_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementTddModeEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementTddModeEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterCarrierSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterCarrierSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterDynamicSlicerAgcSyncEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterDynamicSlicerAgcSyncEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpExponentBits_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpExponentBits_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpFloatDataFormat_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpFloatDataFormat_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpHideLeadingOnes_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpHideLeadingOnes_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpNanEncEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpNanEncEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpRoundMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpRoundMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntDataFormat_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntDataFormat_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntDataResolution_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntDataResolution_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicer_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicer_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicerNumber_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicerNumber_BfGet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicerPos_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicerPos_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEvenParity_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEvenParity_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntParitySupport_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntParitySupport_BfGet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntSlicerLsbOnQ_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntSlicerLsbOnQ_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatI0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatI0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatI1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatI1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatQ0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatQ0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatQ1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatQ1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterStatic3bitSlicerModeEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                                          const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterStatic3bitSlicerModeEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                                          const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterStaticSlicerAgcSyncEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                                             const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiDefaultMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiDefaultMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiLatchLinearPowerReadback_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiLinearPowerReadback_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint64_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiManualMeas_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiManualPause_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration0_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration0_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration1_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration1_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration2_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration2_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration3_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration3_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor0_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor0_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor1_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor1_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor2_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor2_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor3_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor3_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiPinMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiPinMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiResetOnGainChange_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiResetOnGainChange_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiRsTriggerSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiRsTriggerSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiStartDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiStartDelay_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiWaitDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiWaitDelay_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint32_t* const bfValue);

#endif // _ADRV904X_BF_CDDC_FUNCS_H_

