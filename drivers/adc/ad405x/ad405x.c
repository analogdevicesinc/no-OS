/********************************************************************************
 *   @file   ad405x.c
 *   @brief  Implementation of AD405X Driver.
 *   @author George Mois (george.mois@analog.com)
 *   @author Ribhu Das Purkayastha (ribhu.daspurkayastha@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include "ad405x.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/*
 * Reset pattern bytes.
 */
static const uint8_t reset_pattern_buff[18] = {
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFE,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFE,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFE
};
/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_val - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_write(struct ad405x_dev *dev, uint8_t reg_addr, uint8_t reg_val)
{
	uint8_t buff[2] = { 0 };

	if (!dev)
		return -EINVAL;

	/* Should check if address on 15 bits and if reg is 16 bits. */

	buff[0] = reg_addr;
	buff[1] = reg_val;

	return no_os_spi_write_and_read(dev->spi_desc, buff, 2);
}

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_val - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_read(struct ad405x_dev *dev, uint8_t reg_addr, uint8_t *reg_val)
{
	int32_t ret;
	uint8_t buff[2] = { 0 };

	if (!dev)
		return -EINVAL;

	buff[0] = AD405X_SPI_READ | reg_addr;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, 2);
	if (ret)
		return ret;

	*reg_val = buff[1];

	return 0;
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_val - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_update_bits(struct ad405x_dev *dev,
		       uint8_t reg_addr,
		       uint8_t mask,
		       uint8_t reg_val)
{
	int ret;
	uint8_t data;

	if (!dev)
		return -EINVAL;

	ret = ad405x_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_val;

	return ad405x_write(dev, reg_addr, data);
}

/**
 * @brief Send EXIT command.
 * @param dev- The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_exit_command(struct ad405x_dev *dev)
{
	int ret;
	uint8_t buff[1] = { 0xA8 };

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, 1);
	if (!ret)
		dev->operation_mode = AD405X_CONFIG_MODE_OP;

	return ret;
}

/**
 * Send RESET pattern command.
 * @param dev- The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_reset_pattern_command(struct ad405x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_write_and_read(dev->spi_desc, reset_pattern_buff, 18);
	if (!ret)
		dev->operation_mode = AD405X_CONFIG_MODE_OP;

	return ret;
}

/**
 * @brief Software reset the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_soft_reset(struct ad405x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad405x_update_bits(dev,
				 AD405X_REG_INTERFACE_CONFIG_A,
				 AD405X_SW_RESET_MSK,
				 AD405X_SW_RESET);
	if (ret)
		return ret;

	return ad405x_update_bits(dev,
				  AD405X_REG_INTERFACE_CONFIG_A,
				  AD405X_SW_RESET_MSK,
				  0);
}

/**
 * @brief Enter ADC Mode
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_adc_mode(struct ad405x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Set ADC mode. */
	ret = ad405x_update_bits(dev,
				 AD405X_REG_ADC_MODES,
				 AD405X_ADC_MODES_MSK,
				 AD405X_SAMPLE_MODE);
	if (ret)
		return ret;

	/* Enter ADC_MODE. */
	ret = ad405x_update_bits(dev,
				 AD405X_REG_MODE_SET,
				 AD405X_ENTER_ADC_MODE_MSK,
				 no_os_field_prep(AD405X_ENTER_ADC_MODE_MSK, 1));
	if (ret)
		return ret;

	dev->operation_mode = AD405X_ADC_MODE_OP;

	return 0;
}

/**
 * @brief Enter Burst Averaging Mode
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_burst_averaging_mode(struct ad405x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Set Burst Averaging mode. */
	ret = ad405x_update_bits(dev,
				 AD405X_REG_ADC_MODES,
				 AD405X_ADC_MODES_MSK,
				 AD405X_BURST_AVERAGING_MODE);
	if (ret)
		return ret;

	/* Enter ADC_MODE. */
	ret = ad405x_update_bits(dev,
				 AD405X_REG_MODE_SET,
				 AD405X_ENTER_ADC_MODE_MSK,
				 no_os_field_prep(AD405X_ENTER_ADC_MODE_MSK, 1));
	if (ret)
		return ret;

	dev->operation_mode = AD405X_BURST_AVERAGING_MODE_OP;

	return 0;
}

