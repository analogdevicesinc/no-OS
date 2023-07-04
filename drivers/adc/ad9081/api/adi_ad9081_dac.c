/*!
 * @brief     APIs for DAC NCO configuration and control
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9081_DAC_API
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
/*============= M A C R O S ====================*/
#define AD9081_R2R_DAC_CAL_ADDR 0x17
#define AD9081_R2R_DAC_CAL_CFG 0x4F
#define AD9081_DAC_CAL_RUN_CFG 0x22
#define AD9081_DAC_CAL_COMP_SPEED 0x1
#define AD9081_DAC_CAL_THROWAWAY_CURR_CFG 0x11
/*============= H E L P E R ====================*/
/**
 * @brief  Run Startup DAC Cals
 *         Run After Startup, during startup
 * @param  device    Pointer to the device structure.
 * @param  dacs      Mask of Target DACs, refer to enum adi_ad9081_dac_select_e

 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_run_startup_cal(adi_ad9081_device_t *device,
				       uint8_t dacs);

/**
 * @brief  Run Startup DAC Cals
 *         Run After Startup, during startup
 * @param  device    Pointer to the device structure.
 * @param  config    Config Setting for R2R Cals, Do Not Modify. Use ADI recommended Setting (0x79)
 * @param  dacs      Mask of Target DACs, refer to enum adi_ad9081_dac_select_e

 * @return API_CMS_ERROR_OK                     API Completed Successfully
 * @return <0                                   Failed. @see adi_cms_error_e for details.
 */
int32_t adi_ad9081_dac_r2r_cal_config_set(adi_ad9081_device_t *device,
					  uint8_t config, uint8_t dacs);

/*============= C O D E ====================*/
int32_t adi_ad9081_dac_select_set(adi_ad9081_device_t *device, uint8_t dacs)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(dacs > AD9081_DAC_ALL);
	return adi_ad9081_hal_reg_set(device, REG_PAGEINDX_DAC_MAINDP_DAC_ADDR,
				      dacs); /* not paged */
}

int32_t adi_ad9081_dac_chan_select_set(adi_ad9081_device_t *device,
				       uint8_t channels)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(channels > AD9081_DAC_CH_ALL);
	return adi_ad9081_hal_reg_set(device, REG_PAGEINDX_DAC_CHAN_ADDR,
				      channels); /* not paged */
}

int32_t adi_ad9081_dac_duc_select_set(adi_ad9081_device_t *device, uint8_t dacs,
				      uint8_t channels)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_dac_select_set(device, dacs);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_dac_chan_select_set(device, channels);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_d2a_dual_spi_enable_set(adi_ad9081_device_t *device,
					       uint8_t duals, uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_2bf_set(
		device, REG_SPI_ENABLE_DAC_ADDR, BF_SPI_EN_D2A0_INFO,
		((duals & 0x1) > 0 ? 0x1 : 0x0) & enable, BF_SPI_EN_D2A1_INFO,
		((duals & 0x2) > 0 ? 0x1 : 0x0) & enable); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_mode_switch_group_select_set(
	adi_ad9081_device_t *device, adi_ad9081_dac_pair_select_e dac_pair)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	return adi_ad9081_hal_bf_set(device, REG_PAGEINDX_DAC_JRX_ADDR,
				     BF_MODS_MSK_INFO, dac_pair);
}

int32_t
adi_ad9081_dac_modulation_mux_mode_set(adi_ad9081_device_t *device,
				       adi_ad9081_dac_pair_select_e dac_pair,
				       adi_ad9081_dac_mod_mux_mode_e mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	if (((mode > 0x3) || (mode < 0) || (dac_pair == 0))) {
		AD9081_INVALID_PARAM_RETURN(1);
	};
	err = adi_ad9081_dac_mode_switch_group_select_set(device, dac_pair);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_DDSM_DATAPATH_CFG_ADDR,
				    BF_DDSM_MODE_INFO, mode); /* paged */
	err = adi_ad9081_dac_mode_switch_group_select_set(device,
							  AD9081_DAC_PAIR_NONE);
	AD9081_ERROR_RETURN(err);
#if 0
    if ((dac_pair & AD9081_DAC_PAIR_0_1) > 0) {
        err = adi_ad9081_dac_mode_switch_group_select_set(device, AD9081_DAC_MODE_SWITCH_GROUP_0);
        AD9081_ERROR_RETURN(err);
        err = adi_ad9081_hal_bf_set(device, REG_DDSM_DATAPATH_CFG_ADDR, BF_DDSM_MODE_INFO, mode); /* paged */
        AD9081_ERROR_RETURN(err);
    }
    if ((dac_pair & AD9081_DAC_PAIR_2_3) > 0) {
        err = adi_ad9081_dac_mode_switch_group_select_set(device, AD9081_DAC_MODE_SWITCH_GROUP_1);
        AD9081_ERROR_RETURN(err);
        err = adi_ad9081_hal_bf_set(device, REG_DDSM_DATAPATH_CFG_ADDR, BF_DDSM_MODE_INFO, mode); /* paged */
        AD9081_ERROR_RETURN(err);
    }
#endif
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_complex_modulation_enable_set(
	adi_ad9081_device_t *device, adi_ad9081_dac_pair_select_e dac_pair,
	uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	if (((enable > 1) || (dac_pair == 0x0))) {
		AD9081_INVALID_PARAM_RETURN(1);
	};
	err = adi_ad9081_dac_mode_switch_group_select_set(device, dac_pair);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_DDSM_DATAPATH_CFG_ADDR,
				    BF_EN_CMPLX_MODULATION_INFO,
				    enable); /* paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_dac_mode_switch_group_select_set(device,
							  AD9081_DAC_PAIR_NONE);
	AD9081_ERROR_RETURN(err);
#if 0
    if ((dac_pair & AD9081_DAC_MODE_SWITCH_GROUP_0) > 0) {
        err = adi_ad9081_dac_mode_switch_group_select_set(device, AD9081_DAC_MODE_SWITCH_GROUP_0);
        AD9081_ERROR_RETURN(err);
        err = adi_ad9081_hal_bf_set(device, REG_DDSM_DATAPATH_CFG_ADDR, BF_EN_CMPLX_MODULATION_INFO, enable); /* paged */
        AD9081_ERROR_RETURN(err);
    }
    if ((dac_pair & AD9081_DAC_MODE_SWITCH_GROUP_1) > 0) {
        err = adi_ad9081_dac_mode_switch_group_select_set(device, AD9081_DAC_MODE_SWITCH_GROUP_1);
        AD9081_ERROR_RETURN(err);
        err = adi_ad9081_hal_bf_set(device, REG_DDSM_DATAPATH_CFG_ADDR, BF_EN_CMPLX_MODULATION_INFO, enable); /* paged */
        AD9081_ERROR_RETURN(err);
    }
