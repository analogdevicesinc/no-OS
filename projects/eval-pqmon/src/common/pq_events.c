/*******************************************************************************
 *   @file   pq_events.c
 *   @brief  Application power quality event tracker
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/*
 * adi_pqlib_ProcessOneCycle() reliably reports when a dip, swell,
 * interruption or RVC starts and ends, but PQLib's own event records are
 * only available at the end of the event, have no phase information and
 * are not reliable after the second event. The event records are therefore
 * built here, from the start/end result codes and the per-phase one-cycle
 * RMS. PQLib's records are only used to refine a record they match.
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stddef.h>
#include "pqlib_example.h"
#include "pq_events.h"
#include "no_os_util.h"

/* Max number of rejected PQLib records printed on the debug console */
#define MAX_BOGUS_EVENT_PRINTS	5

struct pq_ev_rec {
	ADI_PQLIB_TIME start;
	ADI_PQLIB_TIME end;
	uint32_t cycles;
	uint8_t trigMask;
	uint8_t affMask;
	ADI_AFE_RMS_TYPE min[PQ_EV_NUM_PHASES];
	ADI_AFE_RMS_TYPE max[PQ_EV_NUM_PHASES];
	/* RVC: per-phase mean just before the event, largest deviation */
	ADI_AFE_RMS_TYPE ref[PQ_EV_NUM_PHASES];
	uint32_t devMax;
	uint32_t dUss;
	bool valid;
	bool pqlibChecked;
	/* finished without a seen start; start taken from PQLib if it matches */
	bool orphan;
};

struct pq_ev_state {
	bool open;
	/* opened by the tracker because PQLib's start code was masked */
	bool shadow;
	/* open event, copied to rec when it ends */
	struct pq_ev_rec cur;
	/* latest finished event */
	struct pq_ev_rec rec;
	uint32_t total;
	uint32_t unacked;
	/* starts seen, incremented in the start cycle */
	uint32_t onsets;
};

static struct pq_ev_state evState[PQ_EV_NUM_TYPES];
/*
 * Per-phase mean of the last 100 (50 Hz) or 120 (60 Hz) one-cycle RMS
 * values: the reference IEC 61000-4-30 uses to detect a rapid voltage
 * change, used here to find the phase(s) that caused an RVC.
 */
#define PQ_EV_MEAN_MAX_CYCLES	120

static ADI_AFE_RMS_TYPE rmsHist[PQ_EV_NUM_PHASES][PQ_EV_MEAN_MAX_CYCLES];
static uint64_t rmsSum[PQ_EV_NUM_PHASES];
static uint32_t histCount;
static uint32_t histIdx;

/* Lines currently interrupted (PQ_EV_LINE_INTRP), bit 0 = phase A */
static uint8_t linesDown;

/*
 * Hidden history of a dip/swell/interruption condition while no event of
 * that type is open: used for an end whose start code was never reported.
 */
static struct {
	bool valid;
	struct pq_ev_rec cur;
} pending[PQ_EV_NUM_TYPES];

/* Latest event start of any type, for the last_onset device attribute */
static struct {
	uint32_t seq;
	enum pq_ev_type type;
	ADI_PQLIB_TIME start;
	uint8_t trigMask;
} lastOnset;

/* Cycles left in which a start masked by an event code is still checked */
#define PQ_EV_MASK_WINDOW	2
static uint8_t maskWindow;

static uint32_t mean_window(void)
{
	return pqlibExample.exampleConfig.nominalFrequency == 60 ? 120 : 100;
}

static void mean_reset(void)
{
	memset(rmsSum, 0, sizeof(rmsSum));
	histCount = 0;
	histIdx = 0;
}

static void mean_add(const ADI_AFE_RMS_TYPE *rms)
{
	uint32_t win = mean_window();

	for (int p = 0; p < PQ_EV_NUM_PHASES; p++) {
		if (histCount == win)
			rmsSum[p] -= rmsHist[p][histIdx];
		rmsHist[p][histIdx] = rms[p];
		rmsSum[p] += rms[p];
	}
	if (histCount < win)
		histCount++;
	histIdx = (histIdx + 1) % win;
}

