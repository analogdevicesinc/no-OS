// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad9208_api.c
 *
 * \brief Contains AD9208 APIs for ADC Chip Level Configuration and Control
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

#define AD9208_IP_CLK_MIN_HZ 2500000000ULL
#define AD9208_IP_CLK_MAX_HZ 6000000000ULL
#define AD9208_ADC_CLK_MIN_HZ 2500000000ULL
#define AD9208_ADC_CLK_MAX_HZ 3100000000ULL
#define HW_RESET_PERIOD_US 5000
#define SPI_RESET_PERIOD_US 5000

/*
 * ======================================
 * Revision Data
 *=====================================
 */
static uint8_t api_revision[3] = { 1, 0, 1 };

static int spi_configure(ad9208_handle_t *h)
{
	int err;

	err = ad9208_register_write(h, AD9208_IF_CFG_A_REG, 0x00);
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, AD9208_IF_CFG_B_REG, 0x80);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}

int ad9208_init(ad9208_handle_t *h)
{
	int err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (h->dev_xfer == NULL)
		return API_ERROR_INVALID_XFER_PTR;

	if (h->delay_us == NULL)
		return API_ERROR_INVALID_DELAYUS_PTR;

	if (h->hw_open != NULL) {
		err = h->hw_open(h->user_data);
		if (err != 0)
			return API_ERROR_HW_OPEN;
	}

	err = spi_configure(h);
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, 0xA, 0xFB);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}

int ad9208_deinit(ad9208_handle_t *h)
{
	int err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (h->hw_close != NULL) {
		err = h->hw_close(h->user_data);
		if (err != 0)
			return API_ERROR_HW_CLOSE;
	}
	return API_ERROR_OK;
}