#endif
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_nco_gain_set(adi_ad9081_device_t *device,
					uint8_t channels, uint16_t gain)
{
	int32_t err;
	uint8_t i, channel;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(gain > 0x0FFF);

	for (i = 0; i < 8; i++) {
		channel = channels & (AD9081_DAC_CH_0 << i);
		if (channel > 0) {
			err = adi_ad9081_dac_chan_select_set(device, channel);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_CHNL_GAIN0_ADDR,
						    BF_CHNL_GAIN_INFO,
						    gain); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_nco_gains_set(adi_ad9081_device_t *device,
					 uint16_t gains[8])
{
	int32_t err;
	uint8_t i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		err = adi_ad9081_dac_duc_nco_gain_set(
			device, AD9081_DAC_CH_0 << i, gains[i]);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_chan_skew_set(adi_ad9081_device_t *device,
					 uint8_t channels, uint8_t skew)
{
	int32_t err;
	uint8_t i, channel;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		channel = channels & (AD9081_DAC_CH_0 << i);
		if (channel > 0) {
			err = adi_ad9081_dac_chan_select_set(device, channel);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_CHNL_SKEW_ADJUST_ADDR,
						    BF_CHNL_SKEW_ADJ_INFO,
						    skew); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_nco_reset_set(adi_ad9081_device_t *device,
					 uint8_t chan_nco_reset,
					 uint8_t main_nco_reset)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_CHNL_NCO_RST_EN_ADDR,
				    BF_SPI_CHNL_NCO_RST_EN_INFO,
				    chan_nco_reset); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_MAIN_NCO_RST_EN_ADDR,
				    BF_SPI_MAIN_NCO_RST_EN_INFO,
				    main_nco_reset); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_nco_enable_set(adi_ad9081_device_t *device,
					  uint8_t dacs, uint8_t channels,
					  uint8_t enable)
{
	int32_t err;
	uint8_t i, dac, channel;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(dacs > AD9081_DAC_ALL);

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDSM_DATAPATH_CFG_ADDR,
						    BF_DDSM_NCO_EN_INFO,
						    enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	for (i = 0; i < 8; i++) {
		channel = channels & (AD9081_DAC_CH_0 << i);
		if (channel > 0) {
			err = adi_ad9081_dac_chan_select_set(device, channel);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDSC_DATAPATH_CFG_ADDR,
						    BF_DDSC_NCO_EN_INFO,
						    enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_nco_phase_offset_set(adi_ad9081_device_t *device,
						uint8_t dacs,
						uint16_t dac_phase_offset,
						uint8_t channels,
						uint16_t ch_phase_offset)
{
	int32_t err;
	uint8_t i, dac, channel;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(dacs > AD9081_DAC_ALL);

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_DDSM_PHASE_OFFSET0_ADDR,
				BF_DDSM_NCO_PHASE_OFFSET_INFO,
				dac_phase_offset); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	for (i = 0; i < 8; i++) {
		channel = channels & (AD9081_DAC_CH_0 << i);
		if (channel > 0) {
			err = adi_ad9081_dac_chan_select_set(device, channel);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_DDSC_PHASE_OFFSET0_ADDR,
				BF_DDSC_NCO_PHASE_OFFSET_INFO,
				ch_phase_offset); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_nco_ftw0_set(adi_ad9081_device_t *device,
					uint8_t dacs, uint8_t channels,
					uint64_t ftw)
{
	int32_t err;
	uint8_t i, dac, channel, bit_val;
#if AD9081_USE_SPI_BURST_MODE > 0
	uint8_t in_data[8];
#endif
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
#if AD9081_USE_SPI_BURST_MODE > 0
			in_data[0] = (REG_DDSM_FTW0_ADDR >> 8) & 0x3F;
			in_data[1] = (REG_DDSM_FTW0_ADDR >> 0) & 0xFF;
			in_data[2] = (uint8_t)((ftw >> 0) & 0xFF);
			in_data[3] = (uint8_t)((ftw >> 8) & 0xFF);
			in_data[4] = (uint8_t)((ftw >> 16) & 0xFF);
			in_data[5] = (uint8_t)((ftw >> 24) & 0xFF);
			in_data[6] = (uint8_t)((ftw >> 32) & 0xFF);
			in_data[7] = (uint8_t)((ftw >> 40) & 0xFF);
			if (API_CMS_ERROR_OK !=
			    device->hal_info.spi_xfer(device->hal_info.user_data,
						      in_data, NULL, 0x8))
				return API_CMS_ERROR_SPI_XFER;
#else
			err = adi_ad9081_hal_bf_set(device, REG_DDSM_FTW0_ADDR,
						    BF_DDSM_FTW_INFO,
						    ftw); /* paged */
			AD9081_ERROR_RETURN(err);
#endif
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDSM_FTW_UPDATE_ADDR,
						    BF_DDSM_FTW_LOAD_REQ_INFO,
						    0); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDSM_FTW_UPDATE_ADDR,
						    BF_DDSM_FTW_LOAD_REQ_INFO,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device,
						    REG_DDSM_FTW_UPDATE_ADDR,
						    0x00000101, &bit_val,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
			if (bit_val == 0) {
				AD9081_LOG_WARN("main ftw is not updated.");
			}
		}
	}

	for (i = 0; i < 8; i++) {
		channel = channels & (AD9081_DAC_CH_0 << i);
		if (channel > 0) {
			err = adi_ad9081_dac_chan_select_set(device, channel);
			AD9081_ERROR_RETURN(err);
#if AD9081_USE_SPI_BURST_MODE > 0
			in_data[0] = (REG_DDSC_FTW0_ADDR >> 8) & 0x3F;
			in_data[1] = (REG_DDSC_FTW0_ADDR >> 0) & 0xFF;
			in_data[2] = (uint8_t)((ftw >> 0) & 0xFF);
			in_data[3] = (uint8_t)((ftw >> 8) & 0xFF);
			in_data[4] = (uint8_t)((ftw >> 16) & 0xFF);
			in_data[5] = (uint8_t)((ftw >> 24) & 0xFF);
			in_data[6] = (uint8_t)((ftw >> 32) & 0xFF);
			in_data[7] = (uint8_t)((ftw >> 40) & 0xFF);
			if (API_CMS_ERROR_OK !=
			    device->hal_info.spi_xfer(device->hal_info.user_data,
						      in_data, NULL, 0x8))
				return API_CMS_ERROR_SPI_XFER;
#else
			err = adi_ad9081_hal_bf_set(device, REG_DDSC_FTW0_ADDR,
						    BF_DDSC_FTW_INFO,
						    ftw); /* paged */
			AD9081_ERROR_RETURN(err);
#endif
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDSC_FTW_UPDATE_ADDR,
						    BF_DDSC_FTW_LOAD_REQ_INFO,
						    0); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDSC_FTW_UPDATE_ADDR,
						    BF_DDSC_FTW_LOAD_REQ_INFO,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device,
						    REG_DDSC_FTW_UPDATE_ADDR,
						    0x00000101, &bit_val,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
			if (bit_val == 0) {
				AD9081_LOG_WARN("channel ftw is not updated.");
			}
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_nco_ftw_set(adi_ad9081_device_t *device,
				       uint8_t dacs, uint8_t channels,
				       uint64_t ftw, uint64_t acc_modulus,
				       uint64_t acc_delta)
{
	int32_t err;
	uint8_t i, dac, channel;
#if AD9081_USE_SPI_BURST_MODE > 0
	uint8_t in_data[8];
#endif
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	if (acc_modulus + acc_delta >= (1ULL << 48)) {
		return API_CMS_ERROR_INVALID_PARAM;
	}
	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(
				device, REG_DDSM_FTW_UPDATE_ADDR,
				BF_DDSM_FTW_LOAD_SYSREF_INFO, 0, 0x00000304,
				0); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_DDSM_DATAPATH_CFG_ADDR,
				BF_DDSM_MODULUS_EN_INFO,
				(acc_modulus > 0 ? 1 : 0)); /* paged */
			AD9081_ERROR_RETURN(err);
			if (acc_modulus > 0) {
#if AD9081_USE_SPI_BURST_MODE > 0
				in_data[0] = (REG_DDSM_ACC_MODULUS0_ADDR >> 8) &
					     0x3F;
				in_data[1] = (REG_DDSM_ACC_MODULUS0_ADDR >> 0) &
					     0xFF;
				in_data[2] =
					(uint8_t)((acc_modulus >> 0) & 0xFF);
				in_data[3] =
					(uint8_t)((acc_modulus >> 8) & 0xFF);
				in_data[4] =
					(uint8_t)((acc_modulus >> 16) & 0xFF);
				in_data[5] =
					(uint8_t)((acc_modulus >> 24) & 0xFF);
				in_data[6] =
					(uint8_t)((acc_modulus >> 32) & 0xFF);
				in_data[7] =
					(uint8_t)((acc_modulus >> 40) & 0xFF);
				if (API_CMS_ERROR_OK !=
				    device->hal_info.spi_xfer(
					    device->hal_info.user_data, in_data,
					    NULL, 0x8))
					return API_CMS_ERROR_SPI_XFER;
				in_data[0] =
					(BF_DDSM_ACC_DELTA_INFO >> 8) & 0x3F;
				in_data[1] =
					(BF_DDSM_ACC_DELTA_INFO >> 0) & 0xFF;
				in_data[2] = (uint8_t)((acc_delta >> 0) & 0xFF);
				in_data[3] = (uint8_t)((acc_delta >> 8) & 0xFF);
				in_data[4] =
					(uint8_t)((acc_delta >> 16) & 0xFF);
				in_data[5] =
					(uint8_t)((acc_delta >> 24) & 0xFF);
				in_data[6] =
					(uint8_t)((acc_delta >> 32) & 0xFF);
				in_data[7] =
					(uint8_t)((acc_delta >> 40) & 0xFF);
				if (API_CMS_ERROR_OK !=
				    device->hal_info.spi_xfer(
					    device->hal_info.user_data, in_data,
					    NULL, 0x8))
					return API_CMS_ERROR_SPI_XFER;
#else
				err = adi_ad9081_hal_bf_set(
					device, REG_DDSM_ACC_MODULUS0_ADDR,
					BF_DDSM_ACC_MODULUS_INFO,
					acc_modulus); /* paged */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, REG_DDSM_ACC_DELTA0_ADDR,
					BF_DDSM_ACC_DELTA_INFO,
					acc_delta); /* paged */
				AD9081_ERROR_RETURN(err);
#endif
			}
			err = adi_ad9081_dac_duc_nco_ftw0_set(
				device, dacs, AD9081_DAC_CH_NONE, ftw);
			AD9081_ERROR_RETURN(err);
		}
	}

	for (i = 0; i < 8; i++) {
		channel = channels & (AD9081_DAC_CH_0 << i);
		if (channel > 0) {
			err = adi_ad9081_dac_chan_select_set(device, channel);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(
				device, REG_DDSC_FTW_UPDATE_ADDR,
				BF_DDSC_FTW_LOAD_SYSREF_INFO, 0, 0x00000304,
				0); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_DDSC_DATAPATH_CFG_ADDR,
				BF_DDSC_MODULUS_EN_INFO,
				(acc_modulus > 0 ? 1 : 0)); /* paged */
			AD9081_ERROR_RETURN(err);
			if (acc_modulus > 0) {
#if AD9081_USE_SPI_BURST_MODE > 0
				in_data[0] = (REG_DDSC_ACC_MODULUS0_ADDR >> 8) &
					     0x3F;
				in_data[1] = (REG_DDSC_ACC_MODULUS0_ADDR >> 0) &
					     0xFF;
				in_data[2] =
					(uint8_t)((acc_modulus >> 0) & 0xFF);
				in_data[3] =
					(uint8_t)((acc_modulus >> 8) & 0xFF);
				in_data[4] =
					(uint8_t)((acc_modulus >> 16) & 0xFF);
				in_data[5] =
					(uint8_t)((acc_modulus >> 24) & 0xFF);
				in_data[6] =
					(uint8_t)((acc_modulus >> 32) & 0xFF);
				in_data[7] =
					(uint8_t)((acc_modulus >> 40) & 0xFF);
				if (API_CMS_ERROR_OK !=
				    device->hal_info.spi_xfer(
					    device->hal_info.user_data, in_data,
					    NULL, 0x8))
					return API_CMS_ERROR_SPI_XFER;
				in_data[0] =
					(REG_DDSC_ACC_DELTA0_ADDR >> 8) & 0x3F;
				in_data[1] =
					(REG_DDSC_ACC_DELTA0_ADDR >> 0) & 0xFF;
				in_data[2] = (uint8_t)((acc_delta >> 0) & 0xFF);
				in_data[3] = (uint8_t)((acc_delta >> 8) & 0xFF);
				in_data[4] =
					(uint8_t)((acc_delta >> 16) & 0xFF);
				in_data[5] =
					(uint8_t)((acc_delta >> 24) & 0xFF);
				in_data[6] =
					(uint8_t)((acc_delta >> 32) & 0xFF);
				in_data[7] =
					(uint8_t)((acc_delta >> 40) & 0xFF);
				if (API_CMS_ERROR_OK !=
				    device->hal_info.spi_xfer(
					    device->hal_info.user_data, in_data,
					    NULL, 0x8))
					return API_CMS_ERROR_SPI_XFER;
#else
				err = adi_ad9081_hal_bf_set(
					device, REG_DDSC_ACC_MODULUS0_ADDR,
					BF_DDSC_ACC_MODULUS_INFO,
					acc_modulus); /* paged */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, REG_DDSC_ACC_DELTA0_ADDR,
					BF_DDSC_ACC_DELTA_INFO,
					acc_delta); /* paged */
				AD9081_ERROR_RETURN(err);
#endif
			}
			err = adi_ad9081_dac_duc_nco_ftw0_set(
				device, AD9081_DAC_NONE, channel, ftw);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_main_nco_ftw_get(adi_ad9081_device_t *device,
					    uint8_t dacs, uint64_t *ftw,
					    uint64_t *acc_modulus,
					    uint64_t *acc_delta)
{
	int32_t err;
	uint8_t acc_modulus_en;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_dac_select_set(device, dacs);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_DDSM_FTW0_ADDR,
				    BF_DDSM_FTW_INFO, (uint8_t *)ftw,
				    8); /* paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_DDSC_DATAPATH_CFG_ADDR,
				    BF_DDSC_MODULUS_EN_INFO, &acc_modulus_en,
				    1); /* paged */
	AD9081_ERROR_RETURN(err);
	if (acc_modulus_en) {
		err = adi_ad9081_hal_bf_get(device, REG_DDSM_ACC_MODULUS0_ADDR,
					    BF_DDSM_ACC_MODULUS_INFO,
					    (uint8_t *)acc_modulus,
					    8); /* paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device, REG_DDSM_ACC_MODULUS0_ADDR,
					    BF_DDSM_ACC_MODULUS_INFO,
					    (uint8_t *)acc_delta,
					    8); /* paged */
		AD9081_ERROR_RETURN(err);
	} else {
		*acc_modulus = 0;
		*acc_delta = 0;
	}
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_channel_nco_ftw_get(adi_ad9081_device_t *device,
					       uint8_t channels, uint64_t *ftw,
					       uint64_t *acc_modulus,
					       uint64_t *acc_delta)
{
	int32_t err;
	uint8_t acc_modulus_en;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_dac_chan_select_set(device, channels);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_DDSC_FTW0_ADDR,
				    BF_DDSC_FTW_INFO, (uint8_t *)ftw,
				    8); /* paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_DDSC_DATAPATH_CFG_ADDR,
				    BF_DDSC_MODULUS_EN_INFO, &acc_modulus_en,
				    1); /* paged */
	AD9081_ERROR_RETURN(err);
	if (acc_modulus_en) {
		err = adi_ad9081_hal_bf_get(device, REG_DDSC_ACC_MODULUS0_ADDR,
					    BF_DDSC_ACC_MODULUS_INFO,
					    (uint8_t *)acc_modulus,
					    8); /* paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device, REG_DDSC_ACC_DELTA0_ADDR,
					    BF_DDSC_ACC_DELTA_INFO,
					    (uint8_t *)acc_delta,
					    8); /* paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_nco_set(adi_ad9081_device_t *device, uint8_t dacs,
				   uint8_t channels, int64_t nco_shift_hz)
{
	int32_t err;
	uint64_t ftw;
	uint8_t main_interp = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(device->dev_info.dac_freq_hz == 0);

	/* set main nco */
	if (dacs != AD9081_DAC_NONE) {
		err = adi_ad9081_dac_duc_nco_enable_set(device, dacs,
							AD9081_DAC_CH_NONE, 1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_calc_tx_nco_ftw(
			device, device->dev_info.dac_freq_hz, nco_shift_hz,
			&ftw);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_duc_nco_ftw_set(
			device, dacs, AD9081_DAC_CH_NONE, ftw, 0, 0);
		AD9081_ERROR_RETURN(err);
	}

	/* set channel nco */
	if (channels != AD9081_DAC_CH_NONE) {
		err = adi_ad9081_hal_bf_get(device, REG_INTRP_MODE_ADDR,
					    BF_FINE_INTERP_SEL_INFO,
					    &main_interp, 1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_duc_nco_enable_set(device, AD9081_DAC_NONE,
							channels, 1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_calc_tx_nco_ftw(
			device, device->dev_info.dac_freq_hz,
			nco_shift_hz * main_interp, &ftw);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_duc_nco_ftw_set(device, AD9081_DAC_NONE,
						     channels, ftw, 0, 0);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

#if AD9081_USE_FLOATING_TYPE > 0
int32_t adi_ad9081_dac_duc_nco_set_f(adi_ad9081_device_t *device, uint8_t dacs,
				     uint8_t channels, double nco_shift_hz)
{
	int32_t err;
	uint64_t ftw, a, b;
	uint8_t main_interp = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(device->dev_info.dac_freq_hz == 0);

	/* set main nco */
	if (dacs != AD9081_DAC_NONE) {
		err = adi_ad9081_dac_duc_nco_enable_set(device, dacs,
							AD9081_DAC_CH_NONE, 1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_calc_nco_ftw_f(
			device, (double)device->dev_info.dac_freq_hz,
			nco_shift_hz, &ftw, &a, &b);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_duc_nco_ftw_set(
			device, dacs, AD9081_DAC_CH_NONE, ftw, a, b);
		AD9081_ERROR_RETURN(err);
	}

	/* set channel nco */
	if (channels != AD9081_DAC_CH_NONE) {
		err = adi_ad9081_hal_bf_get(device, REG_INTRP_MODE_ADDR,
					    BF_FINE_INTERP_SEL_INFO,
					    &main_interp, 1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_duc_nco_enable_set(device, AD9081_DAC_NONE,
							channels, 1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_calc_nco_ftw_f(
			device, (double)device->dev_info.dac_freq_hz,
			nco_shift_hz * main_interp, &ftw, &a, &b);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_duc_nco_ftw_set(device, AD9081_DAC_NONE,
						     channels, ftw, a, b);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}
#endif

int32_t adi_ad9081_dac_duc_main_nco_hopf_ftw_set(adi_ad9081_device_t *device,
						 uint8_t dacs,
						 uint8_t hopf_index,
						 uint32_t hopf_ftw)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(hopf_index > 31);
	AD9081_INVALID_PARAM_RETURN(hopf_index < 1);

	/* set ftw1 ~ ftw31 */
	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device,
				REG_DDSM_HOPF_FTW1_0_ADDR +
					4 * (hopf_index - 1),
				BF_DDSM_HOPF_FTW1_INFO, hopf_ftw); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_main_nco_hopf_mode_set(adi_ad9081_device_t *device,
						  uint8_t dacs,
						  uint8_t hopf_mode)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			/* 0: phase continuous switch, 1: phase in-continuous switch, 2: phase coherent switch */
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDSM_HOPF_CTRL0_ADDR,
						    BF_DDSM_HOPF_MODE_INFO,
						    hopf_mode); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_main_nco_hopf_select_set(adi_ad9081_device_t *device,
						    uint8_t dacs,
						    uint8_t hopf_index)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(hopf_index > 31);

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDSM_HOPF_CTRL0_ADDR,
						    BF_DDSM_HOPF_SEL_INFO,
						    hopf_index); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_main_nco_hopf_gpio_no_glitch_en_set(
	adi_ad9081_device_t *device, uint8_t dacs, uint8_t enable)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(enable > 1);

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDSM_HOPF_CTRL0_ADDR,
						    0x00000105,
						    enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_dac_duc_main_nco_hopf_gpio_as_hop_en_set(adi_ad9081_device_t *device,
						    uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(enable > 1);

	if (enable > 0) {
		err = adi_ad9081_hal_reg_set(
			device, REG_GPIO_CFG3_ADDR,
			0x13); /* dac_nco_ffh1.dac_nco_ffh0 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_reg_set(
			device, REG_GPIO_CFG0_ADDR,
			0x33); /* dac_nco_ffh3.dac_nco_ffh2 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_reg_set(
			device, REG_GPIO_CFG1_ADDR,
			0x33); /* dac_nco_ffh5.dac_nco_ffh4 */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_reg_set(
			device, REG_GPIO_CFG2_ADDR,
			0x22); /* dac_nco_ffh_strobe.dac_nco_ffh6 */
		AD9081_ERROR_RETURN(err);
	}
	err = adi_ad9081_hal_bf_set(device, REG_DDSM_HOPF_CTRL1_ADDR,
				    BF_GPIO_HOP_EN_INFO,
				    enable); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_gpio_as_sync1_out_set(adi_ad9081_device_t *device,
					     uint8_t mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(mode > 1);

	/* 0: link1_sync, 1: link1_sync with diff mode */
	err = adi_ad9081_hal_bf_set(device, REG_GPIO_CFG3_ADDR,
				    BF_SYNC1_OUTBP_CFG_INFO,
				    mode == 0 ? 1 : 9); /* link1_sync */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_gpio_as_pa_en_set(adi_ad9081_device_t *device,
					 uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(enable > 1);

	if (enable > 0) {
		err = adi_ad9081_hal_reg_set(device, REG_GPIO_CFG0_ADDR,
					     0x11); /* pa1_en.pa0_en */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_reg_set(device, REG_GPIO_CFG1_ADDR,
					     0x21); /* pa3_en.pa2_en */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_gpio_as_tx_en_set(adi_ad9081_device_t *device,
					 uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(enable > 1);

	if (enable > 0) {
		err = adi_ad9081_hal_reg_set(device, REG_GPIO_CFG2_ADDR,
					     0x11); /* txen3.txen1 */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_interpolation_set(adi_ad9081_device_t *device,
					 uint8_t main_interp, uint8_t ch_interp)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_reg_set(device, REG_INTRP_MODE_ADDR,
				     (main_interp << 4) +
					     ch_interp); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_MAINDP_DAC_1XXX_ENABLES_ADDR,
				    BF_MAINDP_DAC_1XXX_EN_SPI_INFO,
				    (ch_interp > 1) ? 0 : 1); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_spi_enable_set(adi_ad9081_device_t *device,
				      uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_reg_set(device, REG_SPI_ENABLE_DAC_ADDR,
				     (enable > 0) ? 0x1f :
						    0x00); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_dll_startup(adi_ad9081_device_t *device, uint8_t dacs)
{
	int32_t err;
	uint8_t bf_val0, bf_val1;
	uint8_t clk_on_val = 2, clk_off_val = 1;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* setup clocks */
	err = adi_ad9081_dac_d2a_dual_spi_enable_set(
		device, 0x3, 1); /* select dual0 & dual1 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_ENABLE_TIMING_CTRL_GENERAL_ADDR,
				    BF_DLLCLK_ENCTRL_INFO,
				    clk_on_val); /* paged */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_dac_d2a_dual_spi_enable_set(device, 0x1,
						     1); /* select dual0 */
	AD9081_ERROR_RETURN(err);
	bf_val0 = (dacs & AD9081_DAC_0) > 0 ? clk_on_val : clk_off_val;
	err = adi_ad9081_hal_3bf_set(device, REG_ENABLE_TIMING_CTRL_DAC0_ADDR,
				     BF_MUSHICLKEN_CTRL_DAC0_INFO, bf_val0,
				     BF_FORCEHIGH_CTRL_DAC0_INFO, bf_val0,
				     BF_SWDCLKEN_CTRL_DAC0_INFO,
				     bf_val0); /* paged */
	AD9081_ERROR_RETURN(err);
	bf_val0 = (dacs & AD9081_DAC_1) > 0 ? clk_on_val : clk_off_val;
	err = adi_ad9081_hal_3bf_set(device, REG_ENABLE_TIMING_CTRL_DAC1_ADDR,
				     BF_MUSHICLKEN_CTRL_DAC1_INFO, bf_val0,
				     BF_FORCEHIGH_CTRL_DAC1_INFO, bf_val0,
				     BF_SWDCLKEN_CTRL_DAC1_INFO,
				     bf_val0); /* paged */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_dac_d2a_dual_spi_enable_set(device, 0x2,
						     1); /* select dual1 */
	AD9081_ERROR_RETURN(err);
	bf_val0 = (dacs & AD9081_DAC_3) > 0 ? clk_on_val : clk_off_val;
	err = adi_ad9081_hal_3bf_set(device, REG_ENABLE_TIMING_CTRL_DAC0_ADDR,
				     BF_MUSHICLKEN_CTRL_DAC0_INFO, bf_val0,
				     BF_FORCEHIGH_CTRL_DAC0_INFO, bf_val0,
				     BF_SWDCLKEN_CTRL_DAC0_INFO,
				     bf_val0); /* paged */
	AD9081_ERROR_RETURN(err);
	bf_val0 = (dacs & AD9081_DAC_2) > 0 ? clk_on_val : clk_off_val;
	err = adi_ad9081_hal_3bf_set(device, REG_ENABLE_TIMING_CTRL_DAC1_ADDR,
				     BF_MUSHICLKEN_CTRL_DAC1_INFO, bf_val0,
				     BF_FORCEHIGH_CTRL_DAC1_INFO, bf_val0,
				     BF_SWDCLKEN_CTRL_DAC1_INFO,
				     bf_val0); /* paged */
	AD9081_ERROR_RETURN(err);

	/* setup mushi */
	err = adi_ad9081_dac_d2a_dual_spi_enable_set(device, 0x1,
						     1); /* select dual0 */
	AD9081_ERROR_RETURN(err);
	bf_val0 = (dacs & AD9081_DAC_0) > 0 ? 0 : 1;
	bf_val1 = (dacs & AD9081_DAC_1) > 0 ? 0 : 1;
	err = adi_ad9081_hal_4bf_set(device, REG_MUSHI_CTRL_ADDR,
				     BF_MUSHI_PD0_DUM_INFO, bf_val0,
				     BF_MUSHI_PD0_INFO, bf_val0,
				     BF_MUSHI_PD1_DUM_INFO, bf_val1,
				     BF_MUSHI_PD1_INFO, bf_val1); /* paged */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_dac_d2a_dual_spi_enable_set(device, 0x2,
						     1); /* select dual1 */
	AD9081_ERROR_RETURN(err);
	bf_val0 = (dacs & AD9081_DAC_3) > 0 ? 0 : 1;
	bf_val1 = (dacs & AD9081_DAC_2) > 0 ? 0 : 1;
	err = adi_ad9081_hal_4bf_set(device, REG_MUSHI_CTRL_ADDR,
				     BF_MUSHI_PD0_DUM_INFO, bf_val0,
				     BF_MUSHI_PD0_INFO, bf_val0,
				     BF_MUSHI_PD1_DUM_INFO, bf_val1,
				     BF_MUSHI_PD1_INFO, bf_val1); /* paged */
	AD9081_ERROR_RETURN(err);

	/* override dcc code */
	err = adi_ad9081_dac_d2a_dual_spi_enable_set(
		device, 0x3, 1); /* select dual0 & dual1 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_2bf_set(device, 0x00000192, 0x00000600, 0x00,
				     0x00000107, 0x01);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_soft_off_gain_enable_set(adi_ad9081_device_t *device,
						uint8_t dacs, uint8_t enable)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_BE_SOFT_OFF_GAIN_CTRL_ADDR,
				BF_BE_SOFT_OFF_GAIN_EN_INFO,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_soft_off_new_gain_enable_set(adi_ad9081_device_t *device,
						    uint8_t dacs,
						    uint8_t enable)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_BE_SOFT_OFF_GAIN_CTRL_ADDR,
				0x00000103, enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_soft_off_gain_ramp_rate_set(adi_ad9081_device_t *device,
						   uint8_t dacs, uint8_t rate)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_BE_SOFT_OFF_GAIN_CTRL_ADDR,
				BF_BE_GAIN_RAMP_RATE_INFO, rate); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_soft_off_enable_set(adi_ad9081_device_t *device,
					   uint8_t dacs, uint16_t enable)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_BE_SOFT_OFF_ENABLE0_ADDR, 0x1000,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_soft_on_enable_set(adi_ad9081_device_t *device,
					  uint8_t dacs, uint8_t enable)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_BE_SOFT_ON_ENABLE_ADDR,
						    0x0800, enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_long_pa_set(adi_ad9081_device_t *device, uint8_t dacs,
				   uint8_t enable, uint8_t averaging_time,
				   uint16_t average_threshold)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_LONG_PA_CONTROL_ADDR,
						    BF_LONG_PA_AVG_TIME_INFO,
						    averaging_time); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_LONG_PA_THRES_LSB_ADDR,
				BF_LONG_PA_THRESHOLD_INFO,
				average_threshold); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_LONG_PA_CONTROL_ADDR,
						    BF_LONG_PA_ENABLE_INFO,
						    enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_long_pa_power_get(adi_ad9081_device_t *device,
					 uint8_t dacs, uint16_t *power)
{
	int32_t err;
	uint8_t i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((dacs & (AD9081_DAC_0 << i)) > 0) {
			err = adi_ad9081_dac_select_set(
				device, (dacs & (AD9081_DAC_0 << i)));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(
				device, REG_LONG_PA_POWER_LSB_ADDR,
				BF_LONG_PA_POWER_INFO, (uint8_t *)power,
				sizeof(uint16_t)); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_short_pa_set(adi_ad9081_device_t *device, uint8_t dacs,
				    uint8_t enable, uint8_t averaging_time,
				    uint16_t average_threshold)
{
	int32_t err;
	uint8_t i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((dacs & (AD9081_DAC_0 << i)) > 0) {
			err = adi_ad9081_dac_select_set(
				device, (dacs & (AD9081_DAC_0 << i)));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_SHORT_PA_CONTROL_ADDR,
						    BF_SHORT_PA_AVG_TIME_INFO,
						    averaging_time); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_SHORT_PA_THRES_LSB_ADDR,
				BF_SHORT_PA_THRESHOLD_INFO,
				average_threshold); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_SHORT_PA_CONTROL_ADDR,
						    BF_SHORT_PA_ENABLE_INFO,
						    enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_short_pa_power_get(adi_ad9081_device_t *device,
					  uint8_t dacs, uint16_t *power)
{
	int32_t err;
	uint8_t i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((dacs & (AD9081_DAC_0 << i)) > 0) {
			err = adi_ad9081_dac_select_set(
				device, (dacs & (AD9081_DAC_0 << i)));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(
				device, REG_SHORT_PA_POWER_LSB_ADDR,
				BF_SHORT_PA_POWER_INFO, (uint8_t *)power,
				sizeof(uint16_t)); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_rotation_mode_set(adi_ad9081_device_t *device,
					 uint8_t mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* bit0: enable jesd auto off/on during rotation, bit1: enable data path auto soft off/on during rotation */
	err = adi_ad9081_hal_bf_set(device, REG_ROTATION_MODE_ADDR,
				    BF_ROTATION_MODE_INFO,
				    mode); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_xbar_set(adi_ad9081_device_t *device, uint8_t dacs,
				uint8_t channel)
{
	int32_t err;
	uint8_t ch_interp;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(dacs > AD9081_DAC_ALL);

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device, REG_INTRP_MODE_ADDR,
						    BF_COARSE_INTERP_SEL_INFO,
						    &ch_interp,
						    1); /* not paged */
			AD9081_ERROR_RETURN(err);
			if (ch_interp > 1) {
				err = adi_ad9081_hal_bf_set(
					device, 0x000001BA, 0x00000800,
					channel); /* paged */
				AD9081_ERROR_RETURN(err);
			} else { /* crossbar for 1xNx mode */
				err = adi_ad9081_hal_bf_set(
					device,
					REG_MAINDP_DAC_1XXX_ENABLES_ADDR,
					BF_MAINDP_DAC_1XXX_ENABLES_INFO,
					channel); /* paged */
				AD9081_ERROR_RETURN(err);
			}
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_irqs_enable_set(adi_ad9081_device_t *device,
				       uint64_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* enable irqs */
	err = adi_ad9081_hal_bf_set(device, REG_IRQ_ENABLE_0_ADDR, 0x2800,
				    enable); /* 5 8bit regs */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_irqs_status_get(adi_ad9081_device_t *device,
				       uint64_t *status)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* get irqs status */
	err = adi_ad9081_hal_bf_get(device, REG_IRQ_STATUS0_ADDR, 0x2800,
				    (uint8_t *)status, sizeof(uint64_t));
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_irqs_status_clr(adi_ad9081_device_t *device,
				       uint64_t clear)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* clr irqs status */
	err = adi_ad9081_hal_bf_set(device, REG_IRQ_STATUS0_ADDR, 0x2800,
				    clear);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_shuffle_enable_set(adi_ad9081_device_t *device,
					  uint8_t dacs, uint8_t enable)
{
	int32_t err;
	uint8_t duals = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* DAC0/1 in dual0, DAC2/3 in dual1 */
	duals |= ((dacs & AD9081_DAC_0) > 0 ? 0x1 : 0x0);
	duals |= ((dacs & AD9081_DAC_1) > 0 ? 0x1 : 0x0);
	duals |= ((dacs & AD9081_DAC_2) > 0 ? 0x2 : 0x0);
	duals |= ((dacs & AD9081_DAC_3) > 0 ? 0x2 : 0x0);
	err = adi_ad9081_dac_d2a_dual_spi_enable_set(device, duals, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_2bf_set(device, REG_DECODE_MODE_ADDR,
				     BF_MSB_MODE_INFO, enable, BF_ISB_MODE_INFO,
				     enable); /* paged */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_dac_d2a_dual_spi_enable_set(device, 0x3, 1);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_data_xor_set(adi_ad9081_device_t *device, uint8_t dacs,
				    uint8_t enable)
{
	int32_t err;
	uint8_t xor_en, dexor_en1, dexor_en2;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	xor_en = dacs & (enable > 0 ? 0xf : 0x0);
	err = adi_ad9081_hal_bf_set(device, REG_RETIMER_DEBUG0_ADDR,
				    BF_DAC_DATA_XOR_EN_INFO,
				    xor_en); /* not paged */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_dac_d2a_dual_spi_enable_set(device, 0x1,
						     1); /* select dual0 */
	AD9081_ERROR_RETURN(err);
	dexor_en1 = ((dacs & AD9081_DAC_0) > 0 ? 0x1 : 0x0) &
		    (enable > 0 ? 0x1 : 0x0);
	dexor_en2 = ((dacs & AD9081_DAC_1) > 0 ? 0x1 : 0x0) &
		    (enable > 0 ? 0x1 : 0x0);
	err = adi_ad9081_hal_2bf_set(device, REG_HANDOFF_DEBUG_ADDR,
				     BF_DATA_DEXOR_EN_DAC0_INFO, dexor_en1,
				     BF_DATA_DEXOR_EN_DAC1_INFO,
				     dexor_en2); /* paged */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_dac_d2a_dual_spi_enable_set(device, 0x2,
						     1); /* select dual1 */
	AD9081_ERROR_RETURN(err);
	dexor_en1 = ((dacs & AD9081_DAC_3) > 0 ? 0x1 : 0x0) &
		    (enable > 0 ? 0x1 : 0x0);
	dexor_en2 = ((dacs & AD9081_DAC_2) > 0 ? 0x1 : 0x0) &
		    (enable > 0 ? 0x1 : 0x0);
	err = adi_ad9081_hal_2bf_set(device, REG_HANDOFF_DEBUG_ADDR,
				     BF_DATA_DEXOR_EN_DAC0_INFO, dexor_en1,
				     BF_DATA_DEXOR_EN_DAC1_INFO,
				     dexor_en2); /* paged */
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_dac_d2a_dual_spi_enable_set(device, 0x3, 1);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_dac_tx_enable_state_machine_enable_set(adi_ad9081_device_t *device,
						  uint8_t dacs, uint8_t enable)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			/* setup txen state machine */
			err = adi_ad9081_hal_bf_set(device, REG_TXEN_SM_0_ADDR,
						    BF_ENA_TXENSM_INFO,
						    enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_spi_as_tx_en_set(adi_ad9081_device_t *device,
					uint8_t dacs, uint8_t enable)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			/* set txen enable */
			err = adi_ad9081_hal_bf_set(device,
						    REG_BLANKING_CTRL_ADDR,
						    BF_ENA_SPI_TXEN_INFO,
						    enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_tx_enable_set(adi_ad9081_device_t *device, uint8_t dacs,
				     uint8_t enable)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_spi_as_tx_en_set(device, dacs, 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			/* set txen */
			err = adi_ad9081_hal_bf_set(device,
						    REG_BLANKING_CTRL_ADDR,
						    BF_SPI_TXEN_INFO,
						    enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_power_up_set(adi_ad9081_device_t *device, uint8_t dacs,
				    uint8_t enable)
{
	int32_t err;
	uint8_t tmp_reg = 0x00;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(dacs > AD9081_DAC_ALL);
	AD9081_INVALID_PARAM_RETURN(enable > 1);

	/* power up dacs */
	err = adi_ad9081_hal_reg_get(device, (uint16_t)REG_DAC_POWERDOWN_ADDR,
				     &tmp_reg);
	AD9081_ERROR_RETURN(err);
	if (enable) {
		tmp_reg &= (~(uint8_t)dacs);
	} else {
		tmp_reg |= (uint8_t)dacs;
	}
	err = adi_ad9081_hal_reg_set(device, (uint16_t)REG_DAC_POWERDOWN_ADDR,
				     tmp_reg);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_nco_set(adi_ad9081_device_t *device, uint8_t dacs,
			       uint8_t channels, int64_t shift_hz,
			       uint16_t offset, uint8_t test_tone_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* set nco shift */
	err = adi_ad9081_dac_duc_nco_set(device, dacs, channels, shift_hz);
	AD9081_ERROR_RETURN(err);

	/* set test tone amplitude */
	if (dacs != AD9081_DAC_NONE) {
		err = adi_ad9081_dac_duc_main_dc_test_tone_en_set(device, dacs,
								  test_tone_en);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_duc_main_dc_test_tone_offset_set(
			device, dacs, offset);
		AD9081_ERROR_RETURN(err);
	}
	if (channels != AD9081_DAC_CH_NONE) {
		err = adi_ad9081_dac_dc_test_tone_en_set(device, channels,
							 test_tone_en);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_dc_test_tone_offset_set(device, channels,
							     offset);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_dc_test_tone_en_set(adi_ad9081_device_t *device,
					   uint8_t channels, uint8_t enable)
{
	int32_t err;
	uint8_t i, channel;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(enable > 1);

	for (i = 0; i < 8; i++) {
		channel = channels & (AD9081_DAC_CH_0 << i);
		if (channel > 0) {
			err = adi_ad9081_dac_chan_select_set(device, channel);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDSC_DATAPATH_CFG_ADDR,
						    BF_TEST_TONE_EN_INFO,
						    enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_dc_test_tone_offset_set(adi_ad9081_device_t *device,
					       uint8_t channels,
					       uint16_t offset)
{
	int32_t err;
	uint8_t i, channel;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		channel = channels & (AD9081_DAC_CH_0 << i);
		if (channel > 0) {
			err = adi_ad9081_dac_chan_select_set(device, channel);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_DC_OFFSET0_ADDR,
						    BF_DC_OFFSET_INFO,
						    offset); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_main_dc_test_tone_en_set(adi_ad9081_device_t *device,
						    uint8_t dacs,
						    uint8_t enable)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_DDSM_CAL_MODE_DEF_ADDR,
				BF_DDSM_EN_CAL_DC_INPUT_INFO,
				enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_dac_duc_main_dc_test_tone_offset_set(adi_ad9081_device_t *device,
						uint8_t dacs, uint16_t offset)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x000001E0,
						    0x00001000,
						    offset); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_main_dsa_enable_set(adi_ad9081_device_t *device,
					       uint8_t dacs, uint8_t enable)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_DSA_CFG2_ADDR,
						    BF_EN_DSA_CTRL_INFO,
						    enable); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_duc_main_dsa_set(adi_ad9081_device_t *device,
					uint8_t dacs, uint8_t code,
					uint8_t cutover, uint8_t boost,
					uint16_t gain)
{
	int32_t err;
	uint8_t i, dac;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_DSA_CFG0_ADDR,
						    BF_DSA_CTRL_INFO,
						    code); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_DSA_CFG1_ADDR,
						    BF_DSA_CUTOVER_INFO,
						    cutover); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_DSA_CFG2_ADDR,
						    BF_DSA_BOOST_INFO,
						    boost); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_DP_GAIN0_ADDR,
						    BF_DP_GAIN_INFO,
						    gain); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_DP_GAIN1_ADDR,
						    BF_GAIN_LOAD_STROBE_INFO,
						    1); /* paged */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_DP_GAIN1_ADDR,
						    BF_GAIN_LOAD_STROBE_INFO,
						    0); /* paged */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_fsc_set(adi_ad9081_device_t *device, uint8_t dacs,
			       uint32_t uA, uint8_t rerun_cal)
{
	int32_t err;
	uint8_t i, dac, min_code;
	uint16_t c, f, fsc_code;
	uint32_t min_fsc, cf, ff;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	if (uA > 40000) {
		AD9081_LOG_WARN(
			"full-scale current beyond design recommendation (7mA - 40mA)");
	}
	if (uA < 7000)
		AD9081_LOG_WARN(
			"full-scale current beyond design recommendation (7mA - 40mA)");
	if (rerun_cal > 1)
		AD9081_INVALID_PARAM_RETURN(device);

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_select_set(device, dac);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device, REG_FSC0_ADDR,
						    BF_FSC_MIN_CTRL_INFO,
						    &min_code, 1);
			AD9081_ERROR_RETURN(err);

			min_fsc = (min_code * 25000) >> 4;
			if ((min_fsc + 24975) <
			    uA) { /* 24975 = 25.0 * 1023 / 1024 */
				while (((min_fsc + 24975) < uA) &&
				       (min_fsc < 15625)) {
					min_code = (min_code < 10) ?
							   (min_code + 1) :
							   10;
					min_fsc = (min_code * 25000) >> 4;
				}
			} else {
				while (((min_fsc + 5000) > uA) &&
				       (min_fsc > 1600)) {
					min_code = (min_code > 1) ?
							   (min_code - 1) :
							   1;
					min_fsc = (min_code * 25000) >> 4;
				}
			}
			err = adi_ad9081_hal_bf_set(device, REG_FSC0_ADDR,
						    BF_FSC_MIN_CTRL_INFO,
						    min_code);
			AD9081_ERROR_RETURN(err);
			c = (uint16_t)((((uA - min_fsc) << 10) + 25000 + 1) /
				       25000);
			f = (uint16_t)((((uA - min_fsc) << 10) / 25000));
			cf = min_fsc + ((c * 25000) >> 10);
			ff = min_fsc + ((f * 25000) >> 10);
			cf = cf > uA ? (cf - uA) : (uA - cf);
			ff = ff > uA ? (ff - uA) : (uA - ff);
			fsc_code = (cf < ff) ? c : f;
			fsc_code = (fsc_code < 0x3ff) ? fsc_code : 0x3ff;
			err = adi_ad9081_hal_bf_set(device, REG_FSC0_ADDR,
						    0x0200, fsc_code & 3);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, REG_FSC1_ADDR,
						    0x0800, fsc_code >> 2);
			AD9081_ERROR_RETURN(err);
		}
	}
	if (rerun_cal) {
		err = adi_ad9081_dac_run_startup_cal(device, dacs);
	}
	AD9081_ERROR_RETURN(err);
	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_sysref_phase_set(adi_ad9081_device_t *device,
					uint16_t phase)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* phase of measured sysref event, in dac clock unit */
	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_PHASE0_ADDR,
				    BF_SYSREF_PHASE_INFO,
				    phase); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_sysref_sample_type_set(adi_ad9081_device_t *device,
					      uint8_t sample_type)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* 0 – sampled by reference clock then by high speed clock. 1 – sampled directly by high speed clock */
	err = adi_ad9081_hal_bf_set(device, REG_SYSREF_CTRL2_ADDR,
				    BF_SYSREF_SAMPLE_TYPE_INFO,
				    sample_type); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_nco_reset_set(adi_ad9081_device_t *device)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_DATAPATH_NCO_SYNC_CFG_ADDR,
				    BF_START_NCO_SYNC_INFO, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_DATAPATH_NCO_SYNC_CFG_ADDR,
				    BF_START_NCO_SYNC_INFO, 0);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_dac_nco_sync_reset_via_sysref_set(adi_ad9081_device_t *device,
					     uint8_t enable)
{
	return adi_ad9081_device_nco_sync_reset_via_sysref_set(device, enable);
}

int32_t adi_ad9081_dac_nco_sync_sysref_mode_set(adi_ad9081_device_t *device,
						uint8_t mode)
{
	return adi_ad9081_device_nco_sync_sysref_mode_set(device, mode);
}

int32_t adi_ad9081_dac_nco_master_slave_mode_set(adi_ad9081_device_t *device,
						 uint8_t mode)
{
	return adi_ad9081_device_nco_sync_mode_set(device, mode);
}

int32_t adi_ad9081_dac_nco_master_slave_gpio_set(adi_ad9081_device_t *device,
						 uint8_t gpio_index,
						 uint8_t output)
{
	return adi_ad9081_device_nco_sync_gpio_set(device, gpio_index, output);
}

int32_t
adi_ad9081_dac_nco_master_slave_trigger_source_set(adi_ad9081_device_t *device,
						   uint8_t source)
{
	return adi_ad9081_device_nco_sync_trigger_source_set(device, source);
}

int32_t
adi_ad9081_dac_nco_master_slave_extra_lmfc_num_set(adi_ad9081_device_t *device,
						   uint8_t num)
{
	return adi_ad9081_device_nco_sync_extra_lmfc_num_set(device, num);
}

int32_t adi_ad9081_dac_nco_master_slave_trigger_set(adi_ad9081_device_t *device)
{
	return adi_ad9081_device_nco_sync_trigger_set(device);
}

int32_t adi_ad9081_dac_nco_sync_set(adi_ad9081_device_t *device,
				    uint8_t align_source)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(align_source > 4);

	if (align_source == 0) { /* oneshot */
		/*err = adi_ad9081_jesd_oneshot_sync(device);
        AD9081_ERROR_RETURN(err);*/
	} else if (align_source == 1) { /* spi */
		err = adi_ad9081_dac_nco_reset_set(device);
		AD9081_ERROR_RETURN(err);
	} else if (align_source == 2) { /* sysref */
		err = adi_ad9081_dac_nco_sync_sysref_mode_set(device, 0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_nco_sync_reset_via_sysref_set(device, 1);
		AD9081_ERROR_RETURN(err);
	} else if (align_source == 3) { /* lmfc rising edge */
		err = adi_ad9081_dac_nco_sync_sysref_mode_set(device, 1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_nco_sync_reset_via_sysref_set(device, 1);
		AD9081_ERROR_RETURN(err);
	} else if (align_source == 4) { /* lmfc falling edge */
		err = adi_ad9081_dac_nco_sync_sysref_mode_set(device, 2);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_dac_nco_sync_reset_via_sysref_set(device, 1);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_nco_master_slave_sync(adi_ad9081_device_t *device,
					     uint8_t is_master,
					     uint8_t trigger_src,
					     uint8_t gpio_index,
					     uint8_t extra_lmfc_num)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_MAIN_AUTO_CLK_GATING_ADDR,
				    0x00000400, 7);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_dac_nco_master_slave_mode_set(device,
						       is_master > 0 ? 1 : 2);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_dac_nco_master_slave_trigger_source_set(device,
								 trigger_src);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_dac_nco_master_slave_gpio_set(device, gpio_index,
						       is_master > 0 ? 1 : 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_dac_nco_master_slave_extra_lmfc_num_set(
		device, extra_lmfc_num);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_dac_nco_sync_reset_via_sysref_set(device, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_dac_nco_sync_reset_via_sysref_set(device, 1);
	AD9081_ERROR_RETURN(err);

	if (is_master > 0) {
		err = adi_ad9081_dac_nco_master_slave_trigger_set(device);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_dac_r2r_cal_config_set(adi_ad9081_device_t *device,
					  uint8_t config, uint8_t dacs)
{
	int32_t err;
	uint8_t i = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/*Page Target Dacs*/
	err = adi_ad9081_dac_select_set(device, dacs);
	AD9081_ERROR_RETURN(err);

	/*set cal config*/
	err = adi_ad9081_hal_bf_set(device, REG_CAL_DEBUG0_ADDR, 0x00000103, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_CAL_DEBUG0_ADDR, 0x00000102, 1);
	AD9081_ERROR_RETURN(err);

	/*Set R2R Cal Config*/
	for (i = 0; i < 8; i++) {
		err = adi_ad9081_hal_bf_set(device, (0x00000111), 0x00000600,
					    AD9081_R2R_DAC_CAL_ADDR + i);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, (0x00000112), 0x00000700,
					    config);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, (REG_CAL_CTRL_ADDR),
					    0x00000103, 1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_delay_us(device, 1000);
		err = adi_ad9081_hal_bf_set(device, (REG_CAL_CTRL_ADDR),
					    0x00000103, 0);
		AD9081_ERROR_RETURN(err);
	}

	/*set cal config*/
	err = adi_ad9081_hal_bf_set(device, REG_CAL_DEBUG0_ADDR, 0x00000103, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_CAL_DEBUG0_ADDR, 0x00000102, 0);
	AD9081_ERROR_RETURN(err);
	return API_CMS_ERROR_OK;
}
int32_t adi_ad9081_dac_run_startup_cal(adi_ad9081_device_t *device,
				       uint8_t dacs)
{
	int32_t err;
	uint8_t i = 0, reg_val = 0x00, dac = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	/*Page Target Dacs*/
	err = adi_ad9081_dac_select_set(device, dacs);

	/*Setup Calibration Clocks*/
	/*Power Up CAL CLKS for target DACs*/
	err = adi_ad9081_hal_reg_get(device, REG_CLK_CTRL2_ADDR, &reg_val);
	AD9081_ERROR_RETURN(err);
	reg_val &= ~0x0F;
	reg_val |= dacs;
	err = adi_ad9081_hal_reg_set(device, REG_CLK_CTRL2_ADDR, reg_val);

	/*Set Cal Div*/
	err = adi_ad9081_hal_bf_set(device, 0x0000010E, 0x00000400, 0x8);
	AD9081_ERROR_RETURN(err);

	/*Disable Ring Osc Set CAL CLK source to be DAC CLK*/
	err = adi_ad9081_hal_bf_set(device, REG_RING_OSC_ADDR,
				    BF_RINGOSC_EN_INFO, 0);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_hal_reg_get(device, 0x0000019C, &reg_val);
	AD9081_ERROR_RETURN(err);
	reg_val &= ~0x0F;
	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			reg_val |= ~(AD9081_DAC_0 << i);
		}
	}
	err = adi_ad9081_hal_reg_set(device, 0x0000019C, reg_val);
	AD9081_ERROR_RETURN(err);

	/*Reset CAL Status*/
	err = adi_ad9081_hal_bf_set(device, REG_CAL_CTRL_ADDR,
				    BF_CAL_RESETB_INFO, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_CAL_CTRL_ADDR,
				    BF_CAL_RESETB_INFO, 1);
	AD9081_ERROR_RETURN(err);

	/*Set Calibration Config: cal settings*/
	err = adi_ad9081_hal_bf_set(device, REG_CAL_DEBUG0_ADDR, 0x00000106, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_CAL_DEBUG0_ADDR, 0x00000103, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_CAL_DEBUG0_ADDR,
				    BF_CAL_EN_G_INFO, 1);
	AD9081_ERROR_RETURN(err);
	/*CAL_MODE to GMode(0x1)*/
	err = adi_ad9081_hal_bf_set(device, REG_CAL_CTRL_ADDR, BF_CAL_MODE_INFO,
				    1);
	AD9081_ERROR_RETURN(err);

	/*Set Calibration Config: cal run settings  with ADI Recommended Settings*/
	err = adi_ad9081_hal_reg_set(device, 0x0000011D,
				     AD9081_DAC_CAL_RUN_CFG);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x00000127, 0x00000200,
				    AD9081_DAC_CAL_COMP_SPEED);
	AD9081_ERROR_RETURN(err);

	/*Set Throwaway Currents*/
	for (i = 0; i < 7; i++) {
		err = adi_ad9081_hal_reg_set(device, 0x00000128 + i,
					     AD9081_DAC_CAL_THROWAWAY_CURR_CFG);
		AD9081_ERROR_RETURN(err);
	}
	err = adi_ad9081_hal_reg_set(device, 0x00000128 + i, 0x01);
	AD9081_ERROR_RETURN(err);

	/*Trigger Cal Start*/
	err = adi_ad9081_hal_bf_set(device, REG_CAL_CTRL_ADDR,
				    BF_CAL_START_INFO, 0x1);
	AD9081_ERROR_RETURN(err);

	/*Delay*/
	err = adi_ad9081_hal_delay_us(device, 1000);
	AD9081_ERROR_RETURN(err);

	for (i = 0; i < 4; i++) {
		dac = dacs & (AD9081_DAC_0 << i);
		if (dac > 0) {
			err = adi_ad9081_dac_r2r_cal_config_set(device, 79,
								dac);
			AD9081_ERROR_RETURN(err);
		}
	}
	return API_CMS_ERROR_OK;
}
/*! @} */