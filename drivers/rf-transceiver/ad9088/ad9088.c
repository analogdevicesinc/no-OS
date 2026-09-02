// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for AD9088 and similar mixed signal front end (MxFE®)
 *
 * Copyright 2022 Analog Devices Inc.
 */

#include "ad9088.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_util.h"
#include <string.h>

#include "adi_cms_api_common.h"
#include "adi_apollo_startup.h"
#include "adi_apollo_device.h"
#include "adi_apollo_mailbox.h"

#define INDIRECT_REG_TEST_ADDR  (0x60366045)
#define ARM_REG_TEST_BASE_ADDR  (0x20000000U)

/**
 * @brief Convert an NCO frequency shift into a frequency tuning word.
 *
 * The tuning word is the shift expressed as a fraction of the NCO clock, scaled
 * to the accumulator width. Whatever the integer word cannot express is
 * returned as the fraction frac_a / frac_b, which the dual-modulus modes use to
 * reach an exact shift.
 *
 * @param phy       - The device structure.
 * @param freq      - Converter sample rate, in Hz.
 * @param nco_shift - Signed frequency shift to encode, in Hz.
 * @param div       - Decimation or interpolation ratio between the converter
 *		      and the NCO, so that freq / div is the NCO clock.
 * @param bits      - Accumulator width of the target NCO, in bits.
 * @param ftw       - Integer frequency tuning word.
 * @param frac_a    - Numerator of the leftover fraction.
 * @param frac_b    - Denominator of the leftover fraction.
 * @return          - 0 in case of success, negative error code otherwise.
 */
int adi_ad9088_calc_nco_ftw(struct ad9088_phy *phy, uint64_t freq,
			    int64_t nco_shift, uint32_t div, uint32_t bits,
			    uint64_t *ftw, uint64_t *frac_a, uint64_t *frac_b)
{
	bool neg = false;
	int ret;
	uint64_t f_clamp = freq;
	int64_t val;

	if (!freq || !bits || bits > 48 || !ftw || !frac_a || !frac_b || !div)
		return -EINVAL;

	f_clamp /= div;

	pr_debug("%s: freq=%llu, nco_shift=%lld, bits=%u\n",
		 __func__, f_clamp, nco_shift, bits);

	val = (int64_t)(f_clamp >> 1);
	nco_shift = no_os_clamp(nco_shift, -val, val);

	if (nco_shift < 0) {
		nco_shift = -nco_shift;
		neg = true;
	}

	ret = adi_api_utils_ratio_decomposition(nco_shift * div, freq, bits,
						ftw, frac_a, frac_b);
	if (ret) {
		pr_err("Error in ratio decomposition: (%d)\n", ret);
		return ret;
	}

	if (bits == 32 && !phy->cnco_dual_modulus_mode_en) {
		*frac_a = 0;
		*frac_b = 1;
	} else if ((bits == 48) && !phy->fnco_dual_modulus_mode_en) {
		*frac_a = 0;
		*frac_b = 1;
	} else if ((bits == 48) && phy->fnco_dual_modulus_mode_en) {
		/* frac_a and frac_b are 24-bit registers */
		while (*frac_a >= (1 << 24) || *frac_b >= (1 << 24)) {
			*frac_a >>= 1;
			*frac_b >>= 1;
		};
	};

	if (neg)
		*ftw = (1ULL << bits) - *ftw;

	pr_debug("%s: ftw=%llx, frac_a=%llu, frac_b=%llu\n",
		 __func__, *ftw, *frac_a, *frac_b);

	return 0;
}

/**
 * @brief Convert a frequency tuning word back into a frequency shift.
 *
 * The inverse of adi_ad9088_calc_nco_ftw(). Tuning words in the upper half of
 * the accumulator range encode a negative shift.
 *
 * @param phy       - The device structure.
 * @param freq      - NCO clock rate, in Hz.
 * @param ftw       - Integer frequency tuning word.
 * @param a         - Numerator of the fractional part.
 * @param b         - Denominator of the fractional part.
 * @param bits      - Accumulator width of the source NCO, in bits.
 * @param nco_shift - Decoded signed frequency shift, in Hz.
 * @return          - 0 in case of success, negative error code otherwise.
 */
int adi_ad9088_calc_nco_freq(struct ad9088_phy *phy, uint64_t freq,
			     uint64_t ftw, uint32_t a, uint32_t b,
			     uint32_t bits, int64_t *nco_shift)
{
	uint64_t hi, lo, mod;
	bool neg = false;

	pr_debug("%s: freq=%llu, ftw=%llu, a=%u, b=%u, bits=%u\n",
		 __func__, freq, ftw, a, b, bits);

	if (!b)
		b = 1;

	if (!freq || !bits || bits > 48 || a > b)
		return -EINVAL;

	mod = (1ULL << bits);

	if (ftw > (mod >> 1)) {
		ftw = mod - ftw;
		neg = true;
	}

	adi_api_utils_mult_128(freq, (ftw * 100ULL) + ((100 * a) / b), &hi, &lo);
	adi_api_utils_add_128(hi, lo, 0, (mod * 100) >> 1, &hi, &lo);
	adi_api_utils_div_128(hi, lo, 0, (mod * 100), &hi,
			      (uint64_t *)nco_shift);

	if (neg)
		*nco_shift *= -1;

	return 0;
}

/**
 * @brief Set the coarse NCO frequency shift.
 *
 * @param phy      - The device structure.
 * @param terminal - Selects the transmit or receive datapath.
 * @param side     - Datapath side index.
 * @param cddc_num - Coarse datapath index within the side.
 * @param freq_hz  - Signed frequency shift, in Hz.
 * @return         - 0 in case of success, negative error code otherwise.
 */
int ad9088_set_cnco_freq(struct ad9088_phy *phy, adi_apollo_terminal_e terminal,
			 uint8_t side, uint8_t cddc_num, int64_t freq_hz)
{
	struct adi_apollo_cnco_cfg *nco;
	uint64_t ftw, frac_a, frac_b, f;
	uint32_t mask;
	uint8_t cddc_pi;
	int ret;

	if (!phy || side >= ADI_APOLLO_NUM_SIDES || cddc_num >= 4)
		return -EINVAL;

	mask = cnco_masks[side][cddc_num];
	cddc_pi = cddc_num % ADI_APOLLO_CDUCS_PER_SIDE;

	if (terminal == ADI_APOLLO_TX)
		f = phy->profile.dac_cfg[side].dac_sampling_rate_Hz;
	else
		f = phy->profile.adc_cfg[side].adc_sampling_rate_Hz;

	ret = adi_ad9088_calc_nco_ftw(phy, f, freq_hz, 1, 32, &ftw,
				      &frac_a, &frac_b);
	if (ret)
		return ret;

	ret = adi_apollo_cnco_ftw_set(&phy->ad9088, terminal, mask, 0, 1, ftw);
	if (ret)
		return ret;

	ret = adi_apollo_cnco_mod_set(&phy->ad9088, terminal, mask,
				      frac_a, frac_b);
	if (ret)
		return ret;

	if (terminal == ADI_APOLLO_TX)
		nco = &phy->profile.tx_path[side].tx_cduc[cddc_pi].nco[0];
	else
		nco = &phy->profile.rx_path[side].rx_cddc[cddc_pi].nco[0];

	nco->nco_phase_inc = ftw;
	nco->nco_phase_inc_frac_a = frac_a;
	nco->nco_phase_inc_frac_b = frac_b;

	return 0;
}

