/** 
 * \file adrv904x_bf_tx_cfr_reg.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_TX_CFR_REG_H_
#define _ADRV904X_BF_TX_CFR_REG_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_tx_cfr_reg_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlByp_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlByp_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0PulHpl_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0PulHpl_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0PulInterp_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0ThrCor_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0ThrSq_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0ThrSqrt_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlByp_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlByp_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1PulHpl_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1PulHpl_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1PulInterp_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1ThrCor_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1ThrSq_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1ThrSqrt_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrCtrlActiveSet_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrCtrlChSet_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrCtrlPulSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrCtrlPulSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkClrStat_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkFoundCurWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkFoundPrvWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostCpcCurWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostCpcPrvWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostPfCurWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostPfPrvWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostWfCurWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostWfPrvWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkReadStat_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrRamShadowRamSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

#endif // _ADRV904X_BF_TX_CFR_REG_H_

