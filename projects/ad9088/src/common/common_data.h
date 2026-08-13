/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by adf4382 examples.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "jesd204_clk.h"

#include "apollo_cpu_device_profile_types.h"

/*UC42*/
#define AD9088_RX_JESD_F			1
#define AD9088_RX_JESD_K			256
#define AD9088_RX_JESD_SUBCLASS			1

#define AD9088_TX_JESD_F			1
#define AD9088_TX_JESD_K			256
#define AD9088_TX_JESD_M 			4
#define AD9088_TX_JESD_N			16
#define AD9088_TX_JESD_NP			16
#define AD9088_TX_JESD_HD			0
#define AD9088_TX_JESD_CS			0
#define AD9088_TX_JESD_SUBCLASS			1

#define AD9088_DEVICE_CLK_KHZ			20000000
#define AD9088_LANE_RATE_KHZ			20625000

/*
 * BSYNC / SYSREF rate. This is the 204C LEMC rate of the link:
 *   lane_rate / (66 * 32 * E) = 20.625e9 / 2112 = 9765625 Hz
 * It matches SYSREF_CLK_MHz in the kernel devicetree
 * (vcu118_ad9084_204C_M4_L8_NP16_20p0_4x2_CLL.dts). The ADF4030 distributes it
 * to both the Apollo (ch5) and the FPGA (ch8); the HMC7044 supplies the
 * ADF4030's own BSYNC0 input on its ch3 and the 125 MHz reference on ch1.
 */
#define AD9088_SYSREF_CLK_HZ			9765625
#define ADF4030_VCO_FREQ_HZ			2500000000
#define ADF4030_REF_FREQ_HZ			125000000

/* ADF4030 channel assignment, per the kernel devicetree */
#define ADF4030_CH_HMC_REF			0	/* ADF4030_SCLKOUT3 */
#define ADF4030_CH_APOLLO_SYSREF		5	/* APOLLO_SYSREF */
#define ADF4030_CH_FPGA_SYSREF			8	/* SYSREF_IN_F */

#define AD9088_MULTIDEVICE_INST_CNT		1
#define AD9088_NYQUIST_ZONE			1
#define AD9088_TX0_LOGICAL_LANE_MAPPING		{11, 2, 3, 5, 10, 1, 9, 0, 6, 7, 8, 4}
#define AD9088_TX1_LOGICAL_LANE_MAPPING		{3, 9, 5, 4, 2, 6, 1, 7, 8, 11, 0, 10}
#define AD9088_RX0_PHYSICAL_LANE_MAPPING	{10, 8, 9, 11, 5, 1, 3, 7, 4, 6, 2, 0}
#define AD9088_RX1_PHYSICAL_LANE_MAPPING	{4, 6, 2, 0, 1, 7, 10, 3, 5, 8, 9, 11}

extern struct no_os_spi_init_param		adf4382_spi_param;
extern struct adf4382_init_param 		adf4382_ip;
extern struct adf4030_init_param		adf4030_ip;
extern struct hmc7044_init_param		hmc7044_ip;
extern struct no_os_gpio_init_param		reset_gpio_param;
extern struct no_os_uart_init_param		platform_uart_ip;
extern struct axi_dmac_init 			rx_dmac_ip;
extern struct axi_dmac_init 			tx_dmac_ip;
extern struct jesd204_rx_init			rx_jesd204_ip;
extern struct jesd204_tx_init			tx_jesd204_ip;
extern struct adxcvr_init			rx_adxcvr_ip;
extern struct adxcvr_init			tx_adxcvr_ip;
extern struct ad9088_init_param			ad9088_ip;


#endif /* __COMMON_DATA_H__ */
