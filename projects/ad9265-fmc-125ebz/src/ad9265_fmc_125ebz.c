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
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "parameters.h"
#include "no_os_error.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "xilinx_uart.h"
#endif

#include "no_os_print_log.h"

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

	struct no_os_spi_init_param ad9265_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 10000000u,
		.chip_select = 0,
		.mode = NO_OS_SPI_MODE_0,
		.extra = &xil_spi_param,
		.platform_ops = &xil_spi_ops,
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
		.irq_option = IRQ_DISABLED
	};
	struct axi_dmac *ad9265_dmac;

	/* AD9265 */
	struct ad9265_init_param ad9265_param = {
		.spi_init = ad9265_spi_param
	};
	struct ad9265_dev *ad9265_device;

#ifdef XILINX_PLATFORM
	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();
#endif /* XILINX_PLATFORM */

	status = axi_adc_init(&ad9265_core,  &ad9265_core_param);
	if (status != 0) {
		pr_err("axi_adc_init() error: %s\n", ad9265_core->name);
		return -1;
	}

	status = axi_dmac_init(&ad9265_dmac, &ad9265_dmac_param);
	if (status != 0) {
		pr_err("axi_dmac_init() error: %s\n", ad9265_dmac->name);
		return -1;
	}

	status = ad9265_setup(&ad9265_device, ad9265_param, *ad9265_core);
	if (status != 0) {
		pr_err("ad9265_setup() failed!\n");
		return -1;
	}

	status = ad9265_testmode_set(ad9265_device, TESTMODE_ONE_ZERO_TOGGLE);
	if (status != 0) {
		pr_err("ad9265_testmode_set() TESTMODE_ONE_ZERO_TOGGLE failed!\n");
		return -1;
	}

	struct axi_dma_transfer read_transfer = {
		// Number of bytes to write/read
		.size = 16384 * 2,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
	};
	status = axi_dmac_transfer_start(ad9265_dmac, &read_transfer);
	if (status != 0) {
		pr_err("axi_dmac_transfer_start() failed!\n");
		return -1;
	}
	/* Wait until transfer finishes */
	status = axi_dmac_transfer_wait_completion(ad9265_dmac, 500);
	if(status < 0)
		return status;
	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR, 16384 * 2);

	pr_info("Capture done.\n");

	status = ad9265_testmode_set(ad9265_device, TESTMODE_OFF);
	if (status != 0) {
		pr_err("ad9265_testmode_set() TESTMODE_OFF failed!\n");
		return -1;
	}

#ifdef IIO_SUPPORT

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_device *dev_desc;
	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad9265_core,
		.rx_dmac = ad9265_dmac,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
	};

	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};

	struct xil_uart_init_param platform_uart_init_par = {
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
	};

	struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		return status;
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &dev_desc);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad9265_dev", iio_axi_adc_desc, dev_desc,
			       &read_buff, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
#endif

	pr_info("Done\n");

#ifdef XILINX_PLATFORM
	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();
#endif /* XILINX_PLATFORM */

	return 0;
}
