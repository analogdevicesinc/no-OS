/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of xilinx platform data used by ad463x_fmcz project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "parameters.h"
#include "clk_axi_clkgen.h"

struct xil_uart_init_param uart_extra_ip = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
	.type = UART_PL,
#else
	.type = UART_PS,
	.irq_id = UART_IRQ_ID
#endif
};


struct spi_engine_init_param spi_eng_init_param  = {
	.ref_clk_hz = 100000000,
	.type = SPI_ENGINE,
	.spi_engine_baseaddr = SPI_ENGINE_BASEADDR,
	.cs_delay = 0,
	.data_width = 32,
};

struct spi_engine_offload_init_param spi_engine_offload_init_param = {
	.offload_config = OFFLOAD_RX_EN,
	.rx_dma_baseaddr = DMA_BASEADDR,
};

struct axi_clkgen_init clkgen_init = {
	.name = "rx_clkgen",
	.base = RX_CLKGEN_BASEADDR,
	.parent_rate = 100000000,
};

struct axi_pwm_init_param axi_pwm_init_param = {
	.base_addr = AXI_PWMGEN_BASEADDR,
	.ref_clock_Hz = 100000000,
	.channel = 0
};

