/***************************************************************************//**
 *   @file   adf4030.c
 *   @brief  Implementation of ADF4030 Driver.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
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

#include "adf4030.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

/**
 * @struct adf4030_reg_sequence
 * @brief ADF4030 register format structure for default values
 */
struct adf4030_reg_sequence {
	uint16_t reg;
	uint8_t val;
};

/**
 * @struct adf4030_reg_defaults
 * @brief ADF4030 register initialization
 */
static const struct adf4030_reg_sequence adf4030_reg_defaults[] = {

	{ 0x06A, 0x0A },
	{ 0x069, 0x0A },
	{ 0x066, 0x80 },
	{ 0x064, 0x1E },
	{ 0x063, 0x1E },
	{ 0x062, 0x4C },
	{ 0x061, 0x01 },
	{ 0x060, 0x2B },
	{ 0x05F, 0x5D },
	{ 0x05E, 0x32 },
	{ 0x05D, 0x10 },
	{ 0x05C, 0x1E },
	{ 0x05B, 0xC9 },
	{ 0x05A, 0x17 },
	{ 0x059, 0x49 },
	{ 0x058, 0x53 },
	{ 0x057, 0x45 },
	{ 0x056, 0x7D },
	{ 0x055, 0x01 },
	{ 0x054, 0x90 },
	{ 0x053, 0x19 },
	{ 0x052, 0xE9 },
	{ 0x050, 0xE9 },
	{ 0x04E, 0xE9 },
	{ 0x04C, 0xE9 },
	{ 0x04A, 0xE9 },
	{ 0x048, 0xE9 },
	{ 0x046, 0xE9 },
	{ 0x044, 0xE9 },
	{ 0x042, 0xE9 },
	{ 0x040, 0xE9 },
	{ 0x03C, 0xFF },
	{ 0x03B, 0xFC },
	{ 0x037, 0x02 },
	{ 0x035, 0x05 },
	{ 0x034, 0x24 },
	{ 0x033, 0x1D },
	{ 0x032, 0x1D },
	{ 0x031, 0x45 },
	{ 0x016, 0x06 },
	{ 0x011, 0x1F },
	{ 0x010, 0x1F },
};

/**
 * @brief Writes data to ADF4030 over SPI.
 * @param dev	   - The device structure.
 * @param reg_addr - The register address.
 * @param data 	   - Data value to write.
 * @return 	   - 0 in case of success or negative error code otherwise.
 */