/**
 * @brief Read back the coarse NCO frequency shift.
 *
 * @param phy      - The device structure.
 * @param terminal - Selects the transmit or receive datapath.
 * @param side     - Datapath side index.
 * @param cddc_num - Coarse datapath index within the side.
 * @param freq_hz  - Signed frequency shift, in Hz.
 * @return         - 0 in case of success, negative error code otherwise.
 */
int ad9088_get_cnco_freq(struct ad9088_phy *phy, adi_apollo_terminal_e terminal,
			 uint8_t side, uint8_t cddc_num, int64_t *freq_hz)
{
	struct adi_apollo_cnco_cfg *nco;
	uint8_t cddc_pi;
	uint64_t f;

	if (!phy || !freq_hz || side >= ADI_APOLLO_NUM_SIDES || cddc_num >= 4)
		return -EINVAL;

	cddc_pi = cddc_num % ADI_APOLLO_CDUCS_PER_SIDE;

	if (terminal == ADI_APOLLO_TX) {
		nco = &phy->profile.tx_path[side].tx_cduc[cddc_pi].nco[0];
		f = phy->profile.dac_cfg[side].dac_sampling_rate_Hz;
	} else {
		nco = &phy->profile.rx_path[side].rx_cddc[cddc_pi].nco[0];
		f = phy->profile.adc_cfg[side].adc_sampling_rate_Hz;
	}

	return adi_ad9088_calc_nco_freq(phy, f, nco->nco_phase_inc,
					nco->nco_phase_inc_frac_a,
					nco->nco_phase_inc_frac_b, 32,
					freq_hz);
}

/**
 * @brief Set the fine NCO frequency shift.
 *
 * @param phy      - The device structure.
 * @param terminal - Selects the transmit or receive datapath.
 * @param side     - Datapath side index.
 * @param fddc_num - Fine datapath index within the side.
 * @param freq_hz  - Signed frequency shift, in Hz.
 * @return         - 0 in case of success, negative error code otherwise.
 */
int ad9088_set_fnco_freq(struct ad9088_phy *phy, adi_apollo_terminal_e terminal,
			 uint8_t side, uint8_t fddc_num, int64_t freq_hz)
{
	adi_apollo_fine_nco_main_pgm_t config = {0};
	struct adi_apollo_fnco_cfg *nco;
	uint64_t ftw, frac_a, frac_b, f;
	uint32_t mask, cddc_dcm;
	int64_t fnco_phase;
	uint8_t fddc_pi, cddc_pi;
	int ret;

	if (!phy || side >= ADI_APOLLO_NUM_SIDES || fddc_num >= 8)
		return -EINVAL;

	mask = fnco_masks[side][fddc_num];
	fddc_pi = fddc_num % ADI_APOLLO_FDUCS_PER_SIDE;
	cddc_pi = (fddc_num / 2) % ADI_APOLLO_CDUCS_PER_SIDE;

	if (terminal == ADI_APOLLO_TX) {
		adi_apollo_cduc_ratio_e ratio =
			phy->profile.tx_path[side].tx_cduc[cddc_pi].drc_ratio;

		ret = adi_apollo_cduc_interp_bf_to_val(&phy->ad9088, ratio,
						       &cddc_dcm);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_cduc_interp_bf_to_val");
		f = phy->profile.dac_cfg[side].dac_sampling_rate_Hz;
	} else {
		adi_apollo_cddc_ratio_e ratio =
			phy->profile.rx_path[side].rx_cddc[cddc_pi].drc_ratio;

		ret = adi_apollo_cddc_dcm_bf_to_val(&phy->ad9088, ratio,
						    &cddc_dcm);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_cddc_dcm_bf_to_val");
		f = phy->profile.adc_cfg[side].adc_sampling_rate_Hz;
	}

	/* Handed to calc_nco_ftw as the divisor, so it must be sane. */
	if (ret)
		return ret;

	if (!cddc_dcm) {
		pr_err("Zero decimation ratio for side %u fddc %u\n",
		       side, fddc_num);
		return -EINVAL;
	}

	ret = adi_ad9088_calc_nco_ftw(phy, f, freq_hz, cddc_dcm, 48, &ftw,
				      &frac_a, &frac_b);
	if (ret)
		return ret;

	fnco_phase = phy->fnco_phase[terminal][side][fddc_num];
	config.main_phase_inc = ftw;
	config.main_phase_offset = no_os_div_s64(fnco_phase * 14073748835533,
						 18000LL);
	config.drc_phase_inc_frac_a = frac_a;
	config.drc_phase_inc_frac_b = frac_b;

	ret = adi_apollo_fnco_main_pgm(&phy->ad9088, terminal, mask, &config);
	if (ret)
		return ret;

	if (terminal == ADI_APOLLO_TX)
		nco = &phy->profile.tx_path[side].tx_fduc[fddc_pi].nco[0];
	else
		nco = &phy->profile.rx_path[side].rx_fddc[fddc_pi].nco[0];

	nco->nco_phase_inc = ftw;
	nco->nco_phase_inc_frac_a = frac_a;
	nco->nco_phase_inc_frac_b = frac_b;

	return 0;
}

/**
 * @brief Read back the fine NCO frequency shift.
 *
 * @param phy      - The device structure.
 * @param terminal - Selects the transmit or receive datapath.
 * @param side     - Datapath side index.
 * @param fddc_num - Fine datapath index within the side.
 * @param freq_hz  - Signed frequency shift, in Hz.
 * @return         - 0 in case of success, negative error code otherwise.
 */
int ad9088_get_fnco_freq(struct ad9088_phy *phy, adi_apollo_terminal_e terminal,
			 uint8_t side, uint8_t fddc_num, int64_t *freq_hz)
{
	struct adi_apollo_fnco_cfg *nco;
	uint64_t f;
	uint32_t cddc_dcm;
	uint8_t fddc_pi, cddc_pi;
	int ret;

	if (!phy || !freq_hz || side >= ADI_APOLLO_NUM_SIDES || fddc_num >= 8)
		return -EINVAL;

	fddc_pi = fddc_num % ADI_APOLLO_FDUCS_PER_SIDE;
	cddc_pi = (fddc_num / 2) % ADI_APOLLO_CDUCS_PER_SIDE;

	if (terminal == ADI_APOLLO_TX) {
		nco = &phy->profile.tx_path[side].tx_fduc[fddc_pi].nco[0];

		ret = adi_apollo_cduc_interp_bf_to_val(&phy->ad9088,
			phy->profile.tx_path[side].tx_cduc[cddc_pi].drc_ratio,
			&cddc_dcm);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_cduc_interp_bf_to_val");
		f = phy->profile.dac_cfg[side].dac_sampling_rate_Hz;
	} else {
		nco = &phy->profile.rx_path[side].rx_fddc[fddc_pi].nco[0];

		ret = adi_apollo_cddc_dcm_bf_to_val(&phy->ad9088,
			phy->profile.rx_path[side].rx_cddc[cddc_pi].drc_ratio,
			&cddc_dcm);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_cddc_dcm_bf_to_val");
		f = phy->profile.adc_cfg[side].adc_sampling_rate_Hz;
	}

	if (ret)
		return ret;

	if (!cddc_dcm) {
		pr_err("Zero decimation ratio for side %u fddc %u\n",
		       side, fddc_num);
		return -EINVAL;
	}

	f /= cddc_dcm;

	return adi_ad9088_calc_nco_freq(phy, f, nco->nco_phase_inc,
					nco->nco_phase_inc_frac_a,
					nco->nco_phase_inc_frac_b, 48,
					freq_hz);
}

