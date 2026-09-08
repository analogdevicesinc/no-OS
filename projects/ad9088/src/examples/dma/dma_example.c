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
 *
 * A platform header may pin it to the offload its bitstream actually has, which
 * matters where the buffer competes for on-chip memory rather than sitting in
 * external DDR.
 */
#ifndef TX_OFFLOAD_MAX_BYTES
#define TX_OFFLOAD_MAX_BYTES		(512 * 1024)
#endif

/*
 * axi_data_offload register map, the part of it this example needs, from
 * data_offload_regmap.v. That file gives word addresses; these are the byte
 * offsets, i.e. word * 4. MEMORY_SIZE_LSB is the storage depth in bytes, read
 * only; RESETN_OFFLOAD bit 0 low holds the IP in reset. The two STATUS flags are
 * write-1-to-clear and latch, so they must be cleared before a run to mean
 * anything after it.
 */
#define AXI_DO_REG_CONFIG		0x0010
#define AXI_DO_CONFIG_HAS_BYPASS	NO_OS_BIT(2)
#define AXI_DO_REG_MEMORY_SIZE_LSB	0x0014
#define AXI_DO_REG_STATUS		0x0080
#define AXI_DO_STATUS_SRC_OVERFLOW	NO_OS_BIT(4)
#define AXI_DO_STATUS_DST_UNDERFLOW	NO_OS_BIT(5)
#define AXI_DO_REG_RESETN_OFFLOAD	0x0084
#define AXI_DO_REG_CONTROL		0x0088
#define AXI_DO_CONTROL_BYPASS		NO_OS_BIT(0)
/*
 * Whether the transmit data offload runs in bypass. Off makes the offload store
 * one DMA fill and replay it, which is the path the HDL is built around; on makes
 * the DMA feed the transport layer continuously.
 */
#define TX_OFFLOAD_BYPASS		0

/*
 * Converter test mode. The generator sits in the device's JTX datapath, past the
 * ADC and past the decimators, so a capture taken with it on exercises the
 * serial link, the FPGA deframer, the transport layer, the offload and the DMA
 * without the converter contributing anything.
 *
 * That is the one measurement the loopback cannot make. Every register on the
 * FPGA side matches a working Linux bring-up, the 8B/10B error counters are zero
 * -- so the octets arrive with valid codes and correct disparity -- and yet the
 * samples are uniformly distributed over full scale. Uniform is the signature of
 * random bits, not of a converter: thermal noise is Gaussian and small. If a
 * known pattern comes back intact, the fault is upstream of the JTX and the
 * whole FPGA side is exonerated; if it comes back as noise, it is not.
 *
 * RAMP is the default because it checks sample order and converter assignment as
 * well as bit integrity. MIDSCALE (constant zero) is the blunter test: any
 * non-zero sample is then a defect, with no interpretation needed.
 */
/*
 * SYSREF status, at the same offset in both the receive and the transmit link
 * core (jesd204_up_sysref.v word 0x042). Bit 0 latches a captured edge and bit 1
 * an edge that did not land on an LMFC boundary; both are write-1-to-clear, and
 * the clear is not gated by the core being disabled.
 */
#define JESD_REG_SYSREF_STATUS		0x108
#define JESD_SYSREF_CAPTURED		NO_OS_BIT(0)
#define JESD_SYSREF_ALIGNMENT_ERROR	NO_OS_BIT(1)


/*
 * Periods of the transmit tone across the whole buffer. The buffer is replayed
 * cyclically, so an integer count is what keeps the wrap free of a phase step;
 * the capture is the same length, so the tone also lands on exactly this bin of
 * an FFT of the capture.
 *
 * It must not be a multiple of 128. SYSREF here is 1.953125 MHz, which is the
 * transport-layer sample rate over 128, so a tone at any multiple of that shares
 * an FFT bin with SYSREF and with every harmonic of it -- a SYSREF-correlated
 * spur and the test tone become one line and cannot be told apart. 617 is prime
 * and 23 bins clear of the nearest SYSREF harmonic; at 250 MHz it is 9.41 MHz,
 * about 26.6 samples per period.
 */
