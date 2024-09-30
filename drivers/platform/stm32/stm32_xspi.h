/***************************************************************************//**
 *   @file   stm32/stm32_xspi.h
 *   @brief  Header file for the stm32 xspi driver.
 *   @author Naga Himanshu Indraganti (naga.indraganti@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
} stm32_xspi_command;

/**
 * @struct stm32_xspi_init_param
 * @brief Structure holding the initialization parameters for stm32 platform.
 * specific XSPI parameters.
 */
struct stm32_xspi_init_param {
	/** Default XSPI command */
	stm32_xspi_command *cmd;
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
