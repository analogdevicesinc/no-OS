/***************************************************************************//**
 *   @file   adi_platform.h
 *   @brief  adrv9002 HAL types.
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

#ifndef __ADI_PLATFORM_TYPES_H__
#define __ADI_PLATFORM_TYPES_H__

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "gpio.h"
#include "spi.h"

#define ADI_HAL_STRING_LENGTH 64
#define ADI_HAL_MAX_LOG_LINE 1000

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief Enum of possible Errors Detected by HAL layer to be communicated
 *         to ADI APIs.
 */
typedef enum adi_hal_Err {
	ADI_HAL_OK                    = 0,  /*!< HAL function successful. No error Detected */
	ADI_HAL_I2C_FAIL,                   /*!< HAL I2C operation failure. I2C controller Down */
	ADI_HAL_SPI_FAIL,                   /*!< HAL SPI operation failure. SPI controller Down */
	ADI_HAL_GPIO_FAIL,                  /*!< HAL GPIO function Failure */
	ADI_HAL_TIMER_FAIL,                 /*!< HAL Timer function Failure */
	ADI_HAL_LOGGING_FAIL,               /*!< HAL Logging function Failure */
	ADI_HAL_LOGGGING_LEVEL_FAIL,        /*!< HAL Logging level Failure */
	ADI_HAL_WAIT_TIMEOUT,               /*!< HAL function Timeout */
	ADI_HAL_GEN_SW,                     /*!< HAL function failed due to general invalid  HAL data*/
	ADI_HAL_FUNCTION_NOT_IMP,           /*!< HAL function error, not implemented in HAL layer */
	ADI_HAL_LIBRARY_NOT_AVAILABLE,      /*!< HAL function error, HAL layer library not found or available in this build */
	ADI_HAL_WARNING,                    /*!< HAL function warning that non critical error was detected*/
	ADI_HAL_BBICCTRL_FAIL,              /*!< BBIC control interface HAL function failure */
	ADI_HAL_NULL_PTR                    /*!< Function supplied with null pointer */
} adi_hal_Err_e;

/**
 *  @brief An enumerated type in bit mask format to list the log message
 *         categories or groups.
 */
/* In order to seperate trx_hal and the platform layer this enum is duplicated.
   its sister enum is adi_common_LogLevel_e in adi_common_log_types.h which need to reflect
   any changes to this enum.
*/
typedef enum adi_hal_LogLevel {
	ADI_HAL_LOG_NONE = 0x0,  /*!< HAL Log enum to represent all types of log messages not selected */
	ADI_HAL_LOG_MSG  = 0x1,  /*!< HAL Log enum to represent a log message type*/
	ADI_HAL_LOG_WARN = 0x2,  /*!< HAL Log enum to represent a warning message type*/
	ADI_HAL_LOG_ERR  = 0x4,  /*!< HAL Log enum to represent a error message type*/
	ADI_HAL_LOG_API  = 0x8,  /*!< HAL Log enum to represent an API function entry for loging pourposes*/
	ADI_HAL_LOG_API_PRIV = 0x10,  /*!< HAL Log enum to represent an Private API function entry for loging pourposes*/
	ADI_HAL_LOG_BF   = 0x20, /*!< HAL Log enum to represent a BF function entry for loging pourposes*/
	ADI_HAL_LOG_HAL  = 0x40, /*!< HAL Log enum to represent a ADI HAL function entry for loging pourposes*/
	ADI_HAL_LOG_SPI  = 0x80, /*!< HAL Log enum to represent a spi transaction type*/
	ADI_HAL_LOG_ALL  = 0xFF  /*!< HAL Log enum to represent all types of log messages selected */
} adi_hal_LogLevel_e;

/**
 * @brief Enum type for HAL platforms
 */
typedef enum adi_hal_Platforms {
	ADI_HW_PLATFORM,    /*!< Normal Mode of operation */
	ADI_SW_PLATFORM,    /*!< Provides ability to use alt software emulation */
	ADI_STS_PLATFORM    /*!< Test platform, uses DLL from test */
} adi_hal_Platforms_e;

/**
 * @brief Enum type for the hardware interfaces supported by the platform.
 *        Each device specifies which interfaces it needs from the BBIC in its
 *        devHalCfg structure.
 */
