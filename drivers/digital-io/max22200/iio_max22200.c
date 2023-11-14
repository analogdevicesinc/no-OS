/***************************************************************************//**
 *   @file   iio_max22200.c
 *   @brief  Source file of IIO MAX22200 Driver.
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

#include "iio_max22200.h"

#define MAX22200_CHANNEL(_addr)		\
	{				\
		.ch_type = IIO_VOLTAGE,	\
		.indexed = true,	\
		.channel = _addr,	\
		.address = _addr,	\
	}

static int max22200_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int max22200_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int max22200_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int max22200_iio_read_hit(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int max22200_iio_read_hold(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int max22200_iio_read_channel_trig(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22200_iio_read_ch_mode(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);

static int max22200_iio_read_channel_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22200_iio_read_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

static int max22200_iio_read_fault_mask(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int max22200_iio_read_chopping_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22200_iio_read_available(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int max22200_iio_reg_read(struct max22200_iio_desc *, uint32_t,
				 uint32_t *);

static int max22200_iio_write_ch_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv);

static int max22200_iio_write_enable(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);

static int max22200_iio_write_fault_mask(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22200_iio_write_trig_state(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22200_iio_write_chopping_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22200_iio_write_ch_avail_attrs(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22200_iio_write_channel_attrs(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int max22200_iio_reg_write(struct max22200_iio_desc *, uint32_t,
				  uint32_t);

static struct iio_attribute max22200_attrs[] = {
	{
		.name = "raw",
		.show = max22200_iio_read_raw,
		.store = max22200_iio_write_raw
	},
	{
		.name = "scale",
		.show = max22200_iio_read_scale,
		.store = max22200_iio_write_ch_avail_attrs,
		.priv = MAX22200_IIO_SCALE
	},
	{
		.name = "scale_available",
		.show = max22200_iio_read_available,
		.priv = MAX22200_IIO_SCALE_AVAILABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "hit_current",
		.show = max22200_iio_read_hit,
		.store = max22200_iio_write_channel_attrs,
		.priv = MAX22200_IIO_HIT_CURRENT
	},
	{
		.name = "hit_time",
		.show = max22200_iio_read_hit,
		.store = max22200_iio_write_channel_attrs,
		.priv = MAX22200_IIO_HIT_TIME
	},
	{
		.name = "hold_current",
		.show = max22200_iio_read_hold,
		.store = max22200_iio_write_channel_attrs,
		.priv = MAX22200_IIO_HOLD_CURRENT
	},
	{
		.name = "channel_trig",
		.show = max22200_iio_read_channel_trig,
		.store = max22200_iio_write_ch_avail_attrs,
		.priv = MAX22200_IIO_CH_TRIG
	},
	{
		.name = "channel_trig_available",
		.show = max22200_iio_read_available,
		.priv = MAX22200_IIO_CH_TRIG_AVAILABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "channel_drive",
		.show = max22200_iio_read_ch_mode,
		.store = max22200_iio_write_ch_mode,
		.priv = MAX22200_IIO_CH_DRIVE
	},
	{
		.name = "channel_drive_available",
		.show = max22200_iio_read_available,
		.priv = MAX22200_IIO_CH_DRIVE_AVAILABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "channel_side",
		.show = max22200_iio_read_ch_mode,
		.store = max22200_iio_write_ch_mode,
		.priv = MAX22200_IIO_CH_SIDE
	},
	{
		.name = "channel_side_available",
		.show = max22200_iio_read_available,
		.priv = MAX22200_IIO_CH_SIDE_AVAILABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "channel_op_mode",
		.show = max22200_iio_read_ch_mode,
		.store = max22200_iio_write_ch_mode,
		.priv = MAX22200_IIO_CH_OP_MODE
	},
	{
		.name = "channel_op_mode_available",
		.show = max22200_iio_read_available,
		.priv = MAX22200_IIO_CH_OP_MODE_AVAILABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "channel_freq",
		.show = max22200_iio_read_channel_freq,
		.store = max22200_iio_write_ch_avail_attrs,
		.priv = MAX22200_IIO_CH_FREQ
	},
	{
		.name = "channel_freq_available",
		.show = max22200_iio_read_available,
		.priv = MAX22200_IIO_CH_FREQ_AVAILABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "slew_rate_control",
		.show = max22200_iio_read_enable,
		.store = max22200_iio_write_enable,
		.priv = MAX22200_SRC
	},
	{
		.name = "open_load_detection",
		.show = max22200_iio_read_enable,
		.store = max22200_iio_write_enable,
		.priv = MAX22200_OL_ENABLE
	},
	{
		.name = "dpm_enable",
		.show = max22200_iio_read_enable,
		.store = max22200_iio_write_enable,
		.priv = MAX22200_DPM_ENABLE
	},
	{
		.name = "hit_currrent_check",
		.show = max22200_iio_read_enable,
		.store = max22200_iio_write_enable,
		.priv = MAX22200_HHF_ENABLE
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max22200_debug_attrs[] = {
	{
		.name = "thermal_protection_mask",
		.show = max22200_iio_read_fault_mask,
		.store = max22200_iio_write_fault_mask,
		.priv = MAX22200_M_OVT
	},
	{
		.name = "overcurrent_protection_mask",
		.show = max22200_iio_read_fault_mask,
		.store = max22200_iio_write_fault_mask,
		.priv = MAX22200_M_OCP
	},
	{
		.name = "open_load_mask",
		.show = max22200_iio_read_fault_mask,
		.store = max22200_iio_write_fault_mask,
		.priv = MAX22200_M_OLF
	},
	{
		.name = "hit_current_fault_mask",
		.show = max22200_iio_read_fault_mask,
		.store = max22200_iio_write_fault_mask,
		.priv = MAX22200_M_HHF
	},
	{
		.name = "dpm_fault_mask",
		.show = max22200_iio_read_fault_mask,
		.store = max22200_iio_write_fault_mask,
		.priv = MAX22200_M_DPM
	},
	{
		.name = "com_err_mask",
		.show = max22200_iio_read_fault_mask,
		.store = max22200_iio_write_fault_mask,
		.priv = MAX22200_M_COMF
	},
	{
		.name = "uvlo_mask",
		.show = max22200_iio_read_fault_mask,
		.store = max22200_iio_write_fault_mask,
		.priv = MAX22200_M_UVM
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max22200_global_attrs[] = {
	{
		.name = "chopping_frequency",
		.show = max22200_iio_read_chopping_freq,
		.store = max22200_iio_write_chopping_freq,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "chopping_frequency_available",
		.show = max22200_iio_read_available,
		.priv = MAX22200_IIO_CHOP_FREQ_AVAILABLE,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "external_trig_state",
		.store = max22200_iio_write_trig_state,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device max22200_iio_dev = {
	.debug_reg_read = (int32_t (*)())max22200_iio_reg_read,
	.debug_reg_write = (int32_t (*)())max22200_iio_reg_write,
	.debug_attributes = max22200_debug_attrs,
	.attributes = max22200_global_attrs
};

/**
 * @brief Read the raw attribute for a specific channel (state of the channel)
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	bool ch_state;
	int32_t val;
	int ret;

	ret = max22200_get_ch_state(desc->max22200_desc, channel->address,
				    &ch_state);
	if (ret)
		return ret;

	val = ch_state ? 1 : 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the scale attribute for a specific channel.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	enum max22200_scale ch_scale;
	int ret;

	ret = max22200_get_ch_scale(desc->max22200_desc, channel->address,
				    &ch_scale);
	if (ret)
		return ret;

	strcpy(buf, max22200_scale_avail[ch_scale]);

	return strlen(buf);
}

/**
 * @brief Read the hit attribute.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
*/
static int max22200_iio_read_hit(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	int32_t hit_c, hit_t;
	uint8_t hit_c_byte, hit_t_byte;
	int ret;

	ret = max22200_get_ch_hit(desc->max22200_desc, channel->address,
				  &hit_c_byte, &hit_t_byte);
	if (ret)
		return ret;

	hit_c = hit_c_byte;
	hit_t = hit_t_byte;

	switch (priv) {
	case MAX22200_IIO_HIT_CURRENT:
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &hit_c);
	case MAX22200_IIO_HIT_TIME:
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &hit_t);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read the hit attribute.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
*/
static int max22200_iio_read_hold(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	int32_t val;
	uint8_t hold_byte;
	int ret;

	ret = max22200_get_ch_hold(desc->max22200_desc, channel->address,
				   &hold_byte);
	if (ret)
		return ret;

	val = hold_byte;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the channel_trig attribute.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
*/
static int max22200_iio_read_channel_trig(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	enum max22200_trig ch_trig;
	int32_t val;
	int ret;

	ret = max22200_get_ch_trig(desc->max22200_desc, channel->address,
				   &ch_trig);
	if (ret)
		return ret;

	val = ch_trig;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the channel_drive/channel_side/channel_op_mode attribute.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
*/
static int max22200_iio_read_ch_mode(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	enum max22200_ch_drive ch_drive;
	enum max22200_ch_side ch_side;
	enum max22200_ch_op_mode ch_op_mode;
	int ret;

	ret = max22200_get_ch_mode(desc->max22200_desc, channel->address,
				   &ch_drive, &ch_side, &ch_op_mode);
	if (ret)
		return ret;

	switch (priv) {
	case MAX22200_IIO_CH_DRIVE:
		return sprintf(buf, "%s ", max22200_ch_drive_avail[ch_drive]);
	case MAX22200_IIO_CH_SIDE:
		return sprintf(buf, "%s ", max22200_ch_side_avail[ch_side]);
	case MAX22200_IIO_CH_OP_MODE:
		return sprintf(buf, "%s ",
			       max22200_ch_op_mode_avail[ch_op_mode]);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read the channel_freq attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_read_channel_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	enum max22200_ch_freq ch_freq;
	int ret;

	ret = max22200_get_ch_freq(desc->max22200_desc, channel->address,
				   &ch_freq);
	if (ret)
		return ret;

	return sprintf(buf, "%s ", max22200_channel_freq_avail[ch_freq]);
}

/**
 * @brief Read the enable attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_read_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	bool ch_enable;
	int32_t val;
	int ret;

	ret = max22200_get_ch_enable(desc->max22200_desc, channel->address,
				     priv, &ch_enable);
	if (ret)
		return ret;

	val = ch_enable ? 1 : 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the fault_mask debug attribute for MAX22200 IIO descriptor
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_read_fault_mask(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	int32_t val;
	bool enabled;
	int ret;

	ret = max22200_fault_mask_get(desc->max22200_desc, priv, &enabled);
	if (ret)
		return ret;

	val = enabled ? 1 : 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the chopping_freq attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_read_chopping_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	enum max22200_chopping_freq chopping_freq;
	int32_t val;
	int ret;

	ret = max22200_get_chop_freq(desc->max22200_desc, &chopping_freq);
	if (ret)
		return ret;

	val = chopping_freq;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the available attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_read_available(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	uint32_t length = 0;
	uint32_t i;

	switch (priv) {
	case MAX22200_IIO_SCALE_AVAILABLE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_scale_avail); i++)
			length += sprintf(buf + length, "%s ", max22200_scale_avail[i]);

		return length;
	case MAX22200_IIO_CH_TRIG_AVAILABLE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_channel_trig_avail); i++)
			length += sprintf(buf + length, "%s ", max22200_channel_trig_avail[i]);

		return length;
	case MAX22200_IIO_CH_FREQ_AVAILABLE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_channel_freq_avail); i++)
			length += sprintf(buf + length, "%s ",
					  max22200_channel_freq_avail[i]);

		return length;
	case MAX22200_IIO_CHOP_FREQ_AVAILABLE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_chopping_freq_avail); i++)
			length += sprintf(buf + length, "%s ",
					  max22200_chopping_freq_avail[i]);

		return length;
	case MAX22200_IIO_CH_DRIVE_AVAILABLE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_ch_drive_avail); i++)
			length += sprintf(buf + length, "%s ",
					  max22200_ch_drive_avail[i]);

		return length;
	case MAX22200_IIO_CH_SIDE_AVAILABLE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_ch_side_avail); i++)
			length += sprintf(buf + length, "%s ",
					  max22200_ch_side_avail[i]);

		return length;
	case MAX22200_IIO_CH_OP_MODE_AVAILABLE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_ch_op_mode_avail); i++)
			length += sprintf(buf + length, "%s ",
					  max22200_ch_op_mode_avail[i]);

		return length;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Register read wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_reg_read(struct max22200_iio_desc *dev, uint32_t reg,
				 uint32_t *readval)
{
	return max22200_reg_read(dev->max22200_desc, reg, readval);
}

/**
 * @brief Write the raw attribute for a specific channel (state of the channel)
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return max22200_set_ch_state(desc->max22200_desc, channel->address,
				     val ? true : false);
}

/**
 * @brief Write the ch_mode attribute for a specific channel.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_write_ch_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	enum max22200_ch_drive ch_drive;
	enum max22200_ch_side ch_side;
	enum max22200_ch_op_mode ch_op_mode;
	size_t i;
	int ret;

	ret = max22200_get_ch_mode(desc->max22200_desc, channel->address,
				   &ch_drive, &ch_side, &ch_op_mode);
	if (ret)
		return ret;

	switch (priv) {
	case MAX22200_IIO_CH_DRIVE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_ch_drive_avail); i++) {
			if (!strcmp(buf, max22200_ch_drive_avail[i])) {
				ch_drive = i;

				return max22200_set_ch_mode(desc->max22200_desc, channel->address, ch_drive,
							    ch_side, ch_op_mode);
			}
		}

		return -EINVAL;
	case MAX22200_IIO_CH_SIDE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_ch_side_avail); i++) {
			if (!strcmp(buf, max22200_ch_side_avail[i])) {
				ch_side = i;

				return max22200_set_ch_mode(desc->max22200_desc, channel->address, ch_drive,
							    ch_side, ch_op_mode);
			}
		}

		return -EINVAL;
	case MAX22200_IIO_CH_OP_MODE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_ch_op_mode_avail); i++) {
			if (!strcmp(buf, max22200_ch_op_mode_avail[i])) {
				ch_op_mode = i;

				return max22200_set_ch_mode(desc->max22200_desc, channel->address, ch_drive,
							    ch_side, ch_op_mode);
			}
		}

		return -EINVAL;
	default:
		return -EINVAL;
	}


}

/**
 * @brief Write the channel_enable attribute for a specific channel.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_write_enable(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return max22200_set_ch_enable(desc->max22200_desc, channel->address,
				      priv, val ? true : false);
}

/**
 * @brief Write the fault_mask debug attribute for MAX22200 IIO device.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_write_fault_mask(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return max22200_fault_mask_set(desc->max22200_desc, priv, val ? true : false);
}

/**
 * @brief Write the fault_mask debug attribute for MAX22200 IIO device.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_write_trig_state(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return max22200_set_trig_state(desc->max22200_desc,
				       val ? true : false);
}

/**
 * @brief Write the chopping_freq attribute for MAX22200 IIO device.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_write_chopping_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	enum max22200_chopping_freq chopping_freq;
	size_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_chopping_freq_avail); i++) {
		if (!strcmp(buf, max22200_chopping_freq_avail[i])) {
			chopping_freq = i;

			return max22200_set_chop_freq(desc->max22200_desc, chopping_freq);
		}
	}

	return -EINVAL;
}

/**
 * @brief Write any channel attribute with an available attribute related to it
 * 	  for MAX22200 IIO device.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_write_ch_avail_attrs(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	uint32_t val;
	size_t i;

	switch (priv) {
	case MAX22200_IIO_SCALE:
		for(i = 0; i < NO_OS_ARRAY_SIZE(max22200_scale_avail); i++) {
			if (!strcmp(buf, max22200_scale_avail[i])) {
				val = no_os_field_prep(MAX22200_HFS_MASK, i);

				return max22200_set_ch_scale(desc->max22200_desc, channel->address,
							     val ? MAX22200_HALF_FULL_SCALE : MAX22200_FULLSCALE);
			}
		}

		return -EINVAL;
	case MAX22200_IIO_CH_TRIG:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_channel_trig_avail); i++) {
			if (!strcmp(buf, max22200_channel_trig_avail[i])) {
				val = no_os_field_prep(MAX22200_TRGNSP_IO_MASK, i);

				return max22200_set_ch_trig(desc->max22200_desc, channel->address,
							    val ? MAX22200_TRIG : MAX22200_ONCH_SPI);
			}
		}

		return -EINVAL;
	case MAX22200_IIO_CH_FREQ:
		for (i = 0; i < NO_OS_ARRAY_SIZE(max22200_channel_freq_avail); i++)
			if (!strcmp(buf, max22200_channel_freq_avail[i]))
				return max22200_set_ch_freq(desc->max22200_desc, channel->address,
							    (enum max22200_ch_freq)i);

		return -EINVAL;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Write hit_current/hit_time/hold_current attribute
 * 	  for MAX22200 IIO device.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_write_channel_attrs(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max22200_iio_desc *desc = dev;
	uint32_t val;
	uint8_t read_byte1, read_byte2;
	int ret;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	switch (priv) {
	case MAX22200_IIO_HIT_CURRENT:
		if (val > MAX22200_HIT_MAX_VAL)
			return -EINVAL;

		ret = max22200_get_ch_hit(desc->max22200_desc, channel->address,
					  &read_byte1, &read_byte2);
		if (ret)
			return ret;

		return max22200_set_ch_hit(desc->max22200_desc,
					   channel->address, (uint8_t)val,
					   read_byte2);
	case MAX22200_IIO_HIT_TIME:
		if (val > MAX22200_HIT_T_MAX_VAL)
			return -EINVAL;
		ret = max22200_get_ch_hit(desc->max22200_desc, channel->address,
					  &read_byte1, &read_byte2);
		if (ret)
			return ret;

		return max22200_set_ch_hit(desc->max22200_desc,
					   channel->address, read_byte1,
					   (uint8_t)val);
	case MAX22200_IIO_HOLD_CURRENT:
		if (val > MAX22200_HOLD_MAX_VAL)
			return -EINVAL;

		return max22200_set_ch_hold(desc->max22200_desc, channel->address,
					    (uint8_t)val);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Register write wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value
 * @return 0 in case of success, error code otherwise
 */
static int max22200_iio_reg_write(struct max22200_iio_desc *dev, uint32_t reg,
				  uint32_t writeval)
{
	return max22200_reg_write(dev->max22200_desc, reg, writeval);
}

int max22200_iio_setup_channels(struct max22200_iio_desc *desc, bool *ch_enable)
{
	struct iio_channel *max22200_iio_channels;
	uint32_t enabled_ch = 0;
	uint32_t ch_offset = 0;
	uint32_t i;
	int ret;

	for (i = 0; i < MAX22200_CHANNELS; i++)
		if (ch_enable[i])
			enabled_ch++;

	max22200_iio_channels = no_os_calloc(enabled_ch,
					     sizeof(*max22200_iio_channels));

	if (!max22200_iio_channels)
		return -ENOMEM;

	for (i = 0; i < MAX22200_CHANNELS; i++) {
		if (ch_enable[i]) {
			max22200_iio_channels[ch_offset] = (struct iio_channel)MAX22200_CHANNEL(i);
			max22200_iio_channels[ch_offset].attributes = max22200_attrs;
			max22200_iio_channels[ch_offset].ch_out = true;
			ch_offset++;
			ret = max22200_set_ch_state(desc->max22200_desc, i,
						    true);
			if (ret)
				return ret;
		}
	}

	desc->iio_dev->channels = max22200_iio_channels;
	desc->iio_dev->num_ch = enabled_ch;

	return 0;
}

/**
 * @brief Initializes the MAX22200 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int max22200_iio_init(struct max22200_iio_desc **iio_desc,
		      struct max22200_iio_desc_init_param *init_param)
{
	struct max22200_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->max22200_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = max22200_init(&descriptor->max22200_desc,
			    init_param->max22200_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &max22200_iio_dev;

	ret = max22200_iio_setup_channels(descriptor, init_param->ch_enable);
	if (ret)
		goto free_dev;

	*iio_desc = descriptor;

	return 0;
free_dev:
	max22200_remove(descriptor->max22200_desc);
free_desc:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int max22200_iio_remove(struct max22200_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	no_os_free(iio_desc->iio_dev->channels);
	max22200_remove(iio_desc->max22200_desc);
	no_os_free(iio_desc);

	return 0;
}
