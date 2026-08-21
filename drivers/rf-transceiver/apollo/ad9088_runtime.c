// SPDX-License-Identifier: GPL-2.0
/*
 * AD9088 runtime control: NCO test tones and datapath loopback
 *
 * Copyright 2026 Analog Devices Inc.
 */

#include "ad9088.h"
#include "no_os_util.h"

#include "adi_apollo_loopback.h"

/*
 * Restore an NCO to the mixer mode requested by the profile.
 */
static adi_apollo_nco_mixer_mode_e
ad9088_cnco_profile_mode(struct ad9088_phy *phy,
			 adi_apollo_terminal_e terminal, uint8_t side,
			 uint8_t cddc_pi)
{
	if (terminal == ADI_APOLLO_TX)
		return phy->profile.tx_path[side].tx_cduc[cddc_pi].nco[0].nco_if_mode;

	return phy->profile.rx_path[side].rx_cddc[cddc_pi].nco[0].nco_if_mode;
}

static adi_apollo_nco_mixer_mode_e
ad9088_fnco_profile_mode(struct ad9088_phy *phy,
			 adi_apollo_terminal_e terminal, uint8_t side,
			 uint8_t fddc_pi)
{
	if (terminal == ADI_APOLLO_TX)
		return phy->profile.tx_path[side].tx_fduc[fddc_pi].nco[0].nco_if_mode;

	return phy->profile.rx_path[side].rx_fddc[fddc_pi].nco[0].nco_if_mode;
}

/*
 * Enable or disable the coarse NCO (CNCO) DC test tone for a given side/CDDC.
 * When enabled the mixer is switched to test mode and offset selects the test
 * tone amplitude. offset is clamped to the datapath maximum.
 */
int ad9088_set_cnco_test_tone(struct ad9088_phy *phy,
			      adi_apollo_terminal_e terminal, uint8_t side,
			      uint8_t cddc_num, bool enable, uint16_t offset)
{
	adi_apollo_nco_mixer_mode_e mode;
	uint16_t offset_max;
	uint32_t mask;
	uint8_t cddc_pi;
	int ret;

	if (!phy || side >= ADI_APOLLO_NUM_SIDES || cddc_num >= 4)
		return -EINVAL;

	mask = cnco_masks[side][cddc_num];
	cddc_pi = cddc_num % ADI_APOLLO_CDUCS_PER_SIDE;
	offset_max = (terminal == ADI_APOLLO_TX) ? 0x1FFF : 0x7FF;

	mode = enable ? ADI_APOLLO_MXR_TEST_MODE :
	       ad9088_cnco_profile_mode(phy, terminal, side, cddc_pi);

	ret = adi_apollo_cnco_mode_set(&phy->ad9088, terminal, mask, mode);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_cnco_mode_set");
	if (ret)
		return ret;

	if (!enable)
		return 0;

	if (offset > offset_max)
		offset = offset_max;

	ret = adi_apollo_cnco_test_mode_val_set(&phy->ad9088, terminal, mask,
						offset);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_cnco_test_mode_val_set");
	if (ret)
		return ret;

	return 0;
}

/*
 * Enable or disable the fine NCO (FNCO) DC test tone for a given side/FDDC.
 * terminal selects TX (FDUC) or RX (FDDC).
 */
int ad9088_set_fnco_test_tone(struct ad9088_phy *phy,
			      adi_apollo_terminal_e terminal, uint8_t side,
			      uint8_t fddc_num, bool enable, uint16_t offset)
{
	adi_apollo_nco_mixer_mode_e mode;
	uint16_t offset_max;
	uint32_t mask;
	uint8_t fddc_pi;
	int ret;

	if (!phy || side >= ADI_APOLLO_NUM_SIDES || fddc_num >= 8)
		return -EINVAL;

	mask = fnco_masks[side][fddc_num];
	fddc_pi = fddc_num % ADI_APOLLO_FDUCS_PER_SIDE;
	offset_max = (terminal == ADI_APOLLO_TX) ? 0x7FFF : 0x1FFF;

	mode = enable ? ADI_APOLLO_MXR_TEST_MODE :
	       ad9088_fnco_profile_mode(phy, terminal, side, fddc_pi);

	ret = adi_apollo_fnco_mode_set(&phy->ad9088, terminal, mask, mode);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_fnco_mode_set");
	if (ret)
		return ret;

	if (!enable)
		return 0;

	if (offset > offset_max)
		offset = offset_max;

	ret = adi_apollo_fnco_test_mode_val_set(&phy->ad9088, terminal, mask,
						offset);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_fnco_test_mode_val_set");
	if (ret)
		return ret;

	return 0;
}

