/***************************************************************************//**
 *   @file   iio_admt_evb_kit.c
 *   @brief  Implementation of IIO wrapper for the evaluation peripherals.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "iio.h"
#include "iio_admt_evb.h"

#define ADMT_EVB_BOOL_ATTR(_name, _priv) { \
	.name = _name,                \
	.priv = _priv,                \
	.show = admt_evb_bool_attr_show,   \
	.store = admt_evb_bool_attr_store, \
}

static int admt_evb_bool_attr_store(void *dev, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int admt_evb_bool_attr_show(void *dev, char *buf, uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static struct iio_attribute admt_evb_iio_attrs[] = {
	ADMT_EVB_BOOL_ATTR("pulse_shutdown", ADMT_EVB_SHDN),
	ADMT_EVB_BOOL_ATTR("power_enable", ADMT_EVB_V_EN),
	ADMT_EVB_BOOL_ATTR("coil_reset", ADMT_COIL_RS),
	END_ATTRIBUTES_ARRAY
};

static struct iio_device admt_evb_iio_dev = {
	.attributes = admt_evb_iio_attrs,
	.debug_reg_read = NULL,
	.debug_reg_write = NULL
};

/**
 * @brief Initializes the cal kit IIO driver
 * @param iio_dev - The iio device structure.
 * @param init_param - Parameters for the initialization of iio_dev
 *
 * @return 0 in case of success, errno errors otherwise
 */
int admt_evb_iio_init(struct admt_evb_iio_desc **iio_desc,
		     struct admt_evb_iio_init_param *init_param)
{
	int i, ret;
	struct admt_evb_iio_desc *descriptor;

	if (!init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_gpio_get(&descriptor->gpio_v_en_desc,
			     &init_param->gpio_v_en_ip);
	if (ret)
		goto v_en_err;

	ret = no_os_gpio_direction_output(descriptor->gpio_v_en_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto v_en_out_err;

	ret = no_os_gpio_get(&descriptor->gpio_shdn_n_desc,
			     &init_param->gpio_shdn_n_ip);
	if (ret)
		goto err;
	
	ret = no_os_gpio_direction_output(descriptor->gpio_shdn_n_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto err;
	no_os_mdelay(10);
	ret = no_os_gpio_get(&descriptor->gpio_rstb_desc,
			     &init_param->gpio_rstb_ip);
	if (ret)
		goto v_en_err;

	ret = no_os_gpio_direction_output(descriptor->gpio_rstb_desc, NO_OS_GPIO_LOW);
	if (ret)
		goto v_en_out_err;
	
	no_os_mdelay(10);
	ret = no_os_gpio_set_value(descriptor->gpio_rstb_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&descriptor->gpio_coil_rs_desc,
			     &init_param->gpio_coil_rs_ip);
	if (ret)
		goto v_en_err;

	ret = no_os_gpio_direction_output(descriptor->gpio_coil_rs_desc, NO_OS_GPIO_LOW);
	if (ret)
		goto v_en_out_err;


	descriptor->iio_dev = &admt_evb_iio_dev;

	*iio_desc = descriptor;

	return 0;

v_en_out_err:
	

v_en_err:
	no_os_gpio_remove(descriptor->gpio_shdn_n_desc);
err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - The iio device structure.
 *
 * @return 0 in case of success, errno errors otherwise
 */
int admt_evb_iio_remove(struct admt_evb_iio_desc *desc)
{
	int ret;
	no_os_gpio_remove(desc->gpio_v_en_desc);
	no_os_gpio_remove(desc->gpio_shdn_n_desc);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Handles the read request for IIO attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt_evb_bool_attr_show(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt_evb_iio_desc *iio_admt_evb_desc;
	int ret;
	int32_t vals[2];


	if (!dev)
		return -ENODEV;

	iio_admt_evb_desc = (struct admt_evb_iio_dev *)dev;

	switch (priv) {
	case ADMT_EVB_SHDN:
		ret = no_os_gpio_get_value(iio_admt_evb_desc->gpio_shdn_n_desc,
					   &vals[0]);
		vals[0] = !vals[0];
		break;
	case ADMT_EVB_V_EN:
		ret = no_os_gpio_get_value(iio_admt_evb_desc->gpio_v_en_desc,
					   &vals[0]);
		break;
	case ADMT_COIL_RS:
		ret = no_os_gpio_get_value(iio_admt_evb_desc->gpio_coil_rs_desc,
					   &vals[0]);
		break;
	default:
		return -ENOTSUP;
	}

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);
}

/**
 * @brief Handles the write request for IIO attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt_evb_bool_attr_store(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt_evb_iio_desc *iio_admt_evb_desc = (struct admt_evb_iio_desc *)dev;
	int32_t val;

	int ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret)
		return ret;

	if (val < 0 || val > 1)
		return -EINVAL;

	switch (priv) {
	case ADMT_EVB_SHDN:
		val = !val;
		ret = no_os_gpio_set_value(iio_admt_evb_desc->gpio_shdn_n_desc,
					   val);
		break;
	case ADMT_EVB_V_EN:
		ret = no_os_gpio_set_value(iio_admt_evb_desc->gpio_v_en_desc,
					   val);
		break;
	case ADMT_COIL_RS:
		ret = no_os_gpio_set_value(iio_admt_evb_desc->gpio_coil_rs_desc,
					   val);
		break;
	default:
		return -ENOTSUP;
	}

	if (ret)
		return ret;

	return len;
}
