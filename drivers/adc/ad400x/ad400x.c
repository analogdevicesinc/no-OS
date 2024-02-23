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
#include "ad400x.h"
#if !defined(USE_STANDARD_SPI)
#include "spi_engine.h"
#endif
#include "no_os_error.h"
#include "no_os_alloc.h"

/**
 * @brief Device resolution
 */
const uint16_t ad400x_device_resol[] = {
	[ID_AD4003] = 18,
	[ID_AD4007] = 18,
	[ID_AD4011] = 18,
	[ID_AD4020] = 20,
	[ID_ADAQ4003] = 18
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

	buf[0] = AD400X_READ_COMMAND;
	buf[1] = 0xFF;

#if !defined(USE_STANDARD_SPI)
	// register access runs at a lower clock rate (~2MHz)
	spi_engine_set_speed(dev->spi_desc, dev->reg_access_speed);
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

#if !defined(USE_STANDARD_SPI)
	// register access runs at a lower clock rate (~2MHz)
	spi_engine_set_speed(dev->spi_desc, dev->reg_access_speed);
#endif

	buf[0] = AD400X_WRITE_COMMAND;
	buf[1] = reg_data | AD400X_RESERVED_MSK;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);

#if !defined(USE_STANDARD_SPI)
	spi_engine_set_speed(dev->spi_desc, dev->spi_desc->max_speed_hz);
#endif

	return ret;
}

/**
 * Read conversion result from device.
 * @param dev - The device structure.
 * @param adc_data - The conversion result data
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad400x_spi_single_conversion(struct ad400x_dev *dev,
				     uint32_t *adc_data)
{
	uint32_t buf = 0;
	int32_t ret;

	ret = no_os_spi_write_and_read(dev->spi_desc, (uint8_t *)&buf, 4);

	*adc_data = buf & 0xFFFFF;

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

#if !defined(USE_STANDARD_SPI)
	struct spi_engine_init_param *spi_eng_init_param;

	if (!init_param)
		return -1;

	spi_eng_init_param = init_param->spi_init.extra;
#endif

	dev = (struct ad400x_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret < 0)
		goto error;

	dev->dev_id = init_param->dev_id;
#if !defined(USE_STANDARD_SPI)
	dev->reg_access_speed = init_param->reg_access_speed;

	spi_engine_set_transfer_width(dev->spi_desc, 16);
#endif

	ad400x_spi_reg_read(dev, &data);

	data |= AD400X_TURBO_MODE(init_param->turbo_mode) |
		AD400X_HIGH_Z_MODE(init_param->high_z_mode) |
		AD400X_SPAN_COMPRESSION(init_param->span_compression) |
		AD400X_EN_STATUS_BITS(init_param->en_status_bits);
	ret = ad400x_spi_reg_write(dev, data);
	if (ret < 0)
		goto error;

#if !defined(USE_STANDARD_SPI)
	spi_engine_set_transfer_width(dev->spi_desc, spi_eng_init_param->data_width);
#endif

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

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}
