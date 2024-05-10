/** 
 * \file adrv904x_bf_rx_dig.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_RX_DIG_H_
#define _ADRV904X_BF_RX_DIG_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_rx_dig_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AdcTestClkSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AdcTestClkSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AdcTestGenEnSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AdcTestGenEnSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AdcTestGenEnSpi_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AdcTestGenEnSpi_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AdcTestGenSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AdcTestGenSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AdcTestGenSineFreq_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AdcTestGenSineFreq_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AgcDecGainGpioSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AgcGainChangeGpioSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AgcIncGainGpioSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AgcManualGainLockGpioSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_AgcSlowloopFreezeEnableGpioSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_Ddc1Hb1OutClkDivideRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_DecpwrClkDivideRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_DecpwrClkEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_Hb2OutClkDivideRatio_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_PeakDetectClkDivideRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_PeakDetectClkEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_ReferenceClockCycles_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_RoutClkDivideRatio_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_RssiEnableGpioSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_RxGpioSourceSelection_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_StreamprocDdc1Hb1OutClkEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDig_TestGenEnGpioSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxDigChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

#endif // _ADRV904X_BF_RX_DIG_H_

