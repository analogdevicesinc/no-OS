/***************************************************************************//**
 *   @file   max25603.h
 *   @brief  Header file of MAX25603 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
};

struct max25603_init_param {
	struct no_os_gpio_init_param *shunt_param;
	struct no_os_gpio_init_param *flt_param;

	struct no_os_pwm_init_param *en1_param;
	struct no_os_pwm_init_param *en2_param;
	struct no_os_pwm_init_param *lb_param;
	struct no_os_pwm_init_param *hb_param;
};

struct max25603_desc {
	struct no_os_gpio_desc *shunt_desc;
	struct no_os_gpio_desc *flt_desc;

	struct no_os_pwm_desc *en1_desc;
	struct no_os_pwm_desc *en2_desc;
	struct no_os_pwm_desc *lb_desc;
	struct no_os_pwm_desc *hb_desc;

	enum max25603_enable enable;
	enum max25603_beam beam;
	enum max25603_comp comp;
};

int max25603_sel_comp(struct max25603_desc *desc, enum max25603_comp comp);

int max25603_set_enable(struct max25603_desc *desc, enum max25603_enable enable,
			uint32_t freq_hz, uint8_t duty);

int max25603_set_beam(struct max25603_desc *desc, enum max25603_beam beam,
		      uint32_t freq_hz, uint8_t duty);

int max25603_init(struct max25603_desc **desc,
		  struct max25603_init_param *init_param);

int max25603_remove(struct max25603_desc *desc);

#endif /* __MAX25603_H__ */
