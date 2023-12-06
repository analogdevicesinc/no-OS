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
#include <limits.h>
#include <stdio.h>
#include "no_os_print_log.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_clk.h"
#include "ad9528.h"
#include "jesd204.h"

struct ad9528_jesd204_priv {
	struct ad9528_dev *device;
};

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
	ret |= no_os_spi_write_and_read(dev->spi_desc,
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
	ret |= no_os_spi_write_and_read(dev->spi_desc,
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
		ret |= no_os_spi_write_and_read(dev->spi_desc,
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
		ret |= no_os_spi_write_and_read(dev->spi_desc,
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
		no_os_mdelay(1);
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
	init_param->pdata->jdev_max_sysref_freq = INT_MAX;
	init_param->pdata->jdev_desired_sysref_freq = 0;
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
		(&init_param->pdata->channels[i])->output_dis = 1;
		(&init_param->pdata->channels[i])->
		driver_mode = DRIVER_MODE_LVDS;
		(&init_param->pdata->channels[i])->signal_source = SOURCE_VCO;
		(&init_param->pdata->channels[i])->divider_phase = 0;
		(&init_param->pdata->channels[i])->channel_divider = 1;
	}

	return(0);
}

/**
 * @brief Get the current frequency of the clock.
 *
 * @param desc - The CLK descriptor.
 * @param rate - The CLK rate.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9528_clk_recalc_rate(struct no_os_clk_desc *desc,
			       uint64_t *rate)
{
	struct ad9528_dev *ad9528_dev;
	uint32_t reg_val, val;
	uint8_t	 s_source;
	int ret;

	ad9528_dev = desc->dev_desc;

	if(!ad9528_dev)
		return -ENODEV;

	s_source = ad9528_dev->pdata->channels[desc->hw_ch_num].signal_source;

	val = ad9528_dev->ad9528_st.vco_out_freq[s_source];
	if (s_source != AD9528_SYSREF) {
		ret = ad9528_spi_read_n(ad9528_dev,
					AD9528_CHANNEL_OUTPUT(desc->hw_ch_num),
					&reg_val);
		if (ret)
			return ret;
		val /= AD9528_CLK_DIST_DIV_REV(reg_val);
	}

	*rate = val;

	return 0;
}

/**
 * @brief Set the clock rate.
 *
 * @param desc - The CLK descriptor.
 * @param rate - The desired rate.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad9528_clk_set_rate_no_os(struct no_os_clk_desc *desc, uint64_t rate)
{
	return ad9528_clk_set_rate(desc->dev_desc, desc->hw_ch_num,
				   (uint32_t)rate);
}

/**
 * @brief Calculate closest possible rate.
 *
 * @param desc - The CLK descriptor.
 * @param rate - The desired rate.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad9528_clk_round_rate_no_os(struct no_os_clk_desc *desc,
				       uint64_t rate, uint64_t *rounded_rate)
{
	*rounded_rate = ad9528_clk_round_rate(desc->dev_desc, desc->hw_ch_num,
					      rate);

	if (*rounded_rate <= 0)
		return -EINVAL;

	return 0;
}

/**
 * @brief Enable clock.
 *
 * @param desc - The CLK descriptor.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad9528_clk_enable(struct no_os_clk_desc *desc)
{
	uint32_t reg_val;
	int ret;

	ret = ad9528_spi_read_n(desc->dev_desc,
				AD9528_CHANNEL_PD_EN,
				&reg_val);
	if (ret)
		return ret;

	reg_val &= ~NO_OS_BIT(desc->hw_ch_num);

	ret = ad9528_spi_write_n(desc->dev_desc,
				 AD9528_CHANNEL_PD_EN,
				 reg_val);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Disable clock.
 *
 * @param desc - The CLK descriptor.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad9528_clk_disable(struct no_os_clk_desc *desc)
{
	uint32_t reg_val;
	int ret;

	ret = ad9528_spi_read_n(desc->dev_desc,
				AD9528_CHANNEL_PD_EN,
				&reg_val);
	if (ret)
		return ret;

	reg_val |= NO_OS_BIT(desc->hw_ch_num);

	ret = ad9528_spi_write_n(desc->dev_desc,
				 AD9528_CHANNEL_PD_EN,
				 reg_val);
	if (ret)
		return ret;

	return 0;
}

static const struct no_os_clk_platform_ops ad9528_no_os_clk_ops = {
	.clk_recalc_rate = &ad9528_clk_recalc_rate,
	.clk_enable = &ad9528_clk_enable,
	.clk_disable = &ad9528_clk_disable,
	.clk_round_rate = &ad9528_clk_round_rate_no_os,
	.clk_set_rate = &ad9528_clk_set_rate_no_os,
};

static int ad9528_lmfc_lemc_validate(struct ad9528_dev *dev, uint64_t dividend,
				     uint32_t divisor)
{
	uint32_t rem, rem_l, rem_u, gcd_val, min;

	if (divisor > dividend) {
		uint32_t best_num, best_den;

		no_os_rational_best_approximation(dividend, divisor,
						  65535, 65535, &best_num, &best_den);

		divisor /= best_den;
	}

	gcd_val = no_os_greatest_common_divisor(dividend, divisor);
	min = NO_OS_DIV_ROUND_CLOSEST(dev->ad9528_st.sysref_src_pll2, 65535UL);

	if (gcd_val >= min) {
		pr_debug(
			"%s: dividend=%llu divisor=%u GCD=%u (st->sysref_src_pll2=%lu, min=%u)",
			__func__, dividend, divisor, gcd_val, dev->ad9528_st.sysref_src_pll2, min);

		dev->jdev_lmfc_lemc_gcd = gcd_val;
		return 0;
	}

	no_os_div_u64_rem(dev->ad9528_st.sysref_src_pll2, divisor, &rem);

	pr_debug(
		"%s: dividend=%llu divisor=%u GCD=%u rem=%u (dev->ad9528_st.sysref_src_pll2=%lu)",
		__func__, dividend, divisor, gcd_val, rem, dev->ad9528_st.sysref_src_pll2);

	no_os_div_u64_rem(dividend, divisor, &rem);
	no_os_div_u64_rem(dividend, divisor - 1, &rem_l);
	no_os_div_u64_rem(dividend, divisor + 1, &rem_u);

	if (rem_l > rem && rem_u > rem) {
		if (dev->jdev_lmfc_lemc_gcd)
			dev->jdev_lmfc_lemc_gcd = no_os_min(dev->jdev_lmfc_lemc_gcd, divisor);
		else
			dev->jdev_lmfc_lemc_gcd = divisor;

		return 0;
	}

	return -EINVAL;
}

static int ad9528_jesd204_link_supported(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct ad9528_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9528_dev *dev = priv->device;
	int ret;
	unsigned long rate;

	if (reason != JESD204_STATE_OP_REASON_INIT) {
		dev->jdev_lmfc_lemc_rate = 0;
		dev->jdev_lmfc_lemc_gcd = 0;

		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__, lnk->link_id,
		 jesd204_state_op_reason_str(reason));

	ret = jesd204_link_get_lmfc_lemc_rate(lnk, &rate);
	if (ret < 0)
		return ret;

	if (dev->jdev_lmfc_lemc_rate) {
		dev->jdev_lmfc_lemc_rate = no_os_min(dev->jdev_lmfc_lemc_rate, (uint32_t)rate);
		ret = ad9528_lmfc_lemc_validate(dev, dev->jdev_lmfc_lemc_gcd, rate);
	} else {
		dev->jdev_lmfc_lemc_rate = rate;
		ret = ad9528_lmfc_lemc_validate(dev, dev->ad9528_st.sysref_src_pll2, rate);
	}

	pr_debug("%s:%d link_num %u LMFC/LEMC %u/%lu gcd %u\n",
		 __func__, __LINE__, lnk->link_id, dev->jdev_lmfc_lemc_rate,
		 rate, dev->jdev_lmfc_lemc_gcd);

	if (ret && lnk->subclass != JESD204_SUBCLASS_0)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9528_jesd204_sysref(struct jesd204_dev *jdev)
{
	struct ad9528_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9528_dev *dev = priv->device;
	uint32_t val;
	int ret;

	pr_debug("%s:%d\n", __func__, __LINE__);

	if (dev->sysref_req_gpio && dev->pdata->sysref_req_en) {
		no_os_gpio_direction_output(dev->sysref_req_gpio, 1);
		no_os_mdelay(1);
		ret = no_os_gpio_direction_output(dev->sysref_req_gpio, 0);
	} else {
		ret = ad9528_spi_read_n(dev, AD9528_SYSREF_CTRL, &val);
		if (ret) {
			return ret;
		}

		val &= ~AD9528_SYSREF_PATTERN_REQ;

		ad9528_spi_write_n(dev, AD9528_SYSREF_CTRL, val);

		val |= AD9528_SYSREF_PATTERN_REQ;

		ret = ad9528_spi_write_n(dev, AD9528_SYSREF_CTRL, val);

		ad9528_io_update(dev);
	}

	return ret;
}

static int ad9528_jesd204_link_pre_setup(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct ad9528_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9528_dev *dev = priv->device;
	int ret, kdiv;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__, lnk->link_id,
		 jesd204_state_op_reason_str(reason));


	if (dev->pdata->jdev_desired_sysref_freq && (dev->jdev_lmfc_lemc_gcd %
			dev->pdata->jdev_desired_sysref_freq == 0)) {
		dev->jdev_lmfc_lemc_gcd = dev->pdata->jdev_desired_sysref_freq;
	} else {
		while ((dev->jdev_lmfc_lemc_gcd >
			dev->pdata->jdev_max_sysref_freq) &&
		       (dev->jdev_lmfc_lemc_gcd %
			(dev->jdev_lmfc_lemc_gcd >> 1) == 0))
			dev->jdev_lmfc_lemc_gcd >>= 1;
	}

	kdiv = NO_OS_DIV_ROUND_CLOSEST(dev->ad9528_st.sysref_src_pll2,
				       dev->jdev_lmfc_lemc_gcd);
	kdiv = no_os_clamp_t(uint32_t, kdiv, 1UL, 65535UL);

	ret = ad9528_spi_write_n(dev, AD9528_SYSREF_K_DIVIDER,
				 AD9528_SYSREF_K_DIV(kdiv));

	if (!ret)
		dev->ad9528_st.vco_out_freq[AD9528_SYSREF] =
			NO_OS_DIV_ROUND_CLOSEST(dev->ad9528_st.sysref_src_pll2, kdiv);

	ad9528_io_update(dev);

	if (dev->sysref_req_gpio && dev->pdata->sysref_req_en &&
	    dev->pdata->sysref_pattern_mode == SYSREF_PATTERN_CONTINUOUS)
		no_os_gpio_direction_output(dev->sysref_req_gpio, 1);

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9528_jesd204_clks_sync(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason)
{
	struct ad9528_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9528_dev *dev = priv->device;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	ret = ad9528_sync(dev);
	if (ret)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_ad9528_init = {
	.sysref_cb = ad9528_jesd204_sysref,
	.state_ops = {
		[JESD204_OP_LINK_SUPPORTED] = {
			.per_link = ad9528_jesd204_link_supported,
		},
		[JESD204_OP_LINK_PRE_SETUP] = {
			.per_link = ad9528_jesd204_link_pre_setup,
		},
		[JESD204_OP_CLK_SYNC_STAGE1] = {
			.per_device = ad9528_jesd204_clks_sync,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},
};

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
	struct ad9528_jesd204_priv *priv;
	struct ad9528_channel_spec *chan;
	uint32_t active_mask = 0;
	uint32_t ignoresync_mask = 0;
	uint32_t vco_freq;
	uint32_t vco_ctrl = 0;
	uint32_t sysref_ctrl;
	uint32_t stat_en_mask = 0;
	uint32_t reg_data;
	int32_t ret;
	uint32_t i, j;
	uint32_t pll2_ndiv, pll2_ndiv_a_cnt, pll2_ndiv_b_cnt;
	struct ad9528_dev *dev;
	struct no_os_clk_desc **clocks = NULL;
	struct no_os_clk_init_param clk_init;
	const char *names[AD9528_NUM_CHAN] = {
		"ad9528-1_out0", "ad9528-1_out1", "ad9528-1_out2", "ad9528-1_out3", "ad9528-1_out4",
		"ad9528-1_out5", "ad9528-1_out6", "ad9528-1_out7", "ad9528-1_out8", "ad9528-1_out9",
		"ad9528-1_out10", "ad9528-1_out11", "ad9528-1_out12", "ad9528-1_out13"
	};

	dev = (struct ad9528_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	if (init_param.export_no_os_clk) {
		clocks = no_os_calloc(AD9528_NUM_CHAN, sizeof(struct no_os_clk_desc *));
		if (!clocks) {
			no_os_free(dev);
			return -1;
		}
		for (i = 0; i < AD9528_NUM_CHAN; i++) {
			clocks[i] = no_os_calloc(1, sizeof(struct no_os_clk_desc));
			if (!clocks[i]) {
				for (j = 0; j < i; j++) {
					no_os_free(clocks[j]);
				}
				return -1;
			}
			/* Initialize clk component */
			clk_init.name = names[i];
			clk_init.hw_ch_num = i;
			clk_init.platform_ops = &ad9528_no_os_clk_ops;
			clk_init.dev_desc = dev;

			ret = no_os_clk_init(&clocks[i], &clk_init);
			if (ret) {
				for (j = 0; j < i; j++) {
					no_os_free(clocks[j]);
				}
				return ret;
			}
		}
	}

	dev->clk_desc = clocks;

	dev->pdata = init_param.pdata;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret < 0)
		goto error;

	/* GPIO */
	ret = no_os_gpio_get_optional(&dev->gpio_resetb, init_param.gpio_resetb);
	if (ret < 0)
		goto error;

	ad9528_reset(dev);

	ret = ad9528_spi_write_n(dev,
				 AD9528_SERIAL_PORT_CONFIG_B,
				 AD9528_SER_CONF_READ_BUFFERED);
	if (ret < 0)
		goto error;

	ret = ad9528_io_update(dev);
	if (ret < 0)
		goto error;

	ret = ad9528_spi_read_n(dev,
				AD9528_CHIP_ID,
				&reg_data);
	if (ret < 0)
		goto error;

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
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL1_REF_B_DIVIDER,
				 dev->pdata->refb_r_div);
	if (ret < 0)
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL1_FEEDBACK_DIVIDER,
				 dev->pdata->pll1_feedback_div);
	if (ret < 0)
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL1_CHARGE_PUMP_CTRL,
				 AD_IFE(pll1_bypass_en,
					AD9528_PLL1_CHARGE_PUMP_TRISTATE,
					AD9528_PLL1_CHARGE_PUMP_CURRENT_nA(dev->pdata->
							pll1_charge_pump_current_nA) |
					AD9528_PLL1_CHARGE_PUMP_MODE_NORMAL |
					AD9528_PLL1_CHARGE_PUMP_AUTO_TRISTATE_DIS));
	if (ret < 0)
		goto error;

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
				 AD9528_PLL1_REF_MODE(dev->pdata->ref_mode) |
				 AD9528_PLL1_OSC_CTRL_FAIL_VCC_BY2_EN);
	if (ret < 0)
		goto error;

	/*
	 * PLL2 Setup
	 */

	if (dev->pdata->pll2_bypass_en) {
		ret = ad9528_spi_write_n(dev, AD9528_PLL2_CTRL,
					 AD9528_PLL2_CHARGE_PUMP_MODE_TRISTATE);
		if (ret < 0)
			goto error;

		ret = ad9528_spi_write_n(dev, AD9528_SYSREF_RESAMPLE_CTRL, 0x1);
		if (ret < 0)
			goto error;

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
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_FEEDBACK_DIVIDER_AB,
				 AD9528_PLL2_FB_NDIV_A_CNT(pll2_ndiv_a_cnt) |
				 AD9528_PLL2_FB_NDIV_B_CNT(pll2_ndiv_b_cnt));
	if (ret < 0)
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_CTRL,
				 AD9528_PLL2_CHARGE_PUMP_MODE_NORMAL |
				 AD_IF(pll2_freq_doubler_en,
				       AD9528_PLL2_FREQ_DOUBLER_EN));
	if (ret < 0)
		goto error;

	vco_freq = no_os_div_u64((uint64_t)dev->pdata->vcxo_freq *
				 (dev->pdata->pll2_freq_doubler_en ? 2 : 1) * pll2_ndiv,
				 dev->pdata->pll2_r1_div);

	vco_ctrl = AD_IF(pll2_freq_doubler_en || dev->pdata->pll2_r1_div != 1,
			 AD9528_PLL2_DOUBLER_R1_EN);
	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_VCO_CTRL,
				 vco_ctrl);
	if (ret < 0)
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_VCO_DIVIDER,
				 AD9528_PLL2_VCO_DIV_M1(dev->pdata->
						 pll2_vco_div_m1) |
				 AD_IFE(pll2_vco_div_m1,
					0,
					AD9528_PLL2_VCO_DIV_M1_PWR_DOWN_EN));
	if (ret < 0)
		goto error;

	if (dev->pdata->pll2_vco_div_m1)
		dev->ad9528_st.vco_out_freq[AD9528_VCO] =
			vco_freq / dev->pdata->pll2_vco_div_m1;
	else
		dev->ad9528_st.vco_out_freq[AD9528_VCO] = vco_freq;

	dev->ad9528_st.vco_out_freq[AD9528_VCXO] = dev->pdata->vcxo_freq;

	dev->ad9528_st.sysref_src_pll2 = vco_freq / (pll2_ndiv * 2);

	dev->ad9528_st.vco_out_freq[AD9528_SYSREF] = dev->ad9528_st.sysref_src_pll2 /
			dev->pdata->sysref_k_div;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_R1_DIVIDER,
				 AD9528_PLL2_R1_DIV(dev->pdata->pll2_r1_div));
	if (ret < 0)
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_N2_DIVIDER,
				 AD9528_PLL2_N2_DIV(dev->pdata->pll2_n2_div));
	if (ret < 0)
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PLL2_LOOP_FILTER_CTRL,
				 AD9528_PLL2_LOOP_FILTER_CPOLE1(dev->pdata->cpole1) |
				 AD9528_PLL2_LOOP_FILTER_RZERO(dev->pdata->rzero) |
				 AD9528_PLL2_LOOP_FILTER_RPOLE2(dev->pdata->rpole2) |
				 AD_IF(rzero_bypass_en,
				       AD9528_PLL2_LOOP_FILTER_RZERO_BYPASS_EN));
	if (ret < 0)
		goto error;

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
				goto error;
		}
	}

	ret = ad9528_spi_write_n(dev,
				 AD9528_CHANNEL_PD_EN,
				 AD9528_CHANNEL_PD_MASK(~active_mask));
	if (ret < 0)
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_CHANNEL_SYNC_IGNORE,
				 AD9528_CHANNEL_IGNORE_MASK(ignoresync_mask));
	if (ret < 0)
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_SYSREF_K_DIVIDER,
				 AD9528_SYSREF_K_DIV(dev->pdata->sysref_k_div));
	if (ret < 0)
		goto error;

	sysref_ctrl = AD9528_SYSREF_PATTERN_MODE(dev->pdata->sysref_pattern_mode) |
		      AD9528_SYSREF_SOURCE(dev->pdata->sysref_src) |
		      AD9528_SYSREF_NSHOT_MODE(dev->pdata->sysref_nshot_mode) |
		      AD9528_SYSREF_PATTERN_TRIGGER_CTRL(dev->pdata->sysref_req_trigger_mode) |
		      (dev->pdata->sysref_req_en ? AD9528_SYSREF_REQUEST_BY_PIN : 0);
	ret = ad9528_spi_write_n(dev,
				 AD9528_SYSREF_CTRL,
				 sysref_ctrl);
	if (ret < 0)
		goto error;

	ret = ad9528_spi_write_n(dev,
				 AD9528_PD_EN,
				 AD9528_PD_BIAS |
				 AD_IF(pll1_bypass_en, AD9528_PD_PLL1) |
				 AD_IF(pll2_bypass_en, AD9528_PD_PLL2));
	if (ret < 0)
		goto error;

	ret = ad9528_io_update(dev);
	if (ret < 0)
		goto error;

	if (!dev->pdata->pll2_bypass_en) {
		ret = ad9528_spi_write_n(dev,
					 AD9528_PLL2_VCO_CTRL,
					 vco_ctrl | AD9528_PLL2_VCO_CALIBRATE);
		if (ret < 0)
			goto error;

		ret = ad9528_io_update(dev);
		if (ret < 0)
			goto error;

		ret = ad9528_poll(dev,
				  AD9528_READBACK,
				  AD9528_IS_CALIBRATING,
				  0);
		if (ret < 0)
			goto error;
	}

	sysref_ctrl |= AD9528_SYSREF_PATTERN_REQ;
	ret = ad9528_spi_write_n(dev,
				 AD9528_SYSREF_CTRL,
				 sysref_ctrl);
	if (ret < 0)
		goto error;

	if (dev->pdata->stat0_pin_func_sel != 0xFF) {
		ret = ad9528_spi_write_n(dev, AD9528_STAT_MON0,
					 dev->pdata->stat0_pin_func_sel);
		if (ret < 0)
			goto error;

		stat_en_mask |= AD9528_STAT0_PIN_EN;
	}

	if (dev->pdata->stat1_pin_func_sel != 0xFF) {
		ret = ad9528_spi_write_n(dev, AD9528_STAT_MON1,
					 dev->pdata->stat1_pin_func_sel);
		if (ret < 0)
			goto error;

		stat_en_mask |= AD9528_STAT1_PIN_EN;
	}

	if (stat_en_mask) {
		ret = ad9528_spi_write_n(dev, AD9528_STAT_PIN_EN,
					 stat_en_mask);
		if (ret < 0)
			goto error;
	}

	ret = ad9528_io_update(dev);
	if (ret < 0)
		goto error;

	ret = ad9528_sync(dev);
	if (ret < 0)
		goto error;

	ret = jesd204_dev_register(&dev->jdev, &jesd204_ad9528_init);
	if (ret < 0)
		goto error;
	priv = jesd204_dev_priv(dev->jdev);
	priv->device = dev;

	*device = dev;

	return 0;

