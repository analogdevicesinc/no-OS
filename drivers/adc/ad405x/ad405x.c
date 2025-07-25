/***************************************************************************//**
 *   @file   ad405x.c
 *   @brief  Implementation of AD405X Driver.
 *   @author George Mois (george.mois@analog.com)
 *   @author Ribhu Das Purkayastha (ribhu.daspurkayastha@analog.com)
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2024~2025(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "ad405x.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

static const uint8_t reset_pattern_buff[18] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE
};

static const uint8_t ad405x_part_id[] = {
	[ID_AD4050] = AD405X_PROD_ID_0_RESET(0),
	[ID_AD4052] = AD405X_PROD_ID_0_RESET(2),
	[ID_AD4056] = AD405X_PROD_ID_0_RESET(6),
	[ID_AD4058] = AD405X_PROD_ID_0_RESET(8),
	[ID_AD4060] = AD405X_PROD_ID_0_RESET(10),
	[ID_AD4062] = AD405X_PROD_ID_0_RESET(12),
};

static int32_t ad405x_adc_array_conv(struct ad405x_dev *dev,
				     uint8_t *data);
static int ad405x_update_bits(struct ad405x_dev *dev, uint8_t addr,
			      uint8_t mask,
			      uint8_t value);
static void ad405x_set_to_reset(struct ad405x_dev *dev);
static int ad405x_set_adc_modes(struct ad405x_dev *dev, uint8_t mode);
static int ad405x_reset_pattern_command(struct ad405x_dev *dev);
static int ad405x_init_gpios(struct ad405x_dev *dev,
			     struct ad405x_init_param *init_param);
static bool ad405x_fits_nbits(uint16_t number, bool is_signed, uint8_t bit);

/**
 * @brief Initialize the communication device.
 * @param device - The device descriptor.
 * @param init_param - The device initial descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_init(struct ad405x_dev **device,
		struct ad405x_init_param init_param)
{
	struct ad405x_dev *dev;
	uint8_t data[2];
	uint8_t vendor_val[2];
	int ret;

	switch (init_param.dev_type) {
	case ID_AD4050:
	case ID_AD4052:
	case ID_AD4056:
	case ID_AD4058:
	case ID_AD4060:
	case ID_AD4062:
		break;
	default:
		return -EINVAL;
	}

	dev = (struct ad405x_dev *)no_os_calloc(1, sizeof(*dev));

	if (!dev)
		return -ENOMEM;

	dev->comm_type = init_param.comm_type;

	ret = ad405x_init_gpios(dev, &init_param);
	if (ret)
		goto error_dev;

	if (dev->comm_type == AD405X_SPI_COMM) {
		ret = no_os_spi_init(&dev->com_desc.spi_desc,
				     init_param.comm_init.spi_init);
	} else {
		/* Set I3C values to reset values */
		dev->extra.i3c_extra.addr_ptr = AD405X_REG_INTF_CONFIG_A;
		dev->extra.i3c_extra.ibi_en_mask = 0x0;
		dev->extra.i3c_extra.ibi_sts = 0x1;
		/* Attach initialized i3c device to ad405x_dev.comm_desc */
		ret = no_os_i3c_init(&dev->com_desc.i3c_desc,
				     init_param.comm_init.i3c_init);
	}
	if (ret)
		goto error_dev;

	dev->dev_type = init_param.dev_type;

	ret = ad405x_soft_reset(dev);
	if (ret)
		goto error_com;

	ret = ad405x_clear_reset_flags(dev);
	if (ret)
		goto error_com;

	/* Check if product identification matches */
	ret = ad405x_read(dev, AD405X_REG_PROD_ID_1, data, 2);
	if (ret)
		goto error_com;

	if (data[0] != AD405X_PROD_ID_1_RESET ||
	    data[1] != ad405x_part_id[dev->dev_type]) {
		ret = -EFAULT;
		goto error_com;
	}

	/* Check if vendor identification matches */
	ret = ad405x_read(dev, AD405X_REG_VENDOR_H, data, 2);
	if (ret)
		goto error_com;

	/* MIPI's ADI and ADI's nSPI vendor IDs are different */
	if (dev->comm_type == AD405X_SPI_COMM) {
		vendor_val[0] = AD405X_VENDOR_SPI_H_RESET;
		vendor_val[1] = AD405X_VENDOR_SPI_L_RESET;
	} else {
		vendor_val[0] = AD405X_VENDOR_I3C_H_RESET;
		vendor_val[1] = AD405X_VENDOR_I3C_L_RESET;
	}

	if (data[0] != vendor_val[0] ||
	    data[1] != vendor_val[1]) {
		ret = -EFAULT;
		goto error_com;
	}

	*device = dev;

	return ret;
error_com:
	if (dev->comm_type == AD405X_SPI_COMM)
		no_os_spi_remove(dev->com_desc.spi_desc);
	else
		no_os_i3c_remove(dev->com_desc.i3c_desc);
