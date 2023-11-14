/***************************************************************************//**
 *   @file   projects/fmcdaq3/src/examples/fmcdaq3/fmcdaq3_example.c
 *   @brief  Main example for fmcdaq3 project
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author EBalas (eliza.balas@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "fmcdaq3_example.h"

struct fmcdaq3_init_param fmcdaq3_init;
struct fmcdaq3_dev fmcdaq3;

#ifdef JESD_FSM_ON
struct link_init_param {
	uint32_t	link_id;
	bool        is_transmit;
	uint8_t     device_id;
	uint8_t     bank_id;
	uint8_t     octets_per_frame;
	uint16_t     frames_per_multiframe;
	uint8_t     samples_per_conv_frame;
	uint8_t	    high_density;
	uint8_t		scrambling;
	uint8_t	    converter_resolution;
	uint8_t	    bits_per_sample;
	uint8_t	    num_converters;
	uint8_t	    ctrl_bits_per_sample;
	uint8_t	    num_lanes;
	uint8_t	    subclass;
	uint8_t     jesd_version;
	uint8_t jesd_encoder;
	uint8_t *lane_ids;
	struct jesd204_sysref sysref;
};

struct jesd204_clk rx_jesd_clk = {0};
struct jesd204_clk tx_jesd_clk = {0};

#endif

static int fmcdaq3_gpio_init(struct fmcdaq3_dev *dev)
{
	int status;

	/* Initialize GPIO structures */
	struct no_os_gpio_init_param gpio_dac_txen_param = {
		.number = GPIO_DAC_TXEN,
		.platform_ops = &xil_gpio_ops
	};
	struct no_os_gpio_init_param gpio_adc_pd_param = {
		.number = GPIO_ADC_PD,
		.platform_ops = &xil_gpio_ops
	};

#ifndef ALTERA_PLATFORM
	struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	gpio_dac_txen_param.extra = &xil_gpio_param;
	gpio_adc_pd_param.extra = &xil_gpio_param;
#else
	struct altera_gpio_init_param altera_gpio_param = {
		.base_address = GPIO_BASEADDR,
		.type = NIOS_II_GPIO,
		.device_id = GPIO_DEVICE_ID
	};
	gpio_dac_txen_param.extra = &altera_gpio_param;
	gpio_adc_pd_param.extra = &altera_gpio_param;
#endif
	/* set GPIOs */
	status = no_os_gpio_get(&dev->gpio_dac_txen, &gpio_dac_txen_param);
	if (status < 0)
		return status;
	status = no_os_gpio_get(&dev->gpio_adc_pd, &gpio_adc_pd_param);
	if (status < 0)
		return status;
//TODO: DIRECTION IS GOOD??
	status = no_os_gpio_direction_output(dev->gpio_dac_txen, NO_OS_GPIO_LOW);
	if (status < 0)
		return status;
	status = no_os_gpio_direction_output(dev->gpio_adc_pd, NO_OS_GPIO_HIGH);
	if (status < 0)
		return status;
	status = no_os_gpio_set_value(dev->gpio_dac_txen, NO_OS_GPIO_HIGH);
	if (status < 0)
		return status;
	return no_os_gpio_set_value(dev->gpio_adc_pd, NO_OS_GPIO_LOW);
}

static int fmcdaq3_spi_init(struct fmcdaq3_init_param *dev_init)
{
	/* Initialize SPI structures */
	struct no_os_spi_init_param ad9528_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 2000000u,
		.chip_select = 0,
		.mode = NO_OS_SPI_MODE_0
	};
	struct no_os_spi_init_param ad9152_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 2000000u,
		.chip_select = 1,
		.mode = NO_OS_SPI_MODE_0
	};
	struct no_os_spi_init_param ad9680_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 2000000u,
		.chip_select = 2,
		.mode = NO_OS_SPI_MODE_0
	};

#ifndef ALTERA_PLATFORM
	static struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
	};
	ad9528_spi_param.platform_ops = &xil_spi_ops;
	ad9528_spi_param.extra = &xil_spi_param;

	ad9152_spi_param.platform_ops = &xil_spi_ops;
	ad9152_spi_param.extra = &xil_spi_param;

	ad9680_spi_param.platform_ops = &xil_spi_ops;
	ad9680_spi_param.extra = &xil_spi_param;
#else
	struct altera_spi_init_param altera_spi_param = {
		.type = NIOS_II_SPI,
		.base_address = SYS_SPI_BASE
	};
	ad9528_spi_param.platform_ops = &altera_spi_ops;
	ad9528_spi_param.extra = &altera_spi_param;

	ad9152_spi_param.platform_ops = &altera_spi_ops;
	ad9152_spi_param.extra = &altera_spi_param;

	ad9680_spi_param.platform_ops = &altera_spi_ops;
	ad9680_spi_param.extra = &altera_spi_param;
#endif

	dev_init->ad9528_param.spi_init = ad9528_spi_param;
	dev_init->ad9152_param.spi_init = ad9152_spi_param;
	dev_init->ad9680_param.spi_init = ad9680_spi_param;

	return 0;
}

