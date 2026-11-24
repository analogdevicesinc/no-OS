/**
 * \file
 * \brief Hardware abstraction layer (HAL) for interacting with the FPGA910X
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_HAL_TYPES_H_
#define _ADI_FPGA910X_HAL_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

/**
 * \brief DMA channels within the FPGA910X
 */
typedef enum adi_fpga910x_hal_Dma
{
    ADI_FPGA910X_HAL_DMA_RX1,
    ADI_FPGA910X_HAL_DMA_RX2,
    ADI_FPGA910X_HAL_DMA_TX,
    ADI_FPGA910X_HAL_DMA_ORX1,
    ADI_FPGA910X_HAL_DMA_ORX2,
    ADI_FPGA910X_HAL_DMA_SPI
} adi_fpga910x_hal_Dma_e;

/**
 * \brief Supported hardware platforms
 */
typedef enum adi_fpga910x_Platform
{
    ADI_FPGA910X_PLATFORM_UNSUPPORTED,
    ADI_FPGA910X_PLATFORM_ZC706,
    ADI_FPGA910X_PLATFORM_ZCU102
} adi_fpga910x_Platform_e;

/**
 * \brief UART configuration object for the linux_uio implementation of the FPGA910X HAL
 */
typedef struct adi_hal_uartCfg
{
    uint32_t baudRate;
    uint32_t dataBits;
    uint32_t stopBits;
    bool parityEnable;
    bool evenParity;
    bool hardwareFlowControl;
} adi_hal_uartCfg_t;

/**
 * \brief QSPI configuration object, for standard (single bit) SPI mode
 *
 * These are the SPI control register fields a caller can meaningfully choose, per the AXI Quad SPI
 * product guide (pg153) Table 6, SPICR at core base + 0x60 - plus the slave select register:
 *
 *   loopback           SPICR bit 0, LOOP
 *   clkActiveLow       SPICR bit 3, CPOL
 *   clkPhase1          SPICR bit 4, CPHA
 *   manualSlaveSelect  SPICR bit 7, Manual Slave Select Assertion Enable
 *   lsbFirst           SPICR bit 9, LSB First
 *   slaveSelect        SPISSR at core base + 0x70, not a control register field
 *
 * The other SPICR bits are deliberately not exposed, because none of them is a per-open choice: bit 1
 * (SPE) is the enable and belongs to Qspi_Open and Qspi_Close, bits 5 and 6 (TX / RX FIFO Reset) are
 * one-shot and self-clearing, and bit 8 (Master Transaction Inhibit) is driven around each frame by the
 * transfer itself.
 *
 * \note The transfer width is not here either. It is a Vivado generic (c_num_transfer_bits), not a
 *       register field - pg153 Table 6 has no width field - so there is nothing to write and nothing to
 *       read back. A caller able to set it could only disagree with the hardware, silently. It is a
 *       compile-time constant in the driver instead, FPGA910X_QSPI_DATA_WIDTH, which is where the
 *       alignment and byte-order consequences of widening it are documented.
 *
 * \note Master, SPICR bit 2, is fixed as master rather than exposed. Slave mode is not just the other
 *       value of that bit - the far end would drive SCK and slave select, so the transfer model here
 *       does not apply - and pg153 notes that loopback is master-mode only, that manual slave select
 *       has no effect on slave operation, and that standard slave mode is unsupported at an SCK ratio
 *       of 2. Exposing the bit without that work would let a caller select slave and still get master
 *       behaviour.
 *
 * \note The bus clock rate is not settable here. It is fixed by the SPI clock ratio chosen when the
 *       FPGA image was built.
 */
typedef struct adi_hal_qspiCfg
{
    uint8_t slaveSelect;     /*!< SPISSR. Zero-based index of the slave select line to assert. The
                              *   current FPGA image builds the core with one slave select line, so 0 is
                              *   the only value it accepts */
    bool clkActiveLow;       /*!< SPICR bit 3, CPOL. false: SCK idles low, true: SCK idles high */
    bool clkPhase1;          /*!< SPICR bit 4, CPHA. false: data valid on the first SCK edge after
                              *   slave select asserts, true: on the second */
    bool manualSlaveSelect;  /*!< SPICR bit 7. true: slave select is held for the whole transfer. false:
                              *   it toggles between words. Standard SPI mode only, per pg153.
                              *
                              *   This field is a precondition of the Qspi_Read and Qspi_Write
                              *   contracts, so it is not a free choice: the guarantee those two make
                              *   of slave select staying asserted for the whole call holds only while
                              *   this is true. Set it false and every call is framed one word at a
                              *   time instead, so a command and its response can no longer be issued
                              *   as a single frame - which is the usage those functions document.
                              *   Only set it false for a far end that specifically expects per-word
                              *   framing. See adi_fpga910x_hal_wrapper.h */
    bool loopback;           /*!< SPICR bit 0. true: internal loopback. The core ties its transmitter to its own
                              *   receiver, so no data leaves the FPGA and no external device is
                              *   needed. The pads are not involved, so this exercises the driver and
                              *   the core only, never the wiring or a slave */
    bool lsbFirst;           /*!< SPICR bit 9. Bit order within a byte on the wire. false: MSB first, the SPI
                              *   default, supported by every build of the core. true: LSB first,
                              *   which must match the device at the far end exactly or every byte
                              *   arrives bit reversed. Two limits apply. It is only permitted in
                              *   standard (single bit) SPI mode, which is the only mode Qspi_Open
                              *   configures - the AXI Quad SPI core is MSB first only for dual and
                              *   quad I/O. And it is not available in every build of the core, so
                              *   Qspi_Open reads the bit back and fails if this build ignored it,
                              *   rather than letting the mismatch look like data corruption */
} adi_hal_qspiCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_HAL_TYPES_H_ */
