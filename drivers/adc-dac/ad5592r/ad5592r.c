/***************************************************************************//**
 *   @file   ad5592r.c
 *   @brief  Implementation of AD5592R driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018, 2020(c) Analog Devices, Inc.
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
#include "no_os_error.h"
#include "ad5592r-base.h"
#include "ad5592r.h"

const struct ad5592r_rw_ops ad5592r_rw_ops = {
	.write_dac = ad5592r_write_dac,
	.read_adc = ad5592r_read_adc,
	.multi_read_adc = ad5592r_multi_read_adc,
	.reg_write = ad5592r_reg_write,
	.reg_read = ad5592r_reg_read,
	.gpio_read = ad5592r_gpio_read,
};

/**
 * Write NOP and read value.
 *
 * @param dev - The device structure.
 * @param buf - buffer where to read
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t ad5592r_spi_wnop_r16(struct ad5592r_dev *dev, uint16_t *buf)
{
	int32_t ret;
	uint16_t spi_msg_nop = 0; /* NOP */

	ret = no_os_spi_write_and_read(dev->spi, (uint8_t *)&spi_msg_nop,
				       sizeof(spi_msg_nop));
	if (ret < 0)
		return ret;

	*buf = swab16(spi_msg_nop);

	return ret;
}

/**
 * Write DAC channel.
 *
 * @param dev - The device structure.
 * @param chan - The channel number.
 * @param value - DAC value
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_write_dac(struct ad5592r_dev *dev, uint8_t chan,
			  uint16_t value)
{
	if (!dev)
		return -1;

	dev->spi_msg = swab16( NO_OS_BIT(15) | (uint16_t)(chan << 12) | value);

	return no_os_spi_write_and_read(dev->spi, (uint8_t *)&dev->spi_msg,
					sizeof(dev->spi_msg));
}

/**
 * Read ADC channel.
 *
 * @param dev - The device structure.
 * @param chan - The channel number.
 * @param value - ADC value
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_read_adc(struct ad5592r_dev *dev, uint8_t chan,
			 uint16_t *value)
{
	int32_t ret;

	if (!dev)
		return -1;

	dev->spi_msg = swab16((uint16_t)(AD5592R_REG_ADC_SEQ << 11) |
			      NO_OS_BIT(chan));

	ret = no_os_spi_write_and_read(dev->spi, (uint8_t *)&dev->spi_msg,
				       sizeof(dev->spi_msg));
	if (ret < 0)
		return ret;

	/*
	 * Invalid data:
	 * See Figure 40. Single-Channel ADC Conversion Sequence
	 */
	ret = ad5592r_spi_wnop_r16(dev, &dev->spi_msg);
	if (ret < 0)
		return ret;

	ret = ad5592r_spi_wnop_r16(dev, &dev->spi_msg);
	if (ret < 0)
		return ret;

	*value = dev->spi_msg;

	return 0;
}

/**
 * Read Multiple ADC Channels.
 *
 * @param dev - The device structure.
 * @param chans - The ADC channels to be readback
 * @param values - ADC value array
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_multi_read_adc(struct ad5592r_dev *dev, uint16_t chans,
			       uint16_t *values)
{
	int32_t ret;
	uint8_t samples;
	uint8_t i;

	if (!dev)
		return -1;

	samples = no_os_hweight16(chans);

	dev->spi_msg = swab16((uint16_t)(AD5592R_REG_ADC_SEQ << 11) | chans);

	ret = no_os_spi_write_and_read(dev->spi, (uint8_t *)&dev->spi_msg,
				       sizeof(dev->spi_msg));
	if (ret < 0)
		return ret;

	/*
	 * Invalid data:
	 * See Figure 40. Single-Channel ADC Conversion Sequence
	 */
	ret = ad5592r_spi_wnop_r16(dev, &dev->spi_msg);
	if (ret < 0)
		return ret;

	for (i = 0; i < samples; i++) {
		ret = ad5592r_spi_wnop_r16(dev, &dev->spi_msg);
		if (ret < 0)
			return ret;
		values[i] = dev->spi_msg;
	}

	return 0;
}

/**
 * Write register.
 *
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param value - register value
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_reg_write(struct ad5592r_dev *dev, uint8_t reg, uint16_t value)
{
	if (!dev)
		return -1;

	dev->spi_msg = swab16((reg << 11) | value);

	return no_os_spi_write_and_read(dev->spi, (uint8_t *)&dev->spi_msg,
					sizeof(dev->spi_msg));
}

/**
 * Read register.
 *
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param value - register value
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_reg_read(struct ad5592r_dev *dev, uint8_t reg, uint16_t *value)
{
	int32_t ret;

	if (!dev)
		return -1;

	dev->spi_msg = swab16((AD5592R_REG_LDAC << 11) |
			      AD5592R_LDAC_READBACK_EN | (reg << 2) | dev->ldac_mode);

	ret = no_os_spi_write_and_read(dev->spi, (uint8_t *)&dev->spi_msg,
				       sizeof(dev->spi_msg));
	if (ret < 0)
		return ret;

	ret = ad5592r_spi_wnop_r16(dev, &dev->spi_msg);
	if (ret < 0)
		return ret;

	*value = dev->spi_msg;

	return 0;
}

/**
 * Read GPIOs.
 *
 * @param dev - The device structure.
 * @param value - GPIOs value.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_gpio_read(struct ad5592r_dev *dev, uint8_t *value)
{
	int32_t ret;

	if (!dev)
		return -1;

	ret = ad5592r_reg_write(dev, AD5592R_REG_GPIO_IN_EN,
				AD5592R_GPIO_READBACK_EN | dev->gpio_in);
	if (ret < 0)
		return ret;

	ret = ad5592r_spi_wnop_r16(dev, &dev->spi_msg);
	if (ret < 0)
		return ret;

	*value = (uint8_t)dev->spi_msg;

	return 0;
}

/**
 * Initialize AD5593r device.
 *
 * @param dev - The device structure.
 * @param init_param - The initial parameters of the device.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_init(struct ad5592r_dev *dev,
		     struct ad5592r_init_param *init_param)
{
	int32_t ret;
	uint16_t temp_reg_val;

	if (!dev)
		return -1;

	dev->ops = &ad5592r_rw_ops;

	ret = ad5592r_software_reset(dev);
	if (ret < 0)
		return ret;

	ret = ad5592r_set_channel_modes(dev);
	if (ret < 0)
		return ret;

	if(init_param->int_ref) {
		ret = ad5592r_reg_read(dev, AD5592R_REG_PD, &temp_reg_val);
		if (ret < 0)
			return ret;
		temp_reg_val |= AD5592R_REG_PD_EN_REF;

		return ad5592r_reg_write(dev, AD5592R_REG_PD, temp_reg_val);
	}

	return ret;
}
