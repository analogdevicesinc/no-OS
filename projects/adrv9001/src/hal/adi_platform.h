/***************************************************************************//**
 *   @file   adi_platform.h
 *   @brief  adrv9002 HAL functions interface.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
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

#ifdef _ADI_STS_PLATFORM
#include "sts_hal/niSTSHAL.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLIENT_IGNORE
/**
 * BBIC Init, Open, Close functions
 */
extern int32_t(*adi_hal_HwOpen)(void *devHalCfg);
extern int32_t(*adi_hal_HwClose)(void *devHalCfg);
extern int32_t(*adi_hal_HwReset)(void *devHalCfg, uint8_t pinLevel);
extern void* (*adi_hal_DevHalCfgCreate)(uint32_t interfaceMask,
					uint8_t spiChipSelect, const char *logFilename);
extern int32_t(*adi_hal_DevHalCfgFree)(void *devHalCfg);

/**
 * BBIC SPI functions
 */
extern int32_t(*adi_hal_SpiWrite)(void *devHalCfg, const uint8_t txData[],
				  uint32_t numTxBytes);
extern int32_t(*adi_hal_SpiRead)(void *devHalCfg, const uint8_t txData[],
				 uint8_t rxData[], uint32_t numRxBytes);

/**
 * BBIC I2C functions
 */
extern int32_t(*adi_hal_I2C_Write)(void *devHalCfg, uint8_t slaveAddress,
				   const uint8_t wrData[], uint32_t numWrBytes);
extern int32_t(*adi_hal_I2C_Read)(void *devHalCfg, uint8_t slaveAddress,
				  const uint8_t txData[], uint32_t numTxBytes, uint8_t rxData[],
				  uint32_t numRxBytes);

/**
 * BBIC Logging functions
 */
extern int32_t(*adi_hal_LogFileOpen)(void *devHalCfg, const char *filename);
extern int32_t(*adi_hal_LogFileClose)(void *devHalCfg);
extern int32_t(*adi_hal_LogLevelSet)(void *devHalCfg, int32_t logLevel);
extern int32_t(*adi_hal_LogLevelGet)(void *devHalCfg, int32_t *logLevel);
extern int32_t(*adi_hal_LogWrite)(void *devHalCfg, int32_t logLevel,
				  const char *comment, va_list argp);

/**
 * BBIC Timer functions
 */
extern int32_t(*adi_hal_Wait_us)(void *devHalCfg, uint32_t time_us);
extern int32_t(*adi_hal_Wait_ms)(void *devHalCfg, uint32_t time_ms);

/**
 * BBIC Hal layer setup
 */
extern int32_t adi_hal_PlatformSetup(void *devHalCfg,
				     adi_hal_Platforms_e platform);

/**
 * BBIC File-IO abstraction
 */
extern int32_t(*adi_hal_ArmImagePageGet)(void *devHalCfg,
		const char *armImagePath, uint32_t pageIndex, uint32_t pageSize,
		uint8_t *rdBuff);
extern int32_t(*adi_hal_StreamImagePageGet)(void *devHalCfg,
		const char *streamImagePath, uint32_t pageIndex, uint32_t pageSize,
		uint8_t *rdBuff);
extern int32_t(*adi_hal_RxGainTableEntryGet)(void *devHalCfg,
		const char *rxGainTablePath, uint16_t lineCount, uint8_t *gainIndex,
		uint8_t *rxFeGain, uint8_t *tiaControl, uint8_t *adcControl,
		uint8_t *extControl, uint16_t *phaseOffset, int16_t *digGain);
extern int32_t(*adi_hal_TxAttenTableEntryGet)(void *devHalCfg,
		const char *txAttenTablePath, uint16_t lineCount, uint16_t *attenIndex,
		uint8_t *txAttenHp, uint16_t *txAttenMult);

/**
 * BBIC GPIO functions - Currently only used to toggle hard reset pin on each device.
 *
 * ADI FPGA platform functions (only required for ADI FPGA platform)
 */
extern int32_t (*adi_hal_BbicRegisterRead)(void *devHalCfg, uint32_t addr,
		uint32_t *data);
extern int32_t (*adi_hal_BbicRegisterWrite)(void *devHalCfg, uint32_t addr,
		uint32_t data);
extern int32_t (*adi_hal_BbicRegistersRead)(void *devHalCfg, uint32_t addr,
		uint32_t data[], uint32_t numDataWords);
extern int32_t (*adi_hal_BbicRegistersWrite)(void *devHalCfg, uint32_t addr,
		uint32_t data[], uint32_t numDataWords);
#endif /* CLIENT_IGNORE */

/**
 * @brief Send the specified number of MCS pulses
 *
 * @param[in] devHalCfg         User HAL configuration
 * @param[in] numberOfPulses    The number of times to pulse the MCS pin
 *
 * @returns 0 indicating success, otherwise negative to indicate error
 */
extern int32_t (*adi_hal_Mcs_Pulse)(void *devHalCfg, uint8_t numberOfPulses);

/**
 * @brief Reset the SSI interface
 *
 * @param[in] devHalCfg         User HAL configuration
 *
 * @returns 0 indicating success, otherwise negative to indicate error
 */
extern int32_t(*adi_hal_ssi_Reset)(void *devHalCfg);

#ifdef __cplusplus
}
#endif
#endif /* __ADI_PLATFORM_H__ */


