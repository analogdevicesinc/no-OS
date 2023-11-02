/***************************************************************************//**
 *   @file   projects/fmcdaq2/src/app/fmcdaq2.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include <inttypes.h>
#include "app_config.h"
#include "parameters.h"
#ifndef ALTERA_PLATFORM
#include <xparameters.h>
#include <xil_printf.h>
#include <xil_cache.h>
#include "axi_adxcvr.h"
#else
#include "clk_altera_a10_fpll.h"
#include "altera_adxcvr.h"
#endif
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "ad9523.h"
#include "ad9680.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "axi_jesd204_tx.h"
#include "axi_jesd204_rx.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "iio_ad9680.h"
#include "xilinx_uart.h"
#endif

#include "no_os_print_log.h"
#include "no_os_alloc.h"
#include "jesd204.h"
#include "jesd204_clk.h"

struct fmcdaq2_dev {
	struct ad9523_dev *ad9523_device;
	struct ad9680_dev *ad9680_device;

	struct ad9523_channel_spec ad9523_channels[8];

	struct no_os_gpio_desc *gpio_clkd_sync;
	struct no_os_gpio_desc *gpio_dac_reset;
	struct no_os_gpio_desc *gpio_dac_txen;
	struct no_os_gpio_desc *gpio_adc_pd;

	struct adxcvr *ad9144_xcvr;
	struct adxcvr *ad9680_xcvr;

	struct axi_jesd204_rx *ad9680_jesd;

	struct axi_adc	*ad9680_core;

	struct axi_dmac *ad9680_dmac;
} fmcdaq2;

struct link_init_param {
	uint32_t	link_id;
	uint32_t	device_id;
	uint32_t	octets_per_frame;
	uint32_t	frames_per_multiframe;
	uint32_t	samples_per_converter_per_frame;
	uint32_t	high_density;
	uint8_t		scrambling;
	uint32_t	converter_resolution;
	uint32_t	bits_per_sample;
	uint32_t	converters_per_device;
	uint32_t	control_bits_per_sample;
	uint32_t	lanes_per_device;
	uint32_t	subclass;
	uint32_t	version;
	uint8_t		logical_lane_mapping[8];
	/* JTX */
	uint8_t		link_converter_select[16];
	/* JRX */
	uint32_t	tpl_phase_adjust;
};

struct jesd204_clk rx_jesd_clk = {0};
struct jesd204_clk tx_jesd_clk = {0};

struct fmcdaq2_init_param {
	struct ad9523_init_param ad9523_param;
	struct ad9680_init_param ad9680_param;

	struct adxcvr_init ad9144_xcvr_param;
	struct adxcvr_init ad9680_xcvr_param;

	struct jesd204_rx_init ad9680_jesd_param;

	struct axi_adc_init ad9680_core_param;

	struct axi_dmac_init ad9680_dmac_param;

	struct link_init_param	jrx_link_tx;
} fmcdaq2_init;

static int fmcdaq2_gpio_init(struct fmcdaq2_dev *dev)
{
	int status;

	/* Initialize GPIO structures */
	struct no_os_gpio_init_param gpio_clkd_sync_param = {
		.number = GPIO_CLKD_SYNC,
		.platform_ops = &xil_gpio_ops
	};
	struct no_os_gpio_init_param gpio_adc_pd_param = {
		.number = GPIO_ADC_PD,
		.platform_ops = &xil_gpio_ops
	};

	struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	gpio_clkd_sync_param.extra = &xil_gpio_param;
	gpio_adc_pd_param.extra = &xil_gpio_param;

	/* set GPIOs */
	status = no_os_gpio_get(&dev->gpio_clkd_sync, &gpio_clkd_sync_param);
	if (status < 0)
		return status;

	status = no_os_gpio_get(&dev->gpio_adc_pd, &gpio_adc_pd_param);
	if (status < 0)
		return status;

	status = no_os_gpio_direction_output(dev->gpio_clkd_sync, NO_OS_GPIO_LOW);
	if (status < 0)
		return status;

	status = no_os_gpio_direction_output(dev->gpio_adc_pd, NO_OS_GPIO_HIGH);
	if (status < 0)
		return status;

	no_os_mdelay(5);

	status = no_os_gpio_set_value(dev->gpio_clkd_sync, NO_OS_GPIO_HIGH);
	if (status < 0)
		return status;

	return no_os_gpio_set_value(dev->gpio_adc_pd, NO_OS_GPIO_LOW);
}

