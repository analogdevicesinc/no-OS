/*!
 * @brief     APIs for SYSREF configuration and control
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9081_SYSREF_API
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"

/*============= C O D E ====================*/

int32_t adi_ad9081_jesd_sysref_enable_set(adi_ad9081_device_t *device,
					  uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* Power down the sysref receiver and sync circuitry. */
	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_CTRL_ADDR,
				    BF_SYSREF_PD_INFO, !enable); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_sysref_d2acenter_enable_set(adi_ad9081_device_t *device,
						    uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_SPI_ENABLE_DAC_ADDR,
				    BF_SPI_EN_D2ACENTER_INFO, enable);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_sysref_input_mode_set(
	adi_ad9081_device_t *device, uint8_t enable_receiver,
	uint8_t enable_capture, adi_cms_signal_coupling_e input_mode)
{
	int32_t err;
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(device);
	if ((input_mode >= COUPLING_UNKNOWN) || (input_mode < 0)) {
		return API_CMS_ERROR_INVALID_PARAM;
	}

	err = adi_ad9081_jesd_sysref_d2acenter_enable_set(device, 1);
	AD9081_ERROR_RETURN(err);

	/* 1: AC couple, 0: DC couple */
	err = adi_ad9081_hal_bf_set(
		device, REG_SYSREF_CTRL_ADDR, BF_SYSREF_INPUTMODE_INFO,
		((input_mode == COUPLING_AC) ? 1 : 0)); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* Power down the sysref receiver and sync circuitry. */
	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_CTRL_ADDR,
				    BF_SYSREF_PD_INFO,
				    !enable_receiver); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* enables sysref capture */
	err = adi_ad9081_hal_bf_set(
		device, 0x0fb0, 0x0103,
		enable_capture); /* not paged, spi_sysref_en@sysref_control */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_jesd_sysref_d2acenter_enable_set(device, 0);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_sync_sysref_input_config_set(
	adi_ad9081_device_t *device, adi_cms_signal_coupling_e coupling_mode,
	adi_cms_signal_type_e signal_type, uint8_t sysref_single_end_p,
	uint8_t sysref_single_end_n)
{
	int32_t err;
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_INVALID_PARAM_RETURN(sysref_single_end_n > 15 ||
				    sysref_single_end_n < 0);
	AD9081_INVALID_PARAM_RETURN(sysref_single_end_p > 15 ||
				    sysref_single_end_p < 0);
	AD9081_INVALID_PARAM_RETURN(coupling_mode != COUPLING_AC &&
				    coupling_mode != COUPLING_DC);
	AD9081_INVALID_PARAM_RETURN(signal_type == SIGNAL_UNKNOWN);

	if ((coupling_mode == COUPLING_AC &&
	     (signal_type == SIGNAL_LVDS || signal_type == SIGNAL_CML ||
	      signal_type == SIGNAL_LVPECL)) ||
	    coupling_mode == COUPLING_DC) {
		err = adi_ad9081_jesd_sysref_input_mode_set(
			device, 1, 1,
			(signal_type == SIGNAL_LVDS ||
			 signal_type == SIGNAL_CML ||
			 signal_type == SIGNAL_LVPECL) ?
				0 :
				1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_sysref_d2acenter_enable_set(device, 1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, 0x0fb9, 0x100,
			(coupling_mode == COUPLING_AC) ?
				0 :
				1); /* not paged, sysref_dc_mode_sel */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, 0x0fb9, 0x104,
			(signal_type == SIGNAL_CMOS) ?
				1 :
				0); /* not paged, sysref_single_end_mode_sel */
		AD9081_ERROR_RETURN(err);

		/* for 1.8V CMOS or higher, set ground ref resistor to 6.3 kohm. For 1.5V CMOS, set to 7.9 kohm.*/
		if (signal_type == SIGNAL_CMOS) {
			err = adi_ad9081_hal_bf_set(
				device, 0x0fba, 0x400,
				sysref_single_end_p); /* not paged, sysref_single_end_p */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0fba, 0x404,
				sysref_single_end_n); /* not paged, sysref_single_end_n */
			AD9081_ERROR_RETURN(err);
		}
		err = adi_ad9081_jesd_sysref_d2acenter_enable_set(device, 0);
		AD9081_ERROR_RETURN(err);
	} else {
		AD9081_LOG_ERR(
			"The SYSREF receiver input buffer cannot be configured in the mode specified.");
		return API_CMS_ERROR_INVALID_PARAM;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_sync_sysref_ctrl(adi_ad9081_device_t *device)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(device->clk_info.sysref_clk);
	AD9081_NULL_POINTER_RETURN(device->clk_info.sysref_ctrl);

	if (API_CMS_ERROR_OK !=
	    device->clk_info.sysref_ctrl(device->clk_info.sysref_clk)) {
		return API_CMS_ERROR_SYSREF_CTRL;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_oneshot_sync(adi_ad9081_device_t *device,
				     adi_cms_jesd_subclass_e subclass)
{
	int32_t err;
	uint8_t pd_fdacby4, sync_done;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_get(device, REG_CLK_CTRL1_ADDR, 0x00000102,
				    &pd_fdacby4, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_CLK_CTRL1_ADDR, 0x00000102,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_ROTATION_MODE_ADDR,
				    BF_ROTATION_MODE_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_hal_bf_set(device, REG_SPI_ENABLE_DAC_ADDR,
				    BF_SPI_EN_D2A0_INFO, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_SPI_ENABLE_DAC_ADDR,
				    BF_SPI_EN_D2A1_INFO, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_SPI_ENABLE_DAC_ADDR,
				    BF_SPI_EN_ANACENTER_INFO, 1);
	AD9081_ERROR_RETURN(err);

	if (device->dev_info.dev_rev == 3) { /* r2 */
		err = adi_ad9081_hal_bf_set(device, REG_ACLK_CTRL_ADDR,
					    BF_PD_TXDIGCLK_INFO,
					    1); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_ADC_DIVIDER_CTRL_ADDR,
					    0x00000107, 0); /* not paged */
		AD9081_ERROR_RETURN(err);
	}

	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_MODE_ADDR,
				    BF_SYSREF_MODE_ONESHOT_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_MODE_ADDR,
				    BF_SYSREF_MODE_ONESHOT_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);

	if (device->clk_info.sysref_mode == SYSREF_ONESHOT) {
		err = adi_ad9081_sync_sysref_ctrl(device);
		AD9081_ERROR_RETURN(err);
	}

	if (err = adi_ad9081_hal_bf_wait_to_clear(
		    device, REG_SYSREF_MODE_ADDR,
		    BF_SYSREF_MODE_ONESHOT_INFO), /* not paged */
	    err != API_CMS_ERROR_OK) {
		AD9081_LOG_WARN("sysref_mode_oneshot bit never cleared.");
	}
	err = adi_ad9081_hal_bf_get(device, REG_SYSREF_MODE_ADDR,
				    BF_ONESHOT_SYNC_DONE_INFO, &sync_done,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	if (sync_done != 1) {
		AD9081_LOG_WARN("oneshot sync not finished.");
	}

	if (device->dev_info.dev_rev == 3) { /* r2 */
		err = adi_ad9081_hal_bf_set(device, REG_ADC_DIVIDER_CTRL_ADDR,
					    0x00000107, 1); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_ACLK_CTRL_ADDR,
					    BF_PD_TXDIGCLK_INFO,
					    0); /* not paged */
		AD9081_ERROR_RETURN(err);
	}

	err = adi_ad9081_hal_bf_set(device, REG_CLK_CTRL1_ADDR, 0x00000102,
				    pd_fdacby4); /* not paged */
	AD9081_ERROR_RETURN(err);

	if (sync_done != 1) {
		return API_CMS_ERROR_JESD_SYNC_NOT_DONE;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_sysref_setup_hold_get(adi_ad9081_device_t *device,
					      uint8_t *setup_risk_violation,
					      uint8_t *hold_risk_violation)
{
	int32_t err;
	uint8_t i = 0;
	uint8_t scount = 0, snum, hcount = 0, hnum;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(setup_risk_violation);
	AD9081_NULL_POINTER_RETURN(hold_risk_violation);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_get(device, 0x0fb7, 0x800, setup_risk_violation,
				    sizeof(uint8_t)); /* SYSREF_SETUP */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_hal_bf_get(device, 0x0fb8, 0x800, hold_risk_violation,
				    sizeof(uint8_t)); /* SYSREF_HOLD */
	AD9081_ERROR_RETURN(err);

	/* Number of 1s in setup time register */
	snum = *setup_risk_violation;

	for (i = 0; i < 8; i++) {
		if (1 & snum) {
			scount++;
			snum = snum >> 1;
		}
	}
	*setup_risk_violation = scount;

	/* Number of 1s in hold time register */
	hnum = *hold_risk_violation;

	for (i = 0; i < 8; i++) {
		if (1 & hnum) {
			hcount++;
			hnum = hnum >> 1;
		}
	}
	*hold_risk_violation = hcount;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_sysref_fine_superfine_delay_set(
	adi_ad9081_device_t *device, uint8_t enable, uint8_t fine_delay,
	uint8_t superfine_delay)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (enable > 3) {
		return API_CMS_ERROR_INVALID_PARAM;
	}

	/*Enable Fine and Super fine delay on the SYSREF input
    00: SYSREF delay disabled
    01: Fine delay
    10: Super fine delay
    11: both Fine and Superfine Delay*/

	err = adi_ad9081_hal_bf_set(device, 0x0fb1, 0x200, enable);
	AD9081_ERROR_RETURN(err);
	if (enable == 1 || enable == 3) {
		err = adi_ad9081_hal_bf_set(
			device, 0x0fb2, 0x800,
			fine_delay); /* maximum effective setting is 0x2F */
		AD9081_ERROR_RETURN(err);
	}
	if (enable == 2 || enable == 3) {
		err = adi_ad9081_hal_bf_set(
			device, 0xfb3, 0x800,
			superfine_delay); /* maximum is approx. 4ps (255 x 16 fs) */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_sysref_count_set(adi_ad9081_device_t *device,
					 uint8_t edges)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_COUNT_ADDR,
				    BF_SYSREF_COUNT_INFO, edges);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_sysref_average_set(adi_ad9081_device_t *device,
					   uint8_t pulses)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (pulses > 0x7) {
		return API_CMS_ERROR_INVALID_PARAM;
	}

	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_AVERAGE_ADDR,
				    BF_SYSREF_AVERAGE_INFO, pulses);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_sysref_monitor_phase_get(adi_ad9081_device_t *device,
						 uint16_t *sysref_phase)
{
	int32_t err;
	uint8_t phase0_val;
	uint8_t phase1_val;

	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(sysref_phase);
	AD9081_LOG_FUNC();

	/* Write strobe to trigger a value update */
	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_PHASE0_ADDR, 0x800,
				    0x00);

	err = adi_ad9081_hal_bf_get(device, REG_SYSREF_PHASE0_ADDR, 0x800,
				    &phase0_val, sizeof(uint8_t));
	err = adi_ad9081_hal_bf_get(device, REG_SYSREF_PHASE1_ADDR, 0x400,
				    &phase1_val, sizeof(uint8_t));

	*sysref_phase = (phase1_val << 8) + phase0_val;

	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_sysref_monitor_lmfc_align_error_get(adi_ad9081_device_t *device,
						    uint8_t *lmfc_align_err)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(lmfc_align_err);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_get(device, REG_SYSREF_ERR_WINDOW_ADDR,
				    BF_SYSREF_WITHIN_LMFC_ERRWINDOW_INFO,
				    lmfc_align_err, sizeof(uint8_t));

	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_sysref_monitor_lmfc_align_threshold_set(
	adi_ad9081_device_t *device, uint8_t sysref_error_window)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (sysref_error_window > 0x7F) {
		return API_CMS_ERROR_INVALID_PARAM;
	}

	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_ERR_WINDOW_ADDR,
				    BF_SYSREF_ERR_WINDOW_INFO,
				    sysref_error_window);

	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_sysref_irq_enable_set(adi_ad9081_device_t *device,
					      uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_IRQ_ENABLE_0_ADDR,
				    BF_EN_SYSREF_IRQ_INFO, enable);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_sysref_irq_jitter_mux_set(adi_ad9081_device_t *device,
						  uint8_t pin)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (pin != 0 && pin != 1) {
		return API_CMS_ERROR_INVALID_PARAM;
	}

	err = adi_ad9081_hal_bf_set(device, REG_IRQ_OUTPUT_MUX_0_ADDR,
				    BF_MUX_SYSREF_JITTER_INFO, pin);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_sysref_oneshot_sync_done_get(adi_ad9081_device_t *device,
					     uint8_t *sync_done)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(sync_done);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_get(device, REG_SYSREF_MODE_ADDR,
				    BF_ONESHOT_SYNC_DONE_INFO, sync_done,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);

	if (*sync_done != 1) {
		AD9081_LOG_ERR("oneshot sync not finished.");
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_sync_calc_jrx_lmfc_lemc(uint64_t dac_clk,
					   uint8_t main_interp,
					   uint8_t ch_interp,
					   adi_cms_jesd_param_t *jesd_param,
					   uint64_t *lmfc_freq)
{
	if (lmfc_freq == NULL) {
		return API_CMS_ERROR_NULL_PARAM;
	}

#ifdef __KERNEL__
	*lmfc_freq =
		div64_u64(dac_clk, jesd_param->jesd_s * jesd_param->jesd_k *
					   main_interp * ch_interp);
#else
	*lmfc_freq = (dac_clk) / (jesd_param->jesd_s * jesd_param->jesd_k *
				  main_interp * ch_interp);
#endif
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_sync_calc_jtx_lmfc_lemc(uint64_t adc_clk,
					   uint8_t cddc_dcm[4],
					   uint8_t fddc_dcm[8],
					   adi_ad9081_jesd_link_select_e links,
					   adi_cms_jesd_param_t jesd_param[2],
					   uint64_t *lmfc_freq)
{
	uint64_t lmfc_link0, lmfc_link1, gcd, min_link, max_link, lmfc_gcd;
	uint8_t cdcm, fdcm;
	if (lmfc_freq == NULL) {
		return API_CMS_ERROR_NULL_PARAM;
	}

	cdcm = adi_ad9081_adc_ddc_coarse_dcm_decode(cddc_dcm[0]);
	fdcm = adi_ad9081_adc_ddc_fine_dcm_decode(fddc_dcm[0]);

	if (jesd_param->jesd_duallink > 0) {
#ifdef __KERNEL__
		/* link 0 */
		lmfc_link0 = div64_u64(adc_clk, jesd_param[0].jesd_s *
							jesd_param[0].jesd_k *
							cdcm * fdcm);

		/* link 1 */
		lmfc_link1 = div64_u64(adc_clk, jesd_param[1].jesd_s *
							jesd_param[1].jesd_k *
							cdcm * fdcm);
#else
		/* link 0 */
		lmfc_link0 = (adc_clk) / (jesd_param[0].jesd_s *
					  jesd_param[0].jesd_k * cdcm * fdcm);

		/* link 1 */
		lmfc_link1 = (adc_clk) / (jesd_param[1].jesd_s *
					  jesd_param[1].jesd_k * cdcm * fdcm);
#endif

		/* gcd between links */
		max_link = (lmfc_link0 >= lmfc_link1) ? lmfc_link0 : lmfc_link1;
		min_link = (lmfc_link0 >= lmfc_link1) ? lmfc_link1 : lmfc_link0;
		gcd = adi_api_utils_gcd(min_link, max_link);
		lmfc_gcd = gcd;
	} else {
		/* link 0 */
#ifdef __KERNEL__
		lmfc_link0 = div64_u64(adc_clk, jesd_param[0].jesd_s *
							jesd_param[0].jesd_k *
							cdcm * fdcm);
#else
		lmfc_link0 = (adc_clk) / (jesd_param[0].jesd_s *
					  jesd_param[0].jesd_k * cdcm * fdcm);
#endif
		lmfc_gcd = lmfc_link0;
	}

	*lmfc_freq = lmfc_gcd;
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_sync_sysref_frequency_set(
	adi_ad9081_device_t *device, uint64_t *sysref_freq, uint64_t dac_clk,
	uint64_t adc_clk, uint8_t main_interp, uint8_t ch_interp,
	uint8_t cddc_dcm[4], uint8_t fddc_dcm[8],
	adi_ad9081_jesd_link_select_e jtx_links,
	adi_cms_jesd_param_t *jrx_param, adi_cms_jesd_param_t jtx_param[2])
{
	uint64_t jrx_lmfc, jtx_lmfc, max_lmfc, min_lmfc, gcd = 0;
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(sysref_freq);
	AD9081_NULL_POINTER_RETURN(jrx_param);
	AD9081_LOG_FUNC();

	/* jrx */
	err = adi_ad9081_sync_calc_jrx_lmfc_lemc(
		dac_clk, main_interp, ch_interp, jrx_param, &jrx_lmfc);
	AD9081_ERROR_RETURN(err);

	/* jtx */
	err = adi_ad9081_sync_calc_jtx_lmfc_lemc(
		adc_clk, cddc_dcm, fddc_dcm, jtx_links, jtx_param, &jtx_lmfc);
	AD9081_ERROR_RETURN(err);

	/* gcd */
	max_lmfc = (jrx_lmfc >= jtx_lmfc) ? jrx_lmfc : jtx_lmfc;
	min_lmfc = (jrx_lmfc >= jtx_lmfc) ? jtx_lmfc : jrx_lmfc;
	gcd = adi_api_utils_gcd(min_lmfc, max_lmfc);
	*sysref_freq = gcd;

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_sync_jrx_tpl_phase_diff_get(adi_ad9081_device_t *device,
				       adi_ad9081_jesd_link_select_e links,
				       uint8_t *jrx_phase_diff)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(jrx_phase_diff);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device, REG_JRX_TPL_5_ADDR,
					    BF_JRX_TPL_PHASE_DIFF_INFO,
					    jrx_phase_diff, 1);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device, REG_JRX_TPL_5_ADDR,
					    BF_JRX_TPL_PHASE_DIFF_INFO,
					    jrx_phase_diff, 1);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}