/***************************************************************************//**
 *   @file   ad9172.h
 *   @brief  Header file of ad9172 Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#ifndef __AD9172_H__
#define __AD9172_H__

#include "AD917x.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"

typedef struct ad9172_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_reset;
	struct no_os_gpio_desc	*gpio_txen0;
	struct no_os_gpio_desc	*gpio_txen1;
	struct ad9172_state *st;
	struct jesd204_dev *jdev;
} ad9172_dev;

enum chip_id {
	CHIPID_AD9171 = 0x71,
	CHIPID_AD9172 = 0x72,
	CHIPID_AD9173 = 0x73,
	CHIPID_AD9174 = 0x74,
	CHIPID_AD9175 = 0x75,
	CHIPID_AD9176 = 0x76,
};

struct ad9172_state {
	enum chip_id id;
	ad917x_handle_t dac_h;
	jesd_param_t appJesdConfig;
	uint32_t dac_rate_khz;
	uint64_t dac_clkin_Hz;
	uint32_t dac_interpolation;
	uint32_t channel_interpolation;
	uint32_t interpolation;
	uint32_t jesd_link_mode;
	uint32_t jesd_dual_link_mode;
	uint32_t jesd_subclass;
	uint32_t clock_output_config;
	uint32_t scrambling;
	uint32_t sysref_mode;
	bool pll_bypass;
	signal_type_t syncoutb_type;
	signal_coupling_t sysref_coupling;
	uint8_t nco_main_enable;
	uint8_t nco_channel_enable;
	uint8_t logic_lanes[8];
	struct no_os_clk_desc *dac_clk;
	struct no_os_clk_desc *clk_data;
	struct ad9172_dev *parent;
};

typedef struct ad9172_init_param {
	/* SPI */
	struct no_os_spi_init_param *spi_init;
	/* GPIO */
	struct no_os_gpio_init_param gpio_txen0;
	struct no_os_gpio_init_param gpio_txen1;
	struct no_os_gpio_init_param gpio_reset;
	struct no_os_clk_desc *dac_clk;
	struct no_os_clk_desc *clk_data;
	uint32_t dac_rate_khz;
	uint32_t dac_clkin_Hz;
	uint32_t jesd_link_mode;
	uint32_t jesd_subclass;
	uint32_t dac_interpolation;
	uint32_t channel_interpolation;
	uint32_t clock_output_config;
	uint32_t scrambling;
	uint32_t sysref_mode;
	bool pll_bypass;
	signal_type_t syncoutb_type;
	signal_coupling_t sysref_coupling;
	uint8_t logic_lanes_mapping;
	uint8_t logic_lanes[8];
	bool use_jesd_fsm;
} ad9172_init_param;

int32_t ad9172_init(ad9172_dev **device,
		    ad9172_init_param *init_param);
int32_t ad9172_remove(ad9172_dev *device);
#endif // __AD9172_H__