static ADI_AFE_RMS_TYPE mean_value(int p)
{
	return histCount ? (ADI_AFE_RMS_TYPE)(rmsSum[p] / histCount) : 0;
}

static uint32_t abs_diff(uint32_t a, uint32_t b)
{
	return a > b ? a - b : b - a;
}

/* Absolute deviation of a phase from its running mean, raw RMS units */
static uint32_t mean_deviation(const ADI_AFE_RMS_TYPE *rms, int p)
{
	uint32_t mean = (uint32_t)(rmsSum[p] / histCount);

	return rms[p] > mean ? rms[p] - mean : mean - rms[p];
}

ADI_PQLIB_EVENTS accumulated_events;
uint32_t bogus_event_count;

/* Common leading fields of ADI_PQLIB_DIP/SWELL/INTRP/RVC */
typedef struct {
	ADI_PQLIB_TIME startTime;
	ADI_PQLIB_TIME endTime;
	uint32_t durationInCycles;
} EVENT_TIMING;

#define EVENT_TIMING_LAYOUT_OK(type)					\
	(offsetof(type, endTime) == offsetof(EVENT_TIMING, endTime) &&	\
	 offsetof(type, durationInCycles) ==				\
	 offsetof(EVENT_TIMING, durationInCycles))

_Static_assert(EVENT_TIMING_LAYOUT_OK(ADI_PQLIB_DIP), "dip layout");
_Static_assert(EVENT_TIMING_LAYOUT_OK(ADI_PQLIB_SWELL), "swell layout");
_Static_assert(EVENT_TIMING_LAYOUT_OK(ADI_PQLIB_INTRP), "intrp layout");
_Static_assert(EVENT_TIMING_LAYOUT_OK(ADI_PQLIB_RVC), "rvc layout");

static uint32_t nominal_frequency(void)
{
	uint32_t freq = pqlibExample.exampleConfig.nominalFrequency;

	return freq ? freq : 50;
}

/**
 * @brief Map a PQLib result code to an event type.
 * @param res     - result code
 * @param started - set to true for *_STARTED, false for *_ENDED
 * @return event type, or PQ_EV_NUM_TYPES if the code is not an event
 */
static enum pq_ev_type event_from_result(ADI_PQLIB_RESULT res, bool *started)
{
	switch (res) {
	case ADI_PQLIB_RESULT_DIP_STARTED:
	case ADI_PQLIB_RESULT_SWELL_STARTED:
	case ADI_PQLIB_RESULT_INTRP_STARTED:
	case ADI_PQLIB_RESULT_RVC_STARTED:
		*started = true;
		break;
	case ADI_PQLIB_RESULT_DIP_ENDED:
	case ADI_PQLIB_RESULT_SWELL_ENDED:
	case ADI_PQLIB_RESULT_INTRP_ENDED:
	case ADI_PQLIB_RESULT_RVC_ENDED:
		*started = false;
		break;
	default:
		return PQ_EV_NUM_TYPES;
	}

	switch (res) {
	case ADI_PQLIB_RESULT_DIP_STARTED:
	case ADI_PQLIB_RESULT_DIP_ENDED:
		return PQ_EV_DIP;
	case ADI_PQLIB_RESULT_SWELL_STARTED:
	case ADI_PQLIB_RESULT_SWELL_ENDED:
		return PQ_EV_SWELL;
	case ADI_PQLIB_RESULT_INTRP_STARTED:
	case ADI_PQLIB_RESULT_INTRP_ENDED:
		return PQ_EV_INTRP;
	default:
		return PQ_EV_RVC;
	}
}

/**
 * @brief Threshold in raw RMS units from a PQLib percentage (% x 100).
 */
static uint32_t pct_of_nominal(ADI_PQLIB_PCT_TYPE pct)
{
	return (uint32_t)((uint64_t)pqlibExample.config.nominalVoltage * pct /
			  10000);
}

/**
 * @brief Phases beyond the event threshold in one cycle.
 * @param type - event type
 * @param rms  - one-cycle RMS of phases A..C
 * @param best_out - set to the most affected phase (may be NULL)
 * @return bit mask, bit 0 = phase A
 */
