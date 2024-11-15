/***************************************************************************//**
 *   @file   adrv9009/src/app/app_config.h
 *   @brief  Config file for ADRV9009 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

/* Uncomment if using adrv2crr-fmc + adrv9009-zu11eg: */
// #define ZU11EG

/* Uncomment if using zcu102 + fmcomms8: */
// #define FMCOMMS8_ZCU102

/* Provided ZU11EG or FMCOMMS8_ZCU102 were not defined, this project builds
 * by default for zc706 + adrv9009-w/pcbz.
 */

/* To build ADRV9008-1/ADRV9008-2, uncomment one (only one) of the lines below: */
//#define ADRV9008_1
//#define ADRV9008_2

/* To build a specific example, uncomment one (only one) of the lines below: */
// #define DMA_EXAMPLE
// #define IIO_SUPPORT

#endif /* APP_CONFIG_H_ */
