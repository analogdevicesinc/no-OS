/***************************************************************************//**
 *   @file   iio_ad559xr.c
 *   @brief  Implementation of AD5592R/AD5593R IIO driver.
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 ******************************************************************************/
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_list.h"
#include "iio_ad559xr.h"
#include "ad5592r.h"
#include "ad5593r.h"

static char *pdown_modes[] = {
	"disabled",
	"enabled",
	0
};

static struct scan_type aio_scan_type = {
	.sign = 'u',
	.realbits = AD559XR_ADC_RESOLUTION,
	.storagebits = AD559XR_STORAGE_BITS,
	.shift = 0,
	.is_big_endian = false,
};

/**
 * @brief get the scale factor of ad559xr device depending on the configured
 *        VREF
 *
 * @param[in] device pointer to #iio_ad559xr_desc
 * @param[in] vref_range voltage refeference being used by the ad559xr device
 *
 * @return 0 fail
 * @return length of buffer used.
 */
static double get_scale_factor(struct iio_ad559xr_desc *desc,
			       enum ad559xr_range vref_range)
{
	double vref;

	if (!desc)
		return 0.0f;

	vref = desc->vref;

	if (vref_range == ZERO_TO_2VREF) /* check if VRef x 2 */
		vref *= 2;

	return (vref / (1 << AD559XR_ADC_RESOLUTION)) * 1000;
}

/**
 * @brief Callback function for IIO attribute reading
 *
 * @param[in] device pointer to #iio_ad559xr_desc
 * @param[in] buf buffer to use when filling up information
 * @param[in] len total length of \p buf
 * @param[in] channel channel information
 * @param[in] priv attribute descriptor
 *
 * @return <1 fail
 * @return length of buffer used.
 */
static int ad559xr_iio_show(void *device, char *buf, uint32_t len,
			    const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int val;
	uint16_t adc_value;
	double scale_factor;
	struct ad5592r_dev *ad5592r;
	struct ad559xr_aio_port *aio_port;
	struct iio_ad559xr_desc *desc = device;

	aio_port = (struct ad559xr_aio_port *)channel->address;
	ad5592r = aio_port->ad5592r;
	switch (priv) {
	case AD559XR_AIO_RAW:
		if (aio_port->mode == CH_MODE_DAC)
			adc_value = ad5592r->cached_dac[aio_port->number];
		else {
			ret = ad5592r->ops->read_adc(ad5592r,
						     aio_port->number,
						     &adc_value);
			if (ret)
				return ret;
		}
		adc_value = AD5592R_REG_ADC_SEQ_CODE_MSK(adc_value);

		return sprintf(buf, "%d", adc_value);

	case AD559XR_AIO_OFFSET:
		return sprintf(buf, "%d", 0);

	case AD559XR_AIO_SCALE:
		if (aio_port->mode == CH_MODE_ADC)
			scale_factor = get_scale_factor(device, ad5592r->adc_range);
		else if (aio_port->mode == CH_MODE_DAC)
			scale_factor = get_scale_factor(device, ad5592r->dac_range);
		else
			return -EINVAL;

		return sprintf(buf, "%.8f", scale_factor);

	case AD559XR_GPIO_LABEL:
		return sprintf(buf, "%s", aio_port->name);

	case AD559XR_GPIO_RAW:
		if (aio_port->mode == CH_MODE_GPI)
			adc_value = ad5592r_gpio_get(ad5592r, aio_port->number);
		else if (aio_port->mode == CH_MODE_GPO)
			adc_value = ad5592r->cached_gpo[aio_port->number];
		else
			break;

		return sprintf(buf, "%d", adc_value);

	case AD559XR_DAC_POWER_DOWN:
		return sprintf(buf, "%s", pdown_modes[ad5592r->power_down[aio_port->number]]);

	case AD559XR_TEMP_RAW:
		ret = ad5592r->ops->read_adc(ad5592r, 8, &adc_value);
		if (ret)
			return ret;

		return sprintf(buf, "%d", adc_value & 0xFFF);

	case AD559XR_TEMP_SCALE:
		scale_factor = (desc->vref * 1000) * AD559XR_SLOPE_CONSTANT;
		scale_factor = scale_factor / AD559XR_SLOPE_NANO_SCALE; /* convert to NANO */
		return sprintf(buf, "%f", scale_factor);

	case AD559XR_TEMP_OFFSET:
		/* values here are lifted off of linux drivers/iio/dac/ad5592r-base.c */
		if (ad5592r->adc_range == ZERO_TO_VREF)
			val = (-75365 * 25) / (INTERNAL_VREF_VOLTAGE * 1000);
		else {
			assert(ad5592r->adc_range == ZERO_TO_2VREF);
			val = (-34365 * 25) / (desc->vref * 1000);
		}
		return sprintf(buf, "%d", val);
	}

	return 0;
}

