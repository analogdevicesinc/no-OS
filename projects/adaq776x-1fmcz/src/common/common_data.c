/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by adaq776x-1fmcz examples.
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

#include <stdbool.h>
#include "common_data.h"
#include "spi_engine.h"
#include "no_os_spi.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#ifdef IIO_SUPPORT
#include "no_os_uart.h"
#endif

#ifdef IIO_SUPPORT
struct no_os_uart_init_param adaq776x1_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &platform_uart_param,
	.platform_ops = &xil_uart_ops,
};
#endif

struct ad77681_init_param adaq776x1_init_param = {
	.spi_eng_dev_init = {
		.chip_select = AD77681_SPI_CS,
		.max_speed_hz = 1000000,
		.mode = NO_OS_SPI_MODE_2,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,
	},
	.power_mode = AD77681_FAST,
	.mclk_div = AD77681_MCLK_DIV_8,
	.conv_mode = AD77681_CONV_CONTINUOUS,
	.diag_mux_sel = AD77681_POSITIVE_FS,
	.conv_diag_sel = false,
	.conv_len = AD77681_CONV_24BIT,
	.crc_sel = AD77681_CRC,
	.status_bit = 0,
	.VCM_out = AD77681_VCM_HALF_VCC,
	.AINn = AD77681_AINn_ENABLED,
	.AINp = AD77681_AINp_ENABLED,
	.REFn = AD77681_BUFn_ENABLED,
	.REFp = AD77681_BUFp_ENABLED,
	.filter = AD77681_FIR,
	.decimate = AD77681_SINC5_FIR_DECx32,
	.sinc3_osr = 0,
	.vref = AD77681_VOLTAGE_REF,
	.mclk = AD77681_MCLK,
	.sample_rate = AD77681_DEFAULT_SAMPLING_FREQ,
#if ADAQ77671_DEV
	.has_pga = false,
	.has_variable_aaf = true,
	.aaf_gain = AD7768_AAF_IN,
#elif ADAQ77681_DEV
	.has_variable_aaf = false,
	.has_pga = true,
	.num_pga_modes = ARRAY_SIZE(adaq7768_gains),
	.default_pga_mode = AD7768_PGA_GAIN_2,
	.pgia_mode2pin_offset = 6,
	.pga_gains = adaq7768_gains,
#elif ADAQ7769_DEV
	.has_variable_aaf = true,
	.has_pga = true,
	.num_pga_modes = ARRAY_SIZE(adaq7769_gains),
	.default_pga_mode = AD7769_PGA_GAIN_0,
	.pgia_mode2pin_offset = 0,
	.pga_gains = adaq7769_gains,
	.gain_mode = AD7768_PGA_GAIN_0,
	.aaf_gain = AD7768_AAF_IN3,
#endif
};
