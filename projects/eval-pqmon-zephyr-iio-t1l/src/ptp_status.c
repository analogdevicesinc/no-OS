/*
 * ptp_status.c - PTP sync status query implementation
 *
 * Queries the Zephyr PTP subsystem internals (clock.h, port.h, ds.h)
 * to report port state, offset from time transmitter, mean path delay,
 * and a convergence flag.
 *
 * These are internal Zephyr headers (subsys/net/lib/ptp/) — not public API.
 * CMakeLists.txt adds the include path via target_include_directories().
 */

#include "ptp_status.h"

#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/sys/slist.h>
#include <stdlib.h>

#include "clock.h"
#include "port.h"
#include "ds.h"
#include "state_machine.h"

static const char *state_strings[] = {
	[0]                          = "UNKNOWN",
	[PTP_PS_INITIALIZING]        = "INITIALIZING",
	[PTP_PS_FAULTY]              = "FAULTY",
	[PTP_PS_DISABLED]            = "DISABLED",
	[PTP_PS_LISTENING]           = "LISTENING",
	[PTP_PS_PRE_TIME_TRANSMITTER] = "PRE_TIME_TRANSMITTER",
	[PTP_PS_TIME_TRANSMITTER]    = "TIME_TRANSMITTER",
	[PTP_PS_GRAND_MASTER]        = "GRAND_MASTER",
	[PTP_PS_PASSIVE]             = "PASSIVE",
	[PTP_PS_UNCALIBRATED]        = "UNCALIBRATED",
	[PTP_PS_TIME_RECEIVER]       = "TIME_RECEIVER",
};

const char *ptp_port_state_str(int state)
{
	if (state >= 0 && state < (int)(sizeof(state_strings) / sizeof(state_strings[0])) &&
	    state_strings[state] != NULL) {
		return state_strings[state];
	}
	return "UNKNOWN";
}

int ptp_get_sync_status(struct ptp_sync_status *status)
{
	if (!status) {
		return -EINVAL;
	}

	/* Default: unknown state, not converged */
	status->port_state = 0;
	status->offset_from_tt = 0;
	status->mean_delay = 0;
	status->converged = false;

	/* Get current dataset (offset_from_tt, mean_delay) */
	const struct ptp_current_ds *cur_ds = ptp_clock_current_ds();

	if (!cur_ds) {
		return -ENODATA;
	}

	/* ptp_timeinterval is nanoseconds * 2^16 — shift right to get ns */
	status->offset_from_tt = cur_ds->offset_from_tt >> 16;
	status->mean_delay = cur_ds->mean_delay >> 16;

	/* Get port state from the first PTP port */
	sys_slist_t *ports = ptp_clock_ports_list();

	if (ports && !sys_slist_is_empty(ports)) {
		struct ptp_port *port = CONTAINER_OF(sys_slist_peek_head(ports),
						     struct ptp_port, node);
		status->port_state = ptp_port_state(port);
	}

	/* Convergence: active PTP role and small offset.
	 *
	 * GRAND_MASTER is always converged — it IS the time reference.
	 *
	 * TIME_RECEIVER must also have mean_delay > 0, which proves
	 * at least one delay request/response exchange has completed.
	 * Without this check, offset_from_tt=0 (initial default) passes
	 * the threshold before any sync messages arrive. */
	int64_t abs_offset = status->offset_from_tt;

	if (abs_offset < 0) {
		abs_offset = -abs_offset;
	}

	if (status->port_state == PTP_PS_GRAND_MASTER ||
	    status->port_state == PTP_PS_TIME_TRANSMITTER) {
		status->converged = true;
	} else if (status->port_state == PTP_PS_TIME_RECEIVER) {
		status->converged = (abs_offset < PTP_CONVERGE_THRESHOLD_NS) &&
				    (status->mean_delay > 0);
	} else {
		status->converged = false;
	}

	return 0;
}
