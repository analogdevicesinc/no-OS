/***************************************************************************//**
 *   @file   spi.c
 *   @brief  Implementation of Xilinx SPI Generic Driver.
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
#include "error.h"
#include "spi.h"
#include "xilinx_platform_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param init_param - The structure that contains the SPI parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_init(struct spi_desc **desc,
		 const struct spi_init_param *param)
{
	spi_desc *descriptor;
	xil_spi_desc *xil_descriptor;
	xil_spi_init_param *xil_param;
	int32_t ret;

	descriptor = calloc(1, sizeof *descriptor);
	if (!descriptor)
		return FAILURE;

	descriptor->extra = calloc(1, sizeof *xil_descriptor);
	if (!(descriptor->extra)) {
		free(descriptor);
		return FAILURE;
	}

	xil_descriptor = descriptor->extra;
	xil_param = param->extra;

	descriptor->mode = param->mode;
	descriptor->chip_select = param->chip_select;
	xil_descriptor->flags = xil_param->flags;

#ifdef _XPARAMETERS_PS_H_
	xil_descriptor->config = XSpiPs_LookupConfig(xil_param->id);
	if (xil_descriptor->config == NULL)
		goto error;

	ret = XSpiPs_CfgInitialize(&xil_descriptor->instance,
				   xil_descriptor->config, xil_descriptor->config->BaseAddress);
	if (ret != 0)
		goto error;

	XSpiPs_SetOptions(&xil_descriptor->instance,
			  XSPIPS_MASTER_OPTION |
			  ((xil_descriptor->flags & SPI_CS_DECODE) ?
			   XSPIPS_DECODE_SSELECT_OPTION : 0) |
			  XSPIPS_FORCE_SSELECT_OPTION |
			  ((descriptor->mode & SPI_CPOL) ?
			   XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
			  ((descriptor->mode & SPI_CPHA) ?
			   XSPIPS_CLK_PHASE_1_OPTION : 0));

	XSpiPs_SetClkPrescaler(&xil_descriptor->instance,
			       XSPIPS_CLK_PRESCALE_64);

	XSpiPs_SetSlaveSelect(&xil_descriptor->instance, 0xf);
#else
	ret = XSpi_Initialize(&xil_descriptor->instance, xil_param->id);
	if (ret != 0)
		goto error;

	XSpi_SetOptions(&xil_descriptor->instance,
			XSP_MASTER_OPTION |
			((descriptor->mode & SPI_CPOL) ?
			 XSP_CLK_ACTIVE_LOW_OPTION : 0) |
			((descriptor->mode & SPI_CPHA) ?
			 XSP_CLK_PHASE_1_OPTION : 0));

	XSpi_Start(&xil_descriptor->instance);

	XSpi_IntrGlobalDisable(&xil_descriptor->instance);
#endif

	*desc = descriptor;

	return SUCCESS;

error:
	free(descriptor);

	return FAILURE;
}

/**
 * @brief Free the resources allocated by spi_init().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_remove(struct spi_desc *desc)
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

int32_t spi_write_and_read(struct spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number)
{
	xil_spi_desc *xil_desc;
	xil_desc = desc->extra;

#ifdef _XPARAMETERS_PS_H_
	XSpiPs_SetOptions(&xil_desc->instance,
			  XSPIPS_MASTER_OPTION |
			  ((xil_desc->flags & SPI_CS_DECODE) ?
			   XSPIPS_DECODE_SSELECT_OPTION : 0) |
			  XSPIPS_FORCE_SSELECT_OPTION |
			  ((desc->mode & SPI_CPOL) ?
			   XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
			  ((desc->mode & SPI_CPHA) ?
			   XSPIPS_CLK_PHASE_1_OPTION : 0));

	XSpiPs_SetSlaveSelect(&xil_desc->instance,
			      desc->chip_select);
	XSpiPs_PolledTransfer(&xil_desc->instance,
			      data, data, bytes_number);
#else
	XSpi_SetOptions(&xil_desc->instance,
			XSP_MASTER_OPTION |
			((desc->mode & SPI_CPOL) ?
			 XSP_CLK_ACTIVE_LOW_OPTION : 0) |
			((desc->mode & SPI_CPHA) ?
			 XSP_CLK_PHASE_1_OPTION : 0));

	XSpi_SetSlaveSelect(&xil_desc->instance,
			    0x01 << desc->chip_select);

	XSpi_Transfer(&desc->instance,
		      data, data, bytes_number);
#endif
	return SUCCESS;
}