/**
 * @brief Callback function for IIO available attribute reading
 *
 * @param[in] device pointer to #iio_ad559xr_desc
 * @param[in] buf buffer to use when filling up information
 * @param[in] len total length of \p buf
 * @param[in] channel channel information
 * @param[in] priv attribute descriptor
 *
 * @return -EINVAL fail
 * @return length of buffer used.
 */
static int ad559xr_iio_avail_show(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	switch (priv) {
	case AD559XR_DAC_POWER_DOWN:
		return sprintf(buf, "%s %s", pdown_modes[0], pdown_modes[1]);
	}

	return -EINVAL;
}

/**
 * @brief Callback function for IIO attribute updates
 *
 * @param[in] device pointer to #iio_ad559xr_desc
 * @param[in] buf buffer to use when filling up information
 * @param[in] len total length of \p buf
 * @param[in] channel channel information
 * @param[in] priv attribute descriptor
 *
 * @return negative value fail
 * @return length of buffer used.
 */
static int ad559xr_iio_store(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint16_t dac_value;
	uint16_t gpio_value;
	struct ad559xr_aio_port *aio_port;

	aio_port = (struct ad559xr_aio_port *)channel->address;
	switch (priv) {
	case AD559XR_AIO_RAW:
		if (aio_port->mode == CH_MODE_DAC) {
			dac_value = no_os_str_to_uint32(buf);
			if (dac_value > AD559XR_FULL_SCALE_VALUE)
				dac_value = AD559XR_FULL_SCALE_VALUE;

			ret = aio_port->ad5592r->ops->write_dac(aio_port->ad5592r,
								aio_port->number,
								dac_value);
			if (ret)
				return ret;

		}
		break;
	case AD559XR_AIO_SCALE:
	case AD559XR_AIO_OFFSET:
		break;
	case AD559XR_GPIO_RAW:
		if (aio_port->mode == CH_MODE_GPO) {
			gpio_value = no_os_str_to_uint32(buf);
			ret = ad5592r_gpio_set(aio_port->ad5592r,
					       aio_port->number,
					       gpio_value);
			if (ret)
				return ret;

			aio_port->ad5592r->cached_gpo[aio_port->number] = gpio_value;
		}
		break;
	}

	return len;
}

#define DEFINE_AD559XR_ATTR(_name, _priv) {\
	.name = _name, \
	.priv = _priv, \
	.show = ad559xr_iio_show, \
	.store = ad559xr_iio_store, \
}

#define DEFINE_AD559XR_AVAIL_ATTR(_name, _priv) {\
	.name = _name, \
	.priv = _priv, \
	.show = ad559xr_iio_avail_show, \
}

#define DEFINE_AD559XR_GPI_ATTR(_name, _priv) {\
	.name = _name, \
	.priv = _priv, \
	.show = ad559xr_iio_show, \
}

#define DEFINE_AD559XR_GPO_ATTR(_name, _priv) {\
	.name = _name, \
	.priv = _priv, \
	.show = ad559xr_iio_show, \
	.store = ad559xr_iio_store, \
}

static struct iio_attribute ad559xr_adc_attributes[] = {
	DEFINE_AD559XR_ATTR("raw", AD559XR_AIO_RAW),
	DEFINE_AD559XR_ATTR("scale", AD559XR_AIO_SCALE),
	DEFINE_AD559XR_ATTR("offset", AD559XR_AIO_OFFSET),
	END_ATTRIBUTES_ARRAY,
};

static struct iio_attribute ad559xr_temp_attributes[] = {
	DEFINE_AD559XR_ATTR("raw", AD559XR_TEMP_RAW),
	DEFINE_AD559XR_ATTR("scale", AD559XR_TEMP_SCALE),
	DEFINE_AD559XR_ATTR("offset", AD559XR_TEMP_OFFSET),
	END_ATTRIBUTES_ARRAY,
};

