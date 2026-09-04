/***************************************************************************//**
 *   @file   dma_example.c
 *   @brief  DMA capture example for the ad9088 project
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "dma_example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "ad9088.h"
#include "adi_apollo_clk_mcs.h"
#include "jesd204.h"
#include "axi_adxcvr.h"
#include "axi_dmac.h"
#include "no_os_axi_io.h"
#include "jesd204_clk.h"
#include "parameters.h"

/*
 * Sides the example drives.
 *
 * The board's HDL is built ASYMMETRIC_A_B_MODE=0, so both sides share one JESD
 * core, one transport layer and one DMA: the core reports NUM_LINKS=2 over 16
 * lanes and the TPL 8 converters. A capture is therefore both sides at once,
 * side A's converters first and side B's after them, and side B reaches it
 * only when its own link pair is brought up alongside side A's. There is no
 * second chain to point at -- `_B` register bases exist only in the split
 * .orig bitstream.
 */
#define LOOPBACK_SIDES		2

/*
 * Which side the capture report points at. It selects nothing about the
 * configuration: every side the profile has in use is tuned, brought up and
 * captured either way. Side A's window starts at converter 0, side B's at
 * side A's converter count.
 */
#define LOOPBACK_SIDE		0

/*
 * Most converters this example handles per side. The I/Q pairing below assumes
 * it, so a link carrying more is rejected rather than captured half-wrong.
 */
#define LOOPBACK_CONVERTERS	4

/* Widest capture: every side's converters land in the one buffer. */
#define CAPTURE_CONVERTERS	(LOOPBACK_CONVERTERS * LOOPBACK_SIDES)

/*
 * Default coarse NCO, as a divisor of the DAC rate. The fractional part of the
 * frequency tuning word is discarded unless dual modulus mode is on (see
 * adi_ad9088_calc_nco_ftw()), so only frequencies that divide the DAC rate
 * exactly are tuned without residual error -- a power of two guarantees it.
 */
#define DEFAULT_CNCO_RATE_DIV	8
#define DEFAULT_FNCO_HZ		0

/*
 * The TX data offload replays all of its memory regardless of how much was
 * written into it, so anything left unwritten comes back as noise. This is a
 * ceiling for the static buffer below; the depth the offload actually has is
 * read back from its memory size register at runtime and the transfer clamped
 * to the smaller of the two.
 */
#define TX_OFFLOAD_MAX_BYTES		(512 * 1024)

/*
 * axi_data_offload register map, the part of it this example needs, from
 * data_offload_regmap.v. MEMORY_SIZE_LSB is the storage depth in bytes, read
 * only; RESETN_OFFLOAD bit 0 low holds the IP in reset.
 */
#define AXI_DO_REG_MEMORY_SIZE_LSB	0x0014
#define AXI_DO_REG_RESETN_OFFLOAD	0x0084

/*
 * Width of the TX DMAC source AXI data path in bytes, which has to match the
 * HDL build. The driver rejects a transfer that is not a multiple of it.
 */
#define DMA_SRC_WIDTH_BYTES	128
#define DMA_BUFFER_ALIGN	1024

/*
 * Bring the receive link back if MCS calibration knocks it out.
 *
 * MCS runs at OPT_POST_SETUP_STAGE1, one stage after LINK_RUNNING has already
 * waited for the receiver to reach DATA. It programs the measured BSYNC path
 * delay onto the provider channel feeding the Apollo and realigns that channel
 * alone; the channel feeding the FPGA is left where it was, so the two SYSREFs
 * separate by the path delay -- 2.2 ns here, most of a 3.2 ns link clock. The
 * Apollo re-times off the edge that moved and the receiver does not, which is
 * what drops it to WAIT_BS.
 */
#define RX_LINK_RECOVER		1
#define RX_LINK_ATTEMPTS	3

/*
 * Receive core link status, from axi_jesd204_rx.c. Carried here because the
 * driver exports no accessor for the state -- axi_jesd204_rx_status_read()
 * prints it and returns 0 regardless.
 */
#define JESD204_RX_REG_LINK_STATUS	0x280
#define JESD204_RX_LINK_STATUS_MASK	0x3
#define JESD204_RX_LINK_STATUS_DATA	3

