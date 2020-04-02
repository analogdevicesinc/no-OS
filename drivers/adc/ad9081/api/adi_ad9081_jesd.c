/*!
 * @brief     APIs for JESD configuration and control
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup __AD9081_JESD_API__
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"

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
			BF_JRX_LINK_LANE_SEL_0_INFO, physical_lane);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, REG_JRX_CORE_2_LANE0_ADDR + logical_lane,
			BF_JRX_LINK_LANE_SEL_0_INFO, physical_lane);
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
			err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_3_ADDR,
						    BF_JRX_DSCR_CFG_INFO,
						    jesd_param->jesd_scr);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_3_ADDR,
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
			err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_7_ADDR,
						    0x00000500,
						    jesd_param->jesd_n - 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_7_ADDR,
						    BF_JRX_CS_CFG_INFO,
						    jesd_param->jesd_cs);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_8_ADDR,
						    0x00000500,
						    jesd_param->jesd_np - 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_JRX_L0_8_ADDR,
						    BF_JRX_SUBCLASSV_CFG_INFO,
						    jesd_param->jesd_subclass);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x000004AF,
						    0x00000500,
						    jesd_param->jesd_s - 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x000004AF, 0x00000305,
				jesd_param->jesd_jesdv); /* 0: 204A, 1: 204B, 2: 204C */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x000004B0,
						    0x00000107,
						    jesd_param->jesd_hd);
			AD9081_ERROR_RETURN(err);
		}
	}

	/* configure default lmfc delay */
	err = adi_ad9081_jesd_rx_lmfc_delay_set(device, links, 0xc);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_sysref_enable_set(adi_ad9081_device_t *device,
					     uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_CTRL_ADDR,
				    BF_SYSREF_PD_INFO, !enable); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_sysref_input_mode_set(adi_ad9081_device_t *device,
						 uint8_t input_mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* 0: AC couple, 1: DC couple */
	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_CTRL_ADDR,
				    BF_SYSREF_INPUTMODE_INFO,
				    input_mode); /* not paged */
	AD9081_ERROR_RETURN(err);

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

	if (device->dev_info.dev_rev == 2 ||
	    device->dev_info.dev_rev == 3) { /* r1r/r2 */
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

	if (device->dev_info.dev_rev == 2 ||
	    device->dev_info.dev_rev == 3) { /* r1r/r2 */
		err = adi_ad9081_hal_bf_set(device, 0x21c5, 0x0800,
					    mask); /* rx_boost_mask@rx_boost */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_calibrate_204c(adi_ad9081_device_t *device)
{
	int32_t err;
	uint8_t core_status, jrx_fw_major, jrx_fw_minor;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* start app task */
	err = adi_ad9081_hal_bf_set(device, REG_REG8_SCRATCH1_ADDR,
				    BF_REG8_SCRATCH_2_INFO, 0x0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 20000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, 0x3742, 0x0800, &core_status,
				    1); /* @msg2 */
	AD9081_ERROR_RETURN(err);
	if (core_status != 0xFF)
		AD9081_LOG_ERR(
			"Boot not completed. Expected core_status 0xff.");
	err = adi_ad9081_hal_delay_us(device, 1000000);
	AD9081_ERROR_RETURN(err);

	/* log jrx firmware version */
	err = adi_ad9081_jesd_rx_firmware_version_get(device, &jrx_fw_major,
						      &jrx_fw_minor);
	AD9081_ERROR_RETURN(err);

	if (device->dev_info.dev_rev == 1) { /* r1 */
		err = adi_ad9081_hal_bf_set(
			device, 0x21d9, 0x0105,
			0x0); /* rx_bg_cal_skip_lms@rx_set_state1 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, 0x21d9, 0x0106,
			0x0); /* rx_bg_cal_skip_offsets@rx_set_state1 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, 0x21d9, 0x0104,
			0x1); /* rx_fg_cal_only_run@rx_set_state1 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, 0x21d9, 0x0103,
			0x1); /* rx_bg_cal_run@rx_set_state1 */
		AD9081_ERROR_RETURN(err);
	}
	if (device->dev_info.dev_rev == 2 ||
	    device->dev_info.dev_rev == 3) { /* r1r/r2 */
		err = adi_ad9081_hal_bf_set(
			device, 0x21c1, 0x0105,
			0x0); /* rx_bg_cal_skip_lms@rx_set_state1 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, 0x21c1, 0x0106,
			0x0); /* rx_bg_cal_skip_offsets@rx_set_state1 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, 0x21c1, 0x0104,
			0x1); /* rx_fg_cal_only_run@rx_set_state1 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(
			device, 0x21c1, 0x0103,
			0x1); /* rx_bg_cal_run@rx_set_state1 */
		AD9081_ERROR_RETURN(err);
	}

	err = adi_ad9081_hal_delay_us(device, 15000000);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_startup_des(adi_ad9081_device_t *device,
				       uint8_t deser_rate_config)
{
	int32_t err;
	uint8_t jrx_cbus_reg_val, jesd204b_en, jesd204c_en, data_mode;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* reset CDR */
	err = adi_ad9081_hal_bf_set(device, REG_CDR_RESET_ADDR,
				    BF_RSTB_DES_RC_INFO, 0x1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_CDR_RESET_ADDR,
				    BF_RSTB_DES_RC_INFO, 0x0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_CDR_RESET_ADDR,
				    BF_RSTB_DES_RC_INFO, 0x1); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* set amptarget and hysteresis */
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x19, &jrx_cbus_reg_val,
		0x00); /* rx/trim_pkdet_hyst_des_rc */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x19, (jrx_cbus_reg_val & 0xE0) | 0x04, 0xFF);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x1B, &jrx_cbus_reg_val,
		0x00); /* rx/trim_pkdet_lvl0_des_rc */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x1B, (jrx_cbus_reg_val & 0xC0) | 0x21, 0xFF);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x1C, &jrx_cbus_reg_val,
		0x00); /* rx/trim_pkdet_lvl1_des_rc */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x1C, (jrx_cbus_reg_val & 0xC0) | 0x21, 0xFF);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x1D, &jrx_cbus_reg_val,
		0x00); /* rx/trim_pkdet_lvl2_des_rc */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x1D, (jrx_cbus_reg_val & 0xC0) | 0x21, 0xFF);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x1E, &jrx_cbus_reg_val,
		0x00); /* rx/trim_pkdet_lvl3_des_rc */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x1E, (jrx_cbus_reg_val & 0xC0) | 0x21, 0xFF);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 100000);
	AD9081_ERROR_RETURN(err);

	if (deser_rate_config == 1) {
		/* DESER is in half rate */
		err = adi_ad9081_hal_bf_set(device,
					    REG_LF_QUARTERRATE_DES_RC_ADDR,
					    BF_SEL_LF_QUARTERRATE_DES_RC_INFO,
					    0x0); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, 0x00000438, 0x00000100,
					    0x1); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, 0x00000438, 0x00000101,
					    0x1); /* not paged */
		AD9081_ERROR_RETURN(err);

		/* set CTLE evals: set DCgain, Peaking, and Fzero was 8-4-8 */
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0x4, &jrx_cbus_reg_val,
			0x0); /* rx/ctle_dcgain_des_rc */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0x4, (jrx_cbus_reg_val & 0xF0) | 0x0E, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0x5, &jrx_cbus_reg_val,
			0x0); /* rx/ctle_peaking_des_rc */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0x5, (jrx_cbus_reg_val & 0xF0) | 0x04, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0x4, &jrx_cbus_reg_val,
			0x0); /* rx/ctle_fzero_des_rc */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0x4, (jrx_cbus_reg_val & 0x0F) | (0x0A << 4),
			0xFF);
		AD9081_ERROR_RETURN(err);
	}
	if (deser_rate_config == 2) {
		/* DESER is in quarter rate */
		err = adi_ad9081_hal_bf_set(device,
					    REG_LF_QUARTERRATE_DES_RC_ADDR,
					    BF_SEL_LF_QUARTERRATE_DES_RC_INFO,
					    0xff); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, 0x00000438, 0x00000100,
					    0x1); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, 0x00000438, 0x00000101,
					    0x1); /* not paged */
		AD9081_ERROR_RETURN(err);

		err = adi_ad9081_hal_cbusjrx_reg_set(device, 0xC0, 0x00,
						     0xFF); /* rx/DFE_CTL0 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(device, 0xCD,
						     &jrx_cbus_reg_val,
						     0x0); /* rx/s1_pd_clkgen */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xCD, jrx_cbus_reg_val & 0xBF, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xD2, &jrx_cbus_reg_val,
			0x0); /* rx/s0_pd_dac_thr */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xD2, jrx_cbus_reg_val & 0xC7, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xE2, &jrx_cbus_reg_val,
			0x0); /* rx/s1_pd_dac_thr */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xE2, jrx_cbus_reg_val & 0xC7, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xD3, &jrx_cbus_reg_val,
			0x0); /* rx/s0_pd_dac_off */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xD3, jrx_cbus_reg_val & 0x00, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xE3, &jrx_cbus_reg_val,
			0x0); /* rx/s1_pd_dac_off */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xE3, jrx_cbus_reg_val & 0x00, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xDC, &jrx_cbus_reg_val,
			0x0); /* rx/s0_pd_comp_f0 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xDC, jrx_cbus_reg_val & 0xF0, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xDC, &jrx_cbus_reg_val,
			0x0); /* rx/s0_pd_comp_f1 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xDC, jrx_cbus_reg_val & 0x0F, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xEC, &jrx_cbus_reg_val,
			0x0); /* rx/s1_pd_comp_f0 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xEC, jrx_cbus_reg_val & 0xF0, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xEC, &jrx_cbus_reg_val,
			0x0); /* rx/s1_pd_comp_f1 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xEC, jrx_cbus_reg_val & 0x0F, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(device, 0xF4,
						     &jrx_cbus_reg_val,
						     0x0); /* rx/s0_en_gmsw */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xF4, jrx_cbus_reg_val | 0x01, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xF4, &jrx_cbus_reg_val,
			0x0); /* rx/s0_short_gmsw */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xF4, jrx_cbus_reg_val & 0xFD, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(device, 0xF4,
						     &jrx_cbus_reg_val,
						     0x0); /* rx/s1_en_gmsw */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xF4, jrx_cbus_reg_val | 0x04, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xF4, &jrx_cbus_reg_val,
			0x0); /* rx/s1_short_gmsw */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xF4, jrx_cbus_reg_val & 0xF7, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xD2, &jrx_cbus_reg_val,
			0x0); /* rx/s0_mux_en_ovrd */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xD2, jrx_cbus_reg_val & 0x3F, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xE2, &jrx_cbus_reg_val,
			0x0); /* rx/s1_mux_en_ovrd */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xE2, jrx_cbus_reg_val & 0x3F, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xD2, &jrx_cbus_reg_val,
			0x0); /* rx/s0_polarity_swap */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xD2, jrx_cbus_reg_val & 0xF8, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_get(
			device, 0xE2, &jrx_cbus_reg_val,
			0x0); /* rx/s1_polarity_swap */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(
			device, 0xE2, jrx_cbus_reg_val & 0xF8, 0xFF);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_cbusjrx_reg_set(device, 0xE2, 0x00,
						     0xFF); /* rx/DFE_CTL31 */
		AD9081_ERROR_RETURN(err);
	}

	/* set data mode */
	err = adi_ad9081_hal_bf_get(device, REG_JRX_DL_204B_2_ADDR,
				    BF_JRX_DL_204B_ENABLE_INFO, &jesd204b_en,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_JRX_DL_204C_0_ADDR,
				    BF_JRX_DL_204C_ENABLE_INFO, &jesd204c_en,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	data_mode = 0;
	if (jesd204b_en > 0)
		data_mode = 2;
	if (jesd204c_en > 0)
		data_mode = 0;
	err = adi_ad9081_hal_bf_set(device, REG_LF_PARDATAMODE_DES_RC0_ADDR,
				    BF_SEL_LF_PARDATAMODE_DES_RC_CH0_INFO,
				    data_mode); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LF_PARDATAMODE_DES_RC0_ADDR,
				    BF_SEL_LF_PARDATAMODE_DES_RC_CH1_INFO,
				    data_mode); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LF_PARDATAMODE_DES_RC0_ADDR,
				    BF_SEL_LF_PARDATAMODE_DES_RC_CH2_INFO,
				    data_mode); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LF_PARDATAMODE_DES_RC0_ADDR,
				    BF_SEL_LF_PARDATAMODE_DES_RC_CH3_INFO,
				    data_mode); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LF_PARDATAMODE_DES_RC1_ADDR,
				    BF_SEL_LF_PARDATAMODE_DES_RC_CH4_INFO,
				    data_mode); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LF_PARDATAMODE_DES_RC1_ADDR,
				    BF_SEL_LF_PARDATAMODE_DES_RC_CH5_INFO,
				    data_mode); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LF_PARDATAMODE_DES_RC1_ADDR,
				    BF_SEL_LF_PARDATAMODE_DES_RC_CH6_INFO,
				    data_mode); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LF_PARDATAMODE_DES_RC1_ADDR,
				    BF_SEL_LF_PARDATAMODE_DES_RC_CH7_INFO,
				    data_mode); /* not paged */
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
	err = adi_ad9081_hal_bf_get(device, REG_INTRP_MODE_ADDR,
				    BF_DP_INTERP_MODE_INFO, &main_interp,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_INTRP_MODE_ADDR,
				    BF_CH_INTERP_MODE_INFO, &ch_interp,
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
	err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_MSG,
				       "jrx bit rate is %llu bps", bit_rate);
	AD9081_ERROR_RETURN(err);

	*bit_rate_bps = bit_rate;
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_pll_startup(adi_ad9081_device_t *device,
				       uint64_t bit_rate)
{
	int32_t err;
	uint8_t i, pll_reg, b_lcpll, div_m, div_p, ref_in_div, rx_div_rate,
		jesd_pll_locked;
	uint8_t jesd_f, jesd204b_en;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* spi default changes */
	err = adi_ad9081_hal_bf_set(device, REG_LCPLL_RST_ADDR,
				    BF_RSTB_LCPLL_RC_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	/* The HPC+ change to var_dac_offset is 19*32=608, 0x260, pll/vcovardacinovd_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0xB1, 0x20);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0xB2, 0x02);
	AD9081_ERROR_RETURN(err);
	/* increase the regulator voltage level from 3 to 4, pll/sel_regref_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0x8D, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0x8D,
					     (pll_reg & 0xF8) | 0x4);
	AD9081_ERROR_RETURN(err);
	/* pll/ptatslope_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0xB6, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0xB6,
					     (pll_reg & 0x0F) | (0x7 << 4));
	AD9081_ERROR_RETURN(err);
	/* pll/vcobiastcf_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0x93, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0x93,
					     (pll_reg & 0x8F) | (0x5 << 4));
	AD9081_ERROR_RETURN(err);
	/* changed from 2 -> 1 (48 to 24), pll/vco_band_init_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0xD3, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0xD3,
					     (pll_reg & 0xCF) | (0x1 << 4));
	AD9081_ERROR_RETURN(err);
	/* bits[7:6] changed from 1 -> 2 (2 % to 4 % ), pll/spare2_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0xB5, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0xB5,
					     (pll_reg | 0x80) & 0xBF);
	AD9081_ERROR_RETURN(err);
	/* reduced default from 52 to 44, pll/regthresh_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0x8E, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0x8E,
					     (pll_reg & 0xC0) | 0x2C);
	AD9081_ERROR_RETURN(err);

	/* override OctaveCal */
	err = adi_ad9081_jesd_rx_link_select_set(device, AD9081_LINK_0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, 0x000004AA, 0x00000800, &jesd_f, 1);
	AD9081_ERROR_RETURN(err);
	jesd_f = jesd_f + 1;
	err = adi_ad9081_hal_bf_get(device, REG_JRX_DL_204B_2_ADDR,
				    BF_JRX_DL_204B_ENABLE_INFO, &jesd204b_en,
				    1);
	AD9081_ERROR_RETURN(err);
	ref_in_div =
		((jesd204b_en == 0) && (bit_rate > 16230000000ULL)) ? 1 : 0;
	div_m = ((jesd204b_en == 0) && (bit_rate > 16230000000ULL)) ? 0 : 1;
	div_p = ((jesd204b_en > 0)) ? 0 : 1;
	b_lcpll = (((jesd_f == 3) || (jesd_f == 6) || (jesd_f == 12)) ? 3 : 1) *
		  (jesd204b_en > 0 ? 5 : 11);
	if (bit_rate <= 2000000000ULL) {
		AD9081_LOG_ERR("jrx bit rate is lower than 2Gbps.");
		return API_CMS_ERROR_ERROR;
	} else if (bit_rate > 2000000000ULL && bit_rate <= 4000000000ULL) {
		b_lcpll = b_lcpll * 4;
		rx_div_rate = 1;
	} else if (bit_rate > 4000000000ULL && bit_rate <= 7000000000ULL) {
		b_lcpll = b_lcpll * 2;
		rx_div_rate = 0;
	} else if (bit_rate > 7000000000ULL) {
		rx_div_rate = 8;
	}
	err = adi_ad9081_hal_bf_set(device, REG_LCPLL_REF_CLK_DIV1_REG_ADDR,
				    BF_REFINDIV_LCPLL_RC_INFO,
				    ref_in_div); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LCPLL_REF_CLK_DIV1_REG_ADDR,
				    BF_DIVM_LCPLL_RC_INFO,
				    div_m); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LCPLL_REF_CLK_DIV1_REG_ADDR,
				    BF_DIVP_LCPLL_RC_INFO,
				    div_p); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIV2_ADDR,
				    BF_B_LCPLL_RC_INFO,
				    b_lcpll); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_RXDIVRATE_ADDR,
				    BF_RXDIVRATE_LCPLL_RC_INFO,
				    rx_div_rate); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_REFINDIVOVD_LCPLL_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_REFCLK_CPL_ADDR,
				    BF_SEL_REFINDIV3_LCPLL_RC_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_BOVD_LCPLL_RC_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_DIVMOVD_LCPLL_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_RXDIVRATEOVD_LCPLL_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENCAL_ADDR,
				    BF_EN_OCTAVECAL_LCPLL_RC_INFO,
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
	for (i = 0; i < 2; i++) {
		err = adi_ad9081_hal_delay_us(device, 1000000);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_pll_lock_status_get(device,
							  &jesd_pll_locked);
		AD9081_ERROR_RETURN(err);
		if (jesd_pll_locked == 1)
			break;
	}
	if (jesd_pll_locked == 0) {
		AD9081_LOG_ERR("JESD PLL is not locked.");
	}
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENABLE_CTRL_ADDR,
				    BF_LCPLL_JTX_PLL_BYPASS_LOCK_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_bring_up(adi_ad9081_device_t *device,
				    adi_ad9081_jesd_link_select_e links,
				    uint8_t lanes)
{
	int32_t err;
	uint8_t i, jesd204b_en, deser_rate_config;
	uint64_t bit_rate;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* setup qbd */
	for (i = 0; i < 8; i++) {
		err = adi_ad9081_jesd_rx_lane_xbar_set(device, links, i,
						       i); /* default xbar */
		AD9081_ERROR_RETURN(err);
	}
	err = adi_ad9081_hal_bf_set(device, REG_MASTER_PD_ADDR,
				    BF_PD_MASTER_RC_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PHY_PD_ADDR,
				    BF_PD_DES_RC_CH_INFO,
				    (uint8_t)(~lanes)); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* calculate bit rate */
	err = adi_ad9081_jesd_rx_bit_rate_get(device, &bit_rate);
	AD9081_ERROR_RETURN(err);

	/* startup jesd pll */
	err = adi_ad9081_jesd_rx_pll_startup(device, bit_rate);
	AD9081_ERROR_RETURN(err);

	/* startup deserializer */
	err = adi_ad9081_hal_bf_get(device, REG_JRX_DL_204B_2_ADDR,
				    BF_JRX_DL_204B_ENABLE_INFO, &jesd204b_en,
				    1);
	AD9081_ERROR_RETURN(err);
	deser_rate_config = (jesd204b_en > 0) ?
				    ((bit_rate > 8000000000ULL) ? 1 : 0) :
				    ((bit_rate < 16230000000ULL) ?
					     1 :
					     2); /* 0: full, 1: 1/2, 2: 1/4 */
	err = adi_ad9081_jesd_rx_startup_des(device, deser_rate_config);
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
		err = adi_ad9081_hal_bf_get(device,
					    REG_JRX_DL_204B_20_LANE0_ADDR + i,
					    BF_JRX_DL_204B_CGS_0_INFO, &cgs,
					    1); /* paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device,
					    REG_JRX_DL_204B_20_LANE0_ADDR + i,
					    BF_JRX_DL_204B_FS_0_INFO, &fs,
					    1); /* paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device,
					    REG_JRX_DL_204B_20_LANE0_ADDR + i,
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

int32_t adi_ad9081_jesd_tx_link_config_set(adi_ad9081_device_t *device,
					   adi_ad9081_jesd_link_select_e links,
					   adi_cms_jesd_param_t *jesd_param)
{
	int32_t err;
	uint8_t reg13, i, link;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(jesd_param);
	AD9081_INVALID_PARAM_RETURN(jesd_param->jesd_jesdv > 2);

	/* disable jtx link */
	err = adi_ad9081_jesd_tx_link_enable_set(device, links, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_jesd_tx_link_reset(device, 1);
	AD9081_ERROR_RETURN(err);

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
				jesd_param->jesd_did); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_1_ADDR, BF_JTX_BID_CFG_INFO,
				jesd_param->jesd_bid); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_3_ADDR, BF_JTX_L_CFG_INFO,
				jesd_param->jesd_l - 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_3_ADDR, BF_JTX_SCR_CFG_INFO,
				jesd_param->jesd_scr); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_4_ADDR, BF_JTX_F_CFG_INFO,
				jesd_param->jesd_f - 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_5_ADDR, BF_JTX_K_CFG_INFO,
				jesd_param->jesd_k - 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_6_ADDR, BF_JTX_M_CFG_INFO,
				jesd_param->jesd_m - 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_7_ADDR, BF_JTX_N_CFG_INFO,
				jesd_param->jesd_n - 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_7_ADDR, BF_JTX_CS_CFG_INFO,
				jesd_param->jesd_cs); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_8_ADDR, BF_JTX_NP_CFG_INFO,
				jesd_param->jesd_np - 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_8_ADDR,
				BF_JTX_SUBCLASSV_CFG_INFO,
				jesd_param->jesd_subclass); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_9_ADDR, BF_JTX_S_CFG_INFO,
				jesd_param->jesd_s - 1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_9_ADDR,
				BF_JTX_JESDV_CFG_INFO,
				jesd_param->jesd_jesdv); /* paged, 0: 204A, 1: 204B, 2: 204C */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_L0_10_ADDR, BF_JTX_HD_CFG_INFO,
				jesd_param->jesd_hd); /* paged */
			AD9081_ERROR_RETURN(err);
			if (jesd_param->jesd_jesdv == 2) { /* 204C */
				err = adi_ad9081_hal_bf_set(
					device, REG_JTX_DL_204C_1_ADDR,
					BF_JTX_E_CFG_INFO,
					((jesd_param->jesd_k *
					  jesd_param->jesd_f) >>
					 8) - 1); /* not paged */
				AD9081_ERROR_RETURN(err);
			}
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_QUICK_CFG_ADDR,
				BF_JTX_MODE_INFO,
				jesd_param->jesd_mode_id); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_C2R_EN_ADDR,
				BF_JTX_MODE_C2R_EN_INFO,
				jesd_param->jesd_mode_c2r_en); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_QUICK_CFG_ADDR,
				BF_JTX_MODE_S_SEL_INFO,
				jesd_param->jesd_mode_s_sel); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	err = adi_ad9081_hal_cbusjtx_reg_get(device, 0x0d, &reg13, 0x00);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjtx_reg_set(device, 0x0d, (reg13 & 0x1) | 0x2,
					     0xff); /* tx/fifo_start_addr = 1 */
	AD9081_ERROR_RETURN(err);

	if (jesd_param->jesd_jesdv < 2) { /* 204A/B */
		err = adi_ad9081_hal_bf_set(device, REG_JTX_CORE_1_ADDR,
					    BF_JTX_LINK_204C_SEL_INFO,
					    0); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_PARDATAMODE_SER_ADDR,
					    BF_PARDATAMODE_SER_RC_INFO,
					    2); /* not paged */
		AD9081_ERROR_RETURN(err);
	}
	if (jesd_param->jesd_jesdv == 2) { /* 204C */
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
		err = adi_ad9081_hal_bf_set(device, REG_JTX_CORE_1_ADDR,
					    BF_JTX_LINK_204C_SEL_INFO,
					    1); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_PARDATAMODE_SER_ADDR,
					    BF_PARDATAMODE_SER_RC_INFO,
					    0); /* not paged */
		AD9081_ERROR_RETURN(err);
	}

	/* release jtx link */
	err = adi_ad9081_jesd_tx_link_reset(device, 0);
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

