/***************************************************************************//**
 *   @file   ada4250.c
 *   @brief  Implementation of ADA4250 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include "ada4250.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * @brief Writes data to ada4250 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */

int32_t ada4250_write(struct ada4250_dev *dev, uint8_t reg_addr,
		      uint8_t data)
{
	uint8_t buff[ADA4250_BUFF_SIZE_BYTES];

	if(dev->device_id != ADA4250)
		return -ENODEV;

	buff[0] = ADA4250_SPI_WRITE_CMD | reg_addr;
	buff[1] = data;

	return no_os_spi_write_and_read(dev->spi_desc, buff, ADA4250_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ada4250 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */

int32_t ada4250_read(struct ada4250_dev *dev, uint8_t reg_addr,
		     uint8_t *data)
{
	int32_t ret;
	uint8_t buff[ADA4250_BUFF_SIZE_BYTES];

	if(dev->device_id != ADA4250)
		return -ENODEV;

	buff[0] = ADA4250_SPI_READ_CMD | reg_addr;
	buff[1] = ADA4250_SPI_DUMMY_DATA;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, ADA4250_BUFF_SIZE_BYTES);
	if(ret != 0)
		return ret;

	*data = buff[1];

	return ret;
}

/**
 * @brief Update ADA4250 register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Mask for specific register bits to be updated.
 * @param data - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t ada4250_update(struct ada4250_dev *dev, uint8_t reg_addr,
		       uint8_t mask, uint8_t data)
{
	uint8_t read_val;
	int32_t ret;

	if(dev->device_id != ADA4250)
		return -ENODEV;

	ret = ada4250_read(dev, reg_addr, &read_val);
	if (ret != 0)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	ret = ada4250_write(dev, reg_addr, read_val);

	return ret;
}

/**
 * @brief Update ADA4250 device descriptor.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t ada4250_update_desc(struct ada4250_dev *dev)
{
	uint8_t reg_data;
	int32_t ret;

	ret = ada4250_read(dev, ADA4250_REG_GAIN_MUX, &reg_data);
	if (ret != 0)
		return ret;
	dev->gain = reg_data & ADA4250_GAIN_MUX_MSK;

	ret = ada4250_read(dev, ADA4250_REG_REFBUF_EN, &reg_data);
	if (ret != 0)
		return ret;
	dev->refbuf_en = reg_data & ADA4250_REFBUF_MSK;

	ret = ada4250_read(dev, ADA4250_REG_SNSR_CAL_CNFG, &reg_data);
	if (ret != 0)
		return ret;
	dev->bias = (reg_data & ADA4250_BIAS_SET_MSK) >> 2;
	dev->offset_range = reg_data & ADA4250_RANGE_SET_MSK;

	return ret;
}

/**
 * @brief Software reset.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t ada4250_soft_reset(struct ada4250_dev *dev)
{
	int32_t ret;
	uint16_t timeout = 0xFFFF;
	uint8_t data;

	if(dev->device_id != ADA4250)
		return -ENODEV;

	ret = ada4250_update(dev, ADA4250_REG_RESET, ADA4250_RESET_MSK,
			     ADA4250_RESET(ADA4250_RESET_ENABLE));

	while(timeout--) {
		ret = ada4250_read(dev, ADA4250_REG_RESET, &data);
		if (ret != 0)
			return ret;

		if (!(data & ADA4250_RESET(ADA4250_RESET_ENABLE))) {
			ret = ada4250_update_desc(dev);
			if (ret != 0)
				return ret;
			else
				break;
		}
	}

	if (timeout < 1)
		ret = -ETIME;

	return ret;
}

/**
 * @brief Enable/Disable Reference Buffer.
 * @param dev - The device structure.
 * @param refbuf - REFBUF enable/disable.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t ada4250_en_refbuf(struct ada4250_dev *dev, bool refbuf)
{
	int32_t ret;

	switch(dev->device_id) {
	case ADA4250:
		ret = ada4250_update(dev, ADA4250_REG_REFBUF_EN, ADA4250_REFBUF_MSK,
				     ADA4250_REFBUF(refbuf));
		break;
	case ADA4230:
		ret = no_os_gpio_set_value(dev->gpio_bufen, dev->refbuf_en);
		break;
	default:
		return -ENODEV;
	}

	if (ret < 0)
		return ret;

	dev->refbuf_en = refbuf;
	return 0;
}

/**
 * @brief Set current bias for ADA4250.
 * @param dev - The device structure.
 * @param bias - Current bias option.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t ada4250_set_bias(struct ada4250_dev *dev, enum ada4250_bias bias)
{
	int32_t ret;

	if(dev->device_id != ADA4250)
		return -ENODEV;

	ret = ada4250_update(dev, ADA4250_REG_SNSR_CAL_CNFG, ADA4250_BIAS_SET_MSK,
			     ADA4250_BIAS_SET(bias));
	if (ret < 0)
		return ret;

	dev->bias = bias;
	return 0;
}

/**
 * @brief Set offset trim range for ADA4250.
 * @param dev - The device structure.
 * @param range - Offset range option.
 * @return Returns 0 in case of success or negative error code.
 */
static int32_t ada4250_set_range(struct ada4250_dev *dev,
				 enum ada4250_offset_range range)
{
	int32_t ret;

	if(dev->device_id != ADA4250)
		return -ENODEV;

	ret = ada4250_update(dev, ADA4250_REG_SNSR_CAL_CNFG, ADA4250_RANGE_SET_MSK,
			     ADA4250_RANGE_SET(range));
	if (ret < 0)
		return ret;

	dev->offset_range = range;
	return 0;
}

/**
 * @brief Set gain for ADA4250.
 * @param dev - The device structure.
 * @param gain - Gain Value.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t ada4250_set_gain(struct ada4250_dev *dev, enum ada4250_gain gain)
{
	int32_t ret;

	if(dev->device_id == ADA4250) {
		ret = ada4250_update(dev, ADA4250_REG_GAIN_MUX, ADA4250_GAIN_MUX_MSK,
				     ADA4250_GAIN_MUX(gain));
	} else {
		ret = no_os_gpio_set_value(dev->gpio_g2, ((dev->gain >> 2) & 0x1));
		if (ret != 0)
			return ret;

		ret = no_os_gpio_set_value(dev->gpio_g1, ((dev->gain >> 1) & 0x1));
		if (ret != 0)
			return ret;

		ret = no_os_gpio_set_value(dev->gpio_g0, (dev->gain & 0x1));
	}

	if (ret < 0)
		return ret;

	dev->gain = gain;
	return 0;
}

/**
 * @brief Set offset value for ADA4250.
 * @param dev - The device structure.
 * @param offset - Offset Value in nV.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t ada4250_set_offset(struct ada4250_dev *dev, int64_t offset)
{
	int32_t ret;
	uint32_t i;
	uint8_t offset_raw;
	enum ada4250_offset_range range;
	int64_t x[8], vlsb, max_vos, min_vos;

	if(dev->device_id != ADA4250)
		return -ENODEV;

	if (dev->bias == ADA4250_BIAS_DISABLE)
		return -EINVAL;

	x[0] = (dev->bias == ADA4250_BIAS_AVDD) ? dev->avdd_v : 5000;

	x[1] = 126 * (x[0] - 1000);
	x[2] = x[1] * 1000 / 1333;
	x[3] = x[1] * 1000 / 2301;
	x[4] = x[1] * 1000 / 4283;
	x[5] = x[1] * 1000 / 8289;
	x[6] = x[1] * 1000 / 16311;
	x[7] = x[1] * 1000 / 31599;

	if (dev->gain != ADA4250_GAIN_1) {
		for (i = ADA4250_RANGE1; i <= ADA4250_RANGE4; i++) {
			max_vos = x[dev->gain] *  127 * ((1 << (i + 1)) - 1);
			min_vos = (-1) * max_vos;
			if(offset > min_vos && offset < max_vos) {
				range = i;
				vlsb = x[dev->gain] * ((1 << (i + 1)) - 1);
				break;
			}
		}
	} else
		return -EINVAL;

	offset_raw = abs(offset) / vlsb;

	ret = ada4250_set_range(dev, range);
	if (ret != 0)
		return ret;

	if (offset < 0) {
		dev->offset_nv = (-1) * offset_raw * vlsb;
		return ada4250_write(dev, ADA4250_REG_SNSR_CAL_VAL, offset_raw);
	} else {
		dev->offset_nv = offset_raw * vlsb;
		return ada4250_write(dev, ADA4250_REG_SNSR_CAL_VAL, (1 << 8 | offset_raw));
	}
}

/**
 * @brief Set the bandwidth value for ADA4250.
 * @param dev - The device structure.
 * @param bw - Bandwidth value.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t ada4250_set_bandwidth(struct ada4250_dev *dev,
			      enum ada4250_bandwidth bw)
{
	int32_t ret;
	uint8_t val;

	if (!dev)
		return -ENOMEM;

	switch (bw) {
	case ADA4250_BANDWIDTH_LOW:
		val = NO_OS_GPIO_HIGH;
		break;
	case ADA4250_BANDWIDTH_HIGH:
		val = NO_OS_GPIO_LOW;
		break;
	default:
		return -EINVAL;
	}

	ret = no_os_gpio_set_value(dev->gpio_bw, val);
	if (ret < 0)
		return ret;

	dev->bandwidth = bw;
	return 0;
}

/**
 * @brief Set the configurations for ADA4250.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
static int32_t ada4250_set_config(struct ada4250_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -ENOMEM;

	ret = ada4250_set_bandwidth(dev, dev->bandwidth);
	if (ret != 0)
		return ret;

	ret = ada4250_set_gain(dev, dev->gain);
	if (ret != 0)
		return ret;

	ret = ada4250_en_refbuf(dev, dev->refbuf_en);
	if (ret != 0)
		return ret;

	if (dev->device_id == ADA4250) {
		ret = ada4250_set_bias(dev, dev->bias);
		if (ret != 0)
			return ret;

		if (dev->bias != ADA4250_BIAS_DISABLE) {
			ret = ada4250_set_offset(dev, dev->offset_nv);
			if (ret != 0)
				return ret;
		}
	}

	return 0;
}

/**
 * @brief Set the ADA4250 into sleep or shutdown mode.
 * @param dev - The device structure.
 * @param pwrmode - Power mode option.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t ada4250_set_slp_shtdwn_mode(struct ada4250_dev *dev,
				    enum ada4250_power_mode pwrmode)
{
	int32_t ret;

	if (!dev)
		return -ENOMEM;

	switch (pwrmode) {
	case ADA4250_POWER_SLEEP:
		ret = no_os_gpio_set_value(dev->gpio_slp, NO_OS_GPIO_LOW);
		break;
	case ADA4250_POWER_SHUTDOWN:
		ret = no_os_gpio_set_value(dev->gpio_shtdwn, NO_OS_GPIO_LOW);
		break;
	default:
		return -EINVAL;
	}

	if (ret < 0)
		return ret;

	dev->power_mode = pwrmode;
	return 0;
}

/**
 * @brief Sets the ADA4250 into normal mode and reconfigures it according
 *        to the user input.
 * @param dev - The device structure.
 * @param reconfig - Reconfiguration enable/disable.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t ada4250_set_normal_mode(struct ada4250_dev *dev, bool reconfig)
{
	int32_t ret;

	if (!dev)
		return -ENOMEM;

	switch (dev->power_mode) {
	case ADA4250_POWER_SHUTDOWN:
		ret = no_os_gpio_set_value(dev->gpio_shtdwn, NO_OS_GPIO_HIGH);
		break;
	case ADA4250_POWER_SLEEP:
		ret = no_os_gpio_set_value(dev->gpio_slp, NO_OS_GPIO_HIGH);
		break;
	default:
		return -EINVAL;
	}

	if (ret != 0)
		return ret;

	/* Wait for 200/400 us for the device to wake up from sleep/shutdown mode */
	no_os_udelay(dev->power_mode == ADA4250_POWER_SLEEP ? 200 : 400);

	if (dev->power_mode == ADA4250_POWER_SHUTDOWN) {
		if (reconfig) {
			/* Reconfiguring to state prior to shutdown */
			ret = ada4250_set_config(dev);
			if (ret != 0)
				return ret;
		} else { /* Update driver configurations */
			ret = ada4250_update_desc(dev);
			if (ret != 0)
				return ret;
		}
	}

	dev->power_mode = ADA4250_POWER_NORMAL;
	return 0;
}

