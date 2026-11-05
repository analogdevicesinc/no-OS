/***************************************************************************//**
 *   @file   ad9082/src/common/common_data.c
 *   @brief  Shared bring-up (clocks + JESD204) for the AD9082 VPK180 project.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "parameters.h"
#include "no_os_error.h"
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "jesd204_clk.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/* Clocks */
struct hmc7044_dev *hmc7044_dev;
static struct no_os_clk_hw hmc7044_hw;

/* JESD204 link cores */
struct axi_jesd204_rx *rx_jesd;
struct axi_jesd204_tx *tx_jesd;

static struct jesd204_clk rx_jesd_clk;
static struct jesd204_clk tx_jesd_clk;
static struct no_os_clk_desc rx_lane_clk;
static struct no_os_clk_desc tx_lane_clk;

/**
 * @brief Configure the on-board HMC7044 and export the MxFE device clock.
 *
 * VPK180 / AD9082-FMCA-EBZ: HMC7044 from PLL2 = 3000 MHz.  Clock plan and
 * dividers from versal-vpk180-reva-ad9081.dts (the config the PDI was built
 * for: 24.75 Gbps lanes).  These MUST match the PDI's GT reference clock, or
 * the GT PLL will not lock (reset-done timeout).
 *
 * @return 0 on success, negative error code otherwise.
 */
int32_t app_clock_init(struct no_os_clk dev_refclk[MULTIDEVICE_INSTANCE_COUNT])
{
	int32_t ret;

	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS,
#ifdef SDT
		.base_addr = XPAR_XSPIPS_1_BASEADDR,
#endif
	};

	struct no_os_spi_init_param clkchip_spi_init_param = {
		.device_id = CLK_SPI_DEVICE_ID,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = CLK_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};

	struct hmc7044_chan_spec chan_spec[] = {
		{
			.num = 0,		// CORE_CLK_RX
			.divider = 8,		// 375 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 2,		// DEV_REFCLK (MxFE dev_clk)
			.divider = 8,		// 375 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 3,		// DEV_SYSREF
			.divider = 768,		// 3.90625 MHz
			.driver_mode = 2,	// LVDS
			.is_sysref = true,
		}, {
			.num = 6,		// CORE_CLK_TX
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 8,		// FPGA_REFCLK1
			.divider = 4,		// 750 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 10,		// CORE_CLK_RX
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 12,		// FPGA_REFCLK
			.divider = 8,		// 375 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 13,		// FPGA_SYSREF
			.divider = 768,		// 3.90625 MHz
			.driver_mode = 2,	// LVDS
			.is_sysref = true,
		}
	};

	struct hmc7044_init_param hmc7044_param = {
		.spi_init = &clkchip_spi_init_param,
		/*
		 * AD9082-FMCA-EBZ-A2: VCXO = 100 MHz, XO = 100 MHz.  (The original
		 * AD9082-FMC-EBZ uses 122.88 MHz; read the frequency on the VCXO.)
		 */
		.clkin_freq = {100000000, 10000000, 0, 0},
		.vcxo_freq = 100000000,
		.pfd1_limit = 0,
		.pll1_cp_current = 0,
		.pll2_freq = 3000000000,	/* 24.75 Gbps lane plan */
		.pll1_loop_bw = 200,
		.sysref_timer_div = 1024,
		.in_buf_mode = {0x07, 0x07, 0x00, 0x00, 0x15},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x00},
		.gpo_ctrl = {0x37, 0x33, 0x00, 0x00},
		.num_channels = sizeof(chan_spec) /
		sizeof(struct hmc7044_chan_spec),
		.pll1_ref_prio_ctrl = 0xe4,
		.pll1_ref_autorevert_en = false,
		.sync_pin_mode = 0x1,
		.high_performance_mode_clock_dist_en = false,
		.pulse_gen_mode = 0x0,
		.channels = chan_spec
	};

	/*
	 * Wait for the FMC rail before trusting the clock-chip SPI.
	 *
	 * On the VPK180 the on-board system controller raises VADJ_FMC a few
	 * seconds after power-on, but a JTAG-downloaded app can start before
	 * that -- the AD9082-FMCA-EBZ regulators (and so the HMC7044 / AD9082
	 * SPI buses) are then still off and every read returns 0xFF, which
	 * later fails ad9081_init with -50. hmc7044_init() only *warns* on a
	 * failed read-back (read_write_confirmed stays false) and returns 0, so
	 * poll on that flag: re-init until the clock chip actually answers, or
	 * give up after ~20 s. Once VADJ is up this passes on the first try.
	 */
	for (int tries = 0; tries < 40; tries++) {
		ret = hmc7044_init(&hmc7044_dev, &hmc7044_param);
		if (ret)
			return ret;

		if (hmc7044_dev->read_write_confirmed)
			break;

		hmc7044_remove(hmc7044_dev);
		hmc7044_dev = NULL;

		if (tries == 0)
			pr_info("Waiting for FMC power (VADJ) -- clock SPI reads 0xFF\n");
		no_os_mdelay(500);
	}

	if (!hmc7044_dev || !hmc7044_dev->read_write_confirmed) {
		pr_err("HMC7044 SPI never responded -- check FMC VADJ / seating\n");
		return -ETIMEDOUT;
	}

	hmc7044_hw.dev = hmc7044_dev;

	dev_refclk[0].hw = &hmc7044_hw;
	dev_refclk[0].hw_ch_num = 2;
	dev_refclk[0].name = "dev_refclk";

	struct no_os_clk_desc *clk_desc;
	struct no_os_clk_init_param clk_desc_init = { 0 };

	clk_desc_init.dev_desc = hmc7044_dev;
	clk_desc_init.hw_ch_num = 2;
	clk_desc_init.name = "dev_refclk";
	clk_desc_init.platform_ops = &hmc7044_clk_ops;

	ret = no_os_clk_init(&clk_desc, &clk_desc_init);
	if (ret)
		return ret;

	dev_refclk[0].clk_desc = clk_desc;

	return 0;
}

