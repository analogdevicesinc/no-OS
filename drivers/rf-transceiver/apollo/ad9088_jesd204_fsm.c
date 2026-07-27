// SPDX-License-Identifier: GPL-2.0
/*
 * AD9088 JESD204 FSM support
 *
 * Copyright 2026 Analog Devices Inc.
 */

#include "ad9088.h"
#include "no_os_delay.h"
#include "no_os_util.h"

static int ad9088_jesd204_link_init(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason,
				    struct jesd204_link *lnk)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;
	adi_apollo_jesd_tx_cfg_t *jtx;
	adi_apollo_jesd_rx_cfg_t *jrx;
	uint8_t sideIdx, linkIdx;
	unsigned long lane_rate_kbps;
	int ret;

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	switch (lnk->link_id) {
	case DEFRAMER_LINK_A0_TX:
	case DEFRAMER_LINK_A1_TX:
	case DEFRAMER_LINK_B0_TX:
	case DEFRAMER_LINK_B1_TX:
		sideIdx = (lnk->link_id - DEFRAMER_LINK_A0_TX) / 2;
		linkIdx = (lnk->link_id - DEFRAMER_LINK_A0_TX) % 2;

		jrx = &phy->profile.jrx[sideIdx];

		lnk->is_transmit = 1;
		lnk->num_lanes =
			jrx->rx_link_cfg[linkIdx].l_minus1 + 1;
		lnk->num_converters =
			jrx->rx_link_cfg[linkIdx].m_minus1 + 1;
		lnk->octets_per_frame =
			jrx->rx_link_cfg[linkIdx].f_minus1 + 1;
		lnk->frames_per_multiframe =
			jrx->rx_link_cfg[linkIdx].k_minus1 + 1;
		lnk->num_of_multiblocks_in_emb =
			jrx->rx_link_cfg[linkIdx].e_minus1 + 1;
		lnk->bits_per_sample =
			jrx->rx_link_cfg[linkIdx].np_minus1 + 1;
		lnk->converter_resolution =
			jrx->rx_link_cfg[linkIdx].n_minus1 + 1;
		if (jrx->common_link_cfg.ver == ADI_APOLLO_JESD_204C)
			lnk->jesd_version = JESD204_VERSION_C;
		else
			lnk->jesd_version = JESD204_VERSION_B;
		lnk->subclass = jrx->common_link_cfg.subclass;
		lnk->scrambling = jrx->rx_link_cfg[linkIdx].scr;
		lnk->high_density = jrx->rx_link_cfg[linkIdx].high_dens;
		lnk->ctrl_words_per_frame_clk = 0;
		lnk->ctrl_bits_per_sample =
			jrx->rx_link_cfg[linkIdx].cs;
		lnk->samples_per_conv_frame =
			jrx->rx_link_cfg[linkIdx].s_minus1 + 1;

		lnk->sample_rate =
			phy->profile.dac_cfg[sideIdx].dac_sampling_rate_Hz;
		lnk->sample_rate_div =
			jrx->rx_link_cfg[linkIdx].link_total_ratio;
		priv->serdes_jrx_cal_run = false;
		break;
	case FRAMER_LINK_A0_RX:
	case FRAMER_LINK_A1_RX:
	case FRAMER_LINK_B0_RX:
	case FRAMER_LINK_B1_RX:
		sideIdx = (lnk->link_id - FRAMER_LINK_A0_RX) / 2;
		linkIdx = (lnk->link_id - FRAMER_LINK_A0_RX) % 2;

		jtx = &phy->profile.jtx[sideIdx];

		lnk->is_transmit = 0;
		lnk->num_lanes =
			jtx->tx_link_cfg[linkIdx].l_minus1 + 1;
		lnk->num_converters =
			jtx->tx_link_cfg[linkIdx].m_minus1 + 1;
		lnk->octets_per_frame =
			jtx->tx_link_cfg[linkIdx].f_minus1 + 1;
		lnk->frames_per_multiframe =
			jtx->tx_link_cfg[linkIdx].k_minus1 + 1;
		lnk->num_of_multiblocks_in_emb =
			jtx->tx_link_cfg[linkIdx].e_minus1 + 1;
		lnk->bits_per_sample =
			jtx->tx_link_cfg[linkIdx].np_minus1 + 1;
		lnk->converter_resolution =
			jtx->tx_link_cfg[linkIdx].n_minus1 + 1;
		if (jtx->common_link_cfg.ver == ADI_APOLLO_JESD_204C)
			lnk->jesd_version = JESD204_VERSION_C;
		else
			lnk->jesd_version = JESD204_VERSION_B;
		lnk->subclass = jtx->common_link_cfg.subclass;
		lnk->scrambling = jtx->tx_link_cfg[linkIdx].scr;
		lnk->high_density = jtx->tx_link_cfg[linkIdx].high_dens;
		lnk->ctrl_words_per_frame_clk = 0;
		lnk->ctrl_bits_per_sample =
			jtx->tx_link_cfg[linkIdx].cs;
		lnk->samples_per_conv_frame =
			jtx->tx_link_cfg[linkIdx].s_minus1 + 1;

		lnk->sample_rate =
			phy->profile.adc_cfg[sideIdx].adc_sampling_rate_Hz;
		lnk->sample_rate_div =
			jtx->tx_link_cfg[linkIdx].link_total_ratio;
		break;
	default:
		return -EINVAL;
	}

	if (lnk->jesd_version == JESD204_VERSION_C)
		lnk->jesd_encoder = JESD204_ENCODER_64B66B;
	else
		lnk->jesd_encoder = JESD204_ENCODER_8B10B;

	ret = jesd204_link_get_rate_khz(lnk, &lane_rate_kbps);
	if (ret)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_link_setup(struct jesd204_dev *jdev,
				     enum jesd204_state_op_reason reason)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;
	struct adi_apollo_device_t *device = &phy->ad9088;
	adi_apollo_rxen_pwrup_ctrl_t rxen_config = {
		.sm_clk_rate = ADI_APOLLO_PUC_CLK_RATE_FS_DIV_32,
		.sm_en = 0,
		.spi_rxen = 1,
		.spi_rxen_en = 1
	};
	adi_apollo_txen_pwrup_ctrl_t txen_config = {
		.sm_clk_rate = ADI_APOLLO_PUC_CLK_RATE_FS_DIV_32,
		.sm_en = 0,
		.spi_txen = 1,
		.spi_txen_en = 1
	};
	uint32_t subclass = 0;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	/* Enable Apollo JTx links */
	ret = adi_apollo_jtx_link_enable_set(device,
					     ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0,
					     ADI_APOLLO_ENABLE);
	if (ret) {
		pr_err("Error enabling JTx links %d\n", ret);
		return ret;
	}
	ret = adi_apollo_jtx_link_enable_set(device,
					     ADI_APOLLO_LINK_A1 | ADI_APOLLO_LINK_B1,
					     ADI_APOLLO_DISABLE);
	if (ret) {
		pr_err("Error enabling JTx links %d\n", ret);
		return ret;
	}

	/* Enable Apollo JRx links */
	ret = adi_apollo_jrx_link_enable_set(device,
					     ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0,
					     ADI_APOLLO_ENABLE);
	if (ret) {
		pr_err("Error enabling JRx links %d\n", ret);
		return ret;
	}
	ret = adi_apollo_jrx_link_enable_set(device,
					     ADI_APOLLO_LINK_A1 | ADI_APOLLO_LINK_B1,
					     ADI_APOLLO_DISABLE);
	if (ret) {
		pr_err("Error enabling JRx links %d\n", ret);
		return ret;
	}

	/* Enable Rx blocks - enable/disable via spi */
	ret = adi_apollo_rxen_pwrup_ctrl_set(device, ADI_APOLLO_RXEN_ADC_ALL,
					     &rxen_config);
	if (ret) {
		pr_err("Error activating Rx blocks (%d)\n", ret);
		return ret;
	}

	/* Enable Tx blocks - enable/disable via spi */
	ret = adi_apollo_txen_pwrup_ctrl_set(device, ADI_APOLLO_TXEN_DAC_ALL,
					     &txen_config);
	if (ret) {
		pr_err("Error activating Tx blocks (%d)\n", ret);
		return ret;
	}

	/* Datapath reset */
	adi_apollo_rxmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 1);
	adi_apollo_txmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 1);
	adi_apollo_rxmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 0);
	adi_apollo_txmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 0);

	if (phy->profile.jtx->common_link_cfg.subclass ||
	    phy->profile.jrx->common_link_cfg.subclass)
		subclass = 1;

	ret = adi_apollo_clk_mcs_subclass_set(device, subclass);
	if (ret) {
		pr_err("Error setting subclass %d\n", ret);
		return ret;
	}

	/* Enable the MCS SYSREF receiver if subclass 1 */
	ret = adi_apollo_clk_mcs_sysref_en_set(device, (subclass == 1) ?
					       ADI_APOLLO_ENABLE : ADI_APOLLO_DISABLE);
	if (ret) {
		pr_err("Error setting MCS SYSREF receiver %d\n", ret);
		return ret;
	}

	ret = adi_apollo_adc_bgcal_freeze(device, device->dev_info.is_8t8r ?
					  ADI_APOLLO_ADC_ALL : ADI_APOLLO_ADC_ALL_4T4R);
	if (ret) {
		pr_err("Error in adi_apollo_adc_bgcal_freeze %d\n", ret);
		return ret;
	}

	ret = adi_apollo_clk_mcs_dyn_sync_sequence_run(device);
	if (ret) {
		pr_err("Error in adi_apollo_clk_mcs_dyn_sync_sequence_run %d\n",
		       ret);
		return ret;
	}
	ret = adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run(device);
	if (ret) {
		pr_err("Error in adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run %d\n",
		       ret);
		return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_setup_stage1(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;
	struct adi_apollo_device_t *device = &phy->ad9088;
	uint32_t adc_cal_chans = device->dev_info.is_8t8r ?
				 ADI_APOLLO_ADC_ALL : ADI_APOLLO_ADC_ALL_4T4R;
	uint32_t n_adc = device->dev_info.is_8t8r ?
			 ADI_APOLLO_ADC_ALL : ADI_APOLLO_ADC_ALL_4T4R;
	/* no-OS stores rx_nyquist_zone as (zone - 1); HAL expects 1 or 2 */
	uint32_t nyquist_zone = phy->rx_nyquist_zone[0][0] + 1;
	adi_apollo_sysclock_cond_cfg_e cc_cal_cfg;
	adi_apollo_init_cal_cfg_e init_cal_cfg;
	uint16_t jrx_phase_adjust;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	phy->rx_en_mask = ADI_APOLLO_RXEN_ADC_ALL;
	phy->tx_en_mask = ADI_APOLLO_TXEN_DAC_ALL;

	ret = adi_apollo_adc_nyquist_zone_set(device, n_adc, nyquist_zone);
	if (ret) {
		pr_err("Error setting ADC Nyquist zone %d\n", ret);
		return ret;
	}

	/* Warmboot-from-user cal data is not supported yet in no-OS */
	cc_cal_cfg = ADI_APOLLO_SYSCLKCONDITIONING_ENABLED;
	pr_info("Run clock conditioning cal (can take up to %d secs)...\n",
		ADI_APOLLO_SYSCLK_COND_CENTER_MAX_TO);

	ret = adi_apollo_cfg_clk_cond_cal_cfg_set(device, cc_cal_cfg);
	if (ret) {
		pr_err("Error in adi_apollo_cfg_clk_cond_cal_cfg_set %d\n", ret);
		return ret;
	}

	ret = adi_apollo_sysclk_cond_cal(device);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_sysclk_cond_cal");
	if (ret)
		return ret;

	/* Inspect the Apollo JRx and JTx link config */
	ret = ad9088_inspect_jrx_link_all(phy);
	if (ret) {
		pr_err("Error in ad9088_inspect_jrx_link_all %d\n", ret);
		return ret;
	}
	ret = ad9088_inspect_jtx_link_all(phy);
	if (ret) {
		pr_err("Error in ad9088_inspect_jtx_link_all %d\n", ret);
		return ret;
	}

	ret = adi_apollo_jrx_phase_adjust_calc(device,
					       ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0,
					       ADI_APOLLO_JRX_PHASE_ADJ_MARGIN_DEFAULT,
					       &jrx_phase_adjust);
	if (ret) {
		pr_err("Error in adi_apollo_jrx_phase_adjust_calc %d\n", ret);
		return ret;
	}

	pr_debug("JRX Phase Adjust: %d\n", jrx_phase_adjust);

	/* Set the jrx phase adjust */
	ret = adi_apollo_jrx_phase_adjust_set(device,
					      ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0,
					      jrx_phase_adjust);
	if (ret) {
		pr_err("Error in adi_apollo_jrx_phase_adjust_set %d\n", ret);
		return ret;
	}

	/* Set the jtx phase adjust */
	ret = adi_apollo_jtx_phase_adjust_set(device,
					      ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0,
					      0);
	if (ret) {
		pr_err("Error in adi_apollo_jtx_phase_adjust_set %d\n", ret);
		return ret;
	}

	/* ADC calibration - warmboot/NVM paths not supported yet in no-OS */
	init_cal_cfg = ADI_APOLLO_INIT_CAL_ENABLED;
	pr_info("Run ADC cal from scratch (can take up to 100 secs)...\n");

	ret = adi_apollo_adc_init_cal_start(device, adc_cal_chans, init_cal_cfg);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_adc_init_cal_start");
	if (ret)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_setup_stage2(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;
	struct adi_apollo_device_t *device = &phy->ad9088;
	uint32_t n_adc = device->dev_info.is_8t8r ?
			 ADI_APOLLO_ADC_ALL : ADI_APOLLO_ADC_ALL_4T4R;
	/* no-OS stores rx_nyquist_zone as (zone - 1); HAL expects 1 or 2 */
	uint32_t nyquist_zone = phy->rx_nyquist_zone[0][0] + 1;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	ret = adi_apollo_adc_init_cal_complete(device, n_adc);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_adc_init_cal_complete");
	if (ret)
		return ret;

	ret = adi_apollo_adc_nyquist_zone_set(device, n_adc, nyquist_zone);
	if (ret) {
		pr_err("Error setting ADC Nyquist zone %d\n", ret);
		return ret;
	}

	ret = adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run(device);
	if (ret) {
		pr_err("Error in adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run %d\n",
		       ret);
		return ret;
	}

	/*
	 * Block-memory CDDC/FDDC sample-delay calibration is deferred:
	 * phy->cddc_sample_delay_en / fddc_sample_delay_en are not yet
	 * ported to no-OS.
	 */

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_clks_enable(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;
	struct adi_apollo_device_t *device = &phy->ad9088;
	uint32_t lane_rate_khz = phy->profile.jrx[0].common_link_cfg.lane_rate_kHz;
	uint16_t serdes;
	int ret;

	switch (lnk->link_id) {
	case DEFRAMER_LINK_A0_TX:
	case DEFRAMER_LINK_A1_TX:
		serdes = ADI_APOLLO_TXRX_SERDES_12PACK_A;
		break;
	case DEFRAMER_LINK_B0_TX:
	case DEFRAMER_LINK_B1_TX:
		serdes = ADI_APOLLO_TXRX_SERDES_12PACK_B;
		break;
	default:
		serdes = ADI_APOLLO_TXRX_SERDES_12PACK_NONE;
		break;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	/* On teardown, freeze the SERDES JRx bg cal for fast lanes */
	if (lnk->is_transmit && reason == JESD204_STATE_OP_REASON_UNINIT &&
	    lane_rate_khz > 16000000) {
		ret = adi_apollo_serdes_jrx_bgcal_freeze(device, serdes);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_serdes_jrx_bgcal_freeze");
		if (ret)
			return ret;

		pr_debug("Link%u: SERDES JRx bg cal freeze\n", lnk->link_id);
	}

	/* On init, run the SERDES JRx init-cal (mandatory above 8 Gbps) */
	if (lnk->is_transmit && reason == JESD204_STATE_OP_REASON_INIT &&
	    lane_rate_khz > 8000000) {
		/* Warmboot from FW cal data is not supported yet in no-OS */
		adi_apollo_init_cal_cfg_e init_cal = ADI_APOLLO_INIT_CAL_ENABLED;

		pr_info("Link%u: SERDES JRx cal Rate %u kBps via INIT_CAL ...\n",
			lnk->link_id, lane_rate_khz);

		ret = adi_apollo_serdes_jrx_init_cal(device, serdes, init_cal);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_serdes_jrx_init_cal");
		if (ret)
			return ret;

		if (lane_rate_khz > 16000000) {
			ret = adi_apollo_serdes_jrx_bgcal_unfreeze(device, serdes);
			ret = ad9088_check_apollo_error(ret,
							"adi_apollo_serdes_jrx_bgcal_unfreeze");
			if (ret)
				return ret;

			pr_debug("Link%u: SERDES JRx bg cal unfreeze\n",
				 lnk->link_id);
		}
	}

	/* Framer (JTx) links: enable on init, disable on teardown */
	if (!lnk->is_transmit) {
		ret = adi_apollo_jtx_link_enable_set(device,
						     ad9088_to_link(lnk->link_id),
						     reason == JESD204_STATE_OP_REASON_INIT);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_jtx_link_enable_set");
		if (ret)
			return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_link_enable(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;
	struct adi_apollo_device_t *device = &phy->ad9088;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	/* Deframer (JRx) links: enable on init, disable on teardown */
	if (lnk->is_transmit) {
		ret = adi_apollo_jrx_link_enable_set(device,
						     ad9088_to_link(lnk->link_id),
						     reason == JESD204_STATE_OP_REASON_INIT);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_jrx_link_enable_set");
		if (ret)
			return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_link_running(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason,
				       struct jesd204_link *lnk)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT) {
		phy->is_initialized = false;

		return JESD204_STATE_CHANGE_DONE;
	}

	if (lnk->is_transmit) {
		ad9088_print_link_phase(phy, lnk);
		ret = ad9088_jesd_rx_link_status_print(phy, lnk, 3);
		if (ret < 0)
			return JESD204_STATE_CHANGE_ERROR;
	} else {
		ret = ad9088_jesd_tx_link_status_print(phy, lnk, 3);
		if (ret < 0)
			return JESD204_STATE_CHANGE_ERROR;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_post_setup_stage1(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (!phy->iio_adf4030 || !phy->iio_adf4382) {
		if (reason == JESD204_STATE_OP_REASON_INIT)
			pr_info("Skipping MCS calibration\n");
		return JESD204_STATE_CHANGE_DONE;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_post_setup_stage2(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;
	struct adi_apollo_device_t *device = &phy->ad9088;
	int ret;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT) {
		adi_apollo_clk_mcs_trig_sync_enable(device, 0);
		adi_apollo_clk_mcs_trig_reset_disable(device);

		return JESD204_STATE_CHANGE_DONE;
	}

	if (phy->trig_sync_en) {
		/* Use Trigger pin A0 to sync Rx and Tx */
		ret = adi_apollo_clk_mcs_sync_trig_map(device, ADI_APOLLO_RX_TX_ALL,
						       ADI_APOLLO_TRIG_PIN_A0);
		if (ret) {
			pr_err("Error in adi_apollo_clk_mcs_sync_trig_map %d\n", ret);
			return ret;
		}

		/* Resync the Rx and Tx dig only during trig sync */
		ret = adi_apollo_clk_mcs_trig_sync_enable(device, 0);
		if (ret) {
			pr_err("Error in adi_apollo_clk_mcs_trig_sync_enable %d\n",
			       ret);
			return ret;
		}
		ret = adi_apollo_clk_mcs_trig_reset_disable(device);
		if (ret) {
			pr_err("Error in adi_apollo_clk_mcs_trig_reset_disable %d\n",
			       ret);
			return ret;
		}

		ret = adi_apollo_clk_mcs_trig_reset_dsp_enable(device);
		if (ret) {
			pr_err("Error in adi_apollo_clk_mcs_trig_reset_dsp_enable %d\n",
			       ret);
			return ret;
		}

		/*
		 * Set trig_syn to 1. Apollo will wait for a trigger from the FPGA. When
		 * received, the FSRC will be reset.
		 *
		 * trig_sync is not self-clearing
		 */
		ret = adi_apollo_clk_mcs_trig_sync_enable(device, 1);
		if (ret) {
			pr_err("Error in adi_apollo_clk_mcs_trig_sync_enable %d\n",
			       ret);
			return ret;
		}
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_post_setup_stage3(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (phy->triq_req_gpio && phy->trig_sync_en) {
		no_os_gpio_set_value(phy->triq_req_gpio, 1);
		no_os_udelay(1);
		no_os_gpio_set_value(phy->triq_req_gpio, 0);
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_post_setup_stage4(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9088_phy *phy = priv->phy;
	struct adi_apollo_device_t *device = &phy->ad9088;
	uint32_t period_fs, temp;
	uint64_t period_rem;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT) {
		phy->is_initialized = false;
		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (phy->trig_sync_en) {
		uint16_t phase, phase1;
		uint16_t period, margin_low, margin_high;

		/* Wait for the trigger sync to finish. */
		ret = adi_apollo_hal_bf_wait_to_set(device,
						    BF_TRIGGER_SYNC_DONE_A0_INFO(MCS_SYNC_MCSTOP0),
						    1000000, 100);
		if (ret) {
			pr_err("Error in adi_apollo_hal_bf_wait_to_set %d\n", ret);
			return ret;
		}
		ret = adi_apollo_clk_mcs_trig_phase_get(device,
							ADI_APOLLO_TRIG_PIN_A0,
							&phase, &phase1);
		if (ret) {
			pr_err("Error in adi_apollo_clk_mcs_trig_phase_get %d\n",
			       ret);
			return ret;
		}

		if (phy->profile.clk_cfg.clocking_mode ==
		    ADI_APOLLO_CLOCKING_MODE_SDR_DIV_8)
			temp = 8;
		else
			temp = 4;

		period_fs = no_os_div64_u64_rem(1000000000000000ULL * temp,
						phy->profile.clk_cfg.dev_clk_freq_Hz,
						&period_rem);

		pr_info("Trigger Phase %d (ideal %u) period %u fs\n", phase,
			phy->profile.mcs_cfg.internal_sysref_prd_digclk_cycles_center / 2,
			(unsigned int)period_fs);

		/*
		 * Validate trigger phase is within safe margin. Per UG-2300:
		 * "The user is recommended to maintain the trigger phase close to
		 * internal_sysref_prd_digclk_cycles/2. If the trigger is too close
		 * to the rising edge of the internal SYSREF, the jitter on the
		 * trigger path may cause the latency varying +/-1 internal SYSREF
		 * clock cycle."
		 *
		 * Use 25%-75% of period as safe range (centered around ideal 50%).
		 */
		period = phy->profile.mcs_cfg.internal_sysref_prd_digclk_cycles_center;
		margin_low = period / 4;
		margin_high = (period * 3) / 4;

		if (phase < margin_low || phase > margin_high)
			pr_warning("Trigger phase %u outside safe margin [%u, %u]. Risk of +/-1 SYSREF cycle latency jitter.\n",
				   phase, margin_low, margin_high);

		ret = adi_apollo_clk_mcs_trig_sync_enable(device, 0);
		if (ret) {
			pr_err("Error in adi_apollo_clk_mcs_trig_sync_enable %d\n",
			       ret);
			return ret;
		}
		ret = adi_apollo_clk_mcs_trig_reset_disable(device);
		if (ret) {
			pr_err("Error in adi_apollo_clk_mcs_trig_reset_disable %d\n",
			       ret);
			return ret;
		}
	}

	ad9088_print_sysref_phase(phy);

	ret = adi_apollo_adc_bgcal_unfreeze(device, device->dev_info.is_8t8r ?
					    ADI_APOLLO_ADC_ALL : ADI_APOLLO_ADC_ALL_4T4R);
	if (ret) {
		pr_err("Error in adi_apollo_adc_bgcal_unfreeze %d\n", ret);
		return ret;
	}

	phy->is_initialized = true;

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9088_jesd204_uninit(struct jesd204_dev *jdev,
				 enum jesd204_state_op_reason reason)
{
	if (reason != JESD204_STATE_OP_REASON_UNINIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	return JESD204_STATE_CHANGE_DONE;
}

const struct jesd204_dev_data jesd204_ad9088_init = {
	.state_ops = {
		[JESD204_OP_DEVICE_INIT] = {
			.per_device = ad9088_jesd204_uninit,
		},
		[JESD204_OP_LINK_INIT] = {
			.per_link = ad9088_jesd204_link_init,
		},
		[JESD204_OP_LINK_SETUP] = {
			.per_device = ad9088_jesd204_link_setup,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_SETUP_STAGE1] = {
			.per_device = ad9088_jesd204_setup_stage1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_SETUP_STAGE2] = {
			.per_device = ad9088_jesd204_setup_stage2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = ad9088_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = ad9088_jesd204_link_enable,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = ad9088_jesd204_link_running,
		},
		[JESD204_OP_OPT_POST_SETUP_STAGE1] = {
			.per_device = ad9088_jesd204_post_setup_stage1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_POST_SETUP_STAGE2] = {
			.per_device = ad9088_jesd204_post_setup_stage2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_POST_SETUP_STAGE3] = {
			.per_device = ad9088_jesd204_post_setup_stage3,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_OPT_POST_RUNNING_STAGE] = {
			.per_device = ad9088_jesd204_post_setup_stage4,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},

	.max_num_links = 4,
	.num_retries = 0,
	.sizeof_priv = sizeof(struct ad9088_jesd204_priv),
};
