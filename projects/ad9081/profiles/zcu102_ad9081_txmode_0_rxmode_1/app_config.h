/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  AD9081 profile & configuration.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Ramona Nechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#define MULTIDEVICE_INSTANCE_COUNT	1

#define ADXCVR_REF_CLK_KHZ	181250
#define ADXCVR_REF_CLK_RX_KHZ	181250
#define ADXCVR_REF_CLK_TX_KHZ	362424
#define ADXCVR_RX_DEV_CLK_KHZ	90625
#define ADXCVR_TX_DEV_CLK_KHZ	181250
#define ADXCVR_RX_LANE_CLK_KHZ	5981250
#define ADXCVR_TX_LANE_CLK_KHZ	11962500
#define AD9081_DAC_FREQUENCY	5800000000
#define AD9081_ADC_FREQUENCY	2900000000
#define AD9081_ADC_NYQUIST_ZONE	{0, 0, 0, 0}

/* TX path */

#define AD9081_TX_JESD_MODE		0
#define AD9081_TX_JESD_SUBCLASS		1
#define AD9081_TX_JESD_VERSION		2
#define AD9081_TX_JESD_M		8
#define AD9081_TX_JESD_F		12
#define AD9081_TX_JESD_K		64
#define AD9081_TX_JESD_N		12
#define AD9081_TX_JESD_NP		12
#define AD9081_TX_JESD_CS		0
#define AD9081_TX_JESD_L		1
#define AD9081_TX_JESD_S		1
#define AD9081_TX_JESD_HD		1
#define AD9081_TX_LOGICAL_LANE_MAPPING	{0, 2, 7, 7, 1, 7, 7, 3}

#define AD9081_JRX_TPL_PHASE_ADJUST 0x03

#define AD9081_TX_MAIN_INTERPOLATION	12
#define AD9081_TX_CHAN_INTERPOLATION	4
#define AD9081_TX_MAIN_NCO_SHIFT	{1000000000, 1000000000, 1000000000, 1000000000}
#define AD9081_TX_CHAN_NCO_SHIFT	{0, 0, 0, 0, 0, 0, 0, 0}
#define AD9081_TX_CHAN_GAIN		{2048, 2048, 2048, 2048, 0, 0, 0, 0}

#define AD9081_TX_FSC	{0, 0, 0, 0}
#define AD9081_TX_DAC_CHAN_CROSSBAR	{0x1, 0x2, 0x4, 0x8}
#define AD9081_TX_DAC_1X_NON1X_CROSSBAR	{0x1, 0x2, 0x4, 0x8}

/* RX path */

#define AD9081_RX_JESD_MODE		1
#define AD9081_RX_JESD_SUBCLASS		1
#define AD9081_RX_JESD_VERSION		2
#define AD9081_RX_JESD_M		8
#define AD9081_RX_JESD_F		12
#define AD9081_RX_JESD_K		64
#define AD9081_RX_JESD_N		12
#define AD9081_RX_JESD_NP		12
#define AD9081_RX_JESD_CS		0
#define AD9081_RX_JESD_L		1
#define AD9081_RX_JESD_S		1
#define AD9081_RX_JESD_HD		1
#define AD9081_RX_LOGICAL_LANE_MAPPING	{2, 0, 7, 7, 7, 7, 3, 1}
#define AD9081_RX_LINK_CONVERTER_SELECT	{0, 1, 2, 3, 8, 9, 10, 11, 0, 0, 0, 0, 0, 0, 0, 0}

#define AD9081_RX_MAIN_DECIMATION	{6, 6, 6, 6}
#define AD9081_RX_CHAN_DECIMATION	{8, 8, 0, 0, 8, 8, 0, 0}
#define AD9081_RX_MAIN_ENABLE		{1, 1, 1, 1}
#define AD9081_RX_CHAN_ENABLE		{1, 1, 0, 0, 1, 1, 0, 0}
#define AD9081_RX_MAIN_NCO_SHIFT	{1000000000, 1000000000, 1000000000, 1000000000}
#define AD9081_RX_CHAN_NCO_SHIFT	{0, 0, 0, 0, 0, 0, 0, 0}

#endif