int ad9208_get_chip_id(ad9208_handle_t *h, adi_chip_id_t *chip_id)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (chip_id == NULL)
		return API_ERROR_INVALID_PARAM;
	err = ad9208_register_read(h, AD9208_CHIP_TYPE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	chip_id->chip_type = tmp_reg;

	err = ad9208_register_read(h, AD9208_PROD_ID_MSB_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	chip_id->prod_id = tmp_reg;
	chip_id->prod_id <<= 8;

	err = ad9208_register_read(h, AD9208_PROD_ID_LSB_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	chip_id->prod_id |= tmp_reg;

	err = ad9208_register_read(h, AD9208_CHIP_GRADE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	chip_id->prod_grade = (tmp_reg >> 4);
	chip_id->dev_revision = (tmp_reg & 0x0F);

	return API_ERROR_OK;
}

int ad9208_reset(ad9208_handle_t *h, uint8_t hw_reset)
{
	int err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (hw_reset) {
		if (h->reset_pin_ctrl == NULL)
			return API_ERROR_INVALID_PARAM;
		err = h->reset_pin_ctrl(h->user_data, 0x1);
		if (err != 0)
			return API_ERROR_RESET_PIN_CTRL;
		if (h->delay_us != NULL) {
			err = h->delay_us(h->user_data, HW_RESET_PERIOD_US);
			if (err != 0)
				return API_ERROR_US_DELAY;
		}
		err = h->reset_pin_ctrl(h->user_data, 0x0);
		if (err != 0)
			return API_ERROR_RESET_PIN_CTRL;
	}

	err = ad9208_register_write(h, AD9208_IF_CFG_A_REG, 0x81);
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h, AD9208_IF_CFG_B_REG, 0x82);
	if (err != API_ERROR_OK)
		return err;

	if (h->delay_us != NULL) {
		err = h->delay_us(h->user_data, HW_RESET_PERIOD_US);
		if (err != 0)
			return API_ERROR_US_DELAY;
	}

	return API_ERROR_OK;

}

int ad9208_adc_set_channel_select(ad9208_handle_t *h, uint8_t ch)
{
	int err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (ch > AD9208_ADC_CH_ALL)
		return API_ERROR_INVALID_PARAM;
	err = ad9208_register_write(h, AD9208_CH_INDEX_REG, ch);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_adc_get_channel_select(ad9208_handle_t *h, uint8_t *ch)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (ch == NULL)
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_CH_INDEX_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	*ch = tmp_reg;

	return API_ERROR_OK;
}

int ad9208_set_pdn_pin_mode(ad9208_handle_t *h,
			    uint8_t pin_enable, ad9208_pdn_mode_t pin_mode)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if ((pin_mode != AD9208_POWERDOWN) && (pin_mode != AD9208_STANDBY))
		return API_ERROR_INVALID_PARAM;
	if (pin_enable > 1)
		return API_ERROR_INVALID_PARAM;

	if (!pin_enable) {
		err = ad9208_register_write(h,
					    AD9208_CHIP_PIN_CTRL0_REG, (uint8_t)
					    AD9208_CHIP_PDN_PIN_DISABLE);
		if (err != API_ERROR_OK)
			return err;
	} else {
		err = ad9208_register_write(h,
					    AD9208_CHIP_PIN_CTRL0_REG,
					    (uint8_t) ~
					    AD9208_CHIP_PDN_PIN_DISABLE);
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_read(h, AD9208_CHIP_PIN_CTRL1_REG,
					   &tmp_reg);
		if (err != API_ERROR_OK)
			return err;
		tmp_reg &= (~AD9208_CHIP_PDN_MODE(ALL));
		tmp_reg |= ((pin_mode == AD9208_STANDBY) ? 0x1 : 0x0);
		err = ad9208_register_write(h, AD9208_CHIP_PIN_CTRL1_REG,
					    tmp_reg);
		if (err != API_ERROR_OK)
			return err;
	}

	return API_ERROR_OK;
}

int ad9208_adc_set_ch_pdn_mode(ad9208_handle_t *h, ad9208_pdn_mode_t mode)
{
	int err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (mode > AD9208_POWERDOWN)
		return API_ERROR_INVALID_PARAM;
#if 0
	err = ad9208_register_read(h, AD9208_DEV_CFG_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_PDN_MODE(ALL);
	tmp_reg |= AD9208_PDN_MODE(mode);
#endif
	err =
		ad9208_register_write(h, AD9208_DEV_CFG_REG, AD9208_PDN_MODE(mode));
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_set_input_clk_cfg(ad9208_handle_t *h, uint64_t clk_freq_hz,
			     uint8_t div)
{
	int err;
	uint64_t fs_hz = 0x0;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((clk_freq_hz > AD9208_IP_CLK_MAX_HZ) ||
	    (clk_freq_hz < AD9208_IP_CLK_MIN_HZ))
		return API_ERROR_INVALID_PARAM;
	if ((div != 1) && (div != 2) && (div != 4))
		return API_ERROR_INVALID_PARAM;

	fs_hz = NO_OS_DIV_U64(clk_freq_hz, div);

	if ((fs_hz > AD9208_ADC_CLK_MAX_HZ) || (fs_hz < AD9208_ADC_CLK_MIN_HZ))
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_IP_CLK_CFG_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	tmp_reg &= ~AD9208_IP_CLK_DIV(ALL);
	tmp_reg |= AD9208_IP_CLK_DIV(div - 1);
	err = ad9208_register_write(h, AD9208_IP_CLK_CFG_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	h->adc_clk_freq_hz = fs_hz;
	return API_ERROR_OK;
}

int ad9208_get_adc_clk_freq(ad9208_handle_t *h, uint64_t *adc_clk_freq_hz)
{
	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (adc_clk_freq_hz == NULL)
		return API_ERROR_INVALID_PARAM;
	*adc_clk_freq_hz = h->adc_clk_freq_hz;

	return API_ERROR_OK;
}

int ad9208_set_input_clk_duty_cycle_stabilizer(ad9208_handle_t *h, uint8_t en)
{
	int err;
	uint16_t tmp_reg_addr;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (en > 1)
		return API_ERROR_INVALID_PARAM;

	tmp_reg_addr = AD9208_IP_CLK_DCS1_REG;
	err = ad9208_register_write(h, tmp_reg_addr, en);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg_addr = AD9208_IP_CLK_DCS2_REG;
	err = ad9208_register_write(h, tmp_reg_addr, en);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

#if 0
int ad9208_clk_set_phase_adjust(ad9208_handle_t *h,
				uint8_t ch, int8_t phase_adjust)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (ch > AD9208_ADC_CH_ALL)
		return API_ERROR_INVALID_PARAM;
	err = ad9208_adc_select_ch(h, ch);

	err = ad9208_register_read(h, AD9208_IP_CLK_PHASE_ADJ_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~(AD9208_IP_CLK_DIV(ALL));
	tmp_reg |= AD9208_IP_CLK_PHASE_ADJ(phase_adjust);
	err = ad9208_register_write(h, AD9208_IP_CLK_PHASE_ADJ_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}
#endif
int ad9208_get_revision(ad9208_handle_t *h, uint8_t *rev_major,
			uint8_t *rev_minor, uint8_t *rev_rc)
{
	int err = API_ERROR_OK;

	if (rev_major != NULL)
		*rev_major = api_revision[0];
	else
		err = API_ERROR_INVALID_PARAM;
	if (rev_minor != NULL)
		*rev_minor = api_revision[1];
	else
		err = API_ERROR_INVALID_PARAM;
	if (rev_rc != NULL)
		*rev_rc = api_revision[2];
	else
		err = API_ERROR_INVALID_PARAM;

	return err;
}
