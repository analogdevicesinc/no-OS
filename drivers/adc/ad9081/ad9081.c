/***************************************************************************//**
 *   @file   ad9081.c
 *   @brief  Implementation of AD9081 Driver.
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
#include <stdlib.h>
#include <inttypes.h>
#include "error.h"
#include "adi_cms_api_common.h"
#include "util.h"
#include "delay.h"
#include "adi_ad9081_hal.h"
#include "ad9081.h"

#define CHIPID_AD9081	0x9081
#define CHIPID_MASK	0xFFFF

static int32_t ad9081_nco_sync_master_slave(struct ad9081_phy *phy,
		bool master)
{
	int32_t ret;

	/* avoid the glitch before nco reset */
	ret = adi_ad9081_hal_bf_set(&phy->ad9081,
				    REG_MAIN_AUTO_CLK_GATING_ADDR, 0x00000400, 7);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_hal_bf_set(&phy->ad9081,
				    REG_NCOSYNC_MS_MODE_ADDR,
				    BF_NCO_SYNC_MS_EXTRA_LMFC_NUM_INFO,
				    phy->nco_sync_ms_extra_lmfc_num);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_dac_nco_master_slave_gpio_set(&phy->ad9081, 0, master);
	if (ret < 0)
		return ret;
	/* source  0: sysref, 1: lmfc rising edge, 2: lmfc falling edge */
	ret = adi_ad9081_dac_nco_master_slave_trigger_source_set(
		      &phy->ad9081, 1); /* REG 0xCC */
	if (ret < 0)
		return ret;

	ret = adi_ad9081_dac_nco_master_slave_mode_set(&phy->ad9081,
			master ? 1 : 2); /* REG 0xCC */

	adi_ad9081_dac_nco_sync_reset_via_sysref_set(&phy->ad9081, 0);
	adi_ad9081_dac_nco_sync_reset_via_sysref_set(&phy->ad9081, 1);

	if (master)
		return adi_ad9081_dac_nco_master_slave_trigger_set(
			       &phy->ad9081); /* REG 0xBC */

	return ret;
}

static uint64_t ad9081_calc_lanerate(struct ad9081_jesd_link *link,
				     uint64_t converter_rate,
				     uint32_t intp_decim)
{
	uint64_t rate;
	uint64_t encoding_n;
	uint64_t encoding_d;

	if (!intp_decim || !link->jesd_param.jesd_l) {
		printf("%s: Invalid parameter", __func__);
		return 0;
	}

	switch (link->jesd_param.jesd_jesdv) {
	case 2:
		encoding_n = 66; /* JESD 204C */
		encoding_d = 64;
		break;
	default:
		encoding_n = 10; /* JESD 204AB */
		encoding_d = 8;
		break;
	}

	rate = link->jesd_param.jesd_m * link->jesd_param.jesd_np *
	       encoding_n * converter_rate;
	do_div(&rate, link->jesd_param.jesd_l * encoding_d *
	       intp_decim * 1000);

	return rate;
}

static int32_t ad9081_main_decimation_to_val(uint32_t decim)
{
	switch (decim) {
	case 1:
		return AD9081_CDDC_DCM_1;
	case 2:
		return AD9081_CDDC_DCM_2;
	case 3:
		return AD9081_CDDC_DCM_3;
	case 4:
		return AD9081_CDDC_DCM_4;
	case 6:
		return AD9081_CDDC_DCM_6;
	case 8:
		return AD9081_CDDC_DCM_8;
	case 9:
		return AD9081_CDDC_DCM_9;
	case 12:
		return AD9081_CDDC_DCM_12;
	case 16:
		return AD9081_CDDC_DCM_16;
	case 18:
		return AD9081_CDDC_DCM_18;
	case 24:
		return AD9081_CDDC_DCM_24;
	case 36:
		return AD9081_CDDC_DCM_36;
	default:
		return -1;
	}
}

static int32_t ad9081_chan_decimation_to_val(uint32_t decim)
{
	switch (decim) {
	case 1:
		return AD9081_FDDC_DCM_1;
	case 2:
		return AD9081_FDDC_DCM_2;
	case 3:
		return AD9081_FDDC_DCM_3;
	case 4:
		return AD9081_FDDC_DCM_4;
	case 6:
		return AD9081_FDDC_DCM_6;
	case 8:
		return AD9081_FDDC_DCM_8;
	case 12:
		return AD9081_FDDC_DCM_12;
	case 16:
		return AD9081_FDDC_DCM_16;
	case 24:
		return AD9081_FDDC_DCM_24;
	default:
		return -1;
	}
}

