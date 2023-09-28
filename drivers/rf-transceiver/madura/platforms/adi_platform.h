/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_platform.h
* \brief Contains ADI Transceiver Hardware Abstraction functions interface
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*/

/**
* \Page Disclaimer Legal Disclaimer
* Copyright 2015 - 2019 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information.
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef __ADI_PLATFORM_H__
#define __ADI_PLATFORM_H__

#ifdef __GNUC__                    /* __unix__ verify if our linux image declare this */
#define OS_Windows 0
#else     /* windows  */

#if _WIN64 == 0      /* _Win32  */
  #define OS_Windows 32
#elif _WIN64 == 1      /* _Win64  */
  #define OS_Windows 64
#endif
#endif

#include "adi_platform_types.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNUSED_PARA
#define UNUSED_PARA(x) (void)(x)
#endif

/**
 * BBIC Init, Open, Close functions
 */
extern int32_t (*adi_hal_HwOpen)(void* devHalCfg);

extern int32_t (*adi_hal_HwClose)(void* devHalCfg);

extern int32_t (*adi_hal_HwReset)(void*   devHalCfg,
                                  uint8_t pinLevel);

extern int32_t (*adi_hal_HwVerify)(void *devHalCfg);

extern void* (*adi_hal_DevHalCfgCreate)(uint32_t    interfaceMask,
                                        uint8_t     spiChipSelect,
                                        const char* logFilename);

extern int32_t (*adi_hal_DevHalCfgFree)(void* devHalCfg);

/**
 * BBIC SPI functions
 */
extern int32_t (*adi_hal_SpiInit)(void* devHalCfg); /* TODO Remove */
extern int32_t (*adrv9025_hal_SpiWrite)(void*         devHalCfg,
                                   const uint8_t txData[],
                                   uint32_t      numTxBytes);

extern int32_t (*adrv9025_hal_SpiRead)(void*         devHalCfg,
                                  const uint8_t txData[],
                                  uint8_t       rxData[],
                                  uint32_t      numRxBytes);

/**
* Custom Spi streaming functions. Users don't need to implement streaming specific hal layer functions if they want to use standard SpiWrite/Read hal layer functions for streaming
*/
extern int32_t (*adi_hal_CustomSpiStreamWrite)(void*          devHalCfg,
                                               const uint16_t address,
                                               const uint8_t  txData[],
                                               uint32_t       numTxBytes,
                                               uint8_t        numBytesofAddress,
                                               uint8_t        numBytesOfDataPerStream);

extern int32_t (*adi_hal_CustomSpiStreamRead)(void*          devHalCfg,
                                              const uint16_t address,
                                              uint8_t        rxData[],
                                              uint32_t       numRxBytes,
                                              uint8_t        numBytesofAddress,
                                              uint8_t        numBytesOfDataPerStream);

/**
 * BBIC Logging functions
 */
extern int32_t (*adi_hal_LogFileOpen)(void*       devHalCfg,
                                      const char* filename);

extern int32_t (*adi_hal_LogFileClose)(void* devHalCfg);

extern int32_t (*adi_hal_LogLevelSet)(void*   devHalCfg,
                                      int32_t logLevel);

extern int32_t (*adi_hal_LogLevelGet)(void*    devHalCfg,
                                      int32_t* logLevel);

extern int32_t (*adi_hal_LogWrite)(void*       devHalCfg,
                                   int32_t     logLevel,
                                   const char* comment,
                                   va_list     argp);

/**
 * BBIC Timer functions
 */
extern int32_t (*adi_hal_Wait_us)(void*    devHalCfg,
                                  uint32_t time_us);

extern int32_t (*adi_hal_Wait_ms)(void*    devHalCfg,
                                  uint32_t time_ms);

/**
 * BBIC Hal layer setup
 */
extern int32_t adi_hal_PlatformSetup(void*               devHalCfg,
                                     adi_hal_Platforms_e platform);

/**
 * BBIC GPIO functions - Currently only used to toggle hard reset pin on each device.
 *
 * ADI FPGA platform functions (only required for ADI FPGA platform)
 */
extern int32_t (*adi_hal_BbicRegisterRead)(void*     devHalCfg,
                                           uint32_t  addr,
                                           uint32_t* data);

extern int32_t (*adi_hal_BbicRegisterWrite)(void*    devHalCfg,
                                            uint32_t addr,
                                            uint32_t data);

extern int32_t (*adi_hal_BbicRegistersRead)(void*    devHalCfg,
                                            uint32_t addr,
                                            uint32_t data[],
                                            uint32_t numDataWords);

extern int32_t (*adi_hal_BbicRegistersWrite)(void*    devHalCfg,
                                             uint32_t addr,
                                             uint32_t data[],
                                             uint32_t numDataWords);

/**
 * Platform and board discovery functions
 * This functions are called while in board discovery in order to assignate the correct HAL layer and FPGA drivers
 */
extern int32_t (*adi_hal_PlatformIdentify)(uint32_t* platform);

extern struct FRU_DATA*(*adi_hal_BoardIdentify)(void* devHalCfg);

#ifdef __cplusplus
}
#endif
#endif /* __ADI_PLATFORM_H__ */
