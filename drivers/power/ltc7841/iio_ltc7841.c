/***************************************************************************//**
 *   @file   iio_ltc7841.c
 *   @brief  Source file for the LTC7841 IIO Driver
 *   @author Marvin Cabuenas (marvinneil.cabuenas@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "iio_ltc7841.h"

static struct iio_device ltc7841_iio_dev;

enum ltc7841_iio_chan_type 
{
	LTC7841_IIO_VOUT_CHAN,
	LTC7841_IIO_VIN_CHAN,
	LTC7841_IIO_IOUT_CHAN,
	LTC7841_IIO_IIN_CHAN,
	LTC7841_IIO_TEMP_CHAN,
};

/**
 * @brief Read register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to read.
 * @param readval - Read value.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
//static
static int32_t ltc7841_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
    int ret;
    struct ltc7841_iio_desc *iio_ltc7841 = dev;
	struct ltc7841_desc *ltc7841 = iio_ltc7841->ltc7841_desc;
    uint8_t register_order;
    
    register_order = get_ltc7841_register_order((uint8_t)reg);
    ret = ltc7841_reg_read(ltc7841, &ltc7841_regs[register_order]);
    if (ret == 0)
    {
        *readval = ltc7841_regs[reg].value; 
    }
    return ret;
}

/**
 * @brief Write register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to write.
 * @param writeval - Value to write.
 * @return ret    - Result of the writing procedure.
*/
static int32_t ltc7841_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
    int ret;
    struct ltc7841_iio_desc *iio_ltc7841 = dev;
	struct ltc7841_desc *ltc7841 = iio_ltc7841->ltc7841_desc;
    struct ltc7841_reg_st ltc7841_register;

    ltc7841_register.addr = (uint8_t) reg;
    ltc7841_register.value = (uint16_t) writeval;

    ret = ltc7841_reg_write(ltc7841, ltc7841_register);
    return ret;
}

/**
 * @brief Initializes the LTC7841 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int ltc7841_iio_init(struct ltc7841_iio_desc **iio_desc,
		     struct ltc7841_iio_desc_init_param *init_param)
{
	struct ltc7841_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->ltc7841_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = ltc7841_init(&descriptor->ltc7841_desc,
			   init_param->ltc7841_init_param);
	if (ret)
    {
		goto dev_err;
    }

	descriptor->iio_dev = &ltc7841_iio_dev;

	*iio_desc = descriptor;

	return 0;

dev_err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int ltc7841_iio_remove(struct ltc7841_iio_desc *desc)
{
	if (!desc)
    {
		return -ENODEV;
    }

	no_os_free(desc->iio_dev->channels);
	ltc7841_remove(desc->ltc7841_desc);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7841_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ltc7841_iio_desc *iio_ltc7841 = dev;
	struct ltc7841_desc *ltc7841 = iio_ltc7841->ltc7841_desc;
	int ret;
	int32_t value[2];
    uint16_t temp;

	switch (channel->address) 
    {
        case LTC7841_IIO_VIN_CHAN:
        {
            ret = ltc7841_reg_read(ltc7841, &ltc7841_regs[LTC7841_READ_VIN_ORDER]);
            if (0 == ret)
            {
                temp = ltc7841_regs[LTC7841_READ_VIN_ORDER].value;
				value[0] = (int32_t)(temp / CENTI);
				value[1] = (int32_t)(temp % CENTI) * DECIMAL_POINT_SCALE_VOLTAGE;
            }
            break;
        }
        case LTC7841_IIO_VOUT_CHAN:
        {
            ret = ltc7841_reg_read(ltc7841, &ltc7841_regs[LTC7841_READ_VOUT_ORDER]);
            if (0 == ret)
            {
                temp = ltc7841_regs[LTC7841_READ_VOUT_ORDER].value;
				value[0] = (int32_t)(temp / CENTI);
				value[1] = (int32_t)(temp % CENTI) * DECIMAL_POINT_SCALE_VOLTAGE;
            }
            break;
        }
        case LTC7841_IIO_IIN_CHAN:
        {
            ret = ltc7841_reg_read(ltc7841, &ltc7841_regs[LTC7841_READ_IIN_ORDER]);
            if (0 == ret)
            {
				double current_value;
                temp = ltc7841_regs[LTC7841_READ_IIN_ORDER].value;
				/* Note: this is using DC 2798 rsense resistance value*/
				current_value = (double) temp;
				current_value *= CURRENT_SCALE;
				current_value /= RSENSE_VALUE_IIN;
				value[0] = (int32_t)(current_value);
				current_value -= value[0];
				value[1] = (int32_t)current_value * DECI;
            }
            break;
        }
        case LTC7841_IIO_IOUT_CHAN:
        {
            ret = ltc7841_reg_read(ltc7841, &ltc7841_regs[LTC7841_READ_IOUT_ORDER]);
            if (0 == ret)
            {
				double current_value;
                temp = ltc7841_regs[LTC7841_READ_IOUT_ORDER].value;
				/* Note: this is using DC 2798 rsense resistance value*/
				current_value = (double) temp;
				current_value *= CURRENT_SCALE;
				current_value /= RSENSE_VALUE_IIN;
				value[0] = (int32_t)(current_value);
				current_value -= value[0];
				value[1] = (int32_t)current_value * DECI;
            }
            break;
        }
        case LTC7841_IIO_TEMP_CHAN:
            ret = ltc7841_reg_read(ltc7841, &ltc7841_regs[LTC7841_READ_TEMPERATURE_1_ORDER]);
            if (0 == ret)
            {
                temp = ltc7841_regs[LTC7841_READ_TEMPERATURE_1_ORDER].value;
				value[0] = (int32_t)(temp);
				value[1] = 0;
            }
            break;
        default:
            return -EINVAL;
	}

	if (ret)
    {
		return ret;
    }

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 1, value);
}

