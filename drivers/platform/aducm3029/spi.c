/***************************************************************************//**
 *   @file   aducm3029/spi.c
 *   @author MChindri (mihail.chindris@analog.com)
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
/************************* Include Files **************************************/
/******************************************************************************/

#include "spi_extra.h"
#include "spi.h"
#include "error.h"
#include <stdlib.h>

/******************************************************************************/
/*****************************  Variables   **********************************/
/******************************************************************************/

static uint8_t	active_spi = 0; //Stores the active SPIs

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Allocates the memory needed for the descriptor
 * @return A address to the allocated memory, NULL if the allocation fails.
 */
static struct spi_desc *alloc_data_mem(void)
{
	struct spi_desc		*desc;
	struct aducm_spi_desc	*adicup_desc;

	desc = calloc(1, sizeof(struct spi_desc));
	if (!desc)
		return NULL;

	adicup_desc = calloc(1, sizeof(struct aducm_spi_desc));
	if (!adicup_desc) {
		free(desc);
		return NULL;
	}
	desc->extra = adicup_desc;

	adicup_desc->buffer = calloc(1, ADI_SPI_MEMORY_SIZE);
	if (NULL == adicup_desc->buffer) {
		free(adicup_desc);
		free(desc);
		return NULL;
	}

	return desc;
}

/**
 * @brief Deallocates the memory needed for the descriptor
 * @param desc - Descriptor to be deallocated
 */
static void free_desc_mem(struct spi_desc *desc)
{
	free(((struct aducm_spi_desc*)(desc->extra))->buffer);
	free(desc->extra);
	free(desc);
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_init(struct spi_desc **desc,
		 const struct spi_init_param *param)
{
	ADI_SPI_RESULT			spi_ret;
	struct aducm_spi_desc		*adicup_desc;
	struct aducm_spi_init_param	*adicup_init_param;


	if (!desc || !param || !(param->extra))
		return FAILURE;

	/* Verify if the SPI is already activated */
	adicup_init_param = param->extra;
	if (active_spi & (1 << adicup_init_param->spi_channel))
		return FAILURE;

	*desc = alloc_data_mem();
	if (!(*desc))
		return FAILURE;
	(*desc)->chip_select = param->chip_select;
	(*desc)->max_speed_hz = param->max_speed_hz;
	(*desc)->mode = param->mode;
	adicup_desc = (*desc)->extra;
	adicup_desc->spi_channel = adicup_init_param->spi_channel;
	adicup_desc->master_mode = adicup_init_param->master_mode;
	adicup_desc->dma = false;
	adicup_desc->half_duplex = false;

	/* Configure the SPI with user parameters*/
	spi_ret = adi_spi_Open(adicup_init_param->spi_channel,adicup_desc->buffer,
			       ADI_SPI_MEMORY_SIZE, &adicup_desc->spi_handle);
	if (spi_ret != ADI_SPI_SUCCESS)
		goto failure;
	spi_ret = adi_spi_SetBitrate(adicup_desc->spi_handle, param->max_speed_hz);
	if (spi_ret != ADI_SPI_SUCCESS)
		goto failure;
	spi_ret = adi_spi_SetChipSelect(adicup_desc->spi_handle, param->chip_select);
	if (spi_ret != ADI_SPI_SUCCESS)
		goto failure;
	spi_ret = adi_spi_SetClockPhase(adicup_desc->spi_handle,
					param->mode & SPI_CPHA);
	if (spi_ret != ADI_SPI_SUCCESS)
		goto failure;
	spi_ret = adi_spi_SetClockPolarity(adicup_desc->spi_handle,
					   param->mode & SPI_CPOL);
	if (spi_ret != ADI_SPI_SUCCESS)
		goto failure;
	spi_ret = adi_spi_SetMasterMode(adicup_desc->spi_handle,
					adicup_init_param->master_mode);
	if (spi_ret != ADI_SPI_SUCCESS)
		goto failure;
	spi_ret = adi_spi_SetContinuousMode(adicup_desc->spi_handle,
					    adicup_init_param->continuous_mode);
	if (spi_ret != ADI_SPI_SUCCESS)
		goto failure;

	/* SPI configured. Set it as activated */
	active_spi |= (1 << adicup_init_param->spi_channel);

	return SUCCESS;
failure:
	free_desc_mem(*desc);
	return FAILURE;
}

/**
 * @brief Free the resources allocated by spi_init().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_remove(struct spi_desc *desc)
{
	struct aducm_spi_desc *adicup_desc;

	if (desc == NULL)
		return FAILURE;
	adicup_desc = desc->extra;
	if (ADI_SPI_SUCCESS != adi_spi_Close(adicup_desc->spi_handle))
		return FAILURE;

	active_spi &= ~(1<<(adicup_desc->spi_channel));
	free_desc_mem(desc);
	return SUCCESS;
}

/**
 * @brief Write and read data to/from SPI. If bytes number is 0 the function will return failure.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_write_and_read(struct spi_desc *desc,
			   uint8_t *data,
			   uint16_t bytes_number)
{
	struct aducm_spi_desc	*adicup_desc = desc->extra;
	ADI_SPI_RESULT		spi_ret;
	ADI_SPI_TRANSCEIVER	spi_trans;

	if (bytes_number == 0)
		return FAILURE;

	spi_trans.TransmitterBytes = bytes_number;
	spi_trans.pTransmitter = data;
	spi_trans.nTxIncrement = 1;

	spi_trans.ReceiverBytes = bytes_number;
	spi_trans.pReceiver = data;
	spi_trans.nRxIncrement = 1;

	spi_trans.bDMA = adicup_desc->dma;
	spi_trans.bRD_CTL = adicup_desc->half_duplex;

	if (adicup_desc->master_mode == MASTER) {
		spi_ret = adi_spi_MasterReadWrite(adicup_desc->spi_handle, &spi_trans);
		if (spi_ret != ADI_SPI_SUCCESS)
			return FAILURE;
	} else {
		spi_ret = adi_spi_SlaveReadWrite(adicup_desc->spi_handle, &spi_trans);
		if (spi_ret != ADI_SPI_SUCCESS)
			return FAILURE;
	}
	return SUCCESS;
}

