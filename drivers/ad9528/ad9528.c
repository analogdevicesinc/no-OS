/***************************************************************************//**
 *   @file   AD9528.c
 *   @brief  Implementation of AD9528 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <math.h>
#include "platform_drivers.h"
#include "ad9528.h"
#include <xil_printf.h>

/******************************************************************************/
/************************ Local variables and types ***************************/
/******************************************************************************/
uint8_t ad9528_slave_select;

enum {
	AD9528_STAT_PLL1_LD,
	AD9528_STAT_PLL2_LD,
	AD9528_STAT_REFA,
	AD9528_STAT_REFB,
	AD9528_STAT_REFAB_MISSING,
	AD9528_STAT_VCXO,
	AD9528_STAT_PLL1_FB_CLK,
	AD9528_STAT_PLL2_FB_CLK,
	AD9528_SYNC,
};

enum {
	AD9528_VCO,
	AD9528_VCXO,
	AD9528_NUM_CLK_SRC,
};

struct ad9528_state
{
	uint32_t vco_out_freq[AD9528_NUM_CLK_SRC];
}ad9528_st;

/* Helpers to avoid excess line breaks */
#define AD_IFE(_pde, _a, _b) ((pdata->_pde) ? _a : _b)
#define AD_IF(_pde, _a) AD_IFE(_pde, _a, 0)

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param reg_addr - The address of the register to read.
 * @param reg_data - The register's value.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_spi_read(uint32_t reg_addr, uint32_t *reg_data)
{
	uint8_t buf[3];
	int32_t ret = 0;
	uint8_t index;

	*reg_data = 0;
	for(index = 0; index < AD9528_TRANSF_LEN(reg_addr); index++)
	{
		buf[0] = 0x80 | (reg_addr >> 8);
		buf[1] = reg_addr & 0xFF;
		buf[2] = 0x00;
		ret |= spi_write_and_read(ad9528_slave_select, buf, 3);
		reg_addr--;
		*reg_data <<= 8;
		*reg_data |= buf[2];
	}

	return ret;
}

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param reg_addr - The address of the register to write to.
 * @param reg_data - The value to write to the register.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_spi_write(uint32_t reg_addr, uint32_t reg_data)
{
	uint8_t buf[3];
	int32_t ret = 0;
	uint8_t index;

	for(index = 0; index < AD9528_TRANSF_LEN(reg_addr); index++)
	{
		buf[0] = reg_addr >> 8;
		buf[1] = reg_addr & 0xFF;
		buf[2] = (reg_data >> ((AD9528_TRANSF_LEN(reg_addr) - index - 1) * 8)) & 0xFF;
		ret |= spi_write_and_read(ad9528_slave_select, buf, 3);
		reg_addr--;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Poll register.
 *
 * @param reg_addr - The address of the register.
 * @param mask - The mask that is applied.
 * @param data - The expected data.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_poll(uint32_t reg_addr, uint32_t mask, uint32_t data)
{
	uint32_t reg_data;
	uint8_t  timeout = 100;
	int32_t ret;

	do {
		ret = ad9528_spi_read(reg_addr, &reg_data);
		if (ret < 0)
			return ret;
		mdelay(1);
	} while (((reg_data & mask) != data) && --timeout);

	return timeout ? 0 : -1;

	return 0;
}

/***************************************************************************//**
 * @brief Updates the AD9528 configuration
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_io_update(void)
{
    return ad9528_spi_write(AD9528_IO_UPDATE, AD9528_IO_UPDATE_EN);
}

/***************************************************************************//**
 * @brief Updates the AD9528 configuration.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_sync(void)
{
	int32_t ret;

	ret = ad9528_spi_write(AD9528_CHANNEL_SYNC, AD9528_CHANNEL_SYNC_SET);
	if (ret < 0)
		return ret;

	ret = ad9528_io_update();
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_CHANNEL_SYNC, 0);
	if (ret < 0)
		return ret;

	ret = ad9528_io_update();
	if (ret < 0)
		return ret;

	ret  = ad9528_poll(AD9528_READBACK,
			AD9528_VCXO_OK | AD9528_PLL2_LOCKED,
			AD9528_VCXO_OK | AD9528_PLL2_LOCKED);

	return ret;
}

/***************************************************************************//**
 * @brief Initializes the AD9528.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_setup(uint32_t spi_device_id,
					 uint8_t slave_select,
					 struct ad9528_platform_data ad9528_pdata)
{
	struct ad9528_state *st = &ad9528_st;
    struct ad9528_platform_data *pdata = &ad9528_pdata;
	struct ad9528_channel_spec *chan;
	uint32_t active_mask = 0;
	uint32_t ignoresync_mask = 0;
	uint32_t vco_freq;
	uint32_t vco_ctrl;
	uint32_t sysref_ctrl;
	uint32_t reg_data;
	int32_t ret, i;

	ad9528_slave_select = slave_select;
	spi_init(spi_device_id, 1, 1);
	mdelay(1);

	ret = ad9528_spi_write(AD9528_SERIAL_PORT_CONFIG,
			AD9528_SER_CONF_SOFT_RESET |
			(pdata->spi3wire ? 0 :
			AD9528_SER_CONF_SDO_ACTIVE));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_SERIAL_PORT_CONFIG_B,
			AD9528_SER_CONF_READ_BUFFERED);
	if (ret < 0)
		return ret;

	ret = ad9528_io_update();
	if (ret < 0)
		return ret;

	ret = ad9528_spi_read(AD9528_CHIP_ID, &reg_data);
	if (ret < 0)
		return ret;

	if ((reg_data & 0xFFFFFF) != AD9528_SPI_MAGIC) {
		xil_printf("AD9528 SPI Read Verify failed (0x%X).\n", reg_data);
		return -1;
	}

	/*
	 * PLL1 Setup
	 */
	ret = ad9528_spi_write(AD9528_PLL1_REF_A_DIVIDER,
		pdata->refa_r_div);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PLL1_REF_B_DIVIDER,
		pdata->refb_r_div);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PLL1_FEEDBACK_DIVIDER,
		pdata->pll1_feedback_div);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PLL1_CHARGE_PUMP_CTRL,
		AD_IFE(pll1_bypass_en, AD9528_PLL1_CHARGE_PUMP_TRISTATE,
		AD9528_PLL1_CHARGE_PUMP_CURRENT_nA(pdata->
			pll1_charge_pump_current_nA) |
		AD9528_PLL1_CHARGE_PUMP_MODE_NORMAL |
		AD9528_PLL1_CHARGE_PUMP_AUTO_TRISTATE_DIS));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PLL1_CTRL,
		AD_IFE(pll1_bypass_en,
		AD_IF(osc_in_diff_en, AD9528_PLL1_OSC_IN_DIFF_EN) |
		AD_IF(osc_in_cmos_neg_inp_en,
		      AD9528_PLL1_OSC_IN_CMOS_NEG_INP_EN) |
		AD9528_PLL1_REFB_BYPASS_EN | AD9528_PLL1_REFA_BYPASS_EN |
		AD9528_PLL1_FEEDBACK_BYPASS_EN,
		AD_IF(refa_en, AD9528_PLL1_REFA_RCV_EN) |
		AD_IF(refb_en, AD9528_PLL1_REFB_RCV_EN) |
		AD_IF(osc_in_diff_en, AD9528_PLL1_OSC_IN_DIFF_EN) |
		AD_IF(osc_in_cmos_neg_inp_en,
		      AD9528_PLL1_OSC_IN_CMOS_NEG_INP_EN) |
		AD_IF(refa_diff_rcv_en, AD9528_PLL1_REFA_DIFF_RCV_EN) |
		AD_IF(refb_diff_rcv_en, AD9528_PLL1_REFB_DIFF_RCV_EN)) |
		AD_IF(refa_cmos_neg_inp_en, AD9528_PLL1_REFA_CMOS_NEG_INP_EN) |
		AD_IF(refb_cmos_neg_inp_en, AD9528_PLL1_REFB_CMOS_NEG_INP_EN) |
		AD_IF(pll1_feedback_src_vcxo, AD9528_PLL1_SOURCE_VCXO) |
		AD9528_PLL1_REF_MODE(pdata->ref_mode));
	if (ret < 0)
		return ret;

	/*
	 * PLL2 Setup
	 */

	ret = ad9528_spi_write(AD9528_PLL2_CHARGE_PUMP,
		AD9528_PLL2_CHARGE_PUMP_CURRENT_nA(pdata->
			pll2_charge_pump_current_nA));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PLL2_FEEDBACK_DIVIDER_AB,
		AD9528_PLL2_FB_NDIV_A_CNT(pdata->pll2_ndiv_a_cnt) |
		AD9528_PLL2_FB_NDIV_B_CNT(pdata->pll2_ndiv_b_cnt));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PLL2_CTRL,
		AD9528_PLL2_CHARGE_PUMP_MODE_NORMAL |
		AD_IF(pll2_freq_doubler_en, AD9528_PLL2_FREQ_DOUBLER_EN));
	if (ret < 0)
		return ret;

	vco_freq = (pdata->vcxo_freq * (pdata->pll2_freq_doubler_en ? 2 : 1)
			/ pdata->pll2_r1_div) * AD9528_PLL2_FB_NDIV(pdata->
			pll2_ndiv_a_cnt, pdata->pll2_ndiv_b_cnt);

	vco_ctrl = AD_IF(pll2_freq_doubler_en || pdata->pll2_r1_div != 1,
				AD9528_PLL2_DOUBLER_R1_EN);
	ret = ad9528_spi_write(AD9528_PLL2_VCO_CTRL, vco_ctrl);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PLL2_VCO_DIVIDER,
		AD9528_PLL2_VCO_DIV_M1(pdata->pll2_vco_diff_m1) |
		AD_IFE(pll2_vco_diff_m1, 0,
		       AD9528_PLL2_VCO_DIV_M1_PWR_DOWN_EN));
	if (ret < 0)
		return ret;

	if (pdata->pll2_vco_diff_m1)
		st->vco_out_freq[AD9528_VCO] =
			vco_freq / pdata->pll2_vco_diff_m1;
	else
		st->vco_out_freq[AD9528_VCO] = vco_freq;

	st->vco_out_freq[AD9528_VCXO] = pdata->vcxo_freq;

	ret = ad9528_spi_write(AD9528_PLL2_R1_DIVIDER,
		AD9528_PLL2_R1_DIV(pdata->pll2_r1_div));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PLL2_N2_DIVIDER,
		AD9528_PLL2_N2_DIV(pdata->pll2_n2_div));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PLL2_LOOP_FILTER_CTRL,
		AD9528_PLL2_LOOP_FILTER_CPOLE1(pdata->cpole1) |
		AD9528_PLL2_LOOP_FILTER_RZERO(pdata->rzero) |
		AD9528_PLL2_LOOP_FILTER_RPOLE2(pdata->rpole2) |
		AD_IF(rzero_bypass_en,
		      AD9528_PLL2_LOOP_FILTER_RZERO_BYPASS_EN));
	if (ret < 0)
		return ret;


	for (i = 0; i < pdata->num_channels; i++) {
		chan = &pdata->channels[i];

		if (chan->output_dis)
			continue;

		if (chan->channel_num < AD9528_NUM_CHAN) {
			active_mask |= (1 << chan->channel_num);
			if (chan->sync_ignore_en)
				ignoresync_mask |= (1 << chan->channel_num);

			ret = ad9528_spi_write(
				AD9528_CHANNEL_OUTPUT(chan->channel_num),
				AD9528_CLK_DIST_DRIVER_MODE(chan->driver_mode) |
				AD9528_CLK_DIST_DIV(chan->channel_divider) |
				AD9528_CLK_DIST_DIV_PHASE(chan->divider_phase) |
				AD9528_CLK_DIST_CTRL(chan->signal_source));
			if (ret < 0)
				return ret;
		}
	}

	ret = ad9528_spi_write(AD9528_CHANNEL_PD_EN,
			AD9528_CHANNEL_PD_MASK(~active_mask));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_CHANNEL_SYNC_IGNORE,
			AD9528_CHANNEL_IGNORE_MASK(ignoresync_mask));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_SYSREF_K_DIVIDER,
			AD9528_SYSREF_K_DIV(pdata->sysref_k_div));
	if (ret < 0)
		return ret;

	sysref_ctrl = AD9528_SYSREF_PATTERN_MODE(SYSREF_PATTERN_CONTINUOUS) |
			AD9528_SYSREF_SOURCE(pdata->sysref_src);
	ret = ad9528_spi_write(AD9528_SYSREF_CTRL, sysref_ctrl);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PD_EN, AD9528_PD_BIAS);
	if (ret < 0)
		return ret;

	ret = ad9528_io_update();
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write(AD9528_PLL2_VCO_CTRL,
			vco_ctrl | AD9528_PLL2_VCO_CALIBRATE);
	if (ret < 0)
		return ret;

	ret = ad9528_io_update();
	if (ret < 0)
		return ret;

	ret = ad9528_poll(AD9528_READBACK,
			AD9528_IS_CALIBRATING, 0);
	if (ret < 0)
		return ret;

	sysref_ctrl |= AD9528_SYSREF_PATTERN_REQ;
	ret = ad9528_spi_write(AD9528_SYSREF_CTRL, sysref_ctrl);
	if (ret < 0)
		return ret;

	ret = ad9528_io_update();
	if (ret < 0)
		return ret;

	ret = ad9528_sync();
	if (ret < 0)
		return ret;

	xil_printf("AD9528 successfully initialized.\n");

	return 0;
}
