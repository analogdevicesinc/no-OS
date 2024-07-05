/***************************************************************************//**
 *   @file   ad400x.c
 *   @brief  Implementation of ad400x Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "ad400x.h"
#if !defined(USE_STANDARD_SPI)
#include "spi_engine.h"
#endif
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

const struct ad400x_dev_info ad400x_devices[] = {
	[ID_AD4000] = {.resolution = 16, .sign = 'u', .max_rate = 2000},
	[ID_AD4001] = {.resolution = 16, .sign = 's', .max_rate = 2000},
	[ID_AD4002] = {.resolution = 18, .sign = 'u', .max_rate = 2000},
	[ID_AD4003] = {.resolution = 18, .sign = 's', .max_rate = 2000},
	[ID_AD4004] = {.resolution = 16, .sign = 'u', .max_rate = 1000},
	[ID_AD4005] = {.resolution = 16, .sign = 's', .max_rate = 1000},
	[ID_AD4006] = {.resolution = 18, .sign = 'u', .max_rate = 1000},
	[ID_AD4007] = {.resolution = 18, .sign = 's', .max_rate = 1000},
	[ID_AD4008] = {.resolution = 16, .sign = 'u', .max_rate = 500},
	[ID_AD4010] = {.resolution = 18, .sign = 'u', .max_rate = 500},
	[ID_AD4011] = {.resolution = 18, .sign = 's', .max_rate = 500},
	[ID_AD4020] = {.resolution = 20, .sign = 's', .max_rate = 1800},
	[ID_AD4021] = {.resolution = 20, .sign = 's', .max_rate = 1000},
	[ID_AD4022] = {.resolution = 20, .sign = 's', .max_rate = 500},
	[ID_ADAQ4003] = {.resolution = 18, .sign = 's',.max_rate = 2000},
	[ID_AD7690] = {.resolution = 18, .sign = 's', .max_rate = 400},
	[ID_AD7691] = {.resolution = 18, .sign = 's', .max_rate = 250},
	[ID_AD7693] = {.resolution = 16, .sign = 's', .max_rate = 500},
	[ID_AD7942] = {.resolution = 14, .sign = 'u', .max_rate = 250},
	[ID_AD7944] = {.resolution = 14, .sign = 'u', .max_rate = 2500},
	[ID_AD7946] = {.resolution = 14, .sign = 's', .max_rate = 500},
	[ID_AD7980] = {.resolution = 16, .sign = 'u', .max_rate = 1000},
	[ID_AD7982] = {.resolution = 18, .sign = 's', .max_rate = 1000},
	[ID_AD7983] = {.resolution = 16, .sign = 'u', .max_rate = 1330},
	[ID_AD7984] = {.resolution = 18, .sign = 's', .max_rate = 1330},
	[ID_AD7985] = {.resolution = 16, .sign = 'u', .max_rate = 2500},
	[ID_AD7986] = {.resolution = 18, .sign = 's', .max_rate = 2000},
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad400x_spi_reg_read(struct ad400x_dev *dev,
			    uint8_t *reg_data)
{
	int32_t ret;
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	buf[0] = AD400X_READ_COMMAND;
	buf[1] = 0xFF;

#if !defined(USE_STANDARD_SPI)
	// register access runs at a lower clock rate (~2MHz)
	spi_engine_set_speed(dev->spi_desc, dev->reg_access_speed);
#else
	/*
	 * CNV pin should toggle and be low before register access
	 * no need to add an explicit delay as the required pulse
	 * width is only 10ns
	 */
	ret = no_os_gpio_set_value(dev->gpio_cnv, 1);
	if (ret)
		return ret;
	ret = no_os_gpio_set_value(dev->gpio_cnv, 0);
	if (ret)
		return ret;
#endif

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	*reg_data = buf[1];

#if !defined(USE_STANDARD_SPI)
	spi_engine_set_speed(dev->spi_desc, dev->spi_desc->max_speed_hz);
#endif

	return ret;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad400x_spi_reg_write(struct ad400x_dev *dev,
			     uint8_t reg_data)
{
	int32_t ret;
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

#if !defined(USE_STANDARD_SPI)
	// register access runs at a lower clock rate (~2MHz)
	spi_engine_set_speed(dev->spi_desc, dev->reg_access_speed);
#else
	/*
	 * CNV pin should toggle and be low before register access
	 * no need to add an explicit delay as the required pulse
	 * width is only 10ns
	 */
	ret = no_os_gpio_set_value(dev->gpio_cnv, 1);
	if (ret)
		return ret;
	ret = no_os_gpio_set_value(dev->gpio_cnv, 0);
	if (ret)
		return ret;
#endif

	buf[0] = AD400X_WRITE_COMMAND;
	buf[1] = reg_data | AD400X_RESERVED_MSK;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);

#if !defined(USE_STANDARD_SPI)
	spi_engine_set_speed(dev->spi_desc, dev->spi_desc->max_speed_hz);
#endif

	return ret;
}

