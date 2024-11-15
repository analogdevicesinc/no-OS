/***************************************************************************//**
 * @file adaq7980.c
 * @brief Header file for adaq7980 Driver.
 * @author ADI
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