static void ad9081_convert_link_converter_select(
	adi_ad9081_jtx_conv_sel_t *jesd_conv_sel, uint8_t *vals)
{
	jesd_conv_sel->virtual_converter0_index = *vals++;
	jesd_conv_sel->virtual_converter1_index = *vals++;
	jesd_conv_sel->virtual_converter2_index = *vals++;
	jesd_conv_sel->virtual_converter3_index = *vals++;
	jesd_conv_sel->virtual_converter4_index = *vals++;
	jesd_conv_sel->virtual_converter5_index = *vals++;
	jesd_conv_sel->virtual_converter6_index = *vals++;
	jesd_conv_sel->virtual_converter7_index = *vals++;
	jesd_conv_sel->virtual_converter8_index = *vals++;
	jesd_conv_sel->virtual_converter9_index = *vals++;
	jesd_conv_sel->virtual_convertera_index = *vals++;
	jesd_conv_sel->virtual_converterb_index = *vals++;
	jesd_conv_sel->virtual_converterc_index = *vals++;
	jesd_conv_sel->virtual_converterd_index = *vals++;
	jesd_conv_sel->virtual_convertere_index = *vals++;
	jesd_conv_sel->virtual_converterf_index = *vals++;
}

static const char *const ad9081_jrx_204c_states[] = {
	"Reset", "Undef", "Sync header alignment done",
	"Extended multiblock sync complete",
	"Extended multiblock alignment complete",
	"Undef", "Link is good", "Undef",
};

static int32_t ad9081_jesd_rx_link_status_print(struct ad9081_phy *phy)
{
	int32_t ret, l;
	uint16_t stat;

	for (l = AD9081_LINK_0; l < AD9081_LINK_ALL; l++) {

		ret = adi_ad9081_jesd_rx_link_status_get(
			      &phy->ad9081, l, &stat);
		if (ret)
			return -1;

		if (phy->jesd_tx_link.jesd_param.jesd_jesdv == 2) {
			stat >>= 8;
			printf("%s: JESD TX (JRX) Link%" PRId32 " 204C status: %s (%d)\n",
			       __func__, l, ad9081_jrx_204c_states[stat & 0x7], stat);

			if (stat == 6) /* FIXME DUAL Link */
				return 0xF;
			else
				return 0;
		} else {
			printf("%s: JESD TX (JRX) Link%" PRId32 " 0x%X lanes in DATA\n",
			       __func__, l, stat & 0xF);
		}

		if (!phy->jesd_tx_link.jesd_param.jesd_duallink)
			return stat & 0xF;
	}

	return stat & 0xF;
}

static const char *const ad9081_jtx_qbf_states[] = {
	"CGS", "ILA_M0R", "ILA_M0", "ILA_M1R", "ILA_M1C1", "ILA_M1C2",
	"ILA_M1C3", "ILA_M1", "ILA_M2R", "ILA_M2", "ILA_M3R", "ILA_M3",
	"ILA_BP", "DATA"
};

static int32_t ad9081_jesd_tx_link_status_print(struct ad9081_phy *phy)
{
	int32_t ret, l;
	uint16_t stat;

	for (l = AD9081_LINK_0; l < AD9081_LINK_ALL; l++) {

		ret = adi_ad9081_jesd_tx_link_status_get(
			      &phy->ad9081, l, &stat);

		if (ret)
			return -1;

		if (phy->jesd_rx_link[l - 1].jesd_param.jesd_jesdv == 2)
			printf("%s: JESD RX (JTX) Link%" PRId32 " PLL %s, PHASE %s, MODE %s\n",
			       __func__, l,
			       (stat & BIT(5)) ? "locked" : "unlocked",
			       (stat & BIT(6)) ? "established" : "lost",
			       (stat & BIT(7)) ? "invalid" : "valid");
		else
			printf("%s: JESD RX (JTX) Link%" PRId32
			       " in %s, SYNC %s, PLL %s, PHASE %s, MODE %s\n",
			       __func__, l, ad9081_jtx_qbf_states[stat & 0xF],
			       (stat & BIT(4)) ? "deasserted" : "asserted",
			       (stat & BIT(5)) ? "locked" : "unlocked",
			       (stat & BIT(6)) ? "established" : "lost",
			       (stat & BIT(7)) ? "invalid" : "valid");


		if (!phy->jesd_rx_link[l - 1].jesd_param.jesd_duallink)
			return 0;
	}

	return 0;
}

