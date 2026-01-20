/***************************************************************************//**
 *   @file   tmc5240.h
 *   @brief  Header file for TMC5240 driver.
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

#include "no_os_spi.h"
#include "no_os_util.h"

#define TMC5240_DEFAULT_D1		10
#define TMC5240_DEFAULT_D2		10
#define TMC5240_DEFAULT_VSTOP		10

/**
 * @struct tmc5240_dev
 * @brief TMC5240 device descriptor
 */
struct tmc5240_dev {
	/** SPI descriptor */
	struct no_os_spi_desc *spi_desc;
	/** Start velocity */
	uint32_t vstart;
	/** First acceleration phase */
	uint32_t a1;
	/** First velocity threshold */
	uint32_t v1;
	/** Second acceleration phase */
	uint32_t a2;
	/** Second velocity threshold */
	uint32_t v2;
	/** Maximum acceleration */
	uint32_t amax;
	/** Maximum velocity */
	uint32_t vmax;
	/** Maximum deceleration */
	uint32_t dmax;
	/** Second deceleration phase */
	uint32_t d2;
	/** First deceleration phase */
	uint32_t d1;
	/** Stop velocity */
	uint32_t vstop;
	/** Internal clock frequency in Hz */
	uint32_t clock;
	/** Motor step angle in millidegrees */
	uint32_t step_angle_millidegrees;
	/** Microstepping resolution (0=256, 1=128, etc.) */
	uint8_t microsteps_res;
	/** Device ID for multi-instance support */
	uint8_t id;
};

/**
 * @struct tmc5240_init_param
 * @brief TMC5240 initialization parameters
 */
struct tmc5240_init_param {
	/** SPI initialization parameters */
	struct no_os_spi_init_param spi_init_param;
	/** Start velocity */
	uint32_t vstart;
	/** First acceleration phase */
	uint32_t a1;
	/** First velocity threshold */
	uint32_t v1;
	/** Second acceleration phase */
	uint32_t a2;
	/** Second velocity threshold */
	uint32_t v2;
	/** Maximum acceleration */
	uint32_t amax;
	/** Maximum velocity */
	uint32_t vmax;
	/** Maximum deceleration */
	uint32_t dmax;
	/** Second deceleration phase */
	uint32_t d2;
	/** First deceleration phase */
	uint32_t d1;
	/** Stop velocity */
	uint32_t vstop;
	/** Internal clock frequency in Hz */
	uint32_t clock;
	/** Motor step angle in millidegrees */
	uint32_t step_angle_millidegrees;
	/** Microstepping resolution */
	uint8_t microsteps_res;
	/** Hold current delay */
	uint8_t iholddelay;
	/** Run current delay */
	uint8_t irundelay;
	/** Hold current (0-31, scaled by 1/32) */
	uint8_t current_hold;
	/** Run current (0-31, scaled by 1/32) */
	uint8_t current_run;
	/** Off-time setting */
	uint8_t toff;
	/** Blank time setting */
	uint8_t tbl;
	/** Global current scaling (0-255) */
	uint8_t global_scaler;
	/** Slope control for voltage ramping */
	uint8_t slope_control;
	/** Current range selection */
	uint8_t current_range;
};

/** Initialize the TMC5240 device */
int tmc5240_init(struct tmc5240_dev **device,
		 struct tmc5240_init_param *init_param);

/** Free resources allocated by tmc5240_init() */
int tmc5240_remove(struct tmc5240_dev *device);

/** Read from a TMC5240 register */
int tmc5240_reg_read(struct tmc5240_dev *device, uint8_t reg_addr,
		     uint32_t *reg_data);

/** Write to a TMC5240 register */
int tmc5240_reg_write(struct tmc5240_dev *device, uint8_t reg_addr,
		      uint32_t reg_data);

/** Read-modify-write a TMC5240 register field */
int tmc5240_reg_update(struct tmc5240_dev *device, uint8_t reg_addr,
		       uint32_t mask, uint32_t val);

/** Set motion profile parameters */
int tmc5240_set_motion_profile(struct tmc5240_dev *device);

