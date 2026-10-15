/*
 * afe_sync.h - Synchronized ADE9430 start via UDP + PTP 1PPS
 *
 * Coordinates both boards to restart their AFEs at the same PTP-synchronized
 * 1PPS edge, aligning cycle boundaries for sub-ms event correlation.
 *
 * Protocol: Slave sends AFE_SYNC_READY after PTP convergence,
 * master replies with AFE_SYNC_GO containing a target PTP second.
 * Both boards restart their AFE at the 1PPS edge matching that second.
 */
#ifndef AFE_SYNC_H
#define AFE_SYNC_H

#include <stdbool.h>
#include <stdint.h>

#define AFE_SYNC_UDP_PORT     9999
#define AFE_SYNC_MAGIC        0x41464553U  /* "AFES" */

/* Message types */
#define AFE_SYNC_READY        1  /* Slave → Master: PTP converged, ready to sync */
#define AFE_SYNC_GO           2  /* Master → Slave: restart AFE at target_sec */

/* Wire format (9 bytes, packed) */
struct afe_sync_msg {
	uint32_t magic;
	uint8_t  type;
	uint32_t target_sec;  /* Only meaningful in AFE_SYNC_GO */
} __packed;

/**
 * Initialize AFE sync subsystem.
 * Creates UDP socket and starts listener thread on both roles.
 *
 * @param is_master  true for GRAND_MASTER, false for TIME_RECEIVER
 * @param peer_ip    Slave: master IP ("192.0.2.1"). Master: NULL — slaves are
 *                   discovered dynamically from incoming READY messages.
 * @return 0 on success, negative errno on failure
 */
int afe_sync_init(bool is_master, const char *peer_ip);

/**
 * Called by slave after PTP convergence + TS_TIMER realignment.
 * Sends AFE_SYNC_READY to the master and starts retry timer.
 */
void afe_sync_send_ready(void);

/**
 * Check if the AFE sync restart has completed.
 * @return true if both boards have restarted their AFE in sync
 */
bool afe_sync_is_complete(void);

/**
 * Check if AFE sync restart should fire on this 1PPS edge.
 * Called from the 1PPS handler thread with the captured PTP second.
 *
 * @param ptp_sec  The PTP second captured at this 1PPS edge
 * @return true if the restart was triggered on this edge
 */
bool afe_sync_check_1pps(uint32_t ptp_sec);

/**
 * Check if a synchronized AFE restart was requested by the 1PPS handler.
 * Called from the AFE thread on each iteration. If a restart is pending,
 * performs afe_wait_settling() + PQLib state reset and marks sync complete.
 *
 * @return true if restart was performed
 */
bool afe_sync_check_restart(void);

#endif /* AFE_SYNC_H */
