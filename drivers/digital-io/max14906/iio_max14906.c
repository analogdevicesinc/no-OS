/***************************************************************************//**
 *   @file   iio_max14906.c
 *   @brief  Source file of IIO MAX14906 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "max14906.h"
#include "iio_max14906.h"

#define MAX14906_CHANNEL(_addr)			\
        {					\
            	.ch_type = IIO_VOLTAGE,		\
        	.indexed = 1,			\
		.channel = _addr,		\
	    	.address = _addr,		\
	}

#define MAX14906_FAULT_CHANNEL				\
	{						\
            	.ch_type = IIO_VOLTAGE,			\
		.name = "fault",			\
		.attributes = max14906_fault_attrs,	\
	}

static int max14906_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_do_mode(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_do_mode_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);
static int max14906_iio_write_do_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_climit(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_write_climit(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_climit_avail(void *dev, char *buf, uint32_t len,
					  const struct iio_ch_info *channel,
					  intptr_t priv);
static int max14906_iio_read_fault_raw(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_config_iec(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_write_config_iec(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_config_iec_available(void *dev, char *buf,
						  uint32_t len, const struct iio_ch_info *channel,
						  intptr_t priv);

static int max14906_iio_reg_read(struct max14906_iio_desc *, uint32_t, uint32_t *);
static int max14906_iio_reg_write(struct max14906_iio_desc *, uint32_t, uint32_t);

static uint32_t max14906_limit_avail[4] = {600, 130, 300, 1200};

static struct iio_attribute max14906_out_attrs[] = {
	{
		.name = "raw",
		.show = max14906_iio_read_raw,
		.store = max14906_iio_write_raw,
	},
	{
		.name = "offset",
		.show = max14906_iio_read_offset,
	},
	{
		.name = "scale",
		.show = max14906_iio_read_scale,
	},
	{
		.name = "do_mode",
		.show = max14906_iio_read_do_mode,
		.store = max14906_iio_write_do_mode,
	},
	{
		.name = "do_mode_available",
		.shared = IIO_SHARED_BY_ALL,
		.show = max14906_iio_read_do_mode_avail
	},
	{
		.name = "current_limit",
		.show = max14906_iio_read_climit,
		.store = max14906_iio_write_climit,
	},
	{
		.name = "current_limit_available",
		.shared = IIO_SHARED_BY_ALL,
		.show = max14906_iio_read_climit_avail
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max14906_in_attrs[] = {
	{
		.name = "raw",
		.show = max14906_iio_read_raw,
		.store = max14906_iio_write_raw,
	},
	{
		.name = "offset",
		.show = max14906_iio_read_offset,
	},
	{
		.name = "scale",
		.show = max14906_iio_read_scale,
	},
	{
		.name = "IEC_type",
		.shared = IIO_SHARED_BY_DIR,
		.show = max14906_iio_read_config_iec,
		.store = max14906_iio_write_config_iec,
	},
	{
		.name = "IEC_type_available",
		.show = max14906_iio_read_config_iec_available,
	},
	
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max14906_fault_attrs[] = {
	{
		.name = "raw",
		.show = max14906_iio_read_fault_raw,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device max14906_iio_dev = {
	.debug_reg_read = (int32_t (*)())max14906_iio_reg_read,
	.debug_reg_write = (int32_t (*)())max14906_iio_reg_write,
};

static int max14906_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *desc = dev;
	uint32_t val;
	int ret;

	ret = max14906_ch_get(desc->max14906_desc, channel->address, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

static int max14906_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *desc = dev;
	int32_t val;

	if (!channel->ch_out)
		return -EINVAL;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return max14906_ch_set(desc->max14906_desc, channel->address, val);
}

static int max14906_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int max14906_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val = 1;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int max14906_iio_read_do_mode(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint32_t val = 0;
	int ret;

	ret = max14906_reg_read(desc, MAX14906_CONFIG_DO_REG, &val);
	if (ret)
		return ret;

	val = no_os_field_get(MAX14906_DO_MASK(channel->address), val);

	return sprintf(buf, "%s", max14906_do_mode_avail[val]);
}

static int max14906_iio_read_do_mode_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	int i;

	for (i = 0; i < 4; i++)
		length += sprintf(buf + length, "%s ", max14906_do_mode_avail[i]);

	return length;
}

static int max14906_iio_write_do_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint32_t do_mode;
	size_t i;

	do_mode = 0;
	for (i = 0; i < NO_OS_ARRAY_SIZE(max14906_do_mode_avail); i++) {
		if (!strcmp(buf, max14906_do_mode_avail[i]))
			break;

		do_mode++;
	}

	if (do_mode == NO_OS_ARRAY_SIZE(max14906_do_mode_avail))
		return -EINVAL;

	do_mode = no_os_field_prep(MAX14906_DO_MASK(channel->address), do_mode);

	return max14906_reg_update(desc, MAX14906_CONFIG_DO_REG,
				   MAX14906_DO_MASK(channel->address), do_mode);
}

static int max14906_iio_read_climit(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint32_t current_limit;
	int ret;

	ret = max14906_reg_read(desc, MAX14906_CONFIG_CURR_LIM, &current_limit);
	if (ret)
		return ret;

	current_limit = no_os_field_get(MAX14906_CL_MASK(channel->ch_num),
					current_limit);

	return iio_format_value(buf, len, IIO_VAL_INT, 1,
				(int32_t *)&max14906_limit_avail[current_limit]);
}

static int max14906_iio_write_climit(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint32_t val;
	uint32_t i;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	for (i = 0; i < NO_OS_ARRAY_SIZE(max14906_limit_avail) ; i++) {
		if (val == max14906_limit_avail[i])
			break;

		if (i == NO_OS_ARRAY_SIZE(max14906_limit_avail) - 1)
			return -EINVAL;
	}

	return max14906_reg_update(desc, MAX14906_CONFIG_CURR_LIM,
				   MAX14906_CL_MASK(channel->ch_num),
				   no_os_field_prep(MAX14906_CL_MASK(channel->ch_num), i));
}

static int max14906_iio_read_climit_avail(void *dev, char *buf, uint32_t len,
					  const struct iio_ch_info *channel,
					  intptr_t priv)
{
	uint32_t length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(max14906_limit_avail); i++)
		length += sprintf(buf + length, "%d ", max14906_limit_avail[i]);

	return length;
}

static int max14906_iio_read_config_iec(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	enum max14906_iec_type iec_type;
	struct max14906_desc *desc;
	uint32_t val;
	int ret;

	desc = iio_desc->max14906_desc;
	ret = max14906_reg_read(desc, MAX14906_CONFIG_DI_REG, &val);
	if (ret)
		return ret;

	iec_type = no_os_field_get(MAX14906_IEC_TYPE_MASK, val);
	strcpy(buf, max14906_iec_avail[iec_type]);

	return strlen(buf);
}

static int max14906_iio_write_config_iec(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	enum max14906_iec_type iec_type;
	struct max14906_desc *desc;
	uint8_t val;
	size_t i;

	desc = iio_desc->max14906_desc;
	for (i = 0; i < NO_OS_ARRAY_SIZE(max14906_iec_avail); i++) {
		if (!strcmp(buf, max14906_iec_avail[i])) {
			iec_type = i;
			break;
		}

		if (i == NO_OS_ARRAY_SIZE(max14906_iec_avail) - 1)
			return -EINVAL;
	}

	val = no_os_field_prep(MAX14906_IEC_TYPE_MASK, iec_type);

	return max14906_reg_update(desc, MAX14906_CONFIG_DI_REG, MAX14906_IEC_TYPE_MASK, val);
}

static int max14906_iio_read_config_iec_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	size_t i;
	size_t avail_size = NO_OS_ARRAY_SIZE(max14906_iec_avail);

	strcpy(buf, "");
	for (i = 0; i < avail_size; i++) {
		strcat(buf, max14906_iec_avail[i]);

		if (i != avail_size - 1)
			strcat(buf, " ");
	}

	return strlen(buf);
}

int max14906_iio_setup_channels(struct max14906_iio_desc *desc,
				struct max14906_iio_desc_init_param *init_param)
{
	struct iio_channel *max14906_iio_channels;
	/* The fault channel is always active */
	uint32_t enabled_ch = 1;
	struct max14906_ch_config *ch_cfg;
	size_t ch_offset = 0;
	size_t i;
	int ret;

	ch_cfg = *init_param->channel_configs;
	for (i = 0; i < MAX14906_CHANNELS; i++)
		if (ch_cfg[i].enabled)
			enabled_ch++;

	max14906_iio_channels = no_os_calloc(enabled_ch, sizeof(*max14906_iio_channels));
	if (!max14906_iio_channels)
		return -ENOMEM;

	for (i = 0; i < MAX14906_CHANNELS; i++) {
		if (!ch_cfg[i].enabled || ch_cfg[i].function == MAX14906_HIGH_Z) {
			ret = max14906_ch_func(desc->max14906_desc, i, MAX14906_HIGH_Z);
			if (ret)
				return ret;
			continue;
		}

		max14906_iio_channels[ch_offset] = (struct iio_channel)MAX14906_CHANNEL(i);

		/* Set the direction and attributes based on configuration */
		if (ch_cfg[i].function == MAX14906_IN) {
			max14906_iio_channels[ch_offset].attributes = max14906_in_attrs;
			max14906_iio_channels[ch_offset].ch_out = 0;
			ch_offset++;
		} else if (ch_cfg[i].function == MAX14906_OUT) {
			max14906_iio_channels[ch_offset].attributes = max14906_out_attrs;
			max14906_iio_channels[ch_offset].ch_out = 1;
			ch_offset++;
		}		

		ret = max14906_ch_func(desc->max14906_desc, i, ch_cfg[i].function);
		if (ret)
			return ret;
	}

	max14906_iio_channels[ch_offset++] = (struct iio_channel)MAX14906_FAULT_CHANNEL;
	max14906_iio_dev.num_ch = ch_offset;
	max14906_iio_dev.channels = max14906_iio_channels;

	return 0;
}