static uint8_t beyond_threshold(enum pq_ev_type type,
				const ADI_AFE_RMS_TYPE *rms, int *best_out)
{
	ADI_PQLIB_CONFIG *cfg = &pqlibExample.config;
	uint32_t limit;
	uint8_t mask = 0;
	int best = 0;

	switch (type) {
	case PQ_EV_DIP:
	case PQ_EV_INTRP:
		limit = pct_of_nominal(type == PQ_EV_DIP ?
				       cfg->dipThreshold.low :
				       cfg->intrpThreshold.low);
		for (int p = 0; p < PQ_EV_NUM_PHASES; p++) {
			if (rms[p] < limit)
				mask |= 1 << p;
			if (rms[p] < rms[best])
				best = p;
		}
		break;
	case PQ_EV_SWELL:
		limit = pct_of_nominal(cfg->swellThreshold.high);
		for (int p = 0; p < PQ_EV_NUM_PHASES; p++) {
			if (rms[p] > limit)
				mask |= 1 << p;
			if (rms[p] > rms[best])
				best = p;
		}
		break;
	default:
		/* RVC: phases deviating from their running mean by more
		 * than the RVC threshold (% of nominal) */
		if (!histCount)
			break;
		limit = pct_of_nominal(cfg->rvcThreshold.high);
		for (int p = 0; p < PQ_EV_NUM_PHASES; p++) {
			if (mean_deviation(rms, p) > limit)
				mask |= 1 << p;
			if (mean_deviation(rms, p) > mean_deviation(rms, best))
				best = p;
		}
		break;
	}

	if (best_out)
		*best_out = best;

	return mask;
}

/**
 * @brief Check that an event condition has cleared (threshold + hysteresis).
 * @param type - PQ_EV_DIP, PQ_EV_SWELL or PQ_EV_INTRP
 * @param rms  - one-cycle RMS of phases A..C
 * @return true if the event would have ended
 */
static bool recovered(enum pq_ev_type type, const ADI_AFE_RMS_TYPE *rms)
{
	ADI_PQLIB_CONFIG *cfg = &pqlibExample.config;
	uint32_t dipHigh = pct_of_nominal(cfg->dipThreshold.high);
	uint32_t swellLow = pct_of_nominal(cfg->swellThreshold.low);
	uint32_t intrpHigh = pct_of_nominal(cfg->intrpThreshold.high);
	int ok = 0;

	for (int p = 0; p < PQ_EV_NUM_PHASES; p++) {
		if (type == PQ_EV_DIP && rms[p] >= dipHigh)
			ok++;
		else if (type == PQ_EV_SWELL && rms[p] <= swellLow)
			ok++;
		else if (type == PQ_EV_INTRP && rms[p] >= intrpHigh)
			return true;
	}

	return ok == PQ_EV_NUM_PHASES;
}

/**
 * @brief Phases that triggered an event in its start cycle.
 * An interruption needs all phases below its threshold, so its trigger is
 * the phase(s) that started the dip in progress (the first to fall), or of
 * a dip that ended in the last 2 cycles.
 * @param type - event type
 * @param rms  - one-cycle RMS of phases A..C
 * @param now  - time of this cycle
 * @return bit mask, bit 0 = phase A
 */
static uint8_t trigger_phases(enum pq_ev_type type,
			      const ADI_AFE_RMS_TYPE *rms, ADI_PQLIB_TIME now)
{
	const struct pq_ev_state *dip = &evState[PQ_EV_DIP];
	ADI_PQLIB_TIME tol = 2 * 1000 / nominal_frequency();
	uint8_t mask;
	int best;

	if (type == PQ_EV_INTRP) {
		if (dip->open && dip->cur.trigMask)
			return dip->cur.trigMask;
		if (dip->rec.valid && dip->rec.trigMask &&
		    now - dip->rec.end <= tol)
			return dip->rec.trigMask;
	}

	mask = beyond_threshold(type, rms, &best);

	/* Borderline start: report the most affected phase */
	if (!mask)
		mask = 1 << best;

	return mask;
}

/**
 * @brief Write a finished record into the PQLib-layout record read by IIO.
 */
