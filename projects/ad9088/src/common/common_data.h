/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by the ad9088 examples.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "adf4030.h"
#include "adf4382.h"
#include "hmc7044.h"
#include "ad9088.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_util.h"

#include "axi_dmac.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adxcvr.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"

#include "apollo_cpu_device_profile_types.h"

#define AD9088_JESD_SUBCLASS			1

/*
 * Profile-dependent link and clock-tree parameters.
 *
 * The AD9084-EBZ (Agilex 5 / Nios V) bitstream runs a JESD204B link (8b10b,
 * lane rate / 40), while the VCU118 (MicroBlaze) bitstream runs JESD204C
 * (64b66b, lane rate / 66). Every value that differs between the two profiles
 * lives in this one guarded block so both boards build correctly from the same
 * tree - keep it consistent with the per-board firmware .bin selected in
 * boards/basic_example/<board>.conf. All shared clock-tree topology (ADF4030
 * VCO/reference, channel assignment) stays common below.
 */
#ifdef CONFIG_ALTERA_PLATFORM_NIOSV
/* AD9084-EBZ (Agilex 5): JESD204B, L2-per-link, 10 Gbps lanes, 250 MHz link. */
#define AD9088_LANE_RATE_KHZ			10000000
#define AD9088_DEVICE_CLK_KHZ			(AD9088_LANE_RATE_KHZ / 40)
#define AD9088_SYSREF_CLK_HZ			1953125		/* 2.5 GHz VCO / 1280 */
#define AD9088_ADF4382_SPI_HZ			5000000
#define AD9088_ADF4382_FREQ_HZ			16000000000	/* AD9084 converter dev_clk = 16 GHz (N=128 @ 125 MHz PFD); matches profile dev_clk_freq_Hz */
#define AD9088_ADF4382_ID			ID_ADF4382
#define AD9088_HMC_SYSREF_DIV			1280		/* HMC7044 ch3  -> 1.953125 MHz */
#define AD9088_HMC_CORECLK_DIV			10		/* HMC7044 ch8-12 -> 250 MHz */
#define AD9088_ADXCVR_REF_KHZ			250000
/*
 * L2-per-link (204B) lane mapping of the AD9084-EBZ design; 11 marks an unused
 * lane. Matches the reference DT (socfpga_agilex5_socdk_ad9084.dts):
 * adi,jtx{0,1}-logical-lane-mapping / adi,jrx{0,1}-physical-lane-mapping.
 */
#define AD9088_TX0_LOGICAL_LANE_MAPPING		{11, 11, 11,  1, 11, 11, 11, 11,  2,  3, 11,  0}
#define AD9088_TX1_LOGICAL_LANE_MAPPING		{11, 11,  1,  0, 11,  2, 11,  3, 11, 11, 11, 11}
#define AD9088_RX0_PHYSICAL_LANE_MAPPING	{ 5,  1,  3,  7, 11, 11, 11, 11, 11, 11, 11, 11}
#define AD9088_RX1_PHYSICAL_LANE_MAPPING	{ 1,  7, 10,  3, 11, 11, 11, 11, 11, 11, 11, 11}
#else
/* VCU118 (MicroBlaze): JESD204C, 20.625 Gbps lanes, 312.5 MHz link. */
#define AD9088_LANE_RATE_KHZ			20625000
#define AD9088_DEVICE_CLK_KHZ			(AD9088_LANE_RATE_KHZ / 66)
#define AD9088_SYSREF_CLK_HZ			9765625
#define AD9088_ADF4382_SPI_HZ			1500000
#define AD9088_ADF4382_FREQ_HZ			20000000000
#define AD9088_ADF4382_ID			ID_ADF4382A
#define AD9088_HMC_SYSREF_DIV			512
#define AD9088_HMC_CORECLK_DIV			8
#define AD9088_ADXCVR_REF_KHZ			312500
#define AD9088_TX0_LOGICAL_LANE_MAPPING		{11, 2, 3, 5, 10, 1, 9, 0, 6, 7, 8, 4}
#define AD9088_TX1_LOGICAL_LANE_MAPPING		{3, 9, 5, 4, 2, 6, 1, 7, 8, 11, 0, 10}
#define AD9088_RX0_PHYSICAL_LANE_MAPPING	{10, 8, 9, 11, 5, 1, 3, 7, 4, 6, 2, 0}
#define AD9088_RX1_PHYSICAL_LANE_MAPPING	{4, 6, 2, 0, 1, 7, 10, 3, 5, 8, 9, 11}
#endif

/*
 * The axi_jesd204 cores only use AD9088_DEVICE_CLK_KHZ to print the "Reported
 * Link Clock" (lane rate / line-code divisor). SYSREF/BSYNC is generated by the
 * ADF4030 from a 2.5 GHz VCO off a 125 MHz reference and distributed to both the
 * Apollo (ch5) and the FPGA (ch8); the HMC7044 supplies the ADF4030's own BSYNC0
 * input on its ch3 and the reference clock on ch1. This topology is common to
 * both profiles - only the divide ratios above differ.
 */
#define ADF4030_VCO_FREQ_HZ			2500000000
#define ADF4030_REF_FREQ_HZ			125000000

/* ADF4030 channel assignment on this board */
#define ADF4030_CH_HMC_REF			0	/* ADF4030_SCLKOUT3 */
#define ADF4030_CH_APOLLO_SYSREF		5	/* APOLLO_SYSREF */
#define ADF4030_CH_FPGA_SYSREF			8	/* SYSREF_IN_F */

#define AD9088_NYQUIST_ZONE			1

extern struct adf4382_init_param 		adf4382_ip;
extern struct adf4030_init_param		adf4030_ip;
extern struct hmc7044_init_param		hmc7044_ip;
extern struct no_os_uart_init_param		platform_uart_ip;
extern struct axi_dmac_init 			rx_dmac_ip;
extern struct axi_dmac_init 			tx_dmac_ip;
extern struct jesd204_rx_init			rx_jesd204_ip;
extern struct jesd204_tx_init			tx_jesd204_ip;
extern struct adxcvr_init			rx_adxcvr_ip;
extern struct adxcvr_init			tx_adxcvr_ip;
extern struct ad9088_init_param			ad9088_ip;
extern struct axi_adc_init 			rx_adc_init;
extern struct axi_dac_init 			tx_dac_init;

/**
 * @brief Hand the AD9088 the clock chips its MCS calibration drives.
 *
 * Publishes the accessor tables into ad9088_ip along with the runtime device
 * handles, which do not exist until the chips are probed. Call this after
 * adf4030_init() and adf4382_init(), and before ad9088_init(). Skipping it
 * leaves MCS calibration disabled rather than broken.
 *
 * @param adf4030 - The BSYNC (SYSREF) provider.
 * @param adf4382 - The AD9088 device clock PLL.
 * @return        - 0 in case of success, negative error code otherwise.
 */
int ad9088_mcs_ops_bind(struct adf4030_dev *adf4030,
			struct adf4382_dev *adf4382);

#endif /* __COMMON_DATA_H__ */
