/***************************************************************************//**
*   @file   ade7754.c
*   @brief  Implementation of ADE7754 Driver.
*   @author REtz (radu.etz@analog.com)
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

#include "ade7754.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_crc16.h"
#include "no_os_print_log.h"
#include <stdlib.h>
#include <errno.h>
#include <math.h>

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_init(struct ade7754_dev **device,
		 struct ade7754_init_param init_param)
{
	int ret;
	/* ADE7754 device structure */
	struct ade7754_dev *dev;
	/* value read from register */
	uint32_t reg_val;
	/* timeout value */
	uint16_t timeout = 1000;

	dev = (struct ade7754_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc,
			     init_param.spi_init);
	if (ret)
		goto error_spi;

	/* interrupt */
	dev->irq_ctrl = init_param.irq_ctrl;
	/* reset gpio */
	dev->gpio_reset = init_param.gpio_reset;
	/* reset the device */
	if (dev->gpio_reset) {
		ret = ade7754_hw_reset(dev);
		if (ret)
			goto error_spi;
	} else {
		ret = ade7754_sw_reset(dev);
		if (ret)
			goto error_spi;
	}
	/* wait for reset ack */
	do {
		ret = ade7754_get_int_status(dev, ADE7754_RESET_MSK, &reg_val);
		if (ret)
			goto error_spi;
	} while ((!reg_val) && (timeout--));
	if (!timeout) {
		ret = -ETIMEDOUT;
		goto error_spi;
	}
	/* reset the status register */
	ret = ade7754_clear_irq_status(dev, &reg_val);
	if (ret)
		goto error_spi;
	/* Read version product */
	ret = ade7754_version_product(dev, &reg_val);
	if (ret)
		goto error_spi;

	if (reg_val != ADE7754_VERSION) {
		ret = -ENODEV;
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
 * @brief Setup the ADE7754 device.
 * @param dev - The device structure.
 * @return	0 in case of success, negative error code otherwise.
 */
int ade7754_setup(struct ade7754_dev *dev)
{
	int ret;

	/* set the measurement channel for period */
	ret = ade7754_period_source(dev, ADE7754_PHASE_A);
	if (ret)
		return ret;
	/* set the peak measurement channel */
	ret = ade7754_peak_source(dev, ADE7754_PHASE_A);
	if (ret)
		return ret;
	/* set the zero crossing channel */
	ret = ade7754_zxsel(dev, ADE7754_PHASE_A);
	if (ret)
		return ret;
	/* set the waveform register update */
	ret = ade7754_wave_update_rate(dev, ADE7754_DATA_RATE_26_KSPS);
	if (ret)
		return ret;
	/* set the source of sampled data for wave register */
	ret = ade7754_wave_sample_data_source(dev, ADE7754_VOLTAGE_PHASE_A);
	if (ret)
		return ret;
	/* set the formula for active energy calculation */
	ret = ade7754_active_energy_formula(dev, ADE7754_ACTIVE_E_FORMULA_1);
	if (ret)
		return ret;
	/* set the formula for apparent energy calculation */
	ret = ade7754_apparent_energy_formula(dev, ADE7754_APP_E_FORMULA_1);
	if (ret)
		return ret;
	/* set the current gain */
	ret = ade7754_current_gain(dev, ADE7754_PGA_GAIN_1);
	if (ret)
		return ret;
	/* set the voltage gain */
	ret = ade7754_voltage_gain(dev, ADE7754_PGA_GAIN_1);
	if (ret)
		return ret;
	/* set the operation mode of the ADCs */
	ret = ade7754_adcs_mode(dev, ADE7754_NORMAL_OPERATION);
	if (ret)
		return ret;
	/* enable zero crossing interrupts */
	ret = ade7754_enable_irq(dev, ADE7754_ZXA_MSK, 1);
	if (ret)
		return ret;
	ret = ade7754_enable_irq(dev, ADE7754_ZXB_MSK, 1);
	if (ret)
		return ret;
	ret = ade7754_enable_irq(dev, ADE7754_ZXC_MSK, 1);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from 32bit register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_read(struct ade7754_dev *dev, uint16_t reg_addr,
		 int32_t *reg_data)
{
	int ret;
	/* number of bytes */
	uint8_t no_bytes = 2;
	/* offset of data read in the read buffer */
	uint8_t data_byte_offset = 1;
	/* buffer for data read */
	uint8_t buff[4] = { 0 };
	/* index */
	uint8_t i;
	/* register addres */
	uint32_t addr;

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	/* set the read command */
	buff[0] = (uint8_t) no_os_field_prep(NO_OS_GENMASK(5, 0), reg_addr);

	/* 8/5 bits registers */
	if (IS_8BITS_REG(reg_addr) || IS_5BITS_REG(reg_addr)) {
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes);
		if (ret)
			return ret;
		if (IS_5BITS_REG(reg_addr))
			*reg_data = no_os_sign_extend32(buff[data_byte_offset], 4);
		else
			*reg_data = no_os_sign_extend32(buff[data_byte_offset], 7);
	} else if (IS_16BITS_REG(reg_addr) || IS_12BITS_REG(reg_addr)
		   || IS_15BITS_REG(reg_addr)) {
		/* 16/15/12 bits registers */
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 1);
		if (ret)
			return ret;
		if (IS_12BITS_REG(reg_addr))
			*reg_data = no_os_sign_extend32(no_os_field_prep(NO_OS_GENMASK(11, 0),
							no_os_get_unaligned_be16(&buff[data_byte_offset])), 11);
		else if (IS_15BITS_REG(reg_addr))
			*reg_data = no_os_sign_extend32(no_os_get_unaligned_be16(
								&buff[data_byte_offset]), 14);
		else
			*reg_data = no_os_sign_extend32(no_os_get_unaligned_be16(
								&buff[data_byte_offset]), 15);
	} else {
		/* 24 bits registers */
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 2);
		if (ret)
			return ret;
		*reg_data = no_os_sign_extend32(no_os_get_unaligned_be24(
							&buff[data_byte_offset]), 23);
	}

	return 0;
}

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_write(struct ade7754_dev *dev, uint16_t reg_addr,
		  uint32_t reg_data)
{
	int ret;
	/* number of bytes */
	uint8_t no_bytes = 2;
	/* buffer for data write */
	uint8_t buff[4] = {0};
	/* register addres */
	uint16_t addr;
	/* data to be written */
	uint32_t data;
	/* offset of data to be written in the buffer */
	uint8_t data_byte_offset = 1;
	int i;

	if (!dev)
		return -ENODEV;

	buff[0] = (uint8_t) no_os_field_prep(NO_OS_GENMASK(5, 0), reg_addr);
	/* set read bit */
	buff[0] |= ADE7754_SPI_READ;

	/* 8/5 bits registers */
	if (IS_8BITS_REG(reg_addr) || IS_5BITS_REG(reg_addr)) {
		if (IS_5BITS_REG(reg_addr))
			reg_data = no_os_field_prep(NO_OS_GENMASK(4, 0), reg_data);
		buff[1] = (uint8_t)reg_data;

		return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes);
	} else if (IS_16BITS_REG(reg_addr) || IS_12BITS_REG(reg_addr)
		   || IS_15BITS_REG(reg_addr)) {
		/* 16/15/12 bits registers */
		if (IS_12BITS_REG(reg_addr))
			reg_data = no_os_field_prep(NO_OS_GENMASK(11, 0), reg_data);
		else if (IS_15BITS_REG(reg_addr))
			reg_data = no_os_field_prep(NO_OS_GENMASK(14, 0), reg_data);
		no_os_put_unaligned_be16(reg_data, &buff[data_byte_offset]);

		return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 1);
	} else {
		/* 24 bits registers */
		no_os_put_unaligned_be24(reg_data, &buff[data_byte_offset]);

		return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 2);
	}
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_update_bits(struct ade7754_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data)
{
	int ret;
	/* data to be written */
	uint32_t data;

	ret = ade7754_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade7754_write(dev, reg_addr, data);
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_remove(struct ade7754_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_reset);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_remove(dev->irq_ctrl);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Reset the device using SW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_sw_reset(struct ade7754_dev *dev)
{
	int ret;

	ret = ade7754_update_bits(dev, ADE7754_REG_OPMODE, ADE7754_SWRST_MSK,
				  no_os_field_prep(ADE7754_SWRST_MSK, 1));
	if (ret)
		return ret;

	/* Wait for device to initialize */
	no_os_udelay(ADE7754_RESET_DEL);

	return 0;
}

/**
 * @brief Reset the device using HW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_hw_reset(struct ade7754_dev *dev)
{
	int ret;

	if (!dev || !dev->gpio_reset)
		return -ENODEV;

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Version product
 * @param dev - The device structure.
 * @param data_read - The version product read value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_version_product(struct ade7754_dev *dev, uint32_t *data_read)
{
	int ret;
	/* version product */
	uint32_t version;

	if (!data_read)
		return -EINVAL;

	ret = ade7754_read(dev, ADE7754_REG_VERSION_PRODUCT, &version);
	if (ret)
		return ret;

	*data_read = version;

	return 0;
}

