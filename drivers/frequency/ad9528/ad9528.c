/***************************************************************************//**
 *   @file   frequency/ad9528/ad9528.c
 *   @brief  Implementation of AD9528 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015-2016(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "ad9528.h"

static bool ad9528_pll2_valid_calib_div(unsigned int div)
{
	if (div < 16 || div > 255)
		return false;

	switch (div) {
	case 18:
	case 19:
	case 23:
	case 27:
		return false;
	}

	return true;
}

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param dev - The device structure.
 * @param reg_addr - The address of the register to read
 * @param reg_data - The register's value.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/

int32_t ad9528_spi_read(struct ad9528_dev *dev,
			uint32_t reg_addr,
			uint32_t *reg_data)
{
	uint8_t buf[3];

	int32_t ret = 0;

	*reg_data = 0;
	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;
	ret |= spi_write_and_read(dev->spi_desc,
				  buf,
				  3);
	*reg_data = buf[2];

	return ret;
}

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param dev - The device structure.
 * @param reg_addr - The address of the register to write
 * @param reg_data - The value to write to the register.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/

int32_t ad9528_spi_write(struct ad9528_dev *dev,
			 uint32_t reg_addr,
			 uint32_t reg_data)
{
	uint8_t buf[3];

	int32_t ret = 0;

	buf[0] = (uint8_t) reg_addr >> 8;
	buf[1] = (uint8_t) reg_addr & 0xFF;
	buf[2] = (uint8_t) reg_data;
	ret |= spi_write_and_read(dev->spi_desc,
				  buf,
				  3);

	return ret;
}

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param dev - The device structure.
 * @param reg_addr - The address of the register to read - address[31:16]
 *  holds the number of bytes to read (a round about method)-- it is also
 *  limited to 4 bytes max (to fill in a 32 bit integer type).
 * @param reg_data - The register's value.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_spi_read_n(struct ad9528_dev *dev,
			  uint32_t reg_addr,
			  uint32_t *reg_data)
{
	uint8_t buf[3];

	int32_t ret = 0;
	uint8_t index;

	*reg_data = 0;
	for(index = 0; index < AD9528_TRANSF_LEN(reg_addr); index++) {
		buf[0] = 0x80 | (reg_addr >> 8);
		buf[1] = reg_addr & 0xFF;
		buf[2] = 0x00;
		ret |= spi_write_and_read(dev->spi_desc,
					  buf,
					  3);
		reg_addr--;
		*reg_data <<= 8;
		*reg_data |= buf[2];
	}

	return ret;
}

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param dev - The device structure.
 * @param reg_addr - The address of the register to write - address[31:16]
 *  holds the number of bytes to write (a round about method)-- it is also
 *  limited to 4 bytes max (to fill in a 32 bit integer type).
 * @param reg_data - The value to write to the register.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_spi_write_n(struct ad9528_dev *dev,
			   uint32_t reg_addr,
			   uint32_t reg_data)
{
	uint8_t buf[3];

	int32_t ret = 0;
	uint8_t index;

	for(index = 0; index < AD9528_TRANSF_LEN(reg_addr); index++) {
		buf[0] = reg_addr >> 8;
		buf[1] = reg_addr & 0xFF;
		buf[2] = (reg_data >> ((AD9528_TRANSF_LEN(reg_addr) -
					index - 1) * 8)) & 0xFF;
		ret |= spi_write_and_read(dev->spi_desc,
					  buf,
					  3);
		reg_addr--;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Poll register.
 *
 * @param dev - The device structure.
 * @param reg_addr - The address of the register.
 * @param mask - The mask that is applied.
 * @param data - The expected data.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_poll(struct ad9528_dev *dev,
		    uint32_t reg_addr,
		    uint32_t mask,
		    uint32_t data)
{
	uint32_t reg_data;
	uint8_t timeout = 100;
	int32_t ret;

	do {
		ret = ad9528_spi_read_n(dev,
					reg_addr,
					&reg_data);
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
 * @param dev - The device structure.

 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_io_update(struct ad9528_dev *dev)
{
	return ad9528_spi_write_n(dev,
				  AD9528_IO_UPDATE,
				  AD9528_IO_UPDATE_EN);
}

/***************************************************************************//**
 * @brief Updates the AD9528 configuration.
 *
 * @param dev - The device structure.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_sync(struct ad9528_dev *dev)
{
	int32_t ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_CHANNEL_SYNC,
				 AD9528_CHANNEL_SYNC_SET);
	if (ret < 0)
		return ret;

	ret = ad9528_io_update(dev);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_CHANNEL_SYNC,
				 0);
	if (ret < 0)
		return ret;

	ret = ad9528_io_update(dev);
	if (ret < 0)
		return ret;

	if (ad9528_poll(dev, AD9528_READBACK,
			AD9528_VCXO_OK,
			AD9528_VCXO_OK) < 0) {
		ret = -1;
		printf("AD9528 VCXO missing!\n");
	}

	if (ad9528_poll(dev,
			AD9528_READBACK,
			AD9528_PLL2_LOCKED,
			AD9528_PLL2_LOCKED) < 0) {
		ret = -1;
		printf("AD9528 PLL2 NOT locked!\n");
	}

	return ret;
}

/***************************************************************************//**
 * @brief Initializes the AD9528.
 *
 * @param init_param - The structure containing the device initial parameters.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_init(struct ad9528_init_param *init_param)
{
	uint32_t i;

	// init assumes vcxo configuration, all channels lvds & vco source

	init_param->pdata->spi3wire = 0;
	init_param->pdata->ref_mode = 0;
	init_param->pdata->refa_en = 0;
	init_param->pdata->refa_diff_rcv_en = 0;
	init_param->pdata->refa_cmos_neg_inp_en = 0;
	init_param->pdata->refa_r_div = 1;
	init_param->pdata->refb_en = 0;
	init_param->pdata->refb_diff_rcv_en = 0;
	init_param->pdata->refb_cmos_neg_inp_en = 0;
	init_param->pdata->refb_r_div = 1;
	init_param->pdata->vcxo_freq = 0;
	init_param->pdata->osc_in_diff_en = 1;
	init_param->pdata->osc_in_cmos_neg_inp_en = 0;
	init_param->pdata->pll1_feedback_div = 1;
	init_param->pdata->pll1_feedback_src_vcxo = 1;
	init_param->pdata->pll1_charge_pump_current_nA = 5000;
	init_param->pdata->pll1_bypass_en = 0;
	init_param->pdata->pll2_charge_pump_current_nA = 805000;
	init_param->pdata->pll2_freq_doubler_en = 0;
	init_param->pdata->pll2_r1_div = 1;
	init_param->pdata->pll2_vco_div_m1 = 1;
	init_param->pdata->pll2_ndiv_a_cnt = 1;
	init_param->pdata->pll2_ndiv_b_cnt = 1;
	init_param->pdata->pll2_n2_div = 1;
	init_param->pdata->sysref_src = SYSREF_SRC_INTERNAL;
	init_param->pdata->sysref_k_div = 512;
	init_param->pdata->rpole2 = RPOLE2_900_OHM;
	init_param->pdata->rzero= RZERO_1850_OHM;
	init_param->pdata->cpole1 = CPOLE1_16_PF;
	init_param->pdata->rzero_bypass_en = 0;

	for (i = 0; i < init_param->pdata->num_channels; i++) {
		(&init_param->pdata->channels[i])->channel_num = 0;
		(&init_param->pdata->channels[i])->sync_ignore_en = 0;
		(&init_param->pdata->channels[i])->output_dis = 0;
		(&init_param->pdata->channels[i])->
		driver_mode = DRIVER_MODE_LVDS;
		(&init_param->pdata->channels[i])->signal_source = SOURCE_VCO;
		(&init_param->pdata->channels[i])->divider_phase = 0;
		(&init_param->pdata->channels[i])->channel_divider = 1;
	}

	return(0);
}


/***************************************************************************//**
 * @brief Initializes the AD9528.
 *
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9528_setup(struct ad9528_dev **device,
		     struct ad9528_init_param init_param)
{
	struct ad9528_channel_spec *chan;
	uint32_t active_mask = 0;
	uint32_t ignoresync_mask = 0;
	uint32_t vco_freq;
	uint32_t vco_ctrl = 0;
	uint32_t sysref_ctrl;
	uint32_t stat_en_mask = 0;
	uint32_t reg_data;
	int32_t ret;
	uint32_t i;
	uint32_t pll2_ndiv, pll2_ndiv_a_cnt, pll2_ndiv_b_cnt;
	struct ad9528_dev *dev;

	dev = (struct ad9528_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->pdata = init_param.pdata;

	/* SPI */
	ret = spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret < 0)
		return ret;

	/* GPIO */
	ret = gpio_get_optional(&dev->gpio_resetb, init_param.gpio_resetb);
	if (ret < 0)
		return ret;

	ad9528_reset(dev);

	ret = ad9528_spi_write_n(dev,
				 AD9528_SERIAL_PORT_CONFIG_B,
				 AD9528_SER_CONF_READ_BUFFERED);
	if (ret < 0)
		return ret;

	ret = ad9528_io_update(dev);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_read_n(dev,
				AD9528_CHIP_ID,
				&reg_data);
	if (ret < 0)
		return ret;

	if ((reg_data & 0xFFFFFF) != AD9528_SPI_MAGIC) {
		printf("AD9528 SPI Read Verify failed (0x%X).\n", reg_data);
		return -1;
	}

	/*
	 * PLL1 Setup
	 */
	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL1_REF_A_DIVIDER,
				 dev->pdata->refa_r_div);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL1_REF_B_DIVIDER,
				 dev->pdata->refb_r_div);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL1_FEEDBACK_DIVIDER,
				 dev->pdata->pll1_feedback_div);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL1_CHARGE_PUMP_CTRL,
				 AD_IFE(pll1_bypass_en,
					AD9528_PLL1_CHARGE_PUMP_TRISTATE,
					AD9528_PLL1_CHARGE_PUMP_CURRENT_nA(dev->pdata->
							pll1_charge_pump_current_nA) |
					AD9528_PLL1_CHARGE_PUMP_MODE_NORMAL |
					AD9528_PLL1_CHARGE_PUMP_AUTO_TRISTATE_DIS));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL1_CTRL,
				 AD_IFE(pll1_bypass_en,
					AD_IF(osc_in_diff_en,
					      AD9528_PLL1_OSC_IN_DIFF_EN) |
					AD_IF(osc_in_cmos_neg_inp_en,
					      AD9528_PLL1_OSC_IN_CMOS_NEG_INP_EN) |
					AD9528_PLL1_REFB_BYPASS_EN |
					AD9528_PLL1_REFA_BYPASS_EN |
					AD9528_PLL1_FEEDBACK_BYPASS_EN,
					AD_IF(refa_en,
					      AD9528_PLL1_REFA_RCV_EN) |
					AD_IF(refb_en,
					      AD9528_PLL1_REFB_RCV_EN) |
					AD_IF(osc_in_diff_en,
					      AD9528_PLL1_OSC_IN_DIFF_EN) |
					AD_IF(osc_in_cmos_neg_inp_en,
					      AD9528_PLL1_OSC_IN_CMOS_NEG_INP_EN) |
					AD_IF(refa_diff_rcv_en,
					      AD9528_PLL1_REFA_DIFF_RCV_EN) |
					AD_IF(refb_diff_rcv_en,
					      AD9528_PLL1_REFB_DIFF_RCV_EN)) |
				 AD_IF(refa_cmos_neg_inp_en,
				       AD9528_PLL1_REFA_CMOS_NEG_INP_EN) |
				 AD_IF(refb_cmos_neg_inp_en,
				       AD9528_PLL1_REFB_CMOS_NEG_INP_EN) |
				 AD_IF(pll1_feedback_src_vcxo,
				       AD9528_PLL1_SOURCE_VCXO) |
				 AD9528_PLL1_REF_MODE(dev->pdata->ref_mode));
	if (ret < 0)
		return ret;

	/*
	 * PLL2 Setup
	 */

	if (dev->pdata->pll2_bypass_en) {
		ret = ad9528_spi_write_n(dev, AD9528_PLL2_CTRL,
					 AD9528_PLL2_CHARGE_PUMP_MODE_TRISTATE);
		if (ret < 0)
			return ret;

		ret = ad9528_spi_write_n(dev, AD9528_SYSREF_RESAMPLE_CTRL, 0x1);
		if (ret < 0)
			return ret;

		dev->pdata->sysref_src = SYSREF_SRC_EXTERNAL;

		dev->ad9528_st.vco_out_freq[AD9528_VCO] = dev->pdata->vcxo_freq;
		dev->ad9528_st.vco_out_freq[AD9528_VCXO] = dev->pdata->vcxo_freq;
		dev->ad9528_st.vco_out_freq[AD9528_SYSREF] = dev->pdata->vcxo_freq;

		goto pll2_bypassed;
	}

	pll2_ndiv = dev->pdata->pll2_vco_div_m1 * dev->pdata->pll2_n2_div;
	if (!ad9528_pll2_valid_calib_div(pll2_ndiv)) {
		printf("Feedback calibration divider value (%u) out of range\n", pll2_ndiv);
		return -1;
	}

	pll2_ndiv_a_cnt = pll2_ndiv % 4;
	pll2_ndiv_b_cnt = pll2_ndiv / 4;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_CHARGE_PUMP,
				 AD9528_PLL2_CHARGE_PUMP_CURRENT_nA(dev->pdata->
						 pll2_charge_pump_current_nA));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_FEEDBACK_DIVIDER_AB,
				 AD9528_PLL2_FB_NDIV_A_CNT(pll2_ndiv_a_cnt) |
				 AD9528_PLL2_FB_NDIV_B_CNT(pll2_ndiv_b_cnt));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_CTRL,
				 AD9528_PLL2_CHARGE_PUMP_MODE_NORMAL |
				 AD_IF(pll2_freq_doubler_en,
				       AD9528_PLL2_FREQ_DOUBLER_EN));
	if (ret < 0)
		return ret;

	vco_freq = div_u64((uint64_t)dev->pdata->vcxo_freq *
			   (dev->pdata->pll2_freq_doubler_en ? 2 : 1) * pll2_ndiv,
			   dev->pdata->pll2_r1_div);

	vco_ctrl = AD_IF(pll2_freq_doubler_en || dev->pdata->pll2_r1_div != 1,
			 AD9528_PLL2_DOUBLER_R1_EN);
	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_VCO_CTRL,
				 vco_ctrl);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_VCO_DIVIDER,
				 AD9528_PLL2_VCO_DIV_M1(dev->pdata->
						 pll2_vco_div_m1) |
				 AD_IFE(pll2_vco_div_m1,
					0,
					AD9528_PLL2_VCO_DIV_M1_PWR_DOWN_EN));
	if (ret < 0)
		return ret;

	if (dev->pdata->pll2_vco_div_m1)
		dev->ad9528_st.vco_out_freq[AD9528_VCO] =
			vco_freq / dev->pdata->pll2_vco_div_m1;
	else
		dev->ad9528_st.vco_out_freq[AD9528_VCO] = vco_freq;

	dev->ad9528_st.vco_out_freq[AD9528_VCXO] = dev->pdata->vcxo_freq;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_R1_DIVIDER,
				 AD9528_PLL2_R1_DIV(dev->pdata->pll2_r1_div));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_N2_DIVIDER,
				 AD9528_PLL2_N2_DIV(dev->pdata->pll2_n2_div));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_LOOP_FILTER_CTRL,
				 AD9528_PLL2_LOOP_FILTER_CPOLE1(dev->pdata->cpole1) |
				 AD9528_PLL2_LOOP_FILTER_RZERO(dev->pdata->rzero) |
				 AD9528_PLL2_LOOP_FILTER_RPOLE2(dev->pdata->rpole2) |
				 AD_IF(rzero_bypass_en,
				       AD9528_PLL2_LOOP_FILTER_RZERO_BYPASS_EN));
	if (ret < 0)
		return ret;

