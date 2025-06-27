/***************************************************************************//**
 *   @file   adf4377.c
 *   @brief  Implementation of adf4377 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <malloc.h>
#include "adf4377.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/**
 * @struct adf4377_reg_sequence
 * @brief ADF4377 register format structure for default values
 */
struct adf4377_reg_sequence {
	uint16_t reg;
	uint8_t val;
};

/**
 * @struct adf4377_reg_defaults
 * @brief ADF4377 register initialization
 */
static const struct adf4377_reg_sequence adf4377_reg_defaults[] = {
	{ 0x00, 0x18 },
	{ 0x01, 0x00 },
	{ 0x0A, 0xA5 },
	{ 0x0F, 0x00 },
	{ 0x10, 0x28 },
	{ 0x11, 0xC0 },
	{ 0x12, 0x01 },
	{ 0x13, 0x00 },
	{ 0x14, 0x00 },
	{ 0x15, 0x0F },
	{ 0x19, 0xF0 },
	{ 0x1A, 0x00 },
	{ 0x1B, 0xAC },
	{ 0x1C, 0xC1 },
	{ 0x1F, 0xA7 },
	{ 0x20, 0x09 },
	{ 0x21, 0xD3 },
	{ 0x22, 0x32 },
	{ 0x23, 0x18 },
	{ 0x24, 0x04 },
	{ 0x25, 0xD6 },
	{ 0x26, 0x3B },
	{ 0x27, 0xA8 },
	{ 0x28, 0x61 },
	{ 0x29, 0x6A },
	{ 0x2A, 0x18 },
	{ 0x2C, 0xC0 },
	{ 0x2D, 0x4E },
	{ 0x2E, 0x83 },
	{ 0x31, 0x09 },
	{ 0x32, 0x09 },
	{ 0x33, 0x18 },
	{ 0x34, 0x08},
	{ 0x3A, 0x5D },
	{ 0x3B, 0x2B },
	{ 0x42, 0x05 },
	{ 0x10, 0x28 },
};

/**
 * @brief Writes data to ADF4377 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adf4377_spi_write(struct adf4377_dev *dev, uint8_t reg_addr,
		      uint8_t data)
{
	uint8_t buff[ADF4377_BUFF_SIZE_BYTES];
	uint16_t cmd;

	if (!dev)
		return -EINVAL;

	cmd = ADF4377_SPI_WRITE_CMD | reg_addr;
	if (dev->spi_desc->bit_order) {
		buff[0] = no_os_bit_swap_constant_8(cmd & 0xFF);
		buff[1] = no_os_bit_swap_constant_8(cmd >> 8);
		buff[2] = no_os_bit_swap_constant_8(data);
	} else {
		buff[0] = cmd >> 8;
		buff[1] = cmd & 0xFF;
		buff[2] = data;
	}

	return no_os_spi_write_and_read(dev->spi_desc, buff,
					ADF4377_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ADF4377 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adf4377_spi_read(struct adf4377_dev *dev, uint8_t reg_addr,
		     uint8_t *data)
{
	int32_t ret;
	uint8_t buff[ADF4377_BUFF_SIZE_BYTES];

	if (dev->spi_desc->bit_order) {
		buff[0] = no_os_bit_swap_constant_8(reg_addr);
		buff[1] = no_os_bit_swap_constant_8(ADF4377_SPI_READ_CMD);
		buff[2] = no_os_bit_swap_constant_8(ADF4377_SPI_DUMMY_DATA);
	} else {
		buff[0] = ADF4377_SPI_READ_CMD;
		buff[1] = reg_addr;
		buff[2] = ADF4377_SPI_DUMMY_DATA;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buff,
				       ADF4377_BUFF_SIZE_BYTES);
	if (ret < 0)
		return ret;

	*data = buff[2];

	return ret;
}

/**
 * @brief Update ADF4377 register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Mask for specific register bits to be updated.
 * @param data - Data written to the device (requires prior bit shifting).
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adf4377_spi_update_bit(struct adf4377_dev *dev, uint16_t reg_addr,
			   uint8_t mask, uint8_t data)
{
	uint8_t orig, tmp;
	int32_t ret;

	ret = adf4377_spi_read(dev, reg_addr, &orig);
	if (ret < 0)
		return ret;
	tmp = orig & ~mask;
	tmp |= data & mask;

	if (tmp != orig)
		return adf4377_spi_write(dev, reg_addr, tmp);

	return 0;
}

/**
 * @brief ADF4377 SPI Scratchpad check.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int adf4377_check_scratchpad(struct adf4377_dev *dev)
{
	uint8_t scratchpad;
	int ret;

	ret = adf4377_spi_write(dev, 0x0A, ADF4377_SPI_SCRATCHPAD_TEST_A);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_read(dev, ADF4377_REG(0x0A), &scratchpad);
	if (ret < 0)
		return ret;

	if (scratchpad != ADF4377_SPI_SCRATCHPAD_TEST_A)
		return -EINVAL;

	return 0;
}

/**
 * @brief Set the desired reference frequency and reset everything over to maximum
 * supported value of 5GHz to the max. value and everything under the minimum
 * supported value of 10MHz to the min. value.
 * @param dev 		- The device structure.
 * @param val		- The desired reference frequency in Hz.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_set_ref_clk(struct adf4377_dev *dev, uint64_t val)
{
	if (!dev)
		return -EINVAL;

	dev->clkin_freq = val;

	if (val > ADF4377_CLKIN_REF_MAX)
		dev->clkin_freq = ADF4377_CLKIN_REF_MAX;

	if (val < ADF4377_CLKIN_REF_MIN)
		dev->clkin_freq = ADF4377_CLKIN_REF_MIN;

	return adf4377_set_freq(dev);
}

/**
 * @brief Gets the user proposed reference frequency
 * @param dev 		- The device structure.
 * @param val		- The set value of the reference frequency in Hz.
 * @return    		-  0 in case of success or negative error code.
 */
