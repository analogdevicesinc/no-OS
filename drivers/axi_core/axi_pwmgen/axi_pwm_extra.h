/*******************************************************************************
 *   @file   axi_pwm_extra.h
 *   @brief  Header containing types used in the axi_pwm driver.
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

#ifndef AXI_PWM_EXTRA_H_
#define AXI_PWM_EXTRA_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_pwm.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct axi_pwm_init_param
 * @brief Structure holding the initialization parameters for axi PWM.
 */
struct axi_pwm_init_param {
	/** PWM core base address */
	uint32_t base_addr;
	/** PWM reference clock */
	uint32_t ref_clock_Hz;
	/** Desired channel of the pwm controller */
	uint32_t channel;
};

/**
 * @struct axi_pwm_desc
 * @brief AXI PWM descriptor
 */
struct axi_pwm_desc {
	/** PWM core base address */
	uint32_t base_addr;
	/** PWM reference clock */
	uint32_t ref_clock_Hz;
	/** Desired channel of the pwm controller */
	uint32_t channel;
	/** Used to store the period when the channel is disabled */
	uint32_t ch_period;
	/** Hardware version necessary for checking for the right register offsets  */
	uint32_t hw_major_ver;
};

/**
 * @brief AXI specific PWM platform ops structure
 */
extern const struct no_os_pwm_platform_ops axi_pwm_ops;

#endif
