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
#include "spi.h"
#include "gpio.h"
#include "spi_extra.h"
#include "gpio_extra.h"
#include "delay.h"
#include "error.h"
#include "ad9144.h"
#include "ad9523.h"
#include "ad9680.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "axi_jesd204_tx.h"
#include "axi_jesd204_rx.h"

#ifdef IIO_SUPPORT
#include "app_iio.h"
#endif

struct fmcdaq2_dev {
	struct gpio_desc *gpio_clkd_sync;
	struct gpio_desc *gpio_dac_reset;
	struct gpio_desc *gpio_dac_txen;
	struct gpio_desc *gpio_adc_pd;
}fmcdaq2;

static int fmcdaq2_gpio_init(struct fmcdaq2_dev *dev)
{
	int status;

	/* Initialize GPIO structures */
	struct gpio_init_param gpio_clkd_sync_param = {
		.number = GPIO_CLKD_SYNC,
		.platform_ops = &xil_gpio_platform_ops
	};
	struct gpio_init_param gpio_dac_reset_param = {
		.number = GPIO_DAC_RESET,
		.platform_ops = &xil_gpio_platform_ops
	};
	struct gpio_init_param gpio_dac_txen_param = {
		.number = GPIO_DAC_TXEN,
		.platform_ops = &xil_gpio_platform_ops
	};
	struct gpio_init_param gpio_adc_pd_param = {
		.number = GPIO_ADC_PD,
		.platform_ops = &xil_gpio_platform_ops
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
	gpio_clkd_sync_param.extra = &xil_gpio_param;
	gpio_dac_reset_param.extra = &xil_gpio_param;
	gpio_dac_txen_param.extra = &xil_gpio_param;
	gpio_adc_pd_param.extra = &xil_gpio_param;
#else
	struct altera_gpio_init_param altera_gpio_param = {
		.base_address = GPIO_BASEADDR,
		.type = NIOS_II_GPIO,
		.device_id = GPIO_DEVICE_ID
	};
	gpio_clkd_sync_param.extra = &altera_gpio_param;
	gpio_dac_reset_param.extra = &altera_gpio_param;
	gpio_dac_txen_param.extra = &altera_gpio_param;
	gpio_adc_pd_param.extra = &altera_gpio_param;
#endif

	/* set GPIOs */
	status = gpio_get(&dev->gpio_clkd_sync, &gpio_clkd_sync_param);
	if (status < 0)
		return status;

	status = gpio_get(&dev->gpio_dac_reset, &gpio_dac_reset_param);
	if (status < 0)
		return status;

	status = gpio_get(&dev->gpio_dac_txen, &gpio_dac_txen_param);
	if (status < 0)
		return status;

	status = gpio_get(&dev->gpio_adc_pd, &gpio_adc_pd_param);
	if (status < 0)
		return status;

	status = gpio_direction_output(dev->gpio_clkd_sync, GPIO_LOW);
	if (status < 0)
		return status;

	status = gpio_direction_output(dev->gpio_dac_reset, GPIO_LOW);
	if (status < 0)
		return status;

	status = gpio_direction_output(dev->gpio_dac_txen, GPIO_LOW);
	if (status < 0)
		return status;

	status = gpio_direction_output(dev->gpio_adc_pd, GPIO_HIGH);
	if (status < 0)
		return status;

	mdelay(5);

	status = gpio_set_value(dev->gpio_clkd_sync, GPIO_HIGH);
	if (status < 0)
		return status;

	status = gpio_set_value(dev->gpio_dac_reset, GPIO_HIGH);
	if (status < 0)
		return status;

	status = gpio_set_value(dev->gpio_dac_txen, GPIO_HIGH);
	if (status < 0)
		return status;

	return gpio_set_value(dev->gpio_adc_pd, GPIO_LOW);
}

int fmcdaq2_reconfig(struct ad9144_init_param *p_ad9144_param,
		     struct adxcvr_init *ad9144_xcvr_param,
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
		/* REF clock = 100 MHz */
		p_ad9523_param->channels[DAC_DEVICE_CLK].channel_divider = 10;
		p_ad9144_param->pll_ref_frequency_khz = 100000;

		/* DAC at 2 GHz using the internal PLL and 2 times interpolation */
		p_ad9144_param->interpolation = 2;
		p_ad9144_param->pll_enable = 1;
		p_ad9144_param->pll_dac_frequency_khz = 2000000;
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
		p_ad9144_param->lane_rate_kbps = 6000000;
		ad9144_xcvr_param->lane_rate_khz = 6000000;
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->ref_rate_khz = 300000;
#else
		ad9144_xcvr_param->parent_rate_khz = 300000;
#endif
		p_ad9680_param->lane_rate_kbps = 6000000;
		ad9680_xcvr_param->lane_rate_khz = 6000000;
#ifndef ALTERA_PLATFORM
		ad9680_xcvr_param->ref_rate_khz = 300000;
#else
		ad9680_xcvr_param->parent_rate_khz = 300000;
#endif
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->lpm_enable = 0;
		ad9144_xcvr_param->cpll_enable = 1;
		ad9144_xcvr_param->sys_clk_sel = 0;
		ad9144_xcvr_param->out_clk_sel = 4;

		ad9680_xcvr_param->lpm_enable = 1;
		ad9680_xcvr_param->cpll_enable = 1;
		ad9680_xcvr_param->out_clk_sel = 4;
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
		p_ad9144_param->lane_rate_kbps = 5000000;
		ad9144_xcvr_param->lane_rate_khz = 5000000;
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->ref_rate_khz = 250000;
#else
		ad9144_xcvr_param->parent_rate_khz = 250000;
#endif
		p_ad9680_param->lane_rate_kbps = 5000000;
		ad9680_xcvr_param->lane_rate_khz = 5000000;
#ifndef ALTERA_PLATFORM
		ad9680_xcvr_param->ref_rate_khz = 250000;
#else
		ad9680_xcvr_param->parent_rate_khz = 250000;
#endif
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->sys_clk_sel = 0;
		ad9144_xcvr_param->lpm_enable = 1;
		ad9144_xcvr_param->cpll_enable = 1;
		ad9144_xcvr_param->out_clk_sel = 4;

		ad9680_xcvr_param->lpm_enable = 1;
		ad9680_xcvr_param->cpll_enable = 1;
		ad9680_xcvr_param->out_clk_sel = 4;
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
		p_ad9144_param->lane_rate_kbps = 10000000;
		ad9144_xcvr_param->lane_rate_khz = 10000000;
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->ref_rate_khz = 500000;
#else
		ad9144_xcvr_param->parent_rate_khz = 500000;
#endif
		p_ad9680_param->lane_rate_kbps = 5000000;
		ad9680_xcvr_param->lane_rate_khz = 5000000;
#ifndef ALTERA_PLATFORM
		ad9680_xcvr_param->ref_rate_khz = 500000;
#else
		ad9680_xcvr_param->parent_rate_khz = 500000;
#endif
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->lpm_enable = 0;
		ad9144_xcvr_param->cpll_enable = 0;
		ad9144_xcvr_param->out_clk_sel = 4;

		ad9680_xcvr_param->lpm_enable = 1;
		ad9680_xcvr_param->cpll_enable = 1;
		ad9680_xcvr_param->out_clk_sel = 4;
#endif
		break;
	default:
		printf ("1 - ADC 1000 MSPS; DAC 1000 MSPS\n");
#ifndef ALTERA_PLATFORM
		ad9144_xcvr_param->ref_rate_khz = 500000;
		ad9680_xcvr_param->ref_rate_khz = 500000;
#else
		ad9144_xcvr_param->parent_rate_khz = 500000;
		ad9680_xcvr_param->parent_rate_khz = 500000;
#endif
		break;
	}

	return(0);
}

