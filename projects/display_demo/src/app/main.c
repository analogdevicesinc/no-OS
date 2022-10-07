/***************************************************************************//**
 *   @file   display_demo/src/app/main.c
 *   @brief  Implementation of Main Function.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