int adf4030_spi_write(struct adf4030_dev *dev, uint16_t reg_addr, uint8_t data)
{
	uint8_t buff[ADF4030_BUFF_SIZE_BYTES];
	uint16_t cmd;
	uint8_t addr;

	if (!dev)
		return -EINVAL;

	/*
	 * To write Register 0x00 and Register 0x01, the ADDR bits must be
	 * cleared to 0000, independent of the ADDR pin code.
	 */
	addr = (reg_addr <= 0x01) ? 0 : dev->chip_addr;
	cmd = ADF4030_SPI_WRITE_CMD | ADF4030_CHIP_ADDRESS(addr) | reg_addr;

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
					ADF4030_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ADF4030 over SPI.
 * @param dev 	   - The device structure.
 * @param reg_addr - The register address.
 * @param data	   - Data read from the device.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adf4030_spi_read(struct adf4030_dev *dev, uint16_t reg_addr, uint8_t *data)
{
	uint8_t buff[ADF4030_BUFF_SIZE_BYTES];
	uint16_t cmd;
	int ret;

	if (!dev || !data)
		return -EINVAL;

	cmd = ADF4030_SPI_READ_CMD | ADF4030_CHIP_ADDRESS(dev->chip_addr) | reg_addr;
	if (dev->spi_desc->bit_order) {
		buff[0] = no_os_bit_swap_constant_8(cmd & 0xFF);
		buff[1] = no_os_bit_swap_constant_8(cmd >> 8);
		buff[2] = no_os_bit_swap_constant_8(ADF4030_SPI_DUMMY_DATA);
	} else {
		buff[0] = cmd >> 8;
		buff[1] = cmd & 0xFF;
		buff[2] = ADF4030_SPI_DUMMY_DATA;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buff,
				       ADF4030_BUFF_SIZE_BYTES);
	if (ret)
		return ret;

	if (dev->spi_desc->bit_order)
		*data = no_os_bit_swap_constant_8(buff[2]);
	else
		*data = buff[2];

	return 0;
}

/**
 * @brief Updates the values of the ADF4030 register.
 * @param dev 	   - The device structure.
 * @param reg_addr - The register address.
 * @param mask 	   - Bits to be updated.
 * @param data 	   - Update value for the mask.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adf4030_spi_update_bits(struct adf4030_dev *dev, uint16_t reg_addr,
			    uint8_t mask, uint8_t data)
{
	uint8_t tmp, orig;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4030_spi_read(dev, reg_addr, &orig);
	if (ret)
		return ret;

	tmp = orig & ~mask;
	tmp |= data & mask;

	if (tmp != orig)
		return adf4030_spi_write(dev, reg_addr, tmp);

	return 0;
}

/**
 * @brief Poll register.
 * @param dev - The device structure.
 * @param reg_addr - The address of the register.
 * @param mask - The mask that is applied.
 * @param data - The expected data.
 *
 * @return Returns 0 in case of success or negative error code.
 */
static int adf4030_poll(struct adf4030_dev *dev, uint32_t reg_addr,
			uint32_t mask,
			uint32_t data)
{
	uint8_t reg_data;
	uint16_t timeout = 1000;
	int32_t ret;

	do {
		ret = adf4030_spi_read(dev,
				       reg_addr,
				       &reg_data);
		if (ret)
			return ret;
		no_os_mdelay(2);
	} while (((reg_data & mask) != data) && --timeout);

	return timeout ? 0 : -ETIMEDOUT;
}

/**
 * @brief Sets the Power down controls of the device.
 * @param dev 	     - The device structure.
 * @param power_up   - Enable value of the power down control.
 * @return 	     - 0 in case of success or negative error code.
 */
static int adf4030_power(struct adf4030_dev *dev, bool power_up)
{
	uint8_t val;
	uint8_t msk;

	val = no_os_field_prep(ADF4030_PD_ALL, !power_up) |
	      no_os_field_prep(ADF4030_PD_PLL, !power_up) |
	      no_os_field_prep(ADF4030_PD_TDC, !power_up);
	msk = ADF4030_PD_ALL | ADF4030_PD_PLL | ADF4030_PD_TDC;

	return adf4030_spi_update_bits(dev, 0x3C, msk, val);
}

/**
 * @brief Applies a softreset, sets the SPI 4 wire mode and
 * writes the default registers.
 * @param dev 		- The device structure
 * @param spi_4wire 	- SPI 4 wire feature enable input
 * @return 		- 0 in case of success, negative error code otherwise.
 */
int adf4030_set_default_regs(struct adf4030_dev *dev, bool spi_4wire)
{
	uint8_t i;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adf4030_spi_write(dev, 0x00, ADF4030_RESET_CMD);
	if (ret)
		return ret;

	no_os_udelay(ADF4030_POR_DELAY_US);

	ret = adf4030_spi_write(dev, 0x00,
				ADF4030_SPI_4W_CFG(spi_4wire) |
				ADF4030_SPI_LSB_CFG(dev->spi_desc->bit_order));
	if (ret)
		return ret;

	/* Loads Default Registers */
	for (i = 0; i < NO_OS_ARRAY_SIZE(adf4030_reg_defaults); i++) {
		ret = adf4030_spi_write(dev,
					adf4030_reg_defaults[i].reg,
					adf4030_reg_defaults[i].val);
		if (ret)
			return ret;
	}

	return adf4030_power(dev, true);
}

/**
 * @brief ADF4030 SPI Scratchpad check.
 * @param dev 	- The device structure.
 * @return 	- 0 in case of success or negative error code.
 */
static int adf4030_check_scratchpad(struct adf4030_dev *dev)
{
	uint8_t scratchpad;
	int ret;

	ret = adf4030_spi_write(dev, 0x00A, ADF4030_SPI_SCRATCHPAD_TEST);
	if (ret)
		return ret;

	ret = adf4030_spi_read(dev, 0x00A, &scratchpad);
	if (ret)
		return ret;

	if (scratchpad != ADF4030_SPI_SCRATCHPAD_TEST)
		return -EIO;

	return 0;
}

/**
 * @brief Computes the ADEL_M and returns the value.
 * @param dev 	     - The device structure.
 * @param delcal     - DELCAL calibration value.
 * @return 	     	 - ADEL_M value.
 */
static uint32_t adf4030_adel_m_compute(struct adf4030_dev *dev, uint8_t delcal)
{
	uint64_t vco_period_fs;
	uint64_t vco_delay_lsb;

	if (!dev || !dev->vco_freq || !delcal)
		return 0;

	vco_period_fs = NO_OS_DIV_ROUND_CLOSEST(1000000000000000ULL, dev->vco_freq);
	vco_delay_lsb = NO_OS_DIV_ROUND_CLOSEST(vco_period_fs, 8);

	return NO_OS_DIV_ROUND_CLOSEST(vco_delay_lsb, delcal);
}

/**
 * @brief Computes the Voltage Value from RCM values and returns the value.
 * @param dev 	     - The device structure.
 * @param rcm 	     - RCM value of the channel.
 * @param boost	     - Boost value of the channel.
 * @return 	     - Voltage value.
 */
static uint32_t adf4030_channel_voltage_compute(struct adf4030_dev *dev,
		uint32_t rcm, uint8_t boost)
{
	uint32_t r_value;
	uint32_t i_value;

	/* R in milliohms: R = 7000000 / (735 - 10 * rcm) */
	r_value = ADF4030_RCM_CONST1 / (ADF4030_RCM_CONST2 - ADF4030_RCM_SLOPE * rcm);
	i_value = boost ? ADF4030_RCM_CURRENT1 : ADF4030_RCM_CURRENT0;

	/* V[mV] = (R[mOhm] + 26500) * I[mA] / 1000 */
	return ((r_value + ADF4030_RCM_CONST4) * i_value) / ADF4030_RCM_MV_SCALE;
}

/**
 * @brief Computes the RCM Register Value of the channel and returns the value.
 * @param dev 	     - The device structure.
 * @param voltage_mv - Voltage value of the channel in mV.
 * @param boost	     - Boost value of the channel.
 * @return 	     	 - RCM Register value.
 */
static uint8_t adf4030_channel_rcm_compute(struct adf4030_dev *dev,
		uint32_t voltage_mv, uint8_t boost)
{
	uint32_t r_value;
	uint32_t i_value;
	int32_t reg_value;

	i_value = boost ? ADF4030_RCM_CURRENT1 : ADF4030_RCM_CURRENT0;

	if ((voltage_mv * ADF4030_RCM_MV_SCALE / i_value) <= ADF4030_RCM_CONST4)
		return 0;

	/* Requested resistance in milliohms: R = V[mV] * 1000 / I[mA] - 26500 */
	r_value = (voltage_mv * ADF4030_RCM_MV_SCALE / i_value) - ADF4030_RCM_CONST4;

	/* reg = round((735 - 7000000 / R) / 10), clamped to [0, 63] */
	reg_value = (int32_t)ADF4030_RCM_CONST2 -
		    (int32_t)(ADF4030_RCM_CONST1 / r_value);
	if (reg_value < 0)
		reg_value = 0;

	reg_value = NO_OS_DIV_ROUND_CLOSEST(reg_value, ADF4030_RCM_SLOPE);
	if (reg_value > 63)
		reg_value = 63;

	return (uint8_t)reg_value;
}

/**
 * @brief Sets the VCO Auto Calibration.
 * @param dev 		- The device structure.
 * @param en	 	- The enable or disable value of the VCO auto calibration.
 * @return    		- 0 in case of success or negative error code.
 */
static int adf4030_set_vco_cal(struct adf4030_dev *dev, bool en)
{
	if (!dev)
		return -EINVAL;

	return adf4030_spi_update_bits(dev, 0x5A, ADF4030_PLL_CAL_EN,
				      no_os_field_prep(ADF4030_PLL_CAL_EN, en));
}

/**
 * @brief Set Temperature Readback feature's initial state.
 * This function should be called before reading temperature to trigger measurement.
 * @param dev 		- The device structure.
 * @param en	 	- The enable or disable Temperature readback feature.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4030_set_temperature(struct adf4030_dev *dev, bool en)
{
	int ret;
	uint8_t val;
	uint8_t msk;

	if (!dev)
		return -EINVAL;

	if (en) {
		ret = adf4030_spi_update_bits(dev, 0x3C, ADF4030_PD_ADC, 0x0);
		if (ret)
			return ret;

		val = no_os_field_prep(ADF4030_ADC_CLK_SEL, false) |
		      no_os_field_prep(ADF4030_EN_ADC_CNV, true) |
		      no_os_field_prep(ADF4030_EN_ADC_CLK, true) |
		      no_os_field_prep(ADF4030_EN_ADC, true);
		msk = ADF4030_ADC_CLK_SEL | ADF4030_EN_ADC_CNV |
		      ADF4030_EN_ADC_CLK | ADF4030_EN_ADC;
		ret = adf4030_spi_update_bits(dev, 0x61, msk, val);
		if (ret)
			return ret;
		ret = adf4030_spi_write(dev, 0x62, ADF4030_ADC_CLK_DIV);
		if (ret)
			return ret;
		return adf4030_spi_write(dev, 0x72, ADF4030_ADC_ST_CNV);
	} else {
		msk = ADF4030_EN_ADC_CNV | ADF4030_EN_ADC_CLK | ADF4030_EN_ADC;
		ret = adf4030_spi_update_bits(dev, 0x61, msk, 0x0);
		if (ret)
			return ret;

		return adf4030_spi_update_bits(dev, 0x3C, ADF4030_PD_ADC, 0xFF);
	}
}

/**
 * @brief Gets the value of the approximate die temperature.
 * @param dev 			- The device structure.
 * @param temperature	- The read value of the Temperature Readback in deg C.
 * @return    			- 0 in case of success or negative error code.
 */
int adf4030_get_temperature(struct adf4030_dev *dev, int16_t *temperature)
{
	int ret;
	uint8_t lsb, msb;
	int16_t raw;

	if (!dev || !temperature)
		return -EINVAL;

	/* Start ADC conversion */
	ret = adf4030_spi_write(dev, 0x72, ADF4030_ADC_ST_CNV);
	if (ret)
		return ret;

	/* Wait for ADC conversion to complete */
	ret = adf4030_poll(dev, 0x8F, ADF4030_ADC_BUSY, false);
	if (ret)
		return ret;

	ret = adf4030_spi_read(dev, 0x93, &msb);
	if (ret)
		return ret;

	ret = adf4030_spi_read(dev, 0x92, &lsb);
	if (ret)
		return ret;

	/*
	 * Temperature is a 9-bit sign-magnitude value:
	 * Bit 8 comes from REG0093 and represents the sign
	 * (0 = positive, 1 = negative). Bits 7:0 from REG0092
	 * represent the magnitude in degrees Celsius.
	 */
	raw = lsb;
	if (no_os_field_get(ADF4030_TEMP_MEAS_MSB, msb))
		raw = -raw;

	*temperature = raw;

	return 0;
}

/**
 * @brief Set the chip address value.
 * @param dev 		- The device structure.
 * @param addr		- The desired chip address value.
 * @return    		- 0 in case of success or negative error code.
 */
int adf4030_set_chip_address(struct adf4030_dev *dev, uint8_t addr)
{
	if (!dev)
		return -EINVAL;

	if (addr > ADF4030_CHIP_ADDRESS_MAX)
		return -EINVAL;

	dev->chip_addr = addr;

	return 0;
}

/**
 * @brief Set the reference divider (RDIV) value.
 * @param dev  - The device structure.
 * @param rdiv - The desired reference divider value.
 * @return     - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_rdiv(struct adf4030_dev *dev, uint8_t rdiv)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (rdiv < ADF4030_R_DIV_MIN || rdiv > ADF4030_R_DIV_MAX)
		return -EINVAL;

	if (dev->ref_freq / rdiv < ADF4030_PFD_FREQ_MIN ||
	    NO_OS_DIV_ROUND_UP(dev->ref_freq, rdiv) > ADF4030_PFD_FREQ_MAX)
		return -EINVAL;

	ret = adf4030_set_vco_cal(dev, true);
	if (ret)
		return ret;

	ret = adf4030_spi_update_bits(dev, 0x57, ADF4030_RDIV,
				      no_os_field_prep(ADF4030_RDIV, rdiv));
	if (ret)
		return ret;

	/* Writing NDIV starts VCO calibration */
	ret = adf4030_spi_write(dev, 0x56, dev->ndiv);
	if (ret)
		return ret;

	/* Wait for calibration to complete */
	ret = adf4030_poll(dev, 0xBA, ADF4030_CAL_BUSY, false);
	if (ret)
		return ret;

	/* Wait for lock detect */
	ret = adf4030_poll(dev, 0x90, ADF4030_PLL_LD, true);
	if (ret)
		return ret;

	ret = adf4030_set_vco_cal(dev, false);
	if (ret)
		return ret;

	dev->ref_div = rdiv;

	return adf4030_get_vco_freq(dev, &dev->vco_freq);
}

