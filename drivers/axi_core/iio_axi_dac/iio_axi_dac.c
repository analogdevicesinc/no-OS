/***************************************************************************//**
 *   @file   iio_axi_dac.c
 *   @brief  Implementation of iio_axi_dac.
 *   Instantiates iio_axi_adc and iio_axi_dac devices, for
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
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "iio.h"
#include "iio_axi_dac.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

#define STORAGE_BITS 16

/**
 * @brief get_dds_calibscale().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_voltage_calibscale(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int32_t val, val2;
	struct iio_axi_dac_desc *iio_dac = (struct iio_axi_dac_desc*)device;
	int ret = axi_dac_dds_get_calib_scale(iio_dac->dac, channel->ch_num,
					      &val, &val2);
	int32_t i = 0;
	if(ret < 0)
		return ret;
	if(val2 < 0 && val >= 0) {
		ret = (int) snprintf(buf, len, "-");
		if(ret < 0)
			return ret;
		i++;
	}
	ret = i + (int) snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val,
				 abs(val2));

	return ret;
}

/**
 * @brief get_dds_calibphase().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static int get_voltage_calibphase(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int32_t val, val2;
	int32_t i = 0;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	int ret = axi_dac_dds_get_calib_phase(iio_dac->dac, channel->ch_num,
					      &val, &val2);
	if(ret < 0)
		return ret;
	if(val2 < 0 && val >= 0) {
		i++;
	}

	return i + snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val, abs(val2));
}

/**
 * @brief get_dds_sampling_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static int get_voltage_sampling_frequency(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	/* This function doesn't have an equivalent function in axi_dac_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * get_dds_altvoltage_phase().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static int get_altvoltage_phase(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	uint32_t phase;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	int ret = axi_dac_dds_get_phase(iio_dac->dac, channel->ch_num, &phase);
	if (ret < 0)
		return ret;

	return snprintf(buf, len, "%"PRIu32"", phase);
}

/**
 * @brief get_dds_altvoltage_scale().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static int get_altvoltage_scale(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	int32_t scale;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	int ret = axi_dac_dds_get_scale(iio_dac->dac, channel->ch_num, &scale);
	if (ret < 0)
		return ret;

	return snprintf(buf, len, "%"PRIi32".%.6"PRIi32"", (scale / 1000000),
			(scale % 1000000));
}

/**
 * @brief get_dds_altvoltage_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static int get_altvoltage_frequency(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	uint32_t freq;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	int ret = axi_dac_dds_get_frequency(iio_dac->dac, channel->ch_num, &freq);
	if (ret < 0)
		return ret;

	return snprintf(buf, len, "%"PRIi32"", freq);
}

/**
 * @brief get_dds_altvoltage_raw().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static int get_altvoltage_raw(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	/* This function doesn't have an equivalent function in axi_dac_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief get_dds_altvoltage_sampling_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static int get_altvoltage_sampling_frequency(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	/* This function doesn't have an equivalent function in axi_dac_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_dds_calibscale().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static int set_voltage_calibscale(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	float calib= strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	int ret = axi_dac_dds_set_calib_scale(iio_dac->dac, channel->ch_num, val,
					      val2);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_calibphase().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_voltage_calibphase(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	float calib = strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	int ret = axi_dac_dds_set_calib_phase(iio_dac->dac, channel->ch_num, val,
					      val2);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_sampling_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
static int set_voltage_sampling_frequency(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	/* This function doesn't have an equivalent function in axi_dac_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_dds_altvoltage_phase().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_altvoltage_phase(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	uint32_t phase = no_os_str_to_uint32(buf);
	struct iio_axi_dac_desc * iio_dac = (struct iio_axi_dac_desc *)device;
	int ret = axi_dac_dds_set_phase(iio_dac->dac, channel->ch_num, phase);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_altvoltage_scale().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_altvoltage_scale(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	float fscale = strtof(buf, NULL);
	int32_t scale = fscale * 1000000;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	int ret = axi_dac_dds_set_scale(iio_dac->dac, channel->ch_num, scale);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_altvoltage_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_altvoltage_frequency(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	uint32_t freq = no_os_str_to_uint32(buf);
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	int ret = axi_dac_dds_set_frequency(iio_dac->dac, channel->ch_num, freq);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_altvoltage_raw().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
static int set_altvoltage_raw(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	uint32_t dds_mode = no_os_str_to_uint32(buf);
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	int ret;
	ret = axi_dac_set_datasel(iio_dac->dac, -1,
				  dds_mode ? AXI_DAC_DATA_SEL_DDS : AXI_DAC_DATA_SEL_DMA);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_altvoltage_sampling_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_altvoltage_sampling_frequency(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	/* This function doesn't have an equivalent function in axi_dac_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * List containing attributes, corresponding to "voltage" channels.
 */