int32_t adi_ad9081_jesd_tx_bit_rate_get(adi_ad9081_device_t *device,
					adi_ad9081_jesd_link_select_e link,
					uint64_t *bit_rate_bps)
{
	int32_t err;
	uint8_t jesd_m, jesd_np, jesd_l, jesd_dcm, jesd204b_en, jesd_link_id;
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
				    BF_JTX_LINK_204C_SEL_INFO, &jesd204b_en,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	jesd204b_en = jesd204b_en > 0 ? 0 : 1;

	bit_rate = jesd_np * jesd_m * device->dev_info.adc_freq_hz *
		   (jesd204b_en > 0 ? 10 : 66);
#ifdef __KERNEL__
	bit_rate = div_u64(bit_rate,
			   jesd_l * jesd_dcm * (jesd204b_en > 0 ? 8 : 64));
#else
	bit_rate = bit_rate / (jesd_l * jesd_dcm * (jesd204b_en > 0 ? 8 : 64));
#endif
	jesd_link_id = (link & AD9081_LINK_0) > 0 ? 0 : 1;
	err = adi_ad9081_hal_log_write(device, ADI_CMS_LOG_MSG,
				       "jtx link%d bit rate is %llu bps",
				       jesd_link_id, bit_rate);
	AD9081_ERROR_RETURN(err);

	*bit_rate_bps = bit_rate;
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_pll_startup(adi_ad9081_device_t *device,
				       uint64_t bit_rate)
{
	int32_t err;
	uint8_t i, b_lcpll, div_m, div_p, ref_in_div, sdsrefclk_ratio,
		jesd_pll_locked, pll_reg, jesd204b_en;
	uint8_t jesd204b_lcpll[] = { 5, 10, 20, 40 };
	uint8_t jesd204c_lcpll[] = { 11, 22, 33, 44 };
	uint8_t *jesd204_lcpll;
	uint64_t a, b;
	uint8_t c;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(bit_rate == 0);

	err = adi_ad9081_hal_bf_get(device, REG_JTX_CORE_1_ADDR,
				    BF_JTX_LINK_204C_SEL_INFO, &jesd204b_en,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	jesd204b_en = jesd204b_en > 0 ? 0 : 1;
	err = adi_ad9081_hal_bf_set(device,
				    REG_MANUAL_SEDESPLLREFCLK_PERIOD2_ADDR,
				    BF_SDSPLLREFCLK_DIV_SPI_EN_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);

	ref_in_div =
		((jesd204b_en == 0) && (bit_rate > 16000000000ULL)) ? 1 : 0;
	div_m = ((jesd204b_en == 0) && (bit_rate > 16000000000ULL)) ? 0 : 1;
	div_p = ((jesd204b_en > 0)) ? 0 : 1;

	jesd204_lcpll = (jesd204b_en > 0) ? jesd204b_lcpll : jesd204c_lcpll;
	b_lcpll = jesd204_lcpll[0];
	sdsrefclk_ratio = 1;
	for (i = 0; i < 4; i++) {
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
	if (i == 4) {
		AD9081_LOG_ERR("SDSPLLREFCLK divider not found.");
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

	/* spi default changes */
	err = adi_ad9081_hal_bf_set(device, REG_LCPLL_RST_ADDR,
				    BF_RSTB_LCPLL_RC_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	/* The HPC+ change to var_dac_offset is 19*32=608, 0x260, pll/vcovardacinovd_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0xB1, 0x20);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0xB2, 0x02);
	AD9081_ERROR_RETURN(err);
	/* increase the regulator voltage level from 3 to 4, pll/sel_regref_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0x8D, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0x8D,
					     (pll_reg & 0xF8) | 0x4);
	AD9081_ERROR_RETURN(err);
	/* pll/ptatslope_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0xB6, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0xB6,
					     (pll_reg & 0x0F) | (0x7 << 4));
	AD9081_ERROR_RETURN(err);
	/* pll/vcobiastcf_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0x93, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0x93,
					     (pll_reg & 0x8F) | (0x5 << 4));
	AD9081_ERROR_RETURN(err);
	/* changed from 2 -> 1 (48 to 24), pll/vco_band_init_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0xD3, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0xD3,
					     (pll_reg & 0xCF) | (0x1 << 4));
	AD9081_ERROR_RETURN(err);
	/* bits[7:6] changed from 1 -> 2 (2 % to 4 % ), pll/spare2_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0xB5, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0xB5,
					     (pll_reg | 0x80) & 0xBF);
	AD9081_ERROR_RETURN(err);
	/* reduced default from 52 to 44, pll/regthresh_lcpll_rc */
	err = adi_ad9081_hal_cbuspll_reg_get(device, 0x8E, &pll_reg);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbuspll_reg_set(device, 0x8E,
					     (pll_reg & 0xC0) | 0x2C);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_hal_bf_set(device, REG_LCPLL_REF_CLK_DIV1_REG_ADDR,
				    BF_REFINDIV_LCPLL_RC_INFO,
				    ref_in_div); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LCPLL_REF_CLK_DIV1_REG_ADDR,
				    BF_DIVM_LCPLL_RC_INFO,
				    div_m); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_LCPLL_REF_CLK_DIV1_REG_ADDR,
				    BF_DIVP_LCPLL_RC_INFO,
				    div_p); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIV2_ADDR,
				    BF_B_LCPLL_RC_INFO,
				    b_lcpll); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device,
				    REG_MANUAL_SEDESPLLREFCLK_PERIOD2_ADDR,
				    BF_SDSPLLREFCLK_DIV_RATIO_SPI_INFO,
				    sdsrefclk_ratio - 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_REFINDIVOVD_LCPLL_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_REFCLK_CPL_ADDR,
				    BF_SEL_REFINDIV3_LCPLL_RC_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_BOVD_LCPLL_RC_INFO, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_DIVMOVD_LCPLL_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_DIVOVD_ADDR,
				    BF_RXDIVRATEOVD_LCPLL_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENCAL_ADDR,
				    BF_EN_OCTAVECAL_LCPLL_RC_INFO,
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
	for (i = 0; i < 2; i++) {
		err = adi_ad9081_hal_delay_us(device, 1000000);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_pll_lock_status_get(device,
							  &jesd_pll_locked);
		AD9081_ERROR_RETURN(err);
		if (jesd_pll_locked == 1)
			break;
	}
	if (jesd_pll_locked == 0) {
		AD9081_LOG_ERR("JESD PLL is not locked.");
	}

	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENABLE_CTRL_ADDR,
				    BF_LCPLL_JTX_PLL_BYPASS_LOCK_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_tx_bring_up(adi_ad9081_device_t *device,
				    adi_ad9081_jesd_link_select_e links,
				    uint8_t lanes,
				    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2])
{
	int32_t err;
	uint8_t i, link, jesd_pll_locked, div_m, jesd_f, jesd_s, jesd_dcm,
		jesd204b_en, jesd_bit_repeat_ratio, jesd_link_async[2];
	uint32_t a, b, c;
	uint64_t bit_rate[2];
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* write default settings to jtx */
	/* not invert lanes at default, default lane xbar and lane id settings */
	for (i = 0; i < 8; i++) {
		err = adi_ad9081_jesd_tx_lane_invert_set(device, links, i, 0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_lane_xbar_set(device, links, i, i);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_lid_cfg_set(device, links, i, i);
		AD9081_ERROR_RETURN(err);
	}
	/* set virtual converter mask */
	for (i = 0; i < 16; i++) {
		err = adi_ad9081_jesd_tx_conv_mask_set(device, links, i, 0);
		AD9081_ERROR_RETURN(err);
	}
	/* set virtual converter */
	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0x0,
			jesd_conv_sel[0].virtual_converter0_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0x1,
			jesd_conv_sel[0].virtual_converter1_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0x2,
			jesd_conv_sel[0].virtual_converter2_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0x3,
			jesd_conv_sel[0].virtual_converter3_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0x4,
			jesd_conv_sel[0].virtual_converter4_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0x5,
			jesd_conv_sel[0].virtual_converter5_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0x6,
			jesd_conv_sel[0].virtual_converter6_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0x7,
			jesd_conv_sel[0].virtual_converter7_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0x8,
			jesd_conv_sel[0].virtual_converter8_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0x9,
			jesd_conv_sel[0].virtual_converter9_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0xa,
			jesd_conv_sel[0].virtual_convertera_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0xb,
			jesd_conv_sel[0].virtual_converterb_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0xc,
			jesd_conv_sel[0].virtual_converterc_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0xd,
			jesd_conv_sel[0].virtual_converterd_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0xe,
			jesd_conv_sel[0].virtual_convertere_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_0, 0xf,
			jesd_conv_sel[0].virtual_converterf_index);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0x0,
			jesd_conv_sel[1].virtual_converter0_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0x1,
			jesd_conv_sel[1].virtual_converter1_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0x2,
			jesd_conv_sel[1].virtual_converter2_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0x3,
			jesd_conv_sel[1].virtual_converter3_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0x4,
			jesd_conv_sel[1].virtual_converter4_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0x5,
			jesd_conv_sel[1].virtual_converter5_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0x6,
			jesd_conv_sel[1].virtual_converter6_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0x7,
			jesd_conv_sel[1].virtual_converter7_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0x8,
			jesd_conv_sel[1].virtual_converter8_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0x9,
			jesd_conv_sel[1].virtual_converter9_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0xa,
			jesd_conv_sel[1].virtual_convertera_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0xb,
			jesd_conv_sel[1].virtual_converterb_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0xc,
			jesd_conv_sel[1].virtual_converterc_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0xd,
			jesd_conv_sel[1].virtual_converterd_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0xe,
			jesd_conv_sel[1].virtual_convertere_index);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_tx_conv_sel_set(
			device, AD9081_LINK_1, 0xf,
			jesd_conv_sel[1].virtual_converterf_index);
		AD9081_ERROR_RETURN(err);
	}

	/* misc */
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENCAL_ADDR,
				    BF_PD_RXCLK_DIST_RC_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_ENCAL_ADDR,
				    BF_PD_TXCLK_DIST_RC_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_SYNCA_CTRL_ADDR,
				    BF_PD_SYNCA_RX_RC_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_SYNCA_CTRL_ADDR,
				    BF_SYNCA_RX_MODE_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_SYNCB_CTRL_ADDR,
				    BF_PD_SYNCB_RX_RC_INFO, 0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_SYNCB_CTRL_ADDR,
				    BF_SYNCB_RX_MODE_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	/* default mode */
	err = adi_ad9081_jesd_tx_fbw_sel_set(
		device, links,
		0); /* not using fbw mode for any converter at default */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_jesd_tx_res_sel_set(
		device, links,
		AD9081_CHIP_OUT_RES_16BIT); /* 16bit resolution as default */
	AD9081_ERROR_RETURN(err);

	/* calculate bit rate */
	bit_rate[0] = 0ULL;
	bit_rate[1] = 0ULL;
	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_bit_rate_get(device, AD9081_LINK_0,
						      &bit_rate[0]);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_bit_rate_get(device, AD9081_LINK_1,
						      &bit_rate[1]);
		AD9081_ERROR_RETURN(err);
	}

	/* startup jesd pll if using rx only */
	err = adi_ad9081_jesd_pll_lock_status_get(device, &jesd_pll_locked);
	AD9081_ERROR_RETURN(err);
	if (jesd_pll_locked == 0) {
		err = adi_ad9081_jesd_tx_pll_startup(
			device, (bit_rate[0] > bit_rate[1]) ? bit_rate[0] :
							      bit_rate[1]);
		AD9081_ERROR_RETURN(err);
	}

	/* configure async mode */
	err = adi_ad9081_hal_bf_get(device, REG_LCPLL_REF_CLK_DIV1_REG_ADDR,
				    BF_DIVM_LCPLL_RC_INFO, &div_m,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PLL_REF_CLK_DIV1_REG_ADDR,
				    BF_DIVM_LCPLL_RC_RX_INFO,
				    div_m); /* not paged */
	AD9081_ERROR_RETURN(err);
	jesd_link_async[0] = 0;
	jesd_link_async[1] = 0;
	for (i = 0; i < 2; i++) {
		link = (uint8_t)(links & (AD9081_LINK_0 << i));
		if (link > 0) {
			jesd_bit_repeat_ratio = 0;
			if (bit_rate[i] > 4000000000ULL &&
			    bit_rate[i] <= 8000000000ULL) {
				jesd_bit_repeat_ratio = 1;
			} else if (bit_rate[i] > 2000000000ULL &&
				   bit_rate[i] <= 4000000000ULL) {
				jesd_bit_repeat_ratio = 2;
			} else if (bit_rate[i] > 1000000000ULL &&
				   bit_rate[i] <= 2000000000ULL) {
				jesd_bit_repeat_ratio = 3;
			} else if (bit_rate[i] > 500000000ULL &&
				   bit_rate[i] <= 1000000000ULL) {
				jesd_bit_repeat_ratio = 4;
			} else if (bit_rate[i] > 250000000ULL &&
				   bit_rate[i] <= 500000000ULL) {
				jesd_bit_repeat_ratio = 5;
			}
			err = adi_ad9081_jesd_tx_link_select_set(device, link);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device, REG_JTX_L0_9_ADDR,
						    BF_JTX_S_CFG_INFO, &jesd_s,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
			jesd_s = jesd_s + 1;
			err = adi_ad9081_hal_bf_get(device, REG_JTX_L0_4_ADDR,
						    BF_JTX_F_CFG_INFO, &jesd_f,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
			jesd_f = jesd_f + 1;
			err = adi_ad9081_hal_bf_get(
				device, REG_CHIP_DECIMATION_RATIO_ADDR,
				BF_CHIP_DECIMATION_RATIO_INFO, &jesd_dcm,
				1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device, REG_JTX_CORE_1_ADDR,
						    BF_JTX_LINK_204C_SEL_INFO,
						    &jesd204b_en,
						    1); /* not paged */
			AD9081_ERROR_RETURN(err);
			jesd204b_en = jesd204b_en > 0 ? 0 : 1;
			a = ((jesd204b_en > 0) ? 1 : 2) * jesd_dcm * jesd_s;
			b = (1 << jesd_bit_repeat_ratio) * jesd_f;
			c = a / b;
			jesd_link_async[i] = (a == (b * c)) ? 0 : 1;
			err = adi_ad9081_hal_bf_set(
				device, REG_JTX_TPL_0_ADDR,
				BF_JTX_CONV_ASYNCHRONOUS_INFO,
				jesd_link_async[i]); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}
	if ((jesd_link_async[0] > 0) || (jesd_link_async[1] > 0)) {
		err = adi_ad9081_hal_bf_set(
			device, REG_RESET_CTRL_REG_ADDR,
			BF_FORCE_JTX_DIGITAL_RESET_ON_RSTEN_FORCE_EN_INFO,
			1); /* not paged */
		AD9081_ERROR_RETURN(err);
	}

	/* startup serializer */
	err = adi_ad9081_jesd_tx_startup_ser(device, lanes);
	AD9081_ERROR_RETURN(err);

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
	uint8_t pll_locked, invalid_mode, phase_establish, state_204b,
		sync_204b;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(status);

	err = adi_ad9081_jesd_tx_link_select_set(device, links);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_hal_bf_get(device, REG_JTX_DL_204B_4_ADDR,
				    BF_JTX_DL_204B_STATE_INFO, &state_204b,
				    1); /* paged, QBF status */
	AD9081_ERROR_RETURN(err);
	res = state_204b & 0x0F;

	err = adi_ad9081_hal_bf_get(device, REG_JTX_DL_204B_3_ADDR,
				    BF_JTX_DL_204B_SYNC_N_INFO, &sync_204b,
				    1); /* paged, Frame Sync */
	AD9081_ERROR_RETURN(err);
	res += (sync_204b & 0x1) << 4;

	err = adi_ad9081_hal_bf_get(device, REG_PLL_STATUS_ADDR,
				    BF_JTX_PLL_LOCKED_INFO, &pll_locked,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	res += (pll_locked & 0x1) << 5;

	err = adi_ad9081_hal_bf_get(device, REG_PHASE_ESTABLISH_STATUS_ADDR,
				    BF_JTX_PHASE_ESTABLISHED_INFO,
				    &phase_establish, 1); /* paged */
	AD9081_ERROR_RETURN(err);
	res += (phase_establish & 0x1) << 6;

	err = adi_ad9081_hal_bf_get(device, REG_QC_MODE_STATUS_ADDR,
				    BF_JTX_INVALID_MODE_INFO, &invalid_mode,
				    1); /* paged */
	AD9081_ERROR_RETURN(err);
	res += (invalid_mode & 0x1) << 7;

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

	err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
	AD9081_ERROR_RETURN(err);
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
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		/* 0: 16bit, 1: 15bit, 2: 14bit, 3: 13bit, 4: 12bit, ..., 8: 8bit */
		err = adi_ad9081_hal_bf_set(device, REG_OUT_RES_ADDR,
					    BF_DFORMAT_RES_INFO,
					    resolution); /* paged */
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_OUT_RES_ADDR,
					    BF_DFORMAT_RES_INFO,
					    resolution); /* paged */
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

int32_t adi_ad9081_jesd_tx_startup_ser(adi_ad9081_device_t *device,
				       uint8_t lanes)
{
	int32_t err;
	uint8_t reg13;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* toggle pown down bits */
	err = adi_ad9081_hal_bf_set(device, REG_PWR_DN_ADDR, BF_PD_SER_INFO,
				    lanes); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 300000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_PWR_DN_ADDR, BF_PD_SER_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* drive slice offsets */
	err = adi_ad9081_hal_bf_set(device, REG_EN_DRVSLICEOFFSET_ADDR,
				    0x00000107, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_EN_DRVSLICEOFFSET_ADDR,
				    0x00000106, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_EN_DRVSLICEOFFSET_ADDR,
				    0x00000105, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_EN_DRVSLICEOFFSET_ADDR,
				    0x00000104, 1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_EN_DRVSLICEOFFSET_ADDR,
				    BF_EN_DRVSLICEOFFSET_CH67_SER_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_EN_DRVSLICEOFFSET_ADDR,
				    BF_EN_DRVSLICEOFFSET_CH45_SER_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_EN_DRVSLICEOFFSET_ADDR,
				    BF_EN_DRVSLICEOFFSET_CH23_SER_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_EN_DRVSLICEOFFSET_ADDR,
				    BF_EN_DRVSLICEOFFSET_CH01_SER_RC_INFO,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 100000);
	AD9081_ERROR_RETURN(err);

	/* reset phy */
	err = adi_ad9081_hal_bf_set(device, REG_RSTB_ADDR, BF_RSTB_SER_INFO,
				    lanes); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 100000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RSTB_ADDR, BF_RSTB_SER_INFO,
				    0); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 100000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RSTB_ADDR, BF_RSTB_SER_INFO,
				    lanes); /* not paged */
	AD9081_ERROR_RETURN(err);

	/* set fifo start addr to 1 */
	err = adi_ad9081_hal_cbusjtx_reg_get(device, 0x0D, &reg13, 0);
	AD9081_ERROR_RETURN(err);
	reg13 = (reg13 & 0x1) | 0x2;
	err = adi_ad9081_hal_cbusjtx_reg_set(device, 0x0D, reg13, lanes);
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
				 uint32_t time_sec)
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
	for (i = 0; i < time_sec; i++) {
		err = adi_ad9081_hal_delay_us(device, 1000000); /* 1s */
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
	uint8_t jrx_cbus_reg_val;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_cbusjrx_reg_get(device, 0x0D, &jrx_cbus_reg_val,
					     lane); /* rx/sel_lf_spo_des_rc */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x0D, (jrx_cbus_reg_val & 0x80) | spo, lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x0D, &jrx_cbus_reg_val,
		lane); /* rx/sel_lf_spo_latch_des_rc */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x0D, (jrx_cbus_reg_val & 0x7F) | 0, lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x0D, &jrx_cbus_reg_val,
		lane); /* rx/sel_lf_spo_latch_des_rc */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x0D, (jrx_cbus_reg_val & 0x7F) | 1, lane);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_get(
		device, 0x0D, &jrx_cbus_reg_val,
		lane); /* rx/sel_lf_spo_latch_des_rc */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_cbusjrx_reg_set(
		device, 0x0D, (jrx_cbus_reg_val & 0x7F) | 0, lane);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_jesd_rx_spo_test(adi_ad9081_device_t *device, uint8_t lane,
				    adi_cms_jesd_prbs_pattern_e prbs_pattern,
				    adi_ad9081_spo_mode_e spo_mode,
				    adi_ad9081_spo_t *spo, uint32_t time_sec)
{
	int32_t err;
	int8_t i;
	uint8_t spo_value, temp_len;
	adi_ad9081_prbs_test_t prbs_rx_result;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* decode SPO mode */
	switch (spo_mode) {
	case AD9081_HALF_RATE:
		temp_len = 33;
		break;
	case AD9081_QUART_RATE:
		temp_len = 16;
		break;
	default:
		return API_CMS_ERROR_INVALID_PARAM;
	}

	for (i = 1; i < temp_len; i++) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, -1 * i, 7,
						     &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_phy_prbs_test(device, prbs_pattern,
						       time_sec);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_phy_prbs_test_result_get(
			device, lane, &prbs_rx_result);
		AD9081_ERROR_RETURN(err);
		if (prbs_rx_result.phy_prbs_err_cnt == 0) {
			spo->left_spo = i;
		} else {
			break;
		}
	}
	for (i = -1 * spo->left_spo; i <= 0; i++) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, i, 7, &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);
	}

	for (i = 1; i < temp_len; i++) {
		err = adi_ad9081_jesd_rx_gen_2s_comp(device, i, 7, &spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_spo_set(device, lane, spo_value);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_phy_prbs_test(device, prbs_pattern,
						       time_sec);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_jesd_rx_phy_prbs_test_result_get(
			device, lane, &prbs_rx_result);
		AD9081_ERROR_RETURN(err);
		if (prbs_rx_result.phy_prbs_err_cnt == 0) {
			spo->right_spo = i;
		} else {
			break;
		}
	}

	for (i = -1 * spo->right_spo; i >= 0; i--) {
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
					    uint8_t *locked)
{
	int32_t err;
	uint8_t freq_acq_state, lock_bit;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_cbuspll_reg_get(
		device, 0xaa, &freq_acq_state); /* pll/freqacqstate_lcpll_rs */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_PLL_STATUS_LCPLL_ADDR,
				    BF_LCPLLLOCK_LCPLL_RS_INFO, &lock_bit,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	*locked = 1;
	if (((freq_acq_state & 0x0f) == 0x8) && (lock_bit == 0x00)) {
		err = adi_ad9081_hal_bf_set(device, REG_PLL_ENABLE_CTRL_ADDR,
					    BF_LCPLL_JTX_PLL_BYPASS_LOCK_INFO,
					    1); /* not paged */
		AD9081_ERROR_RETURN(err);
	} else if ((freq_acq_state & 0x0f) != 0x8) {
		*locked = 0;
	}

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

int32_t adi_ad9081_jesd_oneshot_sync(adi_ad9081_device_t *device)
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
		AD9081_LOG_ERR("sysref_mode_oneshot bit never cleared.");
	}
	err = adi_ad9081_hal_bf_get(device, REG_SYSREF_MODE_ADDR,
				    BF_ONESHOT_SYNC_DONE_INFO, &sync_done,
				    1); /* not paged */
	AD9081_ERROR_RETURN(err);
	if (sync_done != 1)
		AD9081_LOG_ERR("oneshot sync not finished.");
	err = adi_ad9081_hal_bf_set(device, REG_CLK_CTRL1_ADDR, 0x00000102,
				    pd_fdacby4); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

/*! @} */