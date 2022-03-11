/***************************************************************************//**
* @file ad738x_fmc.c
* @brief Implementation of Main Function.
* @author SPopa (stefan.popa@analog.com)
* @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <xil_cache.h>
#include <xparameters.h>
#include "spi_engine.h"
#include "ad738x.h"
#include "parameters.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "app_config.h"

#include "no_os_print_log.h"

#define AD738x_EVB_SAMPLE_NO		1000

/** Main function */
int main()
{
	struct ad738x_dev *dev;
	uint16_t adc_data[2];
	int32_t ret;

	struct spi_engine_offload_init_param spi_engine_offload_init_param = {
		.offload_config = OFFLOAD_RX_EN,
		.rx_dma_baseaddr = AD738X_DMA_BASEADDR,
	};

	struct spi_engine_init_param spi_eng_init_param  = {
		.ref_clk_hz = 100000000,
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = AD738X_SPI_ENGINE_BASEADDR,
		.cs_delay = 0,
		.data_width = 16,
	};

	struct no_os_spi_init_param ad738x_spi_init = {
		.chip_select = AD738X_SPI_CS,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_1,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,
	};

	struct ad738x_init_param ad738x_default_init_param = {
		/* SPI engine*/
		.spi_param = &ad738x_spi_init,
		.offload_init_param = &spi_engine_offload_init_param,
		/* Configuration */
		.conv_mode = ONE_WIRE_MODE,
		.ref_sel = INT_REF,
		.dcache_invalidate_range =
		(void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange,
	};

#ifdef SPI_ENGINE_OFFLOAD_EXAMPLE
	uint32_t* buf = XPAR_PS7_DDR_0_S_AXI_BASEADDR;
	uint32_t i;
#endif

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ad738x_init(&dev, &ad738x_default_init_param);

#ifdef SPI_ENGINE_OFFLOAD_EXAMPLE
	/* Read data */
	while (true) {
		ret = ad738x_read_data(dev, buf, AD738x_EVB_SAMPLE_NO);
		if (ret != 0)
			return ret;
		for (i = 0; i < AD738x_EVB_SAMPLE_NO; i++) {
			pr_info("ADC0 sample: %lu : %lu \n", i, (uint16_t)buf[i]);
			pr_info("ADC1 sample: %lu : %lu \n", i, (uint16_t)(buf[i] >> 16));
		}
	}
#else
	while(true) {
		ad738x_spi_single_conversion(dev, adc_data);
		pr_info("ADC0: %d\n", adc_data[0]);
		if (dev->conv_mode == ONE_WIRE_MODE)
			pr_info("ADC1: %d\n", adc_data[1]);
	}
#endif

	ad738x_remove(dev);

	pr_info("Bye\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