static struct iio_attribute ad559xr_dac_attributes[] = {
	DEFINE_AD559XR_ATTR("raw", AD559XR_AIO_RAW),
	DEFINE_AD559XR_ATTR("scale", AD559XR_AIO_SCALE),
	DEFINE_AD559XR_ATTR("offset", AD559XR_AIO_OFFSET),
	DEFINE_AD559XR_ATTR("power_down", AD559XR_DAC_POWER_DOWN),
	DEFINE_AD559XR_AVAIL_ATTR("power_down_available", AD559XR_DAC_POWER_DOWN),
	END_ATTRIBUTES_ARRAY,
};

static struct iio_attribute ad559xr_gpi_attributes[] = {
	DEFINE_AD559XR_GPI_ATTR("label", AD559XR_GPIO_LABEL),
	DEFINE_AD559XR_GPI_ATTR("raw", AD559XR_GPIO_RAW),
	END_ATTRIBUTES_ARRAY,
};

static struct iio_attribute ad559xr_gpo_attributes[] = {
	DEFINE_AD559XR_GPI_ATTR("label", AD559XR_GPIO_LABEL),
	DEFINE_AD559XR_GPO_ATTR("raw", AD559XR_GPIO_RAW),
	END_ATTRIBUTES_ARRAY,
};

/**
 * @brief Add a generic channel information to an ad559xr iio descriptor.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc
 * @param[in] ad5592r device descriptor pointer to #ad5592r_dev
 * @param[in] name device channel name
 * @param[in] number the ad559xr device channel number
 * @param[in] gpio_map this is only relevant when channel is a GPIO.
 * @param[in] mode the type of channel information this is.
 *
 * @return 0 on success
 * @return -EINVAL when desc, ad5592r or name are NULL
 */
static int new_ad559xr_add_port(struct iio_ad559xr_desc *desc,
				struct ad5592r_dev *ad5592r,
				char *name,
				const uint8_t number,
				const uint8_t gpio_map, /* only relevant for GPIO */
				const uint8_t mode)
{
	struct ad559xr_aio_port *port;
	int err = -ENOMEM;

	if (!desc)
		return -EINVAL;
	if (!ad5592r)
		return -EINVAL;
	if (!name)
		return -EINVAL;

	port = no_os_calloc(1, sizeof * port);
	if (port) {
		snprintf(port->name, AD559XR_MAX_PORT_NAMELEN, "%s%d", name, number);
		port->mode = mode;
		port->number = number;
		port->ad5592r = ad5592r;
		port->gpio_map = gpio_map;

		err = no_os_list_add_last(desc->aio_ports, port);
		if (err)
			no_os_free(port);
	}
	return err;
}

/**
 * @brief Add ADC channel information to an ad559xr iio descriptor.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc
 * @param[in] ad5592r device descriptor pointer to #ad5592r_dev
 * @param[in] port ad559xr device channel idx of this port.
 *
 * @return 0 on success
 */
static int iio_ad559xr_add_adc(struct iio_ad559xr_desc *desc,
			       struct ad5592r_dev *ad5592r,
			       const uint8_t port)
{
	return new_ad559xr_add_port(desc, ad5592r, "adc", port, 0, CH_MODE_ADC);
}

/**
 * @brief Add DAC channel information to an ad559xr iio descriptor.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc
 * @param[in] ad5592r device descriptor pointer to #ad5592r_dev
 * @param[in] port ad559xr device channel idx of this port.
 *
 * @return 0 on success
 */
static int iio_ad559xr_add_dac(struct iio_ad559xr_desc *desc,
			       struct ad5592r_dev *ad5592r,
			       const uint8_t port)
{
	return new_ad559xr_add_port(desc, ad5592r, "dac", port, 0, CH_MODE_DAC);
}

/**
 * @brief Add GPIO channel information to an ad559xr iio descriptor.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc
 * @param[in] ad5592r device descriptor pointer to #ad5592r_dev
 * @param[in] label_prefix prefix to add to name of this channel.
 * @param[in] port ad559xr device channel idx of this port.
 * @param[in] gpio_map pointer to #gpio_map
 * @param[in] input flag if this is a GPO or a GPI.
 *
 * @return 0 on success
 * @return -EINVAL when desc, ad5592r, label_prefix, and gpio_map are all NULL
 */