/**
 * @brief Initialize the ADA4250 device.
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t ada4250_init(struct ada4250_dev **device,
		     struct ada4250_init_param *init_param)
{
	int32_t ret;
	uint8_t data;
	uint16_t chip_id;
	struct ada4250_dev *dev;

	dev = (struct ada4250_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->refbuf_en = init_param->refbuf_en;
	dev->gain = init_param->gain;
	dev->bias = init_param->bias;
	dev->offset_nv = init_param->offset_nv;
	dev->avdd_v = init_param->avdd_v;
	dev->device_id = init_param->device_id;
	dev->bandwidth = init_param->bandwidth;

	/* Initialize gpio sleep/shutdown and pull it to high */
	ret = no_os_gpio_get_optional(&dev->gpio_shtdwn, init_param->gpio_shtdwn);
	if (ret != 0)
		goto error_dev;

	ret = no_os_gpio_direction_output(dev->gpio_shtdwn, NO_OS_GPIO_HIGH);
	if (ret != 0)
		goto error_shtdwn;

	ret = no_os_gpio_get_optional(&dev->gpio_slp, init_param->gpio_slp);
	if (ret != 0)
		goto error_shtdwn;

	ret = no_os_gpio_direction_output(dev->gpio_slp, NO_OS_GPIO_HIGH);
	if (ret != 0)
		goto error_slp;

	/* Wait for the device to wake up*/
	no_os_mdelay(1);
	dev->power_mode = ADA4250_POWER_NORMAL;

	if(dev->device_id == ADA4250) {
		/* SPI */
		ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
		if (ret != 0)
			goto error_dev;

		ret = ada4250_read(dev, ADA4250_REG_CHIP_ID2, &data);
		if (ret != 0)
			goto error_spi;

		chip_id = data;

		ret = ada4250_read(dev, ADA4250_REG_CHIP_ID1, &data);
		if (ret != 0)
			goto error_spi;

		chip_id = (chip_id << 8 | data);

		if (chip_id != ADA4250_CHIP_ID) {
			ret = -EINVAL;
			goto error_spi;
		}

		ret = ada4250_soft_reset(dev);
		if(ret != 0)
			return -1;
	} else {
		ret = no_os_gpio_get(&dev->gpio_g2, init_param->gpio_g2_param);
		if (ret != 0)
			goto error_dev;

		ret = no_os_gpio_direction_output(dev->gpio_g2, NO_OS_GPIO_LOW);
		if (ret != 0)
			goto error_g2;

		ret = no_os_gpio_get(&dev->gpio_g1, init_param->gpio_g1_param);
		if (ret != 0)
			goto error_g2;

		ret = no_os_gpio_direction_output(dev->gpio_g1, NO_OS_GPIO_LOW);
		if (ret != 0)
			goto error_g1;

		ret = no_os_gpio_get(&dev->gpio_g0, init_param->gpio_g0_param);
		if (ret != 0)
			goto error_g1;

		ret = no_os_gpio_direction_output(dev->gpio_g0, NO_OS_GPIO_LOW);
		if (ret != 0)
			goto error_g0;

		ret = no_os_gpio_get(&dev->gpio_bufen, init_param->gpio_bufen_param);
		if (ret != 0)
			goto error_g0;
		ret = no_os_gpio_direction_output(dev->gpio_bufen, NO_OS_GPIO_LOW);
		if (ret != 0)
			goto error_bufen;
	}

	/* Initialize gpio bandwidth and pull it to high */
	ret = no_os_gpio_get_optional(&dev->gpio_bw, init_param->gpio_bw_param);
	if (ret != 0)
		return -1;

	ret = no_os_gpio_direction_output(dev->gpio_bw, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return -1;

	ret = ada4250_set_config(dev);
	if (ret != 0)
		return -1;

	*device = dev;

	return ret;

error_spi:
	no_os_spi_remove(dev->spi_desc);
	goto error_dev;
error_bufen:
	no_os_gpio_remove(dev->gpio_bufen);
error_g0:
	no_os_gpio_remove(dev->gpio_g0);
error_g1:
	no_os_gpio_remove(dev->gpio_g1);
error_g2:
	no_os_gpio_remove(dev->gpio_g2);
error_slp:
	no_os_gpio_remove(dev->gpio_slp);
error_shtdwn:
	no_os_gpio_remove(dev->gpio_shtdwn);
error_dev:
	no_os_free(dev);

	return ret;
}
/**
 * @brief Free resoulces allocated for ADA4250
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t ada4250_remove(struct ada4250_dev *dev)
{
	int32_t ret;

	ret = no_os_gpio_remove(dev->gpio_slp);
	if (ret != 0)
		return ret;
	ret = no_os_gpio_remove(dev->gpio_shtdwn);
	if (ret != 0)
		return ret;
	ret = no_os_gpio_remove(dev->gpio_bw);
	if (ret != 0)
		return ret;

	if (dev->device_id == ADA4250) {
		ret = no_os_spi_remove(dev->spi_desc);
		if (ret != 0)
			return ret;
	} else {
		ret = no_os_gpio_remove(dev->gpio_g2);
		if (ret != 0)
			return ret;
		ret = no_os_gpio_remove(dev->gpio_g1);
		if (ret != 0)
			return ret;
		ret = no_os_gpio_remove(dev->gpio_g0);
		if (ret != 0)
			return ret;
		ret = no_os_gpio_remove(dev->gpio_bufen);
		if (ret != 0)
			return ret;
	}

	no_os_free(dev);

	return ret;
}
