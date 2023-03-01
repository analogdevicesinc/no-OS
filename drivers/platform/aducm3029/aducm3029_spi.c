/***************************************************************************//**
 *   @file   aducm3029/aducm3029_spi.c
 *   @brief  Implementation of SPI driver for ADuCM302x
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

#include "aducm3029_spi.h"
#include "no_os_spi.h"
#include "no_os_error.h"
#include <stdlib.h>
#include "no_os_util.h"
#include "no_os_alloc.h"

#define	NB_SPI_DEVICES	3
#define	MAX_CS_NUMBER	3

#warning SPI delays are not supported on the aducm3029 platform

/******************************************************************************/
/*****************************  Variables   **********************************/
/******************************************************************************/

/** Structure storing the device info */
static struct aducm_device_desc	*devices[NB_SPI_DEVICES];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Update hardware device if needed.
 *
 * Check if the current state of the SPI device match with the setting of the
 * the current instance. If not, the settings for the SPI device will be
 * changed.
 * @param dev - SPI instance
 * @param desc - SPI device descriptor
 * @param init - True if it is the first configuration of the SPI device.
 * @return
 */
static int32_t config_device(struct aducm_device_desc *dev,
			     struct no_os_spi_desc *desc,
			     bool init)
{
	struct aducm_spi_desc *aducm_desc = desc->extra;

	if (init || desc->max_speed_hz != dev->bitrate) {
		if (ADI_SPI_SUCCESS != adi_spi_SetBitrate(dev->spi_handle,
				desc->max_speed_hz))
			return -1;
		if (init) {
			if (ADI_SPI_SUCCESS != adi_spi_GetBitrate(
				    dev->spi_handle,
				    &dev->bitrate))
				return -1;
			desc->max_speed_hz = dev->bitrate;
		}
	}
	if (init || desc->chip_select != dev->cs) {
		adi_spi_SetChipSelect(dev->spi_handle, desc->chip_select);
		dev->cs = desc->chip_select;
	}
	if (init || desc->mode != dev->mode) {
		adi_spi_SetClockPhase(dev->spi_handle, desc->mode & NO_OS_SPI_CPHA);
		adi_spi_SetClockPolarity(dev->spi_handle,
					 desc->mode & NO_OS_SPI_CPOL);
		dev->mode = desc->mode;
	}
	if (init || aducm_desc->aducm_conf.master_mode != dev->master_mode) {
		adi_spi_SetMasterMode(dev->spi_handle,
				      aducm_desc->aducm_conf.master_mode);
		dev->master_mode = aducm_desc->aducm_conf.master_mode;
	}
	if (init || aducm_desc->aducm_conf.continuous_mode
	    != dev->continuous_mode) {
		adi_spi_SetContinuousMode(dev->spi_handle,
					  aducm_desc->aducm_conf.continuous_mode);
		dev->continuous_mode = aducm_desc->aducm_conf.continuous_mode;
	}

	return 0;
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_spi_init(struct no_os_spi_desc **desc,
			   const struct no_os_spi_init_param *param)
{
	struct no_os_spi_desc			*spi_desc;
	struct aducm_spi_desc		*aducm_desc;
	struct aducm_spi_init_param	*config;
	struct aducm_device_desc	*dev;

	/* Initial checks */
	if (!desc || !param || !(param->extra))
		return -1;

	config = param->extra;
	if (param->device_id >= NB_SPI_DEVICES ||
	    param->chip_select > MAX_CS_NUMBER)
		return -1;

	/* Memory allocation */
	spi_desc = (struct no_os_spi_desc *)no_os_calloc(1, sizeof(*spi_desc));
	if (!spi_desc)
		return -1;
	aducm_desc = (struct aducm_spi_desc *)no_os_calloc(1, sizeof(*aducm_desc));
	if (!aducm_desc) {
		no_os_free(spi_desc);
		return -1;
	}

	/* Initialize descriptor */
	aducm_desc->aducm_conf = *config;
	spi_desc->extra = aducm_desc;
	spi_desc->chip_select = (1 << param->chip_select);
	spi_desc->max_speed_hz = param->max_speed_hz;
	spi_desc->mode = param->mode;
	spi_desc->device_id = param->device_id;

	dev = devices[param->device_id];
	/* If device not initialized initialize it */
	if (!dev) {
		dev = (struct aducm_device_desc *)no_os_calloc(1, sizeof(*dev));
		if (!dev) {
			no_os_free(spi_desc);
			no_os_free(aducm_desc);
			return -1;
		}
		if (ADI_SPI_SUCCESS != adi_spi_Open(param->device_id,
						    dev->buffer,
						    ADI_SPI_MEMORY_SIZE,
						    &dev->spi_handle))
			goto failure;
		if (0 != config_device(dev, spi_desc, true))
			goto failure;
		devices[param->device_id] = dev;
	}
	dev->ref_instances++;
	aducm_desc->dev = dev;
	*desc = spi_desc;

	return 0;
failure:
	no_os_free(dev);
	no_os_free(spi_desc);
	no_os_free(aducm_desc);
	return -1;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_spi_remove(struct no_os_spi_desc *desc)
{
	struct aducm_spi_desc *aducm_desc;

	if (!desc || !desc->extra)
		return -1;

	aducm_desc = desc->extra;
	if (!aducm_desc->dev)
		return -1;

	if (aducm_desc->dev->ref_instances == 1) {
		if (ADI_SPI_SUCCESS != adi_spi_Close(
			    aducm_desc->dev->spi_handle))
			return -1;
		no_os_free(aducm_desc->dev);
		devices[desc->device_id] = NULL;
	}
	no_os_free(aducm_desc);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write and read data to/from SPI. If bytes number is 0 the function will return failure.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_spi_write_and_read(struct no_os_spi_desc *desc,
				     uint8_t *data,
				     uint16_t bytes_number)
{
	struct aducm_spi_desc		*aducm_desc;
	ADI_SPI_TRANSCEIVER		spi_trans;
	uint16_t			n;

	if (!desc)
		return -1;

	aducm_desc = desc->extra;
	if (!aducm_desc->dev)
		return -1;

	if (0 != config_device(aducm_desc->dev, desc, false))
		return -1;

	if (bytes_number == 0)
		return -1;

	spi_trans.nTxIncrement = 1;
	spi_trans.nRxIncrement = 1;
	spi_trans.bDMA = aducm_desc->aducm_conf.dma;
	spi_trans.bRD_CTL = aducm_desc->aducm_conf.half_duplex;

	while (bytes_number) {
		if (aducm_desc->aducm_conf.dma)
			/* Maximum 2048 bytes over dma */
			n = no_os_max(2048, bytes_number);
		else
			n = bytes_number;

		spi_trans.TransmitterBytes = n;
		spi_trans.pTransmitter = data;
		spi_trans.ReceiverBytes = n;
		spi_trans.pReceiver = data;

		if (aducm_desc->aducm_conf.master_mode == MASTER) {
			if (ADI_SPI_SUCCESS != adi_spi_MasterReadWrite(
				    aducm_desc->dev->spi_handle,
				    &spi_trans))
				return -1;
		} else {
			if (ADI_SPI_SUCCESS != adi_spi_SlaveReadWrite(
				    aducm_desc->dev->spi_handle,
				    &spi_trans))
				return -1;
		}

		data += n;
		bytes_number -= n;
	}

	return 0;
}

/**
 * @brief ADuCM3029 platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops aducm_spi_ops = {
	.init = &aducm3029_spi_init,
	.write_and_read = &aducm3029_spi_write_and_read,
	.remove = &aducm3029_spi_remove
};