typedef enum adi_hal_Interfaces {
	ADI_HAL_INTERFACE_SPI       = 0x01, /* Device requires SPI interface from BBIC */
	ADI_HAL_INTERFACE_LOG       = 0x02, /* Device requires Logging interface from BBIC */
	ADI_HAL_INTERFACE_BBICCTRL  = 0x04, /* Device requires BBIC Register/Control interface from BBIC */
	ADI_HAL_INTERFACE_HWRESET   = 0x08, /* Device requires GPIO (reset pins) interface from BBIC */
	ADI_HAL_INTERFACE_TIMER     = 0x10, /* Device requires Timer interface from BBIC */
	ADI_HAL_INTERFACE_I2C       = 0x20  /* Device requires I2C interface from BBIC */
} adi_hal_Interfaces_e;

/**
 * @brief Data structure for Logging facility
 */
typedef struct adi_hal_LogCfg {
	uint8_t interfaceEnabled;
	int32_t logLevel;         /*!< valid 0 - 0xFF */
	uintptr_t logfd;
	char logFileName[ADI_HAL_STRING_LENGTH];
	uint32_t currentLineNumber;
} adi_hal_LogCfg_t;

/**
 * @brief Data structure for SPI configuration
 */
typedef struct adi_hal_SpiCfg {
	uint8_t interfaceEnabled;
	int fd;
	char spidevName[ADI_HAL_STRING_LENGTH];
	uint8_t chipSelectIndex;        /*!< valid 1~8 */
	uint8_t CPHA;                   /*!< clock phase, sets which clock edge the data updates (valid 0 or 1) */
	uint8_t CPOL;                   /*!< clock polarity 0 = clock starts low, 1 = clock starts high */
	uint8_t enSpiStreaming;         /*!< Not implemented. SW feature to improve SPI throughput. */
	uint8_t autoIncAddrUp;          /*!< Not implemented. For SPI Streaming, set address increment direction. 1= next addr = addr+1, 0:addr = addr-1 */
	uint8_t fourWireMode;           /*!< 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode. */
	uint32_t spiClkFreq_Hz;         /*!< SPI Clk frequency in Hz (default 25000000), platform will use next lowest frequency that it's baud rate generator can create */
	uint8_t  spiActionDisable;      /*!< Disables the spi Driver actually writing the data */
} adi_hal_SpiCfg_t;

/**
 * @brief Data structure for Hardware reset pin functionality
 */
typedef struct adi_hal_HwResetCfg {
	uint8_t interfaceEnabled;
	uint32_t resetPinIndex;
} adi_hal_HwResetCfg_t;


/**
 * @brief Data structure for I2C configuration
 */
typedef struct adi_hal_I2cCfg {
	uint8_t interfaceEnabled;
	int fd;
	char drvName[ADI_HAL_STRING_LENGTH];
} adi_hal_I2cCfg_t;

/**
 * @brief Data structure for memory Fpga Driver configuration
 */
typedef struct adi_hal_fpga_AxiCfg {
	char drvName[ADI_HAL_STRING_LENGTH];
	uint32_t mapSize;
	int fd;
} adi_hal_fpga_AxiCfg_t;

/**
 * @brief Data structure to hold platform Hardware layer
 *        settings for all system related feature of the ADI transceiver device
 * If the system has multiple ADI transceivers, the user should create one of
 * theses structures per transceiver device to specify the HAL settings
 * per transceiver device.
 */
typedef struct adi_hal_BbicCfg {
	uint8_t interfaceEnabled;
	adi_hal_fpga_AxiCfg_t coreRegsCfg;
	adi_hal_fpga_AxiCfg_t ramCfg;
	adi_hal_fpga_AxiCfg_t spiAdvRegsCfg;
} adi_hal_BbicCfg_t;

typedef struct adi_hal_TimerCfg {
	uint8_t interfaceEnabled;
	/* Platform does not require any instance members per device */
} adi_hal_TimerCfg_t;


/**
 * @brief Data structure to hold platform Hardware layer
 *        settings for all system/platform related features.

 * If the system has multiple ADI transceivers/devices, the user should create one of
 * theses structures per device to specify the HAL settings
 * per transceiver device.
 */
typedef struct adi_hal_Cfg {
	char typeName[ADI_HAL_STRING_LENGTH];
	adi_hal_SpiCfg_t spiCfg;
	adi_hal_LogCfg_t logCfg;
	adi_hal_BbicCfg_t bbicCfg;
	adi_hal_HwResetCfg_t hwResetCfg;
	adi_hal_I2cCfg_t i2cCfg;
	adi_hal_TimerCfg_t timerCfg;

	/* no-OS specifics */
	struct gpio_desc *gpio_reset_n;
	struct gpio_desc *gpio_ssi_sync;
	struct spi_desc *spi;
} adi_hal_Cfg_t;

#ifdef __cplusplus
}
#endif
#endif /* __ADI_PLATFORM_TYPES_H__*/


