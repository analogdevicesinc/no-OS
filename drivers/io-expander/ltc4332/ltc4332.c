/***************************************************************************//**
 *   @file   ltc4332.c
 *   @brief  Implementation of the SPI ltc4332 Interface
 *   @author Paul Benoit (paul.benoit@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "ltc4332.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/**************************** Types Definitions *******************************/
/******************************************************************************/

/**
 * @brief LTC4332 specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops ltc4332_spi_platform_ops = {
	.init = ltc4332_spi_init,
	.remove = ltc4332_spi_remove,
	.write_and_read = ltc4332_spi_write_and_read
};

/**
 * @brief Initialize the SPI ltc4332 layer.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ltc4332_spi_init(struct no_os_spi_desc **desc,
			 const struct no_os_spi_init_param *param)
{
	int32_t ret;
	struct no_os_spi_init_param temp_param;

	if (!param->parent)
		return -EINVAL;

	temp_param = *param;
	temp_param.platform_ops = param->parent->platform_ops;
	ret = no_os_spi_init(desc, &temp_param);
	if (ret)
		return ret;

	(*desc)->platform_ops = param->platform_ops;

	return 0;
}

/**
 * @brief Free the resources allocated by ltc4332_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ltc4332_spi_remove(struct no_os_spi_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc->extra);
	no_os_free(desc);
	return 0;
}

/**
 * @brief Write and read data to/from SPI ltc4332 layer.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.S
 */
int32_t ltc4332_spi_write_and_read(struct no_os_spi_desc *desc, uint8_t *data,
				   uint16_t bytes_number)
{
	int32_t ret;
	uint8_t *buff;
	struct no_os_spi_platform_ops *temp_ops;

	if (!desc || !desc->parent)
		return -EINVAL;

	temp_ops = desc->platform_ops;
	desc->platform_ops = desc->parent->platform_ops;

	buff = no_os_calloc(bytes_number + 1, sizeof *buff);
	if (!buff)
		return -ENOMEM;

	memcpy(buff, data, bytes_number);
	ret = no_os_spi_write_and_read(desc, buff, bytes_number + 1);
	if(ret)
		goto error;

	memcpy(data, buff+1, bytes_number);
	desc->platform_ops = temp_ops;
	no_os_free(buff);

	return ret;
error:
	no_os_free(buff);
	return ret;
}

