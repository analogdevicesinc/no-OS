/** 
 * \file adrv904x_bf_orx_dig.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_ORX_DIG_H_
#define _ADRV904X_BF_ORX_DIG_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_orx_dig_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_CptBusy_BfGet(adi_adrv904x_Device_t* const device,
                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_CptTrigger_BfSet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_IntDataFormat_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_IntDataFormat_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_IntDataResolution_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_IntDataResolution_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_ObsCapMemClkDivideRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_ObsCapMemClkDivideRatio_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_Orx6dbGainEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection10_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection11_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection12_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection13_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection14_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection15_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection16_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection17_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection18_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection19_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection2_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection20_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection21_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection22_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection23_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection3_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection4_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection5_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection6_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection7_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection8_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OrxGpioSourceSelection9_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OverloadCountTh_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OverloadDurationCount_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OverloadEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OverloadPowerMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OverloadThHigh_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                      const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDig_OverloadThPre_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfOrxDigChanAddr_e baseAddr,
                                                                     const uint16_t bfValue);

#endif // _ADRV904X_BF_ORX_DIG_H_

