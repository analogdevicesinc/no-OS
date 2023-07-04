/*!
 * @brief     APIs for ADC configuration and control
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9081_ADC_API
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"

/*============= C O D E ====================*/
int32_t adi_ad9081_adc_select_set(adi_ad9081_device_t *device, uint8_t adcs)
{
	int32_t err;
	uint8_t page_val;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_reg_get(device, REG_ADC_COARSE_PAGE_ADDR,
				     &page_val);
	AD9081_ERROR_RETURN(err);

	page_val = (adcs & 0x0f) | (page_val & 0xf0);
	err = adi_ad9081_hal_reg_set(device, REG_ADC_COARSE_PAGE_ADDR,
				     page_val);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_core_analog_regs_enable_set(adi_ad9081_device_t *device,
						   uint8_t adc_cores,
						   uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((adc_cores & 0x01) > 0) {
		err = adi_ad9081_hal_bf_set(device, REG_SPI_ENABLE_ADC_ADDR,
					    BF_SPI_EN_REG8_ADC0_INFO, enable);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_SPI_ENABLE_ADC_ADDR,
					    BF_SPI_EN_REG32_ADC0_INFO, enable);
		AD9081_ERROR_RETURN(err);
	}

	if ((adc_cores & 0x02) > 0) {
		err = adi_ad9081_hal_bf_set(device, REG_SPI_ENABLE_ADC_ADDR,
					    BF_SPI_EN_REG8_ADC1_INFO, enable);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device, REG_SPI_ENABLE_ADC_ADDR,
					    BF_SPI_EN_REG32_ADC1_INFO, enable);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_analog_input_buffer_set(adi_ad9081_device_t *device,
				       uint8_t adc_cores,
				       adi_cms_signal_coupling_e coupling)
{
	int32_t err;
	uint8_t enable_adc_core = 3, enable_cmbuf, cmin_input, cmin_out,
		cmin_out_pulldown;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (adc_cores == 1 && coupling == COUPLING_AC) {
		enable_cmbuf = 3;
		cmin_input = 14;
		cmin_out = 14;
		cmin_out_pulldown = 4;
	} else if (adc_cores == 1 && coupling == COUPLING_DC) {
		enable_cmbuf = 0;
		cmin_input = 14;
		cmin_out = 4;
		cmin_out_pulldown = 7;
	} else if (adc_cores == 2 && coupling == COUPLING_AC) {
		enable_cmbuf = 2;
		cmin_input = 0;
		cmin_out = 0;
		cmin_out_pulldown = 3;
	} else if (adc_cores == 2 && coupling == COUPLING_DC) {
		enable_cmbuf = 0;
		cmin_input = 0;
		cmin_out = 4;
		cmin_out_pulldown = 7;
	} else {
		return API_CMS_ERROR_INVALID_PARAM;
	}
	err = adi_ad9081_hal_bf_set(
		device, 0x2112, 0x100,
		1); /* CAL_FREEZE_GLOBAL freeze calibration for reconfig of common-mode loop*/
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_core_analog_regs_enable_set(
		device, adc_cores,
		enable_adc_core); /*Global enable of ADC0 and ADC1 SPI access */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x1721, 0x206,
				    enable_cmbuf); /* Power down CMBUF_PD */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(
		device, 0x1732, 0x400,
		cmin_input); /* SPI_CMIN_INPUT_SEL select common mode loop for TxFE or MxFE*/
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(
		device, 0x1733, 0X403,
		cmin_out); /* SPI_CMIN_OUT_SEL select common mode loop for TxFE or MxFE*/
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(
		device, 0x1733, 0x300,
		cmin_out_pulldown); /* SPI_CMIN_OUT_PULDWN pulls VCMx pin low when common mode buffer is disabled*/
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x2112, 0x100,
				    0); /* CAL_FREEZE_GLOBAL */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_power_up_set(adi_ad9081_device_t *device, uint8_t adcs,
				    uint8_t enable)
{
	int32_t err;
	uint8_t adc_cores = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	adc_cores |= ((adcs & AD9081_ADC_0) > 0) ? 0x1 : 0x0;
	adc_cores |= ((adcs & AD9081_ADC_1) > 0) ? 0x1 : 0x0;
	adc_cores |= ((adcs & AD9081_ADC_2) > 0) ? 0x2 : 0x0;
	adc_cores |= ((adcs & AD9081_ADC_3) > 0) ? 0x2 : 0x0;
	err = adi_ad9081_adc_core_setup(device, adc_cores);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_digital_datapath_reset_set(adi_ad9081_device_t *device,
						  uint8_t reset)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, 0x0380, 0x0100, reset);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

uint8_t
adi_ad9081_adc_ddc_coarse_dcm_decode(adi_ad9081_adc_coarse_ddc_dcm_e cddc_dcm)
{
	uint8_t cddc_dcm_value = 1;

	if (cddc_dcm == AD9081_CDDC_DCM_2)
		cddc_dcm_value = 2;
	else if (cddc_dcm == AD9081_CDDC_DCM_3)
		cddc_dcm_value = 3;
	else if (cddc_dcm == AD9081_CDDC_DCM_4)
		cddc_dcm_value = 4;
	else if (cddc_dcm == AD9081_CDDC_DCM_6)
		cddc_dcm_value = 6;
	else if (cddc_dcm == AD9081_CDDC_DCM_8)
		cddc_dcm_value = 8;
	else if (cddc_dcm == AD9081_CDDC_DCM_9)
		cddc_dcm_value = 9;
	else if (cddc_dcm == AD9081_CDDC_DCM_12)
		cddc_dcm_value = 12;
	else if (cddc_dcm == AD9081_CDDC_DCM_16)
		cddc_dcm_value = 16;
	else if (cddc_dcm == AD9081_CDDC_DCM_18)
		cddc_dcm_value = 18;
	else if (cddc_dcm == AD9081_CDDC_DCM_24)
		cddc_dcm_value = 24;
	else if (cddc_dcm == AD9081_CDDC_DCM_36)
		cddc_dcm_value = 36;

	return cddc_dcm_value;
}

uint8_t
adi_ad9081_adc_ddc_fine_dcm_decode(adi_ad9081_adc_fine_ddc_dcm_e fddc_dcm)
{
	uint8_t fddc_dcm_value = 1;

	if (fddc_dcm == AD9081_FDDC_DCM_2)
		fddc_dcm_value = 2;
	else if (fddc_dcm == AD9081_FDDC_DCM_3)
		fddc_dcm_value = 3;
	else if (fddc_dcm == AD9081_FDDC_DCM_4)
		fddc_dcm_value = 4;
	else if (fddc_dcm == AD9081_FDDC_DCM_6)
		fddc_dcm_value = 6;
	else if (fddc_dcm == AD9081_FDDC_DCM_8)
		fddc_dcm_value = 8;
	else if (fddc_dcm == AD9081_FDDC_DCM_12)
		fddc_dcm_value = 12;
	else if (fddc_dcm == AD9081_FDDC_DCM_16)
		fddc_dcm_value = 16;
	else if (fddc_dcm == AD9081_FDDC_DCM_24)
		fddc_dcm_value = 24;

	return fddc_dcm_value;
}