static int iio_ad559xr_add_gpio(struct iio_ad559xr_desc *desc,
				struct ad5592r_dev *ad5592r,
				char *label_prefix,
				const uint8_t port,
				struct iio_ad559xr_gpio_map *gpio_map,
				const bool input)
{
	uint8_t mode;
	char label[AD559XR_MAX_PORT_NAMELEN];

	if (!desc)
		return -EINVAL;
	if (!ad5592r)
		return -EINVAL;
	if (!label_prefix)
		return -EINVAL;
	if (!gpio_map)
		return -EINVAL;

	if (!input) {
		mode = CH_MODE_GPO;
		sprintf(label, "%s_GPO_CH_", label_prefix);
	} else {
		mode = CH_MODE_GPI;
		sprintf(label, "%s_GPI_CH_", label_prefix);
	}

	return new_ad559xr_add_port(desc, ad5592r, label, port, gpio_map->map[port],
				    mode);
}

/**
 * @brief Initialize the linked list resources used in an ad559xr iio
 *        descriptor.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc
 *
 * @return 0 on success
 * @return -EINVAL when desc is NULL
 */
static int ad559xr_list_init(struct iio_ad559xr_desc *desc)
{
	int err;

	if (!desc)
		return -EINVAL;

	err = no_os_list_init(&desc->aio_ports, NO_OS_LIST_DEFAULT, NULL);
	if (err)
		return err;

	err = no_os_iterator_init(&desc->aio_ports_iter, desc->aio_ports, 0);
	if (err)
		no_os_list_remove(desc->aio_ports);

	return err;
}

/**
 * @brief Release linked list resources used in an ad559xr iio descriptor.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc
 *
 * @return 0 on success
 * @return -EINVAL when desc is NULL
 */
static int ad559xr_list_free(struct iio_ad559xr_desc *desc)
{
	int err;

	if (!desc)
		return -EINVAL;

	err = no_os_list_remove(desc->aio_ports);
	if (err)
		return err;

	return no_os_iterator_remove(desc->aio_ports_iter);
}

/**
 * @brief Initialize one scan line in a buffer to be submitted to host via IIO.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc
 * @param[in] mask the active channel mask
 * @param[in] scan_line buffer pointer to use
 */
static void iio_ad559xr_one_scan(struct iio_ad559xr_desc *desc,
				 uint32_t mask, uint8_t *scan_line)
{
	size_t i;
	uint16_t value;
	int err;
	struct ad559xr_aio_port *aio_port;

	for (i = 0; i < NO_OS_BITS_PER_LONG; i++) {
		uint16_t *ptr = (uint16_t *)scan_line;
		uint32_t walking_ones = 1 << i;
		struct ad5592r_dev *ad559xr;
		if ((mask & walking_ones) != walking_ones)
			continue;

		/* when we reach here, this AIO port is active */
		no_os_list_read_idx(desc->aio_ports, (void **)&aio_port, i);

		ad559xr = aio_port->ad5592r;
		if (aio_port->mode == CH_MODE_ADC) {
			err = ad559xr->ops->read_adc(ad559xr,
						     aio_port->number,
						     &value);
			if (err)
				value = 0;

			value = AD5592R_REG_ADC_SEQ_CODE_MSK(value);
			*ptr = value;
			scan_line += 2;
		}
	}

	return;
}

/**
 * @brief Support buffered streaming data for all ADC channels.
 *
 * @param[in] dev pointer to #iio_device_data passed by the IIO subsystem.
 *
 * @return 0 on success
 */
static int32_t iio_ad559xr_submit(struct iio_device_data *dev)
{
	size_t scan;
	struct iio_ad559xr_desc *desc;
	uint8_t *bufptr;

	/* if the requested sample size is more than we support */
	if (dev->buffer->samples > AD5599XR_MAX_SAMPLE_SIZE)
		return -EIO;

	/* also get the iio channel for the aio port, so we can determine the
	 * scan index */
	desc = dev->dev;
	bufptr = desc->raw_buffer;
	for (scan = 0; scan < dev->buffer->samples; scan++) {
		iio_ad559xr_one_scan(desc, dev->buffer->active_mask, bufptr);
		bufptr += dev->buffer->bytes_per_scan;
	}
	no_os_cb_write(dev->buffer->buf, desc->raw_buffer, dev->buffer->size);

	return 0;
}

/**
 * @brief Initialiazes the iio channel array.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc ad559xr iio descriptor
 * @param[in] analog flag to tell which channels to initialize, the analog
 *            channels adc-dac or the digital channels gpi-gpo.
 *
 * @return 0 on success
 * @return -EINVAL when \p desc is NULL.
 * @return -ENOMEM when memory allocation failed.
 * @return count of channels that were initialized.
 */
