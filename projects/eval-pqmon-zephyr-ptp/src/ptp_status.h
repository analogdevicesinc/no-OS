/********************************************************************************
 *   @file   ptp_status.h
 *   @brief  PTP sync status query interface header file
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#ifndef PTP_STATUS_H
#define PTP_STATUS_H

#include <stdbool.h>
#include <stdint.h>

struct ptp_sync_status {
	int     port_state;       /* enum ptp_port_state value */
	int64_t offset_from_tt;   /* nanoseconds (raw ptp_timeinterval >> 16) */
	int64_t mean_delay;       /* nanoseconds (raw ptp_timeinterval >> 16) */
	bool    converged;        /* true when port is active role and |offset| < threshold */
};

/* Convergence threshold: |offset_from_tt| must be below this (nanoseconds) */
#define PTP_CONVERGE_THRESHOLD_NS  1000000  /* 1 ms */

/* Port state constant — mirrored from Zephyr internal state_machine.h
 * so callers don't need to include PTP internals. */
#define PTP_PORT_STATE_TIME_RECEIVER   10  /* PTP_PS_TIME_RECEIVER */

/**
 * Query current PTP sync status.
 * Returns 0 on success, negative errno if PTP stack is not available.
 */
int ptp_get_sync_status(struct ptp_sync_status *status);

/**
 * Convert ptp_port_state enum value to human-readable string.
 */
const char *ptp_port_state_str(int state);

#endif /* PTP_STATUS_H */