/**
 * @brief Enter Config Mode
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_config_mode(struct ad405x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Send EXIT command . */
	ret = ad405x_exit_command(dev);
	if (ret)
		return ret;

	dev->operation_mode = AD405X_CONFIG_MODE_OP;

	return 0;
}

/**
 * @brief Set operation mode
 * @param dev - The device structure.
 * @param mode - Operation mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_operation_mode(struct ad405x_dev *dev,
			      enum ad405x_operation_mode mode)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (dev->operation_mode != mode) {
		if (dev->operation_mode != AD405X_CONFIG_MODE_OP) {
			ret = ad405x_exit_command(dev);
			if (ret)
				return ret;
		}

		switch (mode) {
		case AD405X_CONFIG_MODE_OP:
			return 0;
		case AD405X_ADC_MODE_OP:
			return ad405x_set_adc_mode(dev);
		case AD405X_BURST_AVERAGING_MODE_OP:
			return ad405x_set_burst_averaging_mode(dev);
		default:
			return -EINVAL;
		}
	}

	return 0;
}

/**
 * @brief Toggle the CNV pin to start a conversion.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_convst(struct ad405x_dev *dev)
{
	int ret;

	ret = no_os_gpio_set_value(dev->gpio_cnv, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	/** CNV High Time 10 ns */
	no_os_udelay(1);

	ret = no_os_gpio_set_value(dev->gpio_cnv, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	/* Conversion Time (CNV Rising Edge to Data Ready) 250ns */
	no_os_udelay(1);

	return 0;
}

/**
 * @brief Read conversion data.
 * @param dev - The device structure.
 * @param data - Pointer to location of buffer where to store the data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_spi_data_read(struct ad405x_dev *dev, int32_t *data)
{
	int ret = 0;
	uint16_t bytes_number = 2;
	uint8_t data_read[4] = { 0x00 };

	switch (dev->operation_mode) {
	case AD405X_ADC_MODE_OP:
		bytes_number = 2;
		break;
	case AD405X_BURST_AVERAGING_MODE_OP:
		bytes_number += dev->active_device;
		break;
	default:
		bytes_number = 2;
		break;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, data_read, bytes_number);
	if (ret)
		return ret;

	switch (bytes_number) {
	case 2:
		*data = no_os_get_unaligned_be16(data_read);
		break;
	case 3:
		*data = no_os_get_unaligned_be24(data_read);
		break;
	default:
		*data = no_os_get_unaligned_be16(data_read);
		break;
	}

	return ret;
}

/**
 * @brief Trigger conversion and read data.
 * @param dev - The device structure.
 * @param data - Pointer to location of buffer where to store the data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_read_val(struct ad405x_dev *dev, int32_t *data)
{
	int ret;
	int32_t data_read;

	ret = ad405x_convst(dev);
	if (ret)
		return -EINVAL;

	ret = ad405x_spi_data_read(dev, &data_read);
	if (!ret)
		*data = data_read;

	return ret;
}

/**
 * @brief Select sample rate for Burst and Autonomous Modes
 * @param dev - The device structure.
 * @param rate - Sample rate.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_sample_rate(struct ad405x_dev *dev,
			   enum ad405x_sample_rate rate)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad405x_update_bits(dev,
				 AD405X_REG_TIMER_CONFIG,
				 AD405X_FS_BURST_AUTO_MSK,
				 no_os_field_prep(AD405X_FS_BURST_AUTO_MSK, rate));
	if (ret)
		return ret;

	dev->rate = rate;

	return 0;
}

/**
 * @brief Set averaging filter window length
 * @param dev - The device structure.
 * @param length - Averaging filter window length.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_averaging_filter_length(struct ad405x_dev *dev,
				       enum ad405x_avg_filter_l length)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Restrict filter length depending on active device selected */
	switch (dev->active_device) {
	case ID_AD4050:
		if (length > AD405X_LENGTH_256)
			return -EINVAL;
		break;
	case ID_AD4052:
		break;

	default:
		return -EINVAL;
	}
	ret = ad405x_update_bits(dev,
				 AD405X_REG_AVG_CONFIG,
				 AD405X_AVG_WIN_LEN_MSK,
				 no_os_field_prep(AD405X_AVG_WIN_LEN_MSK, length));
	if (ret)
		return ret;

	dev->filter_length = length;

	return 0;
}

