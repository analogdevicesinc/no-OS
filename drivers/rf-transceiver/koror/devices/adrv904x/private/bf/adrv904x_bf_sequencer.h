/** 
 * \file adrv904x_bf_sequencer.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_SEQUENCER_H_
#define _ADRV904X_BF_SEQUENCER_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_sequencer_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ApplyNow_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ApplyOnMultiframeBoundary_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_DbgState_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ExtendedSymbolSpacing_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ExtendedSymbolWidth_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                              const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ExternalJumpEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ExternalJumpPointer_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ExternalJumpPointer_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_GeneralPurposeReg_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_GlobalReset_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_MultiframePhaseAdjust_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                                const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_PointerInit_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ShortSymbolWidth_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                           const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncGpioSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncPeriod_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                        const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncPhaseError_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncPhaseErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncTrackingMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncTrackingMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_XbarCtrlSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_XbarSequencerSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           const uint8_t bfValue);

#endif // _ADRV904X_BF_SEQUENCER_H_

