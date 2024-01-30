/***************************************************************************//**
 *   @file   adf4382.c
 *   @brief  Implementation of adf4382 Driver.
 *   @author Ciprian Hegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include "adf4382.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* Charge pump current values expressed in uA */
static const int adf4382_ci_ua[] = {
	700,
	900,
	1100,
	1300,
	1400,
	1800,
	2200,
	2500,
	2900,
	3600,
	4300,
	5000,
	5800,
	7200,
	8600,
	11100
};

/**
 * @brief Writes data to ADF4382 over SPI.
 * @param dev	   - The device structure.
 * @param reg_addr - The register address.
 * @param data 	   - Data value to write.
 * @return 	   - 0 in case of success or negative error code otherwise.
 */
int adf4382_spi_write(struct adf4382_dev *dev, uint16_t reg_addr, uint8_t data)
{
	uint8_t buff[ADF4382_BUFF_SIZE_BYTES];
	uint16_t cmd;

	if (!dev)
		return -EINVAL;

	cmd = ADF4382_SPI_WRITE_CMD | reg_addr;
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
					ADF4382_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ADF4382 over SPI.
 * @param dev 	   - The device structure.
 * @param reg_addr - The register address.
 * @param data	   - Data read from the device.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adf4382_spi_read(struct adf4382_dev *dev, uint16_t reg_addr, uint8_t *data)
{
	int ret;
	uint8_t buff[ADF4382_BUFF_SIZE_BYTES];
	uint16_t cmd;

	if (!dev)
		return -EINVAL;

	cmd = ADF4382_SPI_READ_CMD | reg_addr;
	if (dev->spi_desc->bit_order) {
		buff[0] = no_os_bit_swap_constant_8(cmd & 0xFF);
		buff[1] = no_os_bit_swap_constant_8(cmd >> 8);
		buff[2] = no_os_bit_swap_constant_8(ADF4382_SPI_DUMMY_DATA);
	} else {
		buff[0] = cmd >> 8;
		buff[1] = cmd & 0xFF;
		buff[2] = ADF4382_SPI_DUMMY_DATA;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buff,
				       ADF4382_BUFF_SIZE_BYTES);
	if (ret)
		return ret;

	*data = buff[2];

	return 0;
}

/**
 * @brief Updates the values of the ADF4382 register.
 * @param dev 	   - The device structure.
 * @param reg_addr - The register address.
 * @param mask 	   - Bits to be updated.
 * @param data 	   - Update value for the mask.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adf4382_spi_update_bits(struct adf4382_dev *dev, uint16_t reg_addr,
			    uint8_t mask, uint8_t data)
{
	uint8_t tmp, orig;
	int ret;

	ret = adf4382_spi_read(dev, reg_addr, &orig);
	if (ret)
		return ret;

	tmp = orig & ~mask;
	tmp |= data & mask;

	if (tmp != orig)
		return adf4382_spi_write(dev, reg_addr, tmp);

	return 0;
}

/**
 * @brief Will output on the terminal the values of all the ADF4382 registers.
 * @param dev 	- The device structure.
 * @return 	- 0 in case of success or negative error code.
 */
int adf4382_reg_dump(struct adf4382_dev *dev)
{
	uint8_t val;
	uint16_t i;
	int ret;

	for(i = 0; i < 0x68; i++) {
		ret = adf4382_spi_read(dev, i, &val);
		if (ret)
			return ret;

		pr_info("0x%X    0x%X\n", i, val);
	}

	for (i = 0x100; i < 0x112; i++) {
		ret = adf4382_spi_read(dev, i, &val);
		if (ret)
			return ret;

		pr_info("0x%X    0x%X\n", i, val);
	}

	for (i = 0x200; i < 0x274; i++) {
		ret = adf4382_spi_read(dev, i, &val);
		if (ret)
			return ret;

		pr_info("0x%X    0x%X\n", i, val);
	}

	return ret;
}

/**
 * @brief Set the desired reference frequency and reset everything over to maximum
 * supported value of 5GHz to the max. value and everything under the minimum
 * supported value of 10MHz to the min. value.
 * @param dev 		- The device structure.
 * @param val		- The desired reference frequency in Hz.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4382_set_ref_clk(struct adf4382_dev *dev, uint64_t val)
{
	if (!dev)
		return -EINVAL;

	dev->ref_freq_hz = val;

	if (val > ADF4382_REF_CLK_MAX)
		dev->ref_freq_hz = ADF4382_REF_CLK_MAX;

	if (val < ADF4382_REF_CLK_MIN)
		dev->ref_freq_hz = ADF4382_REF_CLK_MIN;

	return adf4382_set_freq(dev);
}

/**
 * @brief Gets the user proposed reference frequency
 * @param dev 		- The device structure.
 * @param val		- The set value of the reference frequency in Hz.
 * @return    		- Reference frequency in KHz.
 */
int adf4382_get_ref_clk(struct adf4382_dev *dev, uint64_t *val)
{
	if (!dev)
		return -EINVAL;

	*val = dev->ref_freq_hz;

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
int adf4382_set_en_ref_doubler(struct adf4382_dev *dev, bool en)
{
	if (!dev)
		return -EINVAL;

	dev->ref_doubler_en = en;

	return adf4382_set_freq(dev);
}

/**
 * @brief Gets the value the doubler if it is enabled or disable and stores it
 * it the dev structure.
 * @param dev 		- The device structure.
 * @param en		- The read value of the reference doubler.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4382_get_en_ref_doubler(struct adf4382_dev *dev, bool *en)
{
	uint8_t tmp;
	int ret;

	ret = adf4382_spi_read(dev, 0x20, &tmp);
	if (ret)
		return ret;

	dev->ref_doubler_en = no_os_field_get(tmp, ADF4382_EN_RDBLR_MSK);
	*en = dev->ref_doubler_en;

	return 0;
}

/**
 * @brief Set the reference divider value and reset everything over to maximum
 * supported value of 63 to the max. value
 * @param dev 		- The device structure.
 * @param div	 	- The reference divider value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4382_set_ref_div(struct adf4382_dev *dev, int32_t div)
{
	if (!dev)
		return -EINVAL;

	dev->ref_div = div;

	if (div > ADF4382_REF_DIV_MAX)
		dev->ref_div = ADF4382_REF_DIV_MAX;

	return adf4382_set_freq(dev);
}

/**
 * @brief Gets the value the reference divider.
 * @param dev 		- The device structure.
 * @param div	 	- The read reference divider value.
 * @return    		- Result of the reading procedure, error code otherwise.
 */
int adf4382_get_ref_div(struct adf4382_dev *dev, int32_t *div)
{
	uint8_t tmp;
	int ret;

	ret = adf4382_spi_read(dev, 0x20, &tmp);
	if (ret)
		return ret;

	dev->ref_div = no_os_field_get(ADF4382_R_DIV_MSK, tmp);
	*div = dev->ref_div;

	return 0;
}

/**
 * @brief Set the charge pump value which will be written to the register. The
 * value will be between 0 and 15 on 8 bits. For more information please
 * consult the Datasheet.
 * @param dev 		- The device structure.
 * @param reg_val 	- The desired charge pump register value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4382_set_cp_i(struct adf4382_dev *dev, int32_t reg_val)
{
	if (!dev)
		return -EINVAL;

	dev->cp_i = (uint8_t)reg_val;

	if (reg_val > ADF4382_CPI_VAL_MAX)
		dev->cp_i = ADF4382_CPI_VAL_MAX;

	return adf4382_set_freq(dev);
}

/**
 * @brief Gets the charge pump value from the register. The value will be
 * between 0 and 15 on 8 bits. For more information please consult the
 * Datasheet.
 * @param dev 		- The device structure.
 * @param reg_val 	- The read charge pump register value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4382_get_cp_i(struct adf4382_dev *dev, int32_t *reg_val)
{
	uint8_t tmp;
	int ret;

	ret = adf4382_spi_read(dev, 0x1F, &tmp);
	if (ret)
		return ret;

	dev->cp_i = no_os_field_get(ADF4382_CP_I_MSK, tmp);
	*reg_val = dev->cp_i;

	return 0;
}

/**
 * @brief Set the bleed word, which represents the value of the bleed current
 * written to the register space.
 * @param dev 		- The device structure.
 * @param word	 	- The bleed current register value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4382_set_bleed_word(struct adf4382_dev *dev, int32_t word)
{
	if (!dev)
		return -EINVAL;

	dev->bleed_word = (uint16_t)word;

	if (word > ADF4382_BLEED_WORD_MAX)
		dev->bleed_word = ADF4382_BLEED_WORD_MAX;

	return adf4382_set_freq(dev);
}

/**
 * @brief Gets the value of the set bleed word.
 * @param dev 		- The device structure.
 * @param word	 	- The read bleed current register value.
 * @return    		- Result of the reading procedure, error code otherwise.
 */
int adf4382_get_bleed_word(struct adf4382_dev *dev, int32_t *word)
{
	uint8_t upper;
	uint8_t lower;
	int ret;

	ret = adf4382_spi_read(dev, 0x1E, &upper);
	if (ret)
		return ret;

	upper &= (ADF4382_COARSE_BLEED_MSK | ADF4382_FINE_BLEED_MSB_MSK);

	ret = adf4382_spi_read(dev, 0x1D, &lower);
	if (ret)
		return ret;

	dev->bleed_word = (upper << 8) | lower;
	*word = dev->bleed_word;

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
int adf4382_set_out_power(struct adf4382_dev *dev, uint8_t ch, int32_t pwr)
{
	uint8_t tmp;

	if (pwr > ADF4382_OUT_PWR_MAX)
		pwr = ADF4382_OUT_PWR_MAX;

	if (!ch) {
		tmp = no_os_field_prep(ADF4382_CLK1_OPWR_MSK, pwr);
		return adf4382_spi_update_bits(dev, 0x29, ADF4382_CLK1_OPWR_MSK,
					       tmp);
	}

	tmp = no_os_field_prep(ADF4382_CLK2_OPWR_MSK, pwr);
	return adf4382_spi_update_bits(dev, 0x29, ADF4382_CLK2_OPWR_MSK, tmp);
}

/**
 * @brief Gets the output power register value.
 * @param dev 		- The device structure.
 * @param ch	 	- The channel to get the power off.
 * @param pwr	 	- The output power register value.
 * @return    		- Result of the reading procedure, error code otherwise.
 */
int adf4382_get_out_power(struct adf4382_dev *dev, uint8_t ch, int32_t *pwr)
{

	uint8_t tmp;
	int ret;

	ret = adf4382_spi_read(dev, 0x29, &tmp);
	if (ret)
		return ret;

	if (!ch)
		*pwr = no_os_field_get(ADF4382_CLK1_OPWR_MSK, tmp);
	else
		*pwr = no_os_field_get(ADF4382_CLK2_OPWR_MSK, tmp);

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
int adf4382_set_en_chan(struct adf4382_dev *dev, uint8_t ch, bool en)
{
	uint8_t enable;

	if (!ch) {
		enable = no_os_field_prep(ADF4382_PD_CLKOUT1_MSK, !en);
		return adf4382_spi_update_bits(dev, 0x2B,
					       ADF4382_PD_CLKOUT1_MSK,
					       enable);
	}

	enable = no_os_field_prep(ADF4382_PD_CLKOUT2_MSK, !en);
	return adf4382_spi_update_bits(dev, 0x2B, ADF4382_PD_CLKOUT2_MSK,
				       enable);
}

/**
 * @brief Gets the value the output channel if it is enabled or disable.
 * @param dev 		- The device structure.
 * @param ch	 	- The channel to get state.
 * @param en	 	- The status of the output channel.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4382_get_en_chan(struct adf4382_dev *dev, uint8_t ch, bool *en)
{
	uint8_t tmp;
	bool enable;
	int ret;

	ret = adf4382_spi_read(dev, 0x2B, &tmp);
	if (ret)
		return ret;

	if (!ch)
		enable = no_os_field_get(tmp, ADF4382_PD_CLKOUT1_MSK);
	else
		enable = no_os_field_get(tmp, ADF4382_PD_CLKOUT2_MSK);

	*en = !enable;

	return 0;
}

/**
 * @brief Set the sync to enable or disable based on the passed parameter. If
 * the parameter is different then 0 it will set the doubler to enable.
 * @param dev 		- The device structure.
 * @param en	 	- The enable or disable sync.
 * @return    		- Result of the writing procedure, error code otherwise.
 */
int adf4382_set_en_sync(struct adf4382_dev *dev, bool en)
{
	uint8_t enable;

	enable = no_os_field_prep(ADF4382_PD_SYNC_MSK, en);
	return adf4382_spi_update_bits(dev, 0x2A, ADF4382_PD_SYNC_MSK, enable);
}

/**
 * @brief Gets the value the sync if it is enabled or disable.
 * @param dev 		- The device structure.
 * @param en	 	- The read status of the sync enable.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4382_get_en_sync(struct adf4382_dev *dev, bool *en)
{
	uint8_t tmp;
	int ret;

	ret = adf4382_spi_read(dev, 0x2A, &tmp);
	if (ret)
		return ret;

	*en = no_os_field_get(tmp, ADF4382_PD_SYNC_MSK);
	return 0;

}

/**
 * @brief Set the desired output frequency and reset everything over to maximum
 * supported value of 22GHz (21GHz for ADF4382A) to the max. value and
 * everything under the minimum supported value of 687.5MHz (2.875GHz for
 * ADF4382A) to the min. value.
 * @param dev 		- The device structure.
 * @param val	 	- The desired output frequency in Hz.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4382_set_rfout(struct adf4382_dev *dev, uint64_t val)
{
	if (!dev)
		return -EINVAL;

	dev->freq= val;

	if (val > dev->freq_max)
		dev->freq = dev->freq_max;

	if (val < dev->freq_min)
		dev->freq = dev->freq_min;

	return adf4382_set_freq(dev);
}

/**
 * @brief Computes the PFD frequency and returns the value in KHz.
 * @param dev 	     - The device structure.
 * @return 	     - PFD value in Hz.
 */
static uint64_t adf4382_pfd_compute(struct adf4382_dev *dev)
{
	uint64_t pfd_freq;

	pfd_freq = NO_OS_DIV_ROUND_CLOSEST(dev->ref_freq_hz, dev->ref_div);
	if (dev->ref_doubler_en)
		pfd_freq *=  2;

	return pfd_freq;
}

/**
 * @brief Gets the user proposed output frequency
 * @param dev 		- The device structure.
 * @param val		- The set value of the output frequency in Hz.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4382_get_rfout(struct adf4382_dev *dev, uint64_t *val)
{
	uint32_t frac1 = 0;
	uint32_t frac2 = 0;
	uint32_t mod2 = 0;
	uint64_t freq;
	uint64_t pfd;
	uint8_t tmp;
	uint16_t n;
	int ret;

	pfd = adf4382_pfd_compute(dev);

	ret = adf4382_spi_read(dev, 0x11, &tmp);
	if  (ret)
		return ret;
	n = tmp & ADF4382_N_INT_MSB_MSK;
	n = n << 8;

	ret = adf4382_spi_read(dev, 0x10, &tmp);
	if  (ret)
		return ret;
	n |= tmp;

	ret = adf4382_spi_read(dev, 0x15, &tmp);
	if  (ret)
		return ret;
	frac1 = tmp & ADF4382_FRAC1WORD_MSB;
	frac1 = frac1 << 8;

	ret = adf4382_spi_read(dev, 0x14, &tmp);
	if  (ret)
		return ret;
	frac1 |= tmp;
	frac1 = frac1 << 8;

	ret = adf4382_spi_read(dev, 0x13, &tmp);
	if  (ret)
		return ret;
	frac1 |= tmp;
	frac1 = frac1 << 8;

	ret = adf4382_spi_read(dev, 0x12, &tmp);
	if  (ret)
		return ret;
	frac1 |= tmp;

	ret = adf4382_spi_read(dev, 0x19, &tmp);
	if  (ret)
		return ret;
	frac2 = tmp;
	frac2 = frac2 << 8;

	ret = adf4382_spi_read(dev, 0x18, &tmp);
	if  (ret)
		return ret;
	frac2 |= tmp;
	frac2 = frac2 << 8;

	ret = adf4382_spi_read(dev, 0x17, &tmp);
	if  (ret)
		return ret;
	frac2 |= tmp;

	ret = adf4382_spi_read(dev, 0x1c, &tmp);
	if  (ret)
		return ret;
	mod2 = tmp;
	mod2 = mod2 << 8;

	ret = adf4382_spi_read(dev, 0x1b, &tmp);
	if  (ret)
		return ret;
	mod2 |= tmp;
	mod2 = mod2 << 8;

	ret = adf4382_spi_read(dev, 0x1a, &tmp);
	if  (ret)
		return ret;
	mod2 |= tmp;

	freq = frac2 * pfd;
	freq = no_os_div_u64(freq, mod2);
	freq = freq + (frac1 * pfd);
	freq = no_os_div_u64(freq, ADF4382_MOD1WORD);
	freq = freq + (n * pfd);

	*val = freq;
	return 0;
}

/**
 * @brief Computes the second fractional part of the feedback divider if needed.
 * @param dev 	     - The device structure.
 * @param res 	     - Residue from the first fractional part.
 * @param pfd_freq   - Phase/frequency detector frequency.
 * @param frac2_word - Second fractional part of the feedback divider, which
 * 		       will be returned.
 * @param mod2_word  - Modulo for the second fractional part of the feedback
 * 		       divider, which will be returned.
 * @return 	     - 0 in case of success, negative error code otherwise.
 */
static int adf4382_frac2_compute(struct adf4382_dev *dev, uint64_t res,
				 uint64_t pfd_freq, uint32_t *frac2_word,
				 uint32_t *mod2_word)
{
	uint32_t channel_spacing;
	uint8_t en_phase_resync;
	uint32_t chsp_freq;
	uint32_t mod2_tmp;
	uint32_t mod2_max;
	uint32_t mod2_wd;
	uint32_t gcd;
	uint8_t tmp;
	int ret;

	channel_spacing = 1;
	mod2_wd = 1;

	ret = adf4382_spi_read(dev, 0x1E, &tmp);
	if (ret)
		return ret;

	en_phase_resync = no_os_field_get(tmp, ADF4382_EN_PHASE_RESYNC_MSK);;

	if (en_phase_resync)
		mod2_max = ADF4382_PHASE_RESYNC_MOD2WORD_MAX;
	else
		mod2_max = ADF4382_MOD2WORD_MAX;

	do {

		chsp_freq = channel_spacing * ADF4382_MOD1WORD;
		gcd = no_os_greatest_common_divisor(chsp_freq, pfd_freq);
		mod2_tmp = NO_OS_DIV_ROUND_UP(pfd_freq, gcd);

		if (mod2_tmp > mod2_max) {
			channel_spacing *= 5;
		} else {
			mod2_wd = mod2_tmp;
			break;
		}

	} while (channel_spacing < ADF4382_CHANNEL_SPACING_MAX);

	if (!en_phase_resync) {
		mod2_wd *= NO_OS_DIV_U64(mod2_max, mod2_wd);
	}

	*frac2_word = NO_OS_DIV_ROUND_CLOSEST_ULL(res * mod2_wd, pfd_freq);
	*mod2_word = mod2_wd;

	return 0;
}

/**
 * @brief Computes the feedback divider values for the PLL.
 * @param dev 	     - The device structure.
 * @param freq 	     - The output frequency.
 * @param pfd_freq   - Phase/frequency detector frequency.
 * @param n_int      - Integer part of the feedback divider, which will be
 * 		       returned.
 * @param frac1_word - First fractional part of the feedback divider, which will
 * 		       be returned.
 * @param frac2_word - Second fractional part of the feedback divider, which
 * 		       will be returned.
 * @param mod2_word  - Modulo for the second fractional part of the feedback
 * 		       divider, which will be returned.
 * @return 	     - 0 in case of success, negative error code otherwise.
 */
static int adf4382_pll_fract_n_compute(struct adf4382_dev *dev, uint64_t freq,
				       uint64_t pfd_freq, uint16_t *n_int,
				       uint32_t *frac1_word, uint32_t *frac2_word,
				       uint32_t *mod2_word)
{
	uint64_t rem;
	uint64_t res;

	*n_int = no_os_div64_u64_rem(freq, pfd_freq, &rem);

	res = rem * ADF4382_MOD1WORD;
	*frac1_word = (uint32_t)no_os_div64_u64_rem(res, pfd_freq, &rem);

	*frac2_word = 0;
	*mod2_word = 0;

	if (rem > 0)
		return adf4382_frac2_compute(dev, rem, pfd_freq, frac2_word,
					     mod2_word);

	return 0;
}

/**
 * @brief Set the output frequency.
 * @param dev 	- The device structure.
 * @return    	- 0 in case of success, negative error code otherwise.
 */
int adf4382_set_freq(struct adf4382_dev *dev)
{
	uint32_t frac2_word;
	uint32_t frac1_word;
	uint32_t mod2_word;
	uint8_t clkout_div;
	uint8_t dclk_div1;
	uint64_t pfd_freq;
	uint8_t ldwin_pw = 0;
	uint8_t int_mode;
	uint8_t en_bleed;
	uint8_t locked;
	uint16_t n_int;
	uint8_t div1;
	uint64_t tmp;
	uint64_t vco = 0;
	uint8_t val;
	int ret;

	val = no_os_field_prep(ADF4382_EN_RDBLR_MSK, dev->ref_doubler_en) |
	      no_os_field_prep(ADF4382_R_DIV_MSK, dev->ref_div);
	ret = adf4382_spi_update_bits(dev, 0x20,
				      ADF4382_EN_RDBLR_MSK | ADF4382_R_DIV_MSK,
				      val);
	if (ret)
		return ret;

	for (clkout_div = 0; clkout_div <= dev->clkout_div_reg_val_max; clkout_div++) {
		tmp = (1 << clkout_div) * dev->freq;
		if (tmp < dev->vco_min || tmp > dev->vco_max )
			continue;

		vco = tmp;
		break;
	}

	if (!vco) {
		pr_err("VCO is 0\n");
		return -EINVAL;
	}

	//Calculates the PFD freq. the output will be in KHz
	pfd_freq = adf4382_pfd_compute(dev);

	ret = adf4382_spi_update_bits(dev, 0x1F, ADF4382_CP_I_MSK,
				      no_os_field_prep(ADF4382_CP_I_MSK,
						      dev->cp_i));
	if (ret)
		return ret;

	ret = adf4382_pll_fract_n_compute(dev, dev->freq, pfd_freq, &n_int,
					  &frac1_word, &frac2_word, &mod2_word);
	if (ret)
		return ret;

	if (frac1_word || frac2_word) {
		int_mode = 0;
		en_bleed = 1;

		/*The lock detector pulse window is determined based on the
		PFD frequency as described in the datasheet*/
		if (pfd_freq <= 40*MHZ) {
			ldwin_pw = 7;
		} else if (pfd_freq <= 50*MHZ) {
			ldwin_pw = 6;
		} else if (pfd_freq <= 100*MHZ) {
			ldwin_pw = 5;
		} else if (pfd_freq <= 200*MHZ) {
			ldwin_pw = 4;
		} else if (pfd_freq <= 250*MHZ) {
			if (dev->freq >= 5000U*MHZ &&
			    dev->freq < 6400U*MHZ) {
				ldwin_pw = 3;
			} else {
				ldwin_pw = 2;
			}
		}
	} else {
		int_mode = 1;
		en_bleed = 0;

		tmp = NO_OS_DIV_ROUND_UP(pfd_freq, MICROAMPER_PER_AMPER);
		tmp *= adf4382_ci_ua[dev->cp_i];
		tmp = NO_OS_DIV_ROUND_UP(dev->bleed_word, tmp);
		if (tmp <= 85)
			ldwin_pw = 0;
		else
			ldwin_pw = 1;
	}

	if (frac2_word) {
		ret = adf4382_spi_update_bits(dev, 0x28, ADF4382_VAR_MOD_EN_MSK,
					      0xff);
		if (ret)
			return ret;
	} else {
		ret = adf4382_spi_update_bits(dev, 0x28, ADF4382_VAR_MOD_EN_MSK,
					      0x0);
		if (ret)
			return ret;
	}

	ret = adf4382_spi_update_bits(dev, 0x15, ADF4382_INT_MODE_MSK,
				      no_os_field_prep(ADF4382_INT_MODE_MSK,
						      int_mode));
	if (ret)
		return ret;

	val = dev->bleed_word & ADF4382_FINE_BLEED_LSB_MSK;
	ret = adf4382_spi_write(dev, 0x1D, val);
	if (ret)
		return ret;
	val = (dev->bleed_word >> 8) & ADF4382_BLEED_MSB_MSK;
	ret = adf4382_spi_update_bits(dev, 0x1E, ADF4382_BLEED_MSB_MSK,
				      no_os_field_prep(ADF4382_BLEED_MSB_MSK,
						      val));
	if (ret)
		return ret;

	ret = adf4382_spi_update_bits(dev, 0x1F, ADF4382_EN_BLEED_MSK,
				      no_os_field_prep(ADF4382_EN_BLEED_MSK,
						      en_bleed));
	if (ret)
		return ret;

	val = mod2_word & ADF4382_MOD2WORD_LSB_MSK;
	ret = adf4382_spi_write(dev, 0x1A, val);
	if (ret)
		return ret;
	val = (mod2_word >> 8) & ADF4382_MOD2WORD_MID_MSK;
	ret = adf4382_spi_write(dev, 0x1B, val);
	if (ret)
		return ret;
	val = (mod2_word >> 16) & ADF4382_MOD2WORD_MSB_MSK;
	ret = adf4382_spi_write(dev, 0x1C, val);
	if (ret)
		return ret;

	val = frac2_word  & ADF4382_FRAC2WORD_LSB_MSK;
	ret = adf4382_spi_write(dev, 0x17, val);
	if (ret)
		return ret;
	val = (frac2_word >> 8)  & ADF4382_FRAC2WORD_MID_MSK;
	ret = adf4382_spi_write(dev, 0x18, val);
	if (ret)
		return ret;
	val = (frac2_word >> 16) & ADF4382_FRAC2WORD_MSB_MSK;
	ret = adf4382_spi_write(dev, 0x19, val);
	if (ret)
		return ret;

	val = frac1_word  & ADF4382_FRAC1WORD_LSB_MSK;
	ret = adf4382_spi_write(dev, 0x12, val);
	if (ret)
		return ret;
	val = (frac1_word >> 8)  & ADF4382_FRAC1WORD_MID_MSK;
	ret = adf4382_spi_write(dev, 0x13, val);
	if (ret)
		return ret;
	val = (frac1_word >> 16) & ADF4382_FRAC1WORD_MSB_MSK;
	ret = adf4382_spi_write(dev, 0x14, val);
	if (ret)
		return ret;

	val = (frac1_word >> 24) & ADF4382_FRAC1WORD_MSB;
	ret = adf4382_spi_update_bits(dev, 0x15, ADF4382_FRAC1WORD_MSB, val);
	if (ret)
		return ret;

	dclk_div1 = 2;
	div1 = 8;
	if (pfd_freq <= ADF4382_DCLK_DIV1_0_MAX) {
		dclk_div1 = 0;
		div1 = 1;
	} else if (pfd_freq <= ADF4382_DCLK_DIV1_1_MAX) {
		dclk_div1 = 1;
		div1 = 2;
	}

	ret = adf4382_spi_update_bits(dev, 0x24, ADF4382_DCLK_DIV1_MSK,
				      no_os_field_prep(ADF4382_DCLK_DIV1_MSK,
						      dclk_div1));
	if (ret)
		return ret;

	ret = adf4382_spi_update_bits(dev, 0x31, ADF4382_DCLK_MODE_MSK, 0xff);
	if (ret)
		return ret;

	ret = adf4382_spi_update_bits(dev, 0x31, ADF4382_CAL_CT_SEL_MSK, 0xff);
	if (ret)
		return ret;

	ret = adf4382_spi_update_bits(dev, 0x31, ADF4382_EN_ADC_CLK_MSK, 0xff);
	if (ret)
		return ret;

	//Time for each VCO calibration decision
	ret = adf4382_spi_write(dev, 0x37, ADF4382_VCO_CAL_CNT);
	if (ret)
		return ret;

	//Time for VCO calibration based on Vtune
	ret = adf4382_spi_write(dev, 0x38, ADF4382_VCO_CAL_VTUNE);
	if (ret)
		return ret;

	//VCO automatic level calibration time
	ret = adf4382_spi_write(dev, 0x3A, ADF4382_VCO_CAL_ALC);
	if (ret)
		return ret;

	tmp = NO_OS_DIV_ROUND_UP(no_os_div_u64(pfd_freq, div1 * 400000) - 2, 4);
	tmp = no_os_clamp(tmp, 0U, 255U);
	ret = adf4382_spi_write(dev, 0x3E, tmp);
	if (ret)
		return ret;

	// Set LD COUNT
	ret = adf4382_spi_update_bits(dev, 0x2C, ADF4382_LD_COUNT_OPWR_MSK,
				      dev->ld_count);
	if (ret)
		return ret;

	ret = adf4382_spi_update_bits(dev, 0x2C, ADF4382_LDWIN_PW_MSK,
				      no_os_field_prep(ADF4382_LDWIN_PW_MSK,
						      ldwin_pw));
	if (ret)
		return ret;

	ret = adf4382_spi_update_bits(dev, 0x11, ADF4382_CLKOUT_DIV_MSK,
				      no_os_field_prep(ADF4382_CLKOUT_DIV_MSK,
						      clkout_div));
	if (ret)
		return ret;

	val = (n_int >> 8) & ADF4382_N_INT_MSB_MSK;
	ret = adf4382_spi_update_bits(dev, 0x11, ADF4382_N_INT_MSB_MSK, val);
	if (ret)
		return ret;

	// Need to set N_INT last to trigger an auto-calibration
	val = n_int & ADF4382_N_INT_LSB_MSK;
	ret = adf4382_spi_write(dev, 0x10, val);
	if (ret)
		return ret;

	no_os_udelay(ADF4382_LKD_DELAY_US);
	ret = adf4382_spi_read(dev, 0x58, &val);
	if (ret)
		return ret;

	locked = no_os_field_get(val, ADF4382_LOCKED_MSK);
	if (!locked)
		return -EIO;

	return 0;
}

/**
 * @brief Set the phase adjustment in pico-seconds. The phase adjust will
 * enable the Bleed current option as well as delay mode to 0.
 * @param dev 		- The device structure.
 * @param phase_ps 	- The phase adjustment in pico-seconds.
 * @return    		- 0 in case of success, negative error code otherwise.
 */
int adf4382_set_phase_adjust(struct adf4382_dev *dev, uint32_t phase_ps)
{
	uint64_t phase_reg_value;
	uint32_t rfout_deg_ns;
	uint32_t phase_deg_ns;
	uint64_t rfout_deg_s;
	uint64_t phase_bleed;
	uint16_t phase_deg;
	uint64_t phase_ci;
	uint64_t pfd_freq;
	int ret;

	ret = adf4382_spi_update_bits(dev, 0x1E, ADF4382_EN_PHASE_RESYNC_MSK, 0xff);
	if (ret)
		return ret;

	ret = adf4382_spi_update_bits(dev, 0x1F, ADF4382_EN_BLEED_MSK, 0xff);
	if (ret)
		return ret;

	ret = adf4382_spi_update_bits(dev, 0x32, ADF4382_DEL_MODE_MSK, 0x0);
	if (ret)
		return ret;

	dev->phase_adj = phase_ps;

	//Determine the output freq. in degrees/s
	rfout_deg_s = 360 * dev->freq;
	//Convert it to degrees/ns
	rfout_deg_ns = no_os_div_u64(rfout_deg_s, S_TO_NS);
	//Determine the phase adjustment in degrees relative the output freq.
	phase_deg_ns = rfout_deg_ns * phase_ps;
	//Convert it to degrees/ps
	phase_deg = no_os_div_u64(phase_deg_ns, NS_TO_PS);

	if (phase_deg > 360) {
		pr_err("Phase Adjustment cannot exceed 360deg per Clock Period\n");
		return EINVAL;
	}

	//Phase adjustment can only be done if bleed is active, and a bleed
	//constant needs to be added
	phase_bleed = phase_deg * ADF4382_PHASE_BLEED_CNST;
	//The charge pump current will also need to be taken in to account
	phase_ci = phase_bleed * adf4382_ci_ua[dev->cp_i];
	phase_ci = no_os_div_u64(phase_ci, MICROAMPER_PER_AMPER);

	//Computation of the register value for the phase adjust
	pfd_freq = adf4382_pfd_compute(dev);
	phase_reg_value = no_os_div_u64((phase_ci * pfd_freq), (360 * dev->freq));

	if (phase_reg_value > 255)
		phase_reg_value -= 255;

	ret = adf4382_spi_write(dev, 0x33, phase_reg_value);
	if (ret)
		return ret;

	return adf4382_spi_update_bits(dev, 0x34, ADF4382_PHASE_ADJ_MSK, 0xff);
}

/**
 * @brief Set the phase polarity. If pol = 0 then it will add the phase value
 * otherwise it will subtract the phase value.
 * @param dev 		- The device structure.
 * @param polarity 	- The polarity to be set.
 * @return    		- Result of the writing procedure, error code otherwise.
 */
int adf4382_set_phase_pol(struct adf4382_dev *dev, bool polarity)
{
	uint8_t pol;

	pol = no_os_field_prep(ADF4382_PHASE_ADJ_POL_MSK, polarity);
	return adf4382_spi_update_bits(dev, 0x32, ADF4382_PHASE_ADJ_POL_MSK, pol);
}

/**
	uint8_t pol;
 * @brief Gets the polarity of the phase adjust.
 * @param dev 		- The device structure.
 * @param polarity 	- The read polarity of the phase.
 * @return    		- Result of the tesint procedure, negative error code
 * 			  otherwise.
 */
int adf4382_get_phase_pol(struct adf4382_dev *dev, bool *polarity)
{
	uint8_t tmp;
	int ret;

	ret = adf4382_spi_read(dev, 0x32, &tmp);
	if (ret)
		return ret;

	*polarity = no_os_field_get(tmp, ADF4382_PHASE_ADJ_POL_MSK);
	return 0;
}

/**
 * @brief ADF4382 SPI Scratchpad check.
 * @param dev 	- The device structure.
 * @return 	- 0 in case of success or negative error code.
 */
static int adf4382_check_scratchpad(struct adf4382_dev *dev)
{
	uint8_t scratchpad;
	int ret;

	ret = adf4382_spi_write(dev, 0x00A, ADF4382_SPI_SCRATCHPAD_TEST);
	if (ret)
		return ret;

	ret = adf4382_spi_read(dev, 0x00A, &scratchpad);
	if (ret)
		return ret;

	if (scratchpad != ADF4382_SPI_SCRATCHPAD_TEST)
		return -EINVAL;

	return 0;
}

/**
 * @brief Initializes the ADF4382.
 * @param dev	     - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return 	     - 0 in case of success or negative error code.
 */
int adf4382_init(struct adf4382_dev **dev,
		 struct adf4382_init_param *init_param)
{
	struct adf4382_dev *device;
	bool en = true;
	uint8_t i;
	int ret;

	device = (struct adf4382_dev *)no_os_calloc(1, sizeof(*device));
	if (!device)
		return -ENOMEM;

	ret = no_os_spi_init(&device->spi_desc, init_param->spi_init);
	if (ret)
		goto error_dev;

	device->spi_3wire_en = init_param->spi_3wire_en;
	device->cmos_3v3 = init_param->cmos_3v3;
	device->ref_freq_hz = init_param->ref_freq_hz;
	device->freq = init_param->freq;
	device->ref_doubler_en = init_param->ref_doubler_en;
	device->ref_div = init_param->ref_div;
	device->cp_i = init_param->cp_i;
	device->bleed_word = init_param->bleed_word;
	device->ld_count = init_param->ld_count;
	device->phase_adj = 0;

	switch (init_param->id) {
	case ID_ADF4382:
		device->freq_max = ADF4382_RFOUT_MAX;
		device->freq_min = ADF4382_RFOUT_MIN;
		device->vco_max = ADF4382_VCO_FREQ_MAX;
		device->vco_min = ADF4382_VCO_FREQ_MIN;
		device->clkout_div_reg_val_max = ADF4382_CLKOUT_DIV_REG_VAL_MAX;
		break;
	case ID_ADF4382A:
		device->freq_max = ADF4382A_RFOUT_MAX;
		device->freq_min = ADF4382A_RFOUT_MIN;
		device->vco_max = ADF4382A_VCO_FREQ_MAX;
		device->vco_min = ADF4382A_VCO_FREQ_MIN;
		device->clkout_div_reg_val_max = ADF4382A_CLKOUT_DIV_REG_VAL_MAX;
		break;
	default:
		goto error_spi;
	}

	ret = adf4382_spi_write(device, 0x00, ADF4382_RESET_CMD);
	if (ret)
		goto error_spi;

	no_os_udelay(ADF4382_POR_DELAY_US);

	if (device->spi_3wire_en)
		en = false;

	/* SPI set to 4 wire */
	ret = adf4382_spi_write(device, 0x00, ADF4382_SPI_3W_CFG(en));
	if (ret)
		goto error_spi;

	ret = adf4382_spi_write(device, 0x3D,
				no_os_field_prep(ADF4382_CMOS_OV_MSK, device->cmos_3v3));
	if (ret)
		goto error_spi;

	ret = adf4382_check_scratchpad(device);
	if (ret)
		goto error_spi;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adf4382_reg_defaults); i++) {
		ret = adf4382_spi_write(device,
					adf4382_reg_defaults[i].reg,
					adf4382_reg_defaults[i].val);
		if (ret)
			goto error_spi;
	}

	ret = adf4382_set_freq(device);
	if (ret)
		goto error_spi;

	ret = adf4382_set_out_power(device, 0, 9);
	if (ret)
		goto error_spi;
	ret = adf4382_set_out_power(device, 1, 9);
	if (ret)
		goto error_spi;
	*dev = device;

	return ret;
error_spi:
	no_os_spi_remove(device->spi_desc);
error_dev:
	no_os_free(device);
	return ret;
}

/**
 * @brief Free resources allocated for ADF4382
 * @param dev 	- The device structure.
 * @return 	- 0 in case of success or negative error code.
 */
int adf4382_remove(struct adf4382_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		no_os_free(dev);

	return 0;
}
