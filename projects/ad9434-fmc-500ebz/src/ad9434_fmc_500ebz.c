/***************************************************************************//**
 *   @file   ad9434_fmc_500ebz.c
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
#include "ad9434.h"
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
	uint8_t	nr_of_lanes = 12;
	uint8_t	over_range_signal = 1;

	/* Initialize SPI structures */
	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS,
	};

	struct spi_init_param ad9434_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 10000000u,
		.chip_select = 0,
		.mode = SPI_MODE_0,
		.extra = &xil_spi_param,
		.platform_ops = &xil_platform_ops,
	};

	/* ADC Core */
	struct axi_adc_init ad9434_core_param = {
		.name = "ad9434_core",
		.num_channels = 1,
		.base = RX_CORE_BASEADDR
	};
	struct axi_adc *ad9434_core;

	/* AXI DMAC */
	struct axi_dmac_init ad9434_dmac_param = {
		.name = "ad9434_dmac",
		.base = RX_DMA_BASEADDR,
		.direction = DMA_DEV_TO_MEM,
		.flags = 0
	};
	struct axi_dmac *ad9434_dmac;

	/* AD9434 */
	struct ad9434_init_param ad9434_param = {
		.spi_init = ad9434_spi_param
	};
	struct ad9434_dev *ad9434_device;

	status = ad9434_setup(&ad9434_device, ad9434_param);
	if (status != SUCCESS) {
		pr_info("ad9434_setup() failed!\n");
		return FAILURE;
	}

	status = axi_adc_init(&ad9434_core,  &ad9434_core_param);
	if (status != SUCCESS) {
		pr_info("axi_adc_init() error: %s\n", ad9434_core->name);
		return FAILURE;
	}

	status = axi_dmac_init(&ad9434_dmac, &ad9434_dmac_param);
	if (status != SUCCESS) {
		pr_info("axi_dmac_init() error: %s\n", ad9434_dmac->name);
		return FAILURE;
	}

	status = ad9434_testmode_set(ad9434_device, TESTMODE_PN9_SEQ);
	if (status != SUCCESS) {
		pr_info("ad9434_testmode_set() PN9_SEQ failed!");
		return FAILURE;
	}

	status = axi_adc_delay_calibrate(ad9434_core, nr_of_lanes + over_range_signal,
					 AXI_ADC_PN9);
	if (status != SUCCESS) {
		pr_info("axi_adc_delay_calibrate() failed!");
		return FAILURE;
	}

	status = ad9434_testmode_set(ad9434_device, TESTMODE_OFF);
	if (status != SUCCESS) {
		pr_info("ad9434_testmode_set() TESTMODE_OFF failed!");
		return FAILURE;
	}

	status = ad9434_outputmode_set(ad9434_device, OUTPUT_MODE_TWOS_COMPLEMENT);
	if (status != SUCCESS) {
		pr_info("ad9434_outputmode_set() OUTPUT_MODE_TWOS_COMPLEMENT failed!");
		return FAILURE;
	}

	status = axi_dmac_transfer(ad9434_dmac, ADC_DDR_BASEADDR, 16384 * 2);
	if (status != SUCCESS) {
		pr_info("axi_dmac_transfer() failed!");
		return FAILURE;
	}

#ifdef IIO_SUPPORT
	pr_info("The board accepts libiio clients connections through the serial backend.\n");

	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad9434_core,
		.rx_dmac = ad9434_dmac,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
	};

	return iio_app_start(&iio_axi_adc_init_par);
#endif

	pr_info("Capture done.\n");

	return SUCCESS;
}
