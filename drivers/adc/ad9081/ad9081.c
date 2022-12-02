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
#include "no_os_error.h"
#include "adi_cms_api_common.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "adi_ad9081_hal.h"
#include "ad9081.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define CHIPID_AD9081	0x9081
#define CHIPID_MASK	0xFFFF

#define for_each_cddc(bit, mask) \
	for ((bit) = 0; (bit) < MAX_NUM_MAIN_DATAPATHS; (bit)++) \
		if ((mask) & NO_OS_BIT(bit))

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

	adi_ad9081_adc_ddc_coarse_sync_enable_set(&phy->ad9081,
			AD9081_ADC_CDDC_ALL, 0);
	adi_ad9081_adc_ddc_coarse_sync_enable_set(&phy->ad9081,
			AD9081_ADC_CDDC_ALL, 1);

	adi_ad9081_adc_ddc_fine_sync_enable_set(&phy->ad9081,
						AD9081_ADC_FDDC_ALL, 0);
	adi_ad9081_adc_ddc_fine_sync_enable_set(&phy->ad9081,
						AD9081_ADC_FDDC_ALL, 1);

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
	no_os_do_div(&rate, link->jesd_param.jesd_l * encoding_d *
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
	uint16_t stat = 0;

	for (l = AD9081_LINK_0; l < AD9081_LINK_ALL; l++) {

		ret = adi_ad9081_jesd_rx_link_status_get(
			      &phy->ad9081, l, &stat);
		if (ret)
			return -1;

		if (phy->jrx_link_tx.jesd_param.jesd_jesdv == 2) {
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

		if (!phy->jrx_link_tx.jesd_param.jesd_duallink)
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

		if (phy->jtx_link_rx[l - 1].jesd_param.jesd_jesdv == 2)
			printf("%s: JESD RX (JTX) Link%" PRId32 " PLL %s, PHASE %s, MODE %s\n",
			       __func__, l,
			       (stat & NO_OS_BIT(5)) ? "locked" : "unlocked",
			       (stat & NO_OS_BIT(6)) ? "established" : "lost",
			       (stat & NO_OS_BIT(7)) ? "invalid" : "valid");
		else
			printf("%s: JESD RX (JTX) Link%" PRId32
			       " in %s, SYNC %s, PLL %s, PHASE %s, MODE %s\n",
			       __func__, l, ad9081_jtx_qbf_states[stat & 0xF],
			       (stat & NO_OS_BIT(4)) ? "deasserted" : "asserted",
			       (stat & NO_OS_BIT(5)) ? "locked" : "unlocked",
			       (stat & NO_OS_BIT(6)) ? "established" : "lost",
			       (stat & NO_OS_BIT(7)) ? "invalid" : "valid");


		if (!phy->jtx_link_rx[l - 1].jesd_param.jesd_duallink)
			return 0;
	}

	return 0;
}

static int32_t ad9081_multichip_sync(struct ad9081_phy *phy, int step)
{
	adi_cms_jesd_subclass_e subclass = JESD_SUBCLASS_0;
	int ret;

	printf("%s:%d\n", __func__, step);

	switch (step & 0xFF) {
	case 0:
		/* disable txfe RX (JTX) link */
		ret = adi_ad9081_jesd_tx_link_enable_set(&phy->ad9081,
				(phy->jtx_link_rx[0].jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 0);
		if (ret != 0)
			return ret;

		if (phy->jesd_rx_clk)
			no_os_clk_disable(phy->jesd_rx_clk);

		break;
	case 1:
		/* enable txfe RX (JTX) link */
		ret = adi_ad9081_jesd_tx_link_enable_set(&phy->ad9081,
				(phy->jtx_link_rx[0].jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 1);
		if (ret != 0)
			return ret;

		if (phy->jesd_rx_clk) {
			ret = no_os_clk_enable(phy->jesd_rx_clk);
			if (ret < 0) {
				printf("Failed to enable JESD204 link: %d\n",
				       ret);
				return ret;
			}
		}
		ad9081_jesd_tx_link_status_print(phy);
		break;
	case 2:
		/* disable txfe TX (JRX) link */
		ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				(phy->jrx_link_tx.jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 0);
		if (ret != 0)
			return ret;

		if (phy->jesd_tx_clk)
			no_os_clk_disable(phy->jesd_tx_clk);

		break;
	case 3:
		/* enable txfe TX (JRX) link */
		if (phy->jesd_tx_clk) {
			ret = no_os_clk_enable(phy->jesd_tx_clk);
			if (ret < 0) {
				printf("Failed to enable JESD204 link: %d\n",
				       ret);
				return ret;
			}
		}

		ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				(phy->jrx_link_tx.jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 1);
		if (ret != 0)
			return ret;

		ret = ad9081_jesd_rx_link_status_print(phy);
		if (ret < 0)
			ad9081_work_func(phy);
		//schedule_delayed_work(&phy->dwork, msecs_to_jiffies(1000));	TODO
		break;
	case 4:
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

		if (phy->jrx_link_tx.jesd_param.jesd_subclass ||
		    phy->jtx_link_rx[0].jesd_param.jesd_subclass)
			subclass = JESD_SUBCLASS_1;

		ret = adi_ad9081_jesd_oneshot_sync(&phy->ad9081, subclass);
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
		/* Master Slave NCO Sync */
		ret = ad9081_nco_sync_master_slave(phy,
						   phy->jesd_rx_clk ? true : false);
		if (ret != 0)
			return ret;
		break;
	case 6:
		/* Debug Only */
		ret = adi_ad9081_dac_nco_sync_set(&phy->ad9081, step >> 8);
		break;
	case 7:
		/* Toggle TX (JRX) link */
		ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				(phy->jrx_link_tx.jesd_param.jesd_duallink > 0) ?
				AD9081_LINK_ALL : AD9081_LINK_0, 0);
		if (ret != 0)
			return ret;
		no_os_mdelay(1);
		ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				(phy->jrx_link_tx.jesd_param.jesd_duallink > 0) ?
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

	if (!(status & NO_OS_BIT(6))) {
		printf("IRQ_STATUS0: 0x%X\n", status);
		if (phy->jrx_link_tx.jesd_param.jesd_jesdv == 2) {
			ad9081_multichip_sync(phy, 7);
		} else {
			ad9081_multichip_sync(phy, 2);
			no_os_mdelay(20);
			ad9081_multichip_sync(phy, 3);


			/* disable txfe TX (JRX) link */
			adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
							   (phy->jrx_link_tx.jesd_param.jesd_duallink > 0) ?
							   AD9081_LINK_ALL : AD9081_LINK_0, 0);

			if (phy->jesd_tx_clk)
				no_os_clk_disable(phy->jesd_tx_clk);

			no_os_mdelay(20);

			/* enable txfe TX (JRX) link */
			if (phy->jesd_tx_clk)
				no_os_clk_enable(phy->jesd_tx_clk);

			adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
							   (phy->jrx_link_tx.jesd_param.jesd_duallink > 0) ?
							   AD9081_LINK_ALL : AD9081_LINK_0, 1);

			ad9081_jesd_rx_link_status_print(phy);
		}
	}
	//schedule_delayed_work(&phy->dwork, msecs_to_jiffies(1000));	// TODO
}

static int32_t ad9081_setup(struct ad9081_phy *phy)
{
	adi_cms_jesd_subclass_e subclass = JESD_SUBCLASS_0;
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
	uint32_t timeout;

	no_os_clk_recalc_rate(phy->dev_clk, &dev_frequency_hz);

	tx_lane_rate_kbps = ad9081_calc_lanerate(&phy->jrx_link_tx,
			    phy->dac_frequency_hz,
			    phy->tx_main_interp * phy->tx_chan_interp);

	/* The 204c calibration routine requires the link to be up */
	if (phy->jesd_tx_clk) {
		ret = no_os_clk_set_rate(phy->jesd_tx_clk, tx_lane_rate_kbps);
		if (ret < 0) {
			printf("Failed to set lane rate to %llu kHz: %"PRId32"\n",
			       tx_lane_rate_kbps, ret);
		}
		if (phy->jrx_link_tx.jesd_param.jesd_jesdv == 2) {
			ret = no_os_clk_enable(phy->jesd_tx_clk);
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

	/* DC couple SYSREF default */
	ret = adi_ad9081_jesd_sysref_input_mode_set(&phy->ad9081, 1, 1,
			phy->sysref_coupling_ac_en ? COUPLING_AC : COUPLING_DC);
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
		      dev_frequency_hz);
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
		      &phy->jrx_link_tx.jesd_param);

	if (ret != 0)
		return ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->adc_main_decimation); i++) {
		ret = ad9081_main_decimation_to_val(
			      phy->adc_main_decimation[i]);
		if (ret >= 0)
			phy->rx_cddc_dcm[i] = ret;
	}

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->adc_chan_decimation); i++) {
		ret = ad9081_chan_decimation_to_val(
			      phy->adc_chan_decimation[i]);
		if (ret >= 0)
			phy->rx_fddc_dcm[i] = ret;
	}

	/* FIXME - the API should change here */
	ad9081_convert_link_converter_select(&jesd_conv_sel[0],
					     phy->jtx_link_rx[0].link_converter_select);
	ad9081_convert_link_converter_select(&jesd_conv_sel[1],
					     phy->jtx_link_rx[1].link_converter_select);

	jesd_param[0] = phy->jtx_link_rx[0].jesd_param;
	jesd_param[1] = phy->jtx_link_rx[1].jesd_param;

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

	if (phy->jrx_link_tx.jesd_param.jesd_subclass ||
	    phy->jtx_link_rx[0].jesd_param.jesd_subclass)
		subclass = JESD_SUBCLASS_1;

	ret = adi_ad9081_jesd_oneshot_sync(&phy->ad9081, subclass);
	if (ret != 0)
		return ret;

	/* Fix: 4x4 Crossbar Mux0 Mappings for AD9081 */
	ret  = adi_ad9081_adc_pfir_din_select_set(&phy->ad9081,
			AD9081_ADC_PFIR_ADC_PAIR0, 0, 1);
	if (ret != 0)
		return ret;

	ret  = adi_ad9081_adc_pfir_din_select_set(&phy->ad9081,
			AD9081_ADC_PFIR_ADC_PAIR1, 3, 0);
	if (ret != 0)
		return ret;

	/* setup txfe dac channel gain */
	ret = adi_ad9081_dac_duc_nco_gains_set(&phy->ad9081,
					       phy->dac_cache.chan_gain);
	if (ret != 0)
		return ret;

	adi_ad9081_jesd_rx_lmfc_delay_set(&phy->ad9081, AD9081_LINK_0,
					  phy->jrx_link_tx.jrx_tpl_phase_adjust);

	ret = adi_ad9081_jesd_rx_lanes_xbar_set(&phy->ad9081, AD9081_LINK_0,
						phy->jrx_link_tx.logiclane_mapping);
	if (ret != 0)
		return ret;

	if (phy->jrx_link_tx.jesd_param.jesd_duallink > 0) {
		ret = adi_ad9081_jesd_rx_lanes_xbar_set(
			      &phy->ad9081, AD9081_LINK_1,
			      phy->jrx_link_tx.logiclane_mapping);
		if (ret != 0)
			return ret;
	}

	ret = adi_ad9081_jesd_tx_lanes_xbar_set(&phy->ad9081, AD9081_LINK_0,
						phy->jtx_link_rx[0].logiclane_mapping);
	if (ret != 0)
		return ret;
	ret = adi_ad9081_jesd_tx_lids_cfg_set(&phy->ad9081, AD9081_LINK_0,
					      phy->jtx_link_rx[0].logiclane_mapping);
	if (ret != 0)
		return ret;


	/* Setup txfe jtx converter mapping */
	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->jtx_link_rx[0].link_converter_select);
	     i++) {
		ret = adi_ad9081_jesd_tx_conv_sel_set(
			      &phy->ad9081, AD9081_LINK_0, i,
			      phy->jtx_link_rx[0].link_converter_select[i]);
		if (ret != 0)
			return ret;
	}
	if (phy->jtx_link_rx[0].jesd_param.jesd_duallink > 0) {
		for (i = 0;
		     i < NO_OS_ARRAY_SIZE(phy->jtx_link_rx[1].link_converter_select);
		     i++) {
			ret = adi_ad9081_jesd_tx_conv_sel_set(
				      &phy->ad9081, AD9081_LINK_1, i,
				      phy->jtx_link_rx[1].link_converter_select[i]);
			if (ret != 0)
				return ret;
		}
	}

	ret = adi_ad9081_adc_chip_dcm_ratio_get(&phy->ad9081,
						AD9081_LINK_0, &dcm);
	if (ret != 0 || !dcm)
		return ret;

	if (phy->config_sync_01_swapped &&
	    phy->jrx_link_tx.jesd_param.jesd_jesdv != 2) {
		adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYNCB_CTRL_ADDR,
				      BF_SYNCB_RX_MODE_RC_INFO, 1); /* not paged */
		adi_ad9081_jesd_rx_syncb_driver_powerdown_set(&phy->ad9081, 0);
		adi_ad9081_hal_reg_set(&phy->ad9081,
				       REG_GENERAL_JRX_CTRL_ADDR, 0x80);
		/* Differential mode */
		adi_ad9081_dac_gpio_as_sync1_out_set(&phy->ad9081, 1);
		adi_ad9081_jesd_tx_sync_mode_set(&phy->ad9081,
						 AD9081_LINK_0, 1);
		adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYNCB_CTRL_ADDR,
				      BF_PD_SYNCB_RX_RC_INFO, 0);
	}

	if (phy->jesd_rx_clk) {
		rx_lane_rate_kbps = ad9081_calc_lanerate(&phy->jtx_link_rx[0],
				    phy->adc_frequency_hz,
				    dcm);

		ret = no_os_clk_set_rate(phy->jesd_rx_clk, rx_lane_rate_kbps);
		if (ret < 0) {
			printf("Failed to set lane rate to %llu kHz: %"PRId32"\n",
			       rx_lane_rate_kbps, ret);
		}
	}

	if ((phy->jrx_link_tx.jesd_param.jesd_jesdv == 2) &&
	    (tx_lane_rate_kbps > 16230000UL)) {
		ret = adi_ad9081_jesd_rx_calibrate_204c(&phy->ad9081, 1, 0, 0);
		if (ret < 0)
			return ret;
	}

	if (phy->jrx_link_tx.jesd_param.jesd_jesdv == 2) {
		/* FIXME */
		ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_JRX_TPL_1_ADDR,
					    BF_JRX_TPL_BUF_PROTECT_EN_INFO,
					    0);
		if (ret != 0)
			return ret;
	}

	ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
			(phy->jrx_link_tx.jesd_param.jesd_duallink > 0) ?
			AD9081_LINK_ALL : AD9081_LINK_0, 1);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_jesd_tx_link_enable_set(&phy->ad9081,
			(phy->jtx_link_rx[0].jesd_param.jesd_duallink > 0) ?
			AD9081_LINK_ALL : AD9081_LINK_0, 1);
	if (ret != 0)
		return ret;

	no_os_mdelay(10);

	if (phy->jesd_rx_clk) {
		timeout = 2000;
		while(timeout) {
			ret = no_os_clk_enable(phy->jesd_rx_clk);
			if (ret) {
				no_os_mdelay(100);
				timeout -= 100;
				continue;
			}
			break;
		}

		if (ret || !timeout) {
			printf("Failed to enable JESD204 link: %"PRId32"\n", ret);
			return ret;
		}
	}

	if (phy->jesd_tx_clk &&
	    (phy->jrx_link_tx.jesd_param.jesd_jesdv == 1)) {
		timeout = 2000;
		while(timeout) {
			ret = no_os_clk_enable(phy->jesd_tx_clk);
			if (ret) {
				no_os_mdelay(100);
				timeout -= 100;
				continue;
			}
			break;
		}

		if (ret || !timeout) {
			printf("Failed to enable JESD204 link: %"PRId32"\n", ret);
			return ret;
		}
	}

	/*
	 * 204c doesn't have a SYNC, so the link should come up.
	 * This needs to be revisited once we move this driver to the
	 * new JESD framework ...
	 */

	if (phy->jrx_link_tx.jesd_param.jesd_jesdv == 2 ||
	    phy->jesd_tx_clk) {
		do {	/* temp workaround until API is fixed */
			no_os_mdelay(10);
			stat = ad9081_jesd_rx_link_status_print(phy);
			if (stat <= 0) {
				ret = adi_ad9081_jesd_rx_link_enable_set(
					      &phy->ad9081,
					      (phy->jrx_link_tx.jesd_param.jesd_duallink > 0) ?
					      AD9081_LINK_ALL : AD9081_LINK_0, 0);
				if (ret != 0)
					return ret;

				if (phy->jesd_tx_clk) {
					no_os_clk_disable(phy->jesd_tx_clk);

					no_os_mdelay(100);

					ret = no_os_clk_enable(phy->jesd_tx_clk);
					if (ret < 0) {
						printf("Failed to enable JESD204 link: %"PRId32"\n",
						       ret);
						return ret;
					}
				} else {
					no_os_mdelay(100);
				}

				ret = adi_ad9081_jesd_rx_link_enable_set(
					      &phy->ad9081,
					      (phy->jrx_link_tx.jesd_param.jesd_duallink > 0) ?
					      AD9081_LINK_ALL : AD9081_LINK_0, 1);
				if (ret != 0)
					return ret;

				no_os_mdelay(100);
			}
		} while (stat <= 0 && retry--);
	}

	ad9081_jesd_tx_link_status_print(phy);

	adi_ad9081_dac_irqs_status_get(&phy->ad9081, &status64);
	printf("DAC IRQ status 0x%"PRIx64"\n", status64);

	sample_rate = phy->adc_frequency_hz;
	no_os_do_div(&sample_rate, dcm);

	//no_os_clk_set_rate(phy->clks[RX_SAMPL_CLK], sample_rate); // TODO

	sample_rate = phy->dac_frequency_hz;
	no_os_do_div(&sample_rate, phy->tx_main_interp * phy->tx_chan_interp);

	//no_os_clk_set_rate(phy->clks[TX_SAMPL_CLK], sample_rate); // TODO

	for_each_cddc(i, phy->rx_cddc_select) {
		ret = adi_ad9081_adc_nyquist_zone_set(&phy->ad9081, NO_OS_BIT(i),
						      phy->rx_nyquist_zone[i]);
		if (ret != 0)
			return ret;
	}

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
	no_os_udelay(us);

	return 0;
}

