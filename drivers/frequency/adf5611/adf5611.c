/**
 *   @file   adf5611.c
 *   @brief  Implementation of adf5611 Driver.
 *   @author Jude Osemene (jude.osemene@analog.com)
 *******************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include "adf5611.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"

//Charge pump current values expressed in uA
static const int adf5611_cp_ua[] = {
	200,
	400,
	600,
	800,
	1000,
	1200,
	1400,
	1600,
	1800,
	2000,
	2200,
	2400,
	2600,
	2800,
	3000,
	3200,
};

/**
 * @brief Writes data to ADF5611 over SPI.
 * @param dev	   - The device structure.
 * @param reg_addr - The register address.
 * @param data 	   - Data value to write.
 * @return 0 in case of success or negative error code otherwise.
 */
int adf5611_spi_write(struct adf5611_dev *dev, uint16_t reg_addr, uint8_t data)
{
	uint8_t buff[ADF5611_BUFF_SIZE_BYTES];
	uint16_t cmd;

	cmd = ADF5611_SPI_WRITE_CMD | reg_addr;
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
					ADF5611_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ADF5611 over SPI.
 * @param dev 	   - The device structure.
 * @param reg_addr - The register address.
 * @param data	   - Data read from the device.
 * @return 0 in case of success or negative error code otherwise.
 */
int adf5611_spi_read(struct adf5611_dev *dev, uint16_t reg_addr, uint8_t *data)
{
	int ret;
	uint8_t buff[ADF5611_BUFF_SIZE_BYTES];
	uint16_t cmd;

	cmd = ADF5611_SPI_READ_CMD | reg_addr;
	if (dev->spi_desc->bit_order) {
		buff[0] = no_os_bit_swap_constant_8(cmd & 0xFF);
		buff[1] = no_os_bit_swap_constant_8(cmd >> 8);
		buff[2] = no_os_bit_swap_constant_8(ADF5611_SPI_DUMMY_DATA);
	} else {
		buff[0] = cmd >> 8;
		buff[1] = cmd & 0xFF;
		buff[2] = ADF5611_SPI_DUMMY_DATA;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buff,
				       ADF5611_BUFF_SIZE_BYTES);
	if (ret)
		return ret;

	*data = buff[2];

	return 0;
}

/**
 * @brief Updates the values of the ADF5611 register.
 * @param dev 	   - The device structure.
 * @param reg_addr - The register address.
 * @param mask 	   - Bits to be updated.
 * @param data 	   - Update value for the mask.
 * @return 0 in case of success or negative error code otherwise.
 */
int adf5611_spi_update_bits(struct adf5611_dev *dev, uint16_t reg_addr,
			    uint8_t mask, uint8_t data)
{
	uint8_t tmp, orig;
	int ret;

	ret = adf5611_spi_read(dev, reg_addr, &orig);
	if (ret)
		return ret;

	tmp = orig & ~mask;
	tmp |= data & mask;

	if (tmp != orig)
		return adf5611_spi_write(dev, reg_addr, tmp);

	return 0;
}

/**
 * @brief Set the desired reference frequency and reset everything over to
 * maximum supported value of 300MHz to the max. value and everything under the
 * minimum supported value of 50MHz to the min. value.
 * @param dev 		- The device structure.
 * @param val		- The desired reference frequency in Hz.
 * @return 0 in case of success or negative error code.
 */
int adf5611_set_ref_clk(struct adf5611_dev *dev, uint64_t val)
{
	if (!dev)
		return -EINVAL;

	dev->ref_clk_freq = val;

	if (val > ADF5611_REF_CLK_MAX)
		dev->ref_clk_freq = ADF5611_REF_CLK_MAX;

	if (val < ADF5611_REF_CLK_MIN)
		dev->ref_clk_freq = ADF5611_REF_CLK_MIN;

	return adf5611_set_freq(dev);
}

/**
 * @brief Gets the user proposed reference frequency
 * @param dev 		- The device structure.
 * @param val		- The reference frequency in Hz.
 * @return 0 in case of success or negative error code.
 */
int adf5611_get_ref_clk(struct adf5611_dev *dev, uint64_t *val)
{
	if (!dev)
		return -EINVAL;

	*val = dev->ref_clk_freq;
	return 0;
}

/**
 * @brief Set the reference divider value and reset everything over to maximum
 * supported value of 60 to the max value
 * @param dev 		- The device structure.
 * @param div	 	- The reference divider value.
 * @return set frequency function in case of success or negative error code.
 */
int adf5611_set_ref_div(struct adf5611_dev *dev, int32_t div)
{
	if (!dev)
		return -EINVAL;

	dev->ref_div = div;

	if (div > ADF5611_REF_DIV_MAX)
		dev->ref_div = ADF5611_REF_DIV_MAX;

	return adf5611_set_freq(dev);
}

/**
 * @brief Gets the value the reference divider.
 * @param dev 		- The device structure.
 * @param div	 	- The reference divider value.
 * @return 0 in case of success or negative error code.
 */
int adf5611_get_ref_div(struct adf5611_dev *dev, int32_t *div)
{
	uint8_t tmp_lsb;
	uint8_t tmp_msb;
	uint8_t ret;

	ret = adf5611_spi_read(dev, 0x1F, &tmp_lsb);
	if (ret)
		return ret;

	ret = adf5611_spi_read(dev, 0x20, &tmp_msb);
	if (ret)
		return ret;

	dev->ref_div = no_os_field_get(ADF5611_R_DIV_MSB_MSK, tmp_msb) << 8
		       | no_os_field_get(ADF5611_R_DIV_LSB_MSK, tmp_lsb);
	*div = dev->ref_div;

	return 0;
}

/**
 * @brief Set the charge pump value which will be written to the register. The
 * value will be between 0 and 15 on 8 bits. For more information please
 * consult the Datasheet.
 * @param dev 		- The device structure.
 * @param reg_val 	- The desired charge pump register value.
 * @return set frequency function in case of success or negative
 * error code.
 */
int adf5611_set_cp_i(struct adf5611_dev *dev, int32_t reg_val)
{
	dev->cp_i = (uint8_t)reg_val;

	if (reg_val > ADF5611_CPI_VAL_MAX)
		dev->cp_i = ADF5611_CPI_VAL_MAX;

	return adf5611_set_freq(dev);
}

/**
 * @brief Gets the charge pump value from the register. The value will be
 * between 0 and 15 on 8 bits. For more information please consult the
 * Datasheet.
 * @param dev 		- The device structure.
 * @param reg_val 	- The charge pump register value.
 * @return 0 in case of success or negative error code.
 */
int adf5611_get_cp_i(struct adf5611_dev *dev, int32_t *reg_val)
{
	uint8_t tmp_cp_i;
	uint8_t ret;

	ret = adf5611_spi_read(dev, 0x21, &tmp_cp_i);
	if (ret)
		return ret;

	dev->cp_i = no_os_field_get(ADF5611_CP_I_MSK, tmp_cp_i);
	*reg_val = dev->cp_i;

	return 0;
}

/**
 * @brief Computes the PFD frequency and returns the value in Hz.
 * @param dev 	     - The device structure.
 * @return Returns PFD value in Hz.
 */
static uint64_t adf5611_pfd_compute(struct adf5611_dev *dev)
{
	uint64_t pfd_freq;

	pfd_freq = NO_OS_DIV_U64(dev->ref_clk_freq, dev->ref_div);

	return pfd_freq;
}

/**
 * @brief Set the rfoutput power register value and reset everything over to
 * maximum supported value of 3 to the max. value.
 * @param dev 		- The device structure.
 * @param pwr	 	- The output power register value.
 * @return Result of the writing procedure, error code otherwise.
 */
int adf5611_set_output_power(struct adf5611_dev *dev, int8_t pwr)
{
	uint8_t tmp;

	if (pwr > ADF5611_RFOUT_PWR_MAX)
		pwr = ADF5611_RFOUT_PWR_MAX;

	tmp = no_os_field_prep(ADF5611_RFOUT_PWR_MSK, pwr);
	return adf5611_spi_update_bits(dev, 0x22, ADF5611_RFOUT_PWR_MSK, tmp);
}

/**
 * @brief Gets the rfoutput power register value.
 * @param dev 		- The device structure.
 * @param pwr	 	- The output power register value.
 * @return 0 in case of success or negative error code.
 */
int adf5611_get_output_power(struct adf5611_dev *dev, int8_t *pwr)
{
	uint8_t tmp;
	int ret;

	ret = adf5611_spi_read(dev, 0x22, &tmp);
	if (ret)
		return ret;

	*pwr = no_os_field_get(ADF5611_RFOUT_PWR_MSK, tmp);

	return 0;
}

/**
 * @brief Set the rfoutdiv power register value and reset everything over to
 * maximum supported value of 3 to the max. value.
 * @param dev 		- The device structure.
 * @param pwr	 	- The rfoutdiv power register value.
 * @return Result of the writing procedure, error code otherwise.
 */
int adf5611_set_rfoutdiv_power(struct adf5611_dev *dev, int32_t pwr)
{
	uint8_t tmp;

	if (pwr > ADF5611_RFOUTDIV_PWR_MAX)
		pwr = ADF5611_RFOUTDIV_PWR_MAX;

	tmp = no_os_field_prep(ADF5611_DIV_PWR_MSK, pwr);
	return adf5611_spi_update_bits(dev, 0x22, ADF5611_DIV_PWR_MSK, tmp);
}

/**
 * @brief Gets the rfoutdiv power register value.
 * @param dev 		- The device structure.
 * @param pwr	 	- The rfoutdiv power register value.
 * @return 0 in case of success or negative error code.
 */
int adf5611_get_rfoutdiv_power(struct adf5611_dev *dev, int32_t *pwr)
{

	uint8_t tmp;
	int ret;

	ret = adf5611_spi_read(dev, 0x22, &tmp);
	if (ret)
		return ret;

	*pwr = no_os_field_get(ADF5611_DIV_PWR_MSK, tmp);

	return 0;
}

/**
 * @brief Set the rfoutdiv frequency divider register value and reset everything
 * over to maximum supported value of 128 to the max. value.
 * @param dev 		- The device structure.
 * @param div_val 	- The rfoutdiv divider register value.
 * @return Result of the writing procedure, error code otherwise.
 */
int adf5611_set_rfoutdiv_divider(struct adf5611_dev *dev, uint8_t div_val)
{
	uint8_t tmp;
	if (!dev)
		return -EINVAL;

	dev->rfoutdiv_div = (uint8_t) div_val;

	if (div_val > ADF5611_RFOUTDIV_DIV_MAX)
		dev->rfoutdiv_div = ADF5611_RFOUTDIV_DIV_MAX;

	tmp = no_os_field_prep(ADF5611_RFOUT_DIV_MSK, dev->rfoutdiv_div);
	return adf5611_spi_update_bits(dev, 0x22, ADF5611_RFOUT_DIV_MSK, tmp);
}

/**
 * @brief Gets the rfoutdiv divider register value.
 * @param dev 		- The device structure.
 * @param div	 	- The rfoutdiv divider register value.
 * @return 0 in case of success or negative error code.
 */
int adf5611_get_rfoutdiv_divider(struct adf5611_dev *dev, int8_t *div)
{

	uint8_t tmp;
	int ret;

	ret = adf5611_spi_read(dev, 0x22, &tmp);
	if (ret)
		return ret;

	dev->rfoutdiv_div = no_os_field_get(ADF5611_RFOUT_DIV_MSK, tmp);
	*div = dev->rfoutdiv_div;

	return 0;
}

/**
 * @brief Sets the rfoutdiv output block to enable or disable. 1 means the block
 * is powered down else block is powered up.
 * @param dev 		- The device structure.
 * @param en		- Enable/Disable rfoutdiv output.
 * @return Result of the writing procedure, error code otherwise.
 */
int adf5611_set_en_rfoutdiv(struct adf5611_dev *dev, bool en)
{
	uint8_t enable;

	enable = no_os_field_prep(ADF5611_PD_ODIV_MSK, en);
	return adf5611_spi_update_bits(dev, 0x28, ADF5611_PD_ODIV_MSK, enable);
}

/**
 * @brief Gets the value of rfoutdiv output block if enable or disable.
 * @param dev 		- The device structure.
 * @param en		- The status of rfoutdiv output block.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf5611_get_en_rfoutdiv(struct adf5611_dev *dev, bool *en)
{
	uint8_t tmp;
	bool enable;
	int ret;

	ret = adf5611_spi_read(dev, 0x28, &tmp);
	if (ret)
		return ret;

	enable = no_os_field_get(tmp, ADF5611_PD_ODIV_MSK);
	*en = enable;

	return 0;
}

/**
 * @brief ADF5611 SPI Scratchpad check.
 * @param dev 	- The device structure.
 * @return 0 in case of success or negative error code.
 */
static int adf5611_check_scratchpad(struct adf5611_dev *dev)
{
	uint8_t scratchpad;
	int ret;

	ret = adf5611_spi_write(dev, 0x00A, ADF5611_SPI_SCRATCHPAD_TEST);
	if (ret)
		return ret;

	ret = adf5611_spi_read(dev, 0x00A, &scratchpad);
	if (ret)
		return ret;

	if (scratchpad != ADF5611_SPI_SCRATCHPAD_TEST)
		return -EINVAL;

	return 0;
}

/**
 * @brief Set the desired output frequency and reset everything over to maximum
 * supported value of 14.6GHz to the max. value and everything under the minimum
 * supported value of 7.3GHz to the min. value.
 * @param dev 		- The device structure.
 * @param val	 	- The desired output frequency in Hz.
 * @return set frequency function in case of success or negative
 * error code.
 */
int adf5611_set_rfout(struct adf5611_dev *dev, uint64_t val)
{
	dev->rfout_freq = val;

	if (val > ADF5611_RFOUT_MAX)
		dev->rfout_freq = ADF5611_RFOUT_MAX;

	if (val < ADF5611_RFOUT_MIN)
		dev->rfout_freq = ADF5611_RFOUT_MIN;

	return adf5611_set_freq(dev);
}

/**
 * @brief Gets the user proposed output frequency
 * @param dev 		- The device structure.
 * @param val	 	- The output frequency in Hz.
 * @return 0 in case of success or negative error code.
 */
int adf5611_get_rfout(struct adf5611_dev *dev, uint64_t *val)
{
	uint32_t frac1 = 0;
	uint32_t frac2 = 0;
	uint32_t mod2 = 0;
	uint64_t freq;
	uint64_t pfd;
	uint8_t tmp;
	uint32_t n;
	int ret;

	pfd = adf5611_pfd_compute(dev);

	ret = adf5611_spi_read(dev, 0x12, &tmp);
	if (ret)
		return ret;

	n = tmp  & ADF5611_N_INT_MSB_MSK;
	n = n << 8;

	ret = adf5611_spi_read(dev, 0x11, &tmp);
	if (ret)
		return ret;
	n |= tmp & ADF5611_N_INT_MID_MSK;
	n = n << 8;

	ret = adf5611_spi_read(dev, 0x10, &tmp);
	if (ret)
		return ret;
	n |= tmp & ADF5611_N_INT_LSB_MSK;

	// Get FRAC1WORD
	ret = adf5611_spi_read(dev, 0x15, &tmp);
	if (ret)
		return ret;
	frac1 = tmp & ADF5611_FRAC1WORD_MSB_MSK;
	frac1 = frac1 << 8;

	ret = adf5611_spi_read(dev, 0x14, &tmp);
	if (ret)
		return ret;
	frac1 |= tmp;
	frac1 = frac1 << 8;

	ret = adf5611_spi_read(dev, 0x13, &tmp);
	if (ret)
		return ret;
	frac1 |= tmp;
	frac1 = frac1 << 4;

	ret = adf5611_spi_read(dev, 0x12, &tmp);
	if (ret)
		return ret;
	frac1 |= tmp & ADF5611_FRAC1WORD_LSB_MSK;

	// Get FRAC2WORD
	ret = adf5611_spi_read(dev, 0x19, &tmp);
	if (ret)
		return ret;
	frac2 = tmp;
	frac2 = frac2 << 8;

	ret = adf5611_spi_read(dev, 0x18, &tmp);
	if (ret)
		return ret;
	frac2 |= tmp;
	frac2 = frac2 << 8;

	ret = adf5611_spi_read(dev, 0x17, &tmp);
	if (ret)
		return ret;
	frac2 |= tmp;

	// get MOD2WORD
	ret = adf5611_spi_read(dev, 0x1c, &tmp);
	if (ret)
		return ret;
	mod2 = tmp;
	mod2 = mod2 << 8;

	ret = adf5611_spi_read(dev, 0x1b, &tmp);
	if (ret)
		return ret;
	mod2 |= tmp;
	mod2 = mod2 << 8;

	ret = adf5611_spi_read(dev, 0x1a, &tmp);
	if (ret)
		return ret;
	mod2 |= tmp;

	freq = frac2 * pfd;
	freq = no_os_div_u64(freq, mod2);
	freq = freq + (frac1 * pfd);
	freq = no_os_div_u64(freq, ADF5611_MOD1WORD);
	freq = (freq + (n * pfd)) * ADF5611_OUTPUT_DOUBLER;

	*val = freq;
	return 0;
}

/**
 * @brief Compute the output frequency register values.
 * @param f_vco 	- The vco frequency.
 * @param pfd_freq 	- The pfd frequency.
 * @param n_int 	- The N integer value.
 * @param frac1_word 	- The frac1word value .
 * @param frac2_word 	- The frac2word value.
 * @param mod2_word 	- mod2_word value
 * @return 0 in case of success, negative error code otherwise.
 */
static int adf5611_frac_compute(uint64_t f_vco, uint64_t pfd_freq,
				uint16_t *n_int,
				uint32_t *frac1_word, uint32_t *frac2_word,
				uint32_t *mod2_word)
{
	uint64_t rem;
	uint64_t res;
	uint32_t mod2_max;
	uint64_t mod2_wd;
	uint32_t mod2_tmp;
	uint32_t channel_spacing;
	uint32_t chsp_freq;
	uint32_t gcd;
	channel_spacing = 1;
	mod2_wd = 1;

	mod2_max = ADF5611_MOD2WORD_MAX;

	do {
		chsp_freq = channel_spacing * ADF5611_MOD1WORD;
		gcd = no_os_greatest_common_divisor(chsp_freq, pfd_freq);
		mod2_tmp = NO_OS_DIV_ROUND_UP(pfd_freq, gcd);
		if (mod2_tmp > mod2_max) {
			channel_spacing *= 5;
		} else {
			mod2_wd = mod2_tmp;
			break;
		}
	} while (channel_spacing < ADF5611_CHANNEL_SPACING_MAX);

	*n_int = no_os_div64_u64_rem(f_vco, pfd_freq, &rem);

	res = rem * ADF5611_MOD1WORD;
	*frac1_word = (uint32_t)no_os_div64_u64_rem(res, pfd_freq, &rem);
	*frac2_word = 0;
	*mod2_word = 0;

	if (rem > 0) {
		mod2_wd *= NO_OS_DIV_U64(mod2_max, mod2_wd);
		*frac2_word = NO_OS_DIV_ROUND_CLOSEST_ULL(res * mod2_wd, pfd_freq);
		*mod2_word = mod2_wd;
	}
	return 0;
}

/**
 * @brief Set the output frequency.
 * @param dev 	- The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adf5611_set_freq(struct adf5611_dev *dev)
{
	uint64_t tmp_bleedtime;
	uint16_t cal_vtune_to;
	uint16_t cal_count_to;
	uint16_t cal_vco_to;
	uint32_t frac2_word;
	uint32_t frac1_word;
	uint32_t mod2_word;
	uint64_t pfd_freq;
	uint8_t ldwin_pw;
	uint8_t int_mode;
	uint8_t en_bleed;
	uint64_t tmp_div;
	uint8_t locked;
	uint16_t n_int;
	uint64_t f_vco;
	uint64_t freq;
	uint8_t val;
	int ret;

	freq = dev->rfout_freq;
	val = dev->ref_div & ADF5611_R_DIV_LSB_MSK;
	ret = adf5611_spi_write(dev, 0x1F, val);

	// write R_DIV Register
	val = (dev->ref_div >> 8) & ADF5611_R_DIV_MSB_MSK;
	ret = adf5611_spi_update_bits(dev, 0x20, ADF5611_R_DIV_MSB_MSK, val);
	if (ret)
		return ret;

	f_vco = NO_OS_DIV_U64(freq, ADF5611_OUTPUT_DOUBLER);

	if (f_vco == 0) {
		pr_err("VCO is 0\n");
	}

	// Compute the PFD Frequency
	pfd_freq = adf5611_pfd_compute(dev);

	// Write Charge pump value
	ret = adf5611_spi_update_bits(dev, 0x21, ADF5611_CP_I_MSK,
				      no_os_field_prep(ADF5611_CP_I_MSK,
						      dev->cp_i));
	if (ret)
		return ret;

	// adf5611_frac_compute function to be continued
	ret = adf5611_frac_compute(f_vco, pfd_freq, &n_int, &frac1_word,
				   &frac2_word, &mod2_word);
	if (ret)
		return ret;

	if (frac1_word || frac2_word) {
		int_mode = 0;
		en_bleed = 1;
		ldwin_pw = 3;

		// Bleed Calculation
		tmp_bleedtime = ADF5611_BLEED_TIME_CONST * pfd_freq *
				adf5611_cp_ua[dev->cp_i] * KHZ;
		tmp_div = freq * ADF5611_BLEED_CURRENT;
		val = ADF5611_BLEED_I_MSK & NO_OS_DIV_ROUND_CLOSEST_ULL(tmp_bleedtime,
				tmp_div);
		ret = adf5611_spi_write(dev, 0x1D, val);
		if (ret)
			return ret;
	} else {
		int_mode = 1;
		en_bleed = 0;
		ldwin_pw = 1;
	}
	if (frac2_word) {
		ret = adf5611_spi_update_bits(dev, 0x26, ADF5611_VAR_MOD_EN_MSK, 0xff);
		if (ret)
			return ret;
	} else {
		ret = adf5611_spi_update_bits(dev, 0x26, ADF5611_VAR_MOD_EN_MSK, 0x0);
		if (ret)
			return ret;
	}
	// Write INT_MODE value
	ret = adf5611_spi_update_bits(dev, 0x21, ADF5611_INTMODE_EN_MSK,
				      no_os_field_prep(ADF5611_INTMODE_EN_MSK,
						      int_mode)); // monitor this behavior
	if (ret)
		return ret;
	// Write EN_BLEED value
	ret = adf5611_spi_update_bits(dev, 0x1E, ADF5611_EN_BLEED_MSK,
				      no_os_field_prep(ADF5611_EN_BLEED_MSK,
						      en_bleed));
	if (ret)
		return ret;
	// Write MOD2_WORD to register
	val = mod2_word & ADF5611_MOD2WORD_LSB_MSK;
	ret = adf5611_spi_write(dev, 0x1A, val);
	if (ret)
		return ret;
	val = (mod2_word >> 8) & ADF5611_MOD2WORD_MID_MSK;
	ret = adf5611_spi_write(dev, 0x1B, val);
	if (ret)
		return ret;
	val = (mod2_word >> 16) & ADF5611_MOD2WORD_MSB_MSK;
	ret = adf5611_spi_write(dev, 0x1C, val);
	if (ret)
		return ret;

	// Write FRAC2_WORD to register
	val = frac2_word & ADF5611_FRAC2WORD_LSB_MSK;
	ret = adf5611_spi_write(dev, 0x17, val);
	if (ret)
		return ret;
	val = (frac2_word >> 8) & ADF5611_FRAC2WORD_MID_MSK;
	ret = adf5611_spi_write(dev, 0x18, val);
	if (ret)
		return ret;
	val = (frac2_word >> 16) & ADF5611_FRAC2WORD_MSB_MSK;
	ret = adf5611_spi_write(dev, 0x19, val);
	if (ret)
		return ret;

	// Write FRAC1_WORD to register
	val = frac1_word & ADF5611_FRAC1WORD_LSB_MSK;
	ret = adf5611_spi_write(dev, 0x12, val);
	if (ret)
		return ret;
	val = (frac1_word >> 4) & ADF5611_FRAC1WORD_MID_MSK;
	ret = adf5611_spi_write(dev, 0x13, val);
	if (ret)
		return ret;
	val = (frac1_word >> 12) & ADF5611_FRAC1WORD_MID_MSB_MSK;
	ret = adf5611_spi_write(dev, 0x14, val);
	if (ret)
		return ret;
	val = (frac1_word >> 20) & ADF5611_FRAC1WORD_MSB_MSK;
	ret = adf5611_spi_write(dev, 0x15, val);
	if (ret)
		return ret;

	// Write LD_COUNT value
	ret = adf5611_spi_update_bits(dev, 0x29, ADF5611_LD_COUNT_MSK,
				      dev->ld_count);
	if (ret)
		return ret;

	// Write LD_WINDOW value
	ret = adf5611_spi_update_bits(dev, 0x29, ADF5611_LDWIN_PW_MSK,
				      no_os_field_prep(ADF5611_LDWIN_PW_MSK,
						      ldwin_pw));
	if (ret)
		return ret;

	// VCO Calibration data write
	cal_vtune_to = NO_OS_DIV_ROUND_UP(pfd_freq, MHZ);
	val = cal_vtune_to & ADF5611_CAL_VTUNE_TO_LSB_MSK;
	ret = adf5611_spi_write(dev, 0x30, val);
	if (ret)
		return ret;
	val = (cal_vtune_to >> 8) & ADF5611_CAL_VTUNE_TO_MSB_MSK;
	ret = adf5611_spi_update_bits(dev, 0x31, ADF5611_CAL_VTUNE_TO_MSB_MSK,
				      val);
	if (ret)
		return ret;

	cal_count_to = NO_OS_DIV_ROUND_UP(pfd_freq, 2 * MHZ);
	val = cal_count_to & ADF5611_CAL_COUNT_TO_MSK;
	ret = adf5611_spi_update_bits(dev, 0x2f, ADF5611_CAL_COUNT_TO_MSK, val);
	if (ret)
		return ret;

	cal_vco_to = NO_OS_DIV_ROUND_UP(2 * pfd_freq, MHZ);
	val = cal_vco_to & ADF5611_CAL_VCO_TO_LSB_MSK;
	ret = adf5611_spi_write(dev, 0x32, val);
	if (ret)
		return ret;
	val = (cal_vco_to >> 8) & ADF5611_CAL_VCO_TO_MSB_MSK;
	ret = adf5611_spi_update_bits(dev, 0x33, ADF5611_CAL_VCO_TO_MSB_MSK,
				      val);
	if (ret)
		return ret;

	// Handle N_INT values
	val = (n_int >> 16) & ADF5611_N_INT_MSB_MSK;
	ret = adf5611_spi_update_bits(dev, 0x12, ADF5611_N_INT_MSB_MSK, val);
	if (ret)
		return ret;

	val = (n_int >> 8) & ADF5611_N_INT_MID_MSK;
	ret = adf5611_spi_update_bits(dev, 0x11, ADF5611_N_INT_MID_MSK, val);
	if (ret)
		return ret;

	// Write Register 0x10 to trigger auto calibration
	val = n_int & ADF5611_N_INT_LSB_MSK;
	ret = adf5611_spi_write(dev, 0x10, val);
	if (ret)
		return ret;

	no_os_udelay(ADF5611_LKD_DELAY_US);
	ret = adf5611_spi_read(dev, 0x48, &val);
	if (ret)
		return ret;

	locked = no_os_field_get(val, ADF5611_LOCKED_MSK);
	if (!locked)
		return -EIO;

	return 0;
}

/**
 * @brief Initialize the ADF5611.
 * @param dev		- The device structure.
 * @param init_param	- The structure containing the device initial parameters.
 * @return 0 in case of success or negative error code
 */
int adf5611_init(struct adf5611_dev **dev,
		 struct adf5611_init_param *init_param)
{
	struct adf5611_dev *device;
	uint8_t i;
	int ret;

	device = (struct adf5611_dev *)no_os_calloc(1, sizeof(*device));
	if (!device)
		return -ENOMEM;

	ret = no_os_spi_init(&device->spi_desc, init_param->spi_init);
	if (ret)
		goto error_dev;

	device->spi4wire = init_param->spi4wire;
	device->cmos_3v3 = init_param->cmos_3v3;
	device->ref_clk_freq = init_param->ref_clk_freq;
	device->rfout_freq = init_param->rfout_freq;
	device->ref_div = init_param->ref_div;
	device->cp_i = init_param->cp_i;
	device->ld_count = init_param->ld_count;
	device->rfoutdiv_div = init_param->rfoutdiv_div;

	switch (init_param->id) {
	case ID_ADF5611:
		device->freq_max = ADF5611_RFOUT_MAX;
		device->freq_min = ADF5611_RFOUT_MIN;
		break;
	case ID_ADF5612:
		device->freq_max = ADF5612_RFOUT_MAX;
		device->freq_min = ADF5612_RFOUT_MIN;
		break;
	default:
		goto error_spi;
	}

	ret = adf5611_spi_write(device, 0x00, ADF5611_RESET_CMD);
	if (ret)
		goto error_spi;

	no_os_udelay(ADF5611_POR_DELAY_US);

	/* Setup SPI for 4 Wire */
	ret = adf5611_spi_write(device, 0x00, ADF5611_SPI_4W_CFG(device->spi4wire));
	if (ret)
		goto error_spi;

	ret = adf5611_spi_write(device, 0x35, ADF5611_CMOS_OV(device->cmos_3v3));
	if (ret)
		goto error_spi;

	/* Run test to confirm SPI works by checking Scratchpad */
	ret = adf5611_check_scratchpad(device);
	if (ret)
		goto error_spi;

	/* Write Default Registers */
	for (i = 0; i < NO_OS_ARRAY_SIZE(adf5611_reg_defaults); i++) {
		ret = adf5611_spi_write(device, adf5611_reg_defaults[i].reg,
					adf5611_reg_defaults[i].val);
		if (ret)
			goto error_spi;
	}

	ret = adf5611_set_freq(device);
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
 * @brief Free resources allocated for ADF5611
 * @param dev 	- The device structure.
 * @return 0 in case of success or negative error code.
 */
int adf5611_remove(struct adf5611_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		no_os_free(dev);

	return 0;
}
