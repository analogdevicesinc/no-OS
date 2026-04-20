/***************************************************************************//**
 *   @file   adf4371.c
 *   @brief  Implementation of ADF4371 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_clk.h"
#include "adf4371.h"

static const int32_t adf4371_cp_current_microamp[] = {
	350, 700, 1050, 1400, 1750, 2100, 2450, 2800,
	3150, 3500, 3850, 4200, 4550, 4900, 5250, 5600
};

static const struct adf4371_pwrdown adf4371_pwrdown_ch[4] = {
	[ADF4371_CH_RF8] = { ADF4371_REG(0x25), 2 },
	[ADF4371_CH_RFAUX8] = { ADF4371_REG(0x72), 3 },
	[ADF4371_CH_RF16] = { ADF4371_REG(0x25), 3 },
	[ADF4371_CH_RF32] = { ADF4371_REG(0x25), 4 },
};

static const struct reg_sequence adf4371_reg_defaults[] = {
	{ ADF4371_REG(0x01), 0x00 },
	{ ADF4371_REG(0x12), 0x40 },
	{ ADF4371_REG(0x1E), 0x48 },
	{ ADF4371_REG(0x20), 0x14 },
	{ ADF4371_REG(0x22), 0x00 },
	{ ADF4371_REG(0x23), 0x00 },
	{ ADF4371_REG(0x24), 0x80 },
	{ ADF4371_REG(0x25), 0x03 },
	{ ADF4371_REG(0x27), 0xC5 },
	{ ADF4371_REG(0x28), 0x83 },
	{ ADF4371_REG(0x2C), 0x44 },
	{ ADF4371_REG(0x2D), 0x11 },
	{ ADF4371_REG(0x2E), 0x12 },
	{ ADF4371_REG(0x2F), 0x94 },
	{ ADF4371_REG(0x32), 0x04 },
	{ ADF4371_REG(0x35), 0xFA },
	{ ADF4371_REG(0x36), 0x30 },
	{ ADF4371_REG(0x39), 0x07 },
	{ ADF4371_REG(0x3A), 0x55 },
	{ ADF4371_REG(0x3E), 0x0C },
	{ ADF4371_REG(0x3F), 0x80 },
	{ ADF4371_REG(0x40), 0x50 },
	{ ADF4371_REG(0x41), 0x28 },
	{ ADF4371_REG(0x47), 0xC0 },
	{ ADF4371_REG(0x52), 0xF4 },
	{ ADF4371_REG(0x70), 0x03 },
	{ ADF4371_REG(0x71), 0x60 },
	{ ADF4371_REG(0x72), 0x32 },
	{ ADF4371_REG(0x73), 0x00 },
};

/**
 * SPI register write to device.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param val - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_write(struct adf4371_dev *dev,
		  uint16_t reg,
		  uint8_t val)
{
	uint8_t buf[3];
	uint16_t cmd;

	cmd = ADF4371_WRITE | ADF4371_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;
	buf[2] = val;

	return no_os_spi_write_and_read(dev->spi_desc, buf, NO_OS_ARRAY_SIZE(buf));
}

/**
 * SPI register write bulk to device.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param val - The buffer of data to be written.
 * @param size - The buffer size.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_write_bulk(struct adf4371_dev *dev,
		       uint16_t reg,
		       uint8_t *val,
		       uint8_t size)
{
	uint8_t buf[10];
	uint16_t cmd;
	uint8_t i;

	cmd = ADF4371_WRITE | ADF4371_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;
	for (i = 0; i < size; i++)
		buf[2 + i] = val[i];

	return no_os_spi_write_and_read(dev->spi_desc, buf, 2 + size);
}

/**
 * SPI register write a sequence of register-value pairs to device.
 * @param dev - The device structure.
 * @param regs - The array of register-value pairs.
 * @param count - The number of entries in the array.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_write_regs(struct adf4371_dev *dev,
		       const struct reg_sequence *regs,
		       uint32_t count)
{
	uint32_t i;
	int ret;

	for (i = 0; i < count; i++) {
		ret = adf4371_write(dev, regs[i].reg, regs[i].val);
		if (ret < 0)
			return ret;
	}

	return 0;
}

/**
 * SPI register read from device.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param val - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_read(struct adf4371_dev *dev,
		 uint16_t reg,
		 uint8_t *val)
{
	uint8_t buf[3];
	uint16_t cmd;
	int ret;

	cmd = ADF4371_READ | ADF4371_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;
	buf[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, NO_OS_ARRAY_SIZE(buf));
	if (ret < 0)
		return ret;

	*val = buf[2];

	return 0;
}

/**
 * SPI register update.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param mask - The bitmask selecting the field to update.
 * @param val - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_update(struct adf4371_dev *dev,
		   uint16_t reg,
		   uint8_t mask,
		   uint8_t val)
{
	uint8_t read_val;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4371_read(dev, reg, &read_val);
	if (ret)
		return ret;

	read_val &= ~mask;
	read_val |= val;

	return adf4371_write(dev, reg, read_val);
}

/**
 * @brief Set the desired reference frequency and reset everything over to maximum
 * supported value of 600MHz to the max. value and everything under the minimum
 * supported value of 10MHz to the min. value. This should also update the
 * frequency and the VCO calibration values.
 * @param dev - The device structure.
 * @param val - The reference frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_set_ref_clk(struct adf4371_dev *dev, uint64_t val)
{
	if (!dev)
		return -EINVAL;

	dev->clkin_freq = val;

	if (val > ADF4371_MAX_FREQ_REFIN)
		dev->clkin_freq = ADF4371_MAX_FREQ_REFIN;

	if (val < ADF4371_MIN_FREQ_REFIN)
		dev->clkin_freq = ADF4371_MIN_FREQ_REFIN;

	return adf4371_set_frequency(dev);

}

/**
 * Get the reference clock frequency.
 * @param dev - The device structure.
 * @param val - The reference clock frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_ref_clk(struct adf4371_dev *dev, uint64_t *val)
{
	if (!dev)
		return -EINVAL;

	*val = dev->clkin_freq;

	return 0;
}

/**
 * @brief Set the charge pump current. This should also update the bleed current
 * value.
 * @param dev - The device structure.
 * @param val - The charge pump current in microamperes.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_set_charge_pump_current(struct adf4371_dev *dev, uint32_t val)
{
	if (!dev)
		return -EINVAL;

	dev->cp_settings.regval = val;

	if (val > ADF4371_CPI_VAL_MAX)
		dev->cp_settings.regval = ADF4371_CPI_VAL_MAX;

	return adf4371_update(dev, ADF4371_REG(0x1E), ADF4371_CP_CURRENT_MSK,
			      ADF4371_CP_CURRENT(dev->cp_settings.regval));
}

/**
 * Get the charge pump current.
 * @param dev - The device structure.
 * @param val - The charge pump current in microamperes.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_charge_pump_current(struct adf4371_dev *dev, uint32_t *val)
{
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4371_read(dev, ADF4371_REG(0x1E), &tmp);
	if (ret)
		return ret;

	dev->cp_settings.regval = no_os_field_get(ADF4371_CP_CURRENT_MSK, tmp);
	*val = dev->cp_settings.regval;

	return 0;
}

/**
 * @brief Set the reference doubler to enable or disable based on the passed
 * parameter. If the parameter is different then 0 it will set the doubler to
 * enable. This should also update the frequency and the VCO calibration values.
 * @param dev 		- The device structure.
 * @param en	 	- The enable or disable value of the reference doubler.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4371_set_ref_doubler(struct adf4371_dev *dev, bool en)
{
	if (!dev)
		return -EINVAL;

	dev->ref_doubler_en = en;

	return adf4371_set_frequency(dev);
}

/**
 * Get the reference doubler enable status.
 * @param dev - The device structure.
 * @param en - The enable or disable value of the reference doubler.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_ref_doubler(struct adf4371_dev *dev, bool *en)
{
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4371_read(dev, ADF4371_REG(0x22), &tmp);
	if (ret)
		return ret;

	dev->ref_doubler_en = no_os_field_get(ADF4371_REF_DOUBLER_EN_MSK, tmp);
	*en = dev->ref_doubler_en;

	return 0;
}

/**
 * @brief Set the RDIV2 enable to enable or disable based on the passed
 * parameter. If the parameter is different then 0 it will set the RDIV2 to
 * enable. This should also update the frequency and the VCO calibration values.
 * @param dev 		- The device structure.
 * @param en	 	- The enable or disable value of the RDIV2.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4371_set_rdiv2_enable(struct adf4371_dev *dev, bool en)
{
	int ret;

	if (!dev)
		return -EINVAL;

	dev->ref_div2_en = en;
	ret = adf4371_update(dev, ADF4371_REG(0x22), ADF4371_RDIV2_EN_MSK,
			     ADF4371_RDIV2_EN(dev->ref_div2_en));
	if (ret)
		return ret;
	return adf4371_set_frequency(dev);;
}

/**
 * Get the RDIV2 enable status.
 * @param dev - The device structure.
 * @param en - The enable or disable value of the RDIV2.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_rdiv2_enable(struct adf4371_dev *dev, bool *en)
{
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4371_read(dev, ADF4371_REG(0x22), &tmp);
	if (ret)
		return ret;
	dev->ref_div2_en = no_os_field_get(ADF4371_RDIV2_EN_MSK, tmp);
	*en = dev->ref_div2_en;
	return 0;
}

/**
 * @brief Set the R div word. This should also update the frequency and the
 * VCO calibration values.
 * @param dev - The device structure.
 * @param val - The R div word value.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_set_r_word(struct adf4371_dev *dev, uint32_t val)
{
	if (!dev)
		return -EINVAL;

	dev->ref_div_factor = val;

	return adf4371_set_frequency(dev);
}

/**
 * Get the R div word value.
 * @param dev - The device structure.
 * @param val - The R div word value.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_r_word(struct adf4371_dev *dev, uint32_t *val)
{
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4371_read(dev, ADF4371_REG(0x1F), &tmp);
	if (ret)
		return ret;

	dev->ref_div_factor = no_os_field_get(ADF4371_R_WORD_MSK, tmp);
	*val = dev->ref_div_factor;

	return 0;
}

/**
 * @brief Set the output divider for one channel. This should also update the
 * frequency and the VCO calibration values.
 * @param dev - The device structure.
 * @param val - The output divider value.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_set_output_divider(struct adf4371_dev *dev, uint32_t val)
{
	if (!dev)
		return -EINVAL;

	dev->rf_div_sel = val;

	return adf4371_write(dev, ADF4371_REG(0x25), ADF4371_RF_DIV_SEL(val));
}

/**
 * @brief Set power down state for one channel.
 * @param dev - The device structure.
 * @param channel - The selected channel.
 * @param power_down - The power down state.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_set_channel_power_down(struct adf4371_dev *dev,
				   uint32_t channel,
				   bool power_down)
{
	uint32_t bit, reg;
	uint8_t readval;
	int32_t ret;

	reg = adf4371_pwrdown_ch[channel].reg;
	bit = adf4371_pwrdown_ch[channel].bit;
	ret = adf4371_read(dev, reg, &readval);
	if (ret < 0)
		return ret;

	readval &= ~NO_OS_BIT(bit);
	readval |= (!power_down << bit);

	return adf4371_write(dev, reg, readval);
}

/**
 * @brief Get power down state for one channel.
 * @param dev - The device structure.
 * @param channel - The selected channel.
 * @param power_down - The power down state.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_channel_power_down(struct adf4371_dev *dev,
				   uint32_t channel,
				   bool *power_down)
{
	uint32_t bit, reg;
	uint8_t readval;
	int32_t ret;

	reg = adf4371_pwrdown_ch[channel].reg;
	bit = adf4371_pwrdown_ch[channel].bit;
	ret = adf4371_read(dev, reg, &readval);
	if (ret < 0)
		return ret;

	*power_down = !(readval & NO_OS_BIT(bit));

	return 0;
}

/**
 * @brief Compute the PFD frequency of the device.
 * @param dev - The device structure.
 * @return The calculated PFD frequency.
 */