static struct iio_attribute iio_voltage_attributes[] = {
	{
		.name = "calibscale",
		.show = get_voltage_calibscale,
		.store = set_voltage_calibscale,
	},
	{
		.name = "calibphase",
		.show = get_voltage_calibphase,
		.store = set_voltage_calibphase,
	},
	{
		.name = "sampling_frequency",
		.show = get_voltage_sampling_frequency,
		.store = set_voltage_sampling_frequency,
	},
	END_ATTRIBUTES_ARRAY
};

/**
 * List containing attributes, corresponding to "altvoltage" channels.
 */
static struct iio_attribute iio_altvoltage_attributes[] = {
	{
		.name = "raw",
		.show = get_altvoltage_raw,
		.store = set_altvoltage_raw,
	},
	{
		.name = "phase",
		.show = get_altvoltage_phase,
		.store = set_altvoltage_phase,
	},
	{
		.name = "scale",
		.show = get_altvoltage_scale,
		.store = set_altvoltage_scale,
	},
	{
		.name = "frequency",
		.show = get_altvoltage_frequency,
		.store = set_altvoltage_frequency,
	},
	{
		.name = "sampling_frequency",
		.show = get_altvoltage_sampling_frequency,
		.store = set_altvoltage_sampling_frequency,
	},
	END_ATTRIBUTES_ARRAY,
};

/**
 * @brief Update active channels
 * @param dev - Instance of the iio_axi_dac
 * @param mask - Mask with new channels to activate
 * @return axi_dac_update_active_channels result.
 */
int32_t iio_axi_dac_prepare_transfer(void *dev, uint32_t mask)
{
	struct iio_axi_dac_desc *iio_dac = dev;
	uint16_t i;
	int32_t	ret;

	iio_dac->mask = mask;

	for (i = 0; i < iio_dac->dev_descriptor.num_ch; i++) {
		if (NO_OS_BIT(i) & mask)
			ret = axi_dac_set_datasel(iio_dac->dac, i, AXI_DAC_DATA_SEL_DMA);
		else
			ret = axi_dac_set_datasel(iio_dac->dac, i, AXI_DAC_DATA_SEL_DDS);
		if(ret < 0)
			return ret;
	}

	iio_dac->mask = mask;

	return 0;
}

/**
 * @brief Update active channels
 * @param dev - Instance of the iio_axi_dac
 * @param buff - Buffer where to read samples
 * @param nb_samples - Number of samples
 * @return 0 in case of success or negative value otherwise.
 */
int32_t iio_axi_dac_write_data(void *dev, void *buff, uint32_t nb_samples)
{
	struct iio_axi_dac_desc *iio_dac;
	int bytes;

	if (!dev)
		return -1;

	iio_dac = (struct iio_axi_dac_desc *)dev;
	bytes = nb_samples * no_os_hweight32(iio_dac->mask) * (STORAGE_BITS / 8);

	if(iio_dac->dcache_flush_range)
		iio_dac->dcache_flush_range((uintptr_t)buff, bytes);

	struct axi_dma_transfer transfer = {
		// Number of bytes to writen/read
		.size = bytes,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = CYCLIC,
		// Address of data source
		.src_addr = (uintptr_t)buff,
		// Address of data destination
		.dest_addr = 0
	};

	return axi_dmac_transfer_start(iio_dac->dmac, &transfer);
}

enum ch_type {
	CH_VOLTGE,
	CH_ALTVOLTGE,
};

/**
 * @brief Delete iio_device.
 * @param iio_device - Structure describing a device, channels and attributes.
 * @return 0 in case of success or negative value otherwise.
 */
static int iio_axi_dac_delete_device_descriptor(
	struct iio_axi_dac_desc *desc)
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
 * @param dac - Axi dac.
 * @param iio_device - iio device.
 * @return iio_device or NULL, in case of failure.
 */
static int32_t iio_axi_dac_create_device_descriptor(
	struct iio_axi_dac_desc *desc, struct iio_device *iio_device)
{
	static struct scan_type scan_type = {
		.sign = 's',
		.realbits = STORAGE_BITS,
		.storagebits = STORAGE_BITS,
		.shift = 0,
		.is_big_endian = false
	};

	static struct iio_channel default_voltage_channel = {
		.ch_type = IIO_VOLTAGE,
		.scan_type = &scan_type,
		.attributes = iio_voltage_attributes,
		.ch_out = true,
		.indexed = true,
	};

