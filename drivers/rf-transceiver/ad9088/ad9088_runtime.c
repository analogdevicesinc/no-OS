/***************************************************************************//**
 *   @file   ad9088_runtime.c
 *   @brief  Runtime control (NCO test tones, loopback) for AD9088.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "ad9088.h"
#include "no_os_util.h"

#include "adi_apollo_loopback.h"

/**
 * @brief Coarse NCO mixer mode requested by the device profile.
 *
 * Used to put a coarse NCO back the way the profile configured it once a test
 * tone is switched off.
 *
 * @param phy      - The device structure.
 * @param terminal - Selects the transmit or receive datapath.
 * @param side     - Datapath side index.
 * @param cddc_pi  - Coarse datapath index within the side.
 * @return         - The mixer mode the profile asks for.
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

/**
 * @brief Fine NCO mixer mode requested by the device profile.
 *
 * Used to put a fine NCO back the way the profile configured it once a test
 * tone is switched off.
 *
 * @param phy      - The device structure.
 * @param terminal - Selects the transmit or receive datapath.
 * @param side     - Datapath side index.
 * @param fddc_pi  - Fine datapath index within the side.
 * @return         - The mixer mode the profile asks for.
 */
static adi_apollo_nco_mixer_mode_e
ad9088_fnco_profile_mode(struct ad9088_phy *phy,
			 adi_apollo_terminal_e terminal, uint8_t side,
			 uint8_t fddc_pi)
{
	if (terminal == ADI_APOLLO_TX)
		return phy->profile.tx_path[side].tx_fduc[fddc_pi].nco[0].nco_if_mode;

	return phy->profile.rx_path[side].rx_fddc[fddc_pi].nco[0].nco_if_mode;
}

/**
 * @brief Enable or disable the coarse NCO DC test tone.
 *
 * While the tone is enabled the mixer runs in test mode and emits a DC offset
 * instead of mixing; disabling it restores the mixer mode the device profile
 * asks for.
 *
 * @param phy      - The device structure.
 * @param terminal - Selects the transmit or receive datapath.
 * @param side     - Datapath side index.
 * @param cddc_num - Coarse datapath index within the side.
 * @param enable   - True to emit the tone, false to restore normal mixing.
 * @param offset   - Tone amplitude as a raw DC offset, clamped to the maximum
 *		     the datapath accepts.
 * @return         - 0 in case of success, negative error code otherwise.
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

/**
 * @brief Enable or disable the fine NCO DC test tone.
 *
 * While the tone is enabled the mixer runs in test mode and emits a DC offset
 * instead of mixing; disabling it restores the mixer mode the device profile
 * asks for.
 *
 * @param phy      - The device structure.
 * @param terminal - Selects the transmit or receive datapath.
 * @param side     - Datapath side index.
 * @param fddc_num - Fine datapath index within the side.
 * @param enable   - True to emit the tone, false to restore normal mixing.
 * @param offset   - Tone amplitude as a raw DC offset, clamped to the maximum
 *		     the datapath accepts.
 * @return         - 0 in case of success, negative error code otherwise.
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

/**
 * @brief Block-select mask covering every coarse interpolator on one side.
 *
 * How many there are depends on the transmit channel count the device reports.
 *
 * @param device - The vendor API device structure.
 * @param side   - Datapath side index.
 * @return       - Mask of the coarse interpolators on that side.
 */
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

/**
 * @brief Fold the converter output back into the transmit datapath.
 *
 * Takes the digitised receive data ahead of the receive datapath and feeds it
 * into the transmit datapath just before the converters, so the whole digital
 * chain is bypassed.
 *
 * @param phy  - The device structure.
 * @param side - Datapath side index.
 * @return     - 0 in case of success, negative error code otherwise.
 */
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

/**
 * @brief Fold the coarse interpolator output back into the coarse decimators.
 *
 * Keeps both digital datapaths in circuit, so the transmit signal reaches the
 * receive side already interpolated. The blend mode selects how the looped-back
 * signal is combined with the live one.
 *
 * @param phy  - The device structure.
 * @param side - Datapath side index.
 * @return     - 0 in case of success, negative error code otherwise.
 */
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

/**
 * @brief Tear down whichever loopback is currently active on one side.
 *
 * Does nothing when no loopback is active, so it is safe to call
 * unconditionally.
 *
 * @param phy  - The device structure.
 * @param side - Datapath side index.
 * @return     - 0 in case of success, negative error code otherwise.
 */
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

/**
 * @brief Select the datapath loopback mode for one side.
 *
 * Any active loopback is torn down first, so switching modes needs a single
 * call. A loopback only makes sense when the two directions run at the same
 * sample rate, which is checked here.
 *
 * @param phy  - The device structure.
 * @param side - Datapath side index.
 * @param mode - One of the ADI_APOLLO_LOOPBACK_* modes.
 * @return     - 0 in case of success, negative error code otherwise.
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