/*
 * Static capture buffer, sized for the widest capture this example accepts --
 * every side at its full converter count. Each side's count is read back from
 * its link at runtime and rejected if it exceeds LOOPBACK_CONVERTERS, so the
 * depth per converter is always the full ADC_BUFFER_SAMPLES.
 */
static uint16_t adc_buffer_dma[ADC_BUFFER_SAMPLES * CAPTURE_CONVERTERS]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

/*
 * Sized to cover the whole TX offload memory, see TX_OFFLOAD_MAX_BYTES. Held as
 * 32-bit words so a sine table entry can be written straight into it.
 */
static uint32_t dac_buffer_dma[TX_OFFLOAD_MAX_BYTES / sizeof(uint32_t)]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

/*
 * First link of each side. Which of these the topology declares is decided at
 * runtime from the profile's link_in_use, so a profile that leaves a side
 * unused simply yields a narrower capture instead of the FSM being asked to
 * bring up a link that was never configured.
 */
static const unsigned int framer_link_id[LOOPBACK_SIDES] = {
	FRAMER_LINK_A0_RX,
	FRAMER_LINK_B0_RX,
};

static const unsigned int deframer_link_id[LOOPBACK_SIDES] = {
	DEFRAMER_LINK_A0_TX,
	DEFRAMER_LINK_B0_TX,
};

#if RX_LINK_RECOVER
/*
 * 64b/66b link states, mirroring axi_jesd204_rx_link_status_64b66b_l[] in
 * axi_jesd204_rx.c. Copied rather than referenced: the driver leaves the table
 * non-static but declares it in no header.
 */
static const char *const rx_link_state[] = {
	"RESET",
	"WAIT_BS",
	"BLOCK_SYNC",
	"DATA",
};

/**
 * @brief Current receive link state, as the core reports it.
 * @return The two-bit link status: 0 reset, 1 WAIT_BS, 2 block sync, 3 data.
 */
static uint32_t dma_example_rx_link_status(void)
{
	uint32_t status = 0;

	no_os_axi_io_read(RX_JESD_BASEADDR, JESD204_RX_REG_LINK_STATUS,
			  &status);

	return status & JESD204_RX_LINK_STATUS_MASK;
}

/**
 * @brief Get the receive link back to DATA if MCS calibration dropped it.
 *
 * Same shape as adrv903x_jesd204_setup_stage1() in the palma driver -- a
 * bounded loop that acts and re-polls until the link reports good -- but the
 * action differs. Palma pulses SYSREF each iteration, which works because its
 * AD9528 fires a burst on demand. Here the provider is the ADF4030, whose
 * jesd204 SYSREF callback is deliberately empty because it emits BSYNC
 * continuously, so jesd204_sysref_async_force() would do nothing.
 *
 * What is left is taking the receiver down and bringing it back up:
 * LINK_DISABLE asserted, 100 ms, deasserted. axi_jesd204_rx_lane_clk_enable()
 * also clears SYSREF_STATUS on the way out, so the core re-acquires against
 * whatever phase MCS left behind.
 *
 * The whole FSM is never restarted: that would re-run MCS and break the link
 * again.
 *
 * @param phy - AD9088 device, for the Apollo side of the re-sync.
 * @param rx_jesd - Receive JESD204 core.
 * @return 0 if the link is in DATA, negative error code otherwise.
 */
static int dma_example_rx_link_recover(struct ad9088_phy *phy,
				       struct axi_jesd204_rx *rx_jesd)
{
	uint32_t status = dma_example_rx_link_status();
	uint32_t attempt;
	uint32_t poll;
	int ret;

	if (status == JESD204_RX_LINK_STATUS_DATA)
		return 0;

	pr_info("rx_jesd: link is %s after the FSM, recovering\n",
		rx_link_state[status]);

	for (attempt = 1; attempt <= RX_LINK_ATTEMPTS; attempt++) {

		ret = adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run(
			      &phy->ad9088);
		if (ret)
			pr_err("Apollo link re-sync failed (%d)\n", ret);

		axi_jesd204_rx_lane_clk_disable(rx_jesd);
		no_os_mdelay(100);
		axi_jesd204_rx_lane_clk_enable(rx_jesd);

		/* The same budget LINK_RUNNING allows the link on bring-up. */
		for (poll = 0; poll < 20; poll++) {
			no_os_mdelay(4);
			status = dma_example_rx_link_status();
			if (status == JESD204_RX_LINK_STATUS_DATA)
				break;
		}

		if (status == JESD204_RX_LINK_STATUS_DATA) {
			pr_info("rx_jesd: link is DATA after %lu attempt%s\n",
				(unsigned long)attempt,
				(attempt == 1) ? "" : "s");
			return 0;
		}
	}

