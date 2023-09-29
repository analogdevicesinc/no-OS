/***************************************************************************//**
 *   @file   max14906.h
 *   @brief  Header file of MAX14906 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