/**
 * @brief Get averaging filter window length
 * @param dev - The device structure.
 * @return Average filter value
 */
enum ad405x_avg_filter_l ad405x_get_averaging_filter_length(
	struct ad405x_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->filter_length;
}

/**
 * @brief Set DEV_EN signal polarity
 * @param dev - The device structure.
 * @param polarity - DEV_EN signal polarity.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_dev_en_polarity(struct ad405x_dev *dev,
			       enum ad405x_dev_en_polarity polarity)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad405x_update_bits(dev,
				 AD405X_REG_GPIO_CONFIG,
				 AD405X_DEV_EN_POL_MSK,
				 no_os_field_prep(AD405X_DEV_EN_POL_MSK, polarity));
	if (ret)
		return ret;

	dev->polarity = polarity;

	return 0;
}

/**
 * @brief Get DEV_EN signal polarity
 * @param dev - The device structure.
 * @return DEV_EN signal polarity
 */
enum ad405x_dev_en_polarity ad405x_get_dev_en_polarity(struct ad405x_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->polarity;
}

/**
 * @brief Enable INVERT_ON_CHOP
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_enable_invert_on_chop(struct ad405x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad405x_update_bits(dev,
				 AD405X_REG_GPIO_CONFIG,
				 AD405X_INVERT_ON_CHOP_MSK,
				 no_os_field_prep(AD405X_INVERT_ON_CHOP_MSK, AD405X_INVERT_ON_CHOP_ENABLED));
	if (ret)
		return ret;

	dev->invert_on_chop_status = AD405X_INVERT_ON_CHOP_ENABLED;

	return 0;
}

/**
 * @brief Disable INVERT_ON_CHOP
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_disable_invert_on_chop(struct ad405x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad405x_update_bits(dev,
				 AD405X_REG_GPIO_CONFIG,
				 AD405X_INVERT_ON_CHOP_MSK,
				 no_os_field_prep(AD405X_INVERT_ON_CHOP_MSK, AD405X_INVERT_ON_CHOP_DISABLED));
	if (ret)
		return ret;

	dev->invert_on_chop_status = AD405X_INVERT_ON_CHOP_DISABLED;

	return 0;
}

/**
 * @brief Get INVERT_ON_CHOP setting
 * @param dev - The device structure.
 * @return INVERT_ON_CHOP setting
 */
enum ad405x_invert_on_chop ad405x_get_invert_on_chop_state(
	struct ad405x_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->invert_on_chop_status;
}

/**
 * @brief Set GP mode
 * @param dev - The device structure.
 * @param gp - GP1/GP0 select.
 * @param mode - mode select.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_gp_mode(struct ad405x_dev *dev,
		       enum ad405x_gp_select gp,
		       enum ad405x_gp_mode mode)
{
	int ret;
	unsigned char msk;

	if (!dev)
		return -EINVAL;

	switch(gp) {
	case AD405X_GP_0 :
		msk = AD405X_GP0_MODE_MSK;
		break;
	case AD405X_GP_1 :
		msk = AD405X_GP1_MODE_MSK;
		break;
	default :
		return -EINVAL;
	}

	if (gp == AD405X_GP_0 && mode == AD405X_GP_MODE_DEV_RDY)
		return -EINVAL;

	ret = ad405x_update_bits(dev,
				 AD405X_REG_GPIO_CONFIG,
				 msk,
				 no_os_field_prep(msk, mode));
	if (ret)
		return ret;

	switch(gp) {
	case AD405X_GP_0 :
		dev->gp0_mode = mode;
		break;
	case AD405X_GP_1 :
		dev->gp1_mode = mode;
		break;
	default :
		return -EINVAL;
	}

	return 0;
}

/** G */
/**
 * @brief Get GP mode setting
 * @param dev - The device structure.
 * @param gp - GP1/GP0 select.
 * @return GP mode.
 */
enum ad405x_gp_mode ad405x_get_gp_mode(
	struct ad405x_dev *dev,
	enum ad405x_gp_select gp)
{
	if (!dev)
		return -EINVAL;