	pr_err("rx_jesd: link stuck at %s after %u attempts\n",
	       rx_link_state[status], RX_LINK_ATTEMPTS);

	return -EIO;
}
#endif

/**
 * @brief Put one side's NCOs on a known default frequency.
 *
 * A CDUC/FDUC upconverts and a CDDC/FDDC downconverts, so a tone written at
 * f_lut comes back at f_lut + (tx shifts) - (rx shifts). A profile is free to
 * leave the transmit and receive NCOs on different frequencies, and any offset
 * between them translates the tone by that much -- generally far enough to put
 * it outside the FDDC passband whatever it was transmitted at. Tuning all of
 * them to the same frequency cancels the translation, so a tone arrives where
 * it was sent.
 *
 * Every datapath on the side is tuned, not just the one carrying the first I/Q
 * pair: the link's converters are spread across several FDUC/FDDC pairs, and a
 * pair left on the profile's own frequencies loses its tone off the passband
 * while the rest of the capture looks fine.
 *
 * Both sides share the transport but keep their own datapaths, so this runs
 * once per side in use rather than once for the capture.
 *
 * @param phy - AD9088 device.
 * @param side - Side to tune.
 * @return 0 on success, negative error code otherwise.
 */
static int dma_example_set_default_nco(struct ad9088_phy *phy, uint8_t side)
{
	uint64_t dac_rate = phy->profile.dac_cfg[side].dac_sampling_rate_Hz;
	int64_t tx_cnco = 0;
	int64_t tx_fnco = 0;
	int64_t rx_cnco = 0;
	int64_t rx_fnco = 0;
	int64_t cnco_hz;
	uint8_t cddc;
	uint8_t fddc;
	int ret;

	cnco_hz = (int64_t)no_os_div_u64(dac_rate, DEFAULT_CNCO_RATE_DIV);

	for (cddc = 0; cddc < ADI_APOLLO_CDDCS_PER_SIDE; cddc++) {
		ret = ad9088_set_cnco_freq(phy, ADI_APOLLO_TX, side, cddc,
					   cnco_hz);
		if (!ret)
			ret = ad9088_set_cnco_freq(phy, ADI_APOLLO_RX, side,
						   cddc, cnco_hz);
		if (ret) {
			pr_err("Side %u: tuning CDDC/CDUC %u failed (%d)\n",
			       side, cddc, ret);
			return ret;
		}
	}

	for (fddc = 0; fddc < ADI_APOLLO_FDDCS_PER_SIDE; fddc++) {
		ret = ad9088_set_fnco_freq(phy, ADI_APOLLO_TX, side, fddc,
					   DEFAULT_FNCO_HZ);
		if (!ret)
			ret = ad9088_set_fnco_freq(phy, ADI_APOLLO_RX, side,
						   fddc, DEFAULT_FNCO_HZ);
		if (ret) {
			pr_err("Side %u: tuning FDDC/FDUC %u failed (%d)\n",
			       side, fddc, ret);
			return ret;
		}
	}

	/*
	 * Read every one of them back rather than trusting the writes: a tone
	 * is only where it was sent if the whole side agrees, so one datapath
	 * that did not take the tuning is enough to leave a converter pair
	 * empty.
	 */
	for (fddc = 0; fddc < ADI_APOLLO_FDDCS_PER_SIDE; fddc++) {
		cddc = (fddc / 2) % ADI_APOLLO_CDDCS_PER_SIDE;

		ret = ad9088_get_cnco_freq(phy, ADI_APOLLO_TX, side, cddc,
					   &tx_cnco);
		if (!ret)
			ret = ad9088_get_fnco_freq(phy, ADI_APOLLO_TX, side,
						   fddc, &tx_fnco);
		if (!ret)
			ret = ad9088_get_cnco_freq(phy, ADI_APOLLO_RX, side,
						   cddc, &rx_cnco);
		if (!ret)
			ret = ad9088_get_fnco_freq(phy, ADI_APOLLO_RX, side,
						   fddc, &rx_fnco);
		if (ret) {
			pr_err("Side %u: reading back the NCOs failed (%d)\n",
			       side, ret);
			return ret;
		}

		/*
		 * The tuning word drops its fractional part, so a rate that the
		 * divisor does not divide exactly lands a few Hz off. Far too
		 * little to move the tone off its bin, but it should not pass
		 * unremarked.
		 */
		if (tx_cnco != cnco_hz || rx_cnco != cnco_hz)
			pr_info("  Warning: side %u CDDC%u asked %ld Hz, tuned "
				"tx %ld rx %ld\n", side, cddc, (long)cnco_hz,
				(long)tx_cnco, (long)rx_cnco);

		if ((tx_cnco + tx_fnco) != (rx_cnco + rx_fnco)) {
			pr_err("Side %u CDDC%u/FDDC%u did not take the default "
			       "tuning: tx c/f %ld/%ld kHz  rx c/f %ld/%ld "
			       "kHz\n", side, cddc, fddc,
			       (long)no_os_div_s64(tx_cnco, 1000),
			       (long)no_os_div_s64(tx_fnco, 1000),
			       (long)no_os_div_s64(rx_cnco, 1000),
			       (long)no_os_div_s64(rx_fnco, 1000));
			return -EIO;
		}
	}

	pr_info("  Side %u NCOs: %u coarse at %ld kHz, %u fine at %ld Hz, tx "
		"and rx matched\n", side, (unsigned)ADI_APOLLO_CDDCS_PER_SIDE,
		(long)no_os_div_s64(cnco_hz, 1000),
		(unsigned)ADI_APOLLO_FDDCS_PER_SIDE, (long)DEFAULT_FNCO_HZ);

	return 0;
}

