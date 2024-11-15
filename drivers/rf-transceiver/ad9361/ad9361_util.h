/***************************************************************************//**
 *   @file   ad9361_util.h
 *   @brief  AD9361 Header file of Util driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