#if !defined(USE_STANDARD_SPI)
/**
 * Read samples using spi offload engine
 * @param dev - The device structure.
 * @param buf - Buffer to hold the conversion results data
 * @param samples - number of samples to read
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad400x_read_data_offload(struct ad400x_dev *dev,
					uint32_t *buf,
					uint16_t samples)
{
	struct spi_engine_offload_message msg;
	uint32_t commands_data[2] = {0xFF, 0xFF};
	int ret;
	uint32_t spi_eng_msg_cmds[3] = {
		CS_LOW,
		READ(2),
		CS_HIGH
	};

	if (!dev)
		return -EINVAL;

	ret = spi_engine_offload_init(dev->spi_desc, dev->offload_init_param);
	if (ret)
		return ret;

	msg.commands = spi_eng_msg_cmds;
	msg.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
	msg.rx_addr = buf;
	msg.commands_data = commands_data;

	ret = spi_engine_offload_transfer(dev->spi_desc, msg, samples);
	if (ret)
		return ret;

	if (dev->dcache_invalidate_range)
		dev->dcache_invalidate_range(msg.rx_addr, samples * 4);

	return ret;
}
#endif

/**
 * Read single conversion result from device.
 * @param dev - The device structure.
 * @param adc_data - The conversion result data
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad400x_spi_single_conversion(struct ad400x_dev *dev,
		uint32_t *data)
{
	int32_t ret;
	uint8_t data_read[4] = { 0xFF, 0xFF, 0xFF, 0xFF }; /* SDI must remain high */
	uint16_t bytes_number = 2;

	if (!dev)
		return -EINVAL;

	if (dev->dev_info->resolution > 16)
		bytes_number = 3;

#if defined(USE_STANDARD_SPI)
	/*
	 * Trigger a conversion
	 * No need to explicitly add a delay as the meassuered
	 * lattency to toggle the gpio and spi clk is arround 3usec
	 */
	ret = no_os_gpio_set_value(dev->gpio_cnv, 1);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_cnv, 0);
	if (ret)
		return ret;
#endif
	ret = no_os_spi_write_and_read(dev->spi_desc, data_read, bytes_number);
	if (ret)
		return ret;

	*data = no_os_get_unaligned_be32(data_read);
	*data >>= 32 - dev->dev_info->resolution;
	*data = *data & NO_OS_GENMASK(dev->dev_info->resolution, 0);

	return 0;
}

/**
 * Read conversion results from device.
 * @param dev - The device structure.
 * @param buf - Buffer to hold the conversion results data
 * @param samples - number of samples to read
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad400x_read_data(struct ad400x_dev *dev,
			 uint32_t *buf,
			 uint16_t samples)
{
	int i, ret;
	uint32_t *p;

	if (!dev)
		return -EINVAL;

#if !defined(USE_STANDARD_SPI)
	if (dev->offload_enable)
		return ad400x_read_data_offload(dev, buf, samples);
#endif
	for (i = 0, p = buf; i < samples; i++, p++) {
		ret = ad400x_spi_single_conversion(dev, p);
		if (ret)
			return ret;
	}
	return ret;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad400x_init(struct ad400x_dev **device,
		    struct ad400x_init_param *init_param)
{
	struct ad400x_dev *dev;
	int32_t ret;
	uint8_t data = 0;
	uint16_t transfer_width;

	if (!init_param)
		return -1;

	dev = (struct ad400x_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret < 0)
		goto error;

	dev->dev_id = init_param->dev_id;
	dev->dev_info = &ad400x_devices[init_param->dev_id];
	dev->reg_access_speed = init_param->reg_access_speed;
	dev->offload_init_param = init_param->offload_init_param;
	dev->dcache_invalidate_range = init_param->dcache_invalidate_range;
	dev->offload_enable =  init_param->offload_enable;

#if defined(USE_STANDARD_SPI)
	ret = no_os_gpio_get(&dev->gpio_cnv, init_param->gpio_cnv);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_output(dev->gpio_cnv, NO_OS_GPIO_LOW);
	if (ret)
		goto error;
#else
	transfer_width = dev->dev_info->resolution;

	/* without offload xfer width has to be byte alligned */
	if (!dev->offload_enable)
		transfer_width = NO_OS_DIV_ROUND_UP(transfer_width, 8) * 8;

	spi_engine_set_transfer_width(dev->spi_desc, transfer_width);
	ret = axi_clkgen_init(&dev->clkgen, init_param->clkgen_init);
	if (ret)
		goto error;

	ret = axi_clkgen_init(&dev->clkgen, init_param->clkgen_init);
	if (ret)
		goto error;

	ret = axi_clkgen_set_rate(dev->clkgen, init_param->axi_clkgen_rate);
	if (ret)
		goto error;

	/* Calculate pwm rate given the part sample rate */
	if (init_param->trigger_pwm_init->period_ns == 0) {
		init_param->trigger_pwm_init->period_ns =
			NO_OS_DIV_ROUND_UP(1000000, ad400x_devices[dev->dev_id].max_rate);
		init_param->trigger_pwm_init->duty_cycle_ns = 10;
	}

	ret = no_os_pwm_init(&dev->trigger_pwm_desc, init_param->trigger_pwm_init);
	if (ret)
		goto error;
#endif

	ad400x_spi_reg_read(dev, &data);

	data |= AD400X_TURBO_MODE(init_param->turbo_mode) |
		AD400X_HIGH_Z_MODE(init_param->high_z_mode) |
		AD400X_SPAN_COMPRESSION(init_param->span_compression) |
		AD400X_EN_STATUS_BITS(init_param->en_status_bits);
	ret = ad400x_spi_reg_write(dev, data);
	if (ret < 0)
		goto error;

	*device = dev;

	return ret;

error:
	ad400x_remove(dev);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad400x_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad400x_remove(struct ad400x_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -EINVAL;

#if defined(USE_STANDARD_SPI)
	ret = no_os_gpio_remove(dev->gpio_cnv);
	if (ret)
		return ret;
#else
	ret = axi_clkgen_remove(dev->clkgen);
	if (ret)
		return ret;

	ret = no_os_pwm_remove(dev->trigger_pwm_desc);
	if (ret)
		return ret;
#endif
	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}