/**
 * @brief Fill the transmit buffer with the shared sine table.
 *
 * One table entry carries a single I/Q pair, so a link with more than one
 * complex channel needs the entry repeated once per channel at every sample
 * time. Tiling the table whole keeps the transmitted tone continuous across the
 * cyclic wrap, which matters because the capture is shorter than the replay and
 * can start anywhere in it.
 *
 * @param num_conv - Converters on the transmit link.
 * @param size - Buffer length to fill, in bytes.
 */
static void dma_example_fill_tone(uint8_t num_conv, uint32_t size)
{
	uint32_t lut_len = NO_OS_ARRAY_SIZE(sine_lut_iq);
	uint32_t words = size / sizeof(dac_buffer_dma[0]);
	uint32_t stride = num_conv / 2;
	uint32_t i;

	for (i = 0; i < words; i++)
		dac_buffer_dma[i] = sine_lut_iq[(i / stride) % lut_len];
}

int dma_example_main(void)
{
	struct no_os_clk_desc rx_lane_clk = {0};
	struct no_os_clk_desc tx_lane_clk = {0};
	struct jesd204_clk rx_jesd_clk = {0};
	struct jesd204_clk tx_jesd_clk = {0};
	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;
	uint32_t tx_offload_size = 0;
	struct adf4382_dev *adf4382_dev;
	struct hmc7044_dev *hmc7044_dev;
	struct adf4030_dev *adf4030_dev;
	struct ad9088_phy *ad9088_phy;
	struct adxcvr *rx_adxcvr;
	struct adxcvr *tx_adxcvr;
	struct axi_dmac *rx_dmac;
	struct axi_dmac *tx_dmac;
	uint32_t rx_size;
	uint32_t tx_lut_bytes;
	uint8_t side_conv[LOOPBACK_SIDES] = {0};
	uint8_t side_first_conv[LOOPBACK_SIDES] = {0};
	unsigned int link_ids[2 * LOOPBACK_SIDES];
	unsigned int links_number = 0;
	unsigned int dev_idx;
	unsigned int link_idx;
	uint8_t link_conv;
	struct axi_adc *rx_adc;
	struct axi_dac *tx_dac;
	uint8_t tx_num_conv;
	uint32_t tx_samples;
	uint32_t tx_size;
	uint8_t rx_num_conv;
	uint8_t side;
	uint8_t np;
	int ret = 0;

	struct jesd204_topology *topology;

	/* Sizes are filled in once the link geometry is known. */
	struct axi_dma_transfer tx_transfer = {
		.cyclic = CYCLIC,
		.src_addr = (uintptr_t)dac_buffer_dma,
	};

	struct axi_dma_transfer rx_transfer = {
		.cyclic = NO,
		.dest_addr = (uintptr_t)adc_buffer_dma,
	};

	pr_info("Enter DMA example\n");

	ret = adf4382_init(&adf4382_dev, &adf4382_ip);
	if (ret) {
		pr_info("ADF4382 initialization failed\n");
		goto error;
	}

	ret = hmc7044_init(&hmc7044_dev, &hmc7044_ip);
	if (ret) {
		pr_info("HMC7044 initialization failed\n");
		goto error_adf4382;
	}

	/* After the HMC7044, which supplies the ADF4030's reference clock. */
	ret = adf4030_init(&adf4030_dev, &adf4030_ip);
	if (ret) {
		pr_info("ADF4030 initialization failed\n");
		goto error_hmc7044;
	}

	/*
	 * Enables MCS calibration, which trims the AD9088's internal SYSREF
	 * onto the external edge. Needs both clock chips probed.
	 */
	ret = ad9088_mcs_ops_bind(adf4030_dev, adf4382_dev);
	if (ret) {
		pr_info("MCS ops bind failed\n");
		goto error_adf4030;
	}

	ret = axi_dmac_init(&rx_dmac, &rx_dmac_ip);
	if (ret) {
		pr_info("RX DMAC initialization failed\n");
		goto error_adf4030;
	}

	ret = axi_dmac_init(&tx_dmac, &tx_dmac_ip);
	if (ret) {
		pr_info("TX DMAC initialization failed\n");
		goto error_rx_dmac;
	}

	ret = adxcvr_init(&tx_adxcvr, &tx_adxcvr_ip);
	if (ret) {
		pr_info("TX ADXCVR initialization failed\n");
		goto error_tx_dmac;
	}
	tx_jesd_clk.xcvr = tx_adxcvr;

	ret = adxcvr_init(&rx_adxcvr, &rx_adxcvr_ip);
	if (ret) {
		pr_info("RX ADXCVR initialization failed\n");
		goto error_tx_adxcvr;
	}
	rx_jesd_clk.xcvr = rx_adxcvr;

	rx_lane_clk.platform_ops = &jesd204_clk_ops;
	rx_lane_clk.dev_desc = &rx_jesd_clk;
	rx_jesd204_ip.lane_clk = &rx_lane_clk;

	tx_lane_clk.platform_ops = &jesd204_clk_ops;
	tx_lane_clk.dev_desc = &tx_jesd_clk;
	tx_jesd204_ip.lane_clk = &tx_lane_clk;

	ret = axi_jesd204_rx_init(&rx_jesd, &rx_jesd204_ip);
	if (ret) {
		pr_info("JESD RX initialization failed\n");
		goto error_rx_adxcvr;
	}
	rx_jesd_clk.jesd_rx = rx_jesd;

	ret = axi_jesd204_tx_init(&tx_jesd, &tx_jesd204_ip);
	if (ret) {
		pr_info("JESD TX initialization failed\n");
		goto error_rx_jesd;
	}
	tx_jesd_clk.jesd_tx = tx_jesd;

	ret = ad9088_init(&ad9088_phy, &ad9088_ip);
	if (ret) {
		pr_info("AD9088 initialization failed\n");
		goto error_tx_jesd;
	}

	/*
	 * Bring up the first link of every side the profile has in use. Both
	 * sides frame into the one JESD core and transport layer, so this list
	 * is what decides whether the capture is one side wide or both -- there
	 * is no second chain to enable instead. A side left unused by the
	 * profile is skipped rather than handed to the FSM as a link that was
	 * never configured.
	 */
	for (side = 0; side < LOOPBACK_SIDES; side++) {
		if (ad9088_phy->profile.jtx[side].tx_link_cfg[0].link_in_use)
			link_ids[links_number++] = framer_link_id[side];
		if (ad9088_phy->profile.jrx[side].rx_link_cfg[0].link_in_use)
			link_ids[links_number++] = deframer_link_id[side];
	}

	if (!links_number) {
		pr_err("The profile has no JESD204 link in use\n");
		ret = -EINVAL;
		goto error_ad9088;
	}

	/*
	 * The SYSREF provider - here the ADF4030, which clocks the SYSREF input
	 * of both the AD9088 and the FPGA - has to be listed before the top
	 * device: jesd204_topology_init() reads is_sysref_provider from this
	 * array but takes the jdev pointer from the top-device-filtered copy,
	 * so the two indices only agree while the provider precedes the top
	 * device.
	 *
	 * The FPGA cores name side A's link and nothing else: the core is one
	 * register set shared by both links, so side B's link has nothing of
	 * its own to attach to. Entries left without links here are given every
	 * link in use just below.
	 */
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = adf4030_dev->jdev,
			.is_sysref_provider = true,
		},
		{
			.jdev = hmc7044_dev->jdev,
		},
		{
			.jdev = rx_jesd->jdev,
			.link_ids = {FRAMER_LINK_A0_RX},
			.links_number = 1,
		},
		{
			.jdev = tx_jesd->jdev,
			.link_ids = {DEFRAMER_LINK_A0_TX},
			.links_number = 1,
		},
		{
			.jdev = ad9088_phy->jdev,
			.is_top_device = true,
		},
	};

	for (dev_idx = 0; dev_idx < NO_OS_ARRAY_SIZE(devs); dev_idx++) {
		if (devs[dev_idx].links_number)
			continue;

		for (link_idx = 0; link_idx < links_number; link_idx++)
			devs[dev_idx].link_ids[link_idx] = link_ids[link_idx];

		devs[dev_idx].links_number = links_number;
	}

	ret = jesd204_topology_init(&topology, devs,
				    NO_OS_ARRAY_SIZE(devs));
	if (ret) {
		pr_info("JESD204 topology init failed\n");
		goto error_ad9088;
	}

	ret = jesd204_fsm_start(topology, JESD204_LINKS_ALL);
	if (ret) {
		pr_info("JESD204 FSM start failed\n");
		goto error_topology;
	}

