/***************************************************************************//**
 *   @file   ad9265_fmc_125ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include "xil_cache.h"
#include "xparameters.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#include "ad9265.h"
#include "spi.h"
#include "spi_extra.h"
#include "parameters.h"
#include "error.h"

#ifdef IIO_SUPPORT
#include "app_iio.h"
#endif

#include "print_log.h"

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	int32_t status;

	/* Initialize SPI structures */
	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS,
	};

	struct spi_init_param ad9265_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 10000000u,
		.chip_select = 0,
		.mode = SPI_MODE_0,
		.extra = &xil_spi_param,
		.platform_ops = &xil_platform_ops,
	};

	/* ADC Core */
	struct axi_adc_init ad9265_core_param = {
		.name = "ad9265_core",
		.num_channels = 1,
		.base = RX_CORE_BASEADDR
	};
	struct axi_adc *ad9265_core;

	/* AXI DMAC */
	struct axi_dmac_init ad9265_dmac_param = {
		.name = "ad9265_dmac",
		.base = RX_DMA_BASEADDR,
		.direction = DMA_DEV_TO_MEM,
		.flags = 0
	};
	struct axi_dmac *ad9265_dmac;

	/* AD9265 */
	struct ad9265_init_param ad9265_param = {
		.spi_init = ad9265_spi_param
	};
	struct ad9265_dev *ad9265_device;

	status = axi_adc_init(&ad9265_core,  &ad9265_core_param);
	if (status != SUCCESS) {
		pr_err("axi_adc_init() error: %s\n", ad9265_core->name);
		return FAILURE;
	}

	status = axi_dmac_init(&ad9265_dmac, &ad9265_dmac_param);
	if (status != SUCCESS) {
		pr_err("axi_dmac_init() error: %s\n", ad9265_dmac->name);
		return FAILURE;
	}

	status = ad9265_setup(&ad9265_device, ad9265_param, *ad9265_core);
	if (status != SUCCESS) {
		pr_err("ad9265_setup() failed!\n");
		return FAILURE;
	}

	status = ad9265_testmode_set(ad9265_device, TESTMODE_ONE_ZERO_TOGGLE);
	if (status != SUCCESS) {
		pr_err("ad9265_testmode_set() TESTMODE_ONE_ZERO_TOGGLE failed!\n");
		return FAILURE;
	}

	status = axi_dmac_transfer(ad9265_dmac, ADC_DDR_BASEADDR, 16384 * 2);
	if (status != SUCCESS) {
		pr_err("axi_dmac_transfer() failed!\n");
		return FAILURE;
	}

	pr_info("Capture done.\n");

	status = ad9265_testmode_set(ad9265_device, TESTMODE_OFF);
	if (status != SUCCESS) {
		pr_err("ad9265_testmode_set() TESTMODE_OFF failed!\n");
		return FAILURE;
	}

#ifdef IIO_SUPPORT
	pr_info("The board accepts libiio clients connections through the serial backend.\n");

	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad9265_core,
		.rx_dmac = ad9265_dmac,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
	};

	return iio_app_start(&iio_axi_adc_init_par);
#endif

	pr_info("Done\n");

	return 0;
}
