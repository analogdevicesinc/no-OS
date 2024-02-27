/***************************************************************************//**
 *   @file   iio_ad463x.c
 *   @brief  Implementation of iio_ad463x.c.
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
#ifdef IIO_SUPPORT

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <string.h>
#include "ad463x.h"
#include "iio.h"
#include "iio_ad463x.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int ad463x_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int ad463x_iio_read_scale(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int ad463x_iio_store_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int ad463x_iio_read_scale_avail(void *dev, char *buf,
				       uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int32_t _iio_ad463x_read_dev(struct iio_ad463x *desc, uint32_t *buff,
				    uint32_t nb_samples);
static int32_t _iio_ad463x_prepare_transfer(struct iio_ad463x *desc,
		uint32_t mask);
/******************************************************************************/
/*************************** Variable Declarations *******************************/
/******************************************************************************/

#define BITS_PER_SAMPLE 32
#define REAL_BITS 24

static struct scan_type adc_scan_type = {
	.sign = 's',
	.realbits = REAL_BITS,
	.storagebits = BITS_PER_SAMPLE,
	.shift = 0,
	.is_big_endian = false
};

static struct iio_attribute ad463x_iio_attrs[] = {
	{
		.name = "raw",
		.show = ad463x_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ad463x_iio_read_scale,
		.store = ad463x_iio_store_scale,
	},
	{
		.name = "scale_available",
		.show = ad463x_iio_read_scale_avail,
	},
	END_ATTRIBUTES_ARRAY
};

#define IIO_AD463x_CHANNEL(_idx) {\
	.name = "ch" # _idx,\
	.ch_type = IIO_VOLTAGE,\
	.channel = _idx,\
	.scan_index = _idx,\
	.indexed = true,\
	.attributes = ad463x_iio_attrs,\
	.scan_type = &adc_scan_type,\
	.ch_out = false,\
}

static struct iio_channel iio_adc_two_channels[] = {
	IIO_AD463x_CHANNEL(0),
	IIO_AD463x_CHANNEL(1),
};

static struct iio_channel iio_adc_one_channel[] = {
	IIO_AD463x_CHANNEL(0)
};

struct iio_device ad463x_iio_desc_one_chn = {
	.channels = iio_adc_one_channel,
	.num_ch = 1,
	.pre_enable = (int32_t (*)())_iio_ad463x_prepare_transfer,
	.read_dev = (int32_t (*)())_iio_ad463x_read_dev
};

struct iio_device ad463x_iio_desc_two_chn = {
	.channels = iio_adc_two_channels,
	.num_ch = 2,
	.pre_enable = (int32_t (*)())_iio_ad463x_prepare_transfer,
	.read_dev = (int32_t (*)())_iio_ad463x_read_dev
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Updates the number of active channels
 * @param dev  - The iio device structure.
 * @param mask - Mask of the active channels
 * @return ret - Result of the updating procedure.
*/
static int32_t _iio_ad463x_prepare_transfer(struct iio_ad463x *desc,
		uint32_t mask)
{
	if (!desc)
		return -EINVAL;

	desc->mask = mask;

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
 *		    In case of success, the size of the read data is returned.
*/
static int ad463x_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t temp[2];
	int ret;
	int32_t temp2;
	struct iio_ad463x *iio_desc = dev;

	/* Exit register configuration mode */
	ret = ad463x_exit_reg_cfg_mode(iio_desc->ad463x_desc);
	if (ret != 0)
		return ret;

	ret = ad463x_read_data(iio_desc->ad463x_desc, temp, 2);
	if (ret != 0)
		return ret;
	/** Get sample in the middle according to the channel */
	if(channel->ch_num == 0)
		temp2 = temp[0];
	else
		temp2 = temp[1];
	temp2 = no_os_sign_extend32(temp2, REAL_BITS - 1);

	ad463x_enter_config_mode(iio_desc->ad463x_desc);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &temp2);
}

/**
 * @brief Handles the read request for Scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 *		    In case of success, the size of the read data is returned.
*/
static int ad463x_iio_read_scale(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct iio_ad463x *iio_desc = dev;
	struct ad463x_dev *ad463x_dev;
	int32_t vals[2];

	if (!iio_desc)
		return -EINVAL;

	ad463x_dev = iio_desc->ad463x_desc;
	if(ad463x_dev->has_pgia) {
		vals[0] = ad463x_dev->scale_table[ad463x_dev->pgia_idx][0];
		vals[1] = ad463x_dev->scale_table[ad463x_dev->pgia_idx][1];
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_NANO, 2, vals);
	}

	vals[0] = (ad463x_dev->vref * 2) / 1000;
	vals[1] = ad463x_dev->capture_data_width;
	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
}

