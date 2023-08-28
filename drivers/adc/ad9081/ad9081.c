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
#include <string.h>
#include "no_os_error.h"
#include "adi_cms_api_common.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "adi_ad9081_hal.h"
#include "no_os_print_log.h"
#include "ad9081.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* ffh: 2 - gpio6, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n */

#define AD9081_PERI_SEL_GPIO6		2
#define AD9081_PERI_SEL_GPIO7		3
#define AD9081_PERI_SEL_GPIO8		4
#define AD9081_PERI_SEL_GPIO9		5
#define AD9081_PERI_SEL_GPIO10		6
#define AD9081_PERI_SEL_SYNCINB1_P	7
#define AD9081_PERI_SEL_SYNCINB1_N	8

#define CHIPID_AD9081	0x9081
#define CHIPID_AD9082	0x9082
#define CHIPID_MASK 0xFFFF

#define for_each_cddc(bit, mask) \
	for ((bit) = 0; (bit) < MAX_NUM_MAIN_DATAPATHS; (bit)++) \
		if ((mask) & NO_OS_BIT(bit))


#define for_each_fddc(bit, mask) \
	for ((bit) = 0; (bit) < MAX_NUM_CHANNELIZER; (bit)++) \
		if ((mask) & NO_OS_BIT(bit))

struct ad9081_jesd204_priv {
	struct ad9081_phy *phy;
};

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

