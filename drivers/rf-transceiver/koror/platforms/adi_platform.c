/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_platform.c
*
* \brief Definitions for ADI Specific Platforms
*
* ADRV904X API Version: 2.9.0.4
*/

#include "adi_platform.h"

#ifdef _ADI_ADS10_PLATFORM
#include "ads10/ads10_init.h"
#include "ads10/ads10_i2c.h"
#include "ads10/ads10_spi.h"
#include "ads10/ads10_timer.h"
#include "ads10/ads10_bbic_control.h"
#include "posix/posix_mutex.h"
#endif

#include "../platforms/common/tls.h"
#include "../platforms/common/adi_logging.h"

///*
// * Function pointer assignment for default configuration
// */
//
///* Initialization interface to open, init, close drivers and pointers to resources */
//adi_hal_Err_e (*adi_hal_HwOpen)(void* const devHalCfg)                          = NULL;
//
//adi_hal_Err_e (*adi_hal_HwClose)(void* const devHalCfg)                         = NULL;
//
//adi_hal_Err_e (*adi_hal_HwReset)(void* const devHalCfg, const uint8_t pinLevel) = NULL;
//
//void*         (*adi_hal_DevHalCfgCreate)(   const uint32_t      interfaceMask,
//                                            const uint8_t       spiChipSelect,
//                                            const char* const   logFilename)    = NULL;
//
//adi_hal_Err_e (*adi_hal_DevHalCfgFree)(void* devHalCfg)                         = NULL;
//
///* SPI Interface */
//adi_hal_Err_e (*adi_hal_SpiWrite)(  void* const     devHalCfg,
//                                    const uint8_t   txData[],
//                                    const uint32_t  numTxBytes) = NULL;
//
//adi_hal_Err_e (*adi_hal_SpiRead)(   void* const     devHalCfg,
//                                    const uint8_t   txData[],
//                                    uint8_t         rxData[],
//                                    const uint32_t  numRxBytes) = NULL;
//
///* I2C Interface */
//adi_hal_Err_e(*adi_hal_I2cWrite)(   void* const     devHalCfg,
//                                    const uint8_t   txData[],
//                                    const uint32_t  numTxBytes) = NULL;
//
//adi_hal_Err_e(*adi_hal_I2cRead)(    void* const     devHalCfg,
//                                    const uint8_t   txData[],
//                                    const uint32_t  numTxBytes,
//                                    uint8_t         rxData[],
//                                    const uint32_t  numRxBytes) = NULL;
//
///* Logging interface */
//adi_hal_Err_e (*adi_hal_LogFileOpen)(   void* const         devHalCfg,
//                                        const char* const   filename)   = NULL;
//
//adi_hal_Err_e (*adi_hal_LogLevelSet)(   void* const     devHalCfg,
//                                        const uint32_t  logLevelMask)   = NULL;
//
//adi_hal_Err_e (*adi_hal_LogLevelGet)(   void* const     devHalCfg,
//                                        uint32_t* const logLevelMask)   = NULL;
//
//adi_hal_Err_e (*adi_hal_LogStatusGet)(  void* const                     devHalCfg,
//                                        adi_hal_LogStatusGet_t* const  logStatus)   = NULL;
//
//adi_hal_Err_e (*adi_hal_LogConsoleSet)( void* const devHalCfg,
//                                        const adi_hal_LogConsole_e  logConsoleFlag) = NULL;
//
//adi_hal_Err_e (*adi_hal_LogWrite)(  void* const                 devHalCfg,
//                                    const adi_hal_LogLevel_e    logLevel,
//                                    const uint8_t               indent,
//                                    const char* const           comment,
//                                    va_adi_hal_DevHalCfgFreelist                     argp)   = NULL;
//
//adi_hal_Err_e (*adi_hal_LogFileClose)(void* const devHalCfg) = NULL;
//
///* Timer interface */
//adi_hal_Err_e (*adi_hal_Wait_ms)(void* const devHalCfg, const uint32_t time_ms) = NULL;
//adi_hal_Err_e (*adi_hal_Wait_us)(void* const devHalCfg, const uint32_t time_us) = NULL;
//
///* BBIC control interface */
//adi_hal_Err_e (*adi_hal_BbicRegisterRead)(  void* const     devHalCfg,
//                                            const uint32_t  addr,
//                                            uint32_t* const data)           = NULL;
//
//adi_hal_Err_e (*adi_hal_BbicRegisterWrite)( void* const     devHalCfg,
//                                            const uint32_t  addr,
//                                            const uint32_t  data)           = NULL;
//
//adi_hal_Err_e (*adi_hal_BbicRegistersRead)( void* const     devHalCfg,
//                                            const uint32_t  addr,
//                                            uint32_t        data[],
//                                            const uint32_t  numDataWords)   = NULL;
//
//adi_hal_Err_e (*adi_hal_BbicRegistersWrite)(void* const     devHalCfg,
//                                            const uint32_t  addr,
//                                            const uint32_t  data[],
//                                            const uint32_t  numDataWords)   = NULL;
//
///* Thread Interface */
//adi_hal_thread_t (*adi_hal_ThreadSelf)(void) = NULL;
//
//adi_hal_Err_e (*adi_hal_TlsSet)(const adi_hal_TlsType_e tlsType, void* const value) = NULL;
//
//void* (*adi_hal_TlsGet)(const adi_hal_TlsType_e tlsType) = NULL;
//
///* Mutex Interface */
//adi_hal_Err_e(*adi_hal_MutexInit)(adi_hal_mutex_t* const mutex) = NULL;
//adi_hal_Err_e(*adi_hal_MutexLock)(adi_hal_mutex_t* const mutex) = NULL;
//adi_hal_Err_e(*adi_hal_MutexUnlock)(adi_hal_mutex_t* const mutex) = NULL;
//adi_hal_Err_e(*adi_hal_MutexDestroy)(adi_hal_mutex_t* const mutex) = NULL;
//
//adi_hal_Err_e(*adi_hal_BoardIdentify)(char** boardNames, int32_t* numBoards) = NULL;
//
//ADI_API adi_hal_Err_e adi_hal_PlatformSetup(const adi_hal_Platforms_e platform)
//{
//    adi_hal_Err_e error = ADI_HAL_ERR_PARAM;
//
//    switch (platform)
//    {
//    case ADI_ADS10_PLATFORM:
//#ifdef _ADI_ADS10_PLATFORM
//        adi_hal_HwOpen = ads10_HwOpen;
//        adi_hal_HwClose = ads10_HwClose;
//        adi_hal_HwReset = ads10_HwReset;
//        adi_hal_DevHalCfgCreate = ads10_DevHalCfgCreate;
//        adi_hal_DevHalCfgFree = ads10_DevHalCfgFree;
//
//#ifdef ADI_ADRV904X_SPI_DEV_DRIVER_EN
//        adi_hal_SpiWrite = ads10_SpiWrite;
//        adi_hal_SpiRead = ads10_SpiRead;
//#else
//        adi_hal_SpiWrite = ads10_SpiWrite_v2;
//        adi_hal_SpiRead = ads10_SpiRead_v2;
//#endif
//        adi_hal_I2cWrite = NULL; /* ADS10 does not require I2C interface to any devices used in device API layer */
//        adi_hal_I2cRead = NULL;  /* ADS10 does not require I2C interface to any devices used in device API layer */
//
//        adi_hal_LogFileOpen     = adi_LogFileOpen;
//        adi_hal_LogLevelSet     = adi_LogLevelSet;
//        adi_hal_LogLevelGet     = adi_LogLevelGet;
//        adi_hal_LogStatusGet    = adi_LogStatusGet;
//        adi_hal_LogConsoleSet   = adi_LogConsoleSet;
//        adi_hal_LogWrite        = adi_LogWrite;
//        adi_hal_LogFileClose    = adi_LogFileClose;
//
//        adi_hal_Wait_us = ads10_TimerWait_us;
//        adi_hal_Wait_ms = ads10_TimerWait_ms;
//
//        /* only required to support the ADI FPGA*/
//        adi_hal_BbicRegisterRead   = ads10_BbicRegisterRead;
//        adi_hal_BbicRegisterWrite  = ads10_BbicRegisterWrite;
//        adi_hal_BbicRegistersRead  = ads10_BbicRegistersRead;
//        adi_hal_BbicRegistersWrite = ads10_BbicRegistersWrite;
//
//        adi_hal_ThreadSelf = posix_ThreadSelf;
//        adi_hal_TlsGet = common_TlsGet;
//        adi_hal_TlsSet = common_TlsSet;
//        adi_hal_MutexInit = posix_MutexInit;
//        adi_hal_MutexLock = posix_MutexLock;
//        adi_hal_MutexUnlock = posix_MutexUnlock;
//        adi_hal_MutexDestroy = posix_MutexDestroy;
//        adi_hal_BoardIdentify = ads10_BoardIdentify;
//        error = common_TlsInit();
//#else
//        error = ADI_HAL_ERR_NOT_IMPLEMENTED;
//#endif
//        break;
//                default:
//            error = ADI_HAL_ERR_PARAM;
//            break;
//    }
//
//    return error;
//}