static void publish_record(enum pq_ev_type type)
{
	const struct pq_ev_rec *r = &evState[type].rec;
	ADI_AFE_RMS_TYPE lo = r->min[0], hi = r->max[0];
	EVENT_TIMING *t;

	for (int p = 1; p < PQ_EV_NUM_PHASES; p++) {
		if (r->min[p] < lo)
			lo = r->min[p];
		if (r->max[p] > hi)
			hi = r->max[p];
	}

	switch (type) {
	case PQ_EV_DIP:
		memset(accumulated_events.dips, 0,
		       sizeof(accumulated_events.dips));
		accumulated_events.dipCount = 1;
		accumulated_events.dips[0].minMag = lo;
		t = (EVENT_TIMING *)&accumulated_events.dips[0];
		break;
	case PQ_EV_SWELL:
		memset(accumulated_events.swells, 0,
		       sizeof(accumulated_events.swells));
		accumulated_events.swellCount = 1;
		accumulated_events.swells[0].maxMag = hi;
		t = (EVENT_TIMING *)&accumulated_events.swells[0];
		break;
	case PQ_EV_INTRP:
		memset(accumulated_events.intrp, 0,
		       sizeof(accumulated_events.intrp));
		accumulated_events.intrpCount = 1;
		t = (EVENT_TIMING *)&accumulated_events.intrp[0];
		break;
	case PQ_EV_RVC:
		memset(accumulated_events.rvc, 0,
		       sizeof(accumulated_events.rvc));
		accumulated_events.rvcCount = 1;
		accumulated_events.rvc[0].deltaUmax = r->devMax;
		accumulated_events.rvc[0].deltaUss = r->dUss;
		t = (EVENT_TIMING *)&accumulated_events.rvc[0];
		break;
	default:
		/* Line interruptions have no PQLib-layout record */
		return;
	}

	t->startTime = r->start;
	t->endTime = r->end;
	t->durationInCycles = r->cycles;
}

/**
 * @brief Count an event start and make it the latest onset.
 */
static void note_onset(enum pq_ev_type type)
{
	struct pq_ev_state *s = &evState[type];

	s->onsets++;
	lastOnset.seq++;
	lastOnset.type = type;
	lastOnset.start = s->cur.start;
	lastOnset.trigMask = s->cur.trigMask;
}

/**
 * @brief Open the slot of an event.
 */
static void open_event(enum pq_ev_type type, const ADI_AFE_RMS_TYPE *rms,
		       ADI_PQLIB_TIME now, bool shadow)
{
	struct pq_ev_state *s = &evState[type];

	memset(&s->cur, 0, sizeof(s->cur));
	s->cur.start = now;
	if (type == PQ_EV_LINE_INTRP) {
		s->cur.trigMask = beyond_threshold(PQ_EV_INTRP, rms, NULL);
	} else {
		s->cur.trigMask = trigger_phases(type, rms, now);
		s->cur.affMask = beyond_threshold(type, rms, NULL);
	}
	s->cur.affMask |= s->cur.trigMask;
	memcpy(s->cur.min, rms, sizeof(s->cur.min));
	memcpy(s->cur.max, rms, sizeof(s->cur.max));
	for (int p = 0; p < PQ_EV_NUM_PHASES; p++)
		s->cur.ref[p] = histCount ? mean_value(p) : rms[p];
	s->shadow = shadow;
	s->open = true;
	note_onset(type);
	pending[type].valid = false;

	/* IEC 61000-4-30: an RVC that turns into a dip, swell or interruption
	 * is not an RVC; PQLib drops it without an end code, so do the same */
	if (type == PQ_EV_DIP || type == PQ_EV_SWELL || type == PQ_EV_INTRP)
		evState[PQ_EV_RVC].open = false;
}

/**
 * @brief Finish the open event of a type and make it the latest record.
 */
