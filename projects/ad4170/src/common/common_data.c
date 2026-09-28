/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by the AD4170 examples.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

struct no_os_uart_init_param ad4170_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = &uart_platform_ops,
	.extra = UART_EXTRA,
};

#define AD4170_SETUP_DEFAULT {					\
	.afe = {						\
		.ref_select = AD4170_REFIN_REFOUT,		\
		.bipolar = true,				\
		.pga_gain = AD4170_PGA_GAIN_1,			\
	},							\
	.filter = {						\
		.post_filter_sel = AD4170_POST_FILTER_NONE,	\
		.filter_type = AD4170_FILT_SINC5_AVG,		\
	},							\
	.filter_fs = AD4170_FILTER_FS,				\
	.gain = AD4170_GAIN_DEFAULT,				\
}

struct ad4170_init_param ad4170_ip = {
	.id = AD4170_DEV_ID,
	.spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED_HZ,
		.chip_select = GPIO_CS_PIN,
		.mode = NO_OS_SPI_MODE_3,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &spi_platform_ops,
		.extra = SPI_EXTRA,
	},
	.rdy_conv_timeout = AD4170_RDY_CONV_TIMEOUT,
	.config = {
		.clock_ctrl = {
			.clockdiv = AD4170_CLKDIVBY1,
			.clocksel = AD4170_INTERNAL_OSC,
		},
		.adc_ctrl = {
			.multi_data_reg_sel = false,
			.cont_read = AD4170_CONT_READ_OFF,
			.mode = AD4170_MODE_CONT,
		},
		.channel_en = AD4170_CHANNEL(0),
		.map = {
			[0] = { .ainp = AD4170_AIN0, .ainm = AD4170_AIN1 },
			[1] = { .ainp = AD4170_AIN2, .ainm = AD4170_AIN3 },
			[2] = { .ainp = AD4170_AIN4, .ainm = AD4170_AIN5 },
			[3] = { .ainp = AD4170_AIN6, .ainm = AD4170_AIN7 },
			[4] = { .ainp = AD4170_AIN8, .ainm = AD4170_AIN9 },
			[5] = { .ainp = AD4170_AIN10, .ainm = AD4170_AIN11 },
			[6] = { .ainp = AD4170_AIN12, .ainm = AD4170_AIN13 },
			[7] = { .ainp = AD4170_AIN14, .ainm = AD4170_AIN15 },
			[8] = { .ainp = AD4170_OPEN, .ainm = AD4170_OPEN },
			[9] = { .ainp = AD4170_OPEN, .ainm = AD4170_OPEN },
			[10] = { .ainp = AD4170_OPEN, .ainm = AD4170_OPEN },
			[11] = { .ainp = AD4170_OPEN, .ainm = AD4170_OPEN },
			[12] = { .ainp = AD4170_OPEN, .ainm = AD4170_OPEN },
			[13] = { .ainp = AD4170_OPEN, .ainm = AD4170_OPEN },
			[14] = { .ainp = AD4170_OPEN, .ainm = AD4170_OPEN },
			[15] = { .ainp = AD4170_OPEN, .ainm = AD4170_OPEN },
		},
		.setups = {
			[0] = AD4170_SETUP_DEFAULT,
			[1] = AD4170_SETUP_DEFAULT,
			[2] = AD4170_SETUP_DEFAULT,
			[3] = AD4170_SETUP_DEFAULT,
			[4] = AD4170_SETUP_DEFAULT,
			[5] = AD4170_SETUP_DEFAULT,
			[6] = AD4170_SETUP_DEFAULT,
			[7] = AD4170_SETUP_DEFAULT,
		},
		.ref_control = {
			.ref_en = true,
		},
		.fir_control = {
			.fir_length = 1,
		},
	},
};
