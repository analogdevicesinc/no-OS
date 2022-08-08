/***************************************************************************//**
 *   @file   aducm3029_spi.h
 *   @brief  ADuCM302x specific header for SPI driver
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

#ifndef ADUCM3029_SPI_H
# define ADUCM3029_SPI_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_spi.h"
#include <drivers/spi/adi_spi.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum master_mode
 * @brief Available operations mode for a SPI channel
 */
enum master_mode {
	/** Slave mode. */
	SLAVE,
	/** Master mode. */
	MASTER
};

/**
 * @struct aducm_device_desc
 * @brief Structure describing the physical SPI devices
 */
struct aducm_device_desc {
	/** Buffer for the ADI driver */
	uint8_t			buffer[ADI_SPI_MEMORY_SIZE];
	/** Handle to identify the SPI device */
	ADI_SPI_HANDLE		spi_handle;
	/** Number of instances referring this structure */
	uint32_t		ref_instances;
	/** Bitrate */
	uint32_t		bitrate;
	/** Chipselect */
	uint8_t			cs;
	/** SPI mode */
	enum no_os_spi_mode		mode;
	/** Select the operation mode */
	enum master_mode	master_mode;
	/** Enable or disable continuous mode */
	bool			continuous_mode;
};

/**
 * @struct aducm_spi_init_param
 * @brief Configuration structure sent in the extra parameter from
 * no_os_spi_init_param.
 */
struct aducm_spi_init_param {
	/** Select the operation mode */
	enum master_mode	master_mode;
	/** Enable or disable continuous mode */
	bool			continuous_mode;
	/** If true, it enables half duplex mode. The default if false */
	bool			half_duplex;
	/**
	 * If true, it enables dma. The maximum number of a transaction with
	 * dma is 2048
	 */
	bool			dma;
};

/**
 * @brief ADuCM3029 specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops aducm_spi_ops;

/**
 * @struct aducm_spi_desc
 * @brief SPI specific descriptor for the ADuCM3029. The structure is available
 * in the extra parameter from spi_desc.
 */
struct aducm_spi_desc {
	/** Aducm configuration for the SPI instance */
	struct aducm_spi_init_param	aducm_conf;
	/** Reference to the hardware device */
	struct aducm_device_desc	*dev;
};

#endif // ADUCM3029_SPI_H
