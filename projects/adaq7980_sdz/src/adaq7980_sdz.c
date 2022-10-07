/***************************************************************************//**
 * @file adaq7980_sdz.c
 * @brief Implementation of Main Function.
 * @author ADI
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xil_cache.h>
#include "parameters.h"
#include "adaq7980.h"
#include "no_os_pwm.h"
#include "axi_pwm_extra.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "no_os_error.h"

#define ADAQ7980_EVB_SAMPLE_NO 1000

int main()
{
	uint16_t buf[ADAQ7980_EVB_SAMPLE_NO] __attribute__ ((aligned));
	struct adaq7980_dev *dev;
	int32_t ret, i;

	struct spi_engine_offload_init_param spi_engine_offload_init_param = {
		.offload_config = OFFLOAD_RX_EN,
		.rx_dma_baseaddr = ADAQ7980_DMA_BASEADDR,
	};

	struct spi_engine_init_param spi_eng_init_param  = {
		.ref_clk_hz = 100000000,
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = ADAQ7980_SPI_ENGINE_BASEADDR,
		.cs_delay = 0,
		.data_width = 16,
	};

	struct axi_pwm_init_param axi_pwm_init = {
		.base_addr = AXI_PWMGEN_BASEADDR,
		.ref_clock_Hz = 100000000,
		.channel = 0,
	};

	struct no_os_pwm_init_param trigger_pwm_init = {
		.period_ns = 10000,		/* 100Khz */
		.duty_cycle_ns = 10,
		.polarity = NO_OS_PWM_POLARITY_HIGH,
		.extra = &axi_pwm_init,
	};

	struct xil_gpio_init_param gpio_extra_param = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS,
	};

	struct no_os_gpio_init_param adaq7980_pd_ldo = {
		.number = GPIO_0,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param adaq7980_ref_pd= {
		.number = GPIO_REF_PUB,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param adaq7980_rbuf_pd = {
		.number = GPIO_RBUF_PUB,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};

	struct no_os_spi_init_param spi_init = {
		.chip_select = SPI_ADAQ7980_CS,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_2,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,
	};

	struct adaq7980_init_param adaq7980_init_param = {
		.spi_init = &spi_init,
		.offload_init_param = &spi_engine_offload_init_param,
		.trigger_pwm_init = &trigger_pwm_init,
		.gpio_pd_ldo = &adaq7980_pd_ldo,
	};

	Xil_DCacheDisable();
	Xil_ICacheEnable();

	ret = adaq7980_setup(&dev, &adaq7980_init_param);
	if (ret < 0)
		return -1;

	while(1) {
		ret = ad7980_read_data(dev, buf, ADAQ7980_EVB_SAMPLE_NO);
		if (ret < 0)
			return -1;

		for (i = 0; i < ADAQ7980_EVB_SAMPLE_NO; i++)
			printf("ADC sample %"PRIu32" %"PRIu16" \n", i, buf[i]);
	}

	printf("Success\n\r");

	Xil_ICacheDisable();

	return 0;
}
