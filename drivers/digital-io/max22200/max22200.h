/***************************************************************************//**
 *   @file   max22200.h
 *   @brief  Header file of MAX22200 Driver.
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
#ifndef _MAX22200_H
#define _MAX22200_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

#define MAX22200_FRAME_SIZE		4

#define MAX22200_CHANNELS		8
#define MAX22200_CHANNELS_CONFIG	4
#define MAX22200_MAX_CHN_IDX		7
#define MAX22200_HIT_MAX_VAL		0x7F
#define MAX22200_HIT_NO_TIME		0x00

#define MAX22200_R1B			NO_OS_BIT(0)

#define MAX22200_STATUS_REG		0x00
#define MAX22200_CFG_CH(x)		(0x02 + (0x02 * (x)))
#define MAX22200_FAULT_REG		0x0C
#define MAX22200_CFG_DPM_REG		0x0E

/* Write/Read Masks */
#define MAX22200_RW_MASK		NO_OS_BIT(7)
#define MAX22200_ADDR_MASK		NO_OS_GENMASK(6, 0)

/* Status Register Masks */
#define MAX22200_ONCH_MASK(x)		NO_OS_BIT((x) + 24)
#define MAX22200_STATUS_ONCH_MASK	NO_OS_GENMASK(31, 24)
#define MAX22200_STATUS_FREQ_MASK	NO_OS_BIT(16)
#define MAX22200_STATUS_FAULT_MASK(x)	NO_OS_BIT(x)
#define MAX22200_STATUS_MODE_MASK	NO_OS_GENMASK(15, 8)
#define MAX22200_CH_MODE_MASK(x)	NO_OS_GENMASK(8 + (2 * (x)), 9 + (2 * (x)))
#define MAX22200_STATUS_FLAG_MASK	NO_OS_GENMASK(7, 0)
#define MAX22200_ACTIVE_MASK		NO_OS_BIT(0)

/* CFG CH Register Masks */
#define MAX22200_HFS_MASK		NO_OS_BIT(31)
#define MAX22200_HOLD_MASK		NO_OS_GENMASK(30, 24)
#define MAX22200_TRGNSP_IO_MASK		NO_OS_BIT(23)
#define MAX22200_HIT_MASK		NO_OS_GENMASK(22, 16)
#define MAX22200_HIT_T_MASK		NO_OS_GENMASK(15, 8)
#define MAX22200_VDRNCDR_MASK		NO_OS_BIT(7)
#define MAX22200_HSNLS_MASK		NO_OS_BIT(6)
#define MAX22200_FREQ_CFG_MASK		NO_OS_GENMASK(5, 4)
#define MAX22200_CH_ENABLE_MASK(x)	NO_OS_BIT(x)

/* CFG DPM Register Masks */
#define MAX22200_DPM_ISTART_MASK	NO_OS_GENMASK(14, 8)
#define MAX22200_DPM_TDEB_MASK		NO_OS_GENMASK(7, 4)
#define MAX22200_DPM_IPTH_MASK		NO_OS_GENMASK(3, 0)

enum max22200_fault_mask {
	MAX22200_M_UVM = 17,
	MAX22200_M_COMF,
	MAX22200_M_DPM,
	MAX22200_M_HHF,
	MAX22200_M_OLF,
	MAX22200_M_OCP,
	MAX22200_M_OVT
};

enum max22200_chopping_freq {
	MAX22200_FREQ_100KHZ,
	MAX22200_FREQ_80KHZ
};

enum max22200_ch_op_mode {
	MAX22200_INDEPENDENT_MODE,
	MAX22200_PARALLEL_MODE,
	MAX22200_HALF_BRIDGE_MODE,
};

enum max22200_scale {
	MAX22200_FULLSCALE,
	MAX22200_HALF_FULL_SCALE
};

enum max22200_trig {
	MAX22200_ONCH_SPI,
	MAX22200_TRIG
};

enum max22200_ch_drive {
	MAX22200_CURRENT_DRIVE,
	MAX22200_VOLTAGE_DRIVE
};

enum max22200_ch_side {
	MAX22200_LOW_SIDE,
	MAX22200_HIGH_SIDE
};

enum max22200_ch_freq {
	MAX22200_FREQMAIN_DIV_4,
	MAX22200_FREQMAIN_DIV_3,
	MAX22200_FREQMAIN_DIV_2,
	MAX22200_FREQMAIN
};

enum max22200_ch_enable {
	MAX22200_HHF_ENABLE,
	MAX22200_DPM_ENABLE,
	MAX22200_OL_ENABLE,
	MAX22200_SRC
};

struct max22200_init_param {
	struct no_os_spi_init_param *comm_param;
	struct no_os_gpio_init_param *fault_param;
	struct no_os_gpio_init_param *enable_param;
	struct no_os_gpio_init_param *cmd_param;
	struct no_os_gpio_init_param *trig_param;
	enum max22200_ch_op_mode ch_config[MAX22200_CHANNELS_CONFIG];
};

