/** 
 * \file adrv903x_bf_tx_dig.h Automatically generated file with generator ver 0.0.13.0.
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

#ifndef _ADRV903X_BF_TX_DIG_H_
#define _ADRV903X_BF_TX_DIG_H_

#include "adi_adrv903x_core.h"
#include "adi_adrv903x_hal.h"
#include "../../private/bf/adrv903x_bf_tx_dig_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_DtxForceGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                         const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_DtxStatus_BfGet(adi_adrv903x_Device_t* const device,
                                                                adi_adrv903x_SpiCache_t* const spiCache,
                                                                const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_DtxZeroCounter_BfGet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                     uint16_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_RadClkDivideRatio_BfSet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_RadClkDivideRatio_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_Spi2TxAttenGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_Spi2TxAttenGpioSelect_BfGet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_StreamprocDuc0Hb3OutClkEnable_BfSet(adi_adrv903x_Device_t* const device,
                                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                                    const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                                    const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_StreamprocPfirClkEnable_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TpcDecrAttenGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TpcIncrAttenGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxAttenClkDivideRatio_BfGet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxAttenClkEnable_BfSet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxAttenUpdGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxAttenUpdGpioSelect_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxCptBusy_BfGet(adi_adrv903x_Device_t* const device,
                                                                adi_adrv903x_SpiCache_t* const spiCache,
                                                                const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxCptTrigger_BfSet(adi_adrv903x_Device_t* const device,
                                                                   adi_adrv903x_SpiCache_t* const spiCache,
                                                                   const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                   const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxDuc0TssiPinEnableSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                                adi_adrv903x_SpiCache_t* const spiCache,
                                                                                const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection0_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection1_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection10_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection11_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection12_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection13_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection14_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection15_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection16_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection17_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection18_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection19_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection2_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection20_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection21_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection22_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection23_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection3_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection4_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection5_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection6_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection7_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection8_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDig_TxGpioSourceSelection9_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfTxDigChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

#endif // _ADRV903X_BF_TX_DIG_H_

