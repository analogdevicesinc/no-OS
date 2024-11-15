/***************************************************************************//**
 *   @file   stm32/stm32_xspi.h
 *   @brief  Header file for the stm32 xspi driver.
 *   @author Naga Himanshu Indraganti (naga.indraganti@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STM32_XSPI_H_
#define STM32_XSPI_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "stm32_hal.h"

/**
 * @struct stm32_xspi_command
 * @brief Structure holding the stm32 platform specific XSPI command parameters.
 */
struct stm32_xspi_command {
	/** Instruction */
	uint32_t Instruction;
	/** Instruction Mode */
	uint32_t InstructionMode;
	/** Instruction Width */
	uint32_t InstructionWidth;
	/** Address */
	uint32_t Address;
	/** Address Mode */
	uint32_t AddressMode;
	/** Address Width */
	uint32_t AddressWidth;
	/** Alternate Bytes */
	uint32_t AlternateBytes;
	/** Alternate Bytes Mode */
	uint32_t AlternateBytesMode;
	/** Alternate Bytes Width */
	uint32_t AlternateBytesWidth;
	/** Data Mode */
	uint32_t DataMode;
	/** Data Length */
	uint32_t DataLength;
	/** Dummy Cycles */
	uint32_t DummyCycles;
};

/**
 * @struct stm32_xspi_init_param
 * @brief Structure holding the initialization parameters for stm32 platform.
 * specific XSPI parameters.
 */
struct stm32_xspi_init_param {
	/** Default XSPI command */
	struct stm32_xspi_command *cmd;
	/** Get peripheral source clock function */
	uint32_t (*get_input_clock)(void);
	/** FIFO threshold */
	uint32_t fifo_threshold;
	/** Sample Shifting */
	bool sample_shifting;
	/** CS high time (in number of clock cycles) */
	uint32_t cs_high_time;
	/** DMA Initialization Parameters */
	struct no_os_dma_init_param *dma_init;
	/** DMA Channel Descriptor */
	struct stm32_dma_channel *dma_ch;
	uint32_t irq_num;
};

/**
 * @struct stm32_xspi_desc
 * @brief stm32 platform specific XSPI descriptor.
 */
struct stm32_xspi_desc {
	/** XSPI instance */
	XSPI_HandleTypeDef hxspi;
	/** XSPI command */
	void *command;
	/** XSPI input clock */
	uint32_t input_clock;
	/** FIFO threshold */
	uint32_t fifo_threshold;
	/** Sample Shifting */
	bool sample_shifting;
	/** CS high time (in number of clock cycles) */
	uint32_t cs_high_time;
	/** DMA Descriptor */
	struct no_os_dma_desc *dma_desc;
	/** DMA Channel Descriptor */
	struct no_os_dma_ch *dma_ch;
	/** XSPI DMA transfer completion indication */
	bool stm32_xspi_dma_done;
	/** User callback function */
	void (*stm32_xspi_dma_user_cb)(void *ctx);
	/** User callback function arguments */
	void *stm32_xspi_dma_user_ctx;
	/** DMA channel transfer */
	struct no_os_dma_xfer_desc *dma_ch_xfer;
};

/**
 * @brief stm32 specific XSPI platform ops structure
 */
extern const struct no_os_spi_platform_ops stm32_xspi_ops;

#endif // STM32_XSPI_H_