static int32_t ad9081_multichip_sync(struct ad9081_phy *phy, int step)
{
	int ret;

	printf("%s:%d\n", __func__, step);

	switch (step & 0xFF) {
	case 0:
		/* enable txfe link */
		ret = adi_ad9081_jesd_tx_link_enable_set(&phy->ad9081,
				(phy->jesd_rx_link[0].jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 0);
		if (ret != 0)
			return ret;

		if (phy->jesd_rx_clk)
			clk_disable(phy->jesd_rx_clk);

		break;
	case 1:
		/* enable txfe link */
		ret = adi_ad9081_jesd_tx_link_enable_set(&phy->ad9081,
				(phy->jesd_rx_link[0].jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 1);
		if (ret != 0)
			return ret;

		if (phy->jesd_rx_clk) {
			ret = clk_enable(phy->jesd_rx_clk);
			if (ret < 0) {
				printf("Failed to enable JESD204 link: %d\n",
				       ret);
				return ret;
			}
		}
		ad9081_jesd_tx_link_status_print(phy);
		break;
	case 2:

		ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				(phy->jesd_tx_link.jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 0);
		if (ret != 0)
			return ret;

		if (phy->jesd_tx_clk)
			clk_disable(phy->jesd_tx_clk);

		break;
	case 3:

		if (phy->jesd_tx_clk) {
			ret = clk_enable(phy->jesd_tx_clk);
			if (ret < 0) {
				printf("Failed to enable JESD204 link: %d\n",
				       ret);
				return ret;
			}
		}

		ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				(phy->jesd_tx_link.jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 1);
		if (ret != 0)
			return ret;

		ad9081_jesd_rx_link_status_print(phy);
		break;
	case 4:
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

		break;
	case 5:
		ret = ad9081_nco_sync_master_slave(phy,
						   phy->jesd_rx_clk ? true : false);
		if (ret != 0)
			return ret;
		break;
	case 6:
		ret = adi_ad9081_dac_nco_sync_set(&phy->ad9081, step >> 8);
		break;
	case 7:
		ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				(phy->jesd_tx_link.jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 0);
		if (ret != 0)
			return ret;
		mdelay(1);
		ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				(phy->jesd_tx_link.jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 1);
		ad9081_jesd_rx_link_status_print(phy);
		if (ret != 0)
			return ret;
		break;
	default:
		return -1;
	}

	return 0;
}

void ad9081_work_func(struct ad9081_phy *phy)
{
	uint8_t status;

	adi_ad9081_hal_reg_get(&phy->ad9081, REG_IRQ_STATUS0_ADDR,
			       &status);

	if (!(status & BIT(6))) {
		printf("IRQ_STATUS0: 0x%X\n", status);
		if (phy->jesd_tx_link.jesd_param.jesd_jesdv == 2) {
			ad9081_multichip_sync(phy, 7);
		} else {
			ad9081_multichip_sync(phy, 2);
			mdelay(20);
			ad9081_multichip_sync(phy, 3);
		}
	}
}

static int32_t ad9081_setup(struct ad9081_phy *phy)
{
	uint64_t tx_lane_rate_kbps;
	uint64_t dev_frequency_hz;
	uint8_t txfe_pll_stat;
	int32_t i;
	int32_t ret;
	adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2];
	adi_cms_jesd_param_t jesd_param[2];
	uint8_t dcm;
	int32_t stat;
	int32_t retry = 5;
	uint64_t status64;
	uint64_t sample_rate;
	uint64_t rx_lane_rate_kbps;

	clk_recalc_rate(phy->dev_clk, &dev_frequency_hz);

	tx_lane_rate_kbps = ad9081_calc_lanerate(&phy->jesd_tx_link,
			    phy->dac_frequency_hz,
			    phy->tx_main_interp * phy->tx_chan_interp);

	/* The 204c calibration routine requires the link to be up */
	if (phy->jesd_tx_clk) {
		ret = clk_set_rate(phy->jesd_tx_clk, tx_lane_rate_kbps);
		if (ret < 0) {
			printf("Failed to set lane rate to %llu kHz: %"PRId32"\n",
			       tx_lane_rate_kbps, ret);
		}
		if (phy->jesd_tx_link.jesd_param.jesd_jesdv == 2) {
			ret = clk_enable(phy->jesd_tx_clk);
			if (ret < 0) {
				printf("Failed to enable JESD204 link: %"PRId32"\n", ret);
				return ret;
			}
		}
	}

	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYNC_LMFC_DELAY_ADDR,
				    BF_SYNC_LMFC_DELAY_SET_INFO,
				    BF_SYNC_LMFC_DELAY_SET(phy->lmfc_delay));
	if (ret != 0)
		return ret;

	/* AC couple SYSREF */
	ret = adi_ad9081_jesd_rx_sysref_input_mode_set(&phy->ad9081, 0);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYNC_DEBUG0_ADDR,
				    BF_AVRG_FLOW_EN_INFO, 1);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYSREF_AVERAGE_ADDR,
				    BF_SYSREF_AVERAGE_INFO,
				    BF_SYSREF_AVERAGE(7));
	if (ret != 0)
		return ret;

	ret = adi_ad9081_device_clk_config_set(
		      &phy->ad9081, phy->dac_frequency_hz, phy->adc_frequency_hz,
		      dev_frequency_hz, dev_frequency_hz != phy->dac_frequency_hz);
	if (ret != 0)
		return ret;

	if (dev_frequency_hz != phy->dac_frequency_hz) {
		ret = adi_ad9081_device_clk_pll_lock_status_get(&phy->ad9081,
				&txfe_pll_stat);
		if (ret != 0)
			return ret;

		if (txfe_pll_stat != 3) {
			printf("CLK PLL Failed to Lock (Status: %d)",
			       txfe_pll_stat);
			return -1;
		}
	}

	/* start txfe tx */
	ret = adi_ad9081_device_startup_tx(
		      &phy->ad9081, phy->tx_main_interp, phy->tx_chan_interp,
		      phy->tx_dac_chan_xbar, phy->tx_main_shift, phy->tx_chan_shift,
		      &phy->jesd_tx_link.jesd_param);

	if (ret != 0)
		return ret;

	for (i = 0; i < ARRAY_SIZE(phy->adc_main_decimation); i++) {
		ret = ad9081_main_decimation_to_val(
			      phy->adc_main_decimation[i]);
		if (ret >= 0)
			phy->rx_cddc_dcm[i] = ret;
	}

	for (i = 0; i < ARRAY_SIZE(phy->adc_chan_decimation); i++) {
		ret = ad9081_chan_decimation_to_val(
			      phy->adc_chan_decimation[i]);
		if (ret >= 0)
			phy->rx_fddc_dcm[i] = ret;
	}

	/* FIXME - the API should change here */
	ad9081_convert_link_converter_select(&jesd_conv_sel[0],
					     phy->jesd_rx_link[0].link_converter_select);
	ad9081_convert_link_converter_select(&jesd_conv_sel[1],
					     phy->jesd_rx_link[1].link_converter_select);

	jesd_param[0] = phy->jesd_rx_link[0].jesd_param;
	jesd_param[1] = phy->jesd_rx_link[1].jesd_param;

	/* start txfe rx and set other settings for normal use cases */
	/* start txfe rx */
	ret = adi_ad9081_device_startup_rx(&phy->ad9081, phy->rx_cddc_select,
					   phy->rx_fddc_select,
					   phy->rx_cddc_shift,
					   phy->rx_fddc_shift, phy->rx_cddc_dcm,
					   phy->rx_fddc_dcm, phy->rx_cddc_c2r,
					   phy->rx_fddc_c2r, jesd_param,
					   jesd_conv_sel);
	if (ret != 0)
		return ret;

	/* setup txfe dac channel gain */
	ret = adi_ad9081_dac_duc_nco_gains_set(&phy->ad9081,
					       phy->dac_cache.chan_gain);
	if (ret != 0)
		return ret;

	adi_ad9081_jesd_rx_lmfc_delay_set(&phy->ad9081, AD9081_LINK_0,
					  phy->jesd_tx_link.jrx_tpl_phase_adjust);

	ret = adi_ad9081_jesd_rx_lanes_xbar_set(&phy->ad9081, AD9081_LINK_0,
						phy->jesd_tx_link.logiclane_mapping);
	if (ret != 0)
		return ret;

	if (phy->jesd_tx_link.jesd_param.jesd_duallink > 0) {
		ret = adi_ad9081_jesd_rx_lanes_xbar_set(
			      &phy->ad9081, AD9081_LINK_1,
			      phy->jesd_tx_link.logiclane_mapping);
		if (ret != 0)
			return ret;
	}

	ret = adi_ad9081_jesd_tx_lanes_xbar_set(&phy->ad9081, AD9081_LINK_0,
						phy->jesd_rx_link[0].logiclane_mapping);
	if (ret != 0)
		return ret;
	ret = adi_ad9081_jesd_tx_lids_cfg_set(&phy->ad9081, AD9081_LINK_0,
					      phy->jesd_rx_link[0].logiclane_mapping);
	if (ret != 0)
		return ret;


	/* setup txfe jtx converter mapping */
	for (i = 0; i < ARRAY_SIZE(phy->jesd_rx_link[0].link_converter_select);
	     i++) {
		ret = adi_ad9081_jesd_tx_conv_sel_set(
			      &phy->ad9081, AD9081_LINK_0, i,
			      phy->jesd_rx_link[0].link_converter_select[i]);
		if (ret != 0)
			return ret;
	}
	if (phy->jesd_rx_link[0].jesd_param.jesd_duallink > 0) {
		for (i = 0;
		     i < ARRAY_SIZE(phy->jesd_rx_link[1].link_converter_select);
		     i++) {
			ret = adi_ad9081_jesd_tx_conv_sel_set(
				      &phy->ad9081, AD9081_LINK_1, i,
				      phy->jesd_rx_link[1].link_converter_select[i]);
			if (ret != 0)
				return ret;
		}
	}

	ret = adi_ad9081_adc_chip_dcm_ratio_get(&phy->ad9081,
						AD9081_LINK_0, &dcm);
	if (ret != 0 || !dcm)
		return ret;

	if (phy->config_sync_01_swapped) {
		adi_ad9081_jesd_rx_syncb_driver_powerdown_set(&phy->ad9081, 0);
		adi_ad9081_hal_reg_set(&phy->ad9081,
				       REG_GENERAL_JRX_CTRL_ADDR, 0x80);
		adi_ad9081_jesd_tx_sync_mode_set(&phy->ad9081,
						 AD9081_LINK_0, 1);
	}

	if (phy->jesd_rx_clk) {
		rx_lane_rate_kbps = ad9081_calc_lanerate(&phy->jesd_rx_link[0],
				    phy->adc_frequency_hz,
				    dcm);

		ret = clk_set_rate(phy->jesd_rx_clk, rx_lane_rate_kbps);
		if (ret < 0) {
			printf("Failed to set lane rate to %llu kHz: %"PRId32"\n",
			       rx_lane_rate_kbps, ret);
		}
	}

	if ((phy->jesd_tx_link.jesd_param.jesd_jesdv == 2) &&
	    (tx_lane_rate_kbps > 16230000UL)) {
		ret = adi_ad9081_jesd_rx_calibrate_204c(&phy->ad9081);
		if (ret < 0)
			return ret;
	}

	ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
			(phy->jesd_tx_link.jesd_param.jesd_duallink > 0) ?
			AD9081_LINK_ALL : AD9081_LINK_0, 1);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_jesd_tx_link_enable_set(&phy->ad9081,
			(phy->jesd_rx_link[0].jesd_param.jesd_duallink > 0) ?
			AD9081_LINK_ALL : AD9081_LINK_0, 1);
	if (ret != 0)
		return ret;

	mdelay(10);

	if (phy->jesd_rx_clk) {
		mdelay(10);
		ret = clk_enable(phy->jesd_rx_clk);
		if (ret < 0) {
			printf("Failed to enable JESD204 link: %"PRId32"\n", ret);
			return ret;
		}
	}

	if (phy->jesd_tx_clk &&
	    (phy->jesd_tx_link.jesd_param.jesd_jesdv == 1)) {
		ret = clk_enable(phy->jesd_tx_clk);
		if (ret < 0) {
			printf("Failed to enable JESD204 link: %"PRId32"\n", ret);
			return ret;
		}
	}

	/*
	 * 204c doesn't have a SYNC, so the link should come up.
	 * This needs to be revisited once we move this driver to the
	 * new JESD framework ...
	 */

	if (phy->jesd_tx_link.jesd_param.jesd_jesdv == 2 ||
	    phy->jesd_tx_clk) {
		do {	/* temp workaround until API is fixed */
			mdelay(10);
			stat = ad9081_jesd_rx_link_status_print(phy);
			if (stat <= 0) {
				ret = adi_ad9081_jesd_rx_link_enable_set(
					      &phy->ad9081,
					      (phy->jesd_tx_link.jesd_param.jesd_duallink > 0) ?
					      AD9081_LINK_ALL : AD9081_LINK_0, 0);
				if (ret != 0)
					return ret;

				if (phy->jesd_tx_clk) {
					clk_disable(phy->jesd_tx_clk);

					mdelay(100);

					ret = clk_enable(phy->jesd_tx_clk);
					if (ret < 0) {
						printf("Failed to enable JESD204 link: %"PRId32"\n",
						       ret);
						return ret;
					}
				} else {
					mdelay(100);
				}

				ret = adi_ad9081_jesd_rx_link_enable_set(
					      &phy->ad9081,
					      (phy->jesd_tx_link.jesd_param.jesd_duallink > 0) ?
					      AD9081_LINK_ALL : AD9081_LINK_0, 1);
				if (ret != 0)
					return ret;

				mdelay(100);
			}
		} while (stat <= 0 && retry--);
	}

	ad9081_jesd_tx_link_status_print(phy);

	adi_ad9081_dac_irqs_status_get(&phy->ad9081, &status64);
	printf("DAC IRQ status 0x%"PRIx64"\n", status64);

	sample_rate = phy->adc_frequency_hz;
	do_div(&sample_rate, dcm);

	//clk_set_rate(phy->clks[RX_SAMPL_CLK], sample_rate); // TODO

	sample_rate = phy->dac_frequency_hz;
	do_div(&sample_rate, phy->tx_main_interp * phy->tx_chan_interp);

	//clk_set_rate(phy->clks[TX_SAMPL_CLK], sample_rate); // TODO

	ret = adi_ad9081_adc_nyquist_zone_set(&phy->ad9081,
					      phy->rx_nyquist_zone);
	if (ret != 0)
		return ret;

	ret = ad9081_nco_sync_master_slave(phy,
					   phy->jesd_rx_clk ? true : false);
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

	// schedule_delayed_work(&phy->dwork, msecs_to_jiffies(1000)); // TODO
	ad9081_work_func(phy);

	return 0;
}

