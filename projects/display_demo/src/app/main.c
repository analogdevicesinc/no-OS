/***************************************************************************//**
 *   @file   display_demo/src/app/main.c
 *   @brief  Implementation of Main Function.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "parameters.h"
#include "xparameters.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "ssd_1306.h"
#include "display.h"
#include "no_os_error.h"
#include "no_os_delay.h"

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	char msg[] = "Example of message here, max 16x4 chars (for 128x32 display)";
	struct display_dev *dev;
	int32_t ret;

	struct xil_spi_init_param spi_extra = {
		.type = SPI_PS,
		.flags = 0U
	};

	struct no_os_spi_init_param ssd_1306_spi_init_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = 10U,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &xil_spi_ops,
		.extra = &spi_extra
	};

	struct xil_gpio_init_param extra = {
		.type = GPIO_PS,
		.device_id = XPAR_PS7_GPIO_0_DEVICE_ID
	};

	struct no_os_gpio_init_param dc_pin = {
		.number = DC_PIN,
		.platform_ops = &xil_gpio_ops,
		.extra = &extra
	};
	struct no_os_gpio_init_param reset_pin = {
		.number = RST_PIN,
		.platform_ops = &xil_gpio_ops,
		.extra = &extra
	};

	struct no_os_gpio_desc *vbat;
	struct no_os_gpio_init_param vbat_pin = {
		.number = VBAT_PIN,
		.platform_ops = &xil_gpio_ops,
		.extra = &extra
	};

	struct no_os_gpio_desc *vdd;
	struct no_os_gpio_init_param vdd_pin = {
		.number = VDD_PIN,
		.platform_ops = &xil_gpio_ops,
		.extra = &extra
	};

	struct ssd_1306_extra ssd1306_extra = {
		.dc_pin_ip = &dc_pin,
		.reset_pin_ip = &reset_pin,
		.spi_ip = &ssd_1306_spi_init_param
	};

	struct display_init_param display_ip = {
		.cols_nb = 16U,
		.rows_nb = 4U,
		.controller_ops = &ssd1306_ops,
		.extra = &ssd1306_extra
	};

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	/* Turn VBAT and VDD on, Zedboard platform specific */
	ret = no_os_gpio_get(&vbat, &vbat_pin);
	if (ret != 0)
		return -1;

	ret = no_os_gpio_direction_output(vbat, VBAT_ON);
	if (ret != 0)
		return -1;

	ret = no_os_gpio_get(&vdd, &vdd_pin);
	if (ret != 0)
		return -1;

	ret = no_os_gpio_direction_output(vdd, VDD_ON);
	if (ret != 0)
		return -1;

	ret = display_init(&dev, &display_ip);
	if (ret != 0)
		return -1;

	ret = display_on(dev);
	if (ret != 0)
		return -1;

	ret = display_clear(dev);
	if (ret != 0)
		return -1;

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	return display_print_string(dev, msg, 0, 0);
}