int adf4377_get_ref_clk(struct adf4377_dev *dev, uint64_t *val)
{
	if (!dev)
		return -EINVAL;

	*val = dev->clkin_freq;

	return 0;
}

/**
 * @brief Set the charge pump value which will be written to the register. The
 * value will be between 0 and 15 on 8 bits. For more information please
 * consult the Datasheet.
 * @param dev 		- The device structure.
 * @param reg_val 	- The desired charge pump register value.
 * @return    		- Result of the writing procedure, error code otherwise.
 */
int adf4377_set_cp_i(struct adf4377_dev *dev, int32_t reg_val)
{
	if (!dev)
		return -EINVAL;

	dev->cp_i = (uint8_t)reg_val;

	if (reg_val > ADF4377_CPI_VAL_MAX)
		dev->cp_i = ADF4377_CPI_VAL_MAX;

	return adf4377_spi_update_bit(dev, 0x15, ADF4377_CP_I_MSK, dev->cp_i);
}

/**
 * @brief Gets the charge pump value from the register. The value will be
 * between 0 and 15 on 8 bits. For more information please consult the
 * Datasheet.
 * @param dev 		- The device structure.
 * @param reg_val 	- The read charge pump register value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_get_cp_i(struct adf4377_dev *dev, int32_t *reg_val)
{
	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev, 0x15, &tmp);
	if (ret)
		return ret;

	dev->cp_i = no_os_field_get(ADF4377_CP_I_MSK, tmp);
	*reg_val = dev->cp_i;

	return 0;
}

/**
 * @brief Set the reference divider value and reset everything over to maximum
 * supported value of 63 to the max. value
 * @param dev 		- The device structure.
 * @param div	 	- The reference divider value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_set_ref_div(struct adf4377_dev *dev, int32_t div)
{
	if (!dev)
		return -EINVAL;

	dev->ref_div_factor = div;

	if (div > ADF4377_REF_DIV_MAX)
		dev->ref_div_factor = ADF4377_REF_DIV_MAX;

	return adf4377_set_freq(dev);
}

/**
 * @brief Gets the value the reference divider.
 * @param dev 		- The device structure.
 * @param div	 	- The read reference divider value.
 * @return    		-  0 in case of success or negative error code.
 */
int adf4377_get_ref_div(struct adf4377_dev *dev, int32_t *div)
{
	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev, 0x12, &tmp);
	if (ret)
		return ret;

	dev->ref_div_factor = no_os_field_get(ADF4377_R_DIV_MSK, tmp);
	*div = dev->ref_div_factor;

	return 0;
}

/**
 * @brief Set the reference doubler to enable or disable based on the passed
 * parameter. If the parameter is different then 0 it will set the doubler to
 * enable.
 * @param dev 		- The device structure.
 * @param en	 	- The enable or disable value of the reference doubler.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_set_en_ref_doubler(struct adf4377_dev *dev, bool en)
{
	if (!dev)
		return -EINVAL;

	dev->ref_doubler_en = en;

	return adf4377_set_freq(dev);
}

/**
 * @brief Gets the value the doubler if it is enabled or disable and stores it
 * it the dev structure.
 * @param dev 		- The device structure.
 * @param en		- The read value of the reference doubler.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_get_en_ref_doubler(struct adf4377_dev *dev, bool *en)
{
	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev, 0x11, &tmp);
	if (ret)
		return ret;

	dev->ref_doubler_en = no_os_field_get(tmp, ADF4377_EN_RDBLR_MSK);
	*en = dev->ref_doubler_en;

	return 0;
}

/**
 * @brief Computes the PFD frequency and returns the value in Hz.
 * @param dev 	     - The device structure.
 * @return 	     - PFD value in Hz.
 */
