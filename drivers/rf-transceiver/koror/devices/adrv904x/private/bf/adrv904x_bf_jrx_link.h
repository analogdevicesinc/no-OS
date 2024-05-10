/** 
 * \file adrv904x_bf_jrx_link.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_JRX_LINK_H_
#define _ADRV904X_BF_JRX_LINK_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_jrx_link_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreBufDepth_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreChksumCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreClearSyncNeCount_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreCsCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreDidCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreDscrCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreECfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreFCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreHdCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreJesdvCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreKCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreLaneSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreLidCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreLCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreMCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreNpCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreNCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCorePclkErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCorePclkErrorClear_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCorePhaseAdjust_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                           const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCorePhaseAdjust_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                           uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCorePhaseDiff_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                         uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSubclassvCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSyncNeCount_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSyncNSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSysrefClrPhaseErr_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSysrefClrPhaseErr_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSysrefEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSysrefEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSysrefForRelink_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSysrefForStartup_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSysrefIgnoreWhenLinked_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSysrefNShotCount_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSysrefNShotEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSysrefRcvd_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreSCfg_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxCoreUsrDataRdy_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxLinkType_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_JrxLink_JrxTplBufDepth_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJrxLinkChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint8_t* const bfValue);

#endif // _ADRV904X_BF_JRX_LINK_H_