int32_t adi_ad9081_adc_ddc_coarse_select_set(adi_ad9081_device_t *device,
					     uint8_t cddcs)
{
	int32_t err;
	uint8_t page_val;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(cddcs > AD9081_ADC_CDDC_ALL);

	err = adi_ad9081_hal_reg_get(device, REG_ADC_COARSE_PAGE_ADDR,
				     &page_val);
	AD9081_ERROR_RETURN(err);

	page_val = (cddcs << 4) | (page_val & 0x0f);
	err = adi_ad9081_hal_reg_set(device, REG_ADC_COARSE_PAGE_ADDR,
				     page_val);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_select_set(adi_ad9081_device_t *device,
					   uint8_t fddcs)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(fddcs > AD9081_ADC_FDDC_ALL);

	err = adi_ad9081_hal_reg_set(device, REG_FINE_DDC_PAGE_ADDR, fddcs);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_clk_freq_get(adi_ad9081_device_t *device,
				    uint64_t *adc_clk_hz)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_NULL_POINTER_RETURN(adc_clk_hz);

	*adc_clk_hz = device->dev_info.adc_freq_hz;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_clk_out_enable_set(adi_ad9081_device_t *device,
					  uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_CLK_CTRL1_ADDR,
				    BF_PD_ADC_DRIVER_INFO, !enable);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_clk_out_voltage_swing_set(adi_ad9081_device_t *device,
						 int16_t swing_mv)
{
	int32_t err;
	uint8_t code;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (swing_mv > 1000 || swing_mv < -1000) {
		return API_CMS_ERROR_INVALID_PARAM;
	}

	code = (993 - swing_mv + (99 / 2)) / 99;

	/* Voltage Swing = 993mV - (code * 99mV).
	0 -> 993mV;		1 -> 894mV;		2 -> 795mV;		3 -> 696mV;		4 -> 597mV;		5 -> 498mV;		6 -> 399mV;
	7 -> 300mV;		8 -> 201mV;		9 -> 102mV;		10 -> 3mV;		11 -> -96mV;	12 -> -195mV;	13 -> -294mV;
	14 -> -393mV;	15 -> -492mV;	16 -> -591mV;	17 -> -690mV;	18 -> -789mV;	19 -> -888mV;	20 -> -987mV;
	*/
	err = adi_ad9081_hal_bf_set(device, REG_ADC_CLK_CTRL0_ADDR,
				    BF_ADC_DRIVER_DATA_CTRL_INFO, code);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_clk_enable_set(adi_ad9081_device_t *device,
				      uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_ADC_DIVIDER_CTRL_ADDR,
				    BF_ADCDIVN_PD_INFO, !enable);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_clk_div_set(adi_ad9081_device_t *device, uint8_t div)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_ADC_DIVIDER_CTRL_ADDR,
				    BF_ADCDIVN_DIVRATIO_SPI_INFO, div - 1);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_ffh_sel_to_gpio_mapping_set(adi_ad9081_device_t *device,
					       uint8_t ffh[6])
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* ffh: 2 - gpio6, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n */
	err = adi_ad9081_hal_bf_set(device, 0x37cc, 0x0800,
				    ffh[2]); /* PERI_I_SEL12 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x37cd, 0x0800,
				    ffh[3]); /* PERI_I_SEL13 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x37ce, 0x0800,
				    ffh[4]); /* PERI_I_SEL14 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x37cf, 0x0800,
				    ffh[5]); /* PERI_I_SEL15 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x37d0, 0x0800,
				    ffh[1]); /* PERI_I_SEL16 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x37d1, 0x0800,
				    ffh[0]); /* PERI_I_SEL17 */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fd_en_to_gpio_mapping_set(adi_ad9081_device_t *device,
						 uint8_t fd_en_gpio)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* 2 - gpio6, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n */
	err = adi_ad9081_hal_bf_set(device, 0x37d4, 0x0800,
				    fd_en_gpio); /* PERI_I_SEL20 */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fdelay_cdelay_pfir_sel_to_gpio_mapping_set(
	adi_ad9081_device_t *device, uint8_t sel0, uint8_t sel1)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* select shift value or coefficient bank */
	/* 2 - gpio6, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n */
	err = adi_ad9081_hal_bf_set(device, 0x37d5, 0x0800,
				    sel0); /* PERI_I_SEL21 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x37d6, 0x0800,
				    sel1); /* PERI_I_SEL22 */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_rx_en_to_gpio_mapping_set(adi_ad9081_device_t *device,
						 uint8_t rxen1_gpio,
						 uint8_t rxen3_gpio)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* 2 - gpio6, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n */
	err = adi_ad9081_hal_bf_set(device, 0x37d7, 0x0800,
				    rxen1_gpio); /* PERI_I_SEL23 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x37d8, 0x0800,
				    rxen3_gpio); /* PERI_I_SEL24 */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_en_to_gpio_mapping_set(adi_ad9081_device_t *device,
						   uint8_t smon_en_gpio)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* 0 - disable, 1 - enable, 2 - gpio6 to enable, 3 - gpio7, 4 - gpio8, 5 - gpio9, 6 - gpio10, 7 - syncinb1_p, 8 - syncinb1_n */
	err = adi_ad9081_hal_bf_set(device, 0x37da, 0x0800,
				    smon_en_gpio); /* PERI_I_SEL26 */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_adc0_rxen_pwdn_ctrl_set(adi_ad9081_device_t *device,
					       uint8_t rxen0_0f_ctrl_en,
					       uint8_t rxengp0_0f_ctrl_en,
					       uint8_t rxen0_0s_ctrl_en,
					       uint8_t rxengp0_0s_ctrl_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(rxen0_0f_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxengp0_0f_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxen0_0s_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxengp0_0s_ctrl_en > 1);

	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL1_ADDR,
				    BF_RXEN0_0F_CTRL_INFO, rxen0_0f_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL1_ADDR,
				    BF_RXENGP0_0F_CTRL_INFO,
				    rxengp0_0f_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL1_ADDR,
				    BF_RXEN0_0S_CTRL_INFO, rxen0_0s_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL1_ADDR,
				    BF_RXENGP0_0S_CTRL_INFO,
				    rxengp0_0s_ctrl_en);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_adc1_rxen_pwdn_ctrl_set(adi_ad9081_device_t *device,
					       uint8_t rxen1_1f_ctrl_en,
					       uint8_t rxengp1_1f_ctrl_en,
					       uint8_t rxen1_1s_ctrl_en,
					       uint8_t rxengp1_1s_ctrl_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(rxen1_1f_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxengp1_1f_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxen1_1s_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxengp1_1s_ctrl_en > 1);

	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL1_ADDR,
				    BF_RXEN1_1F_CTRL_INFO, rxen1_1f_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL1_ADDR,
				    BF_RXENGP1_1F_CTRL_INFO,
				    rxengp1_1f_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL1_ADDR,
				    BF_RXEN1_1S_CTRL_INFO, rxen1_1s_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL1_ADDR,
				    BF_RXENGP1_1S_CTRL_INFO,
				    rxengp1_1s_ctrl_en);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_adc2_rxen_pwdn_ctrl_set(adi_ad9081_device_t *device,
					       uint8_t rxen0_2f_ctrl_en,
					       uint8_t rxengp0_2f_ctrl_en,
					       uint8_t rxen0_2s_ctrl_en,
					       uint8_t rxengp0_2s_ctrl_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(rxen0_2f_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxengp0_2f_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxen0_2s_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxengp0_2s_ctrl_en > 1);

	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL2_ADDR,
				    BF_RXEN0_2F_CTRL_INFO, rxen0_2f_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL2_ADDR,
				    BF_RXENGP0_2F_CTRL_INFO,
				    rxengp0_2f_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL2_ADDR,
				    BF_RXEN0_2S_CTRL_INFO, rxen0_2s_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL2_ADDR,
				    BF_RXENGP0_2S_CTRL_INFO,
				    rxengp0_2s_ctrl_en);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_adc3_rxen_pwdn_ctrl_set(adi_ad9081_device_t *device,
					       uint8_t rxen1_3f_ctrl_en,
					       uint8_t rxengp1_3f_ctrl_en,
					       uint8_t rxen1_3s_ctrl_en,
					       uint8_t rxengp1_3s_ctrl_en)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(rxen1_3f_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxengp1_3f_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxen1_3s_ctrl_en > 1);
	AD9081_INVALID_PARAM_RETURN(rxengp1_3s_ctrl_en > 1);

	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL2_ADDR,
				    BF_RXEN1_3F_CTRL_INFO, rxen1_3f_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL2_ADDR,
				    BF_RXENGP1_3F_CTRL_INFO,
				    rxengp1_3f_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL2_ADDR,
				    BF_RXEN1_3S_CTRL_INFO, rxen1_3s_ctrl_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_NOVALP_CTRL2_ADDR,
				    BF_RXENGP1_3S_CTRL_INFO,
				    rxengp1_3s_ctrl_en);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_rxengp0_ctrl_set(adi_ad9081_device_t *device,
					uint8_t spi_en, uint8_t pol,
					uint8_t rxen)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_RXEN_CTRL_ADDR,
				    BF_RXENGP0_POL_INFO, pol);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_SPI_CTRL_ADDR,
				    BF_RXENGP0_SPIEN_INFO, spi_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_SPI_CTRL_ADDR,
				    BF_RXENGP0_SPI_INFO, rxen);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_rxengp1_ctrl_set(adi_ad9081_device_t *device,
					uint8_t spi_en, uint8_t pol,
					uint8_t rxen)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_RXEN_CTRL_ADDR,
				    BF_RXENGP1_POL_INFO, pol);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_SPI_CTRL_ADDR,
				    BF_RXENGP1_SPIEN_INFO, spi_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_SPI_CTRL_ADDR,
				    BF_RXENGP1_SPI_INFO, rxen);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_rxengp0_sel_set(adi_ad9081_device_t *device,
				       uint8_t cddcs, uint8_t fddcs,
				       uint8_t adcs, uint8_t jtx,
				       uint8_t jtx_phy)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_RXENGP0_SEL1_ADDR,
				    BF_RXENGP0_CDDC_SEL_INFO, cddcs);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXENGP0_SEL0_ADDR,
				    BF_RXENGP0_FDDC_SEL_INFO, fddcs);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXENGP0_SEL1_ADDR,
				    BF_RXENGP0_ADC_SEL_INFO,
				    adcs); /* adc core */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXENGP0_SEL1_ADDR,
				    BF_RXENGP0_JTXL_SEL_INFO,
				    jtx); /* bit0: link0, bit1: link1 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXENGP0_SEL2_ADDR,
				    BF_RXENGP0_JTXPHY_SEL_INFO,
				    jtx_phy); /* bit0: lane0, ... */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_rxengp1_sel_set(adi_ad9081_device_t *device,
				       uint8_t cddcs, uint8_t fddcs,
				       uint8_t adcs, uint8_t jtx,
				       uint8_t jtx_phy)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_RXENGP1_SEL1_ADDR,
				    BF_RXENGP1_CDDC_SEL_INFO, cddcs);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXENGP1_SEL0_ADDR,
				    BF_RXENGP1_FDDC_SEL_INFO, fddcs);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXENGP1_SEL1_ADDR,
				    BF_RXENGP1_ADC_SEL_INFO,
				    adcs); /* adc core */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXENGP1_SEL1_ADDR,
				    BF_RXENGP1_JTXL_SEL_INFO,
				    jtx); /* bit0: link0, bit1: link1 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXENGP1_SEL2_ADDR,
				    BF_RXENGP1_JTXPHY_SEL_INFO,
				    jtx_phy); /* bit0: lane0, ... */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_rxen0_ctrl_set(adi_ad9081_device_t *device,
				      uint8_t use_txen, uint8_t spi_en,
				      uint8_t rxen0_pol, uint8_t rxen0)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_RXEN_CTRL_ADDR,
				    BF_RXEN0_USETXEN_INFO, use_txen);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_CTRL_ADDR,
				    BF_RXEN0_POL_INFO, rxen0_pol);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_SPI_CTRL_ADDR,
				    BF_RXEN0_SPIEN_INFO, spi_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_SPI_CTRL_ADDR,
				    BF_RXEN0_SPI_INFO, rxen0);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_rxen1_ctrl_set(adi_ad9081_device_t *device,
				      uint8_t use_txen, uint8_t spi_en,
				      uint8_t rxen1_pol, uint8_t rxen1)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_RXEN_CTRL_ADDR,
				    BF_RXEN1_USETXEN_INFO, use_txen);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_CTRL_ADDR,
				    BF_RXEN1_POL_INFO, rxen1_pol);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_SPI_CTRL_ADDR,
				    BF_RXEN1_SPIEN_INFO, spi_en);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN_SPI_CTRL_ADDR,
				    BF_RXEN1_SPI_INFO, rxen1);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_rxen0_sel_set(adi_ad9081_device_t *device, uint8_t cddcs,
				     uint8_t fddcs, uint8_t adcs, uint8_t jtx,
				     uint8_t jtx_phy)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_RXEN0_SEL1_ADDR,
				    BF_RXEN0_CDDC_SEL_INFO, cddcs);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN0_SEL0_ADDR,
				    BF_RXEN0_FDDC_SEL_INFO, fddcs);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN0_SEL1_ADDR,
				    BF_RXEN0_ADC_SEL_INFO, adcs); /* adc core */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN0_SEL1_ADDR,
				    BF_RXEN0_JTXL_SEL_INFO,
				    jtx); /* bit0: link0, bit1: link1 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN0_SEL2_ADDR,
				    BF_RXEN0_JTXPHY_SEL_INFO,
				    jtx_phy); /* bit0: lane0, ... */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_rxen1_sel_set(adi_ad9081_device_t *device, uint8_t cddcs,
				     uint8_t fddcs, uint8_t adcs, uint8_t jtx,
				     uint8_t jtx_phy)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_RXEN1_SEL1_ADDR,
				    BF_RXEN1_CDDC_SEL_INFO, cddcs);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN1_SEL0_ADDR,
				    BF_RXEN1_FDDC_SEL_INFO, fddcs);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN1_SEL1_ADDR,
				    BF_RXEN1_ADC_SEL_INFO, adcs); /* adc core */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN1_SEL1_ADDR,
				    BF_RXEN1_JTXL_SEL_INFO,
				    jtx); /* bit0: link0, bit1: link1 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_RXEN1_SEL2_ADDR,
				    BF_RXEN1_JTXPHY_SEL_INFO,
				    jtx_phy); /* bit0: lane0, ... */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_adc2cddc_xbar_set(adi_ad9081_device_t *device,
					 uint8_t adc_cddc_xbar)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_ADC_COARSE_CB_ADDR,
				    BF_ADC_COARSE_CB_INFO, adc_cddc_xbar);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_cddc2fddc_xbar_set(adi_ad9081_device_t *device,
					  uint8_t cddc_fddc_xbar)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_COARSE_FINE_CB_ADDR,
				    BF_COARSE_FINE_CB_INFO, cddc_fddc_xbar);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_xbar_set(adi_ad9081_device_t *device,
				uint8_t adc_cddc_xbar, uint8_t cddc_fddc_xbar)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_adc2cddc_xbar_set(device, adc_cddc_xbar);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_cddc2fddc_xbar_set(device, cddc_fddc_xbar);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_xbar_get(adi_ad9081_device_t *device,
				uint8_t *adc_cddc_xbar, uint8_t *cddc_fddc_xbar)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(adc_cddc_xbar);
	AD9081_NULL_POINTER_RETURN(cddc_fddc_xbar);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_get(device, REG_ADC_COARSE_CB_ADDR,
				    BF_ADC_COARSE_CB_INFO, adc_cddc_xbar, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, REG_COARSE_FINE_CB_ADDR,
				    BF_COARSE_FINE_CB_INFO, cddc_fddc_xbar, 1);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_xbar_find_cddc(adi_ad9081_device_t *device, uint8_t fddc,
				      uint8_t *cddc)
{
	int32_t err;
	uint8_t i, j, cddc_fddc_xbar;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_get(device, REG_COARSE_FINE_CB_ADDR,
				    BF_COARSE_FINE_CB_INFO, &cddc_fddc_xbar, 1);
	AD9081_ERROR_RETURN(err);

	j = 0;
	for (i = 0; i < 8; i++) {
		if ((fddc & (AD9081_ADC_FDDC_0 << i)) > 0)
			j = (i < 4) ?
				    ((cddc_fddc_xbar & (1 << i)) > 0 ? 1 : 0) :
				    ((cddc_fddc_xbar & (1 << i)) > 0 ? 3 : 2);
	}
	*cddc = AD9081_ADC_CDDC_0 << j;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_chip_dcm_ratio_set(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t dcm)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device,
					    REG_CHIP_DECIMATION_RATIO_ADDR,
					    BF_CHIP_DECIMATION_RATIO_INFO, dcm);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_set(device,
					    REG_CHIP_DECIMATION_RATIO_ADDR,
					    BF_CHIP_DECIMATION_RATIO_INFO, dcm);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_chip_dcm_ratio_get(adi_ad9081_device_t *device,
					  adi_ad9081_jesd_link_select_e links,
					  uint8_t *dcm)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if ((links & AD9081_LINK_0) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_0);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device,
					    REG_CHIP_DECIMATION_RATIO_ADDR,
					    BF_CHIP_DECIMATION_RATIO_INFO, dcm,
					    1);
		AD9081_ERROR_RETURN(err);
	}
	if ((links & AD9081_LINK_1) > 0) {
		err = adi_ad9081_jesd_tx_link_select_set(device, AD9081_LINK_1);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device,
					    REG_CHIP_DECIMATION_RATIO_ADDR,
					    BF_CHIP_DECIMATION_RATIO_INFO, dcm,
					    1);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_common_hop_en_set(adi_ad9081_device_t *device,
					 uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_COMMON_HOP_EN_ADDR,
				    BF_COMMON_HOP_EN_INFO, enable);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_trig_nco_reset_enable_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t enable)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_SYNC_CTRL_ADDR,
				BF_COARSE_DDC_TRIG_NCO_RESET_EN_INFO, enable);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_reset_set(adi_ad9081_device_t *device,
					    uint8_t cddcs)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_SYNC_CTRL_ADDR,
				BF_COARSE_DDC_SOFT_RESET_INFO, 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_delay_us(device, 100000);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_SYNC_CTRL_ADDR,
				BF_COARSE_DDC_SOFT_RESET_INFO, 0);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_sync_next_set(adi_ad9081_device_t *device,
						uint8_t cddcs, uint8_t val)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_SYNC_CTRL_ADDR,
				BF_COARSE_DDC_SYNC_NEXT_INFO, val);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_sync_enable_set(adi_ad9081_device_t *device,
						  uint8_t cddcs, uint8_t enable)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_SYNC_CTRL_ADDR,
				BF_COARSE_DDC_SYNC_EN_INFO, enable);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_dcm_set(adi_ad9081_device_t *device,
					  uint8_t cddcs,
					  adi_ad9081_adc_coarse_ddc_dcm_e dcm)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_COARSE_DEC_CTRL_ADDR,
						    BF_COARSE_DEC_SEL_INFO,
						    dcm);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_c2r_set(adi_ad9081_device_t *device,
					  uint8_t cddcs, uint8_t c2r_en)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_COARSE_DEC_CTRL_ADDR,
						    BF_COARSE_C2R_EN_INFO,
						    c2r_en);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_gain_set(adi_ad9081_device_t *device,
					   uint8_t cddcs, uint8_t gain)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(gain > 1);

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_COARSE_DEC_CTRL_ADDR,
						    BF_COARSE_GAIN_INFO, gain);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_coarse_nco_mode_set(adi_ad9081_device_t *device,
				       uint8_t cddcs,
				       adi_ad9081_adc_nco_mode_e nco_mode)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(nco_mode >= AD9081_ADC_NCO_INVALID);

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_COARSE_DEC_CTRL_ADDR,
						    BF_COARSE_MXR_IF_INFO,
						    nco_mode);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_nco_enable_set(adi_ad9081_device_t *device,
						 uint8_t cddcs)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_COARSE_DDC_EN_ADDR,
				    BF_COARSE_DDC_EN_INFO, cddcs);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_nco_channel_selection_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t channel)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			/* 0 ~ 15 */
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_NCO_CTRL_ADDR,
				BF_COARSE_DDC0_NCO_REGMAP_CHAN_SEL_INFO,
				channel);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_nco_channel_select_via_gpio_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t mode)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			/* 0:  Register Map control (Use ddc_nco_regmap_chan_sel)
               1:  profile_pins[0]     is used. Pin level control {3'b0, profile_pins[0]}
               2:  profile_pins[1 :0] are used. Pin level control {2'b0, profile_pins[1:0]}
               3:  profile_pins[2 :0] are used. Pin level control {1'b0, profile_pins[2:0]}
               4:  profile_pins[3 :0] are used. Pin level control { profile_pins[3:0]}
               8:  profile_pins[0] Pin edge control- increment internal counter when rising edge of profile_pins[0] Pin.
               9:  profile_pins[1] Pin edge control- increment internal counter when rising edge of profile_pins[1] Pin.
               10: profile_pins[2] Pin edge control- increment internal counter when rising edge of profile_pins[2] Pin.
               11: profile_pins[3] Pin edge control- increment internal counter when rising edge of profile_pins[3] Pin.
               12: FHT expire based control - increment internal counter when FHT is expired.
             */
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_NCO_CTRL_ADDR,
				BF_COARSE_DDC0_NCO_CHAN_SEL_MODE_INFO, mode);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_nco_channel_update_index_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t channel)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_PROFILE_CTRL_ADDR,
				BF_COARSE_DDC0_PROFILE_UPDATE_INDEX_INFO,
				channel);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_nco_channel_update_mode_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t mode)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			/* 0: Instantaneous/Continuous Update. Phase increment and phase offset values are updated immediately.
               1: Phase increment and phase offset values are updated synchronously either with the chip_transfer
                  bit is set high or based on the GPIO (pin ddc_chip_gpio_transfer at nova_dig_dp_top hierarchy)
                  pin low to high transition. The chip transfer bit will be cleared once the transfer is complete.
             */
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_PROFILE_CTRL_ADDR,
				BF_COARSE_DDC0_PROFILE_UPDATE_MODE_INFO, mode);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_coarse_gpio_chip_xfer_mode_set(adi_ad9081_device_t *device,
						  uint8_t cddcs, uint8_t mode)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			/* used when ddc0_phase_update_mode is '1'
               0: Phase increment and phase offset values are updated synchronously
                  when the chip_transfer bit is set high. The chip transfer bit will
                  be cleared once the transfer is complete.
               1: Phase increment and phase offset values are updated based on the
                  GPIO (pin ddc_chip_gpio_transfer at nova_dig_dp_top hierarchy) pin
                  low to high transition.
             */
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_PROFILE_CTRL_ADDR,
				BF_COARSE_DDC0_GPIO_CHIP_TRANSFER_MODE_INFO,
				mode);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_trig_hop_en_set(adi_ad9081_device_t *device,
						  uint8_t cddcs, uint8_t enable)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			/* 0: Frequency hopping is independent of trig signal. 1: Trig signal us used for frequency hoping. */
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_TRIG_CTRL_ADDR,
				BF_COARSE_DDC_TRIG_HOP_EN_INFO, enable);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_nco_set(adi_ad9081_device_t *device,
					  uint8_t cddcs, int64_t cddc_shift_hz)
{
	int32_t err;
	uint64_t ftw;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_calc_rx_nco_ftw(
		device, device->dev_info.adc_freq_hz, cddc_shift_hz, &ftw);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_coarse_nco_ftw_set(device, cddcs, ftw, 0, 0);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

#if AD9081_USE_FLOATING_TYPE > 0
int32_t adi_ad9081_adc_ddc_coarse_nco_set_f(adi_ad9081_device_t *device,
					    uint8_t cddcs, double cddc_shift_hz)
{
	int32_t err;
	uint64_t ftw, a, b;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_calc_nco_ftw_f(
		device, (double)device->dev_info.adc_freq_hz, cddc_shift_hz,
		&ftw, &a, &b);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_coarse_nco_ftw_set(device, cddcs, ftw, a, b);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}
#endif

int32_t adi_ad9081_adc_ddc_coarse_nco_ftw_set(adi_ad9081_device_t *device,
					      uint8_t cddcs, uint64_t ftw,
					      uint64_t modulus_a,
					      uint64_t modulus_b)
{
	int32_t err;
#if AD9081_USE_SPI_BURST_MODE > 0
	uint8_t in_data[8];
#endif
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN((ftw >> 48) > 0);
	AD9081_INVALID_PARAM_RETURN((modulus_a >> 48) > 0);
	AD9081_INVALID_PARAM_RETURN((modulus_b >> 48) > 0);

	err = adi_ad9081_adc_ddc_coarse_select_set(device, cddcs);
	AD9081_ERROR_RETURN(err);
#if AD9081_USE_SPI_BURST_MODE > 0
	in_data[0] = (REG_COARSE_DDC_PHASE_INC0_ADDR >> 8) & 0x3F;
	in_data[1] = (REG_COARSE_DDC_PHASE_INC0_ADDR >> 0) & 0xFF;
	in_data[2] = (uint8_t)((ftw >> 0) & 0xFF);
	in_data[3] = (uint8_t)((ftw >> 8) & 0xFF);
	in_data[4] = (uint8_t)((ftw >> 16) & 0xFF);
	in_data[5] = (uint8_t)((ftw >> 24) & 0xFF);
	in_data[6] = (uint8_t)((ftw >> 32) & 0xFF);
	in_data[7] = (uint8_t)((ftw >> 40) & 0xFF);
	if (API_CMS_ERROR_OK !=
	    device->hal_info.spi_xfer(device->hal_info.user_data, in_data, NULL,
				      0x8))
		return API_CMS_ERROR_SPI_XFER;
	in_data[0] = (REG_COARSE_DDC_PHASE_INC_FRAC_A0_ADDR >> 8) & 0x3F;
	in_data[1] = (REG_COARSE_DDC_PHASE_INC_FRAC_A0_ADDR >> 0) & 0xFF;
	in_data[2] = (uint8_t)((modulus_a >> 0) & 0xFF);
	in_data[3] = (uint8_t)((modulus_a >> 8) & 0xFF);
	in_data[4] = (uint8_t)((modulus_a >> 16) & 0xFF);
	in_data[5] = (uint8_t)((modulus_a >> 24) & 0xFF);
	in_data[6] = (uint8_t)((modulus_a >> 32) & 0xFF);
	in_data[7] = (uint8_t)((modulus_a >> 40) & 0xFF);
	if (API_CMS_ERROR_OK !=
	    device->hal_info.spi_xfer(device->hal_info.user_data, in_data, NULL,
				      0x8))
		return API_CMS_ERROR_SPI_XFER;
	in_data[0] = (REG_COARSE_DDC_PHASE_INC_FRAC_B0_ADDR >> 8) & 0x3F;
	in_data[1] = (REG_COARSE_DDC_PHASE_INC_FRAC_B0_ADDR >> 0) & 0xFF;
	in_data[2] = (uint8_t)((modulus_b >> 0) & 0xFF);
	in_data[3] = (uint8_t)((modulus_b >> 8) & 0xFF);
	in_data[4] = (uint8_t)((modulus_b >> 16) & 0xFF);
	in_data[5] = (uint8_t)((modulus_b >> 24) & 0xFF);
	in_data[6] = (uint8_t)((modulus_b >> 32) & 0xFF);
	in_data[7] = (uint8_t)((modulus_b >> 40) & 0xFF);
	if (API_CMS_ERROR_OK !=
	    device->hal_info.spi_xfer(device->hal_info.user_data, in_data, NULL,
				      0x8))
		return API_CMS_ERROR_SPI_XFER;
#else
	err = adi_ad9081_hal_bf_set(device, REG_COARSE_DDC_PHASE_INC0_ADDR,
				    0x3000, ftw);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device,
				    REG_COARSE_DDC_PHASE_INC_FRAC_A0_ADDR,
				    0x3000, modulus_a);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device,
				    REG_COARSE_DDC_PHASE_INC_FRAC_B0_ADDR,
				    0x3000, modulus_b);
	AD9081_ERROR_RETURN(err);