static uint64_t adf4377_pfd_compute(struct adf4377_dev *dev)
{
	uint64_t pfd_freq;

	pfd_freq = NO_OS_DIV_ROUND_CLOSEST(dev->clkin_freq, dev->ref_div_factor);
	if (dev->ref_doubler_en)
		pfd_freq *= 2;

	return pfd_freq;
}

/**
 * @brief Software reset and loads default register values to bring the part to
 * a known state.
 * @param dev 		- The device structure.
 * @param spi_4wire	- The SPI interface mode.
 * @return 		- Returns 0 in case of success or negative error code.
 */
int adf4377_soft_reset(struct adf4377_dev *dev, bool spi_4wire)
{
	uint8_t i;
	int ret;

	if (!dev)
		return -EINVAL;

	/* Software Reset */
	ret = adf4377_spi_write(dev, 0x00, ADF4377_RESET_CMD);
	if (ret < 0)
		return ret;

	no_os_udelay(ADF4377_POR_DELAY_US);

	/* SPI Configuration */
	ret = adf4377_spi_write(dev, 0x00,
				ADF4377_SPI_4W_CFG(spi_4wire) |
				ADF4377_SPI_LSB_CFG(dev->spi_desc->bit_order));
	if (ret)
		return ret;

	/* Load Default Registers */
	for (i = 0; i < NO_OS_ARRAY_SIZE(adf4377_reg_defaults); i++) {
		ret = adf4377_spi_write(dev,
					adf4377_reg_defaults[i].reg,
					adf4377_reg_defaults[i].val);
		if (ret)
			return ret;
	}
	return 0;
}

/**
 * @brief Set the bleed word, which represents the value of the bleed current
 * written to the register space.
 * @param dev 		- The device structure.
 * @param word	 	- The bleed current register value.
 * @return    		- Result of the writing procedure, error code otherwise.
 */
int adf4377_set_bleed_word(struct adf4377_dev *dev, int32_t word)
{
	uint8_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	dev->bleed_word = (uint16_t)word;

	if (word > ADF4377_BLEED_WORD_MAX)
		dev->bleed_word = ADF4377_BLEED_WORD_MAX;

	val = dev->bleed_word & ADF4377_BLEED_I_LSB_MSK;
	ret = adf4377_spi_update_bit(dev, 0x15, ADF4377_BLEED_I_LSB_MSK,
				     no_os_field_prep(ADF4377_BLEED_I_LSB_MSK,
						     val));
	if (ret)
		return ret;
	val = (dev->bleed_word >> 2) & ADF4377_EN_BLEED_MSK;
	return adf4377_spi_write(dev, 0x16, val);
}

/**
 * @brief Gets the value of the set bleed word.
 * @param dev 		- The device structure.
 * @param bleed_word	- The read bleed current register value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_get_bleed_word(struct adf4377_dev *dev, int32_t *bleed_word)
{
	uint8_t upper, lower;
	int ret;

	ret = adf4377_spi_read(dev, 0x1E, &upper);
	if (ret)
		return ret;
	dev->bleed_word = upper;

	ret = adf4377_spi_read(dev, 0x1F, &lower);
	if (ret)
		return ret;

	dev->bleed_word = (upper << 8) | lower;
	*bleed_word = dev->bleed_word;

	return 0;
}

/**
 * @brief Set the output channel to enable or disable based on the passed
 * parameter. If the parameter is different then 0 it will set the doubler to
 * enable.
 * @param dev 		- The device structure.
 * @param ch	 	- The channel to set state.
 * @param en	 	- The enable or disable value of the output channel.
 * @return    		- Result of the writing procedure, error code otherwise.
 */
int adf4377_set_en_chan(struct adf4377_dev *dev, uint8_t ch, bool en)
{
	uint8_t enable;

	if (!ch) {
		enable = no_os_field_prep(ADF4377_PD_CLKOUT1_MSK, !en);
		return adf4377_spi_update_bit(dev, 0x1A, ADF4377_PD_CLKOUT1_MSK,
					      enable);
	}

	enable = no_os_field_prep(ADF4377_PD_CLKOUT2_MSK, !en);
	return adf4377_spi_update_bit(dev, 0x1A, ADF4377_PD_CLKOUT2_MSK,
				      enable);
}