static void close_event(enum pq_ev_type type, const ADI_AFE_RMS_TYPE *rms,
			ADI_PQLIB_TIME now)
{
	struct pq_ev_state *s = &evState[type];
	/* A start seen only at the end still counts as a start */
	bool lateStart = !s->open;

	if (!s->open && pending[type].valid) {
		/* End without a seen start: use the condition's history */
		s->cur = pending[type].cur;
		s->cur.orphan = true;
	} else if (!s->open) {
		/* End without a seen start or history: keep it, no phase */
		memset(&s->cur, 0, sizeof(s->cur));
		s->cur.start = now;
		s->cur.orphan = true;
		memcpy(s->cur.min, rms, sizeof(s->cur.min));
		memcpy(s->cur.max, rms, sizeof(s->cur.max));
	}
	pending[type].valid = false;
	if (lateStart)
		note_onset(type);
	s->cur.end = now;
	s->cur.cycles = (uint32_t)(((now - s->cur.start) *
				    nominal_frequency() + 500) / 1000);
	if (type == PQ_EV_RVC) {
		/* the mean after the event vs the mean before it */
		for (int p = 0; p < PQ_EV_NUM_PHASES; p++) {
			uint32_t d = abs_diff(mean_value(p), s->cur.ref[p]);

			if (d > s->cur.dUss)
				s->cur.dUss = d;
		}
	}
	s->cur.valid = true;
	s->rec = s->cur;
	s->open = false;
	s->shadow = false;
	s->total++;
	s->unacked++;
	publish_record(type);
}

/**
 * @brief adi_pqlib_ProcessOneCycle() returns one result code per cycle,
 * so a start in the same cycle as another event change is not reported.
 * Open such events here, in the cycle of the other code and in the next
 * PQ_EV_MASK_WINDOW cycles (the RMS of the cycle with the code may still be
 * on its way across the threshold). PQLib's later end code confirms them.
 * @param coded - type reported by PQLib in this cycle, or PQ_EV_NUM_TYPES
 */
static void masked_starts(enum pq_ev_type coded, const ADI_AFE_RMS_TYPE *rms,
			  ADI_PQLIB_TIME now)
{
	static const enum pq_ev_type types[] = {
		PQ_EV_DIP, PQ_EV_SWELL, PQ_EV_INTRP
	};
	uint8_t mask;

	for (size_t i = 0; i < NO_OS_ARRAY_SIZE(types); i++) {
		if (types[i] == coded || evState[types[i]].open)
			continue;
		mask = beyond_threshold(types[i], rms, NULL);
		if (types[i] == PQ_EV_INTRP ? mask == 0x7 : mask != 0)
			open_event(types[i], rms, now, true);
	}
}

/**
 * @brief Drop tracker-opened events that PQLib never confirmed, once the
 * condition has cleared.
 */
static void expire_shadows(const ADI_AFE_RMS_TYPE *rms)
{
	for (int i = PQ_EV_DIP; i <= PQ_EV_INTRP; i++) {
		if (i == PQ_EV_RVC)
			continue;
		if (evState[i].open && evState[i].shadow && recovered(i, rms))
			evState[i].open = false;
	}
}

/**
 * @brief Follow dip/swell/interruption conditions while no event of that
 * type is open, without exposing them (see pending[]).
 */
static void track_pending(const ADI_AFE_RMS_TYPE *rms, ADI_PQLIB_TIME now)
{
	struct pq_ev_rec *c;
	uint8_t mask;
	bool cond;

	for (int t = PQ_EV_DIP; t <= PQ_EV_INTRP; t++) {
		if (t == PQ_EV_RVC)
			continue;
		if (evState[t].open) {
			pending[t].valid = false;
			continue;
		}
		c = &pending[t].cur;
		mask = beyond_threshold(t, rms, NULL);
		cond = t == PQ_EV_INTRP ? mask == 0x7 : mask != 0;
		if (cond && !pending[t].valid) {
			memset(c, 0, sizeof(*c));
			c->start = now;
			c->trigMask = trigger_phases(t, rms, now);
			c->affMask = mask | c->trigMask;
			memcpy(c->min, rms, sizeof(c->min));
			memcpy(c->max, rms, sizeof(c->max));
			pending[t].valid = true;
		} else if (pending[t].valid) {
			c->affMask |= mask;
			for (int p = 0; p < PQ_EV_NUM_PHASES; p++) {
				if (rms[p] < c->min[p])
					c->min[p] = rms[p];
				if (rms[p] > c->max[p])
					c->max[p] = rms[p];
			}
			if (!cond && recovered(t, rms))
				pending[t].valid = false;
		}
	}
}

/**
 * @brief Track line interruptions: the single-phase interruption rule
 * applied to every line.
 */