static int fmcdaq2_spi_init(struct fmcdaq2_init_param *dev_init)
{
	/* Initialize SPI structures */
	struct no_os_spi_init_param ad9523_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 2000000u,
		.chip_select = 0,
		.mode = NO_OS_SPI_MODE_0
	};
	struct no_os_spi_init_param ad9680_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 2000000u,
		.chip_select = 2,
		.mode = NO_OS_SPI_MODE_0
	};

	static struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
	};
	ad9523_spi_param.platform_ops = &xil_spi_ops;
	ad9523_spi_param.extra = &xil_spi_param;

	ad9680_spi_param.platform_ops = &xil_spi_ops;
	ad9680_spi_param.extra = &xil_spi_param;

	dev_init->ad9523_param.spi_init = ad9523_spi_param;
	dev_init->ad9680_param.spi_init = ad9680_spi_param;

	return 0;
}

static int fmcdaq2_clk_init(struct fmcdaq2_dev *dev,
			    struct fmcdaq2_init_param *dev_init)
{
	static struct ad9523_platform_data ad9523_pdata;
	int ret;

	/* clock distribution device (AD9523) configuration */
	ad9523_pdata.num_channels = 8;
	ad9523_pdata.channels = &dev->ad9523_channels[0];
	dev_init->ad9523_param.pdata = &ad9523_pdata;

	ret = ad9523_init(&dev_init->ad9523_param);
	if (ret < 0) {
		printf("\nClock init failed");
		return ret;
	}

	dev->ad9523_channels[DAC_DEVICE_CLK].channel_num = 1;
	dev->ad9523_channels[DAC_DEVICE_CLK].channel_divider = 1;
	dev->ad9523_channels[DAC_DEVICE_SYSREF].channel_num = 7;
	dev->ad9523_channels[DAC_DEVICE_SYSREF].channel_divider = 128;
	dev->ad9523_channels[DAC_FPGA_CLK].channel_num = 9;
	dev->ad9523_channels[DAC_FPGA_CLK].channel_divider = 2;
	dev->ad9523_channels[DAC_FPGA_SYSREF].channel_num = 8;
	dev->ad9523_channels[DAC_FPGA_SYSREF].channel_divider = 128;

	// adc device-clk-sysref, fpga-clk-sysref
	dev->ad9523_channels[ADC_DEVICE_CLK].channel_num = 13;
	dev->ad9523_channels[ADC_DEVICE_CLK].channel_divider = 1;
	dev->ad9523_channels[ADC_DEVICE_SYSREF].channel_num = 6;
	dev->ad9523_channels[ADC_DEVICE_SYSREF].channel_divider = 128;
	dev->ad9523_channels[ADC_FPGA_CLK].channel_num = 4;
	dev->ad9523_channels[ADC_FPGA_CLK].channel_divider = 2;
	dev->ad9523_channels[ADC_FPGA_SYSREF].channel_num = 5;
	dev->ad9523_channels[ADC_FPGA_SYSREF].channel_divider = 128;
	// VCXO 125MHz
	ad9523_pdata.vcxo_freq = 125000000;
	ad9523_pdata.spi3wire = 1;
	ad9523_pdata.osc_in_diff_en = 1;
	ad9523_pdata.pll2_charge_pump_current_nA = 413000;
	ad9523_pdata.pll2_freq_doubler_en = 0;
	ad9523_pdata.pll2_r2_div = 1;
	ad9523_pdata.pll2_ndiv_a_cnt = 0;
	ad9523_pdata.pll2_ndiv_b_cnt = 6;
	ad9523_pdata.pll2_vco_diff_m1 = 3;
	ad9523_pdata.pll2_vco_diff_m2 = 0;
	ad9523_pdata.rpole2 = 0;
	ad9523_pdata.rzero = 7;
	ad9523_pdata.cpole1 = 2;