/**
 * @brief Gets the value the output channel if it is enabled or disable.
 * @param dev 		- The device structure.
 * @param ch	 	- The channel to get state.
 * @param en	 	- The status of the output channel.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_get_en_chan(struct adf4377_dev *dev, uint8_t ch, bool *en)
{
	uint8_t tmp;
	bool enable;
	int ret;

	ret = adf4377_spi_read(dev, 0x1A, &tmp);
	if (ret)
		return ret;

	if (!ch)
		enable = no_os_field_get(tmp, ADF4377_PD_CLKOUT1_MSK);
	else
		enable = no_os_field_get(tmp, ADF4377_PD_CLKOUT2_MSK);

	*en = !enable;

	return 0;
}

/**
 * @brief Set the output power register value of a channel and reset everything
 * over to maximum supported value of 15 to the max. value.
 * @param dev 		- The device structure.
 * @param ch	 	- The channel to set the power off.
 * @param pwr	 	- The output power register value.
 * @return    		- Result of the writing procedure, error code otherwise.
 */
int adf4377_set_out_power(struct adf4377_dev *dev, uint8_t ch, int8_t pwr)
{
	uint8_t tmp;

	if (pwr > ADF4377_OUT_PWR_MAX)
		pwr = ADF4377_OUT_PWR_MAX;

	if (!ch) {
		tmp = no_os_field_prep(ADF4377_CLKOUT1_OP_MSK, pwr);
		return adf4377_spi_update_bit(dev, 0x19, ADF4377_CLKOUT1_OP_MSK,
					      tmp);
	}

	tmp = no_os_field_prep(ADF4377_CLKOUT2_OP_MSK, pwr);
	return adf4377_spi_update_bit(dev, 0x19, ADF4377_CLKOUT2_OP_MSK, tmp);
}

/**
 * @brief Gets the output power register value.
 * @param dev 		- The device structure.
 * @param ch	 	- The channel to get the power off.
 * @param pwr	 	- The output power register value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_get_out_power(struct adf4377_dev *dev, uint8_t ch, int8_t *pwr)
{

	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev, 0x19, &tmp);
	if (ret)
		return ret;

	if (!ch)
		*pwr = no_os_field_get(ADF4377_CLKOUT1_OP_MSK, tmp);
	else
		*pwr = no_os_field_get(ADF4377_CLKOUT2_OP_MSK, tmp);

	return 0;
}

/**
 * @brief Set the rfout frequency divider register value and reset everything
 * over to maximum supported value of /128 to the max. value.
 * @param dev 	- The device structure.
 * @param div 	- The rfoutdiv divider register value.
 * @return 	- Result of the writing procedure, error code otherwise.
 */
int adf4377_set_rfout_divider(struct adf4377_dev *dev, uint8_t div)
{
	uint8_t tmp;
	if (!dev)
		return -EINVAL;

	dev->clkout_div_sel = (uint8_t) div;

	if (div > ADF4377_CLKOUT_DIV_REG_VAL_MAX)
		dev->clkout_div_sel = ADF4377_CLKOUT_DIV_REG_VAL_MAX;

	tmp = ADF4377_CLKOUT_DIV(dev->clkout_div_sel);
	return adf4377_spi_update_bit(dev, 0x12, ADF4377_CLKOUT_DIV_MSK, tmp);
}

/**
 * @brief Gets the rfout divider register value.
 * @param dev 	- The device structure.
 * @param div	- The rfoutdiv divider register value.
 * @return 	- 0 in case of success or negative error code.
 */
int adf4377_get_rfout_divider(struct adf4377_dev *dev, int8_t *div)
{

	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev, 0x12, &tmp);
	if (ret)
		return ret;

	dev->clkout_div_sel = ADF4377_CLKOUT_DIV(tmp);
	*div = dev->clkout_div_sel;

	return 0;
}

