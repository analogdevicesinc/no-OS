/***************************************************************************//**
 *   @file   iio_powrms.c
 *   @brief  Source code file for IIO POWRMS example
 *   @author robert.budai@analog.com
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include "iio_powrms.h"
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "common_data.h"

static int read_ch_attr(void *device, char *buf, uint32_t len,
			const struct iio_ch_info *channel, intptr_t priv);
static int get_input_impedance(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int set_input_impedance(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int get_output_impedance(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int set_output_impedance(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int get_frequency(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv);
static int set_frequency(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv);
static int32_t powrms_submit_samples(struct iio_device_data *dev_data);

#define POWRMS_VOLTAGE_CHANNEL(_idx, _name)                                 \
{                                                                           \
.name = _name, .ch_type = IIO_VOLTAGE, .channel = _idx,                     \
.scan_index = _idx, .indexed = true, .scan_type = &powrms_scan_type,        \
.attributes = powrms_channel_attributes, .ch_out = false                    \
}

struct iio_attribute powrms_channel_attributes[] = {

	{
		.name = "scale",
		.show = read_ch_attr,
		.priv = 0,
	},
	{
		.name = "offset",
		.show = read_ch_attr,
		.priv = 1,
	},
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute powrms_global_attributes[] = {
	{
		.name = "input_impedance",
		.show = get_input_impedance,
		.store = set_input_impedance,
		.priv = 0,
	},
	{
		.name = "output_impedance",
		.show = get_output_impedance,
		.store = set_output_impedance,
		.priv = 1,
	},
	{
		.name = "frequency",
		.show = get_frequency,
		.store = set_frequency,
		.priv = 2,
	},
	END_ATTRIBUTES_ARRAY,
};

struct scan_type powrms_scan_type = {
	.sign = 's',
	.realbits = 32,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false
}; // scan type for 32-bit signed integer values (scaled raw values)

static struct iio_channel iio_powrms_channels[] = {
	POWRMS_VOLTAGE_CHANNEL(0, "vin_0"),
	POWRMS_VOLTAGE_CHANNEL(1, "vin_1"),
	POWRMS_VOLTAGE_CHANNEL(2, "vrmsf"),
	POWRMS_VOLTAGE_CHANNEL(3, "vrmsr"),
}; // channel definitions for device


struct iio_device powrms_iio_descriptor = {
	.num_ch = TOTAL_PWM_CHANNELS,
	.channels = iio_powrms_channels,
	.attributes = powrms_global_attributes,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = NULL,
	.post_disable = NULL,
	.trigger_handler = (int32_t(*)())NULL,
	.submit = (int32_t(*)())powrms_submit_samples,
}; // PowerRMS IIO descriptor

IIO_BUFF_TYPE iio_data_buffer_loc[MAX_SIZE_BASE_ADDR] = {0};

struct no_os_circular_buffer *powrms_vin0_buffer = NULL;
struct no_os_circular_buffer *powrms_vin1_buffer = NULL;
struct no_os_circular_buffer *powrms_vrmsf_buffer = NULL;
struct no_os_circular_buffer *powrms_vrmsr_buffer = NULL;

static uint32_t powrms_input_impedance = 50;
static uint32_t powrms_output_impedance = 50;
static uint32_t powrms_frequency = 1000;

/**
 * @brief Read channel attributes (scale and offset)
 *
 * @param device - IIO device instance
 * @param buf - Buffer to store the attribute value
 * @param len - Buffer length
 * @param channel - Channel information
 * @param priv - Private data (0 = scale, 1 = offset)
 * @return Number of bytes written to buffer, or negative error code
 */
static int read_ch_attr(void *device, char *buf, uint32_t len,
			const struct iio_ch_info *channel, intptr_t priv)
{
	switch (priv) {
	case 0: // scale
		// Scale factor for voltage channels (mV to V conversion)
		// For ADC readings, typically scale = (vref / (2^resolution)) * gain
		// Here we assume 1:1 scale (voltage values are already in volts)
		return snprintf(buf, len, "%.7f", (1.0 / ((double)(BUFFER_VAL_PRECISSION))));

	case 1: // offset
		// Offset for voltage channels (typically 0 for unipolar ADCs)
		return snprintf(buf, len, "0.0");

	default:
		return -EINVAL;
	}
}

static int get_input_impedance(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	return snprintf(buf, len, "%"PRIu32"", powrms_input_impedance);
}

static int get_output_impedance(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	return snprintf(buf, len, "%"PRIu32"", powrms_output_impedance);
}

static int get_frequency(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	return snprintf(buf, len, "%"PRIu32"", powrms_frequency);
}

static int set_input_impedance(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t val;
	if (sscanf(buf, "%"PRIu32"", &val) == 1) {
		powrms_input_impedance = val;
		return len;
	}
	return -1;
}

