/***************************************************************************//**
 *   @file   app.c
 *   @brief  AD9083 application example.
 *   @author Cristian Pop (cristian.pop@analog.com)
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
#include <stdio.h>
#include "ad9083.h"
#include "inttypes.h"
#include "error.h"
#include "app_clocking.h"
#include "app_jesd.h"
#include "app_ad9083.h"
#include "print_log.h"
#include "parameters.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"

#ifdef IIO_SUPPORT
#include "iio_ad9083.h"
#include "iio_axi_adc.h"
#include "iio_app.h"
#ifdef XILINX_PLATFORM
#include <xil_cache.h>
#endif /* XILINX_PLATFORM */
#endif /* IIO_SUPPORT */

/**
 * @brief Main application.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int main(void)
{
	/* select configuration from uc_settings.c */
	uint8_t uc = 7;
	int32_t status = SUCCESS;
	struct axi_adc *rx_adc;
	struct axi_dmac *rx_dmac;
	struct app_ad9083 *app_ad9083;
	struct app_clocking *app_clocking;
	struct app_jesd *app_jesd;

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 16,
	};
	struct axi_dmac_init rx_dmac_init = {
		"rx_dmac",
		RX_DMA_BASEADDR,
		DMA_DEV_TO_MEM,
		0
	};

	struct app_clocking_init app_clocking_init_param = {
		.uc = uc,
		.lmfc_rate_hz = 3906250, /* LANERATE / (10 * K * F) */
	};

	struct app_jesd_init init_jesd_init_param = {
		.uc = uc,
		.lane_rate_khz = 10000000,
	};

	struct app_ad9083_init app_ad9083_init_param = {
		.uc = uc,
		.jesd_rx_clk = NULL,
	};

	pr_info("Hello\n");

	status = app_clocking_init(&app_clocking, &app_clocking_init_param);
	if (status != SUCCESS) {
		pr_err("error: %"PRId32" app_clock_init() \n", status);

		return FAILURE;
	}

	status = app_jesd_init(&app_jesd, &init_jesd_init_param);
	if (status != SUCCESS) {
		pr_err("error: %"PRId32" app_jesd_init() \n", status);

		return FAILURE;
	}

	app_ad9083_init_param.jesd_rx_clk = &app_jesd->jesd_rx_clk,

	status = app_ad9083_init(&app_ad9083, &app_ad9083_init_param);
	if (status != SUCCESS) {
		pr_err("error: %"PRId32" app_ad9083_init() \n", status);

		return FAILURE;
	}

	status = app_jesd_status(app_jesd);
	if (status != SUCCESS) {
		pr_err("error: %"PRIi32" jesd_status() \n", status);

		return FAILURE;
	}

	status = app_ad9083_status(app_ad9083);
	if (status != SUCCESS) {
		pr_err("error: %"PRIi32" app_ad9083_status()\n", status);

		return FAILURE;
	}

	status = axi_adc_init(&rx_adc, &rx_adc_init);
	if (status != SUCCESS)
		return FAILURE;

	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status != SUCCESS)
		return FAILURE;

#ifdef IIO_SUPPORT
	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_device *iio_adc_dev_desc;
	struct iio_axi_adc_init_param iio_axi_adc_init_par;

	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange,
	};

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		return status;

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &iio_adc_dev_desc);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc,
			       iio_adc_dev_desc, &read_buff, NULL),
		IIO_APP_DEVICE("ad9083", app_ad9083->ad9083_phy,
			       &ad9083_iio_descriptor, NULL, NULL)
	};

	status = iio_app_run(devices, 2);
	if (status != SUCCESS)
		pr_err("error: %"PRIi32" iio_app_run()\n", status);

	status = iio_axi_adc_remove(iio_axi_adc_desc);
	if (status != SUCCESS)
		pr_err("error: %"PRIi32" iio_axi_adc_remove()\n", status);
#else
	pr_info("Bye\n");
#endif
	status = axi_dmac_remove(rx_dmac);
	if (status != SUCCESS)
		pr_err("error: %"PRIi32" axi_dmac_remove()\n", status);

	status = axi_adc_remove(rx_adc);
	if (status != SUCCESS)
		pr_err("error: %"PRIi32" axi_adc_remove()\n", status);

	status = app_ad9083_remove(app_ad9083);
	if (status != SUCCESS)
		pr_err("error: %"PRIi32" app_ad9083_remove()\n", status);

	status = app_jesd_remove(app_jesd);
	if (status != SUCCESS)
		pr_err("error: %"PRIi32" app_jesd_remove()\n", status);

	status = app_clocking_remove(app_clocking);
	if (status != SUCCESS)
		pr_err("error: %"PRIi32" app_clocking_remove()\n", status);

	return status;
}