static int populate_iio_channels(struct iio_ad559xr_desc *desc, bool analog)
{
	int err;
	size_t i;
	uint32_t port_count;
	struct iio_channel *iio_chptr;
	struct ad559xr_aio_port *aio_port;

	if (!desc)
		return -EINVAL;

	err = no_os_list_get_size(desc->aio_ports, &port_count);
	if (err)
		return err;

	iio_chptr = no_os_calloc(port_count, sizeof(struct iio_channel));
	if (!iio_chptr)
		return -ENOMEM;

	desc->channels = iio_chptr;

	for (i = 0; i < port_count; i++) {
		no_os_list_read_idx(desc->aio_ports, (void **)&aio_port, i);
		iio_chptr->ch_type = IIO_VOLTAGE;
		iio_chptr->address = (unsigned long)aio_port;
		iio_chptr->scan_index = i;
		iio_chptr->indexed = true;
		aio_port->chptr = iio_chptr;
		if (analog) {
			iio_chptr->channel = aio_port->number;
			if (aio_port->mode == CH_MODE_ADC) {
				iio_chptr->ch_out = false;

				/* handle the special temperature channel */
				if (aio_port->number < 8) {
					iio_chptr->scan_type = &aio_scan_type;
					iio_chptr->attributes = ad559xr_adc_attributes;
				} else {
					iio_chptr->ch_type = IIO_TEMP;
					iio_chptr->indexed = false;
					iio_chptr->attributes = ad559xr_temp_attributes;
				}
			} else if (aio_port->mode == CH_MODE_DAC) {
				iio_chptr->ch_out = true;
				iio_chptr->scan_type = &aio_scan_type;
				iio_chptr->attributes = ad559xr_dac_attributes;
			}
		} else {
			iio_chptr->channel = aio_port->gpio_map;
			if (aio_port->mode == CH_MODE_GPI) {
				iio_chptr->ch_out = false;
				iio_chptr->attributes = ad559xr_gpi_attributes;
			} else if (aio_port->mode == CH_MODE_GPO) {
				iio_chptr->ch_out = true;
				iio_chptr->attributes = ad559xr_gpo_attributes;
			}
		}
		iio_chptr++;
	}

	return i;
}

/**
 * @brief Initialize an ad559xr iio descriptor for all ADC and DAC channels for
 *        one ad559xr device.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc ad559xr iio descriptor
 * @param[in] ad5592r pointer to #ad5592r_dev
 *
 * @return 0 on success
 * @return -EINVAL when \p desc is NULL.
 * @return -ENODEV when ad5592r is NULL.
 * @return -ENOMEM when memory cannot be allocated.
 */
int iio_ad559xr_aio_init(struct iio_ad559xr_desc **desc,
			 struct ad5592r_dev *ad5592r)
{
	int err;
	size_t i;
	struct iio_ad559xr_desc *ldesc;
	size_t scan_size;

	if (!desc)
		return -EINVAL;
	if (!ad5592r)
		return -ENODEV;

	ldesc = no_os_calloc(1, sizeof * ldesc);
	if (!ldesc)
		return -ENOMEM;

	/* create IIO channel per ad559xr AIO port channel */
	err = ad559xr_list_init(ldesc);
	if (err)
		goto err_free_ldesc;

	for (i = 0; i < AD559XR_MAX_AIO_PORT; i++) {
		if (ad5592r->channel_modes[i] == CH_MODE_ADC)
			err = iio_ad559xr_add_adc(ldesc, ad5592r, i);
		else if (ad5592r->channel_modes[i] == CH_MODE_DAC)
			err = iio_ad559xr_add_dac(ldesc, ad5592r, i);
		else if (ad5592r->channel_modes[i] == CH_MODE_DAC_AND_ADC) {
			err = iio_ad559xr_add_adc(ldesc, ad5592r, i);
			if (err)
				goto err_free_list;
			err = iio_ad559xr_add_dac(ldesc, ad5592r, i);
		}

		if (err)
			goto err_free_list;
	}

	/* we also throw in a "special" temperature channel along with the
	 * ADC/DAC. datasheet lists this as channel 8 */
	err = iio_ad559xr_add_adc(ldesc, ad5592r, 8);
	if (err)
		goto err_free_list;

	err = populate_iio_channels(ldesc, true);
	if (err < 1)
		goto err_free_list;

	ldesc->port_count = err;

