// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad9208_adc_api.c
 *
 * \brief Contains AD9208 APIs for ADC, DDC, NCO configuration and control
 *
 * Release 1.0.x
 *
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */

#include "ad9208_api.h"
#include "ad9208_reg.h"
#include "api_errors.h"
#include <stddef.h>

#define AD9208_NOF_FC_MAX  4
#define AD9208_ADC_DCM_NONE     1
#define AD9208_ADC_DCM_MIN      1
#define AD9208_ADC_DCM_MAX     48
#define NCO_RESET_PERIOD_US   0

typedef struct adi_dec_filt_data {
	uint8_t dec_complex;
	uint8_t dec_real;
	uint8_t ctrl_reg_val;
	uint8_t sel_reg_val;
} adi_dec_filt_data;

static const adi_dec_filt_data ADI_DEC_FILTER_COMPLEX_TBL[] = {
	{2, 1, 0x3, 0x0},
	{3, 0, 0x7, 0x7},
	{4, 2, 0x0, 0x0},
	{6, 3, 0x4, 0x0},
	{8, 4, 0x1, 0x0},
	{10, 5, 0x7, 0x2},
	{12, 6, 0x5, 0x0},
	{15, 0, 0x7, 0x8},
	{16, 8, 0x2, 0x0},
	{20, 10, 0x7, 0x3},
	{24, 12, 0x6, 0x0},
	{40, 20, 0x7, 0x4},
	{48, 24, 0x7, 0x0}
};

#define LOWER_16(A) ((A) & 0xFFFF)
#define UPPER_16(A) (((A) >> 16) & 0xFFFF)
#define LOWER_32(A) ((A) & (uint32_t) 0xFFFFFFFF)
#define MHZ_TO_HZ(x) (((uint64_t)x)*1000*1000)
#define MS_TO_US(x) ((x)*1000)
#define U64MSB 0x8000000000000000ull

typedef struct {
	uint8_t dcm;
	uint8_t dcm_cfg;
} chip_decimation_ratio_cfg;

static const chip_decimation_ratio_cfg ad9208_dcm_table[] = {
	{1, 0x0}, {2, 0x1}, {3, 0x8}, {4, 0x2},
	{5, 0x5}, {6, 0x9}, {8, 0x3}, {10, 0x6},
	{12, 0xA}, {15, 0x7}, {16, 0x4}, {20, 0xD},
	{24, 0xB}, {30, 0xE}, {40, 0xF}, {48, 0xC}
};

static int ad9208_get_decimation_cfg(uint8_t dcm, uint8_t *cfg)
{
	int i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad9208_dcm_table); i++) {
		if (dcm == ad9208_dcm_table[i].dcm) {
			*cfg = ad9208_dcm_table[i].dcm_cfg;
			return API_ERROR_OK;
		}
	}

	return API_ERROR_INVALID_PARAM;
}

int ad9208_get_decimation(ad9208_handle_t *h, uint8_t *dcm)
{
	int i, err;
	uint8_t tmp_reg, cfg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	err = ad9208_register_read(h, AD9208_ADC_DCM_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	cfg = AD9208_ADC_DCM_RATE(tmp_reg);

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad9208_dcm_table); i++) {
		if (cfg == ad9208_dcm_table[i].dcm_cfg) {
			*dcm = ad9208_dcm_table[i].dcm;
			return API_ERROR_OK;
		}
	}

	return API_ERROR_INVALID_PARAM;
}

static int adi_api_utils_gcd(int u, int v)
{
	int t;

	while (v != 0) {
		t = u;
		u = v;
		v = t % v;
	}
	return u < 0 ? -u : u;	/* abs(u) */
}

static void adi_api_utils_mult_128(uint64_t a, uint64_t b, uint64_t *hi,
				   uint64_t *lo)
{
	uint64_t ah = (a >> 32), al = a & 0xffffffff,
		 bh = (b >> 32), bl = b & 0xffffffff,
		 rh = (ah * bh), rl = (al * bl),
		 rm1 = ah * bl, rm2 = al * bh,
		 rm1h = rm1 >> 32, rm2h = rm2 >> 32,
		 rm1l = rm1 & 0xffffffff, rm2l = rm2 & 0xffffffff,
		 rmh = rm1h + rm2h, rml = rm1l + rm2l, c = ((rl >> 32) + rml) >> 32;

	rl = rl + (rml << 32);
	rh = rh + rmh + c;
	*lo = rl;
	*hi = rh;
}