int32_t ad9081_reset_pin_ctrl(void *user_data, uint8_t enable)
{
	struct ad9081_phy *phy = user_data;

	return no_os_gpio_set_value(phy->gpio_reset, enable);
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

	if (phy->ad9081.hal_info.msb == SPI_MSB_FIRST) {
		for (i = 0; i < bytes_number; i++)
			data[i] =  in_data[i];
	} else {
		data[0] = in_data[1];
		data[1] = in_data[0];
		for (i = 2; i < bytes_number; i++)
			data[i] =  in_data[bytes_number - i + 1];
	}

	ret = no_os_spi_write_and_read(phy->spi_desc, data, bytes_number);
	if (ret != 0)
		return -1;

	if (phy->ad9081.hal_info.msb == SPI_MSB_FIRST) {
		for (i = 0; i < bytes_number; i++)
			out_data[i] =  data[i];
	} else {
		for (i = 2; i < bytes_number; i++)
			out_data[i] =  data[bytes_number - i + 1];
	}

	return 0;
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
	for (i = 0; i < NO_OS_ARRAY_SIZE(link->logiclane_mapping); i++)
		link->logiclane_mapping[i] =
			init_param->logical_lane_mapping[i];
	if (jtx) /* JTX - for RX ADC path */
		for (i = 0; i < NO_OS_ARRAY_SIZE(link->link_converter_select); i++)
			link->link_converter_select[i] =
				init_param->link_converter_select[i];
	else /* JRX */
		link->jrx_tpl_phase_adjust = init_param->tpl_phase_adjust;

	return 0;
}