/**
 * @brief Human-readable name for a vendor API error code.
 *
 * @param error_code - An API_CMS_ERROR_* code.
 * @return           - The matching description, or a placeholder for an unknown
 *		       code.
 */
static const char *adi_cms_error_to_string(int error_code)
{
	switch (error_code) {
	case API_CMS_ERROR_OK:
		return "No Error";
	case API_CMS_ERROR_ERROR:
		return "General Error";
	case API_CMS_ERROR_NULL_PARAM:
		return "Null parameter";
	case API_CMS_ERROR_OVERFLOW:
		return "General overflow";
	case API_CMS_ERROR_DIV_BY_ZERO:
		return "Divide by zero";
	case API_CMS_ERROR_FEAT_LOCKOUT:
		return "Device feature is locked out";
	case API_CMS_ERROR_SPI_SDO:
		return "Wrong SDO value in device structure";
	case API_CMS_ERROR_INVALID_HANDLE_PTR:
		return "Invalid device handler pointer";
	case API_CMS_ERROR_INVALID_XFER_PTR:
		return "Invalid SPI xfer function pointer";
	case API_CMS_ERROR_INVALID_DELAYUS_PTR:
		return "Invalid delay_us function pointer";
	case API_CMS_ERROR_INVALID_PARAM:
		return "Invalid parameter";
	case API_CMS_ERROR_INVALID_RESET_CTRL_PTR:
		return "Invalid reset control function pointer";
	case API_CMS_ERROR_NOT_SUPPORTED:
		return "Not supported";
	case API_CMS_ERROR_INVALID_MASK_SELECT:
		return "Invalid bitmask select";
	case API_CMS_ERROR_IN_REF_STATUS:
		return "Input reference signal not available";
	case API_CMS_ERROR_VCO_OUT_OF_RANGE:
		return "VCO out of range";
	case API_CMS_ERROR_PLL_NOT_LOCKED:
		return "PLL not locked";
	case API_CMS_ERROR_DLL_NOT_LOCKED:
		return "DLL not locked";
	case API_CMS_ERROR_MODE_NOT_IN_TABLE:
		return "JESD mode not in table";
	case API_CMS_ERROR_CLK_CKT:
		return "Clock circuit error";
	case API_CMS_ERROR_FTW_LOAD_ACK:
		return "FTW acknowledge not received";
	case API_CMS_ERROR_NCO_NOT_ENABLED:
		return "NCO not enabled";
	case API_CMS_ERROR_INIT_SEQ_FAIL:
		return "Initialization sequence failed";
	case API_CMS_ERROR_TEST_FAILED:
		return "Test failed";
	case API_CMS_ERROR_SPI_XFER:
		return "SPI transfer error";
	case API_CMS_ERROR_TX_EN_PIN_CTRL:
		return "TX enable function error";
	case API_CMS_ERROR_RESET_PIN_CTRL:
		return "HW reset function error";
	case API_CMS_ERROR_EVENT_HNDL:
		return "Event handling error";
	case API_CMS_ERROR_HW_OPEN:
		return "HW open function error";
	case API_CMS_ERROR_HW_CLOSE:
		return "HW close function error";
	case API_CMS_ERROR_LOG_OPEN:
		return "Log open error";
	case API_CMS_ERROR_LOG_WRITE:
		return "Log write error";
	case API_CMS_ERROR_LOG_CLOSE:
		return "Log close error";
	case API_CMS_ERROR_DELAY_US:
		return "Delay error";
	case API_CMS_ERROR_HSCI_LINK_UP:
		return "HSCI linkup error";
	case API_CMS_ERROR_SPI_REGIO_XFER:
		return "SPI register transaction error";
	case API_CMS_ERROR_HSCI_REGIO_XFER:
		return "HSCI register transaction error";
	case API_CMS_ERROR_OPERATION_TIMEOUT:
		return "Operation timeout";
	case API_CMS_ERROR_LINK_DOWN:
		return "JESD links down";
	case API_CMS_ERROR_FILE_OPEN:
		return "File open error";
	case API_CMS_ERROR_SERDES_CAL_ERROR:
		return "SERDES cal error";
	case API_CMS_ERROR_SERDES_CAL_TIMEOUT:
		return "SERDES cal timeout";
	case API_CMS_ERROR_PLATFORM_READ:
		return "Platform read error";
	case API_CMS_ERROR_PLATFORM_WRITE:
		return "Platform write error";
	case API_CMS_ERROR_FILE_READ:
		return "File read error";
	case API_CMS_ERROR_FILE_WRITE:
		return "File write error";
	case API_CMS_ERROR_FILE_OPERATION:
		return "General file error";
	case API_CMS_ERROR_PLATFORM_IMAGE_LOAD:
		return "Platform FPGA image load error";
	case API_CMS_ERROR_NOT_IMPLEMENTED:
		return "Not implemented";
	case API_CMS_ERROR_STRUCT_UNPOPULATED:
		return "Struct not populated";
	case API_CMS_ERROR_PROTOCOL_OP_NOT_SUPPORTED:
		return "Protocol not supported for operation";
	case API_CMS_ERROR_INVALID_CLK_OR_REF_PARAM:
		return "Invalid clock or reference parameter";
	case API_CMS_ERROR_MEM_ALLOC:
		return "Memory allocation error";
	case API_CMS_ERROR_MMAP:
		return "Memory mapping error";
	case API_CMS_ERROR_DEV_MEM_OPEN:
		return "Device memory open error";
	case API_CMS_ERROR_I2C_ERROR:
		return "I2C error";
	case API_CMS_ERROR_I2C_WRITE:
		return "I2C write failed";
	case API_CMS_ERROR_I2C_READ:
		return "I2C read failed";
	case API_CMS_ERROR_I2C_BUSY:
		return "I2C busy";
	case API_CMS_ERROR_PMOD_NVM_LOCK:
		return "Power module NVM fault";
	case API_CMS_ERROR_EC_RAM_LOCK_ERROR:
		return "EC ram-lock error";
	case API_CMS_ERROR_PROFILE_CRC:
		return "Profile CRC invalid";
	case API_CMS_ERROR_MAILBOX_RESP_STATUS:
		return "Mailbox response status error";
	case API_CMS_ERROR_MCS_CAL_CONFIG_ERROR:
		return "MCS cal configuration error";
	case API_CMS_ERROR_MCS_INIT_CAL_ERROR:
		return "MCS init cal error";
	case API_CMS_ERROR_MCS_TRACK_CAL_ERROR:
		return "MCS tracking cal error";
	case API_CMS_ERROR_MCS_CAL_TIMEOUT:
		return "MCS cal timeout";
	case API_CMS_ERROR_ADC_INIT_CAL_ERROR:
		return "ADC init cal error";
	case API_CMS_ERROR_ADC_TRACK_CAL_ERROR:
		return "ADC tracking cal error";
	case API_CMS_ERROR_ADC_CAL_TIMEOUT:
		return "ADC cal timeout";
	case API_CMS_ERROR_BAD_STATE:
		return "Device in wrong state";
	case API_CMS_ERROR_STARTUP_FW_RDY_FOR_PROFILE_ERROR:
		return "FW not ready for profile config";
	case API_CMS_ERROR_STARTUP_FW_MAILBOX_RDY_ERROR:
		return "FW mailbox not ready";
	case API_CMS_ERROR_PLATFORM_CAPTURE_INVALID_CONFIG:
		return "Invalid platform capture config";
	default:
		return "Unknown error";
	}
}