static int32_t ad9081_udelay(void *user_data, uint32_t us)
{
	udelay(us);

	return SUCCESS;
}

int32_t ad9081_reset_pin_ctrl(void *user_data, uint8_t enable)
{
	struct ad9081_phy *phy = user_data;

	return gpio_set_value(phy->gpio_reset, enable);
}

static int32_t ad9081_spi_xfer(void *user_data, uint8_t *in_data,
			       uint8_t *out_data, uint32_t size_bytes)
{
	struct ad9081_phy *phy = user_data;
	uint8_t data[6];
	uint16_t bytes_number;
	int32_t ret;
	int32_t i;

	bytes_number = (size_bytes & 0xFF);

	for (i = 0; i < bytes_number; i++)
		data[i] =  in_data[i];

	ret = spi_write_and_read(phy->spi_desc, data, bytes_number);
	if (ret != SUCCESS)
		return FAILURE;

	for (i = 0; i < bytes_number; i++)
		out_data[i] =  data[i];

	return SUCCESS;
}

int32_t ad9081_log_write(void *user_data, int32_t log_type, const char *message,
			 va_list argp)
{
	char logMessage[160];

	vsnprintf(logMessage, sizeof(logMessage), message, argp);

	switch (log_type) {
	case ADI_CMS_LOG_NONE:
		break;
	case ADI_CMS_LOG_MSG:
		break;
	case ADI_CMS_LOG_WARN:
		printf("%s\n", logMessage);
		break;
	case ADI_CMS_LOG_ERR:
		printf("%s\n", logMessage);
		break;
	case ADI_CMS_LOG_SPI:
		break;
	case ADI_CMS_LOG_API:
		break;
	case ADI_CMS_LOG_ALL:
		printf(logMessage);
		break;
	}

	return 0;
}