error_dev:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Remove device and free the resources allocated by ad405x_init().
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_remove(struct ad405x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_remove(dev->gpio_gpio0);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_gpio1);
	if (ret)
		return ret;


	if (dev->comm_type == AD405X_SPI_COMM) {
		ret = no_os_gpio_remove(dev->extra.spi_extra.gpio_cnv);
		if (ret)
			return ret;
		ret = no_os_spi_remove(dev->com_desc.spi_desc);
	} else
		ret = no_os_i3c_remove(dev->com_desc.i3c_desc);

	no_os_free(dev);

	return ret;
}

/**
 * @brief Soft reset of the device.
 * For I3C, the I3C peripheral is not reset, keeping its dynamic address and any
 * other I3C property.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_soft_reset(struct ad405x_dev *dev)
{
	int ret;
	uint8_t data;
	uint8_t flags;
	uint16_t nb_of_retries = 0xFFFF;

	if (dev->comm_type == AD405X_SPI_COMM) {
		/* Reset device using RESET pattern */
		ret = ad405x_reset_pattern_command(dev);
		if (ret)
			goto error_soft_comm;

		if (dev->gpio_gpio1) {
			/* Wait device ready assertion */
			data = 0;
			while (!data) {
				ret = no_os_gpio_get_value(dev->gpio_gpio1, &data);
				if (ret)
					goto error_soft_comm;
			}
		} else {
			/* Wait at least Tswreset time - 500us */
			no_os_mdelay(5);
		}
	} else {
		/* Perform soft reset */
		data = AD405X_RESET_CODE;
		ret = ad405x_write(dev, AD405X_REG_INTF_CONFIG_A,
				   &data, 1);
		if (ret)
			return ret;

		/* Pool device ready and reset flags */
		ret = ad405x_get_sts_reg(dev, &flags, NULL);
		if (ret)
			goto error_soft_comm;
		while ((!(AD405X_STS_DEVICE_READY(flags)) | !AD405X_STS_DEVICE_RESET(flags))
		       && --nb_of_retries) {
			ret = ad405x_get_sts_reg(dev, &flags, NULL);
			if (ret)
				goto error_soft_comm;
		}
		if (!nb_of_retries)
			goto error_soft_dev;

	}

	ad405x_set_to_reset(dev);

	return 0;

error_soft_dev:
	return -EFAULT;
error_soft_comm:
	return ret;
}

