/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by the ad9088 examples.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

static struct no_os_spi_init_param adf4382_spi_ip = {
	.device_id = CLK_SPI_DEVICE_ID,
	.max_speed_hz = 1500000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS_CLK,
	.extra = SPI_EXTRA_CLK,
	.chip_select = SPI_CS_ADF4382,
};

static struct no_os_spi_init_param hmc7044_spi_ip = {
	.device_id = CLK_SPI_DEVICE_ID,
	.max_speed_hz = 1000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS_CLK,
	.extra = SPI_EXTRA_CLK,
	.chip_select = SPI_CS_HMC7044,
};

static struct no_os_spi_init_param adf4030_spi_ip = {
	.device_id = CLK_SPI_DEVICE_ID,
	.max_speed_hz = 1000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS_CLK,
	.extra = SPI_EXTRA_CLK,
	.chip_select = SPI_CS_ADF4030,
};

static struct no_os_spi_init_param ad9088_spi_ip = {
	.device_id = APOLLO_SPI_DEVICE_ID,
	.max_speed_hz = 13000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS_APOLLO,
	.extra = SPI_EXTRA_APOLLO,
	.chip_select = SPI_CS_APOLLO,
};

static struct no_os_gpio_init_param gpio_reset_ip = {
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
	.number = GPIO_OFFSET + GPIO_RESET_N,
};