int32_t ad9081_parse_jesd_link_init_param(struct ad9081_jesd_link *link,
		const struct link_init_param *init_param, bool jtx)
{
	int32_t i;

	link->jesd_param.jesd_scr = 1; /* Force scambling on */
	link->jesd_param.jesd_did = init_param->device_id;
	link->jesd_param.jesd_f = init_param->octets_per_frame;
	link->jesd_param.jesd_k = init_param->frames_per_multiframe;
	link->jesd_param.jesd_s = init_param->samples_per_converter_per_frame;
	link->jesd_param.jesd_hd = init_param->high_density;
	link->jesd_param.jesd_n = init_param->converter_resolution;
	link->jesd_param.jesd_np = init_param->bits_per_sample;
	link->jesd_param.jesd_m = init_param->converters_per_device;
	link->jesd_param.jesd_cs = init_param->control_bits_per_sample;
	link->jesd_param.jesd_l = init_param->lanes_per_device;
	link->jesd_param.jesd_subclass = init_param->subclass;
	link->jesd_param.jesd_mode_id = init_param->link_mode;
	link->jesd_param.jesd_duallink = init_param->dual_link;
	link->jesd_param.jesd_jesdv = init_param->version;
	for (i = 0; i < ARRAY_SIZE(link->logiclane_mapping); i++)
		link->logiclane_mapping[i] =
			init_param->logical_lane_mapping[i];
	if (jtx) /* JTX - for RX ADC path */
		for (i = 0; i < ARRAY_SIZE(link->link_converter_select); i++)
			link->link_converter_select[i] =
				init_param->link_converter_select[i];
	else /* JRX */
		link->jrx_tpl_phase_adjust = init_param->tpl_phase_adjust;

	return SUCCESS;
}

