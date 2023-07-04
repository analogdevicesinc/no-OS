/*!
 * @brief     APIs for JESD configuration and control
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9081_JESD_API
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))
/*============= C O D E ====================*/
int32_t adi_ad9081_jesd_rx_link_select_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_PAGEINDX_DAC_JRX_ADDR,
				    BF_JRX_LINK_MSK_INFO, links);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_descrambler_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t dsr_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(dsr_en > 0x1);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_3_ADDR,
					    BF_JRX_DSCR_CFG_INFO,
					    dsr_en); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_3_ADDR,
					    BF_JRX_DSCR_CFG_INFO,
					    dsr_en); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_lane_invert_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t logical_lane,
					   uint8_t invert_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(logical_lane > 0x7);
	AD9081_INVALID_PARAM_RETURN(invert_en > 0x1);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JRX_CORE_2_LANE0_ADDR + logical_lane,
			BF_JRX_LINK_LANE_INVERSE_0_INFO, invert_en);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JRX_CORE_2_LANE0_ADDR + logical_lane,
			BF_JRX_LINK_LANE_INVERSE_0_INFO, invert_en);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_lane_xbar_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t physical_lane,
					 uint8_t logical_lane)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(logical_lane > 0x7);
	AD9081_INVALID_PARAM_RETURN(physical_lane > 0x7);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JRX_CORE_2_LANE0_ADDR + logical_lane,
			BF_JRX_SRC_LANE_0_INFO, physical_lane);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JRX_CORE_2_LANE0_ADDR + logical_lane,
			BF_JRX_SRC_LANE_0_INFO, physical_lane);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_lanes_xbar_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t logical_lanes[8])
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		err = adi_ad9081_jesd_rx_lane_xbar_set(device, links, i,
						       logical_lanes[i]);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_synca_mode_set(adi_ad9081_device_t *device,
					  uint8_t mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* 0: CMOS, 1: LVDS */
	err = adi_ad9081_hal_bf_set(device, REG_SYNCA_ADDR,
				    BF_SEL_SYNCA_MODE_RC_INFO,
				    mode); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_syncb_mode_set(adi_ad9081_device_t *device,
					  uint8_t mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* 0: CMOS, 1: LVDS */
	err = adi_ad9081_hal_bf_set(device, REG_SYNCB_ADDR,
				    BF_SEL_SYNCB_MODE_RC_INFO,
				    mode); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_synca_driver_powerdown_set(adi_ad9081_device_t *device,
					      uint8_t powerdown)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_GENERIC_PD_ADDR,
				    BF_PD_SYNCA_RC_INFO,
				    powerdown); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_syncb_driver_powerdown_set(adi_ad9081_device_t *device,
					      uint8_t powerdown)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_GENERIC_PD_ADDR,
				    BF_PD_SYNCB_RC_INFO,
				    powerdown); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_link_config_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   adi_cms_jesd_param_t *jesd_param)
{
	int32_t err;
	uint8_t i, link, not_in_table;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(jesd_param);

	if (jesd_param->jesd_duallink == 0) {
		err = adi_ad9081_jesd_rx_synca_driver_powerdown_set(device, 0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_syncb_driver_powerdown_set(device, 1);
		AD9081_ERROR_RETURN(err);
	} else {
		err = adi_ad9081_jesd_rx_synca_driver_powerdown_set(device, 0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_syncb_driver_powerdown_set(device, 0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_sync_mode_set(device, AD9081_LINK_0,
						       1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_sync_mode_set(device, AD9081_LINK_1,
						       2);
		AD9081_ERROR_RETURN(err);
	}
	err = adi_ad9081_jesd_rx_synca_mode_set(device, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_jesd_rx_syncb_mode_set(device, 1);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_hal_bf_set(device, REG_JESD_MODE_ADDR,
				    BF_TX_JESD_MODE_INFO,
				    jesd_param->jesd_mode_id); /* not paged */
	AD9081_ERROR_RETURN(err);

	if (jesd_param->jesd_jesdv < 2) { /* 204A/B */
		err = adi_ad9081_hal_bf_set(device, REG_JRX_DL_204B_2_ADDR,
					    BF_JRX_DL_204B_ENABLE_INFO,
					    1); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_DL_204C_0_ADDR,
					    BF_JRX_DL_204C_ENABLE_INFO,
					    0); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_DL_204H_37_ADDR,
					    BF_JRX_DL_204H_ENABLE_INFO,
					    0); /* not paged */
		AD9081_ERROR_RETURN(err);
	}
	if (jesd_param->jesd_jesdv == 2) { /* 204C */
		err = adi_ad9081_hal_bf_set(device, REG_JRX_DL_204B_2_ADDR,
					    BF_JRX_DL_204B_ENABLE_INFO,
					    0); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_DL_204C_0_ADDR,
					    BF_JRX_DL_204C_ENABLE_INFO,
					    1); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_DL_204H_37_ADDR,
					    BF_JRX_DL_204H_ENABLE_INFO,
					    0); /* not paged */
		AD9081_ERROR_RETURN(err);
	}

	err = adi_ad9081_hal_bf_set(
		device, REG_GENERAL_JRX_CTRL_ADDR, BF_LINK_MODE_INFO,
		(jesd_param->jesd_duallink == 0) ? 0 : 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x00000D0F, 0x00000100,
				    0); /* paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x00000E2C, 0x00000102,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_jesd_rx_config_status_get(device, &not_in_table);
	AD9081_ERROR_RETURN(err);
	if (not_in_table == 1) { /* warning */
		AD9081_WARN_REPORT(not_in_table,
				   "jrx configuration is not in table.");
	}

	for (i = 0; i < 2; i++) {
		link = (uint8_t)(links & (AD9081_LINK_0 << i));
		if (link > 0) {
			err = adi_ad9081_jesd_rx_link_select_set(device, link);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_SYNCB_GEN_1_ADDR, 0x00000400,
				(jesd_param->jesd_f == 3) ?
					2 :
					(8 / jesd_param->jesd_f));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_0_ADDR,
						    BF_JRX_DID_CFG_INFO,
						    jesd_param->jesd_did);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_2_ADDR,
						    BF_JRX_LID_CFG_INFO,
						    jesd_param->jesd_lid0);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(device, REG_JRX_L0_3_ADDR,
						     BF_JRX_DSCR_CFG_INFO,
						     jesd_param->jesd_scr,
						     0x00000500,
						     jesd_param->jesd_l - 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x000004AA,
						    0x00000800,
						    jesd_param->jesd_f - 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x000004AB,
						    0x00000800,
						    jesd_param->jesd_k - 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x000004AC,
						    0x00000800,
						    jesd_param->jesd_m - 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(device, REG_JRX_L0_7_ADDR,
						     0x00000500,
						     jesd_param->jesd_n - 1,
						     BF_JRX_CS_CFG_INFO,
						     jesd_param->jesd_cs);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(device, REG_JRX_L0_8_ADDR,
						     0x00000500,
						     jesd_param->jesd_np - 1,
						     BF_JRX_SUBCLASSV_CFG_INFO,
						     jesd_param->jesd_subclass);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(device, 0x000004AF,
						     0x00000500,
						     jesd_param->jesd_s - 1,
						     0x00000305,
						     jesd_param->jesd_jesdv);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x000004B0,
						    0x00000107,
						    jesd_param->jesd_hd);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JRX_TPL_1_ADDR,
				BF_JRX_TPL_BUF_PROTECTION_INFO, 0);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JRX_CORE_1_ADDR,
				BF_JRX_SYSREF_FOR_STARTUP_INFO,
				jesd_param->jesd_subclass);
			AD9081_ERROR_RETURN(err);
		}
	}

	/* configure default lmfc delay */
	err = adi_ad9081_jesd_rx_lmfc_delay_set(device, links, 0xc);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_lmfc_delay_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint16_t delay)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_TPL_3_ADDR,
					    BF_JRX_TPL_PHASE_ADJUST_INFO,
					    delay);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_TPL_3_ADDR,
					    BF_JRX_TPL_PHASE_ADJUST_INFO,
					    delay);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_sync_mode_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t sync_mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_CORE_3_ADDR,
					    BF_JRX_SYNC_N_SEL_INFO, sync_mode);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_CORE_3_ADDR,
					    BF_JRX_SYNC_N_SEL_INFO, sync_mode);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_firmware_version_get(adi_ad9081_device_t *device,
						uint8_t *major, uint8_t *minor)
{
	int32_t err;
	uint32_t reg_rx_ver_addr = 0x21df;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (device->dev_info.dev_rev == 1) { /* r1 */
		reg_rx_ver_addr = 0x21e7;
	}
	if (device->dev_info.dev_rev == 2 ||
	    device->dev_info.dev_rev == 3) { /* r1r/r2 */
		reg_rx_ver_addr = 0x21df;
	}

	err = adi_ad9081_hal_bf_get(device, reg_rx_ver_addr, 0x0400, minor,
				    1); /* rx_ver_lower@rx_version */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, reg_rx_ver_addr, 0x0404, major,
				    1); /* rx_ver_upper@rx_version */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_run_cal_mask_set(adi_ad9081_device_t *device,
					    uint8_t mask)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (device->dev_info.dev_rev == 2) { /* r1r */
		err = adi_ad9081_hal_bf_set(
			device, 0x21c3, 0x0800,
			mask); /* rx_run_cal_mask@rx_bg_cal_en */
		AD9081_ERROR_RETURN(err);
	}
	if (device->dev_info.dev_rev == 3) { /* r2 */
		err = adi_ad9081_hal_bf_set(
			device, 0x21c4, 0x0800,
			mask); /* rx_run_cal_mask@rx_bg_cal_en */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_boost_mask_set(adi_ad9081_device_t *device,
					  uint8_t mask)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (device->dev_info.dev_rev == 2) { /* r1r */
		err = adi_ad9081_hal_bf_set(device, 0x21c4, 0x0800,
					    mask); /* rx_boost_mask@rx_boost */
		AD9081_ERROR_RETURN(err);
	}
	if (device->dev_info.dev_rev == 3) { /* r2 */
		err = adi_ad9081_hal_bf_set(device, 0x21c5, 0x0800,
					    mask); /* rx_boost_mask@rx_boost */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_cal_boot_wait(adi_ad9081_device_t *device)
{
	int32_t err;
	uint16_t rx_set_state1_addr, rx_scratch0_addr;
	uint8_t i, rx_reset_state, rx_scratch0;
	const uint8_t max_wait = 50;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (device->dev_info.dev_rev == 1) { /* r1 */
		rx_set_state1_addr = 0x21d9;
		rx_scratch0_addr = 0x21e6;
	}
	if (device->dev_info.dev_rev == 2 ||
	    device->dev_info.dev_rev == 3) { /* r1r/r2 */
		rx_set_state1_addr = 0x21c1;
		rx_scratch0_addr = 0x21de;
	}

	/* patched code */
	err = adi_ad9081_hal_reg_set(device, rx_scratch0_addr, 0xaa);
	AD9081_ERROR_RETURN(err);

	/* if set to high, will reset tracking structures and register maps. self clears bit for acknowledgment. */
	err = adi_ad9081_hal_reg_set(device, rx_set_state1_addr, 0x80);
	AD9081_ERROR_RETURN(err);

	for (i = 0; i < max_wait; i++) {
		err = adi_ad9081_hal_bf_get(device, rx_set_state1_addr, 0x0107,
					    &rx_reset_state, 1);
		AD9081_ERROR_RETURN(err);
		if (rx_reset_state == 0)
			break;
		err = adi_ad9081_hal_delay_us(device,
					      AD9081_JESD_CAL_BOOT_WAIT);
		AD9081_ERROR_RETURN(err);
	}
	if (i == max_wait) {
		err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_ERR,
					       "rx_reset_state timed out");
		AD9081_ERROR_RETURN(err);
		return API_CMS_ERROR_ERROR;
	}

	err = adi_ad9081_hal_reg_get(device, rx_scratch0_addr, &rx_scratch0);
	AD9081_ERROR_RETURN(err);
	if (rx_scratch0 != 0x00) {
		err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_ERR,
					       "rx_reset_state internal error");
		AD9081_ERROR_RETURN(err);
		return API_CMS_ERROR_ERROR;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_cal_manual_call(adi_ad9081_device_t *device,
					uint8_t state)
{
	int32_t err;
	uint8_t i, jrx_at_idle, rx_state_go;
	const uint8_t max_wait = 200;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_reg_set(device, 0x21c0, state);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(device, 0x21c1, 0x01);
	AD9081_ERROR_RETURN(err);

	for (i = 0; i < max_wait; i++) {
		err = adi_ad9081_hal_bf_get(device, 0x21c1, 0x0100,
					    &rx_state_go, 1);
		AD9081_ERROR_RETURN(err);
		if (rx_state_go == 0)
			break;
		err = adi_ad9081_hal_delay_us(device, AD9081_JESD_MAN_CAL_WAIT);
		AD9081_ERROR_RETURN(err);
	}
	if (i == max_wait) {
		err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_ERR,
					       "manual call timed out");
		AD9081_ERROR_RETURN(err);
		return API_CMS_ERROR_ERROR;
	}

	for (i = 0; i < max_wait; i++) {
		err = adi_ad9081_hal_bf_get(device, 0x21dd, 0x0100,
					    &jrx_at_idle,
					    1); /* rx_at_idle@rx_state_status */
		AD9081_ERROR_RETURN(err);
		if (jrx_at_idle == 1)
			break;
		err = adi_ad9081_hal_delay_us(device, AD9081_JESD_MAN_CAL_WAIT);
		AD9081_ERROR_RETURN(err);
	}
	if (i == max_wait) {
		err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_ERR,
					       "manual call timed out");
		AD9081_ERROR_RETURN(err);
		return API_CMS_ERROR_ERROR;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_cal_fg_cal_wait(adi_ad9081_device_t *device)
{
	int32_t err;
	uint8_t i, jrx_at_idle, rx_bg_cal_run;
	const uint8_t max_wait = 50;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < max_wait; i++) {
		err = adi_ad9081_hal_bf_get(device, 0x21c1, 0x0103,
					    &rx_bg_cal_run, 1);
		AD9081_ERROR_RETURN(err);
		if (rx_bg_cal_run == 0)
			break;
		err = adi_ad9081_hal_delay_us(device, AD9081_JESD_FG_CAL_WAIT);
		AD9081_ERROR_RETURN(err);
	}
	if (i == max_wait) {
		err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_ERR,
					       "fg cal timed out");
		AD9081_ERROR_RETURN(err);
		return API_CMS_ERROR_ERROR;
	}

	for (i = 0; i < max_wait; i++) {
		err = adi_ad9081_hal_bf_get(device, 0x21dd, 0x0100,
					    &jrx_at_idle,
					    1); /* rx_at_idle@rx_state_status */
		AD9081_ERROR_RETURN(err);
		if (jrx_at_idle == 1)
			break;
		err = adi_ad9081_hal_delay_us(device, AD9081_JESD_FG_CAL_WAIT);
		AD9081_ERROR_RETURN(err);
	}
	if (i == max_wait) {
		err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_ERR,
					       "fg cal timed out");
		AD9081_ERROR_RETURN(err);
		return API_CMS_ERROR_ERROR;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_cal_bg_cal_pause(adi_ad9081_device_t *device)
{
	int32_t err;
	uint8_t i, jrx_at_idle;
	uint16_t rx_set_state1_addr;
	const uint8_t max_wait = 50;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* set rx_set_state1 reg addr */
	if (device->dev_info.dev_rev == 1) { /* r1 */
		rx_set_state1_addr = 0x21d9;
	}
	if (device->dev_info.dev_rev == 2 ||
	    device->dev_info.dev_rev == 3) { /* r1r/r2 */
		rx_set_state1_addr = 0x21c1;
	}

	err = adi_ad9081_hal_bf_set(device, rx_set_state1_addr, 0x0103,
				    0x0); /* rx_bg_cal_run = 0 */
	AD9081_ERROR_RETURN(err);

	for (i = 0; i < max_wait; i++) {
		err = adi_ad9081_hal_bf_get(device, 0x21dd, 0x0100,
					    &jrx_at_idle,
					    1); /* rx_at_idle@rx_state_status */
		AD9081_ERROR_RETURN(err);
		if (jrx_at_idle == 1)
			break;
		err = adi_ad9081_hal_delay_us(device,
					      AD9081_JESD_RX_204C_CAL_WAIT);
		AD9081_ERROR_RETURN(err);
	}
	if (i == max_wait) {
		err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_ERR,
					       "bg cal pause timed out");
		AD9081_ERROR_RETURN(err);
		return API_CMS_ERROR_ERROR;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_cal_bg_cal_start(adi_ad9081_device_t *device)
{
	int32_t err;
	uint16_t rx_set_state1_addr;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* set rx_set_state1 reg addr */
	if (device->dev_info.dev_rev == 1) { /* r1 */
		rx_set_state1_addr = 0x21d9;
	}
	if (device->dev_info.dev_rev == 2 ||
	    device->dev_info.dev_rev == 3) { /* r1r/r2 */
		rx_set_state1_addr = 0x21c1;
	}

	err = adi_ad9081_hal_bf_set(device, rx_set_state1_addr, 0x0103,
				    0x1); /* rx_bg_cal_run = 1 */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_calibrate_204c(adi_ad9081_device_t *device,
					  uint8_t force_cal_reset,
					  uint8_t boost_mask,
					  uint8_t run_bg_cal)
{
	int32_t err;
	uint8_t core_status, jrx_fw_major, jrx_fw_minor, rx_bg_cal_run, lanes;
	uint16_t rx_set_state1_addr, rx_set_state2_addr;
	uint8_t rx_run_cal_mask = 0xFF;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* set cal mask to run cal for enabled phy lanes only*/
	err = adi_ad9081_hal_bf_get(device, REG_PHY_PD_ADDR,
				    BF_PD_DES_RC_CH_INFO, &lanes,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	rx_run_cal_mask = ~(lanes);

	/* set rx_set_state1 reg addr */
	if (device->dev_info.dev_rev == 1) { /* r1 */
		rx_set_state1_addr = 0x21d9;
	}
	if (device->dev_info.dev_rev == 2 ||
	    device->dev_info.dev_rev == 3) { /* r1r/r2 */
		rx_set_state1_addr = 0x21c1;
	}
	if (device->dev_info.dev_rev == 3) { /* r2 */
		rx_set_state2_addr = 0x21c2;
	}

	/* reset or start app task */
	err = adi_ad9081_hal_bf_get(device, 0x3742, 0x0800, &core_status,
				    1); /* @msg2 */
	AD9081_ERROR_RETURN(err);
	if (core_status != 0xff) {
		err = adi_ad9081_hal_bf_set(device, REG_REG8_SCRATCH1_ADDR,
					    BF_REG8_SCRATCH_2_INFO, 0x0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_delay_us(device,
					      AD9081_JESD_RX_204C_CAL_WAIT);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device, 0x3742, 0x0800,
					    &core_status, 1); /* @msg2 */
		AD9081_ERROR_RETURN(err);
		if (core_status != 0xff)
			AD9081_LOG_ERR(
				"Boot not completed. Expected core_status 0xff.");
	}

	/* log jrx firmware version */
	err = adi_ad9081_jesd_rx_firmware_version_get(device, &jrx_fw_major,
						      &jrx_fw_minor);
	AD9081_ERROR_RETURN(err);

	/* calibrate */
	if (device->dev_info.dev_rev == 1) { /* r1 */
		/* reset cal state machine */
		err = adi_ad9081_jesd_cal_boot_wait(device);
		AD9081_ERROR_RETURN(err);

		/* start204CCal()@ad9081_tx_r1.py, run_bg_cal()@rx_firmware_base.py */
		err = adi_ad9081_hal_bf_set(
			device, rx_set_state1_addr, 0x0106,
			0x1); /* rx_bg_cal_skip_offsets = 0 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, rx_set_state1_addr, 0x0105,
					    0x1); /* rx_bg_cal_skip_lms = 0 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, rx_set_state1_addr, 0x0104,
					    0x1); /* rx_fg_cal_only_run = 1 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, rx_set_state1_addr, 0x0103,
					    0x1); /* rx_bg_cal_run = 1 */
		AD9081_ERROR_RETURN(err);

		/* wait for fg cal to complete */
		err = adi_ad9081_hal_delay_us(device, 15000000);
		AD9081_ERROR_RETURN(err);
	}
	if (device->dev_info.dev_rev == 2 ||
	    device->dev_info.dev_rev == 3) { /* r1r/r2 */
		if (force_cal_reset >
		    0) { /* must be 1 for the 1st time calibartion after boot */
			/* reset cal state machine */
			err = adi_ad9081_jesd_cal_boot_wait(device);
			AD9081_ERROR_RETURN(err);
		} else {
			err = adi_ad9081_hal_bf_get(device, rx_set_state1_addr,
						    0x0103, &rx_bg_cal_run, 1);
			AD9081_ERROR_RETURN(err);
			if (rx_bg_cal_run == 1) {
				/* bg cal's are currently running, need to stop before resetting */
				err = adi_ad9081_hal_bf_set(
					device, rx_set_state1_addr, 0x0103,
					0x0); /* rx_bg_cal_run = 0 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_jesd_cal_fg_cal_wait(device);
				AD9081_ERROR_RETURN(err);
			}
			err = adi_ad9081_jesd_cal_manual_call(device, 62);
			AD9081_ERROR_RETURN(err);
		}

		/* Bypass calibration for all lanes when enabled */
		if (device->serdes_info.des_settings.cal_mode ==
		    AD9081_CAL_MODE_BYPASS) {
			err = adi_ad9081_device_spi_register_set(
				device, 0x21c8, rx_run_cal_mask);
			AD9081_ERROR_RETURN(err);
		}

		/* set Equalizer boost mode */
		err = adi_ad9081_jesd_rx_run_cal_mask_set(device,
							  rx_run_cal_mask);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_boost_mask_set(device, boost_mask);
		AD9081_ERROR_RETURN(err);

		/* enable temperature calibration */
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_reg_set(device, rx_set_state2_addr,
						     0x31);
			AD9081_ERROR_RETURN(err);
		}

		/* start204CCal()@ad9081_tx_r1.py, run_bg_cal()@rx_firmware_base.py */
		err = adi_ad9081_hal_bf_set(device, rx_set_state1_addr, 0x0104,
					    0x1); /* rx_fg_cal_only_run = 1 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, rx_set_state1_addr, 0x0103,
					    0x1); /* rx_bg_cal_run = 1 */
		AD9081_ERROR_RETURN(err);

		/* wait for FG cal to complete */
		err = adi_ad9081_jesd_cal_fg_cal_wait(device);
		AD9081_ERROR_RETURN(err);
		/* TODO: log FG cal results here */
		if ((device->dev_info.dev_rev == 3) &&
		    (run_bg_cal > 0)) { /* R2 only */
			err = adi_ad9081_hal_bf_set(
				device, rx_set_state1_addr, 0x0104,
				0x0); /* rx_fg_cal_only_run = 0 */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, rx_set_state1_addr, 0x0103,
				0x1); /* rx_bg_cal_run = 1 */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_load_cbus_table(adi_ad9081_device_t *device,
					   adi_ad9081_deser_mode_e deser_mode)
{
	int32_t err;
	uint8_t i, table_offset;
	const uint8_t rx_cbus_table[14][4] = {
		/* Reg  FR    HR    QR */
		{ 0x08, 0x02, 0x02,
		  0x02 }, /* sel_lf_dllslew_des_rc@ADDR_CBUS_RX_LF_DLL */
		{ 0x15, 0x01, 0x00,
		  0x01 }, /* override DCD DAC@ADDR_CBUS_RX_CAL_DCD_DAC */
		{ 0x50, 0x04, 0x04,
		  0x04 }, /* reverse DCD DAC loop direction@ADDR_CBUS_RX_SPARE0 */
		{ 0xbc, 0x80, 0x80,
		  0x00 }, /* s0_pd_fb456@ADDR_CBUS_RX_DFE_CTL56 */
		{ 0xbe, 0x80, 0x80,
		  0x00 }, /* s1_pd_fb456@ADDR_CBUS_RX_DFE_CTL58 */
		{ 0xc0, 0xff, 0xff,
		  0x00 }, /* s1_pd_ifb3, s1_pd_ifb2, s0_pd_ifb3, s0_pd_ifb2@ADDR_CBUS_RX_DFE_CTL0 */
		{ 0xcd, 0x40, 0x00,
		  0x00 }, /* s1_pd_clkgen@ADDR_CBUS_RX_DFE_CTL13 */
		{ 0xd2, 0xf0, 0xf0,
		  0x00 }, /* s0_mux_en_ovrd, s0_pd_dac_thr@ADDR_CBUS_RX_DFE_CTL18 */
		{ 0xd3, 0xee, 0xee,
		  0x00 }, /* s0_pd_dac_off@ADDR_CBUS_RX_DFE_CTL19*/
		{ 0xdc, 0xee, 0xee,
		  0x00 }, /* s0_pd_comp_f1, s0_pd_comp_f0@ADDR_CBUS_RX_DFE_CTL28 */
		{ 0xe2, 0xf0, 0xF0,
		  0x00 }, /* s1_mux_en_ovrd, s1_pd_dac_thr@ADDR_CBUS_RX_DFE_CTL31 */
		{ 0xe3, 0xff, 0xee,
		  0x00 }, /* s1_pd_dac_off@ADDR_CBUS_RX_DFE_CTL32 */
		{ 0xec, 0xff, 0xee,
		  0x00 }, /* s1_pd_comp_f1, s1_pd_comp_f0@ADDR_CBUS_RX_DFE_CTL41 */
		{ 0xf4, 0x01, 0x05,
		  0x05 }, /* s1_en_gmsw, s0_en_gmsw@ADDR_CBUS_RX_DFE_CTL46 */
	};
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	switch (deser_mode) {
	case AD9081_FULL_RATE:
		table_offset = 1u;
		break;
	case AD9081_HALF_RATE:
		table_offset = 2u;
		break;
	case AD9081_QUART_RATE:
	default:
		table_offset = 3u;
		break;
	}

	for (i = 0; i < 14; i++) {
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, rx_cbus_table[i][0],
			rx_cbus_table[i][table_offset], 0xFF);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_ctle_filter_set(adi_ad9081_device_t *device,
					   uint8_t lanes, uint8_t ctle_filter)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(
		ctle_filter < 1 ||
		ctle_filter >
			4) /*Range 1-4 corresponding CTLE cutoff frequency to channel insertion loss*/

	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0xfd, (1 << ctle_filter) - 1,
		lanes); /* @ADDR_CBUS_RX_DFE_CTL55 */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_startup_des(adi_ad9081_device_t *device,
				       adi_ad9081_deser_mode_e deser_mode)
{
	int32_t err;
	uint8_t i, jesd204b_en, par_mode;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* startupDes()@AD9081_serdes_r0.py */

	/* reset CDR, rx_ad9081_startup()@ad9081_serdes_jrx_r0.py */
	err = adi_ad9081_hal_bf_set(device, REG_CDR_RESET_ADDR,
				    BF_RSTB_DES_RC_INFO, 0x1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_CDR_RESET_ADDR,
				    BF_RSTB_DES_RC_INFO, 0x0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_CDR_RESET_ADDR,
				    BF_RSTB_DES_RC_INFO, 0x1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, AD9081_SERDES_RST_WAIT);
	AD9081_ERROR_RETURN(err);

	/* set Deserializer Equaliser CTLE filter*/
	/* Set CTLE based on Insertion Loss or Boost Setting*/
	/*JESDB bit rate > 8GHZ HALF RATE*/
	/*JESDB bit rate < 8 GHZ FULL RATE*/
	/*JESDC bit rate > 16 GHZ QUART RATE*/
	/*JEADC bit rate < 16 GHZ HALF RATE*/
	if ((deser_mode == AD9081_FULL_RATE) ||
	    (deser_mode == AD9081_HALF_RATE)) {
		err = adi_ad9081_hal_bf_set(device,
					    REG_LF_QUARTERRATE_DES_RC_ADDR,
					    BF_SEL_LF_QUARTERRATE_DES_RC_INFO,
					    0x00); /* not paged */
		AD9081_ERROR_RETURN(err);

		for (i = 0; i < AD9081_JESD_DESER_COUNT; ++i) {
			/*CTLE Registers Settings for Lane Rate =16 Gbps*/
			/*Set based on Insertion loss value <10dB/>10dB*/
			/*for >10Db set config ctle setting 0*/
			/*for <10dB set config ctle setting 1*/
			/*Resuse Boost Setting Mask to determine Insertion loss Range*/
			err = adi_ad9081_jesd_rx_ctle_config_set(
				device, 1 << i,
				((((device->serdes_info.des_settings
					    .boost_mask >>
				    i) &
				   0x1) == 0x1) ?
					 AD9081_IL_CTLE_UPPER_DB_THRESH :
					 0));
			AD9081_ERROR_RETURN(err);
		}
	}
	if (deser_mode == AD9081_QUART_RATE) {
		err = adi_ad9081_hal_bf_set(device,
					    REG_LF_QUARTERRATE_DES_RC_ADDR,
					    BF_SEL_LF_QUARTERRATE_DES_RC_INFO,
					    0xff); /* not paged */
		AD9081_ERROR_RETURN(err);

		/* set_ctle_filter()@rx_firmware_base.py */
		if (device->dev_info.dev_rev == 3) { /* r2 */
			/*CTLE Registers Settings for Lane Rate >16 Gbps*/
			for (i = 0; i < AD9081_JESD_DESER_COUNT; ++i) {
				err = adi_ad9081_jesd_rx_ctle_filter_set(
					device, 1 << i,
					device->serdes_info.des_settings
						.ctle_filter[i]);
				AD9081_ERROR_RETURN(err);
			}
		}
	}

	/* rx_cbus_startup()@rx_firmware_base.py */
	err = adi_ad9081_jesd_rx_load_cbus_table(device, deser_mode);
	AD9081_ERROR_RETURN(err);

	/* rx invert mask */
	err = adi_ad9081_hal_reg_set(
		device, REG_CDR_BITINVERSE_ADDR,
		device->serdes_info.des_settings.invert_mask); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* set data mode */
	err = adi_ad9081_hal_bf_get(device, REG_JRX_DL_204B_2_ADDR,
				    BF_JRX_DL_204B_ENABLE_INFO, &jesd204b_en,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	par_mode = (jesd204b_en > 0) ? 0xaa : 0x00;
	err = adi_ad9081_hal_reg_set(device, REG_LF_PARDATAMODE_DES_RC0_ADDR,
				     par_mode);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(device, REG_LF_PARDATAMODE_DES_RC1_ADDR,
				     par_mode);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_bit_rate_get(adi_ad9081_device_t *device,
					uint64_t *bit_rate_bps)
{
	int32_t err;
	uint8_t jesd_l, jesd_m, jesd_np, jesd204b_en, main_interp, ch_interp;
	uint64_t bit_rate;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* get configuration */
	err = adi_ad9081_hal_2bf_get(device, REG_INTRP_MODE_ADDR,
				     BF_FINE_INTERP_SEL_INFO, &main_interp,
				     BF_COARSE_INTERP_SEL_INFO, &ch_interp,
				     1); /* not paged */
	AD9081_ERROR_RETURN(err);
	if ((main_interp == 0) || (ch_interp == 0)) {
		AD9081_LOG_ERR("main or channel interpolation is 0.");
		return API_CMS_ERROR_ERROR;
	}
	err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_JRX_L0_3_ADDR, 0x00000500,
				    &jesd_l, 1);
	AD9081_ERROR_RETURN(err);
	jesd_l += 1;
	err = adi_ad9081_hal_bf_get(device, 0x000004AC, 0x00000800, &jesd_m, 1);
	AD9081_ERROR_RETURN(err);
	jesd_m += 1;
	err = adi_ad9081_hal_bf_get(device, REG_JRX_L0_8_ADDR, 0x00000500,
				    &jesd_np, 1);
	AD9081_ERROR_RETURN(err);
	jesd_np += 1;
	err = adi_ad9081_hal_bf_get(device, REG_JRX_DL_204B_2_ADDR,
				    BF_JRX_DL_204B_ENABLE_INFO, &jesd204b_en,
				    1);
	AD9081_ERROR_RETURN(err);

	/* calculate jrx bit rate */
	bit_rate = (jesd_m * jesd_np * (jesd204b_en > 0 ? 10 : 66) *
		    device->dev_info.dac_freq_hz);
#ifdef __KERNEL__
	bit_rate = div_u64(bit_rate, jesd_l * (jesd204b_en > 0 ? 8 : 64) *
					     main_interp * ch_interp);
#else
	bit_rate = bit_rate / (jesd_l * (jesd204b_en > 0 ? 8 : 64) *
			       main_interp * ch_interp);
#endif

	device->dev_info.jesd_rx_lane_rate = bit_rate;

	err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_MSG,
				       "jrx bit rate is %llu bps", bit_rate);
	AD9081_ERROR_RETURN(err);

	*bit_rate_bps = bit_rate;
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_pll_load_cbus_table(adi_ad9081_device_t *device)
{
	int32_t err;
	uint8_t i;
	const uint8_t pll_cbus_table[9][2] = {
		{ 0x8D, 0x64 }, /* sel_regref_lcpll_rc@ADDR_PLL_CBUS_PLL_REG1 */
		{ 0x8E, 0xAC }, /* regthresh_lcpll_rc@ADDR_PLL_CBUS_PLL_REG2 */
		{ 0x93,
		  0x54 }, /* vcobiastcf_lcpll_rc@ADDR_PLL_CBUS_PLL_VCO_BIAS */
		{ 0xB1,
		  0x20 }, /* vcovardacinovd_lcpll_rc lower bits @ADDR_PLL_CBUS_PLL_VAR4 */
		{ 0xB2,
		  0x02 }, /* vcovardacinovd_lcpll_rc upper bits @ADDR_PLL_CBUS_PLL_VAR5 */
		{ 0xB5, 0x83 }, /* not sure @ADDR_PLL_CBUS_PLL_SPARE2 */
		{ 0xB6, 0x70 }, /* ptatslope_lcpll_rc @ADDR_PLL_CBUS_PLL_TEMP */
		{ 0xD3,
		  0x10 }, /* vco_band_init_lcpll_rc @ADDR_PLL_CBUS_PRE_TEMP_DELAY_CTL */
		{ 0x8C,
		  0x35 }, /* fll_ppmcount_lcpll_rc = 3 @ADDR_PLL_CBUS_PLL_FLL */
	};
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 9; i++) {
		err = adi_ad9081_hal_cbuspll_reg_set(
			device, pll_cbus_table[i][0], pll_cbus_table[i][1]);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_pll_default_changes(adi_ad9081_device_t *device)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_LCPLL_RST_ADDR,
				    BF_RSTB_LCPLL_RC_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LCPLL_RST_ADDR,
				    BF_RSTB_LCPLL_RC_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 10000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_jesd_pll_load_cbus_table(device);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_pll_startup(adi_ad9081_device_t *device,
				       uint64_t bit_rate, uint8_t jesd204b_en)
{
	int32_t err;
	uint8_t i, b_lcpll, div_m, div_p, ref_in_div, rx_div_rate,
		jesd_pll_locked, jesd_f;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* startupPLL()@AD9081_serdes_r0.py */

	/* spi default changes, spiDefaultChanges()@ad9081_serdes_jpll_r0.py */
	err = adi_ad9081_jesd_pll_default_changes(device);
	AD9081_ERROR_RETURN(err);

	/* override OctaveCal */
	err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, 0x000004AA, 0x00000800, &jesd_f, 1);
	AD9081_ERROR_RETURN(err);
	jesd_f = jesd_f + 1;
	ref_in_div = ((jesd204b_en == 0) &&
		      (bit_rate > AD9081_DESER_MODE_204C_BR_TRESH)) ?
			     1 :
			     0;
	div_m = ((jesd204b_en == 0) &&
		 (bit_rate > AD9081_DESER_MODE_204C_BR_TRESH)) ?
			0 :
			1;
	div_p = ((jesd204b_en > 0)) ? 0 : 1;
	b_lcpll = (((jesd_f == 3) || (jesd_f == 6) || (jesd_f == 12)) ? 3 : 1) *
		  (jesd204b_en > 0 ? 5 : 11);
	if (bit_rate <= 2000000000ULL) {
		AD9081_LOG_ERR("jrx bit rate is lower than 2Gbps.");
		return API_CMS_ERROR_ERROR;
	} else if (bit_rate > 2000000000ULL && bit_rate <= 4000000000ULL) {
		b_lcpll = b_lcpll * 4;
		rx_div_rate = 1;
	} else if (bit_rate > 4000000000ULL && bit_rate <= 8000000000ULL) {
		b_lcpll = b_lcpll * 2;
		rx_div_rate = 0;
	} else if (bit_rate > 8000000000ULL) {
		rx_div_rate = 8;
	}
	err = adi_ad9081_hal_3bf_set(device, REG_LCPLL_REF_CLK_DIV1_REG_ADDR,
				     BF_SERDES_PLL_BF_0_INFO, ref_in_div,
				     BF_DIVM_LCPLL_RC_INFO, div_m,
				     BF_SERDES_PLL_BF_1_INFO,
				     div_p); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIV2_ADDR,
				    BF_SERDES_PLL_BF_9_INFO,
				    b_lcpll); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_RXDIVRATE_ADDR,
				    BF_SERDES_PLL_BF_7_INFO,
				    rx_div_rate); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_SERDES_PLL_BF_5_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_REFCLK_CPL_ADDR,
				    BF_SERDES_PLL_BF_8_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_SERDES_PLL_BF_3_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_SERDES_PLL_BF_4_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_SERDES_PLL_BF_6_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENCAL_ADDR,
				    BF_FIXED_SERDES_PLL_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENABLE_CTRL_ADDR,
				    BF_PWRUP_LCPLL_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 100000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENABLE_CTRL_ADDR,
				    BF_PWRUP_LCPLL_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* check jesd pll lock status */
	for (i = 0; i < 10; i++) {
		err = adi_ad9081_hal_delay_us(device, 100000);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_pll_lock_status_get(device,
							  &jesd_pll_locked);
		AD9081_ERROR_RETURN(err);
		if (jesd_pll_locked == 1)
			break;
	}
	if (jesd_pll_locked == 0) {
		AD9081_LOG_ERR("JESD PLL is not locked.");
		return API_CMS_ERROR_JESD_PLL_NOT_LOCKED;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_power_down_des(adi_ad9081_device_t *device)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_MASTER_PD_ADDR,
				    BF_PD_MASTER_RC_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PHY_PD_ADDR,
				    BF_PD_DES_RC_CH_INFO, 0xff); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENCAL_ADDR,
				    BF_PD_TXCLK_DIST_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_bring_up(adi_ad9081_device_t *device,
				    adi_ad9081_jesd_link_select_e links,
				    uint8_t lanes)
{
	int32_t err;
	uint8_t jesd204b_en;
	uint64_t bit_rate;
	adi_ad9081_deser_mode_e des_rate;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* startupTx()@ad9081_r1.py */

	/* setup qbd, setupQBD()@AD9081_serdes_r0.py */
	err = adi_ad9081_jesd_rx_lanes_xbar_set(
		device, AD9081_LINK_0,
		device->serdes_info.des_settings.lane_mapping[0]);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_jesd_rx_lanes_xbar_set(
		device, AD9081_LINK_1,
		device->serdes_info.des_settings.lane_mapping[1]);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_MASTER_PD_ADDR,
				    BF_PD_MASTER_RC_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PHY_PD_ADDR,
				    BF_PD_DES_RC_CH_INFO,
				    (uint8_t)(0xff & ~lanes)); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* calculate bit rate */
	err = adi_ad9081_jesd_rx_bit_rate_get(device, &bit_rate);
	AD9081_ERROR_RETURN(err);

	/* startup jesd pll */
	err = adi_ad9081_hal_bf_get(device, REG_JRX_DL_204B_2_ADDR,
				    BF_JRX_DL_204B_ENABLE_INFO, &jesd204b_en,
				    1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_jesd_rx_pll_startup(device, bit_rate, jesd204b_en);
	AD9081_ERROR_RETURN(err);

	/* startup deserializer */
	des_rate = (jesd204b_en > 0) ?
			   ((bit_rate > AD9081_DESER_MODE_204B_BR_TRESH) ?
				    AD9081_HALF_RATE :
				    AD9081_FULL_RATE) :
			   ((bit_rate < AD9081_DESER_MODE_204C_BR_TRESH) ?
				    AD9081_HALF_RATE :
				    AD9081_QUART_RATE);
	err = adi_ad9081_jesd_rx_startup_des(device, des_rate);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_link_enable_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t link_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(
		device, REG_GENERAL_JRX_CTRL_ADDR, BF_LINK_EN_INFO,
		link_en); /* LINK_EN use 2bits for link0 & link1 */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_config_status_get(adi_ad9081_device_t *device,
					     uint8_t *cfg_valid)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(cfg_valid);

	err = adi_ad9081_hal_bf_get(device, REG_JESD_MODE_ADDR,
				    BF_MODE_NOT_IN_TABLE_INFO, cfg_valid,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_link_status_get(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint16_t *status)
{
	int32_t err;
	uint8_t i, cgs, fs, ils;
	uint8_t state_204c;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(status);

	*status = 0;
	err = adi_ad9081_jesd_rx_link_select_set(device, links);
	AD9081_ERROR_RETURN(err);
	for (i = 0; i < 8; i++) {
		cgs = fs = ils = 0;
		err = adi_ad9081_hal_3bf_get(device,
					     REG_JRX_DL_204B_20_LANE0_ADDR + i,
					     BF_JRX_DL_204B_CGS_0_INFO, &cgs,
					     BF_JRX_DL_204B_FS_0_INFO, &fs,
					     BF_JRX_DL_204B_ILS_0_INFO, &ils,
					     1); /* paged */
		AD9081_ERROR_RETURN(err);
		*status |= (((cgs == 0) || (fs == 0) || (ils == 0)) ? 0 : 1)
			   << i;
	}
	err = adi_ad9081_hal_bf_get(device, REG_JRX_DL_204C_0_ADDR,
				    BF_JRX_DL_204C_STATE_INFO, &state_204c,
				    1); /* paged */
	AD9081_ERROR_RETURN(err);
	*status |= state_204c << 8; /* 6 means link up */

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_204c_crc_irq_enable(adi_ad9081_device_t *device,
				       adi_ad9081_jesd_link_select_e links,
				       uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_204C_IRQ_ADDR,
					    BF_JRX_204C_CRC_IRQ_ENABLE_INFO,
					    enable); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JRX_204C_IRQ_ADDR,
					    BF_JRX_204C_CRC_IRQ_ENABLE_INFO,
					    enable); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_204c_crc_irq_status_get(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t *status)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_CRC_IRQ_INFO_R1,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_CRC_IRQ_INFO_R2,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_CRC_IRQ_INFO_R1,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_CRC_IRQ_INFO_R2,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_204c_crc_irq_clr(adi_ad9081_device_t *device,
					    adi_ad9081_jesd_link_select_e links)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_CRC_IRQ_INFO_R1,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_CRC_IRQ_INFO_R2,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_CRC_IRQ_INFO_R1,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_CRC_IRQ_INFO_R2,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_204c_mb_irq_enable(adi_ad9081_device_t *device,
				      adi_ad9081_jesd_link_select_e links,
				      uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(
				device, REG_JRX_204C_IRQ_ADDR,
				BF_JRX_204C_MB_IRQ_ENABLE_INFO_R1,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(
				device, REG_JRX_204C_IRQ_ADDR,
				BF_JRX_204C_MB_IRQ_ENABLE_INFO_R2,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(
				device, REG_JRX_204C_IRQ_ADDR,
				BF_JRX_204C_MB_IRQ_ENABLE_INFO_R1,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(
				device, REG_JRX_204C_IRQ_ADDR,
				BF_JRX_204C_MB_IRQ_ENABLE_INFO_R2,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_204c_mb_irq_status_get(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t *status)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_MB_IRQ_INFO_R1,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_MB_IRQ_INFO_R2,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_MB_IRQ_INFO_R1,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_MB_IRQ_INFO_R2,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_204c_mb_irq_clr(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_MB_IRQ_INFO_R1,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_MB_IRQ_INFO_R2,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_MB_IRQ_INFO_R1,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_MB_IRQ_INFO_R2,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_204c_sh_irq_enable(adi_ad9081_device_t *device,
				      adi_ad9081_jesd_link_select_e links,
				      uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(
				device, REG_JRX_204C_IRQ_ADDR,
				BF_JRX_204C_SH_IRQ_ENABLE_INFO_R1,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(
				device, REG_JRX_204C_IRQ_ADDR,
				BF_JRX_204C_SH_IRQ_ENABLE_INFO_R2,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(
				device, REG_JRX_204C_IRQ_ADDR,
				BF_JRX_204C_SH_IRQ_ENABLE_INFO_R1,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(
				device, REG_JRX_204C_IRQ_ADDR,
				BF_JRX_204C_SH_IRQ_ENABLE_INFO_R2,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_204c_sh_irq_status_get(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t *status)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_SH_IRQ_INFO_R1,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_SH_IRQ_INFO_R2,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_SH_IRQ_INFO_R1,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_get(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_SH_IRQ_INFO_R2,
						    status, 1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_204c_sh_irq_clr(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_SH_IRQ_INFO_R1,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_SH_IRQ_INFO_R2,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_SH_IRQ_INFO_R1,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JRX_204C_IRQ_ADDR,
						    BF_JRX_204C_SH_IRQ_INFO_R2,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_ctle_config_set(adi_ad9081_device_t *device,
					   uint8_t lanes, uint8_t il_db)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (il_db >= AD9081_IL_CTLE_UPPER_DB_THRESH) {
		err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x04, 0x66, lanes);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x05, 0x08, lanes);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x06, 0x03, lanes);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x07, 0x63, lanes);
		AD9081_ERROR_RETURN(err);
	}
	if (il_db < AD9081_IL_CTLE_UPPER_DB_THRESH) {
		err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x04, 0x66, lanes);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x05, 0x08, lanes);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x06, 0x03, lanes);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x07, 0x65, lanes);
		AD9081_ERROR_RETURN(err);
	}

	/* toggle link enable, required for R2. doesn't impact previous silicon revisions */
	err = adi_ad9081_hal_cbusjrx_reg_set(device, 0xF5, 0x00, lanes);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(device, 0xF5, 0x10, lanes);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(device, 0xF5, 0x00, lanes);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_ctle_manual_config_set(adi_ad9081_device_t *device,
						  uint8_t lane)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x04,
		device->serdes_info.des_settings.ctle_coeffs[lane][0],
		1 << lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x05,
		device->serdes_info.des_settings.ctle_coeffs[lane][1],
		1 << lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x06,
		device->serdes_info.des_settings.ctle_coeffs[lane][2],
		1 << lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x07,
		device->serdes_info.des_settings.ctle_coeffs[lane][3],
		1 << lane);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_ctle_manual_config_get(adi_ad9081_device_t *device,
						  uint8_t lane)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x04,
		&(device->serdes_info.des_settings.ctle_coeffs[lane][0]), lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x05,
		&(device->serdes_info.des_settings.ctle_coeffs[lane][1]), lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x06,
		&(device->serdes_info.des_settings.ctle_coeffs[lane][2]), lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x07,
		&(device->serdes_info.des_settings.ctle_coeffs[lane][3]), lane);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_link_select_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_reg_set(device, REG_JTX_PAGE_ADDR, links);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_tx_force_digital_reset_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t reset)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		adi_ad9081_hal_bf_set(device, REG_FORCE_LINK_RESET_REG_ADDR,
				      BF_FORCE_LINK_DIGITAL_RESET_INFO, reset);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		adi_ad9081_hal_bf_set(device, REG_FORCE_LINK_RESET_REG_ADDR,
				      BF_FORCE_LINK_DIGITAL_RESET_INFO, reset);
	}

	err = adi_ad9081_hal_delay_us(device, 100000);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

uint64_t adi_ad9081_jesd_tx_calc_bit_rate(adi_ad9081_device_t *device,
					  uint8_t l, uint8_t m, uint8_t np,
					  uint8_t dcm, uint8_t jesd204b_en)
{
	uint64_t bit_rate;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	bit_rate = np * m * device->dev_info.adc_freq_hz *
		   (jesd204b_en > 0 ? 10 : 66);
#ifdef __KERNEL__
	bit_rate = div_u64(bit_rate, l * dcm * (jesd204b_en > 0 ? 8 : 64));
#else
	bit_rate = bit_rate / (l * dcm * (jesd204b_en > 0 ? 8 : 64));
#endif

	return bit_rate;
}

int32_t adi_ad9081_jesd_tx_bit_rate_get(adi_ad9081_device_t *device,
					adi_ad9081_jesd_link_select_e link,
					uint64_t *bit_rate_bps)
{
	int32_t err;
	uint8_t jesd_l, jesd_m, jesd_np, jesd_dcm, jesd204c_en;
	uint64_t bit_rate;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* get configuration */
	err = adi_ad9081_jesd_tx_link_select_set(device, link);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_JTX_L0_3_ADDR,
				    BF_JTX_L_CFG_INFO, &jesd_l, 1); /* paged */
	AD9081_ERROR_RETURN(err);
	jesd_l = jesd_l + 1;
	err = adi_ad9081_hal_bf_get(device, REG_JTX_L0_6_ADDR,
				    BF_JTX_M_CFG_INFO, &jesd_m, 1); /* paged */
	AD9081_ERROR_RETURN(err);
	jesd_m = jesd_m + 1;
	err = adi_ad9081_hal_bf_get(device, REG_JTX_L0_8_ADDR,
				    BF_JTX_NP_CFG_INFO, &jesd_np,
				    1); /* paged */
	AD9081_ERROR_RETURN(err);
	jesd_np = jesd_np + 1;
	err = adi_ad9081_hal_bf_get(device, REG_CHIP_DECIMATION_RATIO_ADDR,
				    BF_CHIP_DECIMATION_RATIO_INFO, &jesd_dcm,
				    1); /* paged */
	AD9081_ERROR_RETURN(err);
	if (jesd_dcm == 0) {
		AD9081_LOG_ERR("chip decimation is 0.");
		return API_CMS_ERROR_ERROR;
	}

	err = adi_ad9081_hal_bf_get(device, REG_JTX_CORE_1_ADDR,
				    BF_JTX_LINK_204C_SEL_INFO, &jesd204c_en,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	bit_rate = adi_ad9081_jesd_tx_calc_bit_rate(device, jesd_l, jesd_m,
						    jesd_np, jesd_dcm,
						    jesd204c_en > 0 ? 0 : 1);
	err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_MSG,
				       "jtx link%d bit rate is %llu bps",
				       (link & AD9081_LINK_0) > 0 ? 0 : 1,
				       bit_rate);
	AD9081_ERROR_RETURN(err);

	*bit_rate_bps = bit_rate;
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_pll_startup(adi_ad9081_device_t *device,
				       uint64_t bit_rate, uint8_t jesd204b_en)
{
	int32_t err;
	uint8_t i, b_lcpll, div_m, div_p, ref_in_div, sdsrefclk_ratio,
		jesd_pll_locked, lcpll_count;
	uint8_t jesd204b_lcpll[] = { 5, 10, 15, 20, 30, 40 };
	uint8_t jesd204c_lcpll[] = { 11, 22, 33, 44 };
	uint8_t *jesd204_lcpll;
	uint64_t a, b;
	uint8_t c;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(bit_rate == 0);

	/* _configurePll()@ad9081_rx_r1.py */

	/* enable sedespllrefclk period calc coming from spi value instead of from tx mode settings, startupRx()@ad9081_r1.py */
	err = adi_ad9081_hal_bf_set(device,
				    REG_MANUAL_SEDESPLLREFCLK_PERIOD2_ADDR,
				    BF_SDSPLLREFCLK_DIV_SPI_EN_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* calculate jesd pll params */
	ref_in_div = ((jesd204b_en == 0) &&
		      (bit_rate > AD9081_DESER_MODE_204C_BR_TRESH)) ?
			     1 :
			     0;
	div_m = ((jesd204b_en == 0) &&
		 (bit_rate > AD9081_DESER_MODE_204C_BR_TRESH)) ?
			0 :
			1;
	div_p = ((jesd204b_en > 0)) ? 0 : 1;
	jesd204_lcpll = (jesd204b_en > 0) ? jesd204b_lcpll : jesd204c_lcpll;
	lcpll_count = (jesd204b_en > 0) ? NELEMS(jesd204b_lcpll) :
					  NELEMS(jesd204c_lcpll);

	sdsrefclk_ratio = 1;
	for (i = 0; i < lcpll_count; i++) {
		a = device->dev_info.dac_freq_hz * jesd204_lcpll[i] *
		    (jesd204b_en > 0 ? 40 : 66);
		b = bit_rate * 4 * (jesd204b_en > 0 ? 5 : 11);
#ifdef __KERNEL__
		c = (uint8_t)div64_u64(a, b);
#else
		c = (uint8_t)(a / b);
#endif
		if (a == (b * c)) {
			b_lcpll = jesd204_lcpll[i];
			sdsrefclk_ratio = c;
			break;
		}
	}
	if (i == lcpll_count) {
		AD9081_LOG_ERR("SDSPLLREFCLK divider not found.");
		return API_CMS_ERROR_JESD_PLL_NOT_LOCKED;
	}
	if (bit_rate > 4000000000ULL && bit_rate <= 8000000000ULL) {
		b_lcpll = b_lcpll * 2;
	} else if (bit_rate > 2000000000ULL && bit_rate <= 4000000000ULL) {
		b_lcpll = b_lcpll * 4;
	} else if (bit_rate > 1000000000ULL && bit_rate <= 2000000000ULL) {
		b_lcpll = b_lcpll * 8;
	} else if (bit_rate > 500000000ULL && bit_rate <= 1000000000ULL) {
		b_lcpll = b_lcpll * 16;
	} else if (bit_rate > 250000000ULL && bit_rate <= 500000000ULL) {
		b_lcpll = b_lcpll * 32;
	}

	/* spi default changes, spiDefaultChanges()@ad9081_serdes_jpll_r0.py */
	err = adi_ad9081_jesd_pll_default_changes(device);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_3bf_set(device, REG_LCPLL_REF_CLK_DIV1_REG_ADDR,
				     BF_SERDES_PLL_BF_0_INFO, ref_in_div,
				     BF_DIVM_LCPLL_RC_INFO, div_m,
				     BF_SERDES_PLL_BF_1_INFO,
				     div_p); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIV2_ADDR,
				    BF_SERDES_PLL_BF_9_INFO,
				    b_lcpll); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device,
				    REG_MANUAL_SEDESPLLREFCLK_PERIOD2_ADDR,
				    BF_SDSPLLREFCLK_DIV_RATIO_SPI_INFO,
				    sdsrefclk_ratio - 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_REFCLK_CPL_ADDR,
				    BF_SERDES_PLL_BF_8_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_4bf_set(
		device, REG_PLL_DIVOVD_ADDR, BF_SERDES_PLL_BF_5_INFO, 1,
		BF_SERDES_PLL_BF_3_INFO, 1, BF_SERDES_PLL_BF_4_INFO, 1,
		BF_SERDES_PLL_BF_6_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENCAL_ADDR,
				    BF_FIXED_SERDES_PLL_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENABLE_CTRL_ADDR,
				    BF_PWRUP_LCPLL_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 100000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENABLE_CTRL_ADDR,
				    BF_PWRUP_LCPLL_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* check jesd pll lock status */
	for (i = 0; i < 10; i++) {
		err = adi_ad9081_hal_delay_us(device, 100000);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_pll_lock_status_get(device,
							  &jesd_pll_locked);
		AD9081_ERROR_RETURN(err);
		if (jesd_pll_locked == 1)
			break;
	}
	if (jesd_pll_locked == 0) {
		AD9081_LOG_ERR("JESD PLL is not locked.");
		return API_CMS_ERROR_JESD_PLL_NOT_LOCKED;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_link_conv_sel_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2], uint8_t jesd_m[2])
{
	int32_t err;
	uint8_t i, link, m;
	uint8_t vc_no_connect;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* find a DFormat out to use for unconnected virtual converters */
	vc_no_connect = adi_ad9081_jesd_determine_common_nc(
		links, jesd_conv_sel, jesd_m);

	/* set virtual converter for specified links */
	for (i = 0; i < 2; i++) {
		link = (uint8_t)(links & (AD9081_LINK_0 << i));
		if (link > 0) {
			/* set virtual converter mask */
			for (m = 0; m < 16; m++) {
				err = adi_ad9081_jesd_tx_conv_mask_set(
					device, link, m,
					((m + 1) > jesd_m[i]) ? 1 : 0);
				AD9081_ERROR_RETURN(err);
			}

			m = 0;
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0x0,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converter0_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0x1,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converter1_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0x2,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converter2_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0x3,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converter3_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0x4,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converter4_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0x5,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converter5_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0x6,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converter6_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0x7,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converter7_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0x8,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converter8_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0x9,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converter9_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0xa,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_convertera_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0xb,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converterb_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0xc,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converterc_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0xd,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converterd_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0xe,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_convertere_index);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_conv_sel_set(
				device, link, 0xf,
				(++m > jesd_m[i]) ?
					vc_no_connect :
					jesd_conv_sel[i]
						.virtual_converterf_index);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_link_config_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   adi_cms_jesd_param_t *jesd_param)
{
	int32_t err;
	uint8_t i, j, link, ser_lanes = 0x0;
	uint8_t jesd_dcm[2], jesd_link_async[2], jesd204b_en, jesd_pll_locked,
		jesd_bit_repeat_ratio, div_m, adc_div, jesd_brr[2];
	uint32_t rx_link_lmfc_periods[2], rx_link_lmfc_period, rx_tx_lmfc_lcm,
		lcm_remainder, lcm_gcd;
	uint32_t a, b, c;
	uint64_t bit_rate[2];
	adi_cms_chip_op_mode_t chip_mode;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(jesd_param);

	/* calculate bit rate, _calcJtxLinkLaneRate()@ad9081_rx_r1.py */
	bit_rate[0] = 0ULL;
	bit_rate[1] = 0ULL;
	jesd204b_en = jesd_param[0].jesd_jesdv < 2 ? 1 : 0;
	for (i = 0; i < 2; i++) {
		link = (uint8_t)(links & (AD9081_LINK_0 << i));
		if (link > 0) {
			err = adi_ad9081_jesd_tx_link_select_set(device, link);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(
				device, REG_CHIP_DECIMATION_RATIO_ADDR,
				BF_CHIP_DECIMATION_RATIO_INFO, &jesd_dcm[i],
				1); /* paged */
			AD9081_ERROR_RETURN(err);
			if (jesd_dcm[i] == 0) {
				AD9081_LOG_ERR("chip decimation is 0.");
				return API_CMS_ERROR_ERROR;
			}
			bit_rate[i] = adi_ad9081_jesd_tx_calc_bit_rate(
				device, jesd_param[i].jesd_l,
				jesd_param[i].jesd_m, jesd_param[i].jesd_np,
				jesd_dcm[i], jesd204b_en);
			err = adi_ad9081_hal_log_write(
				device, ADI_CMS_LOG_MSG,
				"jtx link%d bit rate is %llu bps", i,
				bit_rate[i]);
			AD9081_ERROR_RETURN(err);
		}
	}

	/* startup jesd pll if using rx only, setupJtx()@ad9081_rx_r1.py */
	err = adi_ad9081_jesd_pll_lock_status_get(device, &jesd_pll_locked);
	AD9081_ERROR_RETURN(err);
	if (jesd_pll_locked == 0) {
		/* Determine chip mode = (rx only) */
		chip_mode = RX_ONLY;
		/* _configurePll()@ad9081_rx_r1.py */
		err = adi_ad9081_jesd_tx_pll_startup(
			device,
			(bit_rate[0] > bit_rate[1]) ? bit_rate[0] : bit_rate[1],
			jesd204b_en);
		AD9081_ERROR_RETURN(err);
		/* _configureLmfcPeriod()@ad9081_rx_r1.py */
		err = adi_ad9081_hal_bf_get(device, REG_ADC_DIVIDER_CTRL_ADDR,
					    BF_ADCDIVN_DIVRATIO_SPI_INFO,
					    &adc_div, 1);
		AD9081_ERROR_RETURN(err);
		adc_div += 1;
		rx_tx_lmfc_lcm = 1;
		rx_link_lmfc_periods[0] = 0;
		rx_link_lmfc_periods[1] = 0;
		for (i = 0; i < 2; i++) {
			link = (uint8_t)(links & (AD9081_LINK_0 << i));
			if (link > 0) {
				rx_link_lmfc_periods[i] =
					(adc_div * jesd_param[i].jesd_s *
					 jesd_param[i].jesd_k * jesd_dcm[i]) >>
					2;
			}
		}
		rx_link_lmfc_period =
			(rx_link_lmfc_periods[0] > rx_link_lmfc_periods[1]) ?
				rx_link_lmfc_periods[0] :
				rx_link_lmfc_periods[1];
		if (link == AD9081_LINK_ALL) {
			a = (rx_link_lmfc_periods[0] >
			     rx_link_lmfc_periods[1]) ?
				    rx_link_lmfc_periods[0] :
				    rx_link_lmfc_periods[1];
			b = (rx_link_lmfc_periods[0] >
			     rx_link_lmfc_periods[1]) ?
				    rx_link_lmfc_periods[1] :
				    rx_link_lmfc_periods[0];
			lcm_remainder = a % b;
			while (lcm_remainder > 0) {
				a = b;
				b = lcm_remainder;
				lcm_remainder = a % b;
			}
			lcm_gcd = b;
			rx_link_lmfc_period = (rx_link_lmfc_periods[0] *
					       rx_link_lmfc_periods[1]) /
					      lcm_gcd;
		}
		if (rx_link_lmfc_period > 2047) {
			rx_link_lmfc_period = rx_link_lmfc_period >> 3;
			rx_tx_lmfc_lcm = 8;
		}
		if (rx_link_lmfc_period < 32) {
			rx_link_lmfc_period = ((32 / rx_link_lmfc_period) + 1) *
					      rx_link_lmfc_period;
		}
		err = adi_ad9081_hal_bf_set(device, REG_RX_TX_LMFC_LCM_ADDR,
					    BF_RX_TX_LMFC_LCM_INFO,
					    rx_tx_lmfc_lcm - 1); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_MANUAL_LMFC_PERIOD0_ADDR,
			BF_LMFC_PERIOD_SPI_INFO,
			rx_link_lmfc_period); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device,
					    REG_MANUAL_LMFC_PERIOD1_ADDR,
					    BF_LMFC_PERIOD_SPI_EN_INFO,
					    1); /* not paged */
		AD9081_ERROR_RETURN(err);
	} else { /* Determine chip mode = (tx + rx only) */
		chip_mode = TX_RX_ONLY;
	}

	/* Set jrx subclass mode for Rx only cases */
	if (chip_mode == RX_ONLY) {
		err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_8_ADDR,
					    BF_JRX_SUBCLASSV_CFG_INFO,
					    jesd_param[i].jesd_subclass);
		AD9081_ERROR_RETURN(err);
	}

	/* power down all physical lanes, setupJtx()@ad9081_rx_r1.py, _enableJtxPhyLanes()@ad9081_rx_r1.py */
	err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
	AD9081_ERROR_RETURN(err);
	for (i = 0; i < 8; i++) {
		err = adi_ad9081_jesd_tx_lane_force_pd_set(device, i, 1);
		AD9081_ERROR_RETURN(err);
	}

	/* startup serializer, setupJtx()@ad9081_rx_r1.py */
	err = adi_ad9081_jesd_tx_power_down_ser(device);
	AD9081_ERROR_RETURN(err);
	for (i = 0; i < 2; i++) {
		link = (uint8_t)(links & (AD9081_LINK_0 << i));
		if (link > 0) {
			for (j = 0; j < 8; j++) {
				if (device->serdes_info.ser_settings
					    .lane_mapping[i][j] <
				    jesd_param[i].jesd_l) {
					ser_lanes += 1 << j;
				}
			}
		}
	}
	err = adi_ad9081_jesd_tx_startup_ser(device, ser_lanes);
	AD9081_ERROR_RETURN(err);

	/* disable jtx link, setupJtx()@ad9081_rx_r1.py */
	err = adi_ad9081_jesd_tx_link_reset(device, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_jesd_tx_link_enable_set(device, AD9081_LINK_ALL, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENABLE_CTRL_ADDR,
				    BF_LCPLL_JTX_PLL_BYPASS_LOCK_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* setup jtx lane mapping, setupJtx@ad9081_rx_r1.py, _setupJtxLanes@ad9081_rx_r1.py */
	err = adi_ad9081_jesd_tx_lanes_xbar_set(
		device, AD9081_LINK_0,
		device->serdes_info.ser_settings.lane_mapping[0]);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_jesd_tx_lanes_xbar_set(
		device, AD9081_LINK_1,
		device->serdes_info.ser_settings.lane_mapping[1]);
	AD9081_ERROR_RETURN(err);

	/* _calcJtxLinkBitRepeatRatios()@ad9081_rx_r1.py */
	err = adi_ad9081_jesd_tx_calc_br_ratio(device, chip_mode, jesd_param,
					       links, bit_rate, jesd_brr);
	AD9081_ERROR_RETURN(err);

	/* configure jtx link framer, _configureJtxLinkFramer()@ad9081_rx_r1.py */
	jesd_link_async[0] = 0;
	jesd_link_async[1] = 0;
	for (i = 0; i < 2; i++) {
		link = (uint8_t)(links & (AD9081_LINK_0 << i));
		if (link > 0) {
			err = adi_ad9081_jesd_tx_link_select_set(device, link);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_K_EMB_QC_OVERRIDE_ADDR,
				BF_JTX_K_EMB_QC_OVERRIDE_INFO, 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_0_ADDR, BF_JTX_DID_CFG_INFO,
				jesd_param[i].jesd_did); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_1_ADDR, BF_JTX_BID_CFG_INFO,
				jesd_param[i].jesd_bid); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(
				device, REG_JTX_L0_3_ADDR, BF_JTX_L_CFG_INFO,
				jesd_param[i].jesd_l - 1, BF_JTX_SCR_CFG_INFO,
				jesd_param[i].jesd_scr); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_4_ADDR, BF_JTX_F_CFG_INFO,
				jesd_param[i].jesd_f - 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_5_ADDR, BF_JTX_K_CFG_INFO,
				jesd_param[i].jesd_k - 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_6_ADDR, BF_JTX_M_CFG_INFO,
				jesd_param[i].jesd_m - 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(
				device, REG_JTX_L0_7_ADDR, BF_JTX_N_CFG_INFO,
				jesd_param[i].jesd_n - 1, BF_JTX_CS_CFG_INFO,
				jesd_param[i].jesd_cs); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(
				device, REG_JTX_L0_8_ADDR, BF_JTX_NP_CFG_INFO,
				jesd_param[i].jesd_np - 1,
				BF_JTX_SUBCLASSV_CFG_INFO,
				jesd_param[i].jesd_subclass); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(
				device, REG_JTX_L0_9_ADDR, BF_JTX_S_CFG_INFO,
				jesd_param[i].jesd_s - 1, BF_JTX_JESDV_CFG_INFO,
				jesd_param[i].jesd_jesdv); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_10_ADDR, BF_JTX_HD_CFG_INFO,
				jesd_param[i].jesd_hd); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_TPL_6_ADDR,
				BF_JTX_TPL_SYSREF_MASK_INFO,
				jesd_param[i].jesd_subclass == 0 ?
					1 :
					0); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_C2R_EN_ADDR,
				BF_JTX_MODE_C2R_EN_INFO,
				jesd_param[i].jesd_mode_c2r_en); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(
				device, REG_JTX_QUICK_CFG_ADDR,
				BF_JTX_MODE_INFO, jesd_param[i].jesd_mode_id,
				BF_JTX_MODE_S_SEL_INFO,
				jesd_param[i].jesd_mode_s_sel); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_DL_204C_1_ADDR,
				BF_JTX_E_CFG_INFO,
				(jesd204b_en > 0) ? 0 :
						    (((jesd_param[i].jesd_k *
						       jesd_param[i].jesd_f) >>
						      8) -
						     1)); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_CORE_1_ADDR,
				BF_JTX_LINK_204C_SEL_INFO,
				(jesd204b_en > 0) ? 0 : 1); /* not paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_CORE_1_ADDR,
				BF_JTX_SYSREF_FOR_STARTUP_INFO,
				jesd_param->jesd_subclass);
			AD9081_ERROR_RETURN(err);

			jesd_bit_repeat_ratio = jesd_brr[i];

			/* _configureJtxLinkBitRepeatLaneStates()@ad9081_rx_r1.py */
			for (j = 0; j < 8; j++) {
				if (device->serdes_info.ser_settings
					    .lane_mapping[i][j] <
				    jesd_param[i].jesd_l) {
					err = adi_ad9081_hal_bf_set(
						device,
						REG_JTX_PHY_IFX_0_LANE0_ADDR +
							j,
						BF_JTX_BR_LOG2_RATIO_0_INFO,
						jesd_bit_repeat_ratio);
					AD9081_ERROR_RETURN(err);
					err = adi_ad9081_jesd_tx_lane_force_pd_set(
						device, j,
						0); /* enable physical lane */
					AD9081_ERROR_RETURN(err);
				}
			}
			a = ((jesd204b_en > 0) ? 1 : 2) * jesd_dcm[i] *
			    jesd_param[i].jesd_s;
			b = (1 << jesd_bit_repeat_ratio) * jesd_param[i].jesd_f;
			c = a / b;
			jesd_link_async[i] = (a == (b * c)) ? 0 : 1;
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_TPL_0_ADDR,
				BF_JTX_CONV_ASYNCHRONOUS_INFO,
				jesd_link_async[i]); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	/* configure jtx clocking, _configureJtxClocking()@ad9081_rx_r1.py */
	if ((jesd_link_async[0] > 0) || (jesd_link_async[1] > 0)) {
		err = adi_ad9081_hal_bf_set(
			device, REG_RESET_CTRL_REG_ADDR,
			BF_FORCE_JTX_DIGITAL_RESET_ON_RSTEN_FORCE_EN_INFO,
			1); /* not paged */
		AD9081_ERROR_RETURN(err);
	}
	err = adi_ad9081_jesd_tx_link_enable_set(device, links, 1);
	AD9081_ERROR_RETURN(err);
	if (jesd204b_en > 0) { /* 204A/B */
		err = adi_ad9081_hal_bf_set(device, REG_PARDATAMODE_SER_ADDR,
					    BF_PARDATAMODE_SER_RC_INFO,
					    2); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_2bf_set(device, REG_SYNCA_CTRL_ADDR,
					     BF_PD_SYNCA_RX_RC_INFO, 0,
					     BF_SYNCA_RX_MODE_RC_INFO,
					     1); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_SER_CLK_INVERT_ADDR,
					    BF_JTX_SER_CLK_INVERT_INFO,
					    0); /* not paged */
		AD9081_ERROR_RETURN(err);
		if (jesd_param->jesd_duallink > 0) {
			err = adi_ad9081_hal_2bf_set(device,
						     REG_SYNCB_CTRL_ADDR,
						     BF_PD_SYNCB_RX_RC_INFO, 0,
						     BF_SYNCB_RX_MODE_RC_INFO,
						     1); /* not paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_jesd_tx_link_select_set(device,
								 AD9081_LINK_1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_JTX_CORE_12_ADDR,
						    BF_JTX_SYNC_N_SEL_INFO,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	} else { /* 204C */
		err = adi_ad9081_hal_bf_set(device, REG_PARDATAMODE_SER_ADDR,
					    BF_PARDATAMODE_SER_RC_INFO,
					    0); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_SYNCA_CTRL_ADDR,
					    BF_PD_SYNCA_RX_RC_INFO,
					    1); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_SYNCB_CTRL_ADDR,
					    BF_PD_SYNCB_RX_RC_INFO,
					    1); /* not paged */
		AD9081_ERROR_RETURN(err);
		if (device->dev_info.dev_rev == 1 ||
		    device->dev_info.dev_rev == 2) { /* r1/r1r */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JTX_SER_CLK_INVERT_ADDR,
						    BF_JTX_SER_CLK_INVERT_INFO,
						    1); /* not paged */
			AD9081_ERROR_RETURN(err);
		}
		if (device->dev_info.dev_rev == 3) { /* r2 */
			err = adi_ad9081_hal_bf_set(device,
						    REG_JTX_SER_CLK_INVERT_ADDR,
						    BF_JTX_SER_CLK_INVERT_INFO,
						    0); /* not paged */
			AD9081_ERROR_RETURN(err);
		}
	}
	err = adi_ad9081_hal_bf_get(device, REG_LCPLL_REF_CLK_DIV1_REG_ADDR,
				    BF_DIVM_LCPLL_RC_INFO, &div_m,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_REF_CLK_DIV1_REG_ADDR,
				    BF_DIVM_LCPLL_RC_RX_INFO,
				    div_m); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* release reset, setupJtx()@ad9081_rx_r1.py */
	err = adi_ad9081_jesd_tx_link_reset(device, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 100000);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_link_reset(adi_ad9081_device_t *device,
				      uint8_t reset)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(reset > 1);

	err = adi_ad9081_hal_bf_set(device, REG_FORCE_LINK_RESET_REG_ADDR,
				    BF_FORCE_LINK_RESET_INFO,
				    reset); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_synca_onchip_term_enable(adi_ad9081_device_t *device,
						    uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_SYNCA_CTRL_ADDR,
				    BF_SYNCA_RX_ONCHIP_TERM_RC_INFO,
				    enable > 0 ? 1 : 0); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_syncb_onchip_term_enable(adi_ad9081_device_t *device,
						    uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_SYNCB_CTRL_ADDR,
				    BF_SYNCB_RX_ONCHIP_TERM_RC_INFO,
				    enable > 0 ? 1 : 0); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_bring_up(adi_ad9081_device_t *device,
				    adi_ad9081_jesd_link_select_e links,
				    uint8_t lanes,
				    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2])
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_lmfc_delay_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint16_t delay)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_TPL_2_ADDR,
					    BF_JTX_TPL_PHASE_ADJUST_INFO,
					    delay);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_TPL_3_ADDR,
					    BF_JTX_TPL_PHASE_ADJUST_INFO,
					    delay);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_link_enable_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t link_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(link_en > 1);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_CORE_13_ADDR,
					    BF_JTX_LINK_EN_INFO,
					    link_en); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_CORE_13_ADDR,
					    BF_JTX_LINK_EN_INFO,
					    link_en); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_link_status_get(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint16_t *status)
{
	int32_t err;
	uint16_t res;
	uint8_t reg8;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(status);

	err = adi_ad9081_jesd_tx_link_select_set(device, links);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_hal_bf_get(device, REG_JTX_DL_204B_4_ADDR,
				    BF_JTX_DL_204B_STATE_INFO, &reg8,
				    1); /* paged, QBF status */
	AD9081_ERROR_RETURN(err);
	res = reg8 & 0x0F;

	err = adi_ad9081_hal_bf_get(device, REG_JTX_DL_204B_3_ADDR,
				    BF_JTX_DL_204B_SYNC_N_INFO, &reg8,
				    1); /* paged, frame sync */
	AD9081_ERROR_RETURN(err);
	res += (reg8 & 0x1) << 4;

	err = adi_ad9081_hal_bf_get(device, REG_PLL_STATUS_ADDR,
				    BF_JTX_PLL_LOCKED_INFO, &reg8,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	res += (reg8 & 0x1) << 5;

	err = adi_ad9081_hal_bf_get(device, REG_PHASE_ESTABLISH_STATUS_ADDR,
				    BF_JTX_PHASE_ESTABLISHED_INFO, &reg8,
				    1); /* paged */
	AD9081_ERROR_RETURN(err);
	res += (reg8 & 0x1) << 6;

	err = adi_ad9081_hal_bf_get(device, REG_QC_MODE_STATUS_ADDR,
				    BF_JTX_INVALID_MODE_INFO, &reg8,
				    1); /* paged */
	AD9081_ERROR_RETURN(err);
	res += (reg8 & 0x1) << 7;

	/* log some other status for possible debug */
	err = adi_ad9081_hal_reg_get(
		device, 0x0636,
		&reg8); /* paged, jtx_tpl_6, bit0:jtx_tpl_invalid_cfg, bit1:jtx_tpl_sysref_rcvd */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_get(
		device, 0x0638,
		&reg8); /* paged, jtx_tpl_8, bit7~0:jtx_tpl_latency_added */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_get(
		device, 0x0639,
		&reg8); /* paged, jtx_tpl_9, bit7~0:jtx_tpl_buf_frames */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_get(
		device, 0x0667,
		&reg8); /* paged, jtx_dl_204c_0, bit3:jtx_dl_204c_sysref_rcvd */
	AD9081_ERROR_RETURN(err);

	*status = res;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_pll_status_get(adi_ad9081_device_t *device,
					  uint8_t *pll_locked)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(pll_locked);

	err = adi_ad9081_hal_bf_get(device, REG_PLL_STATUS_ADDR,
				    BF_JTX_PLL_LOCKED_INFO, pll_locked,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_scrambler_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t scr_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(scr_en > 0x1);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_L0_3_ADDR,
					    BF_JTX_SCR_CFG_INFO,
					    scr_en); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_L0_3_ADDR,
					    BF_JTX_SCR_CFG_INFO,
					    scr_en); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_lane_force_pd_set(adi_ad9081_device_t *device,
					     uint8_t physical_lane,
					     uint8_t power_down)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(physical_lane > 7);
	AD9081_INVALID_PARAM_RETURN(power_down > 1);

	err = adi_ad9081_hal_bf_set(device,
				    REG_JTX_CORE_2_LANE0_ADDR + physical_lane,
				    BF_JTX_FORCE_LANE_PD_0_INFO, power_down);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_lane_invert_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t physical_lane,
					   uint8_t invert_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(physical_lane > 7);
	AD9081_INVALID_PARAM_RETURN(invert_en > 1);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_CORE_2_LANE0_ADDR + physical_lane,
			BF_JTX_LANE_INV_0_INFO, invert_en);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_CORE_2_LANE0_ADDR + physical_lane,
			BF_JTX_LANE_INV_0_INFO, invert_en);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_lane_xbar_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t physical_lane,
					 uint8_t logical_lane)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(logical_lane > 7);
	AD9081_INVALID_PARAM_RETURN(physical_lane > 7);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_CORE_2_LANE0_ADDR + physical_lane,
			BF_JTX_LANE_ASSIGN_0_INFO, logical_lane);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_CORE_2_LANE0_ADDR + physical_lane,
			BF_JTX_LANE_ASSIGN_0_INFO, logical_lane);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_lanes_xbar_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t logical_lanes[8])
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		err = adi_ad9081_jesd_tx_lane_xbar_set(device, links, i,
						       logical_lanes[i]);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_lid_cfg_set(device, links, i,
						     logical_lanes[i]);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_lid_cfg_set(adi_ad9081_device_t *device,
				       adi_ad9081_jesd_link_select_e links,
				       uint8_t lane, uint8_t val)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(lane > 7);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device,
					    REG_JTX_L0_14_LANE0_ADDR + lane,
					    BF_JTX_LID_CFG_0_INFO, val);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device,
					    REG_JTX_L0_14_LANE0_ADDR + lane,
					    BF_JTX_LID_CFG_0_INFO, val);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_lids_cfg_set(adi_ad9081_device_t *device,
					adi_ad9081_jesd_link_select_e links,
					uint8_t lids[8])
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		err = adi_ad9081_jesd_tx_lid_cfg_set(device, links, i, lids[i]);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_conv_mask_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t conv_index, uint8_t val)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(conv_index > 15);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_CORE_0_CONV0_ADDR + conv_index,
			BF_JTX_CONV_MASK_0_INFO, val);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_CORE_0_CONV0_ADDR + conv_index,
			BF_JTX_CONV_MASK_0_INFO, val);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_conv_sel_set(adi_ad9081_device_t *device,
					adi_ad9081_jesd_link_select_e links,
					uint8_t conv_index, uint8_t val)
{
	int32_t err;
	uint8_t i, jesd_m, fddc, overall_dcm, chip_dcm;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(conv_index > 15);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_CORE_0_CONV0_ADDR + conv_index,
			BF_JTX_CONV_SEL_0_INFO, val);
		AD9081_ERROR_RETURN(err);

		/* update chip decimation */
		err = adi_ad9081_hal_bf_get(device, REG_JTX_L0_6_ADDR,
					    BF_JTX_M_CFG_INFO, &jesd_m, 1);
		AD9081_ERROR_RETURN(err);
		jesd_m = jesd_m + 1;
		chip_dcm = 0xff;
		for (i = 0; i < jesd_m; i++) {
			err = adi_ad9081_hal_bf_get(
				device, REG_JTX_CORE_0_CONV0_ADDR + i,
				BF_JTX_CONV_SEL_0_INFO, &fddc, 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_adc_ddc_fine_select_set(
				device, 1 << (fddc >> 1));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device,
						    REG_DDC_OVERALL_DECIM_ADDR,
						    BF_DDC_OVERALL_DECIM_INFO,
						    &overall_dcm, 1);
			AD9081_ERROR_RETURN(err);
			chip_dcm = (chip_dcm > overall_dcm) ? overall_dcm :
							      chip_dcm;
		}
		err = adi_ad9081_adc_chip_dcm_ratio_set(device, AD9081_LINK_0,
							chip_dcm);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_CORE_0_CONV0_ADDR + conv_index,
			BF_JTX_CONV_SEL_0_INFO, val);
		AD9081_ERROR_RETURN(err);

		/* update chip decimation */
		err = adi_ad9081_hal_bf_get(device, REG_JTX_L0_6_ADDR,
					    BF_JTX_M_CFG_INFO, &jesd_m, 1);
		AD9081_ERROR_RETURN(err);
		jesd_m = jesd_m + 1;
		chip_dcm = 0xff;
		for (i = 0; i < jesd_m; i++) {
			err = adi_ad9081_hal_bf_get(
				device, REG_JTX_CORE_0_CONV0_ADDR + i,
				BF_JTX_CONV_SEL_0_INFO, &fddc, 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_adc_ddc_fine_select_set(
				device, 1 << (fddc >> 1));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device,
						    REG_DDC_OVERALL_DECIM_ADDR,
						    BF_DDC_OVERALL_DECIM_INFO,
						    &overall_dcm, 1);
			AD9081_ERROR_RETURN(err);
			chip_dcm = (chip_dcm > overall_dcm) ? overall_dcm :
							      chip_dcm;
		}
		err = adi_ad9081_adc_chip_dcm_ratio_set(device, AD9081_LINK_1,
							chip_dcm);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_ctrl_bit_sel_set(adi_ad9081_device_t *device,
					    adi_ad9081_jesd_link_select_e links,
					    uint8_t bit0_sel, uint8_t bit1_sel,
					    uint8_t bit2_sel)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_CTRL_0_1_SEL_ADDR,
					    BF_DFORMAT_CTRL_BIT_0_SEL_INFO,
					    bit0_sel); /* paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_CTRL_0_1_SEL_ADDR,
					    BF_DFORMAT_CTRL_BIT_1_SEL_INFO,
					    bit1_sel); /* paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_CTRL_2_SEL_ADDR,
					    BF_DFORMAT_CTRL_BIT_2_SEL_INFO,
					    bit2_sel); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_CTRL_0_1_SEL_ADDR,
					    BF_DFORMAT_CTRL_BIT_0_SEL_INFO,
					    bit0_sel); /* paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_CTRL_0_1_SEL_ADDR,
					    BF_DFORMAT_CTRL_BIT_1_SEL_INFO,
					    bit1_sel); /* paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_CTRL_2_SEL_ADDR,
					    BF_DFORMAT_CTRL_BIT_2_SEL_INFO,
					    bit2_sel); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_format_sel_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t format)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		/* 0: 2's complement, 1: offset binary, 2: gray code */
		err = adi_ad9081_hal_bf_set(device, REG_OUT_FORMAT_SEL_ADDR,
					    BF_DFORMAT_SEL_INFO,
					    format); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_OUT_FORMAT_SEL_ADDR,
					    BF_DFORMAT_SEL_INFO,
					    format); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_res_sel_set(adi_ad9081_device_t *device,
				       adi_ad9081_jesd_link_select_e links,
				       uint8_t resolution)
{
	int32_t err;
	uint8_t res_setting = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	if (resolution > 16 || resolution < 8)
		AD9081_INVALID_PARAM_RETURN(device);

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		/* 0: 16bit, 1: 15bit, 2: 14bit, 3: 13bit, 4: 12bit, ..., 8: 8bit */
		res_setting = 16 - resolution;
		err = adi_ad9081_hal_bf_set(device, REG_OUT_RES_ADDR,
					    BF_DFORMAT_RES_INFO,
					    res_setting); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_OUT_RES_ADDR,
					    BF_DFORMAT_RES_INFO,
					    res_setting); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_fractional_delay_converter_selection_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	uint16_t converters)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_FD_SEL_0_ADDR,
					    BF_DFORMAT_FD_SEL_INFO,
					    converters); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_FD_SEL_0_ADDR,
					    BF_DFORMAT_FD_SEL_INFO,
					    converters); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_fbw_config_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t jesd_m[2])
{
	int32_t err;
	uint16_t adcs = 0x00, converters = 0x00;
	uint8_t die_id, dual_adc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_device_die_id_get(device, &die_id);
	AD9081_ERROR_RETURN(err);

	dual_adc = ((die_id & 0x80) == 0) ? 1 : 0;

	if ((links & AD9081_LINK_0) > 0) {
		if (dual_adc) {
			adcs = (jesd_m[0] < 2) ? ((1 << jesd_m[0]) - 1) :
						 ((1 << 2) - 1);
		} else {
			adcs = (jesd_m[0] < 4) ? ((1 << jesd_m[0]) - 1) :
						 ((1 << 4) - 1);
		}
		adcs &= 0x0F;
		converters = (uint16_t)(adcs | (adcs << 4) | (adcs << 8) |
					(adcs << 12));
		err = adi_ad9081_jesd_tx_fbw_sel_set(device, AD9081_LINK_0,
						     converters);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		if (dual_adc) {
			adcs = ((jesd_m[1] < 2) ? ((1 << jesd_m[1]) - 1) :
						  ((1 << 2) - 1))
			       << jesd_m[0];
		} else {
			adcs = ((jesd_m[1] < 4) ? ((1 << jesd_m[1]) - 1) :
						  ((1 << 4) - 1))
			       << jesd_m[0];
		}
		adcs &= 0x0F;
		converters = (uint16_t)(adcs | (adcs << 4) | (adcs << 8) |
					(adcs << 12));
		err = adi_ad9081_jesd_tx_fbw_sel_set(device, AD9081_LINK_1,
						     converters);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_fbw_sel_set(adi_ad9081_device_t *device,
				       adi_ad9081_jesd_link_select_e links,
				       uint16_t converters)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_FBW_SEL_0_ADDR,
					    BF_DFORMAT_FBW_SEL_INFO,
					    converters); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_FBW_SEL_0_ADDR,
					    BF_DFORMAT_FBW_SEL_INFO,
					    converters); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_conv_test_mode_enable_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	uint16_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_TMODE_SEL_0_ADDR,
					    BF_DFORMAT_TMODE_SEL_INFO,
					    enable); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_TMODE_SEL_0_ADDR,
					    BF_DFORMAT_TMODE_SEL_INFO,
					    enable); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_set_swing(adi_ad9081_device_t *device, uint8_t lane,
				     adi_ad9081_ser_swing_e swing)
{
	int32_t err;
	uint32_t addr_table[AD9081_JESD_SER_COUNT][2] = {
		{ REG_JTX_SWING_ADDR, BF_DRVSWING_CH0_SER_RC_INFO },
		{ REG_JTX_SWING_ADDR, BF_DRVSWING_CH1_SER_RC_INFO },
		{ REG_JTX_SWING2_ADDR, BF_DRVSWING_CH2_SER_RC_INFO },
		{ REG_JTX_SWING2_ADDR, BF_DRVSWING_CH3_SER_RC_INFO },
		{ REG_JTX_SWING3_ADDR, BF_DRVSWING_CH4_SER_RC_INFO },
		{ REG_JTX_SWING3_ADDR, BF_DRVSWING_CH5_SER_RC_INFO },
		{ REG_JTX_SWING4_ADDR, BF_DRVSWING_CH6_SER_RC_INFO },
		{ REG_JTX_SWING4_ADDR, BF_DRVSWING_CH7_SER_RC_INFO },
	};
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_INVALID_PARAM_RETURN(lane >= AD9081_JESD_SER_COUNT);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, addr_table[lane][0],
				    addr_table[lane][1], swing); /* not paged */
	AD9081_ERROR_RETURN(err);
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_set_pre_emp(adi_ad9081_device_t *device,
				       uint8_t lane,
				       adi_ad9081_ser_pre_emp_e pre_emp)
{
	int32_t err;
	uint32_t addr_table[AD9081_JESD_SER_COUNT][2] = {
		{ REG_PRE_TAP_LEVEL_CH0_ADDR, BF_DRVPREEM_CH0_SER_RC_INFO },
		{ REG_PRE_TAP_LEVEL_CH1_ADDR, BF_DRVPREEM_CH1_SER_RC_INFO },
		{ REG_PRE_TAP_LEVEL_CH2_ADDR, BF_DRVPREEM_CH2_SER_RC_INFO },
		{ REG_PRE_TAP_LEVEL_CH3_ADDR, BF_DRVPREEM_CH3_SER_RC_INFO },
		{ REG_PRE_TAP_LEVEL_CH4_ADDR, BF_DRVPREEM_CH4_SER_RC_INFO },
		{ REG_PRE_TAP_LEVEL_CH5_ADDR, BF_DRVPREEM_CH5_SER_RC_INFO },
		{ REG_PRE_TAP_LEVEL_CH6_ADDR, BF_DRVPREEM_CH6_SER_RC_INFO },
		{ REG_PRE_TAP_LEVEL_CH7_ADDR, BF_DRVPREEM_CH7_SER_RC_INFO },
	};
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_INVALID_PARAM_RETURN(lane >= AD9081_JESD_SER_COUNT);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, addr_table[lane][0],
				    addr_table[lane][1],
				    pre_emp); /* not paged */
	AD9081_ERROR_RETURN(err);
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_set_post_emp(adi_ad9081_device_t *device,
					uint8_t lane,
					adi_ad9081_ser_post_emp_e post_emp)
{
	int32_t err;
	uint32_t addr_table[AD9081_JESD_SER_COUNT][2] = {
		{ REG_POST_TAP_LEVEL1_ADDR, BF_DRVPOSTEM_CH0_SER_RC_INFO },
		{ REG_POST_TAP_LEVEL1_ADDR, BF_DRVPOSTEM_CH1_SER_RC_INFO },
		{ REG_POST_TAP_LEVEL2_ADDR, BF_DRVPOSTEM_CH2_SER_RC_INFO },
		{ REG_POST_TAP_LEVEL2_ADDR, BF_DRVPOSTEM_CH3_SER_RC_INFO },
		{ REG_POST_TAP_LEVEL3_ADDR, BF_DRVPOSTEM_CH4_SER_RC_INFO },
		{ REG_POST_TAP_LEVEL3_ADDR, BF_DRVPOSTEM_CH5_SER_RC_INFO },
		{ REG_POST_TAP_LEVEL4_ADDR, BF_DRVPOSTEM_CH6_SER_RC_INFO },
		{ REG_POST_TAP_LEVEL4_ADDR, BF_DRVPOSTEM_CH7_SER_RC_INFO },
	};
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_INVALID_PARAM_RETURN(lane >= AD9081_JESD_SER_COUNT);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, addr_table[lane][0],
				    addr_table[lane][1],
				    post_emp); /* not paged */
	AD9081_ERROR_RETURN(err);
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_power_down_ser(adi_ad9081_device_t *device)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_PWR_DN_ADDR, BF_PD_SER_INFO,
				    0xffff); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RSTB_ADDR, BF_RSTB_SER_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_startup_ser(adi_ad9081_device_t *device,
				       uint8_t lanes)
{
	int32_t err;
	uint8_t i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* startUp()@ad9081_serdes_jtx_r0.py */

	/* toggle pown down bits */
	err = adi_ad9081_hal_bf_set(device, REG_PWR_DN_ADDR, BF_PD_SER_INFO,
				    lanes); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 1000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PWR_DN_ADDR, BF_PD_SER_INFO,
				    (0xff & ~lanes)); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* drive slice offsets */
	err = adi_ad9081_hal_4bf_set(device, REG_EN_DRVSLICEOFFSET_ADDR,
				     BF_EN_DRVSLICEOFFSET_CH67_SER_RC_INFO, 1,
				     BF_EN_DRVSLICEOFFSET_CH45_SER_RC_INFO, 1,
				     BF_EN_DRVSLICEOFFSET_CH23_SER_RC_INFO, 1,
				     BF_EN_DRVSLICEOFFSET_CH01_SER_RC_INFO,
				     1); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* swing - pre - post */
	for (i = 0; i < AD9081_JESD_SER_COUNT; ++i) {
		err = adi_ad9081_jesd_tx_set_swing(
			device, i,
			device->serdes_info.ser_settings.lane_settings[i]
				.swing_setting);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_set_pre_emp(
			device, i,
			device->serdes_info.ser_settings.lane_settings[i]
				.pre_emp_setting);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_set_post_emp(
			device, i,
			device->serdes_info.ser_settings.lane_settings[i]
				.post_emp_setting);
		AD9081_ERROR_RETURN(err);
	}

	/* tx invert */
	err = adi_ad9081_hal_reg_set(
		device, REG_MAIN_DATA_INV_ADDR,
		device->serdes_info.ser_settings.invert_mask);
	AD9081_ERROR_RETURN(err);

	/* reset phy */
	err = adi_ad9081_hal_bf_set(device, REG_RSTB_ADDR, BF_RSTB_SER_INFO,
				    lanes); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 1000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RSTB_ADDR, BF_RSTB_SER_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 10000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RSTB_ADDR, BF_RSTB_SER_INFO,
				    lanes); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_sync_mode_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t sync_mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_CORE_12_ADDR,
					    BF_JTX_SYNC_N_SEL_INFO, sync_mode);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_CORE_12_ADDR,
					    BF_JTX_SYNC_N_SEL_INFO, sync_mode);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_phy_prbs_test(adi_ad9081_device_t *device,
				 adi_cms_jesd_prbs_pattern_e prbs_pattern,
				 uint32_t time_ms)
{
	int32_t err;
	uint32_t i;
	uint8_t tmp_reg;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* decode pattern */
	switch (prbs_pattern) {
	case PRBS7:
		tmp_reg = AD9081_JESD_RX_PRBS_TEST_MODE_PRBS7;
		break;
	case PRBS9:
		tmp_reg = AD9081_JESD_RX_PRBS_TEST_MODE_PRBS9;
		break;
	case PRBS15:
		tmp_reg = AD9081_JESD_RX_PRBS_TEST_MODE_PRBS15;
		break;
	case PRBS31:
		tmp_reg = AD9081_JESD_RX_PRBS_TEST_MODE_PRBS31;
		break;
	default:
		return API_CMS_ERROR_INVALID_PARAM;
	}

	/* select phy prbs test source and mode */
	err = adi_ad9081_hal_bf_set(
		device, REG_JRX_TEST_2_ADDR, BF_JRX_PRBS_SOURCE_INFO,
		AD9081_JESD_RX_PRBS_TEST_DATA_SRC_LANE); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_JRX_TEST_2_ADDR,
				    BF_JRX_PRBS_MODE_INFO,
				    tmp_reg); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* clear prbs error count */
	err = adi_ad9081_hal_bf_set(device, REG_JRX_TEST_0_ADDR,
				    BF_JRX_PRBS_LANE_CLEAR_ERRORS_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_JRX_TEST_0_ADDR,
				    BF_JRX_PRBS_LANE_CLEAR_ERRORS_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* update prbs error count */
	err = adi_ad9081_hal_bf_set(device, REG_JRX_TEST_0_ADDR,
				    BF_JRX_PRBS_LANE_UPDATE_ERROR_COUNT_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	for (i = 0; i < time_ms; i++) {
		err = adi_ad9081_hal_delay_us(device, 1000); /* 1 ms */
		AD9081_ERROR_RETURN(err);
	}
	err = adi_ad9081_hal_bf_set(device, REG_JRX_TEST_0_ADDR,
				    BF_JRX_PRBS_LANE_UPDATE_ERROR_COUNT_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_phy_prbs_test_result_get(
	adi_ad9081_device_t *device, uint8_t lane,
	adi_ad9081_prbs_test_t *prbs_rx_result)
{
	int32_t err;
	uint8_t tmp_reg;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(prbs_rx_result);
	AD9081_LOG_FUNC();

	/* read error count */
	err = adi_ad9081_hal_reg_get(device, (REG_JRX_TEST_4_LANE0_ADDR + lane),
				     &tmp_reg); /* not paged */
	AD9081_ERROR_RETURN(err);
	prbs_rx_result->phy_prbs_err_cnt = tmp_reg;
	err = adi_ad9081_hal_reg_get(device, (REG_JRX_TEST_5_LANE0_ADDR + lane),
				     &tmp_reg); /* not paged */
	AD9081_ERROR_RETURN(err);
	prbs_rx_result->phy_prbs_err_cnt += tmp_reg << 8;
	err = adi_ad9081_hal_reg_get(device, (REG_JRX_TEST_6_LANE0_ADDR + lane),
				     &tmp_reg); /* not paged */
	AD9081_ERROR_RETURN(err);
	prbs_rx_result->phy_prbs_err_cnt += tmp_reg << 16;

	/* read prbs test flags */
	err = adi_ad9081_hal_reg_get(device, (REG_JRX_TEST_3_LANE0_ADDR + lane),
				     &tmp_reg); /* not paged */
	AD9081_ERROR_RETURN(err);
	prbs_rx_result->phy_prbs_pass = (tmp_reg & 0xc0) == 0 ? 1 : 0;

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_phy_prbs_test_disable_set(adi_ad9081_device_t *device)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_JRX_TEST_2_ADDR,
				    BF_JRX_PRBS_MODE_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_rx_sample_prbs_test(adi_ad9081_device_t *device,
				    adi_cms_jesd_prbs_pattern_e prbs_pattern,
				    uint8_t lane, uint32_t time_sec)
{
	int32_t err;
	uint32_t i;
	uint8_t tmp_reg;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* decode pattern */
	switch (prbs_pattern) {
	case PRBS7:
		tmp_reg = 1;
		break;
	case PRBS9:
		tmp_reg = 2;
		break;
	case PRBS15:
		tmp_reg = 3;
		break;
	case PRBS23:
		tmp_reg = 4;
		break;
	case PRBS31:
		tmp_reg = 5;
		break;
	default:
		return API_CMS_ERROR_INVALID_PARAM;
	}

	/* select phy prbs test source and mode */
	err = adi_ad9081_hal_bf_set(device, REG_SAMPLE_PRBS_CTRL0_ADDR,
				    BF_PRBS_CHNL_SEL_INFO,
				    lane); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_SAMPLE_PRBS_CTRL1_ADDR,
				    BF_PRBS_MODE_INFO, tmp_reg); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_SAMPLE_PRBS_CTRL0_ADDR,
				    BF_SAMPLE_PRBS_ENABLE_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* clear prbs error count */
	err = adi_ad9081_hal_bf_set(device, REG_SAMPLE_PRBS_CTRL0_ADDR,
				    BF_CLR_ERRORS_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_SAMPLE_PRBS_CTRL0_ADDR,
				    BF_CLR_ERRORS_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* update prbs error count */
	err = adi_ad9081_hal_bf_set(device, REG_SAMPLE_PRBS_CTRL0_ADDR,
				    BF_UPDATE_ERROR_COUNT_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	for (i = 0; i < time_sec; i++) {
		err = adi_ad9081_hal_delay_us(device, 1000000); /* 1s */
		AD9081_ERROR_RETURN(err);
	}
	err = adi_ad9081_hal_bf_set(device, REG_SAMPLE_PRBS_CTRL0_ADDR,
				    BF_UPDATE_ERROR_COUNT_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_sample_prbs_test_result_get(
	adi_ad9081_device_t *device, uint8_t *error_flag,
	uint32_t *error_count_i, uint32_t *error_count_q)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_reg_get(device, REG_SAMPLE_PRBS_STATUS0_ADDR,
				     (uint8_t *)error_flag); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_SAMPLE_PRBS_STATUS1_ADDR,
				    BF_ERROR_COUNT_I_INFO,
				    (uint8_t *)error_count_i,
				    4); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, 0x00002067, 0x00001800,
				    (uint8_t *)error_count_q,
				    4); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_gen_2s_comp(adi_ad9081_device_t *device,
				       int8_t input_value, uint8_t bit_length,
				       uint8_t *data)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((input_value < (-(1 << (bit_length - 1)))) ||
	    (input_value > ((1 << (bit_length - 1)) - 1))) {
		return API_CMS_ERROR_INVALID_PARAM;
	}

	*data = (input_value >= 0) ? input_value :
				     ((1 << bit_length) + input_value);
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_spo_set(adi_ad9081_device_t *device, uint8_t lane,
				   uint8_t spo)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	spo = spo & 0x7f;
	err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x0D, spo, 1 << lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x0D, spo | (1 << 7),
					     1 << lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(device, 0x0D, spo, 1 << lane);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_spo_sweep(adi_ad9081_device_t *device, uint8_t lane,
				     adi_cms_jesd_prbs_pattern_e prbs_pattern,
				     adi_ad9081_deser_mode_e deser_mode,
				     uint32_t prbs_delay_sec, uint8_t *left_spo,
				     uint8_t *right_spo)
{
	int32_t err;
	int16_t i;
	uint8_t spo_value, temp_len;
	uint8_t good_left_spo = 0, good_right_spo = 0;
	adi_ad9081_prbs_test_t prbs_rx_result;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(left_spo);
	AD9081_NULL_POINTER_RETURN(right_spo);

	/* decode deser_mode mode */
	switch (deser_mode) {
	case AD9081_HALF_RATE:
		temp_len = 32;
		break;
	case AD9081_QUART_RATE:
		temp_len = 16;
		break;
	default:
		return API_CMS_ERROR_INVALID_PARAM;
	}

	for (i = 1; i < (temp_len + 1); i++) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, -1 * i, 7,
						     &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_phy_prbs_test(device, prbs_pattern,
						       prbs_delay_sec * 1000);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_phy_prbs_test_result_get(
			device, lane, &prbs_rx_result);
		AD9081_ERROR_RETURN(err);
		if (prbs_rx_result.phy_prbs_err_cnt == 0) {
			good_left_spo = i;
		} else {
			break;
		}
	}
	for (i = good_left_spo; i >= 0; i--) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, -1 * i, 7,
						     &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);
	}

	for (i = 1; i < (temp_len + 1); i++) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, i, 7, &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_phy_prbs_test(device, prbs_pattern,
						       prbs_delay_sec * 1000);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_phy_prbs_test_result_get(
			device, lane, &prbs_rx_result);
		AD9081_ERROR_RETURN(err);
		if (prbs_rx_result.phy_prbs_err_cnt == 0) {
			good_right_spo = i;
		} else {
			break;
		}
	}
	for (i = good_right_spo; i >= 0; i--) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, i, 7, &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);
	}
	*left_spo = good_left_spo;
	*right_spo = good_right_spo;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_qr_vertical_eye_scan(adi_ad9081_device_t *device,
						uint8_t direction, uint8_t lane)
{
	int32_t err;
	uint8_t data, state_go, jrx_at_idle, i;
	const uint8_t max_wait = 50;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_device_spi_register_get(device, 0x21D1, &data);
	AD9081_ERROR_RETURN(err);

	data = ((data & 0xB0) | lane) | (direction << 6);
	err = adi_ad9081_device_spi_register_set(device, 0x21D1, data);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_device_spi_register_set(device, 0x21C0, 0x14);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_device_spi_register_get(device, 0x21C1, &data);
	AD9081_ERROR_RETURN(err);

	data |= 0x01;
	err = adi_ad9081_device_spi_register_set(device, 0x21C1, data);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_hal_delay_us(device, 500);
	AD9081_ERROR_RETURN(err);

	for (i = 0; i < max_wait; i++) {
		err = adi_ad9081_hal_bf_get(device, 0x21dd, 0x100, &jrx_at_idle,
					    1); /* rx_at_idle@rx_state_status */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device, 0x21c1, 0x100, &state_go,
					    1);
		AD9081_ERROR_RETURN(err);

		if (((state_go & 0x01) == 0) && (jrx_at_idle & 0x01)) {
			break;
		}
		err = adi_ad9081_hal_delay_us(device, 10000);
		AD9081_ERROR_RETURN(err);
	}
	if (i == max_wait) {
		err = adi_ad9081_hal_log_write(
			device, ADI_CMS_LOG_ERR,
			"vertical eye scan call timed out");
		AD9081_ERROR_RETURN(err);
		return API_CMS_ERROR_ERROR;
	}
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_qr_two_dim_eye_scan(adi_ad9081_device_t *device,
					       uint8_t lane,
					       uint16_t eye_scan_data[96])
{
	int32_t err;
	uint8_t en_flash_src_des_rc, comp_setting, data, spo, quad1, quad2,
		quad3, quad4, i = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_device_cbusjrx_register_get(
		device, 0xFA, &en_flash_src_des_rc, lane);
	AD9081_ERROR_RETURN(err);

	en_flash_src_des_rc &= 0x0F;

	err = adi_ad9081_device_cbusjrx_register_get(device, 0xCA,
						     &comp_setting, lane);
	AD9081_ERROR_RETURN(err);

	comp_setting &= 0x03F;

	err = adi_ad9081_device_cbusjrx_register_get(device, 0xFA, &data, lane);
	AD9081_ERROR_RETURN(err);

	data = (data & 0x0F) | 0xA0;

	err = adi_ad9081_device_cbusjrx_register_set(device, 0xFA, data,
						     1 << lane);
	AD9081_ERROR_RETURN(err);

	/* Sweeping SPO from -1 to -16 */
	for (spo = 127; spo > 111; spo--) {
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo);
		AD9081_ERROR_RETURN(err);

		/* Step up vertical scan */
		err = adi_ad9081_jesd_rx_qr_vertical_eye_scan(device, 1, lane);
		AD9081_ERROR_RETURN(err);

		err = adi_ad9081_device_spi_register_get(device, 0x21D6,
							 &quad2);
		AD9081_ERROR_RETURN(err);

		/* Step down vertical scan */
		err = adi_ad9081_jesd_rx_qr_vertical_eye_scan(device, 0, lane);
		AD9081_ERROR_RETURN(err);

		err = adi_ad9081_device_spi_register_get(device, 0x21D5,
							 &quad3);
		AD9081_ERROR_RETURN(err);

		eye_scan_data[i] = -(128 - spo);
		eye_scan_data[i + 1] = quad2;
		eye_scan_data[i + 2] = -quad3;
		i += 3;
	}
	for (spo = 113; spo < 128; spo++) {
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo);
		AD9081_ERROR_RETURN(err);
	}
	/* Set spo to 0 */
	err = adi_ad9081_jesd_rx_spo_set(device, lane, 0);
	AD9081_ERROR_RETURN(err);

	/* Sweeping SPO from 1 to 16 */
	for (spo = 1; spo < 17; spo++) {
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo);
		AD9081_ERROR_RETURN(err);

		/* Step up vertical scan */
		err = adi_ad9081_jesd_rx_qr_vertical_eye_scan(device, 1, lane);
		AD9081_ERROR_RETURN(err);

		err = adi_ad9081_device_spi_register_get(device, 0x21D5,
							 &quad1);
		AD9081_ERROR_RETURN(err);

		/* Step down vertical scan */
		err = adi_ad9081_jesd_rx_qr_vertical_eye_scan(device, 0, lane);
		AD9081_ERROR_RETURN(err);

		err = adi_ad9081_device_spi_register_get(device, 0x21D6,
							 &quad4);
		AD9081_ERROR_RETURN(err);

		eye_scan_data[i] = spo;
		eye_scan_data[i + 1] = quad1;
		eye_scan_data[i + 2] = -quad4;
		i += 3;
	}

	/* Move SPO back to the center, 0, one step at a time */
	for (spo = 15; spo > 0; spo--) {
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo);
		AD9081_ERROR_RETURN(err);
	}

	/* Set spo to 0 */
	err = adi_ad9081_jesd_rx_spo_set(device, lane, 0);
	AD9081_ERROR_RETURN(err);

	/* Restore the value stored in step 1 to it corresponding register */
	err = adi_ad9081_device_cbusjrx_register_get(device, 0xFA, &data, lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_device_cbusjrx_register_set(
		device, 0xFA, ((data & 0x0F) | en_flash_src_des_rc), lane);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_hr_vertical_eye_scan(
	adi_ad9081_device_t *device, uint8_t direction, uint8_t lane,
	uint8_t *good_mv, adi_cms_jesd_prbs_pattern_e prbs_pattern,
	uint32_t prbs_delay_ms)
{
	int32_t err;
	uint8_t pol, lower, upper, error_free_b1, curr_b1;
	adi_ad9081_prbs_test_t prbs_rx_result_vert;

	/* Set S0_POLARITY_SWAP 0: positive; 1: negative sweep */
	err = adi_ad9081_device_spi_register_get(device, 0x00D2, &pol);
	AD9081_ERROR_RETURN(err);
	pol |= direction;
	err = adi_ad9081_device_spi_register_set(device, 0x00D2, pol);
	AD9081_ERROR_RETURN(err);

	/* Search for failure voltage */
	lower = 0; /* Lower bound of the search window */
	upper = 63; /* Upper bound of the search window */
	error_free_b1 = 0;
	while (lower < upper) {
		curr_b1 = lower + (upper - lower) / 2;
		if ((curr_b1 == lower) || (curr_b1 == upper)) {
			break;
		}

		err = adi_ad9081_device_cbusjrx_register_set(
			device, 0xCA, curr_b1, 1 << lane);
		AD9081_ERROR_RETURN(err);

		err = adi_ad9081_jesd_rx_phy_prbs_test(device, prbs_pattern,
						       prbs_delay_ms);
		AD9081_ERROR_RETURN(err);

		/* Check data */
		err = adi_ad9081_jesd_rx_phy_prbs_test_result_get(
			device, lane, &prbs_rx_result_vert);
		AD9081_ERROR_RETURN(err);

		if (prbs_rx_result_vert.phy_prbs_err_cnt == 0) {
			lower = curr_b1;
			if (curr_b1 > error_free_b1) {
				error_free_b1 = curr_b1;
			}
		} else {
			upper = curr_b1;
		}
	}
	*good_mv = (4 * error_free_b1);

	/*reset to 0mV before next scan*/
	err = adi_ad9081_device_cbusjrx_register_set(device, 0xCA, 0,
						     1 << lane);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_hr_two_dim_eye_scan(
	adi_ad9081_device_t *device, uint8_t lane,
	adi_cms_jesd_prbs_pattern_e prbs_pattern, uint32_t prbs_delay_ms,
	uint16_t eye_scan_data[192])
{
	int32_t err;
	int8_t i;
	uint8_t spo_value, spo_size = 32, vec_inc = 0;
	int8_t good_left_spo = 0, good_right_spo = 0;
	uint8_t good_pos_mv = 0, good_neg_mv = 0;
	adi_ad9081_prbs_test_t prbs_rx_result;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* Set the comparator voltage to the zero by setting COMP_SETTING 0x00 */
	err = adi_ad9081_device_cbusjrx_register_set(device, 0xCA, 0,
						     1 << lane);
	AD9081_ERROR_RETURN(err);

	/* Left Horizontal eye scan */
	for (i = 1; i < (spo_size + 1); i++) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, -1 * i, 7,
						     &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);

		err = adi_ad9081_jesd_rx_phy_prbs_test(device, prbs_pattern,
						       prbs_delay_ms);
		AD9081_ERROR_RETURN(err);

		/* Check JRx results */
		err = adi_ad9081_jesd_rx_phy_prbs_test_result_get(
			device, lane, &prbs_rx_result);
		AD9081_ERROR_RETURN(err);

		if (prbs_rx_result.phy_prbs_err_cnt == 0) {
			good_left_spo = i;

			/* Negative direction */
			err = adi_ad9081_jesd_rx_hr_vertical_eye_scan(
				device, 1, lane, &good_neg_mv, prbs_pattern,
				prbs_delay_ms);
			AD9081_ERROR_RETURN(err);

			/* Positive direction */
			err = adi_ad9081_jesd_rx_hr_vertical_eye_scan(
				device, 0, lane, &good_pos_mv, prbs_pattern,
				prbs_delay_ms);
			AD9081_ERROR_RETURN(err);

			eye_scan_data[vec_inc] = -(good_left_spo);
			eye_scan_data[vec_inc + 1] = good_pos_mv;
			eye_scan_data[vec_inc + 2] = -(good_neg_mv);
			vec_inc += 3;
		} else {
			eye_scan_data[vec_inc] = 0;
			eye_scan_data[vec_inc + 1] = 0;
			eye_scan_data[vec_inc + 2] = 0;
			vec_inc += 3;
		}
	}

	/* Return SPO to 0 */
	for (i = good_left_spo; i >= 0; i--) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, -1 * i, 7,
						     &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);
	}
	/* Right Horizontal eye scan */
	for (i = 1; i < (spo_size + 1); i++) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, i, 7, &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_phy_prbs_test(device, prbs_pattern,
						       prbs_delay_ms);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_phy_prbs_test_result_get(
			device, lane, &prbs_rx_result);
		AD9081_ERROR_RETURN(err);
		if (prbs_rx_result.phy_prbs_err_cnt == 0) {
			good_right_spo = i;

			/* Negative direction */
			err = adi_ad9081_jesd_rx_hr_vertical_eye_scan(
				device, 1, lane, &good_neg_mv, prbs_pattern,
				prbs_delay_ms);
			AD9081_ERROR_RETURN(err);

			/* Positive direction */
			err = adi_ad9081_jesd_rx_hr_vertical_eye_scan(
				device, 0, lane, &good_pos_mv, prbs_pattern,
				prbs_delay_ms);
			AD9081_ERROR_RETURN(err);

			eye_scan_data[vec_inc] = good_right_spo;
			eye_scan_data[vec_inc + 1] = good_pos_mv;
			eye_scan_data[vec_inc + 2] = -(good_neg_mv);
			vec_inc += 3;
		} else {
			eye_scan_data[vec_inc] = 0;
			eye_scan_data[vec_inc + 1] = 0;
			eye_scan_data[vec_inc + 2] = 0;
			vec_inc += 3;
		}
	}

	/* Return spo to 0 */
	for (i = good_left_spo; i >= 0; i--) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, i, 7, &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_tx_gen_test(adi_ad9081_device_t *device,
			    adi_ad9081_jesd_link_select_e links,
			    adi_ad9081_jesd_tx_test_data_src_e data_source,
			    adi_ad9081_jesd_tx_test_mode_e test_mode)
{
	int32_t err;
	uint8_t i, link;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		link = (uint8_t)(links & (AD9081_LINK_0 << i));
		if (link > 0) {
			/* select link */
			err = adi_ad9081_jesd_tx_link_select_set(device, link);
			AD9081_ERROR_RETURN(err);

			/* stop test */
			err = adi_ad9081_hal_bf_set(device, REG_JTX_CORE_3_ADDR,
						    BF_JTX_TEST_USER_GO_INFO,
						    0); /* paged */
			AD9081_ERROR_RETURN(err);

			/* set data source */
			err = adi_ad9081_hal_bf_set(device, REG_JTX_CORE_3_ADDR,
						    BF_JTX_TEST_GEN_SEL_INFO,
						    data_source); /* paged */
			AD9081_ERROR_RETURN(err);

			/* set test mode */
			err = adi_ad9081_hal_bf_set(device, REG_JTX_CORE_3_ADDR,
						    BF_JTX_TEST_GEN_MODE_INFO,
						    test_mode); /* paged */
			AD9081_ERROR_RETURN(err);

			/* update data */
			err = adi_ad9081_hal_bf_set(device, REG_JTX_CORE_3_ADDR,
						    BF_JTX_TEST_USER_GO_INFO,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_checker_board_test(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_ad9081_jesd_tx_test_data_src_e data_source)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_jesd_tx_gen_test(
		device, links, data_source,
		AD9081_JESD_TX_TEST_MODE_CHECKER_BOARD);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_word_toggle_test(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_ad9081_jesd_tx_test_data_src_e data_source)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_jesd_tx_gen_test(device, links, data_source,
					  AD9081_JESD_TX_TEST_MODE_WORD_TOGGLE);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_tx_ramp_test(adi_ad9081_device_t *device,
			     adi_ad9081_jesd_link_select_e links,
			     adi_ad9081_jesd_tx_test_data_src_e data_source)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_jesd_tx_gen_test(device, links, data_source,
					  AD9081_JESD_TX_TEST_MODE_RAMP);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_repeat_user_data_test(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_ad9081_jesd_tx_test_data_src_e data_source, uint8_t data[9])
{
	int32_t err;
	uint8_t i, link;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		link = (uint8_t)(links & (AD9081_LINK_0 << i));
		if (link > 0) {
			/* select link */
			err = adi_ad9081_jesd_tx_link_select_set(device, link);
			AD9081_ERROR_RETURN(err);

			/* set data */
			err = adi_ad9081_hal_reg_set(device,
						     REG_JTX_CORE_4_ADDR,
						     data[0]); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_reg_set(device,
						     REG_JTX_CORE_5_ADDR,
						     data[1]); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_reg_set(device,
						     REG_JTX_CORE_6_ADDR,
						     data[2]); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_reg_set(device,
						     REG_JTX_CORE_7_ADDR,
						     data[3]); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_reg_set(device,
						     REG_JTX_CORE_8_ADDR,
						     data[4]); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_reg_set(device,
						     REG_JTX_CORE_9_ADDR,
						     data[5]); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_reg_set(device,
						     REG_JTX_CORE_10_ADDR,
						     data[6]); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_reg_set(device,
						     REG_JTX_CORE_11_ADDR,
						     data[7]); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_JTX_CORE_12_ADDR,
						    0x0200,
						    data[8]); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	err = adi_ad9081_jesd_tx_gen_test(device, links, data_source,
					  AD9081_JESD_TX_TEST_MODE_USER_REPEAT);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_tx_phy_prbs_test(adi_ad9081_device_t *device,
				 adi_ad9081_jesd_link_select_e links,
				 adi_cms_jesd_prbs_pattern_e prbs_pattern)
{
	int32_t err;
	uint8_t pattern;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* decode prbs pattern */
	switch (prbs_pattern) {
	case PRBS7:
		pattern = AD9081_JESD_TX_TEST_MODE_PN7;
		break;
	case PRBS15:
		pattern = AD9081_JESD_TX_TEST_MODE_PN15;
		break;
	case PRBS31:
		pattern = AD9081_JESD_TX_TEST_MODE_PN31;
		break;
	default:
		return API_CMS_ERROR_INVALID_PARAM;
	}

	err = adi_ad9081_jesd_tx_gen_test(
		device, links, AD9081_JESD_TX_TEST_DATA_PHY, pattern);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_sample_data_prbs_test(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_cms_jesd_prbs_pattern_e prbs_pattern)
{
	int32_t err;
	uint8_t pattern;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* decode prbs pattern */
	switch (prbs_pattern) {
	case PRBS7:
		pattern = AD9081_JESD_TX_TEST_MODE_PN7;
		break;
	case PRBS15:
		pattern = AD9081_JESD_TX_TEST_MODE_PN15;
		break;
	case PRBS31:
		pattern = AD9081_JESD_TX_TEST_MODE_PN31;
		break;
	default:
		return API_CMS_ERROR_INVALID_PARAM;
	}

	err = adi_ad9081_jesd_tx_gen_test(
		device, links, AD9081_JESD_TX_TEST_DATA_SAMPLE, pattern);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_ilas_test_mode_enable_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_DL_204B_0_ADDR,
			BF_JTX_DL_204B_ILAS_TEST_EN_CFG_INFO, enable);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_DL_204B_0_ADDR,
			BF_JTX_DL_204B_ILAS_TEST_EN_CFG_INFO, enable);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_continuous_d215_enable_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	uint8_t lane_id, uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_DL_204B_6_LANE0_ADDR + lane_id,
			BF_JTX_DL_204B_SCR_DATA_SEL_CFG_0_INFO, enable);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_DL_204B_6_LANE0_ADDR + lane_id,
			BF_JTX_DL_204B_SCR_DATA_SEL_CFG_0_INFO, enable);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_rjspat_enable_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	uint8_t lane_id, uint8_t mode, uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_DL_204B_3_ADDR,
					    BF_JTX_DL_204B_RJSPAT_EN_CFG_INFO,
					    enable);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_DL_204B_3_ADDR,
					    BF_JTX_DL_204B_RJSPAT_SEL_CFG_INFO,
					    mode);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_DL_204B_6_LANE0_ADDR + lane_id,
			BF_JTX_DL_204B_PHY_DATA_SEL_CFG_0_INFO, enable);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_DL_204B_3_ADDR,
					    BF_JTX_DL_204B_RJSPAT_EN_CFG_INFO,
					    enable);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_JTX_DL_204B_3_ADDR,
					    BF_JTX_DL_204B_RJSPAT_SEL_CFG_INFO,
					    mode);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JTX_DL_204B_6_LANE0_ADDR + lane_id,
			BF_JTX_DL_204B_PHY_DATA_SEL_CFG_0_INFO, enable);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_rpat_enable_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   uint8_t lane_id, uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_jesd_tx_rjspat_enable_set(device, links, lane_id, 0x00,
						   enable);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_jspat_enable_set(adi_ad9081_device_t *device,
					    adi_ad9081_jesd_link_select_e links,
					    uint8_t lane_id, uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_jesd_tx_rjspat_enable_set(device, links, lane_id, 0x01,
						   enable);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_jesd_tx_jtspat_enable_set(adi_ad9081_device_t *device,
				     adi_ad9081_jesd_link_select_e links,
				     uint8_t lane_id, uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_jesd_tx_rjspat_enable_set(device, links, lane_id, 0x02,
						   enable);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_pll_lock_status_get(adi_ad9081_device_t *device,
					    uint8_t *jesd_pll_status)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_get(device, REG_PLL_STATUS_LCPLL_ADDR,
				    BF_RFPLLLOCK_LCPLL_RS_INFO, jesd_pll_status,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_loopback_mode_set(adi_ad9081_device_t *device,
					  uint8_t mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, 0x00000941, 0x00000300,
				    mode); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

uint16_t adi_ad9081_jesd_find_dformat_out_nc(
	adi_ad9081_jtx_conv_sel_t const *jesd_conv_sel, uint8_t jesd_m)
{
	/* find an unused DFormat output for a link. This will be assigned to unused virtual converters */
	uint16_t df_out = 0;
	uint8_t m = 0;

	df_out |= (1 << jesd_conv_sel->virtual_converter0_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converter1_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converter2_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converter3_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converter4_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converter5_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converter6_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converter7_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converter8_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converter9_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_convertera_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converterb_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converterc_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converterd_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_convertere_index);
	m++;
	if (m == jesd_m)
		return df_out;
	df_out |= (1 << jesd_conv_sel->virtual_converterf_index);

	return df_out;
}

uint8_t
adi_ad9081_jesd_determine_common_nc(adi_ad9081_jesd_link_select_e links,
				    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2],
				    uint8_t jesd_m[2])
{
	uint16_t dformat_out_mask0, dformat_out_mask1, dformat_out_mask;
	uint8_t i, nc = 0xf;

	/* determine the active dformat outs for each link */
	dformat_out_mask0 = ((links & AD9081_LINK_0) > 0) ?
				    adi_ad9081_jesd_find_dformat_out_nc(
					    &jesd_conv_sel[0], jesd_m[0]) :
				    0;
	dformat_out_mask1 = ((links & AD9081_LINK_1) > 0) ?
				    adi_ad9081_jesd_find_dformat_out_nc(
					    &jesd_conv_sel[1], jesd_m[1]) :
				    0;

	/* combine dformat out masks from both links */
	dformat_out_mask = dformat_out_mask0 | dformat_out_mask1;

	/* find the highest common unused DFormat output for both links */
	for (i = 15; i >= 0; i--) {
		if ((dformat_out_mask & (1 << i)) == 0) {
			nc = (uint8_t)i;
			break;
		}
	}

	return nc;
}

int32_t adi_ad9081_jesd_tx_calc_br_ratio(adi_ad9081_device_t *device,
					 adi_cms_chip_op_mode_t chip_op_mode,
					 adi_cms_jesd_param_t *jesd_param,
					 adi_ad9081_jesd_link_select_e links,
					 uint64_t jtx_lane_rate[2],
					 uint8_t jtx_brr[2])
{
	uint8_t i, link, max_jtx_brr, min_jtx_brr, jtx_link_lane_rate_spread,
		jtx_link_brr_spread, jtx_brr_adjust, quotient, jtx_br_ratio;
	uint64_t jrx_lane_rate, max_jtx_lane_rate, min_jtx_lane_rate;
	uint32_t jrx_jtx_ratio;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(jesd_param);

	jrx_lane_rate = device->dev_info.jesd_rx_lane_rate;
	if (chip_op_mode == TX_RX_ONLY) { /* Determine chip mode = (tx + rx) */
		for (i = 0; i < 2; i++) {
			link = (uint8_t)(links & (AD9081_LINK_0 << i));
			if (link > 0) {
#ifdef __KERNEL__
				jrx_jtx_ratio = div64_u64(jrx_lane_rate,
							  jtx_lane_rate[i]);
#else
				jrx_jtx_ratio =
					jrx_lane_rate / jtx_lane_rate[i];
#endif
				if (jtx_lane_rate[i] > 8000000000ULL) {
					jtx_brr[i] = adi_api_utils_log2(
						jrx_jtx_ratio);
				} else if (jtx_lane_rate[i] <= 8000000000ULL) {
#ifdef __KERNEL__
					quotient = div64_u64(8000000000ULL,
							     jtx_lane_rate[i]) +
						   1;
#else
					quotient = ((8000000000ULL /
						     jtx_lane_rate[i]) +
						    1);
#endif
					jtx_br_ratio =
						adi_api_utils_is_power_of_two(
							quotient) ?
							quotient :
							((quotient &
							  (quotient - 1))
							 << 1);

					if (jtx_br_ratio > jrx_jtx_ratio) {
						jtx_brr[i] = adi_api_utils_log2(
							jtx_br_ratio);
					} else {
						jtx_brr[i] = adi_api_utils_log2(
							jrx_jtx_ratio);
					}
				}
			}
		}
	}
	if (chip_op_mode == RX_ONLY) { /* Determine chip mode = (rx only) */
		for (i = 0; i < 2; i++) {
			link = (uint8_t)(links & (AD9081_LINK_0 << i));
			if (link > 0) {
				jtx_brr[i] = 0;
				if (jtx_lane_rate[i] > 4000000000ULL &&
				    jtx_lane_rate[i] <=
					    8000000000ULL) { /* 4Gbps ~ 8Gbps */
					jtx_brr[i] = 1;
				} else if (jtx_lane_rate[i] > 2000000000ULL &&
					   jtx_lane_rate[i] <=
						   4000000000ULL) { /* 2Gbps ~ 4Gbps */
					jtx_brr[i] = 2;
				} else if (jtx_lane_rate[i] > 1000000000ULL &&
					   jtx_lane_rate[i] <=
						   2000000000ULL) { /* 1Gbps ~ 2Gbps */
					jtx_brr[i] = 3;
				} else if (jtx_lane_rate[i] > 500000000ULL &&
					   jtx_lane_rate[i] <=
						   1000000000ULL) { /* 0.5Gbps ~ 1Gbps */
					jtx_brr[i] = 4;
				} else if (jtx_lane_rate[i] > 250000000ULL &&
					   jtx_lane_rate[i] <=
						   500000000ULL) { /* 0.25Gbps ~ 0.5Gbps */
					jtx_brr[i] = 5;
				}
			}
		}

		if ((jesd_param->jesd_duallink == 1) &&
		    (jtx_lane_rate[0] != jtx_lane_rate[1])) {
			max_jtx_lane_rate =
				(jtx_lane_rate[0] > jtx_lane_rate[1]) ?
					jtx_lane_rate[0] :
					jtx_lane_rate[1];
			min_jtx_lane_rate =
				(jtx_lane_rate[0] < jtx_lane_rate[1]) ?
					jtx_lane_rate[0] :
					jtx_lane_rate[1];

			max_jtx_brr = (jtx_brr[0] > jtx_brr[1]) ? jtx_brr[0] :
								  jtx_brr[1];
			min_jtx_brr = (jtx_brr[0] < jtx_brr[1]) ? jtx_brr[0] :
								  jtx_brr[1];

#ifdef __KERNEL__
			jtx_link_lane_rate_spread =
				div64_u64(max_jtx_lane_rate, min_jtx_lane_rate);
#else
			jtx_link_lane_rate_spread =
				max_jtx_lane_rate / min_jtx_lane_rate;
#endif
			jtx_link_brr_spread =
				(1 << max_jtx_brr) / (1 << min_jtx_brr);

			if (jtx_link_lane_rate_spread != jtx_link_brr_spread) {
				jtx_brr_adjust = adi_api_utils_log2(
					jtx_link_lane_rate_spread /
					jtx_link_brr_spread);
				((jtx_brr[0] == min_jtx_brr)) ?
					(jtx_brr[0] =
						 max_jtx_brr + jtx_brr_adjust) :
					(jtx_brr[1] =
						 max_jtx_brr + jtx_brr_adjust);
			}
		}
	}

	return API_CMS_ERROR_OK;
}

/*! @} */