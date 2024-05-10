/** 
 * \file adrv904x_bf_cddc_hb_dpath.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_CDDC_HB_DPATH_H_
#define _ADRV904X_BF_CDDC_HB_DPATH_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_cddc_hb_dpath_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_BandSource_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       const adrv904x_Bf_CddcHbDpath_BandSource_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain0_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain0_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain1_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain1_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain2_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain2_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain3_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain3_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain4_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain4_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain5_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain5_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain6_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain6_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain7_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain7_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGainEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect0_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect1_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect2_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect3_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect4_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect5_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect6_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect7_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_DaisyChainEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_DecRate_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const adrv904x_Bf_CddcHbDpath_DecRate_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_DelayCmpEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_DelayMemEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_FreqKhz_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_InterleaverMaxSlot_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_InterleaverSlotValidLower_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                                      const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_InterleaverSlotValidUpper_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                                      const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_MixerEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_NcoEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_NcoFtwUpdate_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_OneMsClocks_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_UpsamplerRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           const adrv904x_Bf_CddcHbDpath_UpsamplerRatio_e bfValue);

#endif // _ADRV904X_BF_CDDC_HB_DPATH_H_

