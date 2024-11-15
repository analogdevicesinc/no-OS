/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by ad9545
 *           project.
 *   @author Jonathan Santos (jonathan.santos@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "linux_spi.h"
#include "linux_i2c.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SPI_DEVICE_ID	0
#define SPI_CS		0
#define SPI_BAUDRATE	1000000
#define SPI_OPS		&linux_spi_ops
#define SPI_EXTRA	NULL

#define I2C_EXTRA	&ad9545_i2c_extra_ip
#define I2C_OPS		&linux_i2c_ops

#define COMM_TYPE       SPI

extern struct linux_i2c_init_param ad9545_i2c_extra_ip;

#endif /* __PARAMETERS_H__ */