/**
 * @brief Get the reference divider (RDIV) value.
 * @param dev  - The device structure.
 * @param rdiv - Read RDIV value.
 * @return     - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_rdiv(struct adf4030_dev *dev, uint8_t *rdiv)
{
	uint8_t tmp;
	int ret;

	if (!dev || !rdiv)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x57, &tmp);
	if (ret)
		return ret;

	dev->ref_div = no_os_field_get(ADF4030_RDIV, tmp);
	*rdiv = dev->ref_div;

	return 0;
}

/**
 * @brief Set the N divider (NDIV) value.
 * @param dev  - The device structure.
 * @param ndiv - The desired N divider value.
 * @return     - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_ndiv(struct adf4030_dev *dev, uint8_t ndiv)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (ndiv < ADF4030_N_DIV_MIN)
		return -EINVAL;

	ret = adf4030_set_vco_cal(dev, true);
	if (ret)
		return ret;

	/* Writing NDIV starts VCO calibration */
	ret = adf4030_spi_write(dev, 0x56, ndiv);
	if (ret)
		return ret;

	/* Wait for calibration to complete */
	ret = adf4030_poll(dev, 0xBA, ADF4030_CAL_BUSY, false);
	if (ret)
		return ret;

	/* Wait for lock detect */
	ret = adf4030_poll(dev, 0x90, ADF4030_PLL_LD, true);
	if (ret)
		return ret;

	ret = adf4030_set_vco_cal(dev, false);
	if (ret)
		return ret;

	dev->ndiv = ndiv;

	return adf4030_get_vco_freq(dev, &dev->vco_freq);
}

/**
 * @brief Get the N divider (NDIV) value.
 * @param dev  - The device structure.
 * @param ndiv - Read NDIV value.
 * @return     - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_ndiv(struct adf4030_dev *dev, uint8_t *ndiv)
{
	uint8_t tmp;
	int ret;

	if (!dev || !ndiv)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x56, &tmp);
	if (ret)
		return ret;

	dev->ndiv = no_os_field_get(ADF4030_NDIV, tmp);
	*ndiv = dev->ndiv;

	return 0;
}

/**
 * @brief Set the output divider A (ODIVA) value.
 * @param dev   - The device structure.
 * @param odiva - The desired output divider A value (12-bit).
 * @return      - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_odiva(struct adf4030_dev *dev, uint16_t odiva)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (odiva < ADF4030_O_DIV_MIN || odiva > ADF4030_O_DIV_MAX)
		return -EINVAL;

	ret = adf4030_spi_write(dev, 0x53,
				no_os_field_prep(ADF4030_ODIVA_LSB, odiva & 0xFF));
	if (ret)
		return ret;

	ret = adf4030_spi_update_bits(dev, 0x54, ADF4030_ODIVA_MSB,
				       no_os_field_prep(ADF4030_ODIVA_MSB, (odiva >> 8)));
	if (ret)
		return ret;

	dev->bsync_freq_odiv_a = dev->vco_freq / odiva;

	return 0;
}

/**
 * @brief Get the output divider A (ODIVA) value.
 * @param dev   - The device structure.
 * @param odiva - Read ODIVA value (12-bit).
 * @return      - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_odiva(struct adf4030_dev *dev, uint16_t *odiva)
{
	uint8_t tmp;
	uint16_t odiv_val;
	int ret;

	if (!dev || !odiva)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x53, &tmp);
	if (ret)
		return ret;
	odiv_val = tmp;

	ret = adf4030_spi_read(dev, 0x54, &tmp);
	if (ret)
		return ret;
	odiv_val |= (no_os_field_get(ADF4030_ODIVA_MSB, tmp) << 8);

	*odiva = odiv_val;

	return 0;
}

/**
 * @brief Set the output divider B (ODIVB) value.
 * @param dev   - The device structure.
 * @param odivb - The desired output divider B value (12-bit).
 * @return      - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_odivb(struct adf4030_dev *dev, uint16_t odivb)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (odivb < ADF4030_O_DIV_MIN || odivb > ADF4030_O_DIV_MAX)
		return -EINVAL;

	ret = adf4030_spi_update_bits(dev, 0x54, ADF4030_ODIVB_LSB,
				      no_os_field_prep(ADF4030_ODIVB_LSB, odivb & 0x0F));
	if (ret)
		return ret;

	ret = adf4030_spi_write(dev, 0x55, (odivb >> 4));
	if (ret)
		return ret;

	dev->bsync_freq_odiv_b = dev->vco_freq / odivb;

	return 0;
}

/**
 * @brief Get the output divider B (ODIVB) value.
 * @param dev   - The device structure.
 * @param odivb - Read ODIVB value (12-bit).
 * @return      - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_odivb(struct adf4030_dev *dev, uint16_t *odivb)
{
	uint8_t tmp;
	uint16_t odiv_val;
	int ret;

	if (!dev || !odivb)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x54, &tmp);
	if (ret)
		return ret;
	odiv_val = no_os_field_get(ADF4030_ODIVB_LSB, tmp);

	ret = adf4030_spi_read(dev, 0x55, &tmp);
	if (ret)
		return ret;
	odiv_val |= (tmp << 4);

	*odivb = odiv_val;

	return 0;
}

/**
 * @brief Set the desired reference frequency.
 * @param dev - The device structure.
 * @param val - The desired reference frequency in Hz.
 * @return    - 0 in case of success or negative error code.
 */
int adf4030_set_ref_clk(struct adf4030_dev *dev, uint32_t val)
{
	if (!dev)
		return -EINVAL;

	if (val < ADF4030_REF_FREQ_MIN || val > ADF4030_REF_FREQ_MAX)
		return -EINVAL;

	dev->ref_freq = val;

	return adf4030_set_vco_freq(dev, dev->vco_freq);
}

/**
 * @brief Set the desired VCO frequency.
 * @param dev      - The device structure.
 * @param vco_freq - The desired VCO frequency in Hz.
 * @return         - 0 in case of success or negative error code.
 */
int adf4030_set_vco_freq(struct adf4030_dev *dev, uint32_t vco_freq)
{
	uint8_t i, max_r, min_r, rdiv = 0, ndiv = 0;
	int ret;

	if (!dev)
		return -EINVAL;

	if (vco_freq < ADF4030_VCO_FREQ_MIN || vco_freq > ADF4030_VCO_FREQ_MAX)
		return -EINVAL;

	if (dev->ref_freq < ADF4030_REF_FREQ_MIN ||
	    dev->ref_freq > ADF4030_REF_FREQ_MAX)
		return -EINVAL;

	max_r = dev->ref_freq / ADF4030_PFD_FREQ_MIN;
	min_r = NO_OS_DIV_ROUND_UP(dev->ref_freq, ADF4030_PFD_FREQ_MAX);

	for (i = min_r; i <= max_r; i++) {
		uint64_t vco_r = (uint64_t)vco_freq * i;

		if ((vco_r % dev->ref_freq) == 0) {
			uint64_t n = vco_r / dev->ref_freq;

			if (n >= ADF4030_N_DIV_MIN && n <= ADF4030_N_DIV_MAX) {
				rdiv = i;
				ndiv = (uint8_t)n;
				break;
			}
		}
	}
	if (!rdiv)
		return -EINVAL;

	ret = adf4030_set_vco_cal(dev, true);
	if (ret)
		return ret;

	dev->ndiv = ndiv;
	dev->vco_freq = vco_freq;
	dev->ref_div = rdiv;

	/* Write RDIV */
	ret = adf4030_spi_update_bits(dev, 0x57, ADF4030_RDIV,
				      no_os_field_prep(ADF4030_RDIV, dev->ref_div));
	if (ret)
		return ret;

	/* Write NDIV (starts VCO calibration) */
	ret = adf4030_spi_write(dev, 0x56, dev->ndiv);
	if (ret)
		return ret;

	/* Wait for calibration to complete */
	ret = adf4030_poll(dev, 0xBA, ADF4030_CAL_BUSY, false);
	if (ret)
		return ret;

	/* Wait for lock detect */
	ret = adf4030_poll(dev, 0x90, ADF4030_PLL_LD, true);
	if (ret)
		return ret;

	return adf4030_set_vco_cal(dev, false);
}

