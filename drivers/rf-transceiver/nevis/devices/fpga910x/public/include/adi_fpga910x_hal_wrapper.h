/**
 * \file
 * \brief Contains ADI Transceiver Hardware Abstraction functions Analog Devices maintains and provides updates to this
 *        code layer. The end user should not modify this file or any code in this directory.
 *
 * Copyright (c) 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_HAL_WRAPPER_H_
#define _ADI_FPGA910X_HAL_WRAPPER_H_

#include "adi_fpga910x_hal_types.h"
#include "adi_fpga910x_hal.h"
#include "adi_fpga910x_ssi_types.h"
#include "adi_fpga910x_types.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ADI_FPGA910x_HAL_WRAPPER_H_
#define _ADI_FPGA910x_HAL_WRAPPER_H_
#endif

/**
 * \brief Performs a read of a single 32-bit register
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  addr      Address of the register to be read. Must be on a word boundary
 * \param[out] data      The value read from the register
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Register_Read(adi_fpga910x_Device_t *fpga910x, const uint32_t addr, uint32_t *data);

/**
 * \brief Performs a write of a single 32-bit register
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] addr      Address of the register to be written. Must be on a word boundary
 * \param[in] data      The value to write to the register
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Register_Write(adi_fpga910x_Device_t *fpga910x, uint32_t addr, uint32_t data);

/**
 * \brief Performs a RAM memory read
 *
 * \param[in]  fpga910x    Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  ramAddress  Address to begin the read from
 * \param[out] data        The array of values read from RAM. Must have enough space to store 'length' elements
 * \param[in]  length      The number of words to read starting from ramAddress
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Ram_Read(adi_fpga910x_Device_t *fpga910x,
                                          const uint32_t ramAddress,
                                          uint32_t data[],
                                          uint32_t length);

/**
 * \brief Performs a RAM memory write
 *
 * \param[in] fpga910x    Context variable - Pointer to the FPGA910X device data structure
 * \param[in] ramAddress  Address to begin writing to
 * \param[in] data        Array of values to write to RAM
 * \param[in] length      The number of words to write starting at ramAddress
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Ram_Write(adi_fpga910x_Device_t *fpga910x,
                                           const uint32_t ramAddress,
                                           uint32_t data[],
                                           uint32_t length);

/**
 * \brief Set the FPGA SSI type to CMOS or LVDS
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in] ssiType   The desired SSI type: CMOS or LVDS
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_SsiType_Set(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_SsiType_e ssiType);

/**
 * \brief Get the relative memory address where data for the specified DMA will be stored
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  dma       The DMA of interest
 * \param[out] addr      The returned address
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get(adi_fpga910x_Device_t *fpga910x,
                                                        adi_fpga910x_hal_Dma_e dma,
                                                        uint32_t *addr);

/**
 * \brief Get the current FPGA platform. ZC 706 and ZCU102 are the current supported platforms
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[out] platform  The current FPGA platform
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_CurrentPlatform_Get(adi_fpga910x_Device_t *fpga910x,
                                                     adi_fpga910x_Platform_e *platform);

/**
 * \brief Opens and configures the local UART for communication
 *
 * \param[in] fpga910x    Context variable - Pointer to the FPGA910X device data structure
 * \param[in] uartConfig  The configuration parameters for the UART
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Uart_Open(adi_fpga910x_Device_t *fpga910x, const adi_hal_uartCfg_t *uartConfig);

/**
 * \brief Closes and releases the local UART
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Uart_Close(adi_fpga910x_Device_t *fpga910x);

/**
 * \brief Read from the local UART
 *
 * \param[in]     fpga910x     Context variable - Pointer to the FPGA910X device data structure
 * \param[out]    rxData       The user supplied buffer to store rx bytes in
 * \param[in,out] rxByteCount  On entry caller must set to the number of bytes in rxData, on return set to the number of bytes received
 * \param[in]     timeOut_ms   The number of milliseconds to wait for data, 0 return immediately, 0xFFFFFFFF to block until data is received
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Uart_Read(adi_fpga910x_Device_t *fpga910x,
                                           uint8_t rxData[],
                                           uint32_t *rxByteCount,
                                           uint32_t timeOut_ms);

/**
 * \brief Writes to the local UART
 *
 * \param[in] fpga910x     Context variable - Pointer to the FPGA910X device data structure
 * \param[in] txData       The data to transmit over the UART
 * \param[in] txByteCount  The number of bytes in txData to transmit
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Uart_Write(adi_fpga910x_Device_t *fpga910x,
                                            const uint8_t txData[],
                                            uint32_t txByteCount);

/**
 * \brief Opens and configures the QSPI bus in standard (single bit) SPI mode as a master
 *
 * \param[in] fpga910x    Context variable - Pointer to the FPGA910X device data structure
 * \note Bit order must match the far end. Setting qspiConfig->lsbFirst reads SPICR bit 9 back and
 *       fails if this build of the core does not support LSB first, because a silent mismatch shows
 *       up as every byte being bit reversed rather than as a configuration error.
 * \note LSB first is only permitted in standard (single bit) SPI mode. The AXI Quad SPI core supports
 *       it for standard mode alone - dual and quad I/O transfers are MSB first only. This function
 *       always configures standard mode, so the constraint is satisfied by construction and
 *       qspiConfig->lsbFirst is always legal here. It becomes a real restriction only if this driver
 *       is ever extended to dual or quad mode, where lsbFirst would have to be rejected rather than
 *       passed through.
 *
 * \param[in] qspiConfig  The configuration parameters for the QSPI bus
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Qspi_Open(adi_fpga910x_Device_t *fpga910x, const adi_hal_qspiCfg_t *qspiConfig);

/**
 * \brief Closes and releases the QSPI bus
 *
 * \param[in] fpga910x  Context variable - Pointer to the FPGA910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Qspi_Close(adi_fpga910x_Device_t *fpga910x);

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
 * by a read. With it false the core deasserts slave select between words, which frames the call one
 * word at a time - a far end expecting a single continuous frame will not see one, and a command
 * and its response cannot be issued as one frame at all. Pass true unless the device at the far end
 * specifically wants per-word framing.
 *
 * \note No memory is allocated for the NULL txData case. The dummy bytes are clocked out of rxData
 *       itself, which is filled with them first, so rxData must be writable for the whole of
 *       rxByteCount.
 *
 * \note The current build transfers one byte per word, so these buffers have no alignment requirement.
 *       A build that widens it (FPGA910X_QSPI_DATA_WIDTH in fpga910x_qspi_external_ip.h) rejects unaligned
 *       buffers and reverses the byte order within each word; both are documented there.
 *
 * \param[in]  fpga910x     Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  txData       The data to transmit, or NULL to transmit the dummy pattern
 * \param[out] rxData       The user supplied buffer to store rx bytes in, must hold rxByteCount bytes
 * \param[in]  rxByteCount  The number of bytes to transfer
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Qspi_Read(adi_fpga910x_Device_t *fpga910x,
                                           const uint8_t txData[],
                                           uint8_t rxData[],
                                           uint32_t rxByteCount);

/**
 * \brief Writes to the QSPI bus, discarding the bytes clocked in
 *
 * Slave select framing is set at Qspi_Open, not here, by adi_hal_qspiCfg_t.manualSlaveSelect. With
 * that true, slave select is asserted for the whole call and released on return. With it false the
 * core deasserts slave select between words, so the call is framed one word at a time rather than
 * as one block.
 *
 * \note The current build transfers one byte per word, so txData has no alignment requirement. A build
 *       that widens it (FPGA910X_QSPI_DATA_WIDTH in fpga910x_qspi_external_ip.h) rejects unaligned buffers
 *       and reverses the byte order within each word; both are documented there.
 *
 * \param[in] fpga910x     Context variable - Pointer to the FPGA910X device data structure
 * \param[in] txData       The data to transmit over the QSPI bus
 * \param[in] txByteCount  The number of bytes in txData to transmit
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_hal_wrapper_Qspi_Write(adi_fpga910x_Device_t *fpga910x,
                                            const uint8_t txData[],
                                            uint32_t txByteCount);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_HAL_WRAPPER_H_ */
