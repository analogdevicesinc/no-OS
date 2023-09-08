/*******************************************************************************
 *   @file   chibios/chibios_spi.c
 *   @brief  Implementation of chibios spi driver.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <errno.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_alloc.h"
#include "chibios_spi.h"

#if (HAL_USE_SPI==TRUE)

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t chibios_spi_init(struct no_os_spi_desc **desc,
			 const struct no_os_spi_init_param *param)
{
	int32_t ret;

	struct no_os_spi_desc *spi_desc;
	struct chibios_spi_desc *sdesc;

	if (!desc || !param)
		return -EINVAL;

	spi_desc = (struct no_os_spi_desc *)no_os_calloc(1, sizeof(*spi_desc));

	if (!spi_desc) {
		ret = -ENOMEM;
		goto error_1;
	}

	sdesc = (struct chibios_spi_desc*)no_os_calloc(1, sizeof(*sdesc));

	if (!sdesc) {
		ret = -ENOMEM;
		goto error_2;
	}

	sdesc->hspi = ((struct chibios_spi_init_param *)(param->extra))->hspi;
	sdesc->spicfg = ((struct chibios_spi_init_param *)(param->extra))->spicfg;
	spi_desc->extra = sdesc;

	spiStart((sdesc->hspi), (sdesc->spicfg));

	*desc = spi_desc;
	return 0;

error_2:
	no_os_free(sdesc);
error_1:
	no_os_free(spi_desc);

	return ret;

}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t chibios_spi_remove(struct no_os_spi_desc *desc)
{
	struct chibios_spi_desc *sdesc;

	if (!desc || !desc->extra)
		return -EINVAL;
	sdesc = desc->extra;

	spiStop(sdesc->hspi);

	no_os_free(desc->extra);
	no_os_free(desc);
	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t chibios_spi_write_and_read(struct no_os_spi_desc *desc,
				   uint8_t *data,
				   uint16_t bytes_number)
{
	struct chibios_spi_desc *sdesc = (struct chibios_spi_desc *)desc->extra;
	// select device
	spiSelect((sdesc->hspi));

	//send and receive buffer
	//switch implementation to support two word transfer as well
	while(bytes_number--) {
		spiExchange((sdesc->hspi), 1, data, data);
		data++;
	}
	// unselect device
	spiUnselect((sdesc->hspi));

	return 0;
}

/**
 * @brief chibios platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops chibios_spi_ops = {
	.init = &chibios_spi_init,
	.write_and_read = &chibios_spi_write_and_read,
	.remove = &chibios_spi_remove
};

#endif //HAL_USE_SPI==TRUE
