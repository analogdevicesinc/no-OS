// SPDX-License-Identifier: GPL-2.0
/*
 * AD9088 MCS (Multi-Chip Synchronization) calibration support
 *
 * Copyright 2026 Analog Devices Inc.
 */

#include "ad9088.h"
#include "no_os_util.h"

/*
 * The alignment the init cal achieves is checked against +/- 0.4 device clock
 * cycles. The residual is computed in milli-cycles (femtoseconds x hertz scaled
 * down by 1e12), so the limit is expressed the same way.
 */
#define AD9088_MCS_ALIGN_LIMIT_MILLICYCLES	400
#define AD9088_MCS_FS_PER_CYCLE_SCALE		1000000000000ULL

/**
 * @brief Zero the desired internal-to-external SYSREF offsets before init cal.
 *
 * The offsets are what the calibration aims for; zero asks for the internal
 * SYSREF to land on the external edge. Which offsets exist depends on whether
 * the device runs one clock or two.
 *
 * @param phy - The device structure.
 * @return    - 0 in case of success, negative error code otherwise.
 */
int ad9088_mcs_init_cal_setup(struct ad9088_phy *phy)
{
	adi_apollo_device_t *device = &phy->ad9088;
	int ret;

	if (!device->dev_info.is_dual_clk) {
		ret = adi_apollo_mcs_cal_parameter_set(device,
						      MCS_OFFSET_C_FEMTOSECONDS_INT64,
						      0);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_mcs_cal_parameter_set");
		if (ret)
			return ret;

		return 0;
	}

	ret = adi_apollo_mcs_cal_parameter_set(device,
					       MCS_OFFSET_A_FEMTOSECONDS_INT64, 0);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_mcs_cal_parameter_set");
	if (ret)
		return ret;

	ret = adi_apollo_mcs_cal_parameter_set(device,
					       MCS_OFFSET_B_FEMTOSECONDS_INT64, 0);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_mcs_cal_parameter_set");
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Start a BSYNC delta-t measurement.
 *
 * Mode 0 aligns against the incoming BSYNC. Mode 1 makes the device drive
 * BSYNC back out so the provider can measure the return path; mode 2 stops it
 * driving. Running 0 then 1 yields the two halves of the round-trip delay.
 *
 * @param phy  - The device structure.
 * @param mode - 0 align, 1 output enable and loopback measure, 2 output
 *		 disable.
 * @return     - 0 in case of success, negative error code otherwise.
 */
int ad9088_delta_t_measurement_set(struct ad9088_phy *phy, uint32_t mode)
{
	adi_apollo_mailbox_cmd_mcs_bsync_set_config_t bsync_set_config_cmd = {0};
	adi_apollo_mailbox_resp_mcs_bsync_set_config_t bsync_set_config_resp = {0};
	adi_apollo_mailbox_resp_mcs_bsync_go_t bsync_go_resp = {0};
	adi_apollo_device_t *device = &phy->ad9088;
	uint32_t bsync_divider;
	int ret;

	if (phy->profile.clk_cfg.clocking_mode == ADI_APOLLO_CLOCKING_MODE_SDR_DIV_8)
		bsync_divider = 8 *
			phy->profile.mcs_cfg.internal_sysref_prd_digclk_cycles_center;
	else
		bsync_divider = 4 *
			phy->profile.mcs_cfg.internal_sysref_prd_digclk_cycles_center;

	switch (mode) {
	case 0:
		bsync_set_config_cmd.func_mode = APOLLO_MCS_BSYNC_ALIGN;
		break;
	case 1:
		bsync_set_config_cmd.func_mode = APOLLO_MCS_BSYNC_OUTPUT_EN;
		break;
	case 2:
		bsync_set_config_cmd.func_mode = APOLLO_MCS_BSYNC_OUTPUT_DIS;
		break;
	default:
		return -EINVAL;
	}

	bsync_set_config_cmd.bsync_div = bsync_divider;

	ret = adi_apollo_mailbox_mcs_bsync_set_config(device,
			&bsync_set_config_cmd, &bsync_set_config_resp);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_mailbox_mcs_bsync_set_config");
	if (ret)
		return ret;

	if (bsync_set_config_resp.status)
		pr_warning("bsync_set_config_resp.status: %u\n",
			   (unsigned int)bsync_set_config_resp.status);

	ret = adi_apollo_mailbox_mcs_bsync_go(device, &bsync_go_resp);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_mailbox_mcs_bsync_go");
	if (ret)
		return ret;

	if (bsync_go_resp.status)
		pr_warning("bsync_go_resp.status: %u\n",
			   (unsigned int)bsync_go_resp.status);

	return 0;
}

/**
 * @brief Read back the result of a BSYNC delta-t measurement.
 * @param phy            - The device structure.
 * @param mode           - 0 for the post-alignment difference, 1 for the delay
 *			   that needs compensating.
 * @param apollo_delta_t - Read value, in femtoseconds.
 * @return               - 0 in case of success, negative error code otherwise.
 */
int ad9088_delta_t_measurement_get(struct ad9088_phy *phy, uint32_t mode,
				   int64_t *apollo_delta_t)
{
	adi_apollo_mailbox_resp_mcs_bsync_get_config_t bsync_get_config_resp = {0};
	adi_apollo_device_t *device = &phy->ad9088;
	int ret;

	if (!apollo_delta_t)
		return -EINVAL;

	ret = adi_apollo_mailbox_mcs_bsync_get_config(device,
			&bsync_get_config_resp);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_mailbox_mcs_bsync_get_config");
	if (ret)
		return ret;

	switch (mode) {
	case 0:
		*apollo_delta_t = bsync_get_config_resp.delta_t0;
		break;
	case 1:
		*apollo_delta_t = bsync_get_config_resp.delta_t1;
		break;
	default:
		return -EINVAL;
	}

	pr_debug("bsync_get_config_resp: status %u func_mode %u bsync_div %u "
		 "done %u\n",
		 (unsigned int)bsync_get_config_resp.status,
		 (unsigned int)bsync_get_config_resp.func_mode,
		 (unsigned int)bsync_get_config_resp.bsync_div,
		 (unsigned int)bsync_get_config_resp.done_flag);
	pr_debug("bsync_get_config_resp: delta_t0 %lld delta_t1 %lld\n",
		 bsync_get_config_resp.delta_t0, bsync_get_config_resp.delta_t1);

	return 0;
}

/**
 * @brief Residual alignment error, in milli-device-clock-cycles.
 *
 * fs x Hz / 1e12 gives thousandths of a clock cycle. The divisor exceeds what
 * no_os_div_u64() accepts, so the 64-bit-divisor variant is required.
 */
static uint32_t ad9088_mcs_align_millicycles(int64_t after_fs,
		int64_t recommended_fs,
		uint64_t dev_clk_hz)
{
	uint64_t diff_fs;
	uint64_t rem;

	diff_fs = (after_fs >= recommended_fs) ?
		  (uint64_t)(after_fs - recommended_fs) :
		  (uint64_t)(recommended_fs - after_fs);

	return (uint32_t)no_os_div64_u64_rem(diff_fs * dev_clk_hz,
					     AD9088_MCS_FS_PER_CYCLE_SCALE, &rem);
}

/**
 * @brief Check that MCS init cal aligned the internal SYSREF and locked.
 *
 * The external and internal SYSREF must end up within +/- 0.4 device clock
 * cycles of the offset the calibration recommended, and the relevant SYSREF
 * must report locked. In dual clock mode both sides are checked and at least
 * one must lock; in single clock mode only the center matters.
 *
 * @param phy        - The device structure.
 * @param cal_status - Init cal status read back from the device.
 * @return           - 0 when the calibration is good, negative otherwise.
 */
int ad9088_mcs_init_cal_validate(struct ad9088_phy *phy,
				 adi_apollo_mcs_cal_init_status_t *cal_status)
{
	adi_apollo_device_t *device = &phy->ad9088;
	uint64_t dev_clk_hz = phy->profile.clk_cfg.dev_clk_freq_Hz;
	uint32_t diff_millicycles;
	int ret = 0;

	if (!cal_status)
		return -EINVAL;

	if (device->dev_info.is_dual_clk) {
		diff_millicycles = ad9088_mcs_align_millicycles(
					   cal_status->data.diff_A_After_femtoseconds,
					   cal_status->data.recommended_offset_A_femtoseconds,
					   dev_clk_hz);
		if (diff_millicycles > AD9088_MCS_ALIGN_LIMIT_MILLICYCLES) {
			pr_err("Side A: time difference too large: %u.%03u cycles\n",
			       (unsigned int)(diff_millicycles / 1000),
			       (unsigned int)(diff_millicycles % 1000));
			ret = -EFAULT;
			goto end;
		}

		diff_millicycles = ad9088_mcs_align_millicycles(
					   cal_status->data.diff_B_After_femtoseconds,
					   cal_status->data.recommended_offset_B_femtoseconds,
					   dev_clk_hz);
		if (diff_millicycles > AD9088_MCS_ALIGN_LIMIT_MILLICYCLES) {
			pr_err("Side B: time difference too large: %u.%03u cycles\n",
			       (unsigned int)(diff_millicycles / 1000),
			       (unsigned int)(diff_millicycles % 1000));
			ret = -EFAULT;
			goto end;
		}

		if (!cal_status->data.is_A_Locked && !cal_status->data.is_B_Locked) {
			pr_err("MCS Init Cal did not lock SysRefs (A=%u, B=%u)\n",
			       (unsigned int)cal_status->data.is_A_Locked,
			       (unsigned int)cal_status->data.is_B_Locked);
			ret = -EFAULT;
			goto end;
		}
	} else {
		diff_millicycles = ad9088_mcs_align_millicycles(
					   cal_status->data.diff_C_After_femtoseconds,
					   cal_status->data.recommended_offset_C_femtoseconds,
					   dev_clk_hz);
		if (diff_millicycles > AD9088_MCS_ALIGN_LIMIT_MILLICYCLES) {
			pr_err("Time difference between internal and external "
			       "SysRefs is too large: %u.%03u cycles\n",
			       (unsigned int)(diff_millicycles / 1000),
			       (unsigned int)(diff_millicycles % 1000));
			ret = -EFAULT;
			goto end;
		}

		if (cal_status->data.is_C_Locked != 1) {
			pr_err("MCS Init Cal did not lock SysRefs\n");
			ret = -EFAULT;
			goto end;
		}
	}

	if (cal_status->hdr.errorCode) {
		pr_err("MCS Init Cal Apollo CPU errorCode: 0x%X\n",
		       (unsigned int)cal_status->hdr.errorCode);
		ret = -EFAULT;
		goto end;
	}

	if (cal_status->mcsErr) {
		pr_err("MCS Init Cal MCS errorCode: 0x%X\n",
		       (unsigned int)cal_status->mcsErr);
		ret = -EFAULT;
		goto end;
	}

end:
	pr_info("MCS Init Cal Validation: %s\n", ret ? "Failed!" : "Passed");

	return ret;
}

/**
 * @brief Print the MCS init cal status and measurements.
 * @param phy        - The device structure.
 * @param cal_status - Init cal status read back from the device.
 */
void ad9088_mcs_init_cal_status_print(struct ad9088_phy *phy,
				      adi_apollo_mcs_cal_init_status_t *cal_status)
{
	if (!phy || !cal_status)
		return;

	pr_info("MCS init cal: errorCode %u percentComplete %u performanceMetric %u\n",
		(unsigned int)cal_status->hdr.errorCode,
		(unsigned int)cal_status->hdr.percentComplete,
		(unsigned int)cal_status->hdr.performanceMetric);
	pr_info("MCS init cal: iterCount %u updateCount %u mcsErr %u\n",
		(unsigned int)cal_status->hdr.iterCount,
		(unsigned int)cal_status->hdr.updateCount,
		(unsigned int)cal_status->mcsErr);
	pr_info("MCS init cal: locked C=%u A=%u B=%u\n",
		(unsigned int)cal_status->data.is_C_Locked,
		(unsigned int)cal_status->data.is_A_Locked,
		(unsigned int)cal_status->data.is_B_Locked);
	pr_info("MCS init cal: diff before C %lld A %lld B %lld fs\n",
		cal_status->data.diff_C_Before_femtoseconds,
		cal_status->data.diff_A_Before_femtoseconds,
		cal_status->data.diff_B_Before_femtoseconds);
	pr_info("MCS init cal: internal period C %lld A %lld B %lld fs\n",
		cal_status->data.internal_period_C_femtoseconds,
		cal_status->data.internal_period_A_femtoseconds,
		cal_status->data.internal_period_B_femtoseconds);
	pr_info("MCS init cal: diff after C %lld A %lld B %lld fs\n",
		cal_status->data.diff_C_After_femtoseconds,
		cal_status->data.diff_A_After_femtoseconds,
		cal_status->data.diff_B_After_femtoseconds);
	pr_info("MCS init cal: recommended offset C %lld A %lld B %lld fs\n",
		cal_status->data.recommended_offset_C_femtoseconds,
		cal_status->data.recommended_offset_A_femtoseconds,
		cal_status->data.recommended_offset_B_femtoseconds);
}

/**
 * @brief Print the MCS tracking cal status.
 * @param phy              - The device structure.
 * @param cal_status       - Tracking cal status read back from the device.
 * @param print_full_state - Print every field rather than just the clock PLL
 *			     correction currently applied.
 */
void ad9088_mcs_track_cal_status_print(struct ad9088_phy *phy,
				       adi_apollo_mcs_cal_status_t *cal_status,
				       uint8_t print_full_state)
{
	adi_apollo_mcs_private_cal_status_t *mcs_cal_status;
	int i;

	if (!phy || !cal_status)
		return;

	mcs_cal_status = &cal_status->mcs_tracking_cal_status;

	if (!print_full_state) {
		pr_info("MCS tracking cal[0]: bleed_pol %u coarse %d fine %d "
			"measure %lld fs\n",
			(unsigned int)mcs_cal_status->adf4382_specific_status[0].bleed_pol,
			(int)mcs_cal_status->adf4382_specific_status[0].current_coarse_value,
			(int)mcs_cal_status->adf4382_specific_status[0].current_fine_value,
			mcs_cal_status->current_measure[0]);
		return;
	}

	pr_info("MCS tracking cal: errorCode %u percentComplete %u "
		"performanceMetric %u\n",
		(unsigned int)cal_status->hdr.errorCode,
		(unsigned int)cal_status->hdr.percentComplete,
		(unsigned int)cal_status->hdr.performanceMetric);
	pr_info("MCS tracking cal: iterCount %u updateCount %u\n",
		(unsigned int)cal_status->hdr.iterCount,
		(unsigned int)cal_status->hdr.updateCount);
	pr_info("MCS tracking cal: foreground_done %u halt_active %u abort_done %u\n",
		(unsigned int)mcs_cal_status->foreground_done,
		(unsigned int)mcs_cal_status->halt_active,
		(unsigned int)mcs_cal_status->abort_done);

	for (i = 0; i < 2; i++) {
		pr_info("MCS tracking cal[%d]: state %u lock %u force_bg_done %u\n",
			i, (unsigned int)mcs_cal_status->track_state[i],
			(unsigned int)mcs_cal_status->track_lock[i],
			(unsigned int)mcs_cal_status->force_background_done[i]);
		pr_info("MCS tracking cal[%d]: bleed_pol %u coarse %d fine %d\n",
			i,
			(unsigned int)mcs_cal_status->adf4382_specific_status[i].bleed_pol,
			(int)mcs_cal_status->adf4382_specific_status[i].current_coarse_value,
			(int)mcs_cal_status->adf4382_specific_status[i].current_fine_value);
		pr_info("MCS tracking cal[%d]: EOR pos %u neg %u coarse %u measure %lld fs\n",
			i,
			(unsigned int)mcs_cal_status->adf4382_specific_status[i].EOR_POS,
			(unsigned int)mcs_cal_status->adf4382_specific_status[i].EOR_NEG,
			(unsigned int)mcs_cal_status->adf4382_specific_status[i].EOR_Coarse,
			mcs_cal_status->current_measure[i]);
	}
}

/**
 * @brief Arm MCS tracking calibration.
 *
 * Tracking cal keeps the internal SYSREF on the external edge as the device
 * clock drifts, by trimming the clock PLL. Only supported in single clock mode.
 *
 * @param phy                  - The device structure.
 * @param mcs_track_decimation - TDC decimation; higher is more precise.
 * @param initialize_track_cal - Initialize the tracking cal, needed unless the
 *				 device profile already did it.
 * @return                     - 0 in case of success, negative error otherwise.
 */
int ad9088_mcs_tracking_cal_setup(struct ad9088_phy *phy,
				  uint32_t mcs_track_decimation,
				  uint16_t initialize_track_cal)
{
	adi_apollo_device_t *device = &phy->ad9088;
	int ret;

	/* More decimation buys more precise TDC measurements. */
	ret = adi_apollo_mcs_cal_tracking_decimation_set(device,
			(uint16_t)mcs_track_decimation);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_mcs_cal_tracking_decimation_set");
	if (ret)
		return ret;

	/*
	 * Target offset between internal and external SYSREF. Zero asks the
	 * tracking loop to hold them coincident; a board that needs a standing
	 * skew would set these instead.
	 */
	ret = adi_apollo_mcs_cal_parameter_set(device,
					       MCS_ADF4382_TRACK_TARGET_0_INT32, 0);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_mcs_cal_parameter_set");
	if (ret)
		return ret;

	if (device->dev_info.is_dual_clk) {
		ret = adi_apollo_mcs_cal_parameter_set(device,
						       MCS_ADF4382_TRACK_TARGET_1_INT32,
						       0);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_mcs_cal_parameter_set");
		if (ret)
			return ret;
	}

	/* Decimation has to be in place before the cal is enabled. */
	ret = adi_apollo_mcs_cal_tracking_enable(device, 1);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_mcs_cal_tracking_enable");
	if (ret)
		return ret;

	if (initialize_track_cal) {
		ret = adi_apollo_mcs_cal_tracking_initialize_set(device);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_mcs_cal_tracking_initialize_set");
		if (ret)
			return ret;
	}

	/*
	 * The device clock PLL has to keep re-applying its phase correction for
	 * the tracking loop to have anything to steer.
	 */
	if (phy->clk_ops) {
		if (phy->clk_ops->auto_align_set) {
			ret = phy->clk_ops->auto_align_set(phy->clk_ops->ctx, true);
			if (ret) {
				pr_err("Failed to enable clock auto alignment: %d\n",
				       ret);
				return ret;
			}
		}

		if (phy->clk_ops->phase_set_fs) {
			ret = phy->clk_ops->phase_set_fs(phy->clk_ops->ctx,
							 AD9088_MCS_CLK_PHASE_FS);
			if (ret) {
				pr_err("Failed to set clock phase: %d\n", ret);
				return ret;
			}
		}
	}

	return 0;
}