/**
 * @brief Handles the write request for scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 *		    In case of success, the size of the read data is returned.
*/
static int ad463x_iio_store_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct iio_ad463x *iio_desc = dev;
	struct ad463x_dev *ad463x_dev;
	int32_t gain_idx;
	int ret;
	int32_t vals[2];

	ad463x_dev = iio_desc->ad463x_desc;
	/** Scale only configurable if PGIA is available */
	if(!ad463x_dev->has_pgia)
		return -EINVAL;
	iio_parse_value(buf, IIO_VAL_FRACTIONAL, &vals[0], &vals[1]);

	ret = ad463x_calc_pgia_gain(vals[0], vals[1], ad463x_dev->vref,
				    ad463x_dev->real_bits_precision, &gain_idx);
	if (ret != 0)
		return ret;
	return ad463x_set_pgia_gain(ad463x_dev, gain_idx);
}

/**
 * @brief Handles the read request for scale available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 *		    In case of success, the size of the read data is returned.
*/
static int ad463x_iio_read_scale_avail(void *dev, char *buf,
				       uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct iio_ad463x *iio_desc = dev;
	struct ad463x_dev *ad463x_dev;
	ad463x_dev = iio_desc->ad463x_desc;
	char *buffer = buf;

	if(ad463x_dev->has_pgia) {
		/** Go through the values in the table and add them to the buffer */
		for (uint8_t i = 0; i < NO_OS_ARRAY_SIZE(ad463x_dev->scale_table); i++) {
			vals[0] = ad463x_dev->scale_table[i][0];
			vals[1] = ad463x_dev->scale_table[i][1];
			buffer += iio_format_value(buffer, len, IIO_VAL_INT_PLUS_NANO, 2, vals) + 1;
			strcat(buf, " ");
		}
		buf[strlen(buf)] = '\0';
		return strlen(buf);
	}

	vals[0] = (ad463x_dev->vref * 2) / 1000;
	vals[1] = ad463x_dev->capture_data_width;
	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
}

/**
 * @brief Reads the number of given samples for the selected channels
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param samples - Number of samples to be returned
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*/
static int32_t _iio_ad463x_read_dev(struct iio_ad463x *desc, uint32_t *buff,
				    uint32_t nb_samples)
{
	int ret;
	uint16_t total_samples = nb_samples * 2;
	uint32_t *data;
	uint32_t i, j, ch;

	if (!desc)
		return -EINVAL;

	data = (uint32_t *)no_os_calloc(total_samples * 2, sizeof(*data));
	if (!data)
		return -ENOMEM;

	/* Exit register configuration mode */
	ret = ad463x_exit_reg_cfg_mode(desc->ad463x_desc);
	if (ret < 0)
		goto error_comm;

	/** Read samples */
	ret = ad463x_read_data(desc->ad463x_desc, data, total_samples * 2);
	if (ret != 0)
		goto error_comm;

	/** Fill IIO Buffer */
	for (i = 0, j = 0; i < total_samples; i++)
		if (desc->mask & NO_OS_BIT(i & 0x01)) {
			buff[j++] = data[i];
		}

	ad463x_enter_config_mode(desc->ad463x_desc);
	free(data);
	return nb_samples;

error_comm:
	free(data);
	return ret;
}

/**
 * @brief Init for reading/writing and parameterization of a ad463x device.
 * @param desc - Descriptor.
 * @param param - Configuration structure.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t iio_ad463x_init(struct iio_ad463x **desc,
			struct ad463x_dev *dev)
{
	struct iio_ad463x *iio_ad463x;

	iio_ad463x = (struct iio_ad463x *)no_os_calloc(1, sizeof(struct iio_ad463x));
	if (!iio_ad463x)
		return -1;

	iio_ad463x->ad463x_desc = dev;

	if (dev->device_id <= ID_AD4632_16)
		iio_ad463x->iio_dev_desc = ad463x_iio_desc_two_chn;
	else
		iio_ad463x->iio_dev_desc = ad463x_iio_desc_one_chn;

	*desc = iio_ad463x;

	return 0;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t iio_ad463x_remove(struct iio_ad463x *desc)
{
	if (!desc)
		return -1;

	no_os_free(desc);

	return 0;
}

#endif /* IIO_SUPPORT */
