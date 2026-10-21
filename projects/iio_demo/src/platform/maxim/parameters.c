/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by iio_demo project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param iio_demo_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS,
};

#ifdef CONFIG_USB_UART_MAXIM
struct max_usb_uart_init_param iio_demo_usb_uart_extra_ip = {
	.vid = 0x0B6B,
	.pid = 0x003C
};
#endif

#ifdef CONFIG_IIO_DEMO_LIBIIO_EXAMPLE
static struct max_uart_init_param libiio_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS,
};

struct no_os_uart_init_param libiio_uart_ip = {
	.device_id = 0,
	.baud_rate = 115200,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = &max_uart_ops,
	.extra = &libiio_uart_extra_ip,
};

#ifdef CONFIG_LIBIIO_TRANSPORT_NETWORK
#include "maxim_spi.h"
#include "maxim_gpio.h"
/*
 * app_config.h's ENABLE_LOOPBACK (the demo's DAC to ADC loopback) and lwIP's
 * (its loopback interface) share a name; lwIP's is the one used below.
 */
#undef ENABLE_LOOPBACK
#include "adin1110.h"
#include "lwip_adin1110.h"
#include "lwip_socket.h"

static struct max_spi_init_param libiio_adin_spi_extra_ip = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

static struct max_gpio_init_param libiio_adin_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

static struct adin1110_init_param libiio_adin_ip = {
	.chip_type = ADIN1110,
	.comm_param = {
		.device_id = 3,
		.max_speed_hz = 15000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.extra = &libiio_adin_spi_extra_ip,
	},
	.reset_param = {
		.port = 0,
		.number = 15,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &libiio_adin_gpio_extra_ip,
	},
	.mac_address = { 0x00, 0x18, 0x80, 0x03, 0x25, 0x60 },
	.append_crc = true,
};

struct lwip_network_param libiio_lwip_ip = {
	.hwaddr = { 0x00, 0x18, 0x80, 0x03, 0x25, 0x60 },
	.platform_ops = &adin1110_lwip_ops,
	.mac_param = &libiio_adin_ip,
};
#endif /* CONFIG_LIBIIO_TRANSPORT_NETWORK */
#endif /* CONFIG_IIO_DEMO_LIBIIO_EXAMPLE */
