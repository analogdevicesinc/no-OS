/***************************************************************************//**
 *   @file   AD9523.c
 *   @brief  Implementation of AD9523 Driver.
 *   @author ACozma(andrei.cozma@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <math.h>
#include "platform_drivers.h"
#include "ad9523.h"
#include <xil_printf.h>

/******************************************************************************/
/************************ Local variables and types ***************************/
/******************************************************************************/
uint8_t ad9523_slave_select;

struct ad9523_state 
{
    struct ad9523_platform_data *pdata;
    uint32_t vcxo_freq;
    uint32_t vco_freq;
    uint32_t vco_out_freq[3];
    uint8_t vco_out_map[14];
}ad9523_st;

enum
{
	AD9523_VCO1,
	AD9523_VCO2,
	AD9523_VCXO,
	AD9523_NUM_CLK_SRC,
};

/* Helpers to avoid excess line breaks */
#define AD_IFE(_pde, _a, _b) ((pdata->_pde) ? _a : _b)
#define AD_IF(_pde, _a) AD_IFE(_pde, _a, 0)

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param registerAddress - The address of the register to read.
 *
 * @return registerValue - The register's value or negative error code.
*******************************************************************************/
int32_t ad9523_spi_read(uint32_t reg_addr, uint32_t *reg_data)
{
	uint8_t buf[3];
	int32_t ret = 0;
	uint8_t index;

	*reg_data = 0;
	for(index = 0; index < AD9523_TRANSF_LEN(reg_addr); index++)
	{
		buf[0] = 0x80 | (reg_addr >> 8);
		buf[1] = reg_addr & 0xFF;
		buf[2] = 0x00;
		ret |= spi_write_and_read(ad9523_slave_select, buf, 3);
		reg_addr--;
		*reg_data <<= 8;
		*reg_data |= buf[2];
	}

	return ret;
}

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param registerAddress - The address of the register to write to.
 * @param registerValue - The value to write to the register.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_spi_write(uint32_t reg_addr, uint32_t reg_data)
{
	uint8_t buf[3];
	int32_t ret = 0;
	uint8_t index;

	for(index = 0; index < AD9523_TRANSF_LEN(reg_addr); index++)
	{
		buf[0] = reg_addr >> 8;
		buf[1] = reg_addr & 0xFF;
		buf[2] = (reg_data >> ((AD9523_TRANSF_LEN(reg_addr) - index - 1) * 8)) & 0xFF;
		ret |= spi_write_and_read(ad9523_slave_select, buf, 3);
		reg_addr--;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Updates the AD9523 configuration
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_io_update(void)
{
    return ad9523_spi_write(AD9523_IO_UPDATE, AD9523_IO_UPDATE_EN);
}

/***************************************************************************//**
 * @brief Sets the clock provider for selected channel.
 *
 * @param ch - Selected channel.
 * @param out - Selected clock provider.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_vco_out_map(uint32_t ch, uint32_t out)
{
	struct ad9523_state *st = &ad9523_st;
	int32_t ret;
	uint32_t mask;
	uint32_t reg_data;

	switch (ch) {
	case 0 ... 3:
		ret = ad9523_spi_read(AD9523_PLL1_OUTPUT_CHANNEL_CTRL, &reg_data);
		if (ret < 0)
			break;
		mask = AD9523_PLL1_OUTP_CH_CTRL_VCXO_SRC_SEL_CH0 << ch;
		if (out) {
			reg_data |= mask;
			out = AD9523_VCXO;
		} else {
			reg_data &= ~mask;
		}
		ret = ad9523_spi_write(
				   AD9523_PLL1_OUTPUT_CHANNEL_CTRL, reg_data);
		break;
	case 4 ... 6:
		ret = ad9523_spi_read(AD9523_PLL1_OUTPUT_CTRL, &reg_data);
		if (ret < 0)
			break;
		mask = AD9523_PLL1_OUTP_CTRL_VCO_DIV_SEL_CH4_M2 << (ch - 4);
		if (out)
			reg_data |= mask;
		else
			reg_data &= ~mask;
		ret = ad9523_spi_write(AD9523_PLL1_OUTPUT_CTRL, reg_data);
		break;
	case 7 ... 9:
		ret = ad9523_spi_read(AD9523_PLL1_OUTPUT_CHANNEL_CTRL, &reg_data);
		if (ret < 0)
			break;
		mask = AD9523_PLL1_OUTP_CH_CTRL_VCO_DIV_SEL_CH7_M2 << (ch - 7);
		if (out)
			reg_data |= mask;
		else
			reg_data &= ~mask;
		ret = ad9523_spi_write(
				   AD9523_PLL1_OUTPUT_CHANNEL_CTRL, reg_data);
		break;
	default:
		return 0;
	}

	st->vco_out_map[ch] = out;

	return ret;
}

/***************************************************************************//**
 * @brief Updates the AD9523 configuration.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_sync(void)
{
	int32_t ret, tmp;
	uint32_t reg_data;

	ret = ad9523_spi_read(AD9523_STATUS_SIGNALS, &reg_data);
	if (ret < 0)
		return ret;

	tmp = reg_data;
	tmp |= AD9523_STATUS_SIGNALS_SYNC_MAN_CTRL;

	ret = ad9523_spi_write(AD9523_STATUS_SIGNALS, tmp);
	if (ret < 0)
		return ret;

	ad9523_io_update();
	tmp &= ~AD9523_STATUS_SIGNALS_SYNC_MAN_CTRL;

	ret = ad9523_spi_write(AD9523_STATUS_SIGNALS, tmp);
	if (ret < 0)
		return ret;

	return ad9523_io_update();

}

/***************************************************************************//**
 * @brief Initializes the AD9523.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_setup(uint32_t spi_device_id,
					 uint8_t slave_select,
					 struct ad9523_platform_data ad9523_pdata)
{
	struct ad9523_state *st = &ad9523_st;
    struct ad9523_platform_data *pdata = &ad9523_pdata;
	struct ad9523_channel_spec *chan;
	uint32_t active_mask = 0;
	int32_t ret, i;
	uint32_t reg_data;
	uint32_t version_id;

	ad9523_slave_select = slave_select;
	spi_init(spi_device_id, 1, 1);

	mdelay(1);
	ret = ad9523_spi_write(AD9523_SERIAL_PORT_CONFIG,
			   AD9523_SER_CONF_SOFT_RESET |
			  (pdata->spi3wire ? 0 :
			  AD9523_SER_CONF_SDO_ACTIVE));
	if (ret < 0)
		return ret;
	mdelay(1);

	ret = ad9523_spi_write(AD9523_READBACK_CTRL,
			  AD9523_READBACK_CTRL_READ_BUFFERED);
	if (ret < 0)
		return ret;

	ret = ad9523_io_update();
	if (ret < 0)
		return ret;

	ret = ad9523_spi_read(AD9523_EEPROM_CUSTOMER_VERSION_ID, &version_id);
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_EEPROM_CUSTOMER_VERSION_ID, 0xAD95);
	if (ret < 0)
		return ret;

	ret = ad9523_spi_read(AD9523_EEPROM_CUSTOMER_VERSION_ID, &reg_data);
	if (ret < 0)
		return ret;

	if (reg_data != 0xAD95) {
		xil_printf("SPI Read Verify failed (0x%X).\n\r", reg_data);
		return -1;
	}

	ret = ad9523_spi_write(AD9523_EEPROM_CUSTOMER_VERSION_ID, version_id);
	if (ret < 0)
		return ret;

	/*
	 * PLL1 Setup
	 */
	ret = ad9523_spi_write(AD9523_PLL1_REF_A_DIVIDER,
		pdata->refa_r_div);
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL1_REF_B_DIVIDER,
		pdata->refb_r_div);
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL1_FEEDBACK_DIVIDER,
		pdata->pll1_feedback_div);
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL1_CHARGE_PUMP_CTRL,
		AD_IFE(pll1_bypass_en, AD9523_PLL1_CHARGE_PUMP_TRISTATE,
		AD9523_PLL1_CHARGE_PUMP_CURRENT_nA(pdata->
			pll1_charge_pump_current_nA) |
		AD9523_PLL1_CHARGE_PUMP_MODE_NORMAL |
		AD9523_PLL1_BACKLASH_PW_MIN));
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL1_INPUT_RECEIVERS_CTRL,
		AD_IFE(pll1_bypass_en, AD9523_PLL1_REFA_REFB_PWR_CTRL_EN |
		AD_IF(osc_in_diff_en, AD9523_PLL1_OSC_IN_DIFF_EN) |
		AD_IF(osc_in_cmos_neg_inp_en, AD9523_PLL1_OSC_IN_CMOS_NEG_INP_EN),
		AD_IF(refa_diff_rcv_en, AD9523_PLL1_REFA_RCV_EN) |
		AD_IF(refb_diff_rcv_en, AD9523_PLL1_REFB_RCV_EN) |
		AD_IF(osc_in_diff_en, AD9523_PLL1_OSC_IN_DIFF_EN) |
		AD_IF(osc_in_cmos_neg_inp_en,
		      AD9523_PLL1_OSC_IN_CMOS_NEG_INP_EN) |
		AD_IF(refa_diff_rcv_en, AD9523_PLL1_REFA_DIFF_RCV_EN) |
		AD_IF(refb_diff_rcv_en, AD9523_PLL1_REFB_DIFF_RCV_EN)));
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL1_REF_CTRL,
		AD_IFE(pll1_bypass_en, AD9523_PLL1_BYPASS_FEEDBACK_DIV_EN |
		AD9523_PLL1_ZERO_DELAY_MODE_INT,
		AD_IF(zd_in_diff_en, AD9523_PLL1_ZD_IN_DIFF_EN) |
		AD_IF(zd_in_cmos_neg_inp_en,
		      AD9523_PLL1_ZD_IN_CMOS_NEG_INP_EN) |
		AD_IF(zero_delay_mode_internal_en,
		      AD9523_PLL1_ZERO_DELAY_MODE_INT) |
		AD_IF(osc_in_feedback_en, AD9523_PLL1_OSC_IN_PLL_FEEDBACK_EN) |
		AD_IF(refa_cmos_neg_inp_en, AD9523_PLL1_REFA_CMOS_NEG_INP_EN) |
		AD_IF(refb_cmos_neg_inp_en, AD9523_PLL1_REFB_CMOS_NEG_INP_EN)));
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL1_MISC_CTRL,
		AD9523_PLL1_REFB_INDEP_DIV_CTRL_EN |
		AD9523_PLL1_REF_MODE(pdata->ref_mode));
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL1_LOOP_FILTER_CTRL,
		AD9523_PLL1_LOOP_FILTER_RZERO(pdata->pll1_loop_filter_rzero));
	if (ret < 0)
		return ret;

	/*
	 * PLL2 Setup
	 */

	ret = ad9523_spi_write(AD9523_PLL2_CHARGE_PUMP,
		AD9523_PLL2_CHARGE_PUMP_CURRENT_nA(pdata->
			pll2_charge_pump_current_nA));
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL2_FEEDBACK_DIVIDER_AB,
		AD9523_PLL2_FB_NDIV_A_CNT(pdata->pll2_ndiv_a_cnt) |
		AD9523_PLL2_FB_NDIV_B_CNT(pdata->pll2_ndiv_b_cnt));
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL2_CTRL,
		AD9523_PLL2_CHARGE_PUMP_MODE_NORMAL |
		AD9523_PLL2_BACKLASH_CTRL_EN |
		AD_IF(pll2_freq_doubler_en, AD9523_PLL2_FREQ_DOUBLER_EN));
	if (ret < 0)
		return ret;

	st->vco_freq = (pdata->vcxo_freq * (pdata->pll2_freq_doubler_en ? 2 : 1)
			/ pdata->pll2_r2_div) * AD9523_PLL2_FB_NDIV(pdata->
			pll2_ndiv_a_cnt, pdata->pll2_ndiv_b_cnt);

	ret = ad9523_spi_write(AD9523_PLL2_VCO_CTRL,
		AD9523_PLL2_VCO_CALIBRATE);
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL2_VCO_DIVIDER,
		AD9523_PLL2_VCO_DIV_M1(pdata->pll2_vco_diff_m1) |
		AD9523_PLL2_VCO_DIV_M2(pdata->pll2_vco_diff_m2) |
		AD_IFE(pll2_vco_diff_m1, 0,
		       AD9523_PLL2_VCO_DIV_M1_PWR_DOWN_EN) |
		AD_IFE(pll2_vco_diff_m2, 0,
		       AD9523_PLL2_VCO_DIV_M2_PWR_DOWN_EN));
	if (ret < 0)
		return ret;

	if (pdata->pll2_vco_diff_m1)
		st->vco_out_freq[AD9523_VCO1] =
			st->vco_freq / pdata->pll2_vco_diff_m1;

	if (pdata->pll2_vco_diff_m2)
		st->vco_out_freq[AD9523_VCO2] =
			st->vco_freq / pdata->pll2_vco_diff_m2;

	st->vco_out_freq[AD9523_VCXO] = pdata->vcxo_freq;

	ret = ad9523_spi_write(AD9523_PLL2_R2_DIVIDER,
		AD9523_PLL2_R2_DIVIDER_VAL(pdata->pll2_r2_div));
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_PLL2_LOOP_FILTER_CTRL,
		AD9523_PLL2_LOOP_FILTER_CPOLE1(pdata->cpole1) |
		AD9523_PLL2_LOOP_FILTER_RZERO(pdata->rzero) |
		AD9523_PLL2_LOOP_FILTER_RPOLE2(pdata->rpole2) |
		AD_IF(rzero_bypass_en,
		      AD9523_PLL2_LOOP_FILTER_RZERO_BYPASS_EN));
	if (ret < 0)
		return ret;

	for (i = 0; i < pdata->num_channels; i++) {
		chan = &pdata->channels[i];
		if (chan->channel_num < AD9523_NUM_CHAN) {
			active_mask |= (1 << chan->channel_num);
			ret = ad9523_spi_write(
				AD9523_CHANNEL_CLOCK_DIST(chan->channel_num),
				AD9523_CLK_DIST_DRIVER_MODE(chan->driver_mode) |
				AD9523_CLK_DIST_DIV(chan->channel_divider) |
				AD9523_CLK_DIST_DIV_PHASE(chan->divider_phase) |
				(chan->sync_ignore_en ?
					AD9523_CLK_DIST_IGNORE_SYNC_EN : 0) |
				(chan->divider_output_invert_en ?
					AD9523_CLK_DIST_INV_DIV_OUTPUT_EN : 0) |
				(chan->low_power_mode_en ?
					AD9523_CLK_DIST_LOW_PWR_MODE_EN : 0) |
				(chan->output_dis ?
					AD9523_CLK_DIST_PWR_DOWN_EN : 0));
			if (ret < 0)
				return ret;

			ret = ad9523_vco_out_map(chan->channel_num,
					   chan->use_alt_clock_src);
			if (ret < 0)
				return ret;
		}
	}

	for(i = 0; i < AD9523_NUM_CHAN; i++)
	{
		if(!(active_mask & (1 << i)))
		{
			ad9523_spi_write(
					 AD9523_CHANNEL_CLOCK_DIST(i),
					 AD9523_CLK_DIST_DRIVER_MODE(TRISTATE) |
					 AD9523_CLK_DIST_PWR_DOWN_EN);
		}
	}

	ret = ad9523_spi_write(AD9523_POWER_DOWN_CTRL, 0);
	if (ret < 0)
		return ret;

	ret = ad9523_spi_write(AD9523_STATUS_SIGNALS,
			   AD9523_STATUS_MONITOR_01_PLL12_LOCKED);
	if (ret < 0)
		return ret;

	ret = ad9523_io_update();
	if (ret < 0)
		return ret;

	ret = ad9523_sync();
	if (ret < 0)
		return ret;

	xil_printf("AD9523 successfully initialized.\n\r");

	return 0;
}