/**
 * @brief Get the VCO frequency in Hz.
 * @param dev      - The device structure.
 * @param vco_freq - The read VCO frequency in Hz.
 * @return         - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_vco_freq(struct adf4030_dev *dev, uint32_t *vco_freq)
{
	uint32_t pfd_freq;
	uint8_t ndiv, tmp;
	int ret;

	if (!dev || !vco_freq)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x57, &tmp);
	if (ret)
		return ret;

	dev->ref_div = no_os_field_get(ADF4030_RDIV, tmp);

	ret = adf4030_spi_read(dev, 0x56, &ndiv);
	if (ret)
		return ret;

	dev->ndiv = ndiv;

	pfd_freq = dev->ref_freq / (dev->ref_div ? dev->ref_div : 1);
	dev->vco_freq = pfd_freq * ndiv;

	*vco_freq = dev->vco_freq;

	return 0;
}

/**
 * @brief Set the BSYNC frequency in Hz. Output divider will be chosen according
 *  to odivb_sel argument.
 * @param dev        - The device structure.
 * @param bsync_freq - The desired BSYNC frequency in Hz.
 * @param odivb_sel  - Selects the output divider which will be set (true = ODIVB, false = ODIVA).
 * @return           - 0 in case of success, negative error code otherwise.
 */
int adf4030_set_bsync_freq(struct adf4030_dev *dev, uint32_t bsync_freq,
			   bool odivb_sel)
{
	uint32_t odiv;

	if (!dev)
		return -EINVAL;

	if (bsync_freq < ADF4030_BSYNC_FREQ_MIN ||
	    bsync_freq > ADF4030_BSYNC_FREQ_MAX)
		return -EINVAL;

	odiv = NO_OS_DIV_ROUND_CLOSEST(dev->vco_freq, bsync_freq);

	if (odiv < ADF4030_O_DIV_MIN || odiv > ADF4030_O_DIV_MAX)
		return -EINVAL;

	if (odivb_sel)
		return adf4030_set_odivb(dev, (uint16_t)odiv);

	return adf4030_set_odiva(dev, (uint16_t)odiv);
}

/**
 * @brief Get the BSYNC frequency in Hz.
 * @param dev        - The device structure.
 * @param bsync_freq - The read BSYNC frequency in Hz.
 * @param odivb_sel  - Selects the output divider to read (true = ODIVB, false = ODIVA).
 * @return           - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_bsync_freq(struct adf4030_dev *dev, uint32_t *bsync_freq,
			   bool odivb_sel)
{
	uint16_t odiv;
	int ret;

	if (!dev || !bsync_freq)
		return -EINVAL;

	if (odivb_sel) {
		ret = adf4030_get_odivb(dev, &odiv);
		if (ret)
			return ret;
		if (!odiv)
			return -EINVAL;

		dev->bsync_freq_odiv_b = NO_OS_DIV_ROUND_CLOSEST(dev->vco_freq, odiv);
		*bsync_freq = dev->bsync_freq_odiv_b;
	} else {
		ret = adf4030_get_odiva(dev, &odiv);
		if (ret)
			return ret;
		if (!odiv)
			return -EINVAL;

		dev->bsync_freq_odiv_a = NO_OS_DIV_ROUND_CLOSEST(dev->vco_freq, odiv);
		*bsync_freq = dev->bsync_freq_odiv_a;
	}

	return 0;
}

/**
 * @brief Set the TDC source.
 * @param dev        - The device structure.
 * @param tdc_source - The desired TDC source value (0-9 for RX0-RX9, 26 for REFIN).
 * @return           - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_tdc_source(struct adf4030_dev *dev, uint8_t tdc_source)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (tdc_source >= ADF4030_CHANNEL_NUMBER && tdc_source != 26)
		return -EINVAL;

	ret = adf4030_spi_update_bits(dev, 0x11, ADF4030_TDC_SOURCE,
				      no_os_field_prep(ADF4030_TDC_SOURCE, tdc_source));
	if (ret)
		return ret;

	dev->tdc_source = tdc_source;

	return 0;
}

/**
 * @brief Get the TDC source.
 * @param dev        - The device structure.
 * @param tdc_source - Read TDC source value.
 * @return           - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_tdc_source(struct adf4030_dev *dev, uint8_t *tdc_source)
{
	uint8_t tmp;
	int ret;

	if (!dev || !tdc_source)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x11, &tmp);
	if (ret)
		return ret;

	dev->tdc_source = no_os_field_get(ADF4030_TDC_SOURCE, tmp);
	*tdc_source = dev->tdc_source;

	return 0;
}

/**
 * @brief Set the TDC measurement target and start the measurement.
 * Before calling this function, please set tdc_source to the desired bsync channel or REFIN.
 * @param dev        - The device structure.
 * @param tdc_target - TDC measurement target (0-9 for RX0-RX9, 26 for REFIN).
 * @return           - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_tdc_measurement(struct adf4030_dev *dev, uint8_t tdc_target)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (tdc_target >= ADF4030_CHANNEL_NUMBER && tdc_target != 26)
		return -EINVAL;

	ret = adf4030_spi_update_bits(dev, 0x11, ADF4030_MANUAL_MODE, 0xFF);
	if (ret)
		return ret;

	dev->tdc_target = tdc_target;
	ret = adf4030_spi_write(dev, 0x10, dev->tdc_target);
	if (ret)
		return ret;

	ret = adf4030_spi_update_bits(dev, 0x61, ADF4030_RST_TDC_ERR, ADF4030_RST_TDC_ERR);
	if (ret)
		return ret;

	ret = adf4030_spi_update_bits(dev, 0x61, ADF4030_RST_TDC_ERR, 0x00);
	if (ret)
		return ret;

	ret = adf4030_spi_update_bits(dev, 0x16, ADF4030_TDC_ARM_M, 0xFF);
	if (ret)
		return ret;

	dev->tdc_status = true;

	return 0;
}

/**
 * @brief Get the TDC measurement result. Reads bitfields and calculates the
 * TDC result with period of the BSYNC or REFIN signal.
 * @param dev           - The device structure.
 * @param tdc_result_fs - Read TDC measurement result in femtoseconds.
 * @return              - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_tdc_measurement(struct adf4030_dev *dev, int64_t *tdc_result_fs)
{
	uint32_t bsync_freq = 0;
	uint32_t raw_ui = 0;
	int64_t tdc_tmp;
	uint8_t tmp;
	int ret, i;

	if (!dev || !tdc_result_fs)
		return -EINVAL;

	if (dev->tdc_source == 26) {
		bsync_freq = dev->ref_freq;
	} else if (dev->tdc_source < ADF4030_CHANNEL_NUMBER) {
		bsync_freq = dev->channels[dev->tdc_source].odivb_en ?
			     dev->bsync_freq_odiv_b : dev->bsync_freq_odiv_a;
	} else {
		return -EINVAL;
	}

	if (!bsync_freq)
		return -EINVAL;

	/* Start measurement if not already started */
	if (!dev->tdc_status) {
		ret = adf4030_spi_update_bits(dev, 0x16, ADF4030_TDC_ARM_M, 0xFF);
		if (ret)
			return ret;
	}

	/* Wait for TDC measurement to complete */
	ret = adf4030_poll(dev, 0x8F, ADF4030_TDC_BUSY, false);
	if (ret)
		return ret;

	/* Check for TDC error */
	ret = adf4030_spi_read(dev, 0x90, &tmp);
	if (ret)
		return ret;

	if (no_os_field_get(ADF4030_TDC_ERR, tmp) != 0) {
		adf4030_spi_update_bits(dev, 0x16, ADF4030_TDC_ARM_M, 0x00);
		dev->tdc_status = false;
		return -EIO;
	}

	/* Poll until MATH_BUSY is cleared */
	ret = adf4030_poll(dev, 0x8F, ADF4030_MATH_BUSY, false);
	if (ret)
		return ret;

	/* Read TDC_RSLT_UI MSB to LSB */
	for (i = 0; i < 3; i++) {
		ret = adf4030_spi_read(dev, 0x75 - i, &tmp);
		if (ret)
			return ret;

		raw_ui = (raw_ui << 8) | tmp;
	}

	/* Clear TDC_ARM_M to stop measurement */
	ret = adf4030_spi_update_bits(dev, 0x16, ADF4030_TDC_ARM_M, 0x00);
	if (ret)
		return ret;
	dev->tdc_status = false;

	/*
	 * TDC_RSLT_UI is a 24-bit two's complement integer representing the
	 * measured phase difference in units of 1 / 2^24 UI of the clock.
	 *
	 * Time Difference (fs) = (TDC_RSLT_UI * 10^15) / (2^24 * f_BSYNC)
	 *
	 * Since 10^15 / 2^24 = 30517578125 / 512, this simplifies to:
	 * Time Difference (fs) = (TDC_RSLT_UI * 30517578125) / (f_BSYNC * 512)
	 *
	 * Maximum numerator value: 2^23 * 30517578125 ≈ 2.56 * 10^17,
	 * which comfortably fits in a 64-bit signed integer without overflow.
	 */
	tdc_tmp = (int64_t)no_os_sign_extend32(raw_ui, 23);
	*tdc_result_fs = NO_OS_DIV_ROUND_CLOSEST(tdc_tmp * 30517578125LL,
						 (int64_t)bsync_freq * 512LL);

	return 0;
}