/**
 * @brief Translate a vendor API return code into an errno value.
 *
 * The vendor API has its own error space and does not log, so every call site
 * funnels through here to get the failure reported once, with the name of the
 * call that produced it.
 *
 * @param ret      - Value returned by the vendor API call.
 * @param api_name - Name of that call, used in the error message.
 * @return         - 0 when the call succeeded, -EIO otherwise.
 */
int ad9088_check_apollo_error(int ret, const char *api_name)
{
	if (ret != API_CMS_ERROR_OK) {
		pr_err("%s failed: %s (%d)\n",
		       api_name, adi_cms_error_to_string(ret), ret);
		return -EIO;
	}

	return 0;
}

/**
 * @brief Map a state-machine link identifier onto a device link identifier.
 *
 * The state machine numbers the transmit and receive links in one sequence,
 * while the device numbers each direction from zero, so the two halves of the
 * range fold onto the same device identifiers.
 *
 * @param linkid - State-machine link identifier.
 * @return       - The corresponding device link identifier.
 */
uint8_t ad9088_to_link(uint8_t linkid)
{
	uint8_t lut[8] = {
		ADI_APOLLO_LINK_A0, ADI_APOLLO_LINK_A1, /* DEFRAMER */
		ADI_APOLLO_LINK_B0, ADI_APOLLO_LINK_B1,
		ADI_APOLLO_LINK_A0, ADI_APOLLO_LINK_A1, /* FRAMER */
		ADI_APOLLO_LINK_B0, ADI_APOLLO_LINK_B1
	};

	return lut[linkid];
}

/**
 * @brief Print the negotiated link parameters of every deframer link.
 *
 * Reports what the device ended up with rather than what was requested, so it
 * can be compared against the peer side.
 *
 * @param phy - The device structure.
 * @return    - 0 in case of success, negative error code otherwise.
 */
int ad9088_inspect_jrx_link_all(struct ad9088_phy *phy)
{
	int err;
	struct adi_apollo_device_t *device = &phy->ad9088;
	adi_apollo_jesd_rx_inspect_t jrx_status;
	uint16_t links_to_inspect[] = {
		ADI_APOLLO_LINK_A0, ADI_APOLLO_LINK_A1,
		ADI_APOLLO_LINK_B0, ADI_APOLLO_LINK_B1
	};
	const char *const links_to_inspect_str[] = { "A0", "A1", "B0", "B1" };
	uint32_t l;

	for (l = 0; l < NO_OS_ARRAY_SIZE(links_to_inspect); l++) {
		err = adi_apollo_jrx_link_inspect(device, links_to_inspect[l],
						  &jrx_status);
		err = ad9088_check_apollo_error(err,
						"adi_apollo_jrx_link_inspect");
		if (err)
			return err;

		pr_info("JRX ADI_APOLLO_LINK_%s: L=%2d M=%2d F=%2d "
			"S=%2d Np=%2d CS=%2d Subclass=%u link_en= %-8s\n",
			links_to_inspect_str[l],
			jrx_status.l_minus1 + 1,
			jrx_status.m_minus1 + 1,
			jrx_status.f_minus1 + 1,
			jrx_status.s_minus1 + 1,
			jrx_status.np_minus1 + 1,
			jrx_status.cs,
			(unsigned int)jrx_status.subclass,
			jrx_status.link_en ? "Enabled" : "Disabled");
	}

	return 0;
}

/**
 * @brief Print the negotiated link parameters of every framer link.
 *
 * Reports what the device ended up with rather than what was requested, so it
 * can be compared against the peer side.
 *
 * @param phy - The device structure.
 * @return    - 0 in case of success, negative error code otherwise.
 */
int ad9088_inspect_jtx_link_all(struct ad9088_phy *phy)
{
	int err;
	struct adi_apollo_device_t *device = &phy->ad9088;
	adi_apollo_jesd_tx_inspect_t jtx_status;
	uint16_t links_to_inspect[] = {
		ADI_APOLLO_LINK_A0, ADI_APOLLO_LINK_A1,
		ADI_APOLLO_LINK_B0, ADI_APOLLO_LINK_B1
	};
	const char *const links_to_inspect_str[] = { "A0", "A1", "B0", "B1" };
	uint32_t l;

	for (l = 0; l < NO_OS_ARRAY_SIZE(links_to_inspect); l++) {
		err = adi_apollo_jtx_link_inspect(device, links_to_inspect[l],
						  &jtx_status);
		err = ad9088_check_apollo_error(err,
						"adi_apollo_jtx_link_inspect");
		if (err)
			return err;

		pr_info("JTX ADI_APOLLO_LINK_%s: L=%2d M=%2d F=%2d "
			"S=%2d Np=%2d CS=%2d Subclass=%u link_en= %-8s\n",
			links_to_inspect_str[l],
			jtx_status.l_minus1 + 1,
			jtx_status.m_minus1 + 1,
			jtx_status.f_minus1 + 1,
			jtx_status.s_minus1 + 1,
			jtx_status.np_minus1 + 1,
			jtx_status.cs,
			(unsigned int)jtx_status.subclass,
			jtx_status.link_en ? "Enabled" : "Disabled");
	}

	return 0;
}

const char *const ad9088_fsm_links_to_str[] = {
	[DEFRAMER_LINK_A0_TX] = "JESD TX (JRX Deframer Link A0)",
	[DEFRAMER_LINK_A1_TX] = "JESD TX (JRX Deframer Link A1)",
	[DEFRAMER_LINK_B0_TX] = "JESD TX (JRX Deframer Link B0)",
	[DEFRAMER_LINK_B1_TX] = "JESD TX (JRX Deframer Link B1)",
	[FRAMER_LINK_A0_RX] = "JESD RX (JTX Framer Link A0)",
	[FRAMER_LINK_A1_RX] = "JESD RX (JTX Framer Link A1)",
	[FRAMER_LINK_B0_RX] = "JESD RX (JTX Framer Link B0)",
	[FRAMER_LINK_B1_RX] = "JESD RX (JTX Framer Link B1)",
};

static const char *const ad9088_jrx_204c_states[] = {
	"Reset", "Undef", "Sync header alignment done",
	"Extended multiblock sync complete",
	"Extended multiblock alignment complete",
	"Undef", "Link is good", "Undef",
};

/**
 * @brief Print the deframer phase difference for one link.
 *
 * @param phy - The device structure.
 * @param lnk - The JESD204 link.
 */
void ad9088_print_link_phase(struct ad9088_phy *phy,
			     struct jesd204_link *lnk)
{
	struct adi_apollo_device_t *device = &phy->ad9088;
	uint8_t id = ad9088_to_link(lnk->link_id);
	uint16_t jrx_phase_diff;

	adi_apollo_jrx_phase_diff_get(device, id, &jrx_phase_diff);
	pr_info("%s Phase Difference %d\n",
		ad9088_fsm_links_to_str[lnk->link_id], jrx_phase_diff);
}

