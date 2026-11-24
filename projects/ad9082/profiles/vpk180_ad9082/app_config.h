/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  AD9082 profile & configuration for VPK180 (Versal).
 *
 *           JESD204C M=8, L=8, S=2, 24.75 Gbps (TX link mode 24, RX link
 *           mode 26), as in the Linux versal-vpk180-reva-ad9082.dts.  The XSA
 *           must be built for M=8 (HDL ad9082_fmca_ebz/vpk180 with
 *           RX_JESD_M=8 RX_JESD_S=2 TX_JESD_M=8 TX_JESD_S=2).
 *
 *           TX: DAC 12 GHz, interpolation 6x1 (UG-1578 Table 93, mode 24).
 *           RX: ADC 6 GHz, decimation 3x1 (UG-1578 Table 76, mode 26), so
 *           the DDCs run and the RX data is complex I/Q at 2 GSPS.  The
 *           1 GHz TX main NCO and the 1 GHz CDDC0 NCO bring a DAC0 -> ADC0
 *           loopback tone back to baseband.
 *
 *           HMC7044 (PLL2 = 3000 MHz): DEV_REFCLK/FPGA_REFCLK = 375 MHz,
 *           CORE_CLK = 250 MHz, FPGA_REFCLK1 = 750 MHz.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#define MULTIDEVICE_INSTANCE_COUNT	1

#define ADXCVR_REF_CLK_KHZ	375000
#define ADXCVR_RX_DEV_CLK_KHZ	250000
#define ADXCVR_TX_DEV_CLK_KHZ	250000
#define ADXCVR_RX_LANE_CLK_KHZ	24750000
#define ADXCVR_TX_LANE_CLK_KHZ	24750000

#define AD9081_DAC_FREQUENCY	12000000000
#define AD9081_ADC_FREQUENCY	6000000000
#define AD9081_ADC_NYQUIST_ZONE	{0, 0, 0, 0}

/* TX path */

#define AD9081_TX_JESD_MODE		24
#define AD9081_TX_JESD_SUBCLASS		1
#define AD9081_TX_JESD_VERSION		2
#define AD9081_TX_JESD_M		8
#define AD9081_TX_JESD_F		3
#define AD9081_TX_JESD_K		256
#define AD9081_TX_JESD_N		12
#define AD9081_TX_JESD_NP		12
#define AD9081_TX_JESD_CS		0
#define AD9081_TX_JESD_L		8
#define AD9081_TX_JESD_S		2
#define AD9081_TX_JESD_HD		0
#define AD9081_TX_LOGICAL_LANE_MAPPING	{0, 2, 7, 6, 1, 5, 4, 3}

#define AD9081_JRX_TPL_PHASE_ADJUST	0x3b

#define AD9081_TX_MAIN_INTERPOLATION	6
#define AD9081_TX_CHAN_INTERPOLATION	1
#define AD9081_TX_MAIN_NCO_SHIFT	{1000000000, 1100000000, 1200000000, 1300000000}
#define AD9081_TX_CHAN_NCO_SHIFT	{0, 0, 0, 0, 0, 0, 0, 0}
#define AD9081_TX_CHAN_GAIN		{2048, 2048, 2048, 2048, 0, 0, 0, 0}

#define AD9081_TX_FSC	{0, 0, 0, 0}
#define AD9081_TX_DAC_CHAN_CROSSBAR	{0x1, 0x2, 0x4, 0x8}
#define AD9081_TX_DAC_1X_NON1X_CROSSBAR	{0x1, 0x2, 0x4, 0x8}

/* RX path */

#define AD9081_RX_JESD_MODE		26
#define AD9081_RX_JESD_SUBCLASS		1
#define AD9081_RX_JESD_VERSION		2
#define AD9081_RX_JESD_M		8
#define AD9081_RX_JESD_F		3
#define AD9081_RX_JESD_K		256
#define AD9081_RX_JESD_N		12
#define AD9081_RX_JESD_NP		12
#define AD9081_RX_JESD_CS		0
#define AD9081_RX_JESD_L		8
#define AD9081_RX_JESD_S		2
#define AD9081_RX_JESD_HD		0
#define AD9081_RX_LOGICAL_LANE_MAPPING	{2, 0, 7, 6, 5, 4, 3, 1}
/* FDDC0 I/Q, FDDC1 I/Q, FDDC4 I/Q, FDDC5 I/Q (as in the Linux DTS) */
#define AD9081_RX_LINK_CONVERTER_SELECT	{0, 1, 2, 3, 8, 9, 10, 11, 0, 0, 0, 0, 0, 0, 0, 0}

#define AD9081_RX_MAIN_DECIMATION	{3, 3, 3, 3}
#define AD9081_RX_MAIN_NCO_SHIFT	{1000000000, -400000000, 100000000, 100000000}
#define AD9081_RX_CHAN_DECIMATION	{1, 1, 0, 0, 1, 1, 0, 0}
#define AD9081_RX_MAIN_ENABLE		{1, 1, 1, 1}
#define AD9081_RX_CHAN_ENABLE		{1, 1, 0, 0, 1, 1, 0, 0}
#define AD9081_RX_CHAN_NCO_SHIFT	{0, 0, 0, 0, 0, 0, 0, 0}

#endif