	return ret;
}

static int fmcdaq2_jesd_init(struct fmcdaq2_init_param *dev_init)
{
	dev_init->ad9144_xcvr_param = (struct adxcvr_init) {
		.name = "ad9144_xcvr",
		.base = XPAR_AXI_AD9144_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK_DIV2,
		.lpm_enable = 1,
		.ref_rate_khz = 500000,
		.lane_rate_khz = 10000000,
		.export_no_os_clk = true
	};
	dev_init->ad9680_xcvr_param = (struct adxcvr_init) {
		.name = "ad9680_xcvr",
		.base = XPAR_AXI_AD9680_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_REFCLK_DIV2,
		.lpm_enable = 1,
		.ref_rate_khz = 500000,
		.lane_rate_khz = 10000000,
		.export_no_os_clk = true
	};

	dev_init->ad9680_jesd_param = (struct jesd204_rx_init) {
		.name = "ad9680_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.subclass = 1,
		.device_clk_khz = 10000000/40,
		.lane_clk_khz = 10000000
	};

	struct link_init_param jrx_link_tx = {
		.link_id = 1,
		.device_id = 0,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.samples_per_converter_per_frame = 1,
		.scrambling = 0,
		.high_density = 0,
		.converter_resolution = 14,
		.bits_per_sample = 16,
		.converters_per_device = 2,
		.control_bits_per_sample = 2,
		.lanes_per_device = 4,
		.subclass = 1,
		.version = 2,
	};

	fmcdaq2_init.jrx_link_tx = jrx_link_tx;

	return 0;
}

static int fmcdaq2_trasnceiver_setup(struct fmcdaq2_dev *dev,
				     struct fmcdaq2_init_param *dev_init)
{
	int status;

//	status = adxcvr_init(&dev->ad9144_xcvr, &dev_init->ad9144_xcvr_param);
//	if (status != 0) {
//		printf("error: %s: adxcvr_init() failed\n", dev_init->ad9144_xcvr_param.name);
//		return status;
//	}
//
//	status = adxcvr_clk_enable(dev->ad9144_xcvr);
//	if (status != 0) {
//		printf("error: %s: adxcvr_clk_enable() failed\n", dev->ad9144_xcvr->name);
//		return status;
//	}
//
//	tx_jesd_clk.xcvr = dev->ad9680_xcvr;

	status = adxcvr_init(&dev->ad9680_xcvr, &dev_init->ad9680_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", dev_init->ad9680_xcvr_param.name);
		return status;
	}

	status = adxcvr_clk_enable(dev->ad9680_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n",
		       dev_init->ad9680_xcvr_param.name);
		return status;
	}

	rx_jesd_clk.xcvr = dev->ad9680_xcvr;

	dev_init->ad9680_jesd_param.lane_clk = dev->ad9680_xcvr->clk_out;

	status = axi_jesd204_rx_init(&dev->ad9680_jesd,
				     &dev_init->ad9680_jesd_param);
	if (status) {
		printf("error: %s: axi_jesd204_rx_init() failed\n",
		       dev_init->ad9680_jesd_param.name);
		return status;
	}

	return status;
}

static int fmcdaq2_iio_init(struct fmcdaq2_dev *dev,
			    struct fmcdaq2_init_param *dev_init)
{
#ifdef IIO_SUPPORT

#ifndef ALTERA_PLATFORM
	Xil_DCacheDisable();
	Xil_ICacheEnable();
#endif
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

	int32_t status;
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_device *adc_dev_desc;
	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = fmcdaq2.ad9680_core,
		.rx_dmac = fmcdaq2.ad9680_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (NO_OS_IS_ERR_VALUE(status))
		return -1;
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);

	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if (NO_OS_IS_ERR_VALUE(status))
		return -1;
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);

	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc, adc_dev_desc,
			       &read_buff, NULL, NULL),
		IIO_APP_DEVICE("ad9680_dev", dev->ad9680_device,
			       &ad9680_iio_descriptor, NULL, NULL, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
#endif

	return 0;
}

