/***************************************************************************//**
 *   @file   iio_axi_adc.c
 *   @brief  Implementation of iio_axi_adc.
 *   This instantiates iio_axi_adc and iio_axi_dac devices, for
 *   reading/writing and parameterization.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include <inttypes.h>
#include <stdlib.h>
#include "error.h"
#include "iio.h"
#include "iio_axi_adc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

#define STORAGE_BITS 16

/**
 * @brief get_cf_calibphase().
 * @param device - Physical instance of a iio_axi_adc_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static ssize_t get_calibphase(void *device, char *buf, size_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	int32_t val, val2;
	int32_t i = 0;
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;
	ssize_t ret = axi_adc_get_calib_phase(iio_adc->adc, channel->ch_num, &val,
					      &val2);
	if (ret < 0)
		return ret;

	if (val2 < 0 && val >= 0) {
		snprintf(buf, len, "-");
		i++;
	}

	return i + snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val, labs(val2));
}

/**
 * @brief get_cf_calibbias().
 * @param device - Physical instance of a iio_axi_adc_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static ssize_t get_calibbias(void *device, char *buf, size_t len,
			     const struct iio_ch_info *channel,
			     intptr_t priv)
{
	int32_t val;
	ssize_t ret;
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;

	ret = axi_adc_get_calib_bias(iio_adc->adc,
				     channel->ch_num,
				     &val,
				     NULL);
	if (ret < 0)
		return ret;

	return snprintf(buf, len, "%"PRIi32"", val);
}

/**
 * @brief get_cf_calibscale().
 * @param device - Physical instance of a iio_axi_adc_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static ssize_t get_calibscale(void *device, char *buf, size_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	int32_t val, val2;
	int32_t i = 0;
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;
	ssize_t ret = axi_adc_get_calib_scale(iio_adc->adc, channel->ch_num, &val,
					      &val2);
	if (ret < 0)
		return ret;

	if (val2 < 0 && val >= 0) {
		ret = snprintf(buf, len, "-");
		if (ret < 0)
			return ret;
		i++;
	}
	ret = i + snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val,
			   labs(val2));

	return ret;
}

/**
 * @brief get_cf_samples_pps().
 * @param device - Physical instance of a iio_axi_adc_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return: Length of chars written in buf, or negative value on failure.
 */
static ssize_t get_samples_pps(void *device, char *buf, size_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	/* This function doesn't have an equivalent function in axi_adc_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief get_cf_sampling_frequency().
 * @param device - Physical instance of a iio_axi_adc_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static ssize_t get_sampling_frequency(void *device, char *buf, size_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	uint64_t sampling_freq_hz;
	ssize_t ret;
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;

	if (iio_adc->get_sampling_frequency)
		ret = iio_adc->get_sampling_frequency(iio_adc->adc, channel->ch_num,
						      &sampling_freq_hz);
	else
		ret = axi_adc_get_sampling_freq(iio_adc->adc, channel->ch_num,
						&sampling_freq_hz);
	if (ret < 0)
		return ret;

	return snprintf(buf, len, "%"PRIi64"", sampling_freq_hz);
}

/**
 * @brief set_cf_calibphase().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_calibphase(void *device, char *buf, size_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	ssize_t ret;
	float calib = strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib * 1000000) % 1000000;
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;

	ret = axi_adc_set_calib_phase(iio_adc->adc, channel->ch_num, val, val2);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_cf_calibbias().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_calibbias(void *device, char *buf, size_t len,
			     const struct iio_ch_info *channel,
			     intptr_t priv)
{
	int32_t val = str_to_int32(buf);
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;
	ssize_t ret;

	ret = axi_adc_set_calib_bias(iio_adc->adc,
				     channel->ch_num,
				     val,
				     0);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_cf_calibscale().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_calibscale(void *device, char *buf, size_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	float calib= strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;
	ssize_t ret = axi_adc_set_calib_scale(iio_adc->adc, channel->ch_num, val,
					      val2);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_cf_samples_pps().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_samples_pps(void *device, char *buf, size_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	/* This function doesn't have an equivalent function in axi_adc_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_cf_sampling_frequency().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_sampling_frequency(void *device, char *buf, size_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	/* This function doesn't have an equivalent function in axi_adc_core,
	 * and it should be implemented there first */

	return -ENOENT;
}


/**
 * List containing attributes, corresponding to "voltage" channels.
 */
static struct iio_attribute iio_voltage_attributes[] = {
	{
		.name = "calibphase",
		.show = get_calibphase,
		.store = set_calibphase,
	},
	{
		.name = "calibbias",
		.show = get_calibbias,
		.store = set_calibbias,
	},
	{
		.name = "calibscale",
		.show = get_calibscale,
		.store = set_calibscale,
	},
	{
		.name = "samples_pps",
		.show = get_samples_pps,
		.store = set_samples_pps,
	},
	{
		.name = "sampling_frequency",
		.show = get_sampling_frequency,
		.store = set_sampling_frequency,
	},
	END_ATTRIBUTES_ARRAY
};

/**
 * @brief Update active channels
 * @param dev - Instance of the iio_axi_adc
 * @param mask - Mask with new channels to activate
 * @return axi_adc_update_active_channels result.
 */
int32_t iio_axi_adc_prepare_transfer(void *dev, uint32_t mask)
{
	struct iio_axi_adc_desc *iio_adc = dev;

	iio_adc->mask = mask;

	return axi_adc_update_active_channels(iio_adc->adc, mask);
}

