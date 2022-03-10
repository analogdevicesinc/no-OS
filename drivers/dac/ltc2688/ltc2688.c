/***************************************************************************//**
 *   @file   LTC2688.c
 *   @brief  Implementation of LTC2688 Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include "no_os_error.h"

#include "ltc2688.h" /* LTC2688 definitions. */

static const struct ltc2688_span_tbl ltc2688_span_tbl[] = {
	[LTC2688_VOLTAGE_RANGE_0V_5V] = {0, 5},
	[LTC2688_VOLTAGE_RANGE_0V_10V] = {0, 10},
	[LTC2688_VOLTAGE_RANGE_M5V_5V] = {-5, 5},
	[LTC2688_VOLTAGE_RANGE_M10V_10V] = {-10, 10},
	[LTC2688_VOLTAGE_RANGE_M15V_15V] = {-15, 15}
};
/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/**
 * SPI command write to device.
 * @param dev - The device structure.
 * @param cmd - The command.
 * @param data - The data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
static int32_t _ltc2688_spi_write(struct ltc2688_dev *dev, uint8_t reg,
				  uint16_t data)
{
	uint8_t buf[3];
	int32_t ret;

	if (!dev)
		return -ENODEV;

	buf[0] = reg;
	buf[1] = (data & 0xFF00) >> 8;
	buf[2] = data & 0x00FF;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);

	return ret;
}

/**
 * SPI read from device.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param data - The register data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
static int32_t _ltc2688_spi_read(struct ltc2688_dev *dev, uint8_t reg,
				 uint16_t *data)
{
	uint8_t buf[3] = {0, 0, 0};
	int32_t ret;

	if (!dev)
		return -ENODEV;

	_ltc2688_spi_write(dev, reg | LTC2688_READ_OPERATION, 0x0000);

	buf[0] = LTC2688_CMD_NOOP;
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);

	*data = (buf[1] << 8) | buf[2];

	return ret;
}

/**
 * SPI readback register from device.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param mask - The register mask.
 * @param val - The updated value.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
static int32_t _ltc2688_spi_update_bits(struct ltc2688_dev *dev, uint8_t reg,
					uint16_t mask, uint16_t val)
{
	uint16_t regval;
	int32_t ret;

	ret = _ltc2688_spi_read(dev, reg, &regval);
	if (ret < 0)
		return ret;

	regval &= ~mask;
	regval |= val;

	return _ltc2688_spi_write(dev, reg, regval);
}

/**
 * Power down the selected channels.
 * @param dev - The device structure.
 * @param setting - The setting.
 *		    Accepted values: LTC2688_PWDN(x) | LTC2688_PWDN(y) | ...
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_set_pwr_dac(struct ltc2688_dev *dev, uint16_t setting)
{
	return _ltc2688_spi_write(dev, LTC2688_CMD_POWERDOWN_REG,
				  setting);
}

/**
 * Enable dither/toggle for selected channels.
 * @param dev - The device structure.
 * @param setting - The setting.
 *		    Accepted values: LTC2688_DITH_EN(x) | LTC2688_DITH_EN(y) | ...
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_set_dither_toggle(struct ltc2688_dev *dev, uint16_t setting)
{
	int32_t ret;

	ret = _ltc2688_spi_write(dev, LTC2688_CMD_TOGGLE_DITHER_EN_REG,
				 setting);
	if (ret < 0)
		return ret;

	dev->dither_toggle_en = setting;

	return 0;
}

/**
 * Set channel to dither mode.
 * @param dev - The device structure.
 * @param channel - The channel for which to change the mode.
 * @param en - enable or disable dither mode
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_set_dither_mode(struct ltc2688_dev *dev, uint8_t channel,
				bool en)
{
	uint16_t val = 0;
	int32_t ret;

	if (channel >= LTC2688_DAC_CHANNELS)
		return -ENOENT;

	if (en)
		val = LTC2688_CH_MODE;

	ret = _ltc2688_spi_update_bits(dev, LTC2688_CMD_CH_SETTING(channel),
				       LTC2688_CH_MODE, val);
	if (ret < 0)
		return ret;

	dev->dither_mode[channel] = en;

	return 0;
}

/**
 * Set channel span.
 * @param dev - The device structure.
 * @param channel - The channel for which to change the mode.
 * @param range - Voltage range.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_set_span(struct ltc2688_dev *dev,
			 uint8_t channel, enum ltc2688_voltage_range range)
{
	int32_t ret;

	if (channel >= LTC2688_DAC_CHANNELS)
		return -ENOENT;

	ret = _ltc2688_spi_update_bits(dev, LTC2688_CMD_CH_SETTING(channel),
				       LTC2688_CH_SPAN_MSK, LTC2688_CH_SPAN(range));
	if (ret < 0)
		return ret;

	dev->crt_range[channel] = range;

	return 0;
}

/**
 * Set channel dither phase.
 * @param dev - The device structure.
 * @param channel - The channel for which to change the mode.
 * @param phase - Dither phase.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_set_dither_phase(struct ltc2688_dev *dev,
				 uint8_t channel, enum  ltc2688_dither_phase phase)
{
	int32_t ret;

	if (channel >= LTC2688_DAC_CHANNELS)
		return -ENOENT;

	ret = _ltc2688_spi_update_bits(dev, LTC2688_CMD_CH_SETTING(channel),
				       LTC2688_CH_DIT_PH_MSK, LTC2688_CH_DIT_PH(phase));
	if (ret < 0)
		return ret;

	dev->dither_phase[channel] = phase;

	return 0;
}

/**
 * Set channel dither period.
 * @param dev - The device structure.
 * @param channel - The channel for which to change the mode.
 * @param period - Dither period.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_set_dither_period(struct ltc2688_dev *dev,
				  uint8_t channel, enum  ltc2688_dither_period period)
{
	int32_t ret;

	if (channel >= LTC2688_DAC_CHANNELS)
		return -ENOENT;

	ret = _ltc2688_spi_update_bits(dev, LTC2688_CMD_CH_SETTING(channel),
				       LTC2688_CH_DIT_PER_MSK, LTC2688_CH_DIT_PER(period));
	if (ret < 0)
		return ret;

	dev->dither_period[channel] = period;

	return 0;
}

/**
 * Select register A or B for value.
 * @param dev - The device structure.
 * @param channel - The channel for which to change the mode.
 * @param sel_reg - Select register A or B to store DAC output value.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_select_reg(struct ltc2688_dev *dev,
			   uint8_t channel, enum  ltc2688_a_b_register sel_reg)
{
	int32_t ret;

	if (channel >= LTC2688_DAC_CHANNELS)
		return -ENOENT;

	ret = _ltc2688_spi_update_bits(dev, LTC2688_CMD_A_B_SELECT_REG,
				       NO_OS_BIT(channel), sel_reg << channel);
	if (ret < 0)
		return ret;

	dev->reg_select[channel] = sel_reg;

	return 0;
}

/**
 * Select dither/toggle clock input.
 * @param dev - The device structure.
 * @param channel - The channel for which to change the mode.
 * @param clk_input - Select the source for the clock input.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_select_tg_dith_clk(struct ltc2688_dev *dev, uint8_t channel,
				   enum  ltc2688_clk_input clk_input)
{
	int32_t ret;

	if (channel >= LTC2688_DAC_CHANNELS)
		return -ENOENT;

	ret = _ltc2688_spi_update_bits(dev, LTC2688_CMD_CH_SETTING(channel),
				       LTC2688_CH_TD_SEL_MSK, LTC2688_CH_TD_SEL(clk_input));
	if (ret < 0)
		return ret;

	dev->clk_input[channel] = clk_input;

	return 0;
}

/**
 * Toggle the software source for dither/toggle.
 * @param dev - The device structure.
 * @param channel - The channel for which to change the mode.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_software_toggle(struct ltc2688_dev *dev, uint8_t channel)
{
	int32_t ret;
	uint16_t regval;

	if (channel >= LTC2688_DAC_CHANNELS)
		return -ENOENT;

	ret = _ltc2688_spi_read(dev, LTC2688_CMD_SW_TOGGLE_REG, &regval);
	if (ret < 0)
		return ret;

	regval ^= NO_OS_BIT(channel);

	return _ltc2688_spi_write(dev, LTC2688_CMD_SW_TOGGLE_REG, regval);
}

/**
 * Software reset the device.
 * @param dev - The device structure.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_software_reset(struct ltc2688_dev *dev)
{
	return _ltc2688_spi_update_bits(dev, LTC2688_CMD_CONFIG_REG,
					LTC2688_CONFIG_RST, LTC2688_CONFIG_RST);
}

/**
 *  Sets the output voltage of a channel.
 *
 * @param dev     - The device structure.
 * @param channel - Channel option.
 *
 * @param voltage - Value to be outputted by the DAC(Volts).
 *
 * @return The actual voltage value that can be outputted by the channel.
 */
