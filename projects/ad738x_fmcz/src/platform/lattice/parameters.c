/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Lattice platform data used by ad738x_fmcz project.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "parameters.h"

struct spi_engine_offload_init_param spi_engine_offload_init_param = {
	.offload_config = OFFLOAD_RX_EN,
	.rx_dma_baseaddr = DMA_BASEADDR,
};

struct latt_ip_uart_init_param uart_extra_ip = {
	.base_addr = LATTICE_UART_BASE_ADDR,
	.sys_clk = UART_REFCLK_RATE,
	.irq_priority = UART_IRQ_PRIORITY,
};

struct axi_pwm_init_param axi_pwm_init_param = {
	.base_addr = AXI_PWMGEN_BASEADDR,
	.ref_clock_Hz = AXI_PWM_REFCLK_RATE,
	.channel = 0
};

struct spi_engine_init_param spi_eng_init_param = {
	.ref_clk_hz = SPI_ENG_REFCLK_RATE,
	.spi_engine_baseaddr = SPI_ENGINE_BASEADDR,
	.cs_delay = 0,
	.data_width = 16,
};
