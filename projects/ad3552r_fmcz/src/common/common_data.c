/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by iio examples.
 *   @author Angelo Dureghello (adureghello@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "ad3552r.h"
#include "axi_dmac.h"
#include "clk_axi_clkgen.h"
#include "axi_dac_core.h"
#ifdef IIO_SUPPORT
#include "no_os_uart.h"
#endif

struct axi_dac_init ad3552r_core_ip = {
	.name = "ad3552r_core",
	.base = TX_CORE_BASEADDR,
	.bus_type = AXI_DAC_BUS_TYPE_QSPI,
	.num_channels = 2,
};

struct axi_clkgen_init clkgen_ip = {
	.name = "tx_clkgen",
	.base = TX_CLKGEN_BASEADDR,
	.parent_rate = 100000000,
};

struct axi_dmac_init dmac_ip = {
	.name = "tx_dmac",
	.base = TX_DMA_BASEADDR,
	.irq_option = IRQ_DISABLED
};

struct no_os_gpio_init_param gpio_ldac_param = {
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
	.number = GPIO_OFFSET + GPIO_LDAC_N,
};

struct no_os_gpio_init_param gpio_reset_param = {
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
	.number = GPIO_OFFSET + GPIO_RESET_N,
};

struct no_os_gpio_init_param default_gpio_param = {
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

#ifdef IIO_SUPPORT
struct no_os_uart_init_param uart_init_param = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &platform_uart_param,
	.platform_ops = &xil_uart_ops,
};
#endif
