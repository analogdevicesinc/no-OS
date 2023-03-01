/***************************************************************************//**
 *   @file   ad7689.c
 *   @brief  Source file for the ad7689 driver
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#include <stdlib.h>
#include "ad7689.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

const char *ad7689_device_name[] = {
	"AD7689",
	"AD7682",
	"AD7949",
	"AD7699",
};

static void _ad7689_config_put(struct ad7689_dev *dev,
			       struct ad7689_config *config)
{
	dev->configs[1] = dev->configs[0];
	if (config)
		dev->configs[0] = *config;
	else
		dev->configs[0] = dev->configs[1];
}

static struct ad7689_config *_ad7689_config_get(struct ad7689_dev *dev)
{
	// dev->configs[1] is the config currently in use. If the current
	// SPI transaction is numbered N, this config was written at N-2
	// and applied at the EOC of N-1.
	return &dev->configs[1];
}

static int32_t _ad7689_rac(struct ad7689_dev *dev,
			   struct ad7689_config *config_in, struct ad7689_config *config_out,
			   uint16_t *data)
{
	int32_t ret;
	uint16_t cfg = 0;
	uint8_t buf[4] = {0, 0, 0, 0};
	uint16_t sz;
	struct ad7689_config *c;

	if (!dev)
		return -EINVAL;

	c = _ad7689_config_get(dev);

	if (config_in) {
		cfg |= no_os_field_prep(AD7689_CFG_CFG_MSK, 1);
		cfg |= no_os_field_prep(AD7689_CFG_INCC_MSK, config_in->incc);
		cfg |= no_os_field_prep(AD7689_CFG_INX_MSK, config_in->inx);
		cfg |= no_os_field_prep(AD7689_CFG_BW_MSK, config_in->bw);
		cfg |= no_os_field_prep(AD7689_CFG_REF_MSK, config_in->ref);
		cfg |= no_os_field_prep(AD7689_CFG_SEQ_MSK, config_in->seq);
		cfg |= no_os_field_prep(AD7689_CFG_RB_MSK, !config_in->rb);
		cfg <<= 2;
		buf[0] = cfg >> 8;
		buf[1] = cfg;
	}
	sz = c->rb && config_out ? 4 : 2;
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, sz);
	if (ret < 0)
		return ret;

	_ad7689_config_put(dev, config_in);

	if (data) {
		// by default, data width is 16-bits
		*data = ((uint16_t)buf[0] << 8) | buf[1];

		// handle 14-bit data width:
		if (dev->id == ID_AD7949) {
			// Bipolar samples are in two's complement, scale down to 14-bit
			// by keeping the sign bit using division instead of implementation
			// specific right shift. This works for unipolar samples too.
			*data = (int16_t)*data / 4;
		}
	}

	if (c->rb && config_out) {
		if (dev->id == ID_AD7949)
			cfg = (((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | buf[3]) >> 2;
		else
			cfg = ((uint16_t)buf[2] << 8) | buf[3];
		cfg >>= 2;
		config_out->incc = no_os_field_get(AD7689_CFG_INCC_MSK, cfg);
		config_out->inx = no_os_field_get(AD7689_CFG_INX_MSK, cfg);
		config_out->bw = no_os_field_get(AD7689_CFG_BW_MSK, cfg);
		config_out->ref = no_os_field_get(AD7689_CFG_REF_MSK, cfg);
		config_out->seq = no_os_field_get(AD7689_CFG_SEQ_MSK, cfg);
		config_out->rb = !(bool)no_os_field_get(AD7689_CFG_RB_MSK, cfg);
	}

	return 0;
}

/***************************************************************************//**
 * @brief Initialize the ad7689 driver and create a descriptor.
 *
 * @param dev - Device descriptor to create.
 * @param init_param - Initialization parameters.
 *
 * @return Returns negative error code or 0 in case of success.
 *         Example: -EINVAL - Bad input parameters.
 *                  -ENOMEM - Failed to allocate memory.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7689_init(struct ad7689_dev **dev,
		    struct ad7689_init_param *init_param)
{
	struct ad7689_dev *d;
	int32_t ret;

	if (init_param->id > ID_AD7699)
		return -EINVAL;

	d = (struct ad7689_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->id = init_param->id;
	d->name = ad7689_device_name[d->id];

	ret = no_os_spi_init(&d->spi_desc, &init_param->spi_init);
	if (ret < 0)
		goto error_spi;

	ret = ad7689_write_config(d, &init_param->config);
	if (ret < 0)
		goto error_init;

	// perform one extra dummy conversion (2 are needed after power-up)
	ret = _ad7689_rac(d, NULL, NULL, NULL);
	if (ret < 0)
		goto error_init;

	*dev = d;

	return 0;
error_init:
	no_os_spi_remove(d->spi_desc);
error_spi:
	no_os_free(d);
	pr_err("%s initialization failed with status %d\n", d->name,
	       ret);

	return ret;
}

/***************************************************************************//**
 * @brief Write the device's CFG register.
 *
 * @param dev - Device descriptor.
 * @param config - Configuration to write.
 *
 * @return Returns negative error code or 0 in case of success.
 *         Example: -EINVAL - Bad input parameters.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7689_write_config(struct ad7689_dev *dev,
			    struct ad7689_config *config)
{
	return _ad7689_rac(dev, config, NULL, NULL);
}

/***************************************************************************//**
 * @brief Read the device's CFG register.
 *
 * If the readback is enabled when making this call, one SPI transaction
 * is enough to retrieve the CFG register.
 *
 * If the readback is disabled when making this call, it is temporarily
 * enabled, then disabled back. 3 SPI transactions are needed to retrieve
 * the CFG register.
 *
 * @param dev - Device descriptor.
 * @param config - pointer to location where the read configuration gets stored.
 *
 * @return Returns negative error code or 0 in case of success.
 *         Example: -EINVAL - Bad input parameters.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7689_read_config(struct ad7689_dev *dev, struct ad7689_config *config)
{
	int32_t ret = 0;
	struct ad7689_config *c = _ad7689_config_get(dev);
	if (c->rb == true)
		return _ad7689_rac(dev, NULL, config, NULL);

	struct ad7689_config c_in = *c;
	c_in.rb = true;
	ret = _ad7689_rac(dev, &c_in, NULL, NULL);
	if (ret < 0)
		return ret;

	c_in.rb = false;
	ret = _ad7689_rac(dev, &c_in, NULL, NULL);
	if (ret < 0)
		return ret;

	return _ad7689_rac(dev, NULL, config, NULL);
}

/***************************************************************************//**
 * @brief Read ADC samples.
 *
 * This function uses the RAC mode to perform the SPI transactions.
 *
 * @param dev - Device descriptor.
 * @param data - pointer to a large enough buffer where the data gets stored.
 * @param nb_samples - Number of samples to read.
 *
 * @return Returns negative error code or 0 in case of success.
 *         Example: -EINVAL - Bad input parameters.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7689_read(struct ad7689_dev *dev, uint16_t *data, uint32_t nb_samples)
{
	int32_t ret;
	uint32_t i = 0;

	if (!data)
		return -EINVAL;
	if (!nb_samples)
		return 0;

	do {
		ret = _ad7689_rac(dev, NULL, NULL, &data[i]);
		if (ret < 0)
			return ret;
		i++;
	} while (i < nb_samples);

	return 0;
}

/***************************************************************************//**
 * @brief Remove the driver's descriptor by freeing the associated resources.
 *
 * @param dev - Device descriptor.
 *
 * @return Returns negative error code or 0 in case of success.
 *         Example: -EINVAL - Bad input parameters.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7689_remove(struct ad7689_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_spi_remove(dev->spi_desc);
	no_os_free(dev);

	return 0;
}