/**
 * @brief Set the iteration number of alignment.
 * @param dev         - The device structure.
 * @param iter_number - The number of iterations for the alignment (1 to 8).
 * @return            - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_alignment_iter(struct adf4030_dev *dev, uint8_t iter_number)
{
	uint8_t val;

	if (!dev)
		return -EINVAL;

	if (iter_number < ADF4030_ALIGN_CYCLES_MIN ||
	    iter_number > ADF4030_ALIGN_CYCLES_MAX)
		return -EINVAL;

	val = no_os_field_prep(ADF4030_EN_ITER, false) |
	      no_os_field_prep(ADF4030_EN_CYCS_RED, true) |
	      no_os_field_prep(ADF4030_ALIGN_CYCLES, iter_number - 1);
	return adf4030_spi_update_bits(dev, 0x37,
				       ADF4030_EN_ITER | ADF4030_EN_CYCS_RED |
				       ADF4030_ALIGN_CYCLES, val);
}

/**
 * @brief Get the iteration number of alignment.
 * @param dev         - The device structure.
 * @param iter_number - Read value of number of iterations in one alignment (1 to 8).
 * @return            - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_alignment_iter(struct adf4030_dev *dev, uint8_t *iter_number)
{
	uint8_t tmp;
	int ret;

	if (!dev || !iter_number)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x37, &tmp);
	if (ret)
		return ret;
	*iter_number = no_os_field_get(ADF4030_ALIGN_CYCLES, tmp) + 1;

	return 0;
}

/**
 * @brief Set the alignment threshold in femtoseconds.
 * @param dev          - The device structure.
 * @param threshold_fs - The alignment threshold in femtoseconds.
 * @return             - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_alignment_threshold(struct adf4030_dev *dev,
				    uint32_t threshold_fs)
{
	uint32_t adel_m_step;
	uint8_t reg_th;
	uint8_t delcal;
	int ret;

	if (!dev)
		return -EINVAL;

	if (!threshold_fs)
		return adf4030_spi_update_bits(dev, 0x37, ADF4030_EN_ITER, 0x0);

	ret = adf4030_spi_read(dev, 0x34, &delcal);
	if (ret)
		return ret;
	delcal = no_os_field_get(ADF4030_DELCAL, delcal);

	adel_m_step = adf4030_adel_m_compute(dev, delcal);
	if (!adel_m_step)
		return -EINVAL;

	reg_th = threshold_fs / adel_m_step;
	if (reg_th > 63)
		reg_th = 63;

	ret = adf4030_spi_update_bits(dev, 0x37,
				      ADF4030_EN_ITER | ADF4030_EN_CYCS_RED, 0xFF);
	if (ret)
		return ret;

	return adf4030_spi_update_bits(dev, 0x35, ADF4030_ALIGN_THOLD,
				       no_os_field_prep(ADF4030_ALIGN_THOLD, reg_th));
}

/**
 * @brief Get the alignment threshold in femtoseconds.
 * @param dev          - The device structure.
 * @param threshold_fs - Read value of alignment threshold in femtoseconds.
 * @return             - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_alignment_threshold(struct adf4030_dev *dev,
				    uint32_t *threshold_fs)
{
	uint32_t adel_m_step;
	uint8_t tmp, delcal;
	uint32_t reg_th;
	int ret;

	if (!dev || !threshold_fs)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x34, &delcal);
	if (ret)
		return ret;
	delcal = no_os_field_get(ADF4030_DELCAL, delcal);
	adel_m_step = adf4030_adel_m_compute(dev, delcal);

	ret = adf4030_spi_read(dev, 0x35, &tmp);
	if (ret)
		return ret;

	reg_th = no_os_field_get(ADF4030_ALIGN_THOLD, tmp);
	*threshold_fs = adel_m_step * reg_th;

	return 0;
}

/**
 * @brief Perform single-channel alignment. Before calling this function,
 * please set tdc_source to the desired bsync channel.
 * @param dev           - The device structure.
 * @param tdc_target_ch - The TDC Target channel for alignment.
 * @return              - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_single_ch_alignment(struct adf4030_dev *dev,
				    uint8_t tdc_target_ch)
{
	uint8_t timeout = 3;
	uint8_t val;
	uint8_t tmp;
	int ret;

	if (!dev)
		return -EINVAL;

	if (tdc_target_ch >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	val = no_os_field_prep(ADF4030_MANUAL_MODE, false) |
	      no_os_field_prep(ADF4030_EN_ALIGN, true);
	ret = adf4030_spi_update_bits(dev, 0x11, ADF4030_MANUAL_MODE |
				      ADF4030_EN_ALIGN, val);
	if (ret)
		return ret;

	ret = adf4030_spi_update_bits(dev, 0x37,
				      ADF4030_ALIGN_CYCLES | ADF4030_EN_SERIAL_ALIGN, 0x0);
	if (ret)
		return ret;

	do {
		ret = adf4030_spi_update_bits(dev, 0x61, ADF4030_RST_TDC_ERR,
					      ADF4030_RST_TDC_ERR);
		if (ret)
			return ret;
		ret = adf4030_spi_update_bits(dev, 0x61, ADF4030_RST_TDC_ERR, 0x00);
		if (ret)
			return ret;

		ret = adf4030_spi_write(dev, 0x10, tdc_target_ch);
		if (ret)
			return ret;

		dev->tdc_target = tdc_target_ch;

		ret = adf4030_poll(dev, 0x8F, ADF4030_FSM_BUSY, false);
		if (ret)
			return ret;

		ret = adf4030_spi_read(dev, 0x90, &tmp);
		if (ret)
			return ret;

		if (!(tmp & (ADF4030_TDC_ERR | ADF4030_TMP_ALIGN_ERR)))
			return 0;

	} while (timeout--);

	return -ETIMEDOUT;
}

/**
 * @brief Set serial alignment for multiple BSYNC channels.
 * @param dev           - The device structure.
 * @param channel_flags - Flags indicating the channels to align.
 * @return              - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_serial_alignment(struct adf4030_dev *dev,
				 uint16_t channel_flags)
{
	uint8_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	if (!channel_flags || (channel_flags & ~NO_OS_GENMASK(9, 0)))
		return -EINVAL;

	val = no_os_field_prep(ADF4030_MANUAL_MODE, false) |
	      no_os_field_prep(ADF4030_EN_ALIGN, true);

	ret = adf4030_spi_update_bits(dev, 0x11,
				      ADF4030_MANUAL_MODE | ADF4030_EN_ALIGN, val);
	if (ret)
		return ret;

	ret = adf4030_spi_update_bits(dev, 0x35, ADF4030_BSYNC_CAL_ON_1_0,
				      no_os_field_prep(ADF4030_BSYNC_CAL_ON_1_0, channel_flags));
	if (ret)
		return ret;

	ret = adf4030_spi_write(dev, 0x36, (channel_flags >> 2));
	if (ret)
		return ret;

	ret = adf4030_spi_update_bits(dev, 0x37, ADF4030_EN_SERIAL_ALIGN, 0xFF);
	if (ret)
		return ret;

	return adf4030_spi_write(dev, 0x10, dev->tdc_source);
}

/**
 * @brief Get the serial alignment channel flags for multiple BSYNC channels.
 * @param dev           - The device structure.
 * @param channel_flags - Read value of the channel alignment flags.
 * @return              - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_serial_alignment(struct adf4030_dev *dev,
				 uint16_t *channel_flags)
{
	uint32_t ch_flags;
	uint8_t tmp;
	int ret;

	if (!dev || !channel_flags)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x35, &tmp);
	if (ret)
		return ret;

	ch_flags = no_os_field_get(ADF4030_BSYNC_CAL_ON_1_0, tmp);

	ret = adf4030_spi_read(dev, 0x36, &tmp);
	if (ret)
		return ret;

	ch_flags |= (tmp << 2);
	*channel_flags = ch_flags;

	return 0;
}

/**
 * @brief Set background serial alignment for multiple BSYNC channels.
 * @param dev           - The device structure.
 * @param channel_flags - Flags indicating the channels to align.
 * @return              - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_background_serial_alignment(struct adf4030_dev *dev,
		uint16_t channel_flags)
{
	int ret;
	uint8_t val, tmp;
	bool enabled;

	if (!dev || !channel_flags)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x37, &tmp);
	if (ret)
		return ret;
	enabled = no_os_field_get(ADF4030_EN_BKGND_ALGN, tmp);

	/* If enabled : Disable Background alignment */
	if (enabled) {
		ret = adf4030_spi_update_bits(dev, 0x17, ADF4030_STOP_FSM, 0xFF);
		if (ret)
			return ret;

		ret = adf4030_spi_update_bits(dev, 0x37, ADF4030_EN_BKGND_ALGN, 0x0);
		if (ret)
			return ret;

		return adf4030_spi_update_bits(dev, 0x17, ADF4030_STOP_FSM, 0x0);
	}

	ret = adf4030_set_temperature(dev, true);
	if (ret)
		return ret;

	val = no_os_field_prep(ADF4030_MANUAL_MODE, false) |
	      no_os_field_prep(ADF4030_EN_ALIGN, true);

	ret = adf4030_spi_update_bits(dev, 0x11,
				      ADF4030_MANUAL_MODE | ADF4030_EN_ALIGN, val);
	if (ret)
		return ret;

	ret = adf4030_spi_update_bits(dev, 0x35, ADF4030_BSYNC_CAL_ON_1_0,
				      no_os_field_prep(ADF4030_BSYNC_CAL_ON_1_0, channel_flags));
	if (ret)
		return ret;

	ret = adf4030_spi_write(dev, 0x36, (channel_flags >> 2));
	if (ret)
		return ret;

	ret = adf4030_spi_update_bits(dev, 0x37, ADF4030_EN_SERIAL_ALIGN |
				      ADF4030_EN_BKGND_ALGN, 0xFF);
	if (ret)
		return ret;

	return adf4030_spi_write(dev, 0x10, dev->tdc_source);
}