/**
 * @brief Clear reset flags.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_clear_reset_flags(struct ad405x_dev *dev)
{
	int ret;
	uint8_t data, iflags, flags;

	data = AD405X_CLEAR_RESET_FLAG;
	ret = ad405x_write(dev, AD405X_REG_DEVICE_STATUS, &data, 1);
	if (ret)
		goto error_soft_comm;

	data = AD405X_CLEAR_NOT_RDY_FLAG;
	ret = ad405x_write(dev, AD405X_REG_INTF_STATUS, &data, 1);
	if (ret)
		goto error_soft_comm;

	ret = ad405x_get_sts_reg(dev, &flags, &iflags);
	if (ret)
		goto error_soft_comm;
	if (AD405X_STS_DEVICE_RESET(flags) | AD405X_ISTS_NOT_RDY_ERROR(iflags))
		goto error_soft_dev;

	return 0;

error_soft_dev:
	return -EFAULT;
error_soft_comm:
	return ret;
}

/**
 * @brief Read device registers.
 * @param dev - The device descriptor.
 * @param addr - Address of the base register.
 * @param data - The read data buffer
 * @param size - The number of bytes to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_read(struct ad405x_dev *dev, uint8_t addr,
		uint8_t *data, uint8_t size)
{
	int ret;
	uint8_t buff[AD405X_BUFF_SIZE];

	if (!dev)
		return -EINVAL;

	if (dev->comm_type == AD405X_SPI_COMM) {
		if (size >= AD405X_BUFF_SIZE)
			return -ENOMEM;
		buff[0] = AD405X_SPI_READ | addr;
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc, buff,
					       1 + size);
		for (uint8_t idx = 0; idx < size; idx++)
			data[idx] = buff[idx + 1];
	} else {
		if (dev->extra.i3c_extra.addr_ptr != addr) {
			ret = no_os_i3c_write(dev->com_desc.i3c_desc, &addr, 1);
			if (ret)
				return ret;
		}
		dev->extra.i3c_extra.addr_ptr = addr;
		ret = no_os_i3c_read(dev->com_desc.i3c_desc, data, size);
	}

	return ret;
}

/**
 * @brief Write registers.
 * @param dev - The device descriptor.
 * @param addr - Address of the base register.
 * @param data - The data which is going to be written.
 * @param size - The number of bytes to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_write(struct ad405x_dev *dev, uint8_t addr,
		 uint8_t *data, uint8_t size)
{
	int ret;
	uint8_t buff[AD405X_BUFF_SIZE];

	if (!dev)
		return -EINVAL;
	if (size >= AD405X_BUFF_SIZE)
		return -ENOMEM;

	buff[0] = addr;
	for (uint16_t idx = 0; idx < size; idx++)
		buff[1 + idx] = data[idx];

	if (dev->comm_type == AD405X_SPI_COMM) {
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc, buff,
					       size + 1);
	} else {
		dev->extra.i3c_extra.addr_ptr = addr;
		ret = no_os_i3c_write(dev->com_desc.i3c_desc, buff, size + 1);
	}

	return ret;
}


/**
 * @brief Get formatted ADC reading.
 * If gp0 is set as data ready, will be used to delay the acquisition until the
 * signal is deserted (SPI only).
 * @param dev - The device descriptor.
 * @param value - ADC value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_get_adc(struct ad405x_dev *dev, int32_t *value)
{
	uint32_t raw;
	uint8_t drdy = 1;
	int ret;

	if (!dev)
		return -EINVAL;

	if (dev->comm_type == AD405X_SPI_COMM) {
		ret = ad405x_trigger_adc_conv(dev);
		if (ret)
			return ret;

		if (dev->gpio_gpio0 && dev->gp0_mode == AD405X_GP_MODE_DRDY) {
			while (drdy) {
				ret = no_os_gpio_get_value(dev->gpio_gpio0,
							   &drdy);
				if (ret)
					return ret;
			}
		}
	}

	ret = ad405x_get_raw(dev, &raw);
	if (ret)
		return ret;

	*value = raw;

	return 0;
}

/**
 * @brief Get raw ADC reading.
 * @param dev - The device descriptor.
 * @param raw - Raw ADC data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_get_raw(struct ad405x_dev *dev, uint32_t *raw)
{
	uint8_t array_raw[4] = {0};
	int ret;
	uint8_t addr;
	uint8_t size;

	if (!dev)
		return -EINVAL;

	if (dev->comm_type == AD405X_SPI_COMM) {
		switch (dev->operation_mode) {
		case AD405X_BURST_AVERAGING_MODE_OP:
		case AD405X_AVERAGING_MODE_OP:
			if (dev->dev_type == ID_AD4052)
				size = 3;
			else
				size = 2;
			break;
		default:
			size = 2;
			break;
		}
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc,
					       array_raw, size);
	} else {
		if (AD405X_IBI_STS_EN(dev->extra.i3c_extra.ibi_sts) &&
		    AD405X_IBI_EN_CONV_TRIGGER_GET(dev->extra.i3c_extra.ibi_en_mask))
			addr = AD405X_REG_CONV_TRIGGER(3);
		else
			addr = AD405X_REG_CONV_READ(3);
		size = 4;
		ret = ad405x_read(dev, addr, array_raw, size);
	}

	*raw = ad405x_adc_array_conv(dev, array_raw);

	return ret;
}

/**
 * @brief Read the interface status and status registers.
 * @param dev - The device descriptor.
 * @param status_flags - Status register value.
 * @param intf_status_flags - Interface register value (optional).
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_get_sts_reg(struct ad405x_dev *dev,
		       uint8_t *status_flags,
		       uint8_t *intf_status_flags)
{
	int ret;
	uint8_t reg_value;

	ret = ad405x_read(dev, AD405X_REG_DEVICE_STATUS,
			  &reg_value, 1);

	if (!ret)
		*status_flags = reg_value;

	if (ret | !intf_status_flags)
		return ret;

	ret = ad405x_read(dev, AD405X_REG_INTF_STATUS,
			  &reg_value, 1);

	if (!ret)
		*intf_status_flags = reg_value;

	return ret;
}

/**
 * @brief Write to clear the interface status and status registers.
 * @param dev - The device descriptor.
 * @param status_flags - Status register value (optional).
 * @param intf_status_flags - Interface register value (optional).
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_clear_sts_reg(struct ad405x_dev *dev,
			 uint8_t *status_flags,
			 uint8_t *intf_status_flags)
{
	int ret = 0;
	uint8_t reg_value;
	if (status_flags) {
		reg_value = AD405X_STATUS_FLAGS_MSK(*status_flags);
		ret = ad405x_write(dev, AD405X_REG_DEVICE_STATUS,
				   &reg_value, 1);

		if (ret)
			return ret;
	}

	if (intf_status_flags) {
		reg_value = AD405X_INTF_STATUS_FLAGS_MSK(*intf_status_flags);
		ret = ad405x_write(dev, AD405X_REG_INTF_STATUS,
				   &reg_value, 1);
		if (ret)
			return ret;
	}

	return ret;
}


/**
 * @brief Test the scratch pad register.
 * A pseudo-random value to test against is defined at compile time.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_test_scratch_pad(struct ad405x_dev *dev)
{
	const uint8_t value = __TIME__[7] | 1;
	uint8_t data[1] = {value};
	const uint8_t addr = AD405X_REG_SCRATCH_PAD;
	int ret = 0;

	ret = ad405x_write(dev, addr, data, 1);
	if (ret)
		return ret;
	memset(data, 0, sizeof data);
	ret = ad405x_read(dev, addr, data, 1);
	if (ret)
		return ret;

	if (*data != value)
		ret = -EFAULT;

	return ret;
}

/**
 * @brief Enter ADC Mode.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_adc_mode(struct ad405x_dev *dev)
{
	return ad405x_set_adc_modes(dev, AD405X_ADC_MODE);
}

/**
 * @brief Enter Burst Averaging Mode.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_burst_averaging_mode(struct ad405x_dev *dev)
{
	return ad405x_set_adc_modes(dev, AD405X_BURST_AVERAGING_MODE);
}

/**
 * @brief Enter Persistent Auto Mode.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_persistent_auto_mode(struct ad405x_dev *dev)
{
	return ad405x_set_adc_modes(dev, AD405X_PERSISTENT_AUTO_MODE);
}

/**
 * @brief Enter Non-Persistent Auto Mode.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_nonpersistent_auto_mode(struct ad405x_dev *dev)
{
	return ad405x_set_adc_modes(dev, AD405X_NONPERSISTENT_AUTO_MODE);
}

/**
 * @brief Set operation mode.
 * @param dev - The device descriptor.
 * @param mode - Operation mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_operation_mode(struct ad405x_dev *dev,
			      enum ad405x_operation_mode mode)
{
	int ret;

	if (!dev)
		return -EINVAL;
	if (dev->operation_mode == mode)
		return 0;

	if (dev->comm_type == AD405X_SPI_COMM &&
	    dev->operation_mode != AD405X_CONFIG_MODE_OP) {
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
	case AD405X_AVERAGING_MODE_OP:
		return ad405x_set_averaging_mode(dev);
	case AD405X_PERSISTENT_AUTO_MODE_OP:
		return ad405x_set_persistent_auto_mode(dev);
	case AD405X_NONPERSISTENT_AUTO_MODE_OP:
		return ad405x_set_nonpersistent_auto_mode(dev);
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Select sample rate for Burst and Autonomous Modes.
 * @param dev - The device descriptor.
 * @param rate - The sample rate.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_sample_rate(struct ad405x_dev *dev,
			   enum ad405x_sample_rate rate)
{
	int ret;

	ret = ad405x_update_bits(dev, AD405X_REG_TIMER_CONFIG,
				 AD405X_FS_BURST_AUTO_MSK,
				 no_os_field_prep(AD405X_FS_BURST_AUTO_MSK, rate));
	if (!ret)
		dev->rate = rate;

	return ret;
}

/**
 * @brief Get sample rate for Burst and Autonomous Modes.
 * @param dev - The device descriptor.
 * @return Sample rate value or -EINVAL if dev is invalid.
 */