/** Get target position */
int tmc5240_get_target_pos(struct tmc5240_dev *device, int32_t *position);

/** Set target position */
int tmc5240_set_target_pos(struct tmc5240_dev *device, int32_t position);

/** Get current position */
int tmc5240_get_current_pos(struct tmc5240_dev *device, int32_t *position);

/** Set current position */
int tmc5240_set_current_pos(struct tmc5240_dev *device, int32_t position);

/** Get current acceleration */
int tmc5240_get_current_accel(struct tmc5240_dev *device,
			      int32_t *acceleration);

/** Get current velocity */
int tmc5240_get_current_vel(struct tmc5240_dev *device, int32_t *velocity);

/** Set target velocity */
int tmc5240_set_target_vel(struct tmc5240_dev *device, int32_t velocity);

/** Get shaft direction */
int tmc5240_get_shaft_dir(struct tmc5240_dev *device, uint8_t *dir);

/** Set shaft direction */
int tmc5240_set_shaft_dir(struct tmc5240_dev *device, uint8_t dir);

/** Get maximum acceleration */
int tmc5240_get_amax(struct tmc5240_dev *device, int32_t *amax);

/** Set maximum acceleration */
int tmc5240_set_amax(struct tmc5240_dev *device, int32_t amax);

/** Get maximum velocity */
int tmc5240_get_vmax(struct tmc5240_dev *device, int32_t *vmax);

/** Set maximum velocity */
int tmc5240_set_vmax(struct tmc5240_dev *device, int32_t vmax);

/** Get maximum deceleration */
int tmc5240_get_dmax(struct tmc5240_dev *device, int32_t *dmax);

/** Set maximum deceleration */
int tmc5240_set_dmax(struct tmc5240_dev *device, int32_t dmax);

/** Get start velocity */
int tmc5240_get_vstart(struct tmc5240_dev *device, int32_t *vstart);

/** Set start velocity */
int tmc5240_set_vstart(struct tmc5240_dev *device, int32_t vstart);

/** Get first acceleration phase */
int tmc5240_get_a1(struct tmc5240_dev *device, int32_t *a1);

/** Set first acceleration phase */
int tmc5240_set_a1(struct tmc5240_dev *device, int32_t a1);

/** Get first velocity threshold */
int tmc5240_get_v1(struct tmc5240_dev *device, int32_t *v1);

/** Set first velocity threshold */
int tmc5240_set_v1(struct tmc5240_dev *device, int32_t v1);

/** Get second acceleration phase */
int tmc5240_get_a2(struct tmc5240_dev *device, int32_t *a2);

/** Set second acceleration phase */
int tmc5240_set_a2(struct tmc5240_dev *device, int32_t a2);

/** Get second velocity threshold */
int tmc5240_get_v2(struct tmc5240_dev *device, int32_t *v2);

/** Set second velocity threshold */
int tmc5240_set_v2(struct tmc5240_dev *device, int32_t v2);

/** Get first deceleration phase */
int tmc5240_get_d1(struct tmc5240_dev *device, int32_t *d1);

/** Set first deceleration phase */
int tmc5240_set_d1(struct tmc5240_dev *device, int32_t d1);

/** Get second deceleration phase */
int tmc5240_get_d2(struct tmc5240_dev *device, int32_t *d2);

/** Set second deceleration phase */
int tmc5240_set_d2(struct tmc5240_dev *device, int32_t d2);

/** Get stop velocity */
int tmc5240_get_vstop(struct tmc5240_dev *device, int32_t *vstop);

/** Set stop velocity */
int tmc5240_set_vstop(struct tmc5240_dev *device, int32_t vstop);

/** Get ramp mode */
int tmc5240_get_rampmode(struct tmc5240_dev *device, int32_t *rampmode);

/** Set ramp mode */
int tmc5240_set_rampmode(struct tmc5240_dev *device, int32_t rampmode);

/** Stop the motor */
int tmc5240_stop(struct tmc5240_dev *device);

#endif