/**
 * @brief Set the desired output frequency and reset everything over to maximum
 * supported value of 12.8GHz to the max. value and everything under the minimum
 * supported value of 800MHz to the min. value.
 * @param dev 		- The device structure.
 * @param val	 	- The desired output frequency in Hz.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_set_rfout(struct adf4377_dev *dev, uint64_t val)
{
	if (!dev)
		return -EINVAL;

	dev->f_clk = val;

	if (val > ADF4377_RFOUT_MAX)
		dev->f_clk = ADF4377_RFOUT_MAX;

	if (val < ADF4377_RFOUT_MIN)
		dev->f_clk = ADF4377_RFOUT_MIN;

	return adf4377_set_freq(dev);
}

/**
 * Set the output frequency.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4377_set_freq(struct adf4377_dev *dev)
{
	int32_t ret;
	uint64_t tmp;
	uint8_t val;
	uint8_t locked;
	uint64_t vco = 0;
	uint8_t clkout_div;

	for (clkout_div = 0; clkout_div <= ADF4377_CLKOUT_DIV_REG_VAL_MAX;
	     clkout_div++) {
		tmp = (1 << clkout_div) * dev->f_clk;
		if (tmp < ADF4377_MIN_VCO_FREQ || tmp > ADF4377_MAX_VCO_FREQ)
			continue;

		vco = tmp;
		break;
	}

	dev->clkout_div_sel = clkout_div;

	if (!vco) {
		pr_err("VCO is 0\n");
		return -EINVAL;
	}

	dev->f_pfd = adf4377_pfd_compute(dev);

	dev->n_int = NO_OS_DIV_ROUND_CLOSEST(dev->f_clk, dev->f_pfd);

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x11),
				     ADF4377_EN_RDBLR_MSK | ADF4377_N_INT_MSB_MSK,
				     ADF4377_EN_RDBLR(dev->ref_doubler_en)
				     | ADF4377_N_INT_MSB(dev->n_int >> 8));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x12),
				     ADF4377_R_DIV_MSK | ADF4377_CLKOUT_DIV_MSK,
				     ADF4377_CLKOUT_DIV(dev->clkout_div_sel) |
				     ADF4377_R_DIV(dev->ref_div_factor));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x10),
				ADF4377_N_INT_LSB(dev->n_int));
	if (ret < 0)
		return ret;

	no_os_udelay(ADF4377_LKD_DELAY_US);
	ret = adf4377_spi_read(dev, 0x49, &val);
	if (ret)
		return ret;

	locked = no_os_field_get(val, ADF4377_LOCKED_MSK);
	if (!locked)
		return -EIO;

	return 0;

}

/**
 * @brief Gets the user proposed output frequency
 * @param dev 		- The device structure.
 * @param val		- The set value of the output frequency in Hz.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_get_rfout(struct adf4377_dev *dev, uint64_t *val)
{
	uint64_t freq;
	uint64_t pfd;
	uint8_t tmp;
	uint16_t n;
	int ret;

	pfd = adf4377_pfd_compute(dev);

	ret = adf4377_spi_read(dev, 0x11, &tmp);
	if (ret)
		return ret;
	n = tmp & ADF4377_N_INT_MSB_MSK;
	n = n << 8;

	ret = adf4377_spi_read(dev, 0x10, &tmp);
	if (ret)
		return ret;
	n |= tmp;

	freq = n * pfd;
	*val = freq;

	return 0;
}

/**
 * @brief Set the NDEL (N divider Delay) register value.
 * @param dev - The device structure.
 * @param val - The desired NDEL value. The value will be clamped to the
 *              maximum supported value if it exceeds the limit.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adf4377_set_ndel(struct adf4377_dev *dev, int32_t val)
{
	uint8_t tmp;

	if (!dev)
		return -EINVAL;

	tmp = (uint8_t)val;
	if (val > ADF4377_R_N_DEL_MAX)
		tmp = ADF4377_R_N_DEL_MAX;

	return adf4377_spi_update_bit(dev, 0x17, ADF4377_N_DEL_MSK, ADF4377_N_DEL(tmp));
}

/**
 * @brief Gets the value of the set NDEL (N divider Delay) register.
 * @param dev - The device structure.
 * @param val - The read NDEL register value.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adf4377_get_ndel(struct adf4377_dev *dev, int32_t *val)
{
	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev, 0x17, &tmp);
	if (ret)
		return ret;

	*val = no_os_field_get(ADF4377_N_DEL_MSK, tmp);

	return 0;
}

/**
 * @brief Set the RDEL (R divider Delay) register value.
 * @param dev - The device structure.
 * @param val - The desired RDEL value. The value will be clamped to the
 *              maximum supported value if it exceeds the limit.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adf4377_set_rdel(struct adf4377_dev *dev, int32_t val)
{
	uint8_t tmp;

	if (!dev)
		return -EINVAL;

	tmp = (uint8_t)val;
	if (val > ADF4377_R_N_DEL_MAX)
		tmp = ADF4377_R_N_DEL_MAX;

	return adf4377_spi_update_bit(dev, 0x18, ADF4377_R_DEL_MSK, ADF4377_R_DEL(tmp));
}

/**
 * @brief Gets the value of the set RDEL (R divider Delay) register.
 * @param dev - The device structure.
 * @param val - The read RDEL register value.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int adf4377_get_rdel(struct adf4377_dev *dev, int32_t *val)
{
	int32_t rdel;
	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev, 0x18, &tmp);
	if (ret)
		return ret;

	*val =  no_os_field_get(ADF4377_R_DEL_MSK, tmp);

	return 0;
}

/**
 * @brief Set the value of SR_DEL Adjustment Feature Translated to Pico Seconds
 * on the output. Reset to Max  value of 127.
 * @param dev 		- The device structure.
 * @param val	 	- The desired SR_DEL register value.
 * @return    		- Result of the writing procedure, error code otherwise.
 */
