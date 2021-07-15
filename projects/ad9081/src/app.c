/***************************************************************************//**
 *   @file   app.c
 *   @brief  AD9081 application example.
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
#include <stdio.h>
#include <inttypes.h>
#include "error.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "spi.h"
#include "spi_extra.h"
#include "ad9081.h"
#include "adi_ad9081.h"
#include "adi_ad9081_hal.h"
#include "app_clock.h"
#include "app_jesd.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "app_parameters.h"
#include "app_config.h"
#include "print_log.h"
#include "jesd204.h"

#ifdef IIO_SUPPORT
#include "app_iio.h"
#endif

 /*
  * JESD204-FSM defines
  */
#define DEFRAMER_LINK0_TX 0
#define DEFRAMER_LINK1_TX 1
#define FRAMER_LINK0_RX 2
#define FRAMER_LINK1_RX 3

extern struct axi_jesd204_rx *rx_jesd;
extern struct axi_jesd204_tx *tx_jesd;

const char pretty[] = "%-20s: %-10s %-20s() %-10s\n";

struct ad9081_jesd204_priv {
};

static int ad9081_nco_sync_master_slave(struct ad9081_phy *phy, bool master)
{
	int ret;

	ret = adi_ad9081_device_nco_sync_pre(&phy->ad9081);
	if (ret != 0)
		return ret;

	/* trigger_src  0: sysref, 1: lmfc rising edge, 2: lmfc falling edge */

	return adi_ad9081_adc_nco_master_slave_sync(&phy->ad9081,
					     master,
					     1, /* trigger_src */
					     0, /* gpio_index */
					     phy->nco_sync_ms_extra_lmfc_num);
}

