/*******************************************************************************
 *   @file   axi_pwm_extra.h
 *   @brief  Header containing types used in the axi_pwm driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
