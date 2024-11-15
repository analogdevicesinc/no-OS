/***************************************************************************//**
 *   @file   max14906.h
 *   @brief  Header file of MAX14906 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MAX14906_H
#define _MAX14906_H

#include <stdint.h>
#include <stdbool.h>
#include "max149x6-base.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

#define MAX14906_CHANNELS		4

#define MAX14906_SETOUT_REG		0x0
#define MAX14906_SETLED_REG		0x1
#define MAX14906_DOILEVEL_REG		0x2
#define MAX14906_INT_REG		0x3
#define MAX14906_OVR_LD_REG		0x4
#define MAX14906_OPN_WIR_FLT_REG	0x5
#define MAX14906_SHD_VDD_FLT_REG	0x6
#define MAX14906_GLOBAL_FLT_REG		0x7
#define MAX14906_CONFIG1_REG		0xA
#define MAX14906_CONFIG2_REG		0xB
#define MAX14906_CONFIG_DI_REG		0xC
#define MAX14906_CONFIG_DO_REG		0xD
#define MAX14906_CONFIG_CURR_LIM	0xE
#define MAX14906_CONFIG_MASK		0xF

/* DoiLevel register */
#define MAX14906_DOI_LEVEL_MASK(x)	NO_OS_BIT(x)

/* SetOUT register */
#define MAX14906_HIGHO_MASK(x)		NO_OS_BIT(x)

#define MAX14906_DO_MASK(x)		(NO_OS_GENMASK(1, 0) << (2 * (x)))
#define MAX14906_CH_DIR_MASK(x)		NO_OS_BIT((x) + 4)
#define MAX14906_CH(x)			(x)
#define MAX14906_IEC_TYPE_MASK		NO_OS_BIT(7)
#define MAX14906_CL_MASK(x)		(NO_OS_GENMASK(1, 0) << (2 * (x)))

/* Config1 register */
#define MAX14906_SLED_MASK		NO_OS_BIT(1)
#define MAX14906_FLED_MASK		NO_OS_BIT(0)

enum max14906_iec_type {
	MAX14906_TYPE_1_3,
	MAX14906_TYPE_2,
};

/**
 * @brief Channel configuration options.
 */
enum max14906_function {
	MAX14906_OUT,
	MAX14906_IN,
	MAX14906_HIGH_Z
};

/**
 * @brief Configuration options for the output driver (on each channel).
 */
enum max14906_do_mode {
	MAX14906_HIGH_SIDE,
	MAX14906_HIGH_SIDE_INRUSH,
	MAX14906_PUSH_PULL_CLAMP,
	MAX14906_PUSH_PULL
};

/**
 * @brief Current limit options for output channels.
 */
enum max14906_climit {
	MAX14906_CL_600,
	MAX14906_CL_130,
	MAX14906_CL_300,
	MAX14906_CL_1200,
};

/** Read the state of a channel */
int max14906_ch_get(struct max149x6_desc *, uint32_t, uint32_t *);

/** Set the state of a channel */
int max14906_ch_set(struct max149x6_desc *, uint32_t, uint32_t);

/** Configure a channel's function */
int max14906_ch_func(struct max149x6_desc *, uint32_t, enum max14906_function);

/** Configure the current limit for output channels */
int max14906_climit_set(struct max149x6_desc *, uint32_t, enum max14906_climit);

/** Read an output channel's current limit */
int max14906_climit_get(struct max149x6_desc *, uint32_t,
			enum max14906_climit *);

/** Initialize and configure the MAX14906 device */
int max14906_init(struct max149x6_desc **, struct max149x6_init_param *);

/** Free the resources allocated during init and place all the channels in high-z. */
int max14906_remove(struct max149x6_desc *);

#endif
