/***************************************************************************//**
*   @file   ade7758.c
*   @brief  Implementation of ADE7758 Driver.
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

#include "ade7758.h"
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

int ade7758_init(struct ade7758_dev **device,
		 struct ade7758_init_param init_param)
{
	int ret;
	/* ADE7758 device structure */
	struct ade7758_dev *dev;
	/* value read from register */
	uint32_t reg_val;
	/* timeout value */
	uint16_t timeout = 1000;

	dev = (struct ade7758_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc,
			     init_param.spi_init);
	if (ret)
		goto error_spi;

	/* interrupt */
	dev->irq_ctrl = init_param.irq_ctrl;
	/* software reset */
	ret = ade7758_sw_reset(dev);
	if (ret)
		goto error_spi;
	/* wait for reset ack */
	do {
		ret = ade7758_get_int_status(dev, ADE7758_RESET_MSK, &reg_val);
		if (ret)
			goto error_spi;
	} while ((!reg_val) && (timeout--));
	if (!timeout) {
		ret = -ETIMEDOUT;
		goto error_spi;
	}
	/* reset the status register */
	ret = ade7758_clear_irq_status(dev, &reg_val);
	if (ret)
		goto error_spi;
	/* Read version product */
	ret = ade7758_version_product(dev, &reg_val);
	if (ret)
		goto error_spi;

	if (reg_val != ADE7758_VERSION) {
		pr_err("ERROR device not recognized!!!\n");
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
 * @brief Setup the ADE7758 device.
 * @param dev - The device structure.
 * @return	0 in case of success, negative error code otherwise.
 */
int ade7758_setup(struct ade7758_dev *dev)
{
	int ret;

	/* set the waveform register update rate */
	ret = ade7758_wave_update_rate(dev, ADE7758_DATA_RATE_26_04KSPS);
	if (ret)
		return ret;

	/* set the source of sampled data for wave register */
	ret = ade7758_wave_sample_data_source(dev, ADE7758_VOLTAGE);
	if (ret)
		return ret;

	/* set the phase of sampled data for wave register */
	ret = ade7758_wave_phase_sel(dev, ADE7758_PHASE_A);
	if (ret)
		return ret;

	/* set the input to the energy accumulation registers */
	ret = ade7758_consel(dev, ADE7758_CONSEL_0);
	if (ret)
		return ret;

	/* set the source of the measurement of the voltage line frequency */
	ret = ade7758_freq_source(dev, ADE7758_PHASE_A);
	if (ret)
		return ret;

	/* set the operation mode of the ADCs */
	ret = ade7758_adcs_mode(dev, ADE7758_NORMAL_OPERATION);
	if (ret)
		return ret;

	/* enable zero crossing interrupts */
	ret = ade7758_enable_irq(dev, ADE7758_ZXA_MSK, 1);
	if (ret)
		return ret;
	ret = ade7758_enable_irq(dev, ADE7758_ZXB_MSK, 1);
	if (ret)
		return ret;
	ret = ade7758_enable_irq(dev, ADE7758_ZXC_MSK, 1);
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
int ade7758_read(struct ade7758_dev *dev, uint16_t reg_addr,
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
	buff[0] = (uint8_t) no_os_field_prep(NO_OS_GENMASK(6, 0), reg_addr);

	/* 8/7 bits registers */
	if (IS_8BITS_REG(reg_addr) || IS_7BITS_REG(reg_addr)) {
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes);
		if (ret)
			return ret;
		if (IS_7BITS_REG(reg_addr))
			*reg_data = no_os_sign_extend32(no_os_field_prep(NO_OS_GENMASK(6, 0),
							buff[data_byte_offset]), 6);
		else
			*reg_data = no_os_sign_extend32(buff[data_byte_offset], 7);
	} else if (IS_16BITS_REG(reg_addr) || IS_12BITS_REG(reg_addr)) {
		/* 16/12 bits registers */
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 1);
		if (ret)
			return ret;
		if (IS_12BITS_REG(reg_addr))
			*reg_data = no_os_sign_extend32(no_os_field_prep(NO_OS_GENMASK(11, 0),
							no_os_get_unaligned_be16(&buff[data_byte_offset])), 11);
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
int ade7758_write(struct ade7758_dev *dev, uint16_t reg_addr,
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

	buff[0] = (uint8_t) no_os_field_prep(NO_OS_GENMASK(6, 0), reg_addr);
	/* set read bit */
	buff[0] |= ADE7758_SPI_READ;

	/* 8/7 bits registers */
	if (IS_8BITS_REG(reg_addr) || IS_7BITS_REG(reg_addr)) {
		if (IS_7BITS_REG(reg_addr))
			reg_data = no_os_field_prep(NO_OS_GENMASK(6, 0), reg_data);
		buff[1] = (uint8_t)reg_data;

		return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes);
	} else if (IS_16BITS_REG(reg_addr) || IS_12BITS_REG(reg_addr)) {
		/* 16/12 bits registers */
		if (IS_12BITS_REG(reg_addr))
			reg_data = no_os_field_prep(NO_OS_GENMASK(11, 0), reg_data);
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
int ade7758_update_bits(struct ade7758_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data)
{
	int ret;
	/* data to be written */
	uint32_t data;

	ret = ade7758_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade7758_write(dev, reg_addr, data);
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_remove(struct ade7758_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = no_os_spi_remove(dev->spi_desc);
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
int ade7758_sw_reset(struct ade7758_dev *dev)
{
	int ret;

	ret = ade7758_update_bits(dev, ADE7758_REG_OPMODE, ADE7758_SWRST_MSK,
				  no_os_field_prep(ADE7758_SWRST_MSK, 1));
	if (ret)
		return ret;

	/* Wait for device to initialize */
	no_os_mdelay(ADE7758_RESET_DEL);

	return 0;
}

/**
 * @brief Version product
 * @param dev - The device structure.
 * @param data_read - The version product read value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_version_product(struct ade7758_dev *dev, uint32_t *data_read)
{
	int ret;
	/* version product */
	uint32_t version;

	if (!data_read)
		return -EINVAL;

	ret = ade7758_read(dev, ADE7758_REG_VERSION_PRODUCT, &version);
	if (ret)
		return ret;

	*data_read = version;

	return 0;
}

/**
 * @brief read Ipeak val
 * @param dev - The device structure.
 * @param val - Ipeak val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_read_ipk_val(struct ade7758_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!val)
		return -EINVAL;

	ret = ade7758_read(dev, ADE7758_REG_IPEAK, &reg_val);
	if (ret)
		return ret;
	*val = reg_val;

	return 0;
}

/**
 * @brief read Vpeak val
 * @param dev - The device structure.
 * @param val - Vpeak val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_vpk_val(struct ade7758_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!val)
		return -EINVAL;

	ret = ade7758_read(dev, ADE7758_REG_VPEAK, &reg_val);
	if (ret)
		return ret;
	*val = reg_val;

	return 0;
}

/**
 * @brief Get interrupt indicator from STATUS register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_get_int_status(struct ade7758_dev *dev, uint32_t msk,
			   uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!status)
		return -EINVAL;

	ret = ade7758_read(dev, ADE7758_REG_STATUS, &reg_val);
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
int ade7758_clear_irq_status(struct ade7758_dev *dev, int32_t *reg_data)
{
	int ret;
	uint32_t data;

	if (!reg_data)
		return -EINVAL;

	ret = ade7758_read(dev, ADE7758_REG_RSTATUS, &data);
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
int ade7758_enable_irq(struct ade7758_dev *dev, uint32_t msk, uint8_t en)
{
	return ade7758_update_bits(dev, ADE7758_REG_MASK, msk,
				   no_os_field_prep(msk, en));
}

/**
 * @brief Select waveform register update rate.
 * @param dev - The device structure.
 * @param sel - Update rate
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_wave_update_rate(struct ade7758_dev *dev,
			     enum ade7758_data_rate sel)
{
	int ret;

	switch (sel) {
	case ADE7758_DATA_RATE_3_25KSPS:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_DTRT_MSK,
					  no_os_field_prep(ADE7758_DTRT_MSK, ADE7758_DATA_RATE_3_25KSPS));
		break;
	case ADE7758_DATA_RATE_6_51KSPS:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_DTRT_MSK,
					  no_os_field_prep(ADE7758_DTRT_MSK, ADE7758_DATA_RATE_6_51KSPS));
		break;
	case ADE7758_DATA_RATE_13_02KSPS:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_DTRT_MSK,
					  no_os_field_prep(ADE7758_DTRT_MSK, ADE7758_DATA_RATE_13_02KSPS));
		break;
	default:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_DTRT_MSK,
					  no_os_field_prep(ADE7758_DTRT_MSK, ADE7758_DATA_RATE_26_04KSPS));
		break;
	}

	return ret;
}

/**
 * @brief Select source of sampled data for wave register.
 * @param dev - The device structure.
 * @param sel - Update rate
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_wave_sample_data_source(struct ade7758_dev *dev,
				    enum ade7758_wavesel sel)
{
	int ret;

	switch (sel) {
	case ADE7758_APPARENT_POWER:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_WAVSEL_MSK,
					  no_os_field_prep(ADE7758_WAVSEL_MSK, ADE7758_APPARENT_POWER));
		break;
	case ADE7758_REACTIVE_POWER:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_WAVSEL_MSK,
					  no_os_field_prep(ADE7758_WAVSEL_MSK, ADE7758_REACTIVE_POWER));
		break;
	case ADE7758_ACTIVE_POWER:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_WAVSEL_MSK,
					  no_os_field_prep(ADE7758_WAVSEL_MSK, ADE7758_ACTIVE_POWER));
		break;
	case ADE7758_VOLTAGE:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_WAVSEL_MSK,
					  no_os_field_prep(ADE7758_WAVSEL_MSK, ADE7758_VOLTAGE));
		break;
	default:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_WAVSEL_MSK,
					  no_os_field_prep(ADE7758_WAVSEL_MSK, ADE7758_CURRENT));
		break;
	}

	return ret;
}

/**
 * @brief Select the phase of sampled data for wave register.
 * @param dev - The device structure.
 * @param sel - phase
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_wave_phase_sel(struct ade7758_dev *dev,
			   enum ade7758_phsel sel)
{
	int ret;

	switch (sel) {
	case ADE7758_PHASE_C:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_PHSEL_MSK,
					  no_os_field_prep(ADE7758_PHSEL_MSK, ADE7758_PHASE_C));
		break;
	case ADE7758_PHASE_B:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_PHSEL_MSK,
					  no_os_field_prep(ADE7758_PHSEL_MSK, ADE7758_PHASE_B));
		break;
	default:
		ret = ade7758_update_bits(dev, ADE7758_REG_WAVMODE, ADE7758_PHSEL_MSK,
					  no_os_field_prep(ADE7758_PHSEL_MSK, ADE7758_PHASE_A));
		break;
	}

	return ret;
}

/**
 * @brief Select the input to the energy accumulation registers.
 * @param dev - The device structure.
 * @param sel - consel value (see datasheet)
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_consel(struct ade7758_dev *dev,
		   enum ade7758_consel sel)
{
	int ret;

	switch (sel) {
	case ADE7758_CONSEL_2:
		ret = ade7758_update_bits(dev, ADE7758_REG_COMPMODE, ADE7758_CONSEL_MSK,
					  no_os_field_prep(ADE7758_CONSEL_MSK, ADE7758_CONSEL_2));
		break;
	case ADE7758_CONSEL_1:
		ret = ade7758_update_bits(dev, ADE7758_REG_COMPMODE, ADE7758_CONSEL_MSK,
					  no_os_field_prep(ADE7758_CONSEL_MSK, ADE7758_CONSEL_1));
		break;
	default:
		ret = ade7758_update_bits(dev, ADE7758_REG_COMPMODE, ADE7758_CONSEL_MSK,
					  no_os_field_prep(ADE7758_CONSEL_MSK, ADE7758_CONSEL_0));
		break;
	}

	return ret;
}

/**
 * @brief Select the source of the measurement of the voltage line frequency.
 * @param dev - The device structure.
 * @param sel - frequency measurement source
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_freq_source(struct ade7758_dev *dev,
			enum ade7758_phsel sel)
{
	int ret;

	switch (sel) {
	case ADE7758_PHASE_C:
		ret = ade7758_update_bits(dev, ADE7758_REG_MMODE, ADE7758_FREQSEL_MSK,
					  no_os_field_prep(ADE7758_FREQSEL_MSK, ADE7758_PHASE_C));
		break;
	case ADE7758_PHASE_B:
		ret = ade7758_update_bits(dev, ADE7758_REG_MMODE, ADE7758_FREQSEL_MSK,
					  no_os_field_prep(ADE7758_FREQSEL_MSK, ADE7758_PHASE_B));
		break;
	default:
		ret = ade7758_update_bits(dev, ADE7758_REG_MMODE, ADE7758_FREQSEL_MSK,
					  no_os_field_prep(ADE7758_FREQSEL_MSK, ADE7758_PHASE_A));
		break;
	}

	return ret;
}

/**
 * @brief Operation mode of the ADCs.
 * @param dev - The device structure.
 * @param sel - mode of the ADCs
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_adcs_mode(struct ade7758_dev *dev,
		      enum ade7758_dismod sel)
{
	int ret;

	switch (sel) {
	case ADE7758_POWER_DOWN_MODE:
		ret = ade7758_update_bits(dev, ADE7758_REG_OPMODE, ADE7758_DISMOD_MSK,
					  no_os_field_prep(ADE7758_DISMOD_MSK, ADE7758_POWER_DOWN_MODE));
		break;
	case ADE7758_OFF_VOLTAGE_ADCS_REDIRECTED_TO_CURRENT:
		ret = ade7758_update_bits(dev, ADE7758_REG_OPMODE, ADE7758_DISMOD_MSK,
					  no_os_field_prep(ADE7758_DISMOD_MSK,
							  ADE7758_OFF_VOLTAGE_ADCS_REDIRECTED_TO_CURRENT));
		break;
	case ADE7758_OFF_CURRENT_ADCS_REDIRECTED_TO_VOLTAGE:
		ret = ade7758_update_bits(dev, ADE7758_REG_OPMODE, ADE7758_DISMOD_MSK,
					  no_os_field_prep(ADE7758_DISMOD_MSK,
							  ADE7758_OFF_CURRENT_ADCS_REDIRECTED_TO_VOLTAGE));
		break;
	case ADE7758_SWAP_VOLTAGE_CURRENT:
		ret = ade7758_update_bits(dev, ADE7758_REG_OPMODE, ADE7758_DISMOD_MSK,
					  no_os_field_prep(ADE7758_DISMOD_MSK, ADE7758_SWAP_VOLTAGE_CURRENT));
		break;
	case ADE7758_SLEEP_MODE:
		ret = ade7758_update_bits(dev, ADE7758_REG_OPMODE, ADE7758_DISMOD_MSK,
					  no_os_field_prep(ADE7758_DISMOD_MSK, ADE7758_SLEEP_MODE));
		break;
	case ADE7758_SWITCH_OFF_VOLTAGE_ADC:
		ret = ade7758_update_bits(dev, ADE7758_REG_OPMODE, ADE7758_DISMOD_MSK,
					  no_os_field_prep(ADE7758_DISMOD_MSK, ADE7758_SWITCH_OFF_VOLTAGE_ADC));
		break;
	case ADE7758_SWITCH_OFF_CURRENT_ADC:
		ret = ade7758_update_bits(dev, ADE7758_REG_OPMODE, ADE7758_DISMOD_MSK,
					  no_os_field_prep(ADE7758_DISMOD_MSK, ADE7758_SWITCH_OFF_CURRENT_ADC));
		break;
	default:
		ret = ade7758_update_bits(dev, ADE7758_REG_OPMODE, ADE7758_DISMOD_MSK,
					  no_os_field_prep(ADE7758_DISMOD_MSK, ADE7758_NORMAL_OPERATION));
		break;
	}

	return ret;
}

/**
 * @brief Read energy values phase A
 * @param dev - The device structure.
 * @param data - Structure to store energy values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_energy_vals_phase_a(struct ade7758_dev *dev,
				struct ade7758_energy_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read accumulated active energy phase A */
	ret = ade7758_read(dev, ADE7758_REG_AWATTHR, &temp_val);
	if (ret)
		return ret;
	data->active_energy_reg_val_phase_a = (int32_t)temp_val;

	/* read accumulated reactive energy phase A */
	ret = ade7758_read(dev, ADE7758_REG_AVARHR, &temp_val);
	if (ret)
		return ret;
	data->acc_reactive_energy_reg_val_phase_a = (int32_t)temp_val;

	/* read accumulated apparent energy phase A */
	ret = ade7758_read(dev, ADE7758_REG_AVAHR, &temp_val);
	if (ret)
		return ret;
	data->apparent_energy_reg_val_phase_a = (int32_t)temp_val;

	return 0;
}

/**
 * @brief Read energy values phase B
 * @param dev - The device structure.
 * @param data - Structure to store energy values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_energy_vals_phase_b(struct ade7758_dev *dev,
				struct ade7758_energy_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read accumulated active energy phase B */
	ret = ade7758_read(dev, ADE7758_REG_BWATTHR, &temp_val);
	if (ret)
		return ret;
	data->active_energy_reg_val_phase_b = (int32_t)temp_val;

	/* read accumulated reactive energy phase B*/
	ret = ade7758_read(dev, ADE7758_REG_BVARHR, &temp_val);
	if (ret)
		return ret;
	data->acc_reactive_energy_reg_val_phase_b = (int32_t)temp_val;

	/* read accumulated apparent energy phase B */
	ret = ade7758_read(dev, ADE7758_REG_BVAHR, &temp_val);
	if (ret)
		return ret;
	data->apparent_energy_reg_val_phase_b = (int32_t)temp_val;

	return 0;
}

/**
 * @brief Read energy values phase C
 * @param dev - The device structure.
 * @param data - Structure to store energy values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_energy_vals_phase_c(struct ade7758_dev *dev,
				struct ade7758_energy_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read accumulated active energy phase C */
	ret = ade7758_read(dev, ADE7758_REG_CWATTHR, &temp_val);
	if (ret)
		return ret;
	data->active_energy_reg_val_phase_c = (int32_t)temp_val;

	/* read accumulated reactive energy phase C*/
	ret = ade7758_read(dev, ADE7758_REG_CVARHR, &temp_val);
	if (ret)
		return ret;
	data->acc_reactive_energy_reg_val_phase_c = (int32_t)temp_val;

	/* read accumulated apparent energy phase C */
	ret = ade7758_read(dev, ADE7758_REG_CVAHR, &temp_val);
	if (ret)
		return ret;
	data->apparent_energy_reg_val_phase_c = (int32_t)temp_val;

	return 0;
}

/**
 * @brief Read rms values phase A
 * @param dev - The device structure.
 * @param data - Structure to store rms values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_rms_vals_phase_a(struct ade7758_dev *dev,
			     struct ade7758_rms_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read voltage rms value phase A */
	ret = ade7758_read(dev, ADE7758_REG_AVRMS, &temp_val);
	if (ret)
		return ret;
	data->voltage_rms_reg_val_phase_a = temp_val;

	/* read current rms value phase A */
	ret = ade7758_read(dev, ADE7758_REG_AIRMS, &temp_val);
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
int ade7758_rms_vals_phase_b(struct ade7758_dev *dev,
			     struct ade7758_rms_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read voltage rms value phase B */
	ret = ade7758_read(dev, ADE7758_REG_BVRMS, &temp_val);
	if (ret)
		return ret;
	data->voltage_rms_reg_val_phase_b = temp_val;

	/* read current rms value phase B */
	ret = ade7758_read(dev, ADE7758_REG_BIRMS, &temp_val);
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
int ade7758_rms_vals_phase_c(struct ade7758_dev *dev,
			     struct ade7758_rms_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read voltage rms value phase C */
	ret = ade7758_read(dev, ADE7758_REG_CVRMS, &temp_val);
	if (ret)
		return ret;
	data->voltage_rms_reg_val_phase_c = temp_val;

	/* read current rms value phase C */
	ret = ade7758_read(dev, ADE7758_REG_CIRMS, &temp_val);
	if (ret)
		return ret;
	data->current_rms_reg_val_phase_c = temp_val;

	return 0;
}

/**
 * @brief Read frequency/period value
 * @param dev - The device structure.
 * @param data - Structure to store the period val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_frequency_period_val(struct ade7758_dev *dev,
				 struct ade7758_freq_per_value *data)
{
	int ret;
	/* temporary value read from register */
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read periode value */
	ret = ade7758_read(dev, ADE7758_REG_FREQ, &temp_val);
	if (ret)
		return ret;
	data->freq_per_reg_val = (uint32_t)temp_val;

	return 0;
}

/**
 * @brief Read temperature value
 * @param dev - The device structure.
 * @param data - Structure to store the temperature val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7758_temperature_val(struct ade7758_dev *dev,
			    struct ade7758_temp_value *data)
{
	int ret;
	/* temporary value read from register */
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read temperature value */
	ret = ade7758_read(dev, ADE7758_REG_TEMP, &temp_val);
	if (ret)
		return ret;
	data->temp_reg_val = (uint32_t)temp_val;

	return 0;
}