static uint64_t adf4371_pfd_freq_compute(struct adf4371_dev *dev)
{
	uint64_t reference_freq;
	uint64_t pfd_freq;

	if (!dev)
		return -EINVAL;

	reference_freq = dev->clkin_freq;

	if (dev->ref_doubler_en)
		reference_freq *= 2;

	pfd_freq = NO_OS_DIV_ROUND_CLOSEST(reference_freq, dev->ref_div_factor);

	if (dev->ref_div2_en)
		pfd_freq /= 2;

	return pfd_freq;
}

/**
 * @brief Get the output frequency for one channel.
 * @param dev - The device structure.
 * @param value - The output frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_rfout(struct adf4371_dev *dev, uint64_t *value)
{
	uint64_t pfd_freq;
	uint64_t freq;
	uint16_t n_int;
	uint32_t frac1_word;
	uint32_t frac2_word;
	uint32_t mod2_word;
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	pfd_freq = adf4371_pfd_freq_compute(dev);

	// Read N_INT from register
	ret = adf4371_read(dev, ADF4371_REG(0x11), &tmp);
	if (ret)
		return ret;
	n_int = (tmp & ADF4371_NINT_M_MSK) << 8;
	ret = adf4371_read(dev, ADF4371_REG(0x10), &tmp);
	if (ret)
		return ret;
	n_int |= tmp & ADF4371_NINT_L_MSK;

	// Read FRAC1_WORD from register
	ret = adf4371_read(dev, ADF4371_REG(0x14), &tmp);
	if (ret)
		return ret;
	frac1_word = tmp & ADF4371_FRAC1WORD_L_MSK;
	ret = adf4371_read(dev, ADF4371_REG(0x15), &tmp);
	if (ret)
		return ret;
	frac1_word |= (tmp & ADF4371_FRAC1WORD_M_MSK) << 8;
	ret = adf4371_read(dev, ADF4371_REG(0x16), &tmp);
	if (ret)
		return ret;
	frac1_word |= (tmp & ADF4371_FRAC1WORD_MM_MSK) << 16;
	ret = adf4371_read(dev, ADF4371_REG(0x17), &tmp);
	if (ret)
		return ret;
	frac1_word |= (tmp & ADF4371_FRAC1WORD_H_MSK) << 24;

	// Read FRAC2_WORD from register
	ret = adf4371_read(dev, ADF4371_REG(0x17), &tmp);
	if (ret)
		return ret;
	frac2_word = tmp & ADF4371_FRAC2WORD_L_MSK;
	ret = adf4371_read(dev, ADF4371_REG(0x18), &tmp);
	if (ret)
		return ret;
	frac2_word |= (tmp & ADF4371_FRAC2WORD_H_MSK) << 8;

	// Read MOD2_WORD from register
	ret = adf4371_read(dev, ADF4371_REG(0x19), &tmp);
	if (ret)
		return ret;
	mod2_word = tmp & ADF4371_MOD2WORD_L_MSK;
	ret = adf4371_read(dev, ADF4371_REG(0x1A), &tmp);
	if (ret)
		return ret;
	mod2_word |= (tmp & ADF4371_MOD2WORD_H_MSK) << 8;

	if (mod2_word) {
		freq = frac2_word * pfd_freq;
		freq = no_os_div_u64(freq, mod2_word);
	} else
		freq = 0;

	freq += frac1_word * pfd_freq;
	freq = no_os_div_u64(freq, ADF4371_MODULUS1);
	freq += n_int * pfd_freq;

	dev->vco_freq = freq;
	dev->rfout8_freq = dev->vco_freq / (1 << dev->rf_div_sel);
	dev->rfout16_freq = dev->vco_freq << 1;
	dev->rfout32_freq = dev->vco_freq << 2;
	*value = dev->rfout8_freq;
	return 0;
}

/**
 * @brief Get the output frequency for RFOUT16 channel.
 * @param dev - The device structure.
 * @param value - The output frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_rfout16(struct adf4371_dev *dev, uint64_t *value)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4371_get_rfout(dev, value);
	if (ret)
		return ret;

	*value = dev->vco_freq << 1;

	return 0;
}

/**
 * @brief Get the output frequency for RFOUT32 channel.
 * @param dev - The device structure.
 * @param value - The output frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_rfout32(struct adf4371_dev *dev, uint64_t *value)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4371_get_rfout(dev, value);
	if (ret)
		return ret;

	*value = dev->vco_freq << 2;

	return 0;
}

/**
 * @brief Get the output frequency for RFAUX8 channel.
 * @param dev - The device structure.
 * @param value - The output frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_rfout8aux(struct adf4371_dev *dev, uint64_t *value)
{
	if (!dev)
		return -EINVAL;

	*value = dev->rfout8_freq;

	return 0;
}

/**
 * @brief Compute the integer and fractional values for the desired output frequency.
 * @param f_vco - The desired VCO frequency.
 * @param pfd_freq - The PFD frequency.
 * @param n_int - The integer word value.
 * @param frac1_word - The fractional 1 word value.
 * @param frac2_word - The fractional 2 word value.
 * @param mod2_word - The MOD2 word value.
 * @return 0 in case of success, negative error code otherwise.
 */