struct max22200_desc {
	struct no_os_spi_desc *comm_desc;
	struct no_os_gpio_desc *fault_desc;
	struct no_os_gpio_desc *enable_desc;
	struct no_os_gpio_desc *cmd_desc;
	struct no_os_gpio_desc *trig_desc;
	uint8_t buff[MAX22200_FRAME_SIZE];
	enum max22200_ch_op_mode ch_config[MAX22200_CHANNELS_CONFIG];
	enum max22200_ch_side chan_side;
	enum max22200_ch_drive chan_drive;
};

/** Read data from desired register for MAX22200. */
int max22200_reg_read(struct max22200_desc *, uint32_t, uint32_t *);

/** Write data to desired register for MAX22200 */
int max22200_reg_write(struct max22200_desc *, uint32_t, uint32_t);

/** Update data in the desired register. */
int max22200_reg_update(struct max22200_desc *, uint32_t, uint32_t, uint32_t);

/** Set external trigger's state of the MAX22200. */
int max22200_set_trig_state(struct max22200_desc *, bool);

/** Set channel state for specific channel. */
int max22200_set_ch_state(struct max22200_desc *, uint32_t, bool);

/** Set fault mask bits in the status register. */
int max22200_fault_mask_set(struct max22200_desc *, enum max22200_fault_mask,
			    bool);

/** Set device frequency value. */
int max22200_set_chop_freq(struct max22200_desc *, enum max22200_chopping_freq);

/** Set channel HIT time and HIT current. */
int max22200_set_ch_hit(struct max22200_desc *, uint32_t, uint8_t, uint8_t);

/** Set channel hold current. */
int max22200_set_ch_hold(struct max22200_desc *, uint32_t, uint8_t);

/** Set channel's scale to fullscale or half fullscale. */
int max22200_set_ch_scale(struct max22200_desc *, uint32_t,
			  enum max22200_scale);

/** Set channel's trigger to be either SPI or external trigger. */
int max22200_set_ch_trig(struct max22200_desc *, uint32_t, enum max22200_trig);

/** Set chanmel operation mode, high-side/low-side and drive. */
int max22200_set_ch_mode(struct max22200_desc *, uint32_t,
			 enum max22200_ch_drive, enum max22200_ch_side,
			 enum max22200_ch_op_mode);

/** Set channel's frequency. */
int max22200_set_ch_freq(struct max22200_desc *, uint32_t,
			 enum max22200_ch_freq);

/** Set channel's enables for different checks, detection and functions. */
int max22200_set_ch_enable(struct max22200_desc *, uint32_t,
			   enum max22200_ch_enable, bool);

/** Set configuration DPM. */
int max22200_set_cfg_dpm(struct max22200_desc *, uint8_t, uint8_t);

/** Fault mask bit get function */
int max22200_fault_mask_get(struct max22200_desc *, enum max22200_fault_mask,
			    bool *);

/** Get the state of a specific channel. */
int max22200_get_ch_state(struct max22200_desc *, uint32_t, bool *);

/** Read device frequency */
int max22200_get_chop_freq(struct max22200_desc *,
			   enum max22200_chopping_freq *);

/** Read channel HIT configuration. */
int max22200_get_ch_hit(struct max22200_desc *, uint32_t, uint8_t *, uint8_t *);

/** Read channel HOLD configuration. */
int max22200_get_ch_hold(struct max22200_desc *, uint32_t, uint8_t *);

/** Read channel's scale. */
int max22200_get_ch_scale(struct max22200_desc *, uint32_t,
			  enum max22200_scale *);

/** Read channel's selected trigger for specific channel. */
int max22200_get_ch_trig(struct max22200_desc *, uint32_t,
			 enum max22200_trig *);

/** Read channel's mode configuration. */
int max22200_get_ch_mode(struct max22200_desc *, uint32_t,
			 enum max22200_ch_drive *, enum max22200_ch_side *,
			 enum max22200_ch_op_mode *);

/** Read channel's frequency. */
int max22200_get_ch_freq(struct max22200_desc *, uint32_t,
			 enum max22200_ch_freq *);

/** Read channel's enable bits state. */
int max22200_get_ch_enable(struct max22200_desc *, uint32_t,
			   enum max22200_ch_enable, bool *);

/** Read configuration for the detection of the plunger movement of the
    device. */
int max22200_get_cfg_dpm(struct max22200_desc *, uint8_t *, uint8_t *);

/** MAX22200 device initialization function. */
int max22200_init(struct max22200_desc **, struct max22200_init_param *);

/** Deallocates all the resources used at initialization. */
int max22200_remove(struct max22200_desc *);

#endif /* _MAX22200_H */