int main(void)
{
	int32_t status;

	status = fmcdaq2_gpio_init(&fmcdaq2);
	if (status < 0)
		return status;

	/* Initialize SPI structures */
	struct spi_init_param ad9523_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 0,
		.mode = SPI_MODE_0
	};

	struct spi_init_param ad9144_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 1,
		.mode = SPI_MODE_0
	};
	struct spi_init_param ad9680_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 2,
		.mode = SPI_MODE_0
	};

#ifndef ALTERA_PLATFORM
	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.device_id = SPI_DEVICE_ID
	};
	ad9523_spi_param.platform_ops = &xil_platform_ops;
	ad9523_spi_param.extra = &xil_spi_param;
	ad9144_spi_param.platform_ops = &xil_platform_ops;
	ad9144_spi_param.extra = &xil_spi_param;
	ad9680_spi_param.platform_ops = &xil_platform_ops;
	ad9680_spi_param.extra = &xil_spi_param;
#else
	struct altera_spi_init_param altera_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.type = NIOS_II_SPI,
		.base_address = SYS_SPI_BASE
	};
	ad9523_spi_param.platform_ops = &altera_platform_ops;
	ad9523_spi_param.extra = &altera_spi_param;
	ad9144_spi_param.platform_ops = &altera_platform_ops;
	ad9144_spi_param.extra = &altera_spi_param;
	ad9680_spi_param.platform_ops = &altera_platform_ops;
	ad9680_spi_param.extra = &altera_spi_param;