int32_t ad9081_parse_init_param(struct ad9081_phy *phy,
				const struct ad9081_init_param *init_param)
{
	int32_t i;

	phy->sysref_coupling_ac_en = init_param->sysref_coupling_ac_en;
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
	ad9081_parse_jesd_link_init_param(&phy->jrx_link_tx,
					  init_param->jrx_link_tx, false);
	/* RX */
	phy->adc_frequency_hz = init_param->adc_frequency_hz;

	for (i = 0; i < MAX_NUM_MAIN_DATAPATHS; i++)
		phy->rx_nyquist_zone[i] = init_param->nyquist_zone[i];

	/* The 4 ADC Main Datapaths */

	for (i = 0; i < MAX_NUM_MAIN_DATAPATHS; i++) {
		phy->rx_cddc_shift[i] = init_param->rx_main_nco_frequency_shift_hz[i];
		phy->adc_main_decimation[i] = init_param->rx_main_decimation[i];
		phy->rx_cddc_c2r[i] = init_param->rx_main_complex_to_real_enable[i];
		if (init_param->rx_main_enable[i])
			phy->rx_cddc_select |= NO_OS_BIT(i);
	}
	/* The 8 ADC Channelizers */
	for (i = 0; i < MAX_NUM_CHANNELIZER; i++) {
		phy->rx_fddc_shift[i] = init_param->rx_channel_nco_frequency_shift_hz[i];
		phy->adc_chan_decimation[i] = init_param->rx_channel_decimation[i];
		phy->rx_fddc_c2r[i] = init_param->rx_channel_complex_to_real_enable[i];
		if (init_param->rx_channel_enable[i])
			phy->rx_fddc_select |= NO_OS_BIT(i);
	}
	/* RX JESD Link */
	if (init_param->jtx_link_rx[0])
		ad9081_parse_jesd_link_init_param(&phy->jtx_link_rx[0],
						  init_param->jtx_link_rx[0], true);

	if (init_param->jtx_link_rx[1])
		ad9081_parse_jesd_link_init_param(&phy->jtx_link_rx[1],
						  init_param->jtx_link_rx[1], true);

	return 0;
}