#define TX_TONE_PERIODS		617

/*
 * Width of the TX DMAC source AXI data path in bytes, which has to match the
 * HDL build. The driver rejects a transfer that is not a multiple of it.
 *
 * DMA_DATA_WIDTH_SRC is 128 in the HDL and that is bits, so this is 16, not 128.
 */
#define DMA_SRC_WIDTH_BYTES	16
#define DMA_BUFFER_ALIGN	1024

/*
 * Bring the receive link back if MCS calibration knocks it out.
 *
 * MCS runs at OPT_POST_SETUP_STAGE1, one stage after LINK_RUNNING has already
 * waited for the receiver to reach DATA. It programs the measured BSYNC path
 * delay onto the provider channel feeding the Apollo and realigns that channel
 * alone; the channel feeding the FPGA is left where it was, so the two SYSREFs
 * separate by the path delay, which is a large fraction of a link clock either
 * way: 2.2 ns against 3.2 ns on the 204C/VCU118 build this was written for,
 * 2.2 ns against 4.0 ns on the 204B AD9084-EBZ. The Apollo re-times off the edge
 * that moved and the receiver does not, which is what drops it out of DATA --
 * to WAIT_BS on 64b66b, to CGS on 8b10b.
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
 * Frame alignment error count per lane, 8 bits, saturating. jesd204_rx is built
 * with ENABLE_FRAME_ALIGN_CHECK=1 and ENABLE_FRAME_ALIGN_ERR_RESET=0, so the
 * count is maintained but never acted on: state_good is hardcoded 1 in
 * SYNCHRONIZED, the control FSM never returns to CGS, and the counter clears
 * only on a link restart. That makes it the one register-visible proxy for the
 * decoder failing -- LINK_STATUS reads DATA regardless.
 */
#define JESD204_RX_REG_LANE_FRAME_ALIGN_ERR(x)	(((x) * 32) + 0x30C)
#define JESD204_RX_REG_LANE_ERRORS(x)		(((x) * 32) + 0x308)

/*
 * Sticky event latch in jesd204_up_common, accumulated whatever IRQ_ENABLE
 * holds, so it records events on a build that never enables the interrupt.
 * Bit 1 is unexpected_lane_state_error: at least one enabled lane left code
 * group sync while the control FSM was in DATA. jesd204_rx.v:420,
 *
 *   unexpected_lane_state_error = |(~(cgs_ready|cfg_lanes_disable)) &
 *                                 &status_ctrl_state;
 *
 * which is the condition Signal Tap showed indirectly -- see
 * dma_example_rx_link_recover(). Cleared by writing the bit back.
 */
#define JESD204_RX_REG_IRQ_SOURCE		0x088
#define JESD204_RX_IRQ_FRAME_ALIGNMENT_ERROR	NO_OS_BIT(0)
#define JESD204_RX_IRQ_UNEXPECTED_LANE_STATE	NO_OS_BIT(1)

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
 * The core's own state names. Both tables are non-static in axi_jesd204_rx.c but
 * appear in no header, so they are declared rather than copied - a copy is what
 * left this file naming 8b10b states after the 64b66b ones.
 *
 * The two encoders share the numbering but not the names: state 1 is
 * WAIT FOR PHY / WAIT_BS and state 2 is CGS / BLOCK_SYNC, so a table picked
 * without checking the encoder mislabels every state but RESET and DATA.
 */
extern const char *axi_jesd204_rx_link_status_label[];
extern const char *axi_jesd204_rx_link_status_64b66b_l[];

/**
 * @brief Name of a receive link state, for the encoder in use.
 * @param rx_jesd - The receive link core.
 * @param status - The two-bit link status.
 * @return The state name.
 */