#endif

	/* setup the device structures */
	struct ad9523_dev *ad9523_device;
	struct ad9144_dev *ad9144_device;
	struct ad9680_dev *ad9680_device;

	struct ad9523_channel_spec	ad9523_channels[8];
	struct ad9523_platform_data	ad9523_pdata;

	struct ad9523_init_param	ad9523_param;
	struct ad9144_init_param	ad9144_param;
	struct ad9680_init_param	ad9680_param;

	ad9523_param.spi_init = ad9523_spi_param;
	ad9144_param.spi_init = ad9144_spi_param;
	ad9680_param.spi_init = ad9680_spi_param;

#ifndef ALTERA_PLATFORM
	struct adxcvr_init ad9144_xcvr_param = {
		.name = "ad9144_xcvr",
		.base = XPAR_AXI_AD9144_XCVR_BASEADDR,
		.sys_clk_sel = 3,
		.out_clk_sel = 4,
		.lpm_enable = 1,
		.cpll_enable = 0,
		.ref_rate_khz = 500000,
		.lane_rate_khz = 10000000,
	};
	struct adxcvr_init ad9680_xcvr_param = {
		.name = "ad9680_xcvr",
		.base = XPAR_AXI_AD9680_XCVR_BASEADDR,
		.sys_clk_sel = 0,
		.out_clk_sel = 4,
		.lpm_enable = 1,
		.cpll_enable = 1,
		.ref_rate_khz = 500000,
		.lane_rate_khz = 10000000
	};

#else
	struct altera_a10_fpll_init ad9680_device_clk_pll_param = {
		.name = "ad9680_device_clk_pll",
		.base = RX_A10_FPLL_BASEADDR,
		.parent_rate = 500000 * 1000
	};
	struct altera_a10_fpll_init ad9144_device_clk_pll_param = {
		.name = "ad9144_device_clk_pll",
		.base = TX_A10_FPLL_BASEADDR,
		.parent_rate = 500000 * 1000
	};

	struct altera_a10_fpll *ad9680_device_clk_pll;
	struct altera_a10_fpll *ad9144_device_clk_pll;

	struct adxcvr_init ad9144_xcvr_param = {
		.name = "ad9144_xcvr",
		.base = TX_XCVR_BASEADDR,
		.adxcfg_base = {TX_ADXCFG_0_BASEADDR, TX_ADXCFG_1_BASEADDR, TX_ADXCFG_2_BASEADDR, TX_ADXCFG_3_BASEADDR},
		.atx_pll_base = TX_PLL_BASEADDR,
		.lane_rate_khz = 10000000,
		.parent_rate_khz = 500000,
	};
	struct adxcvr_init ad9680_xcvr_param = {
		.name = "ad9680_xcvr",
		.base = RX_XCVR_BASEADDR,
		.adxcfg_base = {RX_ADXCFG_0_BASEADDR, RX_ADXCFG_1_BASEADDR, RX_ADXCFG_2_BASEADDR, RX_ADXCFG_3_BASEADDR},
		.atx_pll_base = RX_PLL_BASEADDR,
		.parent_rate_khz = 500000,
		.lane_rate_khz = 10000000
	};