/**
 * @brief Print the captured SYSREF phase several times over.
 *
 * The value is a signed count of device clock periods between the external and
 * internal SYSREF edges. It is sampled repeatedly because a phase that moves
 * between reads means the capture is not deterministic, which a single read
 * would hide.
 *
 * @param phy - The device structure.
 */
void ad9088_print_sysref_phase(struct ad9088_phy *phy)
{
	struct adi_apollo_device_t *device = &phy->ad9088;
	uint32_t sysref_phase;
	int i;

	for (i = 0; i < 5; i++) {
		adi_apollo_clk_mcs_sysref_phase_get(device, &sysref_phase);
		pr_info("SYSREF_PHASE = %ld (TRY%d)\n",
			(long)no_os_sign_extend32(sysref_phase, 9), i);
	}
}

/**
 * @brief Poll a framer link until it reports good, then print its status.
 *
 * Which status bits have to be set depends on the JESD204 version in use. The
 * status is only printed once settled, or on the final attempt, so a link that
 * comes up normally logs a single line.
 *
 * @param phy   - The device structure.
 * @param lnk   - The JESD204 link.
 * @param retry - Number of extra attempts before giving up.
 * @return      - 0 in case of success, negative error code otherwise.
 */
int ad9088_jesd_tx_link_status_print(struct ad9088_phy *phy,
				     struct jesd204_link *lnk, int retry)
{
	int ret;
	uint16_t stat;

	do {
		ret = adi_apollo_jtx_link_status_get(&phy->ad9088,
						     ad9088_to_link(lnk->link_id),
						     &stat);

		if (ret)
			return -EFAULT;

		if (lnk->jesd_version == JESD204_VERSION_C) {
			if ((stat & 0x60) == 0x60)
				ret = 0;
			else
				ret = -EIO;

			if (ret == 0 || retry == 0)
				pr_info("%s Link%d 204C PLL %s, PHASE %s, MODE %s\n",
					ad9088_fsm_links_to_str[lnk->link_id],
					(int)lnk->link_id,
					stat & NO_OS_BIT(5) ? "locked" : "unlocked",
					stat & NO_OS_BIT(6) ? "established" : "lost",
					stat & NO_OS_BIT(7) ? "invalid" : "valid");
			else
				no_os_mdelay(20);
		} else {
			if ((stat & 0xF0) == 0x70)
				ret = 0;
			else
				ret = -EIO;

			if (ret == 0 || retry == 0)
				pr_info("%s Link%d 204B SYNC %s, PLL %s, PHASE %s, MODE %s, STAT 0x%X\n",
					ad9088_fsm_links_to_str[lnk->link_id],
					(int)lnk->link_id,
					stat & NO_OS_BIT(4) ? "deasserted" : "asserted",
					stat & NO_OS_BIT(5) ? "locked" : "unlocked",
					stat & NO_OS_BIT(6) ? "established" : "lost",
					stat & NO_OS_BIT(7) ? "invalid" : "valid",
					stat);
			else
				no_os_mdelay(20);
		}
	} while (ret && retry--);

	return ret;
}

/**
 * @brief Poll a deframer link until it reports good, then print its status.
 *
 * Which status bits have to be set depends on the JESD204 version and, for
 * subclass 1, additionally on the SYSREF phase having locked. Per-lane state is
 * printed alongside the link state so a single bad lane is visible.
 *
 * @param phy   - The device structure.
 * @param lnk   - The JESD204 link.
 * @param retry - Number of extra attempts before giving up.
 * @return      - 0 in case of success, negative error code otherwise.
 */
int ad9088_jesd_rx_link_status_print(struct ad9088_phy *phy,
				     struct jesd204_link *lnk, int retry)
{
	adi_apollo_jesd_rx_cfg_t *jrx =
		&phy->profile.jrx[(lnk->link_id / 2) & 1];
	adi_apollo_jesd_rx_link_cfg_t *rl =
		&jrx->rx_link_cfg[(lnk->link_id % 2) & 1];
	int ret, i, err;
	uint16_t stat, l_stat, mask;
	uint8_t id = ad9088_to_link(lnk->link_id);

	do {
		ret = adi_apollo_jrx_link_status_get(&phy->ad9088, id, &stat);
		if (ret)
			return -EFAULT;

		if (lnk->jesd_version == JESD204_VERSION_C) {
			if (jrx->common_link_cfg.subclass)
				mask = 0x60; /* Subclass 1 */
			else
				mask = 0x20; /* Ignore SYSREF Phase */

			if ((stat & mask) == mask)
				ret = 0;
			else
				ret = -EIO;

			if (ret == 0 || retry == 0) {
				for (i = 0; i < lnk->num_lanes; i++) {
					uint8_t phys_lane = rl->lane_xbar[i];

					err = adi_apollo_jrx_j204c_lane_status_get(&phy->ad9088,
							id, phys_lane, &l_stat);
					if (err)
						return -EFAULT;
					if ((l_stat & 0x7) == 0x6)
						pr_info("%s Link%d 204C Lane-%d@%d status: %s\n",
							ad9088_fsm_links_to_str[lnk->link_id],
							(int)lnk->link_id, i, phys_lane,
							ad9088_jrx_204c_states[l_stat & 0x7]);
					else
						pr_err("%s Link%d 204C Lane-%d@%d status: %s\n",
						       ad9088_fsm_links_to_str[lnk->link_id],
						       (int)lnk->link_id, i, phys_lane,
						       ad9088_jrx_204c_states[l_stat & 0x7]);
				}

				pr_info("%s Link%d 204C User status: %s, SYSREF Phase: %s\n",
					ad9088_fsm_links_to_str[lnk->link_id],
					(int)lnk->link_id,
					(stat & 0x20) ? "Ready" : "Fail",
					(stat & 0x40) ? "Locked" : "Unlocked");
			} else {
				no_os_mdelay(20);
			}
		} else {
			if (jrx->common_link_cfg.subclass)
				mask = 0x60; /* Subclass 1 */
			else
				mask = 0x20; /* Ignore SYSREF Phase */

			if ((stat & mask) == mask)
				ret = 0;
			else
				ret = -EIO;

			if (ret == 0 || retry == 0) {
				for (i = 0; i < lnk->num_lanes; i++) {
					uint8_t phys_lane = rl->lane_xbar[i];

					err = adi_apollo_jrx_j204b_lane_status_get(&phy->ad9088,
							id, phys_lane, &l_stat);
					if (err)
						return -EFAULT;

					if ((l_stat & 0x3C) == 0x38)
						pr_info("%s Link%d 204B Lane-%d@%d status: Link is good (0x%X)\n",
							ad9088_fsm_links_to_str[lnk->link_id],
							(int)lnk->link_id, i, phys_lane, l_stat);
					else
						pr_err("%s Link%d 204B Lane-%d@%d "
						       "status: 0x%X Frame Sync:%s "
						       "SYNC:%s DATA:%s Checksum:%s\n",
						       ad9088_fsm_links_to_str[lnk->link_id],
						       (int)lnk->link_id, i, phys_lane, l_stat & 0x3C,
						       l_stat & NO_OS_BIT(2) ? "Lost" : "Found",
						       l_stat & NO_OS_BIT(3) ? "Ok" : "Fail",
						       l_stat & NO_OS_BIT(4) ? "Ready" : "Fail",
						       l_stat & NO_OS_BIT(5) ? "Good" : "Bad");
				}

				pr_info("%s Link%d 204B User status: %s, SYSREF Phase: %s\n",
					ad9088_fsm_links_to_str[lnk->link_id],
					(int)lnk->link_id,
					(stat & 0x20) ? "Ready" : "Fail",
					(stat & 0x40) ? "Locked" : "Unlocked");
			} else {
				no_os_mdelay(20);
			}
		}
	} while (ret && retry--);

