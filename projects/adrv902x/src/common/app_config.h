/***************************************************************************//**
 *   @file   adrv902x/src/common/app_config.h
 *   @brief  Config file for the ADRV902x project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#ifdef PLATFORM_MB
#define UART_BAUDRATE                                   115200
#else
#define UART_BAUDRATE                                   921600
#endif

#define ADRV9025_DEVICE_CLK_KHZ                         245760

#if defined(JESD204B_NP12_PROFILE)
#define ADRV9025_LANE_RATE_KHZ				14745600
#elif defined(JESD204C_PROFILE)
#define ADRV9025_LANE_RATE_KHZ				16220160
#else
#define ADRV9025_LANE_RATE_KHZ				9830400
#endif

#if defined(JESD204B_NP12_PROFILE)
#define ADRV9025_TX_JESD_OCTETS_PER_FRAME		3
#else
#define ADRV9025_TX_JESD_OCTETS_PER_FRAME		4
#endif

#if defined(JESD204C_PROFILE)
#define ADRV9025_TX_JESD_FRAMES_PER_MULTIFRAME		64
#else
#define ADRV9025_TX_JESD_FRAMES_PER_MULTIFRAME		32
#endif

#define ADRV9025_TX_JESD_CONVS_PER_DEVICE		8
#if defined(JESD204B_NP12_PROFILE)
#define ADRV9025_TX_JESD_CONV_RESOLUTION		12
#define ADRV9025_TX_JESD_BITS_PER_SAMPLE		12
#else
#define ADRV9025_TX_JESD_CONV_RESOLUTION		16
#define ADRV9025_TX_JESD_BITS_PER_SAMPLE		16
#endif
#define ADRV9025_TX_JESD_HIGH_DENSITY			1
#define ADRV9025_TX_JESD_CTRL_BITS_PER_SAMPLE		0
#define ADRV9025_TX_JESD_SUBCLASS			1

#if defined(JESD204B_NP12_PROFILE)
#define ADRV9025_RX_JESD_OCTETS_PER_FRAME		6
#elif defined(JESD204C_PROFILE)
#define ADRV9025_RX_JESD_OCTETS_PER_FRAME		8
#else
#define ADRV9025_RX_JESD_OCTETS_PER_FRAME		4
#endif
#define ADRV9025_RX_JESD_FRAMES_PER_MULTIFRAME		32
/*
 * The main-Rx converter count is derived from the bitstream's Rx ADC-TPL core
 * at configure time (-DADRV9025_RX_JESD_CONVS_PER_DEVICE, see CMakeLists.txt /
 * scripts/xsa_profile.sh). The fallback covers a manual/IDE build that did not
 * run the probe; all current reference designs expose M=8 (all 4 Rx, I+Q).
 */
#ifndef ADRV9025_RX_JESD_CONVS_PER_DEVICE
#if defined(JESD204B_NP12_PROFILE) || defined(JESD204C_PROFILE)
#define ADRV9025_RX_JESD_CONVS_PER_DEVICE		8
#else
#define ADRV9025_RX_JESD_CONVS_PER_DEVICE		4
#endif
#endif

#define ADRV9025_RX_JESD_SUBCLASS			1

#if defined(JESD204B_NP12_PROFILE)
#define ADRV9025_ORX_JESD_OCTETS_PER_FRAME		3
#else
#define ADRV9025_ORX_JESD_OCTETS_PER_FRAME		4
#endif

#if defined(JESD204C_PROFILE)
#define ADRV9025_ORX_JESD_FRAMES_PER_MULTIFRAME	64
#else
#define ADRV9025_ORX_JESD_FRAMES_PER_MULTIFRAME	32
#endif

#define ADRV9025_ORX_JESD_CONVS_PER_DEVICE		4

#define ADRV9025_STREAM_IMAGE_FILE			"ADRV9025_stream_image.bin"

#endif /* APP_CONFIG_H_ */
