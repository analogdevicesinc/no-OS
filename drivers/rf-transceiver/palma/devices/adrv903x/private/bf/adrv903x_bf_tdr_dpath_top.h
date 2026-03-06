/** 
 * \file adrv903x_bf_tdr_dpath_top.h Automatically generated file with generator ver 0.0.13.0.
 * 
 * \brief Contains BitField functions to support ADRV903X transceiver device.
 * 
 * ADRV903X BITFIELD VERSION: 0.0.0.8
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2025 Analog Devices Inc.
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADRV903X_BF_TDR_DPATH_TOP_H_
#define _ADRV903X_BF_TDR_DPATH_TOP_H_

#include "adi_adrv903x_core.h"
#include "adi_adrv903x_hal.h"
#include "../../private/bf/adrv903x_bf_tdr_dpath_top_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv903x_ErrAction_e adrv903x_TdrDpathTop_FlashThresh_BfSet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfTdrDpathTopChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TdrDpathTop_FlashThresh_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfTdrDpathTopChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);
#endif // _ADRV903X_BF_TDR_DPATH_TOP_H_

