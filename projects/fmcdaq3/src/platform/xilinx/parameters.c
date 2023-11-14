/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of xilinx platform data used by iio_demo project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "parameters.h"

#ifdef IIO_SUPPORT
struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
	.type = UART_PL,
#else
	.type = UART_PS,
	.irq_id = UART_IRQ_ID
#endif
};
#endif

struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
	.type = SPI_PL,
#else
	.type = SPI_PS,
#endif
};

struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
	.type = GPIO_PL,
#else
	.type = GPIO_PS,
#endif
	.device_id = GPIO_DEVICE_ID
};

struct adxcvr_init ad9152_xcvr_init_param = {
	.name = "ad9152_xcvr",
	.base = XPAR_AXI_AD9152_XCVR_BASEADDR,
	.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
	.out_clk_sel = ADXCVR_REFCLK_DIV2,
	.lpm_enable = 1,
	.ref_rate_khz = 616500,
	.lane_rate_khz = 12330000,
};

struct adxcvr_init ad9680_xcvr_init_param = {
	.name = "ad9680_xcvr",
	.base = XPAR_AXI_AD9680_XCVR_BASEADDR,
	.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
	.out_clk_sel = ADXCVR_REFCLK_DIV2,
	.lpm_enable = 1,
	.ref_rate_khz = 616500,
	.lane_rate_khz = 12330000
};
