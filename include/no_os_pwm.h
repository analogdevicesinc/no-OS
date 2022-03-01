/***************************************************************************//**
 *   @file   pwm.h
 *   @brief  Header file of PWM Interface
 *   @author Cristian Pop (cristian.pop@analog.com)
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
#ifndef PWM_H_
#define PWM_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @enum pwm_polarity
 * @brief Possible polarities of the PWM signal
 */
enum pwm_polarity {
	/** PWM duty cycle is high, idle low */
	PWM_POLARITY_HIGH,
	/** PWM duty cycle is low, idle high */
	PWM_POLARITY_LOW,
};

/**
 * @struct pwm_init_param
 * @brief  Structure containing the init parameters needed by the PWM generator
 */
struct pwm_init_param {
	/** Pwm id (Ex. Pin number, timer_id) */
	uint32_t id;
	/** PWM generator period */
	uint32_t period_ns;
	/** PWM generator duty cycle */
	uint32_t duty_cycle_ns;
	/** PWM generator phase */
	uint32_t phase_ns;
	/** PWM generator polarity */
	enum pwm_polarity polarity;
	/** PWM extra parameters (device specific) */
	void *extra;
};

/**
 * @struct pwm_desc
 * @brief  Structure representing an PWM generator device
 */
struct pwm_desc {
	/** Pwm id */
	uint32_t id;
	/** PWM generator period */
	uint32_t period_ns;
	/** PWM generator duty cycle */
	uint32_t duty_cycle_ns;
	/** PWM generator phase */
	uint32_t phase_ns;
	/** PWM generator polarity */
	enum pwm_polarity polarity;
	/** PWM generator enabled */
	bool enabled;
	/** PWM extra parameters (device specific) */
	void *extra;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the PWM generator device */
int32_t pwm_init(struct pwm_desc **desc,
		 const struct pwm_init_param *param);

/* Free the resources used by the PWM generator device */
int32_t pwm_remove(struct pwm_desc *desc);

/* Enable PWM generator device */
int32_t pwm_enable(struct pwm_desc *desc);

/* Disable PWM generator device */
int32_t pwm_disable(struct pwm_desc *desc);

/* Set period of PWM generator device */
int32_t pwm_set_period(struct pwm_desc *desc,
		       uint32_t period_ns);

/* Get period of PWM generator device */
int32_t pwm_get_period(struct pwm_desc *desc,
		       uint32_t *period_ns);

/* Set duty cycle of PWM generator device */
int32_t pwm_set_duty_cycle(struct pwm_desc *desc,
			   uint32_t duty_cycle_ns);

/* Get period of PWM generator device */
int32_t pwm_get_duty_cycle(struct pwm_desc *desc,
			   uint32_t *duty_cycle_ns);

/* Set the phase of PWM generator channel */
int32_t pwm_set_phase(struct pwm_desc *desc,
		      uint32_t phase_ns);

/* Get the phase of PWM generator channel */
int32_t pwm_get_phase(struct pwm_desc *desc,
		      uint32_t *phase_ns);

/* Set polarity of PWM generator device */
int32_t pwm_set_polarity(struct pwm_desc *desc,
			 enum pwm_polarity polarity);

/* Set polarity of PWM generator device */
int32_t pwm_get_polarity(struct pwm_desc *desc,
			 enum pwm_polarity *polarity);

#endif
