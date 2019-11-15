/***************************************************************************//**
 *   @file   spi.h
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

#ifndef SPI_EXTRA_H
# define SPI_EXTRA_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <drivers/spi/adi_spi.h>

/**
 * enum spi_channel - Available SPI channels on the ADuCM3029
 * @SPI0:				SPI0 channel.
 * @SPI1:				SPI1 channel.
 * @SPI2:				SPI2 channel.
 */

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum spi_channel {
	SPI0,
	SPI1,
	SPI2
};

/**
 * enum master_mode - Available operations mode for a SPI channel
 * @SLAVE:				Slave mode.
 * @MASTER:				Master mode.
 */
enum master_mode {
	SLAVE,
	MASTER
};

/**
 * struct aducm_spi_init_param - Configuration structure sent in the extra parameter from spi_init_param.
 * @spi_channel:		Select the SPI channel
 * @master_mode:		Select the operation mode
 * @continuous_mode:	Enable or disable continuous mode
 */
struct aducm_spi_init_param {
	enum spi_channel	spi_channel;
	enum master_mode	master_mode;
	bool				continuous_mode;
};

/**
 * struct aducm_spi_desc - SPI specific descriptor for the ADuCM3029. The structure is available in the
 * extra parameter from spi_desc. The dma and half_duplex parameters can be configurated before every call
 * to spi_read_write.
 * @master_mode:		Select the operation mode
 * @spi_channel:		Select the spi_channel
 * @half_duplex:		If true, it enables half duplex mode. The default if false
 * @dma:				Device name. If true, it enables DMA transfer. The default value is false
 * @buffer:				RESERVED
 * @spi_handle:			Handle to identify the SPI device
 */
struct aducm_spi_desc {
	enum master_mode	master_mode;
	enum spi_channel	spi_channel;
	bool					half_duplex;
	bool					dma;
	void					*buffer;
	ADI_SPI_HANDLE			spi_handle;
};

#endif // SPI_EXTRA_H_
