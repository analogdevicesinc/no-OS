/** 
 * \file adrv904x_bf_cduc_hb_dpath.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_CDUC_HB_DPATH_H_
#define _ADRV904X_BF_CDUC_HB_DPATH_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_cduc_hb_dpath_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_BandDest_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     const adrv904x_Bf_CducHbDpath_BandDest_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain0_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain0_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain1_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain1_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain2_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain2_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain3_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain3_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain4_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain4_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain5_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain5_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain6_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain6_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain7_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain7_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGainEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect0_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect1_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect2_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect3_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect4_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect5_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect6_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect7_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrBand0Attn_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrBand0Attn_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrBand1Attn_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrBand1Attn_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CducCarrierEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CducCarrierEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CducClkToCarrierRate_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const adrv904x_Bf_CducHbDpath_CducClkToCarrierRate_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DaisyChainEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DeinterleaverMaxSlot_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DeinterleaverSlotValidLower_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                                        const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DeinterleaverSlotValidUpper_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                                        const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DelayCompEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DelayMemEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DownsamplerRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const adrv904x_Bf_CducHbDpath_DownsamplerRatio_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_FreqKhz_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_IntRate_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const adrv904x_Bf_CducHbDpath_IntRate_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_MixerEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_NcoEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_NcoFtwUpdate_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_OneMsClocks_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        const uint32_t bfValue);

#endif // _ADRV904X_BF_CDUC_HB_DPATH_H_