int adf4377_set_sr_del_adj(struct adf4377_dev *dev, int32_t val)
{
	uint8_t tmp;

	if (!dev)
		return -EINVAL;

	dev->sr_del_adj = (uint8_t)val;

	if (val > ADF4377_SR_DEL_MAX)
		dev->sr_del_adj = ADF4377_SR_DEL_MAX;

	tmp = ADF4377_SR_DEL(dev->sr_del_adj);
	return adf4377_spi_update_bit(dev, 0x43, ADF4377_SR_DEL_MSK, tmp);
}

/**
 * @brief Gets the value of the set SR_DEL Adjustment Feature Translated to Pico
 * Seconds on the output.
 * @param dev 		- The device structure.
 * @param val	 	- The read SR_DEL register value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_get_sr_del_adj(struct adf4377_dev *dev, int32_t *val)
{
	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev, 0x43, &tmp);
	if (ret)
		return ret;

	dev->sr_del_adj = no_os_field_get(ADF4377_SR_DEL_MSK, tmp);
	*val = dev->sr_del_adj;

	return 0;
}

/**
 * @brief Set the value of SR_INV_ADJ Adjustment to enable or disable which adds
 * a constant value to the skew adjustment output.
 * @param dev 		- The device structure.
 * @param en	 	- The enable or disable value of SR_INV.
 * @return    		- Result of the writing procedure, error code otherwise.
 */
int adf4377_set_en_sr_inv_adj(struct adf4377_dev *dev, bool en)
{
	uint8_t tmp;

	if (!dev)
		return -EINVAL;

	dev->sr_inv = en;

	tmp = ADF4377_INV_SR(dev->sr_inv);
	return adf4377_spi_update_bit(dev, 0x43, ADF4377_INV_SR_MSK, tmp);
}

/**
 * @brief Gets the value of the set SR_INV_ADJ Adjustment to enable or disable
 * which adds a constant value to the skew adjustment output.
 * @param dev 		- The device structure.
 * @param en	 	- The read value of SR_INV.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_get_en_sr_inv_adj(struct adf4377_dev *dev, bool *en)
{
	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev, 0x43, &tmp);
	if (ret)
		return ret;

	dev->sr_inv = no_os_field_get(ADF4377_INV_SR_MSK, tmp);
	*en = dev->sr_inv;

	return 0;
}

/**
 * @brief Set enable/disable sysref monitoring.
 * @param dev 		- The device structure.
 * @param en	 	- The enable or disable value of the sysref monitor.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_set_en_sysref_monitor(struct adf4377_dev *dev, bool en)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4377_spi_update_bit(dev, 0x42, ADF4377_PD_SR_MON_MSK,
				     ADF4377_PD_SR_MON(en));
	if (ret)
		return ret;

	if (en) {
		ret = adf4377_spi_update_bit(dev, 0x42, ADF4377_RST_SR_MON_MSK,
					     ADF4377_RST_SR_MON(en));
		if (ret)
			return ret;

		no_os_udelay(ADF4377_SR_MON_DELAY_US);

		ret = adf4377_spi_update_bit(dev, 0x42, ADF4377_RST_SR_MON_MSK,
					     ADF4377_RST_SR_MON(!en));
		if (ret)
			return ret;
	}
	dev->sysrefout = en;
	return 0;
}

/**
 * @brief Gets the value of the set sysref monitoring.
 * @param dev 		- The device structure.
 * @param en	 	- The read value of the sysref monitor.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4377_get_en_sysref_monitor(struct adf4377_dev *dev, bool *en)
{
	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev, 0x42, &tmp);
	if (ret)
		return ret;

	dev->sysrefout = no_os_field_get(ADF4377_PD_SR_MON_MSK, tmp);
	*en = dev->sysrefout;

	return 0;
}

/**
 * @brief Setup the device and set the frequency.
 * @param dev 	- The device structure.
 * @return 	- 0 in case of success, negative error code otherwise.
 */
