// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for AD9088 and similar mixed signal front end (MxFE®)
 *
 * Copyright 2025 Analog Devices Inc.
 */
//#define DEBUG

#include "ad9088.h"

static void ad9088_jesd_lane_setup(struct ad9088_phy *phy, 
				      const struct ad9088_init_param *init_param)
{	
	uint32_t lane_xbar[12];
	int i;

	for (i = 0; i < 12; i++){
		phy->profile.jtx[0].tx_link_cfg[0].lane_xbar[i] = init_param->jtx0_logical_lane_mapping[i];
		phy->profile.jtx[1].tx_link_cfg[0].lane_xbar[i] = init_param->jtx1_logical_lane_mapping[i];
		phy->profile.jrx[0].rx_link_cfg[0].lane_xbar[i] = init_param->jrx0_logical_lane_mapping[i];
		phy->profile.jrx[1].rx_link_cfg[0].lane_xbar[i] = init_param->jrx1_logical_lane_mapping[i];
	}
	
	phy->profile.jrx[0].common_link_cfg.lane_enables = 0;
	phy->profile.jrx[1].common_link_cfg.lane_enables = 0;
	phy->profile.jtx[0].common_link_cfg.lane_enables = 0;
	phy->profile.jtx[1].common_link_cfg.lane_enables = 0;
	for (i = 0; i < 12; i++) {
		phy->profile.jtx[0].serializer_lane[i].ser_amplitude = init_param->jtx_ser_amplitude;
		phy->profile.jtx[0].serializer_lane[i].ser_pre_emphasis = init_param->jtx_ser_pre_emphasis;
		phy->profile.jtx[0].serializer_lane[i].ser_post_emphasis = init_param->jtx_ser_post_emphasis;

		phy->profile.jtx[1].serializer_lane[i].ser_amplitude = init_param->jtx_ser_amplitude;
		phy->profile.jtx[1].serializer_lane[i].ser_pre_emphasis = init_param->jtx_ser_pre_emphasis;
		phy->profile.jtx[1].serializer_lane[i].ser_post_emphasis = init_param->jtx_ser_post_emphasis;

		/* JRX */
		if (phy->profile.jrx[0].rx_link_cfg[0].link_in_use)
			if (i <= (phy->profile.jrx[0].rx_link_cfg[0].l_minus1))
				phy->profile.jrx[0].common_link_cfg.lane_enables |= (1 << phy->profile.jrx[0].rx_link_cfg[0].lane_xbar[i]);

		if (phy->profile.jrx[1].rx_link_cfg[0].link_in_use)
			if (i <= (phy->profile.jrx[1].rx_link_cfg[0].l_minus1))
				phy->profile.jrx[1].common_link_cfg.lane_enables |= (1 << phy->profile.jrx[1].rx_link_cfg[0].lane_xbar[i]);

		/* JRX Link2 */

		if (phy->profile.jrx[0].rx_link_cfg[1].link_in_use)
			if (i <= (phy->profile.jrx[0].rx_link_cfg[1].l_minus1))
				phy->profile.jrx[0].common_link_cfg.lane_enables |= (1 << phy->profile.jrx[0].rx_link_cfg[1].lane_xbar[i]);

		if (phy->profile.jrx[1].rx_link_cfg[1].link_in_use)
			if (i <= (phy->profile.jrx[1].rx_link_cfg[1].l_minus1))
				phy->profile.jrx[1].common_link_cfg.lane_enables |= (1 << phy->profile.jrx[1].rx_link_cfg[1].lane_xbar[i]);

		/* JTX */
		if (phy->profile.jtx[0].tx_link_cfg[0].link_in_use)
			if ((int32_t)phy->profile.jtx[0].tx_link_cfg[0].lane_xbar[i] <= (int32_t)phy->profile.jtx[0].tx_link_cfg[0].l_minus1)
				phy->profile.jtx[0].common_link_cfg.lane_enables |= (1 << i);

		if (phy->profile.jtx[1].tx_link_cfg[0].link_in_use)
			if ((int32_t)phy->profile.jtx[1].tx_link_cfg[0].lane_xbar[i] <= (int32_t)phy->profile.jtx[1].tx_link_cfg[0].l_minus1)
				phy->profile.jtx[1].common_link_cfg.lane_enables |= (1 << i);

		/* JTX Link2 */

		if (phy->profile.jtx[0].tx_link_cfg[1].link_in_use)
			if ((int32_t)phy->profile.jtx[0].tx_link_cfg[1].lane_xbar[i] <= (int32_t)phy->profile.jtx[0].tx_link_cfg[1].l_minus1)
				phy->profile.jtx[0].common_link_cfg.lane_enables |= (1 << i);

		if (phy->profile.jtx[1].tx_link_cfg[1].link_in_use)
			if ((int32_t)phy->profile.jtx[1].tx_link_cfg[1].lane_xbar[i] <= (int32_t)phy->profile.jtx[1].tx_link_cfg[1].l_minus1)
				phy->profile.jtx[1].common_link_cfg.lane_enables |= (1 << i);
	}

	return 0;
}

