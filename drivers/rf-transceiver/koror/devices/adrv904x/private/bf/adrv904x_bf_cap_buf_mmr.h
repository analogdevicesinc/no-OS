/** 
 * \file adrv904x_bf_cap_buf_mmr.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_CAP_BUF_MMR_H_
#define _ADRV904X_BF_CAP_BUF_MMR_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_cap_buf_mmr_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_CaptureDepth_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                       uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_CapEnSrc_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                   const adrv904x_Bf_CapBufMmr_CapEnSrc_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_CapEnSrc_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                   adrv904x_Bf_CapBufMmr_CapEnSrc_e* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_CapPauseSrc_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                      const adrv904x_Bf_CapBufMmr_CapPauseSrc_e bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_CapPauseSrc_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                      adrv904x_Bf_CapBufMmr_CapPauseSrc_e* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_Ch0SrcSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                    adrv904x_Bf_CapBufMmr_Ch0SrcSel_e* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_Ch1SrcSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                    adrv904x_Bf_CapBufMmr_Ch1SrcSel_e* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_Ch2SrcSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                    adrv904x_Bf_CapBufMmr_Ch2SrcSel_e* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_ClockEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_DebugModeEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_CapBufMmr_NumChannels_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCapBufMmrChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

#endif // _ADRV904X_BF_CAP_BUF_MMR_H_

