/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for the ad9088 project (AD9084-EBZ on Agilex 5).
 *
 *   Brings up the JESD204 link exactly as dma_example.c does - the same clock
 *   chips, DMACs, JESD cores, device init, topology and jesd204_fsm_start - but
 *   instead of capturing to a fixed buffer and parking, it serves the AXI ADC
 *   (axi_adc) and AXI DAC (axi_dac) over IIOD on the JTAG UART, so iio_info /
 *   iio_readdev / the IIO oscilloscope drive them.
 *
 *   Agilex 5 / Nios V only. The transport is the Altera JTAG-UART + CLIC IRQ
 *   driver (see iio_app's altera branch); the cache maintenance uses the Nios V
 *   HAL. The bring-up is copied from dma_example.c rather than shared with it so
 *   the hardware-validated capture path stays byte-for-byte unchanged.
 *
 *   @author Mihaela-Georgeta Petrea (Mihaela-georgeta.Petrea@analog.com)
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

#include "iio_example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "ad9088.h"
#include "adi_apollo_clk_mcs.h"
#include "jesd204.h"
#include "axi_adxcvr.h"
#include "axi_dmac.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "no_os_axi_io.h"
#include "jesd204_clk.h"
#include "parameters.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "iio_app.h"

/*
 * Sides the example brings up. The board's HDL is ASYMMETRIC_A_B_MODE=0, so
 * both sides share one JESD core, one transport layer and one DMA: side A's
 * converters land first in a capture and side B's after them. See dma_example.c
 * for the full reasoning; it is reproduced here only as far as deriving the
 * converter geometry the IIO devices are sized from.
 */
#define LOOPBACK_SIDES		2

/* Most converters this example handles per side. */
#define LOOPBACK_CONVERTERS	4

/* Widest capture: every side's converters land in the one buffer. */
#define CAPTURE_CONVERTERS	(LOOPBACK_CONVERTERS * LOOPBACK_SIDES)

/*
 * DMA buffer alignment, in bytes. The DMAC requires the source/destination
 * aligned to its AXI data-path width; 1024 comfortably covers both DMACs and
 * matches dma_example.c.
 */
#define DMA_BUFFER_ALIGN	1024

/*
 * Bring the receive link back if MCS calibration knocks it out - identical in
 * intent to dma_example.c. Kept so the link reaches DATA before IIOD starts
 * serving captures, rather than every iio_readdev timing out.
 */
#define RX_LINK_RECOVER		1
#define RX_LINK_ATTEMPTS	3

/*
 * Receive core link status, from axi_jesd204_rx.c (the driver exports no
 * accessor for the state).
 */
#define JESD204_RX_REG_LINK_STATUS	0x280
#define JESD204_RX_LINK_STATUS_MASK	0x3
#define JESD204_RX_LINK_STATUS_DATA	3

/* Per-lane frame alignment and character error counts, from axi_jesd204_rx.c. */
#define JESD204_RX_REG_LANE_FRAME_ALIGN_ERR(x)	(((x) * 32) + 0x30C)
#define JESD204_RX_REG_LANE_ERRORS(x)		(((x) * 32) + 0x308)

/* Sticky event latch in jesd204_up_common. */
#define JESD204_RX_REG_IRQ_SOURCE		0x088
#define JESD204_RX_IRQ_FRAME_ALIGNMENT_ERROR	NO_OS_BIT(0)
#define JESD204_RX_IRQ_UNEXPECTED_LANE_STATE	NO_OS_BIT(1)

/*
 * axi_data_offload register map (byte offsets = word * 4), the part this
 * example needs. Both offloads are synthesised AUTO_BRINGUP on this bitstream,
 * so they have been running since power-on and must be re-armed once the link
 * exists. See dma_example.c for the full FSM reasoning.
 *
 *   CONFIG.HAS_BYPASS   - the offload was built with a bypass path.
 *   CONTROL.BYPASS      - 1 streams the DMA straight to the transport layer,
 *                         0 stores one fill and replays it.
 *   RESETN_OFFLOAD      - a low-high edge returns both FSMs to IDLE.
 *   STATUS              - W1C src-overflow / dst-underflow latches.
 */
#define AXI_DO_REG_CONFIG		0x0010
#define AXI_DO_CONFIG_HAS_BYPASS	NO_OS_BIT(2)
#define AXI_DO_REG_STATUS		0x0080
#define AXI_DO_STATUS_SRC_OVERFLOW	NO_OS_BIT(4)
#define AXI_DO_STATUS_DST_UNDERFLOW	NO_OS_BIT(5)
#define AXI_DO_REG_RESETN_OFFLOAD	0x0084
#define AXI_DO_REG_CONTROL		0x0088
#define AXI_DO_CONTROL_BYPASS		NO_OS_BIT(0)

/*
 * Static IIO DMA buffers, 1024-byte aligned. The ADC buffer is sized for the
 * widest capture this example accepts (every side at its full converter count);
 * the IIO framework uses read_buff.size as the capture ceiling. The DAC buffer
 * covers the TX offload's own depth (TX_OFFLOAD_MAX_BYTES). Both live in
 * on-chip memory, which the DMA masters see at the same address the CPU does
 * (see parameters.h), so a plain pointer is a valid DMA address.
 */
static uint16_t adc_buffer[ADC_BUFFER_SAMPLES * CAPTURE_CONVERTERS]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

static uint8_t dac_buffer[TX_OFFLOAD_MAX_BYTES]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

/*
 * First link of each side. Which of these the topology declares is decided at
 * runtime from the profile's link_in_use.
 */
static const unsigned int framer_link_id[LOOPBACK_SIDES] = {
	FRAMER_LINK_A0_RX,
	FRAMER_LINK_B0_RX,
};

static const unsigned int deframer_link_id[LOOPBACK_SIDES] = {
	DEFRAMER_LINK_A0_TX,
	DEFRAMER_LINK_B0_TX,
};

/*
 * The core's own state-name tables, non-static in axi_jesd204_rx.c but declared
 * in no header. Declared here rather than copied so the labels track the driver.
 */
extern const char *axi_jesd204_rx_link_status_label[];
extern const char *axi_jesd204_rx_link_status_64b66b_l[];

/**
 * @brief Name of a receive link state, for the encoder in use.
 * @param rx_jesd - The receive link core.
 * @param status - The two-bit link status.
 * @return The state name.
 */
static const char *iio_example_rx_link_state_str(struct axi_jesd204_rx *rx_jesd,
		uint32_t status)
{
	status &= JESD204_RX_LINK_STATUS_MASK;

	return (rx_jesd->encoder == JESD204_ENCODER_8B10B) ?
	       axi_jesd204_rx_link_status_label[status] :
	       axi_jesd204_rx_link_status_64b66b_l[status];
}

/**
 * @brief Current receive link state, as the core reports it.
 * @return The two-bit link status.
 */
static uint32_t iio_example_rx_link_status(void)
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
static void iio_example_rx_align_errors(struct axi_jesd204_rx *rx_jesd,
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
 * Same action as dma_example_rx_link_recover(): bounce the receiver's lane
 * clock through CGS unconditionally so the soft PCS re-aligns, and only if that
 * does not settle re-run the Apollo MCS sequence and bounce again. See
 * dma_example.c for the reasoning; this is a verbatim copy of that logic so the
 * IIO path reaches DATA before IIOD serves captures.
 *
 * @param phy - AD9088 device, for the Apollo side of the re-sync.
 * @param rx_jesd - Receive JESD204 core.
 * @return 0 if the link is in DATA, negative error code otherwise.
 */
static int iio_example_rx_link_recover(struct ad9088_phy *phy,
				       struct axi_jesd204_rx *rx_jesd)
{
	uint32_t status;
	uint32_t attempt;
	uint32_t poll;
	int ret;

	iio_example_rx_align_errors(rx_jesd, "before re-CGS");

	axi_jesd204_rx_lane_clk_disable(rx_jesd);
	no_os_mdelay(100);
	axi_jesd204_rx_lane_clk_enable(rx_jesd);

	for (poll = 0; poll < 50; poll++) {
		no_os_mdelay(4);
		status = iio_example_rx_link_status();
		if (status == JESD204_RX_LINK_STATUS_DATA)
			break;
	}

	pr_info("rx_jesd: re-CGS reached %s after %lu ms\n",
		iio_example_rx_link_state_str(rx_jesd, status),
		(unsigned long)(poll * 4));

	iio_example_rx_align_errors(rx_jesd, "after re-CGS");

	if (status == JESD204_RX_LINK_STATUS_DATA)
		return 0;

	pr_info("rx_jesd: link is %s after the re-CGS, recovering\n",
		iio_example_rx_link_state_str(rx_jesd, status));

	for (attempt = 1; attempt <= RX_LINK_ATTEMPTS; attempt++) {

		ret = adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run(
			      &phy->ad9088);
		if (ret)
			pr_err("Apollo link re-sync failed (%d)\n", ret);

		axi_jesd204_rx_lane_clk_disable(rx_jesd);
		no_os_mdelay(100);
		axi_jesd204_rx_lane_clk_enable(rx_jesd);

		for (poll = 0; poll < 20; poll++) {
			no_os_mdelay(4);
			status = iio_example_rx_link_status();
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
	       iio_example_rx_link_state_str(rx_jesd, status), RX_LINK_ATTEMPTS);

	return -EIO;
}

/*
 * Re-arm the receive data offload ahead of every IIOD capture.
 *
 * The RX offload is synthesised AUTO_BRINGUP and defaults to one-shot, so it
 * holds a fill captured before the link existed and its store phase is long
 * over. iio_axi_adc's read_dev just runs the DMA; without a fresh store the DMA
 * either times out or returns that stale fill. A low-high edge on RESETN_OFFLOAD
 * drops it and starts a new store against the current converter phase, and the
 * DMAC back-pressures until it completes - the same edge dma_example.c issues
 * immediately before its capture. The wrapper and the saved pointer are
 * file-scope statics (no nested functions, which need GCC trampolines and an
 * executable stack).
 */
static int (*iio_example_adc_read_dev_orig)(void *dev, void *buff,
		uint32_t nb_samples);

static int iio_example_adc_read_dev(void *dev, void *buff,
				    uint32_t nb_samples)
{
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 1);

	return iio_example_adc_read_dev_orig(dev, buff, nb_samples);
}

/*
 * Cache maintenance for the IIO DMA buffers. The iio_axi wrappers take
 * void (*)(uint32_t, uint32_t); the Nios V HAL takes void (*)(void *, alt_u32).
 * These thin shims bridge the two rather than casting the function pointer,
 * which -Wcast-function-type would flag. On rv32 pointer and uint32_t are both
 * 32-bit, so the address passes through untouched.
 *   - invalidate: after an RX capture, so the CPU re-reads DMA-written data.
 *   - flush: before a TX playback, so the DMA sees CPU-written data.
 */
static void iio_example_dcache_invalidate(uint32_t addr, uint32_t len)
{
	alt_dcache_flush_no_writeback((void *)addr, len);
}

static void iio_example_dcache_flush(uint32_t addr, uint32_t len)
{
	alt_dcache_flush((void *)addr, len);
}

/**
 * @brief Put the transmit data offload in bypass so a DMA buffer streams.
 *
 * iio_axi_dac plays a buffer with a CYCLIC DMA straight into the transport
 * layer (dest_addr 0), so the offload has to pass the stream through rather than
 * store one fill and replay it: a cyclic source into a storing offload races its
 * write FSM (dma_example.c documents the hang). Bypass makes the DMA feed the
 * DAC continuously, which is exactly the cyclic-source model. On a bitstream
 * built without a bypass path this can only warn; DDS tones still work either
 * way, since those come from the DAC core and never touch the offload.
 */
static void iio_example_tx_offload_bypass(void)
{
	uint32_t cfg = 0;

	no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_CONFIG, &cfg);
	if (!(cfg & AXI_DO_CONFIG_HAS_BYPASS)) {
		pr_warning("TX data offload: HDL built without bypass; DMA buffer "
			   "playback may not stream (DDS tones unaffected)\n");
		return;
	}

	no_os_axi_io_write(TX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_CONTROL,
			   AXI_DO_CONTROL_BYPASS);
	/* W1C latches, cleared before use so a later read means something. */
	no_os_axi_io_write(TX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_STATUS,
			   AXI_DO_STATUS_SRC_OVERFLOW | AXI_DO_STATUS_DST_UNDERFLOW);
	pr_info("TX data offload: bypass enabled (DMA streams to transport)\n");
}

int iio_example_main(void)
{
	struct no_os_clk_desc rx_lane_clk = {0};
	struct no_os_clk_desc tx_lane_clk = {0};
	struct jesd204_clk rx_jesd_clk = {0};
	struct jesd204_clk tx_jesd_clk = {0};
	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;
	struct adf4382_dev *adf4382_dev;
	struct hmc7044_dev *hmc7044_dev;
	struct adf4030_dev *adf4030_dev;
	struct ad9088_phy *ad9088_phy;
#ifndef CONFIG_ALTERA_PLATFORM_NIOSV
	/*
	 * Xilinx path only, kept so this file also compiles there if ever wired
	 * in. The Agilex 5 link cores release the transceiver themselves, so the
	 * lane clocks keep xcvr == NULL and no adxcvr driver is initialised. See
	 * dma_example.c for the full explanation.
	 */
	struct adxcvr *rx_adxcvr;
	struct adxcvr *tx_adxcvr;
#endif
	struct axi_dmac *rx_dmac;
	struct axi_dmac *tx_dmac;
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

	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_axi_dac_desc *iio_axi_dac_desc;
	struct iio_device *adc_dev_desc;
	struct iio_device *dac_dev_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	pr_info("Enter IIO example\n");

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
	 * Enables MCS calibration, which trims the AD9088's internal SYSREF onto
	 * the external edge. Needs both clock chips probed.
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

	ret = ad9088_init(&ad9088_phy, &ad9088_ip);
	if (ret) {
		pr_info("AD9088 initialization failed\n");
		goto error_tx_jesd;
	}

	/*
	 * Bring up the first link of every side the profile has in use. Both
	 * sides frame into the one JESD core and transport layer, so this list is
	 * what decides whether the capture is one side wide or both.
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
	 * The SYSREF provider (the ADF4030) has to precede the top device: see
	 * dma_example.c. Entries left without links are given every link in use
	 * just below.
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

	ret = jesd204_topology_init(&topology, devs, NO_OS_ARRAY_SIZE(devs));
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
	 * WAIT_BS is reported here rather than surfacing as an IIOD capture
	 * timeout later.
	 */
	ret = iio_example_rx_link_recover(ad9088_phy, rx_jesd);
	if (ret)
		goto error_topology;
#endif

	np = ad9088_phy->profile.jtx[0].tx_link_cfg[0].np_minus1 + 1;

	/*
	 * Derive the converter geometry from the links the FSM just brought up,
	 * so a profile change cannot silently mis-size the IIO channels. Every
	 * side frames into the one transport layer, so the receive count is the
	 * sides' counts together and each side owns a contiguous converter window.
	 */
	rx_num_conv = 0;

	for (side = 0; side < LOOPBACK_SIDES; side++) {
		if (!ad9088_phy->profile.jtx[side].tx_link_cfg[0].link_in_use)
			continue;

		link_conv = ad9088_phy->profile.jtx[side]
			    .tx_link_cfg[0].m_minus1 + 1;

		if (!link_conv || link_conv > LOOPBACK_CONVERTERS) {
			pr_err("Side %u converter count M=%u, this example covers "
			       "up to %u per side\n", side, link_conv,
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

	tx_num_conv = 0;

	for (side = 0; side < LOOPBACK_SIDES; side++) {
		if (!ad9088_phy->profile.jrx[side].rx_link_cfg[0].link_in_use)
			continue;

		link_conv = ad9088_phy->profile.jrx[side]
			    .rx_link_cfg[0].m_minus1 + 1;

		if (!link_conv || link_conv > LOOPBACK_CONVERTERS) {
			pr_err("Side %u transmit converter count M=%u, this "
			       "example covers up to %u per side\n", side,
			       link_conv, LOOPBACK_CONVERTERS);
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

	pr_info("Link geometry: RX M=%u TX M=%u NP=%u\n", rx_num_conv,
		tx_num_conv, np);

	for (side = 0; side < LOOPBACK_SIDES; side++) {
		if (!side_conv[side])
			continue;

		pr_info("  Side %u: converters %u..%u\n", side,
			side_first_conv[side],
			side_first_conv[side] + side_conv[side] - 1);
	}

	rx_adc_init.num_channels = rx_num_conv;
	ret = axi_adc_init(&rx_adc, &rx_adc_init);
	if (ret) {
		pr_err("RX TPL core init failed (%d)\n", ret);
		goto error_topology;
	}

	tx_dac_init.num_channels = tx_num_conv;
	ret = axi_dac_init(&tx_dac, &tx_dac_init);
	if (ret) {
		pr_err("TX TPL core init failed (%d)\n", ret);
		goto error_rx_adc;
	}

	/* Keep the DAC quiet until IIOD selects a data source. */
	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_ZERO);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	/* Stream a pushed DMA buffer straight to the transport layer. */
	iio_example_tx_offload_bypass();

	pr_info("Project configured, starting IIOD\n\n");

	/*
	 * ----------------------------------------------------------------
	 * IIO server: AXI ADC (capture) + AXI DAC (DDS / DMA playback).
	 * ----------------------------------------------------------------
	 */
	struct iio_axi_adc_init_param iio_axi_adc_init_par = {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
		.dcache_invalidate_range = iio_example_dcache_invalidate,
	};

	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (ret < 0) {
		pr_err("iio_axi_adc_init() failed (%d)\n", ret);
		goto error_tx_dac;
	}

	struct iio_data_buffer read_buff = {
		.buff = (void *)adc_buffer,
		.size = sizeof(adc_buffer),
	};
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);

	/*
	 * Wrap the ADC capture callback to re-arm the one-shot RX offload before
	 * each DMA. See iio_example_adc_read_dev().
	 */
	iio_example_adc_read_dev_orig = adc_dev_desc->read_dev;
	adc_dev_desc->read_dev = iio_example_adc_read_dev;

	struct iio_axi_dac_init_param iio_axi_dac_init_par = {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
		.dcache_flush_range = iio_example_dcache_flush,
	};

	ret = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if (ret < 0) {
		pr_err("iio_axi_dac_init() failed (%d)\n", ret);
		goto error_iio_adc;
	}

	struct iio_data_buffer write_buff = {
		.buff = (void *)dac_buffer,
		.size = sizeof(dac_buffer),
	};
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc, adc_dev_desc,
			       &read_buff, NULL, NULL),
		IIO_APP_DEVICE("axi_dac", iio_axi_dac_desc, dac_dev_desc,
			       NULL, &write_buff, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = platform_uart_ip;

	/* Let the log messages above flush before IIOD takes over the UART. */
	no_os_mdelay(100);

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_err("iio_app_init() failed (%d)\n", ret);
		goto error_iio_dac;
	}

	/*
	 * iio_app_run() blocks serving IIOD; a normal run does not return. The
	 * teardown below is reached only on an init failure above.
	 */
	return iio_app_run(app);

error_iio_dac:
	iio_axi_dac_remove(iio_axi_dac_desc);
error_iio_adc:
	iio_axi_adc_remove(iio_axi_adc_desc);
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