enum ad405x_sample_rate ad405x_get_sample_rate(struct ad405x_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->rate;
}

/**
 * @brief Set averaging filter window length.
 * @param dev - The device descriptor.
 * @param length - Averaging filter length value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_avg_filter_length(struct ad405x_dev *dev,
				 enum ad405x_avg_filter_l length)
{
	int ret;

	if (!dev)
		return -EINVAL;

	switch (dev->dev_type) {
	case ID_AD4052:
	case ID_AD4058:
	case ID_AD4062:
		if (length > AD405X_LENGTH_4096)
			return -EINVAL;
		break;
	default:
		if (length > AD405X_LENGTH_256)
			return -EINVAL;
	}

	ret = ad405x_update_bits(dev, AD405X_REG_AVG_CONFIG,
				 AD405X_AVG_WIN_LEN_MSK,
				 no_os_field_prep(AD405X_AVG_WIN_LEN_MSK, length));
	if (!ret)
		dev->filter_length = length;

	return ret;
}

/**
 * @brief Get averaging filter window length.
 * @param dev - The device descriptor.
 * @return Average filter value or -EINVAL if dev is invalid.
 */
enum ad405x_avg_filter_l ad405x_get_avg_filter_length(struct ad405x_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->filter_length;
}

/**
 * @brief Set DEV_EN signal polarity.
 * @param dev - The device descriptor.
 * @param polarity - DEV_EN signal polarity.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_dev_en_polarity(struct ad405x_dev *dev,
			       enum ad405x_dev_en_polarity polarity)
{
	int ret;

	ret = ad405x_update_bits(dev, AD405X_REG_GPIO_CONFIG,
				 AD405X_DEV_EN_POL_MSK,
				 no_os_field_prep(AD405X_DEV_EN_POL_MSK, polarity));
	if (!ret)
		dev->polarity = polarity;

	return ret;
}

/**
 * @brief Get DEV_EN signal polarity.
 * @param dev - The device descriptor.
 * @return DEV_EN power-on timer setting or -EINVAL if dev is invalid.
 */
