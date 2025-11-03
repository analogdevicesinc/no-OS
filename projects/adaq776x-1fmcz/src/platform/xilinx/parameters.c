/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of xilinx platform data used by adaq776x-1fmcz project.
 *   @author Celine Joy Capua (celinejoy.capua@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "parameters.h"
#include "clk_axi_clkgen.h"

struct spi_engine_init_param spi_eng_init_param  = {
	.type = SPI_ENGINE,
	.spi_engine_baseaddr = AD77681_SPI1_ENGINE_BASEADDR,
	.cs_delay = 0,
	.data_width = 32,
	.ref_clk_hz = AD77681_SPI_ENG_REF_CLK_FREQ_HZ,
};

struct spi_engine_offload_init_param spi_engine_offload_init_param = {
	.offload_config = (OFFLOAD_RX_EN | OFFLOAD_TX_EN),
	.rx_dma_baseaddr = AD77681_DMA_1_BASEADDR,
	.tx_dma_baseaddr = AD77681_DMA_1_BASEADDR
	};

struct axi_clkgen_init clkgen_init = {
	.name = "rx_clkgen",
	.base = XPAR_SPI_CLKGEN_BASEADDR,
	.parent_rate = 100000000,
};

#ifdef IIO_SUPPORT
struct xil_uart_init_param platform_uart_param = {
	.type = UART_PS,
	.irq_id = UART_IRQ_ID
};
#endif