static const char *dma_example_rx_link_state_str(struct axi_jesd204_rx *rx_jesd,
		uint32_t status)
{
	status &= JESD204_RX_LINK_STATUS_MASK;

	return (rx_jesd->encoder == JESD204_ENCODER_8B10B) ?
	       axi_jesd204_rx_link_status_label[status] :
	       axi_jesd204_rx_link_status_64b66b_l[status];
}

/**
 * @brief Current receive link state, as the core reports it.
 * @return The two-bit link status. 0 and 3 are RESET and DATA on either
 *         encoder; 1 and 2 are encoder-specific, see
 *         dma_example_rx_link_state_str().
 */
static uint32_t dma_example_rx_link_status(void)
{
	uint32_t status = 0;

	no_os_axi_io_read(RX_JESD_BASEADDR, JESD204_RX_REG_LINK_STATUS,
			  &status);

	return status & JESD204_RX_LINK_STATUS_MASK;
}

/**
 * @brief Per-lane frame alignment and character error counts.
 * @param rx_jesd - The receive link core.
 * @param tag - Printed with the counts, to tell one read from another.
 */
static void dma_example_rx_align_errors(struct axi_jesd204_rx *rx_jesd,
					const char *tag)
{
	uint32_t lane;
	uint32_t fa;
	uint32_t err;
	uint32_t irq;

	no_os_axi_io_read(RX_JESD_BASEADDR, JESD204_RX_REG_IRQ_SOURCE, &irq);
	pr_info("rx_jesd %s: irq_source 0x%08lx%s%s\n", tag,
		(unsigned long)irq,
		(irq & JESD204_RX_IRQ_FRAME_ALIGNMENT_ERROR) ?
		" frame_alignment_error" : "",
		(irq & JESD204_RX_IRQ_UNEXPECTED_LANE_STATE) ?
		" unexpected_lane_state" : "");

	for (lane = 0; lane < rx_jesd->num_lanes; lane++) {
		no_os_axi_io_read(RX_JESD_BASEADDR,
				  JESD204_RX_REG_LANE_FRAME_ALIGN_ERR(lane),
				  &fa);
		no_os_axi_io_read(RX_JESD_BASEADDR,
				  JESD204_RX_REG_LANE_ERRORS(lane), &err);
		pr_info("rx_jesd %s: lane %lu frame_align_err %lu errors 0x%08lx\n",
			tag, (unsigned long)lane, (unsigned long)(fa & 0xFF),
			(unsigned long)err);
	}
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
 * What is left is taking the receiver down and bringing it back up -- the same
 * bounce the kernel's watchdog uses when it finds a lane in error, LINK_DISABLE
 * asserted, 100 ms, deasserted. axi_jesd204_rx_lane_clk_enable() also clears
 * SYSREF_STATUS on the way out, so the core re-acquires against whatever phase
 * MCS left behind.
 *
 * The whole FSM is never restarted: that would re-run MCS and break the link
 * again.
 *
 * The bounce runs unconditionally before the status is trusted. LINK_STATUS
 * reads DATA whatever the lanes are doing -- jesd204_rx_ctrl hardcodes
 * state_good in SYNCHRONIZED -- so an early return on DATA is an early return on
 * nothing. The reason the bounce is needed at all is the soft PCS: its word
 * aligner only slips while the FSM is in CGS (jesd204_pattern_align, driven by
 * phy_en_char_align), so a bit boundary that moves after the first CGS stays
 * latched wrong and the 8b10b decoder rejects a fifth of all octets for the life
 * of the link. Asserting LINK_DISABLE walks the FSM back through CGS with the
 * aligner enabled, and SYNC takes the converter's framer back to K28.5 so there
 * is something to align to.
 *
 * Nothing on the Apollo side is touched for that first pass. The MCS re-sync
 * below is the right action once the link has genuinely fallen out of DATA, but
 * it is also a candidate for having moved the boundary, so it stays out of the
 * unconditional path.
 *
 * @param phy - AD9088 device, for the Apollo side of the re-sync.
 * @param rx_jesd - Receive JESD204 core.
 * @return 0 if the link is in DATA, negative error code otherwise.
 */
static int dma_example_rx_link_recover(struct ad9088_phy *phy,
				       struct axi_jesd204_rx *rx_jesd)
{
	uint32_t status;
	uint32_t attempt;
	uint32_t poll;
	int ret;

	dma_example_rx_align_errors(rx_jesd, "before re-CGS");

#ifdef CONFIG_ALTERA_PLATFORM_NIOSV
	axi_jesd204_rx_lane_clk_disable(rx_jesd);
	no_os_mdelay(100);
	axi_jesd204_rx_lane_clk_enable(rx_jesd);
#endif

	for (poll = 0; poll < 50; poll++) {
		no_os_mdelay(4);
		status = dma_example_rx_link_status();
		if (status == JESD204_RX_LINK_STATUS_DATA)
			break;
	}

	pr_info("rx_jesd: re-CGS reached %s after %lu ms\n",
		dma_example_rx_link_state_str(rx_jesd, status),
		(unsigned long)(poll * 4));

	dma_example_rx_align_errors(rx_jesd, "after re-CGS");

	if (status == JESD204_RX_LINK_STATUS_DATA)
		return 0;

	pr_info("rx_jesd: link is %s after the re-CGS, recovering\n",
		dma_example_rx_link_state_str(rx_jesd, status));

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
	       dma_example_rx_link_state_str(rx_jesd, status), RX_LINK_ATTEMPTS);

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
 * @brief One I/Q sample of the transmit tone.
 * @param phase - Phase, one turn per 2^32.
 * @return The sample as the buffer stores it, I in the low half and Q in the high.
 *
 * sine_lut_iq holds eight periods in its 1024 entries, so a single period is
 * entries 0..127 and entry j sits at phase j/128 of a turn. A tone that is not a
 * multiple of the sample rate over 128 therefore falls between entries, and the
 * gap is interpolated linearly: at 128 points per period that costs about -83 dBc
 * of spur, measured over the whole buffer, and it avoids linking libm for a table
 * that is generated once.
 *
 * j reaches 127, so j + 1 reaches 128 -- inside the array, and the same phase as
 * entry 0 because the table repeats every 128 entries.
 */
static uint32_t dma_example_tone_sample(uint32_t phase)
{
	uint32_t j = phase >> 25;			/* 2^32 / 128 */
	int32_t frac = (int32_t)((phase >> 17) & 0xFF);
	int32_t i0 = (int16_t)(sine_lut_iq[j] & 0xFFFF);
	int32_t q0 = (int16_t)(sine_lut_iq[j] >> 16);
	int32_t i1 = (int16_t)(sine_lut_iq[j + 1] & 0xFFFF);
	int32_t q1 = (int16_t)(sine_lut_iq[j + 1] >> 16);
	int32_t iv = i0 + (((i1 - i0) * frac) >> 8);
	int32_t qv = q0 + (((q1 - q0) * frac) >> 8);

	return ((uint32_t)(uint16_t)qv << 16) | (uint16_t)iv;
}

/**
 * @brief Fill the transmit buffer with the test tone.
 *
 * One buffer word carries a single I/Q pair, so a link with more than one complex
 * channel needs the same pair repeated once per channel at every sample time --
 * every converter then carries the same tone, which is what makes a quiet channel
 * in the capture mean something.
 *
 * @param num_conv - Converters on the transmit link.
 * @param size - Buffer length to fill, in bytes.
 */
static void dma_example_fill_tone(uint8_t num_conv, uint32_t size)
{
	uint32_t words = size / sizeof(dac_buffer_dma[0]);
	uint32_t stride = num_conv / 2;
	uint32_t samples = stride ? words / stride : 0;
	uint32_t step;
	uint32_t i;

	if (!samples)
		return;

	/*
	 * Exact whenever the sample count divides 2^32, which it does here: the
	 * size is floored to a whole number of table passes, leaving a power of
	 * two. Any other count leaves a sub-step residual that the cyclic wrap
	 * carries as a small phase discontinuity.
	 */
	step = (uint32_t)(((uint64_t)TX_TONE_PERIODS << 32) / samples);

	for (i = 0; i < words; i++)
		dac_buffer_dma[i] = dma_example_tone_sample(step * (i / stride));
}


/**
 * @brief Start the transmit tone from memory, over the DMA.
 * @param tx_dac - The transmit transport layer core.
 * @param tx_dmac - The transmit DMA controller.
 * @param tx_num_conv - Converters the transmit link carries.
 * @return 0 on success, negative error code otherwise.
 */
static int dma_example_start_tx_dma(struct axi_dac *tx_dac,
				    struct axi_dmac *tx_dmac,
				    uint8_t tx_num_conv)
{
	uint32_t tx_offload_size = 0;
	uint32_t tx_offload_cfg = 0;
	uint32_t tx_lut_bytes;
	uint32_t tx_samples;
	uint32_t tx_size;
	int ret;

	/*
	 * Cyclic only when the offload is bypassed. With the offload storing, the
	 * repetition is its job -- oneshot resets to 0 on a transmit instance, so it
	 * replays its memory for ever once a store completes -- and a cyclic source
	 * actively breaks the store: the DMAC re-issues the same descriptor with no
	 * gap and never deasserts xfer_req, so the offload's write FSM and the source
	 * wrap together and whether WR_STATE_WR sees wr_response_eot before the next
	 * pass starts is a race. Observed both ways on the same binary: WAIT_RD/RD on
	 * one run, stuck in WR on the next.
	 */
	struct axi_dma_transfer tx_transfer = {
		.cyclic = TX_OFFLOAD_BYPASS ? CYCLIC : NO,
		.src_addr = (uintptr_t)dac_buffer_dma,
	};

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
	 * Floor to a whole pass of the sine table. The tone itself is continuous
	 * across the wrap by construction (TX_TONE_PERIODS whole periods), but a
	 * table pass is a power of two in samples, which is what makes the phase
	 * step below divide 2^32 exactly.
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
		return ret;
	}

	tx_samples = tx_size / (tx_num_conv * sizeof(uint16_t));

#ifndef CONFIG_ALTERA_PLATFORM_NIOSV
	/*
	 * Both DMACs move data behind the cache, and from here to the end of
	 * the capture there is always one in flight, so the cache is kept out
	 * of the way wholesale rather than flushed and invalidated by range.
	 */
	Xil_DCacheDisable();
#endif

	dma_example_fill_tone(tx_num_conv, tx_size);

#ifdef CONFIG_ALTERA_PLATFORM_NIOSV
	/*
	 * Nios V has no way to disable the data cache at runtime, so the two
	 * buffers are maintained by range instead. The DMA does not snoop, so
	 * the tone has to be written back before the transfer below starts.
	 */
	alt_dcache_flush(dac_buffer_dma, tx_size);
#endif
	ret = axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	if (ret) {
		pr_err("Selecting the DMA data source failed (%d)\n", ret);
		return ret;
	}

	/*
	 * Re-arm the transmit offload, for the same reason the receive one is
	 * re-armed before its capture, but a harder one: on this path the write FSM
	 * cannot recover by itself.
	 *
	 * AUTO_BRINGUP is 1, so the offload leaves WR_STATE_IDLE on its own at
	 * bring-up and stores whatever the DMA has not written yet. It then sits in
	 * WR_STATE_WAIT_RD, which is left only on wr_rd_response_eot -- and this is
	 * the transmit instance, so DST_CYCLIC_EN is 1 and RD_STATE_RD loops back to
	 * itself forever instead of returning to RD_STATE_IDLE
	 * (data_offload_fsm.v:169 and :213). That end-of-transfer therefore never
	 * arrives, the write FSM never returns to IDLE, and init_req is sampled only
	 * in IDLE -- so the real fill is ignored and the offload replays its
	 * bring-up contents for good. The DAC goes quiet and no status bit says why.
	 *
	 * Dropping RESETN_OFFLOAD returns both FSMs to IDLE so the fill below is the
	 * one that gets stored.
	 */
	no_os_axi_io_write(TX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0);
	no_os_axi_io_write(TX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 1);

	/*
	 * Bypass makes the DMA stream straight to the transport layer instead of
	 * filling the offload's memory and letting it replay. That moves the
	 * bandwidth requirement onto the DMA, which then has to keep the DAC fed
	 * for the whole run rather than once; DST_UNDERFLOW after the capture says
	 * whether it kept up.
	 *
	 * With bypass off the offload replays its own memory, so the DMA only has to
	 * fill it. up_bypass is outside the reset above, so it is written after it
	 * either way, and the FSM samples it when the store phase starts.
	 */
	no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_CONFIG, &tx_offload_cfg);
	if (!(tx_offload_cfg & AXI_DO_CONFIG_HAS_BYPASS)) {
		pr_warning("TX data offload: HDL built without bypass\n");
	} else {
		no_os_axi_io_write(TX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_CONTROL,
				   TX_OFFLOAD_BYPASS ? AXI_DO_CONTROL_BYPASS : 0);
		pr_info("TX data offload: bypass %s\n",
			TX_OFFLOAD_BYPASS ? "enabled" : "disabled, replaying "
			"from offload memory");
	}

	/* Latching and W1C, so clear both before the run to have them mean something. */
	no_os_axi_io_write(TX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_STATUS,
			   AXI_DO_STATUS_SRC_OVERFLOW | AXI_DO_STATUS_DST_UNDERFLOW);

	tx_transfer.size = tx_size;

	/*
	 * Cyclic is a build option of the DMAC rather than a guarantee:
	 * axi_dmac_transfer_start() rejects CYCLIC outright on a core synthesised
	 * without it, so an HDL build that lacks it fails here rather than
	 * transmitting one pass.
	 */
	ret = axi_dmac_transfer_start(tx_dmac, &tx_transfer);
	if (ret) {
		pr_err("TX DMA transfer start failed (%d)\n", ret);
		return ret;
	}

	/*
	 * A bounded fill does raise end-of-transfer, so wait for it: that is what
	 * makes the store deterministic, and a timeout here says the DMA never
	 * delivered rather than leaving it to be inferred from the offload's FSM.
	 * A cyclic transfer raises no end-of-transfer and would only time out.
	 */
	if (!TX_OFFLOAD_BYPASS) {
		ret = axi_dmac_transfer_wait_completion(tx_dmac, 1000);
		if (ret) {
			pr_err("TX DMA fill did not complete (%d)\n", ret);
			return ret;
		}
	}

	pr_info("DMA_EXAMPLE Tx: address=%#lx samples=%lu channels=%u bits=%lu\n",
		(unsigned long)(uintptr_t)dac_buffer_dma,
		(unsigned long)tx_samples,
		tx_num_conv, (unsigned long)(8 * sizeof(uint16_t)));

	return 0;
}

