/***************************************************************************//**
 *   @file   max25603.h
 *   @brief  Header file of MAX25603 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __MAX25603_H__
#define __MAX25603_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_gpio.h"
#include "no_os_pwm.h"
#include "no_os_util.h"
#include "no_os_error.h"

enum max25603_comp {
	MAX25603_COMP1,
	MAX25603_COMP2
};

enum max25603_enable {
	MAX25603_DISABLE_EN,
	MAX25603_EN1,
	MAX25603_EN2,
};

enum max25603_beam {
	MAX25603_DISABLE_BEAM,
	MAX25603_HIGH_BEAM,
	MAX25603_LOW_BEAM,
	MAX25603_DRL_BEAM,
	MAX25603_TURN_BEAM,
};

struct max25603_init_param {
	struct no_os_gpio_init_param *shunt_param;
	struct no_os_gpio_init_param *flt_param;

	struct no_os_pwm_init_param *en1_param;
	struct no_os_pwm_init_param *en2_param;
	struct no_os_pwm_init_param *lb_param;
	struct no_os_pwm_init_param *hb_param;
	struct no_os_pwm_init_param *drl_param;
	struct no_os_pwm_init_param *turn_param;
};

struct max25603_desc {
	struct no_os_gpio_desc *shunt_desc;
	struct no_os_gpio_desc *flt_desc;

	struct no_os_pwm_desc *en1_desc;
	struct no_os_pwm_desc *en2_desc;
	struct no_os_pwm_desc *lb_desc;
	struct no_os_pwm_desc *hb_desc;
	struct no_os_pwm_desc *drl_desc;
	struct no_os_pwm_desc *turn_desc;

	enum max25603_enable enable;
	enum max25603_beam beam;
	enum max25603_comp comp;
};

/** MAX25603 select comparator function. */
int max25603_sel_comp(struct max25603_desc *desc, enum max25603_comp comp);

/** MAX25603 set and configure ENABLE PWM dimming. */
int max25603_set_enable(struct max25603_desc *desc, enum max25603_enable enable,
			uint32_t freq_hz, uint8_t duty);

/** MAX25603 set and configure BEAM PWM dimming. */
int max25603_set_beam(struct max25603_desc *desc, enum max25603_beam beam,
		      uint32_t freq_hz, uint8_t duty);

/** MAX25603 device initialization function.  */
int max25603_init(struct max25603_desc **desc,
		  struct max25603_init_param *init_param);

/** Deallocates all resouces used at initialization. */
int max25603_remove(struct max25603_desc *desc);

#endif /* __MAX25603_H__ */
