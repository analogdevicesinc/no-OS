/***************************************************************************//**
 *   @file   no_os_tone.h
 *   @brief  Header file for the complex tone generation and analysis utility.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
*******************************************************************************/
#ifndef _NO_OS_TONE_H
#define _NO_OS_TONE_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Generate a complex baseband tone, and judge whether a capture holds the tone
 * it was supposed to hold. Intended for bringing up and verifying an I/Q
 * datapath: a transmit buffer is filled with a known rotation, the received
 * buffer is correlated against the frequency it is expected to arrive at, and
 * the result says whether the energy is where predicted rather than merely
 * whether something is present.
 *
 * Sample format, which the API cannot express and callers must respect:
 *
 * - Buffers hold **signed two's-complement 16-bit** samples carried in a
 *   uint16_t array, interleaved by converter. Offset-binary or unsigned data --
 *   which some converters emit natively -- has to be re-biased to signed by the
 *   caller first. Passing it unconverted does not fail cleanly; see the note on
 *   no_os_tone_coherence().
 * - "LSB" everywhere in this interface means a converter code at that 16-bit
 *   scale, so full scale is 32767. That shared scale is what makes the measured
 *   amplitude comparable against a reference level.
 * - Sample rates are in Hz and may exceed 32 bits, so high-speed converter
 *   clocks need no scaling on the way in.
 *
 * Integer arithmetic throughout, deliberately: no math.h and no floating point,
 * since a libm is not linked on every no-OS platform.
 */

/**
 * Coherence, envelope spread and every other ratio here are carried in
 * per-mille so the whole implementation stays in integers, and printed as a
 * percentage with one decimal, which per-mille divides into exactly.
 */
#define NO_OS_TONE_SCALE		1000

/**
 * Per-mille split into the two arguments of a "%lu.%lu%%" format, since there
 * is no floating point in a no-OS build. Exported so a caller can print its own
 * per-mille ratios the same way this module prints coherence.
 */
#define NO_OS_TONE_PCT_WHOLE(x)		((unsigned long)(x) / 10)
#define NO_OS_TONE_PCT_TENTH(x)		((unsigned long)(x) % 10)

/**
 * Default pass thresholds -- a documented starting point rather than a rule.
 * They were calibrated against a wideband converter capture several thousand
 * samples deep; a path with a different noise floor, or a much shorter capture,
 * may well need others. Both are fields of struct no_os_tone_limits, so no
 * caller is forced to inherit them.
 */
#define NO_OS_TONE_COHERENCE_PASS	900
#define NO_OS_TONE_SPREAD_MAX		250

/**
 * @struct no_os_tone_layout
 * @brief Where the I/Q pair sits in an interleaved converter buffer.
 */
struct no_os_tone_layout {
	/**
	 * Interleaved converters per sample -- the stride from one sample to the
	 * next, in array elements. 2 for a plain I/Q pair; on a JESD204 link it
	 * is the link's M.
	 */
	uint8_t num_conv;
	/** Converter index carrying I, below num_conv */
	uint8_t conv_i;
	/** Converter index carrying Q, below num_conv and not conv_i */
	uint8_t conv_q;
};

/**
 * @struct no_os_tone_result
 * @brief Measurements one analysis pass yields.
 */
struct no_os_tone_result {
	/**
	 * Coherence in per-mille. Reaches NO_OS_TONE_SCALE when all the
	 * captured energy sits exactly at the expected frequency and collapses
	 * to roughly NO_OS_TONE_SCALE/samples for noise or a tone anywhere
	 * else, so it answers "is this the tone we asked for" rather than
	 * merely "is something there".
	 */
	uint32_t coherence;
	/** Mean tone envelope in LSB */
	uint32_t amplitude;
	/** Envelope spread in per-mille */
	uint32_t spread;
};

/**
 * @struct no_os_tone_limits
 * @brief Pass thresholds for one measurement.
 */
struct no_os_tone_limits {
	/** Minimum coherence in per-mille */
	uint32_t coherence_min;
	/** Maximum envelope spread in per-mille */
	uint32_t spread_max;
};

/**
 * @enum no_os_tone_sense
 * @brief What a measurement has to look like to pass.
 */
enum no_os_tone_sense {
	/** A tone is expected at the probed frequency */
	NO_OS_TONE_SENSE_TONE = 0,
	/**
	 * No tone is expected: the signal source is off, the input terminated,
	 * the transmitter disabled -- however the path was quietened. The verdict
	 * inverts, so the capture passes by scoring *below* the threshold.
	 *
	 * Worth running before any measurement that claims to find a tone: it is
	 * what demonstrates the estimator rejects noise instead of scoring
	 * whatever it is handed.
	 */
	NO_OS_TONE_SENSE_QUIET,
	/**
	 * A tone is expected, at either sign. For a path whose I/Q ordering is
	 * not yet established, where a swapped pair arrives at -f instead of +f:
	 * supply a conjugate probe and the swap is named in the report rather
	 * than read as an absent signal.
	 */
	NO_OS_TONE_SENSE_EITHER,
};

/**
 * @struct no_os_tone_test
 * @brief One measurement's identity and what passing means for it.
 *
 * Everything here is about interpretation, not arithmetic -- it tells
 * no_os_tone_report() how to judge and describe a result it cannot infer from
 * the numbers alone.
 */
struct no_os_tone_test {
	/** Test name for the heading, may be NULL */
	const char *name;
	/** Frequency the capture was scored at, signed, for the log */
	int64_t freq_hz;
	/** Samples per converter, to state the expected noise floor */
	uint32_t samples;
	/** What the measurement has to look like to pass */
	enum no_os_tone_sense sense;
	/** Thresholds to compare against */
	struct no_os_tone_limits limits;
	/**
	 * Conjugate probe of the same capture, or NULL. Required to pass
	 * NO_OS_TONE_SENSE_EITHER, ignored otherwise.
	 */
	const struct no_os_tone_result *conjugate;
	/** Transmitted amplitude in LSB to reference against, 0 to omit */
	uint32_t tx_amplitude;
	/** Converter full scale in LSB to reference against, 0 to omit */
	uint32_t full_scale;
};

/* Frequency, signed, to a 32-bit phase step in turns per sample. */
uint32_t no_os_tone_phase_step(int64_t freq_hz, uint64_t rate_hz);

/*
 * Score a capture against a complex tone at the expected offset. buf holds
 * signed two's-complement 16-bit samples, interleaved per layout.
 */
int no_os_tone_coherence(const uint16_t *buf, uint32_t samples,
			 const struct no_os_tone_layout *layout,
			 int64_t freq_hz, uint64_t rate_hz,
			 struct no_os_tone_result *result);

/*
 * Write a complex tone into an interleaved transmit buffer, as signed
 * two's-complement 16-bit samples.
 */
int no_os_tone_fill_iq(uint16_t *buf, uint32_t samples,
		       const struct no_os_tone_layout *layout,
		       int64_t freq_hz, uint64_t rate_hz, int32_t amplitude);

/* Report one measurement and return whether it passes. */
bool no_os_tone_report(const struct no_os_tone_test *test,
		       const struct no_os_tone_result *result);

#endif /* _NO_OS_TONE_H */
