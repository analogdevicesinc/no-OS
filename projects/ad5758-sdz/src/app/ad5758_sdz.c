/***************************************************************************//**
* @file ad5758-sdz/src/app/ad5758_sdz.c
* @brief Implementation of Main Function.
* @author SPopa (stefan.popa@analog.com)
********************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
