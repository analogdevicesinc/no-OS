/***************************************************************************//**
 *   @file   ltc6953.c
 *   @brief  Implementation of LTC6953 Driver.
 *   @author MTinaco (mariel.tinaco@analog.com)
********************************************************************************
 * Copyright 2023-2024(c) Analog Devices, Inc.
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
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ltc6953.h"
#include "no_os_spi.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

static const uint8_t LTC6953_LUT[2][LTC6953_NUM_REGADDR] = {
	{
		0x25, 0x04, 0x08, 0x00, 0x00, 0x00, 0x0C, 0x01, 0x00, 0x2D,
		0x93, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30
	},
	{
		0x25, 0x04, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x06, 0x8c, 0xe0, 0x1b, 0x00, 0x40, 0x80, 0x24, 0x00,
		0x8c, 0xe0, 0x1b, 0x00, 0x40, 0x80, 0x24, 0x00, 0x8c, 0xe0,
		0x00, 0x00, 0x89, 0x80, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x8c, 0xe0, 0x23, 0x00, 0x00, 0x80, 0x24, 0x00, 0x8c, 0xe0,
		0x23, 0x00, 0x00, 0x80, 0x24, 0x00, 0x13
	}
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Initializes the SPI communication with LTC6953.
 * @param device - the device structure.
 * @param init_param - the initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc6953_init(struct ltc6953_dev **device,
		 struct ltc6953_init_param *init_param)
{
	int ret;
	struct ltc6953_dev *dev;

	dev = (struct ltc6953_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret)
		goto error_init;

	ret = ltc6953_reset(dev);
	if (ret)
		goto error;

	dev->vco_frequency = init_param->vco_frequency;

	*device = dev;

	return 0;
error:
	no_os_spi_remove(dev->spi_desc);
error_init:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Software reset of the device.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc6953_reset(struct ltc6953_dev *device)
{
	return ltc6953_update(device, LTC6953_REG_PD_CTL, LTC6953_POR_MSK,
			      no_os_field_prep(LTC6953_POR_MSK, 1));
}

/**
 * @brief Writes data to LTC6953 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int ltc6953_write(struct ltc6953_dev *dev, uint8_t reg_addr,
		  uint16_t data)
{
	uint8_t buf[LTC6953_BUFF_SIZE_BYTES];

	// LTC6953 is always MSB First
	buf[0] = LTC6953_SPI_ADDR_CMD(reg_addr) | LTC6953_SPI_WRITE_CMD;
	buf[1] = data;

	return no_os_spi_write_and_read(dev->spi_desc, buf,
					LTC6953_BUFF_SIZE_BYTES);
}

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc6953_read(struct ltc6953_dev *dev, uint8_t reg_addr,
		 uint8_t *data)
{
	int ret;
	uint8_t buf[LTC6953_BUFF_SIZE_BYTES];

	buf[0] = LTC6953_SPI_ADDR_CMD(reg_addr) | LTC6953_SPI_READ_CMD;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf,
				       LTC6953_BUFF_SIZE_BYTES);
	if (ret)
		return ret;

	*data = buf[1];

	return 0;
}

/**
 * @brief Update LTC6953 register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Mask for specific register bits to be updated.
 * @param data - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int ltc6953_update(struct ltc6953_dev *dev, uint8_t reg_addr,
		   uint8_t mask, uint8_t data)
{
	uint8_t read_val;
	int ret;

	ret = ltc6953_read(dev, reg_addr, &read_val);
	if (ret)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	return ltc6953_write(dev, reg_addr, read_val);
}

/**
 * @brief Free resources allocated for LTC6953
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_remove(struct ltc6953_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return ret;
}

/**
 * @brief Powers down all LTC6953 output channels.
 * @param dev - The device structure.
 * @param is_pwdn - Set to True to power down all channels.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int ltc6953_power_down_all(struct ltc6953_dev *dev, bool is_pwdn)
{
	return ltc6953_update(dev, LTC6953_REG_PD_CTL, LTC6953_PDALL_MSK,
			      no_os_field_prep(LTC6953_PDALL_MSK, (uint8_t)is_pwdn));
}

/**
 * @brief Enable LTC6953 VCO Input filter.
 * @param dev - The device structure.
 * @param is_en - Set to True to enable input filter.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int ltc6953_enable_filter(struct ltc6953_dev *dev, bool is_en)
{
	return ltc6953_update(dev, LTC6953_REG_PD_CTL, LTC6953_FILTV_MSK,
			      no_os_field_prep(LTC6953_FILTV_MSK, (uint8_t)is_en));
}

/**
 * @brief Powers down LTC6953 VCO Input channel.
 * @param dev - The device structure.
 * @param is_pwdn - Set to True to power down input channel.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int ltc6953_power_down_vco(struct ltc6953_dev *dev, bool is_pwdn)
{
	return ltc6953_update(dev, LTC6953_REG_PD_CTL, LTC6953_PDVCOPK_MSK,
			      no_os_field_prep(LTC6953_PDVCOPK_MSK, (uint8_t)is_pwdn));
}

/**
 * @brief Set output divider for LTC6953 output channel
 * @param dev - The device structure.
 * @param channel - Output channel [0-10]
 * @param divider - Divider setting
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_set_output_divider(struct ltc6953_dev *dev, uint32_t channel,
			       uint32_t divider)
{
	/*
	 * Mx = (MPx + 1) * 2**MDx
	 * */
	int mp, md;
	uint8_t writeval;
	uint32_t out_divider;

	if (channel > 10)
		return -EINVAL;

	for (int i = 0; i < 32; i++) {
		if (divider == (i + 1)) {
			mp = i;
			md = 0;
			break;
		}
		if (i <= 15)
			continue;
		for (int j = 0; j < 7; j ++) {
			out_divider = (i + 1) << j;
			if (divider == out_divider) {
				mp = i;
				md = j;
				break;
			}
			if (divider < out_divider)
				break;
		}
	}

	writeval = no_os_field_prep(LTC6953_MP_MSK, mp) |
		   no_os_field_prep(LTC6953_MD_MSK, md);

	return ltc6953_write(dev, LTC6953_REG_OUTPUT_DIVIDER(channel), writeval);
}

