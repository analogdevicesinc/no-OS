/***************************************************************************//**
 *   @file   ad_fmcdaq3_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
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

#include "platform_drivers.h"
#include "ad9152.h"
#include "ad9528.h"
#include "ad9680.h"
#include "adc_core.h"
#include "dac_core.h"
#include "dmac_core.h"
#include "dac_buffer.h"
#include "xcvr_core.h"
#include "jesd_core.h"
#include <stdio.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define GPIO_XCVR_TX_RESET	41
#define GPIO_XCVR_RX_RESET	40
#define GPIO_CLKD_STATUS_0      32
#define GPIO_CLKD_STATUS_1      33
#define GPIO_DAC_IRQ            34
#define GPIO_ADC_FDA            35
#define GPIO_ADC_FDB            36
#define GPIO_DAC_TXEN           37
#define GPIO_ADC_PD             38
#define GPIO_TRIG               39

/******************************************************************************/
/********************** Main **************************************************/
/******************************************************************************/

int main(void) {

	struct spi_device		ad9528_spi_device;
	struct spi_device		ad9152_spi_device;
	struct spi_device		ad9680_spi_device;
	struct ad9528_channel_spec	ad9528_channels[8];
	struct ad9528_platform_data	ad9528_param;
	struct ad9152_init_param	ad9152_param;
	struct ad9680_init_param	ad9680_param;
	struct xcvr_core_phy		daq3_xcvr;
	struct xcvr_core		ad9152_xcvr;
	struct xcvr_core		ad9680_xcvr;
	struct jesd_core		ad9152_jesd;
	struct jesd_core		ad9680_jesd;
	struct dac_channel		ad9152_channels[2];
	struct dac_core			ad9152_core;
	struct adc_core			ad9680_core;
	struct dmac_xfer		ad9680_dma_xfer;
	struct dmac_xfer		ad9152_dma_xfer;
	struct dmac_core		ad9152_dma;
	struct dmac_core		ad9680_dma;

	// base addresses

#ifdef XILINX
	daq3_xcvr.base_address = XPAR_AXI_DAQ3_XCVR_BASEADDR;
	ad9152_core.base_address = XPAR_AXI_AD9152_CORE_BASEADDR;
	ad9152_jesd.base_address = XPAR_AXI_AD9152_JESD_TX_AXI_BASEADDR;
	ad9152_dma.base_address = XPAR_AXI_AD9152_DMA_BASEADDR;
	ad9680_core.base_address = XPAR_AXI_AD9680_CORE_BASEADDR;
	ad9680_jesd.base_address = XPAR_AXI_AD9680_JESD_RX_AXI_BASEADDR;
	ad9680_dma.base_address = XPAR_AXI_AD9680_DMA_BASEADDR;
#endif
#ifdef ALTERA
	ad9152_xcvr.base_address = AXI_AD9152_XCVR_BASE;
	ad9680_xcvr.base_address = AXI_AD9680_XCVR_BASE;
	ad9152_xcvr.mmcm_lpll_base_address = AVL_AD9152_XCVR_CORE_PLL_RECONFIG_BASE;
	ad9680_xcvr.mmcm_lpll_base_address = AVL_AD9680_XCVR_CORE_PLL_RECONFIG_BASE;
	ad9152_xcvr.tx_lane_pll_base_address = AVL_AD9152_XCVR_LANE_PLL_RECONFIG_BASE;
	ad9152_core.base_address = AXI_AD9152_CORE_BASE;
	ad9152_jesd.base_address = AVL_AD9152_XCVR_IP_RECONFIG_BASE;
	ad9152_dma.base_address = AXI_AD9152_DMA_BASE;
	ad9680_core.base_address = AXI_AD9680_CORE_BASE;
	ad9680_jesd.base_address = AVL_AD9680_XCVR_IP_RECONFIG_BASE;
	ad9680_dma.base_address = AXI_AD9680_DMA_BASE;
#endif

	//********************************************************************************
	// SPI interface configuration
	//********************************************************************************

	ad_spi_init(&ad9528_spi_device);
	ad_spi_init(&ad9152_spi_device);
	ad_spi_init(&ad9680_spi_device);

	ad9528_spi_device.chip_select = 0x6;
	ad9152_spi_device.chip_select = 0x5;
	ad9680_spi_device.chip_select = 0x3;

	//********************************************************************************
	// clock distribution device (AD9528) configuration
	//********************************************************************************

	ad9528_param.num_channels = 8;
	ad9528_param.channels = &ad9528_channels[0];
	ad9528_init(&ad9528_param);

	// dac-device-clock (1.25G) & sysref (2.44M)

	ad9528_channels[0].channel_num = 2;
	ad9528_channels[0].channel_divider = 1;
	ad9528_channels[1].channel_num = 8;
	ad9528_channels[1].channel_divider = 512;

	// dac-fpga-clock (625M) & sysref (2.44M)

	ad9528_channels[2].channel_num = 9;
	ad9528_channels[2].channel_divider = 2;
	ad9528_channels[3].channel_num = 7;
	ad9528_channels[3].channel_divider = 512;

	// adc-device-clock (1.2G) & sysref (2.44M)

	ad9528_channels[4].channel_num = 13;
	ad9528_channels[4].channel_divider = 1;
	ad9528_channels[5].channel_num = 5;
	ad9528_channels[5].channel_divider = 512;

	// adc-fpga-clock (625M) & sysref (2.44M)

	ad9528_channels[6].channel_num = 4;
	ad9528_channels[6].channel_divider = 2;
	ad9528_channels[7].channel_num = 6;
	ad9528_channels[7].channel_divider = 512;

	// pllx settings

	ad9528_param.spi3wire = 1;
	ad9528_param.vcxo_freq = 125000000;
	ad9528_param.osc_in_diff_en = 1;
	ad9528_param.pll2_charge_pump_current_nA = 805000;
	ad9528_param.pll2_vco_diff_m1 = 3;
	ad9528_param.pll2_ndiv_a_cnt = 2;
	ad9528_param.pll2_ndiv_b_cnt = 7;
	ad9528_param.pll2_n2_div = 10;
	ad9528_param.sysref_k_div = 512;
	ad9528_param.rpole2 = RPOLE2_900_OHM;
	ad9528_param.rzero= RZERO_3250_OHM;
	ad9528_param.cpole1 = CPOLE1_16_PF;

	//********************************************************************************
	// DAC (AD9152) and the transmit path (JESD-PHY, JESD-IP, AXI_AD9152, TX DMAC)
	//********************************************************************************

	ad9152_param.lane_rate_kbps = 12500000;
	ad9152_param.interpolation = 1;

	ad9152_xcvr.gpio_reset = GPIO_XCVR_TX_RESET;
	ad9152_xcvr.link_pll_present = 0;
	ad9152_xcvr.tx_or_rx_n = 1;
	ad9152_xcvr.no_of_phys = 1;
	ad9152_xcvr.phys = &daq3_xcvr;

	ad9152_jesd.tx_or_rx_n = 1;
	ad9152_jesd.octets_per_frame = 1;
	ad9152_jesd.frames_per_multiframe = 32;

	ad9152_core.no_of_channels = 2;
	ad9152_core.channels = &ad9152_channels[0];
	dac_channel_init(&ad9152_core);

	ad9152_dma.transfer = &ad9152_dma_xfer;
	ad9152_dma.flags = DMAC_FLAGS_TLAST;
	dmac_init(&ad9152_dma, DMAC_TX);

	//********************************************************************************
	// ADC (AD9680) and the receive path (JESD-PHY, JESD-IP, AXI_AD9680, RX DMAC)
	//********************************************************************************

	ad9680_param.lane_rate_kbps = 12500000;

	ad9680_xcvr.gpio_reset = GPIO_XCVR_RX_RESET;
	ad9680_xcvr.link_pll_present = 0;
	ad9680_xcvr.tx_or_rx_n = 0;
	ad9680_xcvr.no_of_phys = 1;
	ad9680_xcvr.phys = &daq3_xcvr;

	ad9680_jesd.tx_or_rx_n = 0;
	ad9680_jesd.octets_per_frame = 1;
	ad9680_jesd.frames_per_multiframe = 32;

	ad9680_core.no_of_channels = 2;
	ad9680_core.resolution = 14;

	ad9680_dma.transfer = &ad9680_dma_xfer;
	dmac_init(&ad9680_dma, DMAC_RX);

	//********************************************************************************
	// Execution Phase (do NOT modify below)
	//********************************************************************************

	// setup GPIOs

	ad_platform_init();
	ad_gpio_set(GPIO_XCVR_TX_RESET, 1);
	ad_gpio_set(GPIO_XCVR_RX_RESET, 1);
	ad_gpio_set(GPIO_DAC_TXEN, 0x1);
	ad_gpio_set(GPIO_ADC_PD, 0x0);

	// setup spi device(s)

	ad9528_setup(&ad9528_spi_device, &ad9528_param);
	ad9680_setup(&ad9680_spi_device, &ad9680_param);
	ad9152_setup(&ad9152_spi_device, &ad9152_param);

	// setup fpga peripherals

	jesd_setup(&ad9152_jesd);
	jesd_setup(&ad9680_jesd);
	xcvr_setup(&ad9152_xcvr);
	xcvr_setup(&ad9680_xcvr);
	mdelay(10);

	xcvr_status(&ad9152_xcvr);
	xcvr_status(&ad9680_xcvr);
	jesd_status(&ad9152_jesd);
	jesd_status(&ad9680_jesd);

	adc_setup(&ad9680_core);
	dac_setup(&ad9152_core);

	ad9152_status(&ad9152_spi_device);

	// AD9152 (short pattern test)

	ad9152_param.stpl_samples[0][0] = (ad9152_channels[0].pat_data>> 0) & 0xffff;
	ad9152_param.stpl_samples[0][1] = (ad9152_channels[0].pat_data>>16) & 0xffff;
	ad9152_param.stpl_samples[0][2] = (ad9152_channels[0].pat_data>> 0) & 0xffff;
	ad9152_param.stpl_samples[0][3] = (ad9152_channels[0].pat_data>>16) & 0xffff;
	ad9152_param.stpl_samples[1][0] = (ad9152_channels[1].pat_data>> 0) & 0xffff;
	ad9152_param.stpl_samples[1][1] = (ad9152_channels[1].pat_data>>16) & 0xffff;
	ad9152_param.stpl_samples[1][2] = (ad9152_channels[1].pat_data>> 0) & 0xffff;
	ad9152_param.stpl_samples[1][3] = (ad9152_channels[1].pat_data>>16) & 0xffff;
	dac_set_source(&ad9152_core, -1, DAC_SRC_SED);
	ad9152_short_pattern_test(&ad9152_spi_device, &ad9152_param);

	// AD9152 (dma source)

	dac_set_source(&ad9152_core, -1, DAC_SRC_DMA);
	dac_buffer_load(&ad9152_core, &ad9152_dma_xfer);
	dmac_start_transaction(&ad9152_dma);

	// AD9680 (PN9 data path test)

	ad9680_test(&ad9680_spi_device, AD9680_TEST_PN9);
	adc_pn_mon(&ad9680_core, ADC_PN9);

	// AD9680 (PN23 data path test)

	ad9680_test(&ad9680_spi_device, AD9680_TEST_PN23);
	adc_pn_mon(&ad9680_core, ADC_PN23A);

	// AD9680 (dma caputure)

	ad9680_test(&ad9680_spi_device, AD9680_TEST_OFF);
	dmac_start_transaction(&ad9680_dma);

	// eye-scan

	xcvr_eyescan_init(&ad9680_xcvr);
	xcvr_eyescan(&ad9680_xcvr);

	ad_printf("done.\n");
	ad_platform_close();
	return(0);
}