/**
 * @brief Get the background serial alignment channel flags for multiple
 * BSYNC channels.
 * @param dev           - The device structure.
 * @param channel_flags - Read value of the channel alignment flags.
 * @return              - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_background_serial_alignment(struct adf4030_dev *dev,
		uint16_t *channel_flags)
{
	uint32_t ch_flags;
	bool enabled;
	uint8_t tmp;
	int ret;

	if (!dev || !channel_flags)
		return -EINVAL;

	ret = adf4030_spi_read(dev, 0x37, &tmp);
	if (ret)
		return ret;
	enabled = no_os_field_get(ADF4030_EN_BKGND_ALGN, tmp);
	if (enabled) {
		ret = adf4030_spi_read(dev, 0x35, &tmp);
		if (ret)
			return ret;

		ch_flags = no_os_field_get(ADF4030_BSYNC_CAL_ON_1_0, tmp);

		ret = adf4030_spi_read(dev, 0x36, &tmp);
		if (ret)
			return ret;

		ch_flags |= (tmp << 2);
		*channel_flags = ch_flags;
	} else {
		*channel_flags = 0;
	}

	return 0;
}

/**
 * @brief Set the delay for a specific BSYNC channel. This delay will show up
 * between TDC_SOURCE and channel.
 * @param dev      - The device structure.
 * @param channel  - The channel to set the delay for.
 * @param delay_fs - The delay in femtoseconds.
 * @return         - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_channel_delay(struct adf4030_dev *dev, uint8_t channel,
			      int64_t delay_fs)
{
	int64_t vco_period_fs;
	int64_t offset_int;
	uint16_t reg;
	int ret;

	if (!dev || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	vco_period_fs = NO_OS_DIV_ROUND_CLOSEST(1000000000000000ULL, dev->vco_freq);
	offset_int = NO_OS_DIV_ROUND_CLOSEST(delay_fs * 512, vco_period_fs);

	/* Channel TDC offset is a 16-bit signed value [-32768, 32767]. */
	if (offset_int > ADF4030_TDC_OFFSET_CH_MAX ||
	    offset_int < ADF4030_TDC_OFFSET_CH_MIN)
		return -EINVAL;

	reg = 0x1D + (channel * 2);

	/* TDC OFFSET LSB */
	ret = adf4030_spi_write(dev, reg, (uint16_t)offset_int & 0xFF);
	if (ret)
		return ret;

	/* TDC OFFSET MSB */
	ret = adf4030_spi_write(dev, reg + 1, ((uint16_t)offset_int >> 8) & 0xFF);
	if (ret)
		return ret;

	dev->channels[channel].delay_fs = delay_fs;

	return 0;
}

/**
 * @brief Get the delay for a specific channel.
 * @param dev      - The device structure.
 * @param channel  - The channel to get the delay for.
 * @param delay_fs - Read value of the delay in femtoseconds.
 * @return         - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_channel_delay(struct adf4030_dev *dev, uint8_t channel,
			      int64_t *delay_fs)
{
	int64_t vco_period_fs;
	int32_t total_offset;
	int16_t ch_offset;
	int32_t com_offset;
	uint32_t com_reg;
	uint16_t reg;
	uint8_t lsb, msb, com_lsb, com_mid, com_msb;
	int ret;

	if (!dev || !delay_fs || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	reg = 0x1D + (channel * 2);

	/* TDC OFFSET LSB */
	ret = adf4030_spi_read(dev, reg, &lsb);
	if (ret)
		return ret;

	/* TDC OFFSET MSB */
	ret = adf4030_spi_read(dev, reg + 1, &msb);
	if (ret)
		return ret;

	ch_offset = (int16_t)(((uint16_t)msb << 8) | lsb);

	/* TDC OFFSET COM LSB, MID, MSB */
	ret = adf4030_spi_read(dev, 0x1A, &com_lsb);
	if (ret)
		return ret;

	ret = adf4030_spi_read(dev, 0x1B, &com_mid);
	if (ret)
		return ret;

	ret = adf4030_spi_read(dev, 0x1C, &com_msb);
	if (ret)
		return ret;

	com_reg = ((uint32_t)no_os_field_get(ADF4030_TDC_OFFSET_COM_MSB, com_msb) << 16) |
		  ((uint32_t)com_mid << 8) | com_lsb;

	/* Sign-extend the 21-bit common offset. */
	com_offset = (int32_t)no_os_sign_extend32(com_reg, 20);

	total_offset = ch_offset + com_offset;

	vco_period_fs = NO_OS_DIV_ROUND_CLOSEST(1000000000000000ULL, dev->vco_freq);
	dev->channels[channel].delay_fs =
		NO_OS_DIV_ROUND_CLOSEST((int64_t)total_offset * vco_period_fs, 512);
	*delay_fs = dev->channels[channel].delay_fs;

	return 0;
}

/**
 * @brief Set the TX-RX direction of a specific channel.
 * @param dev     - The device structure.
 * @param channel - The channel to set the direction for.
 * @param tx_en   - Enable or disable the channel.
 * @return        - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_channel_direction(struct adf4030_dev *dev, uint8_t channel,
				  bool tx_en)
{
	uint8_t msk;
	int ret;

	if (!dev || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	/* Write TX_PD */
	if (channel < ADF4030_CHANNEL_TX_PD_SEPARATOR) {
		msk = (ADF4030_PD_TX_PATH0) << channel;
		ret = adf4030_spi_update_bits(dev, 0x3B,
					      msk, no_os_field_prep(msk, !tx_en));
	} else {
		msk = (ADF4030_PD_TX_PATH6) << (channel - ADF4030_CHANNEL_TX_PD_SEPARATOR);
		ret = adf4030_spi_update_bits(dev, 0x3C,
					      msk, no_os_field_prep(msk, !tx_en));
	}
	if (ret)
		return ret;

	/* Write EN_DRIVE */
	if (channel < ADF4030_CHANNEL_DRV_SEPARATOR) {
		msk = (ADF4030_EN_DRV0) << channel;
		ret = adf4030_spi_update_bits(dev, 0x12,
					      msk, no_os_field_prep(msk, tx_en));
	} else {
		msk = (ADF4030_EN_DRV8) << (channel - ADF4030_CHANNEL_DRV_SEPARATOR);
		ret = adf4030_spi_update_bits(dev, 0x13,
					      msk, no_os_field_prep(msk, tx_en));
	}
	if (ret)
		return ret;
	dev->channels[channel].tx_en = tx_en;
	return 0;
}