/**
 * @brief Write Ipeak val
 * @param dev - The device structure.
 * @param val - Ipeak set val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_write_ipk_val(struct ade7754_dev *dev, uint32_t val)
{
	int ret;

	ret = ade7754_write(dev, ADE7754_REG_IPEAK, val);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Write Vpeak val
 * @param dev - The device structure.
 * @param val - Vpeak set val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_write_vpk_val(struct ade7754_dev *dev, uint32_t val)
{
	int ret;

	ret = ade7754_write(dev, ADE7754_REG_VPEAK, val);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get interrupt indicator from STATUS register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_get_int_status(struct ade7754_dev *dev, uint32_t msk,
			   uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!status)
		return -EINVAL;

	ret = ade7754_read(dev, ADE7754_REG_STATUS, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Clear irq status flags.
 * @param dev - The device structure.
 * @param reg_data - value of the status register
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_clear_irq_status(struct ade7754_dev *dev, int32_t *reg_data)
{
	int ret;
	uint32_t data;

	if (!reg_data)
		return -EINVAL;

	ret = ade7754_read(dev, ADE7754_REG_RSTATUS, &data);
	if (ret)
		return ret;

	*reg_data = data;

	return 0;
}

/**
 * @brief Enable/Disable interrupt.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param en -Enable/Disable
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_enable_irq(struct ade7754_dev *dev, uint32_t msk, uint8_t en)
{
	return ade7754_update_bits(dev, ADE7754_REG_IRQEN, msk,
				   no_os_field_prep(msk, en));
}

/**
 * @brief Select waveform register update rate.
 * @param dev - The device structure.
 * @param sel - Update rate
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_wave_update_rate(struct ade7754_dev *dev,
			     enum ade7754_data_rate sel)
{
	return ade7754_update_bits(dev, ADE7754_REG_WAVMODE, ADE7754_DTRT_MSK,
				   no_os_field_prep(ADE7754_DTRT_MSK, sel));
}

/**
 * @brief Select source of sampled data for wave register.
 * @param dev - The device structure.
 * @param sel - Update rate
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_wave_sample_data_source(struct ade7754_dev *dev,
				    enum ade7754_wavsel sel)
{
	if (sel < ADE7754_VOLTAGE_PHASE_A || sel > ADE7754_CURRENT_PHASE_C)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_WAVMODE, ADE7754_WAVSEL_MSK,
				   no_os_field_prep(ADE7754_WAVSEL_MSK, sel));
}

/**
 * @brief Select the gain of the current channels inputs.
 * @param dev - The device structure.
 * @param sel - gain value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_current_gain(struct ade7754_dev *dev,
			 enum ade7754_pga_gain sel)
{
	if (sel < ADE7754_PGA_GAIN_1 || sel > ADE7754_PGA_GAIN_4)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_GAIN, ADE7754_PGA1_MSK,
				   no_os_field_prep(ADE7754_PGA1_MSK, sel));
}

/**
 * @brief Select the gain of the voltage channels inputs.
 * @param dev - The device structure.
 * @param sel - gain value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_voltage_gain(struct ade7754_dev *dev,
			 enum ade7754_pga_gain sel)
{
	if (sel < ADE7754_PGA_GAIN_1 || sel > ADE7754_PGA_GAIN_4)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_GAIN, ADE7754_PGA2_MSK,
				   no_os_field_prep(ADE7754_PGA2_MSK, sel));
}

/**
 * @brief Select the formula used for active energy calculation
 * @param dev - The device structure.
 * @param sel - formula value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_active_energy_formula(struct ade7754_dev *dev,
				  enum ade7754_watm sel)
{
	if (sel < ADE7754_ACTIVE_E_FORMULA_1 || sel > ADE7754_ACTIVE_E_FORMULA_3)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_WATMODE, ADE7754_WATM_MSK,
				   no_os_field_prep(ADE7754_WATM_MSK, sel));
}

/**
 * @brief Select the formula used for apparent energy calculation
 * @param dev - The device structure.
 * @param sel - formula value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_apparent_energy_formula(struct ade7754_dev *dev,
				    enum ade7754_vamod sel)
{
	if (sel < ADE7754_APP_E_FORMULA_1 || sel > ADE7754_APP_E_FORMULA_3)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_VAMODE, ADE7754_VAMOD_MSK,
				   no_os_field_prep(ADE7754_VAMOD_MSK, sel));
}

/**
 * @brief Select the source of the measurement of the voltage line period.
 * @param dev - The device structure.
 * @param sel - period measurement source
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_period_source(struct ade7754_dev *dev,
			  enum ade7754_phsel sel)
{
	if (sel < ADE7754_PHASE_A || sel > ADE7754_PHASE_C)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_MMODE, ADE7754_PERDSEL_MSK,
				   no_os_field_prep(ADE7754_PERDSEL_MSK, sel));
}

/**
 * @brief  Select the phases used for counting the number of zero
 *  crossing in the line active and apparent accumulation modes
 *  and	enables these phases for the zero-crossing timeout detection,
 *  zero crossing, period measurement, and SAG detection.
 * @param dev - The device structure.
 * @param sel - phase selection
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_zxsel(struct ade7754_dev *dev,
		  enum ade7754_phsel sel)
{
	if (sel < ADE7754_PHASE_A || sel > ADE7754_PHASE_C)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_MMODE, ADE7754_ZXSEL_MSK,
				   no_os_field_prep(ADE7754_ZXSEL_MSK, sel));
}

/**
 * @brief Select part of the line active energy formula separately,
 * depending on the line active energy measurement method
 * @param dev - The device structure.
 * @param sel - formula part selection
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_lwat_formula_term(struct ade7754_dev *dev,
			      enum ade7754_formsel sel)
{
	if (sel < ADE7754_FORMULA_PART_1 || sel > ADE7754_FORMULA_PART_3)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_WATMODE, ADE7754_LWATSEL_MSK,
				   no_os_field_prep(ADE7754_LWATSEL_MSK, sel));
}

/**
 * @brief Select part of the line apparent energy formula separately,
 * depending on the line active energy measurement method.
 * @param dev - The device structure.
 * @param sel - formula part selection
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_lva_formula_term(struct ade7754_dev *dev,
			     enum ade7754_formsel sel)
{
	if (sel < ADE7754_FORMULA_PART_1 || sel > ADE7754_FORMULA_PART_3)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_VAMODE, ADE7754_LVASEL_MSK,
				   no_os_field_prep(ADE7754_LVASEL_MSK, sel));
}

/**
 * @brief Select part of the active energy formula separately,
 * depending on the line active energy measurement method.
 * @param dev - The device structure.
 * @param sel - formula part selection
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_wat_formula_term(struct ade7754_dev *dev,
			     enum ade7754_formsel sel)
{
	if (sel < ADE7754_FORMULA_PART_1 || sel > ADE7754_FORMULA_PART_3)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_WATMODE, ADE7754_WATSEL_MSK,
				   no_os_field_prep(ADE7754_WATSEL_MSK, sel));
}

/**
 * @brief Select part of the apparent energy formula separately,
 * depending on the line active energy measurement method.
 * @param dev - The device structure.
 * @param sel - formula part selection
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_va_formula_term(struct ade7754_dev *dev,
			    enum ade7754_formsel sel)
{
	if (sel < ADE7754_FORMULA_PART_1 || sel > ADE7754_FORMULA_PART_3)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_VAMODE, ADE7754_VASEL_MSK,
				   no_os_field_prep(ADE7754_VASEL_MSK, sel));
}

/**
 * @brief Select the source of the voltage & current peak detection.
 * @param dev - The device structure.
 * @param sel - peak detection source
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_peak_source(struct ade7754_dev *dev,
			enum ade7754_phsel sel)
{
	if (sel < ADE7754_PHASE_A || sel > ADE7754_PHASE_C)
		return -EINVAL;
	return ade7754_update_bits(dev, ADE7754_REG_MMODE, ADE7754_PEAKSEL_MSK,
				   no_os_field_prep(ADE7754_PEAKSEL_MSK, sel));
}

/**
 * @brief Operation mode of the ADCs.
 * @param dev - The device structure.
 * @param sel - mode of the ADCs
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_adcs_mode(struct ade7754_dev *dev,
		      enum ade7754_dismod sel)
{
	return ade7754_update_bits(dev, ADE7754_REG_OPMODE, ADE7754_DISMOD_MSK,
				   no_os_field_prep(ADE7754_DISMOD_MSK, sel));
}

/**
 * @brief Read energy values
 * @param dev - The device structure.
 * @param data - Structure to store energy values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_energy_vals(struct ade7754_dev *dev,
			struct ade7754_energy_values *data)
{
	int ret;
	/* temporary value read from register */
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read accumulated active energy */
	ret = ade7754_read(dev, ADE7754_REG_AENERGY, &temp_val);
	if (ret)
		return ret;
	data->active_energy_reg_val = temp_val;

	/* read accumulated apparent energy */
	ret = ade7754_read(dev, ADE7754_REG_VAENERGY, &temp_val);
	if (ret)
		return ret;
	data->apparent_energy_reg_val = temp_val;

	return 0;
}