static void line_interruptions(const ADI_AFE_RMS_TYPE *rms, ADI_PQLIB_TIME now)
{
	struct pq_ev_state *s = &evState[PQ_EV_LINE_INTRP];
	uint32_t high = pct_of_nominal(pqlibExample.config.intrpThreshold.high);
	uint8_t below = beyond_threshold(PQ_EV_INTRP, rms, NULL);

	if (!s->open) {
		if (!below)
			return;
		open_event(PQ_EV_LINE_INTRP, rms, now, false);
		linesDown = below;
		return;
	}

	s->cur.affMask |= below;
	linesDown |= below;
	for (int p = 0; p < PQ_EV_NUM_PHASES; p++)
		if (rms[p] >= high)
			linesDown &= ~(1 << p);
	if (!linesDown)
		close_event(PQ_EV_LINE_INTRP, rms, now);
}

void pq_events_cycle(ADI_PQLIB_HANDLE hDevice, ADI_PQLIB_RESULT res,
		     const ADI_PQLIB_CYCLE_INPUT *in)
{
	const ADI_AFE_RMS_TYPE rms[PQ_EV_NUM_PHASES] = {
		in->AVRMSONE, in->BVRMSONE, in->CVRMSONE
	};
	struct pq_ev_state *s;
	enum pq_ev_type type;
	ADI_PQLIB_TIME now = 0;
	bool started = false;

	adi_pqlib_GetTime(hDevice, &now);

	/* Per-phase min/max, affected phases and RVC deviation of open events */
	for (int i = 0; i < PQ_EV_NUM_TYPES; i++) {
		s = &evState[i];
		if (!s->open)
			continue;
		if (i != PQ_EV_LINE_INTRP)
			s->cur.affMask |= beyond_threshold(i, rms, NULL);
		for (int p = 0; p < PQ_EV_NUM_PHASES; p++) {
			if (rms[p] < s->cur.min[p])
				s->cur.min[p] = rms[p];
			if (rms[p] > s->cur.max[p])
				s->cur.max[p] = rms[p];
			if (i == PQ_EV_RVC &&
			    abs_diff(rms[p], s->cur.ref[p]) > s->cur.devMax)
				s->cur.devMax = abs_diff(rms[p], s->cur.ref[p]);
		}
	}

	track_pending(rms, now);

	type = event_from_result(res, &started);
	if (type != PQ_EV_NUM_TYPES) {
		s = &evState[type];
		if (started) {
			/* A tracker-opened event keeps its earlier start */
			if (s->open && s->shadow)
				s->shadow = false;
			else
				open_event(type, rms, now, false);
		} else {
			close_event(type, rms, now);
		}
		masked_starts(type, rms, now);
		maskWindow = PQ_EV_MASK_WINDOW;
	} else if (maskWindow) {
		maskWindow--;
		masked_starts(PQ_EV_NUM_TYPES, rms, now);
	}
	expire_shadows(rms);
	line_interruptions(rms, now);

	mean_add(rms);
}

/**
 * @brief Check that PQLib event counts fit the configured event arrays.
 * PQLib occasionally reports garbage counts (e.g. intrpCount=194).
 */
static bool events_counts_valid(const ADI_PQLIB_EVENTS *ev)
{
	return (ev->dipCount <= ADI_PQLIB_CFG_NUM_DIPS &&
		ev->swellCount <= ADI_PQLIB_CFG_NUM_SWELLS &&
		ev->intrpCount <= ADI_PQLIB_CFG_NUM_INTRP &&
		ev->rvcCount <= ADI_PQLIB_CFG_NUM_RVC);
}

/**
 * @brief Check that a PQLib event record is plausible. The start-to-end
 * span must match durationInCycles within 10% + 2 cycles.
 */
static bool event_timing_valid(const EVENT_TIMING *t)
{
	uint32_t freq = nominal_frequency();
	int64_t span, expected, tolerance;

	/* durationInCycles may be 0 for a sub-cycle RVC */
	if (t->startTime < 0 || t->endTime <= t->startTime)
		return false;

	span = t->endTime - t->startTime;
	expected = (int64_t)t->durationInCycles * 1000 / freq;
	tolerance = expected / 10 + 2 * 1000 / freq;

	return (span >= expected - tolerance) && (span <= expected + tolerance);
}

