/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  Application configuration.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#ifdef QUAD_MXFE
#define MULTIDEVICE_INSTANCE_COUNT	4

#define CLKIN_J41_FREQ_HZ		500000000

#define ADF4371_RFx_FREQUENCY_HZ	12000000000

#define HMC7043_FPGA_XCVR_CLKDIV	2
#define HMC7043_FPGA_LINK_CLKDIV_TX	2
#define HMC7043_FPGA_LINK_CLKDIV_RX	2
#define HMC7043_SYSREF_CLKDIV		256

#define ADXCVR_REF_CLK_KHZ	CLKIN_J41_FREQ_HZ
#define ADXCVR_RX_DEV_CLK_KHZ	250000
#define ADXCVR_TX_DEV_CLK_KHZ	250000
#define ADXCVR_RX_LANE_CLK_KHZ	16500000
#define ADXCVR_TX_LANE_CLK_KHZ	16500000

#define AD9081_DAC_FREQUENCY	ADF4371_RFx_FREQUENCY_HZ
#define AD9081_ADC_FREQUENCY	(ADF4371_RFx_FREQUENCY_HZ / 3)
#else
#define MULTIDEVICE_INSTANCE_COUNT	1

#define ADXCVR_REF_CLK_KHZ	500000
#define ADXCVR_RX_DEV_CLK_KHZ	250000
#define ADXCVR_TX_DEV_CLK_KHZ	250000
#define ADXCVR_RX_LANE_CLK_KHZ	10000000
#define ADXCVR_TX_LANE_CLK_KHZ	10000000

#define AD9081_DAC_FREQUENCY	12000000000
#define AD9081_ADC_FREQUENCY	4000000000
#endif

#ifdef QUAD_MXFE
#define AD9081_ADC_NYQUIST_ZONE	{1, 1, 1, 1}
#else
#define AD9081_ADC_NYQUIST_ZONE	{0, 0, 0, 0}
#endif

/* TX path */

#define AD9081_TX_JESD_MODE		11
#define AD9081_TX_JESD_SUBCLASS		1
#define AD9081_TX_JESD_VERSION		2
#define AD9081_TX_JESD_M		16
#define AD9081_TX_JESD_F		8
#define AD9081_TX_JESD_K		32
#define AD9081_TX_JESD_N		16
#define AD9081_TX_JESD_NP		16
#define AD9081_TX_JESD_CS		0
#define AD9081_TX_JESD_L		4
#define AD9081_TX_JESD_S		1
#define AD9081_TX_JESD_HD		1
#ifdef QUAD_MXFE
#define AD9081_TX_LOGICAL_LANE_MAPPING	{0, 1, 2, 3, 4, 5, 6, 7}
#else
#define AD9081_TX_LOGICAL_LANE_MAPPING	{0, 2, 7, 7, 1, 7, 7, 3}
#endif

#define AD9081_JRX_TPL_PHASE_ADJUST	15

#define AD9081_TX_MAIN_INTERPOLATION	6
#define AD9081_TX_CHAN_INTERPOLATION	8
#define AD9081_TX_MAIN_NCO_SHIFT	{2000000000, 2000000000, 2000000000, 2000000000}
#define AD9081_TX_CHAN_NCO_SHIFT	{0, 0, 0, 0, 0, 0, 0, 0}
#define AD9081_TX_CHAN_GAIN		{1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024}

#define AD9081_TX_FSC	{0, 0, 0, 0}

#define AD9081_TX_DAC_CHAN_CROSSBAR	{0x3, 0xc, 0x30, 0xc0}

#define AD9081_TX_DAC_1X_NON1X_CROSSBAR	{0x1, 0x2, 0x4, 0x8}

/* RX path */

#define AD9081_RX_JESD_MODE		4
#define AD9081_RX_JESD_SUBCLASS		1
#define AD9081_RX_JESD_VERSION		2
#define AD9081_RX_JESD_M		8
#define AD9081_RX_JESD_F		8
#define AD9081_RX_JESD_K		32
#define AD9081_RX_JESD_N		16
#define AD9081_RX_JESD_NP		16
#define AD9081_RX_JESD_CS		0
#define AD9081_RX_JESD_L		2
#define AD9081_RX_JESD_S		1
#define AD9081_RX_JESD_HD		1
#ifdef QUAD_MXFE
#define AD9081_RX_LOGICAL_LANE_MAPPING	{0, 1, 2, 3, 4, 5, 6, 7}
#else
#define AD9081_RX_LOGICAL_LANE_MAPPING	{2, 0, 7, 7, 7, 7, 3, 1}
#endif
#define AD9081_RX_LINK_CONVERTER_SELECT	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

#define AD9081_RX_MAIN_DECIMATION	{4, 4, 4, 4}
#define AD9081_RX_CHAN_DECIMATION	{4, 4, 0, 0, 4, 4, 0, 0}
#define AD9081_RX_MAIN_ENABLE		{1, 1, 1, 1}
#define AD9081_RX_CHAN_ENABLE		{1, 1, 0, 0, 1, 1, 0, 0}
#define AD9081_RX_MAIN_NCO_SHIFT	{2000000000, 2000000000, 2000000000, 2000000000}
#define AD9081_RX_CHAN_NCO_SHIFT	{0, 0, 0, 0, 0, 0, 0, 0}

#endif