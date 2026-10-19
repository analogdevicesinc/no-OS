/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for the ad9088 project
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "iio_example.h"
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
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "iio_app.h"
#include "parameters.h"

/*
 * Sides the example drives.
 *
 * As in the DMA example: the HDL is built ASYMMETRIC_A_B_MODE=0, so both sides
 * share one JESD core, one transport layer and one DMA per direction. The IIO
 * devices below are therefore as wide as both sides together, side A's
 * converters first and side B's after them.
 */
#define IIO_SIDES		2

/* Most converters handled per side; a link carrying more is rejected. */
#define IIO_CONVERTERS		4

/* Widest device: every side's converters behind the one transport layer. */
#define CAPTURE_CONVERTERS	(IIO_CONVERTERS * IIO_SIDES)

/*
 * Default NCOs, see iio_example_set_default_nco(). The coarse NCO is a power of
 * two divisor of the DAC rate so it tunes without residual error.
 */
#define DEFAULT_CNCO_RATE_DIV	8
#define DEFAULT_FNCO_HZ		0

/*
 * Ceiling for the transmit buffer. The TX data offload holds at most this much
 * on this HDL build; the depth it actually has is read back at runtime and the
 * buffer handed to IIO clamped to it.
 */
#define TX_OFFLOAD_MAX_BYTES	(512 * 1024)

/*
 * axi_data_offload registers, from data_offload_regmap.v. MEMORY_SIZE_LSB is
 * the storage depth in bytes, read only; RESETN_OFFLOAD bit 0 low holds the IP
 * in reset.
 */
#define AXI_DO_REG_MEMORY_SIZE_LSB	0x0014
#define AXI_DO_REG_RESETN_OFFLOAD	0x0084

#define DMA_BUFFER_ALIGN	1024

/* Bring the receive link back if MCS calibration knocks it out. */
#define RX_LINK_RECOVER		1
#define RX_LINK_ATTEMPTS	3

/* Receive core link status, from axi_jesd204_rx.c. */
#define JESD204_RX_REG_LINK_STATUS	0x280
#define JESD204_RX_LINK_STATUS_MASK	0x3
#define JESD204_RX_LINK_STATUS_DATA	3

static uint16_t adc_buffer[ADC_BUFFER_SAMPLES * CAPTURE_CONVERTERS]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

static uint16_t dac_buffer[TX_OFFLOAD_MAX_BYTES / sizeof(uint16_t)]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

/*
 * DMA staging buffers. IIO hands out blocks at multiples of samples times
 * bytes per scan inside adc_buffer/dac_buffer, which for a channel count that
 * is not a power of two leaves them off the DMAC bus width (128 bytes on this
 * HDL). The DMAC always works on these instead, and the samples are copied to
 * or from the IIO block. The extra DMA_BUFFER_SLACK samples absorb a final bus
 * beat the DMAC writes in full when the length is not a multiple of it.
 */
#define DMA_BUFFER_SLACK	(DMA_BUFFER_ALIGN / sizeof(uint16_t))

static uint16_t adc_dma_buffer[NO_OS_ARRAY_SIZE(adc_buffer) + DMA_BUFFER_SLACK]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

static uint16_t dac_dma_buffer[NO_OS_ARRAY_SIZE(dac_buffer) + DMA_BUFFER_SLACK]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

static const unsigned int framer_link_id[IIO_SIDES] = {
	FRAMER_LINK_A0_RX,
	FRAMER_LINK_B0_RX,
};

static const unsigned int deframer_link_id[IIO_SIDES] = {
	DEFRAMER_LINK_A0_TX,
	DEFRAMER_LINK_B0_TX,
};

/* read_dev of the iio_axi_adc device, wrapped by iio_example_adc_read_dev(). */
static int (*iio_axi_adc_read)(void *dev, void *buff, uint32_t nb_samples);

/* write_dev of the iio_axi_dac device, see iio_example_dac_write_dev(). */
static int (*iio_axi_dac_write)(void *dev, void *buff, uint32_t nb_samples);

