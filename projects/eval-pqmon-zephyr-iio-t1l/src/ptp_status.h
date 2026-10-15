/*
 * ptp_status.h - PTP sync status query interface
 *
 * Thin wrapper around Zephyr PTP internals to expose port state,
 * offset from time transmitter, mean path delay, and convergence flag.
 */
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
