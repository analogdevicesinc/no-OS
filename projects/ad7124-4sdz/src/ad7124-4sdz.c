/***************************************************************************//**
 *   @file   ad7124-4sdz.c
 *   @brief  ad7124 application example.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include <stdio.h>
#include <inttypes.h>
#include "ad7124.h"
#include "ad7124_regs.h"
#include "xilinx_spi.h"
#include "no_os_error.h"

int main(void)
{
	struct ad7124_dev	*dev;		/* A driver handle to pass around */
	enum ad7124_registers	reg_nr;		/* Variable to iterate through registers */
	uint32_t		timeout = 1000;	/* Number of tries before a function times out */
	int32_t			ret = 0;	/* Return value */
	int32_t			sample;		/* Stores raw value read from the ADC */

	static struct xil_spi_init_param xil_spi_init_params = {
		.type = SPI_PS,
	};

	struct no_os_spi_init_param spi_init = {
		.platform_ops = &xil_spi_ops,
		.chip_select = 0,
		.max_speed_hz = 1000000,
		.mode = NO_OS_SPI_MODE_3,
		.extra = &xil_spi_init_params,
	};

	struct ad7124_init_param ad7124_init_param = {
		.spi_init = &spi_init,
		.regs = ad7124_regs,
		.spi_rdy_poll_cnt = 25000,
	};

	/* Initialize AD7124 device. */
	ret = ad7124_setup(&dev, &ad7124_init_param);
	if (ret != 0)
		return ret;

	/* Read all registers */
	for (reg_nr = AD7124_Status; (reg_nr < AD7124_REG_NO) && !(ret < 0); reg_nr++) {
		ret = ad7124_read_register(dev, &ad7124_regs[reg_nr]);
		if (ret != 0)
			return ret;
	}

	while(1) {
		/* Read data from the ADC */
		ret = ad7124_wait_for_conv_ready(dev, timeout);
		if (ret != 0)
			return ret;

		ret = ad7124_read_data(dev, &sample);
		if (ret != 0)
			return ret;

		printf("ADC: %"PRIi32" \n", sample);
	}
	printf("Success\n\r");

	return 0;
}
