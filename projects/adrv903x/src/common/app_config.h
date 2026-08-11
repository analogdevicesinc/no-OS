/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  Application configuration parameters for ADRV903X project.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

/******************************************************************************/
/* UART baud rate for IIO example                                             */
/******************************************************************************/
#define UART_BAUDRATE				921600

/******************************************************************************/
/* SPI bus speeds                                                             */
/******************************************************************************/
#define AD9528_SPI_SPEED_HZ			10000000u
#define ADRV903X_SPI_SPEED_HZ			5000000u

/******************************************************************************/
/* Clock and lane rate configuration                                          */
/* From profile: ADRV903X_UC101_204B_4T4R1OR_NLS                             */
/******************************************************************************/
#define ADRV903X_DEVICE_CLK_KHZ			245760
#define ADRV903X_LANE_RATE_KHZ			16220160

/******************************************************************************/
/* JESD204 framer/deframer parameters                                        */
/* From profile: ADRV903X_UC101_204B_4T4R1OR_NLS                             */
/******************************************************************************/
#define ADRV903X_TX_JESD_OCTETS_PER_FRAME	4
#define ADRV903X_TX_JESD_FRAMES_PER_MULTIFRAME	64
#define ADRV903X_RX_JESD_OCTETS_PER_FRAME	4
#define ADRV903X_RX_JESD_FRAMES_PER_MULTIFRAME	64

/******************************************************************************/
/* Firmware and profile file names                                            */
/* Must match the fopen() mapping in no_os_platform.c                        */
/* Source: linux-adi/firmware/ and ADRV9030 customer API package             */
/******************************************************************************/
#define ADRV903X_CPU_FW_FILE			"ADRV9030_FW.bin"
#define ADRV903X_STREAM_FILE			"stream_image.bin"
#define ADRV903X_PROFILE_FILE			"DeviceProfileTest.bin"
#define ADRV903X_RX_GAIN_TABLE_FILE		"RxGainTable.csv"

/******************************************************************************/
/* Gain table channel mask: bit per RX channel (0xFF = all 8 channels)       */
/******************************************************************************/
#define ADRV903X_RX_GAIN_TABLE_MASK		0xFF

/******************************************************************************/
/* ADXCVR and JESD204 configuration                                           */
/* From profile: ADRV903X_UC101_204B_4T4R1OR_NLS                             */
/******************************************************************************/
#define ADRV903X_ADXCVR_REF_RATE_KHZ			491520	/* AD9528 ch13 REF_CLK0 */

/* TX JESD204 parameters */
#define ADRV903X_TX_JESD_SUBCLASS			1
#define ADRV903X_TX_JESD_CONVS_PER_DEVICE		4
#define ADRV903X_TX_JESD_CONV_RESOLUTION		16
#define ADRV903X_TX_JESD_BITS_PER_SAMPLE		16
#define ADRV903X_TX_JESD_HIGH_DENSITY			0
#define ADRV903X_TX_JESD_CTRL_BITS_PER_SAMPLE		0

/* RX JESD204 parameters */
#define ADRV903X_RX_JESD_SUBCLASS			1

/******************************************************************************/
/* AD9528 Clock Synthesizer configuration                                     */
/* From DTS: zynqmp-zcu102-rev10-adrv9032r-nls.dts                           */
/******************************************************************************/
#define AD9528_VCXO_FREQ_HZ			122880000
/* PLL1 */
#define AD9528_PLL1_FEEDBACK_DIV		4
#define AD9528_PLL1_CHARGE_PUMP_NA		5000
/* PLL2 */
#define AD9528_PLL2_VCO_DIV_M1_VAL		4
#define AD9528_PLL2_R1_DIV_VAL			1
#define AD9528_PLL2_NDIV_A			3
#define AD9528_PLL2_NDIV_B			27
#define AD9528_PLL2_N2_DIV_VAL			4
#define AD9528_PLL2_CHARGE_PUMP_NA		815000
/* SYSREF */
#define AD9528_SYSREF_K_DIV_VAL			512
/* Output channel dividers */
#define AD9528_DEV_SYSREF_DIV			4	/* ch0:  DEV_SYSREF  */
#define AD9528_DEV_CLK_DIV			4	/* ch1:  DEV_CLK → 245.76 MHz */
#define AD9528_CORE_CLK_DIV			4	/* ch3:  CORE_CLK   */
#define AD9528_REF_CLK1_DIV			2	/* ch11: REF_CLK1   */
#define AD9528_FPGA_SYSREF_DIV			4	/* ch12: FPGA_SYSREF */
#define AD9528_REF_CLK0_DIV			2	/* ch13: REF_CLK0   */

#endif /* APP_CONFIG_H_ */
