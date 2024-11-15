/***************************************************************************//**
 *   @file   adrv902x/src/common/app_config.h
 *   @brief  Config file for the ADRV902x project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

//#define IIO_SUPPORT

//#define DMA_EXAMPLE

#ifdef PLATFORM_MB
#define UART_BAUDRATE                                   115200
#else
#define UART_BAUDRATE                                   921600
#endif

#define ADRV9025_DEVICE_CLK_KHZ                         245760
#define ADRV9025_LANE_RATE_KHZ				9830400

#define ADRV9025_TX_JESD_OCTETS_PER_FRAME		4
#define ADRV9025_TX_JESD_FRAMES_PER_MULTIFRAME		32
#define ADRV9025_TX_JESD_CONVS_PER_DEVICE		8
#define ADRV9025_TX_JESD_CONV_RESOLUTION		16
#define ADRV9025_TX_JESD_BITS_PER_SAMPLE		16
#define ADRV9025_TX_JESD_HIGH_DENSITY			1
#define ADRV9025_TX_JESD_CTRL_BITS_PER_SAMPLE		0
#define ADRV9025_TX_JESD_SUBCLASS			1

#define ADRV9025_RX_JESD_OCTETS_PER_FRAME		4
#define ADRV9025_RX_JESD_FRAMES_PER_MULTIFRAME		32
#define ADRV9025_RX_JESD_SUBCLASS		        1

#define ADRV9025_STREAM_IMAGE_FILE			"ADRV9025_stream_image.bin"

#endif /* APP_CONFIG_H_ */