	switch(gp) {
	case AD405X_GP_0 :
		return dev->gp0_mode;
	case AD405X_GP_1 :
		return dev->gp1_mode;
	default :
		return -EINVAL;
	}
}

/**
 * @brief Set output data format.
 * @param dev - The device structure.
 * @param data_format - format select.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_data_format(struct ad405x_dev *dev,
			   enum ad405x_out_data_format data_format)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad405x_update_bits(dev,
				 AD405X_REG_ADC_MODES,
				 AD405X_ADC_DATA_FORMAT_MSK,
				 no_os_field_prep(AD405X_ADC_DATA_FORMAT_MSK, data_format));
	if (ret)
		return ret;

	dev->data_format = data_format;

	return 0;
}

/* Internal function that initializes GPIOs. */
static int ad405x_request_gpios(struct ad405x_dev *dev,
				struct ad405x_init_param *init_param)
{
	int32_t ret;

	/* CNV */
	ret = no_os_gpio_get_optional(&dev->gpio_cnv, init_param->gpio_cnv);
	if (ret < 0)
		return ret;

	if (dev->gpio_cnv) {
		ret = no_os_gpio_direction_output(dev->gpio_cnv, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}

	/* GPIO0 */
	ret = no_os_gpio_get_optional(&dev->gpio_gpio0, init_param->gpio_gpio0);
	if (ret < 0)
		return ret;

	if (dev->gpio_gpio0) {
		ret = no_os_gpio_direction_input(dev->gpio_gpio0);
		if (ret)
			return ret;
	}

	/* GPIO1 */
	ret = no_os_gpio_get_optional(&dev->gpio_gpio1, init_param->gpio_gpio1);
	if (ret < 0)
		return ret;

	if (dev->gpio_gpio1) {
		ret = no_os_gpio_direction_input(dev->gpio_gpio1);
		if (ret)
			return ret;
	}

	return ret;
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_init(struct ad405x_dev **device,
		struct ad405x_init_param init_param)
{
	struct ad405x_dev *dev;
	uint8_t data;
	uint8_t prod_id[2];
	int ret;

	dev = (struct ad405x_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return - ENOMEM;

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto error_dev;

	/* Reset device using RESET pattern */
	ret = ad405x_reset_pattern_command(dev);
	if (ret) {
		pr_info("Reset failed!\n");
		return - 1;
	}

	/* Verify product ID and active device */
	ret = ad405x_read(dev, AD405X_REG_PRODUCT_ID_H, &prod_id[0]);
	if (ret)
		goto error_spi;

	ret = ad405x_read(dev, AD405X_REG_PRODUCT_ID_L, &prod_id[1]);
	if (ret)
		goto error_spi;

	switch (init_param.active_device) {
	case ID_AD4050:
	case ID_AD4052:
		if (no_os_get_unaligned_be16(prod_id) != PROD_ID_AD4052)
			goto error_spi;

		break;

	default:
		goto error_spi;
	}

	ret = ad405x_request_gpios(dev, &init_param);
	if (ret)
		goto error_dev;

	/* Software Reset */
	ret = ad405x_soft_reset(dev);
	if (ret)
		goto error_spi;

	dev->operation_mode = AD405X_CONFIG_MODE_OP;
	dev->polarity = AD405X_DEV_EN_ACTIVE_HIGH;
	dev->gp1_mode = AD405X_GP_MODE_DEV_RDY;
	dev->invert_on_chop_status = AD405X_INVERT_ON_CHOP_DISABLED;
	dev->gp0_mode = AD405X_GP_MODE_HIGH_Z;
	dev->data_format = AD405X_STRAIGHT_BINARY;
	dev->active_device = init_param.active_device;

	*device = dev;

	return 0;

error_spi :
	no_os_spi_remove(dev->spi_desc);
error_dev :
	free(dev);

	return ret;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_remove(struct ad405x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (dev->gpio_cnv) {
		ret = no_os_gpio_remove(dev->gpio_cnv);
		if (ret)
			return ret;
	}

	if (dev->gpio_gpio0) {
		ret = no_os_gpio_remove(dev->gpio_gpio0);
		if (ret)
			return ret;
	}

	if (dev->gpio_gpio1) {
		ret = no_os_gpio_remove(dev->gpio_gpio1);
		if (ret)
			return ret;
	}


	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	free(dev);

	return ret;
}
