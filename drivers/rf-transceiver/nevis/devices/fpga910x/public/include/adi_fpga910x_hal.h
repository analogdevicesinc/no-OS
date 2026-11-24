/**
 * \file
 * \brief Hardware abstraction layer (HAL) for interacting with the FPGA910X
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_HAL_H_
#define _ADI_FPGA910X_HAL_H_

#include "adi_fpga910x_hal_types.h"
#include "adi_fpga910x_ssi_types.h"


#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup fpga910x_hal "FPGA910X HAL"
 * The FPGA910X HAL consists of function pointers which must be implemented by the user, such that the FPGA910X API can
 * call said functions to interact with the chip without having to worry about hardware details.
 * For an example implementation, see adi_fpga910x_hal_linux_uio.c.
 * @{
 */

/**
 * \brief Performs a read of a single 32-bit register
 *
 * \param[in]  devHalCfg  User-defined context variable
 * \param[in]  addr       Address of the register to be read. Must be on a word boundary
 * \param[out] data       The value read from the register
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_Register_Read)(void *devHalCfg, const uint32_t addr, uint32_t *data);

/**
 * \brief Performs a write of a single 32-bit register
 *
 * \param[in] devHalCfg  User-defined context variable
 * \param[in] addr       Address of the register to be written. Must be on a word boundary
 * \param[in] data       The value to write to the register
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_Register_Write)(void *devHalCfg, uint32_t addr, uint32_t data);

/**
 * \brief Performs a RAM memory read
 *
 * \param[in]  devhalCfg   User-defined context variable
 * \param[in]  ramAddress  Address to begin the read from
 * \param[out] data        The array of values read from RAM. Must have enough space to store 'length' elements
 * \param[in]  length      The number of words to read starting from ramAddress
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_Ram_Read)(void *devhalCfg,
                                           const uint32_t ramAddress,
                                           uint32_t data[],
                                           uint32_t length);

/**
 * \brief Set the FPGA SSI type to CMOS or LVDS
 *
 * \param[in] devHalCfg  User-defined context variable
 * \param[in] ssiType    The desired SSI type: CMOS or LVDS
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910x API functions.
 */
extern int32_t(*adi_fpga910x_hal_SsiType_Set)(void *devHalCfg, adi_fpga910x_SsiType_e ssiType);