/**
 * @brief Find the PQLib entry that matches our latest record of a type:
 * same start time (±2 cycles), or for a record whose start was not seen,
 * same end time.
 * @param type   - event type
 * @param first  - first PQLib event structure of that type
 * @param count  - number of PQLib events reported
 * @param stride - size of one PQLib event structure
 * @return matching entry, or NULL
 */
static const EVENT_TIMING *find_match(enum pq_ev_type type, const void *first,
				      uint8_t count, size_t stride)
{
	const struct pq_ev_rec *r = &evState[type].rec;
	ADI_PQLIB_TIME tol = 2 * 1000 / nominal_frequency();
	const uint8_t *e = first;
	const EVENT_TIMING *t;
	ADI_PQLIB_TIME ours, theirs;

	for (uint8_t i = 0; i < count; i++, e += stride) {
		t = (const EVENT_TIMING *)e;
		if (!event_timing_valid(t)) {
			bogus_event_count++;
			if (bogus_event_count <= MAX_BOGUS_EVENT_PRINTS)
				printf("Bogus PQLib event %d[%u]: start=%" PRId64
				       " end=%" PRId64 " cycles=%" PRIu32 "\n\r",
				       type, i, t->startTime, t->endTime,
				       t->durationInCycles);
			continue;
		}
		if (!r->valid || r->pqlibChecked)
			continue;
		ours = r->orphan ? r->end : r->start;
		theirs = r->orphan ? t->endTime : t->startTime;
		if (theirs >= ours - tol && theirs <= ours + tol)
			return t;
	}

	return NULL;
}

/**
 * @brief Take PQLib's timing into our latest record of a type.
 * @param type - event type
 * @param t    - matching PQLib entry
 * @param dst  - the record's entry in accumulated_events
 */
static void apply_match(enum pq_ev_type type, const EVENT_TIMING *t,
			EVENT_TIMING *dst)
{
	struct pq_ev_rec *r = &evState[type].rec;

	/* PQLib's timing is the IEC event timing: e.g. an RVC is reported
	 * after the voltage is steady again, but its duration is shorter */
	r->pqlibChecked = true;
	r->orphan = false;
	r->start = t->startTime;
	r->end = t->endTime;
	r->cycles = t->durationInCycles;
	dst->startTime = t->startTime;
	dst->endTime = t->endTime;
	dst->durationInCycles = t->durationInCycles;
}

void pq_events_cross_check(const ADI_PQLIB_OUTPUT *output)
{
	const ADI_PQLIB_EVENTS *ev;
	const EVENT_TIMING *t;

	if (!output)
		return;

	ev = &output->events;
	if (!ev->dipCount && !ev->swellCount &&
	    !ev->intrpCount && !ev->rvcCount)
		return;

	/* Do not reset or reconfigure PQLib here, it corrupts the handle */
	if (!events_counts_valid(ev)) {
		bogus_event_count++;
		return;
	}

	t = find_match(PQ_EV_DIP, ev->dips, ev->dipCount, sizeof(ev->dips[0]));
	if (t) {
		apply_match(PQ_EV_DIP, t,
			    (EVENT_TIMING *)&accumulated_events.dips[0]);
		accumulated_events.dips[0].minMag =
			((const ADI_PQLIB_DIP *)t)->minMag;
	}

	t = find_match(PQ_EV_SWELL, ev->swells, ev->swellCount,
		       sizeof(ev->swells[0]));
	if (t) {
		apply_match(PQ_EV_SWELL, t,
			    (EVENT_TIMING *)&accumulated_events.swells[0]);
		accumulated_events.swells[0].maxMag =
			((const ADI_PQLIB_SWELL *)t)->maxMag;
	}

	t = find_match(PQ_EV_INTRP, ev->intrp, ev->intrpCount,
		       sizeof(ev->intrp[0]));
	if (t)
		apply_match(PQ_EV_INTRP, t,
			    (EVENT_TIMING *)&accumulated_events.intrp[0]);

	t = find_match(PQ_EV_RVC, ev->rvc, ev->rvcCount, sizeof(ev->rvc[0]));
	if (t) {
		apply_match(PQ_EV_RVC, t,
			    (EVENT_TIMING *)&accumulated_events.rvc[0]);
		accumulated_events.rvc[0].deltaUmax =
			((const ADI_PQLIB_RVC *)t)->deltaUmax;
		accumulated_events.rvc[0].deltaUss =
			((const ADI_PQLIB_RVC *)t)->deltaUss;
	}
}