int32_t ltc2688_set_voltage(struct ltc2688_dev *dev, uint8_t channel,
			    float voltage)
{
	uint16_t offset, gain, code;
	int32_t range_offset, v_ref, ret;

	/* Get the offset, gain and range of the selected channel. */
	ret = _ltc2688_spi_read(dev, LTC2688_CMD_CH_OFFSET(channel), &offset);
	if (ret < 0)
		return ret;

	ret = _ltc2688_spi_read(dev, LTC2688_CMD_CH_GAIN(channel), &gain);
	if (ret < 0)
		return ret;

	range_offset = ltc2688_span_tbl[dev->crt_range[channel]].min;
	v_ref = ltc2688_span_tbl[dev->crt_range[channel]].max -
		ltc2688_span_tbl[dev->crt_range[channel]].min;

	/* Compute the binary code from the value(mA) provided by user. */
	code = (uint32_t)((voltage - range_offset) * (1l << 16) / v_ref);
	if(code > 0xFFFF)
		code = 0xFFFF;

	/* Write to the Data Register of the DAC. */
	return _ltc2688_spi_write(dev, LTC2688_CMD_CH_CODE_UPDATE(channel), code);
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *		       parameters.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_init(struct ltc2688_dev **device,
		     struct ltc2688_init_param init_param)
{
	struct ltc2688_dev *dev;
	uint8_t channel = 0;
	int ret;

	dev = (struct ltc2688_dev*)calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret < 0)
		goto error;

	/* Device Settings */
	ret = ltc2688_software_reset(dev);
	if (ret < 0)
		goto error;

	no_os_mdelay(100);

	/* Powerdown/up channels */
	ret = ltc2688_set_pwr_dac(dev, init_param.pwd_dac_setting);
	if (ret < 0)
		goto error;

	/* Enable dither/toggle */
	ret = ltc2688_set_dither_toggle(dev, init_param.dither_toggle_en);
	if (ret < 0)
		goto error;

	for (channel = 0; channel < LTC2688_DAC_CHANNELS; channel++) {
		/* Setup channel span */
		ret = ltc2688_set_span(dev, channel, init_param.crt_range[channel]);
		if (ret < 0)
			goto error;

		/* Set dither phase */
		ret = ltc2688_set_dither_phase(dev, channel, init_param.dither_phase[channel]);
		if (ret < 0)
			goto error;

		/* Set dither period */
		ret = ltc2688_set_dither_period(dev, channel,
						init_param.dither_period[channel]);
		if (ret < 0)
			goto error;

		ret = ltc2688_set_dither_mode(dev, channel, init_param.dither_mode[channel]);
		if (ret < 0)
			goto error;

		/* Set toggle/dither clock */
		ret = ltc2688_select_tg_dith_clk(dev, channel, init_param.clk_input[channel]);
		if (ret < 0)
			goto error;
	}

	/* Update all dac channels */
	ret = _ltc2688_spi_write(dev, LTC2688_CMD_UPDATE_ALL, 0);
	if (ret < 0)
		goto error;

	*device = dev;
	printf("LTC2688 successfully initialized\n");

	return ret;

error:
	printf("LTC2688 initialization error (%d)\n", ret);
	free(dev);
	return ret;
}

/**
 * @brief Free the resources allocated by ltc2688_init().
 * @param dev - The device structure.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ltc2688_remove(struct ltc2688_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -ENODEV;

	ret = no_os_spi_remove(dev->spi_desc);

	free(dev);

	return ret;
}