/**
 * \brief Performs a RAM memory write
 *
 * \param[in] devHalCfg   User-defined context variable
 * \param[in] ramAddress  Address to begin writing to
 * \param[in] data        Array of values to write to RAM
 * \param[in] length      The number of words to write starting at ramAddress
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_Ram_Write)(void *devHalCfg,
                                            const uint32_t ramAddress,
                                            uint32_t data[],
                                            uint32_t length);

/**
 * \brief Get the relative memory address where data for the specified DMA will be stored
 *
 * \param[in]  devHalCfg  User-defined context variable
 * \param[in]  dma        The DMA of interest
 * \param[out] addr       The returned address
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_DmaRelativeAddress_Get)(void *devHalCfg, adi_fpga910x_hal_Dma_e dma, uint32_t *addr);

/**
 * \brief Get the current FPGA platform. ZC706 and ZCU102 are the current supported platforms
 *
 * \param[in]  devHalCfg  User-defined context variable
 * \param[out] platform   The current FPGA platform
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_CurrentPlatform_Get)(void *devHalCfg, adi_fpga910x_Platform_e *platform);

/**
 * \brief Opens and configure the local UART for communication
 *
 * \param[in] devHalCfg   User-defined context variable
 * \param[in] uartConfig  The requested UART configuration
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_Uart_Open)(void *devHalCfg, const adi_hal_uartCfg_t *uartConfig);

/**
 * \brief Closes the local uart and frees any resources
 *
 * \param[in] devHalCfg  User-defined context variable
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_Uart_Close)(void *devHalCfg);

/**
 * \brief Read from the local UART
 *
 * \param[in]     devHalCfg    User-defined context variable
 * \param[out]    rxData       The user supplied buffer to store rx bytes in
 * \param[in,out] rxByteCount  On entry caller must set to the number of bytes in rxData, on return set to the number of bytes received
 * \param[in]     timeOut_ms   The number of milliseconds to wait for data, 0 return immediately, 0xFFFFFFFF to block until data is received
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
extern int32_t(*adi_fpga910x_hal_Uart_Read)(void *devHalCfg,
                                            uint8_t rxData[],
                                            uint32_t *rxByteCount,
                                            uint32_t timeOut_ms);

/**
 * \brief Writes to the local UART
 *
 * \param[in] devHalCfg    User-defined context variable
 * \param[in] txData       The data to transmit over the UART
 * \param[in] txByteCount  The number of bytes in txData to transmit
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
extern int32_t(*adi_fpga910x_hal_Uart_Write)(void *devHalCfg, const uint8_t txData[], uint32_t txByteCount);

/**
 * \brief Opens and configures the QSPI bus in standard (single bit) SPI mode as a master
 *
 * \param[in] devHalCfg   User-defined context variable
 * \note Bit order must match the far end. Setting qspiConfig->lsbFirst reads SPICR bit 9 back and
 *       fails if this build of the core does not support LSB first, because a silent mismatch shows
 *       up as every byte being bit reversed rather than as a configuration error.
 * \note LSB first is only permitted in standard (single bit) SPI mode. The AXI Quad SPI core supports
 *       it for standard mode alone - dual and quad I/O transfers are MSB first only. This entry point
 *       is standard mode by definition, so the constraint is satisfied by construction. An
 *       implementation that adds dual or quad mode must reject lsbFirst rather than pass it through.
 *
 * \param[in] qspiConfig  The requested QSPI configuration
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_Qspi_Open)(void *devHalCfg, const adi_hal_qspiCfg_t *qspiConfig);

/**
 * \brief Closes the QSPI bus and frees any resources
 *
 * \param[in] devHalCfg  User-defined context variable
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_Qspi_Close)(void *devHalCfg);

/**
 * \brief Performs a full duplex QSPI transfer, capturing the bytes clocked in
 *
 * SPI is full duplex, so reading rxByteCount bytes means clocking rxByteCount bytes out. Supply the
 * bytes to send in txData, or pass NULL to clock out a dummy pattern (0xAA by default, fixed when
 * the driver is built).
 *
 * Slave select framing is set at Qspi_Open, not here, by adi_hal_qspiCfg_t.manualSlaveSelect. With
 * that true, slave select is asserted for the whole call and released on return, so a command and
 * the data it returns must be requested together in a single call rather than as a write followed
 * by a read. With it false the hardware is free to deassert slave select between words, framing the
 * call one word at a time, in which case a command and its response cannot be issued as one frame.
 * An implementation must honour the true case; the false case is whatever its hardware does.
 *
 * \note No memory is allocated for the NULL txData case. The dummy bytes are clocked out of rxData
 *       itself, which is filled with them first, so rxData must be writable for the whole of
 *       rxByteCount.
 *
 * \note An implementation whose hardware transfers more than one byte per word may require these
 *       buffers to be aligned to that width and reject them otherwise. The width is a property of the
 *       implementation, not of this interface, and is not settable through adi_hal_qspiCfg_t.
 *
 * \param[in]  devHalCfg    User-defined context variable
 * \param[in]  txData       The data to transmit, or NULL to transmit the dummy pattern
 * \param[out] rxData       The user supplied buffer to store rx bytes in, must hold rxByteCount bytes
 * \param[in]  rxByteCount  The number of bytes to transfer
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_Qspi_Read)(void *devHalCfg,
                                            const uint8_t txData[],
                                            uint8_t rxData[],
                                            uint32_t rxByteCount);

/**
 * \brief Writes to the QSPI bus, discarding the bytes clocked in
 *
 * Slave select framing is set at Qspi_Open, not here, by adi_hal_qspiCfg_t.manualSlaveSelect. With
 * that true, slave select is asserted for the whole call and released on return. With it false the
 * hardware is free to deassert slave select between words, framing the call one word at a time.
 *
 * \note An implementation whose hardware transfers more than one byte per word may require txData to be
 *       aligned to that width and reject it otherwise. The width is a property of the implementation,
 *       not of this interface, and is not settable through adi_hal_qspiCfg_t.
 *
 * \param[in] devHalCfg    User-defined context variable
 * \param[in] txData       The data to transmit over the QSPI bus
 * \param[in] txByteCount  The number of bytes in txData to transmit
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by FPGA910X API functions.
 */
extern int32_t(*adi_fpga910x_hal_Qspi_Write)(void *devHalCfg, const uint8_t txData[], uint32_t txByteCount);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_HAL_H_ */