/**
 * @brief Handles the read request for status debug attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7841_iio_read_status(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
    struct ltc7841_iio_desc * iio_ltc7841 = dev;
    struct ltc7841_desc *ltc7841 = iio_ltc7841->ltc7841_desc;
    int ret;
    int32_t val;

    ret = ltc7841_reg_read(ltc7841, &ltc7841_regs[LTC7841_STATUS_WORD_ORDER]);

    if (ret)
    {
        return ret;
    }
    else
    {
        val = (int32_t) ltc7841_regs[LTC7841_STATUS_WORD_ORDER].value;
    }

    return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the write request for vout-related attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7841_iio_write_vout(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ltc7841_iio_desc *iio_ltc7841 = dev;
	struct ltc7841_desc *ltc7841 = iio_ltc7841->ltc7841_desc;
	int32_t val1, val2;
	double temp_value;
    uint16_t vout_value;
    struct ltc7841_reg_st ltc7841_reg;

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val1, &val2);

    temp_value = (double)(val1 * (int32_t)MILLI + (val2 / MILLI)); /* value is now in millivolts */
	temp_value = temp_value * CENTI / LTC7841_MAX_VOUT; /* for integer scaling*/
	temp_value = temp_value * MFR_MAX_VALUE ; 
	vout_value = (uint16_t)(temp_value / CENTI); /* final percentage value to be written to register, scaling done on the last part*/
    return ltc7841_change_vout_command(ltc7841, vout_value);
}