int32_t ad9081_jesd_tx_link_dig_reset(adi_ad9081_device_t *device,
				      uint8_t reset)
{
	int32_t err;

	AD9081_NULL_POINTER_RETURN(device);
	AD9081_INVALID_PARAM_RETURN(reset > 1);

	err = adi_ad9081_hal_bf_set(device, REG_FORCE_LINK_RESET_REG_ADDR,
				    BF_FORCE_LINK_DIGITAL_RESET_INFO,
				    reset); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

static const char *const ad9081_jtx_qbf_states[] = {
	"CGS", "ILA_M0R", "ILA_M0", "ILA_M1R", "ILA_M1C1", "ILA_M1C2",
	"ILA_M1C3", "ILA_M1", "ILA_M2R", "ILA_M2", "ILA_M3R", "ILA_M3",
	"ILA_BP", "DATA"
};

int ad9081_jesd_tx_link_status_print(struct ad9081_phy *phy,
				     struct jesd204_link *lnk, int retry)
{
	int ret, l;
	uint16_t stat;

	switch (lnk->link_id) {
	case FRAMER_LINK0_RX:
		l = AD9081_LINK_0;
		break;
	case FRAMER_LINK1_RX:
		l = AD9081_LINK_1;
		break;
	default:
		return -EINVAL;
	}

	do {
		ret = adi_ad9081_jesd_tx_link_status_get(
			&phy->ad9081, l, &stat);
		if (ret)
			return -EFAULT;

		if (lnk->jesd_version == JESD204_VERSION_C) {
			if ((stat & 0x60) == 0x60)
				ret = 0;
			else
				ret = -EIO;

			if (ret == 0 || retry == 0)
				pr_info("JESD RX (JTX) Link%d PLL %s, PHASE %s, MODE %s\n",
					lnk->link_id,
					stat & BIT(5) ? "locked" : "unlocked",
					stat & BIT(6) ? "established" : "lost",
					stat & BIT(7) ? "invalid" : "valid");
			else
				mdelay(20);
		} else {
			if ((stat & 0xFF) == 0x7D)
				ret = 0;
			else
				ret = -EIO;

			if (ret == 0 || retry == 0)
				pr_info("JESD RX (JTX) Link%d in %s, SYNC %s, PLL %s, PHASE %s, MODE %s\n",
					lnk->link_id, ad9081_jtx_qbf_states[stat & 0xF],
					stat & BIT(4) ? "deasserted" : "asserted",
					stat & BIT(5) ? "locked" : "unlocked",
					stat & BIT(6) ? "established" : "lost",
					stat & BIT(7) ? "invalid" : "valid");
			else
				mdelay(20);
		}
	} while (ret && retry--);

	return ret;
}

static const char *const ad9081_jrx_204c_states[] = {
	"Reset", "Undef", "Sync header alignment done",
	"Extended multiblock sync complete",
	"Extended multiblock alignment complete",
	"Undef", "Link is good", "Undef",
};

static int ad9081_jesd_rx_link_status_print(struct ad9081_phy *phy,
				     struct jesd204_link *lnk, int retry)
{
	int ret, l;
	uint16_t stat, mask;

	switch (lnk->link_id) {
	case DEFRAMER_LINK0_TX:
		l = AD9081_LINK_0;
		break;
	case DEFRAMER_LINK1_TX:
		l = AD9081_LINK_1;
		break;
	default:
		return -EINVAL;
	}

	do {
		ret = adi_ad9081_jesd_rx_link_status_get(&phy->ad9081, l, &stat);
		if (ret)
			return -EFAULT;

		if (lnk->jesd_version == JESD204_VERSION_C) {
			stat >>= 8;
			if (stat == 6)
				ret = 0;
			else
				ret = -EIO;

			if (ret == 0 || retry == 0)
				pr_info("JESD TX (JRX) Link%d 204C status: %s (%d)\n",
					lnk->link_id, ad9081_jrx_204c_states[stat & 0x7],
					stat);
			else
				mdelay(20);
		} else {
			mask = (1 << lnk->num_lanes) - 1;

			stat = mask & stat;

			if (stat == mask)
				ret = 0;
			else
				ret = -EIO;

			if (ret == 0 || retry == 0)
				pr_info("JESD TX (JRX) Link%d 0x%X lanes in DATA\n",
					lnk->link_id, stat);
			else
				mdelay(20);
		}
	} while (ret && retry--);

	return ret;
}

int ad9081_jesd204_uninit(struct jesd204_dev *jdev,
			    enum jesd204_state_op_reason reason)
{
	if (reason != JESD204_STATE_OP_REASON_UNINIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __FUNCTION__, __LINE__, jesd204_state_op_reason_str(reason));

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_link_init(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct ad9081_phy *phy = jesd204_dev_priv(jdev);
	struct ad9081_jesd_link *link;
	int ret;

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __FUNCTION__, __LINE__, lnk->link_id, jesd204_state_op_reason_str(reason));

	switch (lnk->link_id) {
	case DEFRAMER_LINK0_TX:
		link = &phy->jesd_tx_link;
		lnk->sample_rate = phy->dac_frequency_hz;
		lnk->sample_rate_div = phy->tx_main_interp * phy->tx_chan_interp;
		break;
	case FRAMER_LINK0_RX:
		link = &phy->jesd_rx_link[0];
		lnk->sample_rate = phy->adc_frequency_hz;
		lnk->sample_rate_div = phy->adc_dcm;
		break;
	default:
		return -EINVAL;
	}

	jesd204_copy_link_params(lnk, &link->jesd204_link);

	if (lnk->jesd_version == JESD204_VERSION_C)
		lnk->jesd_encoder = JESD204_ENCODER_64B66B;
	else
		lnk->jesd_encoder = JESD204_ENCODER_8B10B;

	ret = jesd204_link_get_rate_khz(lnk, &link->lane_rate_kbps);
	if (ret)
		return ret;

	lnk->sysref.mode = JESD204_SYSREF_CONTINUOUS;

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_clks_enable(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct ad9081_phy *phy = jesd204_dev_priv(jdev);
	int ret;

	pr_debug("%s:%d reason %s\n", __FUNCTION__, __LINE__, jesd204_state_op_reason_str(reason));

	if (lnk->is_transmit && (reason == JESD204_STATE_OP_REASON_INIT) &&
		(lnk->jesd_version == JESD204_VERSION_C)) {

		if ((phy->jesd_tx_link.lane_rate_kbps > 16230000UL) &&
			phy->jesd_tx_link.lane_cal_rate_kbps !=
			phy->jesd_tx_link.lane_rate_kbps) {

			ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				(phy->jesd_tx_link.jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 1);
			if (ret != 0)
				return ret;

			pr_info("running jesd_rx_calibrate_204c, LR %lu kbps",
				phy->jesd_tx_link.lane_rate_kbps);

			ret = adi_ad9081_jesd_rx_calibrate_204c(&phy->ad9081, 1, 0, 0);
			if (ret < 0)
				return ret;

			ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				(phy->jesd_tx_link.jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 0);
			if (ret != 0)
				return ret;
		}

		phy->jesd_tx_link.lane_cal_rate_kbps = phy->jesd_tx_link.lane_rate_kbps;
	}

	if (!lnk->is_transmit) {
		/* txfe RX (JTX) link digital reset */
		ret = ad9081_jesd_tx_link_dig_reset(&phy->ad9081,
			reason != JESD204_STATE_OP_REASON_INIT);
		if (ret != 0)
			return ret;

		mdelay(4);
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_link_enable(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct ad9081_phy *phy = jesd204_dev_priv(jdev);
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __FUNCTION__, __LINE__, lnk->link_id, jesd204_state_op_reason_str(reason));

	if (lnk->is_transmit) {
		/* txfe TX (JRX) link */
		ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
			(phy->jesd_tx_link.jesd_param.jesd_duallink > 0) ?
			AD9081_LINK_ALL : AD9081_LINK_0,
			reason == JESD204_STATE_OP_REASON_INIT);
		if (ret != 0)
			return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_link_running(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct ad9081_phy *phy = jesd204_dev_priv(jdev);
	int ret;

	pr_debug("%s:%d reason %s\n", __FUNCTION__, __LINE__, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT) {
//		TODO: evaluate what needs to be done
//		if (lnk->is_transmit && phy->jrx_link_watchdog_en)
//			cancel_delayed_work(&phy->dwork);

		return JESD204_STATE_CHANGE_DONE;
	}

	if (lnk->is_transmit) {
		ret = ad9081_jesd_rx_link_status_print(phy, lnk, 3);
		if (ret < 0)
			return JESD204_STATE_CHANGE_ERROR;
	} else {
		ret = ad9081_jesd_tx_link_status_print(phy, lnk, 3);
		if (ret < 0)
			return JESD204_STATE_CHANGE_ERROR;
	}

//	TODO: evaluate what needs to be done
//	if (lnk->is_transmit && phy->jrx_link_watchdog_en)
//		schedule_delayed_work(&phy->dwork, msecs_to_jiffies(1000));

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_setup_stage1(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)
{
	struct ad9081_phy *phy = jesd204_dev_priv(jdev);
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __FUNCTION__, __LINE__, jesd204_state_op_reason_str(reason));

	ret = ad9081_jesd_tx_link_dig_reset(&phy->ad9081, 0);
	if (ret != 0)
		return ret;

	mdelay(4);

	/* JESD OneShot Sync */
	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYNC_DEBUG0_ADDR,
		BF_AVRG_FLOW_EN_INFO, 1);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYSREF_AVERAGE_ADDR,
		BF_SYSREF_AVERAGE_INFO,
		BF_SYSREF_AVERAGE(7));
	if (ret != 0)
		return ret;

	ret = adi_ad9081_jesd_oneshot_sync(&phy->ad9081);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYNC_DEBUG0_ADDR,
		BF_AVRG_FLOW_EN_INFO, 0);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYSREF_AVERAGE_ADDR,
		BF_SYSREF_AVERAGE_INFO,
		BF_SYSREF_AVERAGE(0));
	if (ret != 0)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_setup_stage2(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)
{
	struct ad9081_phy *phy = jesd204_dev_priv(jdev);
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __FUNCTION__, __LINE__, jesd204_state_op_reason_str(reason));

	/* Master Slave NCO Sync */

	ret = ad9081_nco_sync_master_slave(phy, jesd204_dev_is_top(jdev));
	if (ret != 0)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_setup_stage3(struct jesd204_dev *jdev,
					 enum jesd204_state_op_reason reason)
{
	struct ad9081_phy *phy = jesd204_dev_priv(jdev);
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __FUNCTION__, __LINE__, jesd204_state_op_reason_str(reason));

	ret = adi_ad9081_device_nco_sync_post(&phy->ad9081);
	if (ret != 0)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_ad9081_init = {
	.state_ops = {
		[JESD204_OP_DEVICE_INIT] = {
			.per_device = ad9081_jesd204_uninit,
		},
		[JESD204_OP_LINK_INIT] = {
			.per_link = ad9081_jesd204_link_init,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = ad9081_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = ad9081_jesd204_link_enable,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = ad9081_jesd204_link_running,
		},
		[JESD204_OP_OPT_SETUP_STAGE1] = {
			.per_device = ad9081_jesd204_setup_stage1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_SETUP_STAGE2] = {
			.per_device = ad9081_jesd204_setup_stage2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_SETUP_STAGE3] = {
			.per_device = ad9081_jesd204_setup_stage3,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},

	.max_num_links = 2,
	.num_retries = 3,
	.sizeof_priv = sizeof(struct ad9081_jesd204_priv),
};

int main(void)
{
	struct clk app_clk[MULTIDEVICE_INSTANCE_COUNT];
	struct clk jesd_clk[2];
	struct xil_gpio_init_param  xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	struct gpio_init_param	gpio_phy_resetb = {
		.number = PHY_RESET,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
	};
	struct spi_init_param phy_spi_init_param = {
		.device_id = PHY_SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.mode = SPI_MODE_0,
		.chip_select = PHY_CS,
		.platform_ops = &xil_platform_ops,
		.extra = &xil_spi_param
	};
	struct link_init_param jesd_tx_link = {
		.link_id = DEFRAMER_LINK0_TX,
		.octets_per_frame = AD9081_TX_JESD_F,
		.frames_per_multiframe = AD9081_TX_JESD_K,
		.samples_per_converter_per_frame = AD9081_TX_JESD_S,
		.high_density = AD9081_TX_JESD_HD,
		.converter_resolution = AD9081_TX_JESD_N,
		.bits_per_sample = AD9081_TX_JESD_NP,
		.converters_per_device = AD9081_TX_JESD_M,
		.control_bits_per_sample = AD9081_TX_JESD_CS,
		.lanes_per_device = AD9081_TX_JESD_L,
		.subclass = AD9081_TX_JESD_SUBCLASS,
		.link_mode = AD9081_TX_JESD_MODE,
		.dual_link = 0,
		.version = AD9081_TX_JESD_VERSION,
		.logical_lane_mapping = AD9081_TX_LOGICAL_LANE_MAPPING,
		.tpl_phase_adjust = 12
	};
	struct link_init_param jesd_rx_link = {
		.link_id = FRAMER_LINK0_RX,
		.octets_per_frame = AD9081_RX_JESD_F,
		.frames_per_multiframe = AD9081_RX_JESD_K,
		.samples_per_converter_per_frame = AD9081_RX_JESD_S,
		.high_density = AD9081_RX_JESD_HD,
		.converter_resolution = AD9081_RX_JESD_N,
		.bits_per_sample = AD9081_RX_JESD_NP,
		.converters_per_device = AD9081_RX_JESD_M,
		.control_bits_per_sample = AD9081_RX_JESD_CS,
		.lanes_per_device = AD9081_RX_JESD_L,
		.subclass = AD9081_RX_JESD_SUBCLASS,
		.link_mode = AD9081_RX_JESD_MODE,
		.dual_link = 0,
		.version = AD9081_RX_JESD_VERSION,
		.logical_lane_mapping = AD9081_RX_LOGICAL_LANE_MAPPING,
		.link_converter_select = AD9081_RX_LINK_CONVERTER_SELECT,
	};
	struct ad9081_init_param phy_param = {
		.gpio_reset = &gpio_phy_resetb,
		.spi_init = &phy_spi_init_param,
		.dev_clk = &app_clk[0],
		.jesd_tx_clk = &jesd_clk[1],
		.jesd_rx_clk = &jesd_clk[0],
		.multidevice_instance_count = 1,
#ifdef QUAD_MXFE
		.jesd_sync_pins_01_swap_enable = true,
#else
		.jesd_sync_pins_01_swap_enable = false,
#endif
		.lmfc_delay_dac_clk_cycles = 0,
		.nco_sync_ms_extra_lmfc_num = 0,
		/* TX */
		.dac_frequency_hz = AD9081_DAC_FREQUENCY,
		/* The 4 DAC Main Datapaths */
		.tx_main_interpolation = AD9081_TX_MAIN_INTERPOLATION,
		.tx_main_nco_frequency_shift_hz = AD9081_TX_MAIN_NCO_SHIFT,
		.tx_dac_channel_crossbar_select = AD9081_TX_DAC_CHAN_CROSSBAR,
		/* The 8 DAC Channelizers */
		.tx_channel_interpolation = AD9081_TX_CHAN_INTERPOLATION,
		.tx_channel_nco_frequency_shift_hz = AD9081_TX_CHAN_NCO_SHIFT,
		.tx_channel_gain = AD9081_TX_CHAN_GAIN,
		.jesd_tx_link = &jesd_tx_link,
		/* RX */
		.adc_frequency_hz = AD9081_ADC_FREQUENCY,
		.nyquist_zone = AD9081_ADC_NYQUIST_ZONE,
		/* The 4 ADC Main Datapaths */
		.rx_main_nco_frequency_shift_hz = AD9081_RX_MAIN_NCO_SHIFT,
		.rx_main_decimation = AD9081_RX_MAIN_DECIMATION,
		.rx_main_complex_to_real_enable = {0, 0, 0, 0},
		.rx_main_enable = AD9081_RX_MAIN_ENABLE,
		/* The 8 ADC Channelizers */
		.rx_channel_nco_frequency_shift_hz = AD9081_RX_CHAN_NCO_SHIFT,
		.rx_channel_decimation = AD9081_RX_CHAN_DECIMATION,
		.rx_channel_complex_to_real_enable = {0, 0, 0, 0, 0, 0, 0, 0},
		.rx_channel_enable = AD9081_RX_CHAN_ENABLE,
		.jesd_rx_link[0] = &jesd_rx_link,
		.jesd_rx_link[1] = NULL,
	};

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR
	};
	struct axi_adc *rx_adc;
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.channels = NULL
	};
	struct axi_dac *tx_dac;
	struct axi_dmac_init rx_dmac_init = {
		"rx_dmac",
		RX_DMA_BASEADDR,
		DMA_DEV_TO_MEM,
		0
	};
	struct axi_dmac *rx_dmac;
	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		DMA_MEM_TO_DEV,
		DMA_CYCLIC
	};
	struct axi_dmac *tx_dmac;
	struct ad9081_phy* phy[MULTIDEVICE_INSTANCE_COUNT];
	int32_t status;
	int32_t i;

	printf("Hello\n");