static int adf4371_frac_compute(uint64_t f_vco, uint64_t pfd_freq,
				uint16_t *n_int,
				uint32_t *frac1_word, uint32_t *frac2_word,
				uint32_t *mod2_word)
{
	uint64_t res;
	uint64_t rem;
	uint32_t channel_spacing;
	uint32_t chsp_freq;
	uint32_t mod2_tmp;
	uint32_t mod2_max;
	uint32_t mod2_wd;
	uint32_t gcd;
	channel_spacing = 1;
	mod2_wd = 1;

	mod2_max = ADF4371_MAX_MODULUS2;

	do {

		chsp_freq = channel_spacing * ADF4371_MODULUS1;
		gcd = no_os_greatest_common_divisor(chsp_freq, pfd_freq);
		mod2_tmp = NO_OS_DIV_ROUND_UP(pfd_freq, gcd);

		if (mod2_tmp > mod2_max) {
			channel_spacing *= 5;
		} else {
			mod2_wd = mod2_tmp;
			break;
		}

	} while (channel_spacing < ADF4371_CHANNEL_SPACING_MAX);

	*n_int = no_os_div64_u64_rem(f_vco, pfd_freq, &rem);
	res = rem * ADF4371_MODULUS1;
	*frac1_word = no_os_div64_u64_rem(res, pfd_freq, &rem);
	*frac2_word = 0;
	*mod2_word = 1;

	if (rem > 0) {
		res = rem * mod2_wd;
		*frac2_word = no_os_div64_u64_rem(res, pfd_freq, &rem);
		*mod2_word = mod2_wd;
	}

	return 0;
}

