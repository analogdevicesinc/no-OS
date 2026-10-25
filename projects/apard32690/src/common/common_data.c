/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines data common to all examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "maxim_gpio.h"
#include "capi_dma.h"
#include "capi_spi.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_spi.h"

#if defined(CONFIG_APARD32690_ADIN1110_STANDALONE_EXAMPLE)
#include "adin1110.h"
#endif

#if defined(CONFIG_APARD32690_ECHO_SERVER_EXAMPLE)
#include "adin1110.h"
#include "lwip_socket.h"
#include "lwip_adin1110.h"
#endif

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

#if defined(CONFIG_APARD32690_ECHO_SERVER_EXAMPLE) || defined(CONFIG_APARD32690_ADIN1110_STANDALONE_EXAMPLE)

const struct no_os_gpio_init_param adin1110_rst_gpio_ip = {
	.port = 1,
	.number = 5,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &(struct max_gpio_init_param)
	{
		.vssel = 1
	},
};

static struct capi_dma_config adin1110_capi_dma_cfg = {
	.num_chans = 2,
	.ops = &max_capi_dma_ops,
};

static struct max_capi_spi_extra adin1110_capi_spi_extra = {
	.vssel = MAX_CAPI_GPIO_VSSEL_VDDIOH,
	.chip_select = 1U << 0,
};

static struct capi_spi_config adin1110_capi_spi_cfg = {
	.ops = &max_capi_spi_ops,
	.identifier = 4,
	.clk_freq_hz = 25000000,
	.extra = &adin1110_capi_spi_extra,
};

static struct capi_spi_device adin1110_capi_spi_dev = {
	.max_speed_hz = 25000000,
	.mode = CAPI_SPI_MODE_0,
	.native_cs = 0,
};

int adin1110_spi_init(struct adin1110_init_param *param)
{
	struct capi_spi_controller_handle *spi_ctrl = NULL;
	struct capi_dma_handle *dma = NULL;
	int ret;

	ret = capi_dma_init(&dma, &adin1110_capi_dma_cfg);
	if (ret)
		return ret;

	adin1110_capi_spi_cfg.dma_handle = dma;
	ret = capi_spi_init(&spi_ctrl, &adin1110_capi_spi_cfg);
	if (ret)
		return ret;

	adin1110_capi_spi_dev.controller = spi_ctrl;
	param->comm_desc = &adin1110_capi_spi_dev;

	return 0;
}

struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.reset_param = adin1110_rst_gpio_ip,
	.mac_address = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80},
	.append_crc = false,
};

#endif

#if defined(CONFIG_APARD32690_ECHO_SERVER_EXAMPLE)

struct lwip_network_param lwip_ip = {
	.platform_ops = &adin1110_lwip_ops,
	.mac_param = &adin1110_ip,
};
#endif