static int fmcdaq3_clk_init(struct fmcdaq3_dev *dev,
			    struct fmcdaq3_init_param *dev_init)
{
	static struct ad9528_platform_data ad9528_pdata;
	int ret;

	/* clock distribution device (ad9528) configuration */
	ad9528_pdata.num_channels = 8;
	ad9528_pdata.channels = &dev->ad9528_channels[0];
	dev_init->ad9528_param.pdata = &ad9528_pdata;

	ret = ad9528_init(&dev_init->ad9528_param);
	if (ret < 0) {
		printf("\nClock init failed");
		return ret;
	}

	// dac-device-clock (1.233G)
	dev->ad9528_channels[0].channel_num = 2;
	dev->ad9528_channels[0].channel_divider = 1;

	//adc sysref (4.9M)
	dev->ad9528_channels[1].channel_num = 8;
	dev->ad9528_channels[1].channel_divider = 256;

	// adc-fpga-clock (616.5M)
	dev->ad9528_channels[2].channel_num = 9;
	dev->ad9528_channels[2].channel_divider = 2;

	// adc dev sysref (4.9M)
	dev->ad9528_channels[3].channel_num = 7;
	dev->ad9528_channels[3].channel_divider = 256;

	// adc-device-clock (1.233G)
	dev->ad9528_channels[4].channel_num = 13;
	dev->ad9528_channels[4].channel_divider = 1;

	// dac sysref (4.9M)
	dev->ad9528_channels[5].channel_num = 5;
	dev->ad9528_channels[5].channel_divider = 256;

	// dac-fpga-fmc (616.5M)
	dev->ad9528_channels[6].channel_num = 4;
	dev->ad9528_channels[6].channel_divider = 2;

	// dac dev sysref (4.9M)
	dev->ad9528_channels[7].channel_num = 6;
	dev->ad9528_channels[7].channel_divider = 256;

	// pllx settings
	ad9528_pdata.spi3wire = 1;
	ad9528_pdata.vcxo_freq = 100000000;
	ad9528_pdata.osc_in_diff_en = 1;
	ad9528_pdata.pll2_charge_pump_current_nA = 35000;
	ad9528_pdata.pll2_vco_div_m1 = 3;
	ad9528_pdata.pll2_r1_div = 3;
	ad9528_pdata.pll2_ndiv_a_cnt = 3;
	ad9528_pdata.pll2_ndiv_b_cnt = 27;
	ad9528_pdata.pll2_n2_div = 37;
	ad9528_pdata.sysref_k_div = 128;
	ad9528_pdata.rpole2 = RPOLE2_900_OHM;
	ad9528_pdata.rzero = RZERO_1850_OHM;
	ad9528_pdata.cpole1 = CPOLE1_16_PF;
	ad9528_pdata.sysref_pattern_mode = SYSREF_PATTERN_CONTINUOUS;
	ad9528_pdata.sysref_nshot_mode = SYSREF_NSHOT_4_PULSES;
	ad9528_pdata.sysref_req_en = true;
	ad9528_pdata.pll1_bypass_en = true;
	ad9528_pdata.pll2_bypass_en = false;

	return ret;
}

