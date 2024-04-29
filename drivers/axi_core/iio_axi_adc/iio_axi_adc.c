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

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
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
static int get_calibphase(void *device, char *buf, uint32_t len,
			  const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val, val2;
	int32_t i = 0;
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;
	int ret = axi_adc_get_calib_phase(iio_adc->adc, channel->ch_num, &val,
					  &val2);
	if (ret < 0)
		return ret;

	if (val2 < 0 && val >= 0) {
		snprintf(buf, len, "-");
		i++;
	}

	return i + snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val, abs(val2));
}

/**
 * @brief get_cf_calibbias().
 * @param device - Physical instance of a iio_axi_adc_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_calibbias(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;
	int ret;
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
static int get_calibscale(void *device, char *buf, uint32_t len,
			  const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val, val2;
	int32_t i = 0;
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;
	int ret = axi_adc_get_calib_scale(iio_adc->adc, channel->ch_num, &val,
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
			   abs(val2));

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
static int get_samples_pps(void *device, char *buf, uint32_t len,
			   const struct iio_ch_info *channel, intptr_t priv)
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
static int get_sampling_frequency(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	uint64_t sampling_freq_hz;
	int ret;
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
static int set_calibphase(void *device, char *buf, uint32_t len,
			  const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
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
static int set_calibbias(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val = no_os_str_to_int32(buf);
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;
	int ret;

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
static int set_calibscale(void *device, char *buf, uint32_t len,
			  const struct iio_ch_info *channel, intptr_t priv)
{
	float calib= strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	struct iio_axi_adc_desc *iio_adc = (struct iio_axi_adc_desc *)device;
	int ret = axi_adc_set_calib_scale(iio_adc->adc, channel->ch_num, val,
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
static int set_samples_pps(void *device, char *buf, uint32_t len,
			   const struct iio_ch_info *channel, intptr_t priv)
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
static int set_sampling_frequency(void *device, char *buf, uint32_t len,
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
 * @return 0 in case of success or negative value otherwise.
 */
int32_t	iio_axi_adc_read_dev(void *dev, void *buff, uint32_t nb_samples)
{
	struct iio_axi_adc_desc *iio_adc;
	int ret, bytes;

	if (!dev)
		return -1;

	iio_adc = (struct iio_axi_adc_desc *)dev;
	bytes = nb_samples * no_os_hweight32(iio_adc->mask) *
		(iio_adc->scan_type_common->storagebits / 8);

	struct axi_dma_transfer transfer = {
		// Number of bytes to writen/read
		.size = bytes,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)buff
	};
	ret = axi_dmac_transfer_start(iio_adc->dmac, &transfer);
	if (ret < 0)
		return ret;
	/* Wait until transfer finishes */
	ret = axi_dmac_transfer_wait_completion(iio_adc->dmac, 500);
	if(ret)
		return ret;

	if (iio_adc->dcache_invalidate_range)
		iio_adc->dcache_invalidate_range((uintptr_t)buff, bytes);

	return 0;
}

/**
 * @brief Delete iio_device.
 * @param iio_device - Structure describing a device, channels and attributes.
 * @return 0 in case of success or negative value otherwise.
 */
static int iio_axi_adc_delete_device_descriptor(
	struct iio_axi_adc_desc *desc)
{
	if (!desc)
		return -1;

	if (desc->dev_descriptor.channels)
		no_os_free(desc->dev_descriptor.channels);

	if (desc->ch_names)
		no_os_free(desc->ch_names);

	return 0;
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
	static struct iio_channel default_channel = {
		.ch_type = IIO_VOLTAGE,
		.attributes = iio_voltage_attributes,
		.ch_out = false,
		.indexed = true,
	};
	int32_t i;
	int32_t ret;

	default_channel.scan_type = desc->scan_type_common;

	if (!desc->dmac)
		default_channel.scan_type = NULL;

	iio_device->num_ch = desc->adc->num_channels;
	iio_device->attributes = NULL; /* no device attribute */
	iio_device->channels = no_os_calloc(iio_device->num_ch,
					    sizeof(struct iio_channel));
	if (!iio_device->channels)
		goto error;

	desc->ch_names = no_os_calloc(iio_device->num_ch, sizeof(*desc->ch_names));
	if (!desc->ch_names)
		goto error;

	for (i = 0; i < iio_device->num_ch; i++) {
		default_channel.channel = i;
		iio_device->channels[i] = default_channel;
		iio_device->channels[i].name = desc->ch_names[i];
		iio_device->channels[i].scan_index = i;
		ret = sprintf(desc->ch_names[i], "voltage%"PRIi32"", i);
		if (ret < 0)
			goto error;
	}

	iio_device->pre_enable = iio_axi_adc_prepare_transfer;
	iio_device->read_dev = iio_axi_adc_read_dev;

	return 0;
error:
	iio_axi_adc_delete_device_descriptor(desc);

	return -1;
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
 * @return 0 in case of success, -1 otherwise.
 */
int32_t iio_axi_adc_init(struct iio_axi_adc_desc **desc,
			 struct iio_axi_adc_init_param *init)
{
	struct iio_axi_adc_desc *iio_axi_adc_inst;
	int32_t status;

	static struct scan_type scan_type = {
		.sign = 's',
		.realbits = STORAGE_BITS,
		.storagebits = STORAGE_BITS,
		.shift = 0,
		.is_big_endian = false
	};

	if (!init)
		return -1;

	if (!init->rx_adc)
		return -1;

	iio_axi_adc_inst = (struct iio_axi_adc_desc *)no_os_calloc(1,
			   sizeof(struct iio_axi_adc_desc));
	if (!iio_axi_adc_inst)
		return -1;

	iio_axi_adc_inst->adc = init->rx_adc;
	if (init->rx_dmac) {
		iio_axi_adc_inst->dmac = init->rx_dmac;
		iio_axi_adc_inst->dcache_invalidate_range = init->dcache_invalidate_range;
	}
	iio_axi_adc_inst->get_sampling_frequency = init->get_sampling_frequency;

	if (init->scan_type_common)
		iio_axi_adc_inst->scan_type_common = init->scan_type_common;
	else
		iio_axi_adc_inst->scan_type_common = &scan_type;

	status = iio_axi_adc_create_device_descriptor(iio_axi_adc_inst,
			&iio_axi_adc_inst->dev_descriptor);
	if (NO_OS_IS_ERR_VALUE(status)) {
		no_os_free(iio_axi_adc_inst);
		return status;
	}

	*desc = iio_axi_adc_inst;

	return 0;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t iio_axi_adc_remove(struct iio_axi_adc_desc *desc)
{
	int32_t status;

	if (!desc)
		return -1;

	status = iio_axi_adc_delete_device_descriptor(desc);
	if (status < 0)
		return status;

	no_os_free(desc);

	return 0;
}