/**
 * @brief Handles the write request for vout-related attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7841_iio_clear_status(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ltc7841_iio_desc *iio_ltc7841 = dev;
	struct ltc7841_desc *ltc7841 = iio_ltc7841->ltc7841_desc;
    struct ltc7841_reg_st ltc7841_reg;
    return ltc7841_clear_status_word_bits(ltc7841);
}

/**
 * @brief Handles the read request for vout-related attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7841_iio_read_vout(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ltc7841_iio_desc *iio_ltc7841 = dev;
	struct ltc7841_desc *ltc7841 = iio_ltc7841->ltc7841_desc;
	int ret, temp;
	int32_t vals[2];

	if (!dev)
    {
		return -EINVAL;
    }

	if (!iio_ltc7841->ltc7841_desc)
    {
		return -EINVAL;
    }

	ret = ltc7841_reg_read(ltc7841, &ltc7841_regs[LTC7841_READ_VOUT_ORDER]);
	if (ret)
    {
		return ret;
    }
    else
    {
        temp = ltc7841_regs[LTC7841_READ_VOUT_ORDER].value;
    }

    vals[0] = (int32_t)(temp / CENTI);
	vals[1] = (int32_t)(temp % CENTI) * DECIMAL_POINT_SCALE_VOLTAGE;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 1, vals);
}

/**
 * @brief Handles the read request for vout-related attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7841_iio_read_vout_margin_high(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ltc7841_iio_desc *iio_ltc7841 = dev;
	struct ltc7841_desc *ltc7841 = iio_ltc7841->ltc7841_desc;
	int ret, temp;
	int32_t vals[2];

	if (!dev)
    {
		return -EINVAL;
    }

	if (!iio_ltc7841->ltc7841_desc)
    {
		return -EINVAL;
    }

	ret = ltc7841_reg_read(ltc7841, &ltc7841_regs[LTC7841_MFR_VOUT_MARGIN_HIGH_ORDER]);
	if (ret)
    {
		return ret;
    }
    else
    {
        temp = ltc7841_regs[LTC7841_MFR_VOUT_MARGIN_HIGH_ORDER].value;
    }

    vals[0] = (int32_t)(temp / CENTI);
	vals[1] = (int32_t)(temp % CENTI) * DECIMAL_POINT_SCALE_VOLTAGE;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 1, vals);
}

/**
 * @brief Handles the read request for vout-related attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7841_iio_read_vout_margin_low(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ltc7841_iio_desc *iio_ltc7841 = dev;
	struct ltc7841_desc *ltc7841 = iio_ltc7841->ltc7841_desc;
	int ret, temp;
	int32_t vals[2];

	if (!dev)
    {
		return -EINVAL;
    }

	if (!iio_ltc7841->ltc7841_desc)
    {
		return -EINVAL;
    }

	ret = ltc7841_reg_read(ltc7841, &ltc7841_regs[LTC7841_MFR_VOUT_MARGIN_LOW_ORDER]);
	if (ret)
    {
		return ret;
    }
    else
    {
        temp = ltc7841_regs[LTC7841_MFR_VOUT_MARGIN_LOW_ORDER].value;
    }

    vals[0] = (int32_t)(temp / CENTI);
	vals[1] = (int32_t)(temp % CENTI) * DECIMAL_POINT_SCALE_VOLTAGE;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 1, vals);
}

static struct iio_attribute ltc7841_chan_attrs[] = 
{
	{
		.name = "raw",
		.show = ltc7841_iio_read_raw
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ltc7841_channels[] = {
	{
		.name = "vout",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 0,
		.address = LTC7841_IIO_VOUT_CHAN,
		.attributes = ltc7841_chan_attrs,
		.ch_out = true
	},
	{
		.name = "iout",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = 0,
		.address = LTC7841_IIO_IOUT_CHAN,
		.attributes = ltc7841_chan_attrs,
		.ch_out = true
	},
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 0,
		.address = LTC7841_IIO_VIN_CHAN,
		.attributes = ltc7841_chan_attrs,
		.ch_out = false
	},
	{
		.name = "iin",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = 0,
		.address = LTC7841_IIO_IIN_CHAN,
		.attributes = ltc7841_chan_attrs,
		.ch_out = false
	},
	{
		.name = "temperature",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = 0,
		.address = LTC7841_IIO_TEMP_CHAN,
		.attributes = ltc7841_chan_attrs,
		.ch_out = false
	}
};

static struct iio_attribute ltc7841_debug_attrs[] =
{
    {
		.name = "status_word",
		.show = ltc7841_iio_read_status,
		.store = ltc7841_iio_clear_status,
		.priv = LTC7841_STATUS_WORD
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltc7841_global_attrs[] = 
{
    {
		.name = "vout_command",
		.show = ltc7841_iio_read_vout,
		.store = ltc7841_iio_write_vout,
		.priv = LTC7841_MFR_VOUT_COMMAND
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device ltc7841_iio_dev = 
{
	.num_ch = NO_OS_ARRAY_SIZE(ltc7841_channels),
	.channels = ltc7841_channels,
	.attributes = ltc7841_global_attrs,
	.debug_attributes = ltc7841_debug_attrs,
	.debug_reg_read = ltc7841_iio_reg_read,
	.debug_reg_write = ltc7841_iio_reg_write,
};