static int fmcdaq3_jesd_init(struct fmcdaq3_init_param *dev_init)
{
#ifndef ALTERA_PLATFORM
	dev_init->ad9152_xcvr_param = (struct adxcvr_init) {
		.name = "ad9152_xcvr",
		.base = XPAR_AXI_AD9152_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK_DIV2,
		.lpm_enable = 1,
		.ref_rate_khz = 616500,
		.lane_rate_khz = 12330000,
	};
	dev_init->ad9680_xcvr_param = (struct adxcvr_init) {
		.name = "ad9680_xcvr",
		.base = XPAR_AXI_AD9680_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_REFCLK_DIV2,
		.lpm_enable = 1,
		.ref_rate_khz = 616500,
		.lane_rate_khz = 12330000
	};
#else
	dev_init->ad9152_xcvr_param = (struct adxcvr_init) {
		.name = "ad9152_xcvr",
		.base = TX_XCVR_BASEADDR,
		.adxcfg_base = {TX_ADXCFG_0_BASEADDR,
				TX_ADXCFG_1_BASEADDR,
				TX_ADXCFG_2_BASEADDR,
				TX_ADXCFG_3_BASEADDR},
		.atx_pll_base = TX_PLL_BASEADDR,
		.lane_rate_khz = 12330000,
		.parent_rate_khz = 616500,
	};
	dev_init->ad9680_xcvr_param = (struct adxcvr_init) {
		.name = "ad9680_xcvr",
		.base = RX_XCVR_BASEADDR,
		.adxcfg_base = {RX_ADXCFG_0_BASEADDR,
				RX_ADXCFG_1_BASEADDR,
				RX_ADXCFG_2_BASEADDR,
				RX_ADXCFG_3_BASEADDR},
		.atx_pll_base = RX_PLL_BASEADDR,
		.parent_rate_khz = 616500,
		.lane_rate_khz = 12330000
	};
#endif
	/* JESD initialization */
	dev_init->ad9152_jesd_param = (struct jesd204_tx_init) {
		.name = "ad9152_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.converters_per_device = 2,
		.converter_resolution = 16,
		.bits_per_sample = 16,
		.high_density = false,
		.control_bits_per_sample = 0,
		.subclass = 1,
		.device_clk_khz = 12330000 / 40,
		.lane_clk_khz = 12330000
	};
	dev_init->ad9680_jesd_param = (struct jesd204_rx_init) {
		.name = "ad9680_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.subclass = 1,
		.device_clk_khz = 12330000 / 40,
		.lane_clk_khz = 12330000
	};

#ifdef JESD_FSM_ON
	dev_init->jrx_link_tx = (struct link_init_param) {
		.link_id = 1,
		.device_id = 0,
		.bank_id = 0,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.samples_per_conv_frame = 1,
		.scrambling = 0,
		.high_density = 0,
		.converter_resolution = 16,
		.bits_per_sample = 16,
		.num_converters = 2,
		.ctrl_bits_per_sample = 0,
		.num_lanes = 4,
		.subclass = 1,
		.jesd_version = 1,
		.sysref.mode = JESD204_SYSREF_CONTINUOUS,  // From devicetree ??todo
		.sysref.lmfc_offset = 0,
	};
	dev_init->jtx_link_rx = (struct link_init_param) {
		.link_id = 2,
		.device_id = 0,
		.bank_id = 0,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.samples_per_conv_frame = 1,
		.scrambling = 1,
		.high_density = 1,
		.converter_resolution = 16,
		.bits_per_sample = 16,
		.num_converters = 2,
		.ctrl_bits_per_sample = 0,
		.num_lanes = 4,
		.subclass = 1,
		.jesd_version = 1,
		.sysref.capture_falling_edge = 1,
//		.sysref.mode = JESD204_SYSREF_ONESHOT,
		.sysref.mode = JESD204_SYSREF_CONTINUOUS,
		.sysref.lmfc_offset = 0,
	};
#endif

	return 0;
}

static int fmcdaq3_altera_pll_setup()
{
#ifdef ALTERA_PLATFORM
	int status;
	struct altera_a10_fpll *ad9680_device_clk_pll;
	struct altera_a10_fpll *ad9152_device_clk_pll;
	struct altera_a10_fpll_init ad9680_device_clk_pll_param = {
		.name = "ad9680_device_clk_pll",
		.base = RX_A10_FPLL_BASEADDR,
		.parent_rate = 616500 * 1000
	};
	struct altera_a10_fpll_init ad9152_device_clk_pll_param = {
		.name = "ad9152_device_clk_pll",
		.base = TX_A10_FPLL_BASEADDR,
		.parent_rate = 616500 * 1000
	};

	/* Initialize A10 FPLLs */
	status = altera_a10_fpll_init(&ad9680_device_clk_pll,
				      &ad9680_device_clk_pll_param);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       ad9680_device_clk_pll_param.name);
	}
	status = altera_a10_fpll_init(&ad9152_device_clk_pll,
				      &ad9152_device_clk_pll_param);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       ad9152_device_clk_pll_param.name);
	}

	altera_a10_fpll_disable(ad9680_device_clk_pll);
	status = altera_a10_fpll_set_rate(ad9680_device_clk_pll,
			fmcdaq3_init.ad9680_jesd_param.device_clk_khz * 1000);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       ad9680_device_clk_pll->name);
	}
	altera_a10_fpll_enable(ad9680_device_clk_pll);
	altera_a10_fpll_disable(ad9152_device_clk_pll);
	status = altera_a10_fpll_set_rate(ad9152_device_clk_pll,
			fmcdaq3_init.ad9152_jesd_param.device_clk_khz * 1000);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       ad9152_device_clk_pll->name);
	}
	altera_a10_fpll_enable(ad9152_device_clk_pll);

	return status;
#endif
	return 0;
}

static int fmcdaq3_test(struct fmcdaq3_dev *dev,
			struct fmcdaq3_init_param *dev_init)
{
	int status;

	status = axi_jesd204_rx_status_read(dev->ad9680_jesd);
	if (status != 0) {
		printf("axi_jesd204_rx_status_read() error: %d\n", status);
	}

	status = axi_jesd204_tx_status_read(dev->ad9152_jesd);
	if (status != 0) {
		printf("axi_jesd204_tx_status_read() error: %d\n", status);
	}

	status = ad9152_status(dev->ad9152_device);
	if (status == -1)
		return status;

	/* transport path testing */
	dev->ad9152_channels[0].sel = AXI_DAC_DATA_SEL_SED;
	dev->ad9152_channels[1].sel = AXI_DAC_DATA_SEL_SED;
	axi_dac_data_setup(dev->ad9152_core);
	ad9152_short_pattern_test(dev->ad9152_device, &dev_init->ad9152_param);