static int adf4377_setup(struct adf4377_dev *dev)
{
	int32_t ret;
	uint32_t f_div_rclk;
	uint8_t dclk_div1, dclk_div2;
	bool dclk_mode_en;
	uint16_t synth_lock_timeout, vco_alc_timeout, adc_clk_div, vco_band_div;

	/*Compute PFD */
	dev->f_pfd = adf4377_pfd_compute(dev);

	f_div_rclk = dev->f_pfd;

	if (dev->f_pfd <= ADF4377_FREQ_PFD_80MHZ) {
		dclk_div1 = ADF4377_DCLK_DIV1_1;
		dclk_div2 = ADF4377_DCLK_DIV2_1;
		dclk_mode_en = false;
	} else if (dev->f_pfd <= ADF4377_FREQ_PFD_125MHZ) {
		dclk_div1 = ADF4377_DCLK_DIV1_1;
		dclk_div2 = ADF4377_DCLK_DIV2_1;
		dclk_mode_en = true;
	} else if (dev->f_pfd <= ADF4377_FREQ_PFD_160MHZ) {
		dclk_div1 = ADF4377_DCLK_DIV1_2;
		dclk_div2 = ADF4377_DCLK_DIV2_1;
		dclk_mode_en = false;
		f_div_rclk /= 2;
	} else if (dev->f_pfd <= ADF4377_FREQ_PFD_250MHZ) {
		dclk_div1 = ADF4377_DCLK_DIV1_2;
		dclk_div2 = ADF4377_DCLK_DIV2_1;
		dclk_mode_en = true;
		f_div_rclk /= 2;
	} else if (dev->f_pfd <= ADF4377_FREQ_PFD_320MHZ) {
		dclk_div1 = ADF4377_DCLK_DIV1_2;
		dclk_div2 = ADF4377_DCLK_DIV2_2;
		dclk_mode_en = false;
		f_div_rclk /= 4;
	} else {
		dclk_div1 = ADF4377_DCLK_DIV1_2;
		dclk_div2 = ADF4377_DCLK_DIV2_2;
		dclk_mode_en = true;
		f_div_rclk /= 4;
	}

	synth_lock_timeout = NO_OS_DIV_ROUND_UP(f_div_rclk, 50000);
	vco_alc_timeout = NO_OS_DIV_ROUND_UP(f_div_rclk, 20000);
	vco_band_div = NO_OS_DIV_ROUND_UP(f_div_rclk,
					  150000 * 16 * (1 << dclk_mode_en));
	adc_clk_div = NO_OS_DIV_ROUND_UP((f_div_rclk / 400000 - 2), 4);

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x1C),
				     ADF4377_EN_DNCLK_MSK | ADF4377_EN_DRCLK_MSK,
				     ADF4377_EN_DNCLK(ADF4377_EN_DNCLK_ON) |
				     ADF4377_EN_DRCLK(ADF4377_EN_DRCLK_ON));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x1D), ADF4377_MUXOUT_MSK,
				     ADF4377_MUXOUT(dev->muxout_default));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x11),
				     ADF4377_EN_AUTOCAL_MSK | ADF4377_DCLK_DIV2_MSK,
				     ADF4377_EN_AUTOCAL(ADF4377_VCO_CALIB_EN) |
				     ADF4377_DCLK_DIV2(dclk_div2));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x2E), ADF4377_EN_ADC_CNV_MSK
				     | ADF4377_EN_ADC_MSK | ADF4377_ADC_A_CONV_MSK,
				     ADF4377_EN_ADC_CNV(true) | ADF4377_EN_ADC(true) |
				     ADF4377_ADC_A_CONV(ADF4377_ADC_A_CONV_VCO_CALIB));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x20), ADF4377_EN_ADC_CLK_MSK,
				     ADF4377_EN_ADC_CLK(true));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x2F), ADF4377_DCLK_DIV1_MSK,
				     ADF4377_DCLK_DIV1(dclk_div1));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x24), ADF4377_DCLK_MODE_MSK,
				     ADF4377_DCLK_MODE(dclk_mode_en));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x27),
				ADF4377_SYNTH_LOCK_TO_LSB(synth_lock_timeout));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x28),
				     ADF4377_SYNTH_LOCK_TO_MSB_MSK,
				     ADF4377_SYNTH_LOCK_TO_MSB(synth_lock_timeout
						     >> 8));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x29),
				ADF4377_VCO_ALC_TO_LSB(vco_alc_timeout));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_update_bit(dev, ADF4377_REG(0x2A),
				     ADF4377_VCO_ALC_TO_MSB_MSK,
				     ADF4377_VCO_ALC_TO_MSB(vco_alc_timeout >> 8));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x26),
				ADF4377_VCO_BAND_DIV(vco_band_div));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x2D),
				ADF4377_ADC_CLK_DIV(adc_clk_div));
	if (ret < 0)
		return ret;

	ret = adf4377_set_freq(dev);
	if (ret < 0)
		return ret;

	return 0;
}