#endif

	struct adxcvr	*ad9144_xcvr;
	struct adxcvr	*ad9680_xcvr;

	/* JESD initialization */
	struct jesd204_tx_init ad9144_jesd_param = {
		.name = "ad9144_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.converters_per_device = 2,
		.converter_resolution = 16,
		.bits_per_sample = 16,
		.high_density = false,
		.control_bits_per_sample = 0,
		.subclass = 1,
		.device_clk_khz = 10000000/40,
		.lane_clk_khz = 10000000
	};

	struct jesd204_rx_init  ad9680_jesd_param = {
		.name = "ad9680_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 32,
		.subclass = 1,
		.device_clk_khz = 10000000/40,
		.lane_clk_khz = 10000000
	};

	struct axi_jesd204_tx *ad9144_jesd;
	struct axi_jesd204_rx *ad9680_jesd;

	/* ADC Core */
	struct axi_adc_init ad9680_core_param = {
		.name = "ad9680_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 2
	};
	struct axi_adc	*ad9680_core;

	/* DAC (AD9144) channels configuration */
	struct axi_dac_channel	ad9144_channels[2];
	ad9144_channels[0].dds_dual_tone = 0;
	ad9144_channels[0].dds_frequency_0 = 33*1000*1000;
	ad9144_channels[0].dds_phase_0 = 0;
	ad9144_channels[0].dds_scale_0 = 500000;
	ad9144_channels[0].pat_data = 0xb1b0a1a0;
	ad9144_channels[1].dds_dual_tone = 0;
	ad9144_channels[1].dds_frequency_0 = 11*1000*1000;
	ad9144_channels[1].dds_phase_0 = 0;
	ad9144_channels[1].dds_scale_0 = 500000;
	ad9144_channels[1].pat_data = 0xd1d0c1c0;
	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_DDS;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_DDS;

	/* DAC Core */
	struct axi_dac_init ad9144_core_param = {
		.name = "ad9144_dac",
		.base =	TX_CORE_BASEADDR,
		.num_channels = 2,
		.channels = &ad9144_channels[0]
	};
	struct axi_dac	*ad9144_core;

#ifdef DAC_DMA_EXAMPLE
	struct axi_dmac_init ad9144_dmac_param = {
		.name = "tx_dmac",
		.base = TX_DMA_BASEADDR,
		.direction = DMA_MEM_TO_DEV,
		.flags = 0
	};
	struct axi_dmac		*ad9144_dmac;
	extern const uint32_t sine_lut_iq[1024];