/**
 * Initialize the device.
 * @param dev - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9081_init(struct ad9081_phy **dev,
		    const struct ad9081_init_param *init_param)
{
	adi_cms_chip_id_t chip_id;
	struct ad9081_phy *phy;
	uint8_t api_rev[3];
	int32_t ret;

	phy = (struct ad9081_phy *)calloc(1, sizeof(*phy));
	if (!phy)
		return -1;

	ret = no_os_gpio_get(&phy->gpio_reset, init_param->gpio_reset);
	if (ret < 0)
		goto error_1;

	ret = no_os_spi_init(&phy->spi_desc, init_param->spi_init);
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
	phy->ad9081.hal_info.msb = (phy->spi_desc->bit_order ==
				    NO_OS_SPI_BIT_ORDER_MSB_FIRST) ? SPI_MSB_FIRST : SPI_MSB_LAST;
	phy->ad9081.hal_info.addr_inc = SPI_ADDR_INC_AUTO;
	phy->ad9081.hal_info.spi_xfer = ad9081_spi_xfer;
	phy->ad9081.hal_info.log_write = ad9081_log_write;

	ret = no_os_gpio_direction_output(phy->gpio_reset, 1);
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
		ret = -1;
		goto error_3;
	}

	ret = ad9081_setup(phy);
	if (ret < 0) {
		printf("%s: ad9081_setup failed (%"PRId32")\n", __func__, ret);
		goto error_3;
	}

	adi_ad9081_device_api_revision_get(&phy->ad9081, &api_rev[0],
					   &api_rev[1], &api_rev[2]);

	printf("AD9081 Rev. %u Grade %u (API %u.%u.%u) probed\n",
	       chip_id.dev_revision, chip_id.prod_grade,
	       api_rev[0], api_rev[1], api_rev[2]);

	*dev = phy;

	return 0;

error_3:
	no_os_spi_remove(phy->spi_desc);
error_2:
	no_os_gpio_remove(phy->gpio_reset);
error_1:
	free(phy);

	return ret;
}

/**
 * Remove the device - release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9081_remove(struct ad9081_phy *dev)
{
	int32_t ret;

	ret = no_os_gpio_remove(dev->gpio_reset);
	ret += no_os_spi_remove(dev->spi_desc);
	free(dev);

	return ret;
}