/**
 * @brief Read rms values phase A
 * @param dev - The device structure.
 * @param data - Structure to store rms values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_rms_vals_phase_a(struct ade7754_dev *dev,
			     struct ade7754_rms_values *data)
{
	int ret;
	/* temporary value read from register */
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read voltage rms value phase A */
	ret = ade7754_read(dev, ADE7754_REG_AVRMS, &temp_val);
	if (ret)
		return ret;
	data->voltage_rms_reg_val_phase_a = temp_val;

	/* read current rms value phase A */
	ret = ade7754_read(dev, ADE7754_REG_AIRMS, &temp_val);
	if (ret)
		return ret;
	data->current_rms_reg_val_phase_a = temp_val;

	return 0;
}

/**
 * @brief Read rms values phase B
 * @param dev - The device structure.
 * @param data - Structure to store rms values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_rms_vals_phase_b(struct ade7754_dev *dev,
			     struct ade7754_rms_values *data)
{
	int ret;
	/* temporary value read from register */
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read voltage rms value phase B */
	ret = ade7754_read(dev, ADE7754_REG_BVRMS, &temp_val);
	if (ret)
		return ret;
	data->voltage_rms_reg_val_phase_b = temp_val;

	/* read current rms value phase B */
	ret = ade7754_read(dev, ADE7754_REG_BIRMS, &temp_val);
	if (ret)
		return ret;
	data->current_rms_reg_val_phase_b = temp_val;

	return 0;
}

