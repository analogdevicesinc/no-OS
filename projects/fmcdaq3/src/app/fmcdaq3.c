/***************************************************************************//**
 *   @file   projects/fmcdaq3/src/app/fmcdaq3.c
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
#include "ad9152.h"
#include "ad9528.h"
#include "ad9680.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "axi_jesd204_tx.h"
#include "axi_jesd204_rx.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_ad9680.h"
#include "iio_ad9152.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#ifndef ALTERA_PLATFORM
#include "xilinx_uart.h"
#endif
#endif

/* DAQ3 reconfiguration */
void fmcdaq3_reconfig(struct ad9152_init_param *ad9152_param,
		      struct adxcvr_init *ad9152_xcvr_param,
		      struct ad9680_init_param *ad9680_param,
		      struct adxcvr_init *ad9680_xcvr_param,
		      struct ad9528_platform_data *ad9528_param)
{

	uint8_t mode = 0;

	printf ("Available sampling rates:\n");
	printf ("\t1 - ADC 1233 MSPS; DAC 1233 MSPS\n");
	printf ("\t2 - ADC 616.5 MSPS; DAC 616.5 MSPS\n");

	mode = getc(stdin);

	switch (mode) {
	case '2':
		printf ("2 - ADC 616.5 MSPS; DAC 616.5 MSPS\n");
		ad9680_param->lane_rate_kbps = 6165000;
		ad9152_param->lane_rate_kbps = 6165000;
		ad9152_xcvr_param->lane_rate_khz = 6165000;
#ifndef ALTERA_PLATFORM
		ad9152_xcvr_param->ref_rate_khz = 308250;
#else
		ad9152_xcvr_param->parent_rate_khz = 308250;
#endif
		ad9680_xcvr_param->lane_rate_khz = 6165000;
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
}

/***************************************************************************//**
 * @brief main
 ******************************************************************************/
int main(void)
{

	int32_t status;

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
	struct xil_spi_init_param xil_spi_param = {
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

	/* Initialize GPIO structures */
	struct no_os_gpio_init_param dac_txen_param = {
		.number = GPIO_DAC_TXEN,
	};

	struct no_os_gpio_init_param adc_pd_param = {
		.number = GPIO_ADC_PD
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
	dac_txen_param.extra = &xil_gpio_param;
	dac_txen_param.platform_ops = &xil_gpio_ops;
	adc_pd_param.extra = &xil_gpio_param;
	adc_pd_param.platform_ops = &xil_gpio_ops;
#else
	struct altera_gpio_init_param altera_gpio_param = {
		.base_address = GPIO_BASEADDR,
		.type = NIOS_II_GPIO,
		.device_id = GPIO_DEVICE_ID
	};
	dac_txen_param.extra = &altera_gpio_param;
	dac_txen_param.platform_ops = &altera_gpio_ops;
	adc_pd_param.extra = &altera_gpio_param;
	adc_pd_param.platform_ops = &altera_gpio_ops;
#endif

	struct no_os_gpio_desc *dac_txen;
	struct no_os_gpio_desc *adc_pd;

	struct ad9528_dev* ad9528_device;
	struct ad9152_dev* ad9152_device;
	struct ad9680_dev* ad9680_device;

	struct ad9528_channel_spec ad9528_channels[8];
	struct ad9528_platform_data ad9528_pdata;

	struct ad9528_init_param ad9528_param;
	struct ad9152_init_param ad9152_param;
	struct ad9680_init_param ad9680_param;

	ad9528_param.spi_init = ad9528_spi_param;
	ad9152_param.spi_init = ad9152_spi_param;
	ad9680_param.spi_init = ad9680_spi_param;

#ifndef ALTERA_PLATFORM
	struct adxcvr_init ad9152_xcvr_param = {
		.name = "ad9152_xcvr",
		.base = XPAR_AXI_AD9152_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK_DIV2,
		.lpm_enable = 1,
		.ref_rate_khz = 616500,
		.lane_rate_khz = 12330000,
	};

	struct adxcvr_init ad9680_xcvr_param = {
		.name = "ad9680_xcvr",
		.base = XPAR_AXI_AD9680_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_REFCLK_DIV2,
		.lpm_enable = 1,
		.ref_rate_khz = 616500,
		.lane_rate_khz = 12330000
	};

#else
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

	struct altera_a10_fpll *ad9680_device_clk_pll;
	struct altera_a10_fpll *ad9152_device_clk_pll;

	struct adxcvr_init ad9152_xcvr_param = {
		.name = "ad9152_xcvr",
		.base = TX_XCVR_BASEADDR,
		.adxcfg_base = {TX_ADXCFG_0_BASEADDR, TX_ADXCFG_1_BASEADDR, TX_ADXCFG_2_BASEADDR, TX_ADXCFG_3_BASEADDR},
		.atx_pll_base = TX_PLL_BASEADDR,
		.lane_rate_khz = 12330000,
		.parent_rate_khz = 616500,
	};
	struct adxcvr_init ad9680_xcvr_param = {
		.name = "ad9680_xcvr",
		.base = RX_XCVR_BASEADDR,
		.adxcfg_base = {RX_ADXCFG_0_BASEADDR, RX_ADXCFG_1_BASEADDR, RX_ADXCFG_2_BASEADDR, RX_ADXCFG_3_BASEADDR},
		.atx_pll_base = RX_PLL_BASEADDR,
		.parent_rate_khz = 616500,
		.lane_rate_khz = 12330000
	};
#endif

	struct adxcvr	*ad9152_xcvr;
	struct adxcvr	*ad9680_xcvr;

	/* JESD initialization */
	struct jesd204_tx_init ad9152_jesd_param = {
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

	struct jesd204_rx_init  ad9680_jesd_param = {
		.name = "ad9680_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.subclass = 1,
		.device_clk_khz = 12330000 / 40,
		.lane_clk_khz = 12330000
	};

	struct axi_jesd204_tx *ad9152_jesd;
	struct axi_jesd204_rx *ad9680_jesd;

	/* ADC Core */
	struct axi_adc_init ad9680_core_param = {
		.name = "ad9680_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 2
	};
	struct axi_adc	*ad9680_core;

	/* DAC (AD9152) channels configuration */
	struct axi_dac_channel ad9152_channels[2];
	ad9152_channels[0].dds_dual_tone = 0;
	ad9152_channels[0].dds_frequency_0 = 33*1000*1000;
	ad9152_channels[0].dds_phase_0 = 0;
	ad9152_channels[0].dds_scale_0 = 500000;
	ad9152_channels[0].sel = AXI_DAC_DATA_SEL_DDS;
	ad9152_channels[1].dds_dual_tone = 0;
	ad9152_channels[1].dds_frequency_0 = 11*1000*1000;
	ad9152_channels[1].dds_phase_0 = 0;
	ad9152_channels[1].dds_scale_0 = 500000;
	ad9152_channels[0].pat_data = 0xb1b0a1a0;
	ad9152_channels[1].pat_data = 0xd1d0c1c0;
	ad9152_channels[1].sel = AXI_DAC_DATA_SEL_DDS;

	/* DAC Core */
	struct axi_dac_init ad9152_core_param = {
		.name = "ad9152_dac",
		.base =	TX_CORE_BASEADDR,
		.num_channels = 2,
		.channels = &ad9152_channels[0],
		.rate = 3
	};
	struct axi_dac	*ad9152_core;

	struct axi_dmac_init ad9680_dmac_param = {
		.name = "ad9680_dmac",
		.base = RX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};
	struct axi_dmac *ad9680_dmac;

	// ad9528 defaults
	ad9528_param.gpio_resetb = NULL;
	ad9528_param.pdata = &ad9528_pdata;
	ad9528_param.pdata->num_channels = 8;
	ad9528_param.pdata->channels = &ad9528_channels[0];
	ad9528_init(&ad9528_param);

	// dac-device-clock (1.233G)
	ad9528_channels[0].channel_num = 2;
	ad9528_channels[0].channel_divider = 1;

	//adc sysref (4.9M)
	ad9528_channels[1].channel_num = 8;
	ad9528_channels[1].channel_divider = 256;

	// adc-fpga-clock (616.5M)
	ad9528_channels[2].channel_num = 9;
	ad9528_channels[2].channel_divider = 2;

	// adc dev sysref (4.9M)
	ad9528_channels[3].channel_num = 7;
	ad9528_channels[3].channel_divider = 256;

	// adc-device-clock (1.233G)
	ad9528_channels[4].channel_num = 13;
	ad9528_channels[4].channel_divider = 1;

	// dac sysref (4.9M)
	ad9528_channels[5].channel_num = 5;
	ad9528_channels[5].channel_divider = 256;

	// dac-fpga-fmc (616.5M)
	ad9528_channels[6].channel_num = 4;
	ad9528_channels[6].channel_divider = 2;

	// dac dev sysref (4.9M)
	ad9528_channels[7].channel_num = 6;
	ad9528_channels[7].channel_divider = 256;

	// pllx settings
	ad9528_param.pdata->spi3wire = 1;
	ad9528_param.pdata->vcxo_freq = 100000000;
	ad9528_param.pdata->osc_in_diff_en = 1;
	ad9528_param.pdata->pll2_charge_pump_current_nA = 35000;
	ad9528_param.pdata->pll2_vco_div_m1 = 3;
	ad9528_param.pdata->pll2_r1_div = 3;
	ad9528_param.pdata->pll2_ndiv_a_cnt = 3;
	ad9528_param.pdata->pll2_ndiv_b_cnt = 27;
	ad9528_param.pdata->pll2_n2_div = 37;
	ad9528_param.pdata->sysref_k_div = 128;
	ad9528_param.pdata->rpole2 = RPOLE2_900_OHM;
	ad9528_param.pdata->rzero = RZERO_1850_OHM;
	ad9528_param.pdata->cpole1 = CPOLE1_16_PF;
	ad9528_param.pdata->sysref_pattern_mode = SYSREF_PATTERN_CONTINUOUS;
	ad9528_param.pdata->sysref_nshot_mode = SYSREF_NSHOT_4_PULSES;
	ad9528_param.pdata->sysref_req_en = true;
	ad9528_param.pdata->pll1_bypass_en = true;
	ad9528_param.pdata->pll2_bypass_en = false;

	ad9152_param.stpl_samples[0][0] =
		(ad9152_channels[0].pat_data >> 0)  & 0xffff;
	ad9152_param.stpl_samples[0][1] =
		(ad9152_channels[0].pat_data >> 16) & 0xffff;
	ad9152_param.stpl_samples[0][2] =
		(ad9152_channels[0].pat_data >> 0)  & 0xffff;
	ad9152_param.stpl_samples[0][3] =
		(ad9152_channels[0].pat_data >> 16) & 0xffff;
	ad9152_param.stpl_samples[1][0] =
		(ad9152_channels[1].pat_data >> 0)  & 0xffff;
	ad9152_param.stpl_samples[1][1] =
		(ad9152_channels[1].pat_data >> 16) & 0xffff;
	ad9152_param.stpl_samples[1][2] =
		(ad9152_channels[1].pat_data >> 0)  & 0xffff;
	ad9152_param.stpl_samples[1][3] =
		(ad9152_channels[1].pat_data >> 16) & 0xffff;
	ad9152_param.interpolation = 1;
	ad9152_param.lane_rate_kbps = 12330000;

	// adc settings
	ad9680_param.lane_rate_kbps = 12330000;

#ifndef ALTERA_PLATFORM
	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();
#endif

	/* set GPIOs */
	no_os_gpio_get(&dac_txen,  &dac_txen_param);
	no_os_gpio_get(&adc_pd,    &adc_pd_param);

	no_os_gpio_direction_output(dac_txen,  1);
	no_os_gpio_direction_output(adc_pd,    0);

	fmcdaq3_reconfig(&ad9152_param,
			 &ad9152_xcvr_param,
			 &ad9680_param,
			 &ad9680_xcvr_param,
			 ad9528_param.pdata);

	/* Reconfigure the default JESD configurations */
	ad9680_jesd_param.lane_clk_khz = ad9680_xcvr_param.lane_rate_khz;
	ad9680_jesd_param.device_clk_khz = ad9680_xcvr_param.lane_rate_khz / 40;
	ad9152_jesd_param.lane_clk_khz = ad9152_xcvr_param.lane_rate_khz;
	ad9152_jesd_param.device_clk_khz = ad9152_xcvr_param.lane_rate_khz / 40;

	status = ad9528_setup(&ad9528_device, ad9528_param);
	if (status != 0) {
		printf("error: ad9523_setup() failed\n");
	}
	// Recommended DAC JESD204 link startup sequence
	//   1. FPGA JESD204 Link Layer
	//   2. FPGA JESD204 PHY Layer
	//   3. DAC

#ifdef ALTERA_PLATFORM
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
					  ad9680_jesd_param.device_clk_khz * 1000);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       ad9680_device_clk_pll->name);
	}
	altera_a10_fpll_enable(ad9680_device_clk_pll);
	altera_a10_fpll_disable(ad9152_device_clk_pll);
	status = altera_a10_fpll_set_rate(ad9152_device_clk_pll,
					  ad9152_jesd_param.device_clk_khz * 1000);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       ad9152_device_clk_pll->name);
	}
	altera_a10_fpll_enable(ad9152_device_clk_pll);