struct adf4382_init_param adf4382_ip = {
	.spi_init = &adf4382_spi_ip,
	.spi_3wire_en = true, //MB is using 3-wire SPI
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

static struct hmc7044_chan_spec hmc7044_chans[] = {
	{
		.num = 1,		// ADF4030_REFIN
		.divider = 20,		// 125 MHz
		.driver_mode = 2,	// LVDS
	},
	{
		.num = 3,		// ADF4030_BSYNC0
		.divider = 512,		// retuned to 256 -> 9.765625 MHz
		.driver_mode = 1,	// LVPECL
		.is_sysref = true,
	},
	{
		.num = 8,		// CORE_CLK_TX
		.divider = 8,		// 312.5 MHz
		.driver_mode = 2,	// LVDS
	},
	{
		.num = 9,		// CORE_CLK_RX
		.divider = 8,		// 312.5 MHz
		.driver_mode = 2,	// LVDS
	},
	{
		.num = 10,		// FPGA_REFCLK
		.divider = 8,		// 312.5 MHz
		.driver_mode = 2,	// LVDS
	},
	{
		.num = 11,		// CORE_CLK_RX_B
		.divider = 8,		// 312.5 MHz
		.driver_mode = 2,	// LVDS
	},
	{
		.num = 12,		// CORE_CLK_TX_B
		.divider = 8,		// 312.5 MHz
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
	.num_channels = NO_OS_ARRAY_SIZE(hmc7044_chans),
	.pll1_ref_prio_ctrl = 0xE1,
	.pll1_ref_autorevert_en = true,
	.sync_pin_mode = 0x1,
	.high_performance_mode_clock_dist_en = false,
	.pulse_gen_mode = HMC7044_PULSE_GEN_CONT_PULSE,
	.channels = hmc7044_chans
};

/*
 * ADF4030 BSYNC distribution on this board. ch0 receives the HMC7044's BSYNC0
 * and is the auto-align reference; ch5 drives the Apollo's SYSREF pin; ch8
 * drives the FPGA's sysref_in. All three are realigned on JESD204
 * CLK_SYNC_STAGE4.
 *
 * All three channels are DC coupled with the receiver enabled, which is
 * RX_DC_COUPLED_CLKS in the no-OS termination enum.
 *
 * rcm_mv is left at 0, which skips the receiver common-mode write and keeps the
 * part's reset default. The comparator level this board would otherwise ask for
 * works out to 497 mV, below the driver's own ADF4030_RCM_VOLTAGE_MIN0 of 504,
 * so it is not reachable through adf4030_set_channel_voltage(). Revisit if the
 * BSYNC levels need trimming on the bench.
 */
static struct adf4030_chan_spec adf4030_chans[] = {
	{
		.num = ADF4030_CH_HMC_REF,	// ADF4030_SCLKOUT3, from HMC ch3
		.termination = RX_DC_COUPLED_CLKS,
		.tx_en = false,			// input
		.align_on_sync_en = true,
		.reference_chan = ADF4030_CH_HMC_REF,
	},
	{
		.num = ADF4030_CH_APOLLO_SYSREF,	// APOLLO_SYSREF
		.termination = RX_DC_COUPLED_CLKS,
		.tx_en = true,
		.align_on_sync_en = true,
		.reference_chan = ADF4030_CH_HMC_REF,
	},
	{
		.num = ADF4030_CH_FPGA_SYSREF,	// SYSREF_IN_F, to the FPGA
		.termination = RX_DC_COUPLED_CLKS,
		.tx_en = true,
		.align_on_sync_en = true,
		.reference_chan = ADF4030_CH_HMC_REF,
	}
};

struct adf4030_init_param adf4030_ip = {
	.spi_init = &adf4030_spi_ip,
	.spi_4wire_en = false,		// MB is using 3-wire SPI
	.cmos_3v3 = false,
	.ref_freq = ADF4030_REF_FREQ_HZ,
	.vco_freq = ADF4030_VCO_FREQ_HZ,
	.bsync_freq = AD9088_SYSREF_CLK_HZ,
	.ref_div = 1,
	.chip_addr = 0,
	/*
	 * 8 alignment iterations against a 1400 fs threshold. These are the
	 * values the alignment is known to converge with on this board.
	 *
	 * alignment_threshold_en is deliberately left false, so alignment
	 * converges on ALIGN_CYCLES alone rather than exiting early once the
	 * threshold is met. Enable it only if convergence needs the extra help.
	 */
	.alignment_threshold_fs = 1400,
	.alignment_iter = 8,
	.num_channels = NO_OS_ARRAY_SIZE(adf4030_chans),
	.channels = adf4030_chans,
};

struct axi_dmac_init rx_dmac_ip = {
	.name = "rx_dmac",
	.base = RX_DMA_BASEADDR,
	.irq_option = IRQ_DISABLED
};

struct axi_dmac_init tx_dmac_ip = {
	.name = "tx_dmac",
	.base = TX_DMA_BASEADDR,
	.irq_option = IRQ_DISABLED
};

/*
 * Only the clock rates are given here. The link parameters these structures
 * also carry are applied by axi_jesd204_{rx,tx}_init_legacy(); on the JESD204
 * FSM path the cores are configured from the link the FSM derives from the
 * device profile instead, so setting them here would document nothing.
 */
struct jesd204_rx_init rx_jesd204_ip = {
	.name = "rx_jesd",
	.base = RX_JESD_BASEADDR,
	.device_clk_khz = AD9088_DEVICE_CLK_KHZ,
	.lane_clk_khz = AD9088_LANE_RATE_KHZ
};

struct jesd204_tx_init tx_jesd204_ip = {
	.name = "tx_jesd",
	.base = TX_JESD_BASEADDR,
	.device_clk_khz = AD9088_DEVICE_CLK_KHZ,
	.lane_clk_khz = AD9088_LANE_RATE_KHZ
};

struct adxcvr_init tx_adxcvr_ip = {
	.name = "tx_adxcvr",
	.base = TX_XCVR_BASEADDR,
	.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
	.out_clk_sel = ADXCVR_PROGDIV_CLK,
	.lpm_enable = 0,
	.lane_rate_khz = AD9088_LANE_RATE_KHZ,
	.ref_rate_khz = 312500,
	.export_no_os_clk = true,
};

struct adxcvr_init rx_adxcvr_ip = {
	.name = "rx_adxcvr",
	.base = RX_XCVR_BASEADDR,
	.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
	.out_clk_sel = ADXCVR_PROGDIV_CLK,
	.lpm_enable = 1,
	.lane_rate_khz = AD9088_LANE_RATE_KHZ,
	.ref_rate_khz = 312500,
	.export_no_os_clk = true,
};

struct axi_adc_init rx_adc_init = {
	.name = "rx_adc",
	.base = RX_CORE_BASEADDR,
	.num_channels = 0,
};

struct axi_dac_init tx_dac_init = {
	.name = "tx_dac",
	.base = TX_CORE_BASEADDR,
	.num_channels = 0,
};

/*
 * MCS calibration accessors.
 *
 * The AD9088 driver drives the SYSREF provider and the device clock PLL during
 * MCS calibration but must not know which chips those are, so the board adapts
 * its own to the driver's callbacks here. On this board the provider is the
 * ADF4030 channel feeding the AD9088's SYSREF pin, and the clock PLL is the
 * ADF4382. Handles are filled in by ad9088_mcs_ops_bind() once both are probed.
 */
static struct adf4030_dev *mcs_bsync_dev;
static struct adf4382_dev *mcs_clk_dev;

static int mcs_bsync_freq_get(void *ctx, uint32_t *freq_hz)
{
	return adf4030_get_bsync_freq((struct adf4030_dev *)ctx, freq_hz, false);
}

static int mcs_bsync_output_en_set(void *ctx, bool en)
{
	return adf4030_set_channel_direction((struct adf4030_dev *)ctx,
					     ADF4030_CH_APOLLO_SYSREF, en);
}

/*
 * Time difference of the SYSREF channel from its alignment reference. Arms the
 * TDC, which asserts MANUAL_MODE; the caller's following delay_set() clears it
 * again via the alignment write.
 */
static int mcs_bsync_tdc_measure(void *ctx, int64_t *tdc_fs)
{
	struct adf4030_dev *dev = (struct adf4030_dev *)ctx;
	uint8_t reference_chan;
	int ret;

	reference_chan = dev->channels[ADF4030_CH_APOLLO_SYSREF].reference_chan;

	ret = adf4030_set_tdc_source(dev, reference_chan);
	if (ret)
		return ret;

	ret = adf4030_set_tdc_measurement(dev, ADF4030_CH_APOLLO_SYSREF);
	if (ret)
		return ret;

	return adf4030_get_tdc_measurement(dev, tdc_fs);
}

/*
 * Both SYSREF channels carry the delay MCS asks for, not just the Apollo's.
 *
 * MCS measures the BSYNC path delay to the Apollo (~2.2 ns on this board) and
 * asks for it to be compensated. Applying that to the Apollo channel alone
 * leaves the FPGA's SYSREF where it was, so the two end up ~2.2 ns apart on a
 * 3.2 ns link clock -- 68% of a period, close enough to a link-clock boundary
 * that the receiver resolves its LEMC to a different edge from one bring-up to
 * the next. That shows up as an intermittently misaligned capture: the link
 * still reaches DATA and the DMA still completes, but the samples come back at
 * the wrong frame phase and read as noise. Roughly one bring-up in three.
 *
 * Delaying both keeps their relative phase exactly as it was before MCS ran,
 * which is the part the receiver depends on. Note this is the Apollo's path
 * delay, not the FPGA's -- it preserves the ch5<->ch8 relationship rather than
 * absolutely compensating the FPGA's own SYSREF trace. Absolute compensation
 * would need a separate TDC measurement on the FPGA channel.
 */
static const uint8_t mcs_bsync_sysref_chans[] = {
	ADF4030_CH_APOLLO_SYSREF,
	ADF4030_CH_FPGA_SYSREF,
};

static int mcs_bsync_delay_set(void *ctx, int64_t delay_fs)
{
	struct adf4030_dev *dev = (struct adf4030_dev *)ctx;
	uint8_t reference_chan;
	uint8_t chan;
	unsigned int i;
	int ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(mcs_bsync_sysref_chans); i++) {
		chan = mcs_bsync_sysref_chans[i];

		ret = adf4030_set_channel_delay(dev, chan, delay_fs);
		if (ret)
			return ret;

		reference_chan = dev->channels[chan].reference_chan;

		/*
		 * The delay only takes effect once the channel is realigned.
		 * Realigned one at a time rather than through
		 * adf4030_set_serial_alignment(): the single-channel call polls
		 * FSM_BUSY to completion, so the delay is in effect before the
		 * MCS init cal that follows reads the edge back.
		 */
		ret = adf4030_set_tdc_source(dev, reference_chan);
		if (ret)
			return ret;

		ret = adf4030_set_single_ch_alignment(dev, chan);
		if (ret)
			return ret;
	}

