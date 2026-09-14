/***************************************************************************//**
 *   @file   parameters.c
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#include "parameters.h"

struct max_spi_init_param ad5592r_spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

#if (TARGET_NUM == 32665)
struct max_gpio_init_param ad5592r_spi_ss_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct no_os_gpio_init_param ad5592r_spi_ss_ip = {
	.port = AD5592R_SPI_SS_PORT,
	.number = AD5592R_SPI_SS_NUMBER,
	.pull = AD5592R_SPI_SS_PULL,
	.platform_ops = AD5592R_SPI_SS_OPS,
	.extra = AD5592R_SPI_SS_EXTRA,
};
#endif

struct max_gpio_init_param led_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

#ifdef CONFIG_USB_UART_MAXIM
struct max_usb_uart_init_param uart_extra = {
	.vid = 0x0456,
	.pid = 0x8102,
};
#else
struct max_uart_init_param uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};
#endif

struct max_i2c_init_param ad5593r_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_i2c_init_param lm75_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