#if RX_LINK_RECOVER
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
static uint32_t iio_example_rx_link_status(void)
{
	uint32_t status = 0;

	no_os_axi_io_read(RX_JESD_BASEADDR, JESD204_RX_REG_LINK_STATUS,
			  &status);

	return status & JESD204_RX_LINK_STATUS_MASK;
}

/**
 * @brief Get the receive link back to DATA if MCS calibration dropped it.
 *
 * Same recovery as the DMA example: re-sync the Apollo links, then take the
 * receiver down and back up so it re-acquires against the phase MCS left
 * behind. The FSM is never restarted, since that would re-run MCS.
 *
 * @param phy - AD9088 device, for the Apollo side of the re-sync.
 * @param rx_jesd - Receive JESD204 core.
 * @return 0 if the link is in DATA, negative error code otherwise.
 */
static int iio_example_rx_link_recover(struct ad9088_phy *phy,
				       struct axi_jesd204_rx *rx_jesd)
{
	uint32_t status = iio_example_rx_link_status();
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
	       rx_link_state[status], RX_LINK_ATTEMPTS);

	return -EIO;
}
#endif

/**
 * @brief Put one side's NCOs on a known default frequency.
 *
 * Tuning every transmit and receive NCO on the side to the same frequency
 * cancels the up and down conversion, so a tone played from the DDS or a DAC
 * buffer comes back over a cabled loopback at the frequency it was sent at,
 * instead of wherever the profile's NCOs would translate it.
 *
 * @param phy - AD9088 device.
 * @param side - Side to tune.
 * @return 0 on success, negative error code otherwise.
 */
static int iio_example_set_default_nco(struct ad9088_phy *phy, uint8_t side)
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
	 * Read every datapath back: one that did not take the tuning is enough
	 * to leave a converter pair empty.
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
 * @brief Capture one IIO buffer, re-arming the RX data offload first.
 *
 * The receive offload is synthesised with AUTO_BRINGUP and runs one-shot: it
 * stores one fill and then holds it. Without a low-high edge on RESETN_OFFLOAD
 * every capture after the first would time out, or return the fill taken
 * before the link was up. The edge belongs immediately before the DMAC is
 * started, so the store runs against live converter data and the DMAC
 * back-pressures until it completes.
 *
 * The DMAC writes to adc_dma_buffer and the samples are copied to the IIO
 * block, which need not be aligned to the DMAC bus width.
 *
 * @param dev - iio_axi_adc descriptor.
 * @param buff - Destination buffer.
 * @param nb_samples - Samples per enabled channel.
 * @return 0 on success, negative error code otherwise.
 */
static int iio_example_adc_read_dev(void *dev, void *buff,
				    uint32_t nb_samples)
{
	struct iio_axi_adc_desc *iio_adc = dev;
	uint32_t scan_bytes;
	uint32_t bytes;
	int ret;

	scan_bytes = no_os_hweight32(iio_adc->mask) *
		     (iio_adc->scan_type_common->storagebits / 8);
	if (!scan_bytes || nb_samples > sizeof(adc_buffer) / scan_bytes)
		return -EINVAL;

	bytes = nb_samples * scan_bytes;

	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 1);

	ret = iio_axi_adc_read(dev, adc_dma_buffer, nb_samples);
	if (ret)
		return ret;

	memcpy(buff, adc_dma_buffer, bytes);

	return 0;
}

/**
 * @brief Play back one IIO buffer through an aligned staging buffer.
 *
 * The transmit DMAC runs cyclic out of dac_dma_buffer, so it is stopped before
 * the new samples are copied in and restarted by the iio_axi_dac write.
 *
 * @param dev - iio_axi_dac descriptor.
 * @param buff - Source buffer.
 * @param nb_samples - Samples per enabled channel.
 * @return 0 on success, negative error code otherwise.
 */
static int iio_example_dac_write_dev(void *dev, void *buff,
				     uint32_t nb_samples)
{
	struct iio_axi_dac_desc *iio_dac = dev;
	uint32_t scan_bytes;
	uint32_t bytes;

	scan_bytes = no_os_hweight32(iio_dac->mask) * sizeof(dac_buffer[0]);
	if (!scan_bytes || nb_samples > sizeof(dac_buffer) / scan_bytes)
		return -EINVAL;

	bytes = nb_samples * scan_bytes;

	axi_dmac_transfer_stop(iio_dac->dmac);
	memcpy(dac_dma_buffer, buff, bytes);

	return iio_axi_dac_write(dev, dac_dma_buffer, nb_samples);
}

