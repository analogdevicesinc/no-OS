/***************************************************************************//**
 *   @file   no_os_pid.c
 *   @brief  Source file for PID control utility.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTerrUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <errno.h>
#include "no_os_pid.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

struct no_os_pid {
	int iacc; // integral accumulator
	int dacc; // derivative accumulator
	int64_t output; // on 64-bits to avoid overflow
	struct no_os_pid_config config; // copy of the user-provided configuration
};

/**
 * @brief Initialize a PID controller with given configuration
 * @param pid - Double pointer to a PID descriptor that the function allocates
 * @param config - PID configuration structure
 * @return
 *  - 0 : On success
 *  - -EINVAL : Invalid input
 *  - -ENOMEM : Memory allocation failure
 */
int no_os_pid_init(struct no_os_pid **pid, struct no_os_pid_config config)
{
	if (!pid)
		return -EINVAL;

	if (config.output_clip.high < config.output_clip.low)
		return -EINVAL;

	*pid = no_os_calloc(1, sizeof(**pid));
	if (!*pid)
		return -ENOMEM;

	(*pid)->config = config;
	(*pid)->output = config.initial;
	(*pid)->iacc = 0;
	(*pid)->dacc = 0;

	return 0;
}

/**
 * @brief Perform PID control given a controller, a set-point and a process variable.
 * @param pid - PID descriptor created with no_os_pid_init()
 * @param SP - Set-point
 * @param PV - Process variable
 * @param output - The output of the PID control
 * @return
 *  - 0 : On success
 *  - -EINVAL : Invalid input
 */
int no_os_pid_control(struct no_os_pid *pid, int SP, int PV, int *output)
{
	int err; // error
	int64_t p; // proportional component
	int64_t i; // integral component
	int64_t d; // derivative component
	struct no_os_pid_range *i_clip;
	struct no_os_pid_range *output_clip;

	if (!pid || !output)
		return -EINVAL;

	i_clip = &pid->config.i_clip;
	output_clip = &pid->config.output_clip;

	// error is the difference between the set point and process variable
	err = SP - PV;

	// don't control (maintain output) if within hysteresis range
	if (abs(err) < pid->config.hysteresis) {
		pr_debug("SP: %d PV: %d --> output: %lu for err=%d\n", SP, PV, pid->output,
			 err);
		goto end;
	}

	// compute proportional component
	p = (int64_t)pid->config.Kp * err;

	// clip integrator accumulator if enabled and if needed
	if (i_clip->high > i_clip->low) {
		if (pid->iacc > i_clip->high)
			pid->iacc = i_clip->high;
		else if (pid->iacc < i_clip->low)
			pid->iacc = i_clip->low;
	}

	// compute integral component
	i = (int64_t)pid->config.Ki * pid->iacc;

	// compute the derivative component
	d = (int64_t)pid->config.Kd * (pid->dacc - err);

	// compute the output
	pid->output = (pid->output * 1000000 - (p + i + d)) / 1000000;
	pr_debug("SP: %d PV: %d --> output: %ld for p %ld i %ld d %ld err=%d\n", SP, PV,
		 pid->output, p, i, d, err);

	// clip the output if enabled and if needed
	if (output_clip->high > output_clip->low) {
		if (pid->output > output_clip->high)
			pid->output = output_clip->high;
		else if (pid->output < output_clip->low)
			pid->output = output_clip->low;
	}

	// keep track of error history in the integrator accumulator
	pid->iacc += err;

	// keep track of process variable change rate in the derivative accumulator
	pid->dacc = err;
end:
	*output = pid->output;

	return 0;
}

/**
 * @brief Change the hysteresis.
 * @param pid - PID descriptor created with no_os_pid_init()
 * @param hyst - The new hysteresis value
 * @return
 *  - 0 : On success
 *  - -EINVAL : Invalid input
 */
int no_os_pid_hysteresis(struct no_os_pid *pid, unsigned int hyst)
{
	if (!pid)
		return -EINVAL;

	pid->config.hysteresis = hyst;

	return 0;
}

/**
 * @brief Reset internal accumulators, useful when the same pid descriptor is used to start over.
 * @param pid - PID descriptor created with no_os_pid_init()
 * @return
 *  - 0 : On success
 *  - -EINVAL : Invalid input
 */
int no_os_pid_reset(struct no_os_pid *pid)
{
	if (!pid)
		return -EINVAL;

	pid->iacc = 0;
	pid->dacc = 0;

	return 0;
}

/**
 * @brief De-initialize a PID controller by freeing the allocated memory
 * @param pid - PID descriptor created with no_os_pid_init()
 * @return
 *  - 0 : On success
 *  - -EINVAL : Invalid input
 */
int no_os_pid_remove(struct no_os_pid *pid)
{
	if (!pid)
		return -EINVAL;

	no_os_free(pid);
	pid = NULL;

	return 0;
}