enum ad405x_dev_en_polarity ad405x_get_dev_en_polarity(struct ad405x_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->polarity;
}


/**
 * @brief Set DEV_EN TIMER_PWR_ON.
 * @param dev - The device descriptor.
 * @param time - DEV_EN power-on timer setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_dev_en_timer_pwr_on(struct ad405x_dev *dev,
				   enum ad405x_dev_en_timer_pwr_on time)
{
	int ret;

	ret = ad405x_update_bits(dev, AD405X_REG_TIMER_CONFIG,
				 AD405X_TIMER_PWR_ON_MSK,
				 no_os_field_prep(AD405X_TIMER_PWR_ON_MSK, time));
	if (!ret)
		dev->timer_pwr_on = time;

	return ret;
}

/**
 * @brief Get DEV_EN TIMER_PWR_ON.
 * @param dev - The device descriptor.
 * @return DEV_EN TIMER_PWR_ON setting or -EINVAL if dev is invalid.
 */
enum ad405x_dev_en_timer_pwr_on ad405x_get_dev_en_timer_pwr_on(
	struct ad405x_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->timer_pwr_on;
}

/**
 * @brief Enable INVERT_ON_CHOP.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_enable_invert_on_chop(struct ad405x_dev *dev)
{
	int ret;

	ret = ad405x_update_bits(dev, AD405X_REG_GPIO_CONFIG,
				 AD405X_INVERT_ON_CHOP_MSK,
				 no_os_field_prep(AD405X_INVERT_ON_CHOP_MSK,
						 AD405X_INVERT_ON_CHOP_ENABLED));
	if (!ret)
		dev->invert_on_chop_status = AD405X_INVERT_ON_CHOP_ENABLED;

	return ret;
}

/**
 * @brief Disable INVERT_ON_CHOP
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_disable_invert_on_chop(struct ad405x_dev *dev)
{
	int ret;

	ret = ad405x_update_bits(dev, AD405X_REG_GPIO_CONFIG,
				 AD405X_INVERT_ON_CHOP_MSK,
				 no_os_field_prep(AD405X_INVERT_ON_CHOP_MSK,
						 AD405X_INVERT_ON_CHOP_DISABLED));
	if (!ret)
		dev->invert_on_chop_status = AD405X_INVERT_ON_CHOP_DISABLED;

	return ret;
}

/**
 * @brief Get INVERT_ON_CHOP setting.
 * @param dev - The device descriptor.
 * @return INVERT_ON_CHOP setting or -EINVAL if dev is invalid.
 */
enum ad405x_invert_on_chop ad405x_get_invert_on_chop_state(
	struct ad405x_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->invert_on_chop_status;
}

/**
 * @brief Set GP mode.
 * @param dev - The device descriptor.
 * @param gp - GP1/GP0 select.
 * @param mode - mode select.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_gp_mode(struct ad405x_dev *dev,
		       enum ad405x_gp_select gp, enum ad405x_gp_mode mode)
{
	int ret;
	unsigned char msk;

	switch (gp) {
	case AD405X_GP_0:
		msk = AD405X_GP0_MODE_MSK;
		break;
	case AD405X_GP_1:
		msk = AD405X_GP1_MODE_MSK;
		break;
	default:
		return -EINVAL;
	}

	if (gp == AD405X_GP_0 && mode == AD405X_GP_MODE_DEV_RDY)
		return -EINVAL;

	ret = ad405x_update_bits(dev, AD405X_REG_GPIO_CONFIG, msk,
				 no_os_field_prep(msk, mode));
	if (ret)
		return ret;

	switch (gp) {
	case AD405X_GP_0:
		dev->gp0_mode = mode;
		break;
	case AD405X_GP_1:
		dev->gp1_mode = mode;
		break;
	}

	return 0;
}

/**
 * @brief Get GP mode setting.
 * @param dev - The device descriptor.
 * @param gp - GP1/GP0 select.
 * @return GP mode or -EINVAL if dev or gp is invalid.
 */
enum ad405x_gp_mode ad405x_get_gp_mode(struct ad405x_dev *dev,
				       enum ad405x_gp_select gp)
{
	if (!dev)
		return -EINVAL;

