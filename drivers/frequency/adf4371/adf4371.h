/***************************************************************************//**
 *   @file   adf4371.h
 *   @brief  Header file of ADF4371 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#ifndef ADF4371_H_
#define ADF4371_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Types Declarations *************************/
/******************************************************************************/
struct adf4371_channel_config {
	bool		enable;
	uint64_t	freq;
};

struct adf4371_cp_settings {
	uint32_t	icp;
	uint32_t	regval;
};

struct adf4371_chan_spec {
	uint32_t	num;
	uint64_t	power_up_frequency;
};

struct adf4371_dev {
	struct no_os_spi_desc	*spi_desc;
	bool		spi_3wire_en;
	uint32_t	num_channels;
	struct adf4371_channel_config	channel_cfg[4];
	struct adf4371_cp_settings	cp_settings;
	bool		differential_ref_clk;
	uint32_t	pd_pol;
	bool		mute_till_lock_en;
	uint32_t	muxout_default_mode;
	bool		muxout_en;
	bool		muxout_1v8_en;
	uint32_t	ref_div_factor;
	uint32_t	clkin_freq;
	uint32_t	fpfd;
	uint32_t	integer;
	uint32_t	fract1;
	uint32_t	fract2;
	uint32_t	mod2;
	uint32_t	rf_div_sel;
	uint8_t		buf[10];
};

struct adf4371_init_param {
	struct no_os_spi_init_param	*spi_init;
	bool		spi_3wire_enable;
	uint32_t	clkin_frequency;
	bool		differential_ref_clock;
	bool		loop_filter_inverting;
	uint32_t	charge_pump_microamp;
	bool		mute_till_lock_enable;
	uint32_t	muxout_select;
	bool		muxout_level_1v8_enable;
	uint32_t	num_channels;
	struct adf4371_chan_spec	*channels;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the device. */
int32_t adf4371_init(struct adf4371_dev **device,
		     const struct adf4371_init_param *init_param);

/* Remove the device. */
int32_t adf4371_remove(struct adf4371_dev *device);

/* Recalculate rate corresponding to a channel. */
int32_t adf4371_clk_recalc_rate(struct adf4371_dev *dev, uint32_t chan,
				uint64_t *rate);

/* Calculate closest possible rate */
int32_t adf4371_clk_round_rate(struct adf4371_dev *dev, uint64_t rate,
			       uint64_t *rounded_rate);

/* Set channel rate. */
int32_t adf4371_clk_set_rate(struct adf4371_dev *dev, uint32_t chan,
			     uint64_t rate);

#endif