#endif
	status = ad9680_setup(&ad9680_device, &ad9680_param);
	if (status != 0) {
		printf("error: ad9680_setup() failed\n");
	}
	status = axi_jesd204_tx_init_legacy(&ad9152_jesd, &ad9152_jesd_param);
	if (status != 0) {
		printf("error: %s: axi_jesd204_rx_init_legacy() failed\n", ad9152_jesd->name);
	}
	status = axi_jesd204_tx_lane_clk_enable(ad9152_jesd);
	if (status != 0) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad9152_jesd->name);
	}
	status = adxcvr_init(&ad9152_xcvr, &ad9152_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", ad9152_xcvr->name);
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(ad9152_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", ad9152_xcvr->name);
	}
#endif
	status = adxcvr_init(&ad9680_xcvr, &ad9680_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", ad9680_xcvr->name);
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(ad9680_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", ad9680_xcvr->name);
	}
#endif
	status = axi_jesd204_rx_init_legacy(&ad9680_jesd, &ad9680_jesd_param);
	if (status != 0) {
		printf("error: %s: axi_jesd204_rx_init_legacy() failed\n", ad9680_jesd->name);
	}
	status = axi_jesd204_rx_lane_clk_enable(ad9680_jesd);
	if (status != 0) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad9680_jesd->name);
	}
	status = ad9152_setup(&ad9152_device, ad9152_param);
	if (status != 0) {
		printf("error: ad9152_setup() failed\n");
	}
	status = axi_adc_init(&ad9680_core,  &ad9680_core_param);
	if (status != 0) {
		printf("axi_adc_init() error: %s\n", ad9680_core->name);
	}
	status = axi_dac_init(&ad9152_core, &ad9152_core_param);
	if (status != 0) {
		printf("axi_dac_init() error: %s\n", ad9152_core->name);
	}

	status = axi_jesd204_rx_status_read(ad9680_jesd);
	if (status != 0) {
		printf("axi_jesd204_rx_status_read() error: %"PRIi32"\n", status);
	}

	status = axi_jesd204_tx_status_read(ad9152_jesd);
	if (status != 0) {
		printf("axi_jesd204_tx_status_read() error: %"PRIi32"\n", status);
	}

	ad9680_test(ad9680_device, AD9680_TEST_PN9);
	if(axi_adc_pn_mon(ad9680_core, AXI_ADC_PN9, 10) == -1) {
		printf("%s ad9680 - PN9 sequence mismatch!\n", __func__);
	};
	ad9680_test(ad9680_device, AD9680_TEST_PN23);
	if(axi_adc_pn_mon(ad9680_core, AXI_ADC_PN23A, 10) == -1) {
		printf("%s ad9680 - PN23 sequence mismatch!\n", __func__);
	};

	ad9152_status(ad9152_device);

	// ad9152-x1 do not support data path prbs (use short-tpl)

	ad9152_channels[0].sel = AXI_DAC_DATA_SEL_SED;
	ad9152_channels[1].sel = AXI_DAC_DATA_SEL_SED;
	axi_dac_data_setup(ad9152_core);
	ad9152_short_pattern_test(ad9152_device, ad9152_param);

	// ad9152-xN (n > 1) supports data path prbs

	ad9152_channels[0].sel = AXI_DAC_DATA_SEL_PN23;
	ad9152_channels[1].sel = AXI_DAC_DATA_SEL_PN23;
	axi_dac_data_setup(ad9152_core);
	ad9152_param.prbs_type = AD9152_TEST_PN7;
	ad9152_datapath_prbs_test(ad9152_device, ad9152_param);

	ad9152_channels[0].sel = AXI_DAC_DATA_SEL_PN31;
	ad9152_channels[1].sel = AXI_DAC_DATA_SEL_PN31;
	axi_dac_data_setup(ad9152_core);
	ad9152_param.prbs_type = AD9152_TEST_PN15;
	ad9152_datapath_prbs_test(ad9152_device, ad9152_param);

	// default data

	ad9152_channels[0].sel = AXI_DAC_DATA_SEL_DDS;
	ad9152_channels[1].sel = AXI_DAC_DATA_SEL_DDS;
	axi_dac_data_setup(ad9152_core);
	ad9680_test(ad9680_device, AD9680_TEST_OFF);

	// capture data with DMA
	/* Initialize the DMAC and transfer 16384 samples from ADC to MEM */
	axi_dmac_init(&ad9680_dmac, &ad9680_dmac_param);

	struct axi_dma_transfer transfer_rx = {
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
	axi_dmac_transfer_start(ad9680_dmac, &transfer_rx);
	status = axi_dmac_transfer_wait_completion(ad9680_dmac, 500);
	if(status)
		return status;
#ifdef XILINX_PLATFORM
	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR,
				  16384 * 2);