#ifdef QUAD_MXFE
	struct xil_gpio_init_param  xil_gpio_param_2 = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_2_DEVICE_ID
	};
	struct gpio_init_param	ad9081_gpio0_mux_init = {
		.number = AD9081_GPIO_0_MUX,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param_2
	};
	gpio_desc *ad9081_gpio0_mux;

	status = gpio_get(&ad9081_gpio0_mux, &ad9081_gpio0_mux_init);
	if (status)
		return status;

	status = gpio_set_value(ad9081_gpio0_mux, 1);
	if (status)
		return status;
#endif

	status = app_clock_init(app_clk);
	if (status != SUCCESS)
		printf("app_clock_init() error: %" PRId32 "\n", status);

	status = app_jesd_init(jesd_clk,
			       500000, 250000, 250000, 10000000, 10000000);
	if (status != SUCCESS)
		printf("app_jesd_init() error: %" PRId32 "\n", status);

	rx_adc_init.num_channels = 0;
	tx_dac_init.num_channels = 0;

	for (i = 0; i < MULTIDEVICE_INSTANCE_COUNT; i++) {
		gpio_phy_resetb.number = PHY_RESET + i;
		phy_spi_init_param.chip_select = PHY_CS + i;
		phy_param.dev_clk = &app_clk[i];
		jesd_rx_link.device_id = i;

		status = ad9081_init(&phy[i], &phy_param);
		if (status != SUCCESS)
			printf("ad9081_init() error: %" PRId32 "\n", status);

		rx_adc_init.num_channels += phy[i]->jesd_rx_link[0].jesd_param.jesd_m +
					    phy[i]->jesd_rx_link[1].jesd_param.jesd_m;

		tx_dac_init.num_channels += phy[i]->jesd_tx_link.jesd_param.jesd_m *
					    (phy[i]->jesd_tx_link.jesd_param.jesd_duallink > 0 ? 2 : 1);

		phy[i]->ad9081.serdes_info = (adi_ad9081_serdes_settings_t) {
			.ser_settings = { /* txfe jtx */
				.lane_settings = {
					{.swing_setting = AD9081_SER_SWING_850, .pre_emp_setting = AD9081_SER_PRE_EMP_0DB, .post_emp_setting = AD9081_SER_POST_EMP_0DB},
					{.swing_setting = AD9081_SER_SWING_850, .pre_emp_setting = AD9081_SER_PRE_EMP_0DB, .post_emp_setting = AD9081_SER_POST_EMP_0DB},
					{.swing_setting = AD9081_SER_SWING_850, .pre_emp_setting = AD9081_SER_PRE_EMP_0DB, .post_emp_setting = AD9081_SER_POST_EMP_0DB},
					{.swing_setting = AD9081_SER_SWING_850, .pre_emp_setting = AD9081_SER_PRE_EMP_0DB, .post_emp_setting = AD9081_SER_POST_EMP_0DB},
					{.swing_setting = AD9081_SER_SWING_850, .pre_emp_setting = AD9081_SER_PRE_EMP_0DB, .post_emp_setting = AD9081_SER_POST_EMP_0DB},
					{.swing_setting = AD9081_SER_SWING_850, .pre_emp_setting = AD9081_SER_PRE_EMP_0DB, .post_emp_setting = AD9081_SER_POST_EMP_0DB},
					{.swing_setting = AD9081_SER_SWING_850, .pre_emp_setting = AD9081_SER_PRE_EMP_0DB, .post_emp_setting = AD9081_SER_POST_EMP_0DB},
					{.swing_setting = AD9081_SER_SWING_850, .pre_emp_setting = AD9081_SER_PRE_EMP_0DB, .post_emp_setting = AD9081_SER_POST_EMP_0DB},
				},
				.invert_mask = 0x00,
				.lane_mapping = { { 0, 1, 2, 3, 4, 5, 6, 7}, { 0, 1, 2, 3, 4, 5, 6, 7 } }, /* link0, link1 */
			},
			.des_settings = { /* txfe jrx */
				.boost_mask = 0xff,
				.invert_mask = 0x00,
				.ctle_filter = { 2, 2, 2, 2, 2, 2, 2, 2 },
				.lane_mapping =  { { 0, 1, 2, 3, 4, 5, 6, 7 }, { 0, 1, 2, 3, 4, 5, 6, 7} }, /* link0, link1 */
			}
		};
	}


