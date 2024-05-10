/** 
 * \file adrv904x_bf_core.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_CORE_H_
#define _ADRV904X_BF_CORE_H_

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_hal.h"
#include "../../private/bf/adrv904x_bf_core_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_A55CclkDivideRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_A55CclkDivideRatioDevClk_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_A55Spi0CmdStatusDwl_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint64_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_A55Spi0Command_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_A55Spi0CommandBusy_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_A55SysclkEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_AhbSpiBridgeEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Arm0M3Run_BfSet(adi_adrv904x_Device_t* const device,
                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Arm0M3Run_BfGet(adi_adrv904x_Device_t* const device,
                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Arm0MemHrespMask_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Arm0Spi0Command_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Arm0Spi0CommandBusy_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Arm0Spi0ExtCmdByte1_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Arm1Spi0CommandBusy_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_ArmClkDivideRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_ArmClkDivideRatioDevClk_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_ArmClkEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel0_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel1_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel10_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel11_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel12_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel13_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel14_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel15_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel16_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel17_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel18_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel19_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel2_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel20_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel21_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel22_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel23_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel3_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel4_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel5_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel6_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel7_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel8_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DataFromControlOutSel9_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DevclkBufferEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DevclkBuffTermEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DevclkDividerMcsResetb_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DevclkDivideRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DigitalClockDividerSyncEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_DigitalClockPowerUp_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_EfuseProductId_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_EfuseProductIdReady_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_EfuseReadData_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                   uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_EfuseReadDataValid_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_EfuseReadState_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogFromMaster_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogFromMasterClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogFromMasterSet_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                             const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogFromMasterToggle_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl0_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl0_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl1_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl1_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl10_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl10_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl11_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl11_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl12_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl12_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl13_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl13_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl14_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl14_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl15_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl15_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl2_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl2_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl3_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl3_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl4_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl4_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl5_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl5_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl6_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl6_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl7_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl7_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl8_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl8_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl9_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControl9_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControlOverride_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                     const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSourceControlOverride_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                     uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioAnalogSpiRead_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                       uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioFromMaster_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                    uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioFromMasterClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioFromMasterSet_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                       const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioFromMasterToggle_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl0_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl0_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl1_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl1_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl10_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl10_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl11_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl11_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl12_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl12_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl13_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl13_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl14_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl14_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl15_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl15_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl16_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl16_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl17_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl17_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl18_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl18_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl19_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl19_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl2_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl2_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl20_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl20_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl21_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl21_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl22_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl22_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl23_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl23_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl3_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl3_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl4_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl4_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl5_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl5_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl6_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl6_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl7_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl7_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl8_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl8_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl9_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControl9_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControlOverride_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceControlOverride_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                               uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl0_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl1_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl10_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl11_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl12_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl13_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl14_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl15_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl16_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl17_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl18_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl19_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl2_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl20_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl21_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl22_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl23_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl3_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl4_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl5_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl6_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl7_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl8_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceOrxControl9_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl0_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl1_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl10_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl11_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl12_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl13_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl14_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl15_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl16_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl17_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl18_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl19_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl2_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl20_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl21_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl22_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl23_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl3_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl4_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl5_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl6_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl7_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl8_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceRxControl9_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl0_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl1_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl10_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl11_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl12_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl13_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl14_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl15_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl16_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl17_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl18_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl19_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl2_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl20_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl21_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl22_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl23_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl3_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl4_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl5_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl6_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl7_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl8_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSourceTxControl9_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpioSpiRead_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                 uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsLevelPulseBLowerWord_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint64_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsLevelPulseBLowerWord_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint64_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsLevelPulseBUpperWord_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint64_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsLevelPulseBUpperWord_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint64_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsMaskLowerWordPin0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint64_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsMaskLowerWordPin0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint64_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsMaskLowerWordPin1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint64_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsMaskLowerWordPin1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint64_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsMaskUpperWordPin0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint64_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsMaskUpperWordPin0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint64_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsMaskUpperWordPin1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint64_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsMaskUpperWordPin1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint64_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsStatusLowerWord_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                 const uint64_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsStatusLowerWord_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                 uint64_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsStatusUpperWord_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                 const uint64_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_GpInterruptsStatusUpperWord_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                 uint64_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_MaskRevisionMajor_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_MaskRevisionMinor_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_MasterBiasClkDivRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_MbiasBgCtat_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                 uint8_t channelId, 
                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_MbiasBgPtat_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                 uint8_t channelId, 
                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_MbiasIgenPd_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                 uint8_t channelId, 
                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_MbiasRtrimResetb_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_MbiasTrimCompPd_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_McsStatus_BfSet(adi_adrv904x_Device_t* const device,
                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                               const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_McsStatus_BfGet(adi_adrv904x_Device_t* const device,
                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                               const adrv904x_BfCoreChanAddr_e baseAddr,
                                                               uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_MemoryRepairStart_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_OrxRsModeSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_PadDs0_BfSet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                            uint8_t channelId, 
                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_PadDs0_BfGet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                            uint8_t channelId, 
                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_PadDs1_BfSet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                            uint8_t channelId, 
                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_PadDs1_BfGet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                            uint8_t channelId, 
                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_PadDs2_BfSet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                            uint8_t channelId, 
                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_PadDs2_BfGet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                            uint8_t channelId, 
                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_PadDs3_BfSet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                            uint8_t channelId, 
                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_PadDs3_BfGet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                            uint8_t channelId, 
                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_PadSt_BfSet(adi_adrv904x_Device_t* const device,
                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                           uint8_t channelId, 
                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_PadSt_BfGet(adi_adrv904x_Device_t* const device,
                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                           uint8_t channelId, 
                                                           uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrx0Map_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrx0MapShadow_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrx0MapShadow_BfGet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrx1Map_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrx1MapShadow_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrx1MapShadow_BfGet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrxonReadback_BfGet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrxArmModeSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrxArmModeSelClr_BfSet(adi_adrv904x_Device_t* const device,
                                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                           const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrxSpiEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrxSpiEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrxSpiModeSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                        const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceOrxSpiModeSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxonReadback_BfGet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin2_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin2_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin3_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin3_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin4_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin4_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin5_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin5_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin6_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin6_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin7_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxAntEnPin7_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxArmModeSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxArmModeSelClr_BfSet(adi_adrv904x_Device_t* const device,
                                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin2_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin2_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin3_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin3_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin4_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin4_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin5_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin5_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin6_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin6_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin7_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxEnPin7_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxSpiEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxSpiEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxSpiModeSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceRxSpiModeSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxonReadback_BfGet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin2_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin2_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin3_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin3_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin4_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin4_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin5_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin5_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin6_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin6_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin7_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxAntEnPin7_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxArmModeSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxArmModeSelClr_BfSet(adi_adrv904x_Device_t* const device,
                                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin2_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin2_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin3_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin3_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin4_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin4_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin5_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin5_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin6_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin6_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin7_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxEnPin7_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxSpiEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxSpiEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxSpiModeSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RadioControlInterfaceTxSpiModeSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RsSampleClkDivideRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RsSampleClkEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RxAntRsModeSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_RxRsModeSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_ScratchPadWord_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_ScratchPadWord_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_ScratchReg_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t channelId, 
                                                                const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_ScratchReg_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t channelId, 
                                                                uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Spi1En_BfSet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Spi1En_BfGet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                            uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_SpDbgGlobalResume_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_StreamProcGpioPinMask_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           const uint32_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_StreamProcGpioPinMask_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                           uint32_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_SyncInCmosSchmittTrigEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_SyncInLvdsPnInvert_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_SyncOut0CmosTxDriveStrength_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_SyncOut1CmosTxDriveStrength_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_SyncOutLvdsAndCmosEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_SyncOutLvdsPnInvert_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx0AttenS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx0AttenS0_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx0AttenS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx0AttenS1_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx0AttenUpdateS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx0AttenUpdateS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx1AttenS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx1AttenS0_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx1AttenS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx1AttenS1_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx1AttenUpdateS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx1AttenUpdateS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx2AttenS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx2AttenS0_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx2AttenS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx2AttenS1_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx2AttenUpdateS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx2AttenUpdateS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx3AttenS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx3AttenS0_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx3AttenS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx3AttenS1_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx3AttenUpdateS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx3AttenUpdateS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx4AttenS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx4AttenS0_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx4AttenS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx4AttenS1_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx4AttenUpdateS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx4AttenUpdateS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx5AttenS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx5AttenS0_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx5AttenS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx5AttenS1_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx5AttenUpdateS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx5AttenUpdateS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx6AttenS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx6AttenS0_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx6AttenS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx6AttenS1_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx6AttenUpdateS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx6AttenUpdateS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx7AttenS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx7AttenS0_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx7AttenS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                const uint16_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx7AttenS1_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx7AttenUpdateS0_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_Tx7AttenUpdateS1_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_TxAntRsModeSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_TxAttenUpdCoreSpi_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_TxAttenUpdCoreSpiEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_TxAttenUpdCoreSpiEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_TxRsModeSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                 const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_UseDeviceClkAsHsdigclk_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_VendorId_BfGet(adi_adrv904x_Device_t* const device,
                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                              const adrv904x_BfCoreChanAddr_e baseAddr,
                                                              uint16_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_VswrFwdRev0Sel_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_VswrFwdRev0SelShadow_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_VswrFwdRev1Sel_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);

ADI_API adi_adrv904x_ErrAction_e adrv904x_Core_VswrFwdRev1SelShadow_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCoreChanAddr_e baseAddr,
                                                                          const uint8_t bfValue);

#endif // _ADRV904X_BF_CORE_H_

