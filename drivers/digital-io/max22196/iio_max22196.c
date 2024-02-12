/***************************************************************************//**
 *   @file   iio_max22196.c
 *   @brief  Source file of IIO MAX22196 Driver.
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
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

#include "max22196.h"
#include "iio_max22196.h"

static int max22196_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int max22196_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int max22196_iio_read_filter(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

static int max22196_iio_write_filter(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);

static int max22196_iio_read_filter_delay(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22196_iio_write_filter_delay(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22196_iio_read_filter_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22196_iio_read_di_mode(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);

static int max22196_iio_write_di_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv);

static int max22196_iio_read_di_mode_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22196_iio_read_current(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);

static int max22196_iio_write_current(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv);

static int max22196_iio_read_current_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22196_iio_read_fault(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int max22196_iio_read_fault_mask(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int max22196_iio_write_fault_mask(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22196_iio_write_global_cfg(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22196_iio_reg_read(struct max22196_iio_desc *dev, uint32_t reg,
				 uint32_t *readval);

static int max22196_iio_reg_write(struct max22196_iio_desc *dev, uint32_t reg,
				  uint32_t writeval);

static const uint32_t max22196_delay_avail[8] = {
	50, 100, 400, 800, 1800, 3200, 12800, 20000
};

static const char *const max22196_di_mode_avail[2] = {
	[MAX22196_SOURCE_MODE] = "Source_Mode",
	[MAX22196_SINK_MODE] = "Sink_Mode",
};

static const char *const max22196_current_avail[4] = {
	[MAX22196_HTL_MODE] = "HTL_Mode",
	[MAX22196_1X_CURRENT] = "1x_Current",
	[MAX22196_3X_CURRENT] = "3x_Current",
	[MAX22196_TTL_OP_OFF] = "TTL_Operation_OFF",
};

static struct iio_attribute max22196_attrs[] = {
	{
		.name = "raw",
		.show = max22196_iio_read_raw,
	},
	{
		.name = "scale",
		.show = max22196_iio_read_scale,
	},
	{
		.name = "filter_enable",
		.show = max22196_iio_read_filter,
		.store = max22196_iio_write_filter,
	},
	{
		.name = "filter_delay",
		.show = max22196_iio_read_filter_delay,
		.store = max22196_iio_write_filter_delay,
	},
	{
		.name = "filter_delay_available",
		.show = max22196_iio_read_filter_available,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "di_mode",
		.show = max22196_iio_read_di_mode,
		.store = max22196_iio_write_di_mode,
	},
	{
		.name = "di_mode_available",
		.show = max22196_iio_read_di_mode_available,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "current_source",
		.show = max22196_iio_read_current,
		.store = max22196_iio_write_current,
	},
	{
		.name = "current_source_available",
		.show = max22196_iio_read_current_available,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "refdi_sht_cfg",
		.store = max22196_iio_write_global_cfg,
		.priv = MAX22196_GLOBAL_REFDISHTCFG
	},
	{
		.name = "clrf_filtr",
		.store = max22196_iio_write_global_cfg,
		.priv = MAX22196_GLOBAL_CLRFILTR
	},
	{
		.name = "fspi_clr",
		.store = max22196_iio_write_global_cfg,
		.priv = MAX22196_GLOBAL_FSPICLR
	},
	{
		.name = "led9",
		.store = max22196_iio_write_global_cfg,
		.priv = MAX22196_GLOBAL_LED9
	},
	{
		.name = "led_int",
		.store = max22196_iio_write_global_cfg,
		.priv = MAX22196_GLOBAL_LEDINT
	},
	{
		.name = "gpo",
		.store = max22196_iio_write_global_cfg,
		.priv = MAX22196_GLOBAL_GPO
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max22196_debug_attrs[] = {
	{
		.name = "fault1",
		.show = max22196_iio_read_fault,
		.priv = MAX22196_IIO_FAULT1
	},
	{
		.name = "fault2",
		.show = max22196_iio_read_fault,
		.priv = MAX22196_IIO_FAULT2
	},
	{
		.name = "vmlow_mask",
		.show = max22196_iio_read_fault_mask,
		.store = max22196_iio_write_fault_mask,
		.priv = MAX22196_FAULT1_VMLOW
	},
	{
		.name = "v24uv_mask",
		.show = max22196_iio_read_fault_mask,
		.store = max22196_iio_write_fault_mask,
		.priv = MAX22196_FAULT1_V24UV
	},
	{
		.name = "temp_alarm_mask",
		.show = max22196_iio_read_fault_mask,
		.store = max22196_iio_write_fault_mask,
		.priv = MAX22196_FAULT1_TEMPALM
	},
	{
		.name = "otshdn1_mask",
		.show = max22196_iio_read_fault_mask,
		.store = max22196_iio_write_fault_mask,
		.priv = MAX22196_FAULT1_OTSHDN1
	},
	{
		.name = "fault2_mask",
		.show = max22196_iio_read_fault_mask,
		.store = max22196_iio_write_fault_mask,
		.priv = MAX22196_FAULT1_FAULT2
	},
	{
		.name = "rfdis_mask",
		.show = max22196_iio_read_fault_mask,
		.store = max22196_iio_write_fault_mask,
		.priv = MAX22196_FAULT2_RFDIS
	},
	{
		.name = "rfdio_mask",
		.show = max22196_iio_read_fault_mask,
		.store = max22196_iio_write_fault_mask,
		.priv = MAX22196_FAULT2_RFDIO
	},
	{
		.name = "otshdn2_mask",
		.show = max22196_iio_read_fault_mask,
		.store = max22196_iio_write_fault_mask,
		.priv = MAX22196_FAULT2_OTSHDN2
	},
	{
		.name = "spi8clk_mask",
		.show = max22196_iio_read_fault_mask,
		.store = max22196_iio_write_fault_mask,
		.priv = MAX22196_FAULT2_SPI8CLK
	},
	{
		.name = "vauv_mask",
		.show = max22196_iio_read_fault_mask,
		.store = max22196_iio_write_fault_mask,
		.priv = MAX22196_FAULT2_VAUV
	},
	END_ATTRIBUTES_ARRAY
};

#define MAX22196_CHANNEL(_addr)			\
	{					\
		.ch_type = IIO_VOLTAGE,		\
		.indexed = 1,			\
		.channel = _addr,		\
		.address = _addr,		\
		.ch_out = false,		\
		.attributes = max22196_attrs	\
	}

static struct iio_device max22196_iio_dev = {
	.debug_reg_read = (int32_t (*)())max22196_iio_reg_read,
	.debug_reg_write = (int32_t (*)())max22196_iio_reg_write,
	.debug_attributes = max22196_debug_attrs
};

/**
 * @brief Read the raw attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	uint32_t val;
	int ret, ch;

	ch = channel->address;

	ret = max22196_reg_read(desc->max22196_desc, MAX22196_DI_STATE_REG,
				&val);
	if (ret)
		return ret;

	val = no_os_field_get(MAX22196_DI_STATE_MASK(ch), val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Read the scale attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int32_t val = 1;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the filter attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_read_filter(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	uint32_t val;
	int ret, ch;

	ch = channel->address;

	ret = max22196_reg_read(desc->max22196_desc, MAX22196_CFG_REG(ch),
				&val);
	if (ret)
		return ret;

	val = no_os_field_get(MAX22196_FLTEN_MASK, val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Write the filter attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_write_filter(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	int ch;
	uint32_t val;

	ch = channel->address;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	if (val > 1)
		return -EINVAL;

	return max22196_reg_update(desc->max22196_desc,
				   MAX22196_CFG_REG(ch),
				   MAX22196_FLTEN_MASK,
				   no_os_field_prep(MAX22196_FLTEN_MASK, val));
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
static int max22196_iio_read_filter_delay(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	int ret, ch;
	uint32_t val;

	ch = channel->address;

	ret = max22196_reg_read(desc->max22196_desc, MAX22196_CFG_REG(ch),
				&val);
	if (ret)
		return ret;

	val = max22196_delay_avail[no_os_field_get(MAX22196_DELAY_MASK, val)];

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
static int max22196_iio_write_filter_delay(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	int ch;
	uint32_t val, i;

	ch = channel->address;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	for (i = 0; i < NO_OS_ARRAY_SIZE(max22196_delay_avail); i++)
		if (val == max22196_delay_avail[i])
			break;

	if (i == NO_OS_ARRAY_SIZE(max22196_delay_avail))
		return -EINVAL;

	return max22196_reg_update(desc->max22196_desc,
				   MAX22196_CFG_REG(ch),
				   MAX22196_DELAY_MASK,
				   no_os_field_prep(MAX22196_DELAY_MASK, i));
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
static int max22196_iio_read_filter_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	uint32_t avail_size = NO_OS_ARRAY_SIZE(max22196_delay_avail);
	uint32_t length = 0;
	uint32_t i;

	for (i = 0; i < avail_size; i++)
		length += sprintf(buf + length, "%d ", max22196_delay_avail[i]);

	return length;
}

/**
 * @brief Read the di mode attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_read_di_mode(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	uint32_t val;
	int ret, ch;

	ch = channel->address;

	ret = max22196_reg_read(desc->max22196_desc, MAX22196_CFG_REG(ch),
				&val);
	if (ret)
		return ret;

	val = no_os_field_get(MAX22196_SOURCE_MASK, val);

	return sprintf(buf, "%s ", max22196_di_mode_avail[val]);
}

/**
 * @brief Write the di mode attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_write_di_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	uint32_t i;
	int ch;

	ch = channel->address;

	for (i = 0; i < NO_OS_ARRAY_SIZE(max22196_di_mode_avail); i++)
		if (!strcmp(buf, max22196_di_mode_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(max22196_di_mode_avail))
		return -EINVAL;

	return max22196_reg_update(desc->max22196_desc, MAX22196_CFG_REG(ch),
				   MAX22196_SOURCE_MASK,
				   no_os_field_prep(MAX22196_SOURCE_MASK, i));
}

/**
 * @brief Read the di mode available attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_read_di_mode_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	uint32_t avail_size = NO_OS_ARRAY_SIZE(max22196_di_mode_avail);
	uint32_t length = 0;
	uint32_t i;

	for(i = 0; i < avail_size; i++)
		length += sprintf(buf + length, "%s ", max22196_di_mode_avail[i]);

	return length;
}

/**
 * @brief Read the current attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_read_current(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max22196_iio_desc *iio_desc = dev;
	uint32_t val;
	int ret, ch;

	ch = channel->address;

	ret = max22196_reg_read(iio_desc->max22196_desc, MAX22196_CFG_REG(ch),
				&val);
	if (ret)
		return ret;

	val = no_os_field_get(MAX22196_CURR_MASK, val);

	return sprintf(buf, "%s", max22196_current_avail[val]);
}

/**
 * @brief Write the current attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_write_current(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	uint32_t i;
	int ch;

	ch = channel->address;

	for (i = 0; i < NO_OS_ARRAY_SIZE(max22196_current_avail); i++)
		if (!strcmp(buf, max22196_current_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(max22196_current_avail))
		return -EINVAL;

	i = no_os_field_prep(MAX22196_CURR_MASK, i);

	return max22196_reg_update(desc->max22196_desc, MAX22196_CFG_REG(ch),
				   MAX22196_CURR_MASK, i);
}

/**
 * @brief Read the current available attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_read_current_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(max22196_current_avail); i++)
		length += sprintf(buf + length, "%s ", max22196_current_avail[i]);

	return length;
}

/**
 * @brief Read the fault debug attribute.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
*/
static int max22196_iio_read_fault(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	uint32_t val;
	int ret;

	switch (priv) {
	case MAX22196_IIO_FAULT1:
		ret = max22196_iio_reg_read(desc, MAX22196_FAULT1_REG, &val);
		if (ret)
			return ret;
		break;
	case MAX22196_IIO_FAULT2:
		ret = max22196_iio_reg_read(desc, MAX22196_FAULT2_REG, &val);
		if (ret)
			return ret;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Read fault mask debug attribute.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
*/
static int max22196_iio_read_fault_mask(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	int ret;
	bool enabled;

	ret = max22196_fault_mask_get(desc->max22196_desc, priv, &enabled);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&enabled);
}

/**
 * @brief Write fault mask debug attribute.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
*/
static int max22196_iio_write_fault_mask(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	uint32_t val;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	return max22196_fault_mask_set(desc->max22196_desc, priv, (bool)val);
}

/**
 * @brief Global config register configuration wrapper.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
*/
static int max22196_iio_write_global_cfg(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22196_iio_desc *desc = dev;
	uint32_t val;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	return max22196_global_cfg(desc->max22196_desc, priv, (bool)val);
}

/**
 * @brief Configure a set if IIO channels based on the chip id
 * physical channels.
 * @param desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int max22196_iio_setup_channels(struct max22196_iio_desc *desc)
{
	struct iio_channel *max22196_iio_channels;
	int i;

	max22196_iio_channels = no_os_calloc(desc->max22196_desc->max_chn_nb,
					     sizeof(*max22196_iio_channels));

	if (!max22196_iio_channels)
		return -ENOMEM;

	for (i = 0; i < desc->max22196_desc->max_chn_nb; i++)
		max22196_iio_channels[i] = (struct iio_channel)MAX22196_CHANNEL(i);

	desc->iio_dev->channels = max22196_iio_channels;
	desc->iio_dev->num_ch = desc->max22196_desc->max_chn_nb;

	return 0;
}

/**
 * @brief Register read wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_reg_read(struct max22196_iio_desc *dev, uint32_t reg,
				 uint32_t *readval)
{
	return max22196_reg_read(dev->max22196_desc, reg, readval);
}

/**
 * @brief Register write wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value
 * @return 0 in case of success, error code otherwise
 */
static int max22196_iio_reg_write(struct max22196_iio_desc *dev, uint32_t reg,
				  uint32_t writeval)
{
	return max22196_reg_write(dev->max22196_desc, reg, writeval);
}

/**
 * @brief Initializes the MAX22196 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int max22196_iio_init(struct max22196_iio_desc **iio_desc,
		      struct max22196_iio_desc_init_param *init_param)
{
	struct max22196_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->max22196_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = max22196_init(&descriptor->max22196_desc,
			    init_param->max22196_init_param);
	if (ret)
		goto free_dev;

	descriptor->iio_dev = &max22196_iio_dev;

	ret = max22196_iio_setup_channels(descriptor);
	if (ret)
		goto free_dev;

	*iio_desc = descriptor;

	return 0;

free_dev:
	max22196_iio_remove(descriptor);
	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int max22196_iio_remove(struct max22196_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	max22196_remove(iio_desc->max22196_desc);
	no_os_free(iio_desc->iio_dev->channels);
	no_os_free(iio_desc);

	return 0;
}