	scan_size = (ldesc->port_count * AD559XR_STORAGE_BYTES)
		    * AD5599XR_MAX_SAMPLE_SIZE;
	ldesc->raw_buffer = no_os_malloc(scan_size);
	if (!ldesc->raw_buffer)
		goto err_free_channels;
	ldesc->raw_buffer_size = scan_size;

	ldesc->iiodev.num_ch = ldesc->port_count;
	ldesc->iiodev.channels = ldesc->channels;
	ldesc->iiodev.submit = iio_ad559xr_submit;

	if (ad5592r->int_ref == true)
		ldesc->vref = AD559XR_INTERNAL_VREF;
	else
		ldesc->vref = ldesc->external_vref;
	ldesc->repeat = false;

	*desc = ldesc;

	return 0;
err_free_channels:
	no_os_free(ldesc->channels);
err_free_list:
	ad559xr_list_free(ldesc);
err_free_ldesc:
	no_os_free(ldesc);
	return err;
}

/**
 * @brief Initialize an ad559xr iio descriptor for all GPIO channels.
 *
 * This function takes an array called a #gpio_map. It collects all the
 * channels defined in all ad559xr devices declared in the gpio_map and
 * creates one single ad559xr iio descriptor for all.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc ad559xr iio descriptor
 * @param[in] gpio_map pointer to #gpio_map
 *
 * @return 0 on success
 * @return -EINVAL when \p desc and \p gpio_map are both NULL.
 * @return -ENODEV when gpio_map contains an NULL ad559xr device descriptor.
 */
int iio_ad559xr_gpio_init(struct iio_ad559xr_desc **desc,
			  struct iio_ad559xr_gpio_map *gpio_map,
			  const size_t count)
{
	size_t i;
	size_t dev_idx;
	int err;
	struct iio_ad559xr_desc *ldesc;
	size_t scan_size;
	char *label_prefix;

	if (!desc)
		return -EINVAL;
	if (!gpio_map)
		return -EINVAL;

	ldesc = no_os_calloc(1, sizeof * ldesc);
	if (!ldesc)
		return -ENOMEM;

	/* create IIO channel per ad559xr GPIO port channel per ad559x */
	err = ad559xr_list_init(ldesc);
	if (err)
		goto err_free_ldesc;

	for (i = 0; i < AD559XR_MAX_AIO_PORT; i++) {
		for (dev_idx = 0; dev_idx < count; dev_idx++) {
			struct ad5592r_dev *ad559xr = gpio_map[dev_idx].ad559xr;

			if (!ad559xr) {
				err = -ENODEV;
				goto err_free_list;
			}

			if (ad559xr->spi)
				label_prefix = "AD5592R";
			else if (ad559xr->i2c)
				label_prefix = "AD5593R";
			else {
				/* only ad5592r and ad5593r are valid
				 * devices here. */
				err = -ENODEV;
				goto err_free_list;
			}

			if (ad559xr->channel_modes[i] == CH_MODE_GPI)
				err = iio_ad559xr_add_gpio(ldesc, ad559xr, label_prefix, i, &gpio_map[dev_idx],
							   true);
			else if (ad559xr->channel_modes[i] == CH_MODE_GPO)
				err = iio_ad559xr_add_gpio(ldesc, ad559xr, label_prefix, i, &gpio_map[dev_idx],
							   false);

			if (err)
				goto err_free_list;
		}
	}

	err = populate_iio_channels(ldesc, false);
	if (err < 1)
		goto err_free_list;

	/* this is only the count for the GPIO pins */
	ldesc->port_count = scan_size = err;
	ldesc->iiodev.num_ch = ldesc->port_count;
	ldesc->iiodev.channels = ldesc->channels;
	ldesc->iiodev.submit = iio_ad559xr_submit;

	*desc = ldesc;

	return 0;
err_free_list:
	ad559xr_list_free(ldesc);
err_free_ldesc:
	no_os_free(ldesc);
	return err;
}

/**
 * @brief Remove an ad559xr iio descriptor
 *
 * Releases memory associated with an ad559xr iio descriptor and also frees up
 * the iio descriptor memory itself.
 *
 * @param[in] desc pointer to #iio_ad559xr_desc ad559xr iio descriptor
 */
int iio_ad559xr_remove(struct iio_ad559xr_desc *desc)
{
	int err;

	if (!desc)
		return -EINVAL;
	if (desc->channels)
		no_os_free(desc->channels);
	if (desc->raw_buffer)
		no_os_free(desc->raw_buffer);

	err = ad559xr_list_free(desc);
	if (err)
		return err;

	no_os_free(desc);

	return 0;
}
