/***************************************************************************//**
 *   @file   altera/altera_spi.c
 *   @brief  Implementation of Altera SPI Generic Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include <altera_avalon_spi_regs.h>
#include "parameters.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_spi.h"
#include "altera_spi.h"

#warning SPI delays are not supported on the altera platform

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_spi_init(struct no_os_spi_desc **desc,
			const struct no_os_spi_init_param *param)
{
	struct no_os_spi_desc *descriptor;
	struct altera_spi_desc *altera_descriptor;
	struct altera_spi_init_param *altera_param;

	descriptor = no_os_malloc(sizeof(*descriptor));
	if (!descriptor)
		return -1;

	descriptor->extra = no_os_calloc(1, sizeof *altera_descriptor);
	if (!(descriptor->extra)) {
		no_os_free(descriptor);
		return -1;
	}

	altera_descriptor = descriptor->extra;
	altera_param = param->extra;

	descriptor->chip_select = param->chip_select;
	descriptor->mode = param->mode;
	descriptor->device_id = param->device_id;

	altera_descriptor->type = altera_param->type;
	altera_descriptor->base_address = altera_param->base_address;

	*desc = descriptor;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_spi_remove(struct no_os_spi_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.
 */

int32_t altera_spi_write_and_read(struct no_os_spi_desc *desc,
				  uint8_t *data,
				  uint16_t bytes_number)
{
	uint32_t i;
	struct altera_spi_desc *altera_desc;

	altera_desc = desc->extra;

	switch(altera_desc->type) {
	case NIOS_II_SPI:
		IOWR_32DIRECT(altera_desc->base_address,
			      (ALTERA_AVALON_SPI_CONTROL_REG * 4),
			      ALTERA_AVALON_SPI_CONTROL_SSO_MSK);
		IOWR_32DIRECT(altera_desc->base_address,
			      (ALTERA_AVALON_SPI_SLAVE_SEL_REG * 4),
			      (0x1 << (desc->chip_select)));
		for (i = 0; i < bytes_number; i++) {
			while ((IORD_32DIRECT(altera_desc->base_address,
					      (ALTERA_AVALON_SPI_STATUS_REG * 4)) &
				ALTERA_AVALON_SPI_STATUS_TRDY_MSK) == 0x00) {}
			IOWR_32DIRECT(altera_desc->base_address,
				      (ALTERA_AVALON_SPI_TXDATA_REG * 4),
				      *(data + i));
			while ((IORD_32DIRECT(altera_desc->base_address,
					      (ALTERA_AVALON_SPI_STATUS_REG * 4)) &
				ALTERA_AVALON_SPI_STATUS_RRDY_MSK) == 0x00) {}
			*(data + i) = IORD_32DIRECT(altera_desc->base_address,
						    (ALTERA_AVALON_SPI_RXDATA_REG * 4));
		}
		IOWR_32DIRECT(altera_desc->base_address,
			      (ALTERA_AVALON_SPI_SLAVE_SEL_REG * 4), 0x000);
		IOWR_32DIRECT(altera_desc->base_address,
			      (ALTERA_AVALON_SPI_CONTROL_REG * 4), 0x000);

		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief Altera platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops altera_spi_ops  = {
	.init = &altera_spi_init,
	.write_and_read = &altera_spi_write_and_read,
	.remove = &altera_spi_remove
};
