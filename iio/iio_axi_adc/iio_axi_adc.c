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

#include "error.h"
#include "iio.h"
#include "iio_axi_adc.h"
#include "xml.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief get_cf_calibphase().
 * @param device - Physical instance of a iio_axi_adc_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static ssize_t get_calibphase(void *device, char *buf, size_t len,
			      const struct iio_ch_info *channel)
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
			     const struct iio_ch_info *channel)
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
			      const struct iio_ch_info *channel)
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
			       const struct iio_ch_info *channel)
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
				      const struct iio_ch_info *channel)
{
	uint64_t sampling_freq_hz;
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;
	ssize_t ret = axi_adc_get_sampling_freq(iio_adc->adc, channel->ch_num,
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
			      const struct iio_ch_info *channel)
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
			     const struct iio_ch_info *channel)
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
			      const struct iio_ch_info *channel)
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
			       const struct iio_ch_info *channel)
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
				      const struct iio_ch_info *channel)
{
	/* This function doesn't have an equivalent function in axi_adc_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @struct iio_attr_calibphase
 * @brief Structure for "calibphase" attribute.
 */
static struct iio_attribute iio_attr_calibphase = {
	/** Attribute name */
	.name = "calibphase",
	/** Read attribute from device */
	.show = get_calibphase,
	/** Write attribute to device */
	.store = set_calibphase,
};

/**
 * @struct iio_attr_calibbias
 * @brief Structure for "calibbias" attribute.
 */
static struct iio_attribute iio_attr_calibbias = {
	/** Attribute name */
	.name = "calibbias",
	/** Read attribute from device */
	.show = get_calibbias,
	/** Write attribute to device */
	.store = set_calibbias,
};

/**
 * @struct iio_attr_calibscale
 * @brief Structure for "calibscale" attribute.
 */
static struct iio_attribute iio_attr_calibscale = {
	/** Attribute name */
	.name = "calibscale",
	/** Read attribute from device */
	.show = get_calibscale,
	/** Write attribute to device */
	.store = set_calibscale,
};

/**
 * @struct iio_attr_samples_pps
 * @brief Structure for "samples_pps" attribute.
 */
static struct iio_attribute iio_attr_samples_pps = {
	/** Attribute name */
	.name = "samples_pps",
	/** Read attribute from device */
	.show = get_samples_pps,
	/** Write attribute to device */
	.store = set_samples_pps,
};

/**
 * @struct iio_attr_sampling_frequency
 * @brief Structure for "sampling_frequency" attribute.
 */
static struct iio_attribute iio_attr_sampling_frequency = {
	/** Attribute name */
	.name = "sampling_frequency",
	/** Read attribute from device */
	.show = get_sampling_frequency,
	/** Write attribute to device */
	.store = set_sampling_frequency,
};

/**
 * List containing attributes, corresponding to "voltage" channels.
 */
static struct iio_attribute *iio_voltage_attributes[] = {
	&iio_attr_calibphase,
	&iio_attr_calibbias,
	&iio_attr_calibscale,
	&iio_attr_samples_pps,
	&iio_attr_sampling_frequency,
	NULL,
};

/**
 * @brief Transfer data from device into RAM.
 * @param iio_inst - Physical instance of a iio_axi_adc_desc device.
 * @param bytes_count - Number of bytes to transfer.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
static ssize_t iio_axi_adc_transfer_dev_to_mem(void *iio_inst,
		size_t bytes_count,
		uint32_t ch_mask)
{
	struct iio_axi_adc_desc *iio_adc;
	ssize_t ret, bytes;

	if (!iio_inst)
		return FAILURE;

	iio_adc = (struct iio_axi_adc_desc *)iio_inst;
	bytes = (bytes_count * iio_adc->adc->num_channels) / hweight8(ch_mask);

	iio_adc->dmac->flags = 0;
	ret = axi_dmac_transfer(iio_adc->dmac,
				iio_adc->adc_ddr_base, bytes);
	if (ret < 0)
		return ret;

	if (iio_adc->dcache_invalidate_range)
		iio_adc->dcache_invalidate_range(iio_adc->adc_ddr_base, bytes);

	return bytes_count;
}

/**
 * @brief Read chunk of data from RAM to pbuf.
 * Call "iio_axi_adc_transfer_dev_to_mem" first.
 * This function is probably called multiple times by libtinyiiod after a
 * "iio_axi_adc_transfer_dev_to_mem" call, since we can only read "bytes_count"
 * bytes at a time.
 * @param iio_inst - Physical instance of a iio_axi_adc_desc device.
 * @param pbuf - Buffer where value is stored.
 * @param offset - Offset to the remaining data after reading n chunks.
 * @param bytes_count - Number of bytes to read.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
static ssize_t iio_axi_adc_read_dev(void *iio_inst, char *pbuf, size_t offset,
				    size_t bytes_count, uint32_t ch_mask)
{
	struct iio_axi_adc_desc *iio_adc;
	uint32_t i, j = 0, current_ch = 0;
	uint16_t *pbuf16;
	size_t samples;

	if (!iio_inst)
		return FAILURE;

	if (!pbuf)
		return FAILURE;

	iio_adc = (struct iio_axi_adc_desc *)iio_inst;
	pbuf16 = (uint16_t*)pbuf;
	samples = (bytes_count * iio_adc->adc->num_channels) / hweight8(
			  ch_mask);

	samples /= 2; /* because of uint16_t *pbuf16 = (uint16_t*)pbuf; */
	offset = (offset * iio_adc->adc->num_channels) / hweight8(ch_mask);

	for (i = 0; i < samples; i++) {

		if (ch_mask & BIT(current_ch)) {
			pbuf16[j] = *(uint16_t*)(iio_adc->adc_ddr_base + offset + i * 2);
			j++;
		}

		if (current_ch + 1 < iio_adc->adc->num_channels)
			current_ch++;
		else
			current_ch = 0;
	}

	return bytes_count;
}