	return 0;
}

static int mcs_bsync_bg_align_set(void *ctx, bool en)
{
	struct adf4030_dev *dev = (struct adf4030_dev *)ctx;
	uint16_t mask = 0;
	uint8_t i;

	/* Background alignment covers exactly the channels synced on link-up. */
	if (en)
		for (i = 0; i < ADF4030_CHANNEL_NUMBER; i++)
			if (dev->channels[i].align_on_sync_en)
				mask |= NO_OS_BIT(i);

	return adf4030_set_background_serial_alignment(dev, mask);
}

static int mcs_clk_auto_align_set(void *ctx, bool en)
{
	return adf4382_set_auto_align((struct adf4382_dev *)ctx, en);
}

static int mcs_clk_phase_set_fs(void *ctx, int32_t phase_fs)
{
	struct adf4382_dev *dev = (struct adf4382_dev *)ctx;
	int ret;

	/* The magnitude goes in the register, the sign in the polarity bit. */
	ret = adf4382_set_phase_pol(dev, phase_fs < 0);
	if (ret)
		return ret;

	return adf4382_set_phase_adjust(dev, (uint32_t)(phase_fs < 0 ?
					-(int64_t)phase_fs : phase_fs));
}

static struct ad9088_bsync_ops ad9088_bsync_ops = {
	.freq_get = mcs_bsync_freq_get,
	.output_en_set = mcs_bsync_output_en_set,
	.tdc_measure = mcs_bsync_tdc_measure,
	.delay_set = mcs_bsync_delay_set,
	.bg_align_set = mcs_bsync_bg_align_set,
};

static struct ad9088_clk_ops ad9088_clk_ops = {
	.auto_align_set = mcs_clk_auto_align_set,
	.phase_set_fs = mcs_clk_phase_set_fs,
};

int ad9088_mcs_ops_bind(struct adf4030_dev *adf4030,
			struct adf4382_dev *adf4382)
{
	if (!adf4030 || !adf4382)
		return -EINVAL;

