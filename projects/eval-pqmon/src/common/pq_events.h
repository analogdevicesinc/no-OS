/*******************************************************************************
 *   @file   pq_events.h
 *   @brief  Application power quality event tracker
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PQ_EVENTS_H__
#define __PQ_EVENTS_H__

#include <stdbool.h>
#include <stdint.h>
#include "adi_pqlib.h"

/* Event types, in the order of the IIO event channels */
enum pq_ev_type {
	PQ_EV_DIP,
	PQ_EV_SWELL,
	PQ_EV_RVC,
	PQ_EV_INTRP,
	/*
	 * Line interruption (application extension): the IEC 61000-4-30
	 * single-phase interruption rule applied to each line. Starts when
	 * any line falls below the interruption threshold, ends when every
	 * such line is back above threshold + hysteresis. PQLib only reports
	 * polyphase interruptions (all lines down).
	 */
	PQ_EV_LINE_INTRP,
	PQ_EV_NUM_TYPES
};

#define PQ_EV_NUM_PHASES	3

/* Latest event record in PQLib layout, read by the IIO event attributes */
extern ADI_PQLIB_EVENTS accumulated_events;

/* PQLib event records rejected as implausible */
extern uint32_t bogus_event_count;

/**
 * @brief Track events from the result of adi_pqlib_ProcessOneCycle().
 * Call once per processed cycle while the measurement is running.
 * @param hDevice - PQLib handle (for the event time)
 * @param res     - result code of adi_pqlib_ProcessOneCycle()
 * @param in      - one-cycle input that was processed
 */
void pq_events_cycle(ADI_PQLIB_HANDLE hDevice, ADI_PQLIB_RESULT res,
		     const ADI_PQLIB_CYCLE_INPUT *in);

/**
 * @brief Refine the latest records with PQLib's 10/12 cycle event output,
 * when it is plausible and matches.
 * @param output - PQLib output
 */
void pq_events_cross_check(const ADI_PQLIB_OUTPUT *output);

/**
 * @brief Shift the start of events in progress after the PQLib time was
 * set, so their durations stay correct. Finished records are not changed.
 * @param delta - new time - old time [ms]
 */
void pq_events_time_shift(ADI_PQLIB_TIME delta);

/**
 * @brief Drop open events without recording them. Call after the PQLib
 * time base has been reset.
 */
void pq_events_reset_open(void);

/**
 * @brief Clear all records, counters and open events.
 */
void pq_events_clear(void);

/**
 * @brief Acknowledge the events of one type (unacknowledged count = 0).
 * @param type - event type
 */
void pq_events_ack(enum pq_ev_type type);

bool pq_events_active(enum pq_ev_type type);
ADI_PQLIB_TIME pq_events_onset(enum pq_ev_type type);
uint32_t pq_events_unacked(enum pq_ev_type type);
uint32_t pq_events_total(enum pq_ev_type type);

/**
 * @brief Number of events of this type that ended since the previous call
 * (read and clear). A client that only polls the count, as Scopy does with
 * countEvent, sees each event once.
 * @param type - event type
 */
uint32_t pq_events_take_new(enum pq_ev_type type);

/**
 * @brief Number of event starts since start-up or pq_events_clear().
 * Incremented in the cycle an event starts, so a host polling it sees
 * every start, also of events that ended before the next poll. Starts
 * dropped later (e.g. an RVC that became a dip) are included.
 * @param type - event type
 * @return number of starts
 */
uint32_t pq_events_onsets(enum pq_ev_type type);

/**
 * @brief Latest event start of any type.
 * @param type     - event type of the latest start
 * @param start    - its start time [ms]
 * @param trigMask - its trigger phases, bit 0 = phase A
 * @return number of starts of all types (0 if none yet)
 */
uint32_t pq_events_last_onset(enum pq_ev_type *type, ADI_PQLIB_TIME *start,
			      uint8_t *trigMask);

/**
 * @brief Trigger phases of the open event, or of the latest one.
 * @param type - event type
 * @return bit mask, bit 0 = phase A
 */
uint8_t pq_events_trigger(enum pq_ev_type type);

/**
 * @brief Phases beyond the threshold at any time during the open event,
 * or during the latest one (includes the trigger phases).
 * @param type - event type
 * @return bit mask, bit 0 = phase A
 */
uint8_t pq_events_affected(enum pq_ev_type type);

/**
 * @brief Timing of the latest finished event of a type.
 * @param type   - event type
 * @param start  - start time [ms]
 * @param end    - end time [ms]
 * @param cycles - duration in cycles
 * @return true if a finished event exists
 */
bool pq_events_latest(enum pq_ev_type type, ADI_PQLIB_TIME *start,
		      ADI_PQLIB_TIME *end, uint32_t *cycles);

/**
 * @brief Per-phase min or max RMS of the open event, or of the latest one.
 * @param type  - event type
 * @param phase - 0..2 for A..C
 * @param max   - true for the maximum, false for the minimum
 * @return raw RMS value
 */
ADI_AFE_RMS_TYPE pq_events_mag(enum pq_ev_type type, int phase, bool max);

#endif /* __PQ_EVENTS_H__ */