#endif

	struct axi_dmac_init ad9680_dmac_param = {
		.name = "ad9680_dmac",
		.base = RX_DMA_BASEADDR,
		.direction = DMA_DEV_TO_MEM,
		.flags = 0
	};
	struct axi_dmac *ad9680_dmac;

	/* clock distribution device (AD9523) configuration */
	ad9523_pdata.num_channels = 8;
	ad9523_pdata.channels = &ad9523_channels[0];
	ad9523_param.pdata = &ad9523_pdata;
	ad9523_init(&ad9523_param);

	// dac device-clk-sysref, fpga-clk-sysref

	ad9523_channels[DAC_DEVICE_CLK].channel_num = 1;
	ad9523_channels[DAC_DEVICE_CLK].channel_divider = 1;
	ad9523_channels[DAC_DEVICE_SYSREF].channel_num = 7;
	ad9523_channels[DAC_DEVICE_SYSREF].channel_divider = 128;
	ad9523_channels[DAC_FPGA_CLK].channel_num = 9;
	ad9523_channels[DAC_FPGA_CLK].channel_divider = 2;
	ad9523_channels[DAC_FPGA_SYSREF].channel_num = 8;
	ad9523_channels[DAC_FPGA_SYSREF].channel_divider = 128;

	// adc device-clk-sysref, fpga-clk-sysref

	ad9523_channels[ADC_DEVICE_CLK].channel_num = 13;
	ad9523_channels[ADC_DEVICE_CLK].channel_divider = 1;
	ad9523_channels[ADC_DEVICE_SYSREF].channel_num = 6;
	ad9523_channels[ADC_DEVICE_SYSREF].channel_divider = 128;
	ad9523_channels[ADC_FPGA_CLK].channel_num = 4;
	ad9523_channels[ADC_FPGA_CLK].channel_divider = 2;
	ad9523_channels[ADC_FPGA_SYSREF].channel_num = 5;
	ad9523_channels[ADC_FPGA_SYSREF].channel_divider = 128;

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

	ad9680_param.lane_rate_kbps = 10000000;

	ad9144_param.lane_rate_kbps = 10000000;
	ad9144_param.spi3wire = 1;
	ad9144_param.interpolation = 1;
	ad9144_param.pll_enable = 0;
	ad9144_param.jesd204_subclass = 1;
	ad9144_param.jesd204_scrambling = 1;
	ad9144_param.jesd204_mode = 4;
	for(uint32_t n=0; n<ARRAY_SIZE(ad9144_param.jesd204_lane_xbar); n++)
		ad9144_param.jesd204_lane_xbar[n] = n;

	ad9144_param.stpl_samples[0][0] =
		(ad9144_channels[0].pat_data >> 0)  & 0xffff;
	ad9144_param.stpl_samples[0][1] =
		(ad9144_channels[0].pat_data >> 16) & 0xffff;
	ad9144_param.stpl_samples[0][2] =
		(ad9144_channels[0].pat_data >> 0)  & 0xffff;
	ad9144_param.stpl_samples[0][3] =
		(ad9144_channels[0].pat_data >> 16) & 0xffff;
	ad9144_param.stpl_samples[1][0] =
		(ad9144_channels[1].pat_data >> 0)  & 0xffff;
	ad9144_param.stpl_samples[1][1] =
		(ad9144_channels[1].pat_data >> 16) & 0xffff;
	ad9144_param.stpl_samples[1][2] =
		(ad9144_channels[1].pat_data >> 0)  & 0xffff;
	ad9144_param.stpl_samples[1][3] =
		(ad9144_channels[1].pat_data >> 16) & 0xffff;

	/* change the default JESD configurations, if required */
	fmcdaq2_reconfig(&ad9144_param,
			 &ad9144_xcvr_param,
			 &ad9680_param,
			 &ad9680_xcvr_param,
			 ad9523_param.pdata);

	/* Reconfigure the default JESD configurations */
	ad9680_jesd_param.device_clk_khz =  ad9680_xcvr_param.lane_rate_khz / 40;
	ad9680_jesd_param.lane_clk_khz = ad9680_xcvr_param.lane_rate_khz;
	ad9144_jesd_param.device_clk_khz =  ad9144_xcvr_param.lane_rate_khz / 40;
	ad9144_jesd_param.lane_clk_khz = ad9144_xcvr_param.lane_rate_khz ;

	/* setup clocks */
	status = ad9523_setup(&ad9523_device, &ad9523_param);
	if (status != SUCCESS) {
		printf("error: ad9680_setup() failed\n");
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

#ifdef ALTERA_PLATFORM
	/* Initialize A10 FPLLs */
	status = altera_a10_fpll_init(&ad9680_device_clk_pll,
				      &ad9680_device_clk_pll_param);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       ad9680_device_clk_pll_param.name);
	}
	status = altera_a10_fpll_init(&ad9144_device_clk_pll,
				      &ad9144_device_clk_pll_param);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       ad9144_device_clk_pll_param.name);
	}

	altera_a10_fpll_disable(ad9680_device_clk_pll);
	status = altera_a10_fpll_set_rate(ad9680_device_clk_pll,
					  ad9680_jesd_param.device_clk_khz * 1000);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       ad9680_device_clk_pll->name);
	}
	altera_a10_fpll_enable(ad9680_device_clk_pll);
	altera_a10_fpll_disable(ad9144_device_clk_pll);
	status = altera_a10_fpll_set_rate(ad9144_device_clk_pll,
					  ad9144_jesd_param.device_clk_khz * 1000);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       ad9144_device_clk_pll->name);
	}
	altera_a10_fpll_enable(ad9144_device_clk_pll);