	switch (gp) {
	case AD405X_GP_0:
		return dev->gp0_mode;
	case AD405X_GP_1:
		return dev->gp1_mode;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Set output data format.
 * @param dev - The device descriptor.
 * @param data_format - Format select.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_data_format(struct ad405x_dev *dev,
			   enum ad405x_out_data_format data_format)
{
	int ret;

	ret = ad405x_update_bits(dev,
				 AD405X_REG_ADC_MODES,
				 AD405X_ADC_DATA_FORMAT_MSK,
				 no_os_field_prep(AD405X_ADC_DATA_FORMAT_MSK,
						 data_format));
	if (!ret)
		dev->data_format = data_format;

	return ret;
}

/**
 * @brief Set ADC output data format as straight binary.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_data_format_straight_binary(struct ad405x_dev *dev)
{
	return ad405x_set_data_format(dev, AD405X_STRAIGHT_BINARY);
}

/**
 * @brief Set ADC output data format as two's complement.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_data_format_twos_complement(struct ad405x_dev *dev)
{
	return ad405x_set_data_format(dev, AD405X_TWOS_COMPLEMENT);
}

/**
 * @brief Get output data format setting.
 * @param dev - The device descriptor.
 * @return DATA_FORMAT setting or -EINVAL if dev is invalid.
 */
enum ad405x_out_data_format ad405x_get_data_format(
	struct ad405x_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->data_format;
}

/**
 * @brief Set threshold configuration.
 * A simple memory swap puts the ad405x_thresh strict into format to write the 6
 * registers of interest.
 * @param dev - The device descriptor.
 * @param thresh - The threshold configuration structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_threshold(struct ad405x_dev *dev,
			 struct ad405x_thresh *thresh)
{
	int ret = 0;

	/* Check values */
	ret |= ad405x_fits_nbits(thresh->max_thresh_hyst, false, 6);
	ret |= ad405x_fits_nbits(thresh->min_thresh_hyst, false, 6);
	ret |= ad405x_fits_nbits(thresh->max_thresh_val, dev->data_format, 11);
	ret |= ad405x_fits_nbits(thresh->min_thresh_val, dev->data_format, 11);
	if (ret)
		return -EINVAL;

	no_os_memswap64(thresh, 6, 2);
	ret = ad405x_write(dev, AD405X_REG_MIN_HYST_REG,
			   (uint8_t *)thresh, 6);
	no_os_memswap64(thresh, 6, 2);

	return ret;
}

/**
 * @brief Get threshold configuration.
 * @param dev - The device descriptor.
 * @param thresh - The threshold configuration structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_get_threshold(struct ad405x_dev *dev,
			 struct ad405x_thresh *thresh)
{
	int ret;

	ret = ad405x_read(dev, AD405X_REG_MIN_HYST_REG,
			  (uint8_t *)thresh, 6);
	no_os_memswap64(thresh, 6, 2);

	if (!dev->data_format)
		return ret;

	thresh->max_thresh_val = no_os_sign_extend16(thresh->max_thresh_val, 11);
	thresh->min_thresh_val = no_os_sign_extend16(thresh->min_thresh_val, 11);

	return ret;
}

/**
 * @brief Send EXIT command (SPI only).
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_exit_command(struct ad405x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Gracefully exit if not needed*/
	if (dev->comm_type != AD405X_SPI_COMM)
		return 0;

	ret = ad405x_write(dev, 0xA8, NULL, 0);

	if (ret)
		return ret;

	dev->operation_mode = AD405X_CONFIG_MODE_OP;

	return ret;
}

/**
 * @brief Trigger an ADC conversion (SPI only).
 * Toggle the CNV pin to start the conversion.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_trigger_adc_conv(struct ad405x_dev *dev)
{
	int ret;

	if (!dev || dev->comm_type != AD405X_SPI_COMM)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->extra.spi_extra.gpio_cnv,
				   NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	/* CNV High Time 10 ns. */
	no_os_udelay(1);

	ret = no_os_gpio_set_value(dev->extra.spi_extra.gpio_cnv,
				   NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	/* Conversion Time (CNV Rising Edge to Data Ready) 250ns. */
	no_os_udelay(1);

	return ret;
}

/**
 * @brief Enter Averaging Mode (SPI only).
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_averaging_mode(struct ad405x_dev *dev)
{
	if (!dev || dev->comm_type == AD405X_I3C_COMM)
		return -EINVAL;

	return ad405x_set_adc_modes(dev, AD405X_AVERAGING_MODE);
}

/**
 * @brief Enter Configuration Mode (SPI only).
 * @param dev - The device descriptor.
 * @param mode - Store the current op mode, to resume later (optional).
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_config_mode(struct ad405x_dev *dev,
			   enum ad405x_operation_mode *mode)
{
	if (!dev)
		return -EINVAL;

	if (mode)
		*mode = dev->operation_mode;

	/* Send EXIT command . */
	return ad405x_exit_command(dev);
}

