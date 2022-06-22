// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_hal.h
 * \brief Contains prototypes and macro definitions for Private ADI HAL wrapper
 *        functions implemented in talise_hal.c
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_HAL_H_
#define TALISE_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_hal.h"

#define HAL_TIMEOUT_DEFAULT 100         /* 100ms */
#define HAL_TIMEOUT_NONE 0x0            /* Non-blocking */
#define HAL_TIMEOUT_INFINITE 0xFFFFFFFF /* Blocking */
#define HAL_TIMEOUT_MULT 2              /* HAL timeout worse-case factor */

/**
 * \brief Private wrapper function for ADIHAL_spiReadField with error handling
 *
 * This function can be called any time after devHalInfo has been initialized
 * with valid settings by the user
 *
 * \dep_begin
 * \dep{devHalInfo}
 * \dep_end
 *
 * \param devHalInfo Pointer to device HAL information container
 * \param addr 16-bit SPI address
 * \param fieldVal Pointer to byte read-back value
 * \param mask Bit mask for desired field
 * \param startBit Starting bit position for field mask operation
 *
 * \retval Returns adiHalErr_t enumerated type
 */
adiHalErr_t talSpiReadField(void *devHalInfo, uint16_t addr, uint8_t *fieldVal,
			    uint8_t mask, uint8_t startBit);

/**
 * \brief Private Wrapper function for ADIHAL_spiWriteField with error handling
 *
 * This function can be called any time after the devHalInfo has been initialized
 * with valid settings by the user
 *
 * \dep_begin
 * \dep{devHalInfo}
 * \dep_end
 *
 * \param devHalInfo Pointer to device HAL information container
 * \param addr 16-bit SPI address
 * \param fieldVal Byte value to be written
 * \param mask Bit mask for desired field
 * \param startBit Starting bit position for field mask operation
 *
 * \retval Returns adiHalErr_t enumerated type
 */
adiHalErr_t talSpiWriteField(void *devHalInfo, uint16_t addr, uint8_t fieldVal,
			     uint8_t mask, uint8_t startBit);

/**
 * \brief Private Wrapper function for ADIHAL_writeToLog with error handling
 *
 * This function can be called any time after the devHalInfo has been initialized
 * with valid settings by the user
 *
 * \dep_begin
 * \dep{devHalInfo}
 * \dep_end
 *
 * \param devHalInfo Pointer to device HAL information container
 * \param logLevel Enumerated type to distinguish logging level
 * \param errorCode Error code value to be written to log file
 * \param comment String pointer to error reporting comment string
 *
 * \retval Returns adiHalErr_t enumerated type
 */
adiHalErr_t talWriteToLog(void *devHalInfo, adiLogLevel_t logLevel,
			  uint32_t errorCode, const char *comment);

/**
 * \brief Private Wrapper function for ADIHAL_spiWriteByte with error handling
 *
 * This function can be called any time after the devHalInfo has been initialized
 * with valid settings by the user
 *
 * \dep_begin
 * \dep{devHalInfo}
 * \dep_end
 *
 * \param devHalInfo Pointer to device HAL information container
 * \param addr 16-bit SPI address
 * \param data Byte to be written to addr
 *
 * \retval Returns adiHalErr_t enumerated type
 */
adiHalErr_t talSpiWriteByte(void *devHalInfo, uint16_t addr, uint8_t data);

/**
 * \brief Wrapper function for ADIHAL_spiReadByte with error handling
 *
 * This function can be called any time after the devHalInfo has been initialized
 * with valid settings by the user
 *
 * \dep_begin
 * \dep{devHalInfo}
 * \dep_end
 *
 * \param devHalInfo Pointer to device HAL information container
 * \param addr 16-bit SPI address
 * \param readdata Pointer to byte value read from addr
 *
 * \retval Returns adiHalErr_t enumerated type
 */
adiHalErr_t talSpiReadByte(void *devHalInfo, uint16_t addr, uint8_t *readdata);

/**
 * \brief Wrapper function for ADIHAL_spiWriteBytes with error handling
 *
 * This function can be called any time after the devHalInfo has been initialized
 * with valid settings by the user
 *
 * \dep_begin
 * \dep{devHalInfo}
 * \dep_end
 *
 * \param devHalInfo Pointer to device HAL information container
 * \param addr 16-bit SPI address
 * \param data Pointer to byte array to be written starting at addr
 * \param count Number of bytes to be written
 *
 * \retval Returns adiHalErr_t enumerated type
 */
adiHalErr_t talSpiWriteBytes(void *devHalInfo, uint16_t *addr, uint8_t *data,
			     uint32_t count);

/**
 * \brief Wrapper function for ADIHAL_spiReadBytes with error handling
 *
 * This function can be called any time after the devHalInfo has been initialized
 * with valid settings by the user
 *
 * \dep_begin
 * \dep{devHalInfo}
 * \dep_end
 *
 * \param devHalInfo Pointer to device HAL information container
 * \param addr 16-bit SPI address
 * \param readdata Pointer to byte array for storing read data starting at addr
 * \param count Number of bytes to be read
 *
 * \retval Returns adiHalErr_t enumerated type
 */
adiHalErr_t talSpiReadBytes(void *devHalInfo, uint16_t *addr, uint8_t *readdata,
			    uint32_t count);

#ifdef __cplusplus
}
#endif

#endif /* TALISE_HAL_H_ */