/*
 * Cache maintenance for the IIO buffers. Wrapped rather than passed directly
 * because the MicroBlaze BSP implements both as function-like macros, which
 * have no address to take.
 */
static void iio_example_dcache_invalidate(uint32_t addr, uint32_t len)
{
	Xil_DCacheInvalidateRange(addr, len);
}

static void iio_example_dcache_flush(uint32_t addr, uint32_t len)
{
	Xil_DCacheFlushRange(addr, len);
}

/**
 * @brief Clamp an IIO buffer to the depth of the data offload behind it.
 *
 * A request larger than the offload memory can never complete: on receive the
 * DMAC waits for samples the offload does not store, on transmit the offload
 * cannot hold the replay.
 *
 * @param baseaddr - Data offload base address.
 * @param size - Static buffer size, in bytes.
 * @return The usable buffer size, in bytes.
 */
static uint32_t iio_example_offload_clamp(uint32_t baseaddr, uint32_t size)
{
	uint32_t offload_size = 0;

	no_os_axi_io_read(baseaddr, AXI_DO_REG_MEMORY_SIZE_LSB, &offload_size);

	if (offload_size && offload_size < size)
		return offload_size;

	return size;
}

int iio_example_main(void)
{
	struct iio_axi_adc_init_param iio_axi_adc_ip = {0};
	struct iio_axi_dac_init_param iio_axi_dac_ip = {0};
	struct iio_app_init_param app_init_param = {0};
	struct no_os_clk_desc rx_lane_clk = {0};
	struct no_os_clk_desc tx_lane_clk = {0};
	struct jesd204_clk rx_jesd_clk = {0};
	struct jesd204_clk tx_jesd_clk = {0};
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_axi_dac_desc *iio_axi_dac_desc;
	struct iio_device *adc_dev_desc;
	struct iio_device *dac_dev_desc;
	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;
	struct adf4382_dev *adf4382_dev;
	struct hmc7044_dev *hmc7044_dev;
	struct adf4030_dev *adf4030_dev;
	struct ad9088_phy *ad9088_phy;
	struct adxcvr *rx_adxcvr;
	struct adxcvr *tx_adxcvr;
	struct axi_dmac *rx_dmac;
	struct axi_dmac *tx_dmac;
	struct iio_app_desc *app;
	unsigned int link_ids[2 * IIO_SIDES];
	unsigned int links_number = 0;
	unsigned int dev_idx;
	unsigned int link_idx;
	uint8_t link_conv;
	struct axi_adc *rx_adc;
	struct axi_dac *tx_dac;
	uint8_t tx_num_conv;
	uint8_t rx_num_conv;
	uint8_t side;
	int ret = 0;

	struct jesd204_topology *topology;

	struct iio_data_buffer read_buff = {
		.buff = (void *)adc_buffer,
	};

	struct iio_data_buffer write_buff = {
		.buff = (void *)dac_buffer,
	};

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

	/* The first link of every side the profile has in use. */
	for (side = 0; side < IIO_SIDES; side++) {
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
	 * The SYSREF provider has to precede the top device, see the DMA
	 * example. The FPGA cores name side A's link only; entries left without
	 * links are given every link in use just below.
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
	ret = iio_example_rx_link_recover(ad9088_phy, rx_jesd);
	if (ret)
		goto error_topology;
#endif

	/*
	 * Size the transport cores from the links the FSM brought up, so the
	 * IIO devices expose exactly the converters the profile carries.
	 */
	rx_num_conv = 0;

	for (side = 0; side < IIO_SIDES; side++) {
		if (!ad9088_phy->profile.jtx[side].tx_link_cfg[0].link_in_use)
			continue;

		link_conv = ad9088_phy->profile.jtx[side]
			    .tx_link_cfg[0].m_minus1 + 1;

		if (!link_conv || link_conv > IIO_CONVERTERS) {
			pr_err("Side %u converter count M=%u, this example "
			       "covers up to %u per side\n", side, link_conv,
			       IIO_CONVERTERS);
			ret = -EINVAL;
			goto error_topology;
		}

		rx_num_conv += link_conv;
	}

	tx_num_conv = 0;

	for (side = 0; side < IIO_SIDES; side++) {
		if (!ad9088_phy->profile.jrx[side].rx_link_cfg[0].link_in_use)
			continue;

		link_conv = ad9088_phy->profile.jrx[side]
			    .rx_link_cfg[0].m_minus1 + 1;

		if (!link_conv || link_conv > IIO_CONVERTERS) {
			pr_err("Side %u transmit converter count M=%u, this "
			       "example covers up to %u per side\n", side,
			       link_conv, IIO_CONVERTERS);
			ret = -EINVAL;
			goto error_topology;
		}

		tx_num_conv += link_conv;
	}

	if (!rx_num_conv || !tx_num_conv) {
		pr_err("The profile needs a receive and a transmit link in "
		       "use (rx M=%u, tx M=%u)\n", rx_num_conv, tx_num_conv);
		ret = -EINVAL;
		goto error_topology;
	}

	rx_adc_init.num_channels = rx_num_conv;
	ret = axi_adc_init(&rx_adc, &rx_adc_init);
	if (ret) {
		pr_err("RX TPL core init failed (%d)\n", ret);
		goto error_topology;
	}

	/* Starts on the DDS, so a tone plays as soon as the link is up. */
	tx_dac_init.num_channels = tx_num_conv;
	ret = axi_dac_init(&tx_dac, &tx_dac_init);
	if (ret) {
		pr_err("TX TPL core init failed (%d)\n", ret);
		goto error_rx_adc;
	}

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);
	pr_info("Project configured\n\n");

	for (side = 0; side < IIO_SIDES; side++) {
		if (!ad9088_phy->profile.jtx[side].tx_link_cfg[0].link_in_use &&
		    !ad9088_phy->profile.jrx[side].rx_link_cfg[0].link_in_use)
			continue;

		ret = iio_example_set_default_nco(ad9088_phy, side);
		if (ret)
			goto error_tx_dac;
	}

	read_buff.size = iio_example_offload_clamp(RX_DATA_OFFLOAD_BASEADDR,
			 sizeof(adc_buffer[0]) * ADC_BUFFER_SAMPLES *
			 rx_num_conv);
	write_buff.size = iio_example_offload_clamp(TX_DATA_OFFLOAD_BASEADDR,
			  sizeof(dac_buffer));

	iio_axi_adc_ip.rx_adc = rx_adc;
	iio_axi_adc_ip.rx_dmac = rx_dmac;
	iio_axi_adc_ip.dcache_invalidate_range = iio_example_dcache_invalidate;

	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_ip);
	if (ret) {
		pr_err("IIO AXI ADC init failed (%d)\n", ret);
		goto error_tx_dac;
	}

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);
	iio_axi_adc_read = adc_dev_desc->read_dev;
	adc_dev_desc->read_dev = iio_example_adc_read_dev;

	iio_axi_dac_ip.tx_dac = tx_dac;
	iio_axi_dac_ip.tx_dmac = tx_dmac;
	iio_axi_dac_ip.dcache_flush_range = iio_example_dcache_flush;

	ret = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_ip);
	if (ret) {
		pr_err("IIO AXI DAC init failed (%d)\n", ret);
		goto error_iio_adc;
	}

	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);
	iio_axi_dac_write = dac_dev_desc->write_dev;
	dac_dev_desc->write_dev = iio_example_dac_write_dev;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("cf-ad9088-lpc", iio_axi_adc_desc,
			       adc_dev_desc, &read_buff, NULL, NULL),
		IIO_APP_DEVICE("cf-ad9088-dds-core-lpc", iio_axi_dac_desc,
			       dac_dev_desc, NULL, &write_buff, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = platform_uart_ip;

	pr_info("IIO: rx %u channels, %lu byte buffer; tx %u channels, %lu "
		"byte buffer\n", rx_num_conv, (unsigned long)read_buff.size,
		tx_num_conv, (unsigned long)write_buff.size);

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_err("IIO app init failed (%d)\n", ret);
		goto error_iio_dac;
	}

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