	/* PN7 data path test */
	dev->ad9152_channels[0].sel = AXI_DAC_DATA_SEL_PN23;
	dev->ad9152_channels[1].sel = AXI_DAC_DATA_SEL_PN23;
	axi_dac_data_setup(dev->ad9152_core);
	dev_init->ad9152_param.prbs_type = AD9152_TEST_PN7;
	ad9152_datapath_prbs_test(dev->ad9152_device, &dev_init->ad9152_param);

	/* PN15 data path test */
	dev->ad9152_channels[0].sel = AXI_DAC_DATA_SEL_PN31;
	dev->ad9152_channels[1].sel = AXI_DAC_DATA_SEL_PN31;
	axi_dac_data_setup(dev->ad9152_core);
	dev_init->ad9152_param.prbs_type = AD9152_TEST_PN15;
	ad9152_datapath_prbs_test(dev->ad9152_device, &dev_init->ad9152_param);

	/* receive path testing */
	ad9680_test(dev->ad9680_device, AD9680_TEST_PN9);
	if(axi_adc_pn_mon(dev->ad9680_core, AXI_ADC_PN9, 10) == -1) {
		printf("%s ad9680 - PN9 sequence mismatch!\n", __func__);
	};
	ad9680_test(dev->ad9680_device, AD9680_TEST_PN23);
	if(axi_adc_pn_mon(dev->ad9680_core, AXI_ADC_PN23A, 10) == -1) {
		printf("%s ad9680 - PN23 sequence mismatch!\n", __func__);
	};
	ad9680_test(dev->ad9680_device, AD9680_TEST_OFF);

	return 0;
}
#ifndef IIO_SUPPORT_OFF
static int fmcdaq3_iio_init(struct fmcdaq3_dev *dev,
			    struct fmcdaq3_init_param *dev_init)
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

	dev_init->ad9152_dmac_param = (struct axi_dmac_init ) {
		.name = "ad9152_dmac",
		.base = TX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};
	axi_dmac_init(&dev->ad9152_dmac, &dev_init->ad9152_dmac_param);

	int32_t status;
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_axi_dac_desc *iio_axi_dac_desc;
	struct iio_device *adc_dev_desc, *dac_dev_desc;
	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = fmcdaq3.ad9680_core,
		.rx_dmac = fmcdaq3.ad9680_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};
	struct iio_axi_dac_init_param iio_axi_dac_init_par;
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = fmcdaq3.ad9152_core,
		.tx_dmac = fmcdaq3.ad9152_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange
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

	static struct iio_data_buffer write_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("axi_adc_ad9680", iio_axi_adc_desc, adc_dev_desc,
			       &read_buff, NULL, NULL),
		IIO_APP_DEVICE("axi_dac_ad9152", iio_axi_dac_desc, dac_dev_desc,
			       NULL, &write_buff, NULL),
		IIO_APP_DEVICE("ad9680_dev", dev->ad9680_device,
			       &ad9680_iio_descriptor, NULL, NULL, NULL),
		IIO_APP_DEVICE("ad9152_dev", dev->ad9152_device,
			       &ad9152_iio_descriptor, NULL, NULL, NULL)
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
#endif

static void fmcdaq3_remove(struct fmcdaq3_dev *dev)
{
	/* Memory deallocation for devices and spi */
	ad9152_remove(dev->ad9152_device);
	ad9528_remove(dev->ad9528_device);
	ad9680_remove(dev->ad9680_device);

	/* Memory deallocation for PHY and LINK layers */
	adxcvr_remove(dev->ad9152_xcvr);
	adxcvr_remove(dev->ad9680_xcvr);
	axi_jesd204_tx_remove(dev->ad9152_jesd);
	axi_jesd204_rx_remove(dev->ad9680_jesd);

	/* Memory deallocation for gpios */
	no_os_gpio_remove(dev->gpio_dac_txen);
	no_os_gpio_remove(dev->gpio_adc_pd);
}

int fmcdaq3_reconfig(struct ad9152_init_param *ad9152_param,
		     struct adxcvr_init *ad9152_xcvr_param,
		     struct ad9680_init_param *ad9680_param,
		     struct adxcvr_init *ad9680_xcvr_param,
		     struct ad9528_platform_data *ad9528_param)
{
	uint8_t mode = 0;

	printf ("Available sampling rates:\n");
	printf ("\t1 - ADC 1233 MSPS; DAC 1233 MSPS\n");
	printf ("\t2 - ADC 616.5 MSPS; DAC 616.5 MSPS\n");

//	mode = getc(stdin);