/**
 * @brief Get the TX direction (TX/RX) of a specific channel.
 * @param dev     - The device structure.
 * @param channel - The channel to get the direction for.
 * @param tx_en   - Read the channel's TX direction status.
 * @return        - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_channel_direction(struct adf4030_dev *dev, uint8_t channel,
				  bool *tx_en)
{
	uint8_t tmp;
	int ret;
	bool tx_pd;
	bool en_drv;

	if (!dev || !tx_en || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	/* Read TX_PD */
	if (channel < ADF4030_CHANNEL_TX_PD_SEPARATOR) {
		ret = adf4030_spi_read(dev, 0x3B, &tmp);
		tx_pd = no_os_field_get((ADF4030_PD_TX_PATH0) << channel, tmp);
	} else {
		ret = adf4030_spi_read(dev, 0x3C, &tmp);
		tx_pd = no_os_field_get((ADF4030_PD_TX_PATH6) <<
					(channel - ADF4030_CHANNEL_TX_PD_SEPARATOR), tmp);
	}
	if (ret)
		return ret;

	/* Read EN_DRV */
	if (channel < ADF4030_CHANNEL_DRV_SEPARATOR) {
		ret = adf4030_spi_read(dev, 0x12, &tmp);
		en_drv = no_os_field_get((ADF4030_EN_DRV0) << channel, tmp);
	} else {
		ret = adf4030_spi_read(dev, 0x13, &tmp);
		en_drv = no_os_field_get((ADF4030_EN_DRV8) <<
					 (channel - ADF4030_CHANNEL_DRV_SEPARATOR), tmp);
	}
	if (ret)
		return ret;

	dev->channels[channel].tx_en = en_drv && !tx_pd;
	*tx_en = dev->channels[channel].tx_en;

	return 0;
}

/**
 * @brief Set the termination type for a specific channel.
 * @param dev         - The device structure.
 * @param channel     - The channel to set the termination for.
 * @param termination - The termination type.
 * @return            - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_channel_termination(struct adf4030_dev *dev, uint8_t channel,
				    enum adf4030_terminations_e termination)
{
	uint16_t reg;
	uint8_t val;
	uint8_t msk;
	int ret;

	if (!dev || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	reg = 0x40 + (channel * 2);

	switch (termination) {
	case TX_VOLTAGE_DRIVER:
		ret = adf4030_spi_update_bits(dev, reg, ADF4030_FLOAT_TX0
					      | ADF4030_LINK_TX0, 0x0);
		break;

	case TX_CURRENT_DRIVER_UNTERMINATED:
		val = no_os_field_prep(ADF4030_FLOAT_TX0, true) |
		      no_os_field_prep(ADF4030_LINK_TX0, false);
		ret = adf4030_spi_update_bits(dev, reg, ADF4030_FLOAT_TX0
					      | ADF4030_LINK_TX0, val);
		break;

	case TX_CURRENT_DRIVER_TERMINATED:
		ret = adf4030_spi_update_bits(dev, reg, ADF4030_FLOAT_TX0
					      | ADF4030_LINK_TX0, 0xFF);
		break;

	case RX_DC_COUPLED_CLKS:
		val = no_os_field_prep(ADF4030_FLOAT_RX0, true) |
		      no_os_field_prep(ADF4030_LINK_RX0, true) |
		      no_os_field_prep(ADF4030_AC_COUPLED0, false);

		msk = ADF4030_FLOAT_RX0 | ADF4030_LINK_RX0 | ADF4030_AC_COUPLED0;
		ret = adf4030_spi_update_bits(dev, reg, msk, val);
		break;

	case RX_AC_COUPLED_CLKS:
		ret = adf4030_spi_update_bits(dev, reg, ADF4030_FLOAT_RX0
					      | ADF4030_AC_COUPLED0, 0xFF);
		break;
	case RX_DC_COUPLED_HCSL:
		ret = adf4030_spi_update_bits(dev, reg, ADF4030_FLOAT_RX0
					      | ADF4030_AC_COUPLED0, 0x0);
		break;
	default:
		return -EINVAL;
	};

	if (ret)
		return ret;

	dev->channels[channel].termination = termination;

	return 0;
}

/**
 * @brief Get the termination type for a specific channel.
 * @param dev         - The device structure.
 * @param channel     - The channel to get the termination for.
 * @param termination - Read value of the termination type.
 * @return            - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_channel_termination(struct adf4030_dev *dev, uint8_t channel,
				    enum adf4030_terminations_e *termination)
{
	uint16_t reg;
	uint8_t tmp;
	bool float_rx;
	bool float_tx;
	bool link_rx;
	bool link_tx;
	bool ac_coupled;
	bool tx_en;
	int ret;

	if (!dev || !termination || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	ret = adf4030_get_channel_direction(dev, channel, &tx_en);
	if (ret)
		return ret;

	reg = 0x40 + (channel * 2);
	ret = adf4030_spi_read(dev, reg, &tmp);
	if (ret)
		return ret;
	float_rx = no_os_field_get(ADF4030_FLOAT_RX0, tmp);
	float_tx = no_os_field_get(ADF4030_FLOAT_TX0, tmp);
	link_rx = no_os_field_get(ADF4030_LINK_RX0, tmp);
	link_tx = no_os_field_get(ADF4030_LINK_TX0, tmp);
	ac_coupled = no_os_field_get(ADF4030_AC_COUPLED0, tmp);

	if (!float_tx && tx_en)
		dev->channels[channel].termination = TX_VOLTAGE_DRIVER;
	else if (float_tx && !link_tx && tx_en)
		dev->channels[channel].termination = TX_CURRENT_DRIVER_UNTERMINATED;
	else if (float_tx && link_tx && tx_en)
		dev->channels[channel].termination = TX_CURRENT_DRIVER_TERMINATED;
	else if (float_rx && link_rx && !ac_coupled && !tx_en)
		dev->channels[channel].termination = RX_DC_COUPLED_CLKS;
	else if (float_rx && ac_coupled && !tx_en)
		dev->channels[channel].termination = RX_AC_COUPLED_CLKS;
	else if (!float_rx && !ac_coupled && !tx_en)
		dev->channels[channel].termination = RX_DC_COUPLED_HCSL;
	else
		return -EIO;

	*termination = dev->channels[channel].termination;

	return 0;
}

/**
 * @brief Set the PRBS (Pseudo-Random Binary Sequence) state for a specific channel.
 * @param dev      - The device structure.
 * @param channel  - The channel to set the PRBS state for.
 * @param prbs_en  - Enable or disable PRBS.
 * @return         - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_channel_prbs(struct adf4030_dev *dev, uint8_t channel,
			     bool prbs_en)
{
	uint8_t msk;
	int ret;

	if (!dev || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	if (channel < ADF4030_CHANNEL_PRBS_SEPARATOR) {
		msk = (ADF4030_PRBS0) << channel;
		ret = adf4030_spi_update_bits(dev, 0x13,
					      msk, no_os_field_prep(msk, prbs_en));
	} else {
		msk = (ADF4030_PRBS6) << (channel - ADF4030_CHANNEL_PRBS_SEPARATOR);
		ret = adf4030_spi_update_bits(dev, 0x14,
					      msk, no_os_field_prep(msk, prbs_en));
	}
	if (ret)
		return ret;

	dev->channels[channel].prbs_en = prbs_en;
	return 0;
}

/**
 * @brief Get the PRBS (Pseudo-Random Binary Sequence) state for a specific channel.
 * @param dev      - The device structure.
 * @param channel  - The channel to get the PRBS state for.
 * @param prbs_en  - Pointer to store the PRBS state.
 * @return         - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_channel_prbs(struct adf4030_dev *dev, uint8_t channel,
			     bool *prbs_en)
{
	uint8_t tmp;
	int ret;
	bool prbs;

	if (!dev || !prbs_en || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	/* Read PRBS */
	if (channel < ADF4030_CHANNEL_PRBS_SEPARATOR) {
		ret = adf4030_spi_read(dev, 0x13, &tmp);
		prbs = no_os_field_get((ADF4030_PRBS0) << channel, tmp);
	} else {
		ret = adf4030_spi_read(dev, 0x14, &tmp);
		prbs = no_os_field_get((ADF4030_PRBS6) <<
				       (channel - ADF4030_CHANNEL_PRBS_SEPARATOR), tmp);
	}
	if (ret)
		return ret;

	dev->channels[channel].prbs_en = prbs;
	*prbs_en = dev->channels[channel].prbs_en;
	return 0;
}

