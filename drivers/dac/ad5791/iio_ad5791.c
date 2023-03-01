/***************************************************************************//**
 *   @file   iio_ad5791.c
 *   @brief  Implementation of AD5791 IIO Driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
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
 *
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iio_ad5791.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/**
 * @brief IIO get method to the 'scale' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad5791_iio_get_scale(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ad5791_iio_desc *iio_drv = (struct ad5791_iio_desc *)device;
	uint32_t int_part, fract_part;
	uint64_t vref64;
	uint8_t dac_bits = 20;

	vref64 = (uint64_t)iio_drv->vref_mv * 1000000000ll;
	vref64 /= 1 << dac_bits;
	int_part = vref64 / 1000000000ll;
	fract_part = vref64 - int_part * 1000000000ll;

	return snprintf(buf, len, "%"PRId32".%.9"PRId32"", int_part, fract_part);
}

/**
 * @brief IIO get method to the 'offset' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad5791_iio_get_offset(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad5791_iio_desc *iio_drv = (struct ad5791_iio_desc *)device;
	uint64_t val64;
	uint8_t const dac_bits = 20;

	val64 = iio_drv->vref_neg_mv << (dac_bits - 1);
	val64 /= iio_drv->vref_mv;
	val64 = -val64;

	return snprintf(buf, len, "%"PRIi32"", (int32_t)val64);
}

/**
 * @brief IIO get method to the 'raw' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad5791_iio_get_raw(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad5791_iio_desc *iio_drv = (struct ad5791_iio_desc *)device;
	struct ad5791_dev *dev = (struct ad5791_dev *)iio_drv->ad5791_handle;
	int32_t ret;
	uint32_t value;

	ret = ad5791_get_register_value(dev,
					AD5791_REG_DAC,
					&value);
	if (ret != 0)
		return ret;
	value &= 0xFFFFF;

	return snprintf(buf, len, "%"PRIX32"", value);
}

/**
 * @brief IIO set method to the 'raw' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad5791_iio_set_raw(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad5791_iio_desc *iio_drv = (struct ad5791_iio_desc *)device;
	struct ad5791_dev *dev = (struct ad5791_dev *)iio_drv->ad5791_handle;
	int32_t ret;
	uint32_t value;

	sscanf(buf, "%"PRIX32"", &value);

	ret = ad5791_set_dac_value(dev, value);
	if (ret != 0)
		return ret;

	ret = ad5791_soft_instruction(dev, AD5791_SOFT_CTRL_LDAC);
	if (ret != 0)
		return ret;

	return len;
}

/**
 * @brief IIO get method to the 'powerdown' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad5791_iio_get_powerdown(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ad5791_iio_desc *iio_drv = (struct ad5791_iio_desc *)device;
	struct ad5791_dev *dev = (struct ad5791_dev *)iio_drv->ad5791_handle;
	int32_t ret;
	uint32_t value;
	bool pwrdwn;

	ret = ad5791_get_register_value(dev, AD5791_REG_CTRL, &value);
	if (ret != 0)
		return ret;

	if ((value & AD5791_CTRL_OPGND) || (value & AD5791_CTRL_DACTRI)) {
		pwrdwn = true;
	} else {
		pwrdwn = false;
	}

	return snprintf(buf, len, "%"PRId8"", pwrdwn);
}

/**
 * @brief IIO set method to the 'powerdown' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad5791_iio_set_powerdown(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ad5791_iio_desc *iio_drv = (struct ad5791_iio_desc *)device;
	struct ad5791_dev *dev = (struct ad5791_dev *)iio_drv->ad5791_handle;
	int32_t ret;
	uint32_t value, pwr_dwn_opt;

	sscanf(buf, "%"PRId32"", &value);

	if (iio_drv->curr_mode == AD5791_6kOHMS_TO_GND) {
		pwr_dwn_opt = AD5791_CTRL_OPGND;
	} else if (iio_drv->curr_mode == AD5791_THREE_STATE) {
		pwr_dwn_opt = AD5791_CTRL_DACTRI;
	} else {
		return -EINVAL;
	}

	ret = ad5791_get_register_value(dev, AD5791_REG_CTRL, &value);
	if (ret != 0)
		return ret;
	if (!!value)
		value |= pwr_dwn_opt;
	else
		value &= ~pwr_dwn_opt;
	ret = ad5791_set_register_value(dev, AD5791_REG_CTRL, value);
	if (ret != 0)
		return ret;

	return len;
}

static char const * const ad5791_iio_pwd_modes[] = {
	"6kohm_to_gnd",
	"three_state"
};

/**
 * @brief IIO get method to the 'powerdown_modes' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad5791_iio_get_pd_mode(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ad5791_iio_desc *iio_drv = (struct ad5791_iio_desc *)device;

	return snprintf(buf, len,
			"%s", ad5791_iio_pwd_modes[iio_drv->curr_mode]);
}

/**
 * @brief IIO set method to the 'powerdown_modes' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad5791_iio_set_pd_mode(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ad5791_iio_desc *iio_drv = (struct ad5791_iio_desc *)device;
	int8_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad5791_iio_pwd_modes); i++)
		if (!strncmp(buf, ad5791_iio_pwd_modes[i], len))
			break;

	iio_drv->curr_mode = i;

	return len;
}

/**
 * @brief IIO get method to the 'powerdown_modes_avail' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad5791_iio_get_pd_mode_avail(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	int8_t i;

	strcpy(buf, "");
	for (i = 0; i < NO_OS_ARRAY_SIZE(ad5791_iio_pwd_modes); i++) {
		strcat(buf, ad5791_iio_pwd_modes[i]);
		strcat(buf, " ");
	}

	return len;
}

static struct scan_type const ad5791_iio_scan_type = {
	.sign = 's',
	.realbits = 20,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = true
};

static struct iio_attribute const channel_attributes[] = {
	{
		.name = "raw",
		.priv = 0,
		.show = ad5791_iio_get_raw,
		.store = ad5791_iio_set_raw
	},
	{
		.name = "scale",
		.priv = 0,
		.show = ad5791_iio_get_scale,
		.store = NULL
	},
	{
		.name = "offset",
		.priv = 0,
		.show = ad5791_iio_get_offset,
		.store = NULL
	},
	{
		.name = "powerdown_mode",
		.priv = 0,
		.show = ad5791_iio_get_pd_mode,
		.store = ad5791_iio_set_pd_mode
	},
	{
		.name = "powerdown_mode_available",
		.priv = 0,
		.show = ad5791_iio_get_pd_mode_avail,
		.store = NULL
	},
	{
		.name = "powerdown",
		.priv = 0,
		.show = ad5791_iio_get_powerdown,
		.store = ad5791_iio_set_powerdown
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel const ad5791_channels[] = {
	{
		.name = "ad5791_out",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.channel2 = 0,
		.scan_type = (struct scan_type *)&ad5791_iio_scan_type,
		.attributes = (struct iio_attribute *)channel_attributes,
		.ch_out = 1,
		.indexed = 1,
		.diferential = false
	}
};

struct iio_device const iio_ad5791_device = {
	.num_ch = AD5791_CH_NO,
	.channels = (struct iio_channel *)ad5791_channels,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = NULL,
	.post_disable = NULL,
	.read_dev = NULL,
	.debug_reg_read = (int32_t (*)())ad5791_get_register_value,
	.debug_reg_write = (int32_t (*)())ad5791_set_register_value
};

/**
 * @brief Initialize the AD5791 IIO driver.
 * @param iio_dev - Pointer to the IIO driver handler.
 * @param init_param - Pointer to the initialization structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5791_iio_init(struct ad5791_iio_desc **iio_dev,
			struct ad5791_iio_init_param *init_param)
{
	int32_t ret;
	struct ad5791_iio_desc *desc;

	desc = (struct ad5791_iio_desc *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -1;

	desc->vref_mv = init_param->vref_mv;
	desc->vref_neg_mv = init_param->vref_neg_mv;
	desc->ad5791_iio_dev = (struct iio_device *)&iio_ad5791_device;
	desc->curr_mode = AD5791_THREE_STATE;

	ret = ad5791_init(&desc->ad5791_handle, *init_param->ad5791_initial);
	if (ret != 0)
		goto error_desc;

	*iio_dev = desc;

	return 0;
error_desc:
	no_os_free(desc);

	return ret;
}

/**
 * @brief Free memory allocated by ad5791_iio_init().
 * @param desc -Pointer to the driver handler.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5791_iio_remove(struct ad5791_iio_desc *desc)
{
	int32_t ret;

	ret = ad5791_remove(desc->ad5791_handle);
	if (ret != 0)
		return ret;

	no_os_free(desc);

	return 0;
}