	switch (mode) {
	case '2':
		printf ("2 - ADC 616.5 MSPS; DAC 616.5 MSPS\n");
		ad9680_param->lane_rate_kbps = 6165000;
		ad9152_param->sampling_frequency_khz = 616500;
		ad9152_xcvr_param->lane_rate_khz = 6165000;
#ifndef ALTERA_PLATFORM
		ad9152_xcvr_param->ref_rate_khz = 308250;
#else
		ad9152_xcvr_param->parent_rate_khz = 308250;
#endif
		ad9680_xcvr_param->lane_rate_khz = 6165000;
		ad9680_param->sampling_frequency_hz = 616500000;
#ifndef ALTERA_PLATFORM
		ad9680_xcvr_param->ref_rate_khz = 308250;
#else
		ad9680_xcvr_param->parent_rate_khz = 308250;
#endif
		(&ad9528_param->channels[0])->channel_divider = 2;
		(&ad9528_param->channels[2])->channel_divider = 4;
		(&ad9528_param->channels[4])->channel_divider = 2;
		(&ad9528_param->channels[6])->channel_divider = 4;
		break;
	default:
		printf ("1 - ADC 1233 MSPS; DAC 1233 MSPS\n");
		break;
	}

	return(0);
}

#ifdef JESD_FSM_ON
static int fmcdaq3_setup_link_params( struct jesd204_link *dst,
		const struct link_init_param *src)
{
	dst->link_id = src->link_id;
	dst->is_transmit = src->is_transmit;
	dst->num_lanes = src->num_lanes;
	dst->num_converters = src->num_converters;
	dst->octets_per_frame = src->octets_per_frame;
	dst->frames_per_multiframe = src->frames_per_multiframe;
	dst->bits_per_sample = src->bits_per_sample;
	dst->samples_per_conv_frame = src->samples_per_conv_frame;
	dst->converter_resolution = src->converter_resolution;
	dst->jesd_version = src->jesd_version;
	dst->subclass = src->subclass;
	dst->device_id = src->device_id;
	dst->bank_id = src->bank_id;
	dst->scrambling = src->scrambling;
	dst->high_density = src->high_density;
	dst->ctrl_bits_per_sample = src->ctrl_bits_per_sample;
	dst->jesd_encoder = src->jesd_encoder;
	dst->sysref.mode = src->sysref.mode;
	dst->sysref.capture_falling_edge = src->sysref.capture_falling_edge;
	dst->sysref.valid_falling_edge = src->sysref.valid_falling_edge;
	dst->sysref.lmfc_offset = src->sysref.lmfc_offset;

	dst->lane_ids = no_os_calloc(src->num_lanes, sizeof(uint8_t));
	if (!dst->lane_ids)
		return -ENOMEM;
	for (int lane = 0; lane < src->num_lanes; lane++)
		dst->lane_ids[lane]= lane;

	return 0;
}

static int fmcdaq3_setup_fsm(struct fmcdaq3_dev *dev,
			 struct fmcdaq3_init_param *dev_init,
			 struct jesd204_clk *rx_jesd_clk,
			 struct jesd204_clk *tx_jesd_clk)
{
	int status;

	// Recommended DAC JESD204 link startup sequence
	//   1. FPGA JESD204 Link Layer
	//   2. FPGA JESD204 PHY Layer
	//   3. DAC
	//
	// Recommended ADC JESD204 link startup sequence
	//   1. ADC
	//   2. FPGA JESD204 PHY Layer
	//   3. FPGA JESD204 Link Layer
	//
	// Both sequences are interleaved here so that the transceivers which might
	// be shared between the DAC and ADC link are enabled at the same time.

	status = ad9680_setup_jesd_fsm(&dev->ad9680_device, &dev_init->ad9680_param);
	if (status) {
		printf("error: ad9680_setup_jesd_fsm() failed\n");
		return status;
	}
	fmcdaq3_setup_link_params(&dev->ad9680_device->jesd204_link, &dev_init->jrx_link_tx);

	status = adxcvr_init(&dev->ad9152_xcvr, &dev_init->ad9152_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", dev_init->ad9152_xcvr_param.name);
		return status;
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(dev->ad9152_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", dev->ad9152_xcvr->name);
		return status;
	}
#endif
	tx_jesd_clk->xcvr = dev->ad9680_xcvr;
	dev_init->ad9152_jesd_param.lane_clk.dev_desc = &tx_jesd_clk->xcvr;
	dev_init->ad9152_jesd_param.lane_clk.hw_ch_num = 1;
	dev_init->ad9152_jesd_param.lane_clk.name = dev->ad9152_xcvr->name;
	dev_init->ad9152_jesd_param.lane_clk.platform_ops = &jesd204_clk_ops;

	status = axi_jesd204_tx_init_jesd_fsm(&dev->ad9152_jesd,
						  &dev_init->ad9152_jesd_param);
	if (status) {
		printf("error: %s: axi_jesd204_tx_init_jesd_fsm() failed\n",
			   dev_init->ad9152_jesd_param.name);
		return status;
	}
	status = adxcvr_init(&dev->ad9680_xcvr, &dev_init->ad9680_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", dev_init->ad9680_xcvr_param.name);
		return status;
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(dev->ad9680_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n",
		       dev_init->ad9680_xcvr_param.name);
		return status;
	}
#endif

	rx_jesd_clk->xcvr = dev->ad9680_xcvr;
	dev_init->ad9680_jesd_param.lane_clk.dev_desc = &rx_jesd_clk->xcvr;
	dev_init->ad9680_jesd_param.lane_clk.hw_ch_num = 1;
	dev_init->ad9680_jesd_param.lane_clk.name = dev->ad9680_xcvr->name;
	dev_init->ad9680_jesd_param.lane_clk.platform_ops = &jesd204_clk_ops;

	status = axi_jesd204_rx_init_jesd_fsm(&dev->ad9680_jesd,
						  &dev_init->ad9680_jesd_param);
	if (status) {
		printf("error: %s: axi_jesd204_rx_init_jesd_fsm() failed\n",
			   dev_init->ad9680_jesd_param.name);
		return status;
	}

	status = ad9152_setup_jesd_fsm(&dev->ad9152_device, &dev_init->ad9152_param);
	if (status) {
		printf("error: ad9152_setup_jesd_fsm() failed\n");
		return status;
	}

	fmcdaq3_setup_link_params(&dev->ad9152_device->link_config, &dev_init->jtx_link_rx);

	return status;
}
#else
static int fmcdaq3_setup_legacy(struct fmcdaq3_dev *dev,
			 struct fmcdaq3_init_param *dev_init)
{
	int status;

