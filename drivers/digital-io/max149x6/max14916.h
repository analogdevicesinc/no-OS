/***************************************************************************//**
 *   @file   max14916.h
 *   @brief  Header file of MAX14916 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#ifndef _MAX14916_H
#define _MAX14916_H

#include <stdint.h>
#include <stdbool.h>
#include "max149x6-base.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

#define MAX14916_CHANNELS		8

#define MAX14916_SETOUT_REG		0x0
#define MAX14916_SET_FLED_REG		0x1
#define MAX14916_SET_SLED_REG		0x2
#define MAX14916_INT_REG		0x3
#define MAX14916_OVR_LD_REG		0x4
#define MAX14916_CURR_LIM_REG		0x5
#define MAX14916_OW_OFF_FAULT_REG	0x6
#define MAX14916_OW_ON_FAULT_REG	0x7
#define MAX14916_SHD_VDD_FAULT_REG	0x8
#define MAX14916_GLOB_ERR_REG		0x9
#define MAX14916_OW_OFF_EN_REG		0xA
#define MAX14916_OW_ON_EN_REG		0xB
#define MAX14916_SHD_VDD_EN_REG		0xC
#define MAX14916_CONFIG1_REG		0xD
#define MAX14916_CONFIG2_REG		0xE
#define MAX14916_MASK_REG		0xF

#define MAX14916_SETOUT_MASK(x)		NO_OS_BIT(x)
#define MAX14916_SLED_CH_MASK(x)	NO_OS_BIT(x)
#define MAX14916_SLED_MASK		NO_OS_BIT(1)
#define MAX14916_FLED_MASK		NO_OS_BIT(0)
#define MAX14916_FLED_STRETCH_MASK	NO_OS_GENMASK(2, 1)
#define MAX14916_WD_MASK		NO_OS_GENMASK(7, 6)
#define MAX14916_OW_OFF_CS_MASK		NO_OS_GENMASK(5, 4)
#define MAX14916_SHT_VDD_THR_MASK	NO_OS_GENMASK(3, 2)

enum max14916_fled_time {
	MAX14916_FLED_TIME_DISABLED,
	MAX14916_FLED_TIME_1S,
	MAX14916_FLED_TIME_2S,
	MAX14916_FLED_TIME_3S
};

enum max14916_sled_state {
	MAX14916_SLED_OFF,
	MAX14916_SLED_ON
};

enum max14916_wd {
	MAX14916_WD_DISABLED,
	MAX14916_WD_200MS,
	MAX14916_WD_600MS,
	MAX14916_WD_1200MS
};

enum max14916_ow_off_cs {
	MAX14916_OW_OFF_CS_20UA,
	MAX14916_OW_OFF_CS_100UA,
	MAX14916_OW_OFF_CS_300UA,
	MAX14916_OW_OFF_CS_600UA
};

enum max14916_sht_vdd_thr {
	MAX14916_SHT_VDD_THR_9V,
	MAX14916_SHT_VDD_THR_10V,
	MAX14916_SHT_VDD_THR_12V,
	MAX14916_SHT_VDD_THR_14V
};

/** Read the state of a channel */
int max14916_ch_get(struct max149x6_desc *, uint32_t, uint32_t *);

/** Set the state of a channel */
int max14916_ch_set(struct max149x6_desc *, uint32_t, uint32_t);

/** Set SLED to on/off */
int max14916_sled_set(struct max149x6_desc *, uint32_t,
		      enum max14916_sled_state);

/** Set minimum on-time for the fault LEDs. */
int max14916_fled_time_set(struct max149x6_desc *, enum max14916_fled_time);

/** Set watchdog timeout for the MAX14916 device. */
int max14916_set_wd(struct max149x6_desc *, enum max14916_wd);

/** Set current magnitude for the MAX14916 device current source. */
int max14916_set_ow_off(struct max149x6_desc *, enum max14916_ow_off_cs);

/** Set threshold for short to vdd for the MAX14916 device. */
int max14916_set_sht_vdd_thr(struct max149x6_desc *, enum max14916_sht_vdd_thr);

/** Initialize and configure the MAX14916 device */
int max14916_init(struct max149x6_desc **, struct max149x6_init_param *);

/** Free the resources allocated during init. */
int max14916_remove(struct max149x6_desc *);

#endif