#endif
	status = ad9680_setup(&ad9680_device, &ad9680_param);
	if (status != SUCCESS) {
		printf("error: ad9680_setup() failed\n");
	}
	status = axi_jesd204_tx_init(&ad9144_jesd, &ad9144_jesd_param);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_rx_init() failed\n", ad9144_jesd->name);
	}

	status = axi_jesd204_tx_lane_clk_enable(ad9144_jesd);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad9144_jesd->name);
	}

	status = adxcvr_init(&ad9144_xcvr, &ad9144_xcvr_param);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_init() failed\n", ad9144_xcvr->name);
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(ad9144_xcvr);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_clk_enable() failed\n", ad9144_xcvr->name);
	}
#endif
	status = adxcvr_init(&ad9680_xcvr, &ad9680_xcvr_param);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_init() failed\n", ad9680_xcvr->name);
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(ad9680_xcvr);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_clk_enable() failed\n", ad9680_xcvr->name);
	}
#endif
	status = axi_jesd204_rx_init(&ad9680_jesd, &ad9680_jesd_param);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_rx_init() failed\n", ad9680_jesd->name);
	}

	status = axi_jesd204_rx_lane_clk_enable(ad9680_jesd);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad9144_jesd->name);
	}

	status = ad9144_setup(&ad9144_device, &ad9144_param);
	if (status != SUCCESS) {
		printf("error: ad9144_setup() failed\n");
	}
	status = axi_jesd204_rx_status_read(ad9680_jesd);
	if (status != SUCCESS) {
		printf("axi_jesd204_rx_status_read() error: %"PRIi32"\n", status);
	}

	status = axi_jesd204_tx_status_read(ad9144_jesd);
	if (status != SUCCESS) {
		printf("axi_jesd204_tx_status_read() error: %"PRIi32"\n", status);
	}

	status = axi_adc_init(&ad9680_core,  &ad9680_core_param);
	if (status != SUCCESS) {
		printf("axi_adc_init() error: %s\n", ad9680_core->name);
	}

	status = axi_dac_init(&ad9144_core, &ad9144_core_param);
	if (status != SUCCESS) {
		printf("axi_dac_init() error: %s\n", ad9144_core->name);
	}

	status = ad9144_status(ad9144_device);

	/* transport path testing */
	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_SED;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_SED;
	axi_dac_data_setup(ad9144_core);
	ad9144_short_pattern_test(ad9144_device, &ad9144_param);

	// PN7 data path test

	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_PN23;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_PN23;
	axi_dac_data_setup(ad9144_core);
	ad9144_param.prbs_type = AD9144_PRBS7;
	ad9144_datapath_prbs_test(ad9144_device, &ad9144_param);

	// PN15 data path test

	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_PN31;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_PN31;
	axi_dac_data_setup(ad9144_core);
	ad9144_param.prbs_type = AD9144_PRBS15;
	ad9144_datapath_prbs_test(ad9144_device, &ad9144_param);

	/* receive path testing */
	ad9680_test(ad9680_device, AD9680_TEST_PN9);
	if(axi_adc_pn_mon(ad9680_core, AXI_ADC_PN9, 10) == -1) {
		printf("%s ad9680 - PN9 sequence mismatch!\n", __func__);
	};
	ad9680_test(ad9680_device, AD9680_TEST_PN23);
	if(axi_adc_pn_mon(ad9680_core, AXI_ADC_PN23A, 10) == -1) {
		printf("%s ad9680 - PN23 sequence mismatch!\n", __func__);
	};

	ad9680_test(ad9680_device, AD9680_TEST_OFF);

	/* DAC DMA Example */