// ##################################################### JESD FRAMEWORK #####################################################
	struct jesd204_dev *topjdev;
	status = jesd204_dev_register(&topjdev, "top", &jesd204_ad9081_init);
	if (status < 0)
		return status;

	jesd204_set_dev_priv(topjdev, phy[0]);

	struct jesd204_dev *link_tx_devices[] = {hmc7044_dev->jdev, /* TODO: add more */};
	struct jesd204_link_dev link_tx = {
			.link_id = DEFRAMER_LINK0_TX,
			.top_device = topjdev,
			.devices = link_tx_devices,
			.num_devices = ARRAY_SIZE(link_tx_devices),
			.link = &phy[0]->jesd_tx_link.jesd204_link,
	};

	struct jesd204_dev *link1_rx_devices[] = {hmc7044_dev->jdev, /* TODO: add more */};
	struct jesd204_link_dev link_rx = {
			.link_id = FRAMER_LINK0_RX,
			.top_device = topjdev,
			.devices = link1_rx_devices,
			.num_devices = ARRAY_SIZE(link1_rx_devices),
			.link = &phy[0]->jesd_rx_link[0].jesd204_link,
	};

	jesd204_link_register(topjdev, &link_tx);
	jesd204_link_register(topjdev, &link_rx);

	jesd204_run(topjdev);

// ##################################################### JESD FRAMEWORK #####################################################

	axi_jesd204_rx_watchdog(rx_jesd);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	axi_dac_init(&tx_dac, &tx_dac_init);
	axi_adc_init(&rx_adc, &rx_adc_init);

	axi_dmac_init(&tx_dmac, &tx_dmac_init);
	axi_dmac_init(&rx_dmac, &rx_dmac_init);

#ifdef IIO_SUPPORT
	printf("The board accepts libiio clients connections through the serial backend.\n");

	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
	};

	struct iio_axi_dac_init_param iio_axi_dac_init_par;
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
	};

	return iio_server_init(&iio_axi_adc_init_par, &iio_axi_dac_init_par);
#else
	printf("Bye\n");

	return SUCCESS;
#endif

}