/**
 * @brief Set the output frequency for RFOUT8 channel.
 * @param dev - The device structure.
 * @param freq - The output frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_set_rf8_freq(struct adf4371_dev *dev, uint64_t freq)
{

	if (!dev)
		return -EINVAL;

	dev->rfout8_freq = freq;
	return adf4371_set_frequency(dev);
}

/**
 * @brief Set the output frequency for one channel.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_set_frequency(struct adf4371_dev *dev)
{
	uint32_t frac2_word;
	uint32_t frac1_word;
	uint32_t mod2_word;
	uint64_t pfd_freq;
	uint16_t output_divider;
	uint8_t int_mode;
	uint64_t freq;
	uint8_t en_bleed;
	uint16_t n_int;
	uint8_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	freq = dev->rfout8_freq;

	while (freq < ADF4371_MIN_OUT_RF8_FREQ) {
		freq <<= 1;
		dev->rf_div_sel++;
	}

	output_divider = 1 << dev->rf_div_sel;
	dev->vco_freq = freq * output_divider;
	dev->rfout16_freq = dev->vco_freq << 1;
	dev->rfout32_freq = dev->vco_freq << 2;

	// Write Reference Divider to register
	val = dev->ref_div_factor & ADF4371_R_WORD_MSK;
	ret = adf4371_write(dev, ADF4371_REG(0x1F), val);
	if (ret)
		return ret;

	// Write Reference Doubler to register
	val = ADF4371_REF_DOUBLER_EN(dev->ref_doubler_en);
	ret = adf4371_update(dev, ADF4371_REG(0x22), ADF4371_REF_DOUBLER_EN_MSK, val);
	if (ret)
		return ret;

	// compute PFD frequency
	pfd_freq = adf4371_pfd_freq_compute(dev);

	// Write Charge Pump current to register
	val = ADF4371_CP_CURRENT(dev->cp_settings.regval);
	ret = adf4371_update(dev, ADF4371_REG(0x1E), ADF4371_CP_CURRENT_MSK, val);
	if (ret)
		return ret;

	// compute integer and fractional values
	ret = adf4371_frac_compute(dev->vco_freq, pfd_freq, &n_int, &frac1_word,
				   &frac2_word, &mod2_word);
	if (ret)
		return ret;

	if (frac1_word || frac2_word) {
		int_mode = 0;
		en_bleed = 1;
	} else {
		int_mode = 1;
		en_bleed = 0;
	}

	dev->integer = (uint32_t)n_int;

	// Write INT_MODE to register
	val = ADF4371_VAR_MOD(int_mode);
	ret = adf4371_update(dev, ADF4371_REG(0x2B), ADF4371_VAR_MOD_MSK, val);
	if (ret)
		return ret;

	ret = adf4371_update(dev, ADF4371_REG(0x24),
			     ADF4371_RF_DIV_SEL_MSK,
			     ADF4371_RF_DIV_SEL(dev->rf_div_sel));
	if (ret < 0)
		return ret;

	// Write EN_BLEED to register
	val = ADF4371_BLEED_EN(en_bleed);
	ret = adf4371_update(dev, ADF4371_REG(0x27), ADF4371_BLEED_EN_MSK, val);
	if (ret)
		return ret;

	/*
	 * The optimum bleed current is set by ((4/N) × ICP)/3.75,
	 * where ICP is the charge pump current in μA
	 */
	dev->cp_settings.bleed_icp = NO_OS_DIV_ROUND_UP(400 *
				     dev->cp_settings.icp,
				     dev->integer * 375);
	dev->cp_settings.bleed_icp = no_os_clamp(dev->cp_settings.bleed_icp,
				     1U, 255U);
	ret = adf4371_write(dev, ADF4371_REG(0x26), ADF4371_BLEED_ICP
			    (dev->cp_settings.bleed_icp));
	if (ret < 0)
		return ret;

	// Write MOD2_WORD to register
	val = mod2_word & ADF4371_MOD2WORD_L_MSK;
	ret = adf4371_write(dev, ADF4371_REG(0x19), val);
	if (ret)
		return ret;
	val = (mod2_word >> 8) & ADF4371_MOD2WORD_H_MSK;
	ret = adf4371_write(dev, ADF4371_REG(0x1A), val);
	if (ret)
		return ret;

	// Write FRAC2_WORD to register
	val = (frac2_word >> 7) & ADF4371_FRAC2WORD_H_MSK;
	ret = adf4371_write(dev, ADF4371_REG(0x18), val);
	if (ret)
		return ret;

	// Write FRAC1_WORD to register
	val = frac1_word & ADF4371_FRAC1WORD_L_MSK;
	ret = adf4371_write(dev, ADF4371_REG(0x14), val);
	if (ret)
		return ret;
	val = (frac1_word >> 8) & ADF4371_FRAC1WORD_M_MSK;
	ret = adf4371_write(dev, ADF4371_REG(0x15), val);
	if (ret)
		return ret;
	val = (frac1_word >> 16) & ADF4371_FRAC1WORD_MM_MSK;
	ret = adf4371_write(dev, ADF4371_REG(0x16), val);
	if (ret)
		return ret;
	val = (frac2_word & ADF4371_FRAC2WORD_L_MSK) |
	      ((frac1_word >> 24) & ADF4371_FRAC1WORD_H_MSK);
	ret = adf4371_write(dev, ADF4371_REG(0x17), val);
	if (ret)
		return ret;

	// Write N_INT to register
	val = (n_int >> 8) & ADF4371_NINT_M_MSK;
	ret = adf4371_write(dev, ADF4371_REG(0x11), val);
	if (ret)
		return ret;
	val = n_int & ADF4371_NINT_L_MSK;
	return adf4371_write(dev, ADF4371_REG(0x10), val);
}