/**
 * @brief Test the GETBCR, GETDCR, GETPID, AND GETSTATUS CCCs (I3C only).
 * Compare received BCR and DCR against expected values.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_test_common_ccc(struct ad405x_dev *dev)
{

	struct no_os_i3c_desc *i3c_desc;
	uint8_t ccc_data[9];
	uint8_t *ptr_ccc_data;
	int cccs[] = {NO_OS_I3C_CCC_GETBCR, NO_OS_I3C_CCC_GETDCR,
		      NO_OS_I3C_CCC_GETPID, NO_OS_I3C_CCC_GETSTATUS
		     };
	int ret = 0;

	if (!dev || dev->comm_type != AD405X_I3C_COMM)
		return -EINVAL;

	i3c_desc = dev->com_desc.i3c_desc;
	ptr_ccc_data = ccc_data;
	for (uint8_t i = 0; i < sizeof(cccs) / sizeof(int); i++) {
		ret = no_os_i3c_send_direct_ccc(i3c_desc, cccs[i], ptr_ccc_data);
		if (ret)
			return ret;
		ptr_ccc_data += NO_OS_I3C_CCC_GET_LEN(cccs[i]);
	}

	ptr_ccc_data = ccc_data;
	if (*ptr_ccc_data++ != AD405X_I3C_BCR)
		return -EFAULT;

	if (*ptr_ccc_data++ != AD405X_I3C_DCR)
		return -EFAULT;

	return 0;
}

/**
 * @brief Pool IBI status and config registers (I3C only).
 * Values are updated on  the device descriptor.
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_pool_ibi_config(struct ad405x_dev *dev)
{
	int ret;
	uint8_t reg_value[2];
	struct ad405x_i3c_extra *i3c_extra;

	if (!dev || dev->comm_type != AD405X_I3C_COMM)
		return -EINVAL;

	i3c_extra = &dev->extra.i3c_extra;

	ret = ad405x_read(dev, AD405X_REG_IBI_STATUS,
			  reg_value, 1);
	if (ret)
		return ret;

	i3c_extra->ibi_sts = reg_value[0];

	ret = ad405x_read(dev, AD405X_REG_ADC_IBI_EN,
			  reg_value, 2);
	if (ret)
		return ret;

	i3c_extra->ibi_en_mask = no_os_get_unaligned_be16(reg_value);

	return ret;
}

/**
 * @brief Enable/disable IBI (I3C only).
 * @param dev - The device descriptor.
 * @param enable - Enable IBI.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_ibi_enable(struct ad405x_dev *dev,
			  bool enable)
{
	uint8_t reg_value;
	uint32_t ccc;
	int ret;

	if (!dev || dev->comm_type != AD405X_I3C_COMM)
		return -EINVAL;

	if (enable)
		ccc = NO_OS_I3C_CCC_ENEC_DIRECT;
	else
		ccc = NO_OS_I3C_CCC_DISEC_DIRECT;

	reg_value = NO_OS_I3C_CCC_EVENT_SIR;
	ret = no_os_i3c_send_direct_ccc(dev->com_desc.i3c_desc, ccc, &reg_value);
	if (ret)
		return ret;

	ret = ad405x_pool_ibi_config(dev);

	return ret;
}

/**
 * @brief Set IBI mask (I3C only).
 * @param dev - The device descriptor.
 * @param en_mask - Enable interface and ADC flags.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad405x_set_ibi_mask(struct ad405x_dev *dev,
			uint16_t en_mask)
{
	uint8_t reg_value[2];
	int ret;

	if (!dev || dev->comm_type != AD405X_I3C_COMM)
		return -EINVAL;

	no_os_put_unaligned_be16(en_mask, reg_value);
	ret = ad405x_write(dev, AD405X_REG_ADC_IBI_EN,
			   reg_value, 2);

	if (ret)
		return ret;

	ret = ad405x_pool_ibi_config(dev);

	return ret;
}

/**
 * @brief Send RESET pattern command (SPI only).
 * @param dev - The device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad405x_reset_pattern_command(struct ad405x_dev *dev)
{
	int ret;

	if (!dev || dev->comm_type != AD405X_SPI_COMM)
		return -EINVAL;

	ret = no_os_spi_write_and_read(dev->com_desc.spi_desc,
				       (uint8_t*)reset_pattern_buff, 18);

	ad405x_set_to_reset(dev);

	return ret;
}

/**
 * @brief Convert raw value value to int32_t.
 * @param dev - The device descriptor.
 * @param data - Raw data buffer.
 * @return Converted data.
 */
static int32_t ad405x_adc_array_conv(struct ad405x_dev *dev,
				     uint8_t *data)
{
	uint32_t raw_adc;

	if (dev->comm_type == AD405X_SPI_COMM) {
		switch (dev->operation_mode) {
		case AD405X_BURST_AVERAGING_MODE_OP:
		case AD405X_AVERAGING_MODE_OP:
			if (dev->dev_type == ID_AD4052) {
				raw_adc = no_os_get_unaligned_be24(data);
				if (dev->data_format == AD405X_TWOS_COMPLEMENT)
					raw_adc = no_os_sign_extend32(raw_adc, 23);
				break;
			}
			__attribute__((fallthrough));
		default:
			raw_adc = no_os_get_unaligned_be16(data);
			if (dev->data_format == AD405X_TWOS_COMPLEMENT)
				raw_adc = no_os_sign_extend32(raw_adc, 15);
			break;
		}
	} else {
		raw_adc = no_os_get_unaligned_be32(data);
	}

	return raw_adc;
}