#endif

	if ((cddcs & AD9081_ADC_CDDC_0) > 0) {
		err = adi_ad9081_adc_ddc_coarse_nco_phase_offset_set(
			device, AD9081_ADC_CDDC_0, ftw << 3);
		AD9081_ERROR_RETURN(err);
	}
	if ((cddcs & AD9081_ADC_CDDC_1) > 0) {
		err = adi_ad9081_adc_ddc_coarse_nco_phase_offset_set(
			device, AD9081_ADC_CDDC_1, ftw << 3);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_nco_ftw_get(adi_ad9081_device_t *device,
					      uint8_t cddc, uint64_t *ftw,
					      uint64_t *modulus_a,
					      uint64_t *modulus_b)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(ftw);
	AD9081_NULL_POINTER_RETURN(modulus_a);
	AD9081_NULL_POINTER_RETURN(modulus_b);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_ddc_coarse_select_set(device, cddc);
	AD9081_ERROR_RETURN(err);
	*ftw = 0;
	err = adi_ad9081_hal_bf_get(device,
				    REG_COARSE_DDC_ACTIVE_PHASE_INC0_ADDR,
				    0x3000, (uint8_t *)ftw, 8);
	AD9081_ERROR_RETURN(err);
	*modulus_a = 0;
	err = adi_ad9081_hal_bf_get(device,
				    REG_COARSE_DDC_PHASE_INC_FRAC_A0_ADDR,
				    0x3000, (uint8_t *)modulus_a, 8);
	AD9081_ERROR_RETURN(err);
	*modulus_b = 0;
	err = adi_ad9081_hal_bf_get(device,
				    REG_COARSE_DDC_PHASE_INC_FRAC_B0_ADDR,
				    0x3000, (uint8_t *)modulus_b, 8);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_coarse_nco_phase_offset_set(adi_ad9081_device_t *device,
					       uint8_t cddcs, uint64_t offset)
{
	int32_t err;
#if AD9081_USE_SPI_BURST_MODE > 0
	uint8_t in_data[8];
#endif
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_ddc_coarse_select_set(device, cddcs);
	AD9081_ERROR_RETURN(err);
#if AD9081_USE_SPI_BURST_MODE > 0
	in_data[0] = (REG_COARSE_DDC_PHASE_OFFSET0_ADDR >> 8) & 0x3F;
	in_data[1] = (REG_COARSE_DDC_PHASE_OFFSET0_ADDR >> 0) & 0xFF;
	in_data[2] = (uint8_t)((offset >> 0) & 0xFF);
	in_data[3] = (uint8_t)((offset >> 8) & 0xFF);
	in_data[4] = (uint8_t)((offset >> 16) & 0xFF);
	in_data[5] = (uint8_t)((offset >> 24) & 0xFF);
	in_data[6] = (uint8_t)((offset >> 32) & 0xFF);
	in_data[7] = (uint8_t)((offset >> 40) & 0xFF);
	if (API_CMS_ERROR_OK !=
	    device->hal_info.spi_xfer(device->hal_info.user_data, in_data, NULL,
				      0x8))
		return API_CMS_ERROR_SPI_XFER;
#else
	err = adi_ad9081_hal_bf_set(device, REG_COARSE_DDC_PHASE_OFFSET0_ADDR,
				    0x3000, offset);
	AD9081_ERROR_RETURN(err);
#endif

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_dither_en_set(adi_ad9081_device_t *device,
						uint8_t cddcs,
						uint8_t amp_dither_en,
						uint8_t phase_dither_en)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_DITHER_ADDR,
				BF_COARSE_DDC0_AMP_DITHER_EN_INFO,
				amp_dither_en);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_DITHER_ADDR,
				BF_COARSE_DDC0_PHASE_DITHER_EN_INFO,
				phase_dither_en);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_chip_xfer_set(adi_ad9081_device_t *device,
						uint8_t cddcs)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			/* 1: Used to synchronize the transfer of data from master to slave registers.
               0: Do nothing.
               Note: This bit is used to update the DDC Phase Increment and Phase Offset
               registers when ddc0_phase_update_mode = 1 and ddc0_gpio_chip_transfer_mode = 0
             */
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_TRANSFER_CTRL_ADDR,
				BF_COARSE_DDC0_CHIP_TRANSFER_INFO, 1);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_coarse_chip_xfer_status_get(adi_ad9081_device_t *device,
					       uint8_t cddcs, uint8_t *status)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			/* 1: Transfer of data from master to slave registers is complete.
               0: Indicates the data transfer is not requested or not completed.
             */
			err = adi_ad9081_hal_bf_get(
				device, REG_COARSE_DDC_TRANSFER_STATUS_ADDR,
				BF_COARSE_DDC0_CHIP_TRANSFER_STATUS_INFO,
				status, 1);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_coarse_psw_set(adi_ad9081_device_t *device,
					  uint8_t cddcs, uint64_t psw)
{
	int32_t err;
	uint8_t i, cddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
		if (cddc > 0) {
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_COARSE_DDC_PSW_0_ADDR, 0x2800,
				psw); /* Profile Select Word */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_fine_trig_nco_reset_enable_set(adi_ad9081_device_t *device,
						  uint8_t fddcs, uint8_t enable)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_SYNC_CTRL_ADDR,
				BF_FINE_DDC_TRIG_NCO_RESET_EN_INFO, enable);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_reset_set(adi_ad9081_device_t *device,
					  uint8_t fddcs)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_FINE_DDC_SYNC_CTRL_ADDR,
						    BF_FINE_DDC_SOFT_RESET_INFO,
						    1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_delay_us(device, 100000);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_FINE_DDC_SYNC_CTRL_ADDR,
						    BF_FINE_DDC_SOFT_RESET_INFO,
						    0);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_sync_next_set(adi_ad9081_device_t *device,
					      uint8_t fddcs, uint8_t val)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_FINE_DDC_SYNC_CTRL_ADDR,
						    BF_FINE_DDC_SYNC_NEXT_INFO,
						    val);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_sync_enable_set(adi_ad9081_device_t *device,
						uint8_t fddcs, uint8_t enable)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_FINE_DDC_SYNC_CTRL_ADDR,
						    BF_FINE_DDC_SYNC_EN_INFO,
						    enable);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_dcm_set(adi_ad9081_device_t *device,
					uint8_t fddcs,
					adi_ad9081_adc_fine_ddc_dcm_e dcm)
{
	int32_t err;
	uint8_t bypass;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device,
						    REG_FINE_BYPASS_ADDR,
						    BF_FINE_BYPASS_INFO,
						    &bypass, 1);
			AD9081_ERROR_RETURN(err);
			if (dcm == AD9081_FDDC_DCM_1) { /* 1x */
				bypass |= fddc;
			} else {
				bypass &= ~fddc;
				err = adi_ad9081_hal_bf_set(
					device, REG_FINE_DEC_CTRL_ADDR,
					BF_FINE_DEC_SEL_INFO, dcm);
				AD9081_ERROR_RETURN(err);
			}
			err = adi_ad9081_hal_bf_set(device,
						    REG_FINE_BYPASS_ADDR,
						    BF_FINE_BYPASS_INFO,
						    bypass);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_c2r_set(adi_ad9081_device_t *device,
					uint8_t fddcs, uint8_t c2r_en)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(c2r_en > 1);

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_FINE_DEC_CTRL_ADDR,
						    BF_FINE_C2R_EN_INFO,
						    c2r_en);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_gain_set(adi_ad9081_device_t *device,
					 uint8_t fddcs, uint8_t gain)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(gain > 1);

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_FINE_DEC_CTRL_ADDR,
						    BF_FINE_GAIN_INFO, gain);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_nco_mode_set(adi_ad9081_device_t *device,
					     uint8_t fddcs,
					     adi_ad9081_adc_nco_mode_e nco_mode)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(nco_mode >= AD9081_ADC_NCO_INVALID);

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_FINE_DEC_CTRL_ADDR,
						    BF_FINE_MXR_IF_INFO,
						    nco_mode);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_nco_enable_set(adi_ad9081_device_t *device,
					       uint8_t fddcs)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_FINE_DDC_EN_ADDR,
				    BF_FINE_DDC_EN_INFO, fddcs);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_nco_channel_selection_set(
	adi_ad9081_device_t *device, uint8_t fddcs, uint8_t channel)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_NCO_CTRL_ADDR,
				BF_FINE_DDC0_NCO_REGMAP_CHAN_SEL_INFO, channel);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_nco_channel_select_via_gpio_set(
	adi_ad9081_device_t *device, uint8_t fddcs, uint8_t mode)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			/* 0:  Register Map control (Use ddc_nco_regmap_chan_sel)
               1:  profile_pins[0]     is used. Pin level control {3'b0, profile_pins[0]}
               2:  profile_pins[1 :0] are used. Pin level control {2'b0, profile_pins[1:0]}
               3:  profile_pins[2 :0] are used. Pin level control {1'b0, profile_pins[2:0]}
               4:  profile_pins[3 :0] are used. Pin level control { profile_pins[3:0]}
               8:  profile_pins[0] Pin edge control- increment internal counter when rising edge of profile_pins[0] Pin.
               9:  profile_pins[1] Pin edge control- increment internal counter when rising edge of profile_pins[1] Pin.
               10: profile_pins[2] Pin edge control- increment internal counter when rising edge of profile_pins[2] Pin.
               11: profile_pins[3] Pin edge control- increment internal counter when rising edge of profile_pins[3] Pin.
               12: FHT expire based control - increment internal counter when FHT is expired.
             */
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_NCO_CTRL_ADDR,
				BF_FINE_DDC0_NCO_CHAN_SEL_MODE_INFO, mode);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_nco_channel_update_index_set(
	adi_ad9081_device_t *device, uint8_t fddcs, uint8_t channel)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_PROFILE_CTRL_ADDR,
				BF_FINE_DDC0_PROFILE_UPDATE_INDEX_INFO,
				channel);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_fine_nco_channel_update_mode_set(adi_ad9081_device_t *device,
						    uint8_t fddcs, uint8_t mode)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			/* 0: Instantaneous/Continuous Update. Phase increment and phase offset values are updated immediately.
               1: Phase increment and phase offset values are updated synchronously either with the chip_transfer
                  bit is set high or based on the GPIO (pin ddc_chip_gpio_transfer at nova_dig_dp_top hierarchy)
                  pin low to high transition. The chip transfer bit will be cleared once the transfer is complete.
            */
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_PROFILE_CTRL_ADDR,
				BF_FINE_DDC0_PROFILE_UPDATE_MODE_INFO, mode);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_fine_gpio_chip_xfer_mode_set(adi_ad9081_device_t *device,
						uint8_t fddcs, uint8_t mode)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			/* used when ddc0_phase_update_mode is '1'
               0: Phase increment and phase offset values are updated synchronously
                  when the chip_transfer bit is set high. The chip transfer bit will
                  be cleared once the transfer is complete.
               1: Phase increment and phase offset values are updated based on the
                  GPIO (pin ddc_chip_gpio_transfer at nova_dig_dp_top hierarchy) pin
                  low to high transition.
            */
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_PROFILE_CTRL_ADDR,
				BF_FINE_DDC0_GPIO_CHIP_TRANSFER_MODE_INFO,
				mode);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_trig_hop_en_set(adi_ad9081_device_t *device,
						uint8_t fddcs, uint8_t enable)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			/* 0: Frequency hopping is independent of trig signal. 1: Trig signal us used for frequency hoping. */
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_TRIG_CTRL_ADDR,
				BF_FINE_DDC_TRIG_HOP_EN_INFO, enable);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_nco_set(adi_ad9081_device_t *device,
					uint8_t fddcs, int64_t fddc_shift_hz)
{
	int32_t err;
	uint8_t i, fddc, cddc, cc2r_en, cddc_dcm;
	uint64_t ftw, adc_freq_hz;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_xbar_find_cddc(device, fddc,
							    &cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device,
						    REG_COARSE_DEC_CTRL_ADDR,
						    BF_COARSE_DEC_SEL_INFO,
						    &cddc_dcm, 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device,
						    REG_COARSE_DEC_CTRL_ADDR,
						    BF_COARSE_C2R_EN_INFO,
						    &cc2r_en, 1);
			AD9081_ERROR_RETURN(err);
			cddc_dcm =
				adi_ad9081_adc_ddc_coarse_dcm_decode(cddc_dcm);
			adc_freq_hz = device->dev_info.adc_freq_hz;
#ifdef __KERNEL__
			adc_freq_hz = div_u64(adc_freq_hz, cddc_dcm);
#else
			adc_freq_hz = adc_freq_hz / cddc_dcm;
#endif
			adc_freq_hz =
				(cc2r_en > 0) ? (adc_freq_hz * 2) : adc_freq_hz;
			err = adi_ad9081_hal_calc_tx_nco_ftw(
				device, adc_freq_hz, fddc_shift_hz, &ftw);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_adc_ddc_fine_nco_ftw_set(device, fddc,
								  ftw, 0, 0);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

#if AD9081_USE_FLOATING_TYPE > 0
int32_t adi_ad9081_adc_ddc_fine_nco_set_f(adi_ad9081_device_t *device,
					  uint8_t fddcs, double fddc_shift_hz)
{
	int32_t err;
	uint8_t i, fddc, cddc, cc2r_en, cddc_dcm;
	uint64_t ftw, a, b;
	double adc_freq_hz;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_xbar_find_cddc(device, fddc,
							    &cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_adc_ddc_coarse_select_set(device,
								   cddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device,
						    REG_COARSE_DEC_CTRL_ADDR,
						    BF_COARSE_DEC_SEL_INFO,
						    &cddc_dcm, 1);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(device,
						    REG_COARSE_DEC_CTRL_ADDR,
						    BF_COARSE_C2R_EN_INFO,
						    &cc2r_en, 1);
			AD9081_ERROR_RETURN(err);
			cddc_dcm =
				adi_ad9081_adc_ddc_coarse_dcm_decode(cddc_dcm);
			adc_freq_hz = (double)device->dev_info.adc_freq_hz;
			adc_freq_hz = adc_freq_hz / cddc_dcm;
			adc_freq_hz =
				(cc2r_en > 0) ? (adc_freq_hz * 2) : adc_freq_hz;
			err = adi_ad9081_hal_calc_nco_ftw_f(device, adc_freq_hz,
							    fddc_shift_hz, &ftw,
							    &a, &b);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_adc_ddc_fine_nco_ftw_set(device, fddc,
								  ftw, a, b);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}
#endif

int32_t adi_ad9081_adc_ddc_fine_nco_ftw_set(adi_ad9081_device_t *device,
					    uint8_t fddcs, uint64_t ftw,
					    uint64_t modulus_a,
					    uint64_t modulus_b)
{
	int32_t err;
#if AD9081_USE_SPI_BURST_MODE > 0
	uint8_t in_data[8];
#endif
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN((ftw >> 48) > 0);
	AD9081_INVALID_PARAM_RETURN((modulus_a >> 48) > 0);
	AD9081_INVALID_PARAM_RETURN((modulus_b >> 48) > 0);

	err = adi_ad9081_adc_ddc_fine_select_set(device, fddcs);
	AD9081_ERROR_RETURN(err);
#if AD9081_USE_SPI_BURST_MODE > 0
	in_data[0] = (REG_FINE_DDC_PHASE_INC0_ADDR >> 8) & 0x3F;
	in_data[1] = (REG_FINE_DDC_PHASE_INC0_ADDR >> 0) & 0xFF;
	in_data[2] = (uint8_t)((ftw >> 0) & 0xFF);
	in_data[3] = (uint8_t)((ftw >> 8) & 0xFF);
	in_data[4] = (uint8_t)((ftw >> 16) & 0xFF);
	in_data[5] = (uint8_t)((ftw >> 24) & 0xFF);
	in_data[6] = (uint8_t)((ftw >> 32) & 0xFF);
	in_data[7] = (uint8_t)((ftw >> 40) & 0xFF);
	if (API_CMS_ERROR_OK !=
	    device->hal_info.spi_xfer(device->hal_info.user_data, in_data, NULL,
				      0x8))
		return API_CMS_ERROR_SPI_XFER;
	in_data[0] = (REG_FINE_DDC_PHASE_INC_FRAC_A0_ADDR >> 8) & 0x3F;
	in_data[1] = (REG_FINE_DDC_PHASE_INC_FRAC_A0_ADDR >> 0) & 0xFF;
	in_data[2] = (uint8_t)((modulus_a >> 0) & 0xFF);
	in_data[3] = (uint8_t)((modulus_a >> 8) & 0xFF);
	in_data[4] = (uint8_t)((modulus_a >> 16) & 0xFF);
	in_data[5] = (uint8_t)((modulus_a >> 24) & 0xFF);
	in_data[6] = (uint8_t)((modulus_a >> 32) & 0xFF);
	in_data[7] = (uint8_t)((modulus_a >> 40) & 0xFF);
	if (API_CMS_ERROR_OK !=
	    device->hal_info.spi_xfer(device->hal_info.user_data, in_data, NULL,
				      0x8))
		return API_CMS_ERROR_SPI_XFER;
	in_data[0] = (REG_FINE_DDC_PHASE_INC_FRAC_B0_ADDR >> 8) & 0x3F;
	in_data[1] = (REG_FINE_DDC_PHASE_INC_FRAC_B0_ADDR >> 0) & 0xFF;
	in_data[2] = (uint8_t)((modulus_b >> 0) & 0xFF);
	in_data[3] = (uint8_t)((modulus_b >> 8) & 0xFF);
	in_data[4] = (uint8_t)((modulus_b >> 16) & 0xFF);
	in_data[5] = (uint8_t)((modulus_b >> 24) & 0xFF);
	in_data[6] = (uint8_t)((modulus_b >> 32) & 0xFF);
	in_data[7] = (uint8_t)((modulus_b >> 40) & 0xFF);
	if (API_CMS_ERROR_OK !=
	    device->hal_info.spi_xfer(device->hal_info.user_data, in_data, NULL,
				      0x8))
		return API_CMS_ERROR_SPI_XFER;
#else
	err = adi_ad9081_hal_bf_set(device, REG_FINE_DDC_PHASE_INC0_ADDR,
				    0x3000, ftw);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_FINE_DDC_PHASE_INC_FRAC_A0_ADDR,
				    0x3000, modulus_a);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_FINE_DDC_PHASE_INC_FRAC_B0_ADDR,
				    0x3000, modulus_b);
	AD9081_ERROR_RETURN(err);
#endif

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_nco_ftw_get(adi_ad9081_device_t *device,
					    uint8_t fddc, uint64_t *ftw,
					    uint64_t *modulus_a,
					    uint64_t *modulus_b)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(ftw);
	AD9081_NULL_POINTER_RETURN(modulus_a);
	AD9081_NULL_POINTER_RETURN(modulus_b);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
	AD9081_ERROR_RETURN(err);
	*ftw = 0;
	err = adi_ad9081_hal_bf_get(device, REG_FINE_DDC_ACTIVE_PHASE_INC0_ADDR,
				    0x3000, (uint8_t *)ftw, 8);
	AD9081_ERROR_RETURN(err);
	*modulus_a = 0;
	err = adi_ad9081_hal_bf_get(device, REG_FINE_DDC_PHASE_INC_FRAC_A0_ADDR,
				    0x3000, (uint8_t *)modulus_a, 8);
	AD9081_ERROR_RETURN(err);
	*modulus_b = 0;
	err = adi_ad9081_hal_bf_get(device, REG_FINE_DDC_PHASE_INC_FRAC_B0_ADDR,
				    0x3000, (uint8_t *)modulus_b, 8);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_fine_nco_phase_offset_set(adi_ad9081_device_t *device,
					     uint8_t fddcs, uint64_t offset)
{
	int32_t err;
#if AD9081_USE_SPI_BURST_MODE > 0
	uint8_t in_data[8];
#endif
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_ddc_fine_select_set(device, fddcs);
	AD9081_ERROR_RETURN(err);
#if AD9081_USE_SPI_BURST_MODE > 0
	in_data[0] = (REG_FINE_DDC_PHASE_OFFSET0_ADDR >> 8) & 0x3F;
	in_data[1] = (REG_FINE_DDC_PHASE_OFFSET0_ADDR >> 0) & 0xFF;
	in_data[2] = (uint8_t)((offset >> 0) & 0xFF);
	in_data[3] = (uint8_t)((offset >> 8) & 0xFF);
	in_data[4] = (uint8_t)((offset >> 16) & 0xFF);
	in_data[5] = (uint8_t)((offset >> 24) & 0xFF);
	in_data[6] = (uint8_t)((offset >> 32) & 0xFF);
	in_data[7] = (uint8_t)((offset >> 40) & 0xFF);
	if (API_CMS_ERROR_OK !=
	    device->hal_info.spi_xfer(device->hal_info.user_data, in_data, NULL,
				      0x8))
		return API_CMS_ERROR_SPI_XFER;
#else
	err = adi_ad9081_hal_bf_set(device, REG_FINE_DDC_PHASE_OFFSET0_ADDR,
				    0x3000, offset);
	AD9081_ERROR_RETURN(err);
#endif

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_dither_en_set(adi_ad9081_device_t *device,
					      uint8_t fddcs,
					      uint8_t amp_dither_en,
					      uint8_t phase_dither_en)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_DITHER_ADDR,
				BF_FINE_DDC0_AMP_DITHER_EN_INFO, amp_dither_en);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_DITHER_ADDR,
				BF_FINE_DDC0_PHASE_DITHER_EN_INFO,
				phase_dither_en);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_chip_xfer_set(adi_ad9081_device_t *device,
					      uint8_t fddcs)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			/* 1: Used to synchronize the transfer of data from master to slave registers.
               0: Do nothing.
               Note: This bit is used to update the DDC Phase Increment and Phase Offset
               registers when ddc0_phase_update_mode = 1 and ddc0_gpio_chip_transfer_mode = 0
             */
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_TRANSFER_CTRL_ADDR,
				BF_FINE_DDC0_CHIP_TRANSFER_INFO, 1);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_fine_chip_xfer_status_get(adi_ad9081_device_t *device,
					     uint8_t fddcs, uint8_t *status)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			/* 1: Transfer of data from master to slave registers is complete.
               0: Indicates the data transfer is not requested or not completed.
             */
			err = adi_ad9081_hal_bf_get(
				device, REG_FINE_DDC_TRANSFER_STATUS_ADDR,
				BF_FINE_DDC0_CHIP_TRANSFER_STATUS_INFO, status,
				1);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_psw_set(adi_ad9081_device_t *device,
					uint8_t fddcs, uint64_t psw)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_PSW_0_ADDR, 0x2800,
				psw); /* Profile Select Word */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_ddc_fine_overall_dcm_set(adi_ad9081_device_t *device,
						uint8_t fddcs, uint8_t dcm)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_adc_ddc_fine_select_set(device, fddc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device,
						    REG_DDC_OVERALL_DECIM_ADDR,
						    BF_DDC_OVERALL_DECIM_INFO,
						    dcm);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_core_setup(adi_ad9081_device_t *device,
				  uint8_t adc_cores)
{
	int32_t err;
	uint8_t core_status;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_core_analog_regs_enable_set(device, adc_cores, 1);
	AD9081_ERROR_RETURN(err);

	/* start app task */
	err = adi_ad9081_hal_bf_set(device, REG_REG8_SCRATCH1_ADDR,
				    BF_REG8_SCRATCH_2_INFO, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_delay_us(device, 20000);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_get(device, 0x3742, 0x0800, &core_status,
				    1); /* @msg2 */
	AD9081_ERROR_RETURN(err);
	if (core_status != 0xff)
		AD9081_LOG_ERR("Boot not completed, Expected core_status 0xFF");

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_path_or_bypass_config(adi_ad9081_device_t *device, uint8_t cddcs,
				     uint8_t fddcs, int64_t cddc_shift[4],
				     int64_t fddc_shift[8], uint8_t cddc_dcm[4],
				     uint8_t fddc_dcm[8], uint8_t cc2r_en[4],
				     uint8_t fc2r_en[8], uint8_t bypass_mode)
{
	int32_t err;
	uint64_t adc_freq_hz, ftw;
	uint8_t i, j, cddc, fddc, fddcs_enabled, cddc_dcm_value, fddc_dcm_value,
		overall_dcm_value;
	uint8_t adc_cddc_xbar, cddc_fddc_xbar;
	uint8_t die_id;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(device->dev_info.adc_freq_hz == 0);

	err = adi_ad9081_device_die_id_get(device, &die_id);
	AD9081_ERROR_RETURN(err);

	/* set adc flash tskew writes depending on fadc, setAdcFlashTskew()@ad9081_rx_r2.py */
	if (device->dev_info.dev_rev == 3) { /* r2 */
		if ((die_id & 0x80) == 0) { /* ad9081 */
			if (device->dev_info.adc_freq_hz <=
			    2000000000ULL) { /* 2.0e9 */
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					1); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					0); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			} else if (device->dev_info.adc_freq_hz <=
				   3000000000ULL) { /* 3.0e9 */
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					0); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					3); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			} else if (device->dev_info.adc_freq_hz <=
				   4000000000ULL) { /* 4.0e9 */
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					0); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					2); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			} else if (device->dev_info.adc_freq_hz <=
				   5000000000ULL) { /* 5.0e9 */
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					0); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					1); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			} else if (device->dev_info.adc_freq_hz <=
				   6000000000ULL) { /* 6.0e9 */
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					0); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					0); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			} else {
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					0); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					0); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			}
		} else { /* mxfe */
			if (device->dev_info.adc_freq_hz <=
			    1000000000ULL) { /* 1.0e9 */
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					1); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					2); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			} else if (device->dev_info.adc_freq_hz <=
				   1500000000ULL) { /* 1.5e9 */
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					1); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					1); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			} else if (device->dev_info.adc_freq_hz <=
				   2000000000ULL) { /* 2.0e9 */
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					1); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					0); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			} else if (device->dev_info.adc_freq_hz <=
				   3000000000ULL) { /* 3.0e9 */
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					0); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					3); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			} else if (device->dev_info.adc_freq_hz <=
				   4000000000ULL) { /* 4.0e9 */
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					0); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					2); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			} else {
				err = adi_ad9081_hal_bf_set(
					device, 0x212c, 0x0103,
					0); /* spare_uint8_width1_3 */
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_hal_bf_set(
					device, 0x2114, 0x0204,
					2); /* adc_flash_tskew */
				AD9081_ERROR_RETURN(err);
			}
		}
		err = adi_ad9081_hal_reg_set(device, 0x2100,
					     1); /* @user_ctrl_transfer */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_delay_us(device, 100000);
		AD9081_ERROR_RETURN(err);
	}

	/* set default crossbar */
	adc_cddc_xbar =
		((die_id & 0x80) == 0) ?
			AD9081_ADC_2_ADC_REAL_MODE :
			AD9081_ADC_4_ADC_REAL_MODE; /* ad9081: 2x6G adc, mxfe: 4x4G adc */
	cddc_fddc_xbar = AD9081_ADC_CDDC0_TO_FDDC0 | AD9081_ADC_CDDC1_TO_FDDC1 |
			 AD9081_ADC_CDDC0_TO_FDDC2 | AD9081_ADC_CDDC1_TO_FDDC3;
	cddc_fddc_xbar |= AD9081_ADC_CDDC2_TO_FDDC4 |
			  AD9081_ADC_CDDC3_TO_FDDC5 |
			  AD9081_ADC_CDDC2_TO_FDDC6 | AD9081_ADC_CDDC3_TO_FDDC7;
	err = adi_ad9081_adc_xbar_set(device, adc_cddc_xbar, cddc_fddc_xbar);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_pfir_din_select_set(
		device, AD9081_ADC_PFIR_ADC_PAIR0, 0,
		((die_id & 0x80) == 0) ? 1 : 0);
	AD9081_ERROR_RETURN(err);
	if (!bypass_mode) {
		/* enable coarse and fine nco */
		err = adi_ad9081_adc_ddc_coarse_nco_enable_set(device, cddcs);
		AD9081_ERROR_RETURN(err);
		fddcs_enabled = fddcs;
		for (i = 0; i < 8; i++) {
			fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
			if ((fddc > 0) && (fddc_dcm[i] == AD9081_FDDC_DCM_1)) {
				fddcs_enabled &=
					~fddc; /* do not enable fddc if its dcm is 1 */
			}
		}
		err = adi_ad9081_adc_ddc_fine_nco_enable_set(device,
							     fddcs_enabled);
		AD9081_ERROR_RETURN(err);

		/* set coarse ddc */
		for (i = 0; i < 4; i++) {
			cddc = cddcs & (AD9081_ADC_CDDC_0 << i);
			if (cddc > 0) {
				err = adi_ad9081_adc_ddc_coarse_dcm_set(
					device, cddc, cddc_dcm[i]);
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_adc_ddc_coarse_c2r_set(
					device, cddc, cc2r_en[i]);
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_adc_ddc_coarse_gain_set(
					device, cddc, 0);
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_adc_ddc_coarse_nco_mode_set(
					device, cddc,
					(cddc_shift[i] == 0) ?
						AD9081_ADC_NCO_ZIF :
						AD9081_ADC_NCO_VIF);
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_adc_ddc_coarse_nco_set(
					device, cddc, cddc_shift[i]);
				AD9081_ERROR_RETURN(err);
			}
		}
	} else {
		/* Disable coarse and fine ddcs */
		err = adi_ad9081_adc_ddc_coarse_nco_enable_set(device, 0x0F);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_adc_ddc_fine_nco_enable_set(device, 0xFF);
		AD9081_ERROR_RETURN(err);
	}

	/* set fine ddc complex-real enable, decimation ratio, default gain and nco mode */
	for (i = 0; i < 8; i++) {
		if (!bypass_mode) {
			fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
			if (fddc > 0) {
				err = adi_ad9081_adc_ddc_fine_dcm_set(
					device, fddc, fddc_dcm[i]);
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_adc_ddc_fine_c2r_set(
					device, fddc, fc2r_en[i]);
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_adc_ddc_fine_gain_set(device,
								       fddc, 0);
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_adc_ddc_fine_nco_mode_set(
					device, fddc,
					(fddc_shift[i] == 0) ?
						AD9081_ADC_NCO_ZIF :
						AD9081_ADC_NCO_VIF);
				AD9081_ERROR_RETURN(err);

				j = (i < 4) ? ((cddc_fddc_xbar & (1 << i)) > 0 ?
						       1 :
						       0) :
					      ((cddc_fddc_xbar & (1 << i)) > 0 ?
						       3 :
						       2);
				cddc_dcm_value =
					adi_ad9081_adc_ddc_coarse_dcm_decode(
						cddc_dcm[j]);
				adc_freq_hz = device->dev_info.adc_freq_hz;
#ifdef __KERNEL__
				adc_freq_hz =
					div_u64(adc_freq_hz, cddc_dcm_value);
#else
				adc_freq_hz = adc_freq_hz / cddc_dcm_value;
#endif
				adc_freq_hz = (cc2r_en[j] > 0) ?
						      (adc_freq_hz * 2) :
						      adc_freq_hz;
				err = adi_ad9081_hal_calc_tx_nco_ftw(
					device, adc_freq_hz, fddc_shift[i],
					&ftw);
				AD9081_ERROR_RETURN(err);
				err = adi_ad9081_adc_ddc_fine_nco_ftw_set(
					device, fddc, ftw, 0, 0);
				AD9081_ERROR_RETURN(err);

				fddc_dcm_value =
					adi_ad9081_adc_ddc_fine_dcm_decode(
						fddc_dcm[i]);
				overall_dcm_value =
					cddc_dcm_value * fddc_dcm_value;
				overall_dcm_value =
					(cc2r_en[j] > 0) ?
						(overall_dcm_value / 2) :
						overall_dcm_value;
				err = adi_ad9081_adc_ddc_fine_overall_dcm_set(
					device, fddc, overall_dcm_value);
				AD9081_ERROR_RETURN(err);
			}
		} else {
			err = adi_ad9081_adc_ddc_fine_overall_dcm_set(
				device, AD9081_ADC_FDDC_0 << i, 1);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_bypass_config(adi_ad9081_device_t *device)
{
	int32_t err;

	err = adi_ad9081_adc_path_or_bypass_config(device, 0, 0, NULL, NULL,
						   NULL, NULL, NULL, NULL, 1);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_config(adi_ad9081_device_t *device, uint8_t cddcs,
			      uint8_t fddcs, int64_t cddc_shift[4],
			      int64_t fddc_shift[8], uint8_t cddc_dcm[4],
			      uint8_t fddc_dcm[8], uint8_t cc2r_en[4],
			      uint8_t fc2r_en[8])
{
	int32_t err;

	err = adi_ad9081_adc_path_or_bypass_config(device, cddcs, fddcs,
						   cddc_shift, fddc_shift,
						   cddc_dcm, fddc_dcm, cc2r_en,
						   fc2r_en, 0);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_nyquist_zone_set(adi_ad9081_device_t *device,
					adi_ad9081_adc_select_e adc_sel,
					adi_ad9081_adc_nyquist_zone_e zone)
{
	int32_t err;
	uint32_t reg_nyquist_zone_addr = 0x2110;
	uint8_t nyquist_setting = 0x0;
	int i = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	if (device->dev_info.dev_rev == 1) { /* r1 */
		reg_nyquist_zone_addr = 0x210f;
	}
	if (device->dev_info.dev_rev == 2 ||
	    device->dev_info.dev_rev == 3) { /* r1r/r2 */
		reg_nyquist_zone_addr = 0x2110;
	}
	err = adi_ad9081_hal_reg_get(device, reg_nyquist_zone_addr,
				     &nyquist_setting);

	nyquist_setting |= 0x1; /*enable default override*/
	if (zone == AD9081_ADC_NYQUIST_ZONE_ODD) {
		for (i = 0; i < 4; i++) {
			if ((1 << i) & adc_sel) {
				nyquist_setting &= ~(0x2 << i);
			}
		}
	}
	if (zone == AD9081_ADC_NYQUIST_ZONE_EVEN) {
		for (i = 0; i < 4; i++) {
			if ((1 << i) & adc_sel) {
				nyquist_setting |= (0x2 << i);
			}
		}
	}
	err = adi_ad9081_hal_reg_set(device, reg_nyquist_zone_addr,
				     nyquist_setting);
	err = adi_ad9081_hal_reg_set(device, 0x2100,
				     1); /* Trigger Data Transfer */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_test_mode_usr_pattern_set(adi_ad9081_device_t *device,
						 uint8_t i_pattern[16],
						 uint8_t q_pattern[16])
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		err = adi_ad9081_hal_reg_set(device,
					     REG_TMODE_I_USR_PAT0_LSB_ADDR + i,
					     i_pattern[i]); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_reg_set(device,
					     REG_TMODE_I_USR_PAT4_LSB_ADDR + i,
					     i_pattern[i + 8]); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_reg_set(device,
					     REG_TMODE_Q_USR_PAT0_LSB_ADDR + i,
					     q_pattern[i]); /* not paged */
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_reg_set(device,
					     REG_TMODE_Q_USR_PAT4_LSB_ADDR + i,
					     q_pattern[i + 8]); /* not paged */
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_test_mode_config_set(adi_ad9081_device_t *device,
					    adi_ad9081_test_mode_e i_mode,
					    adi_ad9081_test_mode_e q_mode,
					    adi_ad9081_jesd_link_select_e links)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* set test mode on all converters */
	err = adi_ad9081_jesd_tx_conv_test_mode_enable_set(
		device, links,
		((i_mode == AD9081_TMODE_OFF) && (q_mode == AD9081_TMODE_OFF)) ?
			0x0000 :
			0xffff);
	AD9081_ERROR_RETURN(err);
	/* set test mode type */
	err = adi_ad9081_hal_bf_set(device, REG_TMODE_I_CTRL1_ADDR,
				    BF_TMODE_I_TYPE_SEL_INFO,
				    i_mode); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_TMODE_Q_CTRL1_ADDR,
				    BF_TMODE_Q_TYPE_SEL_INFO,
				    q_mode); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_ctl_page_set(adi_ad9081_device_t *device,
					 adi_ad9081_adc_pfir_ctl_page_e page)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_reg_set(device, REG_PFILT_CTL_PAGE_ADDR, page);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_pfir_coeff_page_set(adi_ad9081_device_t *device,
				   adi_ad9081_adc_pfir_coeff_page_e page)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_reg_set(device, REG_PFILT_COEFF_PAGE_ADDR, page);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_pfir_din_select_set(adi_ad9081_device_t *device,
				   adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				   uint8_t i_sel, uint8_t q_sel)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_2bf_set(
				device, 0x0b12, 0x0200, i_sel, 0x0202,
				q_sel); /* PFILT_DIN_I_SEL, PFILT_DIN_Q_SEL */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_dout_select_set(adi_ad9081_device_t *device,
					    uint8_t adcs, uint8_t out_sel)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0b13, 0x0200,
				out_sel); /* PFILT_OUT_SEL */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_i_mode_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				       adi_ad9081_adc_pfir_i_mode_e i_mode)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x0c0c, 0x0400,
						    i_mode); /* pfir_i_mode */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_q_mode_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				       adi_ad9081_adc_pfir_q_mode_e q_mode)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x0c0c, 0x0404,
						    q_mode); /* pfir_q_mode */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_mode_set(adi_ad9081_device_t *device,
				     adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				     adi_ad9081_adc_pfir_i_mode_e i_mode,
				     adi_ad9081_adc_pfir_q_mode_e q_mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_pfir_i_mode_set(device, ctl_pages, i_mode);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_pfir_q_mode_set(device, ctl_pages, q_mode);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_i_gain_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				       adi_ad9081_adc_pfir_gain_e ix_gain,
				       adi_ad9081_adc_pfir_gain_e iy_gain)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x0c0d, 0x0300,
						    ix_gain); /* pfir_ix_gain */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x0c0d, 0x0303,
						    iy_gain); /* pfir_iy_gain */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_q_gain_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				       adi_ad9081_adc_pfir_gain_e qx_gain,
				       adi_ad9081_adc_pfir_gain_e qy_gain)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x0c0f, 0x0300,
						    qx_gain); /* pfir_qx_gain */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x0c0f, 0x0303,
						    qy_gain); /* pfir_qy_gain */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_gain_set(adi_ad9081_device_t *device,
				     adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				     adi_ad9081_adc_pfir_gain_e ix_gain,
				     adi_ad9081_adc_pfir_gain_e iy_gain,
				     adi_ad9081_adc_pfir_gain_e qx_gain,
				     adi_ad9081_adc_pfir_gain_e qy_gain)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_pfir_i_gain_set(device, ctl_pages, ix_gain,
					     iy_gain);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_pfir_q_gain_set(device, ctl_pages, qx_gain,
					     qy_gain);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_half_complex_delay_set(
	adi_ad9081_device_t *device, adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
	uint8_t delay)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x0c11, 0x0800,
						    delay); /* delay_seting */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_pfir_coeff_xfer_set(adi_ad9081_device_t *device,
				   adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				   uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0c17, 0x0100,
				enable); /* pfir_coeff_transfer */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_hc_prog_delay_set(
	adi_ad9081_device_t *device,
	adi_ad9081_adc_pfir_coeff_page_e coeff_pages, uint8_t delay)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & coeff_pages) {
			err = adi_ad9081_adc_pfir_coeff_page_set(device,
								 (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x0c1a, 0x0700,
						    delay); /* hc_prog_delay */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_pfir_quad_mode_set(adi_ad9081_device_t *device,
				  adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				  uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x0c1c, 0x0100,
						    enable); /* quad_mode */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_pfir_coeff_load_sel_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				       uint8_t sel)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			/* bit0: real_i_load
               bit1: real_q_load
               bit2: real_cross_i_load
               bit3: real_cross_q_load
               bit4: complex_load
             */
			err = adi_ad9081_hal_bf_set(device, 0x0c1d, 0x0500,
						    sel); /* @coeff_load_sel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_pfir_coeff_clear_set(adi_ad9081_device_t *device,
				    adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				    uint8_t clear)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0c1d, 0x0105,
				clear); /* coeff_clear@coeff_load_sel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_pfir_coeff_vle_set(adi_ad9081_device_t *device,
				  adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
				  uint8_t vle)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0c1d, 0x0106,
				vle); /* vle_coeff@coeff_load_sel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_rd_coeff_page_sel_set(
	adi_ad9081_device_t *device, adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
	uint8_t sel)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 2; i++) {
		if ((1 << i) & ctl_pages) {
			err = adi_ad9081_adc_pfir_ctl_page_set(device,
							       (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0c1e, 0x0200,
				sel); /* rd_coeff_page_sel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_coeff_validate(adi_ad9081_device_t *device,
					   uint8_t ntaps, uint16_t coeffs[192])
{
	int32_t i, first_16b_idx, last_16b_idx, first_12b_idx, last_12b_idx;
	int16_t coeff;
	uint16_t abs_coeff;
	uint8_t ntaps_index;
	uint8_t coeff_num[][3] = {
		/* 16bit, 12bit, 7bit */
		{ 48, 48, 96 }, /* 192 taps */
		{ 24, 24, 48 }, /* 96  taps */
		{ 16, 16, 32 }, /* 64  taps */
		{ 12, 12, 24 } /* 48  taps */
	};
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* get ntaps index */
	if (ntaps == 192)
		ntaps_index = 0;
	else if (ntaps == 96)
		ntaps_index = 1;
	else if (ntaps == 64)
		ntaps_index = 2;
	else if (ntaps == 48)
		ntaps_index = 3;
	else
		return API_CMS_ERROR_INVALID_PARAM;

	/* find first and last 16bit coeff */
	first_16b_idx = 192;
	last_16b_idx = -1;
	for (i = 0; i < 192; i++) {
		coeff = (int16_t)coeffs[i];
		abs_coeff =
			(coeff > 0) ? (uint16_t)(coeff) : (uint16_t)(-coeff);
		if ((abs_coeff & 0xf800) > 0) {
			first_16b_idx = first_16b_idx > i ? i : first_16b_idx;
			last_16b_idx = last_16b_idx < i ? i : last_16b_idx;
		}
	}

	/* find first and last 12bit coeff */
	first_12b_idx = 192;
	last_12b_idx = -1;
	for (i = 0; i < 192; i++) {
		coeff = (int16_t)coeffs[i];
		abs_coeff =
			(coeff > 0) ? (uint16_t)(coeff) : (uint16_t)(-coeff);
		if (((abs_coeff & 0xf800) == 0) && ((abs_coeff & 0x0fc0) > 0)) {
			first_12b_idx = first_12b_idx > i ? i : first_12b_idx;
			last_12b_idx = last_12b_idx < i ? i : last_12b_idx;
		}
	}

	/* validate coeffs */
	if ((first_16b_idx == 192) || (last_16b_idx == -1) ||
	    (first_16b_idx >= last_16b_idx))
		return API_CMS_ERROR_ERROR;
	if ((first_12b_idx == 192) || (last_12b_idx == -1) ||
	    (first_12b_idx >= last_12b_idx))
		return API_CMS_ERROR_ERROR;

	/* validate 16bits coeffs number */
	if ((last_16b_idx - first_16b_idx + 1) > coeff_num[ntaps_index][0])
		return API_CMS_ERROR_ERROR;

	/* ...12/12/12/.../12/16/16/16... */
	if (((last_12b_idx + 1) == first_16b_idx) &&
	    ((last_12b_idx - first_12b_idx + 1) <= coeff_num[ntaps_index][1]))
		return API_CMS_ERROR_OK;

	/* ...16/16/16/.../16/12/12/12... */
	if (((last_16b_idx + 1) == first_12b_idx) &&
	    ((last_12b_idx - first_12b_idx + 1) <= coeff_num[ntaps_index][1]))
		return API_CMS_ERROR_OK;

	/* ...12/12/16/.../16/12/12/12... */
	if (((first_12b_idx < first_16b_idx) &&
	     (last_16b_idx < last_12b_idx)) &&
	    ((last_12b_idx - first_12b_idx + 1) <=
	     (coeff_num[ntaps_index][0] + coeff_num[ntaps_index][1])))
		return API_CMS_ERROR_OK;

	return API_CMS_ERROR_ERROR;
}

