/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  AD9081 profile & configuration for JESD204C TX Mode 10, RX Mode 11
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

/* Clock configuration - adjust based on your hardware setup */
#define ADXCVR_REF_CLK_KHZ	250000
#define ADXCVR_RX_DEV_CLK_KHZ	250000
#define ADXCVR_TX_DEV_CLK_KHZ	250000
#define ADXCVR_RX_LANE_CLK_KHZ	16500000
#define ADXCVR_TX_LANE_CLK_KHZ	16500000
#define AD9081_DAC_FREQUENCY	4000000000
#define AD9081_ADC_FREQUENCY	4000000000
#define AD9081_ADC_NYQUIST_ZONE	{1, 1, 1, 1}

/* TX path - JESD204C Mode 10 Configuration */
/* From Table 93: JESD204C Mode 10 - L=4, M=4, F=2, S=1, K=128, E=1, N=16, NP=16 */

#define AD9081_TX_JESD_MODE		10
#define AD9081_TX_JESD_SUBCLASS		1
#define AD9081_TX_JESD_VERSION		2  /* JESD204C */
#define AD9081_TX_JESD_M		4  /* Number of converters */
#define AD9081_TX_JESD_F		2  /* Octets per frame */
#define AD9081_TX_JESD_K		128 /* Frames per multiframe */
#define AD9081_TX_JESD_N		16  /* Converter resolution */
#define AD9081_TX_JESD_NP		16  /* Bits per sample */
#define AD9081_TX_JESD_CS		0   /* Control bits per sample */
#define AD9081_TX_JESD_L		4   /* Number of lanes */
#define AD9081_TX_JESD_S		1   /* Samples per converter per frame */
#define AD9081_TX_JESD_HD		1   /* High density mode */

/* For JESD204C, use logical lane mapping consistent with Linux devicetree */
#define AD9081_TX_LOGICAL_LANE_MAPPING	{0, 2, 7, 6, 1, 5, 4, 3}

#define AD9081_JRX_TPL_PHASE_ADJUST 15

/* TX Datapath Configuration */
/* Mode 10 supports various interpolation combinations */
/* Using 4×2 (coarse=4, fine=2) for total interpolation of 8 */
#define AD9081_TX_MAIN_INTERPOLATION	12  /* Coarse interpolation */
#define AD9081_TX_CHAN_INTERPOLATION	1  /* Fine interpolation */
#define AD9081_TX_MAIN_NCO_SHIFT	{2800000000, 2800000000, 2800000000, 2800000000}
#define AD9081_TX_CHAN_NCO_SHIFT	{0, 0, 0, 0, 0, 0, 0, 0}
#define AD9081_TX_CHAN_GAIN		{1024, 1024, 1024, 1024, 0, 0, 0, 0}

#define AD9081_TX_FSC	{0, 0, 0, 0}
#define AD9081_TX_DAC_CHAN_CROSSBAR	{0x1, 0x2, 0x3, 0x3}
#define AD9081_TX_DAC_1X_NON1X_CROSSBAR	{0x1, 0x2, 0x4, 0x8}


/* RX path - JESD204C Mode 11 Configuration */

#define AD9081_RX_JESD_MODE		11
#define AD9081_RX_JESD_SUBCLASS		1
#define AD9081_RX_JESD_VERSION		2  /* JESD204C */
#define AD9081_RX_JESD_M		4 /* Number of converters */
#define AD9081_RX_JESD_F		2  /* Octets per frame */
#define AD9081_RX_JESD_K		128 /* Frames per multiframe */
#define AD9081_RX_JESD_N		16 /* Converter resolution */
#define AD9081_RX_JESD_NP		16 /* Bits per sample */
#define AD9081_RX_JESD_CS		0  /* Control bits per sample */
#define AD9081_RX_JESD_L		4  /* Number of lanes */
#define AD9081_RX_JESD_S		1  /* Samples per converter per frame */
#define AD9081_RX_JESD_HD		1  /* High density mode */

/* For JESD204C, use logical lane mapping consistent with Linux devicetree */
#define AD9081_RX_LOGICAL_LANE_MAPPING	{2, 0, 7, 6, 5, 4, 3, 1}

/* RX converter to link mapping - map 16 converters across the link */
#define AD9081_RX_LINK_CONVERTER_SELECT	{0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/* RX Datapath Configuration */
/* Mode 11 supports various decimation combinations */
/* Using 4×4 (coarse=4, fine=4) for total decimation of 16 */
#define AD9081_RX_MAIN_DECIMATION      {4, 4, 4, 4}
#define AD9081_RX_CHAN_DECIMATION      {1, 1, 1, 1, 0, 0, 0, 0}
#define AD9081_RX_MAIN_ENABLE		{1, 1, 1, 1}
#define AD9081_RX_CHAN_ENABLE		{1, 1, 1, 1, 0, 0, 0, 0}
#define AD9081_RX_MAIN_NCO_SHIFT	{1000000000, 1000000000, 1000000000, 1000000000}
#define AD9081_RX_CHAN_NCO_SHIFT	{0, 0, 0, 0, 0, 0, 0, 0}

#endif /* APP_CONFIG_H_ */
