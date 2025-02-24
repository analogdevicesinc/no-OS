/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad9545 example.
 *   @author Jonathan Santos (jonathan.santos@analog.com)
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

#include "common_data.h"
#include "no_os_clk.h"
#include "ad9545.h"


#ifndef LINUX_PLATFORM
struct no_os_uart_init_param ad9545_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_2_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};
#endif

struct no_os_spi_init_param ad9545_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA
};

struct no_os_i2c_init_param ad9545_i2c_ip = {
	.device_id = 1,
	.max_speed_hz = 400000,
	.slave_address = 0x48,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA
};

struct ad9545_init_param ad9545_ip = {
	.spi_init = &ad9545_spi_ip,
	.i2c_init = &ad9545_i2c_ip,
	.comm_type = COMM_TYPE,
};


static int32_t refb_clk_recalc_rate(struct no_os_clk_desc *hw, uint64_t *rate)
{
	*rate = REFB_CLK_FREQUENCY;
	return 0;
}

static int32_t refbb_clk_recalc_rate(struct no_os_clk_desc *hw, uint64_t *rate)
{
	*rate = REFBB_CLK_FREQUENCY;
	return 0;
}

static int32_t refm1_clk_recalc_rate(struct no_os_clk_desc *hw, uint64_t *rate)
{
	*rate = REFM1_CLK_FREQUENCY;
	return 0;
}

const struct no_os_clk_platform_ops refb_clk_ops = {
	.clk_recalc_rate = refb_clk_recalc_rate,
};

const struct no_os_clk_platform_ops refbb_clk_ops = {
	.clk_recalc_rate = refbb_clk_recalc_rate,
};

const struct no_os_clk_platform_ops refm1_clk_ops = {
	.clk_recalc_rate = refm1_clk_recalc_rate,
};