/**
 * @brief Read rms values phase C
 * @param dev - The device structure.
 * @param data - Structure to store rms values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_rms_vals_phase_c(struct ade7754_dev *dev,
			     struct ade7754_rms_values *data)
{
	int ret;
	/* temporary value read from register */
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read voltage rms value phase C */
	ret = ade7754_read(dev, ADE7754_REG_CVRMS, &temp_val);
	if (ret)
		return ret;
	data->voltage_rms_reg_val_phase_c = temp_val;

	/* read current rms value phase C */
	ret = ade7754_read(dev, ADE7754_REG_CIRMS, &temp_val);
	if (ret)
		return ret;
	data->current_rms_reg_val_phase_c = temp_val;

	return 0;
}

/**
 * @brief Read period value
 * @param dev - The device structure.
 * @param data - Structure to store the period val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_period_val(struct ade7754_dev *dev,
		       struct ade7754_period_value *data)
{
	int ret;
	/* temporary value read from register */
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read period value */
	ret = ade7754_read(dev, ADE7754_REG_PERIOD, &temp_val);
	if (ret)
		return ret;
	data->per_reg_val = temp_val;

	return 0;
}

/**
 * @brief Read temperature value
 * @param dev - The device structure.
 * @param data - Structure to store the temperature val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7754_temperature_val(struct ade7754_dev *dev,
			    struct ade7754_temp_value *data)
{
	int ret;
	/* temporary value read from register */
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read temperature value */
	ret = ade7754_read(dev, ADE7754_REG_TEMP, &temp_val);
	if (ret)
		return ret;
	data->temp_reg_val = temp_val;

	return 0;
}
