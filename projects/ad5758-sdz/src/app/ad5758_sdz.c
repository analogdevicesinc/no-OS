/***************************************************************************//**
* @file ad5758-sdz/src/app/ad5758_sdz.c
* @brief Implementation of Main Function.
* @author SPopa (stefan.popa@analog.com)
********************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "ad5758.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "parameters.h"
#include "sleep.h"
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xparameters.h"

const struct xil_spi_init_param spi_extra = {
	.type = SPI_PS,
	.flags = NULL
};
const struct no_os_spi_init_param spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 10000,
	.chip_select = AD5758_SPI_CS,
	.mode = NO_OS_SPI_MODE_1,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = &xil_spi_ops,
	.extra = &spi_extra
};

const struct xil_gpio_init_param gpio_extra = {
	.type = GPIO_PS,
	.device_id = GPIO_DEVICE_ID
};
const struct no_os_gpio_init_param reset_ip = {
	.number = GPIO_DAC_RESET_N,
	.platform_ops = &xil_gpio_ops,
	.extra = &gpio_extra
};
const struct no_os_gpio_init_param ldac_ip = {
	.number = GPIO_DAC_LDAC_N,
	.platform_ops = &xil_gpio_ops,
	.extra = &gpio_extra
};

struct ad5758_init_param ad5758_default_init_param = {
	/* SPI */
	.spi_init = spi_ip,
	/* GPIO */
	.reset_n = reset_ip, 			// reset_n
	.ldac_n = ldac_ip, 			// ldac_n
	/* Device Settings */
	.crc_en = true, 			// crc_en
	.dc_dc_mode = DPC_VOLTAGE_MODE, 	// dc_dc_mode
	.clkout_config = CLKOUT_DISABLE,	// clkout_config
	.clkout_freq = CLKOUT_FREQ_500_KHZ,	// clkout_freq
	.dc_dc_ilimit = ILIMIT_200_mA,		// dc_dc_ilimit
	.output_range = RANGE_0V_10V,		// output_range
	.slew_rate_clk = SR_CLOCK_240_KHZ, 	// slew_rate_clk
};

int main()
{
	int32_t ret;
	struct ad5758_dev *dev;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	ret = ad5758_init(&dev, &ad5758_default_init_param);
	if(ret)
		return -1;

	/* Write mid-scale DAC code to the input register
	 * V1_OUT pin will have half the scale (5V) */
	ret = ad5758_dac_input_write(dev, 0x7FFF);
	if(ret)
		return -1;

	printf("Success\n");

	/* Disable the instruction cache. */
	Xil_DCacheDisable();
	/* Disable the data cache. */
	Xil_ICacheDisable();

	return 0;
}
