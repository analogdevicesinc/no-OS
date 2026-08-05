/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  AD9081 profile & configuration.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Ramona Nechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#define MULTIDEVICE_INSTANCE_COUNT	1

#define ADXCVR_REF_CLK_KHZ	250000
#define ADXCVR_RX_DEV_CLK_KHZ	250000
#define ADXCVR_TX_DEV_CLK_KHZ	250000
#define ADXCVR_RX_LANE_CLK_KHZ	10000000
#define ADXCVR_TX_LANE_CLK_KHZ	10000000
#define AD9081_DAC_FREQUENCY	12000000000
#define AD9081_ADC_FREQUENCY	4000000000
#define AD9081_ADC_NYQUIST_ZONE	{0, 0, 0, 0}

/* TX path */

#define AD9081_TX_JESD_MODE		9
#define AD9081_TX_JESD_SUBCLASS		1
#define AD9081_TX_JESD_VERSION		1
#define AD9081_TX_JESD_M		8
#define AD9081_TX_JESD_F		4
#define AD9081_TX_JESD_K		32
#define AD9081_TX_JESD_N		16
#define AD9081_TX_JESD_NP		16
#define AD9081_TX_JESD_CS		0
#define AD9081_TX_JESD_L		4
#define AD9081_TX_JESD_S		1
#define AD9081_TX_JESD_HD		1
#define AD9081_TX_LOGICAL_LANE_MAPPING	{0, 2, 7, 7, 1, 7, 7, 3}

#define AD9081_JRX_TPL_PHASE_ADJUST 0x0c

#define AD9081_TX_MAIN_INTERPOLATION	6
#define AD9081_TX_CHAN_INTERPOLATION	8
#define AD9081_TX_MAIN_NCO_SHIFT	{2000000000, 2000000000, 2000000000, 2000000000}
#define AD9081_TX_CHAN_NCO_SHIFT	{0, 0, 0, 0, 0, 0, 0, 0}
#define AD9081_TX_CHAN_GAIN		{1024, 1024, 1024, 1024, 0, 0, 0, 0}

#define AD9081_TX_FSC	{0, 0, 0, 0}
#define AD9081_TX_DAC_CHAN_CROSSBAR	{0x1, 0x2, 0x4, 0x8}
#define AD9081_TX_DAC_1X_NON1X_CROSSBAR	{0x1, 0x2, 0x4, 0x8}

/* RX path */

#define AD9081_RX_JESD_MODE		10
#define AD9081_RX_JESD_SUBCLASS		1
#define AD9081_RX_JESD_VERSION		1
#define AD9081_RX_JESD_M		8
#define AD9081_RX_JESD_F		4
#define AD9081_RX_JESD_K		32
#define AD9081_RX_JESD_N		16
#define AD9081_RX_JESD_NP		16
#define AD9081_RX_JESD_CS		0
#define AD9081_RX_JESD_L		4
#define AD9081_RX_JESD_S		1
#define AD9081_RX_JESD_HD		1
#define AD9081_RX_LOGICAL_LANE_MAPPING	{2, 0, 7, 7, 7, 7, 3, 1}
#define AD9081_RX_LINK_CONVERTER_SELECT	{0, 1, 2, 3, 8, 9, 10, 11, 0, 0, 0, 0, 0, 0, 0, 0}

#define AD9081_RX_MAIN_DECIMATION	{4, 4, 4, 4}
#define AD9081_RX_CHAN_DECIMATION	{4, 4, 0, 0, 4, 4, 0, 0}
#define AD9081_RX_MAIN_ENABLE		{1, 1, 1, 1}
#define AD9081_RX_CHAN_ENABLE		{1, 1, 0, 0, 1, 1, 0, 0}
#define AD9081_RX_MAIN_NCO_SHIFT	{2000000000, 2000000000, 2000000000, 2000000000}
#define AD9081_RX_CHAN_NCO_SHIFT	{0, 0, 0, 0, 0, 0, 0, 0}

#endif