	static struct iio_channel default_altvoltage_channel = {
		.ch_type = IIO_ALTVOLTAGE,
		.scan_type = NULL,
		.attributes = iio_altvoltage_attributes,
		.ch_out = true,
		.indexed = true,
	};

	int32_t i, j, altvoltage_ch, voltage_ch_no, altvoltage_ch_no;
	int32_t ret;
	char ch;

	if (!desc->dmac)
		default_voltage_channel.scan_type = NULL;

	voltage_ch_no = desc->dac->num_channels;
	altvoltage_ch_no = desc->dac->num_channels * 2;
	iio_device->num_ch = voltage_ch_no + altvoltage_ch_no;
	iio_device->attributes = NULL; /* no device attribute */
	iio_device->channels = no_os_calloc(iio_device->num_ch,
					    sizeof(struct iio_channel));
	if (!iio_device->channels)
		goto error;

	desc->ch_names = no_os_calloc(iio_device->num_ch, sizeof(*desc->ch_names));
	if (!desc->ch_names)
		goto error;

	for (i = 0; i < voltage_ch_no; i++) {
		default_voltage_channel.channel = i;
		iio_device->channels[i] = default_voltage_channel;
		iio_device->channels[i].scan_index = i;
		iio_device->channels[i].name = desc->ch_names[i];
		ret = sprintf(desc->ch_names[i], "voltage%"PRIi32"", i);
		if (ret < 0)
			goto error;
	}

	for (i = voltage_ch_no; i < voltage_ch_no + altvoltage_ch_no; i++) {
		altvoltage_ch = i - voltage_ch_no;
		default_altvoltage_channel.channel = altvoltage_ch;
		iio_device->channels[i] = default_altvoltage_channel;
		iio_device->channels[i].scan_index = altvoltage_ch;
		iio_device->channels[i].name = desc->ch_names[i];

		ch = 'Q';
		if (altvoltage_ch % 4 == 0 || altvoltage_ch % 4 == 1)
			ch = 'I';
		j = ((altvoltage_ch % 4) % 2) + 1;
		ret = sprintf(desc->ch_names[i], "TX%"PRIi32"_%c_F%"PRIi32"",
			      altvoltage_ch / 4 + 1, ch, j);
		if (ret < 0)
			goto error;
	}
	iio_device->pre_enable = iio_axi_dac_prepare_transfer;
	iio_device->write_dev = iio_axi_dac_write_data;

	return 0;

error:
	iio_axi_dac_delete_device_descriptor(desc);

	return -1;
}

/**
 * @brief Create structure describing a device, channels
 * and attributes.
 * @param desc - Descriptor.
 * @param dev_descriptor - device descriptor.
 * @return None.
 */
void iio_axi_dac_get_dev_descriptor(struct iio_axi_dac_desc *desc,
				    struct iio_device **dev_descriptor)
{
	*dev_descriptor = &desc->dev_descriptor;
}

/**
 * @brief Registers a iio_axi_dac_desc for reading/writing and parameterization
 * of axi_dac device.
 * @param desc - Descriptor.
 * @param init - Configuration structure.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t iio_axi_dac_init(struct iio_axi_dac_desc **desc,
			 struct iio_axi_dac_init_param *init)
{
	struct iio_axi_dac_desc *iio_axi_dac_inst;
	int32_t status;

	if (!init)
		return -1;

	if (!init->tx_dac)
		return -1;

	iio_axi_dac_inst = (struct iio_axi_dac_desc *)no_os_calloc(1,
			   sizeof(struct iio_axi_dac_desc));
	if (!iio_axi_dac_inst)
		return -1;

	iio_axi_dac_inst->dac = init->tx_dac;
	if (init->tx_dmac) {
		iio_axi_dac_inst->dmac = init->tx_dmac;
		iio_axi_dac_inst->dcache_flush_range = init->dcache_flush_range;
	}

	status = iio_axi_dac_create_device_descriptor(iio_axi_dac_inst,
			&iio_axi_dac_inst->dev_descriptor);
	if (NO_OS_IS_ERR_VALUE(status)) {
		no_os_free(iio_axi_dac_inst);
		return status;
	}

	*desc = iio_axi_dac_inst;

	return 0;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t iio_axi_dac_remove(struct iio_axi_dac_desc *desc)
{
	int32_t status;

	if (!desc)
		return -1;

	status = iio_axi_dac_delete_device_descriptor(desc);
	if (status < 0)
		return status;

	no_os_free(desc);

	return 0;
}
