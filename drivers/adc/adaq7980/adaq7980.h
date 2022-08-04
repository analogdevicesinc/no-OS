/***************************************************************************//**
 * @file adaq7980.c
 * @brief Header file for adaq7980 Driver.
 * @author ADI
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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
#ifndef ADAQ7980_H_
#define ADAQ7980_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "spi_engine.h"
#include "no_os_pwm.h"
#include "no_os_gpio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct adaq7980_init_param
 * @brief Structure containing the init parameters needed by the adaq7980 device
 */
struct adaq7980_init_param {
	/* SPI */
	struct no_os_spi_init_param		*spi_init;
	/* SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
	/* PWM generator init structure */
	struct no_os_pwm_init_param	*trigger_pwm_init;
	/** Power down GPIO initialization structure. */
	struct no_os_gpio_init_param	*gpio_pd_ldo;
};

/**
 * @struct adaq7980_dev
 * @brief  Structure representing an adaq7980 device
 */
struct adaq7980_dev {
	/* SPI descriptor */
	struct no_os_spi_desc		*spi_desc;
	/* Trigger conversion PWM generator descriptor */
	struct no_os_pwm_desc		*trigger_pwm_desc;
	/* SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
	/** Power down GPIO handler. */
	struct no_os_gpio_desc	*gpio_pd_ldo;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the device. */
int32_t adaq7980_setup(struct adaq7980_dev **device,
		       struct adaq7980_init_param *init_param);

/* Read data from device */
int32_t ad7980_read_data(struct adaq7980_dev *dev,
			 uint16_t *buf,
			 uint16_t samples);

#endif