int ad9088_get_profile(struct ad9088_phy *phy, 
		       const struct ad9088_init_param *init_param)
{
	adi_apollo_top_t *p = &phy->profile;
	size_t firmware_size = (size_t)_binary____noos_drivers_rf_transceiver_apollo_firmware_usecase_bin_size;
	const uint8_t *firmware_ptr = _binary____noos_drivers_rf_transceiver_apollo_firmware_usecase_bin_start;
	
	if (sizeof(*p) != firmware_size) {
		pr_err("Invalid size of profile structure %zu, expected %zu\n",
		       sizeof(*p), firmware_size);
		return -EINVAL;
	}

	memcpy(p, firmware_ptr, firmware_size);

	return 0;
}

int ad9088_parse_struct(struct ad9088_phy **device, 
			const struct ad9088_init_param *init_param)
{
	struct ad9088_phy *phy;
	uint32_t nz;
	int i, j;
	int ret;

	if (!device || !init_param || !init_param->spi_init)
		return -EINVAL;

	phy = (struct ad9088_phy *)no_os_calloc(1, sizeof(*phy));
	if (!phy)
		return -ENOMEM;

	ret = no_os_spi_init(&phy->spi, init_param->spi_init);
	if (ret) {
		goto error_dev;
	}

	ret = no_os_gpio_get(&phy->reset_gpio, init_param->gpio_reset);
	if (ret) {
		goto error_spi;
	}

	ret = no_os_gpio_direction_output(phy->reset_gpio, NO_OS_GPIO_HIGH);
	if (ret) {
		goto error_reset;
	}

	phy->spi_3wire_en = init_param->spi_3wire_en;
	phy->complex_rx = init_param->rx_real_channel_en;
	phy->complex_tx = init_param->tx_real_channel_en;
	phy->side_b_use_own_tpl_en = init_param->side_b_use_own_tpl_en;
	phy->multidevice_instance_count = init_param->multidevice_instance_count;
	phy->trig_sync_en = init_param->trig_sync_en;
	phy->standalone = init_param->standalone_en;
	nz = init_param->nyquist_zone;

	if (nz != 1 && nz != 2) {
		pr_err("Invalid Nyquist zone %u\n", nz);
		return -EINVAL;
	}

	for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++)
		for (j = 0; j < MAX_NUM_MAIN_DATAPATHS; j++)
			phy->rx_nyquist_zone[i][j] = nz - 1;

	ret = ad9088_get_profile(phy, init_param);
	if (ret) {
		pr_err("Failed to get profile: %d\n", ret);
		goto error_reset;
	}

	ad9088_jesd_lane_setup(&phy, init_param);
	
	phy->profile.jtx[0].common_link_cfg.subclass = init_param->subclass;
	phy->profile.jtx[1].common_link_cfg.subclass = init_param->subclass;
	phy->profile.jrx[0].common_link_cfg.subclass = init_param->subclass;
	phy->profile.jrx[1].common_link_cfg.subclass = init_param->subclass;

	*device = phy;
	return 0;

error_reset:
	no_os_gpio_remove(phy->reset_gpio);
error_spi:
	no_os_spi_remove(phy->spi);
error_dev:
	no_os_free(phy);
	return ret;
}

EXPORT_SYMBOL(ad9088_parse_dt);