int32_t ad9081_parse_init_param(struct ad9081_phy *phy,
				const struct ad9081_init_param *init_param)
{
	int32_t i;

	phy->multidevice_instance_count = init_param->multidevice_instance_count;
	phy->config_sync_01_swapped = init_param->jesd_sync_pins_01_swap_enable;
	phy->lmfc_delay = init_param->lmfc_delay_dac_clk_cycles;
	phy->nco_sync_ms_extra_lmfc_num = init_param->nco_sync_ms_extra_lmfc_num;
	/* TX */
	phy->dac_frequency_hz = init_param->dac_frequency_hz;

	/* The 4 DAC Main Datapaths */
	phy->tx_main_interp = init_param->tx_main_interpolation;
	for (i = 0; i < MAX_NUM_MAIN_DATAPATHS; i++) {
		phy->tx_main_shift[i] = init_param->tx_main_nco_frequency_shift_hz[i];
		phy->tx_dac_chan_xbar[i] = init_param->tx_dac_channel_crossbar_select[i];
	}
	/* The 8 DAC Channelizers */
	phy->tx_chan_interp = init_param->tx_channel_interpolation;
	for (i = 0; i < MAX_NUM_CHANNELIZER; i++) {
		phy->tx_chan_shift[i] = init_param->tx_channel_nco_frequency_shift_hz[i];
		phy->dac_cache.chan_gain[i] = init_param->tx_channel_gain[i];
	}
	/* TX JESD Link */
	ad9081_parse_jesd_link_init_param(&phy->jesd_tx_link,
					  init_param->jesd_tx_link, false);
	/* RX */
	phy->adc_frequency_hz = init_param->adc_frequency_hz;
	phy->rx_nyquist_zone = init_param->nyquist_zone;
	/* The 4 ADC Main Datapaths */

	for (i = 0; i < MAX_NUM_MAIN_DATAPATHS; i++) {
		phy->rx_cddc_shift[i] = init_param->rx_main_nco_frequency_shift_hz[i];
		phy->adc_main_decimation[i] = init_param->rx_main_decimation[i];
		phy->rx_cddc_c2r[i] = init_param->rx_main_complex_to_real_enable[i];
		if (init_param->rx_main_enable[i])
			phy->rx_cddc_select |= BIT(i);
	}
	/* The 8 ADC Channelizers */
	for (i = 0; i < MAX_NUM_CHANNELIZER; i++) {
		phy->rx_fddc_shift[i] = init_param->rx_channel_nco_frequency_shift_hz[i];
		phy->adc_chan_decimation[i] = init_param->rx_channel_decimation[i];
		phy->rx_fddc_c2r[i] = init_param->rx_channel_complex_to_real_enable[i];
		if (init_param->rx_channel_enable[i])
			phy->rx_fddc_select |= BIT(i);
	}
	/* RX JESD Link */
	if (init_param->jesd_rx_link[0])
		ad9081_parse_jesd_link_init_param(&phy->jesd_rx_link[0],
						  init_param->jesd_rx_link[0], true);

	if (init_param->jesd_rx_link[1])
		ad9081_parse_jesd_link_init_param(&phy->jesd_rx_link[1],
						  init_param->jesd_rx_link[1], true);

	return SUCCESS;
}

