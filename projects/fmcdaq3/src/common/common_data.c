/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by fmcdaq3 examples.
 *   @author 
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

#include "common_data.h"

struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = UART_EXTRA,
		.platform_ops = UART_OPS
};

struct no_os_spi_init_param ad9528_spi_param = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 2000000u,
	.chip_select = 0,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.mode = NO_OS_SPI_MODE_0
};

struct no_os_spi_init_param ad9152_spi_param = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 2000000u,
	.chip_select = 1,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.mode = NO_OS_SPI_MODE_0
};

struct no_os_spi_init_param ad9680_spi_param = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 2000000u,
	.chip_select = 2,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.mode = NO_OS_SPI_MODE_0
};

struct no_os_gpio_init_param dac_txen_param = {
	.number = GPIO_DAC_TXEN,
	.extra = GPIO_EXTRA,
	.platform_ops = GPIO_OPS
};

struct no_os_gpio_init_param adc_pd_param = {
	.number = GPIO_ADC_PD,
	.extra = GPIO_EXTRA,
	.platform_ops = GPIO_OPS
};

struct ad9528_channel_spec ad9528_channels[] = {
	// dac-device-clock (1.233G)
	{
		.channel_num = 2,
		.channel_divider = 1,
	},
	//adc sysref (4.9M)
	{
		.channel_num = 8,
		.channel_divider = 256,
	},
	// adc-fpga-clock (616.5M)
	{
		.channel_num = 9,
		.channel_divider = 2,
	},
	// adc dev sysref (4.9M)
	{
		.channel_num = 7,
		.channel_divider = 256,
	},
	// adc-device-clock (1.233G)
	{
		.channel_num = 13,
		.channel_divider = 1,
	},
	// dac sysref (4.9M)
	{
		.channel_num = 5,
		.channel_divider = 256,
	},
	// dac-fpga-fmc (616.5M)
	{
		.channel_num = 4,
		.channel_divider = 2,
	},
	// dac dev sysref (4.9M)
	{
		.channel_num = 6,
		.channel_divider = 256,
	}
};

struct ad9528_platform_data ad9528_pdata = {
	.spi3wire = 1,
	.vcxo_freq = 100000000,
	.osc_in_diff_en = 1,
	.pll2_charge_pump_current_nA = 35000,
	.pll2_vco_div_m1 = 3,
	.pll2_r1_div = 3,
	.pll2_ndiv_a_cnt = 3,
	.pll2_ndiv_b_cnt = 27,
	.pll2_n2_div = 37,
	.sysref_k_div = 128,
	.rpole2 = RPOLE2_900_OHM,
	.rzero = RZERO_1850_OHM,
	.cpole1 = CPOLE1_16_PF,
	.sysref_pattern_mode = SYSREF_PATTERN_CONTINUOUS,
	.sysref_nshot_mode = SYSREF_NSHOT_4_PULSES,
	.sysref_req_en = true,
	.pll1_bypass_en = true,
	.pll2_bypass_en = false,
	.num_channels = 8,
	.channels = ad9528_channels,
};

struct ad9528_init_param ad9528_param = {
	.gpio_resetb = NULL,
	.pdata = &ad9528_pdata,
};

struct axi_dac_channel ad9152_channels[] = {
	{
		.dds_dual_tone = 0,
		.dds_frequency_0 = 33*1000*1000,
		.dds_phase_0 = 0,
		.dds_scale_0 = 500000,
		.pat_data = 0xb1b0a1a0,
		.sel = AXI_DAC_DATA_SEL_DDS,
	},
	{
		.dds_dual_tone = 0,
		.dds_frequency_0 = 11*1000*1000,
		.dds_phase_0 = 0,
		.dds_scale_0 = 500000,
		.pat_data = 0xd1d0c1c0,
		.sel = AXI_DAC_DATA_SEL_DDS,
	},
};

/* ADC Core */
struct axi_adc_init ad9680_core_param = {
	.name = "ad9680_adc",
	.base = RX_CORE_BASEADDR,
	.num_channels = 2
};

struct axi_dac_init ad9152_core_param = {
	.name = "ad9152_dac",
	.base =	TX_CORE_BASEADDR,
	.num_channels = 2,
	.channels = ad9152_channels,
	.rate = 3
};

struct axi_dmac_init ad9680_dmac_param = {
	.name = "ad9680_dmac",
	.base = RX_DMA_BASEADDR,
	.irq_option = IRQ_DISABLED
};

struct axi_dmac_init ad9152_dmac_param = {
	.name = "ad9152_dmac",
	.base = TX_DMA_BASEADDR,
	.irq_option = IRQ_DISABLED
};

struct ad9152_init_param ad9152_param = {
	.interpolation = 1,
	.lane_rate_kbps = 12330000,
	.prbs_type = AD9152_TEST_PN7,
};

struct ad9680_init_param ad9680_param = {
	.lane_rate_kbps = 12330000,
};

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

struct jesd204_rx_init ad9680_jesd_param = {
	.name = "ad9680_jesd",
	.base = RX_JESD_BASEADDR,
	.octets_per_frame = 1,
	.frames_per_multiframe = 32,
	.subclass = 1,
	.device_clk_khz = 12330000 / 40,
	.lane_clk_khz = 12330000
};
