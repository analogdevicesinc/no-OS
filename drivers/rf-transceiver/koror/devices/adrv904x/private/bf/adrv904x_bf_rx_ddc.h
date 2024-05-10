/** 
 * \file adrv904x_bf_rx_ddc.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_RX_DDC_H_
#define _ADRV904X_BF_RX_DDC_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_rx_ddc_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerEnSpiOrAgcSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerEnSpiOrAgcSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerMeasurementDuration_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerMeasurementDuration_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerStartDelayCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerTddModeEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerValue_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerValueReadback_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DigitalGainEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_ExternalSlicerPinControlStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpEn_BfSet(adi_adrv904x_Device_t* const device,
                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpEn_BfGet(adi_adrv904x_Device_t* const device,
                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpExponentBits_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     const adrv904x_Bf_RxDdc_FpExponentBits_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpExponentBits_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     adrv904x_Bf_RxDdc_FpExponentBits_e* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpFloatDataFormat_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpFloatDataFormat_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpHideLeadingOnes_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpHideLeadingOnes_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpIntDataAtten_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpIntDataAtten_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpNanEncEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpNanEncEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpRoundMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                  const adrv904x_Bf_RxDdc_FpRoundMode_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpRoundMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                  adrv904x_Bf_RxDdc_FpRoundMode_e* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompForExtGain_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompForExtGain_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompForTempGain_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompForTempGain_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntDataFormat_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntDataFormat_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntDataResolution_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntDataResolution_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicer_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicer_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicerNumber_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicerNumber_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicerPos_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicerPos_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEvenParity_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEvenParity_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntParitySupport_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntParitySupport_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntSlicerLsbOnQ_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntSlicerLsbOnQ_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_MaxSlicer_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_MaxSlicerOverride_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxMonFormatI_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxMonFormatI_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxMonFormatQ_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxMonFormatQ_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxTempGainComp_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxTempGainComp_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_SlicerPinControlMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_SlicerPinControlMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_SlicerPinControlStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_SlicerPinControlStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_Static3bitSlicerModeEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

#endif // _ADRV904X_BF_RX_DDC_H_

