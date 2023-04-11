/***************************************************************************//**
 *   @file   admv8818.c
 *   @brief  Implementation of admv8818 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include <malloc.h>
#include "admv8818.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_units.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

static const unsigned long long freq_range_hpf[4][2] = {
	{1750000000ULL, 3550000000ULL},
	{3400000000ULL, 7250000000ULL},
	{6600000000, 12000000000},
	{12500000000, 19900000000}
};

static const unsigned long long freq_range_lpf[4][2] = {
	{2050000000ULL, 3850000000ULL},
	{3350000000ULL, 7250000000ULL},
	{7000000000, 13000000000},
	{12550000000, 18500000000}
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Writes data to ADMV8818 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv8818_spi_write(struct admv8818_dev *dev, uint8_t reg_addr,
		       uint8_t data)
{
	uint8_t buff[ADMV8818_BUFF_SIZE_BYTES];

	buff[0] = reg_addr;
	buff[1] = data;

	return no_os_spi_write_and_read(dev->spi_desc, buff,
					ADMV8818_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ADMV8818 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv8818_spi_read(struct admv8818_dev *dev, uint8_t reg_addr,
		      uint8_t *data)
{
	uint8_t buff[ADMV8818_BUFF_SIZE_BYTES];
	int ret;

	buff[0] = ADMV8818_SPI_READ_CMD | reg_addr;
	buff[1] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, ADMV8818_BUFF_SIZE_BYTES);
	if(ret)
		return ret;

	*data = buff[1];

	return 0;
}

/**
 * @brief Update ADMV8818 register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Mask for specific register bits to be updated.
 * @param data - Data written to the device (requires prior bit shifting).
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv8818_spi_update_bits(struct admv8818_dev *dev, uint8_t reg_addr,
			     uint8_t mask, uint8_t data)
{
	uint8_t read_val;
	int ret;

	ret = admv8818_spi_read(dev, reg_addr, &read_val);
	if (ret)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	return admv8818_spi_write(dev, reg_addr, read_val);
}

/**
 * @brief Set the HPF Frequency.
 * @param dev - The device structure.
 * @param freq - The HPF Frequency to be set.
 * @return Returns 0 in case of success or negative error code.
 */
int admv8818_hpf_select(struct admv8818_dev *dev, unsigned long long freq)
{
	unsigned int hpf_step = 0, hpf_band = 0, i, j;
	unsigned long long freq_step;
	int ret;

	if (freq < freq_range_hpf[0][0])
		goto hpf_write;

	if (freq > freq_range_hpf[3][1]) {
		hpf_step = 15;
		hpf_band = 4;

		goto hpf_write;
	}

	/* Find and compute the closest HPF band and state relative to the input frequency */
	for (i = 0; i < 4; i++) {
		freq_step = (freq_range_hpf[i][1] - freq_range_hpf[i][0]) / 15;

		if (freq > freq_range_hpf[i][0] &&
		    (freq < freq_range_hpf[i][1] + freq_step)) {
			hpf_band = i + 1;

			for (j = 1; j <= 16; j++) {
				if (freq < (freq_range_hpf[i][0] + (freq_step * j))) {
					hpf_step = j - 1;
					break;
				}
			}
			break;
		}
	}

	/* Close HPF frequency gap between 12 and 12.5 GHz */
	if (freq >= 12000 * HZ_PER_MHZ && freq <= 12500 * HZ_PER_MHZ) {
		hpf_band = 3;
		hpf_step = 15;
	}

hpf_write:
	ret = admv8818_spi_update_bits(dev, ADMV8818_REG_WR0_SW,
				       ADMV8818_SW_IN_SET_WR0_MSK |
				       ADMV8818_SW_IN_WR0_MSK,
				       no_os_field_prep(ADMV8818_SW_IN_SET_WR0_MSK, 1) |
				       no_os_field_prep(ADMV8818_SW_IN_WR0_MSK, hpf_band));
	if (ret)
		return ret;

	return admv8818_spi_update_bits(dev, ADMV8818_REG_WR0_FILTER,
					ADMV8818_HPF_WR0_MSK,
					no_os_field_prep(ADMV8818_HPF_WR0_MSK, hpf_step));
}

/**
 * @brief Get the HPF Frequency.
 * @param dev - The device structure.
 * @param freq - The HPF Frequency.
 * @return Returns 0 in case of success or negative error code.
 */
int admv8818_read_hpf_freq(struct admv8818_dev *dev, unsigned long long *freq)
{
	unsigned int hpf_band, hpf_state;
	uint8_t data;
	int ret;

	ret = admv8818_spi_read(dev, ADMV8818_REG_WR0_SW, &data);
	if (ret)
		return ret;

	hpf_band = no_os_field_get(ADMV8818_SW_IN_WR0_MSK, data);
	if (!hpf_band  || hpf_band > 4) {
		*freq = 0;
		return ret;
	}

	ret = admv8818_spi_read(dev, ADMV8818_REG_WR0_FILTER, &data);
	if (ret)
		return ret;

	hpf_state = no_os_field_get(ADMV8818_HPF_WR0_MSK, data);

	/* Compute HPF value based on the band and state values read from the registers */
	*freq = (freq_range_hpf[hpf_band - 1][1] - freq_range_hpf[hpf_band - 1][0]) /
		15;
	*freq = freq_range_hpf[hpf_band - 1][0] + (*freq * hpf_state);

	return 0;
}

