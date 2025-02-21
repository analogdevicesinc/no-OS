/***************************************************************************//**
 *   @file   no_os_pwm.h
 *   @brief  Header file of PWM Interface
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
*******************************************************************************/
#ifndef _NO_OS_PWM_H_
#define _NO_OS_PWM_H_

#include <stdint.h>
#include <stdbool.h>

#define PWM_MAX_NUMBER 4

/**
 * @enum no_os_pwm_polarity
 * @brief Possible polarities of the PWM signal
 */
enum no_os_pwm_polarity {
	/** PWM duty cycle is high, idle low */
	NO_OS_PWM_POLARITY_HIGH,
	/** PWM duty cycle is low, idle high */
	NO_OS_PWM_POLARITY_LOW,
};

/**
 * @struct no_os_pwm_init_param
 * @brief  Structure containing the init parameters needed by the PWM generator
 */
struct no_os_pwm_init_param {
	/** Pwm id (Ex. Pin number, timer_id) */
	uint32_t id;
	/** PWM generator period */
	uint32_t period_ns;
	/** PWM generator duty cycle */
	uint32_t duty_cycle_ns;
	/** PWM generator phase */
	uint32_t phase_ns;
	/** PWM generator polarity */
	enum no_os_pwm_polarity polarity;
	/** PWM gpio pin init param*/
	struct no_os_gpio_init_param *pwm_gpio;
	/* IRQ ID */
	uint32_t irq_id;
	/** PWM callback **/
	void (*pwm_callback)(void *arg);
	/** PWM platform specific functions */
	const struct no_os_pwm_platform_ops *platform_ops;
	/** PWM extra parameters (device specific) */
	void *extra;
};

/**
 * @struct no_os_pwm_desc
 * @brief  Structure representing an PWM generator device
 */
struct no_os_pwm_desc {
	/** PWM mutex*/
	void *mutex;
	/** Pwm id */
	uint32_t id;
	/** PWM generator period */
	uint32_t period_ns;
	/** PWM generator duty cycle */
	uint32_t duty_cycle_ns;
	/** PWM generator phase */
	uint32_t phase_ns;
	/** PWM generator polarity */
	enum no_os_pwm_polarity polarity;
	/** PWM generator enabled */
	bool enabled;
	/** PWM gpio pin instance */
	struct no_os_gpio_desc *pwm_gpio;
	/* IRQ ID */
	uint32_t irq_id;
	/** PWM callback **/
	void (*pwm_callback)(void* arg);
	/** PWM platform specific functions */
	const struct no_os_pwm_platform_ops *platform_ops;
	/** PWM extra parameters (device specific) */
	void *extra;
};

/**
 * @struct no_os_pwm_platform_ops
 * @brief Structure holding PWM function pointers that point to the platform
 * specific function
 */
struct no_os_pwm_platform_ops {
	/** pwm initialization function pointer */
	int32_t (*pwm_ops_init)(struct no_os_pwm_desc **,
				const struct no_os_pwm_init_param *);
	/** pwm enable function pointer */
	int32_t (*pwm_ops_enable)(struct no_os_pwm_desc *);
	/** pwm disable function pointer */
	int32_t (*pwm_ops_disable)(struct no_os_pwm_desc *);
	/** pwm set period function pointer */
	int32_t (*pwm_ops_set_period)(struct no_os_pwm_desc *, uint32_t);
	/** pwm get period function pointer */
	int32_t (*pwm_ops_get_period)(struct no_os_pwm_desc *, uint32_t *);
	/** pwm set duty cycle function pointer */
	int32_t (*pwm_ops_set_duty_cycle)(struct no_os_pwm_desc *, uint32_t);
	/** pwm get duty cycle function pointer */
	int32_t (*pwm_ops_get_duty_cycle)(struct no_os_pwm_desc *, uint32_t *);
	/** pwm set phase function pointer */
	int32_t (*pwm_ops_set_phase)(struct no_os_pwm_desc *, uint32_t);
	/** pwm get phase function pointer */
	int32_t (*pwm_ops_get_phase)(struct no_os_pwm_desc *, uint32_t *);
	/** pwm set polarity function pointer */
	int32_t (*pwm_ops_set_polarity)(struct no_os_pwm_desc *,
					enum no_os_pwm_polarity);
	/** pwm get polarity function pointer */
	int32_t (*pwm_ops_get_polarity)(struct no_os_pwm_desc *,
					enum no_os_pwm_polarity *);
	/** pwm remove function pointer */
	int32_t(*pwm_ops_remove)(struct no_os_pwm_desc *);
};

/* Initialize the PWM generator device */
int32_t no_os_pwm_init(struct no_os_pwm_desc **desc,
		       const struct no_os_pwm_init_param *param);

/* Free the resources used by the PWM generator device */
int32_t no_os_pwm_remove(struct no_os_pwm_desc *desc);

/* Enable PWM generator device */
int32_t no_os_pwm_enable(struct no_os_pwm_desc *desc);

/* Disable PWM generator device */
int32_t no_os_pwm_disable(struct no_os_pwm_desc *desc);

/* Set period of PWM generator device */
int32_t no_os_pwm_set_period(struct no_os_pwm_desc *desc,
			     uint32_t period_ns);

/* Get period of PWM generator device */
int32_t no_os_pwm_get_period(struct no_os_pwm_desc *desc,
			     uint32_t *period_ns);

/* Set duty cycle of PWM generator device */
int32_t no_os_pwm_set_duty_cycle(struct no_os_pwm_desc *desc,
				 uint32_t duty_cycle_ns);

/* Get period of PWM generator device */
int32_t no_os_pwm_get_duty_cycle(struct no_os_pwm_desc *desc,
				 uint32_t *duty_cycle_ns);

/* Set the phase of PWM generator channel */
int32_t no_os_pwm_set_phase(struct no_os_pwm_desc *desc,
			    uint32_t phase_ns);

/* Get the phase of PWM generator channel */
int32_t no_os_pwm_get_phase(struct no_os_pwm_desc *desc,
			    uint32_t *phase_ns);

/* Set polarity of PWM generator device */
int32_t no_os_pwm_set_polarity(struct no_os_pwm_desc *desc,
			       enum no_os_pwm_polarity polarity);

/* Set polarity of PWM generator device */
int32_t no_os_pwm_get_polarity(struct no_os_pwm_desc *desc,
			       enum no_os_pwm_polarity *polarity);

#endif // _NO_OS_PWM_H_
