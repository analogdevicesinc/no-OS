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

	adi_ad9081_jesd_sysref_d2acenter_enable_set(device, 1);

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

	adi_ad9081_jesd_sysref_d2acenter_enable_set(device, 0);

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

	err = adi_ad9081_hal_bf_get(device, REG_SYSREF_PHASE0_ADDR, 0x800,
				    &phase0_val, sizeof(uint8_t));
	err = adi_ad9081_hal_bf_get(device, REG_SYSREF_PHASE1_ADDR, 0x400,
				    &phase1_val, sizeof(uint8_t));

	*sysref_phase = (phase0_val << 8) + phase1_val;

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