/**
 * @brief Initializes the ADF4377.
 * @param device 	- The device structure.
 * @param init_param 	- The structure containing the device initial parameters.
 * @return 		- Returns 0 in case of success or negative error code.
 */
int32_t adf4377_init(struct adf4377_dev **device,
		     struct adf4377_init_param *init_param)
{
	int32_t ret;
	uint8_t device_id;
	uint8_t chip_type;
	struct adf4377_dev *dev;

	dev = (struct adf4377_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret < 0)
		goto error_dev;

	dev->dev_id = init_param->dev_id;
	dev->spi4wire = init_param->spi4wire;
	dev->clkin_freq = init_param->clkin_freq;
	dev->cp_i = init_param->cp_i;
	dev->muxout_default = init_param->muxout_select;
	dev->ref_doubler_en = init_param->ref_doubler_en;
	dev->f_clk = init_param->f_clk;
	dev->clkout_op = init_param->clkout_op;
	dev->ref_div_factor = init_param->ref_div_factor;

	/* GPIO Chip Enable */
	ret = no_os_gpio_get_optional(&dev->gpio_ce, init_param->gpio_ce_param);
	if (ret < 0)
		goto error_dev;

	if (dev->gpio_ce) {
		ret = no_os_gpio_direction_output(dev->gpio_ce, NO_OS_GPIO_HIGH);
		if (ret < 0)
			goto error_gpio_ce;
	}

	/* Software Reset and load register values*/
	ret = adf4377_soft_reset(dev, dev->spi4wire);
	if (ret < 0)
		goto error_spi;

	/* Read Chip Type */
	ret = adf4377_spi_read(dev, ADF4377_REG(0x03), &chip_type);
	if (ret < 0)
		goto error_spi;

	if (chip_type != ADF4377_CHIP_TYPE)
		goto error_spi;

	/* Scratchpad Check */
	ret = adf4377_check_scratchpad(dev);
	if (ret < 0)
		goto error_spi;

	ret = adf4377_spi_read(dev, ADF4377_REG(0x04), &device_id);
	if (ret < 0)
		goto error_spi;

	if (dev->dev_id == device_id) {
		ret = no_os_gpio_get_optional(&dev->gpio_enclk1,
					      init_param->gpio_enclk1_param);
		if (ret < 0)
			goto error_spi;

		if (dev->gpio_enclk1) {
			ret = no_os_gpio_direction_output(dev->gpio_enclk1,
							  NO_OS_GPIO_HIGH);
			if (ret < 0)
				goto error_gpio_enclk1;
		}

		if (dev->dev_id == ADF4377) {
			ret = no_os_gpio_get_optional(&dev->gpio_enclk2,
						      init_param->gpio_enclk2_param);
			if (ret < 0)
				goto error_gpio_enclk1;

			if (dev->gpio_enclk2) {
				ret = no_os_gpio_direction_output(dev->gpio_enclk2,
								  NO_OS_GPIO_HIGH);
				if (ret < 0)
					goto error_gpio_enclk2;
			}
		}
	} else {
		ret = -ENODEV;
		goto error_spi;
	}

	ret = adf4377_setup(dev);
	if (ret < 0) {
		if (dev->dev_id == ADF4378)
			goto error_gpio_enclk1;
		else
			goto error_gpio_enclk2;
	}
	*device = dev;

	return ret;

error_gpio_enclk2:
	no_os_gpio_remove(dev->gpio_enclk2);

error_gpio_enclk1:
	no_os_gpio_remove(dev->gpio_enclk1);

error_spi:
	no_os_spi_remove(dev->spi_desc);

error_gpio_ce:
	no_os_gpio_remove(dev->gpio_ce);

error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free resoulces allocated for ADF4377
 * @param dev 	- The device structure.
 * @return 	- Returns 0 in case of success or negative error code.
 */
int32_t adf4377_remove(struct adf4377_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret < 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_ce);
	if (ret < 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_enclk1);
	if (ret < 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_enclk2);
	if (ret < 0)
		return ret;

	no_os_free(dev);

	return ret;
}