	return ret;
}

struct fw_entry {
	const uint8_t *start;
	const uint8_t *end;
};

static const struct fw_entry fw_table[ADI_APOLLO_FW_ID_MAX] = {
#ifndef CONFIG_AD9088_FW_SET_PROD	/* engineering set: embedded unless PROD-only */
	[ADI_APOLLO_FW_ID_SECR_BOOT_HDR_BIN] = AD9088_FW_ENTRY(
		app_signed_encrypted_B_flash_image_0x01030000_bin),
	[ADI_APOLLO_FW_ID_CORE_0_TYE_FW_BIN] = AD9088_FW_ENTRY(
		app_signed_encrypted_B_flash_image_0x20000000_bin),
	[ADI_APOLLO_FW_ID_CORE_1_TYE_FW_BIN] = AD9088_FW_ENTRY(
		app_signed_encrypted_B_flash_image_0x02000000_bin),
	[ADI_APOLLO_FW_ID_TYE_OPER_FW_BIN] = AD9088_FW_ENTRY(
		app_signed_encrypted_B_flash_image_0x21000000_bin),
#endif
#ifndef CONFIG_AD9088_FW_SET_ENG	/* production set: embedded unless ENG-only */
	[ADI_APOLLO_FW_ID_PROD_SECR_BOOT_HDR_BIN] = AD9088_FW_ENTRY(
		app_signed_encrypted_prod_B_flash_image_0x01030000_bin),
	[ADI_APOLLO_FW_ID_PROD_CORE_0_TYE_FW_BIN] = AD9088_FW_ENTRY(
		app_signed_encrypted_prod_B_flash_image_0x20000000_bin),
	[ADI_APOLLO_FW_ID_PROD_CORE_1_TYE_FW_BIN] = AD9088_FW_ENTRY(
		app_signed_encrypted_prod_B_flash_image_0x02000000_bin),
	[ADI_APOLLO_FW_ID_PROD_TYE_OPER_FW_BIN] = AD9088_FW_ENTRY(
		app_signed_encrypted_prod_B_flash_image_0x21000000_bin),
#endif
};

/**
 * @brief Vendor API callback: hand out one of the linked-in firmware images.
 *
 * The images are linked into the binary rather than read from storage, so this
 * just resolves an identifier to the symbols bounding that image.
 *
 * @param obj        - The firmware provider, unused.
 * @param fw_id      - Identifies which image is wanted.
 * @param byte_arr   - Set to the start of the image.
 * @param bytes_read - Set to the length of the image, in bytes.
 * @return           - 0 on success, an API_CMS_ERROR_* code otherwise.
 */
static int ad9088_fw_provider_get(adi_apollo_fw_provider_t *obj,
				  adi_apollo_startup_fw_id_e fw_id,
				  uint8_t **byte_arr, uint32_t *bytes_read)
{
	if (fw_id >= ADI_APOLLO_FW_ID_MAX || !fw_table[fw_id].start) {
		pr_err("Unknown firmware ID %d\n", fw_id);
		return API_CMS_ERROR_INVALID_PARAM;
	}

	*byte_arr = (uint8_t *)fw_table[fw_id].start;
	*bytes_read = fw_table[fw_id].end - fw_table[fw_id].start;

	return API_CMS_ERROR_OK;
}

/**
 * @brief Vendor API callback: release a firmware image.
 *
 * Nothing to release, since the images are linked in rather than allocated.
 *
 * @param obj   - The firmware provider, unused.
 * @param fw_id - Identifies which image is being released, unused.
 * @return      - 0 on success, an API_CMS_ERROR_* code otherwise.
 */
static int ad9088_fw_provider_close(adi_apollo_fw_provider_t *obj,
				    adi_apollo_startup_fw_id_e fw_id)
{
	return API_CMS_ERROR_OK;
}


/**
 * @brief Vendor API callback: full-duplex SPI transfer.
 *
 * The underlying transfer is in place, so the received bytes are copied out of
 * the write buffer afterwards.
 *
 * @param dev_obj - The device structure.
 * @param wbuf    - Bytes to send.
 * @param rbuf    - Bytes received.
 * @param len     - Transfer length, in bytes.
 * @return        - 0 in case of success, negative error code otherwise.
 */
static int ad9088_spi_xfer(void *dev_obj, uint8_t *wbuf, uint8_t *rbuf,
			   uint32_t len)
{
	struct ad9088_phy *phy = dev_obj;
	int ret;

	ret = no_os_spi_write_and_read(phy->spi, wbuf, len);
	if (ret) {
		pr_err("SPI transfer failed: %d\n", ret);
		return ret;
	}

	memcpy(rbuf, wbuf, len);

	return 0;
}

/**
 * @brief Vendor API callback: SPI read.
 *
 * The caller's buffers are const and may alias, so the transfer runs through a
 * local buffer.
 *
 * @param dev_obj         - The device structure.
 * @param tx_data         - Bytes to send, carrying the address.
 * @param rx_data         - Bytes received.
 * @param num_tx_rx_bytes - Transfer length, in bytes.
 * @param txn_config      - Vendor transaction configuration, unused.
 * @return                - 0 in case of success, negative error code otherwise.
 */
static int ad9088_spi_read(void *dev_obj, const uint8_t tx_data[],
			   uint8_t rx_data[], uint32_t num_tx_rx_bytes,
			   adi_apollo_hal_txn_config_t *txn_config)
{
	struct ad9088_phy *phy = dev_obj;
	uint8_t buf[16];
	int ret;

	if (num_tx_rx_bytes > sizeof(buf))
		return -EINVAL;

	memcpy(buf, tx_data, num_tx_rx_bytes);
	ret = no_os_spi_write_and_read(phy->spi, buf, num_tx_rx_bytes);
	if (ret) {
		pr_err("SPI transfer failed: %d\n", ret);
		return ret;
	}

	memcpy(rx_data, buf, num_tx_rx_bytes);

	return 0;
}

/**
 * @brief Vendor API callback: SPI write.
 *
 * The caller's buffer is const and the transfer is in place, so it runs through
 * a local buffer.
 *
 * @param dev_obj      - The device structure.
 * @param tx_data      - Bytes to send.
 * @param num_tx_bytes - Transfer length, in bytes.
 * @param txn_config   - Vendor transaction configuration, unused.
 * @return             - 0 in case of success, negative error code otherwise.
 */
static int32_t ad9088_spi_write(void *dev_obj, const uint8_t tx_data[],
			    uint32_t num_tx_bytes, adi_apollo_hal_txn_config_t *txn_config)
{
	struct ad9088_phy *phy = dev_obj;
	uint8_t buf[16];

	if (num_tx_bytes > sizeof(buf))
		return -EINVAL;

	memcpy(buf, tx_data, num_tx_bytes);

	return no_os_spi_write_and_read(phy->spi, buf, num_tx_bytes);
}

/**
 * @brief Vendor API callback: drive the reset pin.
 *
 * @param user_data - The device structure.
 * @param enable    - Level to drive on the pin.
 * @return          - 0 in case of success, negative error code otherwise.
 */