int dma_example_main(void)
{
	struct no_os_clk_desc rx_lane_clk = {0};
	struct no_os_clk_desc tx_lane_clk = {0};
	struct jesd204_clk rx_jesd_clk = {0};
	struct jesd204_clk tx_jesd_clk = {0};
	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;
	uint32_t do_status;
	struct adf4382_dev *adf4382_dev;
	struct hmc7044_dev *hmc7044_dev;
	struct adf4030_dev *adf4030_dev;
	struct ad9088_phy *ad9088_phy;
#ifndef CONFIG_ALTERA_PLATFORM_NIOSV
	/*
	 * Xilinx path only. The Agilex 5 design does have an adxcvr control core per
	 * direction (hdl library/intel/adi_jesd204 instantiates axi_adxcvr as
	 * "axi_xcvr", exported as the link_management window), but neither adxcvr
	 * driver fits it: axi_adxcvr.c is Xilinx-only and altera_adxcvr.c reprograms
	 * Arria10/Stratix10 PLLs through per-lane PMA windows GTS does not expose.
	 * There the link cores release the transceiver themselves from xcvr_base, and
	 * the lane clocks keep xcvr == NULL. Calling adxcvr_init() here anyway reads a
	 * Xilinx SYNTH_CONF that does not exist and fails with
	 * "Unknown transceiver type: 0".
	 */
	struct adxcvr *rx_adxcvr;
	struct adxcvr *tx_adxcvr;
#endif
	struct axi_dmac *rx_dmac;
	struct axi_dmac *tx_dmac;
	uint32_t rx_size;
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
	uint8_t rx_num_conv;
	uint8_t side;
	uint8_t np;
	int ret = 0;

	struct jesd204_topology *topology;

	/* The size is filled in once the link geometry is known. */
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

#ifndef CONFIG_ALTERA_PLATFORM_NIOSV
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
#endif

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

	/*
	 * Left off: the readback shows every complex filter bypassed
	 * (enable=0), so forcing the gain changes the profile checksum and
	 * nothing else.
	 */

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



	/*
	 * Converter-side view of the same links, per lane. The FPGA status above
	 * only says what the link cores see; this says what the AD9088 sees, which
	 * is the half that identifies a dead or mismapped lane before a capture is
	 * blamed on the datapath.
	 */

	/*
	 * The ILAS the converter actually sent, per active lane. This is the only
	 * place the far end's own view of L/SCR/F/K/M/N/NP/S/HD is visible, so it is
	 * what settles a parameter disagreement that leaves the link in DATA but the
	 * samples wrong - SCR above all, since descrambling an unscrambled stream
	 * produces full-scale noise and no error anywhere.
	 */
	// for (i = 0; i < rx_jesd->num_lanes; i++)
	// 	axi_jesd204_rx_laneinfo_read(rx_jesd, i);

	/*
	 * Whether the lanes are still erroring or only did so while coming up. The
	 * counters are cumulative, so only a delta answers that.
	 */

	// /*
	//  * Which LMFC offset, if any, stops the frame alignment errors. Diagnostic:
	//  * bounces the link once per offset and restores the entry value.
	//  */

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

	/* Latching and W1C, so clear it before the run to have it mean something. */
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_STATUS,
			   AXI_DO_STATUS_SRC_OVERFLOW | AXI_DO_STATUS_DST_UNDERFLOW);

	ret = dma_example_start_tx_dma(tx_dac, tx_dmac, tx_num_conv);
	if (ret)
		goto error_tx_stream;

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
#ifndef CONFIG_ALTERA_PLATFORM_NIOSV
	Xil_DCacheEnable();
	Xil_DCacheInvalidate();
#else
	alt_dcache_flush_no_writeback(adc_buffer_dma, rx_size);
#endif

	/*
	 * Both offloads, before the timeout is reported: an underflow on TX or an
	 * overflow on RX explains a timeout or a corrupt capture, and is lost once
	 * the example bails out.
	 */
	no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_STATUS, &do_status);
	if (do_status & AXI_DO_STATUS_DST_UNDERFLOW)
		pr_err("TX data offload underflowed: the DMA did not keep the DAC fed\n");
	no_os_axi_io_read(RX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_STATUS, &do_status);
	if (do_status & AXI_DO_STATUS_SRC_OVERFLOW)
		pr_err("RX data offload overflowed: converter data outran its store\n");

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
#ifndef CONFIG_ALTERA_PLATFORM_NIOSV
	adxcvr_remove(rx_adxcvr);
error_tx_adxcvr:
	adxcvr_remove(tx_adxcvr);
error_tx_dmac:
#endif
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