/**
 * @brief Update active channels
 * @param dev - Instance of the iio_axi_adc
 * @param buff - Buffer where to read samples
 * @param nb_samples - Number of samples
 * @return SUCCESS in case of success or negative value otherwise.
 */
int32_t	iio_axi_adc_read_dev(void *dev, void *buff, uint32_t nb_samples)
{
	struct iio_axi_adc_desc *iio_adc;
	ssize_t ret, bytes;

	if (!dev)
		return FAILURE;

	iio_adc = (struct iio_axi_adc_desc *)dev;
	bytes = nb_samples * hweight8(iio_adc->mask) * (STORAGE_BITS / 8);

	iio_adc->dmac->flags = 0;
	ret = axi_dmac_transfer(iio_adc->dmac, (uint32_t)buff, bytes);
	if (ret < 0)
		return ret;

	if (iio_adc->dcache_invalidate_range)
		iio_adc->dcache_invalidate_range((uint32_t)buff, bytes);

	return SUCCESS;
}

/**
 * @brief Delete iio_device.
 * @param iio_device - Structure describing a device, channels and attributes.
 * @return SUCCESS in case of success or negative value otherwise.
 */
static ssize_t iio_axi_adc_delete_device_descriptor(
	struct iio_axi_adc_desc *desc)
{
	if (!desc)
		return FAILURE;

	if (desc->dev_descriptor.channels)
		free(desc->dev_descriptor.channels);

	if (desc->ch_names)
		free(desc->ch_names);

	return SUCCESS;
}

/**
 * @brief Create structure describing a device, channels
 * and attributes.
 * @param adc - axi adc.
 * @param iio_device - iio device.
 * @return iio_device or NULL, in case of failure.
 */
static int32_t iio_axi_adc_create_device_descriptor(
	struct iio_axi_adc_desc *desc, struct iio_device *iio_device)
{
	static struct scan_type scan_type = {
		.sign = 's',
		.realbits = STORAGE_BITS,
		.storagebits = STORAGE_BITS,
		.shift = 0,
		.is_big_endian = false
	};

	static struct iio_channel default_channel = {
		.ch_type = IIO_VOLTAGE,
		.scan_type =  &scan_type,
		.attributes = iio_voltage_attributes,
		.ch_out = false,
		.indexed = true,
	};
	int32_t i;
	int32_t ret;

	iio_device->num_ch = desc->adc->num_channels;
	iio_device->attributes = NULL; /* no device attribute */
	iio_device->channels = calloc(iio_device->num_ch,
				      sizeof(struct iio_channel));
	if (!iio_device->channels)
		goto error;

	desc->ch_names = calloc(iio_device->num_ch, sizeof(*desc->ch_names));
	if (!desc->ch_names)
		goto error;

	for (i = 0; i < iio_device->num_ch; i++) {
		default_channel.channel = i;
		iio_device->channels[i] = default_channel;
		iio_device->channels[i].name = desc->ch_names[i];
		iio_device->channels[i].scan_index = i;
		ret = sprintf(iio_device->channels[i].name, "voltage%"PRIi32"", i);
		if (ret < 0)
			goto error;
	}

	iio_device->prepare_transfer = iio_axi_adc_prepare_transfer;
	iio_device->read_dev = iio_axi_adc_read_dev;

	return SUCCESS;
error:
	iio_axi_adc_delete_device_descriptor(desc);

	return FAILURE;
}

/**
 * @brief Get device descriptor.
 * @param desc - axi iio axi adc descriptor.
 * @param dev_descriptor - iio device.
 * @return None.
 */
void iio_axi_adc_get_dev_descriptor(struct iio_axi_adc_desc *desc,
				    struct iio_device **dev_descriptor)
{
	*dev_descriptor = &desc->dev_descriptor;
}

/**
 * @brief Registers a iio_axi_adc_desc for reading/writing and parameterization of
 * axi_adc device.
 * @param  desc - Descriptor.
 * @param init - Configuration structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_axi_adc_init(struct iio_axi_adc_desc **desc,
			 struct iio_axi_adc_init_param *init)
{
	struct iio_axi_adc_desc *iio_axi_adc_inst;

	int32_t status;

	if (!init)
		return FAILURE;

	if (!init->rx_adc || !init->rx_dmac)
		return FAILURE;

	iio_axi_adc_inst = (struct iio_axi_adc_desc *)calloc(1,
			   sizeof(struct iio_axi_adc_desc));
	if (!iio_axi_adc_inst)
		return FAILURE;

	iio_axi_adc_inst->adc = init->rx_adc;
	iio_axi_adc_inst->dmac = init->rx_dmac;
	iio_axi_adc_inst->dcache_invalidate_range = init->dcache_invalidate_range;
	iio_axi_adc_inst->get_sampling_frequency = init->get_sampling_frequency;

	status = iio_axi_adc_create_device_descriptor(iio_axi_adc_inst,
			&iio_axi_adc_inst->dev_descriptor);
	if (IS_ERR_VALUE(status)) {
		free(iio_axi_adc_inst);
		return status;
	}

	*desc = iio_axi_adc_inst;

	return SUCCESS;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_axi_adc_remove(struct iio_axi_adc_desc *desc)
{
	int32_t status;

	if (!desc)
		return FAILURE;

	status = iio_axi_adc_delete_device_descriptor(desc);
	if (status < 0)
		return status;

	free(desc);

	return SUCCESS;
}
