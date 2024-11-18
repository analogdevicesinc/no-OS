/***************************************************************************//**
 *   @file   adf4368.c
 *   @brief  Implementation of ADF4368 Driver.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
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

#include "adf4368.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

/**
 * @struct adf4368_reg_sequence
 * @brief ADF4368 register format structure for default values
 */
static struct adf4368_reg_sequence {
	uint16_t reg;
	uint8_t val;
};

/**
 * @struct adf4368_reg_defaults
 * @brief ADF4368 register initialization
 */
static const struct adf4368_reg_sequence adf4368_reg_defaults[] = {

	{ 0x054, 0x00 },
	{ 0x053, 0x25 },
	{ 0x052, 0x00 },
	{ 0x051, 0x00 },
	{ 0x050, 0x00 },
	{ 0x04f, 0x00 },
	{ 0x04e, 0x10 },
	{ 0x04d, 0x00 },
	{ 0x04c, 0x2B },
	{ 0x04b, 0x5D },
	{ 0x04a, 0x00 },
	{ 0x048, 0x00 },
	{ 0x047, 0x00 },
	{ 0x046, 0x00 },
	{ 0x045, 0x08 },
	{ 0x044, 0x18 },
	{ 0x043, 0x09 },
	{ 0x042, 0x09 },
	{ 0x041, 0x00 },
	{ 0x040, 0x00 },
	{ 0x03f, 0x83 },
	{ 0x03e, 0x26 },
	{ 0x03d, 0xC0 },
	{ 0x03c, 0x00 },
	{ 0x03b, 0x8C },
	{ 0x03a, 0x00 },
	{ 0x039, 0xB0 },
	{ 0x038, 0x00 },
	{ 0x037, 0x3E },
	{ 0x036, 0xD6 },
	{ 0x035, 0x04 },
	{ 0x034, 0x99 },
	{ 0x033, 0x32 },
	{ 0x032, 0xD3 },
	{ 0x031, 0x69 },
	{ 0x030, 0x1B },
	{ 0x02f, 0xA7 },
	{ 0x02e, 0x00 },
	{ 0x02d, 0xF1 },
	{ 0x02c, 0x4C },
	{ 0x02b, 0x03 },
	{ 0x02a, 0x10 },
	{ 0x029, 0xDD },
	{ 0x028, 0x20 },
	{ 0x027, 0x28 },
	{ 0x026, 0x80 },
	{ 0x025, 0x00 },
	{ 0x024, 0x00 },
	{ 0x023, 0x80 },
	{ 0x022, 0x00 },
	{ 0x021, 0x09 },
	{ 0x020, 0xC1 },
	{ 0x01f, 0x1E },
	{ 0x01e, 0x28 },
	{ 0x01d, 0xDD },
	{ 0x01c, 0xFF },
	{ 0x01b, 0xFF },
	{ 0x01a, 0xFF },
	{ 0x019, 0x55 },
	{ 0x018, 0x55 },
	{ 0x017, 0x55 },
	{ 0x016, 0x00 },
	{ 0x015, 0x00 },
	{ 0x014, 0x43 },
	{ 0x013, 0x55 },
	{ 0x012, 0x55 },
	{ 0x011, 0x00 },
	{ 0x010, 0x2B },
};

/* Charge pump current values expressed in uA */
static const int adf4368_ci_ua[] = {
	790,
	990,
	1190,
	1380,
	1590,
	1980,
	2390,
	2790,
	3180,
	3970,
	4770,
	5570,
	6330,
	7910,
	9510,
	11100
};

/**
 * @brief Writes data to ADF4368 over SPI.
 * @param dev	   - The device structure.
 * @param reg_addr - The register address.
 * @param data 	   - Data value to write.
 * @return 	   - 0 in case of success or negative error code otherwise.
 */