	// Recommended DAC JESD204 link startup sequence
	//   1. FPGA JESD204 Link Layer
	//   2. FPGA JESD204 PHY Layer
	//   3. DAC
	//
	// Recommended ADC JESD204 link startup sequence
	//   1. ADC
	//   2. FPGA JESD204 PHY Layer
	//   3. FPGA JESD204 Link Layer
	//
	// Both sequences are interleaved here so that the transceivers which might
	// be shared between the DAC and ADC link are enabled at the same time.

	status = ad9680_setup(&dev->ad9680_device, &dev_init->ad9680_param);
	if (status != 0) {
		printf("error: ad9680_setup() failed\n");
		return status;
	}
	status = adxcvr_init(&dev->ad9152_xcvr, &dev_init->ad9152_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", dev_init->ad9152_xcvr_param.name);
		return status;
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(dev->ad9152_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", dev->ad9152_xcvr->name);
		return status;
	}
#endif
	status = axi_jesd204_tx_init(&dev->ad9152_jesd, &dev_init->ad9152_jesd_param);
	if (status != 0) {
		printf("error: %s: axi_jesd204_tx_init() failed\n",
			   dev_init->ad9152_jesd_param.name);
		return status;
	}

	status = axi_jesd204_tx_lane_clk_enable(dev->ad9152_jesd);
	if (status != 0) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
			   dev->ad9152_jesd->name);
		return status;
	}
	status = adxcvr_init(&dev->ad9680_xcvr, &dev_init->ad9680_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", dev_init->ad9680_xcvr_param.name);
		return status;
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(dev->ad9680_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n",
		       dev_init->ad9680_xcvr_param.name);
		return status;
	}
#endif
	status = axi_jesd204_rx_init(&dev->ad9680_jesd, &dev_init->ad9680_jesd_param);
	if (status != 0) {
		printf("error: %s: axi_jesd204_rx_init() failed\n",
			   dev_init->ad9680_jesd_param.name);
		return status;
	}

	status = axi_jesd204_rx_lane_clk_enable(dev->ad9680_jesd);
	if (status != 0) {
		printf("error: %s: axi_jesd204_rx_lane_clk_enable() failed\n",
			   dev->ad9680_jesd->name);
		return status;
	}
	status = ad9152_setup_legacy(&dev->ad9152_device, &dev_init->ad9152_param);
	if (status != 0) {
		printf("error: ad9152_setup_legacy() failed\n");
		return status;
	}

	return status;
}
#endif

static int fmcdaq3_setup(struct fmcdaq3_dev *dev,
			 struct fmcdaq3_init_param *dev_init)
{
	int status;

	status = fmcdaq3_gpio_init(dev);
	if (status < 0)
		return status;

	status = fmcdaq3_spi_init(dev_init);
	if (status < 0)
		return status;

	status = fmcdaq3_clk_init(dev, dev_init);
	if (status < 0)
		return status;

	status = fmcdaq3_jesd_init(dev_init);
	if (status < 0)
		return status;

	/* ADC Core Parameters*/
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

	/* DAC Core Parameters*/
	dev->ad9152_channels[0].dds_dual_tone = 0;
	dev->ad9152_channels[0].dds_frequency_0 = 33*1000*1000;
	dev->ad9152_channels[0].dds_phase_0 = 0;
	dev->ad9152_channels[0].dds_scale_0 = 500000;
	dev->ad9152_channels[0].pat_data = 0xb1b0a1a0;
	dev->ad9152_channels[0].sel = AXI_DAC_DATA_SEL_DDS;