/**
 * @brief Set the output divider selection for a specific channel. If odivb_en is
 *        enabled, the output divider b will be used for the channel. Otherwise, the
 *        output divider a will be used.
 * @param dev      - The device structure.
 * @param channel  - The channel to set the output divider for.
 * @param odivb_en - Enable or disable the output divider b for the channel.
 * @return         - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_channel_odivb(struct adf4030_dev *dev, uint8_t channel,
			      bool odivb_en)
{
	uint16_t reg;
	int ret;

	if (!dev || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	reg = 0x3F + (channel * 2);

	ret = adf4030_spi_update_bits(dev, reg, ADF4030_ODIV_SEL0,
				      no_os_field_prep(ADF4030_ODIV_SEL0, odivb_en));
	if (ret)
		return ret;
	dev->channels[channel].odivb_en = odivb_en;
	return 0;
}

/**
 * @brief Get the output divider selection for a specific channel. If odivb_en is
 *        enabled, the output divider b will be used for the channel. Otherwise, the
 *        output divider a will be used.
 * @param dev      - The device structure.
 * @param channel  - The channel to get the output divider for.
 * @param odivb_en - Pointer to store the output divider state.
 * @return         - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_channel_odivb(struct adf4030_dev *dev, uint8_t channel,
			      bool *odivb_en)
{
	uint16_t reg;
	uint8_t tmp;
	bool sel_odivb;
	int ret;

	if (!dev || !odivb_en || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	reg = 0x3F + (channel * 2);

	ret = adf4030_spi_read(dev, reg, &tmp);
	if (ret)
		return ret;
	sel_odivb = no_os_field_get(ADF4030_ODIV_SEL0, tmp);

	dev->channels[channel].odivb_en = sel_odivb;
	*odivb_en = dev->channels[channel].odivb_en;
	return 0;
}

/**
 * @brief Set the inversion state for the BSYNC channel.
 * @param dev        - The device structure.
 * @param channel    - The channel to set the inversion state for.
 * @param invert_en  - Enable or disable inversion.
 * @return           - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_channel_invert(struct adf4030_dev *dev, uint8_t channel,
			       bool invert_en)
{
	uint8_t msk;
	int ret;

	if (!dev || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	if (channel < ADF4030_CHANNEL_INV_SEPARATOR) {
		msk = (ADF4030_CHAN_INV0) << channel;
		ret = adf4030_spi_update_bits(dev, 0x14,
					      msk, no_os_field_prep(msk, invert_en));
	} else {
		msk = (ADF4030_CHAN_INV4) << (channel - ADF4030_CHANNEL_INV_SEPARATOR);
		ret = adf4030_spi_update_bits(dev, 0x15,
					      msk, no_os_field_prep(msk, invert_en));
	}
	if (ret)
		return ret;

	dev->channels[channel].invert_en = invert_en;
	return 0;
}

/**
 * @brief Get the inversion state for a specific channel.
 * @param dev        - The device structure.
 * @param channel    - The channel to get the inversion state for.
 * @param invert_en  - Pointer to store the inversion state.
 * @return           - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_channel_invert(struct adf4030_dev *dev, uint8_t channel,
			       bool *invert_en)
{
	uint8_t tmp;
	int ret;
	bool invert;

	if (!dev || !invert_en || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	/* Read inversion */
	if (channel < ADF4030_CHANNEL_INV_SEPARATOR) {
		ret = adf4030_spi_read(dev, 0x14, &tmp);
		invert = no_os_field_get((ADF4030_CHAN_INV0) << channel, tmp);
	} else {
		ret = adf4030_spi_read(dev, 0x15, &tmp);
		invert = no_os_field_get((ADF4030_CHAN_INV4) <<
					 (channel - ADF4030_CHANNEL_INV_SEPARATOR), tmp);
	}
	if (ret)
		return ret;

	dev->channels[channel].invert_en = invert;
	*invert_en = dev->channels[channel].invert_en;
	return 0;
}

/**
 * @brief Set the voltage level for the BSYNC channel.
 * @param dev         - The device structure.
 * @param channel     - The channel to set the voltage level for.
 * @param voltage_mv  - The desired voltage level in millivolts.
 * @return            - 0 in case of success or negative error code otherwise.
 */
int adf4030_set_channel_voltage(struct adf4030_dev *dev, uint8_t channel,
				uint32_t voltage_mv)
{
	uint8_t boost, rcm, val;
	uint32_t mv_value;
	uint16_t reg;
	int ret;

	if (!dev || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	if (voltage_mv < ADF4030_RCM_VOLTAGE_MIN0 ||
	    voltage_mv > ADF4030_RCM_VOLTAGE_MAX1)
		return -EINVAL;

	reg = 0x3F + (channel * 2);

	if (voltage_mv < ADF4030_RCM_VOLTAGE_MAX0) {
		boost = 0;
		rcm = adf4030_channel_rcm_compute(dev, voltage_mv, boost);
		mv_value = adf4030_channel_voltage_compute(dev, rcm, boost);
	} else {
		boost = 1;
		rcm = adf4030_channel_rcm_compute(dev, voltage_mv, boost);
		mv_value = adf4030_channel_voltage_compute(dev, rcm, boost);
	}

	val = no_os_field_prep(ADF4030_RCM0, rcm) |
	      no_os_field_prep(ADF4030_BOOST0, boost);
	ret = adf4030_spi_update_bits(dev, reg, ADF4030_RCM0 | ADF4030_BOOST0, val);
	if (ret)
		return ret;

	dev->channels[channel].boost_en = boost;
	dev->channels[channel].rcm_mv = mv_value;
	return 0;
}

/**
 * @brief Get the voltage level for a specific channel.
 * @param dev         - The device structure.
 * @param channel     - The channel to get the voltage level for.
 * @param voltage_mv  - Read value of the voltage level in millivolts.
 * @return            - 0 in case of success or negative error code otherwise.
 */
int adf4030_get_channel_voltage(struct adf4030_dev *dev, uint8_t channel,
				uint32_t *voltage_mv)
{
	uint8_t boost, rcm, tmp;
	uint32_t mv_value;
	uint16_t reg;
	int ret;

	if (!dev || !voltage_mv || channel >= ADF4030_CHANNEL_NUMBER)
		return -EINVAL;

	reg = 0x3F + (channel * 2);

	ret = adf4030_spi_read(dev, reg, &tmp);
	if (ret)
		return ret;
	boost = no_os_field_get(ADF4030_BOOST0, tmp);
	rcm = no_os_field_get(ADF4030_RCM0, tmp);

	mv_value = adf4030_channel_voltage_compute(dev, rcm, boost);

	dev->channels[channel].boost_en = boost;
	dev->channels[channel].rcm_mv = mv_value;
	*voltage_mv = dev->channels[channel].rcm_mv;
	return 0;
}

/**
 * @brief Initializes the adf4030.
 * @param dev	     - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return 	     - 0 in case of success or negative error code.
 */
int adf4030_init(struct adf4030_dev **dev,
		 struct adf4030_init_param *init_param)
{
	struct adf4030_dev *device;
	int ret;

	if (!dev || !init_param || !init_param->spi_init)
		return -EINVAL;

	device = (struct adf4030_dev *)no_os_calloc(1, sizeof(*device));
	if (!device)
		return -ENOMEM;

	ret = no_os_spi_init(&device->spi_desc, init_param->spi_init);
	if (ret)
		goto error_dev;

	device->spi_4wire_en = init_param->spi_4wire_en;
	device->cmos_3v3 = init_param->cmos_3v3;
	device->ref_freq = init_param->ref_freq;
	device->vco_freq = init_param->vco_freq;
	device->ref_div = init_param->ref_div;
	device->bsync_freq_odiv_a = init_param->bsync_freq;
	device->bsync_freq_odiv_b = init_param->bsync_freq;

	ret = adf4030_set_default_regs(device, device->spi_4wire_en);
	if (ret)
		goto error_spi;

	ret = adf4030_check_scratchpad(device);
	if (ret)
		pr_warning("%s:%d ADF4030 scratchpad test failed. Check SPI connection. %x\n",
			   __FILE__,
			   __LINE__, ret);

	ret = adf4030_spi_update_bits(device, 0x5C,
				      no_os_field_prep(ADF4030_CMOS_OV,
						      device->cmos_3v3));
	if (ret)
		goto error_spi;

	ret = adf4030_set_vco_freq(device, device->vco_freq);
	if (ret == -ETIMEDOUT) {
		pr_warning("%s:%d ADF4030 VCO frequency setting failed. %x\n",
			   __FILE__,
			   __LINE__, ret);
	} else if (ret) {
		goto error_spi;
	}

	/* Set BSYNC ODIVA */
	ret = adf4030_set_bsync_freq(device, device->bsync_freq_odiv_a, false);
	if (ret)
		goto error_spi;

	/* Set BSYNC ODIVB */
	ret = adf4030_set_bsync_freq(device, device->bsync_freq_odiv_b, true);
	if (ret)
		goto error_spi;

	*dev = device;

	return 0;

error_spi:
	no_os_spi_remove(device->spi_desc);
error_dev:
	no_os_free(device);

	return ret;
}

/**
 * @brief Free resources allocated for adf4030
 * @param dev 	- The device structure.
 * @return 	- 0 in case of success or negative error code.
 */
int adf4030_remove(struct adf4030_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
