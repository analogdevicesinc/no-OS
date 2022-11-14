/***************************************************************************//**
 *   @file   ltc2358.h
 *   @brief  Implementation of LTC2358 Driver.
 *   @author Kim Seer Paller (kimseer.paller@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#ifndef LTC2358_H
#define LTC2358_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define LTC2358_BYTES_PER_CH    3
#define LTC2358_CHANNEL_MSK     NO_OS_GENMASK(2, 0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct ltc2358_init_param
 * @brief Structure holding the parameters for ltc2358 initialization.
 */
struct ltc2358_init_param {
	/* SPI Initialization structure. */
	struct no_os_spi_init_param *spi_init;
};

/**
 * @struct ltc2358_dev
 * @brief Structure holding ltc2358 descriptor.
 */
struct ltc2358_dev {
	/* SPI handler */
	struct no_os_spi_desc *spi_desc;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Creates 24-bit configuration word for the 8 channels. */
void ltc2358_create_config_word(uint8_t channel, uint8_t config_number,
				uint32_t *config_word);

/* Parse single channel data. */
int32_t ltc2358_channel_data(struct ltc2358_dev *dev, uint32_t config_word,
			     uint8_t data_array[24], uint8_t channel, uint32_t *readval);

/* Initializes the LTC2358. */
int32_t ltc2358_init(struct ltc2358_dev **device,
		     struct ltc2358_init_param *init_param);

/* Free the resources allocated by ltc2358_init(). */
int32_t ltc2358_remove(struct ltc2358_dev *dev);

#endif /* LTC2358_H */