	dev->ad9152_channels[1].dds_dual_tone = 0;
	dev->ad9152_channels[1].dds_frequency_0 = 11*1000*1000;
	dev->ad9152_channels[1].dds_phase_0 = 0;
	dev->ad9152_channels[1].dds_scale_0 = 500000;
	dev->ad9152_channels[1].pat_data = 0xd1d0c1c0;
	dev->ad9152_channels[1].sel = AXI_DAC_DATA_SEL_DDS;

	dev_init->ad9152_core_param = (struct axi_dac_init) {
		.name = "ad9152_dac",
		.base =	TX_CORE_BASEADDR,
		.num_channels = 2,
		.channels = &dev->ad9152_channels[0],
		.rate = 3
	};

	for(uint32_t n=0;
		n < NO_OS_ARRAY_SIZE(dev_init->ad9152_param.lane_mux);
		n++)
		dev_init->ad9152_param.lane_mux[n] = n;

	dev_init->ad9152_param.stpl_samples[0][0] =
		(dev->ad9152_channels[0].pat_data >> 0)  & 0xffff;
	dev_init->ad9152_param.stpl_samples[0][1] =
		(dev->ad9152_channels[0].pat_data >> 16) & 0xffff;
	dev_init->ad9152_param.stpl_samples[0][2] =
		(dev->ad9152_channels[0].pat_data >> 0)  & 0xffff;
	dev_init->ad9152_param.stpl_samples[0][3] =
		(dev->ad9152_channels[0].pat_data >> 16) & 0xffff;
	dev_init->ad9152_param.stpl_samples[1][0] =
		(dev->ad9152_channels[1].pat_data >> 0)  & 0xffff;
	dev_init->ad9152_param.stpl_samples[1][1] =
		(dev->ad9152_channels[1].pat_data >> 16) & 0xffff;
	dev_init->ad9152_param.stpl_samples[1][2] =
		(dev->ad9152_channels[1].pat_data >> 0)  & 0xffff;
	dev_init->ad9152_param.stpl_samples[1][3] =
		(dev->ad9152_channels[1].pat_data >> 16) & 0xffff;

	dev_init->ad9152_param.num_converters =
		dev_init->ad9152_jesd_param.converters_per_device;
	dev_init->ad9152_param.num_lanes = 4;
	dev_init->ad9152_param.interpolation = 1; //bypass
	dev_init->ad9152_param.fcenter_shift = 0; //No NCO modulation
	dev_init->ad9152_param.pll_enable = 0; //Disable DAC PLL
	dev_init->ad9152_param.spi3wire = 1;
	dev_init->ad9152_param.sampling_frequency_khz = 1233000;

	dev_init->ad9680_param.lane_rate_kbps = 12330000;
	dev_init->ad9680_param.sampling_frequency_hz = 1233000000;
	dev_init->ad9680_param.dcm = 1; // from devicetree?? todo

	/* change the default JESD configurations, if required */
	fmcdaq3_reconfig(&dev_init->ad9152_param,
			 &dev_init->ad9152_xcvr_param,
			 &dev_init->ad9680_param,
			 &dev_init->ad9680_xcvr_param,
			 dev_init->ad9528_param.pdata);

	/* Reconfigure the default JESD configurations */
	dev_init->ad9680_jesd_param.device_clk_khz =
		dev_init->ad9680_xcvr_param.lane_rate_khz / 40;
	dev_init->ad9680_jesd_param.lane_clk_khz =
		dev_init->ad9680_xcvr_param.lane_rate_khz;
	dev_init->ad9152_jesd_param.lane_clk_khz =
		dev_init->ad9152_xcvr_param.lane_rate_khz;
	dev_init->ad9152_jesd_param.device_clk_khz =
		dev_init->ad9152_xcvr_param.lane_rate_khz / 40;

	/* setup clocks */
	status = ad9528_setup(&dev->ad9528_device, dev_init->ad9528_param);
	if (status != 0) {
		printf("error: ad9528_setup() failed\n");
		return status;
	}

	status = fmcdaq3_altera_pll_setup();
	if (status != 0)
		return status;

#ifdef JESD_FSM_ON
	status = fmcdaq3_setup_fsm(dev, dev_init, &rx_jesd_clk, &tx_jesd_clk);
	if (status) {
		printf("error: fmcdaq3_setup_fsm_on() failed\n");
		return status;
	}
#else
	status = fmcdaq3_setup_legacy(dev, dev_init);
	if (status) {
		printf("error: fmcdaq3_setup_legacy() failed\n");
		return status;
	}
#endif

	status = axi_adc_init(&dev->ad9680_core,  &dev_init->ad9680_core_param);
	if (status != 0) {
		printf("axi_adc_init() error: %s\n", dev_init->ad9680_core_param.name);
		return status;
	}

	status = axi_dac_init(&dev->ad9152_core, &dev_init->ad9152_core_param);
	if (status != 0) {
		printf("axi_dac_init() error: %s\n", dev_init->ad9152_core_param.name);
		return status;
	}

	return status;
}