/**
 * @brief Bring up the AXI-JESD204 RX/TX link cores.
 *
 * Versal has no ADXCVR: the GT line rate is fixed by the PDI, and only the
 * reset handshake is driven from software over the AXI GPIO.  The GT-reset
 * GPIOs are wired into the link FSM; the lane clocks are the fixed jesd204
 * clocks (no transceiver clock output to chain).
 *
 * @return 0 on success, negative error code otherwise.
 */
int32_t app_jesd_init(struct no_os_clk clk[2],
		      uint32_t reference_clk_khz,
		      uint32_t rx_device_clk_khz,
		      uint32_t tx_device_clk_khz,
		      uint32_t rx_lane_clk_khz,
		      uint32_t tx_lane_clk_khz)
{
	int32_t ret;

	struct jesd204_tx_init tx_jesd_init = {
		.name = "tx_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = AD9081_TX_JESD_F,
		.frames_per_multiframe = AD9081_TX_JESD_K,
		.converters_per_device = AD9081_TX_JESD_M *
		MULTIDEVICE_INSTANCE_COUNT,
		.converter_resolution = AD9081_TX_JESD_N,
		.bits_per_sample = AD9081_TX_JESD_NP,
		.high_density = AD9081_TX_JESD_HD,
		.control_bits_per_sample = AD9081_TX_JESD_CS,
		.subclass = AD9081_TX_JESD_SUBCLASS,
		.device_clk_khz = tx_device_clk_khz,
		.lane_clk_khz = tx_lane_clk_khz
	};

	struct jesd204_rx_init rx_jesd_init = {
		.name = "rx_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = AD9081_RX_JESD_F,
		.frames_per_multiframe = AD9081_RX_JESD_K,
		.subclass = AD9081_RX_JESD_SUBCLASS,
		.device_clk_khz = rx_device_clk_khz,
		.lane_clk_khz = rx_lane_clk_khz
	};

	/*
	 * GT reset over AXI GPIO (SDT BSP: XGpio_LookupConfig() matches on base
	 * address, not DeviceId).  Same AXI GPIO controller as the MxFE reset.
	 */
	struct xil_gpio_init_param gt_gpio_extra = {
		.type = GPIO_PL,
#ifdef SDT
		.base_addr = XPAR_XGPIO_0_BASEADDR,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	struct no_os_gpio_init_param tx_gt_pll_reset = {
		.number = TX_GT_PLL_RESET,
		.platform_ops = &xil_gpio_ops,
		.extra = &gt_gpio_extra
	};
	struct no_os_gpio_init_param tx_gt_dp_reset = {
		.number = TX_GT_DP_RESET,
		.platform_ops = &xil_gpio_ops,
		.extra = &gt_gpio_extra
	};
	struct no_os_gpio_init_param tx_gt_reset_done = {
		.number = TX_GT_RESET_DONE,
		.platform_ops = &xil_gpio_ops,
		.extra = &gt_gpio_extra
	};
	struct no_os_gpio_init_param rx_gt_pll_reset = {
		.number = RX_GT_PLL_RESET,
		.platform_ops = &xil_gpio_ops,
		.extra = &gt_gpio_extra
	};
	struct no_os_gpio_init_param rx_gt_dp_reset = {
		.number = RX_GT_DP_RESET,
		.platform_ops = &xil_gpio_ops,
		.extra = &gt_gpio_extra
	};
	struct no_os_gpio_init_param rx_gt_reset_done = {
		.number = RX_GT_RESET_DONE,
		.platform_ops = &xil_gpio_ops,
		.extra = &gt_gpio_extra
	};

	tx_jesd_init.gt_reset_pll = &tx_gt_pll_reset;
	tx_jesd_init.gt_reset_dp = &tx_gt_dp_reset;
	tx_jesd_init.gt_reset_done = &tx_gt_reset_done;
	rx_jesd_init.gt_reset_pll = &rx_gt_pll_reset;
	rx_jesd_init.gt_reset_dp = &rx_gt_dp_reset;
	rx_jesd_init.gt_reset_done = &rx_gt_reset_done;

	/* Unused on Versal (no ADXCVR reference clock to chain). */
	(void)reference_clk_khz;

	tx_lane_clk.platform_ops = &jesd204_clk_ops;
	tx_jesd_init.lane_clk = &tx_lane_clk;
	tx_lane_clk.dev_desc = &tx_jesd_clk;

	ret = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (ret)
		return ret;

	tx_jesd_clk.jesd_tx = tx_jesd;
	clk[1].clk_desc = tx_jesd->lane_clk;

	rx_lane_clk.platform_ops = &jesd204_clk_ops;
	rx_jesd_init.lane_clk = &rx_lane_clk;
	rx_lane_clk.dev_desc = &rx_jesd_clk;

	ret = axi_jesd204_rx_init(&rx_jesd, &rx_jesd_init);
	if (ret)
		return ret;

	rx_jesd_clk.jesd_rx = rx_jesd;
	clk[0].clk_desc = rx_jesd->lane_clk;

	return 0;
}
