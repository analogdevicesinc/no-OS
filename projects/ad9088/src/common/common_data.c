/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by adf4382 examples.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param platform_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_spi_init_param adf4382_spi_ip = {
	.device_id = CLK_SPI_DEVICE_ID,
	.max_speed_hz = 1500000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS_CLK,
	.extra = SPI_EXTRA_CLK,
	.chip_select = SPI_CS_ADF4382,
};

struct no_os_spi_init_param hmc7044_spi_ip = {
	.device_id = CLK_SPI_DEVICE_ID,
	.max_speed_hz = 1000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS_CLK,
	.extra = SPI_EXTRA_CLK,
	.chip_select = SPI_CS_HMC7044,
};

struct no_os_spi_init_param ad9088_spi_ip = {
	.device_id = APOLLO_SPI_DEVICE_ID,
	.max_speed_hz = 13000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS_CLK,
	.extra = SPI_EXTRA_APOLLO,
	.chip_select = SPI_CS_APOLLO,
};

struct no_os_gpio_init_param gpio_reset_ip = {
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
	.number = GPIO_OFFSET + GPIO_RESET_N,
};

struct adf4382_init_param adf4382_ip = {
	.spi_init = &adf4382_spi_ip,
	.spi_3wire_en = false,
	.cmos_3v3 = false,
	.ref_freq_hz = 125000000,
	.freq = 20000000000,
	.ref_doubler_en = 0,
	.ref_div = 1,
	.cp_i = 15,
	.bleed_word = 0,
	.ld_count = 10,
	.id = ID_ADF4382A,
};


struct hmc7044_chan_spec chan_spec[] = {
	{
		.num = 3,		// FPGA_SYSREF
		.divider = 512,		// 4.882
		.driver_mode = 2,	// LVDS
	},
	{
		.num = 8,		// CORE_CLK_TX
		.divider = 8,		// 156.25
		.driver_mode = 2,	// LVDS
	},
	{
		.num = 9,		// CORE_CLK_RX
		.divider = 8,		// 156.25
		.driver_mode = 2,	// LVDS
	},
	{
		.num = 10,		// FPGA_REFCLK
		.divider = 8,		// 156.25
		.driver_mode = 2,	// LVDS
	},
	{
		.num = 11,		// CORE_CLK_RX_B
		.divider = 8,		// 156.25
		.driver_mode = 2,	// LVDS
	},
	{
		.num = 12,		// CORE_CLK_TX_B
		.divider = 8,		// 156.25
		.driver_mode = 2,	// LVDS
	}
};

struct hmc7044_init_param hmc7044_ip = {
	.spi_init = &hmc7044_spi_ip,
	.clkin_freq = {125000000, 125000000, 125000000, 125000000},
	.vcxo_freq = 125000000,
	.pll2_freq = 2500000000,
	.pll1_loop_bw = 200,
	.sysref_timer_div = 1024,
	.in_buf_mode = {0x07, 0x07, 0x00, 0x00, 0x5},
	.gpi_ctrl = {0x00, 0x00, 0x00, 0x00},
	.gpo_ctrl = {0x37, 0x33, 0x00, 0x00},
	.num_channels = sizeof(chan_spec) /
	sizeof(struct hmc7044_chan_spec),
	.pll1_ref_prio_ctrl = 0xE1,
	.pll1_ref_autorevert_en = true,
	.sync_pin_mode = 0x1,
	.high_performance_mode_clock_dist_en = false,
	.pulse_gen_mode = HMC7044_PULSE_GEN_CONT_PULSE,
	.channels = chan_spec
};

struct axi_dmac_init rx_dmac_ip = {
	"rx_dmac",
	RX_DMA_BASEADDR,
	IRQ_DISABLED
};

struct axi_dmac_init tx_dmac_ip = {
	"tx_dmac",
	TX_DMA_BASEADDR,
	IRQ_DISABLED
};

struct jesd204_rx_init rx_jesd204_ip = {
	.name = "rx_jesd",
	.base = RX_JESD_BASEADDR,
	.octets_per_frame = AD9088_RX_JESD_F,
	.frames_per_multiframe = AD9088_RX_JESD_K,
	.subclass = AD9088_RX_JESD_SUBCLASS,
	.device_clk_khz = AD9088_DEVICE_CLK_KHZ,
	.lane_clk_khz = AD9088_LANE_RATE_KHZ	
};

struct jesd204_tx_init tx_jesd204_ip = {
	.name = "tx_jesd",
	.base = TX_JESD_BASEADDR,
	.octets_per_frame = AD9088_TX_JESD_F,
	.frames_per_multiframe = AD9088_TX_JESD_K,
	.converters_per_device = AD9088_TX_JESD_M,
	.converter_resolution = AD9088_TX_JESD_N,
	.bits_per_sample = AD9088_TX_JESD_NP,
	.high_density = AD9088_TX_JESD_HD,
	.control_bits_per_sample = AD9088_TX_JESD_CS,
	.subclass = AD9088_TX_JESD_SUBCLASS,
	.device_clk_khz = AD9088_DEVICE_CLK_KHZ,
	.lane_clk_khz = AD9088_LANE_RATE_KHZ
};

struct ad9088_init_param ad9088_ip = {
	.spi_init = &ad9088_spi_ip, // to be set by the user
	.gpio_reset = &gpio_reset_ip, // to be set by the user
	.gpio_tri_req = NULL, // to be set by the user
	.gpio_rx1_en = NULL, // to be set by the user
	.gpio_rx2_en = NULL, // to be set by the user
	.gpio_tx1_en = NULL, // to be set by the user
	.gpio_tx2_en = NULL, // to be set by the user
	.versal_xvr_reset = NULL, // to be set by the user
	.spi_3wire_en = false,
	.rx_real_channel_en = false,
	.tx_real_channel_en = false,
	.side_b_use_own_tpl_en = false,
	.multidevice_instance_count = AD9088_MULTIDEVICE_INST_CNT,
	.trig_sync_en = false,
	.standalone_en = false,
	.nyquist_zone = AD9088_NYQUIST_ZONE,
	.subclass = AD9088_TX_JESD_SUBCLASS,
	.jtx0_logical_lane_mapping = AD9088_LOGICAL_LANE_MAPPING,
	.jtx1_logical_lane_mapping = AD9088_LOGICAL_LANE_MAPPING,
	.jrx0_logical_lane_mapping = AD9088_LOGICAL_LANE_MAPPING,
	.jrx1_logical_lane_mapping = AD9088_LOGICAL_LANE_MAPPING,
	.jtx_ser_amplitude = ADI_APOLLO_JESD_DRIVE_SWING_VTT_100,
	.jtx_ser_pre_emphasis = ADI_APOLLO_JESD_PRE_TAP_LEVEL_6_DB,
	.jtx_ser_post_emphasis = ADI_APOLLO_JESD_POST_TAP_LEVEL_3_DB
};
