/***************************************************************************//**
 *   @file   app_clock.c
 *   @brief  Application clocks initialization.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "hmc7044.h"
#ifdef QUAD_MXFE
#include "adf4371.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#endif
#include "no_os_error.h"
#include "parameters.h"
#include "app_clock.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct hmc7044_dev* hmc7044_dev;

struct no_os_clk_hw hmc7044_hw;

#ifdef QUAD_MXFE
struct adf4371_dev* adf4371_dev[MULTIDEVICE_INSTANCE_COUNT];

struct no_os_clk_hw adf4371_hw[MULTIDEVICE_INSTANCE_COUNT];
#endif

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * @brief Application clock setup.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t app_clock_init(struct no_os_clk dev_refclk[MULTIDEVICE_INSTANCE_COUNT])
{
	int32_t ret;

	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
	};

	struct no_os_spi_init_param clkchip_spi_init_param = {
#ifdef QUAD_MXFE
		.device_id = SPI_2_DEVICE_ID,
#else
		.device_id = CLK_SPI_DEVICE_ID,
#endif
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_0,
#ifdef QUAD_MXFE
		.chip_select = HMC7043_CS,
#else
		.chip_select = CLK_CS,
#endif
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};

#ifdef QUAD_MXFE
	struct hmc7044_chan_spec chan_spec[] = {
		{
			.num = 0,				// FPGA_REFCLK
			.divider = 1,				// 500 MHz
			.driver_mode = 2,			// LVDS
		}, {
			.num = 1,				// SYSREF_MXFE0
			.divider = 256,				// 1.953125 MHz
			.driver_mode = 2,			// LVDS
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.coarse_delay = 0,
			.fine_delay = 24,
			.out_mux_mode = 1,
		}, {
			.num = 3,				// SYSREF_MXFE1
			.divider = 256,				// 1.953125 MHz
			.driver_mode = 2,			// LVDS
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.coarse_delay = 1,
			.fine_delay = 0,
			.out_mux_mode = 0,
		}, {
			.num = 5,				// SYSREF_MXFE2
			.divider = 256,				// 1.953125 MHz
			.driver_mode = 2,			// LVDS
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.coarse_delay = 0,
			.fine_delay = 16,
			.out_mux_mode = 1,
		}, {
			.num = 7,				// SYSREF_MXFE3
			.divider = 256,				// 1.953125 MHz
			.driver_mode = 2,			// LVDS
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.coarse_delay = 0,
			.fine_delay = 0,
			.out_mux_mode = 0,
		}, {
			.num = 8,				// CORE_LINK_CLK
			.divider = 2,				// 250 MHz
			.driver_mode = 2,			// LVDS
		}, {
			.num = 9,				// SYSREF_FPGA
			.divider = 256,				// 1.953125 MHz
			.driver_mode = 2,			// LVDS
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.coarse_delay = 0,
			.fine_delay = 0,
			.out_mux_mode = 0,
		}
	};

	struct hmc7044_init_param hmc7044_param = {
		.spi_init = &clkchip_spi_init_param,
		.is_hmc7043 = true,
		.clkin_freq = {500000000, 0, 0, 0},
		.sysref_timer_div = 1024,
		.pulse_gen_mode = 7,
		.rf_reseeder_disable = true,
		.in_buf_mode = {0x07, 0x07, 0x00, 0x00, 0x00},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x00},
		.gpo_ctrl = {0x37, 0x00, 0x00, 0x00},
		.num_channels = sizeof(chan_spec) /
		sizeof(struct hmc7044_chan_spec),
		.channels = chan_spec
	};

	struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	struct no_os_gpio_init_param gpio_adrf5020_ctrl = {
		.number = ADRF5020_CTRL_GPIO,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_param
	};
	no_os_gpio_desc *adrf5020_ctrl;
	int32_t i;

	ret = no_os_gpio_get(&adrf5020_ctrl, &gpio_adrf5020_ctrl);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(adrf5020_ctrl, 1);
	if (ret)
		return ret;

#else
#if defined(PLATFORM_ZYNQMP)
	struct hmc7044_chan_spec chan_spec[] = {
		{
			.num = 0,		// CORE_CLK_RX
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 2,		// DEV_REFCLK
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 3,		// DEV_SYSREF
			.divider = 1536,	// 1.953125 MHz
			.driver_mode = 2,	// LVDS
			.is_sysref = true,
		}, {
			.num = 6,		// CORE_CLK_TX
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 8,		// CORE_CLK_RX
			.divider = 6,		// 500 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 10,		// CORE_CLK_RX_ALT
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 12,		// FPGA_REFCLK
			.divider = 6,		// 500 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 13,		// FPGA_SYSREF
			.divider = 1536,	// 1.953125 MHz
			.driver_mode = 2,	// LVDS
			.is_sysref = true,
		}
	};
#elif defined(PLATFORM_MB)
	struct hmc7044_chan_spec chan_spec[] = {
		{
			.num = 0,		// CORE_CLK_RX
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 2,		// DEV_REFCLK
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 3,		// DEV_SYSREF
			.divider = 1536,	// 1.953125 MHz
			.driver_mode = 2,	// LVDS
			.is_sysref = true,
		}, {
			.num = 6,		// CORE_CLK_TX
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 8,		// CORE_CLK_RX_ALT2
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 10,		// CORE_CLK_RX_ALT
			.divider = 12,		// 250 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 12,		// FPGA_REFCLK2
			.divider = 6,		// 500 MHz
			.driver_mode = 2,	// LVDS
		}, {
			.num = 13,		// FPGA_SYSREF
			.divider = 1536,	// 1.953125 MHz
			.driver_mode = 2,	// LVDS
			.is_sysref = true,
		}
	};
#endif

	struct hmc7044_init_param hmc7044_param = {
		.spi_init = &clkchip_spi_init_param,
		/*
		* There are different versions of the AD9081-FMCA-EBZ & AD9082-FMCA-EBZ:
		* VCXO = 122.880 MHz, XO = 122.880MHz (AD9081-FMC-EBZ & AD9082-FMC-EBZ)
		* VCXO = 100.000 MHz, XO = 100.000MHz (AD9081-FMC-EBZ-A2 & AD9082-FMC-EBZ-A2)
		* To determine the version, read the frequency printed on the VCXO.
		*/
		//.clkin_freq = {122880000, 30720000, 0, 0},
		//.vcxo_freq = 122880000,
		.clkin_freq = {100000000, 10000000, 0, 0},
		.vcxo_freq = 100000000,
		.pfd1_limit = 0,
		.pll1_cp_current = 0,
		.pll2_freq = 3000000000,
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
#endif

	ret = hmc7044_init(&hmc7044_dev, &hmc7044_param);
	if (ret)
		return ret;