/**
 * @brief Create structure describing a device, channels
 * and attributes.
 * @param device_name - Device name.
 * @param num_ch - Number of channels that the device has.
 * @return iio_device or NULL, in case of failure.
 */
static int32_t iio_axi_adc_create_device_descriptor(
		struct axi_adc *adc, struct iio_device *iio_device)
{
	static struct iio_channel default_channel = {
		.ch_type = IIO_VOLTAGE,
		.scan_type =  (struct scan_type) {
			.sign = 's',
			.realbits = 16,
			.storagebits = 16,
			.shift = 0,
			.is_big_endian = false
		},
		.attributes = iio_voltage_attributes,
		.ch_out = false,
	};
	int32_t i;
	int32_t ret;

	iio_device->num_ch = adc->num_channels;
	iio_device->attributes = NULL; /* no device attribute */
	iio_device->channels = calloc(iio_device->num_ch + 1,
					sizeof(struct iio_channel *));
	if (!iio_device->channels)
		goto error;

	for (i = 0; i < iio_device->num_ch; i++) {
		iio_device->channels[i] = calloc(1, sizeof(struct iio_channel));
		if (!iio_device->channels[i])
			goto error;
		iio_device->channels[i]->name = calloc(1, 5);
		if (!iio_device->channels[i]->name)
			goto error;
		*(iio_device->channels[i]) = default_channel;
		iio_device->channels[i]->scan_index = i;
		ret = sprintf(iio_device->channels[i]->name, "CH%d", i);
		if (ret < 0)
			goto error;
	}
	iio_device->channels[i] = NULL;

	iio_device->transfer_dev_to_mem = iio_axi_adc_transfer_dev_to_mem;
	iio_device->transfer_mem_to_dev = NULL;
	iio_device->read_data = iio_axi_adc_read_dev;
	iio_device->write_data = NULL;

error:
	iio_axi_adc_delete_device_descriptor(iio_device);

	return NULL;
}

/**
 * @brief Delete iio_device.
 * @param iio_device - Structure describing a device, channels and attributes.
 * @return SUCCESS in case of success or negative value otherwise.
 */
static ssize_t iio_axi_adc_delete_device_descriptor(
		struct iio_device *iio_device)
{
	uint16_t i = 0;

	if (!iio_device)
		return FAILURE;

	if (iio_device->channels) {
		while (iio_device->channels[i]) {
			if (iio_device->channels[i]->name)
				free(iio_device->channels[i]->name);
			if (iio_device->channels[i])
				free(iio_device->channels[i]);
			i++;
		}
		free(iio_device->channels);
	}

	return SUCCESS;
}


void iio_axi_adc_get_dev_descriptor(struct iio_axi_adc_desc *desc,
				struct iio_device **dev_descriptor)
{
	*dev_descriptor = desc->dev_descriptor;
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

	iio_axi_adc_inst = (struct iio_axi_adc_desc *)calloc(1, sizeof(struct iio_axi_adc_desc));
	if (!iio_axi_adc_inst)
		return FAILURE;

	iio_axi_adc_inst->adc = init->rx_adc;
	iio_axi_adc_inst->dmac = init->rx_dmac;
	iio_axi_adc_inst->adc_ddr_base = init->adc_ddr_base;
	iio_axi_adc_inst->dcache_invalidate_range = init->dcache_invalidate_range;

	status = iio_axi_adc_create_device_descriptor(iio_axi_adc_inst->adc,
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

	iio_axi_adc_delete_device_descriptor(desc->dev_descriptor);
	free(desc);

	return SUCCESS;
}