#endif

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

	struct axi_dmac_init ad9152_dmac_param = {
		.name = "ad9152_dmac",
		.base = TX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED
	};

	struct axi_dmac *ad9152_dmac;

	axi_dmac_init(&ad9152_dmac, &ad9152_dmac_param);

	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_axi_dac_desc *iio_axi_dac_desc;
	struct iio_device *adc_dev_desc, *dac_dev_desc;
	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad9680_core,
		.rx_dmac = ad9680_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};
	struct iio_axi_dac_init_param iio_axi_dac_init_par;
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = ad9152_core,
		.tx_dmac = ad9152_dmac,
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
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc, adc_dev_desc,
			       &read_buff, NULL, NULL),
		IIO_APP_DEVICE("axi_dac", iio_axi_dac_desc, dac_dev_desc,
			       NULL, &write_buff, NULL),
		IIO_APP_DEVICE("ad9680_dev", ad9680_device,
			       &ad9680_iio_descriptor, NULL, NULL, NULL),
		IIO_APP_DEVICE("ad9152_dev", ad9152_device,
			       &ad9152_iio_descriptor, NULL, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	iio_app_run(app);

#endif

	printf("daq3: setup and configuration is done\n");

	ad9528_remove(ad9528_device);
	ad9152_remove(ad9152_device);
	ad9680_remove(ad9680_device);
	no_os_gpio_remove(dac_txen);
	no_os_gpio_remove(adc_pd);

#ifndef ALTERA_PLATFORM
	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();
#endif

	return(0);
}