void pq_events_time_shift(ADI_PQLIB_TIME delta)
{
	for (int i = 0; i < PQ_EV_NUM_TYPES; i++) {
		if (evState[i].open)
			evState[i].cur.start += delta;
		if (pending[i].valid)
			pending[i].cur.start += delta;
	}
	if (lastOnset.seq && evState[lastOnset.type].open)
		lastOnset.start += delta;
}

void pq_events_reset_open(void)
{
	for (int i = 0; i < PQ_EV_NUM_TYPES; i++)
		evState[i].open = false;
	linesDown = 0;
	maskWindow = 0;
	memset(pending, 0, sizeof(pending));
	mean_reset();
}

void pq_events_clear(void)
{
	memset(evState, 0, sizeof(evState));
	memset(&accumulated_events, 0, sizeof(accumulated_events));
	memset(&lastOnset, 0, sizeof(lastOnset));
	linesDown = 0;
	maskWindow = 0;
	memset(pending, 0, sizeof(pending));
	bogus_event_count = 0;
	mean_reset();
}

void pq_events_ack(enum pq_ev_type type)
{
	if (type < PQ_EV_NUM_TYPES)
		evState[type].unacked = 0;
}

bool pq_events_active(enum pq_ev_type type)
{
	return type < PQ_EV_NUM_TYPES && evState[type].open;
}

ADI_PQLIB_TIME pq_events_onset(enum pq_ev_type type)
{
	return pq_events_active(type) ? evState[type].cur.start : 0;
}

uint32_t pq_events_unacked(enum pq_ev_type type)
{
	return type < PQ_EV_NUM_TYPES ? evState[type].unacked : 0;
}

uint32_t pq_events_last_onset(enum pq_ev_type *type, ADI_PQLIB_TIME *start,
			      uint8_t *trigMask)
{
	*type = lastOnset.type;
	*start = lastOnset.start;
	*trigMask = lastOnset.trigMask;

	return lastOnset.seq;
}

uint32_t pq_events_onsets(enum pq_ev_type type)
{
	return type < PQ_EV_NUM_TYPES ? evState[type].onsets : 0;
}

uint32_t pq_events_total(enum pq_ev_type type)
{
	return type < PQ_EV_NUM_TYPES ? evState[type].total : 0;
}

/* The open event if there is one, else the latest finished one */
static const struct pq_ev_rec *shown_record(enum pq_ev_type type)
{
	if (type >= PQ_EV_NUM_TYPES)
		return NULL;

	return evState[type].open ? &evState[type].cur : &evState[type].rec;
}

uint8_t pq_events_trigger(enum pq_ev_type type)
{
	const struct pq_ev_rec *r = shown_record(type);

	return r ? r->trigMask : 0;
}

uint8_t pq_events_affected(enum pq_ev_type type)
{
	const struct pq_ev_rec *r = shown_record(type);

	return r ? r->affMask : 0;
}

bool pq_events_latest(enum pq_ev_type type, ADI_PQLIB_TIME *start,
		      ADI_PQLIB_TIME *end, uint32_t *cycles)
{
	const struct pq_ev_rec *r;

	if (type >= PQ_EV_NUM_TYPES || !evState[type].rec.valid) {
		*start = 0;
		*end = 0;
		*cycles = 0;
		return false;
	}

	r = &evState[type].rec;
	*start = r->start;
	*end = r->end;
	*cycles = r->cycles;

	return true;
}

ADI_AFE_RMS_TYPE pq_events_mag(enum pq_ev_type type, int phase, bool max)
{
	const struct pq_ev_rec *r = shown_record(type);

	if (!r || phase < 0 || phase >= PQ_EV_NUM_PHASES)
		return 0;

	return max ? r->max[phase] : r->min[phase];
}