static void fmcdaq2_remove(struct fmcdaq2_dev *dev)
{
	/* Memory deallocation for devices and spi */
	ad9523_remove(dev->ad9523_device);
	ad9680_remove(dev->ad9680_device);

	/* Memory deallocation for PHY and LINK layers */
	adxcvr_remove(dev->ad9144_xcvr);
	adxcvr_remove(dev->ad9680_xcvr);
	axi_jesd204_rx_remove(dev->ad9680_jesd);

	/* Memory deallocation for gpios */
	no_os_gpio_remove(dev->gpio_clkd_sync);
	no_os_gpio_remove(dev->gpio_adc_pd);
}

int fmcdaq2_reconfig(struct adxcvr_init *ad9144_xcvr_param,
		     struct ad9680_init_param *p_ad9680_param,
		     struct adxcvr_init *ad9680_xcvr_param,
		     struct ad9523_platform_data *p_ad9523_param)
{

	uint8_t mode = 0;

	printf ("Available sampling rates:\n");
	printf ("\t1 - ADC 1000 MSPS; DAC 1000 MSPS\n");
	printf ("\t2 - ADC  500 MSPS; DAC 1000 MSPS\n");
	printf ("\t3 - ADC  500 MSPS; DAC  500 MSPS\n");
	printf ("\t4 - ADC  600 MSPS; DAC  600 MSPS\n");
	printf ("\t5 - ADC 1000 MSPS; DAC 2000 MSPS (2x interpolation)\n");
	printf ("choose an option [default 1]:\n");

	mode = getc(stdin);