/**
 * @brief Set LTC6953 Channel Power Mode
 * @param dev - The device structure.
 * @param channel - Output channel [0-10]
 * @param mode - Power mode [0-3]
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_power_mode(struct ltc6953_dev *dev, uint32_t channel, int32_t mode)
{
	if ((mode > 3) || (mode < 0))
		return -EINVAL;

	if (channel > 10)
		return -EINVAL;

	return ltc6953_update(dev, LTC6953_REG_CHAN_POWER_DOWN(channel),
			      LTC6953_PD_MSK(channel), LTC6953_PD(channel, mode));
}

/**
 * @brief Set LTC6953 Channel Sync mode
 * @param dev - The device structure.
 * @param channel - Output channel [0-10]
 * @param enable - Set to True to enable synchronization mode [SRQMD]
 * @return Returns 0 in case of success or negative error code.
 */

int ltc6953_enable_sync(struct ltc6953_dev *dev, uint32_t channel, bool enable)
{
	if (channel > 10)
		return -EINVAL;

	return ltc6953_update(dev, LTC6953_REG_OUTPUT_CONFIG(channel),
			      LTC6953_SRQEN_MSK,
			      no_os_field_prep(LTC6953_SRQEN_MSK, (uint8_t)enable));
}

/**
 * @brief Set digital delay/phase of LTC6953 output channel
 * @param dev - The device structure.
 * @param channel - Output channel [0-10]
 * @param delay - 16-bit word for delay value
 * @return Returns 0 in case of success or negative error code.
 */

