/***************************************************************************//**
 *   @file   iio_max22200.h
 *   @brief  Header file of IIO MAX22200 Driver.
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
#ifndef IIO_MAX22200_H
#define IIO_MAX22200_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "max22200.h"

#define MAX22200_HOLD_MAX_VAL	0xFF
#define MAX22200_HIT_T_MAX_VAL	0xFF

enum max22200_iio_fault_mask {
	MAX22200_IIO_UVM_MASK = 1,
	MAX22200_IIO_COMER_MASK,
	MAX22200_IIO_DPM_MASK,
	MAX22200_IIO_HHF_MASK,
	MAX22200_IIO_OLF_MASK,
	MAX22200_IIO_OCP_MASK,
	MAX22200_IIO_OVT_MASK
};

enum max22200_iio_ch_attrs {
	MAX22200_IIO_SCALE,
	MAX22200_IIO_HIT_CURRENT,
	MAX22200_IIO_HIT_TIME,
	MAX22200_IIO_HOLD_CURRENT,
	MAX22200_IIO_CH_TRIG,
	MAX22200_IIO_CH_DRIVE,
	MAX22200_IIO_CH_SIDE,
	MAX22200_IIO_CH_OP_MODE,
	MAX22200_IIO_CH_FREQ,
};

enum max22200_iio_available {
	MAX22200_IIO_SCALE_AVAILABLE,
	MAX22200_IIO_CH_TRIG_AVAILABLE,
	MAX22200_IIO_CH_FREQ_AVAILABLE,
	MAX22200_IIO_CHOP_FREQ_AVAILABLE,
	MAX22200_IIO_CH_DRIVE_AVAILABLE,
	MAX22200_IIO_CH_SIDE_AVAILABLE,
	MAX22200_IIO_CH_OP_MODE_AVAILABLE
};

static const char *const max22200_scale_avail[2] = {
	"fullscale",
	"half_fullscale"
};

static const char *const max22200_channel_trig_avail[2] = {
	"onch_spi",
	"ext_trig"
};

static const char *const max22200_ch_drive_avail[2] = {
	"current_drive",
	"voltage_dri"
};

static const char *const max22200_ch_side_avail[2] = {
	"low_side",
	"high_side"
};

static const char *const max22200_ch_op_mode_avail[3] = {
	"independent_mode",
	"parallel_mode",
	"half_bridge_mode",
};

static const char *const max22200_channel_freq_avail[4] = {
	"freqmain_div_4",
	"freqmain_div_3",
	"freqmain_div_2",
	"freqmain"
};

static const char *const max22200_chopping_freq_avail[2] = {
	"100KHz",
	"80KHz"
};

/**
 * @brief MAX22200 specific IIO descriptor.
*/
struct max22200_iio_desc {
	struct max22200_desc *max22200_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Initalization parameter fr the MAX22200 IIO descriptor.
*/
struct max22200_iio_desc_init_param {
	struct max22200_init_param *max22200_init_param;
	bool ch_enable[MAX22200_CHANNELS];
};

/** Initializes the MAXX22200 IIO descriptor. */
int max22200_iio_init(struct max22200_iio_desc **,
		      struct max22200_iio_desc_init_param *);

/** Free resources allocated by the init function. */
int max22200_iio_remove(struct max22200_iio_desc *);

#endif /** IIO_MAX22200_H */
