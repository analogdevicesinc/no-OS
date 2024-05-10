/** 
 * \file adrv904x_bf_jtx_link.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_JTX_LINK_H_
#define _ADRV904X_BF_JTX_LINK_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_jtx_link_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxBidCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxDidCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxDl204bClearSyncNeCount_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxDl204bState_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxDl204bSyncN_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxDl204bSyncNeCount_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxDl204bSyncNForceEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxDl204bSyncNForceEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxDl204bSyncNForceVal_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxDl204cSysrefRcvd_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxECfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxForceLanePd_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxFCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxKCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxLaneInv_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxLaneInv_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxLaneSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxLidCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxLinkType_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxMCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxNpCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxPclkErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxPclkErrorClear_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxPclkFastError_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxPclkSlowError_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxScrCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxSyncNSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxSysrefForRelink_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxSysrefForStartup_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxSCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTestGenMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTestGenMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTestGenSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTestGenSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplCfgInvalid_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplPhaseAdjust_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                          const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplPhaseAdjust_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                          uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplSysrefClrPhaseErr_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplSysrefClrPhaseErr_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplSysrefIgnoreWhenLinked_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplSysrefMask_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplSysrefMask_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplSysrefNShotCount_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplSysrefNShotEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplSysrefPhaseErr_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JtxLink_JtxTplSysrefRcvd_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJtxLinkChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

#endif // _ADRV904X_BF_JTX_LINK_H_

