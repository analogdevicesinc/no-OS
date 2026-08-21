// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for AD9088 and similar mixed signal front end (MxFE®)
 *
 * Copyright 2025 Analog Devices Inc.
 */

#include "ad9088.h"
#include "no_os_alloc.h"
#include "no_os_crc32.h"

static void ad9088_jesd_lane_setup(struct ad9088_phy *phy, 
				      const struct ad9088_init_param *init_param)
{
	int i;

	for (i = 0; i < 12; i++){
		phy->profile.jtx[0].tx_link_cfg[0].lane_xbar[i] = init_param->jtx0_logical_lane_mapping[i];
		phy->profile.jtx[1].tx_link_cfg[0].lane_xbar[i] = init_param->jtx1_logical_lane_mapping[i];
		phy->profile.jrx[0].rx_link_cfg[0].lane_xbar[i] = init_param->jrx0_physical_lane_mapping[i];
		phy->profile.jrx[1].rx_link_cfg[0].lane_xbar[i] = init_param->jrx1_physical_lane_mapping[i];
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
}

int ad9088_get_profile(struct ad9088_phy *phy)
{
	adi_apollo_top_t *p = &phy->profile;
	size_t firmware_size = AD9088_FW_SYM(usecase_bin_end) -
			       AD9088_FW_SYM(usecase_bin_start);
	const uint8_t *firmware_ptr = AD9088_FW_SYM(usecase_bin_start);
	
	if (sizeof(*p) != firmware_size) {
		pr_err("Invalid size of profile structure %zu, expected %zu\n",
		       sizeof(*p), firmware_size);
		return -EINVAL;
	}

	memcpy(p, firmware_ptr, firmware_size);

	return 0;
}

static void ad9088_print_sysref_inp_cfg(const char *name,
					const adi_apollo_sysref_inp_cfg_t *cfg,
					uint16_t int_prd)
{
	pr_info("profile: %s_sysref present=%u term=%u cm900=%u "
		"ref_to_int_ratio=%llu int_prd_digclk=%u\n",
		name, (unsigned int)cfg->sysref_present,
		(unsigned int)cfg->rx_term_en,
		(unsigned int)cfg->cm_above_900mv,
		cfg->ref_to_int_period_ratio, (unsigned int)int_prd);
}

/*
 * Dump the SYSREF/MCS configuration carried by the device profile, before any
 * host-side override is applied.
 *
 * These fields are consumed by the on-chip CPU firmware and no host code reads
 * them (only internal_sysref_prd_digclk_cycles_center, for the trigger-phase
 * margin check), so a runtime dump is the only way to see what the profile
 * binary actually asks for. Needed when moving to subclass 1: sysref_present
 * tells the firmware to expect an external SYSREF, and the internal SYSREF
 * period sets the ratio the external BSYNC has to satisfy.
 */
static void ad9088_print_profile_sysref_cfg(const struct ad9088_phy *phy)
{
	const adi_apollo_mcs_cfg_t *mcs = &phy->profile.mcs_cfg;

	pr_info("profile: subclass jtx0=%u jtx1=%u jrx0=%u jrx1=%u\n",
		(unsigned int)phy->profile.jtx[0].common_link_cfg.subclass,
		(unsigned int)phy->profile.jtx[1].common_link_cfg.subclass,
		(unsigned int)phy->profile.jrx[0].common_link_cfg.subclass,
		(unsigned int)phy->profile.jrx[1].common_link_cfg.subclass);

	ad9088_print_sysref_inp_cfg("center", &mcs->center_sysref,
				    mcs->internal_sysref_prd_digclk_cycles_center);
	ad9088_print_sysref_inp_cfg("side_a", &mcs->side_a_sysref,
				    mcs->internal_sysref_prd_digclk_cycles_side_a);
	ad9088_print_sysref_inp_cfg("side_b", &mcs->side_b_sysref,
				    mcs->internal_sysref_prd_digclk_cycles_side_b);

	pr_info("profile: gapped_sysref=%u leave_sysref_rx_on=%u "
		"num_avg=%u lock_window=%u\n",
		(unsigned int)mcs->use_gapped_sysref,
		(unsigned int)mcs->leave_sysref_rx_on,
		(unsigned int)mcs->num_sysref_avg_mcs_fw,
		(unsigned int)mcs->sysref_lock_window_mcs_fw);
}

int ad9088_parse_struct(struct ad9088_phy **device,
			const struct ad9088_init_param *init_param)
{
	const adi_apollo_profile_version_t *ver;
	struct ad9088_phy *phy;
	uint32_t nz;
	int i, j;
	int ret;

	if (!device || !init_param || !init_param->spi_init)
		return -EINVAL;

	phy = (struct ad9088_phy *)no_os_calloc(1, sizeof(*phy));
	if (!phy)
		return -ENOMEM;

	ret = no_os_gpio_get(&phy->reset_gpio, init_param->gpio_reset);
	if (ret)
		goto error_dev;

	ret = no_os_gpio_direction_output(phy->reset_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_reset;

	/*
	 * Optional: the trigger-sync request pulse in post_setup_stage3 is
	 * skipped when the board leaves this unwired.
	 */
	if (init_param->gpio_tri_req) {
		ret = no_os_gpio_get(&phy->triq_req_gpio,
				     init_param->gpio_tri_req);
		if (ret)
			goto error_reset;

		ret = no_os_gpio_direction_output(phy->triq_req_gpio,
						  NO_OS_GPIO_LOW);
		if (ret)
			goto error_tri_req;
	}

	ret = no_os_spi_init(&phy->spi, init_param->spi_init);
	if (ret)
		goto error_tri_req;

	phy->spi_3wire_en = init_param->spi_3wire_en;
	phy->trig_sync_en = init_param->trig_sync_en;
	phy->cnco_dual_modulus_mode_en = init_param->cnco_dual_modulus_mode_en;
	phy->fnco_dual_modulus_mode_en = init_param->fnco_dual_modulus_mode_en;
	phy->bsync_ops = init_param->bsync_ops;
	phy->clk_ops = init_param->clk_ops;
	phy->aion_background_serial_alignment_en =
		init_param->aion_background_serial_alignment_en;
	phy->mcs_track_decimation = init_param->mcs_track_decimation ?
				    init_param->mcs_track_decimation :
				    AD9088_MCS_TRACK_DECIMATION_DEFAULT;
	nz = init_param->nyquist_zone;

	if (nz != 1 && nz != 2) {
		pr_err("Invalid Nyquist zone %u\n", nz);
		ret = -EINVAL;
		goto error_spi;
	}

	for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++)
		for (j = 0; j < MAX_NUM_MAIN_DATAPATHS; j++)
			phy->rx_nyquist_zone[i][j] = nz - 1;

	ret = ad9088_get_profile(phy);
	if (ret) {
		pr_err("Failed to get profile: %d\n", ret);
		goto error_spi;
	}

	/*
	 * The loader only checks the profile's size, and every version of the
	 * struct has the same size, so a stale binary loads silently and then
	 * has its fields interpreted under the wrong layout.
	 */
	ver = &phy->profile.profile_cfg.profile_version;
	if (ver->major != ADI_APOLLO_PROFILE_VERSION_MAJOR ||
	    ver->minor != ADI_APOLLO_PROFILE_VERSION_MINOR) {
		pr_err("Incompatible profile version %u.%u != %u.%u\n",
		       (unsigned int)ver->major, (unsigned int)ver->minor,
		       (unsigned int)ADI_APOLLO_PROFILE_VERSION_MAJOR,
		       (unsigned int)ADI_APOLLO_PROFILE_VERSION_MINOR);
		ret = -EINVAL;
		goto error_spi;
	}

	ad9088_jesd_lane_setup(phy, init_param);

	ad9088_print_profile_sysref_cfg(phy);

	phy->profile.jtx[0].common_link_cfg.subclass = init_param->subclass;
	phy->profile.jtx[1].common_link_cfg.subclass = init_param->subclass;
	phy->profile.jrx[0].common_link_cfg.subclass = init_param->subclass;
	phy->profile.jrx[1].common_link_cfg.subclass = init_param->subclass;

	/*
	 * Subclass 1 needs the CPU firmware told to expect an external SYSREF.
	 * The profiles shipped here carry sysref_present = 0 on all three MCS
	 * inputs, so without this the SYSREF receiver is enabled on the host
	 * side while the firmware still believes there is nothing to receive.
	 */
	if (init_param->subclass) {
		adi_apollo_mcs_cfg_t *mcs = &phy->profile.mcs_cfg;

		if (!mcs->side_a_sysref.sysref_present ||
		    !mcs->side_b_sysref.sysref_present)
			mcs->center_sysref.sysref_present = true;
	}

	/*
	 * MCS tracking cal corrects the device clock once it drifts past this
	 * window. Overriding it is optional: 0 keeps whatever the loaded device
	 * profile carries, read back here so the tracking cal can use it.
	 */
	if (init_param->mcs_track_win) {
		phy->mcs_track_win = init_param->mcs_track_win;
		phy->profile.mcs_cfg.adf4382_cfg.track_win[0] = phy->mcs_track_win;
		phy->profile.mcs_cfg.adf4382_cfg.track_win[1] = phy->mcs_track_win;
	} else {
		phy->mcs_track_win = phy->profile.mcs_cfg.adf4382_cfg.track_win[0];
	}

	/*
	 * Last, after every mutation above. The firmware CRC-checks the profile
	 * it is handed and warns when it disagrees; recomputing keeps that
	 * check meaningful instead of permanently failed. profile_checksum is
	 * the final member of the packed struct and is excluded from its own
	 * checksum.
	 */
	phy->profile.profile_checksum =
		no_os_crc32_be(0, (const uint8_t *)&phy->profile,
			       sizeof(phy->profile) - sizeof(uint32_t));

	*device = phy;
	return 0;

error_spi:
	no_os_spi_remove(phy->spi);
error_tri_req:
	no_os_gpio_remove(phy->triq_req_gpio);
error_reset:
	no_os_gpio_remove(phy->reset_gpio);
error_dev:
	no_os_free(phy);
	return ret;
}