pll2_bypassed:
	for (i = 0; i < dev->pdata->num_channels; i++) {
		chan = &dev->pdata->channels[i];

		if (chan->output_dis)
			continue;

		if (chan->channel_num < AD9528_NUM_CHAN) {
			active_mask |= (1 << chan->channel_num);
			if (chan->sync_ignore_en)
				ignoresync_mask |= (1 << chan->channel_num);

			ret = ad9528_spi_write_n(dev,
						 AD9528_CHANNEL_OUTPUT(chan->channel_num),
						 AD9528_CLK_DIST_DRIVER_MODE(chan->driver_mode) |
						 AD9528_CLK_DIST_DIV(chan->channel_divider) |
						 AD9528_CLK_DIST_DIV_PHASE(chan->divider_phase) |
						 AD9528_CLK_DIST_CTRL(chan->signal_source));
			if (ret < 0)
				return ret;
		}
	}

	ret = ad9528_spi_write_n(dev,
				 AD9528_CHANNEL_PD_EN,
				 AD9528_CHANNEL_PD_MASK(~active_mask));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_CHANNEL_SYNC_IGNORE,
				 AD9528_CHANNEL_IGNORE_MASK(ignoresync_mask));
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_SYSREF_K_DIVIDER,
				 AD9528_SYSREF_K_DIV(dev->pdata->sysref_k_div));
	if (ret < 0)
		return ret;

	sysref_ctrl = AD9528_SYSREF_PATTERN_MODE(SYSREF_PATTERN_CONTINUOUS) |
		      AD9528_SYSREF_SOURCE(dev->pdata->sysref_src);
	ret = ad9528_spi_write_n(dev,
				 AD9528_SYSREF_CTRL,
				 sysref_ctrl);
	if (ret < 0)
		return ret;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PD_EN,
				 AD9528_PD_BIAS |
				 AD_IF(pll1_bypass_en, AD9528_PD_PLL1) |
				 AD_IF(pll2_bypass_en, AD9528_PD_PLL2));
	if (ret < 0)
		return ret;

	ret = ad9528_io_update(dev);
	if (ret < 0)
		return ret;

	if (!dev->pdata->pll2_bypass_en) {
		ret = ad9528_spi_write_n(dev,
					 AD9528_PLL2_VCO_CTRL,
					 vco_ctrl | AD9528_PLL2_VCO_CALIBRATE);
		if (ret < 0)
			return ret;

		ret = ad9528_io_update(dev);
		if (ret < 0)
			return ret;

		ret = ad9528_poll(dev,
				  AD9528_READBACK,
				  AD9528_IS_CALIBRATING,
				  0);
		if (ret < 0)
			return ret;
	}

	sysref_ctrl |= AD9528_SYSREF_PATTERN_REQ;
	ret = ad9528_spi_write_n(dev,
				 AD9528_SYSREF_CTRL,
				 sysref_ctrl);
	if (ret < 0)
		return ret;

	if (dev->pdata->stat0_pin_func_sel != 0xFF) {
		ret = ad9528_spi_write_n(dev, AD9528_STAT_MON0,
					 dev->pdata->stat0_pin_func_sel);
		if (ret < 0)
			return ret;

		stat_en_mask |= AD9528_STAT0_PIN_EN;
	}

	if (dev->pdata->stat1_pin_func_sel != 0xFF) {
		ret = ad9528_spi_write_n(dev, AD9528_STAT_MON1,
					 dev->pdata->stat1_pin_func_sel);
		if (ret < 0)
			return ret;

		stat_en_mask |= AD9528_STAT1_PIN_EN;
	}

	if (stat_en_mask) {
		ret = ad9528_spi_write_n(dev, AD9528_STAT_PIN_EN,
					 stat_en_mask);
		if (ret < 0)
			return ret;
	}

	ret = ad9528_io_update(dev);
	if (ret < 0)
		return ret;

	ret = ad9528_sync(dev);
	if (ret < 0)
		return ret;

	*device = dev;

	return 0;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad9528_setup().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad9528_remove(struct ad9528_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Calculate the output channel divider.
 *
 * @param rate - The desired rate.
 * @param parent_rate - The parent rate.
 *
 * @return The output divider.
 *******************************************************************************/
uint32_t ad9528_calc_out_div(uint32_t rate,
			     uint32_t parent_rate)
{
	uint32_t div;

	div = DIV_ROUND_CLOSEST(parent_rate, rate);

	div = clamp_t(unsigned int,
		      div,
		      AD9528_CLK_DIST_DIV_MIN,
		      AD9528_CLK_DIST_DIV_MAX);

	return div;
}

/***************************************************************************//**
 * @brief Calculate closest possible rate.
 *
 * @param dev - is a pointer to the ad9528_dev data structure.
 * @param chan - The output channel.
 * @param rate - The desired rate.
 *
 * @return The closest possible rate of desired rate.
 *******************************************************************************/
uint32_t ad9528_clk_round_rate(struct ad9528_dev *dev, uint32_t chan,
			       uint32_t rate)
{
	uint32_t div;
	uint32_t signal_source;
	uint32_t freq;

	if (chan >= dev->pdata->num_channels)
		return -1;

	signal_source = dev->pdata->channels[chan].signal_source;

	if (signal_source == AD9528_VCO) {
		freq = dev->ad9528_st.vco_out_freq[signal_source];
		div = ad9528_calc_out_div(rate, freq);
	} else if (signal_source == AD9528_SYSREF) {
		freq = dev->ad9528_st.vco_out_freq[AD9528_VCXO] / 2;
		div = DIV_ROUND_CLOSEST(freq, rate);
		div = clamp_t(unsigned int,
			      div,
			      AD9528_SYSREF_K_DIV_MIN,
			      AD9528_SYSREF_K_DIV_MAX);
	} else {
		// oops, it seems channels were misconfigured.
		return 0;
	}

	return DIV_ROUND_CLOSEST(freq, div);
}

/***************************************************************************//**
 * @brief Set channel rate.
 *
 * @param dev - is a pointer to the ad9528_dev data structure.
 * @param chan - Channel number.
 * @param rate - Channel rate in Hz.
 *
 * @return 0 in case of success, negative error code otherwise.
 *******************************************************************************/
int32_t ad9528_clk_set_rate(struct ad9528_dev *dev, uint32_t chan,
			    uint32_t rate)
{
	uint32_t div;
	int32_t ret;
	uint32_t reg;
	uint32_t signal_source;

	if (chan >= dev->pdata->num_channels)
		return -1;

	signal_source = dev->pdata->channels[chan].signal_source;

	// if the channel has VCO as source then operate on the channel divider
	if (signal_source == AD9528_VCO) {
		div = ad9528_calc_out_div(rate, dev->ad9528_st.vco_out_freq[signal_source]);
		dev->pdata->channels[chan].channel_divider = div;

		// make a copy of the channel control register.
		ret = ad9528_spi_read_n(dev,
					AD9528_CHANNEL_OUTPUT(chan),
					&reg);

		if (ret < 0)
			return ret;

		// set the divide ratio bits.
		uint32_t divmask = AD9528_CLK_DIST_DIV(0xFF + 1);

		// clear those bits in the channel register copy.
		reg &= ~divmask;

		// apply the new channel divider to the register copy.
		reg |= AD9528_CLK_DIST_DIV(dev->pdata->channels[chan].channel_divider);

		// apply the new channel divider to hardware.
		ret = ad9528_spi_write_n(dev,
					 AD9528_CHANNEL_OUTPUT(dev->pdata->channels[chan].channel_num),
					 reg);
		if (ret < 0)
			return ret;
	}
	// if the channel has SYSREF as source then operate on the sysref K divider
	// note that this affects all other SYSREF sourced channels
	else if (signal_source == AD9528_SYSREF) {
		// SYSREF Generator is sourced from VCXO with a fixed divider of 2 and a K divider
		div = DIV_ROUND_CLOSEST(dev->ad9528_st.vco_out_freq[AD9528_VCXO] / 2, rate);

		div = clamp_t(unsigned int,
			      div,
			      AD9528_SYSREF_K_DIV_MIN,
			      AD9528_SYSREF_K_DIV_MAX);

		// apply the new K divider to hardware.
		reg = div;
		ret = ad9528_spi_write_n(dev,
					 AD9528_SYSREF_K_DIVIDER,
					 reg);

		if(ret < 0)
			return ret;
		else {
			dev->pdata->sysref_k_div = div;
			dev->ad9528_st.vco_out_freq[AD9528_SYSREF] =
				dev->ad9528_st.vco_out_freq[AD9528_VCXO] / 2 / div;
		}
	} else {
		// oops, it seems channels were misconfigured.
		return -2;
	}

	ret = ad9528_io_update(dev);
	if (ret < 0)
		return ret;

	return ret;
}

/***************************************************************************//**
 * @brief Performs a hard reset on the AD9528.
 *
 * @param dev is a pointer to the ad9528_dev data structure.
 *
 * @return Returns 0 for success, negative value for failure.
 *******************************************************************************/
int32_t ad9528_reset(struct ad9528_dev *dev)
{
	int32_t s;

	if(!dev)
		return -1;

	if(dev->gpio_resetb) {
		s = gpio_direction_output(dev->gpio_resetb, 0);
		if(s < 0)
			return s;

		mdelay(100);

		s = gpio_direction_output(dev->gpio_resetb, 1);
		if(s < 0)
			return s;
		mdelay(100);
	}

	s = ad9528_spi_write_n(dev,
			       AD9528_SERIAL_PORT_CONFIG,
			       AD9528_SER_CONF_SOFT_RESET |
			       (dev->pdata->spi3wire ? 0 :
				AD9528_SER_CONF_SDO_ACTIVE));
	if (s < 0)
		return s;

	mdelay(100);

	s = ad9528_spi_write_n(dev, AD9528_SERIAL_PORT_CONFIG_B, 0x00);
	if(s < 0)
		return s;

	mdelay(100);

	return 0;
}