/**
 * Initialize the device.
 * @param dev - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad9081_init(struct ad9081_phy **dev,
		    const struct ad9081_init_param *init_param)
{
	adi_cms_chip_id_t chip_id;
	struct ad9081_phy *phy;
	uint8_t api_rev[3];
	uint32_t fw_rev[2];
	int32_t ret;

	phy = (struct ad9081_phy *)calloc(1, sizeof(*phy));
	if (!phy)
		return FAILURE;

	ret = gpio_get(&phy->gpio_reset, init_param->gpio_reset);
	if (ret < 0)
		goto error_1;

	ret = spi_init(&phy->spi_desc, init_param->spi_init);
	if (ret < 0)
		goto error_2;

	phy->dev_clk = init_param->dev_clk;
	phy->jesd_rx_clk = init_param->jesd_rx_clk;
	phy->jesd_tx_clk = init_param->jesd_tx_clk;

	ad9081_parse_init_param(phy, init_param);

	phy->ad9081.hal_info.user_data = phy;
	phy->ad9081.hal_info.delay_us = ad9081_udelay;
	phy->ad9081.hal_info.reset_pin_ctrl = ad9081_reset_pin_ctrl;
	phy->ad9081.hal_info.sdo = SPI_SDO;
	phy->ad9081.hal_info.msb = SPI_MSB_FIRST;
	phy->ad9081.hal_info.addr_inc = SPI_ADDR_INC_AUTO;
	phy->ad9081.hal_info.spi_xfer = ad9081_spi_xfer;
	phy->ad9081.hal_info.log_write = ad9081_log_write;

	ret = gpio_direction_output(phy->gpio_reset, 1);
	if (ret < 0)
		goto error_3;

	ret = adi_ad9081_device_reset(&phy->ad9081, AD9081_HARD_RESET_AND_INIT);
	if (ret < 0) {
		printf("%s: reset/init failed (%"PRId32")\n", __func__, ret);
		goto error_3;
	}

	ret = adi_ad9081_device_chip_id_get(&phy->ad9081, &chip_id);
	if (ret < 0) {
		printf("%s: chip_id failed (%"PRId32")\n", __func__, ret);
		goto error_3;
	}

	if ((chip_id.prod_id & CHIPID_MASK) != CHIPID_AD9081) {
		printf("%s: Unrecognized CHIP_ID 0x%X\n", __func__,
		       chip_id.prod_id);
		ret = FAILURE;
		goto error_3;
	}

	ret = ad9081_setup(phy);
	if (ret < 0) {
		printf("%s: ad9081_setup failed (%"PRId32")\n", __func__, ret);
		goto error_3;
	}

	adi_ad9081_device_api_revision_get(&phy->ad9081, &api_rev[0],
					   &api_rev[1], &api_rev[2]);

	adi_ad9081_device_firmware_revision_get(&phy->ad9081, &fw_rev[0]);
	adi_ad9081_device_firmware_patch_revision_get(&phy->ad9081, &fw_rev[1]);

	printf("AD9081 Rev. %u Grade %u Firmware %"PRId32".%"PRId32" (API %u.%u.%u) probed\n",
	       chip_id.dev_revision,
	       chip_id.prod_grade, fw_rev[0], fw_rev[1],
	       api_rev[0], api_rev[1], api_rev[2]);

	*dev = phy;

	return SUCCESS;

error_3:
	spi_remove(phy->spi_desc);
error_2:
	gpio_remove(phy->gpio_reset);
error_1:
	free(phy);

	return ret;
}

/**
 * Remove the device - release resources.
 * @param dev - The device structure.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad9081_remove(struct ad9081_phy *dev)
{
	int32_t ret;

	ret = gpio_remove(dev->gpio_reset);
	ret += spi_remove(dev->spi_desc);
	free(dev);

	return ret;
}