int ltc6953_set_digital_delay(struct ltc6953_dev *dev, uint32_t channel,
			      uint16_t delay)
{
	int ret;
	uint16_t ddel_high;
	uint16_t ddel_low;

	if (delay > 4095)
		return -EINVAL;

	if (channel > 10)
		return -EINVAL;

	ddel_low  = (uint8_t)no_os_field_get(LTC6953_DDEL_LOW_MSK, delay);
	ddel_high = (uint8_t)no_os_field_get(LTC6953_DDEL_HIGH_MSK,
					     no_os_shift_right(delay, 8));

	ret = ltc6953_update(dev, LTC6953_REG_DDEL_HIGH(channel), LTC6953_DDEL_HIGH_MSK,
			     no_os_field_prep(LTC6953_DDEL_HIGH_MSK, ddel_high));
	if (ret)
		return ret;
	return ltc6953_update(dev, LTC6953_REG_DDEL_LOW(channel), LTC6953_DDEL_LOW_MSK,
			      no_os_field_prep(LTC6953_DDEL_LOW_MSK, ddel_low));
}

/**
 * @brief Set analog delay/phase of LTC6953 output channel
 * @param dev - The device structure.
 * @param channel - Output channel [0-10]
 * @param delay - 16-bit word for delay value
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_set_analog_delay(struct ltc6953_dev *dev, uint32_t channel,
			     uint16_t delay)
{
	if (delay > 63)
		return -EINVAL;

	if (channel > 10)
		return -EINVAL;

	return ltc6953_update(dev, LTC6953_REG_ADEL(channel), LTC6953_ADEL_MSK,
			      no_os_field_prep(LTC6953_ADEL_MSK, delay));
}

/**
 * @brief Set LTC6953 output channel mode.
 * @param dev - The device structure.
 * @param channel - Output channel [0-10]
 * @param mode - Output channel mode [0-3]
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_set_mode(struct ltc6953_dev *dev, uint32_t channel, uint8_t mode)
{
	if ((mode > 3) || (mode < 0))
		return -EINVAL;

	if (channel > 10)
		return -EINVAL;

	return ltc6953_update(dev, LTC6953_REG_OUTPUT_CONFIG(channel), LTC6953_MODE_MSK,
			      no_os_field_prep(LTC6953_MODE_MSK, mode));
}

/**
 * @brief Set LTC6953 output channel inversion.
 * @param dev - The device structure.
 * @param channel - Output channel [0-10].
 * @param is_invert - Inversion setting.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_invert_output(struct ltc6953_dev *dev, uint32_t channel,
			  bool is_invert)
{
	if (channel > 10)
		return -EINVAL;

	return ltc6953_update(dev, LTC6953_REG_OUTPUT_CONFIG(channel), LTC6953_OINV_MSK,
			      no_os_field_prep(LTC6953_OINV_MSK, (uint8_t)is_invert));
}

/**
 * @brief Set LTC6953 EZSYNC mode.
 * @param dev - The device structure.
 * @param is_en - Synchronization setting.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_ezsync_mode(struct ltc6953_dev *dev, bool is_en)
{
	return ltc6953_update(dev, LTC6953_REG_SYNC_CONFIG, LTC6953_EZMD_MSK,
			      no_os_field_prep(LTC6953_EZMD_MSK, (uint8_t)is_en));
}

/**
 * @brief Set LTC6953 SRQ mode.
 * @param dev - The device structure.
 * @param is_en - Synchronization setting.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_sync_mode(struct ltc6953_dev *dev, bool is_en)
{
	return ltc6953_update(dev, LTC6953_REG_SYNC_CONFIG, LTC6953_SRQMD_MSK,
			      no_os_field_prep(LTC6953_SRQMD_MSK, (uint8_t)is_en));
}

/**
 * @brief Set LTC6953 SSRQ mode.
 * @param dev - The device structure.
 * @param is_en - Synchronization setting.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_ssrq_mode(struct ltc6953_dev *dev, bool is_en)
{
	return ltc6953_update(dev, LTC6953_REG_SYNC_CONFIG, LTC6953_SSRQ_MSK,
			      no_os_field_prep(LTC6953_SSRQ_MSK, (uint8_t)is_en));
}

/**
 * @brief Set LTC6953 SYSCT/Num pulse value.
 * @param dev - The device structure.
 * @param num_pulse - Number of pulses.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_num_pulse(struct ltc6953_dev *dev, uint8_t num_pulse)
{
	if ((num_pulse > 3) || (num_pulse < 0))
		return -EINVAL;

	return ltc6953_update(dev, LTC6953_REG_SYNC_CONFIG, LTC6953_SYSCT_MSK,
			      no_os_field_prep(LTC6953_SYSCT_MSK, num_pulse));
}

/**
 * @brief Set LTC6953 TEMPO value.
 * @param dev - The device structure.
 * @param is_en - TEMPO setting.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_enable_temp_stat(struct ltc6953_dev *dev, bool is_en)
{
	return ltc6953_update(dev, LTC6953_REG_TEMP, LTC6953_TEMPO_MSK,
			      no_os_field_prep(LTC6953_TEMPO_MSK, (uint8_t)is_en));
}

/**
 * @brief Read LTC6953 Check VCO Input stats.
 * @param dev - The device structure.
 * @param is_ok - True if VCOOK bit is 1, False if VCOOK bit is 0
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_vco_status(struct ltc6953_dev *dev, bool *is_ok)
{
	int ret;
	uint8_t readval;

	ret = ltc6953_read(dev, LTC6953_REG_VCO_STATUS, &readval);
	if (ret)
		return ret;

	*is_ok = (bool)no_os_field_get(LTC6953_VCOOK_MSK, readval);

	return 0;
}

/**
 * @brief Read LTC6953 INVSTAT bit.
 * @param dev - The device structure.
 * @param status - Container variable for INVSTAT pin.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_get_invstat(struct ltc6953_dev *dev, bool *status)
{
	int ret;
	uint8_t readval;

	ret = ltc6953_read(dev, LTC6953_REG_STAT, &readval);
	if (ret)
		return ret;

	*status = (bool)no_os_field_get(LTC6953_INVSTAT_MSK, readval);

	return 0;
}

/**
 * @brief Write LTC6953 INVSTAT bit.
 * @param dev - The device structure.
 * @param status - Container for INVSTAT value
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_set_invstat(struct ltc6953_dev *dev, bool status)
{
	return ltc6953_update(dev, LTC6953_REG_STAT, LTC6953_INVSTAT_MSK,
			      no_os_field_prep(LTC6953_INVSTAT_MSK, (uint8_t)status));
}

/**
 * @brief Set LTC6953 x bitfield status.
 * @param dev - The device structure.
 * @param x - Value for x bitfield
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_set_x(struct ltc6953_dev *dev, uint8_t x)
{
	if (x > LTC6953_X_MAX)
		return -EINVAL;

	return ltc6953_update(dev, LTC6953_REG_STAT, LTC6953_STAT_OUT_MSK,
			      no_os_field_prep(LTC6953_STAT_OUT_MSK, x));
}

/**
 * @brief Set LTC6953 x bitfield status.
 * @param dev - The device structure.
 * @param x - Container value for x bitfield.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_get_x(struct ltc6953_dev *dev, uint8_t *x)
{
	int ret;
	uint8_t readval;

	ret = ltc6953_read(dev, LTC6953_REG_STAT, &readval);
	if (ret)
		return ret;

	*x = no_os_field_get(LTC6953_STAT_OUT_MSK, readval);

	return 0;
}

/**
 * @brief Read LTC6953 Part number.
 * @param dev - The device structure.
 * @param rev - container variable for Revision number.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_read_rev(struct ltc6953_dev *dev, uint8_t *rev)
{
	int ret;
	uint8_t readval;

	ret = ltc6953_read(dev, LTC6953_REG_CHIP_INFO, &readval);

	if (ret)
		return ret;

	*rev = no_os_field_get(LTC6953_REV_MSK, readval);

	return 0;
}

/**
 * @brief Read LTC6953 Part number.
 * @param dev - The device structure.
 * @param part - container variable for Part number.
 * @return Returns 0 in case of success or negative error code.
 */
int ltc6953_read_part(struct ltc6953_dev *dev, uint8_t *part)
{
	int ret;
	uint8_t readval;

	ret = ltc6953_read(dev, LTC6953_REG_CHIP_INFO, &readval);

	if (ret)
		return ret;

	*part = no_os_field_get(LTC6953_PART_MSK, readval);

	return 0;
}
