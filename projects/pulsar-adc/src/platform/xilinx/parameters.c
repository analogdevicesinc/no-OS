/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of xilinx platform data used by pulsar-adc project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "parameters.h"

struct xil_uart_init_param uart_extra_ip = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
	.type = UART_PL,
#else
	.type = UART_PS,
	.irq_id = UART_IRQ_ID
#endif
};

struct axi_pwm_init_param pulsar_adc_axi_pwm_init = {
	.base_addr = AXI_PWMGEN_BASEADDR,
	.ref_clock_Hz = REFCLK_RATE,
	.channel = 0,
};

struct spi_engine_init_param spi_eng_init_param  = {
	.ref_clk_hz = SPI_ENG_REF_CLK_FREQ_HZ,
	.type = SPI_ENGINE,
	.spi_engine_baseaddr = SPI_ENGINE_BASEADDR,
	.cs_delay = 1,
	.sdo_idle_state = 1,
};