static int ad9088_reset_pin_ctrl(void *user_data, uint8_t enable)
{
	struct ad9088_phy *phy = user_data;

	return no_os_gpio_set_value(phy->reset_gpio, enable);
}

/**
 * @brief Vendor API callback: busy-wait.
 *
 * @param user_data - The device structure, unused.
 * @param us        - Delay, in microseconds.
 * @return          - 0 in case of success, negative error code otherwise.
 */
static int ad9088_udelay(void *user_data, unsigned int us)
{
	//us = us * 2;
	no_os_udelay(us);
	return 0;
}

/**
 * @brief Vendor API callback: emit a log message.
 *
 * Maps the vendor severity onto the local log levels. Register-level tracing is
 * dropped before formatting, since it is high volume and the format cost would
 * be paid even when the message is discarded.
 *
 * @param user_data - The device structure, unused.
 * @param log_type  - Vendor log severity.
 * @param message   - Format string.
 * @param argp      - Arguments for the format string.
 * @return          - 0 in case of success, negative error code otherwise.
 */
int ad9088_log_write(void *user_data, int32_t log_type, const char *message,
		     va_list argp)
{
	char logMessage[160];

	if (log_type == ADI_CMS_LOG_SPI)
		return 0;

	vsnprintf(logMessage, sizeof(logMessage), message, argp);

	switch (log_type) {
	case ADI_CMS_LOG_NONE:
		break;
	case ADI_CMS_LOG_MSG:
		pr_info("%s\n", logMessage);
		break;
	case ADI_CMS_LOG_WARN:
		pr_warning("%s\n", logMessage);
		break;
	case ADI_CMS_LOG_ERR:
		pr_err("%s\n", logMessage);
		break;
	case ADI_CMS_LOG_SPI:
		pr_debug("%s\n", logMessage);
		break;
	case ADI_CMS_LOG_API:
		pr_debug("%s\n", logMessage);	/* API function-entry trace (LOG_DEBUG only) */
		break;
	case ADI_CMS_LOG_ALL:
		pr_notice("%s\n", logMessage);
		break;
	}

	return 0;
}

/**
 * @brief Check that the device is reachable and its buses work.
 *
 * Writes and reads back known patterns over the direct register interface, the
 * indirect one and the processor memory window. Run before firmware is loaded,
 * so that a wiring or bus-configuration fault is reported as such instead of
 * surfacing later as an unexplained startup failure.
 *
 * @param device - The vendor API device structure.
 * @return       - 0 on success, an API_CMS_ERROR_* code otherwise.
 */
static int ad9088_reg_test(adi_apollo_device_t *device)
{
	int32_t err;
	uint32_t i, data32;
	uint8_t data8, stat;
	adi_apollo_hal_protocol_e protocol;

	const uint32_t direct_addr[] = {
		0x4700000a, 0x4700000a, 0x47000200, 0x47000200
	};
	const uint8_t direct_data[] = { 0x55, 0xaa, 0xcc, 0x33 };

	const uint32_t indirect_addr[] = {
		INDIRECT_REG_TEST_ADDR + 0, INDIRECT_REG_TEST_ADDR + 1,
		INDIRECT_REG_TEST_ADDR + 2, INDIRECT_REG_TEST_ADDR + 3
	};
	const uint8_t indirect_data[] = { 0x12, 0x34, 0x56, 0x78 };

	/* ARM core1 register addresses */
	const uint32_t arm_addr[] = {
		ARM_REG_TEST_BASE_ADDR + 0, ARM_REG_TEST_BASE_ADDR + 4,
		ARM_REG_TEST_BASE_ADDR + 8, ARM_REG_TEST_BASE_ADDR + 12
	};
	const uint32_t arm_data[] = {
		0x55aa55aa, 0xdeadbeef, 0xbeefdead, 0xaa55aa55
	};

	adi_apollo_hal_active_protocol_get(device, &protocol);

	/* Direct register SPI scratch loop rd/wr test */
	stat = 0;
	for (i = 0; i < NO_OS_ARRAY_SIZE(direct_addr); i++) {

		if (protocol == ADI_APOLLO_HAL_PROTOCOL_HSCI &&
		    direct_addr[i] <= 0x4700000Fu)
			continue;

		err = adi_apollo_hal_reg_set(device, direct_addr[i], direct_data[i]);
		if (err != API_CMS_ERROR_OK)
			return err;
		err = adi_apollo_hal_reg_get(device, direct_addr[i], &data8);
		if (err != API_CMS_ERROR_OK)
			return err;

		if (data8 != direct_data[i]) {
			pr_err("data8 0x%X != direct_data[i] 0x%X\n", data8, direct_data[i]);
			stat = 1;
		}
	}

	if (stat != 0) {
		pr_err("Test direct register *** FAILED ***\n");
		return API_CMS_ERROR_ERROR;
	}
	pr_info("Test direct register Passed\n");

	/* Indirect register SPI loop rd/wr test */
	stat = 0;
	for (i = 0; i < NO_OS_ARRAY_SIZE(indirect_addr); i++) {
		err = adi_apollo_hal_reg_set(device, indirect_addr[i],
					     indirect_data[i]);
		if (err != API_CMS_ERROR_OK)
			return err;
		err = adi_apollo_hal_reg_get(device, indirect_addr[i], &data8);
		if (err != API_CMS_ERROR_OK)
			return err;

		if (data8 != indirect_data[i]) {
			pr_err("Test indirect register 0x%X - 0x%X\n", data8, indirect_data[i]);
			stat = 1;
		}
	}

	if (stat != 0) {
		pr_err("Test indirect register *** FAILED ***\n");
		return API_CMS_ERROR_ERROR;
	}
	pr_info("Test indirect register Passed\n");

	/* 32-bit ARM mem rd/wr test */
	stat = 0;
	for (i = 0; i < NO_OS_ARRAY_SIZE(arm_addr); i++) {
		err = adi_apollo_hal_reg32_set(device, arm_addr[i], arm_data[i]);
		if (err != API_CMS_ERROR_OK)
			return err;
		err = adi_apollo_hal_reg32_get(device, arm_addr[i], &data32);
		if (err != API_CMS_ERROR_OK)
			return err;


		if (data32 != arm_data[i]) {
			pr_err("Test ARM memory 0x%X - 0x%X\n", data32, arm_data[i]);
			stat = 1;
		} else {
			stat = 0;
		}
	}

	if (stat != 0) {
		pr_err("Test ARM memory *** FAILED ***\n");
		return API_CMS_ERROR_ERROR;
	}
	pr_info("Test ARM memory Passed\n");

	return API_CMS_ERROR_OK;
}

/**
 * @brief Print the API and firmware versions, and confirm the firmware
 * responds.
 *
 * The running firmware is pinged as well as queried, so a firmware that loaded
 * but is not servicing requests is caught here.
 *
 * @param phy - The device structure.
 * @return    - 0 in case of success, negative error code otherwise.
 */
