/***************************************************************************//**
 *   @file   ad9361_util.h
 *   @brief  AD9361 Header file of Util driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef __AD9361_UTIL_H__
#define __AD9361_UTIL_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ad9361.h"
#include "common.h"
#include "app_config.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define CLK_IGNORE_UNUSED						NO_OS_BIT(3)
#define CLK_GET_RATE_NOCACHE					NO_OS_BIT(6)

#if defined(HAVE_VERBOSE_MESSAGES)
#define dev_err(dev, format, ...)		({printf(format, ## __VA_ARGS__);printf("\n"); })
#define dev_warn(dev, format, ...)		({printf(format, ## __VA_ARGS__);printf("\n"); })
#if defined(HAVE_DEBUG_MESSAGES)
#define dev_dbg(dev, format, ...)		({printf(format, ## __VA_ARGS__);printf("\n"); })
#else
#define dev_dbg(dev, format, ...)	({ if (0) printf(format, ## __VA_ARGS__); })
#endif
#define printk(format, ...)			printf(format, ## __VA_ARGS__)
#else
#define dev_err(dev, format, ...)	({ if (0) printf(format, ## __VA_ARGS__); })
#define dev_warn(dev, format, ...)	({ if (0) printf(format, ## __VA_ARGS__); })
#define dev_dbg(dev, format, ...)	({ if (0) printf(format, ## __VA_ARGS__); })
#define printk(format, ...)			({ if (0) printf(format, ## __VA_ARGS__); })
#endif

struct device {
};

struct spi_device {
	struct device	dev;
	uint8_t 		id_no;
};

struct axiadc_state {
	struct ad9361_rf_phy	*phy;
	uint32_t				pcore_version;
};

struct axiadc_chip_info {
	char		*name;
	int32_t		num_channels;
};

struct axiadc_converter {
	struct axiadc_chip_info	*chip_info;
	uint32_t				scratch_reg[16];
};

#ifdef WIN32
#include "basetsd.h"
typedef SSIZE_T ssize_t;
#define strsep(s, ct)				0
#define snprintf(s, n, format, ...)	0
#define __func__ __FUNCTION__
#endif

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t clk_prepare_enable(struct no_os_clk *clk);
uint32_t clk_get_rate(struct ad9361_rf_phy *phy,
		      struct refclk_scale *clk_priv);
int32_t no_os_clk_set_rate(struct ad9361_rf_phy *phy,
			   struct refclk_scale *clk_priv,
			   uint32_t rate);
uint32_t int_sqrt(uint32_t x);
int32_t ilog2(int32_t x);
uint32_t find_first_bit(uint32_t word);
void * ERR_PTR(long error);

#endif
