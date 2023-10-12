/***************************************************************************//**
 *   @file   fmcadc2.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author AntoniuMiclaus (antoniu.miclaus@analog.com)
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

#include <stdio.h>
#include <inttypes.h>
#include "parameters.h"
#include <xparameters.h>
#include <xil_printf.h>
#include <xil_cache.h>
#include "axi_adxcvr.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "ad9625.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#include "axi_jesd204_rx.h"
#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "xilinx_uart.h"
#endif

static int16_t adc_buff[ADC_MAX_SAMPLES] __attribute__((aligned));

int main(void)
{
	int32_t status;

	// SPI configuration
	struct no_os_spi_init_param ad9625_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 0,
		.platform_ops = &xil_spi_ops,
		.mode = NO_OS_SPI_MODE_0
	};

	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
	};
	ad9625_spi_param.device_id = SPI_DEVICE_ID;
	ad9625_spi_param.extra = &xil_spi_param;

	struct no_os_gpio_init_param gpio_sysref_param = {
		.number = GPIO_JESD204_SYSREF
	};

	struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	gpio_sysref_param.platform_ops = &xil_gpio_ops;
	gpio_sysref_param.extra = &xil_gpio_param;

	struct no_os_gpio_desc *gpio_sysref;

	struct adxcvr_init ad9625_xcvr_param = {
		.name = "ad9152_xcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_OUTCLK_PMA,
		.lpm_enable = 0,
		.ref_rate_khz = 625000,
		.lane_rate_khz = 6250000,
	};
	struct adxcvr	*ad9625_xcvr;

	struct jesd204_rx_init  ad9625_jesd_param = {
		.name = "ad9625_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.subclass = 1,
		.device_clk_khz = 6250000 / 40,
		.lane_clk_khz = 6250000
	};
	struct axi_jesd204_rx *ad9625_jesd;

	struct axi_adc_init ad9625_core_param = {
		.name = "ad9625_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 1
	};
	struct axi_adc	*ad9625_core;

	struct axi_dmac_init ad9625_dmac_param = {
		.name = "ad9625_dmac",
		.base = RX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};
	struct axi_dmac *ad9625_dmac;

	struct ad9625_init_param	ad9625_param;
	ad9625_param.spi_init = ad9625_spi_param;
	struct ad9625_dev		*ad9625_device;

	// ADC configuration
	ad9625_param.lane_rate_kbps = 6250000;
	ad9625_param.test_samples[0] = 0x5A5;
	ad9625_param.test_samples[1] = 0x1E1;
	ad9625_param.test_samples[2] = 0x777;
	ad9625_param.test_samples[3] = 0x444;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	// setup GPIOs
	no_os_gpio_get(&gpio_sysref,  &gpio_sysref_param);
	no_os_gpio_direction_output(gpio_sysref,  1);
	no_os_mdelay(10);

	// set up the device
	status = ad9625_setup(&ad9625_device, ad9625_param);
	if (status != 0) {
		printf("error: ad9625_setup() failed\n");
	}

	// set up the XCVRs
	status = adxcvr_init(&ad9625_xcvr, &ad9625_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", ad9625_xcvr->name);
	}
	status = adxcvr_clk_enable(ad9625_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", ad9625_xcvr->name);
	}

	// setup JESD core
	status = axi_jesd204_rx_init_legacy(&ad9625_jesd, &ad9625_jesd_param);
	if (status != 0) {
		printf("error: %s: axi_jesd204_rx_init_legacy() failed\n", ad9625_jesd->name);
	}
	status = axi_jesd204_rx_lane_clk_enable(ad9625_jesd);
	if (status != 0) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad9625_jesd->name);
	}

	// JESD core status
	status = axi_jesd204_rx_status_read(ad9625_jesd);
	if (status != 0) {
		printf("axi_jesd204_rx_status_read() error: %"PRIi32"\n", status);
	}
	// interface core setup
	status = axi_adc_init(&ad9625_core,  &ad9625_core_param);
	if (status != 0) {
		printf("axi_adc_init() error: %s\n", ad9625_core->name);
	}

	// PRBS test
	ad9625_test(ad9625_device, AD9625_TEST_PNLONG);
	if(axi_adc_pn_mon(ad9625_core, AXI_ADC_PN23, 10) == -1) {
		printf("%s PN23 sequence mismatch at ad9625!\n", __func__);
	};

	// capture data with DMA
	ad9625_test(ad9625_device, AD9625_TEST_OFF);

	axi_dmac_init(&ad9625_dmac, &ad9625_dmac_param);

	struct axi_dma_transfer read_transfer = {
		// Number of bytes to write/read
		.size = sizeof(adc_buff),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)adc_buff
	};
	axi_dmac_transfer_start(ad9625_dmac, &read_transfer);
	status = axi_dmac_transfer_wait_completion(ad9625_dmac, 500);
	if(status)
		return status;
	Xil_DCacheInvalidateRange((uintptr_t)adc_buff, sizeof(adc_buff));

#ifdef IIO_SUPPORT
	struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
#endif
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

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_device *adc_dev_desc;
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad9625_core,
		.rx_dmac = ad9625_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		return status;

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);
	if (status < 0)
		return status;

	struct iio_data_buffer read_buff = {
		.buff = (void *)adc_buff,
		.size = sizeof(adc_buff),
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("axi-ad9625", iio_axi_adc_desc, adc_dev_desc,
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

	printf("adc2: setup and configuration is done\n");

	ad9625_remove(ad9625_device);
	no_os_gpio_remove(gpio_sysref);

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	return(0);
}