/**
 * @brief Set bleed current for the charge pump.
 * @param dev - The device structure.
 * @param val - The bleed word value.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_set_bleed_icp(struct adf4371_dev *dev, uint32_t val)
{
	uint32_t regval;

	if (!dev)
		return -EINVAL;

	if (val > ADF4371_BLEED_ICP_MAX)
		dev->cp_settings.bleed_icp = ADF4371_BLEED_ICP_MAX;
	else
		dev->cp_settings.bleed_icp = val;

	regval = no_os_field_prep(ADF4371_BLEED_ICP_MSK, dev->cp_settings.bleed_icp);

	return adf4371_write(dev, ADF4371_REG(0x26), regval);
}

/**
 * Get bleed current for the charge pump.
 * @param dev - The device structure.
 * @param val - The bleed word value.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_get_bleed_icp(struct adf4371_dev *dev, uint32_t *val)
{
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4371_read(dev, ADF4371_REG(0x26), &tmp);
	if (ret)
		return ret;

	dev->cp_settings.bleed_icp = no_os_field_get(ADF4371_BLEED_ICP_MSK, tmp);
	*val = dev->cp_settings.bleed_icp;

	return 0;
}

/**
 * Setup the ADF4371 device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int adf4371_setup(struct adf4371_dev *dev)
{
	uint32_t vco_alc_timeout = 1;
	uint32_t synth_timeout = 2;
	uint32_t vco_band_div;
	uint32_t timeout = 1;
	uint32_t tmp;
	bool en = true;
	uint8_t mask;
	uint8_t val;
	int32_t ret;
	uint32_t i;

	if (!dev)
		return -EINVAL;

	ret = adf4371_write(dev, ADF4371_REG(0x0), ADF4371_RESET_CMD);
	if (ret < 0)
		return ret;

	if (dev->spi_3wire_en)
		en = false;

	ret = adf4371_write(dev, ADF4371_REG(0x0),
			    ADF4371_SDO_ACT(en) | ADF4371_SDO_ACT_R(en));
	if (ret < 0)
		return ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adf4371_reg_defaults); i++) {
		ret = adf4371_write(dev,
				    adf4371_reg_defaults[i].reg,
				    adf4371_reg_defaults[i].val);
		if (ret < 0)
			return ret;
	}

	if (dev->differential_ref_clk) {
		ret = adf4371_update(dev, ADF4371_REG(0x22),
				     ADF4371_REFIN_MODE_MASK,
				     ADF4371_REFIN_MODE(1));
		if (ret < 0)
			return ret;
	}

	mask = ADF4371_PD_POL_MSK | ADF4371_CP_CURRENT_MSK;
	val = ADF4371_PD_POL(dev->pd_pol) |
	      ADF4371_CP_CURRENT(dev->cp_settings.regval);

	/* Set the phase detector polarity and the charge pump current */
	ret = adf4371_update(dev, ADF4371_REG(0x1E),  mask, val);
	if (ret < 0)
		return ret;

	/* Mute to Lock Detect */
	if (dev->mute_till_lock_en) {
		ret = adf4371_update(dev, ADF4371_REG(0x25),
				     ADF4371_MUTE_LD_MSK,
				     ADF4371_MUTE_LD(1));
		if (ret < 0)
			return ret;
	}

	ret = adf4371_update(dev, ADF4371_REG(0x20),
			     ADF4371_MUXOUT_MSK |
			     ADF4371_MUXOUT_EN_MSK |
			     ADF4371_MUXOUT_LVL_MSK,
			     ADF4371_MUXOUT(dev->muxout_default_mode) |
			     ADF4371_MUXOUT_EN(dev->muxout_en) |
			     ADF4371_MUXOUT_LVL(!dev->muxout_1v8_en));
	if (ret < 0)
		return ret;

	/* Set address in ascending order, so the write_bulk() will work */
	ret = adf4371_update(dev, ADF4371_REG(0x0),
			     ADF4371_ADDR_ASC_MSK | ADF4371_ADDR_ASC_R_MSK,
			     ADF4371_ADDR_ASC(1) | ADF4371_ADDR_ASC_R(1));
	if (ret < 0)
		return ret;

	/*
	 * Calculate and maximize PFD frequency
	 * fPFD = REFIN × ((1 + D)/(R × (1 + T)))
	 * Where D is the REFIN doubler bit, T is the reference divide by 2,
	 * R is the reference division factor
	 * TODO: it is assumed D and T equal 0.
	 */
	do {
		dev->ref_div_factor++;
		dev->fpfd = dev->clkin_freq / dev->ref_div_factor;
	} while (dev->fpfd > ADF4371_MAX_FREQ_PFD);

	/* Calculate ADC_CLK_DIV */
	tmp = NO_OS_DIV_ROUND_UP((dev->fpfd / 100000) - 2, 4);
	tmp = no_os_clamp(tmp, 0U, 255U);

	ret = adf4371_write(dev, ADF4371_REG(0x35), tmp);
	if (ret)
		return ret;

	/* Calculate Timeouts */
	vco_band_div = NO_OS_DIV_ROUND_UP(dev->fpfd, 2400000U);

	tmp = NO_OS_DIV_ROUND_CLOSEST(dev->fpfd, 1000000U);
	do {
		timeout++;
		if (timeout > 1023) {
			timeout = 2;
			synth_timeout++;
		}
	} while (synth_timeout * 1024 + timeout <= 20 * tmp);

	do {
		vco_alc_timeout++;
	} while (vco_alc_timeout * 1024 - timeout <= 50 * tmp);

	dev->buf[0] = vco_band_div;
	dev->buf[1] = timeout & 0xFF;
	dev->buf[2] = ADF4371_TIMEOUT(timeout >> 8) | 0x04;
	dev->buf[3] = synth_timeout;
	dev->buf[4] = ADF4371_VCO_ALC_TOUT(vco_alc_timeout);

	ret = adf4371_write_bulk(dev, ADF4371_REG(0x30), dev->buf, 5);
	if (ret < 0)
		return ret;

	ret = adf4371_update(dev, ADF4371_REG(0x25), ADF4371_RF_EN_MSK,
			     ADF4371_RF_EN(1));
	if (ret < 0)
		return ret;

	return adf4371_set_frequency(dev);
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_init(struct adf4371_dev **device,
		 const struct adf4371_init_param *init_param)
{
	struct adf4371_dev *dev;
	int32_t ret;
	uint32_t i;

	dev = (struct adf4371_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret < 0)
		return ret;

	/* GPIO Chip Enable */
	ret = no_os_gpio_get_optional(&dev->gpio_ce, init_param->gpio_ce_param);
	if (ret < 0)
		goto error;

	if (dev->gpio_ce) {
		ret = no_os_gpio_direction_output(dev->gpio_ce, NO_OS_GPIO_HIGH);
		if (ret < 0)
			goto error_gpio_ce;
	}

	dev->spi_3wire_en = init_param->spi_3wire_enable;

	dev->clkin_freq = init_param->clkin_frequency;

	dev->differential_ref_clk = init_param->differential_ref_clock;

	if (init_param->loop_filter_inverting)
		dev->pd_pol = 0;
	else
		dev->pd_pol = 1;

	if (init_param->charge_pump_microamp == 0) {
		/* Default charge pump current is 1.75mA */
		dev->cp_settings.icp = 1750;
		dev->cp_settings.regval = 0x04;
	} else {
		i = no_os_find_closest(init_param->charge_pump_microamp,
				       adf4371_cp_current_microamp,
				       NO_OS_ARRAY_SIZE(adf4371_cp_current_microamp));
		dev->cp_settings.regval = i;
		dev->cp_settings.icp = adf4371_cp_current_microamp[i];
	}

	dev->mute_till_lock_en = init_param->mute_till_lock_enable;

	if (init_param->muxout_select > 10)
		dev->muxout_default_mode = 0; /* Tristate */
	else
		dev->muxout_default_mode = init_param->muxout_select;

	if (dev->muxout_default_mode > 0)
		dev->muxout_en = true;

	dev->muxout_1v8_en = init_param->muxout_level_1v8_enable;

	dev->rf_div_sel = init_param->rf_div_sel;

	dev->rfout8_freq = init_param->rfout8_freq;

	ret = adf4371_setup(dev);
	if (ret < 0)
		goto error;

	*device = dev;

	return 0;

error_gpio_ce:
	no_os_gpio_remove(dev->gpio_ce);
error:
	no_os_spi_remove(dev->spi_desc);
	no_os_free(dev);

	return ret;
}

/**
 * Remove the device - release resources.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf4371_remove(struct adf4371_dev *device)
{
	int ret;

	ret = no_os_spi_remove(device->spi_desc);

	no_os_gpio_remove(device->gpio_ce);
	no_os_free(device);

	return ret;
}
