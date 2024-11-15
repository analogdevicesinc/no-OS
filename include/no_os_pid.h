/***************************************************************************//**
 *   @file   no_os_pid.h
 *   @brief  Header file for PID control utility.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