static int ad9088_version_info(struct ad9088_phy *phy)
{
	adi_apollo_mailbox_resp_get_fw_version_t fw_ver;
	adi_apollo_mailbox_cmd_ping_t ping_cmd;
	adi_apollo_mailbox_resp_ping_t ping_resp;
	adi_apollo_device_t *device = &phy->ad9088;
	int ret;
	uint16_t maj, min, rc;

	ret = adi_apollo_device_api_revision_get(device, &maj, &min, &rc);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_device_api_revision_get");
	if (ret)
		return ret;

	pr_info("API ver: %d.%d.%d\n", maj, min, rc);

	ret = adi_apollo_mailbox_ready_check(device);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_mailbox_ready_check");
	if (ret)
		return ret;

	ping_cmd.echo_data = 0x00000000;
	ret = adi_apollo_mailbox_ping(device, &ping_cmd, &ping_resp);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_mailbox_ping");
	if (ret)
		return ret;

	pr_info("Ping (core 1) cmd/res: 0x%08x/0x%08x\n",
		(unsigned int)ping_cmd.echo_data,
		(unsigned int)ping_resp.echo_data);

	ping_cmd.echo_data = 0x12345678;
	ret = adi_apollo_mailbox_ping(device, &ping_cmd, &ping_resp);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_mailbox_ping");
	if (ret)
		return ret;

	pr_info("Ping (core 0) cmd/res: 0x%08x/0x%08x\n",
		(unsigned int)ping_cmd.echo_data,
		(unsigned int)ping_resp.echo_data);

	ret = adi_apollo_mailbox_get_fw_version(device, &fw_ver);
	ret = ad9088_check_apollo_error(ret,
					"adi_apollo_mailbox_get_fw_version");
	if (ret)
		return ret;

	pr_info("FW ver: %d.%d.%d\n", fw_ver.major, fw_ver.minor,
		fw_ver.patch);

	return 0;
}

/**
 * @brief Initialize the device.
 *
 * Allocates the device structure, wires up the vendor API to the SPI and GPIO
 * descriptors, verifies the buses, loads the firmware and device profile, and
 * registers the device with the JESD204 state machine. The links themselves are
 * brought up by that state machine, not here.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initialisation
 *		       parameters.
 * @return           - 0 in case of success, negative error code otherwise.
 */
int ad9088_init(struct ad9088_phy **device,
		const struct ad9088_init_param *init_param)
{
	adi_apollo_device_spi_settings_t *spi_cfg;
	struct ad9088_jesd204_priv *priv;
	struct ad9088_phy *phy;
	uint16_t api_rev[3];
	int ret;

	ret = ad9088_parse_struct(&phy, init_param);
	if (ret)
		return ret;

	phy->ad9088.hal_info.spi0_desc.spi_config.sdo = (phy->spi_3wire_en) ?
			ADI_APOLLO_DEVICE_SPI_SDIO : ADI_APOLLO_DEVICE_SPI_SDO;
	spi_cfg = &phy->ad9088.hal_info.spi0_desc.spi_config;
	spi_cfg->msb = ADI_APOLLO_DEVICE_SPI_MSB_FIRST;
	spi_cfg->addr_inc = ADI_APOLLO_DEVICE_SPI_ADDR_INC_AUTO;
	phy->ad9088.hal_info.spi0_desc.is_used = 1;
	phy->ad9088.hal_info.spi0_desc.dev_obj = phy;
	phy->ad9088.hal_info.user_data = phy;
	phy->ad9088.hal_info.spi0_desc.read = ad9088_spi_read;
	phy->ad9088.hal_info.spi0_desc.write = ad9088_spi_write;
	phy->ad9088.hal_info.spi0_desc.xfer = ad9088_spi_xfer;
	phy->ad9088.hal_info.delay_us = ad9088_udelay;
	phy->ad9088.hal_info.reset_pin_ctrl = ad9088_reset_pin_ctrl;
	phy->ad9088.hal_info.log_write = ad9088_log_write;

	phy->fw_provider.desc = "no-OS Embedded FW Provider";
	phy->fw_provider.tag = phy;
	phy->ad9088.startup_info.fw_provider = &phy->fw_provider;
	phy->ad9088.startup_info.get = ad9088_fw_provider_get;
	phy->ad9088.startup_info.close = ad9088_fw_provider_close;
	phy->ad9088.startup_info.open = NULL;

	ret = adi_apollo_device_hw_open(&phy->ad9088,
					phy->reset_gpio ? ADI_APOLLO_HARD_RESET_AND_INIT :
					ADI_APOLLO_SOFT_RESET_AND_INIT);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_device_hw_open");
	if (ret)
		goto error;

	ret = adi_apollo_hal_active_protocol_set(&phy->ad9088,
						 ADI_APOLLO_HAL_PROTOCOL_SPI0);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_hal_active_protocol_set");
	if (ret)
		goto error_hw_close;

	ret = adi_apollo_hal_rmw_enable_set(&phy->ad9088,
					    ADI_APOLLO_HAL_PROTOCOL_SPI0, 0);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_hal_rmw_enable_set");
	if (ret)
		goto error_hw_close;

	ret = ad9088_reg_test(&phy->ad9088);
	if (ret) {
		pr_err("Register test failed (%d)\n", ret);
		ret = -EIO;
		goto error_hw_close;
	}

	/*
	 * For non-8t8r (4t4r) profiles the ADC slice mode-switch must be enabled
	 * before device startup. This mirrors the Linux ad9088 driver, which
	 * performs this write for every non-8t8r profile prior to
	 * adi_apollo_startup_execute().
	 */
	if (!phy->profile.profile_cfg.is_8t8r) {
		ret = adi_apollo_adc_mode_switch_enable_set(&phy->ad9088, 1);
		ret = ad9088_check_apollo_error(ret,
						"adi_apollo_adc_mode_switch_enable_set");
		if (ret)
			goto error_hw_close;
	}

	ret = adi_apollo_startup_execute(&phy->ad9088, &phy->profile,
					 ADI_APOLLO_STARTUP_SEQ_DEFAULT);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_startup_execute");
	if (ret)
		goto error_hw_close;

	ret = ad9088_version_info(phy);
	if (ret)
		goto error_hw_close;

	ret = adi_apollo_device_chip_id_get(&phy->ad9088, &phy->chip_id);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_device_chip_id_get");
	if (ret)
		goto error_hw_close;

	adi_apollo_device_api_revision_get(&phy->ad9088,
					   &api_rev[0], &api_rev[1],
					   &api_rev[2]);

	pr_info("AD%X Rev. %u Grade %u (API %u.%u.%u) probed\n",
		phy->chip_id.prod_id, phy->chip_id.dev_revision,
		phy->chip_id.prod_grade, api_rev[0], api_rev[1],
		api_rev[2]);

	ret = jesd204_dev_register(&phy->jdev, &jesd204_ad9088_init);
	if (ret)
		goto error_hw_close;

	priv = jesd204_dev_priv(phy->jdev);
	priv->phy = phy;

	*device = phy;

	return 0;

error_hw_close:
	adi_apollo_device_hw_close(&phy->ad9088);
error:
	no_os_gpio_remove(phy->reset_gpio);
	no_os_spi_remove(phy->spi);
	no_os_free(phy);

	return ret;
}

/**
 * @brief Free the resources allocated by ad9088_init().
 *
 * @param phy - The device structure.
 * @return    - 0 in case of success, negative error code otherwise.
 */
int ad9088_remove(struct ad9088_phy *phy)
{
	if (!phy)
		return -EINVAL;

	jesd204_dev_unregister(phy->jdev);
	adi_apollo_device_hw_close(&phy->ad9088);

	no_os_gpio_remove(phy->triq_req_gpio);
	no_os_gpio_remove(phy->reset_gpio);
	no_os_spi_remove(phy->spi);
	no_os_free(phy);

	return 0;
}