#ifdef DAC_DMA_EXAMPLE
	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_DMA;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_DMA;
	axi_dac_data_setup(ad9144_core);

	Xil_DCacheFlush();

	axi_dac_load_custom_data(ad9144_core, sine_lut_iq,
				 ARRAY_SIZE(sine_lut_iq), DAC_DDR_BASEADDR);

#ifndef ALTERA_PLATFORM
	Xil_DCacheFlush();
#endif

	axi_dmac_init(&ad9144_dmac, &ad9144_dmac_param);

	axi_dmac_transfer(ad9144_dmac, DAC_DDR_BASEADDR,
			  sizeof(sine_lut_iq) * 4);
#else
	ad9144_channels[0].sel = AXI_DAC_DATA_SEL_DDS;
	ad9144_channels[1].sel = AXI_DAC_DATA_SEL_DDS;
	axi_dac_data_setup(ad9144_core);
#endif

	/* Initialize the DMAC and transfer 16384 samples from ADC to MEM */
	axi_dmac_init(&ad9680_dmac, &ad9680_dmac_param);

	axi_dmac_transfer(ad9680_dmac, ADC_DDR_BASEADDR,
			  16384 * 2);

	printf("daq2: setup and configuration is done\n");

#ifndef ALTERA_PLATFORM
	Xil_DCacheDisable();
	Xil_ICacheDisable();
#endif

#ifdef IIO_SUPPORT
	printf("The board accepts libiio clients connections through the serial backend.\n");

	struct axi_dmac_init ad9144_dmac_param = {
		.name = "ad9144_dmac",
		.base = TX_DMA_BASEADDR,
		.direction = DMA_MEM_TO_DEV,
		.flags = DMA_CYCLIC
	};
	struct axi_dmac		*ad9144_dmac;
	axi_dmac_init(&ad9144_dmac, &ad9144_dmac_param);

	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad9680_core,
		.rx_dmac = ad9680_dmac,
		.adc_ddr_base = ADC_DDR_BASEADDR,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};
	struct iio_axi_dac_init_param iio_axi_dac_init_par;
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = ad9144_core,
		.tx_dmac = ad9144_dmac,
		.dac_ddr_base = DAC_DDR_BASEADDR,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange
#endif
	};

	return iio_server_init(&iio_axi_adc_init_par, &iio_axi_dac_init_par);

#endif

	/* Memory deallocation for devices and spi */
	ad9144_remove(ad9144_device);
	ad9523_remove(ad9523_device);
	ad9680_remove(ad9680_device);

	/* Memory deallocation for PHY and LINK layers */
	adxcvr_remove(ad9144_xcvr);
	adxcvr_remove(ad9680_xcvr);
	axi_jesd204_tx_remove(ad9144_jesd);
	axi_jesd204_rx_remove(ad9680_jesd);

	/* Memory deallocation for gpios */
	gpio_remove(fmcdaq2.gpio_clkd_sync);
	gpio_remove(fmcdaq2.gpio_dac_reset);
	gpio_remove(fmcdaq2.gpio_dac_txen);
	gpio_remove(fmcdaq2.gpio_adc_pd);

	return SUCCESS;
}