static int set_output_impedance(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t val;
	if (sscanf(buf, "%"PRIu32"", &val) == 1) {
		powrms_output_impedance = val;
		return len;
	}
	return -1;
}

static int set_frequency(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t val;
	if (sscanf(buf, "%"PRIu32"", &val) == 1) {
		powrms_frequency = val;
		return len;
	}
	return -1;
}

static int32_t powrms_submit_samples(struct iio_device_data *dev_data)
{
	uint32_t ch_idx = 0;
	uint32_t buff_idx = 0;
	int32_t buff[TOTAL_PWM_CHANNELS];
	extern volatile struct adc_data adc_data_input;

	if (!dev_data)
		return -1;

	for (ch_idx = 0; ch_idx < TOTAL_PWM_CHANNELS; ch_idx++) {
		if (dev_data->buffer->active_mask & (1 << ch_idx)) {
			switch (ch_idx) {
			case 0:
				buff[buff_idx++] = (int32_t)(adc_data_input.adc_vin0_voltage *
							     BUFFER_VAL_PRECISSION);
				break;
			case 1:
				buff[buff_idx++] = (int32_t)(adc_data_input.adc_vin1_voltage *
							     BUFFER_VAL_PRECISSION);
				break;
			case 2:
				buff[buff_idx++] = (int32_t)(adc_data_input.adc_vrmsf * BUFFER_VAL_PRECISSION);
				break;
			case 3:
				buff[buff_idx++] = (int32_t)(adc_data_input.adc_vrmsr * BUFFER_VAL_PRECISSION);
				break;
			default:
				buff[buff_idx++] = 0.0f;
				break;
			}
		}
	}

	for (uint32_t i = 0;
	     i < dev_data->buffer->size / dev_data->buffer->bytes_per_scan; i++) {
		iio_buffer_push_scan(dev_data->buffer, buff);
	}

	return dev_data->buffer->size / dev_data->buffer->bytes_per_scan;
}

int powrms_buffers_init(void)
{
	int ret;

	ret = no_os_cb_init(&powrms_vin0_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_vin0;

	ret = no_os_cb_init(&powrms_vin1_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_vin1;

	ret = no_os_cb_init(&powrms_vrmsf_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_vrmsf;

	ret = no_os_cb_init(&powrms_vrmsr_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_vrmsr;

	return 0;

error_vrmsr:
	no_os_cb_remove(powrms_vrmsf_buffer);
error_vrmsf:
	no_os_cb_remove(powrms_vin1_buffer);
error_vin1:
	no_os_cb_remove(powrms_vin0_buffer);
error_vin0:
	return ret;
}

void powrms_buffers_cleanup(void)
{
	if (powrms_vin0_buffer)
		no_os_cb_remove(powrms_vin0_buffer);
	if (powrms_vin1_buffer)
		no_os_cb_remove(powrms_vin1_buffer);
	if (powrms_vrmsf_buffer)
		no_os_cb_remove(powrms_vrmsf_buffer);
	if (powrms_vrmsr_buffer)
		no_os_cb_remove(powrms_vrmsr_buffer);
}

int powrms_push_vin0(float value)
{
	if (!powrms_vin0_buffer)
		return -ENODEV;
	return no_os_cb_write(powrms_vin0_buffer, &value, sizeof(float));
}

int powrms_push_vin1(float value)
{
	if (!powrms_vin1_buffer)
		return -ENODEV;
	return no_os_cb_write(powrms_vin1_buffer, &value, sizeof(float));
}

int powrms_push_vrmsf(float value)
{
	if (!powrms_vrmsf_buffer)
		return -ENODEV;
	return no_os_cb_write(powrms_vrmsf_buffer, &value, sizeof(float));
}

int powrms_push_vrmsr(float value)
{
	if (!powrms_vrmsr_buffer)
		return -ENODEV;
	return no_os_cb_write(powrms_vrmsr_buffer, &value, sizeof(float));
}

int powrms_get_latest_vin0(float *value)
{
	if (!powrms_vin0_buffer || !value)
		return -ENODEV;
	return no_os_cb_read(powrms_vin0_buffer, value, sizeof(float));
}

int powrms_get_latest_vin1(float *value)
{
	if (!powrms_vin1_buffer || !value)
		return -ENODEV;
	return no_os_cb_read(powrms_vin1_buffer, value, sizeof(float));
}

int powrms_get_latest_vrmsf(float *value)
{
	if (!powrms_vrmsf_buffer || !value)
		return -ENODEV;
	return no_os_cb_read(powrms_vrmsf_buffer, value, sizeof(float));
}

int powrms_get_latest_vrmsr(float *value)
{
	if (!powrms_vrmsr_buffer || !value)
		return -ENODEV;
	return no_os_cb_read(powrms_vrmsr_buffer, value, sizeof(float));
}
