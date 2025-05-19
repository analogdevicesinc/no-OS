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
#include "adf4382.h"
#include "hmc7044.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_util.h"

#include "axi_dmac.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"

/*UC42*/
#define AD9084_RX_JESD_F			2
#define AD9084_RX_JESD_K			128
#define AD9084_RX_JESD_SUBCLASS			0

#define AD9084_TX_JESD_F			2
#define AD9084_TX_JESD_K			128
#define AD9084_TX_JESD_M 			4
#define AD9084_TX_JESD_N			16
#define AD9084_TX_JESD_NP			16
#define AD9084_TX_JESD_HD			0
#define AD9084_TX_JESD_CS			0
#define AD9084_TX_JESD_SUBCLASS			0

#define AD9084_DEVICE_CLK_KHZ			20000000
#define AD9084_LANE_RATE_KHZ			20625000

extern struct no_os_spi_init_param		adf4382_spi_param;
extern struct adf4382_init_param 		adf4382_ip;
extern struct hmc7044_init_param		hmc7044_ip;
extern struct no_os_gpio_init_param		reset_gpio_param;
extern struct no_os_uart_init_param		platform_uart_ip;
extern struct axi_dmac_init 			rx_dmac_ip;
extern struct axi_dmac_init 			tx_dmac_ip;
extern struct jesd204_rx_init			rx_jesd204_ip;
extern struct jesd204_tx_init			tx_jesd204_ip;


#endif /* __COMMON_DATA_H__ */