int32_t
adi_ad9081_adc_pfir_coeff_set(adi_ad9081_device_t *device,
			      adi_ad9081_adc_pfir_coeff_page_e coeff_pages,
			      uint8_t index, uint16_t coeff)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_pfir_coeff_page_set(device, coeff_pages);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(device, 0x1900 + index * 2, coeff & 0xff);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(device, 0x1901 + index * 2, coeff >> 8);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_pfir_coeffs_set(adi_ad9081_device_t *device,
			       adi_ad9081_adc_pfir_coeff_page_e coeff_pages,
			       uint16_t *coeffs)
{
	int32_t err, j;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_pfir_coeff_page_set(device, coeff_pages);
	AD9081_ERROR_RETURN(err);
	for (j = 0; j < 192; j++) {
		err = adi_ad9081_hal_reg_set(device, 0x1900 + j * 2,
					     coeffs[j] & 0xff);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_reg_set(device, 0x1901 + j * 2,
					     coeffs[j] >> 8);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_pfir_config_set(
	adi_ad9081_device_t *device, adi_ad9081_adc_pfir_ctl_page_e ctl_pages,
	adi_ad9081_adc_pfir_coeff_page_e coeff_pages,
	adi_ad9081_adc_pfir_i_mode_e i_mode,
	adi_ad9081_adc_pfir_q_mode_e q_mode, adi_ad9081_adc_pfir_gain_e ix_gain,
	adi_ad9081_adc_pfir_gain_e iy_gain, adi_ad9081_adc_pfir_gain_e qx_gain,
	adi_ad9081_adc_pfir_gain_e qy_gain, uint8_t coeff_load_sel,
	uint16_t *coeffs, uint8_t coeffs_size)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_adc_pfir_coeff_clear_set(device, ctl_pages, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_pfir_coeff_clear_set(device, ctl_pages, 0);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_adc_pfir_i_mode_set(device, ctl_pages, i_mode);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_pfir_q_mode_set(device, ctl_pages, q_mode);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_pfir_i_gain_set(device, ctl_pages, ix_gain,
					     iy_gain);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_pfir_q_gain_set(device, ctl_pages, qx_gain,
					     qy_gain);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_adc_pfir_coeff_load_sel_set(device, ctl_pages,
						     coeff_load_sel);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_pfir_ctl_page_set(device, ctl_pages);
	AD9081_ERROR_RETURN(err);
	for (i = 0; i < coeffs_size; i++) {
		err = adi_ad9081_adc_pfir_coeff_set(device, coeff_pages, i,
						    coeffs[i]);
		AD9081_ERROR_RETURN(err);
	}
	err = adi_ad9081_adc_pfir_coeff_load_sel_set(device, ctl_pages, 0);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_adc_pfir_coeff_xfer_set(device, ctl_pages, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_pfir_coeff_xfer_set(device, ctl_pages, 0);
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_ddc_fine_status_sel_set(adi_ad9081_device_t *device,
				       adi_ad9081_adc_fine_ddc_select_e fddcs,
				       adi_ad9081_adc_select_e i_status_adc,
				       adi_ad9081_adc_select_e q_status_adc)
{
	int32_t err;
	uint8_t i, fddc;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 8; i++) {
		fddc = fddcs & (AD9081_ADC_FDDC_0 << i);
		if (fddc > 0) {
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_STATUS_SEL_ADDR,
				BF_FINE_DDC_I_STATUS_SEL_INFO, i_status_adc);
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, REG_FINE_DDC_STATUS_SEL_ADDR,
				BF_FINE_DDC_Q_STATUS_SEL_INFO, q_status_adc);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fd_thresh_set(adi_ad9081_device_t *device, uint8_t adcs,
				     uint16_t low_thresh, uint16_t up_thresh,
				     uint16_t low_thresh2, uint16_t up_thresh2,
				     uint16_t low_dwell_thresh,
				     uint16_t low_dwell_thresh2,
				     uint16_t up_dwell_thresh)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0331, 0x0b00,
				up_thresh); /* for fd0/fd1 */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0333, 0x0b00,
				low_thresh); /* for fd0/fd1 */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0335, 0x1000,
				low_dwell_thresh); /* for fd0/fd1 */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0337, 0x1000,
				low_dwell_thresh2); /* for fd1 */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0339, 0x1000,
				up_dwell_thresh); /* for fd1 */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x033b, 0x0b00,
						    low_thresh2); /* for fd1 */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x033d, 0x0b00,
						    up_thresh2); /* for fd1 */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fd0_force_set(adi_ad9081_device_t *device, uint8_t adcs,
				     uint8_t enable, uint8_t value)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0330, 0x0101,
				value); /* paged, fd_force_val */
			AD9081_ERROR_RETURN(err);
			/* enable = 0: ADC_FD0 functions normally, = 1, ADC_FD0 gets value programmed in fd_fource_val */
			err = adi_ad9081_hal_bf_set(
				device, 0x0330, 0x0100,
				enable); /* paged, fd_force */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fd1_force_set(adi_ad9081_device_t *device, uint8_t adcs,
				     uint8_t enable, uint8_t value)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0330, 0x0104,
				value); /* paged, fd_gpio2_force_val */
			AD9081_ERROR_RETURN(err);
			/* 0: ADC_FD1 functions normally, 1: ADC_FD1 gets value programmed in fd_gpio2_force_val */
			err = adi_ad9081_hal_bf_set(
				device, 0x0330, 0x0103,
				enable); /* paged, fd_force_gpio2 */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fd_en_set(adi_ad9081_device_t *device, uint8_t adcs,
				 uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			/* valid only if fd_fine_en_gpio = 0, 0: fd outputs pulled low, 1: fd outputs functional */
			err = adi_ad9081_hal_bf_set(
				device, 0x0330, 0x0102,
				enable); /* paged, fd_fine_en */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fd_en_via_gpio_set(adi_ad9081_device_t *device,
					  uint8_t adcs, uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			/* 0: fd pins are enabled based on fd_fine_en, 1: fd pins are enabled based on gpio input */
			err = adi_ad9081_hal_bf_set(
				device, 0x0330, 0x0105,
				enable); /* paged, fd_fine_en_gpio */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fd0_mux_sel_set(adi_ad9081_device_t *device,
				       uint8_t src_adc, uint8_t dst_adc)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & src_adc) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0352, 0x0200,
				dst_adc); /* paged, fd_0_sel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fd1_mux_sel_set(adi_ad9081_device_t *device,
				       uint8_t src_adc, uint8_t dst_adc)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & src_adc) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0352, 0x0202,
				dst_adc); /* paged, fd_1_sel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fd0_function_mode_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t mode)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			/* mode = 0: fd0 functions using upper and lower thresholds fd_low_thresh and fd_up_thresh
             *      = 1: fd0 functions using only fd_up_thresh, like fd0 going low is decided by dwell time and fd_up_thresh
             */
			err = adi_ad9081_hal_bf_set(
				device, 0x0352, 0x0104,
				mode); /* paged, fd0_func_sel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fd1_function_mode_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t mode)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			/* mode = 0: fd1 output functions based on fd_dwell_up_thresh
             *      = 1: fd1 output functions same as fd but with thresh2 value
             */
			err = adi_ad9081_hal_bf_set(
				device, 0x0330, 0x0106,
				mode); /* paged, fd_gpio2_thresh2 */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_cycle_delay_selection_set(adi_ad9081_device_t *device,
						 uint8_t adcs, uint8_t cd_index)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0b05, 0x0200,
				cd_index); /* paged, cdsel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_cycle_delay_enable_set(adi_ad9081_device_t *device,
					      uint8_t adcs, uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(device, 0x0b14, 0x0100,
						    enable); /* paged, cd_en */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_cycle_delay_enable_via_gpio_set(adi_ad9081_device_t *device,
					       uint8_t adcs, uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			/* 0: cdsel bitfield selects CSHIFT, 1: gpio selects CSHIFT */
			err = adi_ad9081_hal_bf_set(
				device, 0x0b05, 0x0102,
				enable); /* paged, cd_gpio_en */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_cycle_delay_set(adi_ad9081_device_t *device,
				       uint8_t adcs, uint8_t delay[4])
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0b01, 0x0400,
				delay[0]); /* paged, cshift0 */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0b02, 0x0400,
				delay[1]); /* paged, cshift1 */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0b03, 0x0400,
				delay[2]); /* paged, cshift2 */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x0b04, 0x0400,
				delay[3]); /* paged, cshift3 */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fractional_delay_status_selection_set(
	adi_ad9081_device_t *device, adi_ad9081_adc_select_e i_status_sel,
	adi_ad9081_adc_select_e q_status_sel)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_FD_EQ_STATUS_SEL_ADDR,
				    BF_FD_EQ_I_STATUS_SEL_INFO,
				    i_status_sel); /* not paged */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, REG_FD_EQ_STATUS_SEL_ADDR,
				    BF_FD_EQ_Q_STATUS_SEL_INFO,
				    q_status_sel); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_fractional_delay_selection_set(adi_ad9081_device_t *device,
					      uint8_t fd_index)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, 0x0b06, 0x0200,
				    fd_index); /* not paged, fdsel */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fractional_delay_down_sample_enable_set(
	adi_ad9081_device_t *device, uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(
		device, 0x0b06, 0x0104,
		enable); /* not paged, fdelay_downsample_en */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fractional_delay_enable_set(adi_ad9081_device_t *device,
						   uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, 0x0b06, 0x0103,
				    enable); /* not paged, fd_en */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_fractional_delay_enable_via_gpio_set(adi_ad9081_device_t *device,
						    uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* 0: fdsel bitfield selects FSHIFT, 1: gpio selects FSHIFT */
	err = adi_ad9081_hal_bf_set(device, 0x0b06, 0x0102,
				    enable); /* not paged, fd_gpio_en */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fractional_delay_set(adi_ad9081_device_t *device,
					    uint8_t delay[4])
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, 0x0b07, 0x0400,
				    delay[0]); /* not paged, fshift0 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x0b08, 0x0400,
				    delay[1]); /* not paged, fshift1 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x0b09, 0x0400,
				    delay[2]); /* not paged, fshift2 */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_bf_set(device, 0x0b0a, 0x0400,
				    delay[3]); /* not paged, fshift3 */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_fractional_delay_io_mux_set(adi_ad9081_device_t *device,
						   uint8_t mux)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* 0: fdelay io <-> coarse ddc0 mixer, 1: fdelay io <-> coarse ddc3 mixer */
	err = adi_ad9081_hal_bf_set(device, 0x0b18, 0x0100,
				    mux); /* not paged, fdelay_io_mux_sel */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_trig_prog_delay_set(adi_ad9081_device_t *device,
					   uint8_t delay)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_TRIG_PROG_DELAY_ADDR,
				    BF_TRIG_PROG_DELAY_INFO,
				    delay); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_sysref_resync_mode_set(adi_ad9081_device_t *device,
					      uint8_t mode)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	/* 0: timestamp mode, 1: resync mode */
	err = adi_ad9081_hal_bf_set(device, REG_SYNC_CTRL1_ADDR,
				    BF_SYSREF_RESYNC_MODE_INFO,
				    mode); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_sysref_rise_edge_enable_set(adi_ad9081_device_t *device,
						   uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_SYNC_CTRL1_ADDR,
				    BF_RISEDGE_SYSREF_INFO,
				    enable); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_trig_rise_edge_enable_set(adi_ad9081_device_t *device,
						 uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_TRIG_CTRL_ADDR,
				    BF_RISEDGE_TRIG_INFO,
				    enable); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_master_trig_enable_set(adi_ad9081_device_t *device,
					      uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_TRIG_CTRL_ADDR,
				    BF_MASTERTRIG_EN_INFO,
				    enable); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_loopback_master_trig_enable_set(adi_ad9081_device_t *device,
					       uint8_t enable)
{
	int32_t err;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	err = adi_ad9081_hal_bf_set(device, REG_TRIG_CTRL_ADDR,
				    BF_LOOPBACK_MASTERTRIG_INFO,
				    enable); /* not paged */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_nco_master_slave_sync(adi_ad9081_device_t *device,
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

	err = adi_ad9081_adc_ddc_coarse_sync_enable_set(device,
							AD9081_ADC_CDDC_ALL, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_coarse_sync_next_set(device,
						      AD9081_ADC_CDDC_ALL, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_coarse_trig_nco_reset_enable_set(
		device, AD9081_ADC_CDDC_ALL, 0);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_adc_ddc_fine_sync_enable_set(device,
						      AD9081_ADC_FDDC_ALL, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_fine_sync_next_set(device, AD9081_ADC_FDDC_ALL,
						    1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_fine_trig_nco_reset_enable_set(
		device, AD9081_ADC_FDDC_ALL, 0);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_device_nco_sync_mode_set(device,
						  is_master > 0 ? 1 : 2);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_device_nco_sync_trigger_source_set(device,
							    trigger_src);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_device_nco_sync_gpio_set(device, gpio_index,
						  is_master > 0 ? 1 : 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_device_nco_sync_extra_lmfc_num_set(device,
							    extra_lmfc_num);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_adc_ddc_coarse_sync_next_set(device,
						      AD9081_ADC_CDDC_ALL, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_fine_sync_next_set(device, AD9081_ADC_FDDC_ALL,
						    0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_coarse_sync_next_set(device,
						      AD9081_ADC_CDDC_ALL, 1);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_adc_ddc_fine_sync_next_set(device, AD9081_ADC_FDDC_ALL,
						    1);
	AD9081_ERROR_RETURN(err);

	err = adi_ad9081_device_nco_sync_reset_via_sysref_set(device, 0);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_device_nco_sync_reset_via_sysref_set(device, 1);
	AD9081_ERROR_RETURN(err);

	if (is_master > 0) {
		err = adi_ad9081_device_nco_sync_trigger_set(device);
		AD9081_ERROR_RETURN(err);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_clk_enable_set(adi_ad9081_device_t *device,
					   uint8_t adcs, uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x341, 0x0100,
				enable); /* paged, smon_clk_en */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_sframer_mode_enable_set(adi_ad9081_device_t *device,
						    uint8_t adcs,
						    uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x341, 0x0201,
				enable); /* paged, smon_sframer_mode_en */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_frame_counter_get(adi_ad9081_device_t *device,
					      uint8_t adcs, uint8_t *fcnt)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(
				device, 0x342, 0x0800, (uint8_t *)fcnt,
				sizeof(uint8_t)); /* paged, smon_status_fcnt */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_period_set(adi_ad9081_device_t *device,
				       uint8_t adcs, uint32_t period)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x343, 0x2000,
				period); /* paged, smon_period */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_status_get(adi_ad9081_device_t *device,
				       uint8_t adcs, uint32_t *status)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_get(
				device, 0x34a, 0x1400, (uint8_t *)status,
				sizeof(uint32_t)); /* paged, smon_status */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_thresh_set(adi_ad9081_device_t *device,
				       uint8_t adcs, uint16_t thresh_low,
				       uint16_t thresh_high)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x34d, 0x1000,
				thresh_low); /* paged, smon_thresh_low */
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x34f, 0x1000,
				thresh_high); /* paged, smon_thresh_low */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_status_update_set(adi_ad9081_device_t *device,
					      uint8_t adcs, uint8_t update)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			/* rising edge will trigger status value update */
			err = adi_ad9081_hal_bf_set(
				device, 0x347, 0x0100,
				update); /* paged, smon_status_update */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_status_read_select_set(adi_ad9081_device_t *device,
						   uint8_t adcs, uint8_t select)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			/* 1: peak detector */
			err = adi_ad9081_hal_bf_set(
				device, 0x347, 0x0301,
				select); /* paged, smon_status_rdsel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_smon_peak_detector_enable_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x347, 0x0104,
				enable); /* paged, smon_peak_en */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_jlink_select_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t select)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			/* 0 : link0, 1: link1 */
			err = adi_ad9081_hal_bf_set(
				device, 0x347, 0x0105,
				select); /* paged, smon_jlink_sel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_output_via_gpio_set(adi_ad9081_device_t *device,
						uint8_t adcs, uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			/* 1: gpio output for peak indication */
			err = adi_ad9081_hal_bf_set(
				device, 0x347, 0x0106,
				enable); /* paged, smon_gpio_en */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_sframer_enable_set(adi_ad9081_device_t *device,
					       uint8_t adcs, uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x348, 0x0100,
				enable); /* paged, smon_sframer_en */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_sframer_mode_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t mode)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x348, 0x0101,
				mode); /* paged, smon_sframer_mode */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_smon_sframer_input_select_set(adi_ad9081_device_t *device,
					     uint8_t adcs, uint8_t select)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x348, 0x0602,
				select); /* paged, smon_sframer_insel */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_sync_enable_set(adi_ad9081_device_t *device,
					    uint8_t adcs, uint8_t enable)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			err = adi_ad9081_hal_bf_set(
				device, 0x349, 0x0100,
				enable); /* paged, smon_sync_en */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_smon_next_sync_mode_set(adi_ad9081_device_t *device,
					       uint8_t adcs, uint8_t mode)
{
	int32_t err, i;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();

	for (i = 0; i < 4; i++) {
		if ((1 << i) & adcs) {
			err = adi_ad9081_adc_select_set(device, (1 << i));
			AD9081_ERROR_RETURN(err);
			/* 0: continous mode, 1: next sync mode */
			err = adi_ad9081_hal_bf_set(
				device, 0x349, 0x0101,
				mode); /* paged, smon_sync_next */
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_data_inversion_dc_coupling_set(adi_ad9081_device_t *device,
					      adi_ad9081_adc_select_e adc_sel,
					      uint8_t enable)
{
	int32_t err;
	uint32_t reg_customer_up_transfer_addr = 0x2100;
	uint32_t reg_user_settings_adc_cal_addr = 0x2115;
	uint32_t reg_data_inversion_dc_coupling_addr = 0x2111;
	uint8_t data_inversion_setting = 0x0;
	int i = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(enable > 1);

	err = adi_ad9081_hal_reg_get(device,
				     reg_data_inversion_dc_coupling_addr,
				     &data_inversion_setting);
	AD9081_ERROR_RETURN(err);

	if (enable) {
		for (i = 0; i < 4; i++) {
			if ((1 << i) & adc_sel) {
				data_inversion_setting |= (0x1 << i);
			}
		}
	} else {
		for (i = 0; i < 4; i++) {
			if ((1 << i) & adc_sel) {
				data_inversion_setting &= ~(0x1 << i);
			}
		}
	}
	err = adi_ad9081_hal_reg_set(device,
				     reg_data_inversion_dc_coupling_addr,
				     data_inversion_setting);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(
		device, reg_user_settings_adc_cal_addr,
		1); /* Enable user-defined ADC calibration settings */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(device, reg_customer_up_transfer_addr,
				     1); /* Trigger Data Transfer */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t
adi_ad9081_adc_offset_timing_calibration_set(adi_ad9081_device_t *device,
					     adi_ad9081_adc_select_e adc_sel,
					     uint8_t enable)
{
	int32_t err;
	uint32_t reg_customer_up_transfer_addr = 0x2100;
	uint32_t reg_user_settings_adc_cal_addr = 0x2115;
	uint32_t reg_offset_timing_calibration_addr = 0x2116;
	uint8_t offset_timing_calibration_setting = 0x0;
	int i = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(enable > 1);

	err = adi_ad9081_hal_reg_get(device, reg_offset_timing_calibration_addr,
				     &offset_timing_calibration_setting);
	AD9081_ERROR_RETURN(err);

	if (enable) {
		for (i = 0; i < 4; i++) {
			if ((1 << i) & adc_sel) {
				offset_timing_calibration_setting |= (0x1 << i);
			}
		}
	} else {
		for (i = 0; i < 4; i++) {
			if ((1 << i) & adc_sel) {
				offset_timing_calibration_setting &=
					~(0x1 << i);
			}
		}
	}
	err = adi_ad9081_hal_reg_set(device, reg_offset_timing_calibration_addr,
				     offset_timing_calibration_setting);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(
		device, reg_user_settings_adc_cal_addr,
		1); /* Enable user-defined ADC calibration settings */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(device, reg_customer_up_transfer_addr,
				     1); /* Trigger Data Transfer */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_gain_calibration_set(adi_ad9081_device_t *device,
					    adi_ad9081_adc_select_e adc_sel,
					    uint8_t enable)
{
	int32_t err;
	uint32_t reg_customer_up_transfer_addr = 0x2100;
	uint32_t reg_user_settings_adc_cal_addr = 0x2115;
	uint32_t reg_gain_calibration_addr = 0x2117;
	uint8_t gain_calibration_setting = 0x0;
	int i = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(enable > 1);

	err = adi_ad9081_hal_reg_get(device, reg_gain_calibration_addr,
				     &gain_calibration_setting);
	AD9081_ERROR_RETURN(err);

	if (enable) {
		for (i = 0; i < 4; i++) {
			if ((1 << i) & adc_sel) {
				gain_calibration_setting |= (0x1 << i);
			}
		}
	} else {
		for (i = 0; i < 4; i++) {
			if ((1 << i) & adc_sel) {
				gain_calibration_setting &= ~(0x1 << i);
			}
		}
	}
	err = adi_ad9081_hal_reg_set(device, reg_gain_calibration_addr,
				     gain_calibration_setting);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(
		device, reg_user_settings_adc_cal_addr,
		1); /* Enable user-defined ADC calibration settings */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(device, reg_customer_up_transfer_addr,
				     1); /* Trigger Data Transfer */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_adc_offset_calibration_set(adi_ad9081_device_t *device,
					      adi_ad9081_adc_select_e adc_sel,
					      uint8_t enable)
{
	int32_t err;
	uint32_t reg_customer_up_transfer_addr = 0x2100;
	uint32_t reg_user_settings_adc_cal_addr = 0x2115;
	uint32_t reg_offset_calibration_addr = 0x2117;
	uint8_t offset_calibration_setting = 0x0;
	int i = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(enable > 1);

	err = adi_ad9081_hal_reg_get(device, reg_offset_calibration_addr,
				     &offset_calibration_setting);
	AD9081_ERROR_RETURN(err);

	if (enable) {
		for (i = 0; i < 4; i++) {
			if ((1 << i) & adc_sel) {
				offset_calibration_setting |= (0x1 << (4 + i));
			}
		}
	} else {
		for (i = 0; i < 4; i++) {
			if ((1 << i) & adc_sel) {
				offset_calibration_setting &= ~(0x1 << (4 + i));
			}
		}
	}
	err = adi_ad9081_hal_reg_set(device, reg_offset_calibration_addr,
				     offset_calibration_setting);
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(
		device, reg_user_settings_adc_cal_addr,
		1); /* Enable user-defined ADC calibration settings */
	AD9081_ERROR_RETURN(err);
	err = adi_ad9081_hal_reg_set(device, reg_customer_up_transfer_addr,
				     1); /* Trigger Data Transfer */
	AD9081_ERROR_RETURN(err);

	return API_CMS_ERROR_OK;
}

/*! @} */