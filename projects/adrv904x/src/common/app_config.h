/***************************************************************************//**
 *   @file   adrv904x/src/common/app_config.h
 *   @brief  Config file for the ADRV904x project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

//#define IIO_SUPPORT

//#define DMA_EXAMPLE

#define UART_BAUDRATE                                   921600

#define ADRV904X_DEVICE_CLK_KHZ                         245760
#define ADRV904X_LANE_RATE_KHZ				16220160

#define ADRV904X_TX_JESD_OCTETS_PER_FRAME		4
#define ADRV904X_TX_JESD_FRAMES_PER_MULTIFRAME	64
#define ADRV904X_TX_JESD_CONVS_PER_DEVICE		16
#define ADRV904X_TX_JESD_CONV_RESOLUTION		16
#define ADRV904X_TX_JESD_BITS_PER_SAMPLE		16
#define ADRV904X_TX_JESD_HIGH_DENSITY			1
#define ADRV904X_TX_JESD_CTRL_BITS_PER_SAMPLE		0
#define ADRV904X_TX_JESD_SUBCLASS			1

#define ADRV904X_RX_JESD_OCTETS_PER_FRAME		4
#define ADRV904X_RX_JESD_FRAMES_PER_MULTIFRAME		64
#define ADRV904X_RX_JESD_SUBCLASS		        1

#endif /* APP_CONFIG_H_ */
