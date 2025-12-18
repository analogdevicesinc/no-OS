/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines data common to all examples.
 *   @author Aron Kis (kis.aron@analog.com)
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

#include "common_data.h"
#include "maxim_gpio.h"
#include "maxim_spi.h"

#include "adin1110.h"

struct max_uart_init_param uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct no_os_uart_init_param uart_ip = {
	.device_id = 0,
	.asynchronous_rx = false,
	.baud_rate = 115200,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &uart_extra_ip,
	.platform_ops = &max_uart_ops,
};

struct max_spi_init_param adin1110_spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

const struct no_os_gpio_init_param adin1110_rst_gpio_ip = {
	.port = 2,
	.number = 31,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &(struct max_gpio_init_param)
	{
		.vssel = 1
	},
};

const struct no_os_spi_init_param adin1110_spi_ip = {
	.device_id = 0,
	.max_speed_hz = 25000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = 0,
	.extra = &adin1110_spi_extra,
};

struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN2111,
	.comm_param = adin1110_spi_ip,
	.reset_param = adin1110_rst_gpio_ip,
	.mac_address = {0x00, 0x18, 0x80, 0x03, 0x25, 0x50},
	.append_crc = false,
};

struct no_os_gpio_init_param port2_cfg_0 = {
	.port = 2,
	.number = 6,
	.pull = NO_OS_PULL_DOWN,
	.platform_ops = &max_gpio_ops,
	.extra = &(struct max_gpio_init_param)
	{
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	},
};

struct no_os_gpio_init_param adin1110_spi_cfg_0 = {
	.port = 2,
	.number = 27,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &(struct max_gpio_init_param)
	{
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	},
};

int spi_cfg_0(const struct no_os_gpio_init_param adin1110_spi_cfg)
{
	int ret;
	struct no_os_gpio_desc *adin1110_spi_cfg0_desc;

	ret = no_os_gpio_get(&adin1110_spi_cfg0_desc, &adin1110_spi_cfg);
	if (ret) {
		pr_err("Error initializing SPI GPIO (%d)\n", ret);
		return ret;
	}

	ret = no_os_gpio_direction_output(adin1110_spi_cfg0_desc, NO_OS_GPIO_HIGH);
	if (ret) {
		pr_err("Error setting the SPI GPIO direction (%d)\n", ret);
		goto remove_gpio;
	}

	ret = no_os_gpio_set_value(adin1110_spi_cfg0_desc, NO_OS_GPIO_HIGH);
	if (ret)
		pr_err("Error setting the SPI GPIO value (%d)\n", ret);

remove_gpio:
	no_os_gpio_remove(adin1110_spi_cfg0_desc);

	return ret;
}

int port2_cfg(const struct no_os_gpio_init_param port2_gpio_cfg, int state)
{
	int ret;
	struct no_os_gpio_desc *port2_gpio_cfg_desc;

	ret = no_os_gpio_get(&port2_gpio_cfg_desc, &port2_gpio_cfg);
	if (ret) {
		pr_err("Error initializing port2 GPIO (%d)\n", ret);
		return ret;
	}

	ret = no_os_gpio_direction_output(port2_gpio_cfg_desc, state);
	if (ret) {
		pr_err("Error setting the port2 GPIO direction (%d)\n", ret);
		no_os_gpio_remove(port2_gpio_cfg_desc);
	}

	return ret;
}