static uint16_t ad9088_lb1_cduc_mask_get(adi_apollo_device_t *device,
		uint8_t side)
{
	if (side == ADI_APOLLO_SIDE_IDX_B) {
		if (device->dev_info.is_8t8r)
			return ADI_APOLLO_CDUC_B0 | ADI_APOLLO_CDUC_B1 |
			       ADI_APOLLO_CDUC_B2 | ADI_APOLLO_CDUC_B3;

		return ADI_APOLLO_CDUC_B0 | ADI_APOLLO_CDUC_B1;
	}

	if (device->dev_info.is_8t8r)
		return ADI_APOLLO_CDUC_A0 | ADI_APOLLO_CDUC_A1 |
		       ADI_APOLLO_CDUC_A2 | ADI_APOLLO_CDUC_A3;

	return ADI_APOLLO_CDUC_A0 | ADI_APOLLO_CDUC_A1;
}

/* Loopback 0: ADC output folded back into the TX datapath before the DACs. */
static int ad9088_device_loopback0(struct ad9088_phy *phy, uint8_t side)
{
	adi_apollo_side_select_e sides = side ? ADI_APOLLO_SIDE_B :
					 ADI_APOLLO_SIDE_A;
	adi_apollo_adc_select_e select_adc = side ? ADI_APOLLO_ADC_B_ALL :
					     ADI_APOLLO_ADC_A_ALL;
	uint16_t xbar[ADI_APOLLO_ADC_PER_SIDE_NUM / 2] = {
		ADI_APOLLO_ADC_0, ADI_APOLLO_ADC_1
	};
	adi_apollo_device_t *device = &phy->ad9088;
	int ret;

	ret = adi_apollo_loopback_lb0_read_ptr_rst_set(device, select_adc, 2);
	ret = ad9088_check_apollo_error(ret,
			"adi_apollo_loopback_lb0_read_ptr_rst_set");
	if (ret)
		return ret;

	ret = adi_apollo_loopback_lb0_write_ptr_rst_set(device, sides, 2);
	ret = ad9088_check_apollo_error(ret,
			"adi_apollo_loopback_lb0_write_ptr_rst_set");
	if (ret)
		return ret;

	ret = adi_apollo_loopback_lb0_tx_xbar_set(device, sides, xbar, 2);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_loopback_lb0_tx_xbar_set");
	if (ret)
		return ret;

	ret = adi_apollo_loopback_lb0_rx_enable_set(device, sides, 1);
	ret = ad9088_check_apollo_error(ret,
			"adi_apollo_loopback_lb0_rx_enable_set");
	if (ret)
		return ret;

	ret = adi_apollo_loopback_lb0_tx_enable_set(device, select_adc, 1);
	ret = ad9088_check_apollo_error(ret,
			"adi_apollo_loopback_lb0_tx_enable_set");
	if (ret)
		return ret;

	phy->loopback_mode[side] = ADI_APOLLO_LOOPBACK_0;

	return 0;
}

/* Loopback 1: TX CDUC output folded back into the RX CDDC datapath. */
static int ad9088_device_loopback1(struct ad9088_phy *phy, uint8_t side)
{
	adi_apollo_side_select_e sides = side ? ADI_APOLLO_SIDE_B :
					 ADI_APOLLO_SIDE_A;
	adi_apollo_device_t *device = &phy->ad9088;
	uint16_t lb1_cducs = ad9088_lb1_cduc_mask_get(device, side);
	int ret;

	ret = adi_apollo_loopback_lb1_enable_set(device, sides, 1);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_loopback_lb1_enable_set");
	if (ret)
		return ret;

	ret = adi_apollo_loopback_lb1_cduc_enable_set(device, lb1_cducs, 1);
	ret = ad9088_check_apollo_error(ret,
			"adi_apollo_loopback_lb1_cduc_enable_set");
	if (ret)
		return ret;

	ret = adi_apollo_loopback_lb1_blend_set(device, lb1_cducs,
						(adi_apollo_loopback_lb1_blend_mode_e)
						phy->lb1_blend[side]);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_loopback_lb1_blend_set");
	if (ret)
		return ret;

	phy->loopback_mode[side] = ADI_APOLLO_LOOPBACK_1;

	return 0;
}