/**
 * @brief Set the LPF Frequency.
 * @param dev - The device structure.
 * @param freq - The LPF Frequency to be set.
 * @return Returns 0 in case of success or negative error code.
 */
int admv8818_lpf_select(struct admv8818_dev *dev, unsigned long long freq)
{
	unsigned int lpf_step = 0, lpf_band = 0, i, j;
	unsigned long long freq_step;
	int ret;

	if (freq > freq_range_lpf[3][1])
		goto lpf_write;

	if (freq < freq_range_lpf[0][0]) {
		lpf_band = 1;

		goto lpf_write;
	}

	/* Find and compute the closest LPF band and state relative to the input frequency */
	for (i = 0; i < 4; i++) {
		if (freq > freq_range_lpf[i][0] && freq < freq_range_lpf[i][1]) {
			lpf_band = i + 1;
			freq_step = (freq_range_lpf[i][1] - freq_range_lpf[i][0]) / 15;

			for (j = 0; j <= 15; j++) {
				if (freq < (freq_range_lpf[i][0] + (freq_step * j))) {
					lpf_step = j;
					break;
				}
			}
			break;
		}
	}

lpf_write:
	ret = admv8818_spi_update_bits(dev, ADMV8818_REG_WR0_SW,
				       ADMV8818_SW_OUT_SET_WR0_MSK |
				       ADMV8818_SW_OUT_WR0_MSK,
				       no_os_field_prep(ADMV8818_SW_OUT_SET_WR0_MSK, 1) |
				       no_os_field_prep(ADMV8818_SW_OUT_WR0_MSK, lpf_band));
	if (ret)
		return ret;

	return admv8818_spi_update_bits(dev, ADMV8818_REG_WR0_FILTER,
					ADMV8818_LPF_WR0_MSK,
					no_os_field_prep(ADMV8818_LPF_WR0_MSK, lpf_step));
}

/**
 * @brief Get the HPF Frequency.
 * @param dev - The device structure.
 * @param freq - The HPF Frequency.
 * @return Returns 0 in case of success or negative error code.
 */
int admv8818_read_lpf_freq(struct admv8818_dev *dev, unsigned long long *freq)
{
	unsigned int lpf_band, lpf_state;
	uint8_t data;
	int ret;

	ret = admv8818_spi_read(dev, ADMV8818_REG_WR0_SW, &data);
	if (ret)
		return ret;

	lpf_band = no_os_field_get(ADMV8818_SW_OUT_WR0_MSK, data);
	if (!lpf_band || lpf_band > 4) {
		*freq= 0;
		return ret;
	}

	ret = admv8818_spi_read(dev, ADMV8818_REG_WR0_FILTER, &data);
	if (ret)
		return ret;

	lpf_state = no_os_field_get(ADMV8818_LPF_WR0_MSK, data);

	/* Compute LPF value based on the band and state values read from the registers */
	*freq = (freq_range_lpf[lpf_band - 1][1] - freq_range_lpf[lpf_band - 1][0]) /
		15;
	*freq = freq_range_lpf[lpf_band - 1][0] + (*freq * lpf_state);

	return 0;
}

/**
 * @brief Set the RF Input Band Select.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int admv8818_rfin_select(struct admv8818_dev *dev)
{
	int ret;

	ret = admv8818_hpf_select(dev, dev->rf_in);
	if (ret)
		return ret;

	return admv8818_lpf_select(dev, dev->rf_in);
}

/**
 * @brief Initializes the admv8818.
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code.
 */
int admv8818_init(struct admv8818_dev **device,
		  struct admv8818_init_param *init_param)
{
	uint8_t chip_id;
	struct admv8818_dev *dev;
	int ret;

	dev = (struct admv8818_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->rf_in = init_param->rf_in;
	dev->mode = init_param->mode;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error_dev;

	ret = admv8818_spi_update_bits(dev, ADMV8818_REG_SPI_CONFIG_A,
				       ADMV8818_SOFTRESET_N_MSK |
				       ADMV8818_SOFTRESET_MSK,
				       no_os_field_prep(ADMV8818_SOFTRESET_N_MSK, 1) |
				       no_os_field_prep(ADMV8818_SOFTRESET_MSK, 1));
	if (ret)
		goto error_spi;

	ret = admv8818_spi_update_bits(dev, ADMV8818_REG_SPI_CONFIG_A,
				       ADMV8818_SDOACTIVE_N_MSK |
				       ADMV8818_SDOACTIVE_MSK,
				       no_os_field_prep(ADMV8818_SDOACTIVE_N_MSK, 1) |
				       no_os_field_prep(ADMV8818_SDOACTIVE_MSK, 1));
	if (ret)
		goto error_spi;

	ret = admv8818_spi_read(dev, ADMV8818_REG_CHIPTYPE, &chip_id);
	if (ret)
		goto error_spi;

	if (chip_id != ADMV8818_CHIP_ID) {
		ret = -EINVAL;
		goto error_spi;
	}

	ret = admv8818_spi_update_bits(dev, ADMV8818_REG_SPI_CONFIG_B,
				       ADMV8818_SINGLE_INSTRUCTION_MSK,
				       no_os_field_prep(ADMV8818_SINGLE_INSTRUCTION_MSK, 1));

	if (ret)
		goto error_spi;

	if (dev->mode == ADMV8818_AUTO) {
		ret = admv8818_rfin_select(dev);
		if (ret)
			goto error_spi;
	}

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief ADMV8818 Resources Deallocation.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admv8818_remove(struct admv8818_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