static int max14906_iio_read_fault_raw(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint32_t reg_val;
	int32_t val = 0;
	int ret;

	ret = max14906_reg_read(desc, MAX14906_OVR_LD_REG, &reg_val);
	if (ret)
		return ret;
	val = reg_val << 24;

	ret = max14906_reg_read(desc, MAX14906_OPN_WIR_FLT_REG, &reg_val);
	if (ret)
		return ret;
	val |= reg_val << 16;

	ret = max14906_reg_read(desc, MAX14906_SHD_VDD_FLT_REG, &reg_val);
	if (ret)
		return ret;
	val |= reg_val << 8;

	ret = max14906_reg_read(desc, MAX14906_GLOBAL_FLT_REG, &reg_val);
	if (ret)
		return ret;
	val |= reg_val;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Register read wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value
 * @return 0 in case of success, error code otherwise
 */
static int max14906_iio_reg_read(struct max14906_iio_desc *dev, uint32_t reg,
				 uint32_t *readval)
{
	return max14906_reg_read(dev->max14906_desc, reg, readval);
}

/**
 * @brief Register write wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value
 * @return 0 in case of success, error code otherwise
 */
static int max14906_iio_reg_write(struct max14906_iio_desc *dev, uint32_t reg,
				  uint32_t writeval)
{
	return max14906_reg_write(dev->max14906_desc, reg, writeval);
}

/**
 * @brief Initializes the MAX14906 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int max14906_iio_init(struct max14906_iio_desc **iio_desc,
		      struct max14906_iio_desc_init_param *init_param)
{
	struct max14906_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->max14906_init_param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = max14906_init(&descriptor->max14906_desc,
			    init_param->max14906_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &max14906_iio_dev;

	ret = max14906_iio_setup_channels(descriptor, init_param);
	if (ret)
		return ret;

	*iio_desc = descriptor;

	return 0;

free_desc:
	free(descriptor);

	return ret;
}

int max14906_iio_remove(struct max14906_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	max14906_remove(iio_desc->max14906_desc);
	free(iio_desc);

	return 0;
}