/* Tear down whichever loopback is currently active on this side. */
static int ad9088_device_loopback_disable(struct ad9088_phy *phy, uint8_t side)
{
	adi_apollo_side_select_e sides = side ? ADI_APOLLO_SIDE_B :
					 ADI_APOLLO_SIDE_A;
	adi_apollo_adc_select_e select_adc = side ? ADI_APOLLO_ADC_B_ALL :
					     ADI_APOLLO_ADC_A_ALL;
	adi_apollo_device_t *device = &phy->ad9088;
	uint16_t lb1_cducs = ad9088_lb1_cduc_mask_get(device, side);
	int ret;

	switch (phy->loopback_mode[side]) {
	case ADI_APOLLO_LOOPBACK_0:
		ret = adi_apollo_loopback_lb0_tx_enable_set(device, select_adc,
				0);
		ret = ad9088_check_apollo_error(ret,
				"adi_apollo_loopback_lb0_tx_enable_set");
		if (ret)
			return ret;

		ret = adi_apollo_loopback_lb0_rx_enable_set(device, sides, 0);
		ret = ad9088_check_apollo_error(ret,
				"adi_apollo_loopback_lb0_rx_enable_set");
		if (ret)
			return ret;
		break;
	case ADI_APOLLO_LOOPBACK_1:
		ret = adi_apollo_loopback_lb1_cduc_enable_set(device, lb1_cducs,
				0);
		ret = ad9088_check_apollo_error(ret,
				"adi_apollo_loopback_lb1_cduc_enable_set");
		if (ret)
			return ret;

		ret = adi_apollo_loopback_lb1_enable_set(device, sides, 0);
		ret = ad9088_check_apollo_error(ret,
				"adi_apollo_loopback_lb1_enable_set");
		if (ret)
			return ret;

		ret = adi_apollo_loopback_lb1_blend_set(device, lb1_cducs,
							ADI_APOLLO_LB1_BLEND_DISABLE);
		ret = ad9088_check_apollo_error(ret,
				"adi_apollo_loopback_lb1_blend_set");
		if (ret)
			return ret;
		break;
	default:
		break;
	}

	phy->loopback_mode[side] = ADI_APOLLO_LOOPBACK_NONE;

	return 0;
}

/*
 * Select the datapath loopback mode for one side. Modes 2 (FDUC) and 3 (JESD)
 * are not ported yet. Loopback requires matching ADC and DAC sampling rates.
 */
int ad9088_set_loopback_mode(struct ad9088_phy *phy, uint8_t side, uint8_t mode)
{
	int ret;

	if (!phy || side >= ADI_APOLLO_NUM_SIDES)
		return -EINVAL;

	if (mode != ADI_APOLLO_LOOPBACK_NONE &&
	    phy->profile.dac_cfg[side].dac_sampling_rate_Hz !=
	    phy->profile.adc_cfg[side].adc_sampling_rate_Hz) {
		pr_err("Loopback requires matching ADC and DAC sample rates\n");
		return -EINVAL;
	}

	/* Always tear down the active mode before selecting a new one */
	ret = ad9088_device_loopback_disable(phy, side);
	if (ret)
		return ret;

	switch (mode) {
	case ADI_APOLLO_LOOPBACK_NONE:
		return 0;
	case ADI_APOLLO_LOOPBACK_0:
		return ad9088_device_loopback0(phy, side);
	case ADI_APOLLO_LOOPBACK_1:
		return ad9088_device_loopback1(phy, side);
	default:
		pr_err("Loopback mode %u not supported\n", mode);
		return -EINVAL;
	}
}