static int adi_ad9081_adc_nco_sync(adi_ad9081_device_t *device,
				   uint8_t trigger_src,
				   uint8_t extra_lmfc_num)
{
	int err;

	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_MAIN_AUTO_CLK_GATING_ADDR,
				    0x00000400, 7);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_adc_ddc_coarse_sync_enable_set(device,
			AD9081_ADC_CDDC_ALL, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_coarse_sync_next_set(device,
			AD9081_ADC_CDDC_ALL, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_coarse_trig_nco_reset_enable_set(
		      device, AD9081_ADC_CDDC_ALL, 0);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_adc_ddc_fine_sync_enable_set(device,
			AD9081_ADC_FDDC_ALL, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_fine_sync_next_set(device, AD9081_ADC_FDDC_ALL,
			1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_fine_trig_nco_reset_enable_set(
		      device, AD9081_ADC_FDDC_ALL, 0);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_device_nco_sync_mode_set(device, 0);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_device_nco_sync_sysref_mode_set(device, trigger_src);
	AD9081_ERROR_RETURN(err);

	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_device_nco_sync_extra_lmfc_num_set(device,
			extra_lmfc_num);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_adc_ddc_coarse_sync_next_set(device,
			AD9081_ADC_CDDC_ALL, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_fine_sync_next_set(device, AD9081_ADC_FDDC_ALL,
			0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_coarse_sync_next_set(device,
			AD9081_ADC_CDDC_ALL, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_fine_sync_next_set(device, AD9081_ADC_FDDC_ALL,
			1);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_device_nco_sync_reset_via_sysref_set(device, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_device_nco_sync_reset_via_sysref_set(device, 1);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int adi_ad9081_device_gpio_set_highz(adi_ad9081_device_t *device,
				     uint8_t gpio_index)
{
	int err;

	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(gpio_index > 5);

	if ((gpio_index & 1) == 0) {
		err = adi_ad9081_hal_bf_set(device,
					    REG_GPIO_CFG0_ADDR + (gpio_index >> 1), 0x0400, 0);
		AD9081_ERROR_RETURN(err);
	} else {
		err = adi_ad9081_hal_bf_set(device,
					    REG_GPIO_CFG0_ADDR + (gpio_index >> 1), 0x0404, 0);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

static int ad9081_nco_sync(struct ad9081_phy *phy, bool master)
{
	int ret;

	ret = adi_ad9081_device_nco_sync_pre(&phy->ad9081);
	if (ret != 0)
		return ret;

	/* trigger_src  0: sysref, 1: lmfc rising edge, 2: lmfc falling edge */

	if (phy->nco_sync_direct_sysref_mode_en)
		return adi_ad9081_adc_nco_sync(&phy->ad9081,
					       0, phy->nco_sync_ms_extra_lmfc_num);
	else
		return adi_ad9081_adc_nco_master_slave_sync(&phy->ad9081,
				master,
				1, /* trigger_src */
				phy->sync_ms_gpio_num, /* gpio_index */
				phy->nco_sync_ms_extra_lmfc_num);
}

static int ad9081_jesd_tx_link_dig_reset(adi_ad9081_device_t *device,
		uint8_t reset)
{
	int err;

	AD9081_NULL_POINTER_RETURN(device);
	AD9081_INVALID_PARAM_RETURN(reset > 1);

	err = adi_ad9081_hal_bf_set(device, REG_FORCE_LINK_RESET_REG_ADDR,
				    BF_FORCE_LINK_DIGITAL_RESET_INFO,
				    reset); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

static bool ad9081_link_is_dual(struct ad9081_jesd_link *link)
{
	return !!link[0].jesd_param.jesd_duallink;
}

static adi_ad9081_jesd_link_select_e
ad9081_link_sel(struct ad9081_jesd_link *link)
{
	return ad9081_link_is_dual(link) ? AD9081_LINK_ALL : AD9081_LINK_0;
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

static int32_t ad9081_jesd_rx_link_status_print(struct ad9081_phy *phy,
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
				no_os_mdelay(20);
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
				no_os_mdelay(20);
		}
	} while (ret && retry--);

	return ret;
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
					stat & NO_OS_BIT(5) ? "locked" : "unlocked",
					stat & NO_OS_BIT(6) ? "established" : "lost",
					stat & NO_OS_BIT(7) ? "invalid" : "valid");
			else
				no_os_mdelay(20);
		} else {
			if ((stat & 0xFF) == 0x7D)
				ret = 0;
			else
				ret = -EIO;

			if (ret == 0 || retry == 0)
				pr_info("JESD RX (JTX) Link%d in %s, SYNC %s, PLL %s, PHASE %s, MODE %s\n",
					lnk->link_id, ad9081_jtx_qbf_states[stat & 0xF],
					stat & NO_OS_BIT(4) ? "deasserted" : "asserted",
					stat & NO_OS_BIT(5) ? "locked" : "unlocked",
					stat & NO_OS_BIT(6) ? "established" : "lost",
					stat & NO_OS_BIT(7) ? "invalid" : "valid");
			else
				no_os_mdelay(20);
		}
	} while (ret && retry--);

	return ret;
}

static int ad9081_setup_tx(struct ad9081_phy *phy)
{
	uint64_t sample_rate, status64;
	int ret, i;

	if (phy->tx_disable) {
		/* Disable DAC core clock domain and reduce power consumption */
		adi_ad9081_dac_tx_enable_set(&phy->ad9081, AD9081_DAC_ALL, 0);
		adi_ad9081_hal_reg_set(&phy->ad9081,
				       REG_ENABLE_TIMING_CTRL_DAC0_ADDR, 0x0);
		adi_ad9081_hal_reg_set(&phy->ad9081,
				       REG_ENABLE_TIMING_CTRL_DAC1_ADDR, 0x0);

		return 0;
	}

	memcpy(phy->ad9081.serdes_info.des_settings.lane_mapping[0],
	       phy->jrx_link_tx[0].logiclane_mapping,
	       sizeof(phy->jrx_link_tx[0].logiclane_mapping));

	memcpy(phy->ad9081.serdes_info.des_settings.lane_mapping[1],
	       phy->jrx_link_tx[1].logiclane_mapping,
	       sizeof(phy->jrx_link_tx[1].logiclane_mapping));

	/* start txfe tx */
	ret = adi_ad9081_device_startup_tx(
		      &phy->ad9081, phy->tx_main_interp, phy->tx_chan_interp,
		      phy->tx_chan_interp == 1 ? phy->tx_dac_chan_xbar_1x_non1x :
		      phy->tx_dac_chan_xbar,
		      phy->tx_main_shift, phy->tx_chan_shift,
		      &phy->jrx_link_tx[0].jesd_param);

	if (ret != 0)
		return ret;

	/* setup txfe dac channel gain */
	ret = adi_ad9081_dac_duc_nco_gains_set(&phy->ad9081,
					       phy->dac_cache.chan_gain);
	if (ret != 0)
		return ret;

	adi_ad9081_jesd_rx_lmfc_delay_set(&phy->ad9081, AD9081_LINK_0,
					  phy->jrx_link_tx[0].jrx_tpl_phase_adjust);

	adi_ad9081_jesd_rx_lmfc_delay_set(&phy->ad9081, AD9081_LINK_1,
					  phy->jrx_link_tx[1].jrx_tpl_phase_adjust);

	if (phy->jrx_link_tx[0].jesd_param.jesd_jesdv == 2  &&
	    phy->ad9081.dev_info.dev_rev < 3) {
		ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_JRX_TPL_1_ADDR,
					    BF_JRX_TPL_BUF_PROTECT_EN_INFO,
					    0);
		if (ret != 0)
			return ret;
	}

	adi_ad9081_dac_irqs_status_get(&phy->ad9081, &status64);
	pr_debug("DAC IRQ status 0x%llX\n", status64);

	sample_rate = NO_OS_DIV_ROUND_CLOSEST_ULL(phy->dac_frequency_hz,
			phy->tx_main_interp * phy->tx_chan_interp);

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->tx_dac_fsc); i++) {
		if (phy->tx_dac_fsc[i]) {
			ret = adi_ad9081_dac_fsc_set(&phy->ad9081, NO_OS_BIT(i), phy->tx_dac_fsc[i], 1);
			if (ret != 0)
				return ret;
		}
	}

	if (phy->tx_ffh_hopf_via_gpio_en) {
		adi_ad9081_jesd_rx_syncb_mode_set(&phy->ad9081, 0);
		adi_ad9081_dac_duc_main_nco_hopf_gpio_as_hop_en_set(&phy->ad9081, 1);
		adi_ad9081_jesd_rx_syncb_driver_powerdown_set(&phy->ad9081, 0);
	}

	return 0;
}

static int ad9081_setup_rx(struct ad9081_phy *phy)
{
	uint64_t sample_rate;
	adi_cms_jesd_param_t jesd_param[2];
	adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2];
	uint8_t dcm;
	int ret, i;

	if (phy->rx_disable) {
		adi_ad9081_adc_clk_enable_set(&phy->ad9081, 0);

		return 0;
	}

	memcpy(phy->ad9081.serdes_info.ser_settings.lane_mapping[0],
	       phy->jtx_link_rx[0].logiclane_mapping,
	       sizeof(phy->jtx_link_rx[0].logiclane_mapping));

	if (ad9081_link_is_dual(phy->jtx_link_rx))
		memcpy(phy->ad9081.serdes_info.ser_settings.lane_mapping[1],
		       phy->jtx_link_rx[1].logiclane_mapping,
		       sizeof(phy->jtx_link_rx[1].logiclane_mapping));

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

//	if (conv->id == CHIPID_AD9081 || conv->id == CHIPID_AD9988) {
	{
		/* Fix: 4x4 Crossbar Mux0 Mappings for AD9081 */
		ret  = adi_ad9081_adc_pfir_din_select_set(&phy->ad9081,
				AD9081_ADC_PFIR_ADC_PAIR0, 0, 1);
		if (ret != 0)
			return ret;

		ret  = adi_ad9081_adc_pfir_din_select_set(&phy->ad9081,
				AD9081_ADC_PFIR_ADC_PAIR1, 3, 0);
		if (ret != 0)
			return ret;
	}

	for_each_cddc(i, phy->rx_cddc_select) {
		ret = adi_ad9081_adc_ddc_coarse_gain_set(
			      &phy->ad9081, NO_OS_BIT(i), phy->rx_cddc_gain_6db_en[i]);
		if (ret != 0)
			return ret;

		ret = adi_ad9081_adc_nyquist_zone_set(&phy->ad9081, NO_OS_BIT(i),
						      phy->rx_nyquist_zone[i]);
		if (ret != 0)
			return ret;

		ret = adi_ad9081_adc_ddc_coarse_nco_channel_select_via_gpio_set(&phy->ad9081,
				NO_OS_BIT(i), phy->rx_cddc_nco_channel_select_mode[i]);
		if (ret != 0)
			return ret;

	}

	for_each_fddc(i, phy->rx_fddc_select) {
		ret = adi_ad9081_adc_ddc_fine_nco_mode_set(
			      &phy->ad9081, NO_OS_BIT(i), phy->rx_fddc_mxr_if[i]);
		if (ret != 0)
			return ret;

		ret = adi_ad9081_adc_ddc_fine_gain_set(
			      &phy->ad9081, NO_OS_BIT(i), phy->rx_fddc_gain_6db_en[i]);
		if (ret != 0)
			return ret;
	}

	/* setup txfe jtx converter mapping */
	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->jtx_link_rx[0].link_converter_select);
	     i++) {
		ret = adi_ad9081_jesd_tx_conv_sel_set(&phy->ad9081,
						      AD9081_LINK_0, i, phy->jtx_link_rx[0].link_converter_select[i]);
		if (ret != 0)
			return ret;
	}
	if (ad9081_link_is_dual(phy->jtx_link_rx)) {
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
		return -EINVAL;

	phy->adc_dcm[0] = dcm;

	if (ad9081_link_is_dual(phy->jtx_link_rx)) {
		ret = adi_ad9081_adc_chip_dcm_ratio_get(&phy->ad9081,
							AD9081_LINK_1, &dcm);
		if (ret != 0 || !dcm)
			return -EINVAL;

		phy->adc_dcm[1] = dcm;
	}

	sample_rate = NO_OS_DIV_ROUND_CLOSEST_ULL(phy->adc_frequency_hz,
			phy->adc_dcm[0]);

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->rx_ffh_gpio_mux_sel); i++)
		if (phy->rx_ffh_gpio_mux_sel[i] == AD9081_PERI_SEL_SYNCINB1_N ||
		    phy->rx_ffh_gpio_mux_sel[i] == AD9081_PERI_SEL_SYNCINB1_P)
			adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYNCB_CTRL_ADDR,
					      BF_PD_SYNCB_RX_RC_INFO, 0);

	return adi_ad9081_adc_ddc_ffh_sel_to_gpio_mapping_set(&phy->ad9081,
			phy->rx_ffh_gpio_mux_sel);
}