static void adi_api_utils_mod_128(uint64_t ah, uint64_t al, uint64_t div,
				  uint64_t *mod)
{
	uint64_t result = 0;
	uint64_t a;

	DIV_U64_REM(~0, div, &a);
	a += 1;

	DIV_U64_REM(ah, div, &ah);

	/*modular multiplication of (2^64*upper) % div */
	while (ah != 0) {
		if ((ah & 1) == 1) {
			result += a;
			if (result >= div)
				result -= div;
		}
		a <<= 1;
		if (a >= div)
			a -= div;
		ah >>= 1;
	}

	/* add up the 2 results and return the modulus */
	if (al > div)
		al -= div;

	DIV_U64_REM(al + result, div, mod);
}

static int ad9208_check_buffer_current(ad9208_adc_buff_curr_t buff_curr)
{
	int err;

	switch (buff_curr) {
	case AD9208_ADC_BUFF_CURR_400_UA:
	case AD9208_ADC_BUFF_CURR_500_UA:
	case AD9208_ADC_BUFF_CURR_600_UA:
	case AD9208_ADC_BUFF_CURR_700_UA:
	case AD9208_ADC_BUFF_CURR_800_UA:
	case AD9208_ADC_BUFF_CURR_1000_UA:
		err = API_ERROR_OK;
		break;
	default:
		err = API_ERROR_INVALID_PARAM;
	}
	return err;
}

static int ad9208_get_dec_filter_cfg(ad9208_adc_data_frmt_t op_data_format,
				     uint8_t dcm_rate,
				     uint8_t *filt_sel_val_0,
				     uint8_t *filt_sel_val_1)
{
	uint8_t i = 0;

	if (op_data_format == AD9208_DATA_FRMT_COMPLEX) {
		for (i = 0; i < NO_OS_ARRAY_SIZE(ADI_DEC_FILTER_COMPLEX_TBL); i++) {
			if (ADI_DEC_FILTER_COMPLEX_TBL[i].dec_complex ==
			    dcm_rate) {
				*filt_sel_val_0 =
					ADI_DEC_FILTER_COMPLEX_TBL[i].ctrl_reg_val;
				*filt_sel_val_1 =
					ADI_DEC_FILTER_COMPLEX_TBL[i].sel_reg_val;
				return API_ERROR_OK;
			}
		}
	}

	if (op_data_format == AD9208_DATA_FRMT_REAL) {
		for (i = 0; i < NO_OS_ARRAY_SIZE(ADI_DEC_FILTER_COMPLEX_TBL); i++) {
			if (ADI_DEC_FILTER_COMPLEX_TBL[i].dec_real == dcm_rate) {
				*filt_sel_val_0 =
					ADI_DEC_FILTER_COMPLEX_TBL[i].ctrl_reg_val;
				*filt_sel_val_1 =
					ADI_DEC_FILTER_COMPLEX_TBL[i].sel_reg_val;
				return API_ERROR_OK;
			}
		}
	}

	return API_ERROR_INVALID_PARAM;
}

