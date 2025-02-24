/***************************************************************************//**
 *   @file   altera/altera_spi.c
 *   @brief  Implementation of Altera SPI Generic Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <altera_avalon_spi_regs.h>
#include "parameters.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_spi.h"
#include "altera_spi.h"

#warning SPI delays are not supported on the altera platform

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

	descriptor->extra = no_os_calloc(1, sizeof * altera_descriptor);
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

	switch (altera_desc->type) {
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
