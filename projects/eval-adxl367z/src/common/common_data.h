/***************************************************************************//**
 *   @file   eval-adxl367z/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-adxl367z examples.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "adxl367.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern const struct no_os_spi_init_param spi_ip;

extern struct adxl367_init_param init_param;

#endif /* __COMMON_DATA_H__ */