	switch (mode) {
	case '5':
		printf("5 - ADC 1000 MSPS; DAC 2000 MSPS (2x interpolation)\n");
		/* REF clock = 100 MHz */
		p_ad9523_param->channels[DAC_DEVICE_CLK].channel_divider = 10;

		/* Set SYSCLK_SEL to QPLL */
		ad9680_xcvr_param->sys_clk_sel = ADXCVR_SYS_CLK_QPLL0;
		break;
	case '4':
		printf ("4 - ADC  600 MSPS; DAC  600 MSPS\n");
		p_ad9523_param->pll2_vco_diff_m1 = 5;
		(&p_ad9523_param->channels[DAC_FPGA_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[DAC_DEVICE_CLK])->
		channel_divider = 1;
		(&p_ad9523_param->channels[DAC_DEVICE_SYSREF])->
		channel_divider = 128;
		(&p_ad9523_param->channels[DAC_FPGA_SYSREF])->
		channel_divider = 128;
		(&p_ad9523_param->channels[ADC_FPGA_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[ADC_DEVICE_CLK])->
		channel_divider = 1;
		(&p_ad9523_param->channels[ADC_DEVICE_SYSREF])->
		channel_divider = 128;
		(&p_ad9523_param->channels[ADC_FPGA_SYSREF])->
		channel_divider = 128;
		ad9144_xcvr_param->lane_rate_khz = 6000000;
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->ref_rate_khz = 300000;
#else
		ad9144_xcvr_param->parent_rate_khz = 300000;
#endif
		p_ad9680_param->lane_rate_kbps = 6000000;
		ad9680_xcvr_param->lane_rate_khz = 6000000;
		p_ad9680_param->sampling_frequency_hz = 600000000;
#ifndef ALTERA_PLATFORM
		ad9680_xcvr_param->ref_rate_khz = 300000;
#else
		ad9680_xcvr_param->parent_rate_khz = 300000;
#endif
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->lpm_enable = 0;
		ad9144_xcvr_param->sys_clk_sel = ADXCVR_SYS_CLK_CPLL;
		ad9144_xcvr_param->out_clk_sel = ADXCVR_REFCLK_DIV2;

		ad9680_xcvr_param->lpm_enable = 1;
		ad9680_xcvr_param->sys_clk_sel = ADXCVR_SYS_CLK_CPLL;
		ad9680_xcvr_param->out_clk_sel = ADXCVR_REFCLK_DIV2;
#endif
		break;
	case '3':
		printf ("3 - ADC  500 MSPS; DAC  500 MSPS\n");
		p_ad9523_param->pll2_vco_diff_m1 = 3;
		(&p_ad9523_param->channels[DAC_FPGA_CLK])->
		channel_divider = 4;
		(&p_ad9523_param->channels[DAC_DEVICE_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[DAC_DEVICE_SYSREF])->
		channel_divider = 256;
		(&p_ad9523_param->channels[DAC_FPGA_SYSREF])->
		channel_divider = 256;
		(&p_ad9523_param->channels[ADC_FPGA_CLK])->
		channel_divider = 4;
		(&p_ad9523_param->channels[ADC_DEVICE_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[ADC_DEVICE_SYSREF])->
		channel_divider = 256;
		(&p_ad9523_param->channels[ADC_FPGA_SYSREF])->
		channel_divider = 256;
		ad9144_xcvr_param->lane_rate_khz = 5000000;
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->ref_rate_khz = 250000;
#else
		ad9144_xcvr_param->parent_rate_khz = 250000;
#endif
		p_ad9680_param->lane_rate_kbps = 5000000;
		p_ad9680_param->sampling_frequency_hz = 500000000;
		ad9680_xcvr_param->lane_rate_khz = 5000000;
#ifndef ALTERA_PLATFORM
		ad9680_xcvr_param->ref_rate_khz = 250000;
#else
		ad9680_xcvr_param->parent_rate_khz = 250000;
#endif
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->sys_clk_sel = ADXCVR_SYS_CLK_CPLL;
		ad9144_xcvr_param->lpm_enable = 1;
		ad9144_xcvr_param->out_clk_sel = ADXCVR_REFCLK_DIV2;

		ad9680_xcvr_param->lpm_enable = 1;
		ad9680_xcvr_param->sys_clk_sel = ADXCVR_SYS_CLK_CPLL;
		ad9680_xcvr_param->out_clk_sel = ADXCVR_REFCLK_DIV2;
#endif
		break;
	case '2':
		printf ("2 - ADC  500 MSPS; DAC 1000 MSPS\n");
		p_ad9523_param->pll2_vco_diff_m1 = 3;
		(&p_ad9523_param->channels[DAC_FPGA_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[DAC_DEVICE_CLK])->
		channel_divider = 1;
		(&p_ad9523_param->channels[DAC_DEVICE_SYSREF])->
		channel_divider = 128;
		(&p_ad9523_param->channels[DAC_FPGA_SYSREF])->
		channel_divider = 128;
		(&p_ad9523_param->channels[ADC_FPGA_CLK])->
		channel_divider = 4;
		(&p_ad9523_param->channels[ADC_DEVICE_CLK])->
		channel_divider = 2;
		(&p_ad9523_param->channels[ADC_DEVICE_SYSREF])->
		channel_divider = 256;
		(&p_ad9523_param->channels[ADC_FPGA_SYSREF])->
		channel_divider = 256;
		ad9144_xcvr_param->lane_rate_khz = 10000000;
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->ref_rate_khz = 500000;
#else
		ad9144_xcvr_param->parent_rate_khz = 500000;
#endif
		p_ad9680_param->lane_rate_kbps = 5000000;
		p_ad9680_param->sampling_frequency_hz = 500000000;
		ad9680_xcvr_param->lane_rate_khz = 5000000;
#ifndef ALTERA_PLATFORM
		ad9680_xcvr_param->ref_rate_khz = 250000;
#else
		ad9680_xcvr_param->parent_rate_khz = 250000;
#endif
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->lpm_enable = 0;
		ad9144_xcvr_param->sys_clk_sel = ADXCVR_SYS_CLK_QPLL0;
		ad9144_xcvr_param->out_clk_sel = ADXCVR_REFCLK_DIV2;

		ad9680_xcvr_param->lpm_enable = 1;
		ad9680_xcvr_param->sys_clk_sel = ADXCVR_SYS_CLK_CPLL;
		ad9680_xcvr_param->out_clk_sel = ADXCVR_REFCLK_DIV2;
#endif
		break;
	default:
		printf ("1 - ADC 1000 MSPS; DAC 1000 MSPS\n");
		ad9680_xcvr_param->ref_rate_khz = 500000;
		/* Set SYSCLK_SEL to QPLL */
		ad9680_xcvr_param->sys_clk_sel = ADXCVR_SYS_CLK_QPLL0;
		break;
	}

	return(0);
}

static int fmcdaq2_setup(struct fmcdaq2_dev *dev,
			 struct fmcdaq2_init_param *dev_init)
{
	int status;

	status = fmcdaq2_gpio_init(dev);
	if (status < 0)
		return status;

	status = fmcdaq2_spi_init(dev_init);
	if (status < 0)
		return status;

	status = fmcdaq2_clk_init(dev, dev_init);
	if (status < 0)
		return status;

	status = fmcdaq2_jesd_init(dev_init);
	if (status < 0)
		return status;

	/* ADC Core */
	dev_init->ad9680_core_param = (struct axi_adc_init) {
		.name = "ad9680_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 2
	};

	dev_init->ad9680_dmac_param = (struct axi_dmac_init) {
		.name = "ad9680_dmac",
		.base = RX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};

	dev_init->ad9680_param.lane_rate_kbps = 10000000;
	dev_init->ad9680_param.dcm = 1; // from devicetree
	dev_init->ad9680_param.sampling_frequency_hz = 1000000000;
	dev_init->ad9680_param.sysref_mode = 1; // From devicetree

	/* change the default JESD configurations, if required */
	fmcdaq2_reconfig(&dev_init->ad9144_xcvr_param,
			 &dev_init->ad9680_param,
			 &dev_init->ad9680_xcvr_param,
			 dev_init->ad9523_param.pdata);

	/* Reconfigure the default JESD configurations */
	dev_init->ad9680_jesd_param.device_clk_khz =
		dev_init->ad9680_xcvr_param.lane_rate_khz / 40;
	dev_init->ad9680_jesd_param.lane_clk_khz =
		dev_init->ad9680_xcvr_param.lane_rate_khz;


	/* setup clocks */
	status = ad9523_setup(&dev->ad9523_device, &dev_init->ad9523_param);
	if (status != 0) {
		printf("error: ad9523_setup() failed\n");
		return status;
	}
	// Recommended DAC JESD204 link startup sequence
	//   1. FPGA JESD204 Link Layer
	//   2. FPGA JESD204 PHY Layer
	//   3. DAC
	//
	// Recommended ADC JESD204 link startup sequence
	//   1. ADC
	//   2. FPGA JESD204 PHY Layer
	//   2. FPGA JESD204 Link Layer
	//
	// Both sequences are interleaved here so that the transceivers which might
	// be shared between the DAC and ADC link are enabled at the same time.

	status = ad9680_setup_jesd_fsm(&dev->ad9680_device, &dev_init->ad9680_param);
	if (status) {
		printf("error: ad9680_setup_jesd_fsm() failed\n");
		return status;
	}

	dev->ad9680_device->jesd204_link.is_transmit = false;
	dev->ad9680_device->jesd204_link.link_id = fmcdaq2_init.jrx_link_tx.link_id;
	dev->ad9680_device->jesd204_link.bank_id = 0;
	dev->ad9680_device->jesd204_link.device_id = fmcdaq2_init.jrx_link_tx.device_id;
	dev->ad9680_device->jesd204_link.octets_per_frame =
		fmcdaq2_init.jrx_link_tx.octets_per_frame;
	dev->ad9680_device->jesd204_link.frames_per_multiframe =
		fmcdaq2_init.jrx_link_tx.frames_per_multiframe;
	dev->ad9680_device->jesd204_link.samples_per_conv_frame =
		fmcdaq2_init.jrx_link_tx.samples_per_converter_per_frame;
	dev->ad9680_device->jesd204_link.high_density =
		fmcdaq2_init.jrx_link_tx.high_density;
	dev->ad9680_device->jesd204_link.scrambling =
		fmcdaq2_init.jrx_link_tx.scrambling;
	dev->ad9680_device->jesd204_link.converter_resolution =
		fmcdaq2_init.jrx_link_tx.converter_resolution;
	dev->ad9680_device->jesd204_link.num_converters =
		fmcdaq2_init.jrx_link_tx.converters_per_device;
	dev->ad9680_device->jesd204_link.bits_per_sample =
		fmcdaq2_init.jrx_link_tx.bits_per_sample;
	dev->ad9680_device->jesd204_link.ctrl_bits_per_sample =
		fmcdaq2_init.jrx_link_tx.control_bits_per_sample;
	dev->ad9680_device->jesd204_link.num_lanes =
		fmcdaq2_init.jrx_link_tx.lanes_per_device;
	dev->ad9680_device->jesd204_link.subclass = fmcdaq2_init.jrx_link_tx.subclass;
	dev->ad9680_device->jesd204_link.jesd_version =
		fmcdaq2_init.jrx_link_tx.version;

	dev->ad9680_device->jesd204_link.sysref.lmfc_offset = 0; //devicetree

	status = fmcdaq2_trasnceiver_setup(&fmcdaq2, &fmcdaq2_init);
	if (status != 0)
		return status;

	status = axi_adc_init(&dev->ad9680_core,  &dev_init->ad9680_core_param);
	if (status != 0) {
		printf("axi_adc_init() error: %s\n", dev_init->ad9680_core_param.name);
		return status;
	}

	return status;
}

int main(void)
{
	unsigned int *data = (unsigned int *)ADC_DDR_BASEADDR;
	int status;

	status = fmcdaq2_setup(&fmcdaq2, &fmcdaq2_init);
	if (status < 0)
		return status;

	struct jesd204_topology *topology;//, *topology_tx;
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = fmcdaq2.ad9680_jesd->jdev,
			.link_ids = {1},
			.links_number = 1,
		},
		{
			.jdev = fmcdaq2.ad9680_device->jdev,
			.link_ids = {1},
			.links_number = 1,
			.is_top_device = true,
		},
	};

	jesd204_topology_init(&topology, devs,
			      sizeof(devs) / sizeof(*devs));

	jesd204_fsm_start(topology, JESD204_LINKS_ALL);

	status = axi_jesd204_rx_status_read(fmcdaq2.ad9680_jesd);
	if (status != 0) {
		printf("axi_jesd204_rx_status_read() error: %d\n", status);
	}

	status = axi_dmac_init(&fmcdaq2.ad9680_dmac, &fmcdaq2_init.ad9680_dmac_param);
	if (status)
		return status;

	struct axi_dma_transfer transfer_rx = {
		// Number of bytes to write/read
		.size = 1024 * sizeof(uint32_t),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
	};
	status = axi_dmac_transfer_start(fmcdaq2.ad9680_dmac, &transfer_rx);
	if(status)
		return status;
	status = axi_dmac_transfer_wait_completion(fmcdaq2.ad9680_dmac, 1000);
	if(status)
		return status;

	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR,
				  1024 * sizeof(uint32_t));

	printf("\ndaq2: setup and configuration is done\n");
	printf("\n SAMPLE NO. |     CH1     |     CH 2     |");
	for (unsigned int i = 0; i < 10; i++)
		printf("\n %4d       |    0x%04x   |    0x%04x    |",
		       i, (*(data + i) & 0xFFFF), (*(data + i) >> 16));

	status = fmcdaq2_iio_init(&fmcdaq2, &fmcdaq2_init);
	if (status != 0)
		return status;

	fmcdaq2_remove(&fmcdaq2);

	return 0;
}
