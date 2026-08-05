/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of xilinx platform data used by pulsar-adc project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct xil_uart_init_param uart_extra_ip = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
	.type = UART_PL,
#ifdef SDT
	.base_addr = XPAR_XUARTLITE_0_BASEADDR,
#endif
#else
	.type = UART_PS,
	.irq_id = UART_IRQ_ID,
#ifdef SDT
	.base_addr = XPAR_XUARTPS_0_BASEADDR,
#endif
#endif
};

struct axi_pwm_init_param pulsar_adc_axi_pwm_init = {
	.base_addr = AXI_PWMGEN_BASEADDR,
	.ref_clock_Hz = REFCLK_RATE,
	.channel = 0,
};

struct spi_engine_init_param spi_eng_init_param  = {
	.ref_clk_hz = SPI_ENG_REF_CLK_FREQ_HZ,
	.spi_engine_baseaddr = SPI_ENGINE_BASEADDR,
	.cs_delay = 1,
	.sdo_idle_state = 1,
};
