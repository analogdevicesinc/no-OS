/***************************************************************************//**
 *   @file   ad2s1210.h
 *   @brief  Header file for the ad2s1210 driver
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 * Copyright (c) 2023 BayLibre, SAS.
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
#ifndef AD2S1210_H_
#define AD2S1210_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"

#define AD2S1210_REG_POSITION		0x80
#define AD2S1210_REG_VELOCITY		0x82
#define AD2S1210_REG_LOS_THRD		0x88
#define AD2S1210_REG_DOS_OVR_THRD	0x89
#define AD2S1210_REG_DOS_MIS_THRD	0x8A
#define AD2S1210_REG_DOS_RST_MAX_THRD	0x8B
#define AD2S1210_REG_DOS_RST_MIN_THRD	0x8C
#define AD2S1210_REG_LOT_HIGH_THRD	0x8D
#define AD2S1210_REG_LOT_LOW_THRD	0x8E
#define AD2S1210_REG_EXCIT_FREQ		0x91
#define AD2S1210_REG_CONTROL		0x92
#define AD2S1210_CONTROL_RES_MASK	NO_OS_GENMASK(1, 0)
#define AD2S1210_CONTROL_RES0_MASK	NO_OS_BIT(0)
#define AD2S1210_CONTROL_RES1_MASK	NO_OS_BIT(1)
#define AD2S1210_ENABLE_HYSTERESIS	NO_OS_BIT(4)

#define AD2S1210_REG_SOFT_RESET		0xF0
#define AD2S1210_REG_FAULT		0xFF

#define AD2S1210_REG_MIN		AD2S1210_REG_POSITION

#define AD2S1210_MIN_CLKIN	6144000
#define AD2S1210_MAX_CLKIN	10240000
#define AD2S1210_MIN_EXCIT	2000
#define AD2S1210_MAX_EXCIT	20000
#define AD2S1210_STEP_EXCIT	250
#define AD2S1210_MIN_FCW	0x4
#define AD2S1210_MAX_FCW	0x50

#define AD2S1210_POS_MASK	NO_OS_BIT(0)
#define AD2S1210_VEL_MASK	NO_OS_BIT(1)

enum ad2s1210_mode {
	MODE_POS,
	MODE_RESERVED,
	MODE_VEL,
	MODE_CONFIG,
};

enum ad2s1210_res {
	AD2S1210_RES_10BIT,
	AD2S1210_RES_12BIT,
	AD2S1210_RES_14BIT,
	AD2S1210_RES_16BIT,
};

enum ad2s1210_channel {
	AD2S1210_POS,
	AD2S1210_VEL,
};

struct ad2s1210_init_param {
	struct no_os_spi_init_param spi_init;
	struct no_os_gpio_init_param gpio_a0;
	struct no_os_gpio_init_param gpio_a1;
	struct no_os_gpio_init_param gpio_res0;
	struct no_os_gpio_init_param gpio_res1;
	struct no_os_gpio_init_param gpio_sample;
	int8_t resolution;
	uint32_t clkin_hz;
};

struct ad2s1210_dev {
	const char *name;
	bool have_mode_pins;
	bool have_resolution_pins;
	enum ad2s1210_mode mode;
	enum ad2s1210_res resolution;
	struct no_os_spi_desc *spi_desc;
	struct no_os_gpio_desc *gpio_a1;
	struct no_os_gpio_desc *gpio_a0;
	struct no_os_gpio_desc *gpio_res0;
	struct no_os_gpio_desc *gpio_res1;
	struct no_os_gpio_desc *gpio_sample;
	uint32_t clkin_hz;
};

int ad2s1210_init(struct ad2s1210_dev **dev,
		  struct ad2s1210_init_param *init_param);
int ad2s1210_remove(struct ad2s1210_dev *dev);
int ad2s1210_reg_write(struct ad2s1210_dev *dev, uint8_t addr,
		       uint8_t val);
int ad2s1210_reg_read(struct ad2s1210_dev *dev, uint8_t addr,
		      uint8_t *val);
int ad2s1210_spi_single_conversion(struct ad2s1210_dev *dev,
				   uint32_t active_mask,
				   void *data, uint32_t size);
int ad2s1210_hysteresis_is_enabled(struct ad2s1210_dev *dev);
int ad2s1210_set_hysteresis(struct ad2s1210_dev *dev, bool enable);
int ad2s1210_reinit_excitation_frequency(struct ad2s1210_dev *dev,
		uint16_t fexcit);
int ad2s1210_get_excitation_frequency(struct ad2s1210_dev *dev,
				      uint16_t *fexcit);
#endif
