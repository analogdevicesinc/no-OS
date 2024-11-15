/***************************************************************************//**
 *   @file   adrv902x/src/common/common_data.h
 *   @brief  Defines common data to be used by adrv902x examples.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern const struct no_os_spi_init_param ad9528_spi_param;
extern const struct no_os_gpio_init_param clkchip_gpio_init_param;

#endif /* __COMMON_DATA_H__ */