#ifdef QUAD_MXFE
	struct adf4371_chan_spec adf_chan_spec[1] = {
		{
			.num = 2,
			.power_up_frequency = 12000000000,
		}
	};

	struct adf4371_init_param adf4371_param = {
		.spi_init = &clkchip_spi_init_param,
		.spi_3wire_enable = true,
		.clkin_frequency = 500000000,
		.muxout_select = 1,
		.num_channels = 1,
		.channels = adf_chan_spec
	};

	for (i = 0; i < MULTIDEVICE_INSTANCE_COUNT; i++) {
		clkchip_spi_init_param.chip_select = ADF4371_CS + i;
		ret = adf4371_init(&adf4371_dev[i], &adf4371_param);
		if (ret)
			return ret;

		adf4371_hw[i].dev = adf4371_dev[i];
		adf4371_hw[i].dev_clk_recalc_rate = adf4371_clk_recalc_rate;
		adf4371_hw[i].dev_clk_round_rate = adf4371_clk_round_rate;
		adf4371_hw[i].dev_clk_set_rate = adf4371_clk_set_rate;

		dev_refclk[i].hw = &adf4371_hw[i];
		dev_refclk[i].hw_ch_num = 2;
		dev_refclk[i].name = "dev_refclk";
	}
#else
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

#endif

	return 0;
}

/**
 * @brief Application clocking remove.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t app_clock_remove(void)
{
	return hmc7044_remove(hmc7044_dev);
}
