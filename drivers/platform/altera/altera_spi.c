/***************************************************************************//**
 *   @file   altera/altera_spi.c
 *   @brief  Implementation of Altera SPI Generic Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include <altera_avalon_spi_regs.h>
#include "parameters.h"
#include "error.h"
#include "spi.h"
#include "spi_extra.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Altera platform specific SPI platform ops structure
 */
const struct spi_platform_ops altera_platform_ops = {
	.init = &altera_spi_init,
	.write_and_read = &altera_spi_write_and_read,
	.remove = &altera_spi_remove
};

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t altera_spi_init(struct spi_desc **desc,
			const struct spi_init_param *param)
{
	spi_desc *descriptor;
	struct altera_spi_desc *altera_descriptor;
	struct altera_spi_init_param *altera_param;

	descriptor = malloc(sizeof(*descriptor));
	if (!descriptor)
		return FAILURE;

	descriptor->extra = calloc(1, sizeof *altera_descriptor);
	if (!(descriptor->extra)) {
		free(descriptor);
		return FAILURE;
	}

	altera_descriptor = descriptor->extra;
	altera_param = param->extra;

	descriptor->chip_select = param->chip_select;
	descriptor->mode = param->mode;
	descriptor->device_id = param->device_id;

	altera_descriptor->type = altera_param->type;
	altera_descriptor->base_address = altera_param->base_address;

	*desc = descriptor;

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by spi_init().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t altera_spi_remove(struct spi_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */

int32_t altera_spi_write_and_read(struct spi_desc *desc,
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
		return FAILURE;
	}

	return SUCCESS;
}


