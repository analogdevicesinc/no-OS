/***************************************************************************//**
 *   @file   tmc5240_wrapper.h
 *   @brief  Header file for the wrapper to the tmc5240 TMC-API.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 ******************************************************************************/

#ifndef TMC5240_H
#define TMC5240_H

#include <stdint.h>
#include <string.h>
#include "no_os_spi.h"
#include "no_os_util.h"

enum tmc5240_rampmode {
	TMC5240_RAMPMODE_POSITION,
	TMC5240_RAMPMODE_POSITIVE_VELOCITY,
	TMC5240_RAMPMODE_NEGATIVE_VELOCITY,
	TMC5240_RAMPMODE_HOLD,
};

struct tmc5240_dev {
	struct no_os_spi_desc *spi_desc;
	uint32_t amax;
	uint32_t vmax;
	uint32_t dmax;
	uint32_t clock;
	uint32_t step_angle_millidegrees;
	uint8_t microsteps_res;
};

struct tmc5240_init_param {
	struct no_os_spi_init_param spi_init_param;
	uint32_t amax;
	uint32_t vmax;
	uint32_t dmax;
	uint32_t clock;
	uint32_t step_angle_millidegrees;
	uint8_t microsteps_res;
	uint8_t iholddelay;
	uint8_t irundelay;
	uint8_t current_hold;
	uint8_t current_run;
	uint8_t toff;
	uint8_t tbl;
	uint8_t global_scaler;
	uint8_t slope_control;
	uint8_t current_range;
};

int tmc5240_init(struct tmc5240_dev **device,
		 struct tmc5240_init_param *init_param);

int tmc5240_remove(struct tmc5240_dev *device);

int tmc5240_read(struct tmc5240_dev *device, uint8_t reg_addr,
		 uint32_t *reg_data);

int tmc5240_write(struct tmc5240_dev *device, uint8_t reg_addr,
		  uint32_t reg_data);

int tmc5240_get_target_pos(struct tmc5240_dev *device, int32_t *position);

int tmc5240_set_target_pos(struct tmc5240_dev *device, int32_t position);

int tmc5240_get_current_pos(struct tmc5240_dev *device, int32_t *position);

int tmc5240_set_current_pos(struct tmc5240_dev *device, int32_t position);

int tmc5240_get_current_accel(struct tmc5240_dev *device,
			      int32_t *acceleration);

int tmc5240_get_current_vel(struct tmc5240_dev *device, int32_t *velocity);

int tmc5240_set_target_vel(struct tmc5240_dev *device, int32_t velocity);

int tmc5240_get_shaft_dir(struct tmc5240_dev *device, uint8_t *dir);

int tmc5240_set_shaft_dir(struct tmc5240_dev *device, uint8_t dir);

int tmc5240_get_amax(struct tmc5240_dev *device, int32_t *amax);

int tmc5240_set_amax(struct tmc5240_dev *device, int32_t amax);

int tmc5240_get_vmax(struct tmc5240_dev *device, int32_t *vmax);

int tmc5240_set_vmax(struct tmc5240_dev *device, int32_t vmax);

int tmc5240_get_dmax(struct tmc5240_dev *device, int32_t *dmax);

int tmc5240_set_dmax(struct tmc5240_dev *device, int32_t dmax);

int tmc5240_get_rampmode(struct tmc5240_dev *device, int32_t *rampmode);

int tmc5240_set_rampmode(struct tmc5240_dev *device, int32_t  rampmode);

int tmc5240_stop(struct tmc5240_dev *device);

#endif
