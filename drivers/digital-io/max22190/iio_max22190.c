/***************************************************************************//**
 *   @file   iio_max22190.c
 *   @brief  Source file of MAX22190 IIO Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#include "max22190.h"
#include "iio_max22190.h"

static int max22190_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int max22190_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int max22190_iio_read_filter(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

static int max22190_iio_write_filter(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);

static int max22190_iio_read_filter_delay(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22190_iio_write_filter_delay(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22190_iio_read_filter_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22190_iio_read_fault(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int max22190_iio_read_fault_enable(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22190_iio_write_fault_enable(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22190_iio_reg_read(struct max22190_iio_desc *dev, uint32_t reg,
				 uint32_t *readval);

static int max22190_iio_reg_write(struct max22190_iio_desc *dev, uint32_t reg,
				  uint32_t writeval);

static const uint32_t max22190_delay_avail[8] = {
	50, 100, 400, 800, 1800, 3200, 12800, 20000
};

static struct iio_attribute max22190_attrs[] = {
	{
		.name = "raw",
		.show = max22190_iio_read_raw,
	},
	{
		.name = "scale",
		.show = max22190_iio_read_scale,
	},
	{
		.name = "filter_bypass",
		.show = max22190_iio_read_filter,
		.store = max22190_iio_write_filter,
	},
	{
		.name = "filter_delay",
		.show = max22190_iio_read_filter_delay,
		.store = max22190_iio_write_filter_delay,
	},
	{
		.name = "filter_delay_available",
		.show = max22190_iio_read_filter_available,
		.shared = IIO_SHARED_BY_ALL
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max22190_debug_attrs[] = {
	{
		.name = "fault1",
		.show = max22190_iio_read_fault,
		.priv = MAX22190_IIO_FAULT1
	},
	{
		.name = "fault2",
		.show = max22190_iio_read_fault,
		.priv = MAX22190_IIO_FAULT2
	},
	{
		.name = "wbg_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT1_WBGE
	},
	{
		.name = "24vm_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT1_24VME
	},
	{
		.name = "24vl_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT1_24VLE
	},
	{
		.name = "alrmt1_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT1_ALRMT1E
	},
	{
		.name = "alrmt2_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT1_ALRMT2E
	},
	{
		.name = "fault2_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT1_FAULT2E
	},
	{
		.name = "rfwbs_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT2_RFWBSE
	},
	{
		.name = "rfwbo_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT2_RFWBOE
	},
	{
		.name = "rfdis_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT2_RFDISE
	},
	{
		.name = "rfdio_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT2_RFDIOE
	},
	{
		.name = "otshdn_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT2_OTSHDNE
	},
	{
		.name = "fault8ck_enable",
		.show = max22190_iio_read_fault_enable,
		.store = max22190_iio_write_fault_enable,
		.priv = MAX22190_FAULT2_FAULT8CKE
	},
	END_ATTRIBUTES_ARRAY
};

#define MAX22190_CHANNEL(_addr)			\
        {					\
            	.ch_type = IIO_VOLTAGE,		\
        	.indexed = 1,			\
		.channel = _addr,		\
	    	.address = _addr,		\
		.attributes = max22190_attrs,	\
		.ch_out = false,		\
	}

static struct iio_channel max22190_channels[] = {
	MAX22190_CHANNEL(0),
	MAX22190_CHANNEL(1),
	MAX22190_CHANNEL(2),
	MAX22190_CHANNEL(3),
	MAX22190_CHANNEL(4),
	MAX22190_CHANNEL(5),
	MAX22190_CHANNEL(6),
	MAX22190_CHANNEL(7)
};


static struct iio_device max22190_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(max22190_channels),
	.channels = max22190_channels,
	.debug_reg_read = (int32_t (*)())max22190_iio_reg_read,
	.debug_reg_write = (int32_t (*)())max22190_iio_reg_write,
	.debug_attributes = max22190_debug_attrs,
};

/**
 * @brief Read the raw attribute for a specific channel
 * @param dev - The iio device structure
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor.
 * @return 0 in case of succes, error code otherwise.
*/
static int max22190_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct max22190_iio_desc *desc = dev;
	uint32_t val;
	int ret, ch;

	ch = channel->address;

	ret = max22190_reg_read(desc->max22190_desc, MAX22190_DIGITAL_INPUT_REG,
				&val);
	if (ret)
		return ret;

	val = no_os_field_get(MAX22190_CH_STATE_MASK(ch), val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Read the scale attribute for a specific channel
 * @param dev - The iio device structure
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor.
 * @return 0 in case of succes, error code otherwise.
*/
static int max22190_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int32_t val = 1;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the filter bypass attribute for a specific channel
 * @param dev - The iio device structure
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor.
 * @return 0 in case of succes, error code otherwise.
*/
static int max22190_iio_read_filter(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct max22190_iio_desc *desc = dev;
	uint32_t val;
	int ret, ch;

	ch = channel->address;

	ret = max22190_reg_read(desc->max22190_desc, MAX22190_FILTER_IN_REG(ch),
				&val);
	if (ret)
		return ret;

	val = no_os_field_get(MAX22190_FBP_MASK, val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Write the filter bypass attribute for a specific channel
 * @param dev - The iio device structure
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor.
 * @return 0 in case of succes, error code otherwise.
*/
static int max22190_iio_write_filter(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max22190_iio_desc *desc = dev;
	int ch;
	uint32_t val;

	ch = channel->address;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	if (val > 1)
		return -EINVAL;

	return max22190_reg_update(desc->max22190_desc,
				   MAX22190_FILTER_IN_REG(ch),
				   MAX22190_FBP_MASK,
				   no_os_field_prep(MAX22190_FBP_MASK,val));
}

/**
 * @brief Read the filter delay attribute for a specific channel
 * @param dev - The iio device structure
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor.
 * @return 0 in case of succes, error code otherwise.
*/
static int max22190_iio_read_filter_delay(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22190_iio_desc *desc = dev;
	int ret, ch;
	uint32_t val;

	ch = channel->address;

	ret = max22190_reg_read(desc->max22190_desc, MAX22190_FILTER_IN_REG(ch),
				&val);
	if (ret)
		return ret;

	val = max22190_delay_avail[no_os_field_get(MAX22190_DELAY_MASK, val)];

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Write the filter delay attribute for a specific channel
 * @param dev - The iio device structure
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor.
 * @return 0 in case of succes, error code otherwise.
*/
static int max22190_iio_write_filter_delay(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22190_iio_desc *desc = dev;
	int ch;
	uint32_t val, i;

	ch = channel->address;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	for (i = 0; i < NO_OS_ARRAY_SIZE(max22190_delay_avail); i++)
		if (val == max22190_delay_avail[i])
			break;

	if (i == NO_OS_ARRAY_SIZE(max22190_delay_avail))
		return -EINVAL;

	return max22190_reg_update(desc->max22190_desc,
				   MAX22190_FILTER_IN_REG(ch),
				   MAX22190_DELAY_MASK,
				   no_os_field_prep(MAX22190_DELAY_MASK, i));
}

/**
 * @brief Read the available values for filter delay attribute for a specific
 * 	  channel
 * @param dev - The iio device structure
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor.
 * @return 0 in case of succes, error code otherwise.
*/
static int max22190_iio_read_filter_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	uint32_t avail_size = NO_OS_ARRAY_SIZE(max22190_delay_avail);
	uint32_t length = 0;
	uint32_t i;

	for (i = 0; i < avail_size; i++)
		length += sprintf(buf + length, "%d ", max22190_delay_avail[i]);

	return length;
}

/**
 * @brief Read the fault1 or fault 2 attribute for a specific channel
 * @param dev - The iio device structure
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor.
 * @return 0 in case of succes, error code otherwise.
*/
static int max22190_iio_read_fault(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct max22190_iio_desc *desc = dev;
	uint32_t val;
	int ret;

	switch(priv) {
	case MAX22190_IIO_FAULT1:
		ret = max22190_reg_read(desc->max22190_desc, MAX22190_FAULT1_REG, &val);
		if (ret)
			return ret;
		break;
	case MAX22190_IIO_FAULT2:
		ret = max22190_reg_read(desc->max22190_desc, MAX22190_FAULT2_REG, &val);
		if (ret)
			return ret;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/****
 * @brief Read fault enable wrapper.
 * @param dev - The iio device structure
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor.
 * @return 0 in case of succes, error code otherwise.
*/
static int max22190_iio_read_fault_enable(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22190_iio_desc *desc = dev;
	bool val;
	int ret;

	ret =  max22190_fault_enable_get(desc->max22190_desc, priv, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/****
 * @brief Write fault enable wrapper.
 * @param dev - The iio device structure
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor.
 * @return 0 in case of succes, error code otherwise.
*/
static int max22190_iio_write_fault_enable(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22190_iio_desc *desc = dev;
	uint32_t val;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	return max22190_fault_enable_set(desc->max22190_desc, priv, (bool)val);
}

/**
 * @brief Register read wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value.
 * @return 0 in case of succes, error code otherwise
*/
static int max22190_iio_reg_read(struct max22190_iio_desc *dev, uint32_t reg,
				 uint32_t *readval)
{
	return max22190_reg_read(dev->max22190_desc, reg, readval);
}

/**
 * @brief Register write wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 *
 * @param writeval - Register value.
 *
 * @return 0 in case of succes, error code otherwise
*/
static int max22190_iio_reg_write(struct max22190_iio_desc *dev, uint32_t reg,
				  uint32_t writeval)
{
	return max22190_reg_write(dev->max22190_desc, reg, writeval);
}

/**
 * @brief Initializes the MAX22190 IIO Descriptor
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device intial parameters.
 * @return 0 in case of succes, an error code otherwise.
*/
int max22190_iio_init(struct max22190_iio_desc **iio_desc,
		      struct max22190_iio_desc_init_param *init_param)
{
	struct max22190_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->max22190_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = max22190_init(&descriptor->max22190_desc,
			    init_param->max22190_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &max22190_iio_dev;

	*iio_desc = descriptor;

	return 0;

free_desc:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free the resources allocated by max22190_iio_init().
 * @param iio_desc - The IIO device structure.
 * @return ret - Result of the remove procedure.
*/
int max22190_iio_remove(struct max22190_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	max22190_remove(iio_desc->max22190_desc);
	no_os_free(iio_desc);

	return 0;
}