#if RX_LINK_RECOVER
	/*
	 * Before anything is derived from the link, so a receiver MCS left in
	 * WAIT_BS is reported here rather than surfacing as a capture timeout a
	 * second later.
	 */
	ret = dma_example_rx_link_recover(ad9088_phy, rx_jesd);
	if (ret)
		goto error_topology;
#endif
	np = ad9088_phy->profile.jtx[LOOPBACK_SIDE]
	     .tx_link_cfg[0].np_minus1 + 1;

	/*
	 * Derive the capture geometry from the links the FSM just brought up
	 * rather than hardcoding it, so a profile change cannot silently
	 * corrupt the buffer layout. Every side frames into the one transport
	 * layer, so the capture is as wide as their converter counts together
	 * and each side owns a contiguous window in converter order.
	 */
	rx_num_conv = 0;

	for (side = 0; side < LOOPBACK_SIDES; side++) {
		if (!ad9088_phy->profile.jtx[side].tx_link_cfg[0].link_in_use)
			continue;

		link_conv = ad9088_phy->profile.jtx[side]
			    .tx_link_cfg[0].m_minus1 + 1;

		if (!link_conv || link_conv > LOOPBACK_CONVERTERS) {
			pr_err("Side %u converter count M=%u, this example "
			       "covers up to %u per side\n", side, link_conv,
			       LOOPBACK_CONVERTERS);
			ret = -EINVAL;
			goto error_topology;
		}

		side_first_conv[side] = rx_num_conv;
		side_conv[side] = link_conv;
		rx_num_conv += link_conv;
	}

	if (!rx_num_conv) {
		pr_err("No receive link in use, nothing to capture\n");
		ret = -EINVAL;
		goto error_topology;
	}

	/*
	 * A sine table entry is one I/Q pair, so every transmit link has to
	 * carry a whole number of complex channels for the tiling further down
	 * to line up.
	 */
	tx_num_conv = 0;

	for (side = 0; side < LOOPBACK_SIDES; side++) {
		if (!ad9088_phy->profile.jrx[side].rx_link_cfg[0].link_in_use)
			continue;

		link_conv = ad9088_phy->profile.jrx[side]
			    .rx_link_cfg[0].m_minus1 + 1;

		if (!link_conv || link_conv > LOOPBACK_CONVERTERS ||
		    link_conv % 2) {
			pr_err("Side %u transmit converter count M=%u, this "
			       "example covers up to %u per side, in I/Q "
			       "pairs\n", side, link_conv, LOOPBACK_CONVERTERS);
			ret = -EINVAL;
			goto error_topology;
		}

		tx_num_conv += link_conv;
	}

	if (!tx_num_conv) {
		pr_err("No transmit link in use, nothing to replay\n");
		ret = -EINVAL;
		goto error_topology;
	}

	rx_size = ADC_BUFFER_SAMPLES * rx_num_conv *
			sizeof(adc_buffer_dma[0]);

	pr_info("Capture geometry: M=%u NP=%u samples/conv=%lu bytes=%lu\n",
		rx_num_conv, np, (unsigned long)ADC_BUFFER_SAMPLES,
		(unsigned long)rx_size);

	for (side = 0; side < LOOPBACK_SIDES; side++) {
		if (!side_conv[side])
			continue;

		pr_info("  Side %u: converters %u..%u%s\n", side,
			side_first_conv[side],
			side_first_conv[side] + side_conv[side] - 1,
			(side == LOOPBACK_SIDE) ? "  <- reported window" : "");
	}

	if (!side_conv[LOOPBACK_SIDE])
		pr_info("  Warning: side %u is not in use in this profile, so "
			"its window is empty\n", LOOPBACK_SIDE);

	rx_adc_init.num_channels = rx_num_conv;
	ret = axi_adc_init(&rx_adc, &rx_adc_init);
	if (ret) {
		pr_err("RX TPL core init failed (%d)\n", ret);
		goto error_topology;
	}

	/* The transmit core belongs to the other link, with its own M. */
	tx_dac_init.num_channels = tx_num_conv;
	ret = axi_dac_init(&tx_dac, &tx_dac_init);
	if (ret) {
		pr_err("TX TPL core init failed (%d)\n", ret);
		goto error_rx_adc;
	}

	/* Keep the DAC quiet until the transmit buffer is in place. */
	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_ZERO);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);
	pr_info("Project configured\n\n");

	/*
	 * The loopback runs from this operating point rather than wherever the
	 * profile happened to leave the NCOs. Each side keeps its own
	 * datapaths, so every side in use is tuned -- one left on the profile's
	 * frequencies would come back as noise in its half of the capture.
	 */
	for (side = 0; side < LOOPBACK_SIDES; side++) {
		if (!ad9088_phy->profile.jtx[side].tx_link_cfg[0].link_in_use &&
		    !ad9088_phy->profile.jrx[side].rx_link_cfg[0].link_in_use)
			continue;

		ret = dma_example_set_default_nco(ad9088_phy, side);
		if (ret)
			goto error_tx_dac;
	}

	/*
	 * The offload replays all of its memory whatever was written into it,
	 * so fill as much as this buffer covers rather than leaving the tail to
	 * come back as noise.
	 */
	no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_MEMORY_SIZE_LSB,
			  &tx_offload_size);

	tx_size = sizeof(dac_buffer_dma);
	if (tx_offload_size && tx_offload_size < tx_size)
		tx_size = tx_offload_size;

	/*
	 * Floor to a whole pass of the sine table so the cyclic wrap leaves no
	 * phase discontinuity for a capture to straddle -- which matters
	 * because the capture is shorter than the replay and can start anywhere
	 * in it.
	 */
	tx_lut_bytes = NO_OS_ARRAY_SIZE(sine_lut_iq) * tx_num_conv *
		       sizeof(uint16_t);
	tx_size -= tx_size % tx_lut_bytes;

	/*
	 * A whole table pass is a multiple of the source width at every
	 * converter count this example accepts, so the floor above should
	 * already have satisfied it. Check rather than assume, since the DMAC
	 * rejects a misaligned transfer with a much less obvious error.
	 */
	if (!tx_size || tx_size % DMA_SRC_WIDTH_BYTES) {
		pr_err("TX size %lu is not a usable multiple of the %u byte "
		       "DMAC source width\n", (unsigned long)tx_size,
		       DMA_SRC_WIDTH_BYTES);
		ret = -EINVAL;
		goto error_tx_dac;
	}

	tx_samples = tx_size / (tx_num_conv * sizeof(uint16_t));

	/*
	 * Both DMACs move data behind the cache, and from here to the end of
	 * the capture there is always one in flight, so the cache is kept out
	 * of the way wholesale rather than flushed and invalidated by range.
	 */
	Xil_DCacheDisable();

	dma_example_fill_tone(tx_num_conv, tx_size);
	ret = axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	if (ret) {
		pr_err("Selecting the DMA data source failed (%d)\n", ret);
		goto error_tx_dac;
	}

	tx_transfer.size = tx_size;

	/*
	 * Cyclic keeps the tone running for the whole capture. It is a build
	 * option of the DMAC rather than a guarantee: axi_dmac_transfer_start()
	 * rejects CYCLIC outright on a core synthesised without it, so an HDL
	 * build that lacks it fails here rather than transmitting one pass.
	 * Completion is never waited on -- a cyclic transfer raises no
	 * end-of-transfer and would only time out.
	 */
	ret = axi_dmac_transfer_start(tx_dmac, &tx_transfer);
	if (ret) {
		pr_err("TX DMA transfer start failed (%d)\n", ret);
		goto error_tx_stream;
	}

	pr_info("DMA_EXAMPLE Tx: address=%#lx samples=%lu channels=%u bits=%lu\n",
		(unsigned long)(uintptr_t)dac_buffer_dma,
		(unsigned long)tx_samples,
		tx_num_conv, (unsigned long)(8 * sizeof(uint16_t)));

	no_os_mdelay(10);

	/*
	 * Re-arm the receive offload. This build synthesises it with
	 * AUTO_BRINGUP, so it has been running since power-on, and the receive
	 * instance defaults to one-shot -- by now it holds a fill captured
	 * before the link existed and its store phase is long over. A low-high
	 * edge on RESETN_OFFLOAD drops that fill and starts a fresh store,
	 * which is what makes the capture below see live converter data rather
	 * than time out. It belongs immediately before the DMAC is started, so
	 * the store runs against the current converter phase and the DMAC
	 * back-pressures until it completes.
	 */
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 1);

	rx_transfer.size = rx_size;
	ret = axi_dmac_transfer_start(rx_dmac, &rx_transfer);
	if (ret) {
		pr_err("RX DMA transfer start failed (%d)\n", ret);
		goto error_tx_stream;
	}

	ret = axi_dmac_transfer_wait_completion(rx_dmac, 1000);

	/* The capture is in memory; drop whatever the cache comes back with. */
	Xil_DCacheEnable();
	Xil_DCacheInvalidate();

	if (ret) {
		pr_err("RX DMA transfer timed out (%d)\n", ret);
		goto error_tx_stream;
	}

	pr_info("DMA_EXAMPLE Rx: address=%#lx samples=%lu channels=%u bits=%u\n",
		(unsigned long)(uintptr_t)adc_buffer_dma,
		(unsigned long)ADC_BUFFER_SAMPLES, rx_num_conv, np);

	/*
	 * Samples interleave across the whole capture, so a side's window is a
	 * stride of rx_num_conv starting at its first converter.
	 */
	pr_info("DMA_EXAMPLE Rx side %u: first sample at byte %lu, stride %lu "
		"bytes, %u converters\n", LOOPBACK_SIDE,
		(unsigned long)(side_first_conv[LOOPBACK_SIDE] *
				sizeof(adc_buffer_dma[0])),
		(unsigned long)(rx_num_conv * sizeof(adc_buffer_dma[0])),
		side_conv[LOOPBACK_SIDE]);

	/*
	 * Park here with the link up rather than tearing down, so the capture
	 * buffer stays intact and readable from a debugger. Nothing below is
	 * reached except by jumping to one of the error labels.
	 */
	while (1);

error_tx_stream:
	axi_dmac_transfer_stop(tx_dmac);
	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_ZERO);
error_tx_dac:
	axi_dac_remove(tx_dac);
error_rx_adc:
	axi_adc_remove(rx_adc);
error_topology:
	jesd204_topology_remove(topology);
error_ad9088:
	ad9088_remove(ad9088_phy);
error_tx_jesd:
	axi_jesd204_tx_remove(tx_jesd);
error_rx_jesd:
	axi_jesd204_rx_remove(rx_jesd);
error_rx_adxcvr:
	adxcvr_remove(rx_adxcvr);
error_tx_adxcvr:
	adxcvr_remove(tx_adxcvr);
error_tx_dmac:
	axi_dmac_remove(tx_dmac);
error_rx_dmac:
	axi_dmac_remove(rx_dmac);
error_adf4030:
	adf4030_remove(adf4030_dev);
error_hmc7044:
	hmc7044_remove(hmc7044_dev);
error_adf4382:
	adf4382_remove(adf4382_dev);
error:
	if (ret)
		pr_info("Error!\n");

	return ret;
}