int ad9208_adc_set_input_scale(ad9208_handle_t *h,
			       ad9208_adc_scale_range_t full_scale_range)
{
	int err;
	uint8_t fs_val;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	switch (full_scale_range) {
	case AD9208_ADC_SCALE_2P04_VPP:
		fs_val = 0;
		break;
	case AD9208_ADC_SCALE_1P13_VPP:
		fs_val = 0x8;
		break;
	case AD9208_ADC_SCALE_1P25_VPP:
		fs_val = 0x9;
		break;
	case AD9208_ADC_SCALE_1P7_VPP:
		fs_val = 0xD;
		break;
	case AD9208_ADC_SCALE_1P81_VPP:
		fs_val = 0xE;
		break;
	case AD9208_ADC_SCALE_1P93_VPP:
		fs_val = 0xF;
		break;
	default:
		return API_ERROR_INVALID_PARAM;
	}
	err = ad9208_register_write(h,
				    AD9208_FULL_SCALE_CFG_REG,
				    AD9208_TRM_VREF(fs_val));
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

static int ad9208_adc_set_vcm_export(ad9208_handle_t *h, uint8_t en)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	err = ad9208_register_read(h, AD9208_EXT_VCM_CTRL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	if (en)
		tmp_reg |= AD9208_EXT_VCM_BUFF;
	else
		tmp_reg &= ~AD9208_EXT_VCM_BUFF;
	err = ad9208_register_write(h, AD9208_EXT_VCM_CTRL_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_adc_set_input_cfg(ad9208_handle_t *h,
			     signal_coupling_t analog_input_mode,
			     uint8_t ext_vref,
			     ad9208_adc_scale_range_t full_scale_range)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((analog_input_mode >= COUPLING_UNKNOWN) || (ext_vref > 1))
		return API_ERROR_INVALID_PARAM;
	if ((analog_input_mode == COUPLING_DC) && (ext_vref == 1)) {
		/*Invalid Configuration DC Coupled Mode can't use external Vref */
		return API_ERROR_INVALID_PARAM;
	}
	/*Set Analog Input Mode Optimization */
	tmp_reg = analog_input_mode ? AD9208_DC_COUPLE_EN(1) :
		  AD9208_DC_COUPLE_EN(0);
	err = ad9208_register_write(h, AD9208_ANALOG_CFG_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/*ac_coupled mode */
	if (analog_input_mode == COUPLING_AC) {
		/*Turn OFF VCM Export Buffer */
		err = ad9208_adc_set_vcm_export(h, 0x0);
		if (err != API_ERROR_OK)
			return err;

		/*Set Internal or External Vref */
		tmp_reg = ext_vref ? AD9208_EXT_VREF_MODE : 0;
		err = ad9208_register_write(h, AD9208_VREF_CTRL_REG, tmp_reg);
		if (err != API_ERROR_OK)
			return err;
	} else {		/*dc_coupled mode */
		/*Disable External Vref Mode */
		err = ad9208_register_write(h, AD9208_VREF_CTRL_REG, 0x0);
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_adc_set_vcm_export(h, 0x1);
		if (err != API_ERROR_OK)
			return err;

	}

	err = ad9208_adc_set_input_scale(h, full_scale_range);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}

int ad9208_adc_set_input_buffer_cfg(ad9208_handle_t *h,
				    ad9208_adc_buff_curr_t buff_curr_n,
				    ad9208_adc_buff_curr_t buff_curr_p,
				    ad9208_adc_buff_curr_t vcm_buff)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	err = ad9208_check_buffer_current(buff_curr_n);
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_check_buffer_current(buff_curr_p);
	if (err != API_ERROR_OK)
		return err;

	err = ad9208_check_buffer_current(vcm_buff);
	if (err != API_ERROR_OK)
		return err;

	/*Optimize Buffer Settings for Common Mode Reference */
	err = ad9208_register_write(h,
				    AD9208_BUFF_CFG_P_REG,
				    AD9208_BUFF_CTRL_P(buff_curr_p));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h,
				    AD9208_BUFF_CFG_N_REG,
				    AD9208_BUFF_CTRL_N(buff_curr_n));
	if (err != API_ERROR_OK)
		return err;
	/*Set Optimal VCM Buffer */
	err = ad9208_register_read(h, AD9208_EXT_VCM_CTRL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_EXT_VCM_BUFF_CURR(ALL);
	tmp_reg |= AD9208_EXT_VCM_BUFF_CURR(vcm_buff);
	err = ad9208_register_write(h, AD9208_EXT_VCM_CTRL_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}

int ad9208_adc_set_dc_offset_filt_en(ad9208_handle_t *h, uint8_t en)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (en > 1)
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_DC_OFFSET_CAL_CTRL, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_DC_OFFSET_CAL_EN;
	tmp_reg |= (en) ? AD9208_DC_OFFSET_CAL_EN : 0;
	err = ad9208_register_write(h, AD9208_DC_OFFSET_CAL_CTRL, tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}

int ad9208_adc_set_fc_ch_mode(ad9208_handle_t *h, uint8_t fc_ch)
{
	int err;
	uint8_t tmp_reg, op_mode;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if ((fc_ch > AD9208_NOF_FC_MAX) || (fc_ch == 3))
		return API_ERROR_INVALID_PARAM;

	if (fc_ch == 4)
		op_mode = 0x3;
	else
		op_mode = fc_ch;

	err = ad9208_register_read(h, AD9208_ADC_MODE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_ADC_MODE(op_mode);
	tmp_reg |= AD9208_ADC_MODE(op_mode);
	err = ad9208_register_write(h, AD9208_ADC_MODE_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_adc_set_dcm_mode(ad9208_handle_t *h, uint8_t dcm)
{
	int err;
	uint8_t tmp_reg, dcm_mode;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	err = ad9208_get_decimation_cfg(dcm, &dcm_mode);
	if (err != API_ERROR_OK)
		return err;

	err = ad9208_register_read(h, AD9208_ADC_DCM_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	tmp_reg &= ~AD9208_ADC_DCM_RATE(ALL);
	tmp_reg |= dcm_mode;

	err = ad9208_register_write(h, AD9208_ADC_DCM_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_adc_set_data_format(ad9208_handle_t *h,
			       ad9208_adc_data_frmt_t ip_data_frmt,
			       ad9208_adc_data_frmt_t op_data_frmt)
{
	int err, offset;
	uint8_t tmp_reg, i;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((ip_data_frmt > AD9208_DATA_FRMT_COMPLEX) ||
	    (op_data_frmt > AD9208_DATA_FRMT_COMPLEX))
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_ADC_MODE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_ADC_Q_IGNORE;
	tmp_reg |= (op_data_frmt == AD9208_DATA_FRMT_COMPLEX) ? 0 :
		   AD9208_ADC_Q_IGNORE;
	err = ad9208_register_write(h, AD9208_ADC_MODE_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/*TODO: Consider dependancy on FC */
	for (i = 0; i < AD9208_NOF_FC_MAX; i++) {
		offset = (AD9208_DDCX_REG_OFFSET * i);
		err = ad9208_register_read(h, AD9208_DDCX_CTRL0_REG + offset,
					   &tmp_reg);
		if (err != API_ERROR_OK)
			return err;
		tmp_reg &= ~(AD9208_DDCX_MIXER_SEL |
			     AD9208_DDCX_COMPLEX_TO_REAL);
		tmp_reg |= (ip_data_frmt == AD9208_DATA_FRMT_COMPLEX) ?
			   AD9208_DDCX_MIXER_SEL : 0;
		tmp_reg |= (op_data_frmt == AD9208_DATA_FRMT_COMPLEX) ?
			   0 : AD9208_DDCX_COMPLEX_TO_REAL;
		err = ad9208_register_write(h, AD9208_DDCX_CTRL0_REG + offset,
					    tmp_reg);
		if (err != API_ERROR_OK)
			return err;
	}

	return API_ERROR_OK;
}

int ad9208_adc_set_sync_update_mode_enable(ad9208_handle_t *h, uint8_t en)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (en > 1)
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_DDC_SYNC_CTRL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_DDC_UPDATE_MODE;
	tmp_reg |= (en) ? AD9208_DDC_UPDATE_MODE : 0;
	err = ad9208_register_write(h, AD9208_DDC_SYNC_CTRL_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_adc_set_ddc_ip_mux(ad9208_handle_t *h,
			      uint8_t ddc_ch, ad9208_adc_ch_t i_data_src,
			      ad9208_adc_ch_t q_data_src)
{
	int err, offset;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((ddc_ch >= AD9208_NOF_FC_MAX) ||
	    (i_data_src < AD9208_ADC_CH_A) || (i_data_src > AD9208_ADC_CH_B) ||
	    (q_data_src < AD9208_ADC_CH_A) || (q_data_src > AD9208_ADC_CH_B))
		return API_ERROR_INVALID_PARAM;

	offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
	err =
		ad9208_register_read(h, AD9208_DDCX_DATA_SEL_REG + offset,
				     &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~(AD9208_DDCX_Q_IP_CHB_SEL | AD9208_DDCX_I_IP_CHB_SEL);
	tmp_reg |=
		(i_data_src == AD9208_ADC_CH_B) ? AD9208_DDCX_I_IP_CHB_SEL : 0;
	tmp_reg |=
		(q_data_src == AD9208_ADC_CH_B) ? AD9208_DDCX_Q_IP_CHB_SEL : 0;

	err = ad9208_register_write(h, AD9208_DDCX_DATA_SEL_REG + offset,
				    tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;

}

/*TODO: Resolve Mis-match in Chip DCM and DDC CH DCM MAX Values*/
int ad9208_adc_set_ddc_dcm(ad9208_handle_t *h, uint8_t ddc_ch, uint8_t dcm)
{
	int err, offset;
	uint8_t tmp_reg, filt_sel_reg_0, filt_sel_reg_1;
	ad9208_adc_data_frmt_t op_data_format = AD9208_DATA_FRMT_COMPLEX;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((ddc_ch >= AD9208_NOF_FC_MAX) || (dcm > AD9208_ADC_DCM_MAX) ||
	    (dcm < AD9208_ADC_DCM_MIN))
		return API_ERROR_INVALID_PARAM;

	offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
	err = ad9208_register_read(h, AD9208_DDCX_CTRL0_REG + offset, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	if (tmp_reg & AD9208_DDCX_COMPLEX_TO_REAL)
		op_data_format = AD9208_DATA_FRMT_REAL;
	err = ad9208_get_dec_filter_cfg(op_data_format, dcm, &filt_sel_reg_0,
					&filt_sel_reg_1);
	if (err != API_ERROR_OK)
		return err;

	tmp_reg &= ~AD9208_DDCX_DCM_FILT_SEL_0(ALL);
	tmp_reg |= AD9208_DDCX_DCM_FILT_SEL_0(filt_sel_reg_0);
	err = ad9208_register_write(h, AD9208_DDCX_CTRL0_REG + offset, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	err = ad9208_register_read(h, AD9208_DDCX_DATA_SEL_REG + offset, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	tmp_reg &= ~AD9208_DDCX_DCM_FILT_SEL_1(ALL);
	tmp_reg |= AD9208_DDCX_DCM_FILT_SEL_1(filt_sel_reg_1);
	err = ad9208_register_write(h, AD9208_DDCX_DATA_SEL_REG + offset,
				    tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_adc_set_ddc_gain(ad9208_handle_t *h,
			    uint8_t ddc_ch, uint8_t gain_db)
{
	int err, offset;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((ddc_ch >= AD9208_NOF_FC_MAX) || ((gain_db != 0) && (gain_db != 6)))
		return API_ERROR_INVALID_PARAM;

	offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
	err = ad9208_register_read(h, AD9208_DDCX_CTRL0_REG + offset, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	tmp_reg &= ~AD9208_DDCX_GAIN_SEL;
	tmp_reg |= (gain_db == 6) ? AD9208_DDCX_GAIN_SEL : 0;
	err = ad9208_register_write(h, AD9208_DDCX_CTRL0_REG + offset, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_adc_set_ddc_nco_mode(ad9208_handle_t *h,
				uint8_t ddc_ch, ad9208_adc_nco_mode_t mode)
{
	int err, offset;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if ((ddc_ch >= AD9208_NOF_FC_MAX) || (mode >= AD9208_ADC_NCO_INVLD))
		return API_ERROR_INVALID_PARAM;

	offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
	err = ad9208_register_read(h, AD9208_DDCX_CTRL0_REG + offset, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_DDCX_NCO_IF_MODE(ALL);
	tmp_reg |= AD9208_DDCX_NCO_IF_MODE(mode);
	err = ad9208_register_write(h, AD9208_DDCX_CTRL0_REG + offset, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_adc_set_ddc_nco_ftw(ad9208_handle_t *h, uint8_t ddc_ch,
			       uint64_t ftw, uint64_t mod_a, uint64_t mod_b)
{
	int err, offset;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (ddc_ch >= AD9208_NOF_FC_MAX)
		return API_ERROR_INVALID_PARAM;
	offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
	err = ad9208_register_write(h, (AD9208_DDCX_FTW0_REG + offset),
				    ADI_GET_BYTE(ftw, 0));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, (AD9208_DDCX_FTW1_REG + offset),
				    ADI_GET_BYTE(ftw, 8));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, (AD9208_DDCX_FTW2_REG + offset),
				    ADI_GET_BYTE(ftw, 16));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, (AD9208_DDCX_FTW3_REG + offset),
				    ADI_GET_BYTE(ftw, 24));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, (AD9208_DDCX_FTW4_REG + offset),
				    ADI_GET_BYTE(ftw, 32));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, (AD9208_DDCX_FTW5_REG + offset),
				    ADI_GET_BYTE(ftw, 40));
	if (err != API_ERROR_OK)
		return err;
	offset = (AD9208_DDCX_FRAC_REG_OFFSET * ddc_ch);
	if ((mod_a != 0) && (mod_b != 0)) {
		err = ad9208_register_write(h, AD9208_DDCX_MAW0_REG + offset,
					    ADI_GET_BYTE(mod_a, 0));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MAW1_REG + offset,
					    ADI_GET_BYTE(mod_a, 8));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MAW2_REG + offset,
					    ADI_GET_BYTE(mod_a, 16));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MAW3_REG + offset,
					    ADI_GET_BYTE(mod_a, 24));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MAW4_REG + offset,
					    ADI_GET_BYTE(mod_a, 32));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MAW5_REG + offset,
					    ADI_GET_BYTE(mod_a, 40));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MBW0_REG + offset,
					    ADI_GET_BYTE(mod_b, 0));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MBW1_REG + offset,
					    ADI_GET_BYTE(mod_b, 8));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MBW2_REG + offset,
					    ADI_GET_BYTE(mod_b, 16));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MBW3_REG + offset,
					    ADI_GET_BYTE(mod_b, 24));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MBW4_REG + offset,
					    ADI_GET_BYTE(mod_b, 32));
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_DDCX_MBW5_REG + offset,
					    ADI_GET_BYTE(mod_b, 40));
		if (err != API_ERROR_OK)
			return err;
	}

	ad9208_is_sync_spi_update_enabled(h, &tmp_reg);
	if (tmp_reg)
		ad9208_register_chip_transfer(h);
	return API_ERROR_OK;
}

int ad9208_adc_set_ddc_nco_phase(ad9208_handle_t *h, uint8_t ddc_ch,
				 uint64_t po)
{
	int err, offset;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (ddc_ch >= AD9208_NOF_FC_MAX)
		return API_ERROR_INVALID_PARAM;
	offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
	err = ad9208_register_write(h, (AD9208_DDCX_PO0_REG + offset),
				    ADI_GET_BYTE(po, 0));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, (AD9208_DDCX_PO1_REG + offset),
				    ADI_GET_BYTE(po, 8));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, (AD9208_DDCX_PO2_REG + offset),
				    ADI_GET_BYTE(po, 16));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, (AD9208_DDCX_PO3_REG + offset),
				    ADI_GET_BYTE(po, 24));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, (AD9208_DDCX_PO4_REG + offset),
				    ADI_GET_BYTE(po, 32));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, (AD9208_DDCX_PO5_REG + offset),
				    ADI_GET_BYTE(po, 40));
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_adc_set_ddc_nco(ad9208_handle_t *h, uint8_t ddc_ch,
			   const int64_t carrier_freq_hz)
{
	uint64_t tmp_freq;
	int err;
	uint8_t is_pow2 = 0;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (!carrier_freq_hz)
		return API_ERROR_INVALID_PARAM;

	if (!((carrier_freq_hz >= (int64_t) (0ll - h->adc_clk_freq_hz / 2)) &&
	      (carrier_freq_hz < (int64_t) (h->adc_clk_freq_hz / 2))))
		return API_ERROR_INVALID_PARAM;

	if (ddc_ch >= AD9208_NOF_FC_MAX)
		return API_ERROR_INVALID_PARAM;

	tmp_freq = carrier_freq_hz;
	while (tmp_freq <= h->adc_clk_freq_hz) {
		if ((tmp_freq) == h->adc_clk_freq_hz) {
			/* It is power of 2 */
			is_pow2 = 1;
			break;
		}
		tmp_freq *= 2;
	}

	if (is_pow2 == 1) {	/* Integer NCO mode */
		/*
		 * As we are in Integer NCO mode it guranteed the
		 *  value is integer power of 2
		 */
		tmp_freq = NO_OS_DIV_U64(h->adc_clk_freq_hz, carrier_freq_hz);
		tmp_freq = NO_OS_DIV_U64(ADI_POW2_48, tmp_freq);

		/* Write FTW */
		err = ad9208_adc_set_ddc_nco_ftw(h, ddc_ch, tmp_freq, 0, 0);
		if (err != API_ERROR_OK)
			return err;
	} else {		/* Programable Modulus Mode */
		int gcd;
		uint64_t ftw;
		uint64_t maw;
		uint64_t mbw;
		uint64_t M, N;

		uint64_t tmp_ah, tmp_al, /*tmp_bh, tmp_bl, tmp_fh, */ tmp_fl;

		gcd = adi_api_utils_gcd(carrier_freq_hz, h->adc_clk_freq_hz);
		M = NO_OS_DIV_U64(carrier_freq_hz, gcd);
		N = NO_OS_DIV_U64(h->adc_clk_freq_hz, gcd);

		if (M > NO_OS_S16_MAX) {
			uint64_t mask = U64MSB;
			int i = 0;

			while (((mask & M) == 0) && (mask != 1)) {
				mask >>= 1;
				i++;
			}
			ftw = NO_OS_DIV_U64(M * ((uint64_t) 1u << i), N);
			ftw *= ((uint64_t) 1u << (48 - i));
		} else
			ftw = NO_OS_DIV_U64(M * (ADI_POW2_48), N);

		adi_api_utils_mult_128(M, ADI_POW2_48, &tmp_ah, &tmp_al);
		adi_api_utils_mod_128(tmp_ah, tmp_al, N, &tmp_fl);

		maw = tmp_fl;
		mbw = N;

		gcd = adi_api_utils_gcd(maw, mbw);
		maw = NO_OS_DIV_U64(maw, gcd);
		mbw = NO_OS_DIV_U64(mbw, gcd);

		if ((maw > ADI_MAXUINT48) || (mbw > ADI_MAXUINT48))
			return API_ERROR_INVALID_PARAM;	/*out of Range */

		err = ad9208_adc_set_ddc_nco_ftw(h, ddc_ch, ftw, maw, mbw);
		if (err != API_ERROR_OK)
			return err;
	}
	return API_ERROR_OK;
}

int ad9208_adc_set_ddc_nco_reset(ad9208_handle_t *h)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	err = ad9208_register_read(h, AD9208_DDC_SYNC_CTRL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, AD9208_DDC_SYNC_CTRL_REG,
				    tmp_reg | AD9208_NCO_SOFT_RESET);
	if (err != API_ERROR_OK)
		return err;

	if ((h->delay_us != NULL) && (NCO_RESET_PERIOD_US != 0)) {
		err = h->delay_us(h->user_data, NCO_RESET_PERIOD_US);
		if (err != 0)
			return API_ERROR_US_DELAY;
	}

	err = ad9208_register_write(h, AD9208_DDC_SYNC_CTRL_REG,
				    (tmp_reg & ~AD9208_NCO_SOFT_RESET));
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}

int ad9208_adc_set_clk_phase(ad9208_handle_t *h, uint8_t phase_adj)
{
	int err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (phase_adj > AD9208_IP_CLK_PHASE_ADJ(ALL))
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_write(h, AD9208_IP_CLK_PHASE_ADJ_REG, phase_adj);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_adc_temp_sensor_set_enable(ad9208_handle_t *h, uint8_t en)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (en > 1)
		return API_ERROR_INVALID_PARAM;

	err = ad9208_adc_set_vcm_export(h, 0x0);
	if (err != API_ERROR_OK)
		return err;
	/*Set Internal or External Vref */
	tmp_reg = (en == 1) ? AD9208_CENTRAL_DIODE_20X_EN : 0x0;
	err = ad9208_register_write(h, AD9208_TEMP_DIODE_CTRL_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}
