// SPDX-License-Identifier: GPL-2.0
/*
 * AD9088 JESD204 FSM support
 *
 * Copyright 2026 Analog Devices Inc.
 */

#include "ad9088.h"

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
	},

	.max_num_links = 4,
	.num_retries = 0,
	.sizeof_priv = sizeof(struct ad9088_jesd204_priv),
};