/**
 * @brief Update specific register bits.
 * @param dev - The device descriptor.
 * @param addr - Address of the base register.
 * @param mask - Specific bits mask.
 * @param value - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad405x_update_bits(struct ad405x_dev *dev, uint8_t addr,
			      uint8_t mask,
			      uint8_t value)
{
	int ret;
	uint8_t data;

	ret = ad405x_read(dev, addr, &data, 1);
	if (ret)
		return ret;

	data &= ~mask;
	data |= value;

	return ad405x_write(dev, addr, &data, 1);
}

/**
 * @brief Set the device configuration to the expected reset values.
 * @param dev - The device descriptor.
 */
static void ad405x_set_to_reset(struct ad405x_dev *dev)
{
	/* Set device parameters to default settings. */
	dev->operation_mode = dev->comm_type == AD405X_SPI_COMM ?
			      AD405X_CONFIG_MODE_OP : AD405X_ADC_MODE_OP;
	dev->polarity = AD405X_DEV_EN_ACTIVE_HIGH;
	dev->timer_pwr_on = AD405X_DEV_EN_TIME_250_NS;
	dev->invert_on_chop_status = AD405X_INVERT_ON_CHOP_DISABLED;
	dev->gp0_mode = AD405X_GP_MODE_HIGH_Z;
	dev->gp1_mode = AD405X_GP_MODE_DEV_RDY;
	dev->data_format = AD405X_TWOS_COMPLEMENT;
	dev->filter_length = AD405X_LENGTH_2;
}

/**
 * @brief Set the ADC to an ADC mode.
 * @param dev - The device descriptor.
 * @param mode - Mode to enter.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad405x_set_adc_modes(struct ad405x_dev *dev, uint8_t mode)
{
	int ret;
	uint8_t data;

	if (dev->comm_type == AD405X_I3C_COMM) {
		data = dev->data_format << 7 | dev->operation_mode;
		if (mode == AD405X_NONPERSISTENT_AUTO_MODE) {
			data |= AD405X_ENTER_AUTO_MODE;
			ret = ad405x_write(dev, AD405X_REG_ADC_MODES, &data, 1);
			if (ret)
				return ret;
		}
		if (mode == AD405X_NONPERSISTENT_AUTO_MODE ||
		    mode == AD405X_PERSISTENT_AUTO_MODE) {
			ret = ad405x_write(dev, AD405X_REG_CONV_AUTO, NULL, 0);
			if (ret)
				return ret;
		}

		data &= ~AD405X_ADC_MODES_MSK;
		data |= mode;
		ret = ad405x_write(dev, AD405X_REG_ADC_MODES, &data, 1);
		if (ret)
			return ret;

		if (mode == AD405X_NONPERSISTENT_AUTO_MODE ||
		    mode == AD405X_PERSISTENT_AUTO_MODE) {
			ret = ad405x_write(dev, AD405X_REG_CONV_AUTO, NULL, 0);
			if (ret)
				return ret;
		}

	} else {
		/* Set ADC_MODES */
		ret = ad405x_update_bits(dev, AD405X_REG_ADC_MODES, AD405X_ADC_MODES_MSK, mode);
		if (ret)
			return ret;
		/* Enter ADC_MODE */
		ret = ad405x_update_bits(dev, AD405X_REG_MODE_SET, AD405X_ENTER_ADC_MODE_MSK,
					 AD405X_ENTER_ADC_MODE);
		if (ret)
			return ret;
	}

	/* Order in enum matches reg val */
	dev->operation_mode = mode;

	return 0;
}

/**
 * @brief Initialize GPIOs.
 * @param dev - The device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad405x_init_gpios(struct ad405x_dev *dev,
			     struct ad405x_init_param *init_param)
{
	int ret;

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

	/* Only SPI has CNV pin */
	if (dev->comm_type != AD405X_SPI_COMM)
		return ret;

	/* CNV */
	ret = no_os_gpio_get_optional(&dev->extra.spi_extra.gpio_cnv,
				      init_param->gpio_cnv);
	if (ret < 0)
		return ret;

	if (dev->extra.spi_extra.gpio_cnv) {
		ret = no_os_gpio_direction_output(dev->extra.spi_extra.gpio_cnv,
						  NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}

	return ret;
}

/**
 * @brief Check if number fits n-bits.
 * @param number - Number.
 * @param is_signed - Is the number signed.
 * @param bit - Bit position.
 * @return 0 if fits, 1 if does not.
 */
static bool ad405x_fits_nbits(uint16_t number, bool is_signed, uint8_t bit)
{
	if (is_signed && no_os_test_bit(15, (void*)&number))
		number = ~number;
	if (number > NO_OS_GENMASK(bit, 0))
		return true;
	return false;
}