int adf4368_spi_write(struct adf4368_dev *dev, uint16_t reg_addr, uint8_t data)
{
	uint8_t buff[ADF4368_BUFF_SIZE_BYTES];
	uint16_t cmd;

	if (!dev)
		return -EINVAL;

	cmd = ADF4368_SPI_WRITE_CMD | reg_addr;
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
					ADF4368_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ADF4368 over SPI.
 * @param dev 	   - The device structure.
 * @param reg_addr - The register address.
 * @param data	   - Data read from the device.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adf4368_spi_read(struct adf4368_dev *dev, uint16_t reg_addr, uint8_t *data)
{
	uint8_t buff[ADF4368_BUFF_SIZE_BYTES];
	uint16_t cmd;
	int ret;

	if (!dev)
		return -EINVAL;

	cmd = ADF4368_SPI_READ_CMD | reg_addr;
	if (dev->spi_desc->bit_order) {
		buff[0] = no_os_bit_swap_constant_8(cmd & 0xFF);
		buff[1] = no_os_bit_swap_constant_8(cmd >> 8);
		buff[2] = no_os_bit_swap_constant_8(ADF4368_SPI_DUMMY_DATA);
	} else {
		buff[0] = cmd >> 8;
		buff[1] = cmd & 0xFF;
		buff[2] = ADF4368_SPI_DUMMY_DATA;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buff,
				       ADF4368_BUFF_SIZE_BYTES);
	if (ret)
		return ret;

	if (dev->spi_desc->bit_order)
		*data = no_os_bit_swap_constant_8(buff[2]);
	else
		*data = buff[2];

	return 0;
}

/**
 * @brief Updates the values of the ADF4368 register.
 * @param dev 	   - The device structure.
 * @param reg_addr - The register address.
 * @param mask 	   - Bits to be updated.
 * @param data 	   - Update value for the mask.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adf4368_spi_update_bits(struct adf4368_dev *dev, uint16_t reg_addr,
			    uint8_t mask, uint8_t data)
{
	uint8_t tmp, orig;
	int ret;

	ret = adf4368_spi_read(dev, reg_addr, &orig);
	if (ret)
		return ret;

	tmp = orig & ~mask;
	tmp |= data & mask;

	if (tmp != orig)
		return adf4368_spi_write(dev, reg_addr, tmp);

	return 0;
}

/**
 * @brief Will output on the terminal the values of all the ADF4368 registers.
 * @param dev 	- The device structure.
 * @return 	- 0 in case of success or negative error code.
 */
int adf4368_reg_dump(struct adf4368_dev *dev)
{
	uint8_t val;
	uint16_t i;
	int ret;

	if (!dev)
		return -EINVAL;

	for (i = 0; i < 0x63; i++) {
		ret = adf4368_spi_read(dev, i, &val);
		if (ret)
			return ret;

		pr_info("0x%X    0x%X\n", i, val);
	}

	return ret;
}

/**
 * @brief ADF4368 SPI Scratchpad check.
 * @param dev 	- The device structure.
 * @return 	- 0 in case of success or negative error code.
 */
static int adf4368_check_scratchpad(struct adf4368_dev *dev)
{
	uint8_t scratchpad;
	int ret;

	ret = adf4368_spi_write(dev, 0x00A, ADF4368_SPI_SCRATCHPAD_TEST);
	if (ret)
		return ret;

	ret = adf4368_spi_read(dev, 0x00A, &scratchpad);
	if (ret)
		return ret;

	if (scratchpad != ADF4368_SPI_SCRATCHPAD_TEST)
		return -EINVAL;

	return 0;
}

/**
 * @brief Computes the PFD frequency and returns the value in Hz.
 * @param dev 	     - The device structure.
 * @return 	     - PFD value in Hz.
 */
static uint64_t adf4368_pfd_compute(struct adf4368_dev *dev)
{
	uint64_t pfd_freq;

	pfd_freq = NO_OS_DIV_ROUND_CLOSEST(dev->ref_freq_hz, dev->ref_div);
	if (dev->ref_doubler_en)
		pfd_freq *=  2;

	return pfd_freq;
}

/**
 * @brief Calculates and Sets the Bleed word.
 * @param dev 	     - The device structure.
 * @return 	     - 0 in case of success or negative error code.
 */
static int adf4368_optimize_bleed_word(struct adf4368_dev *dev)
{

	uint32_t coars_bleed;
	uint32_t fine_bleed;
	uint64_t pfd_freq;
	uint64_t t_bleed; // in ps
	uint64_t bleed_i;
	bool bleed_pol;
	uint64_t rem;
	uint8_t val;
	int ret;

	pfd_freq = adf4368_pfd_compute(dev);

	/* t_bleed and bleed_pol LUT */
	if (pfd_freq >= 120000000UL) {
		/* 4.2GHz > RFout */
		if (dev->freq > 4200000000UL) {
			t_bleed = 390;
			bleed_pol = false;
		}
		/* 4.2GHz < RFout < 3GHz */
		else if (dev->freq > 3000000000UL) {
			t_bleed = 900;
			bleed_pol = false;
		}
		/* 3GHz < RFout < 1.8GHz */
		else if (dev->freq > 1800000000UL) {
			t_bleed = 1200;
			bleed_pol = false;
		}
		/* dev->freq < 1.8 GHz */
		else {
			t_bleed = 1400;
			bleed_pol = false;
		}
	} else {

		if (dev->freq > 4200000000UL) {
			t_bleed = 390;
			bleed_pol = false;
		}
		/* 4.2GHz < RFout < 3GHz */
		else if (dev->freq > 3000000000UL) {
			t_bleed = dev->n_int < ADF4368_BLEED_N_INT_TH ?
				  900 : 1200;
			bleed_pol = dev->n_int < ADF4368_BLEED_N_INT_TH ?
				    false : true;
		}
		/* 3GHz < RFout < 1.8GHz */
		else if (dev->freq > 1800000000UL) {
			t_bleed = 1200;
			bleed_pol = dev->n_int < ADF4368_BLEED_N_INT_TH ?
				    true : false;
		}
		/* 1.8GHz < RFout < 1.2GHz */
		else if (dev->freq > 1200000000UL) {
			t_bleed = 1400;
			bleed_pol = dev->n_int < ADF4368_BLEED_N_INT_TH ?
				    true : false;
		}
		/* dev->freq < 1.2 GHz */
		else {
			t_bleed = dev->n_int < ADF4368_BLEED_N_INT_TH ?
				  2000 : 1400;
			bleed_pol = dev->n_int < ADF4368_BLEED_N_INT_TH ?
				    true : false;
		}

	}

	/* Calculate Bleed Current */
	bleed_i = t_bleed * (pfd_freq * adf4368_ci_ua[dev->cp_i]);

	bleed_i = NO_OS_DIV_ROUND_UP(bleed_i, ADF4368_PS_TO_S);

	coars_bleed = no_os_div64_u64_rem(bleed_i, ADF4368_COARSE_BLEED_CNST, &rem);

	fine_bleed = rem * ADF4368_FINE_BLEED_CNST2;
	fine_bleed = NO_OS_DIV_ROUND_UP(fine_bleed, ADF4368_FINE_BLEED_CNST1);

	dev->bleed_word = (coars_bleed << 9) | fine_bleed;

	/* Set Bleed Pol */
	ret =  adf4368_spi_update_bits(dev, 0x1F,
				       ADF4368_BLEED_POL_MSK,
				       no_os_field_prep(ADF4368_BLEED_POL_MSK, bleed_pol));
	if (ret)
		return ret;

	val = dev->bleed_word & ADF4368_FINE_BLEED_LSB_MSK;
	ret = adf4368_spi_write(dev, 0x1D, val);
	if (ret)
		return ret;

	val = (dev->bleed_word >> 8) & ADF4368_BLEED_MSB_MSK;
	return adf4368_spi_update_bits(dev, 0x1E, ADF4368_BLEED_MSB_MSK,
				       no_os_field_prep(ADF4368_BLEED_MSB_MSK,
						       val));
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
static int adf4368_frac2_compute(struct adf4368_dev *dev, uint64_t res,
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

	ret = adf4368_spi_read(dev, 0x1E, &tmp);
	if (ret)
		return ret;

	en_phase_resync = no_os_field_get(tmp, ADF4368_EN_PHASE_RESYNC_MSK);;

	if (en_phase_resync)
		mod2_max = ADF4368_PHASE_RESYNC_MOD2WORD_MAX;
	else
		mod2_max = ADF4368_MOD2WORD_MAX;

	do {

		chsp_freq = channel_spacing * ADF4368_MOD1WORD;
		gcd = no_os_greatest_common_divisor(chsp_freq, pfd_freq);
		mod2_tmp = NO_OS_DIV_ROUND_UP(pfd_freq, gcd);

		if (mod2_tmp > mod2_max) {
			channel_spacing *= 5;
		} else {
			mod2_wd = mod2_tmp;
			break;
		}

	} while (channel_spacing < ADF4368_CHANNEL_SPACING_MAX);

	if (!en_phase_resync)
		mod2_wd *= NO_OS_DIV_U64(mod2_max, mod2_wd);

	*frac2_word = NO_OS_DIV_ROUND_CLOSEST_ULL(res * mod2_wd, pfd_freq);
	*mod2_word = mod2_wd;

	return 0;
}

/**
 * @brief Computes the bleed word for a given phase adjustment value.
 * @param dev 	     - The device structure.
 * @param phase_ps 	- The phase adjustment in femto-seconds.
 * @param bleed_pol 	- The bleed current pol.
 * @return 	     - Calculated new bleed word value.
 */
static int32_t adf4368_compute_bleed_word(struct adf4368_dev *dev,
		uint32_t phase_fs, bool bleed_pol)
{

	int32_t delta_coarse_b;
	int32_t new_bleed_word;
	int32_t delta_fine_b;
	int64_t phase_tmp;
	uint64_t pfd_freq;
	uint64_t rem;

	if (!dev)
		return -EINVAL;

	pfd_freq = adf4368_pfd_compute(dev);

	// Cpi(uA) * pfd_freq(Hz) = uA*Hz
	phase_tmp = adf4368_ci_ua[dev->cp_i] * (pfd_freq);

	// Hz*A = uA*Hz / 10^6
	phase_tmp = no_os_div_u64(phase_tmp, MEGA);

	// (Delta Bleed Current Value) = Hz*A*fs = fA
	phase_tmp = phase_tmp * phase_fs;

	// fA / 10^6 = nA
	phase_tmp = no_os_div_u64(phase_tmp, MEGA);

	// delta_coarse_bleed = nA / (uA*10^3)
	delta_coarse_b = no_os_div64_u64_rem(phase_tmp,
					     ADF4368_COARSE_BLEED_CNST * KILO, &rem);

	delta_coarse_b = bleed_pol ? -delta_coarse_b : delta_coarse_b;

	// delta_fine_bleed = nA / (nA)
	delta_fine_b = no_os_div_u64(rem, ADF4368_FINE_BLEED_CNST3);
	delta_fine_b = bleed_pol ? -delta_fine_b : delta_fine_b;

	// Update the bleed word with new fine bleed value
	new_bleed_word = (dev->bleed_word & ((ADF4368_FINE_BLEED_MSB_MSK << 8) |
					     ADF4368_FINE_BLEED_LSB_MSK)) + delta_fine_b;

	// fine bleed overflow adjustment
	if (new_bleed_word > 0x1FF) {
		new_bleed_word -= ((ADF4368_COARSE_BLEED_CNST * KILO) /
				   ADF4368_FINE_BLEED_CNST3);
		delta_coarse_b++;
	} else if (new_bleed_word < 0) {

		new_bleed_word += ((ADF4368_COARSE_BLEED_CNST * KILO) /
				   ADF4368_FINE_BLEED_CNST3);
		delta_coarse_b--;
	}

	// Update the new bleed word with updated coarse bleed value
	new_bleed_word += ((dev->bleed_word >> 9) + (delta_coarse_b)) << 9;

	return new_bleed_word;
}

/**
 * @brief Calculates and sets the LD Win in INT Mode.
 * @param dev 	     - The device structure.
 * @return 	     - 0 in case of success or negative error code.
 */
static int adf4368_optimize_ldwin(struct adf4368_dev *dev)
{
	uint64_t pfd_freq;
	uint8_t ldwin_pw;
	uint8_t tmp;

	//Calculates the PFD freq. the output will be in KHz
	pfd_freq = adf4368_pfd_compute(dev);

	/* Calculate LD window */
	tmp = NO_OS_DIV_ROUND_UP(pfd_freq, MICROAMPER_PER_AMPER);
	tmp *= adf4368_ci_ua[dev->cp_i];
	tmp = NO_OS_DIV_ROUND_UP(dev->bleed_word, tmp);
	if (tmp <= 85)
		ldwin_pw = 0;
	else
		ldwin_pw = 1;

	return adf4368_spi_update_bits(dev, 0x2C, ADF4368_LDWIN_PW_MSK,
				       no_os_field_prep(ADF4368_LDWIN_PW_MSK,
						       ldwin_pw));
}

/**
 * @brief Set the output power register value of a channel and reset everything
 * over to maximum supported value of 15 to the max. value.
 * @param dev 		- The device structure.
 * @param ch	 	- The channel to set the power off.
 * @param pwr	 	- The output power register value.
 * @return    		- Result of the writing procedure, error code otherwise.
 */
int adf4368_set_out_power(struct adf4368_dev *dev, uint8_t ch, int32_t pwr)
{
	uint8_t tmp;

	if (!dev)
		return -EINVAL;

	if (pwr > ADF4368_OUT_PWR_MAX)
		pwr = ADF4368_OUT_PWR_MAX;

	if (!ch) {
		tmp = no_os_field_prep(ADF4368_CLK1_OPWR_MSK, pwr);
		return adf4368_spi_update_bits(dev, 0x29, ADF4368_CLK1_OPWR_MSK,
					       tmp);
	}

	tmp = no_os_field_prep(ADF4368_CLK2_OPWR_MSK, pwr);
	return adf4368_spi_update_bits(dev, 0x29, ADF4368_CLK2_OPWR_MSK, tmp);
}

/**
 * @brief Gets the output power register value.
 * @param dev 		- The device structure.
 * @param ch	 	- The channel to get the power off.
 * @param pwr	 	- The output power register value.
 * @return    		- Result of the reading procedure, error code otherwise.
 */
int adf4368_get_out_power(struct adf4368_dev *dev, uint8_t ch, int32_t *pwr)
{

	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_read(dev, 0x29, &tmp);
	if (ret)
		return ret;

	if (!ch)
		*pwr = no_os_field_get(ADF4368_CLK1_OPWR_MSK, tmp);
	else
		*pwr = no_os_field_get(ADF4368_CLK2_OPWR_MSK, tmp);

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
int adf4368_set_en_chan(struct adf4368_dev *dev, uint8_t ch, bool en)
{
	uint8_t enable;

	if (!dev)
		return -EINVAL;

	if (!ch) {
		enable = no_os_field_prep(ADF4368_PD_CLKOUT1_MSK, !en);
		return adf4368_spi_update_bits(dev, 0x2B,
					       ADF4368_PD_CLKOUT1_MSK,
					       enable);
	}

	enable = no_os_field_prep(ADF4368_PD_CLKOUT2_MSK, !en);
	return adf4368_spi_update_bits(dev, 0x2B, ADF4368_PD_CLKOUT2_MSK,
				       enable);
}

/**
 * @brief Gets the value the output channel if it is enabled or disable.
 * @param dev 		- The device structure.
 * @param ch	 	- The channel to get state.
 * @param en	 	- The status of the output channel.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_get_en_chan(struct adf4368_dev *dev, uint8_t ch, bool *en)
{
	uint8_t tmp;
	bool enable;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_read(dev, 0x2B, &tmp);
	if (ret)
		return ret;

	if (!ch)
		enable = no_os_field_get(tmp, ADF4368_PD_CLKOUT1_MSK);
	else
		enable = no_os_field_get(tmp, ADF4368_PD_CLKOUT2_MSK);

	*en = !enable;

	return 0;
}

/**
 * @brief Set the desired reference frequency and reset everything over to maximum
 * supported value of 4GHz to the max. value and everything under the minimum
 * supported value of 10MHz to the min. value.
 * @param dev 		- The device structure.
 * @param val		- The desired reference frequency in Hz.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_set_ref_clk(struct adf4368_dev *dev, uint64_t val)
{
	if (!dev)
		return -EINVAL;

	dev->ref_freq_hz = val;

	if (val > ADF4368_REF_CLK_MAX)
		dev->ref_freq_hz = ADF4368_REF_CLK_MAX;

	if (val < ADF4368_REF_CLK_MIN)
		dev->ref_freq_hz = ADF4368_REF_CLK_MIN;

	return adf4368_set_freq(dev);
}

/**
 * @brief Gets the user proposed reference frequency
 * @param dev 		- The device structure.
 * @param val		- The set value of the reference frequency in Hz.
 * @return    		- Reference frequency in KHz.
 */
int adf4368_get_ref_clk(struct adf4368_dev *dev, uint64_t *val)
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
int adf4368_set_en_ref_doubler(struct adf4368_dev *dev, bool en)
{
	if (!dev)
		return -EINVAL;

	dev->ref_doubler_en = en;

	return adf4368_set_freq(dev);
}

/**
 * @brief Gets the value the doubler if it is enabled or disable and stores it
 * it the dev structure.
 * @param dev 		- The device structure.
 * @param en		- The read value of the reference doubler.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_get_en_ref_doubler(struct adf4368_dev *dev, bool *en)
{
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_read(dev, 0x20, &tmp);
	if (ret)
		return ret;

	dev->ref_doubler_en = no_os_field_get(tmp, ADF4368_EN_RDBLR_MSK);
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
int adf4368_set_ref_div(struct adf4368_dev *dev, int32_t div)
{
	if (!dev)
		return -EINVAL;

	dev->ref_div = div;

	if (div > ADF4368_REF_DIV_MAX)
		dev->ref_div = ADF4368_REF_DIV_MAX;

	return adf4368_set_freq(dev);
}

/**
 * @brief Gets the value the reference divider.
 * @param dev 		- The device structure.
 * @param div	 	- The read reference divider value.
 * @return    		- Result of the reading procedure, error code otherwise.
 */
int adf4368_get_ref_div(struct adf4368_dev *dev, int32_t *div)
{
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_read(dev, 0x20, &tmp);
	if (ret)
		return ret;

	dev->ref_div = no_os_field_get(ADF4368_R_DIV_MSK, tmp);
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
int adf4368_set_cp_i(struct adf4368_dev *dev, int32_t reg_val)
{
	uint8_t en_bleed;
	uint8_t int_mode;
	uint8_t val;
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	dev->cp_i = (uint8_t)reg_val;

	if (reg_val > ADF4368_CPI_VAL_MAX)
		dev->cp_i = ADF4368_CPI_VAL_MAX;

	ret = adf4368_spi_update_bits(dev, 0x1F, ADF4368_CP_I_MSK,
				      no_os_field_prep(ADF4368_CP_I_MSK,
						      dev->cp_i));
	if (ret)
		return ret;

	/* Read Bleed En */
	ret = adf4368_spi_read(dev, 0x1F, &tmp);
	if (ret)
		return ret;
	en_bleed = no_os_field_get(ADF4368_EN_BLEED_MSK, tmp);

	/* Read Int Mode */
	ret = adf4368_spi_read(dev, 0x11, &tmp);
	if (ret)
		return ret;
	int_mode = no_os_field_get(ADF4368_INT_MODE_MSK, tmp);

	if (en_bleed) {
		if (int_mode) {
			ret = adf4368_optimize_ldwin(dev);
			if (ret)
				return ret;
		} else {
			ret = adf4368_optimize_bleed_word(dev);
			if (ret)
				return ret;
		}
	}

	// Need to set N_INT last to write Double Buffered Registers
	val = dev->n_int & ADF4368_N_INT_LSB_MSK;
	return adf4368_spi_write(dev, 0x10, val);
}

/**
 * @brief Gets the charge pump value from the register. The value will be
 * between 0 and 15 on 8 bits. For more information please consult the
 * Datasheet.
 * @param dev 		- The device structure.
 * @param reg_val 	- The read charge pump register value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_get_cp_i(struct adf4368_dev *dev, int32_t *reg_val)
{
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_read(dev, 0x1F, &tmp);
	if (ret)
		return ret;

	dev->cp_i = no_os_field_get(ADF4368_CP_I_MSK, tmp);
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
int adf4368_set_bleed_word(struct adf4368_dev *dev, int32_t word)
{
	uint8_t int_mode;
	uint8_t val;
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	dev->bleed_word = (uint16_t)word;

	if (word > ADF4368_BLEED_WORD_MAX)
		dev->bleed_word = ADF4368_BLEED_WORD_MAX;

	val = dev->bleed_word & ADF4368_FINE_BLEED_LSB_MSK;
	ret = adf4368_spi_write(dev, 0x1D, val);
	if (ret)
		return ret;

	val = (dev->bleed_word >> 8) & ADF4368_BLEED_MSB_MSK;
	ret = adf4368_spi_update_bits(dev, 0x1E, ADF4368_BLEED_MSB_MSK,
				      no_os_field_prep(ADF4368_BLEED_MSB_MSK,
						      val));
	if (ret)
		return ret;

	/* Read Int Mode */
	ret = adf4368_spi_read(dev, 0x11, &tmp);
	if (ret)
		return ret;
	int_mode = no_os_field_get(ADF4368_INT_MODE_MSK, tmp);

	if (int_mode) {

		/* Enable Bleed */
		ret = adf4368_spi_update_bits(dev, 0x1F,
					      ADF4368_EN_BLEED_MSK, 0xff);
		if (ret)
			return ret;

		ret = adf4368_optimize_ldwin(dev);
		if (ret)
			return ret;
	}

	// Need to set N_INT last to write Double Buffered Registers
	val = dev->n_int & ADF4368_N_INT_LSB_MSK;
	return adf4368_spi_write(dev, 0x10, val);
}

/**
 * @brief Gets the value of the bleed word.
 * @param dev 		- The device structure.
 * @param word	 	- The read bleed current register value.
 * @return    		- Result of the reading procedure, error code otherwise.
 */
int adf4368_get_bleed_word(struct adf4368_dev *dev, int32_t *word)
{
	uint8_t upper;
	uint8_t lower;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_read(dev, 0x1E, &upper);
	if (ret)
		return ret;

	upper &= (ADF4368_COARSE_BLEED_MSK | ADF4368_FINE_BLEED_MSB_MSK);

	ret = adf4368_spi_read(dev, 0x1D, &lower);
	if (ret)
		return ret;

	dev->bleed_word = (upper << 8) | lower;
	*word = dev->bleed_word;

	return 0;
}

/**
 * @brief Set the desired output frequency and reset everything over to maximum
 * supported value of 12.8GHz to the max. value and
 * everything under the minimum supported value of 800MHz to the min. value.
 * @param dev 		- The device structure.
 * @param val	 	- The desired output frequency in Hz.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_set_rfout(struct adf4368_dev *dev, uint64_t val)
{
	if (!dev)
		return -EINVAL;

	dev->freq = val;

	if (val > dev->freq_max)
		dev->freq = dev->freq_max;

	if (val < dev->freq_min)
		dev->freq = dev->freq_min;

	return adf4368_set_freq(dev);
}

/**
 * @brief Gets the user proposed output frequency
 * @param dev 		- The device structure.
 * @param val		- The value of the output frequency in Hz.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_get_rfout(struct adf4368_dev *dev, uint64_t *val)
{
	uint32_t frac1 = 0;
	uint32_t frac2 = 0;
	uint32_t mod2 = 0;
	uint16_t n_value;
	uint64_t freq;
	uint64_t pfd;
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	pfd = adf4368_pfd_compute(dev);

	ret = adf4368_spi_read(dev, 0x11, &tmp);
	if (ret)
		return ret;
	n_value = tmp & ADF4368_N_INT_MSB_MSK;
	n_value = n_value << 8;

	ret = adf4368_spi_read(dev, 0x10, &tmp);
	if (ret)
		return ret;
	n_value |= tmp;

	ret = adf4368_spi_read(dev, 0x15, &tmp);
	if (ret)
		return ret;
	frac1 = tmp & ADF4368_FRAC1WORD_MSB;
	frac1 = frac1 << 8;

	ret = adf4368_spi_read(dev, 0x14, &tmp);
	if (ret)
		return ret;
	frac1 |= tmp;
	frac1 = frac1 << 8;

	ret = adf4368_spi_read(dev, 0x13, &tmp);
	if (ret)
		return ret;
	frac1 |= tmp;
	frac1 = frac1 << 8;

	ret = adf4368_spi_read(dev, 0x12, &tmp);
	if (ret)
		return ret;
	frac1 |= tmp;

	ret = adf4368_spi_read(dev, 0x19, &tmp);
	if (ret)
		return ret;
	frac2 = tmp;
	frac2 = frac2 << 8;

	ret = adf4368_spi_read(dev, 0x18, &tmp);
	if (ret)
		return ret;
	frac2 |= tmp;
	frac2 = frac2 << 8;

	ret = adf4368_spi_read(dev, 0x17, &tmp);
	if (ret)
		return ret;
	frac2 |= tmp;

	ret = adf4368_spi_read(dev, 0x1c, &tmp);
	if (ret)
		return ret;
	mod2 = tmp;
	mod2 = mod2 << 8;

	ret = adf4368_spi_read(dev, 0x1b, &tmp);
	if (ret)
		return ret;
	mod2 |= tmp;
	mod2 = mod2 << 8;

	ret = adf4368_spi_read(dev, 0x1a, &tmp);
	if (ret)
		return ret;
	mod2 |= tmp;

	freq = frac2 * pfd;
	freq = no_os_div_u64(freq, mod2);
	freq = freq + (frac1 * pfd);
	freq = no_os_div_u64(freq, ADF4368_MOD1WORD);
	freq = freq + (n_value * pfd);

	*val = freq;
	return 0;
}

/**
 * @brief Set EZSYNC and Timed SYNC features' initial state. Waits for SW_SYNC
 * toggle or SYNC pin.
 * @param dev 		- The device structure.
 * @param en	 	- The enable or disable SYNC feature.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_set_sync_setup(struct adf4368_dev *dev, bool en)
{
	uint64_t pfd_freq;
	uint8_t delay;
	uint8_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	if (en) {

		ret = adf4368_spi_update_bits(dev, 0x2A, ADF4368_PD_SYNC, 0);
		if (ret)
			return ret;

		ret = adf4368_spi_update_bits(dev, 0x53,
					      ADF4368_SYNC_SEL_MSK, 0xff);
		if (ret)
			return ret;

		ret = adf4368_spi_update_bits(dev, 0x1E,
					      (ADF4368_EN_REF_RST_MSK | ADF4368_TIMED_SYNC_MSK
					       | ADF4368_EN_PHASE_RESYNC_MSK), 0xff);
		if (ret)
			return ret;

		ret = adf4368_spi_update_bits(dev, 0x2D,
					      ADF4368_EN_DRCLK_MSK, 0xff);
		if (ret)
			return ret;

		// Determine the Digital Delays
		pfd_freq = adf4368_pfd_compute(dev);
		if (pfd_freq >= 200 * ADF4368_MHZ) {
			delay = 3;
		} else if (pfd_freq >= 175 * ADF4368_MHZ) {
			delay = 4;
		} else if (pfd_freq < 175 * ADF4368_MHZ) {
			delay = 7;
		}

		if (pfd_freq < 70 * ADF4368_MHZ && dev->n_int >= ADF4368_BLEED_N_INT_TH) {
			delay = 0;
		}

		ret = adf4368_spi_update_bits(dev, 0x31,
					      ADF4368_SYNC_DEL_MSK,
					      no_os_field_prep(ADF4368_SYNC_DEL_MSK, delay));
		if (ret)
			return ret;

		val = no_os_field_prep(ADF4368_DRCLK_DEL_MSK, delay) |
		      no_os_field_prep(ADF4368_DNCLK_DEL_MSK, delay);
		return adf4368_spi_update_bits(dev, 0x30,
					       ADF4368_DRCLK_DEL_MSK | ADF4368_DNCLK_DEL_MSK, val);

	} else
		/* Power Down SYNC */
		return adf4368_spi_update_bits(dev, 0x2A, ADF4368_PD_SYNC, 0xff);

	return 0;
}

/**
 * @brief Gets the value of the SYNC features' powerdown bit.
 * @param dev 		- The device structure.
 * @param en		- The read value of the SYNC features' powerdown.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_get_sync_setup(struct adf4368_dev *dev, bool *en)
{
	uint8_t pd_sync;
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_read(dev, 0x2A, &tmp);
	if (ret)
		return ret;
	pd_sync = no_os_field_get(tmp, ADF4368_PD_SYNC);

	*en = (!pd_sync);

	return 0;
}

/**
 * @brief Set Software SYNC Request. Setting SW_SYNC resets the RF block.
 * Clearing SW_SYNC makes ready for a new reference clock.
 * @param dev 		- The device structure.
 * @param sw_sync 	- Set send SW_SYNC request
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_set_sw_sync(struct adf4368_dev *dev, uint8_t sw_sync)
{
	uint8_t tmp;

	if (!dev)
		return -EINVAL;

	tmp = no_os_field_prep(ADF4368_SW_SYNC_MSK, sw_sync);
	return adf4368_spi_update_bits(dev, 0x1F, ADF4368_SW_SYNC_MSK, tmp);
}

/**
 * @brief Gets the value of the SW_SYNC bit.
 * @param dev 		- The device structure.
 * @param sw_sync	- The read value of the SW_SYNC.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_get_sw_sync(struct adf4368_dev *dev, uint8_t *sw_sync)
{
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_read(dev, 0x1F, &tmp);
	if (ret)
		return ret;
	*sw_sync = no_os_field_get(tmp, ADF4368_SW_SYNC_MSK);

	return 0;
}

/**
 * @brief Set Temperature Readback feature's initial state.
 * This feature should be disabled after reading temperature.
 * @param dev 		- The device structure.
 * @param en	 	- The enable or disable Temperature readback feature.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_set_temperature(struct adf4368_dev *dev, bool en)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (en) {

		ret = adf4368_spi_update_bits(dev, 0x2D,
					      ADF4368_EN_DRCLK_MSK, 0xff);
		if (ret)
			return ret;

		ret = adf4368_spi_update_bits(dev, 0x31,
					      ADF4368_EN_ADC_CLK_MSK, 0xff);
		if (ret)
			return ret;
		ret = adf4368_spi_update_bits(dev, 0x54,
					      ADF4368_ADC_ST_CNV_MSK, 0xff);
		if (ret)
			return ret;

	} else {
		ret = adf4368_spi_update_bits(dev, 0x54,
					      ADF4368_ADC_ST_CNV_MSK, 0x0);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Gets the value of the approximate die temperature.
 * @param dev 		- The device structure.
 * @param temp		- The read value of the Temperature Readback.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4368_get_temperature(struct adf4368_dev *dev, int32_t *temp)
{
	int32_t temperature;
	uint8_t sign;
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_read(dev, 0x5B, &tmp);
	if (ret)
		return ret;
	temperature = tmp;

	ret = adf4368_spi_read(dev, 0x5C, &tmp);
	if (ret)
		return ret;

	sign = no_os_field_get(tmp, ADF4368_CHIP_TEMP_MSB_MSK);

	*temp = sign ? (-temperature) : temperature;

	return 0;
}

/**
 * @brief Set the phase adjustment in femto-seconds with Sigma Delta Modulation.
 *  This approach only support MAX %22 in degree phase.
 *  Recommend to use in fractional mode.
 * @param dev 		- The device structure.
 * @param phase_fs 	- The phase adjustment in femto-seconds.
 * @return    		- 0 in case of success, negative error code otherwise.
 */
int adf4368_set_phase_sdm(struct adf4368_dev *dev, uint32_t phase_fs)
{
	uint8_t phase_reg_value;
	uint64_t rfout_mhz;
	uint64_t phase_ns;
	uint64_t phase_s;
	int ret;

	if (!dev)
		return -EINVAL;

	rfout_mhz = no_os_div_u64(dev->freq, ADF4368_MHZ);

	phase_ns = rfout_mhz * phase_fs * ADF4368_SIGMA_DELTA_MOD_CNST;

	phase_s = no_os_div_u64(phase_ns, ADF4368_NS_TO_S);

	if (phase_s > 0xff)
		return -EINVAL;

	phase_reg_value = phase_s;

	ret = adf4368_spi_update_bits(dev, 0x1E, ADF4368_EN_PHASE_RESYNC_MSK, 0xff);
	if (ret)
		return ret;

	ret = adf4368_spi_write(dev, 0x24, phase_reg_value);
	if (ret)
		return ret;

	// Toggle PHSAE_ADJ
	ret = adf4368_spi_update_bits(dev, 0x1F, ADF4368_PHASE_ADJ_MSK, 0xff);
	if (ret)
		return ret;

	return adf4368_spi_update_bits(dev, 0x1F, ADF4368_PHASE_ADJ_MSK, 0x0);;
}

/**
 * @brief Get the phase adjustment in femto-seconds with Sigma Delta Modulation.
 * @param dev 		- The device structure.
 * @param phase_fs 	- The Read value of the phase adjustment in femto-seconds.
 * @return    		- 0 in case of success, negative error code otherwise.
 */
int adf4368_get_phase_sdm(struct adf4368_dev *dev, uint32_t *phase_fs)
{
	uint64_t rfout_mhz;
	uint64_t phase_tmp;
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_read(dev, 0x24, &tmp);
	if (ret)
		return ret;

	rfout_mhz = no_os_div_u64(dev->freq, ADF4368_MHZ);

	phase_tmp = rfout_mhz * ADF4368_SIGMA_DELTA_MOD_CNST;

	phase_tmp = no_os_div_u64(ADF4368_US_TO_FS, phase_tmp);

	*phase_fs = phase_tmp * tmp;

	return 0;
}

/**
 * @brief Set the phase adjustment in femto-seconds. The phase adjust will
 * enable the Bleed current. Recommend to use in integer mode.
 * @param dev 		- The device structure.
 * @param phase_fs 	- The phase adjustment in femto-seconds.
 * @param phase_pol 	- The phase adjustment value's sign.
 * @return    		- 0 in case of success, negative error code otherwise.
 */
int adf4368_set_phase_bleedi(struct adf4368_dev *dev, uint32_t phase_fs,
			     bool phase_pol)
{
	int32_t new_bleed_word;
	uint64_t rfout_period;
	uint32_t rfout_deg_ns;
	uint32_t phase_deg_ns;
	uint64_t rfout_deg_s;
	uint32_t phase_deg;
	uint64_t tmp;
	uint8_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_update_bits(dev, 0x1E, ADF4368_EN_PHASE_RESYNC_MSK, 0xff);
	if (ret)
		return ret;

	ret = adf4368_spi_update_bits(dev, 0x1F, ADF4368_EN_BLEED_MSK, 0xff);
	if (ret)
		return ret;

	dev->phase_adj = phase_fs;
	dev->phase_pol = phase_pol;
	//Determine the output freq. in degrees/s
	rfout_deg_s = 360 * dev->freq;
	//Convert it to degrees/ns
	rfout_deg_ns = no_os_div_u64(rfout_deg_s, ADF4368_S_TO_NS);
	//Determine the phase adjustment in degrees relative the output freq.
	phase_deg_ns = rfout_deg_ns * (phase_fs);
	//Convert it to degrees/ps
	phase_deg = no_os_div_u64(phase_deg_ns, ADF4368_NS_TO_PS);

	if (phase_deg > 360 * KILO) {
		pr_err("Phase Adjustment cannot exceed 360deg per Clock Period\n");
		return EINVAL;
	}

	new_bleed_word = adf4368_compute_bleed_word(dev, phase_fs, !phase_pol);

	// Decrease or increase bleed word with period of RF-out
	if (new_bleed_word > ADF4368_BLEED_WORD_MAX) {

		dev->bleed_word = new_bleed_word;
		tmp = no_os_div_u64(dev->freq, MICRO);
		rfout_period =  no_os_div_u64(ADF4368_US_TO_FS, tmp);
		new_bleed_word = adf4368_compute_bleed_word(dev, rfout_period, true);

	} else if (new_bleed_word < 0) {
		dev->bleed_word = new_bleed_word;
		tmp = no_os_div_u64(dev->freq, MICRO);
		rfout_period =  no_os_div_u64(ADF4368_US_TO_FS, tmp);
		new_bleed_word = adf4368_compute_bleed_word(dev, rfout_period,
				 false);
	}

	dev->bleed_word = new_bleed_word;
	val = dev->bleed_word & ADF4368_FINE_BLEED_LSB_MSK;
	ret = adf4368_spi_write(dev, 0x1D, val);
	if (ret)
		return ret;

	val = (dev->bleed_word >> 8) & ADF4368_BLEED_MSB_MSK;
	ret = adf4368_spi_update_bits(dev, 0x1E, ADF4368_BLEED_MSB_MSK,
				      no_os_field_prep(ADF4368_BLEED_MSB_MSK,
						      val));
	if (ret)
		return ret;

	// Need to set N_INT last to write Double Buffered Registers
	val = dev->n_int & ADF4368_N_INT_LSB_MSK;
	return adf4368_spi_write(dev, 0x10, val);
}

/**
 * @brief Set the phase adjustment in femto-seconds. Function choose the phase
 * adjustment method according to Integer mode.
 * @param dev 		- The device structure.
 * @param phase_fs 	- The phase adjustment in femto-seconds.
 * @param phase_pol 	- The phase adjustment value's sign.
 * @return    		- 0 in case of success, negative error code otherwise.
 */
int adf4368_set_phase(struct adf4368_dev *dev, uint32_t phase_fs,
		      bool phase_pol)
{
	uint8_t int_mode;
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	/* Read Int Mode */
	ret = adf4368_spi_read(dev, 0x11, &tmp);
	if (ret)
		return ret;
	int_mode = no_os_field_get(ADF4368_INT_MODE_MSK, tmp);

	if (int_mode) {
		ret = adf4368_set_phase_bleedi(dev, phase_fs, phase_pol);
		if (ret)
			return ret;
	} else {
		// Only Positive Phase
		if (phase_pol)
			return -EINVAL;
		return adf4368_set_phase_sdm(dev, phase_fs);
	}

	return 0;
}

/**
 * @brief Get the phase adjustment in femto-seconds. Function choose the phase
 * adjustment method according to Integer mode.
 * @param dev 		- The device structure.
 * @param phase_fs 	- The phase adjustment in femto-seconds.
 * @param phase_pol 	- The phase adjustment value's sign.
 * @return    		- 0 in case of success, negative error code otherwise.
 */
int adf4368_get_phase(struct adf4368_dev *dev, uint32_t *phase_fs,
		      bool *phase_pol)
{
	uint8_t int_mode;
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	/* Read Int Mode */
	ret = adf4368_spi_read(dev, 0x11, &tmp);
	if (ret)
		return ret;
	int_mode = no_os_field_get(ADF4368_INT_MODE_MSK, tmp);

	if (int_mode) {
		*phase_fs = dev->phase_adj;
		*phase_pol = dev->phase_pol;

	} else {
		ret = adf4368_get_phase_sdm(dev, phase_fs);
		*phase_pol = false;
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Applys a softreset, sets the SPI 4 wire mode and
 * writes the default registers.
 * @param dev 		- The device structure
 * @param spi_4wire 	- SPI 4 wire feature enable input
 * @return 		- 0 in case of success, negative error code otherwise.
 */
int adf4368_set_default_regs(struct adf4368_dev *dev, bool spi_4wire)
{
	uint8_t i;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4368_spi_write(dev, 0x00, ADF4368_RESET_CMD);
	if (ret)
		return ret;

	no_os_udelay(ADF4368_POR_DELAY_US);

	ret = adf4368_spi_write(dev, 0x00,
				ADF4368_SPI_4W_CFG(spi_4wire) |
				ADF4368_SPI_LSB_CFG(dev->spi_desc->bit_order));
	if (ret)
		return ret;

	/* Loads Default Registers */
	for (i = 0; i < NO_OS_ARRAY_SIZE(adf4368_reg_defaults); i++) {
		ret = adf4368_spi_write(dev,
					adf4368_reg_defaults[i].reg,
					adf4368_reg_defaults[i].val);
		if (ret)
			return ret;
	}

	return 0;
}
/**
 * @brief Reads and Checks the registers values equal to default values.
 * @param dev	- The device structure
 * @param check	- default state value
 * @return 	- 0 in case of success, negative error code otherwise.
 */
int adf4368_get_default_regs(struct adf4368_dev *dev, bool *check)
{

	uint8_t tmp;
	uint8_t i;
	int ret;

	if (!dev)
		return -EINVAL;

	/* Reads and Check Default Registers */
	for (i = 0; i < NO_OS_ARRAY_SIZE(adf4368_reg_defaults); i++) {

		ret = adf4368_spi_read(dev, adf4368_reg_defaults[i].reg, &tmp);
		if (ret)
			return ret;

		if (tmp != adf4368_reg_defaults[i].val) {
			*check = false;
			return 0;
		}
	}

	*check = true;
	return 0;
}

/**
 * @brief Set the output frequency.
 * @param dev 	- The device structure.
 * @return    	- 0 in case of success, negative error code otherwise.
 */
int adf4368_set_freq(struct adf4368_dev *dev)
{
	uint32_t frac2_word = 0;
	uint32_t frac1_word = 0;
	uint32_t mod2_word = 0;
	uint8_t ldwin_pw = 0;
	uint8_t clkout_div;
	uint8_t dclk_div1;
	uint64_t pfd_freq;
	uint8_t int_mode;
	uint8_t en_bleed;
	uint64_t vco = 0;
	uint64_t rem = 0;
	uint64_t res = 0;
	uint8_t locked;
	uint16_t n_int;
	uint8_t div1;
	uint64_t tmp;
	uint8_t val;

	int ret;

	if (!dev)
		return -EINVAL;

	/* Set R Doubler and R div */
	val = no_os_field_prep(ADF4368_EN_RDBLR_MSK, dev->ref_doubler_en) |
	      no_os_field_prep(ADF4368_R_DIV_MSK, dev->ref_div);
	ret = adf4368_spi_update_bits(dev, 0x20,
				      ADF4368_EN_RDBLR_MSK | ADF4368_R_DIV_MSK,
				      val);
	if (ret)
		return ret;

	for (clkout_div = 0; clkout_div <= dev->clkout_div_reg_val_max;
	     clkout_div++) {
		tmp = (1 << clkout_div) * dev->freq;
		if (tmp < dev->vco_min || tmp > dev->vco_max)
			continue;

		vco = tmp;
		break;
	}

	if (!vco) {
		pr_err("VCO is 0\n");
		return -EINVAL;
	}

	//Calculates the PFD freq. the output will be in KHz
	pfd_freq = adf4368_pfd_compute(dev);

	ret = adf4368_spi_update_bits(dev, 0x1F, ADF4368_CP_I_MSK,
				      no_os_field_prep(ADF4368_CP_I_MSK,
						      dev->cp_i));
	if (ret)
		return ret;

	/* Calculate N_int value */
	n_int = no_os_div64_u64_rem(dev->freq, pfd_freq, &rem);
	dev->n_int = n_int;

	/* Calculate Frac1 value */
	if (rem) {
		res = rem * ADF4368_MOD1WORD;
		frac1_word = (uint32_t)no_os_div64_u64_rem(res, pfd_freq, &rem);

		/* Calculate Frac2 and Mod2 values */
		if (rem > 0) {
			ret = adf4368_frac2_compute(dev, rem, pfd_freq,
						    &frac2_word, &mod2_word);
			if (ret)
				return ret;
		}
	}

	if (frac1_word || frac2_word) {
		/* Calculate ldwin value for fractional mode */
		if (dev->n_int < ADF4368_FRAC_N_INT_MIN)
			return -EINVAL;
		int_mode = 0;
		en_bleed = 1;

		/*The lock detector pulse window is determined based on the
		PFD frequency as described in the datasheet*/
		if (pfd_freq <= 40 * ADF4368_MHZ) {
			ldwin_pw = 7;
		} else if (pfd_freq <= 50 * ADF4368_MHZ) {
			ldwin_pw = 6;
		} else if (pfd_freq <= 100 * ADF4368_MHZ) {
			ldwin_pw = 5;
		} else if (pfd_freq <= 200 * ADF4368_MHZ) {
			ldwin_pw = 4;
		} else if (pfd_freq <= 250 * ADF4368_MHZ) {
			if (dev->freq >= 5000U * ADF4368_MHZ &&
			    dev->freq < 6400U * ADF4368_MHZ) {
				ldwin_pw = 3;
			} else {
				ldwin_pw = 2;
			}
		}
		ret = adf4368_optimize_bleed_word(dev);
		if (ret)
			return ret;
	} else {
		if (dev->n_int < ADF4368_INT_N_INT_MIN)
			return -EINVAL;
		int_mode = 1;
		en_bleed = 0;
		dev->bleed_word = 0;
		ldwin_pw = 0;
	}

	if (frac2_word) {
		ret = adf4368_spi_update_bits(dev, 0x28, ADF4368_VAR_MOD_EN_MSK,
					      0xff);
		if (ret)
			return ret;
	} else {
		ret = adf4368_spi_update_bits(dev, 0x28, ADF4368_VAR_MOD_EN_MSK,
					      0x0);
		if (ret)
			return ret;
	}

	ret = adf4368_spi_update_bits(dev, 0x11, ADF4368_INT_MODE_MSK,
				      no_os_field_prep(ADF4368_INT_MODE_MSK,
						      int_mode));
	if (ret)
		return ret;

	ret = adf4368_spi_update_bits(dev, 0x1F, ADF4368_EN_BLEED_MSK,
				      no_os_field_prep(ADF4368_EN_BLEED_MSK,
						      en_bleed));
	if (ret)
		return ret;

	val = mod2_word & ADF4368_MOD2WORD_LSB_MSK;
	ret = adf4368_spi_write(dev, 0x1A, val);
	if (ret)
		return ret;
	val = (mod2_word >> 8) & ADF4368_MOD2WORD_MID_MSK;
	ret = adf4368_spi_write(dev, 0x1B, val);
	if (ret)
		return ret;
	val = (mod2_word >> 16) & ADF4368_MOD2WORD_MSB_MSK;
	ret = adf4368_spi_write(dev, 0x1C, val);
	if (ret)
		return ret;

	val = frac2_word  & ADF4368_FRAC2WORD_LSB_MSK;
	ret = adf4368_spi_write(dev, 0x17, val);
	if (ret)
		return ret;
	val = (frac2_word >> 8)  & ADF4368_FRAC2WORD_MID_MSK;
	ret = adf4368_spi_write(dev, 0x18, val);
	if (ret)
		return ret;
	val = (frac2_word >> 16) & ADF4368_FRAC2WORD_MSB_MSK;
	ret = adf4368_spi_write(dev, 0x19, val);
	if (ret)
		return ret;

	val = frac1_word  & ADF4368_FRAC1WORD_LSB_MSK;
	ret = adf4368_spi_write(dev, 0x12, val);
	if (ret)
		return ret;
	val = (frac1_word >> 8)  & ADF4368_FRAC1WORD_MID_MSK;
	ret = adf4368_spi_write(dev, 0x13, val);
	if (ret)
		return ret;
	val = (frac1_word >> 16) & ADF4368_FRAC1WORD_MSB_MSK;
	ret = adf4368_spi_write(dev, 0x14, val);
	if (ret)
		return ret;

	val = (frac1_word >> 24) & ADF4368_FRAC1WORD_MSB;
	ret = adf4368_spi_update_bits(dev, 0x15, ADF4368_FRAC1WORD_MSB, val);
	if (ret)
		return ret;

	dclk_div1 = 2;
	div1 = 8;
	if (pfd_freq <= ADF4368_DCLK_DIV1_0_MAX) {
		dclk_div1 = 0;
		div1 = 1;
	} else if (pfd_freq <= ADF4368_DCLK_DIV1_1_MAX) {
		dclk_div1 = 1;
		div1 = 2;
	}

	ret = adf4368_spi_update_bits(dev, 0x4E, ADF4368_DCLK_DIV1_MSK,
				      no_os_field_prep(ADF4368_DCLK_DIV1_MSK,
						      dclk_div1));
	if (ret)
		return ret;

	ret = adf4368_spi_update_bits(dev, 0x35, ADF4368_DCLK_MODE_MSK, 0xff);
	if (ret)
		return ret;

	ret = adf4368_spi_update_bits(dev, 0x35, ADF4368_EN_ADC_CLK_MSK, 0xff);
	if (ret)
		return ret;

	/* Calculate ADC CLK Div */
	tmp = NO_OS_DIV_ROUND_UP(no_os_div_u64(pfd_freq, div1 * 400000) - 2, 4);
	tmp = no_os_clamp(tmp, 0U, 255U);
	tmp &= ADF4368_ADC_CLK_DIV_MSK;
	ret = adf4368_spi_write(dev, 0x3E, tmp);

	if (ret)
		return ret;

	// Set LD COUNT
	ret = adf4368_spi_update_bits(dev, 0x2C, ADF4368_LD_COUNT_MSK,
				      dev->ld_count);
	if (ret)
		return ret;

	ret = adf4368_spi_update_bits(dev, 0x2C, ADF4368_LDWIN_PW_MSK,
				      no_os_field_prep(ADF4368_LDWIN_PW_MSK,
						      ldwin_pw));
	if (ret)
		return ret;

	ret = adf4368_spi_update_bits(dev, 0x11, ADF4368_CLKOUT_DIV_MSK,
				      no_os_field_prep(ADF4368_CLKOUT_DIV_MSK,
						      clkout_div));
	if (ret)
		return ret;

	val = (n_int >> 8) & ADF4368_N_INT_MSB_MSK;
	ret = adf4368_spi_update_bits(dev, 0x11, ADF4368_N_INT_MSB_MSK, val);
	if (ret)
		return ret;

	// Need to set N_INT last to trigger an auto-calibration
	val = n_int & ADF4368_N_INT_LSB_MSK;
	ret = adf4368_spi_write(dev, 0x10, val);
	if (ret)
		return ret;

	no_os_mdelay(ADF4368_LKD_DELAY_MS);
	ret = adf4368_spi_read(dev, 0x58, &val);
	if (ret)
		return ret;

	locked = no_os_field_get(val, ADF4368_LOCKED_MSK);
	if (!locked)
		return -EIO;

	return 0;
}

/**
 * @brief Initializes the adf4368.
 * @param dev	     - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return 	     - 0 in case of success or negative error code.
 */
int adf4368_init(struct adf4368_dev **dev,
		 struct adf4368_init_param *init_param)
{
	struct adf4368_dev *device;
	bool en = true;
	int ret;

	device = (struct adf4368_dev *)no_os_calloc(1, sizeof(*device));
	if (!device)
		return -ENOMEM;

	ret = no_os_spi_init(&device->spi_desc, init_param->spi_init);
	if (ret)
		goto error_dev;

	device->spi_4wire_en = init_param->spi_4wire_en;
	device->cmos_3v3 = init_param->cmos_3v3;
	device->ref_freq_hz = init_param->ref_freq_hz;
	device->freq = init_param->freq;
	device->ref_doubler_en = init_param->ref_doubler_en;
	device->ref_div = init_param->ref_div;
	device->cp_i = init_param->cp_i;
	device->ld_count = init_param->ld_count;
	device->phase_adj = 0;
	device->freq_max = ADF4368_RFOUT_MAX;
	device->freq_min = ADF4368_RFOUT_MIN;
	device->vco_max = ADF4368_VCO_FREQ_MAX;
	device->vco_min = ADF4368_VCO_FREQ_MIN;
	device->clkout_div_reg_val_max = ADF4368_CLKOUT_DIV_REG_VAL_MAX;

	ret = adf4368_set_default_regs(device, device->spi_4wire_en);
	if (ret)
		goto error_spi;

	ret = adf4368_check_scratchpad(device);
	if (ret)
		goto error_spi;

	ret = adf4368_spi_write(device, 0x3D,
				no_os_field_prep(ADF4368_CMOS_OV_MSK,
						device->cmos_3v3));
	if (ret)
		goto error_spi;

	ret = adf4368_set_freq(device);
	if (ret) {
		pr_info("adf4368_set_freq %d\n", ret);
		goto error_spi;
	}

	ret = adf4368_set_en_chan(device, 0, en);
	if (ret) {
		pr_info("adf4368_set_en_chan %d\n", ret);
		goto error_spi;
	}

	ret = adf4368_set_en_chan(device, 1, en);
	if (ret) {
		pr_info("adf4368_set_en_chan %d\n", ret);
		goto error_spi;
	}

	ret = adf4368_set_out_power(device, 0, 9);
	if (ret) {
		pr_info("adf4368_set_out_power %d\n", ret);
		goto error_spi;
	}

	ret = adf4368_set_out_power(device, 1, 9);
	if (ret) {
		pr_info("adf4368_set_out_power %d\n", ret);
		goto error_spi;
	}

	*dev = device;

	return ret;

error_spi:
	no_os_spi_remove(device->spi_desc);
error_dev:
	no_os_free(device);
	return ret;
}

/**
 * @brief Free resources allocated for ADF4368
 * @param dev 	- The device structure.
 * @return 	- 0 in case of success or negative error code.
 */
int adf4368_remove(struct adf4368_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		no_os_free(dev);

	return 0;
}