static int ad9081_setup(struct ad9081_phy *phy)
{
	uint64_t dev_frequency_hz;
	uint8_t txfe_pll_stat;
	int ret;

	no_os_clk_recalc_rate(phy->dev_clk->clk_desc, &dev_frequency_hz);

	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYNC_LMFC_DELAY_ADDR,
				    BF_SYNC_LMFC_DELAY_SET_INFO,
				    BF_SYNC_LMFC_DELAY_SET(phy->lmfc_delay));
	if (ret != 0)
		return ret;

	/* Configure SYSREF */
	ret = adi_ad9081_sync_sysref_input_config_set(&phy->ad9081,
			phy->sysref_coupling_ac_en ? COUPLING_AC : COUPLING_DC,
			phy->sysref_cmos_input_en ? SIGNAL_CMOS : SIGNAL_LVDS,
			phy->sysref_cmos_single_end_term_pos,
			phy->sysref_cmos_single_end_term_neg);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYNC_DEBUG0_ADDR,
				    BF_AVRG_FLOW_EN_INFO, 1);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYSREF_AVERAGE_ADDR,
				    BF_SYSREF_AVERAGE_INFO,
				    BF_SYSREF_AVERAGE(phy->sysref_average_cnt_exp));
	if (ret != 0)
		return ret;

	ret = adi_ad9081_device_clk_config_set(&phy->ad9081,
					       phy->dac_frequency_hz, phy->adc_frequency_hz, dev_frequency_hz);
	if (ret != 0)
		return ret;

	if (dev_frequency_hz != phy->dac_frequency_hz) {
		ret = adi_ad9081_device_clk_pll_lock_status_get(&phy->ad9081,
				&txfe_pll_stat);
		if (ret != 0)
			return ret;

		if (txfe_pll_stat != 3) {
			pr_err("CLK PLL Failed to Lock (Status: %d)",
			       txfe_pll_stat);
			return -EFAULT;
		}
	}

	ret = ad9081_setup_tx(phy);
	if (ret)
		return ret;

	ret = ad9081_setup_rx(phy);
	if (ret)
		return ret;

	if (phy->config_sync_0a_cmos_en) {
		adi_ad9081_jesd_rx_synca_mode_set(&phy->ad9081, 0);
		adi_ad9081_hal_reg_set(&phy->ad9081, REG_SYNCA_CTRL_ADDR, 0x0);
	}

	if (phy->config_sync_01_swapped) {
		adi_ad9081_jesd_rx_syncb_driver_powerdown_set(&phy->ad9081, 0);
		adi_ad9081_hal_reg_set(&phy->ad9081,
				       REG_GENERAL_JRX_CTRL_ADDR, 0x80);
		/* Differential mode */
		adi_ad9081_dac_gpio_as_sync1_out_set(&phy->ad9081, 1);
		adi_ad9081_jesd_tx_sync_mode_set(&phy->ad9081,
						 AD9081_LINK_0, 1);

		adi_ad9081_hal_2bf_set(&phy->ad9081, REG_SYNCA_CTRL_ADDR,
				       BF_PD_SYNCB_RX_RC_INFO, 1,
				       BF_SYNCB_RX_MODE_RC_INFO, 1);

		adi_ad9081_hal_2bf_set(&phy->ad9081, REG_SYNCB_CTRL_ADDR,
				       BF_PD_SYNCB_RX_RC_INFO, 0,
				       BF_SYNCB_RX_MODE_RC_INFO, 1);
	}

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

	link->jesd204_link.is_transmit = !jtx;
	link->jesd204_link.scrambling = link->jesd_param.jesd_scr;
	link->jesd204_link.device_id = link->jesd_param.jesd_did;
	link->jesd204_link.octets_per_frame = link->jesd_param.jesd_f;
	link->jesd204_link.frames_per_multiframe = link->jesd_param.jesd_k;
	link->jesd204_link.samples_per_conv_frame = link->jesd_param.jesd_s;
	link->jesd204_link.high_density = link->jesd_param.jesd_hd;
	link->jesd204_link.converter_resolution = link->jesd_param.jesd_n;
	link->jesd204_link.bits_per_sample = link->jesd_param.jesd_np;
	link->jesd204_link.num_converters = link->jesd_param.jesd_m;
	link->jesd204_link.ctrl_bits_per_sample = link->jesd_param.jesd_cs;
	link->jesd204_link.num_lanes = link->jesd_param.jesd_l;
	link->jesd204_link.jesd_version = link->jesd_param.jesd_jesdv;
	link->jesd204_link.subclass = link->jesd_param.jesd_subclass;

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

	phy->sync_ms_gpio_num = init_param->master_slave_sync_gpio_num;
	phy->sysref_coupling_ac_en = init_param->sysref_coupling_ac_en;
	phy->sysref_cmos_input_en = init_param->sysref_cmos_input_enable;
	phy->sysref_cmos_single_end_term_pos = init_param->sysref_cmos_input_enable;
	phy->sysref_cmos_single_end_term_neg = init_param->sysref_cmos_input_enable;
	phy->multidevice_instance_count = init_param->multidevice_instance_count;
	phy->config_sync_01_swapped = init_param->jesd_sync_pins_01_swap_enable;
	phy->config_sync_0a_cmos_en = init_param->config_sync_0a_cmos_enable;
	phy->lmfc_delay = init_param->lmfc_delay_dac_clk_cycles;
	phy->nco_sync_ms_extra_lmfc_num = init_param->nco_sync_ms_extra_lmfc_num;
	phy->nco_sync_direct_sysref_mode_en =
		init_param->nco_sync_direct_sysref_mode_enable;
	phy->sysref_average_cnt_exp = init_param->sysref_average_cnt_exp;
	phy->sysref_continuous_dis = init_param->continuous_sysref_mode_disable;
	phy->tx_disable = init_param->tx_disable;
	phy->rx_disable = init_param->rx_disable;

	/* TX */
	phy->dac_frequency_hz = init_param->dac_frequency_hz;

	/* The 4 DAC Main Datapaths */
	phy->tx_main_interp = init_param->tx_main_interpolation;
	for (i = 0; i < MAX_NUM_MAIN_DATAPATHS; i++) {
		phy->tx_main_shift[i] = init_param->tx_main_nco_frequency_shift_hz[i];
		phy->tx_dac_chan_xbar[i] = init_param->tx_dac_channel_crossbar_select[i];
		phy->tx_dac_chan_xbar_1x_non1x[i] =
			init_param->tx_maindp_dac_1x_non1x_crossbar_select[i];
		phy->tx_dac_fsc[i] = init_param->tx_full_scale_current_ua[i];
	}
	/* The 8 DAC Channelizers */
	phy->tx_chan_interp = init_param->tx_channel_interpolation;
	for (i = 0; i < MAX_NUM_CHANNELIZER; i++) {
		phy->tx_chan_shift[i] = init_param->tx_channel_nco_frequency_shift_hz[i];
		phy->dac_cache.chan_gain[i] = init_param->tx_channel_gain[i];
	}
	/* TX JESD Link */
	if (init_param->jrx_link_tx[0])
		ad9081_parse_jesd_link_init_param(&phy->jrx_link_tx[0],
						  init_param->jrx_link_tx[0], false);

	if (init_param->jrx_link_tx[1])
		ad9081_parse_jesd_link_init_param(&phy->jrx_link_tx[1],
						  init_param->jrx_link_tx[1], false);
	/* RX */
	phy->adc_frequency_hz = init_param->adc_frequency_hz;

	for (i = 0; i < MAX_NUM_MAIN_DATAPATHS; i++)
		phy->rx_nyquist_zone[i] = init_param->nyquist_zone[i];

	/* The 4 ADC Main Datapaths */

	for (i = 0; i < MAX_NUM_MAIN_DATAPATHS; i++) {
		phy->rx_cddc_shift[i] = init_param->rx_main_nco_frequency_shift_hz[i];
		phy->adc_main_decimation[i] = init_param->rx_main_decimation[i];
		phy->rx_cddc_c2r[i] = init_param->rx_main_complex_to_real_enable[i];
		phy->rx_cddc_gain_6db_en[i] = init_param->rx_main_digital_gain_6db_enable[i];
		if (init_param->rx_main_enable[i])
			phy->rx_cddc_select |= NO_OS_BIT(i);
		phy->rx_cddc_nco_channel_select_mode[i] =
			init_param->rx_cddc_nco_channel_select_mode[i];
	}
	/* The 8 ADC Channelizers */
	for (i = 0; i < MAX_NUM_CHANNELIZER; i++) {
		phy->rx_fddc_shift[i] = init_param->rx_channel_nco_frequency_shift_hz[i];
		phy->adc_chan_decimation[i] = init_param->rx_channel_decimation[i];
		phy->rx_fddc_c2r[i] = init_param->rx_channel_complex_to_real_enable[i];
		phy->rx_fddc_mxr_if[i] = init_param->rx_channel_nco_mixer_mode[i];
		phy->rx_fddc_gain_6db_en[i] = init_param->rx_channel_digital_gain_6db_enable[i];
		if (init_param->rx_channel_enable[i])
			phy->rx_fddc_select |= NO_OS_BIT(i);
	}

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->rx_ffh_gpio_mux_sel); i++) {
		phy->rx_ffh_gpio_mux_sel[i] = init_param->rx_ffh_gpio_mux_selection[i];
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

static int ad9081_jesd204_clks_enable(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct ad9081_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9081_phy *phy = priv->phy;
	uint8_t jesd_pll_status;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));


	if (reason == JESD204_STATE_OP_REASON_INIT) {
		ret = adi_ad9081_jesd_pll_lock_status_get(&phy->ad9081,
				&jesd_pll_status);
		if (ret != 0)
			return ret;

		if (!jesd_pll_status) {
			pr_err("JESD PLL Not Locked!\n");
			return -EFAULT;
		}
	}

	if (lnk->is_transmit && (reason == JESD204_STATE_OP_REASON_INIT) &&
	    (lnk->jesd_version == JESD204_VERSION_C)) {

		if ((phy->jrx_link_tx[0].lane_rate_kbps >
		     (AD9081_JESDRX_204C_CAL_THRESH / 1000)) &&
		    phy->jrx_link_tx[0].lane_cal_rate_kbps !=
		    phy->jrx_link_tx[0].lane_rate_kbps) {

			ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
					ad9081_link_sel(phy->jrx_link_tx), 1);
			if (ret != 0)
				return ret;

			pr_info("running jesd_rx_calibrate_204c, LR %lu kbps\n",
				phy->jrx_link_tx[0].lane_rate_kbps);

			ret = adi_ad9081_jesd_rx_calibrate_204c(&phy->ad9081, 1, 0, 1);
			if (ret < 0)
				return ret;

			ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
					ad9081_link_sel(phy->jrx_link_tx), 0);
			if (ret != 0)
				return ret;
		}

		phy->jrx_link_tx[0].lane_cal_rate_kbps = phy->jrx_link_tx[0].lane_rate_kbps;
	}

	if (!lnk->is_transmit) {
		/* txfe RX (JTX) link digital reset */
		ret = ad9081_jesd_tx_link_dig_reset(&phy->ad9081,
						    reason != JESD204_STATE_OP_REASON_INIT);
		if (ret != 0)
			return ret;

		no_os_mdelay(4);
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_link_enable(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct ad9081_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9081_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__, lnk->link_id,
		 jesd204_state_op_reason_str(reason));

	if (lnk->is_transmit) {
		/* txfe TX (JRX) link */
		ret = adi_ad9081_jesd_rx_link_enable_set(&phy->ad9081,
				ad9081_link_sel(phy->jrx_link_tx),
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
	struct ad9081_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9081_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT) {
//		if (lnk->is_transmit && phy->jrx_link_watchdog_en)
//			cancel_delayed_work(&phy->dwork);

		phy->is_initialized = false;

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

//	if (lnk->is_transmit && phy->jrx_link_watchdog_en)
//		schedule_delayed_work(&phy->dwork, msecs_to_jiffies(1000));

	phy->is_initialized = true;

	/* Need to redo this since GPIOx might have been clobbered by master/slave sync */
	adi_ad9081_dac_duc_main_nco_hopf_gpio_as_hop_en_set(&phy->ad9081,
			phy->tx_ffh_hopf_via_gpio_en);

	if (phy->ms_sync_en_gpio)
		no_os_gpio_set_value(phy->ms_sync_en_gpio, 0);

	return JESD204_STATE_CHANGE_DONE;
}

int ad9081_jesd204_uninit(struct jesd204_dev *jdev,
			  enum jesd204_state_op_reason reason)
{
	if (reason != JESD204_STATE_OP_REASON_UNINIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_link_init(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason,
				    struct jesd204_link *lnk)
{
	struct ad9081_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9081_phy *phy = priv->phy;
	struct ad9081_jesd_link *link;
	int ret;

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__, lnk->link_id,
		 jesd204_state_op_reason_str(reason));

	switch (lnk->link_id) {
	case DEFRAMER_LINK0_TX:
	case DEFRAMER_LINK1_TX:
		if (phy->tx_disable)
			return -ENODEV;
		link = &phy->jrx_link_tx[0];
		lnk->sample_rate = phy->dac_frequency_hz;
		lnk->sample_rate_div = phy->tx_main_interp * phy->tx_chan_interp;
		break;
	case FRAMER_LINK0_RX:
	case FRAMER_LINK1_RX:
		if (phy->rx_disable)
			return -ENODEV;
		link = &phy->jtx_link_rx[lnk->link_id - FRAMER_LINK0_RX];
		lnk->sample_rate = phy->adc_frequency_hz;
		lnk->sample_rate_div = phy->adc_dcm[lnk->link_id - FRAMER_LINK0_RX];
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

	if (phy->sysref_continuous_dis) {
		lnk->sysref.mode = JESD204_SYSREF_ONESHOT;
		phy->ad9081.clk_info.sysref_mode = SYSREF_ONESHOT;
	} else {
		lnk->sysref.mode = JESD204_SYSREF_CONTINUOUS;
		phy->ad9081.clk_info.sysref_mode = SYSREF_CONT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_setup_stage1(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason)
{
	struct ad9081_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9081_phy *phy = priv->phy;
	adi_cms_jesd_subclass_e subclass = JESD_SUBCLASS_0;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT) {
		if (phy->ms_sync_en_gpio)
			no_os_gpio_set_value(phy->ms_sync_en_gpio, 0);

		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	ret = ad9081_jesd_tx_link_dig_reset(&phy->ad9081, 0);
	if (ret != 0)
		return ret;

	no_os_mdelay(4);

	/* JESD OneShot Sync */
	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYNC_DEBUG0_ADDR,
				    BF_AVRG_FLOW_EN_INFO, 1);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_hal_bf_set(&phy->ad9081, REG_SYSREF_AVERAGE_ADDR,
				    BF_SYSREF_AVERAGE_INFO,
				    BF_SYSREF_AVERAGE(phy->sysref_average_cnt_exp));
	if (ret != 0)
		return ret;

	if (phy->jrx_link_tx[0].jesd_param.jesd_subclass ||
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

	if (phy->ms_sync_en_gpio)
		no_os_gpio_set_value(phy->ms_sync_en_gpio, 1);

	if (jesd204_dev_is_top(jdev)) {
		/* We need to make sure the master-slave master GPIO is enabled before we move on */
		ret = adi_ad9081_device_nco_sync_gpio_set(&phy->ad9081, phy->sync_ms_gpio_num,
				1);
		if (ret != 0)
			return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}


static int ad9081_jesd204_setup_stage2(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason)
{
	struct ad9081_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9081_phy *phy = priv->phy;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	/* NCO Sync */

	ret = ad9081_nco_sync(phy, jesd204_dev_is_top(jdev));
	if (ret != 0)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9081_jesd204_setup_stage3(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason)
{
	struct ad9081_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9081_phy *phy = priv->phy;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	ret = adi_ad9081_device_nco_sync_post(&phy->ad9081);
	if (ret != 0)
		return ret;

	ret = adi_ad9081_device_gpio_set_highz(&phy->ad9081, phy->sync_ms_gpio_num);
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
		[JESD204_OP_OPT_SETUP_STAGE1] = {
			.per_device = ad9081_jesd204_setup_stage1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_SETUP_STAGE2] = {
			.per_device = ad9081_jesd204_setup_stage2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
			.post_state_sysref = true,
		},
		[JESD204_OP_OPT_SETUP_STAGE3] = {
			.per_device = ad9081_jesd204_setup_stage3,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = ad9081_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = ad9081_jesd204_link_enable,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = ad9081_jesd204_link_running,
		},
	},

	.max_num_links = 4,
	.num_retries = 3,
	.sizeof_priv = sizeof(struct ad9081_jesd204_priv),
};

//static int ad9081_sysref_ctrl(void *clk_src)
//{
//	struct ad9081_phy *phy = clk_src;
//
//	if (phy->jdev)
//		return jesd204_sysref_async_force(phy->jdev);
//
//	return 0;
//}

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
	struct ad9081_jesd204_priv *priv;
	adi_cms_chip_id_t chip_id;
	struct ad9081_phy *phy;
	uint8_t api_rev[3];
	int32_t ret;

	phy = (struct ad9081_phy *)no_os_calloc(1, sizeof(*phy));
	if (!phy)
		return -1;

	ret = no_os_gpio_get(&phy->gpio_reset, init_param->gpio_reset);
	if (ret < 0)
		goto error_1;

	ret = no_os_spi_init(&phy->spi_desc, init_param->spi_init);
	if (ret < 0)
		goto error_2;

	ret = no_os_gpio_get_optional(&phy->ms_sync_en_gpio,
				      init_param->ms_sync_enable);
	if (ret < 0)
		goto error_1;
	if (phy->ms_sync_en_gpio)
		no_os_gpio_set_value(phy->ms_sync_en_gpio, 0);

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

	phy->ad9081.serdes_info = (adi_ad9081_serdes_settings_t) {
		.ser_settings = { /* txfe jtx */
			.lane_settings = {
				{
					.swing_setting = AD9081_SER_SWING_850,
					.pre_emp_setting = AD9081_SER_PRE_EMP_0DB,
					.post_emp_setting = AD9081_SER_POST_EMP_0DB
				}, {
					.swing_setting = AD9081_SER_SWING_850,
					.pre_emp_setting = AD9081_SER_PRE_EMP_0DB,
					.post_emp_setting = AD9081_SER_POST_EMP_0DB
				}, {
					.swing_setting = AD9081_SER_SWING_850,
					.pre_emp_setting = AD9081_SER_PRE_EMP_0DB,
					.post_emp_setting = AD9081_SER_POST_EMP_0DB
				}, {
					.swing_setting = AD9081_SER_SWING_850,
					.pre_emp_setting = AD9081_SER_PRE_EMP_0DB,
					.post_emp_setting = AD9081_SER_POST_EMP_0DB
				}, {
					.swing_setting = AD9081_SER_SWING_850,
					.pre_emp_setting = AD9081_SER_PRE_EMP_0DB,
					.post_emp_setting = AD9081_SER_POST_EMP_0DB
				}, {
					.swing_setting = AD9081_SER_SWING_850,
					.pre_emp_setting = AD9081_SER_PRE_EMP_0DB,
					.post_emp_setting = AD9081_SER_POST_EMP_0DB
				}, {
					.swing_setting = AD9081_SER_SWING_850,
					.pre_emp_setting = AD9081_SER_PRE_EMP_0DB,
					.post_emp_setting = AD9081_SER_POST_EMP_0DB
				}, {
					.swing_setting = AD9081_SER_SWING_850,
					.pre_emp_setting = AD9081_SER_PRE_EMP_0DB,
					.post_emp_setting = AD9081_SER_POST_EMP_0DB
				},
			},
			.invert_mask = 0x00,
			.lane_mapping = {
				{ 0, 1, 2, 3, 4, 5, 6, 7 },
				{ 7, 7, 7, 7, 7, 7, 7, 7 }
			}, /* link0, link1 */
		},
		.des_settings = { /* txfe jrx */
			.boost_mask = 0xff,
			.invert_mask = 0x00,
			.ctle_filter = { 2, 2, 2, 2, 2, 2, 2, 2 },
			.lane_mapping =  {
				{ 0, 1, 2, 3, 4, 5, 6, 7 },
				{ 0, 1, 2, 3, 4, 5, 6, 7 }
			}, /* link0, link1 */
		}
	};

//	phy->ad9081.clk_info.sysref_ctrl = ad9081_sysref_ctrl;
//	phy->ad9081.clk_info.sysref_clk = phy;

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

	if (((chip_id.prod_id & CHIPID_MASK) != CHIPID_AD9081) &&
	    ((chip_id.prod_id & CHIPID_MASK) != CHIPID_AD9082)) {
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

	ret = jesd204_dev_register(&phy->jdev, &jesd204_ad9081_init);
	if (ret < 0)
		goto error_3;
	priv = jesd204_dev_priv(phy->jdev);
	priv->phy = phy;


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
	no_os_free(phy);

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
	no_os_free(dev);

	return ret;
}