int fmcdaq3_example_main()
{
	unsigned int *data = (unsigned int *)ADC_DDR_BASEADDR;
	int status;

	printf("Start main ...\r\n");
	status = fmcdaq3_setup(&fmcdaq3, &fmcdaq3_init);
	if (status < 0)
		return status;

#ifdef JESD_FSM_ON
	pr_info("Using JESD FSM.\n");

	struct jesd204_topology *topology, *topology_tx;
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = fmcdaq3.ad9680_jesd->jdev,
			.link_ids = {1},
			.links_number = 1,
		},
		{
			.jdev = fmcdaq3.ad9680_device->jdev,
			.link_ids = {1},
			.links_number = 1,
			.is_top_device = true,
		},
	};
	struct jesd204_topology_dev devs_tx[] = {
		{
			.jdev = fmcdaq3.ad9152_jesd->jdev,
			.link_ids = {2},
			.links_number = 1,
		},
		{
			.jdev = fmcdaq3.ad9152_device->jdev,
			.link_ids = {2},
			.links_number = 1,
			.is_top_device = true,
		},
	};
	jesd204_topology_init(&topology, devs,
			      sizeof(devs) / sizeof(*devs));
	jesd204_topology_init(&topology_tx, devs_tx,
			      sizeof(devs_tx) / sizeof(*devs_tx));

	jesd204_fsm_start(topology, JESD204_LINKS_ALL);
	jesd204_fsm_start(topology_tx, JESD204_LINKS_ALL);
#endif

	fmcdaq3_test(&fmcdaq3, &fmcdaq3_init);

	status = axi_dmac_init(&fmcdaq3.ad9680_dmac, &fmcdaq3_init.ad9680_dmac_param);
	if (status)
		return status;

	/* DMA Example */
#ifdef DMA_EXAMPLE
	extern const uint32_t sine_lut_iq[1024];
	fmcdaq3_init.ad9152_dmac_param = (struct axi_dmac_init) {
		.name = "ad9152_dmac",
		.base = TX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};

	fmcdaq3.ad9152_channels[0].sel = AXI_DAC_DATA_SEL_DMA;
	fmcdaq3.ad9152_channels[1].sel = AXI_DAC_DATA_SEL_DMA;
#ifdef USE_NCO
	status = ad9152_set_nco(fmcdaq3.ad9152_device,62500,1);
	if (status)
		return status;
#endif
	status = axi_dmac_init(&fmcdaq3.ad9152_dmac, &fmcdaq3_init.ad9152_dmac_param);
	if (status)
		return status;
	axi_dac_data_setup(fmcdaq3.ad9152_core);
	axi_dac_load_custom_data(fmcdaq3.ad9152_core, sine_lut_iq,
				 NO_OS_ARRAY_SIZE(sine_lut_iq), DAC_DDR_BASEADDR);
#ifndef ALTERA_PLATFORM
	Xil_DCacheFlush();
#endif
	struct axi_dma_transfer transfer_tx = {
		// Number of bytes to write/read
		.size = NO_OS_ARRAY_SIZE(sine_lut_iq) * sizeof(uint32_t),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = (uintptr_t)DAC_DDR_BASEADDR,
		// Address of data destination
		.dest_addr = 0
	};
	status = axi_dmac_transfer_start(fmcdaq3.ad9152_dmac, &transfer_tx);
	if (status)
		return status;
#ifndef ALTERA_PLATFORM
	Xil_DCacheInvalidateRange((uintptr_t)DAC_DDR_BASEADDR,
				  NO_OS_ARRAY_SIZE(sine_lut_iq) * sizeof(uint32_t));
#endif
#endif
//	struct axi_dma_transfer transfer_rx = {
//		// Number of bytes to write/read
//		.size = 1024 * sizeof(uint32_t),
//		// Transfer done flag
//		.transfer_done = 0,
//		// Signal transfer mode
//		.cyclic = NO,
//		// Address of data source
//		.src_addr = 0,
//		// Address of data destination
//		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
//	};
//	status = axi_dmac_transfer_start(fmcdaq3.ad9680_dmac, &transfer_rx);
//	if(status)
//		return status;
//
//	status = axi_dmac_transfer_wait_completion(fmcdaq3.ad9680_dmac, 1000);
//	if(status)
//		return status;
//
//#ifndef ALTERA_PLATFORM
//	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR,
//				  1024 * sizeof(uint32_t));
//#endif
//	printf("\ndaq3: setup and configuration is done\n");
//
//	printf("\n SAMPLE NO. |     CH1     |     CH2     |");
//	for (unsigned int i = 0; i < 1024; i++)
//		printf("\n %4d       |    %4d   |    %4d   |",
//		       i, (*(data + i) & 0xFFFF), (*(data + i) >> 16));

#ifndef IIO_SUPPORT_OFF
	status = fmcdaq3_iio_init(&fmcdaq3, &fmcdaq3_init);
	if (status != 0)
		return status;
#endif
	fmcdaq3_remove(&fmcdaq3);
	printf("\n Done main...\r\n");
	return 0;
}