error:

	for (i = 0; i < AD9528_NUM_CHAN; i++) {
		no_os_free(clocks[i]);
	}
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad9528_setup().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad9528_remove(struct ad9528_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -ENODEV;

	if (dev->gpio_resetb) {
		ret = no_os_gpio_remove(dev->gpio_resetb);
		if (ret)
			return ret;
	}

	if (dev->sysref_req_gpio) {
		ret = no_os_gpio_remove(dev->sysref_req_gpio);
		if (ret)
			return ret;
	}

	ret = jesd204_dev_unregister(dev->jdev);
	if (ret)
		return ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;
	no_os_free(dev);

	return 0;
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

	div = NO_OS_DIV_ROUND_CLOSEST(parent_rate, rate);

	div = no_os_clamp_t(unsigned int,
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
	uint32_t signal_source;
	uint32_t freq, div;

	if (chan >= dev->pdata->num_channels)
		return -1;

	signal_source = dev->pdata->channels[chan].signal_source;

	if (signal_source == AD9528_SYSREF) {
		freq = dev->ad9528_st.sysref_src_pll2;
		div = NO_OS_DIV_ROUND_CLOSEST(freq, rate);
		div = no_os_clamp_t(unsigned int,
				    div,
				    AD9528_SYSREF_K_DIV_MIN,
				    AD9528_SYSREF_K_DIV_MAX);
	} else {
		freq = dev->ad9528_st.vco_out_freq[signal_source];
		div = NO_OS_DIV_ROUND_CLOSEST(freq, rate);
		div = no_os_clamp(div, 1UL, 256UL);
	}

	return NO_OS_DIV_ROUND_CLOSEST(freq, div);
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
	uint32_t signal_source;
	uint32_t reg_val;
	uint32_t tmp;
	int ret;

	if (chan >= dev->pdata->num_channels)
		return -1;

	signal_source = dev->pdata->channels[chan].signal_source;

	if (signal_source == AD9528_SYSREF) {
		tmp = NO_OS_DIV_ROUND_CLOSEST(dev->ad9528_st.sysref_src_pll2, rate);
		tmp = no_os_clamp_t(unsigned long,
				    tmp,
				    AD9528_SYSREF_K_DIV_MIN,
				    AD9528_SYSREF_K_DIV_MAX);

		ret = ad9528_spi_write_n(dev,
					 AD9528_SYSREF_K_DIVIDER,
					 AD9528_SYSREF_K_DIV(tmp));

		if (!ret)
			dev->ad9528_st.vco_out_freq[AD9528_SYSREF] =
				NO_OS_DIV_ROUND_CLOSEST(dev->ad9528_st.sysref_src_pll2,
							tmp);

		ret = ad9528_io_update(dev);
		if (ret)
			return ret;
		goto out;
	}

	// make a copy of the channel control register.
	ret = ad9528_spi_read_n(dev,
				AD9528_CHANNEL_OUTPUT(chan),
				&reg_val);
	if (ret)
		return ret;

	tmp = NO_OS_DIV_ROUND_CLOSEST(dev->ad9528_st.vco_out_freq[signal_source], rate);
	tmp = no_os_clamp(tmp, 1, 256);

	reg_val &= ~(AD9528_CLK_DIST_CTRL_MASK | AD9528_CLK_DIST_DIV_MASK);
	reg_val |= AD9528_CLK_DIST_DIV(tmp);
	reg_val |= AD9528_CLK_DIST_CTRL(signal_source);

	ret = ad9528_spi_write_n(dev,
				 AD9528_CHANNEL_OUTPUT(chan),
				 reg_val);
	if (ret)
		return ret;

	ret = ad9528_io_update(dev);
	if (ret)
		return ret;

out:
	return 0;
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
		s = no_os_gpio_direction_output(dev->gpio_resetb, 0);
		if(s < 0)
			return s;

		no_os_mdelay(100);

		s = no_os_gpio_direction_output(dev->gpio_resetb, 1);
		if(s < 0)
			return s;
		no_os_mdelay(100);
	}

	s = ad9528_spi_write_n(dev,
			       AD9528_SERIAL_PORT_CONFIG,
			       AD9528_SER_CONF_SOFT_RESET |
			       (dev->pdata->spi3wire ? 0 :
				AD9528_SER_CONF_SDO_ACTIVE));
	if (s < 0)
		return s;

	no_os_mdelay(100);

	s = ad9528_spi_write_n(dev, AD9528_SERIAL_PORT_CONFIG_B, 0x00);
	if(s < 0)
		return s;

	no_os_mdelay(100);

	return 0;
}
