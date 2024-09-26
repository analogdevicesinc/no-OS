/***************************************************************************//**
 *   @file   no_os_pid.h
 *   @brief  Header file for PID control utility.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef _NO_OS_PID_H
#define _NO_OS_PID_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/**
 * @struct no_os_pid_range
 * @brief Range definition for limiting PID control output or internal integrator accumulator
 */
struct no_os_pid_range {
	/* High limit of the range */
	int high;
	/** Low limit of the range */
	int low;
};

/**
 * @struct no_os_pid_config
 * @brief Configuration of the PID
 */
struct no_os_pid_config {
	/** Proportional gain (micro-units) */
	unsigned int Kp;
	/** Integral gain (micro-units) */
	unsigned int Ki;
	/** Derivative gain (micro-units) */
	unsigned int Kd;
	/** (Optional) Control supressed when process variable  is within set point +/- hysteresis */
	unsigned int hysteresis;
	/** (Optional) Boundary limits for integral component */
	struct no_os_pid_range i_clip;
	/** (Optional) Boundary limits for the output (for example, for an 8-bit controlled PWM, one would clip the output to 0-255 range) */
	struct no_os_pid_range output_clip;
	/** (Optional) Initial output */
	int initial;
};

struct no_os_pid;

int no_os_pid_init(struct no_os_pid **pid, struct no_os_pid_config config);
int no_os_pid_control(struct no_os_pid *pid, int SP, int PV, int *output);
int no_os_pid_hysteresis(struct no_os_pid *pid, unsigned int hyst);
int no_os_pid_reset(struct no_os_pid *pid);
int no_os_pid_remove(struct no_os_pid *pid);

#endif
