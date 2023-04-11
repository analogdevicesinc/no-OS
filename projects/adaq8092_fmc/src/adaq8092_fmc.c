/***************************************************************************//**
 *   @file   adaq8092_fmc.c
 *   @brief  ADAQ8092_FMC Application
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "adaq8092.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "xilinx_spi.h"
#include "parameters.h"
#include "no_os_error.h"
#include "xilinx_gpio.h"

#include "no_os_print_log.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "xilinx_uart.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADAQ8092_SAMPLES_PER_CH	1000
#define ADAQ8092_NUM_CH		2

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	int ret;
	uint16_t adc_buffer[ADAQ8092_SAMPLES_PER_CH * ADAQ8092_NUM_CH]
	__attribute__ ((aligned));

	struct xil_spi_init_param xil_spi_init = {
		.flags = 0,
		.type = SPI_PS
	};

	/* SPI */
	struct no_os_spi_init_param adaq8092_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz =  1000000u,
		.chip_select = 0,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_init
	};

	/* GPIO */
	struct xil_gpio_init_param xil_gpio_init = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS
	};

	struct no_os_gpio_init_param gpio_par_ser_init_param = {
		.number = GPIO_PAR_SER_NR,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_init
	};

	struct no_os_gpio_init_param gpio_adc_pd1_param = {
		.number = GPIO_PD1_NR,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_init
	};

	struct no_os_gpio_init_param gpio_adc_pd2_param = {
		.number = GPIO_PD2_NR,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_init
	};

	struct no_os_gpio_init_param gpio_en_1v8_param = {
		.number = GPIO_1V8_NR,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_init
	};

	/* ADC Core */
	struct axi_adc_init adaq8092_core_param = {
		.name = "adaq8092_core",
		.num_channels = 2,
		.base = RX_CORE_BASEADDR
	};
	struct axi_adc *adaq8092_core;

	/* AXI DMAC */
	struct axi_dmac_init adaq8092_dmac_param = {
		.name = "adaq8092_dmac",
		.base = RX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};
	struct axi_dmac *adaq8092_dmac;

	struct adaq8092_init_param adaq8092_init_param = {
		.spi_init = &adaq8092_spi_param,
		.gpio_adc_pd1_param = &gpio_adc_pd1_param,
		.gpio_adc_pd2_param = &gpio_adc_pd2_param,
		.gpio_en_1p8_param = &gpio_en_1v8_param,
		.gpio_par_ser_param = &gpio_par_ser_init_param,
		.pd_mode = ADAQ8092_NORMAL_OP,
		.clk_pol_mode = ADAQ8092_CLK_POL_INVERTED,
		.clk_phase_mode = ADAQ8092_NO_DELAY,
		.clk_dc_mode = ADAQ8092_CLK_DC_STABILIZER_OFF,
		.lvds_cur_mode = ADAQ8092_3M5A,
		.lvds_term_mode = ADAQ8092_TERM_OFF,
		.dout_en = ADAQ8092_DOUT_ON,
		.dout_mode = ADAQ8092_DOUBLE_RATE_LVDS,
		.test_mode = ADAQ8092_TEST_CHECKERBOARD,
		.alt_bit_pol_en = ADAQ8092_ALT_BIT_POL_OFF,
		.data_rand_en = ADAQ8092_DATA_RAND_OFF,
		.twos_comp = ADAQ8092_TWOS_COMPLEMENT
	};
	struct adaq8092_dev *adaq8092_device;

	ret = adaq8092_init(&adaq8092_device, adaq8092_init_param);
	if (ret) {
		pr_err("ADAQ8092 device initialization failed!");
		return ret;
	}

	ret = axi_adc_init(&adaq8092_core,  &adaq8092_core_param);
	if (ret) {
		pr_err("axi_adc_init() error: %s\n", adaq8092_core->name);
		return ret;
	}

	ret = axi_dmac_init(&adaq8092_dmac, &adaq8092_dmac_param);
	if (ret) {
		pr_err("axi_dmac_init() error: %s\n", adaq8092_dmac->name);
		return ret;
	}

	pr_info("Start Caputre with Test pattern - Checkerboard \n");

	struct axi_dma_transfer read_transfer = {
		// Number of bytes to write/read
		.size = sizeof(adc_buffer),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)adc_buffer
	};
	ret = axi_dmac_transfer_start(adaq8092_dmac, &read_transfer);
	if (ret) {
		pr_err("axi_dmac_transfer_start() failed!\n");
		return ret;
	}
	/* Wait until transfer finishes */
	ret = axi_dmac_transfer_wait_completion(adaq8092_dmac, 500);
	if(ret)
		return ret;
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffer, sizeof(adc_buffer));

	for (int i = 0; i < ADAQ8092_SAMPLES_PER_CH; i+=2)
		pr_info("CH1: %d CH2: %d \n",adc_buffer[i], adc_buffer[i+1]);

	ret = adaq8092_set_test_mode(adaq8092_device, ADAQ8092_TEST_OFF);
	if (ret)
		return ret;

	pr_info("\n Capture done.\n");

#ifdef IIO_SUPPORT
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

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_device *dev_desc;
	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = adaq8092_core,
		.rx_dmac = adaq8092_dmac,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
	};

	struct iio_data_buffer read_buff = {
		.buff = (void *)adc_buffer,
		.size = 0xFFFFFFFF,
	};

	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (ret < 0)
		return ret;
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &dev_desc);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adaq8092_dev", iio_axi_adc_desc, dev_desc,
			       &read_buff, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	iio_app_run(app);
#endif

	return 0;
}