	mcs_bsync_dev = adf4030;
	mcs_clk_dev = adf4382;

	ad9088_bsync_ops.ctx = mcs_bsync_dev;
	ad9088_clk_ops.ctx = mcs_clk_dev;

	/* Publishing the tables is what enables MCS calibration. */
	ad9088_ip.bsync_ops = &ad9088_bsync_ops;
	ad9088_ip.clk_ops = &ad9088_clk_ops;

	return 0;
}

/*
 * The device profile is not selected here: it is linked into the image from the
 * binary named by FIRMWARE in the project Makefile, so pick one with
 * `make FIRMWARE=<path to a profile .bin>`.
 */
struct ad9088_init_param ad9088_ip = {
	.spi_init = &ad9088_spi_ip,
	.gpio_reset = &gpio_reset_ip,
	.gpio_tri_req = NULL,
	.spi_3wire_en = false,
	.trig_sync_en = false,
	.nyquist_zone = AD9088_NYQUIST_ZONE,
	.subclass = AD9088_JESD_SUBCLASS,
	.jtx0_logical_lane_mapping = AD9088_TX0_LOGICAL_LANE_MAPPING,
	.jtx1_logical_lane_mapping = AD9088_TX1_LOGICAL_LANE_MAPPING,
	.jrx0_physical_lane_mapping = AD9088_RX0_PHYSICAL_LANE_MAPPING,
	.jrx1_physical_lane_mapping = AD9088_RX1_PHYSICAL_LANE_MAPPING,
	.jtx_ser_amplitude = ADI_APOLLO_JESD_DRIVE_SWING_VTT_100,
	.jtx_ser_pre_emphasis = ADI_APOLLO_JESD_PRE_TAP_LEVEL_6_DB,
	.jtx_ser_post_emphasis = ADI_APOLLO_JESD_POST_TAP_LEVEL_3_DB,
	/*
	 * bsync_ops/clk_ops stay NULL until ad9088_mcs_ops_bind() runs, so MCS
	 * calibration is skipped in examples that never call it. That is the
	 * right default: without the clock-chip handles the accessors have
	 * nothing to drive.
	 */
	.mcs_track_decimation = 0,	/* use the driver default */
	.mcs_track_win = 0,		/* keep the device profile's window */
	/*
	 * The one-shot alignment performed during clock sync is enough here, so
	 * the provider is not left realigning in the background. Note it would
	 * not undo the deskew MCS programs onto the Apollo's SYSREF channel
	 * either: the provider aligns each channel to its reference plus that
	 * channel's own delay, so background alignment preserves a programmed
	 * offset rather than removing it.
	 */
	.aion_background_serial_alignment_en = false,
};
